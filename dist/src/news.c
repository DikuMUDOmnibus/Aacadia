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
/*				     News Information module		                    */
/************************************************************************/

#include <ctype.h>
#include <string.h>
#include <time.h>
#include "acadia.h"
#include "news.h"

NEWS_DATA *first_news;
NEWS_DATA *last_news;

void load_news args( ( void ) );
void add_news args( ( char *argument ) );
void write_news args( ( void ) );
void generate_html_news args( ( void ) );
void show_news args( ( CHAR_DATA * ch, int show_type, int count ) );
char *align_news args( ( char *argument ) );
char *news_argument args( ( char *argument, char *arg_first ) );
NEWS_DATA *fread_news args( ( FILE * fpin ) );
void clear_news args( ( bool sMatch, int nCount ) );
void format_posttime args( ( NEWS_DATA * news ) );
char *html_color args( ( char *color, char *text ) );
char *html_format args( ( char *argument ) );


void do_news( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_NEWS_LENGTH];
   char arg2[MAX_NEWS_LENGTH];

   if( IS_NPC( ch ) )
      return;

   if( argument && argument[0] != '\0' )
   {
      argument = one_argument( argument, arg1 );

      if( !str_cmp( arg1, "version" ) )
      {
         ch_printf_color( ch, "&r&Y&G&cT&G&Che &G&cS&G&Ctorm &G&cA&G&Cdvocates &G&cV&G&Cersion&G&c: &G&W%s&G&c.\n\r",
                          NEWS_VERSION );
         return;
      }
      else if( !str_cmp( arg1, "all" ) )
      {
         show_news( ch, TYPE_ALL, -1 );
         return;
      }
      else if( !str_cmp( arg1, "first" ) )
      {
         int show_count = -1;

         argument = one_argument( argument, arg2 );

         if( !arg2 || arg2[0] == '\0' )
         {
            show_count = -1;
         }
         else
         {
            if( is_number( arg2 ) )
               show_count = atoi( arg2 );
            else
               show_count = -1;
         }

         show_news( ch, TYPE_LIST_FIRST, show_count );
         return;
      }
      else if( !str_cmp( arg1, "last" ) )
      {
         int show_count = -1;

         argument = one_argument( argument, arg2 );

         if( !arg2 || arg2[0] == '\0' )
         {
            show_count = -1;
         }
         else
         {
            if( is_number( arg2 ) )
               show_count = atoi( arg2 );
            else
               show_count = -1;
         }

         show_news( ch, TYPE_LIST_LAST, show_count );
         return;
      }
      else if( !IS_IMMORTAL( ch ) )
      {
         show_news( ch, TYPE_NORMAL, -1 );
         return;
      }
      else if( !str_cmp( arg1, "add" ) && ( argument && argument[0] != '\0' ) )
      {
         add_news( argument );
         send_to_char_color( "&C&GNews added.\n\r", ch );
      }
      else if( !str_cmp( arg1, "load" ) )
      {
         clear_news( FALSE, 0 );
         load_news(  );
         generate_html_news(  );
         send_to_char_color( "&C&GNews loaded.\n\r", ch );
      }
      else if( !str_cmp( arg1, "list" ) )
      {
         show_news( ch, TYPE_IMM_LIST, -1 );
      }
      else if( !str_cmp( arg1, "remove" ) && ( argument && argument[0] != '\0' ) )
      {
         bool clearAll = FALSE;
         char cmd[1024];

         if( !str_cmp( argument, "all" ) )
         {
            clearAll = TRUE;
         }

         if( !is_number( argument ) )
         {
            send_to_char_color( "Argument must be a number.\n\r", ch );
            return;
         }

         if( clearAll != TRUE )
            clear_news( TRUE, atoi( argument ) );
         else
            clear_news( FALSE, 0 );

         write_news(  );
         generate_html_news(  );

         sprintf( cmd, "cp %s %s", HTML_NEWS_FILE, HTML_DIR );
         system( cmd );

         sprintf( cmd, "chmod 664 %s%s", HTML_DIR, HTML_NEWS_FILE );
         system( cmd );

         send_to_char_color( "&C&GNews removed.\n\r", ch );
      }
      else
      {
         send_to_char_color( "Syntax #1: news\n\r", ch );
         send_to_char_color( "Syntax #2: news <field> <news>\n\r", ch );
         send_to_char_color( "Fields   : all add load list remove\n\r", ch );
         return;
      }
   }
   else
   {
      show_news( ch, TYPE_NORMAL, -1 );
   }
}


