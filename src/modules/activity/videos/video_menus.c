/* filename: video_menus.c
 *  chiefengineer
 *  Sat May 15 12:22:13 PDT 2010
 */

#include "main.h"
#include "menu.h"
#include "vfs.h"
#include "video.h"

/* for history recordage */
static Ecore_Timer* file_played_timer = NULL;
static Vfs_Source* vfs_source = NULL;

static void video_menu_library(void *data);

/* event handlers */

static int
_video_history_track(void* data)
{
	Vfs_Item* vi;
	
	/* kill the timer. */
	if (file_played_timer) { ecore_timer_del(file_played_timer); }
	
	vi =  data;
	vfs_source->record_play(vi);
}

static void
_video_menu_view(void *data)
{
	Vfs_Item *vli;
	
	if (file_played_timer)
		{
			/* reset the timer... */
			ecore_timer_del(file_played_timer);
			file_played_timer = NULL;
		}
	
	vli = data;
	
	main_mode_push(VIDEO);
	video_preview_destroy();
	
	file_played_timer = ecore_timer_add(20.0, _video_history_track, vli);
									
	video_init("xine", vli->vi->path, "video");
}

static void
video_menu_over(void *data)
{
	Vfs_Item *vli;

	vli = data;
	
	video_preview_set(vli->vi);
	video_preview_activate();
}

static void
video_menu_out(void *data)
{
	video_preview_deactivate();
}

/* ****************************** */

static const char*
_menu_add_list(Eina_List* vfs_items, const char* menu_label)
{
	Vfs_Item* item;
	Eina_List* ptr;
	const char* sel = NULL;
	
	EINA_LIST_FOREACH(vfs_items, ptr, item)
		{
			char buf[4096];
			
			if (item->is_menu)
				{
					snprintf(buf, sizeof(buf), "%i", item->count);
					
					menu_item_add(vfs_source->menu_icon, item->label,
												"", buf,
												video_menu_library, item,
												vfs_item_free,
												NULL, NULL);
				}
			else
				{
					buf[0] = 0;
					//			    snprintf(buf, sizeof(buf), "3:00:00");
					
					/* make the menu item,
					 * this is a normal video file, so it gets the standard handlers.
					 */
					menu_item_add(item->path, item->label,
												"", buf,
												_video_menu_view, item,
												vfs_item_free,
												video_menu_over,
												video_menu_out);
				}
			
			menu_item_enabled_set(menu_label, item->label, 1);
			if (!sel) sel = item->label;
		}
	
	return sel;
}

static void _menu_library_create(Vfs_Item* vi)
{
	Eina_List *videos;
	const char* sel;
		
	videos = vfs_source->get_items(vi);
	sel = _menu_add_list(videos, vi->label);
	
	menu_go();
	if (sel) { menu_item_select(sel); }
}

static void 
video_menu_favorites(void* data)
{
	Vfs_Item* vi = data;
	Vfs_Item* vl;
	const char* sel;
	
	vl = (Vfs_Item *)menu_data_get();
	
	if (!vl)
		{
			vl = vfs_item_new("Favorites", "", 0);
			menu_push("menu", vl->label, vfs_item_free, vl);
		}
	
	{
		Eina_List* favorites = vfs_source->get_favorites(50);
		sel = _menu_add_list(favorites, vl->label);
	}
	
	menu_go();
	if (sel) { menu_item_select(sel); }
}

/** display the recent video files.
 */
static void 
video_menu_recents(void* data)
{
	Vfs_Item* vl;
	Vfs_Item* vi;
	const char* sel = NULL;
	
	vi = data;
	vl = (Vfs_Item*)menu_data_get();

	if (!vl)
		{
			vl = vfs_item_new("Recents", "", 0);
			menu_push("menu", vl->label, vfs_item_free, vl);
		}
	
	{
		Eina_List* recents = vfs_source->get_recents(50);
		sel = _menu_add_list(recents, vl->label);
	}
	
	menu_go();
	if (sel) { menu_item_select(sel); }
}

static void
video_menu_library(void *data)
{
	Vfs_Item* vi;
	Vfs_Item* parent;
	
	vi = data;
	
	/* grab my parent, creating if necessary.
	 * because we need to tell the menu about the history. 
	 */
	parent = (Vfs_Item*)menu_data_get();
	
	if (!parent) { parent = vfs_item_new("Library", "", 0); }
	else 
		{
			/* we need to copy the item here, 
			 * because it's stuffed on a stack for restoring menu-item selections
			 * when we traverse the menu, or after a video is done playing, ...
			 */
			parent = vfs_item_copy(vi);
		}
	
	/* tell the menu our history. */
	menu_push("menu", parent->label, vfs_item_free, parent);
	
	/* this creates the new menu, then changes it! */
	_menu_library_create(parent);
}

static void 
video_menu_anime_library(void* data)
{
	Vfs_Item* vli;
	Vfs_Item* vl;
	
	vli = data;
	vl = (Vfs_Item*)menu_data_get();
	
	if (!vl)
		{
			vl = vfs_item_new("anime", "anime", 0);
			menu_push("menu", vl->label, vfs_item_free, vl);
		}
	
	_menu_library_create(vl);
}

static void 
video_menu_movies_library(void* data)
{
	Vfs_Item* vl;
	Vfs_Item* vli;
	
	vli = data;
	vl = (Vfs_Item*)menu_data_get();
	
	if (!vl)
		{
			vl = vfs_item_new("movies", "movies", 0);
			menu_push("menu", vl->label, vfs_item_free, vl);
		}
	
	_menu_library_create(vl);
}

void
main_menu_video(void *data)
{
	if (!vfs_source)
		{
			vfs_source = rage_vfs_source_get(VFS_TYPE_VIDEO);
		}
	
	menu_push("menu", "Video", NULL, NULL);
/* 	menu_item_add("icon/resume", "Resume", */
/* 								"Resume last Video played", NULL, */
/* 								NULL, NULL, NULL, NULL, NULL); */
	menu_item_add("icon/favorites", "Favorites",
								"Favorite Videos", NULL,
								video_menu_favorites, NULL, NULL, NULL, NULL);
	menu_item_enabled_set("Video", "Favorites", 1);

	menu_item_add("icon/history_folder", "Recents",
								"files recently played", NULL,
								video_menu_recents, NULL, NULL, NULL, NULL);
	menu_item_enabled_set("Video", "Recents", 1);
	
	menu_item_add("icon/library", "anime",
								"japanese animated shows/movies", NULL,
								video_menu_anime_library, NULL, NULL, NULL, NULL);
	menu_item_enabled_set("Video", "anime", 1);
	
	menu_item_add("icon/library", "movies",  
								"movies", NULL, 
								video_menu_movies_library, NULL, NULL, NULL, NULL);
	menu_item_enabled_set("Video", "movies", 1);
	
	menu_item_add("icon/library", "Library",
								"Browse all of your Videos", NULL,
								video_menu_library, NULL, NULL, NULL, NULL);
	menu_item_enabled_set("Video", "Library", 1);
	
/* 	menu_item_enabled_set("Video", "Resume", 0); */

	menu_go();
	menu_item_select("Library");
}
