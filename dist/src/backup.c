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
/*					    Backup module					                */
/************************************************************************/

#include <string.h>
#include <time.h>
#include "acadia.h"

char *backup_fname_strings[] = {
   "pfiles_backup.tgz", "system_backup.tgz", "area_backup.tgz", "build_backup.tgz"
};

char *backup_directory_strings[] = {
   "../player/", "../system/", "../area/", "../build/", "../backup/"
};

typedef enum
{
   BACKUP_PFILES, BACKUP_SYSTEM, BACKUP_AREAS, BACKUP_BUILD, BACKUP_DIRECTORY
}
backup_types;

void do_backup args( ( CHAR_DATA * ch, char *argument ) );
void backup args( ( CHAR_DATA * ch, int type ) );

void do_backup( CHAR_DATA * ch, char *argument )
{
   if( !IS_IMMORTAL( ch ) || IS_NPC( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      send_to_char( "Syntax: backup <type>\n\r", ch );
      send_to_char( "Types being one of the following:\n\r" " pfiles systemfiles areas buildfiles\n\r", ch );
      return;
   }

   if( !str_cmp( argument, "pfiles" ) )
   {
      backup( ch, BACKUP_PFILES );
      send_to_char( "Backup complete.\n\r", ch );
      return;
   }
   else if( !str_cmp( argument, "systemfiles" ) || !str_cmp( argument, "system" ) )
   {
      backup( ch, BACKUP_SYSTEM );
      send_to_char( "Backup complete.\n\r", ch );
      return;
   }
   else if( !str_cmp( argument, "area" ) || !str_cmp( argument, "areas" ) )
   {
      backup( ch, BACKUP_AREAS );
      send_to_char( "Backup complete.\n\r", ch );
      return;
   }
   else if( !str_cmp( argument, "build" ) || !str_cmp( argument, "buildfiles" ) )
   {
      backup( ch, BACKUP_BUILD );
      send_to_char( "Backup complete.\n\r", ch );
      return;
   }
   do_backup( ch, "" );
   return;
}

void backup( CHAR_DATA * ch, int type )
{
   char cmd[1024];
   char buf[MAX_INPUT_LENGTH];

   sprintf( cmd, "tar -zcf %s %s", backup_fname_strings[type], backup_directory_strings[type] );
   system( cmd );

   sprintf( cmd, "mv %s %s", backup_fname_strings[type], backup_directory_strings[BACKUP_DIRECTORY] );
   system( cmd );

   sprintf( buf, "BACKUP(%s): %s file written, moved to backup directory.", ch->name, backup_fname_strings[type] );
   log_string( buf );
   return;
}
