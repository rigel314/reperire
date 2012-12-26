/*
 * sql.h
 *
 *  Created on: Dec 5, 2012
 *      Author: cody
 */

#ifndef SQL_H_
#define SQL_H_

struct sqlIPResults
{
	char* ip;
	struct sqlIPResults* next;
};

extern char* dbFile;

int setDBFile(char* file);
int addHost(const char* ip, char* hostname);
int delHost(const char* ip);
struct sqlIPResults* getAllIPs();
void freeSqlIPResult(struct sqlIPResults* blar);

#endif /* SQL_H_ */
