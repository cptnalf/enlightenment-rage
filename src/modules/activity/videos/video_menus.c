/* filename: video_menus.c
 *  chiefengineer
 *  Sat May 15 12:22:13 PDT 2010
 */

#include "main.h"
#include "database.h"
#include "video_lib.h"
#include "video.h"

/* for history recordage */
static Ecore_Timer* file_played_timer = NULL;

static int
main_menu_video_history_track(void* data)
{
	Database* db;
	Volume_Item* vi;
	
	/* kill the timer. */
	if (file_played_timer) { ecore_timer_del(file_played_timer); }
	
	vi =  data;
	db = database_new();
	database_video_file_update(db, vi);
	database_free(db);
}

void
main_menu_video_view(void *data)
{
	Video_Lib_Item *vli;
	
	if (file_played_timer)
		{
			/* reset the timer... */
			ecore_timer_del(file_played_timer);
			file_played_timer = NULL;
		}
	
	vli = data;
	
	main_mode_push(VIDEO);
	video_preview_destroy();
	
	file_played_timer = ecore_timer_add(20.0, main_menu_video_history_track, vli->vi);
									
	video_init("xine", vli->vi->path, "video");
}
