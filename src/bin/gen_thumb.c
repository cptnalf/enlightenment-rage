#include "main.h"
#include "gen_thumb.h"

#define GEN_THUMB_FPS 10
#define GEN_THUMB_RES "480x480"

typedef struct _Thumb Thumb;

struct _Thumb
{
	char *file, *source;
	Evas_Object *o;
	Evas_Object *o_image;
	Evas_Object *o_parent;
	Ecore_Exe *gen_exe;
	Ecore_Timer *timer;
	Ecore_Event_Handler *handler;
	int done;
	int frame;
	int ok;
	time_t source_mod, file_mod;
};

static void _gen_thumb_free(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _gen_thumb_resize(void *data, Evas *e, Evas_Object *obj, void *event_info);
static Eina_Bool _gen_thumb_timer(void *data);
static Eina_Bool _gen_thumb_exe_exit(void *data, int ev_type, void *ev);

static Ecore_Timer *timer = NULL;
static Eina_List *thumbs = NULL;
static int generators = 0;

Evas_Object *
gen_thumb_add(Evas_Object *parent, const char *source)
{
	Evas_Object *o;
	Thumb *thumb;
	const char *chmap = "0123456789abcdef";
	unsigned char sha[40];
	char buf[4096];
	char file[4096];
	int i;
   
	o = edje_object_add(evas);
	edje_object_file_set(o, theme, "mini");
   
	thumb = calloc(1, sizeof(Thumb));
	thumb->o = o;
	thumb->o_image = evas_object_image_add(evas);
   
   edje_extern_object_aspect_set(thumb->o_image, EDJE_ASPECT_CONTROL_BOTH, 640, 480);
   edje_object_part_swallow(thumb->o, "video", thumb->o_image);
   evas_object_show(thumb->o_image);
   thumb->o_parent = parent;
   snprintf(buf, sizeof(buf), "%s/.rage/thumbs", getenv("HOME"));
   ecore_file_mkpath(buf);
   sha1_sum((const unsigned char *)source, strlen(source), sha);
   for (i = 0; i < 20; i++)
     {
	buf[(i * 2) + 0] = chmap[(sha[i] >> 4) & 0xf];
	buf[(i * 2) + 1] = chmap[(sha[i]     ) & 0xf];
     }
   buf[(i * 2)] = 0;
   snprintf(file, sizeof(file), "%s/.rage/thumbs/%s.eet", getenv("HOME"), buf);
   thumb->file = strdup(file);
   thumb->source = strdup(source);
   evas_object_event_callback_add(o, EVAS_CALLBACK_FREE, _gen_thumb_free, thumb);
   evas_object_event_callback_add(thumb->o_image, EVAS_CALLBACK_RESIZE, _gen_thumb_resize, thumb);
   if (!timer) timer = ecore_timer_add(0.1, _gen_thumb_timer, NULL);
   thumbs = eina_list_append(thumbs, thumb);
   thumb->source_mod = ecore_file_mod_time(source);
   thumb->file_mod = ecore_file_mod_time(file);
   return o;
}

void
gen_thumb_pause_set(int pause)
{
	if (pause)
		{
			if (timer)
				{
					ecore_timer_del(timer);
					timer = NULL;
				}
		}
	else
		{
			if (thumbs)
				{
					if (!timer) timer = ecore_timer_add(0.1, _gen_thumb_timer, NULL);
				}
		}
}

static void
_gen_thumb_free(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Thumb *thumb;
   
	thumb = data;
	evas_object_del(thumb->o_image);
	free(thumb->file);
	free(thumb->source);
	if (thumb->timer)
		{
			ecore_timer_del(thumb->timer);
			thumb->timer = NULL;
		}
	if (thumb->gen_exe)
		{
			ecore_exe_kill(thumb->gen_exe);
			ecore_exe_free(thumb->gen_exe);
			if (thumb->handler) ecore_event_handler_del(thumb->handler);
			generators--;
			thumb->gen_exe = NULL;
			thumb->handler = NULL;
		}
	free(thumb);
	thumbs = eina_list_remove(thumbs, thumb);
	if (!thumbs)
		{
			if (timer)
				{
					ecore_timer_del(timer);
					timer = NULL;
				}
		}
}

static int
_gen_thumb_overtime_timer(void* data)
{
	Thumb *thumb;
	
	thumb = data;
	printf("OVER TIME %s\n", thumb->file);
	if (thumb->gen_exe)
		{
			printf("2 OVER TIME %s\n", thumb->file);
			ecore_exe_kill(thumb->gen_exe);
			ecore_exe_free(thumb->gen_exe);
			if (thumb->handler) ecore_event_handler_del(thumb->handler);
			thumb->done = 0;
			generators --;
			thumb->gen_exe = NULL;
			thumb->handler = NULL;
		}
	thumb->timer = NULL;
	return 0;
}

static void
_gen_thumb_resize(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Coord w, h;
   
	evas_object_geometry_get(obj, NULL, NULL, &w, &h);
	evas_object_image_fill_set(obj, 0, 0, w, h);
}

static Eina_Bool
_gen_thumb_timer(void *data)
{
   Thumb *thumb;
   char buf[4096];
   int iw = 0, ih = 0;
   Eina_List *l;
	 int dones = 0;
	 int cnt = 0;

	for (l = thumbs; l; l = l->next)
		{
			cnt++;
			
			thumb = l->data;
			if (thumb->file_mod < thumb->source_mod)
				{
					thumb->file_mod = ecore_file_mod_time(thumb->file);
					if (ecore_file_size(thumb->file))
						thumb->file_mod = 0;
				}
			
			/* if the thumb was created after the source,
			 * verify it's size.
			 * if it's less than or equal to zero, force a regen.
			 */
			if (thumb->file_mod >= thumb->source_mod)
				{ 
					if (0 >= ecore_file_size(thumb->file)) { thumb->file_mod = 0; } 
				}
			
			if (thumb->file_mod < thumb->source_mod)
				iw = 0;
			else
				{
					iw = 1;
				}
			
			if (iw == 0)
				{
					if ((thumb->frame == 0) 
							&& (!thumb->gen_exe)
							&& (generators == 0)
							&&	(thumb->done == 0))
						{
							edje_object_signal_emit(thumb->o, "generate", "begin");
							edje_object_signal_emit(thumb->o_parent, "generate", "begin");
							snprintf(buf, sizeof(buf), 
											 "nice -n 5 %s/rage_thumb \"%s\" \"%s\" -fps %d -og %s",
											 PACKAGE_BIN_DIR, thumb->source, thumb->file, 
											 GEN_THUMB_FPS, GEN_THUMB_RES);
							printf("EXEC: %s\n", buf);
							thumb->gen_exe = ecore_exe_run(buf, thumb);
							thumb->handler = ecore_event_handler_add(ECORE_EXE_EVENT_DEL, _gen_thumb_exe_exit, thumb);
							thumb->timer = ecore_timer_add(240.0, _gen_thumb_overtime_timer, thumb);
							generators++;
						}
					else
						{
							/* this is awaiting generation... */
						}
				}
			else
				{
					++dones;
				}
		}
	
	if (dones == cnt)
		{
			/* so, there's nothing left for us to do... */
			gen_thumb_pause_set(1);
		}
	return EINA_TRUE;
}

static Eina_Bool
_gen_thumb_exe_exit(void *data, int ev_type, void *ev)
{
	Ecore_Exe_Event_Del *e;
	Thumb *thumb;
	
	e = ev;
	thumb = data;
	if (thumb->gen_exe == e->exe)
		{
			edje_object_signal_emit(thumb->o, "generate", "done");
			edje_object_signal_emit(thumb->o_parent, "generate", "done");
			if (thumb->handler) ecore_event_handler_del(thumb->handler);
			thumb->gen_exe = NULL;
			thumb->handler = NULL;
			thumb->done++;
			generators--;
			if (thumb->timer)
				{
					ecore_timer_del(thumb->timer);
					thumb->timer = NULL;
				}
		}
	return EINA_TRUE;
}
