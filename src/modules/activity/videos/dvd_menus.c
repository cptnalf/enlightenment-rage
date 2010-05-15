/* filename: dvd.c
 *  chiefengineer
 *  Sat May 15 12:53:38 PDT 2010
 */

#include "main.h"
#include "video_preview.h"
#include "video.h"

void
main_menu_dvd_watch(void *data)
{
	main_mode_push(VIDEO);
	video_preview_destroy();
	
	video_init("xine", "dvd://", "video");
}

void
main_menu_dvd(void *data)
{
	menu_push("menu", "DVD", NULL, NULL);
	menu_item_add("icon/resume", "Resume",
								"Resume DVD from last play", NULL,
								NULL, NULL, NULL, NULL, NULL);
	menu_item_add("icon/dvd", "Rip",
								"Rip DVD to Disk for storage", NULL,
								NULL, NULL, NULL, NULL, NULL);
	menu_item_add("icon/dvd", "Watch",
								"Watch your DVD", NULL,
								main_menu_dvd_watch, NULL, NULL, NULL, NULL);
	menu_item_enabled_set("DVD", "Resume", 0);
	menu_item_enabled_set("DVD", "Rip", 0);
	menu_item_enabled_set("DVD", "Watch", 1);
	menu_go();
	menu_item_select("Watch");
}
