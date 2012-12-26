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
	#include <signal.h>
	#include <sys/stat.h>
	#include <stdlib.h>
#endif

#include "misc.h"

char* logfile = NULL;
char* bindaddr = NULL;
//char* bindaddr = "192.168.0.100"; // Or whatever.
char* hostname = NULL;
volatile sig_atomic_t numProcs = 1;

void setLogFile(char* file)
{
	if(!file)
	{
		char* home = getenv("HOME");
		logfile = malloc(strlen(home) + 24);
		strcpy(logfile, home);
		logfile = strcat(logfile, "/.reperire");
		_mkdir(logfile);
		logfile = strcat(logfile, "/reperire.log");
	}
	else
		logfile = file;
}

void setHostname(char* host)
{
	if(!host)
		hostname = "Damn Segfaults";
	else
		hostname = host;
}

void printLog(char* msg)
{
	time_t now;
	struct tm* 	lcltime;

	now = time(NULL);
	lcltime = localtime(&now);
	FILE* fp = fopen(logfile,"a+");
	if (!fp)
	{
		fp = stderr;
	}
	fprintf(fp, "%d-%02d-%02d %02d:%02d:%02d ~ %s\n", lcltime->tm_year + 1900, lcltime->tm_mon + 1, lcltime->tm_mday, lcltime->tm_hour, lcltime->tm_min, lcltime->tm_sec, msg);
	if(fp != stderr)
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

	free(msg);
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

int checkValidBuf(char* buf, int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		if(buf[i] == 3)
		{
			buf[i] = 0;
			return 1;
		}
	}
	return 0;
}

unsigned int mkrand()
{
#ifdef WINDOWS
	return rand();
#else
	int out;
	int i;
	FILE* fp = fopen("/dev/urandom", "r");
	if(!fp)
		return 4;
	for(i=0; i<4; i++)
	{
		*(((char*)(&out))+i)=(char)fgetc(fp);
	}
	fclose(fp);
	return out;
#endif
}

void _mkdir(char* path)
{
	#ifdef WINDOWS
		return;
	#else
		mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); // 755
	#endif
}

void _sleep(int millis)
{
	#ifdef WINDOWS
		Sleep(millis);
	#else
		usleep(1000*millis);
	#endif
}
