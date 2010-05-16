/* filename: module.c
 *  chiefengineer
 *  Sat May 08 00:09:51 PDT 2010
 */

#include <Eina.h>
#include "rage.h"
#include "module.h"
#include <stdio.h>

static Eina_List *_rage_modules = NULL;   /** List of Enna_Modules* */
static Eina_Array *_plugins_array = NULL; /** Array of Eina_Modules* (or api* in static mode)*/

#if 0
extern Rage_Module_Api rage_mod_activity_tv_api;
extern Rage_Module_Api rage_mod_activity_video_api;
#endif

extern Rage_Module_Api rage_mod_input_kbd_api;
extern Rage_Module_Api rage_mod_vfs_db_video_api;

#ifdef BUILD_INPUT_LIRC
extern Rage_Module_Api rage_mod_input_lirc_api;
#endif

static Rage_Module *
rage_module_open(Rage_Module_Api *api)
{
	Rage_Module *m;
	
	if (!api || !api->name) return NULL;
	
	if (api->version != RAGE_MODULE_VERSION )
    {
			/* FIXME: popup error message */
			/* Module version doesn't match enna version */
			printf("Bad module version, %s module", api->name);
			return NULL;
    }
	
	m = RAGE_NEW(Rage_Module, 1);
	m->api = api;
	m->enabled = 0;
	_rage_modules = eina_list_append(_rage_modules, m);
	return m;
}

/**
 * @brief Init the module system
 */
int
rage_module_init(void)
{
	Eina_Array_Iterator iterator;
	unsigned int i;
  
	Rage_Module_Api *api;
	
	/* Populate the array of available plugins statically */
	_plugins_array = eina_array_new(20);
#if 0
#ifdef BUILD_ACTIVITY_CONFIGURATION
	eina_array_push(_plugins_array, &rage_mod_activity_configuration_api);
#endif
#ifdef BUILD_ACTIVITY_MUSIC
	eina_array_push(_plugins_array, &rage_mod_activity_music_api);
#endif
#ifdef BUILD_ACTIVITY_PHOTO
	eina_array_push(_plugins_array, &rage_mod_activity_photo_api);
#endif
#ifdef BUILD_ACTIVITY_TV
	eina_array_push(_plugins_array, &rage_mod_activity_tv_api);
#endif
	
	eina_array_push(_plugins_array, &rage_mod_activity_video_api);
#endif
	
#ifdef BUILD_BROWSER_VALHALLA
	eina_array_push(_plugins_array, &rage_mod_browser_valhalla_api);
#endif
	
	eina_array_push(_plugins_array, &rage_mod_input_kbd_api);
#ifdef BUILD_INPUT_LIRC
	eina_array_push(_plugins_array, &rage_mod_input_lirc_api);
#endif
	
	eina_array_push(_plugins_array, &rage_mod_vfs_db_video_api);
	
	/* Log the array */
	printf("Available Plugins (static):\n");
	EINA_ARRAY_ITER_NEXT(_plugins_array, i, api, iterator)
		{ printf("\t * %s\n", api->name); }
	printf("\n");
	return 0;
}

/**
 * @brief Disable/Free all modules registered and free the Eina_Module Array
 */
void
rage_module_shutdown(void)
{
	Rage_Module *m;
	
	/* Disable and free all Rage_Modules */
	EINA_LIST_FREE(_rage_modules, m)
    {
			printf( "Disable module : %s\n", m->api->name);
			if (m->enabled)
				rage_module_disable(m);
			free(m);
    }
	_rage_modules = NULL;
	
	if (_plugins_array)
    {
			/* Free the Eina_Array of static pointer */
			eina_array_free(_plugins_array);
			_plugins_array = NULL;
    }
}

/**
 * @brief Enable the given module
 */
int
rage_module_enable(Rage_Module *m)
{
	if (!m)
		return -1;
	if (m->enabled)
		return 0;
	if (m->api->func.init)
		m->api->func.init(m);
	m->enabled = 1;
	return 0;
}

/**
 * @brief Disable the given module
 */
int
rage_module_disable(Rage_Module *m)
{
	if (!m)
		return -1;
	if (!m->enabled)
		return 0;
	if (m->api->func.shutdown)
    {
			m->api->func.shutdown(m);
			m->enabled = 0;
			return 0;
    }
	return -1;
}

/**
 * @brief Load/Enable all the know modules
 */
void
rage_module_load_all(void)
{
	Rage_Module_Api *api;
	Rage_Module *em;
	Eina_Array_Iterator iterator;
	unsigned int i;
	
	if (!_plugins_array)
		return;
	
	EINA_ARRAY_ITER_NEXT(_plugins_array, i, api, iterator)
    {
			em = rage_module_open(api);
			rage_module_enable(em);
    }
}
