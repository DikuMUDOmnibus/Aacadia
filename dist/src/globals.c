/************************************************************************/
/*        Acadia code team (C) 2002-2005 Kara Enterprises                   */
/************************************************************************/
/*				     Global Variables module		                    */
/************************************************************************/

#include <stdio.h>
#include <time.h>
#include "acadia.h"

FILE *MudError;

int tkills;
int tdeaths;
int tlevels;
int tpkills;
int tpdeaths;
int akills;
int adeaths;


void helphtml(  )
{
   HELP_DATA *help;
   FILE *file;

   file = fopen( "../../helpfiles.html", "w" );
   fprintf( file, "<HTML><HEAD><TITLE></TITLE></HEAD><BODY>" );
   for( help = first_help; help; help = help->next )
   {
      fprintf( file, "%d - %s<br>%s<br><HR align=center witdth=\"80%%\">", help->level, help->keyword, help->text );
   }
   fprintf( file, "</BODY></HTML>" );
   fclose( file );
}


char *friendly_ctime( time_t * time )
{
   static char *day[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
   static char *month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
   static char strtime[128];
   struct tm *timet = localtime( time );

   sprintf( strtime, "%3s %3s %2d %02d:%02d:%02d %cM %04d", day[timet->tm_wday], month[timet->tm_mon], timet->tm_mday,
            timet->tm_hour == 0 ? 12 : timet->tm_hour > 12 ? timet->tm_hour - 12 : timet->tm_hour, timet->tm_min,
            timet->tm_sec, timet->tm_hour < 12 ? 'A' : 'P', timet->tm_year + 1900 );
   return strtime;
}
