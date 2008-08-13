
#include <sqlite3.h>

typedef struct _Database Database;
struct _Database
{
	sqlite3* db;
};

extern void database_init(const char* path);
extern Database* database_new();
extern void database_free(Database* db);
extern Evas_List* database_video_genres_get(Database* db);
extern int database_video_genre_count_get(Database* db, const char* genre);
extern Evas_List* database_video_files_get(Database* db, const int filter_type, const char* str);
extern void database_video_file_del(Database* db, const char* path);
extern void database_video_file_add(Database* db, const Volume_Item* item);
extern Evas_List* database_video_favorites_get(Database* db);
