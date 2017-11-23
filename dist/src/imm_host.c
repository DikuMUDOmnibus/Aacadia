/************************************************************************/
/*        Acadia code team (C) 2002-2005 Kara Enterprises                   */
/************************************************************************/
/*		Immortal Host module			                    */
/************************************************************************/
/*****************************************************
 * **     _________       __			    **
 * **     \_   ___ \_____|__| _____  ________  ___     **
 * **	/    \	\/_  __ \ |/	 \/  ___/_ \/	\   **
 * **	\     \___|  | \/ |  | |  \___ \  / ) |  \  **
 * **	 \______  /__| |__|__|_|  /____ \__/__|  /  **
 * **	   ____\/____ _        \/ ___ \/      \/    **
 * **	   \______   \ |_____  __| _/___	    **
 * **	    |	 |  _/ |\__  \/ __ | __ \	    **
 * **	    |	 |   \ |_/ __ \  / | ___/_	    **
 * **	    |_____  /__/____  /_  /___	/	    **
 * **		 \/Antipode\/  \/    \/ 	    **
 * ******************************************************
 * **	   Crimson Blade Codebase (CbC) 	    **
 * **     (c) 2000-2002 John Bellone (Noplex)	    **
 * **	     Coders: Noplex, Krowe		    **
 * **	  http://www.crimsonblade.org		    **
 * ******************************************************
 * ** Based on SMAUG 1.4a, by; Thoric, Altrag, Blodkai **
 * **  Narn, Haus, Scryn, Rennard, Swordbearer, Gorog  **
 * **    Grishnakh, Nivek, Tricops, and Fireblade	    **
 * ******************************************************
 * ** Merc 2.1 by; Michael Chastain, Michael Quan, and **
 * **		    Mitchell Tse		    **
 * ******************************************************
 * **   DikuMUD by; Sebastian Hammer, Michael Seifert, **
 * **     Hans Staerfeldt, Tom Madsen and Katja Nyobe  **
 * *****************************************************/

/**********************************************
 * **	   Advanced Immortal Host	     **
 * ***********************************************
 * 	 By Noplex, Samson and Senir
 * 	 **********************************************/

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "acadia.h"

#if defined(KEY)
#undef KEY
#endif
#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}
#if defined(FCLOSE)
#undef FCLOSE
#endif
#define FCLOSE(fp) fclose(fp); fp=NULL;

void load_imm_host(  )
{
   FILE *fp;

   first_imm_host = NULL;
   last_imm_host = NULL;

   if( ( fp = fopen( IMM_HOST_FILE, "r" ) ) == NULL )
   {
      bug( "load_imm_host(): could not open immhost file for reading" );
      return;
   }

   for( ;; )
   {
      char letter = fread_letter( fp );
      char *word;

      if( letter == '*' )
      {
         fread_to_eol( fp );
         continue;
      }

      if( letter != '#' )
      {
         bug( "load_imm_host(): # not found" );
         break;
      }

      word = fread_word( fp );

      if( !str_cmp( word, "IMMORTAL" ) )
      {
         IMMORTAL_HOST *host = NULL;

         if( ( host = fread_imm_host( fp ) ) == NULL )
         {
            bug( "load_imm_host(): incomplete immhost" );
            continue;
         }
         LINK( host, first_imm_host, last_imm_host, next, prev );
         continue;
      }
      else if( !str_cmp( word, "END" ) )
         break;
      else
      {
         bug( "load_imm_host(): unknown section %s", word );
         continue;
      }
   }
   FCLOSE( fp );
   return;
}

