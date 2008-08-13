
#include <Evas.h>
#include <stdlib.h>
#include <stdio.h>
#include "volume.h"
#include "database.h"
#include <string.h>

extern Volume_Item* volume_item_new(const char* path, const char* name, const char* genre);

static Evas_List* _database_results_get(char** result_table, const int rows, const int cols);
static char db_path[4096];

void database_init(const char* path)
{
	snprintf(db_path, sizeof(db_path), "%s", path);
}

/** create a connection to a database at path.
 *  @return pointer to the database, or NULL on failure.
 */
Database* database_new()
{
	int result;
	Database* db = calloc(1, sizeof(Database));
	
	//printf("db=%s\n", path);
	
	result = sqlite3_open(db_path, &db->db);
	if (result)
		{
			fprintf(stderr, "error: %s\n", sqlite3_errmsg(db->db));
			sqlite3_close(db->db);
			free(db);
			db = 0;
		}
	
	if (db)
		{
			char* errmsg;
			result = sqlite3_exec(db->db,
														"CREATE TABLE video_files("
														"path TEXT PRIMARY KEY,"  // sha hash of the path
														"genre TEXT,"             // genre of the file
														"title TEXT,"             // title of the file.
														"f_type TEXT,"            // type of file (video, audio, photo
														"playcount INTEGER,"      // number of times its played.
														"length INTEGER,"         // length in seconds.
														"lastplayed INTEGER)"     // time_t it was last played
														, NULL, NULL, &errmsg);
			
			if (result != SQLITE_OK)
				{
					/* don't care about the error message.
					fprintf(stderr, "unable to create table! :%s\n", errmsg);
					*/
				}
		}
	return db;
}

/** free a database connected to with 'database_new'
 */
void database_free(Database* db)
{
	//printf("closing the db.\n");
	sqlite3_close(db->db);
	free(db);
}

/** retrieve all the files in the database.
 *  @return list or NULL if error (or no files)
 */
Evas_List* database_video_files_get(Database* db, int filter_type, const char* str)
{
	char* error_msg;
	int result;
	int rows, cols;
	char** tbl_results=0;
	char* query;
	Evas_List* list;
	
	switch(filter_type)
		{
		default:
		case(0):
			{
				query = 
					sqlite3_mprintf("SELECT "
													"path, title, genre, f_type, playcount, length, lastplayed "
													"FROM video_files "
													"ORDER BY path");
				break;
			}
		case(1):
			{
				query = 
					sqlite3_mprintf("SELECT "
													"path, title, genre, f_type, playcount, length, lastplayed "
													"FROM video_files "
													"WHERE path like '%q%s' ORDER BY path",
													str, "%");
				break;
			}
		case(2):
			{
				query = 
					sqlite3_mprintf("SELECT "
													"path, title, genre, f_type, playcount, length, lastplayed "
													"FROM video_files "
													"WHERE genre = '%q' "
													"ORDER BY path ",
													str);
				break;
			}
		};
	
	result = sqlite3_get_table(db->db, query, &tbl_results, &rows, &cols, &error_msg);
	if (SQLITE_OK == result)
		{
			list = _database_results_get(tbl_results, rows, cols);
			
			sqlite3_free_table(tbl_results);
		}
	sqlite3_free(query);
	
	return list;
}

Evas_List* database_video_favorites_get(Database* db)
{
	Evas_List* list;
	char** tbl_results=0;
	int rows, cols;
	int result;
	char* error_msg;
	char* query = "SELECT "
		"path, title, genre, f_type, playcount, length, lastplayed "
		"FROM video_files "
		"WHERE playcount > 0 "
		"ORDER BY playcount, path "
		"LIMIT 50";
	
	result = sqlite3_get_table(db->db, query, &tbl_results, &rows, &cols, &error_msg);
	if (SQLITE_OK == result)
		{
			list = _database_results_get(tbl_results, rows, cols);
			sqlite3_free_table(tbl_results);
		}
	
	return list;
}

/** get a list of the genres in the database.
 */
Evas_List* database_video_genres_get(Database* db)
{
	Evas_List* list=0;
	char** tbl_results = 0;
	int rows, cols;
	int result;
	char* error_msg;
	char* query = "SELECT DISTINCT genre FROM video_files ORDER BY genre";
	
	result = sqlite3_get_table(db->db, query, &tbl_results, &rows, &cols, &error_msg);
	if (SQLITE_OK == result)
		{
			if (rows > 0)
				{
					const char* genre;
					int i;
					int max_item = rows * cols;
					
					for(i=cols; i <= max_item; i += cols)
						{
							genre = evas_stringshare_add(tbl_results[i + 0]);
							list = evas_list_append(list, genre);
							genre = 0;
						}
				}
			//sqlite3_free(tbl_results);
		}
	
	return list;
}

