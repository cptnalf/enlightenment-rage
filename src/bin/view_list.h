/*
 * GeeXboX Enna Media Center.
 * Copyright (C) 2005-2010 The Enna Project
 *
 * This file is part of Enna.
 *
 * Enna is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Enna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Enna; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef LIST_H
#define LIST_H

//#include "enna.h"
#include "vfs.h"
//#include "input.h"

Evas_Object *rage_list_add (Evas *evas);
void rage_list_file_append(Evas_Object *obj, Vfs_Item* file, //Rage_Vfs_File *file,
    void (*func_activated) (void *data), void *data);
Eina_List* rage_list_files_get(Evas_Object* obj);
void rage_list_select_nth(Evas_Object *obj, int nth);
//Eina_Bool rage_list_input_feed(Evas_Object *obj, enna_input event);
void * rage_list_selected_data_get(Evas_Object *obj);
int rage_list_jump_label(Evas_Object *obj, const char *label);
void rage_list_jump_ascii(Evas_Object *obj, char k);
void rage_list_clear(Evas_Object *obj);

#endif /* LIST_H */

