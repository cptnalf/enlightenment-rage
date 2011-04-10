/* filename: metadata.h
   chiefengineer
   Fri Apr 08 19:54:28  2011
*/

#ifndef _METADATA_H_
#define _METADATA_H_

#include "volume.h"

typedef struct _Metadata Metadata;

struct _Metadata
{
	int id;
	const char* show;
	int season;
	int episode;
	char* title;
	char* synopsis;
	double rating;
	const char* creator;
	const char* airDate;
	const char* writer;
	const char* studio;
};	
	

Metadata* metadata_new(const int id, 
													 const char* show,
													 const int season, const int episode,
													 const char* title, const char* synopsis,
													 const double rating,
													 const char* creator,
													 const char* airDate,
													 const char* writer,
													 const char* studio);
void metadata_free(void* data);

Metadata* metadata_get(const Volume_Item* item);

#endif
