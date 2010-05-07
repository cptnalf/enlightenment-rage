/* filename: video_preview.h
 *  chiefengineer
 *  Thu May 06 01:20:35 PDT 2010
 */

#ifndef _VIDEO_PREVIEW_H
#define _VIDEO_PREVIEW_H

#include "e.h"
#include "volume.h"

void video_preview_set(const Volume_Item* vi);
void video_preview_activate();
void video_preview_deactivate();
void video_preview_destroy();

#endif
