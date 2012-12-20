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
#endif

#define HOSTBLAR "example.com"

extern char* logfile;
extern char* bindaddr;
extern FILE* fp;

void setLogFile(char* file);
void printLog(char* msg);
void printfLog(char* fmt, ...);
void printLogError(char* msg, int err);
void _sleep(int millis);

#endif /* MISC_H_ */
