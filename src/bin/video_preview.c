/* filename: video_preview.c
 *  chiefengineer
 *  Thu May 06 01:18:04 PDT 2010
 */

#include "main.h"
#include "metadata.h"

static Evas_Object* _video_preview = NULL;
static Evas_Object* _mini = NULL;

void video_preview_set(Volume_Item* vi)
{
	if (! _video_preview)
		{
			_video_preview = edje_object_add(evas);
			
			edje_object_file_set(_video_preview, theme, "mini_preview");
		}
	
	/* blank it out. */
	edje_object_part_text_set(_video_preview, "preview.desc", "");
	if (vi)
		{
			Metadata* ep = metadata_get(vi);
			Eina_Bool name_set;
			char buf[100];
			
			if (ep)
				{ 
					if (ep->synopsis) 
						{ edje_object_part_text_set(_video_preview, "preview.desc", ep->synopsis); }
					if (ep->show || ep->title)
						{
							name_set = EINA_TRUE;
							snprintf(buf, sizeof(buf), "%s s%02de%02d - %s", 
											 (ep->show ? ep->show : "?"),
											 ep->season, ep->episode,
											 (ep->title ? ep->title : "?"));
							
							edje_object_part_text_set(_video_preview, "preview.info", buf);
						}
					
					metadata_free(ep);
				}
			
			if (!name_set)
				{
					if (vi->name)
						{
							edje_object_part_text_set(_video_preview, "preview.info", vi->name);
						}
					else { edje_object_part_text_set(_video_preview, "preview.info", ""); }
				}
		}
	
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
