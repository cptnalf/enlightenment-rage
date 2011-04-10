/* filename: metadata.c
   chiefengineer
   Fri Apr 08 19:55:38  2011
*/
#include <Eina.h>
#include <Ecore_File.h>
#include <string.h>

#include "metadata.h"

#include <sqlite3.h>

static char* _char_escape_strdup(const char* text);

Metadata* metadata_new(const int id, 
													 const char* show,
													 const int season, const int episode,
													 const char* title, const char* synopsis,
													 const double rating,
													 const char* creator,
													 const char* airDate,
													 const char* writer,
													 const char* studio)
{
	Metadata* item;
	
	item = calloc(1, sizeof(Metadata));
	if (item)
		{
			item->id = id;
			item->season = season;
			item->episode = episode;
			item->rating = rating;
			
			if (title) { item->title = strdup(title); }
			if (synopsis) 
				{
					/* need to escape this crap. */
					item->synopsis = _char_escape_strdup(synopsis); 
				}
			if (show) { item->show = eina_stringshare_add(show); }
			if (creator) { item->creator = eina_stringshare_add(creator); }
			if (airDate) { item->airDate = eina_stringshare_add(airDate); }
			if (writer) { item->writer = eina_stringshare_add(writer); }
			if (studio) { item->studio = eina_stringshare_add(studio); }
		}
	
	return item;
}

void metadata_free(void* data)
{
	Metadata* item = (Metadata*)data;
	if (item)
		{
			if (item->title) { free(item->title); }
			if (item->synopsis) { free(item->synopsis); }
			if (item->creator) { eina_stringshare_del(item->creator); }
			if (item->airDate) { eina_stringshare_del(item->airDate); }
			if (item->writer)  { eina_stringshare_del(item->writer); }
			if (item->studio)  { eina_stringshare_del(item->studio); }
			if (item->show)    { eina_stringshare_del(item->show); }
			
			free(item);
		}
}

Metadata* metadata_get(const Volume_Item* item)
{
	Metadata* ep = NULL;
	sqlite3* db = NULL;
	int result;
	
	result = sqlite3_open("videodata.db", &db);
	if (result)
		{
			fprintf(stderr, "error: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			db = 0;
		}
	
	if (db)
		{
			{
				int rows;
				int cols;
				char** tbl_results;
				int pos = 0;
				char* error_msg;
				
 /*
  CREATE VIEW episodeview as
select
episode.*
,files.strFileName as strFileName
,path.strPath as strPath
,files.playCount as playCount
,files.lastPlayed as lastPlayed
,tvshow.c00 as strTitle
,tvshow.c14 as strStudio
,tvshow.idShow as idShow
,tvshow.c05 as premiered
, tvshow.c13 as mpaa
from episode
join files on files.idFile=episode.idFile
join tvshowlinkepisode on episode.idepisode=tvshowlinkepisode.idEpisode
join tvshow on tvshow.idShow=tvshowlinkepisode.idShow
join path on files.idPath=path.idPath
;
  */
				/* episode id, title, desc/synopsis, rating, creator,
				 * air date, writer (?), season, episode
				 */
 				const char* query_base =
					"SELECT "
					" idEpisode "
					", c00 as epTitle"
					", c01 as synopsis"
					", c03 as rating"
					", c04 as creator"
					", c05 as airDate"
					", c10 as writer"
					", c12 as season"
					", c13 as ep"
					", strFileName, strPath, strTitle, strStudio, idShow, premiered, mpaa"
					" FROM episodeview"
					" WHERE strFileName = '%q' "
					" limit 1 ;"
					;
				
				char* query;
				const char* filename;
				
				filename = ecore_file_file_get(item->path);
				
				query = sqlite3_mprintf(query_base, filename);
				
				result = sqlite3_get_table(db, query, &tbl_results, &rows, &cols, &error_msg);
				if (result == SQLITE_OK)
					{
						pos += cols;
						if ((rows !=0) && (pos <= (rows * cols)))
							{
								int id;
								double rating;
								int season;
								int episode;
								
								id = atoi(tbl_results[pos + 0]);
								rating = atof(tbl_results[pos + 3]);
								season = atoi(tbl_results[pos + 7]);
								episode = atoi(tbl_results[pos + 8]);
								
								ep =
									metadata_new(id, tbl_results[pos + 11],
															 season, episode,
															 tbl_results[pos + 1],
															 tbl_results[pos + 2],
															 rating, tbl_results[pos + 4],
															 tbl_results[pos + 5], tbl_results[pos + 6],
															 tbl_results[pos + 12]);
							}
					}
				else
					{
						printf("error: %s\n", error_msg);
						sqlite3_free(error_msg);
					}
				sqlite3_free(query);
			}
			sqlite3_close(db);
		}
	
	return ep;
}

static char* _char_escape_strdup(const char* text)
{
	char buf[4096];
	char* ptr = text;
	char* dest = NULL;
	int len = strlen(text);
	int i=0;
	
	if (len > (sizeof(buf) / 2))
		{
			
		}
	else
		{
			while (*ptr)
				{
					switch(*ptr)
						{
						case ('&') :
							{
								buf[i] = '&';
								buf[++i] = 'a';
								buf[++i] = 'm';
								buf[++i] = 'p';
								buf[++i] = ';';
								break;
							}
						default:
							{
								buf[i] = *ptr;
								break;
							}
						}
					
					++ptr;
					++i;
				}
			buf[i] = '\0';
			
			dest = strdup(buf);
		}
	
	return dest;
}
