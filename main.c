/*
 * main.c
 *
 *  Created on: Dec 4, 2012
 *      Author: cody
 */

#define LINUX
//#define WINDOWS

#ifdef LINUX
	#include <sys/socket.h>
	#include <unistd.h>
#endif

#ifdef WINDOWS
	#include <winsock.h>
	#include <windows.h>
#endif

#include <sqlite3.h>
#include <ncurses.h>

void _sleep(int millis);

int main()
{
	;
	return 0;
}

void _sleep(int millis)
{
	#ifdef LINUX
		sleep(millis);
	#endif
	#ifdef WINDOWS
		Sleep(millis);
	#endif
}
