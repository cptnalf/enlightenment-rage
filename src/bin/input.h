/* filename: input.h
 *  chiefengineer
 *  Fri May 07 23:38:38 PDT 2010
 */

#ifndef INPUT_H
#define INPUT_H

#include <Ecore_Evas.h>
#include <Ecore.h>

#define RAGE_EVENT_BLOCK ECORE_CALLBACK_CANCEL
#define RAGE_EVENT_CONTINUE ECORE_CALLBACK_RENEW

typedef struct _Input_Listener Input_Listener;

typedef enum
	{
		RAGE_INPUT_UNKNOWN,
		
		RAGE_INPUT_OK,
		
		RAGE_INPUT_LEFT,
		RAGE_INPUT_RIGHT,
		RAGE_INPUT_UP,
		RAGE_INPUT_DOWN,
		
		RAGE_INPUT_NEXT,
		RAGE_INPUT_PREV,
		
		RAGE_INPUT_QUIT,
		RAGE_INPUT_MENU,
		RAGE_INPUT_FULLSCREEN,
		
		RAGE_INPUT_PAUSE,
		RAGE_INPUT_STOP,
		RAGE_INPUT_FASTFORWARD,
		RAGE_INPUT_REWIND,
		RAGE_INPUT_AUDIO_ROTATE,
		RAGE_INPUT_SUBTITLE_ROTATE,
		
		RAGE_INPUT_KEY_SPACE,
		
		RAGE_INPUT_KEY_0,
		RAGE_INPUT_KEY_1,
		RAGE_INPUT_KEY_2,
		RAGE_INPUT_KEY_3,
		RAGE_INPUT_KEY_4,
		RAGE_INPUT_KEY_5,
		RAGE_INPUT_KEY_6,
		RAGE_INPUT_KEY_7,
		RAGE_INPUT_KEY_8,
		RAGE_INPUT_KEY_9,
		
		RAGE_INPUT_KEY_A,
		RAGE_INPUT_KEY_B,
		RAGE_INPUT_KEY_C,
		RAGE_INPUT_KEY_D,
		RAGE_INPUT_KEY_E,
		RAGE_INPUT_KEY_F,
		RAGE_INPUT_KEY_G,
		RAGE_INPUT_KEY_H,
		RAGE_INPUT_KEY_I,
		RAGE_INPUT_KEY_J,
		RAGE_INPUT_KEY_K,
		RAGE_INPUT_KEY_L,
		RAGE_INPUT_KEY_M,
		RAGE_INPUT_KEY_N,
		RAGE_INPUT_KEY_O,
		RAGE_INPUT_KEY_P,
		RAGE_INPUT_KEY_Q,
		RAGE_INPUT_KEY_R,
		RAGE_INPUT_KEY_S,
		RAGE_INPUT_KEY_T,
		RAGE_INPUT_KEY_U,
		RAGE_INPUT_KEY_V,
		RAGE_INPUT_KEY_W,
		RAGE_INPUT_KEY_X,
		RAGE_INPUT_KEY_Y,
		RAGE_INPUT_KEY_Z,
	} rage_input;

Eina_Bool rage_input_event_emit(rage_input in);
Input_Listener* rage_input_listener_add(const char* name,
																				Eina_Bool(*func)(void* data, 
																												 rage_input event),
																				void* data);
void rage_input_listener_promote(Input_Listener* il);
void rage_input_listener_demote(Input_Listener* il);
void rage_input_listener_del(Input_Listener* il);

#endif
