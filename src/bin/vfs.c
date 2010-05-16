/* filename: vfs.c
 *  chiefengineer
 *  Sat May 15 01:06:11 PDT 2010
 */

#include <Eina.h>
#include <stdio.h>
#include "volume.h"
#include "vfs.h"

static Eina_List* video_sources = NULL;
static Eina_List* audio_sources = NULL;
static Eina_List* photo_sources = NULL;

Vfs_Item* vfs_item_new(const char* label, const char* path, int count)
{
	Vfs_Item* vl = calloc(1, sizeof(Vfs_Item));
	vl->label = eina_stringshare_add(label);
	vl->path = eina_stringshare_add(path);
	vl->count = count;
	vl->vi = NULL;
	vl->is_menu = EINA_TRUE;
	
	return vl;
}

Vfs_Item* vfs_item_new_withvolume(Volume_Item* vi)
{
	Vfs_Item* vli = calloc(1, sizeof(Vfs_Item));
	
	vli->label = eina_stringshare_add(vi->name);
	vli->path = eina_stringshare_add(vi->rpath);
	
	vli->count = 0;
	vli->is_menu = EINA_FALSE;
	vli->vi = vi;
	
	return vli;
}

Vfs_Item* vfs_item_copy(Vfs_Item* item)
{
	Vfs_Item* copy = NULL;
	
	if (item->is_menu)
		{
			copy = vfs_item_new(item->label, item->path, item->count);
		}
	else
		{
			Volume_Item* vi = volume_item_copy(item->vi);
			copy = vfs_item_new_withvolume(vi);
		}
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
	switch(vfs_source->type)
		{
		case(VFS_TYPE_VIDEO):
			{
				video_sources = eina_list_append(video_sources, vfs_source);
				break;
			}
		case(VFS_TYPE_AUDIO):
			{
				audio_sources = eina_list_append(audio_sources, vfs_source);
				break;
			}
		case(VFS_TYPE_PHOTO):
			{
				photo_sources = eina_list_append(photo_sources, vfs_source);
				break;
			}
		default:
			break;
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

void rage_vfs_sources_free()
{
	if (video_sources) { eina_list_free(video_sources); }
	if (audio_sources) { eina_list_free(audio_sources); }
	if (photo_sources) { eina_list_free(photo_sources); }
}
