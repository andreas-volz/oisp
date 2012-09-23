## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src data

oispdocdir = ${prefix}/doc/oisp
oispdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

EXTRA_DIST = $(oispdoc_DATA)

ACLOCAL_AMFLAGS = -I m4

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

