#include "main.h"

typedef struct _Mini Mini;

struct _Mini
{
	char *file, *source;
	Evas_Object *o;
	Evas_Object *o_image;
	Evas_Object *o_parent;
	Ecore_Timer *timer;
	int done;
	int frame;
	int ok;
	time_t source_mod, file_mod;
};

static void _mini_free(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _mini_resize(void *data, Evas *e, Evas_Object *obj, void *event_info);
static Eina_Bool _mini_timer(void *data);

static Ecore_Timer *timer = NULL;
static Eina_List *minis = NULL;

Evas_Object *
mini_add(Evas_Object *parent, const char *source)
{
	Evas_Object *o;
	Mini *mini;
	const char *chmap = "0123456789abcdef";
	unsigned char sha[40];
	char buf[4096];
	char file[4096];
	int i;
   
	o = edje_object_add(evas);
	edje_object_file_set(o, theme, "mini");
   
	mini = calloc(1, sizeof(Mini));
	mini->o = o;
	mini->o_image = evas_object_image_add(evas);
   
   edje_extern_object_aspect_set(mini->o_image, EDJE_ASPECT_CONTROL_BOTH, 640, 480);
   edje_object_part_swallow(mini->o, "video", mini->o_image);
   evas_object_show(mini->o_image);
   mini->o_parent = parent;
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
   mini->file = strdup(file);
   mini->source = strdup(source);
   evas_object_event_callback_add(o, EVAS_CALLBACK_FREE, _mini_free, mini);
   evas_object_event_callback_add(mini->o_image, EVAS_CALLBACK_RESIZE, _mini_resize, mini);
   if (!timer) timer = ecore_timer_add(0.1, _mini_timer, NULL);
   minis = eina_list_append(minis, mini);
   mini->source_mod = ecore_file_mod_time(source);
   mini->file_mod = ecore_file_mod_time(file);
   return o;
}

void
mini_pause_set(int pause)
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
			if (minis)
				{
					if (!timer) timer = ecore_timer_add(0.1, _mini_timer, NULL);
				}
		}
}

static void
_mini_free(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Mini *mini;
   
	mini = data;
	evas_object_del(mini->o_image);
	free(mini->file);
	free(mini->source);
	if (mini->timer)
		{
			ecore_timer_del(mini->timer);
			mini->timer = NULL;
		}
	free(mini);
	minis = eina_list_remove(minis, mini);
	if (!minis)
		{
			if (timer)
				{
					ecore_timer_del(timer);
					timer = NULL;
				}
		}
}

static void
_mini_resize(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Coord w, h;
   
	evas_object_geometry_get(obj, NULL, NULL, &w, &h);
	evas_object_image_fill_set(obj, 0, 0, w, h);
}

static Eina_Bool
_mini_timer(void *data)
{
   Mini *mini;
   char buf[4096];
   int iw = 0, ih = 0;
   Eina_List *l;

	for (l = minis; l; l = l->next)
		{
			mini = l->data;
			if (mini->file_mod < mini->source_mod)
				{
					mini->file_mod = ecore_file_mod_time(mini->file);
					if (ecore_file_size(mini->file))
						mini->file_mod = 0;
				}
			if (mini->file_mod < mini->source_mod)
				iw = 0;
			else
				{
					Evas_Coord x, y, w, h, vw, vh;
	     
					evas_object_geometry_get(mini->o_image, &x, &y, &w, &h);
					evas_output_viewport_get(evas, NULL, NULL, &vw, &vh);
					if (((x < vw) && ((x + w) > 0)) &&
							((y < vh) && ((y + h) > 0)))
						{
							snprintf(buf, sizeof(buf), "v/%i", mini->frame);
							evas_object_image_file_set(mini->o_image, NULL, NULL);
							evas_object_image_file_set(mini->o_image, mini->file, buf);
							evas_object_image_size_get(mini->o_image, &iw, &ih);
						}
					else
						iw = 1;
				}
			if (iw == 0)
				{
					if ((mini->frame == 0) &&	(mini->done == 0))
						{
							//printf("should EXEC: %s\n", buf);
						}
					else
						{
							mini->frame = 0;
							snprintf(buf, sizeof(buf), "v/%i", mini->frame);
							evas_object_image_file_set(mini->o_image, mini->file, buf);
						}
				}
			else
				{
					if (iw != 1)
						{
							if (!mini->ok)
								{
									edje_extern_object_aspect_set(mini->o_image, EDJE_ASPECT_CONTROL_BOTH, iw, ih);
									edje_object_part_swallow(mini->o, "video", mini->o_image);
									edje_object_signal_emit(mini->o, "mini", "begin");
									mini->ok = 1;
								}
						}
					mini->frame++;
				}
		}
	return EINA_TRUE;
}

