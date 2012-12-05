/*
 * server.c
 *
 *  Created on: Dec 5, 2012
 *      Author: cody
 *
 *  port: 49364
 */

#ifdef LINUX
	#include <sys/socket.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <sys/types.h>
#endif

#ifdef WINDOWS
	#include <winsock.h>
#endif

#include "server.h"

extern char* bindaddr;

int createServer()
{
	struct addrinfo hints, *res;
	int sockfd;

	// first, load up address structs with getaddrinfo():

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;			// use IPv4 or IPv6, whichever
	hints.ai_socktype = SOCK_STREAM;		// TCP
	//if(unspecified bind address)
	hints.ai_flags = AI_PASSIVE;			// fill in my IP for me

	getaddrinfo((bindaddr)?bindaddr:NULL, "49364", &hints, &res);

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	bind(sockfd, res->ai_addr, res->ai_addrlen);
	return 0;
}
