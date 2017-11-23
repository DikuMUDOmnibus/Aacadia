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
/*						Changes module				                    */
/************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "acadia.h"
#include "changes.h"

char *current_date args( ( void ) );
int num_changes args( ( void ) );


int maxChanges;
#define  NULLSTR( str )  ( str == NULL || str[0] == '\0' )
CHANGE_DATA *changes_table;

void load_changes( void )
{
   FILE *fp;
   int i;

   if( !( fp = fopen( CHANGES_FILE, "r" ) ) )
   {
      bug( "Could not open Changes File for reading.", 0 );
      return;
   }

   fscanf( fp, "%d\n", &maxChanges );

   changes_table = MudMalloc( sizeof( CHANGE_DATA ) * ( maxChanges + 1 ), 3 );

   for( i = 0; i < maxChanges; i++ )
   {
      changes_table[i].change = fread_string( fp );
      changes_table[i].coder = fread_string( fp );
      changes_table[i].date = fread_string( fp );
      changes_table[i].mudtime = fread_number( fp );
   }
   changes_table[maxChanges].coder = str_dup( "" );
   fclose( fp );
   return;
}

char *current_date(  )
{
   static char buf[128];
   struct tm *datetime;

   datetime = localtime( &current_time );
   strftime( buf, sizeof( buf ), "%m/%d/%Y", datetime );
   return buf;
}

void save_changes( void )
{
   FILE *fp;
   int i;

   if( !( fp = fopen( CHANGES_FILE, "w" ) ) )
   {
      perror( CHANGES_FILE );
      return;
   }

   fprintf( fp, "%d\n", maxChanges );
   for( i = 0; i < maxChanges; i++ )
   {
      fprintf( fp, "%s~\n", changes_table[i].change );
      fprintf( fp, "%s~\n", changes_table[i].coder );
      fprintf( fp, "%s~\n", changes_table[i].date );
      fprintf( fp, "%ld\n", changes_table[i].mudtime );
      fprintf( fp, "\n" );
   }

   fclose( fp );
   return;
}

void delete_change( int iChange )
{
   int i, j;
   CHANGE_DATA *new_table;

   new_table = MudMalloc( sizeof( CHANGE_DATA ) * maxChanges, 4 );

   if( !new_table )
   {
      return;
   }

   for( i = 0, j = 0; i < maxChanges + 1; i++ )
   {
      if( i != iChange )
      {
         new_table[j] = changes_table[i];
         j++;
      }
   }

   MudFree( changes_table, 2 );
   changes_table = new_table;

   maxChanges--;

   return;
}


void do_addchange( CHAR_DATA * ch, char *argument )
{
   char strins[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) )
      return;

   if( argument[0] == '\0' )
   {
      send_to_char( "Syntax: addchange <string>\n\r", ch );
      if( IS_AGOD( ch ) )
         send_to_char( "&wType '&Rchanges&w' to view the list.&d\n\r", ch );
      return;
   }

   sprintf( strins, "INSERT INTO `tbl_changes` (`name`, `date`, `what`) VALUES ('%s', '%s', '%s');",
            ch->name, str_dup( current_date(  ) ), argument );
   mysql_query( &mysql, strins );

   send_to_char( "Changes Created.\n\r", ch );
   send_to_char( "Type 'changes' to see the changes.\n\r", ch );
   talk_info( AT_PLAIN, "&R<&BINFO&R> &zNew Change added to the mud, type '&Cchanges&z' to see it&z" );
   return;
}

/*
void do_addchange(CHAR_DATA *ch, char *argument )
{
    CHANGE_DATA * new_table;
    
    if ( IS_NPC( ch ) )
        return;
    
    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: Addchange ChangeString\n\r", ch );
        send_to_char( "&wType '&Rchanges&w' to view the list.&d\n\r", ch );
        return;
    }

    maxChanges++;
    new_table = MudRealloc( changes_table, sizeof( CHANGE_DATA ) *(maxChanges+1), 2 );

    if (!new_table) 
    {
        send_to_char ("Memory allocation failed. Brace for impact.\n\r",ch);
        return;
    }

    changes_table = new_table;
    
    changes_table[maxChanges-1].change  = str_dup( argument );
    changes_table[maxChanges-1].coder   = str_dup( ch->name );
    changes_table[maxChanges-1].date    = str_dup(current_date());
    changes_table[maxChanges-1].mudtime = current_time;
    
    send_to_char("Changes Created.\n\r",ch);
    send_to_char("Type 'changes' to see the changes.\n\r",ch);
    talk_info( AT_PLAIN, "&R<&BINFO&R> &zNew Change added to the mud, type '&Cchanges&z' to see it&z");
    save_changes();
    return;
}
*/
void do_chsave( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[MSL];

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( IS_NPC( ch ) )
      return;

   if( !ch->desc || NULLSTR( arg1 ) )
   {
      send_to_char( "Syntax: chsave load/save\n\r", ch );
      send_to_char( "Syntax: chsave delete (change number)\n\r", ch );
      return;
   }

   if( !str_cmp( arg1, "load" ) )
   {
      load_changes(  );
      send_to_char( "Changes Loaded.\n\r", ch );
      return;
   }

   if( !str_cmp( arg1, "save" ) )
   {
      save_changes(  );
      send_to_char( "Changes Saved.\n\r", ch );
      return;
   }

   if( !str_cmp( arg1, "delete" ) )
   {
      int num;

      if( NULLSTR( arg2 ) || !is_number( arg2 ) )
      {
         send_to_char( "&wFor chsave delete, you must provide a change number.{x\n\r", ch );
         send_to_char( "Syntax: chsave delete (change number)\n\r", ch );
         return;
      }

      num = atoi( arg2 );
      if( num < 0 || num > ( maxChanges - 1 ) )
      {
         sprintf( buf, "Valid changes are from 0 to %d.\n\r", ( maxChanges - 1 ) );
         send_to_char( buf, ch );
         return;
      }
      delete_change( num );
      send_to_char( "Change deleted.\n\r", ch );
      return;
   }

   return;
}


