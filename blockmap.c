/*- blockmap.c --------------------------------------------------------------*

 Node builder for DOOM levels (c) 1998 Colin Reed, version 3.0 (dos extended)

 Credit to:-

 Raphael Quinet (A very small amount of code has been borrowed from DEU).

 Matt Fell for the doom specs.

 Lee Killough for performance tuning, support for multilevel wads, special
 effects, and wads with lumps besides levels.

 Also, the original idea for some of the techniques where also taken from the
 comment at the bottom of OBJECTS.C in DEU, and the doc by Matt Fell about
 the nodes.

 Use this code for your own editors, but please credit me.

*---------------------------------------------------------------------------*/

/*
 * Separated from bsp.c 2000/08/26 by Colin Phipps
 * 
 * New blockmap algorithm 2000/09/09: Copyright (c) 2000 by Colin Phipps
 * <cph@lxdoom.linuxgames.com>
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 */

#include "structs.h"
#include "bsp.h"

/*--------------------------------------------------------------------------*/

static int
IsLineDefInside(int ldnum, int xmin, int ymin, int xmax, int ymax)
{
	int             x1 = vertices[linedefs[ldnum].start].x;
	int             y1 = vertices[linedefs[ldnum].start].y;
	int             x2 = vertices[linedefs[ldnum].end].x;
	int             y2 = vertices[linedefs[ldnum].end].y;
	int             count = 2;

	for (;;)
		if (y1 > ymax) {
			if (y2 > ymax)
				return (FALSE);
			x1 = x1 + (x2 - x1) * (double) (ymax - y1) / (y2 - y1);
			y1 = ymax;
			count = 2;
		} else if (y1 < ymin) {
			if (y2 < ymin)
				return (FALSE);
			x1 = x1 + (x2 - x1) * (double) (ymin - y1) / (y2 - y1);
			y1 = ymin;
			count = 2;
		} else if (x1 > xmax) {
			if (x2 > xmax)
				return (FALSE);
			y1 = y1 + (y2 - y1) * (double) (xmax - x1) / (x2 - x1);
			x1 = xmax;
			count = 2;
		} else if (x1 < xmin) {
			if (x2 < xmin)
				return (FALSE);
			y1 = y1 + (y2 - y1) * (double) (xmin - x1) / (x2 - x1);
			x1 = xmin;
			count = 2;
		} else {
			int             t;
			if (!--count)
				return (TRUE);
			t = x1;
			x1 = x2;
			x2 = t;
			t = y1;
			y1 = y2;
			y2 = t;
		}
}

/*- Create blockmap --------------------------------------------------------*/

void 
CreateBlockmap_old(const bbox_t bbox)
{
	struct Block    blockhead;
	short int      *blockptrs;
	short int      *blocklists = NULL;
	long            blockptrs_size;

	long            blockoffs = 0;
	int             x, y, n;
	int             blocknum = 0;

	Verbose("Creating Blockmap... ");

	blockhead.minx = bbox[BB_LEFT] & -8;
	blockhead.miny = bbox[BB_BOTTOM] & -8;
	blockhead.xblocks = ((bbox[BB_RIGHT] - (bbox[BB_LEFT] & -8)) / 128) + 1;
	blockhead.yblocks = ((bbox[BB_TOP] - (bbox[BB_BOTTOM] & -8)) / 128) + 1;

	blockptrs_size = (blockhead.xblocks * blockhead.yblocks) * 2;
	blockptrs = GetMemory(blockptrs_size);

	for (y = 0; y < blockhead.yblocks; y++) {
		for (x = 0; x < blockhead.xblocks; x++) {
			progress();

			blockptrs[blocknum] = (blockoffs + 4 + (blockptrs_size / 2));
			swapshort((unsigned short *) blockptrs + blocknum);

			blocklists = ResizeMemory(blocklists, ((blockoffs + 1) * 2));
			blocklists[blockoffs] = 0;
			blockoffs++;
			for (n = 0; n < num_lines; n++) {
				if (IsLineDefInside(n, (blockhead.minx + (x * 128)), (blockhead.miny + (y * 128)), (blockhead.minx + (x * 128)) + 127, (blockhead.miny + (y * 128)) + 127)) {
					/*
					 * printf("found line %d in block
					 * %d\n",n,blocknum);
					 */
					blocklists = ResizeMemory(blocklists, ((blockoffs + 1) * 2));
					blocklists[blockoffs] = n;
					swapshort((unsigned short *) blocklists + blockoffs);
					blockoffs++;
				}
			}
			blocklists = ResizeMemory(blocklists, ((blockoffs + 1) * 2));
			blocklists[blockoffs] = -1;
			swapshort((unsigned short *) blocklists + blockoffs);
			blockoffs++;

			blocknum++;
		}
	}
	{
		long            blockmap_size = blockoffs * 2;
		char           *data = GetMemory(blockmap_size + blockptrs_size + 8);
		memcpy(data, &blockhead, 8);
		swapshort((unsigned short *) data + 0);
		swapshort((unsigned short *) data + 1);
		swapshort((unsigned short *) data + 2);
		swapshort((unsigned short *) data + 3);
		memcpy(data + 8, blockptrs, blockptrs_size);
		memcpy(data + 8 + blockptrs_size, blocklists, blockmap_size);
		free(blockptrs);
		free(blocklists);
		add_lump("BLOCKMAP", data, blockmap_size + blockptrs_size + 8);
	}
	Verbose("done.\n");
}
