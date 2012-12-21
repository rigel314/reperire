/*
 * server.c
 *
 *  Created on: Dec 5, 2012
 *      Author: cody
 *
 *  port: 49364
 */

#ifdef WINDOWS
	#include <winsock.h>
	#include <windows.h>
#else
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <errno.h>
	#include <string.h>
	#include <sys/wait.h>
	#include <signal.h>
	#include <sys/socket.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <sys/types.h>
	#include <math.h>
#endif

#include "server.h"
#include "signals.h"
#include "misc.h"
#include "client.h"

#define MAXBUFLEN 100

void *get_in_addr(struct sockaddr *sa)
{ // get sockaddr, IPv4 or IPv6:
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int createServer()
{
	int sockfd; // listening socket
	int retval;
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	int numbytes;
	char s[INET6_ADDRSTRLEN];
	char buf[MAXBUFLEN];
	socklen_t addr_len;


	// Load up address structs to use with getaddrinfo().
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;				// use IPv4
	hints.ai_socktype = SOCK_DGRAM;			// UDP
	if(!bindaddr)
		hints.ai_flags = AI_PASSIVE;		// fill in my IP for me

	if((retval = getaddrinfo((bindaddr)?bindaddr:NULL, "49364", &hints, &servinfo)))
	{
		printfLog("server getaddrinfo(): %s", gai_strerror(retval));
		return 2;
	}

	for(p = servinfo; p != NULL; p = p->ai_next)
	{ // Loop through all results from getaddrinfo() and use the first one that works.
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			printLogError("sever socket()", errno);
			continue;
		}

		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			printLogError("server bind()", errno);
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // We're done with this.

	if (p == NULL)
	{
		printLog("server failed to bind socket");
		return 3;
	}

	if(!fork())
	{ // this is a child process
		_sleep(1000);
		sendBroadcast();
		exit(0);
	}

	while(1)
	{ // Loop forever waiting for data to come into the socket.
		if(got_sigint)
		{ // Die gracefully on SIGINT or SIGTERM. (Ctrl-C or kill)
			printLog("Closing on signal");
			close(sockfd);
			exit(0);
		}

		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
		{
			printLogError("server recvfrom()", errno);
			continue;
		}

		printfLog("server got packet from %s", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
		printfLog("packet is %d bytes long", numbytes);
		buf[numbytes] = '\0';
		printfLog("packet contains \"%s\"", buf);

		if(!strcmp(buf, "I'm here!"))
			if(!fork())
			{ // this is a child process
				_sleep(rand() % 5000);
				sendReplyPacket(inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
				exit(0);
			}
		if(!strcmp(buf, "I'm here too!"))
		{
			printfLog("%s is here too!",inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
		}
	}

	return 0;
}
