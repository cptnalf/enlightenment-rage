#include "main.h"
#include "input.h"
#include "gen_thumb.h"
#include "video_preview.h"
#include "video_input.h"

#include "video.h"

Evas_Object *o_video = NULL;
Evas_Object *o_video_bg = NULL;

void video_spu_audio_info_print(int show);

static Ecore_Job *video_stopped_job = NULL;
static Ecore_Timer *_hide_timer = NULL;
static Input_Listener* video_listener = NULL;

static int video_menu_bg_hide_tmer_cb(void *data);
static void video_resize(void);
static void video_obj_frame_decode_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_frame_resize_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_length_change_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_stopped_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_channels_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_title_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_progress_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_ref_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_button_num_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_button_cb(void *data, Evas_Object *obj, void *event_info);
static void video_stopped_job_cb(void *data);

/***/
static int
video_menu_bg_hide_tmer_cb(void *data)
{
	background_hide();
	menu_hide();
	mini_pause_set(1);
	gen_thumb_pause_set(1);
	_hide_timer = NULL;
	
	{
		Evas_Coord x,y,h,w;
		evas_object_geometry_get(o_video, &x, &y, &w, &h);
		fprintf(stderr, "(%d,%d) %dx%d\n", x, y, w, h);
	}
	
	return 0;
}

static void
video_resize(void)
{
	Evas_Coord w, h;
	Evas_Coord rw, rh;
	int iw, ih;
	double ratio;

	emotion_object_size_get(o_video, &iw, &ih);
	if ((iw == 0) || (ih == 0)) return;
	ratio = emotion_object_ratio_get(o_video);
	if (ratio > 0.0) iw = (ih * ratio) + 0.5;
	else ratio = (double)iw / (double)ih;
	w = 10240 * ratio;
	h = 10240;
	
	evas_output_viewport_get(evas, 0, 0, &rw, &rh);
	
	/* this provides for the lost area on my mitsubishi dlp. */
	static int x_offset = 50; //65;
	static int y_offset = 15; //20;
	
	rw -= (x_offset * 2);
	rh -= ((y_offset * 2) + 24);
	edje_extern_object_max_size_set(o_video, rw, rh);
	
	if (zoom_mode)
		{
			Edje_Aspect_Control asp_ctrl = EDJE_ASPECT_CONTROL_VERTICAL;
			
			if (w > h) { asp_ctrl = EDJE_ASPECT_CONTROL_HORIZONTAL; }
			
			// fit so there is no blank space
			edje_extern_object_aspect_set(o_video, asp_ctrl, w, h);
		}
	else
		{
			// fit and pad with blank
			edje_extern_object_aspect_set(o_video, EDJE_ASPECT_CONTROL_BOTH, w, h);
		}
	
	edje_object_part_swallow(o_video_bg, "video", o_video);
}

static void
video_obj_frame_decode_cb(void *data, Evas_Object *obj, void *event_info)
{
	double pos, len;
	char buf[256];
	int ph, pm, ps, pf, lh, lm, ls;

	pos = emotion_object_position_get(obj);
	len = emotion_object_play_length_get(obj);
	lh = len / 3600;
	lm = len / 60 - (lh * 60);
	ls = len - ((lh * 3600) + (lm * 60));
	ph = pos / 3600;
	pm = pos / 60 - (ph * 60);
	ps = pos - ((ph * 3600) + (pm * 60));
	pf = pos * 100 - (ps * 100) - (pm * 60 * 100) - (ph * 60 * 60 * 100);
	snprintf(buf, sizeof(buf), "%i:%02i:%02i.%02i / %i:%02i:%02i",
					 ph, pm, ps, pf, lh, lm, ls);
	//	printf("%s\n", buf);
	edje_object_part_text_set(o_video_bg, "position", buf);
}

static void
video_obj_frame_resize_cb(void *data, Evas_Object *obj, void *event_info)
{
	video_resize();
}

static void
video_obj_length_change_cb(void *data, Evas_Object *obj, void *event_info)
{
	double pos, len;
	char buf[256];
	int ph, pm, ps, pf, lh, lm, ls;

	pos = emotion_object_position_get(obj);
	len = emotion_object_play_length_get(obj);
	lh = len / 3600;
	lm = len / 60 - (lh * 60);
	ls = len - (lm * 60);
	ph = pos / 3600;
	pm = pos / 60 - (ph * 60);
	ps = pos - (pm * 60);
	pf = pos * 100 - (ps * 100) - (pm * 60 * 100) - (ph * 60 * 60 * 100);
	snprintf(buf, sizeof(buf), "%i:%02i:%02i.%02i / %i:%02i:%02i",
					 ph, pm, ps, pf, lh, lm, ls);
	edje_object_part_text_set(o_video_bg, "position", buf);
	edje_object_signal_emit(o_video_bg, "active", "");
}

static void
video_obj_stopped_cb(void *data, Evas_Object *obj, void *event_info)
{
	printf("video stopped!\n"); 
	if (!video_stopped_job)
		video_stopped_job = ecore_job_add(video_stopped_job_cb, data);
}

static void
video_obj_channels_cb(void *data, Evas_Object *obj, void *event_info)
{
	video_spu_audio_info_print(0);
	
	printf("channels changed: [AUD %i][VID %i][SPU %i]\n",
				 emotion_object_audio_channel_count(obj),
				 emotion_object_video_channel_count(obj),
				 emotion_object_spu_channel_count(obj));
}

