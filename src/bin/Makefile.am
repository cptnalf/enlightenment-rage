AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src/bin \
@EVAS_CFLAGS@ @ECORE_CFLAGS@ @EDJE_CFLAGS@ @ELEMENTARY_CFLAGS@ \
@EMOTION_CFLAGS@ @SQLITE3_CFLAGS@

bin_PROGRAMS = rage rage_thumb raged

# ACTIVITES_SRCS= \
# $(top_srcdir)/src/modules/activities/dvb.c \
# $(top_srcdir)/src/modules/activities/dvb.h \
# $(top_srcdir)/src/modules/activities/video.c \
# $(top_srcdir)/src/modules/activities/video.h

INPUT_SRCS= \
$(top_srcdir)/src/modules/input/kbd.c \
$(top_srcdir)/src/modules/input/basekeymap.h \
$(top_srcdir)/src/modules/input/videokeymap.h

# VFS_SRCS= \
# $(top_srcdir)/src/modules/vfs/database.c \
# $(top_srcdir)/src/modules/vfs/database.h \
# $(top_srcdir)/src/modules/vfs/vfs_db.c

#$(ACTIVITIES_SRCS) \
#$(VFS_SRCS) \
#

rage_SOURCES = \
$(INPUT_SRCS) \
main.c \
main.h \
layout.c \
layout.h \
background.c \
background.h \
volume.c \
volume.h \
status.c \
status.h \
menu.c \
menu.h \
mini.c \
mini.h \
sha1.c \
sha1.h \
minivid.c \
minivid.h \
e.h \
e_box.c \
e_box.h \
e_table.c \
e_table.h \
e_layout.c \
e_layout.h \
e_flowlayout.c \
e_flowlayout.h \
conf_options.c \
conf_options.h \
video_preview.h \
video_preview.c \
view_list.c \
view_list.h \
vfs.h \
module.c \
module.h \
input.h \
input.c \
database.c \
database.h \
video.c \
video.h \
dvb.c \
dvb.h \
gen_thumb.c \
gen_thumb.h \
video_lib.c \
video_lib.h

rage_LDADD = @my_libs@ \
@EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@ @EMOTION_LIBS@ @ELEMENTARY_LIBS@ \
@SQLITE3_LIBS@

rage_thumb_SOURCES = \
rage_thumb.c

rage_thumb_LDADD = @my_libs@ @EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@ @EMOTION_LIBS@

raged_SOURCES = \
raged_main.c \
sha1.c \
sha1.h

raged_LDADD = @my_libs@ @EVAS_LIBS@ @ECORE_LIBS@ @EMOTION_LIBS@ @EET_LIBS@
