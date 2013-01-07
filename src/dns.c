#include "dns.h"

#define BUFLEN		512

#define as8(buf)	*((uint8_t *)(buf))
#define as16(buf)	*((uint16_t *)(buf))
#define as32(buf)	*((uint32_t *)(buf))

//#define EXIT_ON_ERR_IN_LOOP

uint8_t next8(char * buf, uint8_t * off) {
	uint8_t v = as8(buf + *off);
	*off += 1;
	return v;
}

uint16_t next16(char * buf, uint8_t * off) {
	uint16_t v = as16(buf + *off);
	*off += 2;
	return ntohs(v);
}

uint32_t next32(char * buf, uint8_t * off) {
	uint32_t v = as32(buf + *off);
	*off += 4;
	return ntohl(v);
}

void read_dns_header(char * buf, uint8_t * off, struct dns_header * header) {
	header->id = next16(buf, off);
	header->data = next16(buf, off);
	header->qdcount = next16(buf, off);
	header->ancount = next16(buf, off);
	header->nscount = next16(buf, off);
	header->arcount = next16(buf, off);
}

int dns_name_len(char * buf, uint8_t off) {
	char orig = off;
	if (as8(buf + off) == 0xC0)
		return dns_name_len(buf, next16(buf, &off) - 0xC000);
	else {
		for (int n = 0; n = next8(buf, &off); off += n);
		return off - orig;
	}
}

void read_dns_name(char * buf, uint8_t * off, char * name) {
	if (as8(buf + *off) == 0xC0) {
		char alt_off = next16(buf, off) & ~0xC000;
		read_dns_name(buf, &alt_off, name);
	} else
		for (int n = 0; n = next8(buf, off); as8(name) = '.', name++)
			for (int m = 0; m < n; m++, name++)
				as8(name) = next8(buf, off);
}

void read_dns_query(char * buf, uint8_t * off, struct dns_query * query) {
	query->name = calloc(dns_name_len(buf, *off) + 1, sizeof(char));
	read_dns_name(buf, off, query->name);
	
	query->data = malloc(sizeof(struct dns_query_data));
	query->data->type = next16(buf, off);
	query->data->class = next16(buf, off);
}

void read_dns_record(char * buf, uint8_t * off, struct dns_record * record) {
	record->name = calloc(dns_name_len(buf, *off) + 1, sizeof(char));
	read_dns_name(buf, off, record->name);

	record->data = malloc(sizeof(struct dns_record_data));
	record->data->type = next16(buf, off);
	record->data->class = next16(buf, off);
	record->data->ttl = next32(buf, off);
	record->data->rdlength = next16(buf, off);

	void * rdata = record->rdata = calloc(record->data->rdlength, sizeof(void));
	for (int i = 0; i < record->data->rdlength; i++, rdata++)
		as8(rdata) = next8(buf, off);
}

void read_dns(char * buf, struct dns * msg) {
	uint8_t off;
	
	msg->header = malloc(sizeof(struct dns_header));
        read_dns_header(buf, &off, msg->header);
	
	struct dns_query * queries = msg->queries = calloc(msg->header->qdcount, sizeof(struct dns_query));
	for (int i = 0; i < msg->header->qdcount; i++)
		read_dns_query(buf, &off, queries++);
	
	struct dns_record * answers = msg->answers = calloc(msg->header->ancount, sizeof(struct dns_record));
	for (int i = 0; i < msg->header->ancount; i++)
		read_dns_record(buf, &off, answers++);
	
	struct dns_record * authorities = msg->authorities = calloc(msg->header->nscount, sizeof(struct dns_record));
	for (int i = 0; i < msg->header->nscount; i++) 
		read_dns_record(buf, &off, authorities++);
	
	struct dns_record * additionals = msg->additionals = calloc(msg->header->arcount, sizeof(struct dns_record));
	for (int i = 0; i < msg->header->arcount; i++) 
		read_dns_record(buf, &off, additionals++);
}

void free_dns(struct dns * msg) {
	for (int i = 0; i < msg->header->qdcount; i++) {
		free((msg->queries + i)->name);
		free((msg->queries + i)->data);
	}
	free(msg->queries);
	
	for (int i = 0; i < msg->header->ancount; i++) {
		free((msg->answers + i)->name);
		free((msg->answers + i)->data);
		free((msg->answers + i)->rdata);
	}
	free(msg->answers);
	
	for (int i = 0; i < msg->header->nscount; i++) {
		free((msg->authorities + i)->name);
		free((msg->authorities + i)->data);
		free((msg->authorities + i)->rdata);
	}
	free(msg->authorities);
	
	for (int i = 0; i < msg->header->arcount; i++) {
		free((msg->additionals + i)->name);
		free((msg->additionals + i)->data);
		free((msg->additionals + i)->rdata);
	}
	free(msg->additionals);
	
	free(msg->header);
	free(msg);
}

