# $Id: Makefile.am,v 1.6 2000/08/28 12:59:12 cph Exp $

SUBDIRS = test-wads DOS Win32

EXTRA_DIST = visplane.txt

bin_PROGRAMS=bsp

bsp_SOURCES=bsp.c endian.c funcs.c level.c makenode.c picknode.c bsp.h structs.h

bsp.c funcs.c endian.c level.c makenode.c picknode.c : bsp.h structs.h config.h

config.h	:	configure.in
	./configure

