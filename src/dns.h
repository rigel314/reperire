#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct dns_header {
	uint16_t id;
/*	union {
		struct {
			uint8_t response:1;
			uint8_t opcode:4;
			uint8_t authoritative:1;
			uint8_t truncated:1;
			uint8_t rdesired:1;
			uint8_t ravailable:1;
			uint8_t z:1;
			uint8_t authenticated:
			uint8_t rcode:4;
		} flags;
*/		uint16_t data;
/*	} parameters;
*/	uint16_t qdcount;
	uint16_t ancount;
	uint16_t nscount;
	uint16_t arcount;
};

struct dns_query_data {
	uint16_t type;
	uint16_t class;
};

struct dns_query {
	char * name;
	struct dns_query_data * data;
};

struct dns_record_data {
	uint16_t type;
	uint16_t class;
	uint32_t ttl;
	uint16_t rdlength;
};

struct dns_record {
	char * name;
	struct dns_record_data * data;
	void * rdata;
};

struct dns {
	struct dns_header * header;
	struct dns_query * queries;
	struct dns_record * answers, * authorities, * additionals;
};
