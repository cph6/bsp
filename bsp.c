/*- BSP.C -------------------------------------------------------------------*

 Node builder for DOOM levels (c) 1998 Colin Reed, version 3.0 (dos extended)

 Performance increased 200% over 1.2x

 Many thanks to Mark Harrison for finding a bug in 1.1 which caused some
 texture align problems when a flipped SEG was split.

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

#include "structs.h"
#include "bsp.h"

/*- Global Vars ------------------------------------------------------------*/

static FILE *outfile;
static char *testwad;
static char *outwad;

static struct directory *direc = NULL;

struct Seg *(*PickNode)(struct Seg *)=PickNode_traditional;
static int visplane;
static int noreject;

static unsigned char pcnt;

static struct lumplist *lumplist,*current_level;

static struct wad_header wad;

/*- Prototypes -------------------------------------------------------------*/

static void OpenWadFile(char *);
static void GetThings(void);
static void GetVertexes(void);
static void GetLinedefs(void);
static void GetSidedefs(void);
static void GetSectors(void);

static struct Seg *CreateSegs();

static int IsItConvex(struct Seg *);

static void ReverseNodes(struct Node *);
static long CreateBlockmap(void);

static int IsLineDefInside(int, int, int, int, int);
static int CreateSSector(struct Seg *);

/*--------------------------------------------------------------------------*/

void progress()
{
	if(!((++pcnt)&31))
		fprintf(stderr,"%c\b","/-\\|"[((pcnt)>>5)&3]);
}

/*- get the directory from a wad file --------------------------------------*/
/* rewritten by Lee Killough to support multiple levels and extra lumps */

static void OpenWadFile(char *filename)
{
 long i;
 register struct directory *dir;
 struct lumplist *levelp=NULL;
 FILE *infile;

 if (!(infile = fopen(filename,"rb")))
   ProgError("Error: Cannot open WAD file %s", filename);

 if (fread(&wad,1,sizeof(wad),infile)!=sizeof(wad) ||
     (wad.type[0]!='I' && wad.type[0]!='P') || wad.type[1]!='W'
     || wad.type[2]!='A' || wad.type[3]!='D')
   ProgError("%s does not appear to be a wad file -- bad magic", filename);

 printf("Opened %cWAD file : %s. %lu dir entries at 0x%lX.\n",
	wad.type[0],filename,wad.num_entries,wad.dir_start);

 direc = GetMemory(sizeof(struct directory) * wad.num_entries);

 fseek(infile,wad.dir_start,0);
 fread(direc,sizeof(struct directory),wad.num_entries,infile);

 current_level=NULL;
 dir = direc;
 for (i=wad.num_entries;i--;dir++)
  {
   register struct lumplist *l=GetMemory(sizeof(*l));
   l->dir=dir;
   l->data=NULL;
   l->next=NULL;
   l->islevel=0;

   if ((dir->name[0]=='E' && dir->name[2]=='M' &&
        dir->name[1]>='1' && dir->name[1]<='9' &&
        dir->name[3]>='1' && dir->name[3]<='9' &&
       (!dir->name[4] || (dir->name[4]>='0' &&
                          dir->name[4]<='9' &&
                         !dir->name[5])  )) || (
        dir->name[0]=='M' && dir->name[1]=='A' &&
        dir->name[2]=='P' && !dir->name[5] &&
        dir->name[3]>='0' && dir->name[3]<='9' &&
        dir->name[4]>='0' && dir->name[4]<='9'))
    {
     dir->length=0;
     l->islevel=1;                                 /* Begin new level */
    }
   else
    {
     if (levelp && (!strncmp(dir->name,"SEGS",8) ||
                    !strncmp(dir->name,"SSECTORS",8) ||
                    !strncmp(dir->name,"NODES",8) ||
                    !strncmp(dir->name,"BLOCKMAP",8) ||
                    (!noreject && !strncmp(dir->name,"REJECT",8)) ||
                    FindDir(dir->name)))
       continue;  /* Ignore these since we're rebuilding them anyway */

     if (levelp && FindDir(dir->name))
      {
       printf("Warning: Duplicate entry \"%-8.8s\" ignored in %-.8s\n",
         dir->name, current_level->dir->name);
       continue;
      }

     if (dir->length)
      {
       l->data=GetMemory(dir->length);
       if (fseek(infile,dir->start,0) ||
           fread(l->data,1,dir->length,infile) != dir->length)
         printf("Warning: Trouble reading wad directory entry \"%-8.8s\" in %-.8s\n",
                dir->name, current_level->dir->name);
      }

     if (levelp && (!strncmp(dir->name,"THINGS",8) ||
         !strncmp(dir->name,"LINEDEFS",8) ||
         !strncmp(dir->name,"SIDEDEFS",8) ||
         !strncmp(dir->name,"VERTEXES",8) ||
         !strncmp(dir->name,"SECTORS",8) ||
         (noreject && !strncmp(dir->name,"REJECT",8))))
      {                       /* Store in current level */
       levelp->next=l;
       levelp=l;
       continue;
      }
    }

    /* Mark end of level and advance one main lump entry */

   if (levelp && !(current_level->data=current_level->next))
     current_level->islevel=0;  /* Ignore oddly formed levels */

   levelp = l->islevel ? l : NULL;

   if (current_level)
     current_level->next=l;
   else
     lumplist=l;
   current_level=l;
  }

 if (levelp)
  {
   if (!(current_level->data=current_level->next))
     current_level->islevel=0;  /* Ignore oddly formed levels */
   current_level->next=NULL;
  }
 fclose(infile);
}

