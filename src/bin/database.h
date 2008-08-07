
#include <sqlite3.h>

typedef struct _Database Database;
struct _Database
{
	sqlite3* db;
};

extern Database* database_new(const char* path);
extern void database_free(Database* db);
extern Evas_List* database_get_files(Database* db, const char* path);
extern void database_delete_file(Database* db, const char* path);
extern void database_insert_file(Database* db, const Volume_Item* item);
extern Evas_List* database_favorites_get(Database* db);