void load_news(  )
{
   FILE *fpin = fopen( NEWS_FILE, "r" );

   if( fpin == NULL )
   {
      bug( "Cannot open news.dat for reading.", 0 );
      perror( NEWS_FILE );
      return;
   }

   for( ;; )
   {
      char letter;
      char *word;
      NEWS_DATA *news;

      letter = fread_letter( fpin );

      if( letter == '*' )
      {
         fread_to_eol( fpin );
         continue;
      }

      if( letter != '#' )
      {
         bug( "Load_news: # not found.", 0 );
         break;
      }

      word = fread_word( fpin );

      if( !str_cmp( word, "News" ) )
      {
         news = fread_news( fpin );

         if( news != NULL )
            LINK( news, first_news, last_news, next, prev );
      }
      else if( !str_cmp( word, "End" ) )
      {
         fclose( fpin );
         break;
      }
      else
      {
         bug( "Load_news: bad section encountered.", 0 );
         break;
      }
   }
   return;
}


void add_news( char *argument )
{
   char cmd[1024];
   char *buf;
   NEWS_DATA *news = NULL;

   buf = align_news( argument );

   CREATE( news, NEWS_DATA, 1 );

   news->time_stamp = time( NULL );
   news->news_data = align_news( argument );
   format_posttime( news );

   LINK( news, first_news, last_news, next, prev );

   write_news(  );
   generate_html_news(  );

   sprintf( cmd, "cp %s %s", HTML_NEWS_FILE, HTML_DIR );
   system( cmd );

   sprintf( cmd, "chmod 664 %s%s", HTML_DIR, HTML_NEWS_FILE );
   system( cmd );
}


void write_news(  )
{
   FILE *fpout;
   NEWS_DATA *news;

   if( ( fpout = fopen( NEWS_FILE, "w" ) ) == NULL )
   {
      bug( "Cannot open news.dat for writing.", 0 );
      perror( NEWS_FILE );
      return;
   }

   for( news = first_news; news != NULL; news = news->next )
   {
      fprintf( fpout, "#News\n" );
      fprintf( fpout, "TimeStamp %ld\n", news->time_stamp > 0 ? news->time_stamp : -1 );
      fprintf( fpout, "NewsData  %s~\n", news->news_data );
      fprintf( fpout, "End\n\n" );
   }
   fprintf( fpout, "#End\n" );

   fclose( fpout );
}


