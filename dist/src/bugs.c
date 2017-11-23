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
/*						Bug Report module			                    */
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
#include "bugs.h"

char *current_dateb args( ( void ) );
int num_bugs args( ( void ) );


int maxBugs;
#define  NULLSTR( str )  ( str == NULL || str[0] == '\0' )
BUGS_DATA *bugs_table;

void load_bugs( void )
{
   FILE *fp;
   int i;

   if( !( fp = fopen( BUGS_FILE, "r" ) ) )
   {
      bug( "Could not open Bugs File for reading.", 0 );
      return;
   }

   fscanf( fp, "%d\n", &maxBugs );

   bugs_table = MudMalloc( sizeof( BUGS_DATA ) * ( maxBugs + 1 ), 1 );

   for( i = 0; i < maxBugs; i++ )
   {
      bugs_table[i].vnum = fread_number( fp );
      bugs_table[i].bug = fread_string( fp );
      bugs_table[i].who = fread_string( fp );
      bugs_table[i].date = fread_string( fp );
      bugs_table[i].mudtime = fread_number( fp );
   }
   bugs_table[maxBugs].who = str_dup( "" );
   fclose( fp );
   return;
}

char *current_dateb(  )
{
   static char buf[128];
   struct tm *datetime;

   datetime = localtime( &current_time );
   strftime( buf, sizeof( buf ), "%m/%d/%Y", datetime );
   return buf;
}

void save_bugs( void )
{
   FILE *fp;
   int i;

   if( !( fp = fopen( BUGS_FILE, "w" ) ) )
   {
      perror( BUGS_FILE );
      return;
   }

   fprintf( fp, "%d\n", maxBugs );
   for( i = 0; i < maxBugs; i++ )
   {
      fprintf( fp, "%d\n", bugs_table[i].vnum );
      fprintf( fp, "%s~\n", bugs_table[i].bug );
      fprintf( fp, "%s~\n", bugs_table[i].who );
      fprintf( fp, "%s~\n", bugs_table[i].date );
      fprintf( fp, "%ld\n", bugs_table[i].mudtime );
      fprintf( fp, "\n" );
   }

   fclose( fp );
   return;
}

void delete_bug( int iBug )
{
   int i, j;
   BUGS_DATA *new_table;

   new_table = MudMalloc( sizeof( BUGS_DATA ) * maxBugs, 2 );

   if( !new_table )
   {
      return;
   }

   for( i = 0, j = 0; i < maxBugs + 1; i++ )
   {
      if( i != iBug )
      {
         new_table[j] = bugs_table[i];
         j++;
      }
   }

   MudFree( bugs_table, 1 );
   bugs_table = new_table;

   maxBugs--;

   return;
}

void do_bug( CHAR_DATA * ch, char *argument )
{
   char strins[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) )
      return;

   if( argument[0] == '\0' )
   {
      send_to_char( "Syntax: bug <string>\n\r", ch );
      if( IS_AGOD( ch ) )
         send_to_char( "&wType '&Rbugs&w' to view the list.&d\n\r", ch );
      return;
   }

   sprintf( strins, "INSERT INTO `tbl_bugs` (`vnum`, `name`, `date`, `what`) VALUES ('%d', '%s', '%s', '%s');",
            ch->in_room->vnum, ch->name, str_dup( current_dateb(  ) ), argument );
   mysql_query( &mysql, strins );

   send_to_char( "Bugs Recorded.\n\r", ch );
   if( IS_AGOD( ch ) )
      send_to_char( "Type 'bugs' to see the bugs.\n\r", ch );
   bug( "New Bug found", 0 );
   return;
}

