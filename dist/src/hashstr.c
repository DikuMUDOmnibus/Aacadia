/****************************************************************************/
/* [S]imulated [M]edieval [A]dventure multi[U]ser [G]ame      |   \\._.//   */
/* -----------------------------------------------------------|   (0...0)   */
/* SMAUG 1.4 (C) 1994, 1995, 1996, 1998  by Derek Snider      |    ).:.(    */
/* -----------------------------------------------------------|    {o o}    */
/* SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,      |   / ' ' \   */
/* Scryn, Rennard, Swordbearer, Gorog, Grishnakh, Nivek,      |~'~.VxvxV.~'~*/
/* Tricops and Fireblade                                      |             */
/* ------------------------------------------------------------------------ */
/* Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael        */
/* Chastain, Michael Quan, and Mitchell Tse.                                */
/* Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,          */
/* Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.     */
/****************************************************************************/
/*        Acadia code team (C) 2002-2005 Kara Enterprises                   */
/************************************************************************/
/*			Hash module					                    */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "acadia.h"

#define STR_HASH_SIZE	1024

struct hashstr_data
{
   struct hashstr_data *next;
   unsigned short int links;
   unsigned short int length;
};

char *str_alloc( char *str );
char *quick_link( char *str );
int str_free( char *str );
void show_hash( int count );
char *hash_stats( void );

struct hashstr_data *string_hash[STR_HASH_SIZE];

char *str_alloc( char *str )
{
   /*
    * register
    */ int len, hash, psize;
   /*
    * register
    */ struct hashstr_data *ptr;

   len = strlen( str );
   psize = sizeof( struct hashstr_data );
   hash = len % STR_HASH_SIZE;
   for( ptr = string_hash[hash]; ptr; ptr = ptr->next )
      if( len == ptr->length && !strcmp( str, ( char * )ptr + psize ) )
      {
         if( ptr->links < 65535 )
            ++ptr->links;
         return ( char * )ptr + psize;
      }
   ptr = ( struct hashstr_data * )MudMalloc( len + psize + 1, 6 );
   ptr->links = 1;
   ptr->length = len;
   if( len )
      strcpy( ( char * )ptr + psize, str );
   else
      strcpy( ( char * )ptr + psize, "" );
   ptr->next = string_hash[hash];
   string_hash[hash] = ptr;
   return ( char * )ptr + psize;
}

char *quick_link( char *str )
{
   /*
    * register
    */ struct hashstr_data *ptr;

   ptr = ( struct hashstr_data * )( str - sizeof( struct hashstr_data ) );
   if( ptr->links == 0 )
   {
      fprintf( stderr, "quick_link: bad pointer\n" );
      return NULL;
   }
   if( ptr->links < 65535 )
      ++ptr->links;
   return str;
}

int str_free( char *str )
{
   /*
    * register
    */ int len, hash;
   /*
    * register
    */ struct hashstr_data *ptr, *ptr2, *ptr2_next;

   len = strlen( str );
   hash = len % STR_HASH_SIZE;
   ptr = ( struct hashstr_data * )( str - sizeof( struct hashstr_data ) );
   if( ptr->links == 65535 )
      return ptr->links;
   if( ptr->links == 0 )
   {
      fprintf( stderr, "str_free: bad pointer\n" );
      return -1;
   }
   if( ptr->links == 1 )
   {
      if( string_hash[hash] == ptr )
      {
         string_hash[hash] = ptr->next;
         MudFree( ptr, 3 );
         return 0;
      }
      for( ptr2 = string_hash[hash]; ptr2; ptr2 = ptr2_next )
      {
         ptr2_next = ptr2->next;
         if( ptr2_next == ptr )
         {
            ptr2->next = ptr->next;
            MudFree( ptr, 4 );
            return 0;
         }
      }
      fprintf( stderr, "str_free: pointer not found for string: %s\n", str );
      return -1;
   }
   return ptr->links;
}

void show_hash( int count )
{
   struct hashstr_data *ptr;
   int x, c;

   for( x = 0; x < count; x++ )
   {
      for( c = 0, ptr = string_hash[x]; ptr; ptr = ptr->next, c++ );
      fprintf( stderr, " %d", c );
   }
   fprintf( stderr, "\n" );
}

void hash_dump( int hash )
{
   struct hashstr_data *ptr;
   char *str;
   int c, psize;

   if( hash > STR_HASH_SIZE || hash < 0 )
   {
      fprintf( stderr, "hash_dump: invalid hash size\n\r" );
      return;
   }
   psize = sizeof( struct hashstr_data );
   for( c = 0, ptr = string_hash[hash]; ptr; ptr = ptr->next, c++ )
   {
      str = ( char * )( ( ( int )ptr ) + psize );
      fprintf( stderr, "Len:%4d Lnks:%5d Str: %s\n\r", ptr->length, ptr->links, str );
   }
   fprintf( stderr, "Total strings in hash %d: %d\n\r", hash, c );
}

char *check_hash( char *str )
{
   static char buf[1024];
   int len, hash, psize, p = 0, c;
   struct hashstr_data *ptr, *fnd;

   buf[0] = '\0';
   len = strlen( str );
   psize = sizeof( struct hashstr_data );
   hash = len % STR_HASH_SIZE;
   for( fnd = NULL, ptr = string_hash[hash], c = 0; ptr; ptr = ptr->next, c++ )
      if( len == ptr->length && !strcmp( str, ( char * )ptr + psize ) )
      {
         fnd = ptr;
         p = c + 1;
      }
   if( fnd )
      sprintf( buf, "Hash info on string: %s\n\rLinks: %d  Position: %d/%d  Hash: %d  Length: %d\n\r",
               str, fnd->links, p, c, hash, fnd->length );
   else
      sprintf( buf, "%s not found.\n\r", str );
   return buf;
}

char *hash_stats( void )
{
   static char buf[1024];
   struct hashstr_data *ptr;
   int x, c, total, totlinks, unique, bytesused, wouldhave, hilink;

   totlinks = unique = total = bytesused = wouldhave = hilink = 0;
   for( x = 0; x < STR_HASH_SIZE; x++ )
   {
      for( c = 0, ptr = string_hash[x]; ptr; ptr = ptr->next, c++ )
      {
         total++;
         if( ptr->links == 1 )
            unique++;
         if( ptr->links > hilink )
            hilink = ptr->links;
         totlinks += ptr->links;
         bytesused += ( ptr->length + 1 + sizeof( struct hashstr_data ) );
         wouldhave += ( ptr->links * ( ptr->length + 1 ) );
      }
   }
   sprintf( buf,
            "Hash strings allocated:%8d  Total links  : %d\n\rString bytes allocated:%8d  Bytes saved  : %d\n\rUnique (wasted) links :%8d  Hi-Link count: %d\n\r",
            total, totlinks, bytesused, wouldhave - bytesused, unique, hilink );
   return buf;
}

void show_high_hash( int top )
{
   struct hashstr_data *ptr;
   int x, psize;
   char *str;

   psize = sizeof( struct hashstr_data );
   for( x = 0; x < STR_HASH_SIZE; x++ )
      for( ptr = string_hash[x]; ptr; ptr = ptr->next )
         if( ptr->links >= top )
         {
            str = ( char * )( ( ( int )ptr ) + psize );
            fprintf( stderr, "Links: %5d  String: >%s<\n\r", ptr->links, str );
         }
}