void print_dns(char * buf) {
	struct dns * msg = malloc(sizeof(struct dns));
	read_dns(buf, msg);
	printf("DNS:\n");
	printf("\tHeader:\n");
	printf("\t\tID: %x\n", msg->header->id);
	printf("\t\tFlags: %x\n", msg->header->data);
	printf("\t\tQuery Count: %d\n", msg->header->qdcount);
	printf("\t\tAnswer Count: %d\n", msg->header->ancount);
	printf("\t\tAuthority Count: %d\n", msg->header->nscount);
	printf("\t\tAdditional Count: %d\n", msg->header->arcount);
	for (int i = 0; i < msg->header->qdcount; i++) {
		printf("\tQuery:\n");
		printf("\t\tName: %s\n", (msg->queries + i)->name);
		printf("\t\tType: %x\n", (msg->queries + i)->data->type);
		printf("\t\tClass: %x\n", (msg->queries + i)->data->class);
	}
	for (int i = 0; i < msg->header->ancount; i++) {
		printf("\tAnswer:\n");
		printf("\t\tName: %s\n", (msg->answers + i)->name);
		printf("\t\tType: %x\n", (msg->answers + i)->data->type);
		printf("\t\tClass: %x\n", (msg->answers + i)->data->class);
		printf("\t\tTTL: %d\n", (msg->answers + i)->data->ttl);
		printf("\t\tData Length: %d\n", (msg->answers + i)->data->rdlength);
	}
	for (int i = 0; i < msg->header->nscount; i++) {
		printf("\tAuthority:\n");
		printf("\t\tName: %s\n", (msg->authorities + i)->name);
		printf("\t\tType: %x\n", (msg->authorities + i)->data->type);
		printf("\t\tClass: %x\n", (msg->authorities + i)->data->class);
		printf("\t\tTTL: %d\n", (msg->authorities + i)->data->ttl);
		printf("\t\tData Length: %d\n", (msg->authorities + i)->data->rdlength);
	}
	for (int i = 0; i < msg->header->arcount; i++) {
		printf("\tAdditional Record:\n");
		printf("\t\tName: %s\n", (msg->additionals + i)->name);
		printf("\t\tType: %x\n", (msg->additionals + i)->data->type);
		printf("\t\tClass: %x\n", (msg->additionals + i)->data->class);
		printf("\t\tTTL: %d\n", (msg->additionals + i)->data->ttl);
		printf("\t\tData Length: %d\n", (msg->additionals + i)->data->rdlength);
	}
	free_dns(msg);
}

int forward(int s_local, int s_remote, char buf[BUFLEN], int * len, struct sockaddr_in * remote, struct sockaddr_in * client, int client_len) {
	int remote_len = sizeof(struct sockaddr_in);
	
	if (sendto(s_remote, buf, *len, 0, (struct sockaddr *) remote, remote_len) == -1) {
		perror("sendto");
		return -1;
	}

	if ((*len = recvfrom(s_remote, buf, BUFLEN, 0, (struct sockaddr *) remote, &remote_len)) == -1) {
		perror("recvfrom");
		return -1;
	}

	if (((struct dns_flags *)(buf + 2))->reply_code == 0x3) {
		// no such name
		return 1;
	}
	
	if (sendto(s_local, buf, *len, 0, (struct sockaddr *) client, client_len) == -1) {
		perror("sendto");
		return -1;
	}
	
	return 0;
}

int main(int argc, char *argv[]) {
	struct sockaddr_in * lo, * all, * remote, * client;
	int s_local, s_remote;
	char buf[BUFLEN];
	
	if ((s_local = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("socket");
		return 1;
	}

	if ((s_remote = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("socket");
		return 1;
	}

	lo = calloc(1, sizeof(struct sockaddr_in));
	lo->sin_family = AF_INET;
	lo->sin_port = htons(53);
	lo->sin_addr.s_addr = inet_addr("127.0.0.1");
	
	all = calloc(1, sizeof(struct sockaddr_in));
	all->sin_family = AF_INET;
	all->sin_addr.s_addr = htonl(INADDR_ANY);

	remote = calloc(1, sizeof(struct sockaddr_in));
	remote->sin_family = AF_INET;
	remote->sin_port = htons(53);
	inet_aton("8.8.8.8", (struct in_addr *) &remote->sin_addr.s_addr);

	client = calloc(1, sizeof(struct sockaddr_in));
	
	if (bind(s_local, (struct sockaddr *) lo, sizeof(struct sockaddr_in)) == -1) {
		perror("bind");
		return 1;
	}

	if (bind(s_remote, (struct sockaddr *) all, sizeof(struct sockaddr_in)) == -1) {
		perror("bind");
		return 1;
	}

	
	while (1) {
		int recvd, client_len = sizeof(struct sockaddr_in);
		if ((recvd = recvfrom(s_local, buf, 512, 0, (struct sockaddr *) client, &client_len)) >= 0) {
			int ret;
			
			ret = forward(s_local, s_remote, buf, &recvd, remote, client, client_len);
			
			if (ret == -1)
#ifdef EXIT_ON_ERR_IN_LOOP
				return 1;
#else
				continue;
#endif
			else if (ret == 0)
				continue;
			
			// do some other stuff
			
			/**/
		} else {
			perror("recvfrom");
#ifdef EXIT_ON_ERR_IN_LOOP
			return 1;
#endif
		}
		sleep(1);
	}
	
	perror("?");

	return 0;
}
