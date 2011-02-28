/* filename: database_vfs.c
 *  chiefengineer
 *  Sat May 15 13:57:16 PDT 2010
 */

#include <Eina.h>
#include <stdio.h>
#include "rage.h"
#include "module.h"
#include "vfs.h"

#include "database.h"

#define RAGE_MODULE_NAME  "vfs_database_video"

static int
genre_item_sort(const void* d1, const void* d2)
{
	const Genre* g1;
	const Genre* g2;
	g1 = d1;
	g2 = d2;
	return strcmp(g1->label, g2->label);
}

static void
list_string_free(Eina_List *list)
{
	while (list)
		{
			Genre *ge;
			
			ge = list->data;
			eina_stringshare_del(ge->label);
			free(ge);
			list = eina_list_remove_list(list, list);
		}
}

static Eina_List*
list_string_unique_append(Eina_List* list, const char* str, int count)
{
	Eina_List* l;
	Genre* ge;
	
	EINA_LIST_FOREACH(list, l, ge)
		{
			if (!strcmp(str, ge->label))
				{
					ge->count += count;
					return list;
				}
		}
	
	ge = calloc(1, sizeof(Genre));
	ge->label = eina_stringshare_add(str);
	ge->count = count;
	list = eina_list_append(list, ge);
	return list;
}

static Eina_List *
list_video_genres(const char* genre)
{
	const Eina_List *l;
	Eina_List *genres = NULL;
	Database* db;
	DBIterator* it;
	Genre* ge;
	
	db = database_new();
	
	/* get a list of genre strings */
	it = database_video_genres_get(db, genre);
	
	while(ge = database_iterator_next(it)) { genres = eina_list_append(genres, ge); }
	
	database_iterator_free(it);
	database_free(db);
	
	return genres;
}

static Eina_List*
_get_genres(Eina_List* videos, const char* genre)
{
	const Eina_List *l;
	Eina_List *genres = NULL, *glist = NULL;
	Vfs_Item* vi;
	
	/* determine toplevel genres */
	genres = list_video_genres(genre);
	if (genres)
		{
			int vlpn;
			const char* sel = NULL;
			
			vlpn = strlen(genre);
			printf("--- %s\n", genre);
			/* walk the genres we got back, 
			 * fixing them up to be children of the passed one.
			 */
			if (vlpn > 0)
				{
					/* we have a part of the 'genre' to wack off... */
					Genre* ge;
					
					EINA_LIST_FOREACH(genres, l, ge)
						{
							if ((!strncmp(genre, ge->label, vlpn)) &&
									(strlen(ge->label) != vlpn))
								{
									char *s, *p;
									
									/* move to the first char after the '/'
									 * e.g. anime/world_destruction
									 *            |- here
									 *
									 * we then want to cap off any lower sectioins we might have
									 * the unique append effectively flattens the potentially
									 * deep structure into one level.
									 * e.g.
									 *  anime/sort/bar/foo
									 *  anime/sort/bar
									 *  anime/sort
									 *  anime
									 *  
									 *  if my query was empty or blank,
									 *  this would roll up:
									 *  anime/sort/bar/foo
									 *  anime/sort/bar
									 *  anime/sort
									 *  into:
									 *  anime
									 */
									s = strdup(ge->label + vlpn + 1);
									p = strchr(s, '/');
									if (p) *p = 0;
									
									glist = list_string_unique_append(glist, s, ge->count);
									free(s);
								}
						}
				}
			else
				{
					Genre* ge;
					
					EINA_LIST_FOREACH(genres, l, ge)
						{
							char *s, *p;
							
							/* see above for explaination. */
							s = strdup(ge->label);
							p = strchr(s, '/');
							if (p) *p = 0;
							
							glist = list_string_unique_append(glist, s, ge->count);
							free(s);
						}
				}
			/* we're done with the genres list we queried... */
			list_string_free(genres);
			
			if (glist)
				{
					/* we got some genres, so process them. */
					Genre* ge;
					
					/* sort the glist. */
					glist = eina_list_sort(glist, eina_list_count(glist), genre_item_sort);
					
					
					EINA_LIST_FOREACH(glist, l, ge)
						{
							char buf[4096];
							Vfs_Item* vli;
							
							if (genre[0])
								snprintf(buf, sizeof(buf), "%s/%s", genre, ge->label);
							else
								snprintf(buf, sizeof(buf), "%s", ge->label);
							
							vli = vfs_item_new(ge->label, buf, ge->count);
							videos = eina_list_append(videos, vli);
						}
					
					list_string_free(glist);
				}
		}
	
	return videos;
}

