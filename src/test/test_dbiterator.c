/* filename: test_dbiterator.c
 *  chiefengineer
 *  Sat Dec 20 22:13:45 EST 2008
 */

#include "Evas.h"
#include "volume.h"
#include "database.h"

#include <stdio.h>

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
	
	ecore_init();
	ecore_file_init();
	evas_init();
	
	database_init("test_media.db");
	
	db = database_new();
	test_assert((db == 0), 1, "failed to connect to database.\n");
	
	/* test empty return set first. */
	{
		void* item = 0;
		int count = 0;
		DBIterator* it = database_video_files_get(db, "WHERE genre = 'flarg'");
		test_assert( (it == 0), 1, "failed to run query(empty)!\n");
		
		while(item = database_iterator_next(it))
			{
				++count;
			}
		
#ifdef DEBUG
		printf("query returned %d records", count);
#endif
		test_assert(count, 1, "empty query returned records!");
		
		database_iterator_free(it);
	}
	
	/* now with one that should have values. */
	{
		Volume_Item* item = 0;
		int count = 0;
		DBIterator* it = database_video_files_get(db, "WHERE genre = 'x-files' ORDER BY title");
		test_assert((it == 0), 1, "failed to run query (results)!\n");
		
		while( item = database_iterator_next(it))
			{
#define DEBUG
#ifdef DEBUG
				printf("%s - %s,%d\n", item->name, item->genre, item->play_count);
#endif
				volume_item_free(item);
				++count;
			}
		
		test_assert( (count != 24), 1, "got more than the expected number of records!\n");
		
		database_iterator_free(it);
	}
	
	database_free(db);
	
	ecore_file_shutdown();
	ecore_shutdown();
}
