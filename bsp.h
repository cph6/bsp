/*- BSP.H ------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <assert.h>

#if defined(MSDOS) || defined(__MSDOS__)
#include <dos.h>
#endif
#ifdef __TURBOC__
#include <alloc.h>
#endif

/*- boolean constants ------------------------------------------------------*/

#define TRUE			1
#define FALSE			0

/*- The function prototypes ------------------------------------------------*/

void ProgError( char *, ...);
void *GetMemory(size_t);
void *ResizeMemory(void *, size_t);
unsigned ComputeAngle(int,int);

#undef max
#define max(a,b) (((a)>(b))?(a):(b))

/*- print a resource name by printing first 8 characters --*/

#define Printname(dir) printf("%-8.8s",(dir)->name)

/*- Global functions -------------------------------------------------------*/

/* bsp.c */
void progress(void);
void FindLimits(struct Seg *);
int SplitDist(struct Seg *ts);

/* picknode.c */
int DoLinesIntersect();
void ComputeIntersection(short int *outx,short int *outy);

/*- Global variables -------------------------------------------------------*/

/* bsp.c */
struct lumplist *FindDir(const char *);

/* level.c */
extern struct Vertex *vertices;
extern long num_verts;

extern struct LineDef *linedefs;
extern long num_lines;

extern struct SideDef *sidedefs;
extern long num_sides;

extern struct Sector *sectors;
extern long num_sects;

extern struct SSector *ssectors;
extern long num_ssectors;

extern struct Pseg *psegs;
extern long num_psegs;

extern long num_nodes;

extern short lminx, lmaxx, lminy, lmaxy;

extern unsigned char *SectorHits;

extern long psx,psy,pex,pey,pdx,pdy;
extern long lsx,lsy,lex,ley;

/* makenode.c */
extern struct Node *CreateNode(struct Seg *);

/* picknode.c */
extern int factor;

struct Seg *PickNode_traditional(struct Seg *);
struct Seg *PickNode_visplane(struct Seg *);
extern struct Seg *(*PickNode)(struct Seg *);

/*------------------------------- end of file ------------------------------*/