static Eina_List*
_get_items(Eina_List* videos, const char* genre)
{
	DBIterator* it;
	Database* db = database_new();
	it = database_video_files_genre_search(db, genre);
	
	{
		Vfs_Item* vli;
		Volume_Item* vi;
		
		while (vi = (Volume_Item*)database_iterator_next(it))
			{
				vli = vfs_item_new_withvolume(vi);
				
				videos = eina_list_append(videos, vli);
			}
	}
	
	database_iterator_free(it);
	database_free(db);
	
	return videos;
}

static Eina_List* 
_vfs_db_videos_get(Vfs_Item* item)
{
	Eina_List* videos = NULL;
	
	/* if it's a menu,
	 *  we need to see if it has any children genres first.
	 * after we're done with that, 
	 *  then we figure out if we need to figure out if we need to find some loose items.
	 */
	if (item->is_menu)
		{
			videos = _get_genres(videos, item->path);
		}
	
	if (item->path && item->path[0] != '\0')
		{
			videos = _get_items(videos, item->path);
		}
	
	return videos;
}

/**
 *  @param count  number of recent files to get.
 */
static Eina_List* _vfs_db_videos_recents_get(int count)
{
	Eina_List* recents = NULL;
	Database* db;
	DBIterator* it;
	
	db = database_new();
	it = database_video_recents_get(db);
	
	{
		Vfs_Item* vli;
		Volume_Item* vi;
		
		while (vi = (Volume_Item*)database_iterator_next(it))
			{
				vli = vfs_item_new_withvolume(vi);
				
				recents = eina_list_append(recents, vli);
			}
	}
	
	database_iterator_free(it);
	database_free(db);
	
	return recents;
}

static Eina_List* _vfs_db_videos_news_get(int count)
{
	Eina_List* news = NULL;
	Database* db;
	DBIterator* it;
	
	db = database_new();
	it = database_video_news_get(db);
	{
		Vfs_Item* vli;
		Volume_Item* vi;
		
		while (vi = (Volume_Item*)database_iterator_next(it))
			{
				vli = vfs_item_new_withvolume(vi);
				
				news = eina_list_append(news, vli);
			}
	}
	
	database_iterator_free(it);
	database_free(db);
	
	return news;
}


/**
 *  @param count  number of favorites to get.
 */
static Eina_List* 
_vfs_db_videos_favorites_get(int count)
{
	Eina_List* favorites = NULL;
	Database* db;
	DBIterator* it;
	
	db = database_new();
	it = database_video_favorites_get(db);
	
	{
		Vfs_Item* vli;
		Volume_Item* vi;
		
		/* the next fx gives me a pointer... */
		while (vi = (Volume_Item*)database_iterator_next(it))
			{
				/* construct the video lib item from the volume item. */			
				vli = vfs_item_new_withvolume(vi);
				
				favorites = eina_list_append(favorites, vli);
			}
	}
	
	database_iterator_free(it);
	database_free(db);
	
	return favorites;
}

static void _vfs_db_videos_record_play(Vfs_Item* item)
{
	if (! item->is_menu && item->vi)
		{
			Database* db;
			db = database_new();
			database_video_file_update(db, item->vi);
			database_free(db);
		}
}

Vfs_Source DatabaseVideoVfs = 
	{
		VFS_TYPE_VIDEO,
		"Database Videos VFS",
		"icon/video",
		"icon/video_folder",
		
		_vfs_db_videos_get,
		_vfs_db_videos_recents_get,
		_vfs_db_videos_favorites_get,
		_vfs_db_videos_record_play,
	};

/* Module interface */
#ifdef USE_STATIC_MODULES
#undef MOD_PREFIX
#define MOD_PREFIX rage_mod_vfs_db_video
#endif /* USE_STATIC_MODULES */

static void
module_init(Rage_Module *em)
{
	rage_vfs_source_add(&DatabaseVideoVfs);
}

static void
module_shutdown(Rage_Module *em)
{
	
}

Rage_Module_Api RAGE_MODULE_API =
	{
		RAGE_MODULE_VERSION,
		RAGE_MODULE_NAME,
		"Database Videos VFS",
		NULL,
		"Module to provide rage with video files from a database, as well as recording play",
		NULL,
		{
			module_init,
			module_shutdown
		}
	};

