/*
 * signals.c
 *
 *  Created on: Dec 18, 2012
 *      Author: cody
 */

#ifdef WINDOWS
	#include <windows.h>
#else
	#include <signal.h>
	#include <sys/wait.h>
	#include <stdlib.h>
	#include <errno.h>
	#include <string.h>
	#include <unistd.h>
#endif

#include "signals.h"
#include "misc.h"

volatile sig_atomic_t got_sigint = 0;
volatile sig_atomic_t got_sigusr1 = 0;

void sigHandler(int sig)
{ // Handler for the three registered signals.
	switch(sig)
	{
		case SIGINT:
			got_sigint = 1;
			break;
		case SIGTERM:
			got_sigint = 1;
			break;
		case SIGCHLD:
			while(waitpid(-1, NULL, WNOHANG) > 0)
			{
				numProcs--;
			}
			break;
		case SIGUSR1:
			got_sigusr1 = 1;
			break;
	}
}

void sigHandlerChildren(int sig)
{ // Handler for the three registered signals.
	if(sig == SIGINT)
		_exit(0);
	if(sig == SIGUSR1)
		got_sigusr1 = 1;
}

int createSigHandlers()
{ // Register a signal handler function for SIGINT, SIGTERM, and SIGCHLD.
	struct sigaction sa;
	int retval = 0;

	sa.sa_handler = sigHandler;
	sa.sa_flags = 0; // or SA_RESTART
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask,SIGINT);
	sigaddset(&sa.sa_mask,SIGTERM);
	sigaddset(&sa.sa_mask,SIGCHLD);
	sigaddset(&sa.sa_mask,SIGUSR1);

	if (sigaction(SIGINT,&sa,NULL) == -1)
	{
		printLogError("SIGINT (Ctrl-C) handler failed to register", errno);
		retval |= 1;
	}
	if (sigaction(SIGTERM,&sa,NULL) == -1)
	{
		printLogError("SIGTERM (kill) handler failed to register", errno);
		retval |= 2;
	}
	if (sigaction(SIGCHLD,&sa,NULL) == -1)
	{
		printLogError("SIGCHLD (child process terminated) handler failed to register", errno);
		retval |= 4;
	}
	if (sigaction(SIGUSR1,&sa,NULL) == -1)
	{
		printLogError("SIGUSR1 handler failed to register", errno);
		retval |= 8;
	}

	return retval;
}

int createChildSigHandlers()
{ // Register a signal handler function for SIGINT, SIGTERM, and SIGCHLD.
	struct sigaction sa;
	int retval = 0;

	sa.sa_handler = sigHandlerChildren;
	sa.sa_flags = 0; // or SA_RESTART
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask,SIGINT);
	sigaddset(&sa.sa_mask,SIGTERM);
	sigaddset(&sa.sa_mask,SIGUSR1);

	if (sigaction(SIGINT,&sa,NULL) == -1)
	{
		printLogError("SIGINT (Ctrl-C) handler failed to register", errno);
		retval |= 1;
	}
	if (sigaction(SIGTERM,&sa,NULL) == -1)
	{
		printLogError("SIGTERM (kill) handler failed to register", errno);
		retval |= 2;
	}
	if (sigaction(SIGUSR1,&sa,NULL) == -1)
	{
		printLogError("SIGUSR1 handler failed to register", errno);
		retval |= 8;
	}

	return retval;
}
