AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src/bin \
@EVAS_CFLAGS@ @ECORE_CFLAGS@ @EDJE_CFLAGS@ @ELEMENTARY_CFLAGS@ \
@EMOTION_CFLAGS@ @SQLITE3_CFLAGS@

bin_PROGRAMS = rage rage_thumb raged

rage_SOURCES = \
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
gen_thumb.c \
gen_thumb.h \
video_lib.c \
video_lib.h

include $(top_srcdir)/src/modules/input/files.mak
include $(top_srcdir)/src/modules/activity/dvb/files.mak
include $(top_srcdir)/src/modules/activity/videos/files.mak
include $(top_srcdir)/src/modules/activity/config/files.mak

rage_SOURCES += $(INPUT_SRCS)
rage_SOURCES += $(VIDEO_SRCS)
rage_SOURCES += $(DVB_SRCS)
rage_SOURCES += $(CONFIG_SRCS)

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