IMMORTAL_HOST *fread_imm_host( FILE * fp )
{
   IMMORTAL_HOST *host = NULL;
   IMMORTAL_HOST_LOG *hlog = NULL, *hlognext = NULL;
   char *word;
   bool fMatch = FALSE;
   sh_int dnum = 0;

   CREATE( host, IMMORTAL_HOST, 1 );

   for( ;; )
   {
      word = feof( fp ) ? "ZEnd" : fread_word( fp );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
         case '*':
            fread_to_eol( fp );
            break;

         case 'D':
            if( !str_cmp( word, "Domain_host" ) )
            {
               if( dnum >= MAX_DOMAIN )
                  bug( "fread_imm_host(): more saved domains than MAX_DOMAIN" );
               else
                  host->domain[dnum++] = fread_string( fp );

               fMatch = TRUE;
               break;
            }
            break;

         case 'L':
            if( !str_cmp( word, "LOG" ) )
            {
               if( ( hlog = fread_imm_host_log( fp ) ) == NULL )
                  bug( "fread_imm_host(): incomplete log returned" );
               else
                  LINK( hlog, host->first_log, host->last_log, next, prev );

               fMatch = TRUE;
               break;
            }
            break;

         case 'N':
            KEY( "Name", host->name, fread_string( fp ) );
            break;

         case 'Z':
            if( !str_cmp( word, "ZEnd" ) )
            {
               if( !host->name || host->name[0] == '\0' || !host->domain[0] || host->domain[0][0] == '\0' )
               {
                  if( host->name )
                     STRFREE( host->name );
                  for( dnum = 0; dnum < MAX_DOMAIN && host->domain[dnum] && host->domain[dnum][0] != '\0'; dnum++ )
                     STRFREE( host->domain[dnum] );

                  for( hlog = host->first_log; hlog; hlog = hlognext )
                  {
                     hlognext = hlog->next;
                     if( hlog->date )
                        STRFREE( hlog->date );
                     if( hlog->host )
                        STRFREE( hlog->host );

                     DISPOSE( hlog );
                  }
                  DISPOSE( host );
                  return NULL;
               }
               return host;
            }
            break;
      }

      if( !fMatch )
         bug( "fread_imm_host(): no match for %s", word );

   }
   return NULL;
}

IMMORTAL_HOST_LOG *fread_imm_host_log( FILE * fp )
{
   IMMORTAL_HOST_LOG *hlog = NULL;
   char *word;
   bool fMatch;

   CREATE( hlog, IMMORTAL_HOST_LOG, 1 );

   for( ;; )
   {
      word = feof( fp ) ? "LEnd" : fread_word( fp );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
         case '*':
            fread_to_eol( fp );
            break;

         case 'L':
            if( !str_cmp( word, "LEnd" ) )
            {
               if( hlog->date && hlog->date[0] != '\0' && hlog->host && hlog->host[0] != '\0' )
                  return hlog;

               if( hlog->date )
                  STRFREE( hlog->date );
               if( hlog->host );
               STRFREE( hlog->host );

               DISPOSE( hlog );
               return NULL;
            }

            KEY( "Log_Date", hlog->date, fread_string( fp ) );
            KEY( "Log_Host", hlog->host, fread_string( fp ) );
            break;
      }

      if( !fMatch )
         bug( "fread_imm_host_log(): no match for %s", word );
   }
   return NULL;
}

void save_imm_host(  )
{
   FILE *fp;
   IMMORTAL_HOST *host = NULL;

   if( ( fp = fopen( IMM_HOST_FILE, "w" ) ) == NULL )
   {
      bug( "load_imm_host(): could not open immhost file for writing" );
      return;
   }

   for( host = first_imm_host; host; host = host->next )
   {
      IMMORTAL_HOST_LOG *log = NULL;
      sh_int dnum = 0;

      fprintf( fp, "\n#IMMORTAL\n" );
      fprintf( fp, "Name	       %s~\n", host->name );

      for( dnum = 0; dnum < MAX_DOMAIN && host->domain[dnum] && host->domain[dnum][0] != '\0'; dnum++ )
         fprintf( fp, "Domain_Host       %s~\n", host->domain[dnum] );

      for( log = host->first_log; log; log = log->next )
      {
         fprintf( fp, "LOG\n" );
         fprintf( fp, "Log_Host	%s~\n", log->host );
         fprintf( fp, "Log_Date	%s~\n", log->date );
         fprintf( fp, "LEnd\n" );
      }
      fprintf( fp, "ZEnd\n" );
   }
   fprintf( fp, "#END\n" );
   FCLOSE( fp );
   return;
}

