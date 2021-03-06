#include "main.h"

#include <sys/types.h>
#include <dirent.h>

#define STARTDELAY 1.000
#define SCANDELAY  0.250
#define SCANSPEED  0.001

typedef struct _Scan Scan;
  
struct _Scan
{
   char *vol;
   Ecore_Timer *timer;
   Eina_List *dirstack;
   Eina_List *items;
   char curdir[4096];
	void* db;
};

static char *volume_list_exists(Eina_List *list, char *vol);
static void volume_file_change(void *data, Ecore_File_Monitor *fmon, Ecore_File_Event ev, const char *path);
static Eina_Bool volume_timer(void *data);
static int volume_idler(void *data);
static Volume_Item *volume_file_scan(char *file);
static Volume_Item *volume_dir_scan(char *dir);
static int volume_item_sort(const void *d1, const void *d2);
static void volume_items_sort(Scan *s);

static Eina_List *volumes = NULL;
static Ecore_File_Monitor *volumes_file_mon = NULL;
static Ecore_Timer *volumes_load_timer = NULL;
static Eina_List *scans = NULL;
static Eina_List *items = NULL;
static int video_count = 0;
static int audio_count = 0;
static int photo_count = 0;

int VOLUME_ADD = 0;
int VOLUME_DEL = 0;
int VOLUME_TYPE_ADD = 0;
int VOLUME_TYPE_DEL = 0;
int VOLUME_SCAN_START = 0;
int VOLUME_SCAN_STOP = 0;
int VOLUME_SCAN_GO = 0;

Volume_Item*
volume_item_new(const long long id, const char* path, const char* name, const char* genre, const char* type)
{
	Volume_Item* item = calloc(1, sizeof(Volume_Item));
	
	item->id = id;
	item->path = strdup(path);
	item->rpath = ecore_file_realpath(item->path);
	if (name) { item->name = strdup(name); }
	if (genre) { item->genre = eina_stringshare_add(genre); }
	if (type) { item->type = eina_stringshare_add(type); }
	
	return item;
}

Volume_Item*
volume_item_copy(Volume_Item* item)
{
	Volume_Item* copy = volume_item_new(item->id, item->path, item->name, item->genre, item->type);
	
	copy->last_played = item->last_played;
	copy->play_count = item->play_count;
  copy->last_pos = item->last_pos;
  copy->length = item->length;
	if (item->artist) { copy->artist = eina_stringshare_add(item->artist); }
	if (item->album)  { copy->album = eina_stringshare_add(item->album); }
  copy->track = item->track;
	
	return copy;
}

void
volume_item_free(Volume_Item* item)
{
	if (item)
		{
			free(item->path);
			free(item->rpath);
			
			if (item->name) { free(item->name); }
			
			if (item->genre) { eina_stringshare_del(item->genre); }
			if (item->type) { eina_stringshare_del(item->type); }
			
			if (item->artist) { eina_stringshare_del(item->artist); }
			if (item->album)  { eina_stringshare_del(item->album); }
			free(item);
		}
}

void
volume_init(void)
{
	VOLUME_ADD = ecore_event_type_new();
	VOLUME_DEL = ecore_event_type_new();
	VOLUME_TYPE_ADD = ecore_event_type_new();
	VOLUME_TYPE_DEL = ecore_event_type_new();
	VOLUME_SCAN_START = ecore_event_type_new();
	VOLUME_SCAN_STOP = ecore_event_type_new();
	VOLUME_SCAN_GO = ecore_event_type_new();
	volumes_load_timer = ecore_timer_add(STARTDELAY, volume_timer, NULL);
}

void
volume_update(void)
{
	while (volumes) volume_del(volumes->data);
	volumes_load_timer = ecore_timer_add(SCANDELAY, volume_timer, NULL);
}

