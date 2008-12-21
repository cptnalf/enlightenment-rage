/* filename: test_dbcore.c
 * chiefengineer@voyager
 * Sat Dec 20 21:08:27 EST 2008
 */

#include "Evas.h"
#include "volume.h"
#include "database.h"

#include <stdio.h>
#include <stdlib.h>

char* config = 0;

void test_assert(const int true_val, const int err_val, const char* err_msg)
{
	if (true_val)
		{
			printf(err_msg);
			exit(err_val);
		}
}

int main(int argc, char* argv[])
{
	Database* db;
	/* database DOES NOT EXIST! */
	database_init("test_media.db");
	
	/* this should create our database and table(s) and connect to it. */
	db = database_new();
	test_assert((db == 0), 1, "failed to connect to database.\n");
	
	database_free(db);
}