static void
video_obj_title_cb(void *data, Evas_Object *obj, void *event_info)
{
	printf("video title to: \"%s\"\n", emotion_object_title_get(obj));
}

static void
video_obj_progress_cb(void *data, Evas_Object *obj, void *event_info)
{
	printf("progress: \"%s\" %3.3f\n", 
				 emotion_object_progress_info_get(obj),
				 emotion_object_progress_status_get(obj));
}

static void
video_obj_ref_cb(void *data, Evas_Object *obj, void *event_info)
{
	printf("video ref to: \"%s\" %i\n",
				 emotion_object_ref_file_get(obj),
				 emotion_object_ref_num_get(obj));
}

static void
video_obj_button_num_cb(void *data, Evas_Object *obj, void *event_info)
{
	printf("video spu buttons to: %i\n",
				 emotion_object_spu_button_count_get(obj));
}

static void
video_obj_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	printf("video selected spu button: %i\n",
				 emotion_object_spu_button_get(obj));
}

static void
video_stopped_job_cb(void *data)
{
	main_mode_pop();
	video_shutdown();
}

void video_spu_audio_info_print(int show)
{
	char buf[4096];
	const char* aud_name =0;
	const char* spu_name =0;
	int aud_max, spu_max;
	int aud_chl, spu_chl;
	Eina_Bool spu_mute;
	
	aud_chl = emotion_object_audio_channel_get(o_video);
	spu_chl = emotion_object_spu_channel_get(o_video);
	spu_mute = emotion_object_spu_mute_get(o_video);
	
	aud_max = emotion_object_audio_channel_count(o_video);
	spu_max = emotion_object_spu_channel_count(o_video);
	
	if (aud_max > 0)
		{ aud_name = emotion_object_audio_channel_name_get(o_video, aud_chl); }
	
	/* we have subtitle channels, and the selected one is 0 or bigger. */
	if (spu_max > 0 && spu_chl >= 0 && !spu_mute)
		{ 
			spu_name = emotion_object_spu_channel_name_get(o_video, spu_chl);
			snprintf(buf, sizeof(buf), "a[%02d - %s] s[%02d - %s]",
							 aud_chl, (aud_name ? aud_name : "eng"),
							 spu_chl, (spu_name ? spu_name : "eng"));
		}
	else
		{
			/* subtitles are off */
			snprintf(buf, sizeof(buf), "a[%02d - %s] s[off]",
							 aud_chl, (aud_name ? aud_name : "eng"));
		}
	
	edje_object_part_text_set(o_video_bg, "track_name", buf);
	if (show) edje_object_signal_emit(o_video_bg, "active", "");
}

void
video_init(char *module, char *file, char *swallow)
{
	Evas_Object *o;

	if (o_video_bg) return;
	o = edje_object_add(evas);
	edje_object_file_set(o, theme, "video_container");
	o_video_bg = o;
	   
	o = emotion_object_add(evas); 
	if (!emotion_object_init(o, module))
		{
			printf("ERROR!\n");
		}
   
	//evas_object_smart_callback_add(o, "frame_decode", video_obj_frame_decode_cb, NULL);
	evas_object_smart_callback_add(o, "position_update", video_obj_frame_decode_cb, NULL);
	evas_object_smart_callback_add(o, "frame_resize", video_obj_frame_resize_cb, NULL);
	evas_object_smart_callback_add(o, "length_change", video_obj_length_change_cb, NULL);
   
	evas_object_smart_callback_add(o, "decode_stop", video_obj_stopped_cb, NULL);
	evas_object_smart_callback_add(o, "channels_change", video_obj_channels_cb, NULL);
	evas_object_smart_callback_add(o, "title_change", video_obj_title_cb, NULL);
	evas_object_smart_callback_add(o, "progress_change", video_obj_progress_cb, NULL);
	evas_object_smart_callback_add(o, "ref_change", video_obj_ref_cb, NULL);
	evas_object_smart_callback_add(o, "button_num_change", video_obj_button_num_cb, NULL);
	evas_object_smart_callback_add(o, "button_change", video_obj_button_cb, NULL);
	o_video = o;
   
	emotion_object_file_set(o_video, file);
	emotion_object_play_set(o_video, 1);
	emotion_object_audio_mute_set(o_video, 0);
	emotion_object_audio_volume_set(o_video, 1.0);
	layout_swallow(swallow, o_video_bg);
	
	edje_extern_object_aspect_set(o_video, EDJE_ASPECT_CONTROL_BOTH, 640, 480);
	edje_object_part_swallow(o_video_bg, "video", o_video);
	
	video_resize();
	
	evas_object_show(o_video);
	evas_object_show(o_video_bg);
	
	_hide_timer = ecore_timer_add(10.0, video_menu_bg_hide_tmer_cb, NULL);

	if (! video_listener)
		{
			video_listener = rage_input_listener_add("video", video_event_cb, NULL);
		}
}

void
video_shutdown(void)
{
	if (!o_video_bg) return;
	if (!_hide_timer)
		{
			menu_show();
			background_show();
			mini_pause_set(0);
			gen_thumb_pause_set(0);
		}
	else
		{
			ecore_timer_del(_hide_timer);
			_hide_timer = NULL;
		}
   
	evas_object_del(o_video_bg);
	evas_object_del(o_video);
	if (video_stopped_job) ecore_job_del(video_stopped_job);
	o_video = NULL;
	o_video_bg = NULL;
	video_stopped_job = NULL;
	
	if (video_listener)
		{
			rage_input_listener_del(video_listener);
			video_listener = NULL;
		}
}
