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
	char* name;
	struct sqlIPResults* next;
};

extern char* dbFile;

int setDBFile(char* file);
int addHost(const char* ip, char* hostname);
void addHosts(struct sqlIPResults* entries);
int delHost(const char* ip);
struct sqlIPResults* getAllIPs();
void addSqlIPResult(struct sqlIPResults** ptrEntries, const char* newIP, const char* newName);
int countSqlIPResults(struct sqlIPResults* entries);
void freeSqlIPResult(struct sqlIPResults* blar);

#endif /* SQL_H_ */