void
volume_load(void)
{
   FILE *f;
   char buf[4096];
   Eina_List *tvolumes, *l;
   
   snprintf(buf, sizeof(buf), "%s/volumes", config);
   tvolumes = volumes;
   volumes = NULL;
   f = fopen(buf, "rb");
   for (l = tvolumes; l; l = l->next)
     volumes = eina_list_append(volumes, strdup(l->data));
   if (f)
     {
	while (fgets(buf, sizeof(buf), f))
	  {
	     int len;
	     char *vol;
	     
	     len = strlen(buf);
	     if (len < 1) continue;
	     if (buf[0] == '#') continue;
	     if (buf[len - 1] == '\n') buf[len - 1] = 0;
	     vol = volume_list_exists(tvolumes, buf);
	     if (!vol)
	       volume_add(buf);
	     else
	       {
		  tvolumes = eina_list_remove(tvolumes, vol);
		  free(vol);
	       }
	  }
	fclose(f);
     }
   /* whats left in tvolumes is to be deleted */
   while (tvolumes)
     {
	char *vol;
	
	vol = tvolumes->data;
	tvolumes = eina_list_remove_list(tvolumes, tvolumes);
	volume_del(vol);
	free(vol);
     }
}

void
volume_add(char *vol)
{
   volumes = eina_list_append(volumes, strdup(vol));
   volume_index(vol);
   ecore_event_add(VOLUME_ADD, strdup(vol), NULL, NULL);
}

void
volume_del(char *vol)
{
   volume_deindex(vol);
   vol = volume_list_exists(volumes, vol);
   if (vol)
     {
	ecore_event_add(VOLUME_DEL, strdup(vol), NULL, NULL);
	volumes = eina_list_remove(volumes, vol);
	free(vol);
     }
}

int
volume_exists(char *vol)
{
	return (int)volume_list_exists(volumes, vol);
}

void
volume_index(char *vol)
{
	Scan *s;
   
/* 	s = calloc(1, sizeof(Scan)); */
/* 	s->vol = strdup(vol); */
/* 	s->timer = ecore_timer_add(SCANSPEED, volume_idler, s); */
/* 	scans = eina_list_append(scans, s); */
}

void
volume_deindex(char *vol)
{
   Eina_List *l;
   
	for (l = scans; l; l = l->next)
		{
			Scan *s;
	
			s = l->data;
			if (!strcmp(s->vol, vol))
				{
					free(s->vol);
					if (s->timer)
						{
							ecore_event_add(VOLUME_SCAN_STOP, strdup(s->vol), NULL, NULL);
							ecore_timer_del(s->timer);
						}
					while (s->items)
						{
							Volume_Item *vi;
		  
		  vi = s->items->data;
		  items = eina_list_remove(items, vi);
		  s->items = eina_list_remove_list(s->items, s->items);
		  if 
		    (!strcmp(vi->type, "video"))
		    video_count--;
		  else if
		    (!strcmp(vi->type, "audio"))
		    audio_count--;
		  else if 
		    (!strcmp(vi->type, "photo"))
		    photo_count--;
		  ecore_event_add(VOLUME_TYPE_DEL, strdup(vi->path), NULL, NULL);
 		  free(vi->path);
 		  free(vi->rpath);
		  free(vi->name);
		  if (vi->genre) eina_stringshare_del(vi->genre);
		  free(vi);
	       }
	     free(s);
	     scans = eina_list_remove_list(scans, l);
	     return;
	  }
     }
}

int
volume_type_num_get(char *type)
{
	if
		(!strcmp(type, "video"))
		return video_count;
	else if 
		(!strcmp(type, "audio"))
		return audio_count;
	else if 
		(!strcmp(type, "photo"))
		return photo_count;
	return 0;
}

const Eina_List *
volume_items_get(void)
{
	printf("??0x%X\n", (unsigned int)items);
	return items;
}

static char *
volume_list_exists(Eina_List *list, char *vol)
{
   Eina_List *l;
   
	for (l = list; l; l = l->next)
		{
			if (!strcmp(l->data, vol)) return l->data;
		}
	return NULL;
}
   
