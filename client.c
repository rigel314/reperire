/*
 * client.c
 *
 *  Created on: Dec 5, 2012
 *      Author: cody
 */


void _sleep(int millis)
{
	#ifdef LINUX
		sleep(millis);
	#endif
	#ifdef WINDOWS
		Sleep(millis);
	#endif
}
