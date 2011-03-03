/* filename: vfs.h
 *  chiefengineer
 *  Fri May 07 23:11:02 PDT 2010
 */

#ifndef VFS_H
#define VFS_H

#include "volume.h"

/* typedef struct _Rage_Vfs_File Rage_Vfs_File; */

/* struct _Rage_Vfs_File */
/* { */
/* 	char* uri; */
/* 	char* label; */
/* 	char* icon; */
/* 	char* icon_file; */
/* 	unsigned char is_directory : 1; */
/* 	unsigned char is_menu : 1; */
/* }; */

typedef enum {
	VFS_TYPE_VIDEO,
	VFS_TYPE_AUDIO,
	VFS_TYPE_PHOTO,
} Vfs_Type;

typedef struct _Vfs_Source Vfs_Source;
typedef struct _Vfs_Item Vfs_Item;

struct _Vfs_Item
{
	const char  *label;
	const char  *path;
	int count;
	Volume_Item *vi;
	
	unsigned char is_menu : 1;
};

Vfs_Item* vfs_item_new(const char* label, const char* path, int count);
Vfs_Item* vfs_item_new_withvolume(Volume_Item* vi);
Vfs_Item* vfs_item_copy(Vfs_Item* item);
void vfs_item_free(void* data);

struct _Vfs_Source
{
	int type;
	const char* name;
	const char* item_icon;
	const char* menu_icon;
	
	Eina_List* (*get_items)(Vfs_Item* item);
	Eina_List* (*get_recents)(int count);
	Eina_List* (*get_favorites)(int count);
	Eina_List* (*get_news)(int count);
	void (*record_play)(Vfs_Item* item);
};

void rage_vfs_source_add(Vfs_Source* vfs_source);
Vfs_Source* rage_vfs_source_get(Vfs_Type vfs_type);
void rage_vfs_sources_free();

#endif
