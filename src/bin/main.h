#include "e.h"

#include "layout.h"
#include "background.h"
#include "volume.h"
#include "status.h"
#include "menu.h"
#include "mini.h"
#include "minivid.h"
#include "sha1.h"
#include "conf_options.h"

#define RAGE_FREE(p) do { free(p); p = NULL; } while (0)
#define RAGE_OBJECT_DEL(obj) \
	if (obj) {								 \
	evas_object_del(obj);			 \
	obj = NULL;								 \
	}
#define RAGE_STRINGSHARE_DEL(string) \
	if (string)												 \
		{																 \
	    eina_stringshare_del(string);  \
			string = NULL;								 \
		}
	
extern Evas *evas;
extern char *theme;
extern char *config;
extern int   zoom_mode;
extern Eet_File *eet_config;

#define NONE  0
#define MENU  1
#define VIDEO 2
#define DVB   3

void main_mode_push(int mode);
void main_mode_pop(void);
void main_reset(void);

const char* rage_theme_get();
