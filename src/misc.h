/*
 * misc.h
 *
 *  Created on: Dec 18, 2012
 *      Author: cody
 */

#ifndef MISC_H_
#define MISC_H_

#ifdef WINDOWS
	#include <windows.h>
#else
	#include <stdio.h>
	#include <stdlib.h>
	#include <signal.h>
#endif

#define HOSTBLAR "example.com"

struct fileLines
{
	char* line;
	struct fileLines* next;
};

extern char* logfile;
extern char* bindaddr;
extern char* hostname;
extern volatile sig_atomic_t numProcs;

void setLogFile(char* file);
void setHostname(char* host);
void printLog(char* msg);
void printfLog(char* fmt, ...);
void printLogError(char* msg, int err);
int checkValidBuf(char* buf, int size);
struct fileLines* mkFileLines(char* file);
void writeHosts();
unsigned int mkrand();
void _mkdir(char* path);
void _sleep(int millis);

#endif /* MISC_H_ */