static void AddSpaces( char **ppszText, int iNumber )
{
   int iLoop;

   for( iLoop = 0; iLoop < iNumber; iLoop++ )
   {
      *( *ppszText )++ = ' ';
   }
}

char *change_justify( char *pszText, int iAlignment )
{
   static char s_szResult[4096];
   char *pszResult = &s_szResult[0];
   char szStore[4096];
   int iMax;
   int iLength = iAlignment - 1;
   int iLoop = 0;

   if( strlen( pszText ) < 10 )
   {
      strcpy( s_szResult, "BUG: Justified string cannot be less than 10 characters long." );
      return ( &s_szResult[0] );
   }

   while( *pszText == ' ' )
      pszText++;

   szStore[iLoop++] = *pszText++;

   if( szStore[iLoop - 1] >= 'a' && szStore[iLoop - 1] <= 'z' )
      szStore[iLoop - 1] = UPPER( szStore[iLoop] );

   while( *pszText )
   {
      switch ( *pszText )
      {
         default:
            szStore[iLoop++] = *pszText++;
            break;
         case ' ':
            if( *( pszText + 1 ) != ' ' )
            {
               szStore[iLoop++] = *pszText;
            }
            pszText++;
            break;
         case '.':
         case '?':
         case '!':
            szStore[iLoop++] = *pszText++;
            switch ( *pszText )
            {
               default:
                  szStore[iLoop++] = ' ';
                  szStore[iLoop++] = ' ';
                  while( *pszText == ' ' )
                     pszText++;
                  szStore[iLoop++] = *pszText++;
                  if( szStore[iLoop - 1] >= 'a' && szStore[iLoop - 1] <= 'z' )
                     szStore[iLoop - 1] &= ~32;
                  break;
               case '.':
               case '?':
               case '!':
                  break;
            }
            break;
         case ',':
            szStore[iLoop++] = *pszText++;
            while( *pszText == ' ' )
               pszText++;
            szStore[iLoop++] = ' ';
            break;
         case '$':
            szStore[iLoop++] = *pszText++;
            while( *pszText == ' ' )
               pszText++;
            break;
         case '\n':
         case '\r':
            pszText++;
            break;
      }
   }

   szStore[iLoop] = '\0';

   iMax = strlen( szStore );

   while( iLength < iMax )
   {
      while( szStore[iLength] != ' ' && iLength > 1 )
         iLength--;

      if( szStore[iLength] == ' ' )
      {
         szStore[iLength] = '\n';

         iLength += iAlignment;
      }
      else
         break;
   }


   iLoop = 0;

   while( iLoop < iMax )
   {
      *pszResult++ = szStore[iLoop];
      switch ( szStore[iLoop] )
      {
         default:
            break;
         case '\n':
            *pszResult++ = '\r';
            while( szStore[iLoop + 1] == ' ' )
               iLoop++;
            AddSpaces( &pszResult, 25 );
            break;
      }
      iLoop++;
   }

   *pszResult++ = '\0';

   return ( &s_szResult[0] );
}

