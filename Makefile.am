# $Id: Makefile.am,v 1.8 2000/08/28 16:38:25 cph Exp $

SUBDIRS = test-wads DOS Win32

EXTRA_DIST = visplane.txt README

DOCS = README AUTHORS COPYING NEWS visplane.txt

docdir = $(datadir)/doc/@PACKAGE@
doc_DATA = $(DOCS)

bin_PROGRAMS=bsp

bsp_SOURCES=bsp.c endian.c funcs.c level.c makenode.c picknode.c bsp.h structs.h

bsp.c funcs.c endian.c level.c makenode.c picknode.c : bsp.h structs.h config.h

config.h	:	configure.in
	./configure

dos	:	$(DOCS)
	-mkdir dos-dist
	for fn in $(DOCS); do perl -p -e 's/$$/\r/' < $$fn > dos-dist/$$fn; done

README	:	index.html
	lynx -dump index.html > README

