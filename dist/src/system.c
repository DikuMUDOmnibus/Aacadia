/****************************************************************************/
/*        Acadia code team (C) 2002-2005 Kara Enterprises                   */
/************************************************************************/
/*				     Systems Handling module		                    */
/************************************************************************/
#include <stdio.h>
#include "acadia.h"
extern FILE *MudError;
long int alloccount = 0;
long int freecount = 0;

void *MudMalloc( size_t size, int index )
{
   if( sysdata.debug == 1 )
   {
      MudError = fopen( ERROR_FILE, "a" );
      fprintf( MudError, "%ld Allocated Memory (MudMalloc) %d\n\r", ++alloccount, index );
      fclose( MudError );
   }
   return ( malloc( size ) );
}

void *MudCalloc( size_t num, size_t size, int index )
{
   if( sysdata.debug == 1 )
   {
      MudError = fopen( ERROR_FILE, "a" );
      fprintf( MudError, "%ld Allocated Memory (MudCalloc) %d\n\r", ++alloccount, index );
      fclose( MudError );
   }
   return ( calloc( num, size ) );
}

void *MudRealloc( void *memblock, size_t size, int index )
{
   if( sysdata.debug == 1 )
   {
      MudError = fopen( ERROR_FILE, "a" );
      fprintf( MudError, "Allocated Memory (MudRealloc) %d\n\r", index );
      fclose( MudError );
   }
   return ( realloc( memblock, size ) );
}

void MudFree( void *memblock, int index )
{
   if( sysdata.debug == 1 )
   {
      MudError = fopen( ERROR_FILE, "a" );
      fprintf( MudError, "%ld Freed Memory (MudFree) %d\n\r", ++freecount, index );
      fclose( MudError );
   }
   free( memblock );

   return;
}
