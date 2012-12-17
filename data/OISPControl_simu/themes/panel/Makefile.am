MAINTAINERCLEANFILES = Makefile.in
SUBDIRS = images

NAME = panel

EDJE_CC = @edje_cc@
EDJE_FLAGS = \
	-v \
	-id $(top_srcdir)/data/OISPControl_simu/themes/$(NAME)/images \
	-fd $(top_srcdir)/data/OISPControl_simu/themes/$(NAME)/fonts \
	@EDJE_DEF@

filesdir = $(pkgdatadir)/OISPControl_simu/themes
files_DATA = OISPControl_simu_$(NAME).edj

EXTRA_DIST = OISPControl_simu_$(NAME).edc

OISPControl_simu_$(NAME).edj: Makefile OISPControl_simu_$(NAME).edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/OISPControl_simu/themes/$(NAME)/OISPControl_simu_$(NAME).edc \
	$(top_builddir)/data/OISPControl_simu/themes/$(NAME)/OISPControl_simu_$(NAME).edj

clean-local:
	rm -f *.edj