void generate_html_news(  )
{
   FILE *fpout;
   NEWS_DATA *news;

   if( ( fpout = fopen( HTML_NEWS_FILE, "w" ) ) == NULL )
   {
      bug( "Cannot open %s for writing.", HTML_NEWS_FILE );
      perror( HTML_NEWS_FILE );
      return;
   }

   fprintf( fpout, "<html>\n" );
   fprintf( fpout, "  <head>\n" );
   fprintf( fpout, "    <title>%s News</title>\n", sysdata.mud_name );
   fprintf( fpout, "  </head>\n" );
   fprintf( fpout, "\n" );
   fprintf( fpout, "  <body bgcolor=\"%s\" text=\"%s\" link=\"%s\" vlink=\"%s\" alink=\"%s\" background=\"%sbg.jpg\">\n",
            HTML_WHITE, HTML_BLACK, HTML_BLUE, HTML_BLUE, HTML_BLUE, HTML_NEWS_IMAGES );

   fprintf( fpout, "\n" );
   fprintf( fpout, "<tt>\n" );
   fprintf( fpout, "\n" );

   fprintf( fpout, "        <center>\n" );
   fprintf( fpout, "          <img src=\"%s/banner.gif\" border=\"0\" alt=\"Banner Image\">\n", HTML_NEWS_IMAGES );
   fprintf( fpout, "        </center>\n" );


   fprintf( fpout, "        <center>\n" );
   fprintf( fpout, "%s", html_color( HTML_BLADE, "&#60;XXXXXXXXXXXXXXXXXXXXXXXXXXX" ) );
   fprintf( fpout, "%s", html_color( HTML_BASE, "]" ) );
   fprintf( fpout, "%s", html_color( HTML_HILT, "=====(" ) );
   fprintf( fpout, "%s", html_color( HTML_BALL, "@" ) );
   fprintf( fpout, "%s", html_color( HTML_HILT, ")" ) );
   fprintf( fpout, "%s", html_color( HTML_TITLE, " News " ) );
   fprintf( fpout, "%s", html_color( HTML_HILT, "(" ) );
   fprintf( fpout, "%s", html_color( HTML_BALL, "@" ) );
   fprintf( fpout, "%s", html_color( HTML_HILT, ")=====" ) );
   fprintf( fpout, "%s", html_color( HTML_BASE, "[" ) );
   fprintf( fpout, "%s", html_color( HTML_BLADE, "XXXXXXXXXXXXXXXXXXXXXXXXXXX&#62;" ) );
   fprintf( fpout, "        </center>\n" );

   fprintf( fpout, "\n" );
   fprintf( fpout, "<br><br>" );
   fprintf( fpout, "\n" );

   fprintf( fpout, "        <table width=\"90%%\" border=\"0\" align=\"CENTER\">\n" );

   for( news = first_news; news != NULL; news = news->next )
   {
      char day[MAX_NEWS_LENGTH];
      char month[MAX_NEWS_LENGTH];
      char year[MAX_NEWS_LENGTH];
      char mon_buf[MAX_NEWS_LENGTH];
      char day_buf[MAX_NEWS_LENGTH];
      char year_buf[MAX_NEWS_LENGTH];
      char sep_buf[MAX_NEWS_LENGTH];
      char date_buf[MAX_NEWS_LENGTH];
      char news_data_buf[MAX_NEWS_LENGTH];

      sprintf( day, "%2.2d", news->day );
      sprintf( month, "%2.2d", news->month );
      sprintf( year, "%4.4d", news->year );

      sprintf( mon_buf, "%s", html_color( HTML_DATE, month ) );
      sprintf( day_buf, "%s", html_color( HTML_DATE, day ) );
      sprintf( year_buf, "%s", html_color( HTML_DATE, year ) );
      sprintf( sep_buf, "%s", html_color( HTML_SEPARATOR, "/" ) );

      sprintf( date_buf, "%s%s%s%s%s", mon_buf, sep_buf, day_buf, sep_buf, year_buf );
      sprintf( news_data_buf, "%s", html_color( HTML_NEWS, html_format( news->news_data ) ) );

      fprintf( fpout, "          <tr>\n" );

      fprintf( fpout, "            <td width=\"20%%\" valign=\"TOP\">\n" );
      fprintf( fpout, "<tt>%s</tt>\n", date_buf );
      fprintf( fpout, "            </td>\n" );

      fprintf( fpout, "            <td width=\"80%%\" valign=\"TOP\">\n" );
      fprintf( fpout, "<tt>%s</tt>\n", news_data_buf );
      fprintf( fpout, "            </td>\n" );

      fprintf( fpout, "          </tr>\n" );
      fprintf( fpout, "\n" );
   }

   fprintf( fpout, "        </table>\n" );

   fprintf( fpout, "\n" );
   fprintf( fpout, "<br><br>\n" );
   fprintf( fpout, "\n" );

   fprintf( fpout, "\n" );

   fprintf( fpout, "\n" );
   fprintf( fpout, "<a href=\"links.htm\">Back</a><p>\n" );
   fprintf( fpout, "\n" );

   fprintf( fpout, "  </body>\n" );
   fprintf( fpout, "</html>\n" );

   fclose( fpout );
}