int database_video_genre_count_get(Database* db, const char* genre)
{
	char** tbl_results =0;
	int rows, cols;
	int result;
	char* error_msg;
	char* query;
	int count = 0;
	
	query = sqlite3_mprintf("SELECT COUNT(path) FROM video_files WHERE genre = %Q",
													genre);
	result = sqlite3_get_table(db->db, query, &tbl_results, &rows, &cols, &error_msg);
	if (SQLITE_OK == result)
		{
			if (rows > 0)
				{
					count = atoi(tbl_results[1]);
				}
			//sqlite3_free(tbl_results);
		}
	sqlite3_free(query);
	
	return count;
}

/** delete a file from the database.
 */
void database_video_file_del(Database* db, const char* path)
{
	int result;
	char* error_msg;
	char* query = sqlite3_mprintf("DELETE FROM video_files WHERE path = %Q",
																path);
	
	printf("%s\n", query);
	result = sqlite3_exec(db->db, query, NULL, NULL, &error_msg);
	if (result != SQLITE_OK)
		{
			fprintf(stderr, "db: delete error: %s; %s\n", query, error_msg);
			sqlite3_free(error_msg);
		}
	
	sqlite3_free(query);
}

/** add a new file to the database
 */
void database_video_file_add(Database* db, const Volume_Item* item)
{
	int result;
	char* error_msg =0;
	char* query = sqlite3_mprintf(
		 "INSERT INTO video_files (path, title, genre, f_type, playcount, length, lastplayed) "
		 "VALUES(%Q, %Q, %Q, %Q, %d, %d, %d)",
		 item->path, item->name, item->genre, item->type,
		 item->play_count, item->length, item->last_played);
	
	result = sqlite3_exec(db->db, query, NULL, NULL, &error_msg);
	if (result != SQLITE_OK)
		{
			fprintf(stderr, "db: insert error: \"%s\"; %s\n", query, error_msg);
			sqlite3_free(error_msg);
		}
	
	sqlite3_free(query);
}

/* played the file.
 */
void database_video_file_update(Database* db, Volume_Item* item)
{
	int result;
	char* error_msg;
	char* query;
	time_t lp = time(0);
	
	/* update the values so they can be written to the database. */
	item->play_count ++;
	item->last_played = 0.0 + lp;
	
	query = sqlite3_mprintf("UPDATE video_files "
													"SET playcount = %d, lastplayed = %d "
													"WHERE path = '%q' ",
													item->play_count, lp, item->path);
	printf ("%s;%s;%s\n", query, item->path, item->name);
	
	result = sqlite3_exec(db->db, query, NULL, NULL, &error_msg);
	if (SQLITE_OK != result)
		{
			fprintf(stderr, "db: update error:\"%s\"; %s\n", query, error_msg);
			sqlite3_free(error_msg);
		}
	sqlite3_free(query);
}

/* you HAVE TO SELECT AT LEAST (IN THIS ORDER)
 * path, title, genre, f_type, playcount, length, lastplayed
 */
static Evas_List* _database_results_get(char** result_table, const int rows, const int cols)
{
#define COL_PATH       0
#define COL_TITLE      1
#define COL_GENRE      2
#define COL_FTYPE      3
#define COL_PLAYCOUNT  4
#define COL_LENGTH     5
#define COL_LASTPLAYED 6

	Evas_List* list=0;
	Volume_Item* item;
	int i = 0;
	const int max_item = rows*cols;
	
	//printf("%dx%d\n", rows, cols);
	
	if (rows > 0)
		{
			for(i=cols; i <= max_item; i += cols)
				{
					/*
						printf ("%s;%s;%s\n", result_table[i+COL_PATH], result_table[i + COL_TITLE],
						result_table[i + COL_GENRE]);
					*/
					item = volume_item_new(result_table[i + COL_PATH], result_table[i + COL_TITLE],
																 result_table[i +COL_GENRE]);
					
					item->type = strdup(result_table[i + COL_FTYPE]);
					item->play_count = atoi(result_table[i+COL_PLAYCOUNT]);
					item->length = atoi(result_table[i+COL_LENGTH]);
					item->last_played = atoi(result_table[i+COL_LASTPLAYED]);
					
					list = evas_list_append(list, item);
					item = 0;
				}
		}
	
	return list;
}
