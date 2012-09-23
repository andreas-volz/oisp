MAINTAINERCLEANFILES = Makefile.in
SUBDIRS = images fonts 

NAME = default

EDJE_CC = @edje_cc@
EDJE_FLAGS = \
	-v \
	-id $(top_srcdir)/data/themes/$(NAME)/images \
	-fd $(top_srcdir)/data/themes/$(NAME)/fonts \
	@EDJE_DEF@

filesdir = $(pkgdatadir)/themes
files_DATA = enigator_$(NAME).edj

EXTRA_DIST = enigator_$(NAME).edc \
	     pointer.edc \
	     cursor.edc 

enigator_$(NAME).edj: Makefile enigator_$(NAME).edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/$(NAME)/enigator_$(NAME).edc \
	$(top_builddir)/data/themes/$(NAME)/enigator_$(NAME).edj

clean-local:
	rm -f *.edj

