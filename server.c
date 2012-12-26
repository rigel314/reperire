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
#endif

#include "server.h"
#include "signals.h"
#include "misc.h"
#include "client.h"
#include "sql.h"

#define MAXBUFLEN 100
#define MAXPROCS 100
#define REQUEST 5
#define RESPONSE 6

void *get_in_addr(struct sockaddr *sa)
{ // get sockaddr, IPv4 or IPv6:
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int createServer()
{
	pid_t keepAlive;
	int sockfd; // listening socket
	int retval;
	const char* connectIP;
	char* connectName;
	char packType;
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
		printfLog("server: getaddrinfo(): %s", gai_strerror(retval));
		return 2;
	}

	for(p = servinfo; p != NULL; p = p->ai_next)
	{ // Loop through all results from getaddrinfo() and use the first one that works.
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			printLogError("server: socket()", errno);
			continue;
		}

		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			printLogError("server: bind()", errno);
			continue;
		}

		break;
	}

	if (p == NULL)
	{
		printLog("server: failed to bind socket");
		return 3;
	}

	// -------------Keep Alive Process---------------------------------------------------
	if(!(keepAlive = fork()))
	{ // this is a child process
		struct sqlIPResults* IPs, *IP;

		createChildSigHandlers();

		while(1)
		{
			_sleep(300000);
			IPs = getAllIPs();
			for(IP = IPs; IP != NULL; IP = IP->next)
			{
				printfLog("keep alive: %s", IP->ip);
				//sendAlivePacket(IP->ip);
				//_sleep(mkrand() % 1000);
			}
			freeSqlIPResult(IPs);

			sendBroadcast();
		}
	}
	// ----------------------------------------------------------------------------------

	// -------------Broadcast Process----------------------------------------------------
	if(!fork())
	{ // this is another child process
		_sleep(1000);
		sendBroadcast();
		exit(0);
	}
	// ----------------------------------------------------------------------------------

	while(1)
	{ // Loop forever waiting for data to come into the socket.
		if(got_sigint)
		{ // Die gracefully on SIGINT or SIGTERM. (Ctrl-C or kill)
			printLog("server: Closing on signal.");
			printLog("server: Killing keep alive child process.");
			kill(keepAlive, SIGINT);
			waitpid(keepAlive,NULL,0);
			close(sockfd);
			freeaddrinfo(servinfo); // We're done with this.
			free(hostname);
			//free(dbFile);
			exit(0);
		}

		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
		{
			if(errno != EINTR)
				printLogError("server: recvfrom()", errno);
			continue;
		}

		packType = buf[0];
		if((packType == REQUEST || packType == RESPONSE) && checkValidBuf(buf, numbytes))
		{
			connectIP = inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
			// buf[numbytes - 1] = '\0'; // checkValidBuf now does this.
			connectName = buf + 1;
			printfLog("server: received %s: %s is %s", ((packType==REQUEST) ? "request" : "response"), connectIP, connectName);

			addHost(connectIP, connectName);

			printfLog("server: %d children", numProcs);

			if(packType == REQUEST && numProcs < MAXPROCS)
			{
				numProcs++;
				// -------------Reply Process--------------------------------------------
				if(!fork())
				{ // yay! child processes!  I LOVE fork()!
					_sleep(mkrand() % 5000);
					sendReplyPacket(connectIP);
					exit(0);
				}
				// ----------------------------------------------------------------------
			}
			/*if(packType == RESPONSE)
			{ // I don't know if I care to do anything when receiving a response.
				printfLog("%s is here too!",connectIP);
			}*/
		}
	}

	return 0;
}
