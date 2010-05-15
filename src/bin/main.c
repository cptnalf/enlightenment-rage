#include "main.h"
#include <Ecore_Getopt.h>
#include "database.h"
#include "input.h"
#include "video_lib.h"

extern void main_menu_audio(void *data);
extern void main_menu_dvd(void *data);
extern void main_menu_photo(void *data);
extern void main_menu_tv(void *data);
static void menu_video(void *data);

typedef struct _Mode Mode;

struct _Mode
{
	int mode;
};

Evas        *evas = NULL;
char        *theme = NULL;
char        *config = NULL;
int          zoom_mode=1; /* by default stretch the video to fill my screen */
Eet_File    *eet_config = NULL;
Ecore_Timer* mouse_timeout = NULL;
Ecore_Evas  *ecore_evas = NULL;

static double       start_time = 0.0;
static Evas_Object *o_bg       = NULL;
static Eina_List   *modes      = NULL;
static int          cmode      = NONE;

static void main_usage(void);
static int main_volume_add(void *data, int type, void *ev);
static int main_volume_del(void *data, int type, void *ev);
static Eina_Bool _main_event_cb(void* data, rage_input in);
static void main_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
static int  main_signal_exit(void *data, int ev_type, void *ev);
static void main_delete_request(Ecore_Evas *ee);
static void main_resize(Ecore_Evas *ee);
extern void main_menu_config(void *data);
static void main_menu_scan(void *data);
static void main_get_config(void);

static const Ecore_Getopt options = {
  "rage",
  "%prog [options]",
  "0.2.0",
  "(C) 2009 Enlightenment",
  "BSD with advertisement clause",
  "Simple yet fancy media center.",
  1,
  {
    ECORE_GETOPT_STORE_STR('e', "engine", "ecore-evas engine to use"),
    ECORE_GETOPT_CALLBACK_NOARGS
    ('E', "list-engines", "list ecore-evas engines",
     ecore_getopt_callback_ecore_evas_list_engines, NULL),
    ECORE_GETOPT_STORE_DEF_BOOL('F', "fullscreen", "fullscreen mode", 0),
    ECORE_GETOPT_CALLBACK_ARGS
    ('g', "geometry", "geometry to use in x:y:w:h form.", "X:Y:W:H",
     ecore_getopt_callback_geometry_parse, NULL),
    ECORE_GETOPT_STORE_STR
    ('t', "theme", "path to read the theme file from"),
    ECORE_GETOPT_VERSION('V', "version"),
    ECORE_GETOPT_COPYRIGHT('R', "copyright"),
    ECORE_GETOPT_LICENSE('L', "license"),
    ECORE_GETOPT_HELP('h', "help"),
    ECORE_GETOPT_SENTINEL
  }
};

