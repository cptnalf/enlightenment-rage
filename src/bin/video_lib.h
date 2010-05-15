/* filename: video_lib.h
 *  chiefengineer
 *  Sat May 15 01:06:25 PDT 2010
 */

#ifndef _VIDEO_LIB_H
#define _VIDEO_LIB_H

#include "volume.h"

typedef struct _Video_Lib      Video_Lib;
typedef struct _Video_Lib_Item Video_Lib_Item;

struct _Video_Lib
{
	const char *label;
	const char *path;
};

struct _Video_Lib_Item
{
	const char  *label;
	const char  *path;
	Volume_Item *vi;
};


Video_Lib* video_lib_new(const char* label, const char* path);
void video_lib_free(void* data);

Video_Lib_Item* video_lib_item_new(const char* label, const char* path);
Video_Lib_Item* video_lib_item_new_withvolume(Volume_Item* vi);
void video_lib_item_free(void* data);

#endif
