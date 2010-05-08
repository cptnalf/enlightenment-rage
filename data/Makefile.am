4AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = edje_cc
EDJE_FLAGS = -v \
	-id $(top_srcdir)/data/images \
	 -fd $(top_srcdir)/data/fonts

filesdir = $(datadir)/rage

files_DATA = \
default.edj

IMG_BG= \
	images/01875_horseheadnebula_1920x1080.png \
	images/bg-lcars.png \
	images/bg_over.png \
	images/bg.png

IMG_ICONS= \
images/audio.png \
images/audios.png \
images/book.png \
images/bug.png \
images/busy-1.png \
images/busy-2.png \
images/busy-3.png \
images/busy-4.png \
images/busy-5.png \
images/busy-6.png \
images/busy-7.png \
images/busy-8.png \
images/busy-9.png \
images/config.png \
images/dvd.png \
images/dvd_rip.png \
images/dvd_watch.png \
images/favorites.png \
images/genre.png \
images/photo.png \
images/photos.png \
images/resume.png \
images/update.png \
images/video.png \
images/videos.png

IMG_MENUITEM= \
images/mitem2-dp.png \
images/mitem2-dy.png \
images/mitem2-lb.png \
images/mitem2-lcars.png \
images/mitem2-ly.png \
images/mitem2.png \
images/mitem-dp.png \
images/mitem-dy.png \
images/mitem-lb.png \
images/mitem-lcars.png \
images/mitem-ly.png \
images/mitem.png

IMG_OTHER= \
images/miniover.png \
images/selshadow.png \
images/shad_circ.png

EDC_SRC= \
	default.edc \
	menuitems.edc \
	mini.edc \
	videopreview.edc

EXTRA_DIST = \
	$(IMG_BG) \
	$(IMG_ICONS) \
	$(IMG_MENUITEM) \
	$(IMG_OTHER) \
	$(EDC_SRC)

default.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/default.edc \
	$(top_builddir)/data/default.edj

clean-local:
	rm -f *.edj
