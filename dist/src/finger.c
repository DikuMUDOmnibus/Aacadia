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
/*			  Finger Header                                 */
/************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "acadia.h"

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                                    \
                                if ( !strcmp( word, literal ) ) \
                                {                                       \
                                    field  = value;                     \
                                    fMatch = TRUE;                      \
                                    break;                              \
                                }

WIZINFO_DATA *first_wizinfo;
WIZINFO_DATA *last_wizinfo;

CHAR_DATA *finger_char( char *player )
{
   FILE *fp;
   CHAR_DATA *fch;
   char *word;
   bool fMatch;

   CREATE( fch, CHAR_DATA, 1 );
   CREATE( fch->pcdata, PC_DATA, 1 );
   clear_char( fch );
   fch->name = NULL;
   fch->played = 0;
   fch->sex = 0;
   fch->level = 0;
   fch->sublevel = -1;
   fch->class = 0;
   fch->dualclass = -1;
   fch->race = 0;
   fch->pcdata->flags = 0;
   fch->pcdata->bio = NULL;
   fch->pcdata->authed_by = NULL;
   fch->pcdata->email = NULL;
   fch->pcdata->icq = 0;
   fch->pcdata->title = NULL;
   fch->pcdata->pretit = NULL;
   fch->pcdata->pretiti = 0;

   if( ( fp = fopen( player, "r" ) ) == NULL )
   {
      DISPOSE( fch->pcdata );
      DISPOSE( fch );
      return NULL;
   }

   for( ;; )
   {
      word = feof( fp ) ? "End" : fread_word( fp );
      fMatch = FALSE;
      switch ( UPPER( word[0] ) )
      {
         case '*':
            fMatch = TRUE;
            fread_to_eol( fp );
            break;

         case 'A':
            KEY( "Authedby", fch->pcdata->authed_by, fread_string( fp ) );
            break;

         case 'B':
            KEY( "Bio", fch->pcdata->bio, fread_string( fp ) );
            break;

         case 'C':
            KEY( "Class", fch->class, fread_number( fp ) );
            break;

         case 'D':
            KEY( "DualClass", fch->dualclass, fread_number( fp ) );
            break;

         case 'E':
            KEY( "Email", fch->pcdata->email, fread_string_nohash( fp ) );
            if( !strcmp( word, "End" ) )
               return fch;
            break;

         case 'F':
            KEY( "Flags", fch->pcdata->flags, fread_number( fp ) );
            break;

         case 'I':
            KEY( "ICQ", fch->pcdata->icq, fread_number( fp ) );
            break;

         case 'L':
            KEY( "Level", fch->level, fread_number( fp ) );
            break;

         case 'N':
            KEY( "Name", fch->name, fread_string( fp ) );
            break;

         case 'P':
            KEY( "Played", fch->played, fread_number( fp ) );
            KEY( "Pretit", fch->pcdata->pretit, fread_string( fp ) );
            KEY( "Pretiti", fch->pcdata->pretiti, fread_number( fp ) );
            break;

         case 'R':
            KEY( "Race", fch->race, fread_number( fp ) );
            break;

         case 'S':
            KEY( "Sex", fch->sex, fread_number( fp ) );
            KEY( "SubLevel", fch->sublevel, fread_number( fp ) );
            break;
         case 'T':
            KEY( "Title", fch->pcdata->title, fread_string( fp ) );
      }
   }
   fclose( fp );
   fp = NULL;
   return fch;

}

char *get_ip_from_pfile( FILE * fp );
void do_finger( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *vch;
   FILE *fp;
   struct stat fst;
   char filename[MAX_STRING_LENGTH];


   if( argument[0] == '\0' )
   {
      send_to_char( "Syntax: finger <who>\n\r", ch );
      return;
   }

   if( !str_cmp( argument, "self" ) )
      argument = ch->name;
   sprintf( filename, "%s%c/%c%s.plr", PLAYER_DIR, tolower( argument[0] ), UPPER( argument[0] ), argument + 1 );
   if( ( vch = finger_char( filename ) ) == NULL )
   {
      send_to_char( "No such character.\n\r", ch );
      return;
   }
   fp = fopen( filename, "r" );
   if( IS_IMMORTAL( vch ) && !IS_IMMORTAL( ch ) )
   {
      send_to_char( "You can't finger them.\n\r", ch );
      return;
   }
   if( IS_SET( vch->pcdata->flags, PCFLAG_PRIVACY ) && !IS_IMMORTAL( ch ) )
   {
      ch_printf( ch, "%s has privacy enabled.\n\r", vch->name );
      return;
   }

   send_to_char( "Finger Data\n\r", ch );
   send_to_char( "--------------------------------------------------\n\r", ch );
   ch_printf( ch, "&wName:&G      %-15.15s    &wRace:&G     %s&D\n\r", vch->name, get_race( vch ) );
   ch_printf( ch, "&wSex:&G       %-15.15s    &wAge:&G      %d&D\n\r", vch->sex == SEX_MALE ? "Male" : "Female",
              get_age( vch ) );
   ch_printf( ch, "&wClass:&G     %-15.15s    &wLevel:&G    %d&D\n\r", get_class( vch ), vch->level );
   if( IS_DUAL( vch ) )
      ch_printf( ch, "&wDualClass:&G %-15.15s    &wSublevel:&G %d&D\n\r", get_dualclass( vch ), vch->sublevel );
   if( vch->pcdata->pretiti == 1 )
      ch_printf( ch, "&wPretitle:&G  %s&D\n\r", vch->pcdata->pretit );
   ch_printf( ch, "&wTitle:&G     %s&D\n\r", vch->pcdata->title );
   ch_printf( ch, "&wICQ:&G       %-15d    &wEmail:&G    %s&D\n\r", vch->pcdata->icq, vch->pcdata->email );
   if( stat( filename, &fst ) != -1 )
      ch_printf( ch, "&wLast on:&G   %s\n\r", ctime( &fst.st_mtime ) );
   if( IS_IMMORTAL( ch ) )
   {
      send_to_char( "&wImmortal Information\n\r", ch );
      send_to_char( "--------------------------------------------------\n\r", ch );
      ch_printf( ch, "&wPlayed:&G    %d hours.&D\n\r", ( ( get_age( vch ) - 17 ) * 2 ) );
      ch_printf( ch, "&wAuthed by:&G %s&D\n\r", vch->pcdata->authed_by ? vch->pcdata->authed_by : "System" );
      ch_printf( ch, "&wPrivacy:&G   %s&D\n\r", IS_SET( vch->pcdata->flags, PCFLAG_PRIVACY ) ? "Enabled" : "Disabled" );
      ch_printf( ch, "&wIP:&G        %s&D\n\r", get_ip_from_pfile( fp ) );
   }
   if( !vch->pcdata->bio )
      ch_printf( ch, "&w%s is yet to create a biography.\n\r", vch->name );
   else
      ch_printf( ch, "&wBiography:\n\r&G%s\n\r", vch->pcdata->bio );
   fclose( fp );
   fp = NULL;
   DISPOSE( vch->pcdata );
   DISPOSE( vch );
   return;
}

void add_to_wizinfo( char *name, WIZINFO_DATA * wiz )
{
   WIZINFO_DATA *wiz_prev;

   wiz->name = str_dup( name );
   if( !wiz->email )
      wiz->email = str_dup( "Not Set" );
   if( !wiz->aol )
      wiz->aol = STRALLOC( "Not Set" );
   if( !wiz->yahoo )
      wiz->yahoo = STRALLOC( "Not Set" );

   for( wiz_prev = first_wizinfo; wiz_prev; wiz_prev = wiz_prev->next )
      if( strcmp( wiz_prev->name, name ) >= 0 )
         break;

   if( !wiz_prev )
      LINK( wiz, first_wizinfo, last_wizinfo, next, prev );
   else
      INSERT( wiz, wiz_prev, first_wizinfo, next, prev );

   return;
}

void clear_wizinfo( bool bootup )
{
   WIZINFO_DATA *wiz, *next;

   if( !bootup )
   {
      for( wiz = first_wizinfo; wiz; wiz = next )
      {
         next = wiz->next;
         UNLINK( wiz, first_wizinfo, last_wizinfo, next, prev );
         DISPOSE( wiz->name );
         DISPOSE( wiz->email );
         STRFREE( wiz->aol );
         STRFREE( wiz->yahoo );
         DISPOSE( wiz );
      }
   }

   first_wizinfo = NULL;
   last_wizinfo = NULL;

   return;
}

void fread_wizinfo( WIZINFO_DATA * wiz, FILE * fp )
{
   char *word;
   bool fMatch;

   for( ;; )
   {
      word = feof( fp ) ? "End" : fread_word( fp );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
         case '*':
            fMatch = TRUE;
            fread_to_eol( fp );
            break;

         case 'A':
            KEY( "Aol", wiz->aol, fread_string( fp ) );
            break;

         case 'E':
            KEY( "Email", wiz->email, fread_string_nohash( fp ) );
            if( !str_cmp( word, "End" ) )
               return;
            break;

         case 'I':
            KEY( "ICQ", wiz->icq, fread_number( fp ) );
            break;

         case 'L':
            KEY( "Level", wiz->level, fread_number( fp ) );
            break;

         case 'Y':
            KEY( "Yahoo", wiz->yahoo, fread_string( fp ) );
            break;
      }
      if( !fMatch )
         fread_to_eol( fp );
   }
}

void build_wizinfo( bool bootup )
{
   DIR *dp = opendir( GOD_DIR );
   struct dirent *dentry = readdir( dp );
   FILE *fp;
   WIZINFO_DATA *wiz;
   char buf[MAX_STRING_LENGTH];

   clear_wizinfo( bootup );

   while( dentry )
   {
      if( dentry->d_name[0] != '.' )
      {
         sprintf( buf, "%s%s", GOD_DIR, dentry->d_name );
         fp = fopen( buf, "r" );
         if( fp )
         {
            CREATE( wiz, WIZINFO_DATA, 1 );
            fread_wizinfo( wiz, fp );
            add_to_wizinfo( dentry->d_name, wiz );
            FCLOSE( fp );
         }
      }
      dentry = readdir( dp );
   }
   closedir( dp );
   return;
}

void do_wizinfo( CHAR_DATA * ch, char *argument )
{
   WIZINFO_DATA *wiz;
   char buf[MAX_STRING_LENGTH];

   send_to_pager( "&gContact Information for the Immortals:\n\r\n\r", ch );
   send_to_pager
      ( "Name         Email Address                     ICQ#\n\r            AOL nickname         Yahoo nickname\n\r", ch );
   send_to_pager( "&C------------+---------------------------------+----------\n\r", ch );

   for( wiz = first_wizinfo; wiz; wiz = wiz->next )
   {
      sprintf( buf, "&W%-12s &Y%-33s &B%10d\n\r             &z%-20s &c%-20s\n\r", wiz->name, wiz->email, wiz->icq, wiz->aol,
               wiz->yahoo );
      strcat( buf, "&C---------------------------------------------------------\n\r" );
      send_to_pager( buf, ch );
   }
   return;
}

void do_email( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) )
      return;

   if( argument[0] == '\0' )
   {
      if( !ch->pcdata->email )
         ch->pcdata->email = str_dup( "" );
      ch_printf( ch, "Your email address is: %s\n\r", show_tilde( ch->pcdata->email ) );
      return;
   }

   if( !str_cmp( argument, "clear" ) )
   {
      if( ch->pcdata->email )
         DISPOSE( ch->pcdata->email );
      ch->pcdata->email = str_dup( "" );
      if( IS_IMMORTAL( ch ) );
      {
         save_char_obj( ch );
         build_wizinfo( FALSE );
      }
      send_to_char( "Email address cleared.\n\r", ch );
      return;
   }
   strcpy( buf, argument );
   if( strlen( buf ) > 70 )
      buf[70] = '\0';

   hide_tilde( buf );
   if( ch->pcdata->email )
      DISPOSE( ch->pcdata->email );
   ch->pcdata->email = str_dup( buf );
   if( IS_IMMORTAL( ch ) );
   {
      save_char_obj( ch );
      build_wizinfo( FALSE );
   }
   send_to_char( "Email address set.\n\r", ch );
}

void do_aol( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) )
      return;

   if( argument[0] == '\0' )
   {
      if( !ch->pcdata->aol )
         ch->pcdata->aol = STRALLOC( "" );
      ch_printf( ch, "Your AOL nickname is: %s\n\r", show_tilde( ch->pcdata->aol ) );
      return;
   }

   if( !str_cmp( argument, "clear" ) )
   {
      if( ch->pcdata->aol )
         STRFREE( ch->pcdata->aol );
      ch->pcdata->aol = STRALLOC( "" );
      if( IS_IMMORTAL( ch ) );
      {
         save_char_obj( ch );
         build_wizinfo( FALSE );
      }
      send_to_char( "AOL nickname cleared.\n\r", ch );
      return;
   }

   strcpy( buf, argument );
   if( strlen( buf ) > 70 )
      buf[70] = '\0';

   hide_tilde( buf );
   if( ch->pcdata->aol )
      STRFREE( ch->pcdata->aol );
   ch->pcdata->aol = STRALLOC( buf );
   if( IS_IMMORTAL( ch ) );
   {
      save_char_obj( ch );
      build_wizinfo( FALSE );
   }
   send_to_char( "AOL nickname set.\n\r", ch );
}

void do_yahoo( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) )
      return;

   if( argument[0] == '\0' )
   {
      if( !ch->pcdata->yahoo )
         ch->pcdata->yahoo = STRALLOC( "" );
      ch_printf( ch, "Your Yahoo nickname is: %s\n\r", show_tilde( ch->pcdata->yahoo ) );
      return;
   }

   if( !str_cmp( argument, "clear" ) )
   {
      if( ch->pcdata->yahoo )
         STRFREE( ch->pcdata->yahoo );
      ch->pcdata->yahoo = STRALLOC( "" );
      if( IS_IMMORTAL( ch ) );
      {
         save_char_obj( ch );
         build_wizinfo( FALSE );
      }
      send_to_char( "Yahoo nickname cleared.\n\r", ch );
      return;
   }

   strcpy( buf, argument );
   if( strlen( buf ) > 70 )
      buf[70] = '\0';

   hide_tilde( buf );
   if( ch->pcdata->yahoo )
      STRFREE( ch->pcdata->yahoo );
   ch->pcdata->yahoo = STRALLOC( buf );
   if( IS_IMMORTAL( ch ) );
   {
      save_char_obj( ch );
      build_wizinfo( FALSE );
   }
   send_to_char( "Yahoo nickname set.\n\r", ch );
}

void do_icq( CHAR_DATA * ch, char *argument )
{
   int icq;

   if( IS_NPC( ch ) )
      return;

   if( argument[0] == '\0' )
   {
      if( !ch->pcdata->icq )
         ch->pcdata->icq = 0;
      ch_printf( ch, "Your ICQ# is: %d\n\r", ch->pcdata->icq );
      return;
   }

   if( !str_cmp( argument, "clear" ) )
   {
      ch->pcdata->icq = 0;

      if( IS_IMMORTAL( ch ) );
      {
         save_char_obj( ch );
         build_wizinfo( FALSE );
      }
      send_to_char( "ICQ# cleared.\n\r", ch );
      return;
   }

   if( !is_number( argument ) )
   {
      send_to_char( "You must enter numeric data.\n\r", ch );
      return;
   }
   icq = atoi( argument );
   if( icq < 1 )
   {
      send_to_char( "Valid range is greater than 0.\n\r", ch );
      return;
   }
   ch->pcdata->icq = icq;
   if( IS_IMMORTAL( ch ) );
   {
      save_char_obj( ch );
      build_wizinfo( FALSE );
   }
   send_to_char( "ICQ# set.\n\r", ch );
   return;
}

void do_homepage( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) )
      return;

   if( !argument || argument[0] == '\0' )
   {
      if( !ch->pcdata->homepage )
         ch->pcdata->homepage = str_dup( "" );
      ch_printf( ch, "Your homepage is: %s\n\r", show_tilde( ch->pcdata->homepage ) );
      return;
   }
   if( !str_cmp( argument, "clear" ) )
   {
      if( ch->pcdata->homepage )
         DISPOSE( ch->pcdata->homepage );
      ch->pcdata->homepage = str_dup( "" );
      send_to_char( "Homepage cleared.\n\r", ch );
      return;
   }

   if( strstr( argument, "://" ) )
      strcpy( buf, argument );
   else
      sprintf( buf, "http://%s", argument );
   if( strlen( buf ) > 70 )
      buf[70] = '\0';

   hide_tilde( buf );
   if( ch->pcdata->homepage )
      DISPOSE( ch->pcdata->homepage );
   ch->pcdata->homepage = str_dup( buf );
   send_to_char( "Homepage set.\n\r", ch );
   return;
}

void do_privacy( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) )
   {
      send_to_char( "Mobs can't use the privacy toggle.\n\r", ch );
      return;
   }
   TOGGLE_BIT( ch->pcdata->flags, PCFLAG_PRIVACY );
   if( IS_SET( ch->pcdata->flags, PCFLAG_PRIVACY ) )
      send_to_char( "Privacy flag enabled.\n\r", ch );
   else
      send_to_char( "Privacy flag disabled.\n\r", ch );
   return;
}
