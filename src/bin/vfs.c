/* filename: vfs.c
 *  chiefengineer
 *  Sat May 15 01:06:11 PDT 2010
 */

#include <Eina.h>
#include "volume.h"
#include "vfs.h"

static Eina_List* _vfs_modules = NULL;


Vfs_Item* vfs_item_new(const char* label, const char* path)
{
	Vfs_Item* vl = calloc(1, sizeof(Vfs_Item));
	vl->label = eina_stringshare_add(label);
	vl->path = eina_stringshare_add(path);
	vl->vi = NULL;
	vl->is_menu = EINA_TRUE;
	
	return vl;
}

Vfs_Item* video_lib_item_new_withvolume(Volume_Item* vi)
{
	Vfs_Item* vli = calloc(1, sizeof(Vfs_Item));
	
	vli->label = eina_stringshare_add(vi->name);
	vli->path = eina_stringshare_add(vi->rpath);
	
	vli->is_menu = EINA_FALSE;
	vli->vi = vi;
	
	return vli;
}

void
vfs_item_free(void *data)
{
	Vfs_Item *vl;

   vl = data;
	 eina_stringshare_del(vl->label);
   eina_stringshare_del(vl->path);
	 
	 if (vl->vi) { volume_item_free(vl->vi); }
	 
   free(vl);
}

void rage_vfs_source_add(Vfs_Source* vfs_source)
{
	Eina_List* lst = NULL;
	Eina_Boolean set = EINA_FALSE;
	
	switch(vfs_source->type)
		{
		case(VFS_TYPE_VIDEO):
			{
				lst = video_sources;
				set = EINA_TRUE;
				break;
			}
		case(VFS_TYPE_AUDIO):
			{
				lst = audio_sources;
				set = EINA_TRUE;
				break;
			}
		case(VFS_TYPE_PHOTO):
			{
				lst = photo_sources;
				set = EINA_TRUE;
				break;
			}
		default:
			break;
		}
	
	if (set)
		{
			lst = eina_list_append(vfs_source);
		}
}

Vfs_Source* rage_vfs_source_get(Vfs_Type vfs_type)
{
	Vfs_Source* vfs_source = NULL;
	
	switch(vfs_type)
		{
		case(VFS_TYPE_VIDEO):
			{
				if (video_sources) { vfs_source = video_sources->data; }
				break;
			}
			
		case(VFS_TYPE_AUDIO):
			{
				if (audio_sources) { vfs_source = audio_sources->data; }
				break;
			}
			
		case(VFS_TYPE_PHOTO):
			{
				if (photo_sources) { vfs_source = photo_sources->data; }
				break;
			}
		}
	
	return vfs_source;
}