int
main(int argc, char **argv)
{
	Input_Listener* main_listener = NULL;
	Input_Listener* video_listener = NULL;
	Input_Listener* dvb_listener = NULL;
	Input_Listener* menu_listener = NULL;
   Evas_Object *o;
   int args, size;
   char *engine = NULL;
   unsigned char quit_option = 0, fullscreen = 0;
   Eina_Rectangle geometry = {0, 0, 0, 0};
   Ecore_Getopt_Value values[] = {
     ECORE_GETOPT_VALUE_STR(engine),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(fullscreen),
     ECORE_GETOPT_VALUE_PTR_CAST(geometry),
     ECORE_GETOPT_VALUE_STR(theme),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_NONE
   };
	 
	/* init ecore, eet, evas, edje etc. */
	start_time = ecore_time_get();
	eina_stringshare_init();
	eet_init();
	ecore_init();
	ecore_file_init();
	evas_init();
	edje_init();
	ecore_app_args_set(argc, (const char **)argv);
	/* ctrl-c / term signal - set event handler */
	ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, main_signal_exit, NULL);
	/* try init ecore_evas - if we can't - abort */
	if (!ecore_evas_init())
		{
			printf("ERROR: cannot init ecore_evas\n");
			return -1;
		}

   main_get_config();
   //mode = *(int*)eet_read(eet_config, "/config/mode", &size);
   fullscreen = *(int*)eet_read(eet_config, "/config/fullscreen", &size);

   args = ecore_getopt_parse(&options, values, argc, argv);
   if (args < 0)
     {
        fputs("ERROR: could not parse command line options.\n", stderr);
	return -1;
     }

   if (quit_option)
     return 0;

   /* set up default theme if no custom theme is selected */
   if (!theme)
     theme = eet_read(eet_config, "/config/theme", &size);

   if (geometry.w <= 0)
     geometry.w = 1280;
   if (geometry.h <= 0)
     geometry.h = 720;

   ecore_evas = ecore_evas_new
     (engine, geometry.x, geometry.y, geometry.w, geometry.h, NULL);

   if (!ecore_evas)
     {
	fprintf(stderr, "ERROR: Cannot create canvas, engine: %s, "
		"geometry: %d,%d+%dx%d\n",
		engine ? engine : "<auto>",
		geometry.x, geometry.y, geometry.w, geometry.h);
	return -1;
     }
   ecore_evas_callback_delete_request_set(ecore_evas, main_delete_request);
   ecore_evas_callback_resize_set(ecore_evas, main_resize);
   ecore_evas_title_set(ecore_evas, "Rage");
   ecore_evas_name_class_set(ecore_evas, "main", "Rage");
   evas = ecore_evas_get(ecore_evas);
   evas_image_cache_set(evas, 8 * 1024 * 1024);
   evas_font_cache_set(evas, 1 * 1024 * 1024);
   evas_font_path_append(evas, PACKAGE_DATA_DIR"/fonts");
   /* edje animations should run at 30 fps - might make this config later */
   edje_frametime_set(1.0 / 30.0);

   /* black rectangle behind everything to catch events */
   o = evas_object_rectangle_add(evas);
   evas_object_color_set(o, 0, 0, 0, 255);
   evas_object_move(o, 0, 0);
   evas_object_resize(o, geometry.w, geometry.h);
   evas_object_show(o);
	 
	 rage_module_init();
	 rage_module_load_all();
	 main_listener = rage_input_listener_add("main", _main_event_cb, NULL);
	 
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, main_mouse_move, NULL);
   evas_object_focus_set(o, 1);
   o_bg = o;

	/* if fullscreen mode - go fullscreen and hide mouse */
	if (fullscreen)
		{
			ecore_evas_fullscreen_set(ecore_evas, 1);
			ecore_evas_cursor_set(ecore_evas, "", 999, 0, 0);
		}

	/* init ui elements and volume scanner */
	layout_init();
	background_init();
	volume_init();
	status_init();
	{
		char buf[4096];
		snprintf(buf, sizeof(buf), "%s/media.db", config);
		database_init(buf);
	}
	
	/* build a default menu */
	main_mode_push(MENU);
	menu_push("menu", "Main", NULL, NULL);
	menu_item_add("icon/tv", "TV",
								"Scan all media again and update", NULL,
								main_menu_tv, NULL, NULL, NULL, NULL);
	menu_item_add("icon/video_file", "Videos",
								"Films, Movies and other video footage", NULL,
								main_menu_video, NULL, NULL, NULL, NULL);
	menu_item_add("icon/dvd", "DVD",
								"Play a DVD", NULL,
								main_menu_dvd, NULL, NULL, NULL, NULL);
	menu_item_add("icon/audio", "Music",
								"Music Albums, Songs and Tracks.", NULL,
								main_menu_audio, NULL, NULL, NULL, NULL);
	menu_item_add("icon/photo", "Photos",
								"Photos and images", NULL,
								main_menu_photo, NULL, NULL, NULL, NULL);
	menu_item_add("icon/update", "Scan Media",
								"Scan all media again and update", NULL,
								main_menu_scan, NULL, NULL, NULL, NULL);
	menu_item_add("icon/config", "Settings",
								"Modify settings and preferences", NULL,
								main_menu_config, NULL, NULL, NULL, NULL);

	menu_item_enabled_set("Main", "Settings", 1);
	menu_item_enabled_set("Main", "DVD", 1);

	menu_go();

	menu_item_select("Settings");

	/* show our canvas */
	ecore_evas_show(ecore_evas);

	/* add event handlers for volume add/del event from the volume scanner
	 * system so we know when volumes come and go */
	ecore_event_handler_add(VOLUME_ADD, main_volume_add, NULL);
	ecore_event_handler_add(VOLUME_DEL, main_volume_del, NULL);

	/* ... run the program core loop ... */
	ecore_main_loop_begin();

	rage_input_listener_del(main_listener);
	main_listener = NULL;
	rage_module_shutdown();
	
	ecore_evas_shutdown();
	ecore_file_shutdown();
	ecore_shutdown();
	eet_shutdown();
	return 0;
}

