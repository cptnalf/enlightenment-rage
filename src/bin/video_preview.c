/* filename: video_preview.c
 *  chiefengineer
 *  Thu May 06 01:18:04 PDT 2010
 */

#include "main.h"

static Evas_Object* _video_preview = NULL;
static Evas_Object* _mini = NULL;

void video_preview_set(const Volume_Item* vi)
{
	if (! _video_preview)
		{
			_video_preview = edje_object_add(evas);
			
			edje_object_file_set(_video_preview, theme, "mini_preview");
		}
	
	if (vi && vi->name)
		{
			edje_object_part_text_set(_video_preview, "info", vi->name);
		}
	else { edje_object_part_text_set(_video_preview, "info", ""); }
	
	if (_mini) { evas_object_del(_mini); _mini = NULL; }
	
	_mini = mini_add(_video_preview, vi->path);
	edje_object_part_swallow(_video_preview, "view", _mini);
	evas_object_show(_mini);
	
	layout_swallow("video_preview", _video_preview);
	evas_object_show(_video_preview);
	edje_object_signal_emit(_video_preview, "select", "video_preview.on");
		
	//	printf("videoset: %s 0x%X\n", vi->name, _video_preview);
}

void video_preview_activate()
{
	if (_video_preview)
		{
			edje_object_signal_emit(_video_preview, "select", "video_preview.on");
		}
}

void video_preview_deactivate()
{
	if (_video_preview)
		{
			edje_object_signal_emit(_video_preview, "unselect", "video_preview.off");
		}
}

void video_preview_destroy()
{
	if (_video_preview)
		{
			evas_object_del(_mini);
			evas_object_del(_video_preview);
			_mini = NULL;
			_video_preview = NULL;
		}
}
