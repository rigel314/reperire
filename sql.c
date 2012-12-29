/*
 * sql.c
 *
 *  Created on: Dec 5, 2012
 *      Author: cody
 */

#ifdef WINDOWS

#else
	#include <stdio.h>
	#include <string.h>
	#include <sqlite3.h>
	#include <errno.h>
#endif

#include "sql.h"
#include "misc.h"

char* dbFile = NULL;

int setDBFile(char* file)
{
	int retval;
	sqlite3* handle;

	if (!file)
	{
		char* home = getenv("HOME");
		dbFile = malloc(strlen(home) + 26);
		strcpy(dbFile, home);
		dbFile = strcat(dbFile, "/.reperire/ipsAndHosts.db");
	}
	else
		dbFile = file;

	FILE* fp = fopen(dbFile, "a+");
	if (!fp)
	{ // Test if the dbFile can be opened for writing.  And create it if it doesn't exist.
		printLogError("sqlite: fopen(dbFile)", errno);
		return 1;
	}
	fclose(fp);

	retval = sqlite3_open_v2(dbFile, &handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
	if(retval != SQLITE_OK)
	{
		printfLog("sqlite: sqlite3_open() error: %d", retval);
		sqlite3_close(handle);
		return 2;
	}

	retval = sqlite3_exec(handle, "CREATE TABLE IF NOT EXISTS hosts (pid INTEGER PRIMARY KEY AUTOINCREMENT, ip TEXT, name TEXT);", 0, 0, 0);
	if(retval != SQLITE_OK)
	{
		printfLog("sqlite: sqlite3_exec() error: Create Table: %d", retval);
		exit(1);
	}

	sqlite3_close(handle);

	return 0;
}

int addHost(const char* ip, char* hostname)
{ // Also should update if IP already exists.
	int retval;
	int update = 0;
	sqlite3* handle;
	char* queryFmt;
	sqlite3_stmt* query;

	retval = sqlite3_open_v2(dbFile, &handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
	if(retval != SQLITE_OK)
	{
		printfLog("sqlite: addHost: sqlite3_open() error: %d", retval);
		sqlite3_close(handle);
		return 1;
	}

// ----------------------Check If IP Exists without SQL injections---------------------------------+
	queryFmt = "SELECT * FROM hosts WHERE ip==?001;";											// |
																								// |
	retval = sqlite3_prepare_v2(handle, queryFmt, -1, &query, NULL);							// |
	if(retval != SQLITE_OK)																		// |
	{																							// |
		printfLog("sqlite: addHost: sqlite3_prepare_v2() error: %d", retval);					// |
		sqlite3_close(handle);																	// |
		return 2;																				// |
	}																							// |
																								// |
	retval = sqlite3_bind_text(query, 1, ip, -1, SQLITE_STATIC);								// |
	if(retval != SQLITE_OK)																		// |
	{																							// |
		printfLog("sqlite: addHost: sqlite3_bind_text(ip) error: %d", retval);					// |
		sqlite3_close(handle);																	// |
		return 3;																				// |
	}																							// |
																								// |
	retval = sqlite3_step(query);																// |
	if(retval != SQLITE_DONE && retval != SQLITE_ROW)											// |
	{																							// |
		printfLog("sqlite: addHost: sqlite3_step() error: %d", retval);							// |
		sqlite3_close(handle);																	// |
		return 4;																				// |
	}																							// |
	else if(retval == SQLITE_ROW)																// |
	{																							// |
		update = 1;																				// |
	}																							// |
																								// |
	retval = sqlite3_finalize(query);															// |
	if(retval != SQLITE_OK)																		// |
	{																							// |
		printfLog("sqlite: addHost: sqlite3_finalize() error: %d", retval);						// |
		sqlite3_close(handle);																	// |
		return 5;																				// |
	}																							// |
// -----------------------------Finally Done Checking----------------------------------------------+

	if(update)
	{ // Does exist, update not insert
	// ----------------------Update an existing row without SQL injections-----------------------------+
		queryFmt = "UPDATE hosts SET name=?002 WHERE ip=?001";										// |
																									// |
		retval = sqlite3_prepare_v2(handle, queryFmt, -1, &query, NULL);							// |
		if(retval != SQLITE_OK)																		// |
		{																							// |
			printfLog("sqlite: addHost: sqlite3_prepare_v2() error: %d", retval);					// |
			sqlite3_close(handle);																	// |
			return 2;																				// |
		}																							// |
																									// |
		retval = sqlite3_bind_text(query, 1, ip, -1, SQLITE_STATIC);								// |
		if(retval != SQLITE_OK)																		// |
		{																							// |
			printfLog("sqlite: addHost: sqlite3_bind_text(ip) error: %d", retval);					// |
			sqlite3_close(handle);																	// |
			return 3;																				// |
		}																							// |
																									// |
		retval = sqlite3_bind_text(query, 2, hostname, -1, SQLITE_STATIC);						// |
		if(retval != SQLITE_OK)																		// |
		{																							// |
			printfLog("sqlite: addHost: sqlite3_bind_text(hostname) error: %d", retval);			// |
			sqlite3_close(handle);																	// |
			return 3;																				// |
		}																							// |
																									// |
		retval = sqlite3_step(query);																// |
		if(retval != SQLITE_DONE)																	// |
		{																							// |
			printfLog("sqlite: addHost: sqlite3_step() error: %d", retval);							// |
			sqlite3_close(handle);																	// |
			return 4;																				// |
		}																							// |
																									// |
		retval = sqlite3_finalize(query);															// |
		if(retval != SQLITE_OK)																		// |
		{																							// |
			printfLog("sqlite: addHost: sqlite3_finalize() error: %d", retval);						// |
			sqlite3_close(handle);																	// |
			return 5;																				// |
		}																							// |
	// -----------------------------Finally Done Updating----------------------------------------------+
	}
	else // Does not exist, insert not update
	{
	// ----------------------Insert a new row without SQL injections-----------------------------------+
		queryFmt = "INSERT INTO hosts (ip, name) VALUES (?001, ?002);";								// |
																									// |
		retval = sqlite3_prepare_v2(handle, queryFmt, -1, &query, NULL);							// |
		if(retval != SQLITE_OK)																		// |
		{																							// |
			printfLog("sqlite: addHost: sqlite3_prepare_v2() error: %d", retval);					// |
			sqlite3_close(handle);																	// |
			return 2;																				// |
		}																							// |
																									// |
		retval = sqlite3_bind_text(query, 1, ip, -1, SQLITE_STATIC);								// |
		if(retval != SQLITE_OK)																		// |
		{																							// |
			printfLog("sqlite: addHost: sqlite3_bind_text(ip) error: %d", retval);					// |
			sqlite3_close(handle);																	// |
			return 3;																				// |
		}																							// |
																									// |
		retval = sqlite3_bind_text(query, 2, hostname, -1, SQLITE_STATIC);						// |
		if(retval != SQLITE_OK)																		// |
		{																							// |
			printfLog("sqlite: addHost: sqlite3_bind_text(hostname) error: %d", retval);			// |
			sqlite3_close(handle);																	// |
			return 3;																				// |
		}																							// |
																									// |
		retval = sqlite3_step(query);																// |
		if(retval != SQLITE_DONE)																	// |
		{																							// |
			printfLog("sqlite: addHost: sqlite3_step() error: %d", retval);							// |
			sqlite3_close(handle);																	// |
			return 4;																				// |
		}																							// |
																									// |
		retval = sqlite3_finalize(query);															// |
		if(retval != SQLITE_OK)																		// |
		{																							// |
			printfLog("sqlite: addHost: sqlite3_finalize() error: %d", retval);						// |
			sqlite3_close(handle);																	// |
			return 5;																				// |
		}																							// |
	// -----------------------------Finally Done Inserting---------------------------------------------+
	}

	sqlite3_close(handle);

	return 0;
}

void addHosts(struct sqlIPResults* entries)
{
	for(; entries != NULL; entries = entries->next)
		addHost(entries->ip, entries->name);
	return;
}

int delHost(const char* ip)
{
	int retval;
	char* queryFmt;
	sqlite3* handle;
	sqlite3_stmt* query;

	retval = sqlite3_open_v2(dbFile, &handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
	if(retval != SQLITE_OK)
	{
		printfLog("sqlite: delHost: sqlite3_open() error: %d", retval);
		sqlite3_close(handle);
		return 1;
	}

// ----------------------Delete row without SQL injections-----------------------------------------+
	queryFmt = "DELETE FROM hosts WHERE ip==?001;";												// |
																								// |
	retval = sqlite3_prepare_v2(handle, queryFmt, -1, &query, NULL);							// |
	if(retval != SQLITE_OK)																		// |
	{																							// |
		printfLog("sqlite: delHost: sqlite3_prepare_v2() error: %d", retval);					// |
		sqlite3_close(handle);																	// |
		return 2;																				// |
	}																							// |
																								// |
	retval = sqlite3_bind_text(query, 1, ip, -1, SQLITE_STATIC);								// |
	if(retval != SQLITE_OK)																		// |
	{																							// |
		printfLog("sqlite: delHost: sqlite3_bind_text(ip) error: %d", retval);					// |
		sqlite3_close(handle);																	// |
		return 3;																				// |
	}																							// |
																								// |
	retval = sqlite3_step(query);																// |
	if(retval != SQLITE_DONE)																	// |
	{																							// |
		printfLog("sqlite: delHost: sqlite3_step() error: %d", retval);							// |
		sqlite3_close(handle);																	// |
		return 4;																				// |
	}																							// |
																								// |
	retval = sqlite3_finalize(query);															// |
	if(retval != SQLITE_OK)																		// |
	{																							// |
		printfLog("sqlite: delHost: sqlite3_finalize() error: %d", retval);						// |
		sqlite3_close(handle);																	// |
		return 5;																				// |
	}																							// |
// --------------------Finally Done Deleting-------------------------------------------------------+

	sqlite3_close(handle);

	return 0;
}

struct sqlIPResults* getAllIPs()
{
	int retval;
	char* queryFmt;
	const char* val1, *val2;
	struct sqlIPResults* last;
	struct sqlIPResults* rslts = NULL;
	struct sqlIPResults* firstResult = NULL;
	sqlite3* handle;
	sqlite3_stmt* query;

	retval = sqlite3_open_v2(dbFile, &handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
	if(retval != SQLITE_OK)
	{
		printfLog("sqlite: getAllIPs: sqlite3_open() error: %d", retval);
		sqlite3_close(handle);
		return NULL;
	}

// ----------------------Select all rows-----------------------------------------------------------+
	queryFmt = "SELECT ip, name FROM hosts ORDER BY pid;";										// |
																								// |
	retval = sqlite3_prepare_v2(handle, queryFmt, -1, &query, NULL);							// |
	if(retval != SQLITE_OK)																		// |
	{																							// |
		printfLog("sqlite: getAllIPs: sqlite3_prepare_v2() error: %d", retval);					// |
		sqlite3_close(handle);																	// |
		return NULL;																			// |
	}																							// |
																								// |
	while((retval = sqlite3_step(query)) == SQLITE_ROW)										// |
	{																							// |
		val1 = (void*) sqlite3_column_text(query, 0); // Stupid difference in signedness		// |
		val2 = (void*) sqlite3_column_text(query, 1); // Stupid difference in signedness		// |
		if(!val1 || !val2)						// Casting as a void* makes the warning go away	// |
			continue;																			// |
																								// |
		last = rslts;																			// |
		rslts = calloc(1, sizeof(struct sqlIPResults));										// |
		if(!rslts)																				// |
		{																						// |
			printLogError("sqlite: getAllIPs: calloc()", errno);								// |
			sqlite3_close(handle);																// |
			return NULL;																		// |
		}																						// |
																								// |
		if(last)																				// |
			last->next = rslts;																	// |
		else																					// |
			firstResult = rslts;																// |
																								// |
		rslts->ip = calloc(strlen(val1) + 1, 1);												// |
		if(!rslts->ip)																			// |
		{																						// |
			printLogError("sqlite: getAllIPs: calloc() %d", errno);								// |
			sqlite3_close(handle);																// |
			return NULL;																		// |
		}																						// |
		strcpy(rslts->ip, val1);																// |
																								// |
		rslts->name = calloc(strlen(val2) + 1, 1);												// |
		if(!rslts->name)																		// |
		{																						// |
			printLogError("sqlite: getAllIPs: calloc() %d", errno);								// |
			sqlite3_close(handle);																// |
			return NULL;																		// |
		}																						// |
		strcpy(rslts->name, val2);																// |
	}																							// |
	if(retval != SQLITE_DONE)																	// |
	{																							// |
		printfLog("sqlite: getAllIPs: sqlite3_step() error: %d", retval);						// |
		sqlite3_close(handle);																	// |
		return NULL;																			// |
	}																							// |
																								// |
	retval = sqlite3_finalize(query);															// |
	if(retval != SQLITE_OK)																		// |
	{																							// |
		printfLog("sqlite: getAllIPs: sqlite3_finalize() error: %d", retval);					// |
		sqlite3_close(handle);																	// |
		return NULL;																			// |
	}																							// |
// ----------------------Finally Done Selecting----------------------------------------------------+

	sqlite3_close(handle);

	return firstResult;
}

void addSqlIPResult(struct sqlIPResults** ptrEntries, const char* newIP, const char* newName)
{
	struct sqlIPResults* entries;

	if(!ptrEntries)
		return;

	if(!*ptrEntries)
	{
		*ptrEntries = entries = malloc(sizeof(struct sqlIPResults));
		if(!*ptrEntries)
		{
			printLogError("server: malloc()", errno);
			return;
		}
	}
	else
	{
		for(entries = *ptrEntries; entries->next != NULL; entries = entries->next);

		entries->next = malloc(sizeof(struct sqlIPResults));
		if(!entries->next)
		{
			printLogError("insertSqlIPResult: malloc()", errno);
			return;
		}
		entries = entries->next;
	}

	entries->ip = calloc(strlen(newIP) + 1, 1);
	if(!entries->ip)
	{
		printLogError("insertSqlIPResult: calloc()", errno);
		return;
	}
	strcpy(entries->ip, newIP);

	entries->name = calloc(strlen(newName) + 1, 1);
	if(!entries->name)
	{
		printLogError("insertSqlIPResult: calloc()", errno);
		return;
	}
	strcpy(entries->name, newName);

	entries->next = NULL;
	return;
}

int countSqlIPResults(struct sqlIPResults* entries)
{
	int i;
	for(i = 0; entries != NULL; entries = entries->next, i++);
	return i;
}

void freeSqlIPResult(struct sqlIPResults* rslt)
{
	if(!rslt) // Passed a null pointer
		return;

	free(rslt->ip);
	free(rslt->name);
	freeSqlIPResult(rslt->next);
	free(rslt);

	return;
}
