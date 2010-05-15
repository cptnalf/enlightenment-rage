/* filename: audio_menu.c
 *  chiefengineer
 *  Sat May 15 13:29:35 PDT 2010
 */

#include "main.h"

void
main_menu_audio(void *data)
{
	menu_push("menu", "Audio", NULL, NULL);
	menu_item_add("icon/bug", "Disabled",
								"This feature is incomplete", NULL,
								NULL, NULL, NULL, NULL, NULL);
	menu_go();
	menu_item_select("Disabled");
}