bool check_immortal_domain( CHAR_DATA * ch, char *host )
{
   IMMORTAL_HOST *ihost = NULL;
   IMMORTAL_HOST_LOG *log = NULL;
   char timestr[30];
   sh_int x = 0;

   for( ihost = first_imm_host; ihost; ihost = ihost->next )
   {
      if( !str_cmp( ihost->name, ch->name ) )
         break;
   }

   if( !ihost || !ihost->domain[0] || ihost->domain[0][0] == '\0' )
      return TRUE;

   for( x = 0; x < MAX_DOMAIN && ihost->domain[x] && ihost->domain[x][0] != '\0'; x++ )
   {

      bool suffix = FALSE, prefix = FALSE;
      char chost[50];
      sh_int s = 0, t = 0;

      if( ihost->domain[x][0] == '*' )
      {
         prefix = TRUE;
         t = 1;
      }

      while( ihost->domain[x][t] != '\0' )
         chost[s++] = ihost->domain[x][t++];

      chost[s] = '\0';

      if( chost[strlen( chost ) - 1] == '*' )
      {
         chost[strlen( chost ) - 1] = '\0';
         suffix = TRUE;
      }

      if( ( prefix && suffix && !str_infix( ch->desc->host, chost ) )
          || ( prefix && !str_suffix( chost, ch->desc->host ) )
          || ( suffix && !str_prefix( chost, ch->desc->host ) ) || ( !str_cmp( chost, ch->desc->host ) ) )
      {
         sprintf( log_buf, "&C&GImmotal_Host: %s's host authorized.", ch->name );
         log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
         return TRUE;
      }
   }

   sprintf( log_buf, "&C&RImmortal_Host: %s's host denied. This hacking attempt has been logged.", ch->name );
   log_string_plus( log_buf, LOG_COMM, sysdata.log_level );

   CREATE( log, IMMORTAL_HOST_LOG, 1 );
   log->host = STRALLOC( host );
   sprintf( timestr, "%.24s", ctime( &current_time ) );
   log->date = STRALLOC( timestr );
   LINK( log, ihost->first_log, ihost->last_log, next, prev );

   save_imm_host(  );
   send_to_char( "You have been caught attempting to hack an immortal's character and have been logged.\n\r", ch );
   return FALSE;
}

