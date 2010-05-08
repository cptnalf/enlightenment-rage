
#ifndef LAYOUT_H
#define LAYOUT_H
#include <Evas.h>

Evas_Object* layout_get();
int layout_init(void);
void layout_resize(void);
void layout_swallow(char *layout, Evas_Object *obj);
    
#endif
