# $Id: Makefile.am,v 1.11 2001/11/17 15:42:46 cph Exp $

SUBDIRS = test-wads DOS Win32

EXTRA_DIST = visplane.txt README

DOCS = README AUTHORS COPYING NEWS visplane.txt

docdir = $(datadir)/doc/@PACKAGE@
doc_DATA = $(DOCS)

bin_PROGRAMS=bsp

bsp_SOURCES=blockmap.c bsp.c endian.c funcs.c level.c makenode.c picknode.c bsp.h structs.h

blockmap.c bsp.c funcs.c endian.c level.c makenode.c picknode.c : bsp.h structs.h config.h

dos	:	$(DOCS)
	-mkdir dos-dist
	for fn in $(DOCS); do perl -p -e 's/$$/\r/' < $$fn > `echo dos-dist/$$fn | perl -p -e 's/(.txt)?$$/.txt/'`; done

README	:	index.html
	lynx -dump index.html > README