static void
volume_file_change(void *data, Ecore_File_Monitor *fmon, Ecore_File_Event ev, const char *path)
{
	if (volumes_load_timer) ecore_timer_del(volumes_load_timer);
	volumes_load_timer = ecore_timer_add(SCANDELAY, volume_timer, NULL);
}

static Eina_Bool
volume_timer(void *data)
{
	/* connect to database.
	 * add idler handler to do the database query - no need for prog bar.
	 * dump results into items.
	 */
	char buf[4096];
	
	snprintf(buf, sizeof(buf), "%s/media.db", config);
	
/* 	Database* db = database_new(buf); */
/* 	if (db) */
/* 		{ */
/* 			Scan *s; */
			
/* 			s = calloc(1, sizeof(Scan)); */
/* 			s->vol = 0; */
/* 			s->db = db; */
/* 			s->timer = ecore_timer_add(SCANSPEED, volume_idler, s); */
/* 			scans = evas_list_append(scans, s); */
/* 		} */
	
/*
	volume_load();
	volumes_load_timer = NULL;
	if (volumes_file_mon) ecore_file_monitor_del(volumes_file_mon);
	snprintf(buf, sizeof(buf), "%s/volumes", config);
	volumes_file_mon = ecore_file_monitor_add(buf, volume_file_change, NULL);
*/
	return EINA_FALSE;
}

static int
volume_idler(void *data)
{
	Scan* s;
	Eina_List* list;
	/*	Database* db; */
	
/* 	s = data; */
/* 	list = database_video_files_get(s->db, 0, 0); */
/* 	s->items = list; */
/* 	items = s->items; */
	
/* 	ecore_event_add(VOLUME_SCAN_STOP, strdup("db"), NULL, NULL); */
	
/* 	database_free(s->db); */
/* 	s->db = 0; */
	
	/*
	Scan *s;
	DIR *dp;
	struct dirent *de;
	char buf[4096];
   
	s = data;
	if (!s->dirstack)
		{
			ecore_event_add(VOLUME_SCAN_START, strdup(s->vol), NULL, NULL);
			snprintf(s->curdir, sizeof(s->curdir), "%s", s->vol);
			dp = opendir(s->curdir);
			if (dp) s->dirstack = eina_list_append(s->dirstack, dp);
		}
	if (!s->dirstack)
		{
			ecore_event_add(VOLUME_SCAN_STOP, strdup(s->vol), NULL, NULL);
			volume_items_sort(s);
			s->timer = NULL;
			return 0;
		}
	dp = eina_list_data(eina_list_last(s->dirstack));
	if (!dp)
		{
			ecore_event_add(VOLUME_SCAN_STOP, strdup(s->vol), NULL, NULL);
			volume_items_sort(s);
			s->timer = NULL;
			return 0;
		}
	de = readdir(dp);
	if (de)
		{
			if (de->d_name[0] != '.')
				{
					char *link;
	     
					snprintf(buf, sizeof(buf), "%s/%s", s->curdir, de->d_name);
					link = ecore_file_readlink(buf);
					if (link) free(link);
					else
						{
							Volume_Item *vi;
		       
		  if (ecore_file_is_dir(buf))
		    {
		       ecore_event_add(VOLUME_SCAN_GO, strdup(s->vol), NULL, NULL);
		       vi = volume_dir_scan(buf);
		       if (vi) s->items = eina_list_append(s->items, vi);
		       dp = opendir(buf);
		       if (dp)
			 {
			    snprintf(s->curdir, sizeof(s->curdir), "%s", buf);
			    s->dirstack = eina_list_append(s->dirstack, dp);
			 }
		    }
		  else
		    {
		       ecore_event_add(VOLUME_SCAN_GO, strdup(s->vol), NULL, NULL);
		       vi = volume_file_scan(buf);
		       if (vi)
			 {
			    Eina_List *l;
			    int exists;
			    
											exists = 0;
											for (l = items; l; l = l->next)
												{
													Volume_Item *vi2;
				 
													vi2 = l->data;
													if (!strcmp(vi2->rpath, vi->rpath))
														{
															exists = 1;
															break;
														}
												}
											if (!exists)
												{
													for (l = s->items; l; l = l->next)
														{
															Volume_Item *vi2;
				      
				      vi2 = l->data;
				      if (!strcmp(vi2->rpath, vi->rpath))
					{
				      printf("%s == %s\n", vi2->rpath, vi->rpath);
					   exists = 1;
					   break;
					}
				   }
			      }
			    if (exists)
			      {
				 if 
				   (!strcmp(vi->type, "video"))
				   video_count--;
				 else if
				   (!strcmp(vi->type, "audio"))
				   audio_count--;
				 else if 
				   (!strcmp(vi->type, "photo"))
				   photo_count--;
				 ecore_event_add(VOLUME_TYPE_DEL, strdup(vi->path), NULL, NULL);
				 free(vi->path);
				 free(vi->rpath);
				 free(vi->name);
				 if (vi->genre) eina_stringshare_del(vi->genre);
				 free(vi);
			      }
			    else
			      s->items = eina_list_append(s->items, vi);
			 }
		    }
	       }
	  }
     }
   else
     {
	char *p;
	
			closedir(dp);
			s->dirstack = evas_list_remove(s->dirstack, dp);
			p = strrchr(s->curdir, '/');
			if (p) *p = 0;
		}
	if (!s->dirstack)
		{
			ecore_event_add(VOLUME_SCAN_STOP, strdup(s->vol), NULL, NULL);
			volume_items_sort(s);
			s->timer = NULL;
			return 0;
		}
	s->timer = ecore_timer_add(SCANSPEED, volume_idler, s);
	*/
	return 0;
}

