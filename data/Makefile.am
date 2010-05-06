4AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = edje_cc
EDJE_FLAGS = -v -id $(top_srcdir)/data -fd $(top_srcdir)/data

filesdir = $(datadir)/rage

files_DATA = \
default.edj

EXTRA_DIST = \
default.edc \
menuitems.edc \
mini.edc \
videopreview.edc \
01875_horseheadnebula_1920x1080.png \
audio.png \
audios.png \
bg-lcars.png \
bg_over.png \
bg.png \
book.png \
bug.png \
busy-1.png \
busy-2.png \
busy-3.png \
busy-4.png \
busy-5.png \
busy-6.png \
busy-7.png \
busy-8.png \
busy-9.png \
config.png \
dvd.png \
dvd_rip.png \
dvd_watch.png \
favorites.png \
genre.png \
miniover.png \
mitem2-dp.png \
mitem2-dy.png \
mitem2-lb.png \
mitem2-lcars.png \
mitem2-ly.png \
mitem2.png \
mitem-dp.png \
mitem-dy.png \
mitem-lb.png \
mitem-lcars.png \
mitem-ly.png \
mitem.png \
photo.png \
photos.png \
resume.png \
selshadow.png \
update.png \
video.png \
videos.png 

default.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/default.edc \
	$(top_builddir)/data/default.edj

clean-local:
	rm -f *.edj
