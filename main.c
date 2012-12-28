/*
 * main.c
 *
 *  Created on: Dec 4, 2012
 *      Author: cody
 *
 *  TODO: cli argument for log level
 *  TODO: cli argument for bind address
 *  TODO: find a way to to determine if host never replied to alive packet
 *  TODO: find a less hackish lookup method than editing /etc/hosts
 *
 */

#ifdef WINDOWS
	#include <windows.h>
#else
	#include <stdio.h>
	#include <string.h>
	#include <errno.h>
	#include <unistd.h>
#endif

#include "server.h"
#include "client.h"
#include "signals.h"
#include "misc.h"
#include "sql.h"

int main()
{
	char* name = malloc(100);
	//struct fileLines* lines, *line;

	setLogFile(NULL); // Set the logfile to the default. "~/.reperire/reperire.log"
	FILE* fp = fopen(logfile, "a+");
	if (!fp)
	{ // Test if the logfile can be opened for writing.  If not, use stderr instead.
		perror("fopen(logfile)"); // All the Log functions will default to stderr upon failing fopen(logfile) as well.
		fp = stderr;
	}
	fputs("\n\n",fp);
	if(fp != stderr)
		fclose(fp);
	printLog("Reperire Log");

	if(gethostname(name, 100))
	{
		printLogError("gethostname()", errno);
		return 1;
	}
	setHostname(name);

	setDBFile(NULL);

	createSigHandlers();

	createServer();

	return 0;
}
