/* filename: kbd.c
 *  chiefengineer
 *  Fri May 07 23:56:40 PDT 2010
 *  
 *  mostly stolen from enna.
 */

#include "rage.h"
#include "input.h"
#include "module.h"

#define RAGE_MODULE_NAME "input_kbd"

static const struct
{
    const char *keyname;
    Ecore_Event_Modifier modifier;
    rage_input input;
} rage_keymap[] = {
	
#include "basekeymap.h"
#include "videokeymap.h"

	/* this one needs to be last. */
#include "fallbackkeymap.h"	
    { NULL,           ECORE_NONE,              RAGE_INPUT_UNKNOWN       }
};

static Ecore_Event_Handler *key_down_event_handler;

static rage_input
_input_event_modifier (Ecore_Event_Key *ev)
{
	int i;
	
	for (i = 0; rage_keymap[i].keyname; i++)
    {
			if (ev->modifiers == rage_keymap[i].modifier &&
					!strcmp(rage_keymap[i].keyname, ev->key))
        {
					return rage_keymap[i].input;
        }
    }
	
	return RAGE_INPUT_UNKNOWN;
}

static rage_input
_input_event (Ecore_Event_Key *ev)
{
	int i;
	
	for (i = 0; rage_keymap[i].keyname; i++)
    {
			if ((rage_keymap[i].modifier == ECORE_NONE) &&
					!strcmp(rage_keymap[i].keyname, ev->key))
        {
					return rage_keymap[i].input;
        }
    }
	
	return RAGE_INPUT_UNKNOWN;
}

static rage_input
_get_input_from_event(Ecore_Event_Key *ev)
{
	if (!ev)
		return RAGE_INPUT_UNKNOWN;
	
	/* discard some modifiers */
	if (ev->modifiers >= ECORE_EVENT_LOCK_CAPS)
		ev->modifiers -= ECORE_EVENT_LOCK_CAPS;
	if (ev->modifiers >= ECORE_EVENT_LOCK_NUM)
		ev->modifiers -= ECORE_EVENT_LOCK_NUM;
	if (ev->modifiers >= ECORE_EVENT_LOCK_SCROLL)
		ev->modifiers -= ECORE_EVENT_LOCK_SCROLL;
	
	return ( (ev->modifiers && ev->modifiers < ECORE_LAST)
					 ? _input_event_modifier(ev) 
					 : _input_event(ev)
					 );
}

static int
_ecore_event_key_down_cb(void *data, int type, void *event)
{
	Ecore_Event_Key *e = event;
	rage_input in;
	
	//enna_idle_timer_renew();
	
	in = _get_input_from_event(e);
	if (in != RAGE_INPUT_UNKNOWN)
		rage_input_event_emit(in);
	
	return ECORE_CALLBACK_CANCEL;
}


/* Module interface */

#ifdef USE_STATIC_MODULES
#undef MOD_PREFIX
#define MOD_PREFIX rage_mod_input_kbd
#endif /* USE_STATIC_MODULES */

static void
module_init(Rage_Module *em)
{
	key_down_event_handler =
		ecore_event_handler_add (ECORE_EVENT_KEY_DOWN, _ecore_event_key_down_cb, NULL);
}

static void
module_shutdown(Rage_Module *em)
{
    RAGE_EVENT_HANDLER_DEL(key_down_event_handler);
}

Rage_Module_Api RAGE_MODULE_API =
	{
		RAGE_MODULE_VERSION,
		RAGE_MODULE_NAME,
		"Keyboard Controls",
		NULL,
		"Module to control rage from the keyboard",
		NULL,
		{
			module_init,
			module_shutdown
		}
	};
