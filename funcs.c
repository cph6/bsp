/*- FUNCS.C ----------------------------------------------------------------*/
/* $Id: funcs.c,v 1.3 2000/08/26 12:56:40 cph Exp $ */
/*- terminate the program reporting an error -------------------------------*/

#include "structs.h"
#include "bsp.h"

void ProgError( char *errstr, ...)
{
   va_list args;

   va_start( args, errstr);
   fprintf(stderr, "\nProgram Error: *** ");
   vfprintf( stderr, errstr, args);
   fprintf(stderr, " ***\n");
   va_end( args);
   unlink(outwad);
   exit( 5);
}

/*- allocate memory with error checking ------------------------------------*/

void *GetMemory( size_t size)
{
   void *ret = malloc( size);
   if (!ret)
      ProgError( "out of memory (cannot allocate %u bytes)", size);
   return ret;
}

/*- reallocate memory with error checking ----------------------------------*/

void *ResizeMemory( void *old, size_t size)
{
   void *ret = realloc( old, size);
   if (!ret)
      ProgError( "out of memory (cannot reallocate %u bytes)", size);
   return ret;
}


/*--------------------------------------------------------------------------*/

