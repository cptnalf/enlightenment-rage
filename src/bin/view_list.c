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

#include <Elementary.h>
#include "rage.h"
#include "input.h"
#include "layout.h"
#include "vfs.h"
#include "view_list.h"

#define SMART_NAME "rage_list"

typedef struct _Smart_Data Smart_Data;
typedef struct _List_Item List_Item;

struct _List_Item
{
	//	Rage_Vfs_File *file;
	Vfs_Item* file;
	void (*func_activated) (void *data);
	void *data;
	Elm_Genlist_Item *item;
};

struct _Smart_Data
{
	Evas_Object *obj;
	Eina_List *items;
};


static void
_item_activate(Elm_Genlist_Item *item)
{
	List_Item *li;

	li = (List_Item*)elm_genlist_item_data_get(item);
	if (li->func_activated)
		li->func_activated(li->data);
}

static void
_item_selected(void *data, Evas_Object *obj, void *event_info)
{
	List_Item *li = data;

	evas_object_smart_callback_call(obj, "hilight", li->data);
}

static void
_item_click_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Elm_Genlist_Item *item = data;

	/* Activate item only if it's already selected */
	if (elm_genlist_item_selected_get(item))
		_item_activate(item);
}

static void
_item_realized_cb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Genlist_Item *item = event_info;
	Evas_Object *o_item;

	o_item = (Evas_Object*)elm_genlist_item_object_get(item);
	evas_object_event_callback_add(o_item, EVAS_CALLBACK_MOUSE_UP,_item_click_cb, item);
}

static void
_item_remove(Evas_Object *obj, List_Item *item)
{
	Smart_Data *sd = evas_object_data_get(obj, "sd");

	if (!sd || !item) return;

	sd->items = eina_list_remove(sd->items, item);
	RAGE_FREE(item);

	return;
}

/* List View */
static char *
_list_item_label_get(const void *data, Evas_Object *obj, const char *part)
{
	const List_Item *li = data;

	if (!li || !li->file) return NULL;

	return li->file->label ? strdup(li->file->label) : NULL;
}

static Evas_Object *
_list_item_icon_get(const void *data, Evas_Object *obj, const char *part)
{
	List_Item *li = (List_Item*) data;

	if (!li) return NULL;

	if (!strcmp(part, "elm.swallow.icon"))
    {
			Evas_Object *ic;

			if (!li->file || !li->file->is_menu)
				return NULL;

			ic = elm_icon_add(obj);
			elm_icon_file_set(ic, rage_theme_get(), "icon/video");
			/* if (li->file->icon && li->file->icon[0] == '/') */
			/* 	{ elm_icon_file_set(ic, li->file->icon, NULL); } */
			/* else if (li->file->icon) */
			/* 	{ elm_icon_file_set(ic, rage_theme_get(), li->file->icon); } */
			/* else */
			/* 	{ return NULL; } */
			
			evas_object_size_hint_min_set(ic, 32, 32);
			evas_object_show(ic);
			return ic;
    }
	else if (!strcmp(part, "elm.swallow.end"))
    {
			Evas_Object *ic;

			if (!li->file || !li->file->is_menu)
				{ return NULL; }

			ic = elm_icon_add(obj);
			elm_icon_file_set(ic, rage_theme_get(), "icon/arrow_right");
			evas_object_size_hint_min_set(ic, 24, 24);
			evas_object_show(ic);
			return ic;
    }

	return NULL;
}

static void
_list_item_del(const void *data, Evas_Object *obj)
{
	//Rage_Vfs_File *item = (void *) data;
	Vfs_Item* item = (void*) data;

	if (!item) return;
}

static Elm_Genlist_Item_Class itc_list = {
	"rage",
	{
		_list_item_label_get,
		_list_item_icon_get,
		NULL,
		_list_item_del
	}
};


static void
_smart_select_item(Smart_Data *sd, int n)
{
	List_Item *it;

	it = eina_list_nth(sd->items, n);
	if (!it) return;

	elm_genlist_item_middle_bring_in(it->item);
	elm_genlist_item_selected_set(it->item, 1);
	evas_object_smart_callback_call(sd->obj, "hilight", it->data);
}

static void
list_set_item(Smart_Data *sd, int start, int up, int step)
{
	int n, ns;

	ns = start;
	n = start;

	int boundary = up ? eina_list_count(sd->items) - 1 : 0;

	if (n == boundary)
		n = ns;

	n = up ? n + step : n - step;

	if (n != ns)
		{ _smart_select_item(sd, n); }
}

static void
_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Smart_Data *sd = data;

	if (!sd)
		return;

	rage_list_clear(sd->obj);
	eina_list_free(sd->items);
	free(sd);
}

Evas_Object *
rage_list_add(Evas *evas)
{
	Evas_Object *obj;
	Smart_Data *sd;

	sd = calloc(1, sizeof(Smart_Data));

	obj = elm_genlist_add(layout_get());
	elm_object_style_set(obj, "enna");

	evas_object_size_hint_weight_set(obj, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_genlist_horizontal_mode_set(obj, ELM_LIST_COMPRESS);
	evas_object_show(obj);
	sd->obj = obj;

	evas_object_data_set(obj, "sd", sd);

	evas_object_smart_callback_add(obj, "realized", _item_realized_cb, sd);
	evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL, _del_cb, sd);

	return obj;
}

