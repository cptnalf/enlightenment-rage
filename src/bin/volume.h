
#ifndef VOLUME_H
#define VOLUME_H

typedef struct _Volume_Item Volume_Item;

struct _Volume_Item
{
	long long id;
   char       *path;
   char       *rpath;
   char       *name;
   const char *genre;
   const char *type;
   double      last_played;
   int         play_count;
   double      last_pos;
   double      length;
	const char       *artist;
	const char       *album;
   int         track;
};

void volume_init(void);
void volume_update(void);
void volume_load(void);
void volume_add(char *vol);
void volume_del(char *vol);
int  volume_exists(char *vol);
void volume_index(char *vol);
void volume_deindex(char *vol);
int  volume_type_num_get(char *type);
const Eina_List *volume_items_get(void);

Volume_Item* volume_item_new(const long long id, const char* path, const char* name, const char* genre, const char* type);
Volume_Item* volume_item_copy(Volume_Item* item);
void volume_item_free(Volume_Item* item);
    
extern int VOLUME_ADD;
extern int VOLUME_DEL;
extern int VOLUME_TYPE_ADD;
extern int VOLUME_TYPE_DEL;
extern int VOLUME_SCAN_START;
extern int VOLUME_SCAN_STOP;
extern int VOLUME_SCAN_GO;

#endif
