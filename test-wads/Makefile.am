# $Id: Makefile.am,v 1.5 2001/07/21 12:47:30 cph Exp $

BSP = ../bsp
WADS = bsp23bug.wad conduits.wad transdor.wad visarea.wad
UNWADS = bsp23bug.unbuiltwad conduits.unbuiltwad transdor.unbuiltwad visarea.unbuiltwad
MD5 = md5
DOOM_WAD=/usr/local/share/games/doom/doom.wad
TIME=time

EXTRA_DIST = conduits.unbuiltwad transdor.unbuiltwad visarea.unbuiltwad MD5SUMS

%.wad	:	%.unbuiltwad
	$(TIME) $(BSP) -o $@ $<

doom.wad	:	$(DOOM_WAD)
	$(TIME) $(BSP) -o $@ $(DOOM_WAD)

test	:	$(WADS) doom.wad
	$(MD5) $(WADS) doom.wad | diff MD5SUMS -

clean	:
	rm -f $(WADS) doom.wad