void
rage_list_file_append(Evas_Object *obj, Vfs_Item* file, //Rage_Vfs_File *file,
                      void (*func_activated) (void *data),  void *data)
{
	Smart_Data *sd;
	List_Item *it;

	sd = evas_object_data_get(obj, "sd");

	it = RAGE_NEW(List_Item, 1);
	it->item = elm_genlist_item_append (obj, &itc_list, it,
																			NULL, ELM_GENLIST_ITEM_NONE, _item_selected, it);

	it->func_activated = func_activated;
	it->data = data;
	it->file = file;

	sd->items = eina_list_append(sd->items, it);
}

void
rage_list_select_nth(Evas_Object *obj, int nth)
{
	Smart_Data *sd = evas_object_data_get(obj, "sd");
	_smart_select_item(sd, nth);
}

Eina_List *
rage_list_files_get(Evas_Object* obj)
{
	Smart_Data *sd = evas_object_data_get(obj, "sd");
	Eina_List *files = NULL;
	Eina_List *l;
	List_Item *it;

	EINA_LIST_FOREACH(sd->items, l, it)
		{ files = eina_list_append(files, it->file); }

	return files;
}

int
rage_list_jump_label(Evas_Object *obj, const char *label)
{
	List_Item *it = NULL;
	Eina_List *l;
	int i = 0;

	Smart_Data *sd = evas_object_data_get(obj, "sd");

	if (!sd || !label) return -1;

	EINA_LIST_FOREACH(sd->items, l, it)
    {
			if (it->file->label && !strcmp(it->file->label, label))
        {
					_smart_select_item(sd, i);
					return i;
        }
			i++;
    }

	return -1;
}

int
rage_list_selected_get(Evas_Object *obj)
{
	Eina_List *l;
	List_Item *it;
	int i = 0;
	Smart_Data *sd = evas_object_data_get(obj, "sd");

	if (!sd->items) return -1;
	EINA_LIST_FOREACH(sd->items,l, it)
    {
			if ( elm_genlist_item_selected_get (it->item))
        {
					return i;
        }
			i++;
    }
	return -1;
}

void *
rage_list_selected_data_get(Evas_Object *obj)
{
	Eina_List *l;
	List_Item *it;
	Smart_Data *sd = evas_object_data_get(obj, "sd");

	if (!sd->items) return NULL;

	EINA_LIST_FOREACH(sd->items,l, it)
    {
			if ( elm_genlist_item_selected_get (it->item))
        {
					return it->data;
        }
    }
	return NULL;
}

void
rage_list_jump_ascii(Evas_Object *obj, char k)
{
	List_Item *it;
	Eina_List *l;
	int i = 0;
	Smart_Data *sd = evas_object_data_get(obj, "sd");

	EINA_LIST_FOREACH(sd->items, l, it)
    {
			if (it->file->label[0] == k || it->file->label[0] == k - 32)
        {
					_smart_select_item(sd, i);
					return;
        }
			i++;
    }
}

#define LIST_SEEK_OFFSET 5

static Eina_Bool
view_list_item_select (Evas_Object *obj, int down, int cycle, int range)
{
	int ns, total, start, end;
	Smart_Data *sd = evas_object_data_get(obj, "sd");

	ns    = rage_list_selected_get(obj);
	total = eina_list_count(sd->items);
	start = down ? total - 1 : 0;
	end   = down ? 0 : total - 1;

	if (ns == start)
		_smart_select_item(sd, end);
	else if (cycle)
    {
			if (!down && (ns - range < 0))
				_smart_select_item(sd, 0);
			else if (down && (ns + range > total - 1))
				_smart_select_item(sd, total - 1);
			else
				list_set_item(sd, ns, down, range);
    }
	else
		list_set_item(sd, ns, down, range);

	return ECORE_CALLBACK_CANCEL;
}

Eina_Bool
rage_list_input_feed(Evas_Object *obj, rage_input event)
{
	int total;
	Smart_Data *sd = evas_object_data_get(obj, "sd");

	total = eina_list_count(sd->items);

	switch (event)
    {
		case RAGE_INPUT_UP:
			return view_list_item_select(obj, 0, 0, 1);
		case RAGE_INPUT_PREV:
			return view_list_item_select(obj, 0, 1, LIST_SEEK_OFFSET);
		case RAGE_INPUT_DOWN:
			return view_list_item_select(obj, 1, 0, 1);
		case RAGE_INPUT_NEXT:
			return view_list_item_select(obj, 1, 1, LIST_SEEK_OFFSET);
		/* case RAGE_INPUT_FIRST: */
		/* 	_smart_select_item(sd, 0); */
		/* 	return RAGE_EVENT_BLOCK; */
		/* 	break; */
		/* case RAGE_INPUT_LAST: */
		/* 	_smart_select_item(sd, total - 1); */
		/* 	return RAGE_EVENT_BLOCK; */
		/* 	break; */
		case RAGE_INPUT_OK:
			{
				List_Item *it = eina_list_nth(sd->items, rage_list_selected_get(obj));
				if (it)
					{
						if (it->func_activated)
							it->func_activated(it->data);
					}
			}
			return RAGE_EVENT_BLOCK;
			break;
		default:
			break;
    }
	return RAGE_EVENT_CONTINUE;
}

void
rage_list_clear(Evas_Object *obj)
{
	Smart_Data *sd = evas_object_data_get(obj, "sd");
	List_Item *item;
	Eina_List *l, *l_next;

	elm_genlist_clear(obj);
	EINA_LIST_FOREACH_SAFE(sd->items, l, l_next, item)
    {
			_item_remove(obj, item);
    }
}
