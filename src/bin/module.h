/* filename: module.h
 *  chiefengineer
 *  Sat May 08 00:21:26 PDT 2010
 * 
 *  stolen mostly from enna.
 */

#ifndef MODULE_H
#define MODULE_H

#define RAGE_MODULE_VERSION 4

#define MOD_PREFIX module /* default name for dynamic linking */
#define MOD_APPEND_API(prefix)           prefix##_api
#define RAGE_MOD_PREFIX_API(prefix)      MOD_APPEND_API(prefix)

/* Entries to use in the modules. */
#define RAGE_MODULE_API                  RAGE_MOD_PREFIX_API(MOD_PREFIX)

typedef struct _Rage_Module Rage_Module;
typedef struct _Rage_Module_Api Rage_Module_Api;

struct _Rage_Module
{
    Rage_Module_Api *api;
    unsigned char enabled;
    void *mod;
};

struct _Rage_Module_Api
{
    int version;
    const char *name;
    const char *title;
    const char *icon;
    const char *short_desc;
    const char *long_desc;
    struct
    {
        void (*init)(Rage_Module *m);
        void (*shutdown)(Rage_Module *m);
    } func;
};

int          rage_module_init(void);
void         rage_module_shutdown(void);
void         rage_module_load_all(void);
int          rage_module_enable(Rage_Module *m);
int          rage_module_disable(Rage_Module *m);

#endif /* MODULE_H */