/*
int num_changes( void )
{
	char *test;
    int   today;
    int   i;
    
    i = 0;
    test = current_date();
    today = 0;
    
     for ( i = 0; i < maxChanges; i++)
        if (!str_cmp(test,changes_table[i].date))
            today++;
            
     return today;
}
*/
int num_changes( void )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   MYSQL *con = &mysql;
   int state;
   char *test;
   int today;

   test = str_dup( current_date(  ) );
   today = 0;

   state = mysql_query( con, "SELECT `date` FROM `tbl_changes`" );
   if( state != 0 )
   {
      perror( mysql_error( con ) );
      return 0;
   }
   result = mysql_store_result( con );
   while( ( row = mysql_fetch_row( result ) ) != NULL )
   {
      if( !str_cmp( test, row[0] ) )
         today++;
   }
   mysql_free_result( result );

   return today;
}

void do_oldchanges( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MSL];
   char *test;
   int today;
   int i;
   bool fAll;

   one_argument( argument, arg );

   if( IS_NPC( ch ) )
      return;

   if( maxChanges < 1 )
      return;

   i = 0;
   test = current_date(  );
   today = 0;


   for( i = 0; i < maxChanges; i++ )
      if( !str_cmp( test, changes_table[i].date ) )
         today++;

   if( NULLSTR( arg ) )
      fAll = TRUE;
   else
      fAll = !str_cmp( arg, "all" );

   pager_printf_color( ch, "&wNo.  Coder        Date        Change&d\n\r" );
   pager_printf_color( ch, "&R----------------------------------------\n\r" );
   for( i = 0; i < maxChanges; i++ )
   {
      if( !fAll && changes_table[i].mudtime + ( 7 * 24L * 3600L ) < current_time )
         continue;

      sprintf( buf, "&z[&R%2d&z]&w %-9s &c*%-6s &d%-55s&d\n\r",
               i, changes_table[i].coder, changes_table[i].date, changes_table[i].change );
      pager_printf_color( ch, buf );
   }
   pager_printf_color( ch, "&R----------------------------------------\n\r" );
   sprintf( buf, "&wThere is a total of &z[ &Y%3d &z] &wchanges in the database.&d\n\r", maxChanges );
   pager_printf_color( ch, buf );
/*    pager_printf_color(ch, "&zAlso see: '&Cchanges all&z' for a list of all the changes.&z\n\r" );
    pager_printf_color(ch, "&R----------------------------------------\n\r" );
    sprintf(buf, "&wThere is a total of &z[ &Y%2d &z] &wnew changes that have been added today.&d\n\r", today);
    pager_printf_color(ch, buf );
    pager_printf_color(ch, "&R----------------------------------------\n\r" );*/
   pager_printf_color( ch, "&R----------------------------------------\n\r" );
   return;
}

void do_changes( CHAR_DATA * ch, char *argument )
{
   MYSQL_RES *result;
   MYSQL_ROW row;
   MYSQL *con = &mysql;
   int state;
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int tnum = 0, num = 0;

   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      state = mysql_query( con, "SELECT `index`,`name`,`date`,`what` FROM `tbl_changes`" );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      result = mysql_store_result( con );
      tnum = mysql_num_rows( result );
      num = tnum - 10;
      sprintf( buf, "SELECT `index`,`name`,`date`,`what` FROM `tbl_changes` LIMIT %d, 10", num );
      mysql_free_result( result );

      state = mysql_query( con, buf );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      result = mysql_store_result( con );
      ch_printf( ch,
                 "&YNumber of Changes in the database: &W%d\n\r&GID     Name             Date     What\n\r--------------------------------------------------\n\r",
                 tnum );
      while( ( row = mysql_fetch_row( result ) ) != NULL )
      {
         ch_printf( ch, "&g%-5s  &P%-12s  &B%-10s  &C%s&D\n\r", row[0], row[1], row[2], row[3] );
      }
      mysql_free_result( result );
   }
   else if( !str_cmp( arg, "all" ) )
   {
      state = mysql_query( con, "SELECT `index`,`name`,`date`,`what` FROM `tbl_changes`" );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      result = mysql_store_result( con );
      ch_printf( ch,
                 "&YNumber of Changes in the database: &W%d\n\r&GID     Name             Date     What\n\r--------------------------------------------------\n\r",
                 mysql_num_rows( result ) );
      while( ( row = mysql_fetch_row( result ) ) != NULL )
      {
         ch_printf( ch, "&g%-5s  &P%-12s  &B%-10s  &C%s&D\n\r", row[0], row[1], row[2], row[3] );
      }
      mysql_free_result( result );
   }
   else
   {
      send_to_char( "Syntax: changes <all>\n\r", ch );
   }
   return;
}
