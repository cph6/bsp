# $Id: Makefile.am,v 1.4 2000/08/27 10:28:45 cph Exp $

SUBDIRS = test-wads

bin_PROGRAMS=bsp

bsp_SOURCES=bsp.c funcs.c level.c makenode.c picknode.c bsp.h structs.h

bsp.c funcs.c level.c makenode.c picknode.c : bsp.h structs.h config.h

config.h	:	configure.in
	./configure

