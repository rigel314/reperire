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
	fp = fopen(logfile,"a+");
	if (!fp)
	{ // Test if the logfile can be opened for writing.  If not, die.
		perror("fopen");
		exit(1);
		//setLogFile(stderr); // Maybe in the future.
	}
	fputs("\n\n",fp);
	fclose(fp);
	printLog("Reperire Log");

	createSigHandlers();

	createServer();

	return 0;
}
