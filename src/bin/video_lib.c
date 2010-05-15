/* filename: video_lib.c
 *  chiefengineer
 *  Sat May 15 01:06:11 PDT 2010
 */

#include <Eina.h>
#include "volume.h"
#include "video_lib.h"

Video_Lib* video_lib_new(const char* label, const char* path)
{
	Video_Lib* vl = calloc(1, sizeof(Video_Lib));
	vl->label = eina_stringshare_add(label);
	vl->path = eina_stringshare_add(path);
	
	return vl;
}

void
video_lib_free(void *data)
{
	Video_Lib *vl;

   vl = data;
   eina_stringshare_del(vl->label);
   eina_stringshare_del(vl->path);
   free(vl);
}

Video_Lib_Item* video_lib_item_new(const char* label, const char* path)
{
	Video_Lib_Item* vli = calloc(1, sizeof(Video_Lib_Item));
	vli->label = eina_stringshare_add(label);
	vli->path = eina_stringshare_add(path);
	vli->vi = NULL;
	return vli;
}

Video_Lib_Item* video_lib_item_new_withvolume(Volume_Item* vi)
{
	Video_Lib_Item* vli = calloc(1, sizeof(Video_Lib_Item));
	vli->label = eina_stringshare_add(vi->name);
	vli->path = eina_stringshare_add(vi->rpath);
	vli->vi = vi;
	
	return vli;
}

void
video_lib_item_free(void *data)
{
	Video_Lib_Item *vli;

	vli = data;
	eina_stringshare_del(vli->label);
	eina_stringshare_del(vli->path);
	volume_item_free(vli->vi);
	free(vli);
}
