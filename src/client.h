/*
 * client.h
 *
 *  Created on: Dec 5, 2012
 *      Author: cody
 */

#ifndef CLIENT_H_
#define CLIENT_H_

int sendBroadcast();
int sendAlivePacket(const char* dest);
int sendReplyPacket(const char* dest);

#endif /* CLIENT_H_ */