static Volume_Item *
volume_file_scan(char *file)
{
	Volume_Item *vi;
	char *ext;

	vi = calloc(1, sizeof(Volume_Item));
	vi->path = strdup(file);
	vi->rpath = ecore_file_realpath(vi->path);
	//printf("%s -> %s\n", vi->path, vi->rpath);
	if (!vi->rpath)
		{
			free(vi->path);
			free(vi);
			return NULL;
		}
	ext = strrchr(vi->path, '.');
	if (!ext)
		{
			free(vi->path);
			free(vi->rpath);
			free(vi);
			return NULL;
		}
	ext++;
	if 
		((!strcasecmp(ext, "avi")) || (!strcasecmp(ext, "mov"))  || 
		 (!strcasecmp(ext, "mpg")) || (!strcasecmp(ext, "mpeg")) || 
		 (!strcasecmp(ext, "vob")) || (!strcasecmp(ext, "wmv"))  ||
		 (!strcasecmp(ext, "asf")) || (!strcasecmp(ext, "mng"))  ||
		 (!strcasecmp(ext, "3gp")) || (!strcasecmp(ext, "wmx"))  ||
		 (!strcasecmp(ext, "wvx")) || (!strcasecmp(ext, "mp4"))  ||
		 (!strcasecmp(ext, "mpe")) || (!strcasecmp(ext, "qt"))   ||
		 (!strcasecmp(ext, "fli")) || (!strcasecmp(ext, "dv"))   ||
		 (!strcasecmp(ext, "asx")) || (!strcasecmp(ext, "wm"))   ||
		 (!strcasecmp(ext, "lsf")) || (!strcasecmp(ext, "movie") ||
																	 (!strcasecmp(ext, "mkv")))
		 ) 
		{
			vi->type = "video";
			video_count++;
		}
	else if 
		((!strcasecmp(ext, "mp3")) || (!strcasecmp(ext, "ogg"))  ||
		 (!strcasecmp(ext, "aac")) || (!strcasecmp(ext, "wav"))
		 )
		{
			vi->type = "audio";
			audio_count++;
		}
	else if 
		((!strcasecmp(ext, "jpg")) || (!strcasecmp(ext, "jpeg"))  ||
		 (!strcasecmp(ext, "jpe")) || (!strcasecmp(ext, "jfif"))
		 )
		{
			vi->type = "photo";
			photo_count++;
		}
	else
		{
			free(vi->path);
			free(vi);
			return NULL;
		}
	/* FIXME: get name and genre from local "database" first - if this doesn't
	 * hold the info we want - then invent it from the filesystem */
	
	/* 1. look for full path in db */
	/* 2. look for just filename in db */
   
	/* invent the data */
	if (!vi->name)
		{
			const char *f;
			/* guess name from filename minus extension with _ converted to space */
			f = ecore_file_file_get(file);
			if (f)
				{
					char *c;
	     
					vi->name = strdup(f);
					c = strrchr(vi->name, '.');
					if (c) *c = 0;
					for (c = vi->name; *c; c++)
						{
							switch(*c)
								{
								case('.'):
								case('_'): { *c = ' '; break; }
								}
						}
				}
		}
	if (!vi->genre)
		{
			char *f;
			/* guess genre from parent directory name with _ converted to space */
			f = ecore_file_dir_get(file);
			if (f)
				{
					char *c, *ff;
					Eina_List* ptr = volumes;
					
					/* loop through all the directories we're indexing. */
					for (; ptr; ptr = ptr->next)
						{
							int dir_len = strlen(ptr->data);
							const char* vol_root = (const char*)ptr->data;
							
							/* see if our dir is a match */
							if (! strncmp(vol_root, f, dir_len))
								{
									
									if (strlen(f) == dir_len)
										{
											/* well, crap, we've got the same name.
											 * we need to go back one dir.
											 */
											char* genre_start = f + strlen(f);
											
											while (genre_start != f && *genre_start !='/') { --genre_start; }
											if (*genre_start == '/') { ++genre_start; }
											
											vi->genre = eina_stringshare_add(genre_start);
										}
									else
										{
											/* so we matched the directory to the first portion
											 * of the filename 
											 * verify
											 */
											if (vol_root[dir_len -1] == f[dir_len -1])
												{
													char buf[4096];
													char* it = f + dir_len;
													int i =0;
													
													while(*it=='/' && it != 0) { ++it; }
													
													for(;*it != 0 && *it != '/'; ++i,++it)
														{
															switch(*it)
																{
																	/* translate the name. */
																case '_':
																case '.':
																	{ buf[i] = ' '; break; }
																default: { buf[i] = *it; break; }
																}
														}
													buf[i] = 0;
													
													vi->genre = eina_stringshare_add(buf);
												}
										}
								}
						}
					
					if (! vi->genre)
						{
							vi->genre = eina_stringshare_add("Unknown");
						}
					free(f);
				}
			else
				vi->genre = eina_stringshare_add("Unknown");
			
			//printf("genre=%s\n", vi->genre);
		}
	ecore_event_add(VOLUME_TYPE_ADD, strdup(vi->path), NULL, NULL);
	/* FIXME: check last played and play count db entry */
	return vi;
}

static Volume_Item *
volume_dir_scan(char *dir)
{
	Volume_Item *vi;
	const char *f;
   
	vi = calloc(1, sizeof(Volume_Item));
	vi->path = strdup(dir);
	vi->rpath = ecore_file_realpath(vi->path);
	if (!vi->rpath)
		{
			free(vi->path);
			free(vi);
			return NULL;
		}
	vi->type = "directory";
	f = ecore_file_file_get(dir);
	if (f)
		{
			char *c;
	
			vi->name = strdup(f);
			for (c = vi->name; *c; c++)
				{
					if (*c == '_') *c = ' ';
				}
		}
	return vi;
}

static int
volume_item_sort(const void *d1, const void *d2)
{
	const Volume_Item *vi1;
	const Volume_Item *vi2;
   
	vi1 = d1;
	vi2 = d2;
	return strcmp(vi1->path, vi2->path);
}

static void
volume_items_sort(Scan *s)
{
   Eina_List *l;
   
	s->items = eina_list_sort(s->items, eina_list_count(s->items),
														volume_item_sort);
	for (l = s->items; l; l = l->next)
		items = eina_list_append(items, l->data);
	items = eina_list_sort(items, eina_list_count(items),
												 volume_item_sort);
}
