/*
 * main.c
 *
 *  Created on: Dec 4, 2012
 *      Author: cody
 */

#ifdef LINUX
	#include <sys/socket.h>
	#include <unistd.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <sys/types.h>
#endif

#ifdef WINDOWS
	#include <winsock.h>
	#include <windows.h>
#endif

#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

#include "server.h"
#include "client.h"
#include "sql.h"

#define HOSTBLAR "example.com"

char* bindaddr = NULL;
//char* bindaddr = "192.168.0.100"; // Or whatever.

int main()
{
	createServer();
	return 0;
}
