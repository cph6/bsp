# $Id: Makefile.am,v 1.5 2000/08/27 20:38:18 cph Exp $

SUBDIRS = test-wads

bin_PROGRAMS=bsp

bsp_SOURCES=bsp.c endian.c funcs.c level.c makenode.c picknode.c bsp.h structs.h

bsp.c funcs.c endian.c level.c makenode.c picknode.c : bsp.h structs.h config.h

config.h	:	configure.in
	./configure

