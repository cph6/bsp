# $Id: Makefile.am,v 1.3 2000/08/28 12:59:20 cph Exp $

BSP = ../bsp
WADS = bsp23bug.wad conduits.wad transdor.wad visarea.wad
UNWADS = bsp23bug.unbuiltwad conduits.unbuiltwad transdor.unbuiltwad visarea.unbuiltwad
MD5 = md5
DOOM_WAD=/usr/local/share/games/doom/doom.wad
TIME=time

EXTRA_DIST = $(UNWADS) MD5SUMS

%.wad	:	%.unbuiltwad
	$(TIME) $(BSP) -o $@ $<

doom.wad	:	$(DOOM_WAD)
	$(TIME) $(BSP) -o $@ $(DOOM_WAD)

test	:	$(WADS) doom.wad
	$(MD5) $(WADS) doom.wad | diff MD5SUMS -

clean	:	$(WADS) doom.wad

