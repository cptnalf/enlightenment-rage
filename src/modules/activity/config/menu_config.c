/* filename: menu_config.c
 *  chiefengineer
 *  Sat May 15 10:34:49 PDT 2010
 */

#include "main.h"

extern int zoom_mode;
extern Ecore_Evas  *ecore_evas;

static void
option_zoom_mode_toggle(void* data)
{
	zoom_mode = ! zoom_mode;
}

void
main_menu_config(void *data)
{
	char option1[1024];
	
   menu_push("menu", "Settings", NULL, NULL);

	snprintf(option1, sizeof(option1), "Zoom mode = %s", (zoom_mode ? "on" : "off"));
	menu_item_add("icon/config", option1,
								option1, NULL,
								option_zoom_mode_toggle, NULL, NULL, NULL, NULL);
	menu_item_add("icon/fullscreen", "Fullscreen",
								 "Fullscreen On/Off", NULL,
								 config_option_fullscreen, ecore_evas, NULL, NULL, NULL);
   menu_item_add("icon/themes", "Themes",
								 "Select your theme", NULL,
								 config_option_themes, NULL, NULL, NULL, NULL);
   menu_item_add("icon/modes", "Modes",
								 "Change the engine Rage uses", NULL,
								 config_option_modes, ecore_evas, NULL, NULL, NULL);
   menu_item_add("icon/volumes", "Volumes",
								 "Edit your Volumes", NULL,
								 config_option_volumes, NULL, NULL, NULL, NULL);
   menu_item_enabled_set("Settings", "Fullscreen", 1);
   menu_item_enabled_set("Settings", "Themes", 1);
   menu_item_enabled_set("Settings", "Modes", 1);
   menu_item_enabled_set("Settings", "Volumes", 1);
   menu_go();
   menu_item_select("Fullscreen");
	
}