void show_news( CHAR_DATA * ch, int show_type, int count )
{
   int nCount = 1;
   char buf[MAX_NEWS_LENGTH * 2];
   char bar[MAX_NEWS_LENGTH * 2];
   NEWS_DATA *news = NULL, *curr_news = NULL;

   sprintf( bar,
            "%s<XXXXXXXXXXXXXXXXXXXXXXXXXXX%s]%s=====(%s@%s) %sNews %s(%s@%s)=====%s[%sXXXXXXXXXXXXXXXXXXXXXXXXXXX>%s\n\r",
            AT_BLADE, AT_BASE, AT_HILT, AT_BALL, AT_HILT, AT_TITLE, AT_HILT, AT_BALL, AT_HILT, AT_BASE, AT_BLADE, AT_DATE );
   send_to_pager_color( bar, ch );

   switch ( show_type )
   {
      case TYPE_IMM_LIST:
         for( news = first_news, nCount = 1; news != NULL; news = news->next, nCount++ )
         {
            sprintf( buf, "%s%10d%s]  %s%s\n\r", AT_DATE, nCount, AT_SEPARATOR, AT_NEWS, news->news_data );
            send_to_pager_color( buf, ch );
         }
         break;

      case TYPE_NORMAL:
      case TYPE_ALL:
      case TYPE_LIST_FIRST:
         for( news = first_news, nCount = 1; news != NULL; news = news->next, nCount++ )
         {
            bool fShow = FALSE;
            bool fBreakOut = FALSE;

            if( show_type == TYPE_ALL )
               fShow = TRUE;
            else if( show_type == TYPE_NORMAL &&
                     ( ( news->time_stamp > ch->pcdata->last_read_news ) || ( news->next == NULL ) ) )
               fShow = TRUE;
            else if( show_type == TYPE_LIST_FIRST )
            {
               if( count > 0 )
               {
                  if( nCount <= count )
                     fShow = TRUE;
               }
               else
               {
                  news = first_news;
                  fShow = TRUE;
                  fBreakOut = TRUE;
               }
            }

            if( news && fShow == TRUE )
            {
               sprintf( buf, "%s%2.2d%s/%s%2.2d%s/%s%4.4d  %s%s\n\r", AT_DATE, news->month, AT_SEPARATOR, AT_DATE, news->day,
                        AT_SEPARATOR, AT_DATE, news->year, AT_NEWS, news->news_data );
               send_to_pager_color( buf, ch );
               if( news->time_stamp > ch->pcdata->last_read_news )
               {
                  ch->pcdata->last_read_news = news->time_stamp;
               }
               if( fBreakOut == TRUE )
                  break;
            }
         }
         break;

      case TYPE_LIST_LAST:
         if( count > 0 )
         {
            for( news = last_news, nCount = 1; news != NULL && nCount <= count; news = news->prev, nCount++ )
            {
               curr_news = news;
            }
         }
         else
            curr_news = last_news;

         for( news = curr_news; news != NULL; news = news->next )
         {
            if( news )
            {
               sprintf( buf, "%s%2.2d%s/%s%2.2d%s/%s%4.4d  %s%s\n\r", AT_DATE, news->month, AT_SEPARATOR, AT_DATE, news->day,
                        AT_SEPARATOR, AT_DATE, news->year, AT_NEWS, news->news_data );
               send_to_pager_color( buf, ch );
               if( news->time_stamp > ch->pcdata->last_read_news )
               {
                  ch->pcdata->last_read_news = news->time_stamp;
               }
            }
         }
         break;


      default:
         break;
   }
}


char *align_news( char *argument )
{
   char buf[MAX_NEWS_LENGTH];
   char arg[MAX_NEWS_LENGTH];
   char *return_buf;
   int num = 0;
   int count = 0;
   int date_len = 10;
   int spacer = 2;
   int total = ( date_len + spacer );

   strcpy( buf, "" );

   if( argument == NULL || argument[0] == '\0' )
      return "";

   for( ;; )
   {
      int i = 0;
      int length = 0;
      int longlen = 0;

      argument = news_argument( argument, arg );

      length = strlen( arg );

      if( ( total + length ) >= 79 )
      {
         int index;

         buf[num] = '\n';
         num++;
         buf[num] = '\r';
         num++;
         for( index = 0; index < ( date_len + spacer ); index++ )
         {
            buf[num] = ' ';
            num++;
         }
         total = ( date_len + spacer );
      }

      longlen = strlen( arg );

      for( i = 0; i < longlen; i++ )
      {
         if( arg[count] == '&' || arg[count] == '^' )
         {
            if( arg[count + 1] == '\0' )
            {
               arg[count] = '\0';
            }
            else if( ( arg[count] == '&' && arg[count + 1] == '&' ) || ( arg[count] == '^' && arg[count + 1] == '^' ) )
            {
               buf[num] = arg[count];
               num++;
               count++;
               i++;
               buf[num] = arg[count];
               num++;
               count++;
               total++;
            }
            else
            {
               count += 2;
               i++;
            }
         }
         else
         {
            buf[num] = arg[count];
            total++;
            num++;
            count++;
         }
      }

      if( argument != NULL && argument[0] != '\0' )
      {
         buf[num] = ' ';
         num++;
         total++;
         count = 0;
      }
      else
      {
         buf[num] = '\0';
         break;
      }
   }

   return_buf = STRALLOC( buf );

   return return_buf;
}


char *news_argument( char *argument, char *arg_first )
{
   char cEnd;
   sh_int count;

   count = 0;

   if( !argument || argument[0] == '\0' )
   {
      arg_first[0] = '\0';
      return argument;
   }

   while( isspace( *argument ) )
      argument++;

   cEnd = ' ';

   while( *argument != '\0' || ++count >= 255 )
   {
      if( *argument == cEnd )
      {
         argument++;
         break;
      }
      *arg_first = *argument;
      arg_first++;
      argument++;
   }
   *arg_first = '\0';

   while( isspace( *argument ) )
      argument++;

   return argument;
}


