/* filename: video_input.c
 *  chiefengineer
 *  Sat May 15 12:22:57 PDT 2010
 */

#include "main.h"
#include "video.h"

static double jump = 0.0;
static Ecore_Timer *jump_reset_timer = NULL;

static Eina_Bool
video_jump_reset_timer_cb(void *data)
{
	if (jump > 0.0) jump = 10.0;
	else if (jump < 0.0) jump = -10.0;
	jump_reset_timer = NULL;
	return EINA_FALSE;
};

/* ESC   INS   PSE
 * 1     2     3
 * 4     5     6
 * 7     8     9
 * END   0     BSP
 *
 *     t   g
 * HME         m
 *       UP
 * LFT   RET   RGT
 *       DWN
 * r           spc
 *     s   p
 * 
 * BLF BRT PER l
 * PRI NEX COM k
 * TAB q   w   n
 */

Eina_Bool
video_event_cb(void* data, rage_input in)
{
	Eina_Bool result;
	
	switch(in)
		{
		case (RAGE_INPUT_REWIND):
			{
				/* FIXME: play info display */
				double p;
				
				p = emotion_object_position_get(o_video);
				if (jump >= 0.0) jump = -7.5;
				else jump *= 2.0;
				if (jump < -120.0) jump = -120.0;
				if (jump_reset_timer)
					ecore_timer_del(jump_reset_timer);
				jump_reset_timer = ecore_timer_add(2.0, 
																					 video_jump_reset_timer_cb,
																					 NULL);
				emotion_object_position_set(o_video, p + jump);
				edje_object_signal_emit(o_video_bg, "active", "");
				result = RAGE_EVENT_BLOCK;
				break;
			}
		case (RAGE_INPUT_FASTFORWARD):
			{
				/* FIXME: play info display */
				double p;
	
				p = emotion_object_position_get(o_video);
				if (jump <= 0.0) jump = 7.5;
				else jump *= 2.0;
				if (jump > 120.0) jump = 120.0;
				if (jump_reset_timer)
					ecore_timer_del(jump_reset_timer);
				jump_reset_timer = ecore_timer_add(2.0, 
																					 video_jump_reset_timer_cb,
																					 NULL);
				emotion_object_position_set(o_video, p + jump);
				edje_object_signal_emit(o_video_bg, "active", "");
				result = RAGE_EVENT_BLOCK;
				break;
			}
		case (RAGE_INPUT_PAUSE):
			{
				/* FIXME: play info display end */
				jump = 0.0;
				emotion_object_play_set(o_video, 1);
				edje_object_signal_emit(o_video_bg, "active", "");
				result = RAGE_EVENT_BLOCK;
				break;
			}
		case (RAGE_INPUT_KEY_SPACE):
			{
				/* FIXME: play info display */
				jump = 0.0;
				if (emotion_object_play_get(o_video))
					emotion_object_play_set(o_video, 0);
				else
					emotion_object_play_set(o_video, 1);
				edje_object_signal_emit(o_video_bg, "active", "");
				break;
			}
		case (RAGE_INPUT_STOP):
			{
				printf("stopping...\n");
				/* FIXME: save position for this video */
				main_mode_pop();
				jump = 0.0;
				video_shutdown();
				result = RAGE_EVENT_BLOCK;
				break;
			}

		case (RAGE_INPUT_UP):
			{
				emotion_object_event_simple_send(o_video, EMOTION_EVENT_UP);
				result = RAGE_EVENT_BLOCK;
				break;
			}
		case (RAGE_INPUT_DOWN):
			{
				emotion_object_event_simple_send(o_video, EMOTION_EVENT_DOWN);
				result = RAGE_EVENT_BLOCK;
				break;
			}
		case (RAGE_INPUT_LEFT):
			{
				emotion_object_event_simple_send(o_video, EMOTION_EVENT_LEFT);
				result = RAGE_EVENT_BLOCK;
				break;
			}
		case (RAGE_INPUT_RIGHT):
			{
				emotion_object_event_simple_send(o_video, EMOTION_EVENT_RIGHT);
				result = RAGE_EVENT_BLOCK;
				break;
			}
		case (RAGE_INPUT_OK):
			{
				emotion_object_event_simple_send(o_video, EMOTION_EVENT_SELECT);
				result = RAGE_EVENT_BLOCK;
				break;
			}
		case (RAGE_INPUT_KEY_N):
			{
				emotion_object_event_simple_send(o_video, EMOTION_EVENT_MENU1);
				result = RAGE_EVENT_BLOCK;
				break;
			}
		case (RAGE_INPUT_PREV):
		{
		emotion_object_event_simple_send(o_video, EMOTION_EVENT_PREV);
			result = RAGE_EVENT_BLOCK;
			break;
		}
		case (RAGE_INPUT_NEXT):
		{
		emotion_object_event_simple_send(o_video, EMOTION_EVENT_NEXT);
			result = RAGE_EVENT_BLOCK;
			break;
		}
		case (RAGE_INPUT_KEY_0):
		{
		emotion_object_event_simple_send(o_video, EMOTION_EVENT_0);
			result = RAGE_EVENT_BLOCK;
			break;
		}
		case (RAGE_INPUT_KEY_1):
		{
		emotion_object_event_simple_send(o_video, EMOTION_EVENT_1);
			result = RAGE_EVENT_BLOCK;
			break;
		}
		case (RAGE_INPUT_KEY_2):
			{
		emotion_object_event_simple_send(o_video, EMOTION_EVENT_2);
			result = RAGE_EVENT_BLOCK;
			break;
		}
		case (RAGE_INPUT_KEY_3):
			{
		emotion_object_event_simple_send(o_video, EMOTION_EVENT_3);
			result = RAGE_EVENT_BLOCK;
			break;
		}
		case (RAGE_INPUT_KEY_4):
			{
		emotion_object_event_simple_send(o_video, EMOTION_EVENT_4);
			result = RAGE_EVENT_BLOCK;
			break;
		}
		case (RAGE_INPUT_KEY_5):
			{
		emotion_object_event_simple_send(o_video, EMOTION_EVENT_5);
			result = RAGE_EVENT_BLOCK;
			break;
		}
		case (RAGE_INPUT_KEY_6):
			{
		emotion_object_event_simple_send(o_video, EMOTION_EVENT_6);
			result = RAGE_EVENT_BLOCK;
			break;
		}
		case (RAGE_INPUT_KEY_7):
			{
		emotion_object_event_simple_send(o_video, EMOTION_EVENT_7);
			result = RAGE_EVENT_BLOCK;
			break;
		}
		case (RAGE_INPUT_KEY_8):
			{
		emotion_object_event_simple_send(o_video, EMOTION_EVENT_8);
			result = RAGE_EVENT_BLOCK;
			break;
		}
		case (RAGE_INPUT_KEY_9):
			{
		emotion_object_event_simple_send(o_video, EMOTION_EVENT_9);
			result = RAGE_EVENT_BLOCK;
			break;
		}
		case (RAGE_INPUT_AUDIO_ROTATE):
		{
			/* r= radio, audio track. */
			int chl;
			int chl_max;
			
			chl = emotion_object_audio_channel_get(o_video);
			chl_max = emotion_object_audio_channel_count(o_video);
			
			if (chl_max > 0)
				{
					chl = ++chl % chl_max;
					
					emotion_object_audio_channel_set(o_video, chl);
				}
			
			video_spu_audio_info_print(1);
			result = RAGE_EVENT_BLOCK;
			break;
		}
		case (RAGE_INPUT_SUBTITLE_ROTATE):
		{
			/* i = pIctures, subtitles */
			int chl;
			int chl_max;
			Eina_Bool muted = 0;
			
			chl = emotion_object_spu_channel_get(o_video);
			chl_max = emotion_object_spu_channel_count(o_video);
			muted = emotion_object_spu_mute_get(o_video);
			
			if (chl_max > 0)
				{
					/* ok, so we need to take into account the 'muting' of subs as well.
					 * so:
					 *  -1, 0, mute (never go back to -1...)
					 */
					++chl;
					
					/* once we've gone through the channels, turn subs off. */
					if (chl == chl_max)
						{
							/* see if subs are already off... */
							if (muted) { muted = 0; --chl; }
							else
								{
									/* really only for display purposes. */
									chl = -1;
									muted = 1;
								}
							emotion_object_spu_mute_set(o_video, muted);
						}
					else
						{
							/* -1, 0, 1, mute, 0, 1, ... */
						}
					
					emotion_object_spu_channel_set(o_video, chl);
				}
			
			video_spu_audio_info_print(1);
			result = RAGE_EVENT_BLOCK;
			break;
		}

		default:
			{
				result = RAGE_EVENT_CONTINUE;
				break;
			}

	/* else if (!strcmp(ev->keyname, "k")) */
	/* 	{ */
	/* 		/\* FIXME: volume display *\/ */
	/* 		emotion_object_audio_volume_set(o_video, emotion_object_audio_volume_get(o_video) - 0.1); */
	/* 	} */
	/* else if (!strcmp(ev->keyname, "l")) */
	/* 	{ */
	/* 		/\* FIXME: volume display *\/ */
	/* 		emotion_object_audio_volume_set(o_video, emotion_object_audio_volume_get(o_video) + 0.1); */
	/* 	} */
	/* else if (!strcmp(ev->keyname, "m")) */
	/* 	{ */
	/* 		/\* FIXME: volume display *\/ */
	/* 		if (emotion_object_audio_mute_get(o_video)) */
	/* 			emotion_object_audio_mute_set(o_video, 0); */
	/* 		else */
	/* 			emotion_object_audio_mute_set(o_video, 1); */
	/* 	} */
	/* else if (!strcmp(ev->keyname, "Home")) */
	/* 	{ */
	/* 		/\* FIXME: pop up menu for options etc. *\/ */
	/* 	} */
	/* else if (!strcmp(ev->keyname, "Insert")) */
	/* 	{ */
	/* 		/\* FIXME: program ? *\/ */
	/* 	} */
	/* else if (!strcmp(ev->keyname, "Pause")) */
	/* 	{ */
	/* 		/\* FIXME: standby ? *\/ */
	/* 	} */
	/* else if (!strcmp(ev->keyname, "End")) */
	/* 	{ */
	/* 		/\* FIXME: input ? *\/ */
	/* 	} */
	/* else if (!strcmp(ev->keyname, "BackSpace")) */
	/* 	{ */
	/* 		/\* FIXME: catv/clear ? *\/ */
	/* 	} */
	/* else if (!strcmp(ev->keyname, "t")) */
	/* 	{ */
	/* 		/\* FIXME: tool ? *\/ */
	/* 	} */
	/* else if (!strcmp(ev->keyname, "g")) */
	/* 	{ */
	/* 		/\* FIXME: tv guide ? *\/ */
	/* 	} */
	/* else if (!strcmp(ev->keyname, "period")) */
	/* 	{ */
	/* /\* FIXME: ch up ? *\/ */
  /*       int cnum, c, i; */
        
  /*       cnum = emotion_object_spu_channel_count(o_video); */
  /*       printf("cum: %i\n", cnum); */
  /*       c = emotion_object_spu_channel_get(o_video); */
  /*       for (c = 0; c < cnum; c++) */
  /*         { */
  /*            printf("%i: %s\n", emotion_object_spu_channel_name_get(o_video, c)); */
  /*         } */
  /*       c++; */
  /*       if (c >= cnum) c = 0; */
  /*       emotion_object_spu_channel_set(o_video, c); */
	/* 	} */
	/* else if (!strcmp(ev->keyname, "comma")) */
	/* 	{ */
	/* 		/\* FIXME: ch down ? *\/ */
	/* 	} */
	/* else if (!strcmp(ev->keyname, "Tab")) */
	/* 	{ */
	/* 		/\* FIXME: photo ? *\/ */
	/* 	} */
	/* else if (!strcmp(ev->keyname, "q")) */
	/* 	{ */
	/* 		/\* FIXME: repeat ? *\/ */
	/* 	} */
	/* else if (!strcmp(ev->keyname, "w")) */
	/* 	{ */
	/* 		/\* FIXME: rotate ? (zoom mode)  *\/ */
	/* 	} */
	/* else if (!strcmp(ev->keyname, "n")) */
	/* 	{ */
	/* 		/\* FIXME: play info display toggle *\/ */
	/* 	} */
		}
	
	return result;
}