/*- find the pointer to a resource -----------------------*/

struct lumplist *FindDir(const char *name)
{
 struct lumplist *l = current_level;
 while (l && strncmp(l->dir->name,name,8))
   l=l->next;
 return l;
}

/* Add a lump to current level
   by Lee Killough */

void add_lump(const char *name, void *data, size_t length)
{
 struct lumplist *l;
 for (l=current_level;l;l=l->next)
   if (!strncmp(name,l->dir->name,8))
     break;
 if (!l)
  {
   l=current_level;
   while (l->next)
     l=l->next;
   l->next = GetMemory(sizeof(*l));
   l = l->next;
   l->next=NULL;
   l->dir = GetMemory(sizeof(struct directory));
   strncpy(l->dir->name,name,8);
  }
 l->dir->length=length;
 l->islevel=0;
 l->data=data;
}

static struct directory write_lump(struct lumplist *lump)
{
 if (ftell(outfile)!=lump->dir->start || (lump->dir->length &&
   fwrite(lump->data, 1, lump->dir->length, outfile) != lump->dir->length))
   printf("Warning: Consistency check failure writing %-.8s\n", lump->dir->name);
 return *lump->dir;
}

static void sortlump(struct lumplist **link)
{
 static const char *const lumps[10]={"THINGS", "LINEDEFS", "SIDEDEFS",
  "VERTEXES", "SEGS", "SSECTORS", "NODES", "SECTORS", "REJECT", "BLOCKMAP"};
 int i=sizeof(lumps)/sizeof(*lumps)-1;
 struct lumplist **l;
 do
   for (l=link;*l;l=&(*l)->next)
     if (!strncmp(lumps[i],(*l)->dir->name,8))
      {
       struct lumplist *t=(*l)->next;
       (*l)->next=*link;
       *link=*l;
       *l=t;
       break;
      }
 while (--i>=0);
}


void usage(void)
{
 printf("\nThis Node builder was created from the basic theory stated in DEU5 (OBJECTS.C)\n"
        "\nCredits should go to :-\n"
        "Matt Fell      (msfell@aol.com) for the Doom Specs.\n"
        "Raphael Quinet (Raphael.Quinet@eed.ericsson.se) for DEU and the original idea.\n"
        "Mark Harrison  (harrison@lclark.edu) for finding a bug in 1.1x\n"
        "Jim Flynn      (jflynn@pacbell.net) for many good ideas and encouragement.\n"
        "Jan Van der Veken for finding invisible barrier bug.\n"
#ifdef MSDOS
        "\nUsage: BSP [options] input.wad [[-o] <output.wad>]\n"
#else
        "\nUsage: BSP [options] input.wad [-o <output.wad>]\n"
#endif
        "       (If no output.wad is specified, TMP.WAD is written)\n\n"
        "Options:\n\n"
        "  -factor <nnn>  Changes the cost assigned to SEG splits\n"
        "  -vp            Attempts to prevent visplane overflows\n"
        "  -noreject      Does not clobber reject map\n"
       );
 exit(1);
}