NEWS_DATA *fread_news( FILE * fpin )
{
   char *word;
   bool fMatch;
   NEWS_DATA *news = NULL;

   CREATE( news, NEWS_DATA, 1 );

   for( ;; )
   {
      word = feof( fpin ) ? "End" : fread_word( fpin );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
         case '*':
            fMatch = TRUE;
            fread_to_eol( fpin );
            break;

         case 'D':
            if( !str_cmp( word, "Day" ) )
            {
               news->day = fread_number( fpin );
               fMatch = TRUE;
               break;
            }

         case 'E':
            if( !str_cmp( word, "End" ) )
            {
               return news;
            }

         case 'M':
            if( !str_cmp( word, "Month" ) )
            {
               news->month = fread_number( fpin );
               fMatch = TRUE;
               break;
            }

         case 'N':
            if( !str_cmp( word, "NewsData" ) )
            {
               news->news_data = fread_string( fpin );
               fMatch = TRUE;
               break;
            }

         case 'T':
            if( !str_cmp( word, "TimeStamp" ) )
            {
               news->time_stamp = fread_number( fpin );
               if( news->time_stamp > 0 )
               {
                  format_posttime( news );
               }
               fMatch = TRUE;
               break;
            }

         case 'Y':
            if( !str_cmp( word, "Year" ) )
            {
               news->year = fread_number( fpin );
               fMatch = TRUE;
               break;
            }
      }

      if( !fMatch )
      {
         sprintf( bug_buf, "Load_news: no match: %s", word );
         bug( bug_buf, 0 );
      }
   }
   return NULL;
}


void clear_news( bool sMatch, int nCount )
{
   int nCurrent = 1;
   NEWS_DATA *news;

   if( sMatch == FALSE )
   {
      while( ( news = first_news ) != NULL )
      {
         STRFREE( news->news_data );
         UNLINK( news, first_news, last_news, next, prev );
         DISPOSE( news );
      }
   }
   else
   {
      for( news = first_news; news != NULL; news = news->next )
      {
         if( nCount == nCurrent )
         {
            STRFREE( news->news_data );
            UNLINK( news, first_news, last_news, next, prev );
            DISPOSE( news );
            break;
         }
         else
            nCurrent++;
      }
   }
}


void format_posttime( NEWS_DATA * news )
{
   if( news == NULL )
      return;

   if( news->time_stamp > 0 )
   {
      int day, month, year;
      struct tm *time = localtime( &news->time_stamp );

      day = time->tm_mday;
      month = ( time->tm_mon + 1 );
      year = ( time->tm_year + 1900 );

      news->day = day;
      news->month = month;
      news->year = year;
   }
   else
   {
      int day, month, year;
      time_t t = time( NULL );
      struct tm *time = localtime( &t );

      day = time->tm_mday;
      month = ( time->tm_mon + 1 );
      year = time->tm_year;

      news->day = day;
      news->month = month;
      news->year = year;
      news->time_stamp = t;
   }

   return;
}


char *html_color( char *color, char *text )
{
   static char buf[MAX_HTML_LENGTH];

   if( !color || color[0] == '\0' )
   {
      html_color( "#FFFFFF", text );
   }
   else if( !text || text[0] == '\0' )
   {
      html_color( color, " " );
   }
   else
   {
      sprintf( buf, "%s%s%s%s%s%c", HTML_COLOR_OPEN1, color, HTML_COLOR_OPEN2, text, HTML_COLOR_CLOSE, '\0' );
   }

   return buf;
}


char *html_format( char *argument )
{
   int index = 0, bufcount = 0;
   char bad_chars[] = { '<', '>' };
   char rep_chars[][MAX_NEWS_LENGTH] = { "&#60;", "&#62;" };
   static char buf[MAX_HTML_LENGTH];

   if( !argument || argument[0] == '\0' )
   {
      return "";
   }

   buf[0] = '\0';

   while( *argument != '\0' )
   {
      bool cFound = FALSE;

      for( index = 0; index < strlen( bad_chars ); index++ )
      {
         if( *argument == bad_chars[index] )
         {
            cFound = TRUE;
            break;
         }
      }

      if( cFound )
      {
         int temp;
         char new_char[MAX_HTML_LENGTH];

         sprintf( new_char, "%s", rep_chars[index] );

         for( temp = 0; temp < strlen( new_char ); temp++, bufcount++ )
         {
            buf[bufcount] = new_char[temp];
         }
      }
      else
      {
         buf[bufcount] = *argument;
         bufcount++;
      }

      argument++;
   }

   buf[bufcount] = '\0';
   return buf;
}
