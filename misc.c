/*
 * misc.c
 *
 *  Created on: Dec 18, 2012
 *      Author: cody
 */

#ifdef WINDOWS
	#include <windows.h>
#else
	#include <stdio.h>
	#include <time.h>
	#include <string.h>
	#include <errno.h>
	#include <unistd.h>
	#include <stdarg.h>
#endif

#include "misc.h"

char* logfile = ".reperire.log";
char* bindaddr = NULL;
//char* bindaddr = "192.168.0.100"; // Or whatever.
FILE* fp;

void setLogFile(char* file)
{
	if(!file)
		logfile = strcat(getenv("HOME"), "/.reperire.log");
	else
		logfile = file;
}

void printLog(char* msg)
{
	time_t now;
	struct tm *lcltime;

	now = time(NULL);
	lcltime = localtime(&now);
	fp = fopen(logfile,"a+");
	if (!fp)
	{
		return;
	}
	fprintf(fp, "%d-%02d-%02d %02d:%02d:%02d ~ %s\n", lcltime->tm_year + 1900, lcltime->tm_mon + 1, lcltime->tm_mday, lcltime->tm_hour, lcltime->tm_min, lcltime->tm_sec, msg);
	fclose(fp);
}

void printfLog(char* fmt, ...)
{
	char* msg;

	va_list ap;
	va_start(ap, fmt);

	msg = malloc(vsnprintf(NULL, 0, fmt, ap) + 1);
	va_end(ap);
	if(!msg)
	{
		printLog(strerror(errno));
		return;
	}

	va_start(ap, fmt); // The vsnprintf call clobbered my va_list.  So starting it again.

	vsprintf(msg, fmt, ap);
	printLog(msg);

	va_end(ap);
}

void printLogError(char* msg, int err)
{
	char* logmsg;
	char* errmsg;

	errmsg = strerror(err);

	logmsg = malloc(strlen(msg) + strlen(errmsg) + 3);
	if(!logmsg)
	{
		printLog(errmsg);
		printLog(strerror(errno));
		return;
	}

	sprintf(logmsg, "%s: %s", msg, errmsg);
	printLog(logmsg);
	free(logmsg);
}

void _sleep(int millis)
{
	#ifdef WINDOWS
		Sleep(millis);
	#else
		usleep(1000*millis);
	#endif
}
