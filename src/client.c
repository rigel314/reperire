/*
 * client.c
 *
 *  Created on: Dec 5, 2012
 *      Author: cody
 */

#ifdef WINDOWS
	#include <windows.h>
	#include <winsock.h>
#else
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <errno.h>
	#include <string.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
#endif

#include "misc.h"

#define BCAST 1
#define SINGLE 0
#define REPLY 1
#define INITAL 0

char* findBroadcastAddr()
{
	return "255.255.255.255";
}

char* encapPacket(int reply, char* msg)
{
	char* out;
	char* fmt = (reply) ? "\x06%s\x03" : "\x05%s\x03";

	out = malloc(strlen(hostname) + 3);
	if(!out)
	{
		printLogError("client: encapPacket malloc()", errno);
		return NULL;
	}

	sprintf(out, fmt, msg);

	return out;
}

int SocketAndSendto(int bcast, int reply, const char* dest)
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int retval;
	int numbytes;
	int broadcast = 1;
	char* packet;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = (bcast) ? AF_INET : AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((retval = getaddrinfo(dest, "49364", &hints, &servinfo)) != 0)
	{
		printfLog("client: getaddrinfo(): %s", gai_strerror(retval));
		return 4;
	}

	for(p = servinfo; p != NULL; p = p->ai_next)
	{ // Loop through all results from getaddrinfo() and use the first one that works.
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			printLogError("client: socket()", errno);
			continue;
		}

		break;
	}

	if (p == NULL)
	{
		printLog("client: failed to create socket");
		return 5;
	}

	if(bcast)
		if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1)
		{
			printLogError("client: setsockopt(SO_BROADCAST)", errno);
			return 6;
		}

	packet = encapPacket(reply, hostname);
	printfLog("client: sent %s: %d bytes to %s", ((reply) ? "response" : "request"), strlen(packet), dest);
	if(packet)
	{
		if ((numbytes = sendto(sockfd, packet, strlen(packet), 0, p->ai_addr, p->ai_addrlen)) == -1)
		{
			printLogError("client: sendto()", errno);
			return 7;
		}
		free(packet);
	}

	freeaddrinfo(servinfo);

	close(sockfd);

	return 0;
}

int sendBroadcast()
{
	return SocketAndSendto(BCAST, INITAL, findBroadcastAddr());
}

int sendAlivePacket(const char* dest)
{
	return SocketAndSendto(SINGLE, INITAL, dest);
}

int sendReplyPacket(const char* dest)
{
	return SocketAndSendto(SINGLE, REPLY, dest);
}