void do_immhost( CHAR_DATA * ch, char *argument )
{
   IMMORTAL_HOST *host = NULL;
   IMMORTAL_HOST_LOG *hlog = NULL;
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   sh_int x = 0;

   if( IS_NPC( ch ) || !IS_IMMORTAL( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if( !arg || arg[0] == '\0' )
   {
      send_to_char_color( "&C&RSyntax: &Gimmhost list\n\r"
                          "&RSyntax: &Gimmhost add <&rcharacter&G>\n\r"
                          "&RSyntax: &Gimmhost remove <&rcharacter&G>\n\r"
                          "&RSyntax: &Gimmhost viewlogs <&rcharacter&G>\n\r"
                          "&RSyntax: &Gimmhost removelog <&rcharacter&G> <&rlog number&G>\n\r"
                          "&RSyntax: &Gimmhost viewdomains <&rcharacter&G>\n\r"
                          "&RSyntax: &Gimmhost createdomain <&rcharacter&G> <&rhost&G>\n\r"
                          "&RSyntax: &Gimmhost removedomain <&rcharacter&G> <&rdomain number&G>\n\r", ch );
      return;
   }

   if( !str_cmp( arg, "list" ) )
   {

      if( !first_imm_host || !last_imm_host )
      {
         send_to_char( "No immortals are currently protected at this time.\n\r", ch );
         return;
      }

      send_to_pager_color( "&C&R[&GName&R]     [&GDomains&R]  [&GLogged Attempts&R]\n\r", ch );

      for( host = first_imm_host; host; host = host->next, x++ )
      {
         sh_int lnum = 0, dnum = 0;

         while( dnum < MAX_DOMAIN && host->domain[dnum] && host->domain[dnum][0] != '\0' )
            dnum++;
         for( hlog = host->first_log; hlog; hlog = hlog->next )
            lnum++;

         pager_printf_color( ch, "&C&G%-10s %-10d %d\n\r", host->name, dnum, lnum );
      }

      pager_printf_color( ch, "&C&R%d immortals are being protected.&g\n\r", x );
      return;
   }

   argument = one_argument( argument, arg2 );

   if( !arg2 || arg2[0] == '\0' )
   {
      send_to_char( "Which character would you like to use?\n\r", ch );
      return;
   }

   if( !str_cmp( arg, "add" ) )
   {

      CREATE( host, IMMORTAL_HOST, 1 );

      smash_tilde( arg2 );
      host->name = STRALLOC( capitalize( arg2 ) );
      host->first_log = NULL;
      host->last_log = NULL;

      LINK( host, first_imm_host, last_imm_host, next, prev );

      save_imm_host(  );
      send_to_char( "Immortal host added.\n\r", ch );
      return;
   }

   for( host = first_imm_host; host; host = host->next )
   {
      if( !str_cmp( host->name, arg2 ) )
         break;
   }

   if( !host )
   {
      send_to_char( "There is no immortal host with that name.\n\r", ch );
      return;
   }

   if( !str_cmp( arg, "remove" ) )
   {
      IMMORTAL_HOST_LOG *nexthlog = NULL;

      UNLINK( host, first_imm_host, last_imm_host, next, prev );

      for( x = 0; x < MAX_DOMAIN && host->domain[x] && host->domain[x][0] != '\0'; x++ )
         STRFREE( host->domain[x] );

      for( hlog = host->first_log; hlog; hlog = nexthlog )
      {
         nexthlog = hlog->next;
         UNLINK( hlog, host->first_log, host->last_log, next, prev );
         if( hlog->host )
            STRFREE( hlog->host );
         if( hlog->date )
            STRFREE( hlog->date );
         DISPOSE( hlog );
      }

      if( host->name )
         STRFREE( host->name );
      DISPOSE( host );

      save_imm_host(  );
      send_to_char( "Immortal host removed.\n\r", ch );
      return;
   }

   if( !str_cmp( arg, "viewlogs" ) )
   {

      if( !host->first_log || !host->last_log )
      {
         send_to_char( "There are no logs for this immortal host.\n\r", ch );
         return;
      }

      pager_printf_color( ch, "&C&RImmortal:&W %s\n\r", host->name );
      send_to_pager_color( "&R[&GNum&R]  [&GLogged Host&R]     [&GDate&R]\n\r", ch );

      for( hlog = host->first_log; hlog; hlog = hlog->next )
         pager_printf_color( ch, "&C&G%-6d %-17s %s\n\r", ++x, hlog->host, hlog->date );

      pager_printf_color( ch, "&C&R%d logged hacking attempts.&g\n\r", x );
      return;
   }

   if( !str_cmp( arg, "removelog" ) )
   {
      if( !argument || argument[0] == '\0' || !is_number( argument ) )
      {
         send_to_char( "Syntax: immhost removelog <character> <log number>\n\r", ch );
         return;
      }

      for( hlog = host->first_log; hlog; hlog = hlog->next )
      {
         if( ++x == atoi( argument ) )
            break;
      }

      if( !hlog )
      {
         send_to_char( "That immortal host doesn't have a log with that number.\n\r", ch );
         return;
      }

      UNLINK( hlog, host->first_log, host->last_log, next, prev );

      if( hlog->host )
         STRFREE( hlog->host );
      if( hlog->date )
         STRFREE( hlog->date );
      DISPOSE( hlog );

      save_imm_host(  );
      send_to_char( "Log removed.\n\r", ch );
      return;
   }

   if( !str_cmp( arg, "viewdomains" ) )
   {

      send_to_pager_color( "&C&R[&GNum&R]	[&GHost&R]\n\r", ch );

      for( x = 0; x < MAX_DOMAIN && host->domain[x] && host->domain[x][0] != '\0'; x++ )
         pager_printf_color( ch, "&C&G%-5d	%s\n\r", x + 1, host->domain[x] );

      pager_printf_color( ch, "&C&R%d immortal domains.&g\n\r", x );
      return;
   }

   if( !str_cmp( arg, "createdomain" ) )
   {

      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Syntax: immhost createdomain <character> <host>\n\r", ch );
         return;
      }

      smash_tilde( argument );

      for( x = 0; x < MAX_DOMAIN && host->domain[x] && host->domain[x][0] != '\0'; x++ )
      {
         if( !strcmp( argument, host->domain[x] ) )
         {
            send_to_char( "That immortal host already has an entry like that.\n\r", ch );
            return;
         }

      }

      if( x == MAX_DOMAIN )
      {
         pager_printf( ch, "This immortal host has the maximum allowed, %d domains.\n\r", MAX_DOMAIN );
         return;
      }

      host->domain[x] = str_dup( argument );

      save_imm_host(  );
      send_to_char( "Done.\n\r", ch );
      return;
   }

   if( !str_cmp( arg, "removedomain" ) )
   {

      if( !argument || argument[0] == '\0' || !is_number( argument ) )
      {
         send_to_char( "Syntax: immhost removedomain <character> <domain number>\n\r", ch );
         return;
      }

      x = URANGE( 1, atoi( argument ), MAX_DOMAIN );
      x--;

      if( !host->domain[x] || host->domain[x][0] == '\0' )
      {
         send_to_char( "That immortal host doesn't have a domain with that number.\n\r", ch );
         return;
      }

      STRFREE( host->domain[x] );

      save_imm_host(  );
      send_to_char( "Domain removed.\n\r", ch );
      return;
   }
   do_immhost( ch, "" );
   return;
}
