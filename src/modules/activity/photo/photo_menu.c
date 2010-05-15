/* filename: photo_menu.c
 *  chiefengineer
 *  Sat May 15 13:33:14 PDT 2010
 */

#include "main.h"

void
main_menu_photo(void *data)
{
	menu_push("menu", "Photo", NULL, NULL);
	menu_item_add("icon/bug", "Disabled",
								"This feature is incomplete", NULL,
								NULL, NULL, NULL, NULL, NULL);
	menu_go();
	menu_item_select("Disabled");
}