void do_bsave( CHAR_DATA * ch, char *argument )
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
      send_to_char( "Syntax: bsave load/save\n\r", ch );
      send_to_char( "Syntax: bsave delete (bug number)\n\r", ch );
      return;
   }

   if( !str_cmp( arg1, "load" ) )
   {
      load_bugs(  );
      send_to_char( "Bugs Loaded.\n\r", ch );
      return;
   }

   if( !str_cmp( arg1, "save" ) )
   {
      save_bugs(  );
      send_to_char( "Bugs Saved.\n\r", ch );
      return;
   }

   if( !str_cmp( arg1, "delete" ) )
   {
      int num;

      if( NULLSTR( arg2 ) || !is_number( arg2 ) )
      {
         send_to_char( "&wFor bsave delete, you must provide a bug number.&D\n\r", ch );
         send_to_char( "Syntax: bsave delete (bug number)\n\r", ch );
         return;
      }

      num = atoi( arg2 );
      if( num < 0 || num > ( maxBugs - 1 ) )
      {
         sprintf( buf, "Valid bugs are from 0 to %d.\n\r", ( maxBugs - 1 ) );
         send_to_char( buf, ch );
         return;
      }
      delete_bug( num );
      send_to_char( "Bug deleted.\n\r", ch );
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

char *bug_justify( char *pszText, int iAlignment )
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

int num_bugs( void )
{
   char *test;
   int today;
   int i;

   i = 0;
   test = current_dateb(  );
   today = 0;

   for( i = 0; i < maxBugs; i++ )
      if( !str_cmp( test, bugs_table[i].date ) )
         today++;

   return today;
}

/*
void do_bugs(CHAR_DATA *ch, char *argument) 
{
    char  arg[MAX_INPUT_LENGTH];
    char  buf[MSL];
    char *test;
    int   today;
    int   i;
    bool  fAll;

    one_argument( argument, arg );

    if (IS_NPC(ch))
        return;

    if( maxBugs < 1 )
	return;

    i = 0;
    test = current_dateb();
    today = 0;


    for ( i = 0; i < maxBugs; i++)
        if (!str_cmp(test,bugs_table[i].date))
            today++;

    if( NULLSTR( arg ) )
        fAll = FALSE;
    else fAll = !str_cmp( arg, "all" );

    pager_printf_color(ch, "&wNo.  vnum   Who        Date        Bug&d\n\r" );
    pager_printf_color(ch, "&R----------------------------------------\n\r" );
    for (i = 0; i < maxBugs; i++)
    {
        if( !fAll
            && bugs_table[i].mudtime + (7*24L*3600L) < current_time )
            continue;
        
        sprintf( buf,"&z[&R%2d&z]&w %-5d %-9s &c*%-6s &d%s&d\n\r",
                     i,
					 bugs_table[i].vnum,
                     bugs_table[i].who,
                     bugs_table[i].date,
                     bugs_table[i].bug );
        pager_printf_color(ch, buf );
    }
	
    pager_printf_color(ch, "&R----------------------------------------\n\r" );
    sprintf(buf, "&wThere is a total of &z[ &Y%3d &z] &wbugs in the database.&d\n\r", maxBugs);
    pager_printf_color(ch, buf );
    pager_printf_color(ch, "&zAlso see: '&Cbugs all&z' for a list of all the bugs.&z\n\r" );
    pager_printf_color(ch, "&R----------------------------------------\n\r" );
    sprintf(buf, "&wThere is a total of &z[ &Y%2d &z] &wnew bugs that have been found today.&d\n\r", today);
    pager_printf_color(ch, buf );
    pager_printf_color(ch, "&R----------------------------------------\n\r" );
    pager_printf_color(ch, "&R----------------------------------------\n\r" );
    return;
}
*/

void do_bugs( CHAR_DATA * ch, char *argument )
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
      state = mysql_query( con, "SELECT `index`,`vnum`,`name`,`date`,`what` FROM `tbl_bugs`" );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      result = mysql_store_result( con );
      tnum = mysql_num_rows( result );
      num = tnum - 10;
      sprintf( buf, "SELECT `index`,`vnum`,`name`,`date`,`what` FROM `tbl_bugs` LIMIT %d, 10", num );
      mysql_free_result( result );

      state = mysql_query( con, buf );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      result = mysql_store_result( con );
      ch_printf( ch,
                 "&YNumber of Bugs in the database: &W%d\n\r&GID   VNum     Name             Date     What\n\r--------------------------------------------------\n\r",
                 tnum );
      while( ( row = mysql_fetch_row( result ) ) != NULL )
      {
         ch_printf( ch, "&g%-3s  &w%-7s  &P%-12s  &B%-10s  &C%s&D\n\r", row[0], row[1], row[2], row[3], row[4] );
      }
      mysql_free_result( result );
   }
   else if( !str_cmp( arg, "all" ) )
   {
      state = mysql_query( con, "SELECT `index`,`vnum`,`name`,`date`,`what` FROM `tbl_bugs`" );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      result = mysql_store_result( con );
      ch_printf( ch,
                 "&YNumber of Bugs in the database: &W%d\n\r&GID   VNum     Name             Date     What\n\r--------------------------------------------------\n\r",
                 mysql_num_rows( result ) );
      while( ( row = mysql_fetch_row( result ) ) != NULL )
      {
         ch_printf( ch, "&g%-3s  &w%-7s  &P%-12s  &B%-10s  &C%s&D\n\r", row[0], row[1], row[2], row[3], row[4] );
      }
      mysql_free_result( result );
   }
   else
   {
      send_to_char( "Syntax: bugs <all>\n\r", ch );
   }
   return;
}