static void parse_options(int argc, char *argv[])
{
 static char *fnames[2];
 static const struct {
   const char *option;
   void *var;
   enum {NONE, STRING, INT} arg;
 } tab[]= { {"-vp", &visplane, NONE},
            {"-noreject", &noreject, NONE},
            {"-factor", &factor, INT},
            {"-o", fnames+1, STRING},
          };
 int nf=0;

 while (--argc)
   if (**++argv=='-')
    {
     int i=sizeof(tab)/sizeof(*tab);
     for (;;)
       if (!strcmp(*argv,tab[--i].option))
        {
         if (tab[i].arg==INT)
           if (--argc)
            {
             char *end;
             *(int *) tab[i].var=strtol(*++argv,&end,0);
             if (*end || factor<0)
               usage();
            }
           else
             usage();
         else
	   if (tab[i].arg==STRING)
	     if (--argc)
	       *(char **) tab[i].var = *++argv;
	     else
	       usage();
	   else
	     ++*(int *) tab[i].var;
         break;
        }
       else
         if (!i)
          {
           usage();
           break;
          }
    }
   else
#ifdef MSDOS
     if (nf<2)
#else
     if (nf<1)
#endif
       fnames[nf++]=*argv;
     else
       usage();

 testwad = fnames[0];                          /* Get input name*/

 if (!testwad || factor<0)
   usage();

 outwad = fnames[1] ? fnames[1] : "tmp.wad";   /* Get output name*/
}

/*- Main Program -----------------------------------------------------------*/

int main(int argc,char *argv[])
{
 struct lumplist *lump,*l;
 struct directory *newdirec;

 setbuf(stdout,NULL);

 puts("* Doom BSP node builder ver 3.0 (c) 1998 Colin Reed, Lee Killough *");

 parse_options(argc,argv);

 OpenWadFile(testwad);		/* Opens and reads directory*/

 printf("\nCreating nodes using tunable factor of %d\n",factor);

 if (visplane)
  {
   puts("\nTaking special measures to reduce the chances of visplane overflow");
   PickNode=PickNode_visplane;
  }

 for (lump=lumplist; lump; lump=lump->next)
  if (lump->islevel)
    {
     char current_level_name[9];
     strncpy(current_level_name,lump->dir->name,8);
     current_level_name[8] = 0;
     DoLevel(current_level_name, current_level = lump->data);
   }  /* if (lump->islevel) */

 {
  long offs=12;
  wad.num_entries=0;
  for (lump=lumplist; lump; lump=lump->next)
   {
    lump->dir->start=offs;
    offs+=lump->dir->length;
    wad.num_entries++;
    if (lump->islevel)
     {
      sortlump((struct lumplist **) &lump->data);
      for (l=lump->data; l; l=l->next)
       {
        l->dir->start=offs;
        offs+=l->dir->length;
        wad.num_entries++;
       }
     }
   }

  newdirec = GetMemory(wad.num_entries*sizeof(struct directory));

  if (!(outfile=fopen(outwad,"wb")))
   {
    fputs("Error: Could not open output PWAD file ",stderr);
    perror(outwad);
    exit(1);
   }

  wad.dir_start=offs;
  if (fwrite(&wad,1,12,outfile)!=12)
    puts("Warning: Consistency check failure writing wad header");

  for (offs=0,lump=lumplist; lump; lump=lump->next)
   {
    newdirec[offs++]=write_lump(lump);
    if (lump->islevel)
      for (l=lump->data; l; l=l->next)
        newdirec[offs++]=write_lump(l);
   }

  if (ftell(outfile)!=wad.dir_start ||
    fwrite(newdirec,sizeof(struct directory),wad.num_entries,outfile)!=wad.num_entries)
    puts("Warning: Consistency check failure writing lump directory");
  fclose(outfile);

  if (offs!=wad.num_entries)
    puts("Warning: Lump directory count consistency check failure");
  printf("\nSaved WAD as %s\n",outwad);
 }
 return 0;
}

/*- end of file ------------------------------------------------------------*/
