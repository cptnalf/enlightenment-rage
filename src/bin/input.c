/* filename: input.c
 *  chiefengineer
 *  Sat May 08 00:31:26 PDT 2010
 *  
 *  mostly stolen from enna.
 */

#include <string.h>
#include <ctype.h>
#include "rage.h"
#include "input.h"

struct _Input_Listener {
    const char *name;
    Eina_Bool (*func)(void *data, rage_input event);
    void *data;
};

/* Local Globals */
static Eina_List *_listeners = NULL;

/* Public Functions */
Eina_Bool
rage_input_event_emit(rage_input in)
{
	Input_Listener *il;
	Eina_List *l;
	Eina_Bool ret;
	
	/* enna_log(ENNA_MSG_EVENT, NULL, "Input emit: %d (listeners: %d)", in, eina_list_count(_listeners)); */

	//enna_idle_timer_renew();
	EINA_LIST_FOREACH(_listeners, l, il)
    {
			//enna_log(ENNA_MSG_EVENT, NULL, "  emit to: %s", il->name);
			if (!il->func) continue;
			
			ret = il->func(il->data, in);
			if (ret == RAGE_EVENT_BLOCK)
        {
					//enna_log(ENNA_MSG_EVENT, NULL, "  emission stopped by: %s", il->name);
					return EINA_TRUE;
        }
    }
	
	return EINA_TRUE;
}

Input_Listener *
rage_input_listener_add(const char *name,
                        Eina_Bool(*func)(void *data, rage_input event),
                        void *data)
{
	Input_Listener *il;
	
	//enna_log(ENNA_MSG_INFO, NULL, "listener add: %s", name);
	il = RAGE_NEW(Input_Listener, 1);
	if (!il) return NULL;
	il->name = eina_stringshare_add(name);
	il->func = func;
	il->data = data;
	
	_listeners = eina_list_prepend(_listeners, il);
	return il;
}

void
rage_input_listener_promote(Input_Listener *il)
{
	Eina_List *l;
	
	l = eina_list_data_find_list(_listeners, il);
	if (!l) return;
	
	_listeners  = eina_list_promote_list(_listeners, l);
}

void
rage_input_listener_demote(Input_Listener *il)
{
	Eina_List *l;
	
	l = eina_list_data_find_list(_listeners, il);
	if (!l) return;
	
	_listeners  = eina_list_demote_list(_listeners, l);
}

void
rage_input_listener_del(Input_Listener *il)
{
	if (!il) return;
	//enna_log(ENNA_MSG_INFO, NULL, "listener del: %s", il->name);
	_listeners = eina_list_remove(_listeners, il);
	eina_stringshare_del(il->name);
	RAGE_FREE(il);
}
