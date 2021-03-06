/*
 * signals.h
 *
 *  Created on: Dec 18, 2012
 *      Author: cody
 */

#ifndef SIGNALS_H_
#define SIGNALS_H_

#ifdef WINDOWS
	#include <windows.h>
#else
	#include <signal.h>
#endif

extern volatile sig_atomic_t got_sigint;
extern volatile sig_atomic_t got_sigusr1;

int createSigHandlers();
int createChildSigHandlers();

#endif /* SIGNALS_H_ */