void
main_mode_push(int mode)
{
	Mode *md;

   md = calloc(1, sizeof(Mode));
   md->mode = mode;
   modes = eina_list_prepend(modes, md);
   cmode = md->mode;
}

void
main_mode_pop(void)
{
	Mode *md;

	if (!modes) return;
	md = modes->data;
	modes = eina_list_remove_list(modes, modes);
	free(md);
	if (!modes)
		{
			cmode = NONE;
		}
	else
		{
			md = modes->data;
			cmode = md->mode;
		}
}

/***/

void
main_reset(void)
{
   eet_close(eet_config);
   execlp("rage", "rage", NULL);
}

static int
main_volume_add(void *data, int type, void *ev)
{
	return 1;
}

static int
main_volume_del(void *data, int type, void *ev)
{
	return 1;
}

static Eina_Bool
_main_event_cb(void* data, rage_input in)
{
	Eina_Bool result;
	
	switch(in)
		{
		case (RAGE_INPUT_QUIT):
			{
				eet_close(eet_config);
				ecore_main_loop_quit();
				result = RAGE_EVENT_BLOCK;
				break;
			}
		case (RAGE_INPUT_FULLSCREEN):
			{
				if (!ecore_evas_fullscreen_get(ecore_evas))
					{
						ecore_evas_cursor_set(ecore_evas, "", 999, 0, 0);
						ecore_evas_fullscreen_set(ecore_evas, 1);
					}
				else
					{
						ecore_evas_cursor_set(ecore_evas, NULL, 0, 0, 0);
						ecore_evas_fullscreen_set(ecore_evas, 0);
					}
				result = RAGE_EVENT_BLOCK;
				break;
			}
		default:
			result = RAGE_EVENT_CONTINUE;

			/* else if(!strcmp(ev->keyname, "z")) */
			/* 	{ */
			/* 		option_zoom_mode_toggle(0); */
			/* 	} */
		}
	
	return result;
}

static int
main_signal_exit(void *data, int ev_type, void *ev)
{
	ecore_main_loop_quit();
	return 1;
}

static void
main_delete_request(Ecore_Evas *ee)
{
	ecore_main_loop_quit();
}

static void
main_resize(Ecore_Evas *ee)
{
	Evas_Coord w, h;

	evas_output_viewport_get(evas, NULL, NULL, &w, &h);
	evas_object_resize(o_bg, w, h);
	layout_resize();
}

static void
main_menu_scan(void *data)
{
	volume_update();
}

int
main_mouse_timeout(void* data)
{
   ecore_evas_cursor_set(ecore_evas, "", 999, 0, 0);
   mouse_timeout = NULL;
   return ECORE_CALLBACK_CANCEL;
}

static void
main_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   if (mouse_timeout)
      ecore_timer_delay(mouse_timeout, 1-ecore_timer_pending_get(mouse_timeout));
   else
   {
      mouse_timeout = ecore_timer_add(1, main_mouse_timeout, NULL);
      ecore_evas_cursor_set(ecore_evas, NULL, 0, 0, 0);
   }
}

static void
main_get_config(void)
{
   /* load config */
   char buf[4096];
   int i = 0;

   if (getenv("HOME"))
      snprintf(buf, sizeof(buf), "%s/.rage", getenv("HOME"));
   else if (getenv("TMPDIR"))
      snprintf(buf, sizeof(buf), "%s/.rage", getenv("TMPDIR"));
   else
      snprintf(buf, sizeof(buf), "%s/.rage", "/tmp");
   config = strdup(buf);

   snprintf(buf, sizeof(buf), "%s/config.eet", config);

   if (!ecore_file_exists(buf))
   {
      if (!ecore_file_is_dir(config))
	 ecore_file_mkpath(config);

      eet_config = eet_open(buf, EET_FILE_MODE_WRITE);

      /* write default config */
      eet_write(eet_config, "/config/fullscreen", &i, sizeof(int), 0);
      eet_write(eet_config, "/config/theme", PACKAGE_DATA_DIR"/default.edj",
	    	sizeof(PACKAGE_DATA_DIR"/default.edj"), 0);
      eet_write(eet_config, "/config/mode", &i, sizeof(int), 0);
      eet_close(eet_config);
   }

   eet_config = eet_open(buf, EET_FILE_MODE_READ_WRITE);
}

const char* rage_theme_get() { return theme; }
