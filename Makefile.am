# $Id: Makefile.am,v 1.7 2000/08/28 15:54:04 cph Exp $

SUBDIRS = test-wads DOS Win32

EXTRA_DIST = visplane.txt README

bin_PROGRAMS=bsp

bsp_SOURCES=bsp.c endian.c funcs.c level.c makenode.c picknode.c bsp.h structs.h

bsp.c funcs.c endian.c level.c makenode.c picknode.c : bsp.h structs.h config.h

config.h	:	configure.in
	./configure

README	:	index.html
	lynx -dump index.html > README

