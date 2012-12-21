/*
 * main.c
 *
 *  Created on: Dec 4, 2012
 *      Author: cody
 */

#ifdef WINDOWS
	#include <winsock.h>
	#include <windows.h>
#else
	#include <stdio.h>
	#include <string.h>
	#include <errno.h>
#endif

#include "server.h"
#include "client.h"
#include "signals.h"
#include "misc.h"

int main()
{
	setLogFile(NULL); // Set the logfile to the default. "~/.reperire.log"
	FILE* fp = fopen(logfile,"a+");
	if (!fp)
	{ // Test if the logfile can be opened for writing.  If not, use stderr instead.
		perror("fopen"); // All the Log functions will default to stderr upon failing fopen(logfile) as well.
		fp = stderr;
	}
	fputs("\n\n",fp);
	if(fp != stderr)
		fclose(fp);
	printLog("Reperire Log");

	createSigHandlers();

	createServer();

	return 0;
}
