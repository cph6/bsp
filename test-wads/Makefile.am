# $Id: Makefile.am,v 1.1 2000/08/25 21:04:37 cph Exp $

BSP = ../bsp
WADS = bsp23bug.wad conduits.wad transdor.wad visarea.wad
UNWADS = bsp23bug.unbuiltwad conduits.unbuiltwad transdor.unbuiltwad visarea.unbuiltwad
MD5 = md5

EXTRA_DIST = $(UNWADS)

%.wad	:	%.unbuiltwad
	$(BSP) -o $@ $<

test	:	$(WADS)
	$(MD5) $(WADS) | diff MD5SUMS -

