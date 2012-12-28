/*
 * misc.c
 *
 *  Created on: Dec 18, 2012
 *      Author: cody
 */

#ifdef WINDOWS
	#include <windows.h>

	#define LnEND "\r\n"
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

	#define LnEND "\n"
#endif

#include "misc.h"
#include "sql.h"

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

char* getHostsFile()
{
	#ifdef WINDOWS
		return "C:\\Windows\\System32\\Drivers\\etc\\hosts";
	#else
		//return "/home/cody/.reperire/blar";
		return "/etc/hosts";
	#endif
}

struct fileLines* mkFileLines(char* file)
{
	FILE* fp;
	struct fileLines* lines = NULL, *lastLn, *head;
	int c, lastC = '\n';
	int loc;

	fp = fopen(file, "r");
	if(!fp)
	{
		printLogError("mkFileLines: fopen()", errno);
		return NULL;
	}

	while((c = fgetc(fp)) != -1)
	{
		if(lastC == '\n')
		{
			lastLn = lines;
			lines = calloc(1, sizeof(struct fileLines));
			if(!lines)
			{
				printLogError("mkFileLines: calloc()", errno);
				return NULL;
			}

			if(lastLn)
			{
				lastLn->next = lines;
				lastLn->line = realloc(lastLn->line, loc + 1);
				if(!lastLn->line)
				{
					printLogError("mkFileLines: calloc() or realloc()", errno);
					return NULL;
				}
				*(lastLn->line + loc) = 0;
			}
			else
				head = lines;

			loc = 0;
		}

		if(lines->line)
			lines->line = realloc(lines->line, loc + 1);
		else
			lines->line = calloc(1, 1);
		if(!lines->line)
		{
			printLogError("mkFileLines: calloc() or realloc()", errno);
			return NULL;
		}

		*(lines->line + loc++) = c;

		lastC = c;
	}
	if(*(lines->line + (loc - 1)) != '\n')
	{
		lines->line = realloc(lines->line, loc + 2);
		lines->line = realloc(lines->line, strlen(lines->line) + 2);
		if(!lines->line)
		{
			printLogError("mkFileLines: realloc()", errno);
			return NULL;
		}
		*(lines->line + loc++) = '\n';
	}
	else
	{
		lines->line = realloc(lines->line, loc + 1);
	}
	*(lines->line + loc) = 0;

	fclose(fp);
	return head;
}

void freeFileLines(struct fileLines* lines)
{
	if(!lines) // Passed a null pointer
		return;

	free(lines->line);
	freeFileLines(lines->next);
	free(lines);

	return;
}

struct fileLines* freeSomeFileLines(struct fileLines* lines, char* stop)
{
	struct fileLines* line;

	if(!lines) // Passed a null pointer
		return NULL;

	if(strcmp(lines->line, stop))
	{
		free(lines->line);
		line = lines->next;
		free(lines);
		return freeSomeFileLines(line, stop);
	}

	return lines;
}

void insertFileLine(struct fileLines* lines, char* new)
{
	struct fileLines* line;

	if(!lines)
		return;

	line = lines->next;
	lines->next = malloc(sizeof(struct fileLines));
	if(!lines->next)
	{
		printLogError("insertFileLine: realloc()", errno);
		return;
	}
	lines = lines->next;
	lines->line = calloc(strlen(new) + 1, 1);
	if(!lines->line)
	{
		printLogError("insertFileLine: realloc()", errno);
		return;
	}
	strcpy(lines->line, new);
	lines->next = line;

	return;
}

void writeHosts()
{
	struct sqlIPResults* IPs, *IP;
	struct fileLines* lines, *line;

	lines = mkFileLines(getHostsFile());
	if(!lines)
	{
		printLog("writeHosts: mkFileLines() failed");
		return;
	}
	for(line = lines; line->next != NULL && strcmp(line->line, "#==begin reperire==DO NOT MODIFY==\n"); line = line->next);
	if(line->next)
		line->next = freeSomeFileLines(line->next, "#==end reperire==DO NOT MODIFY==\n");
	else
	{
		insertFileLine(line, "#==begin reperire==DO NOT MODIFY==\n");
		line = line->next;
	}
	insertFileLine(line, "# No changes between the begin and end will be saved.\n");
	line = line->next;

	IPs = getAllIPs();
	for(IP = IPs; IP != NULL; IP = IP->next)
	{
		char* hostsEntry;

		hostsEntry = malloc(strlen(IP->ip) + strlen(IP->name) + 3);
		if(!hostsEntry)
		{
			printLogError("writeHosts: malloc()", errno);
			return;
		}
		sprintf(hostsEntry, "%s\t%s\n", IP->ip, IP->name);
		insertFileLine(line, hostsEntry);
		line = line->next;
	}
	freeSqlIPResult(IPs);

	if(!line->next)
		insertFileLine(line, "#==end reperire==DO NOT MODIFY==\n");

	FILE* fp = fopen(getHostsFile(), "w");
	if(!fp)
	{
		printLogError("writeHosts: fopen()", errno);
		freeFileLines(lines);
		return;
	}
	for(line = lines; line != NULL; line = line->next)
	{
		fwrite(line->line, 1, strlen(line->line), fp);
	}
	fclose(fp);

	freeFileLines(lines);

	return;
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
