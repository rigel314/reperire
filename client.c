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


