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
/*							Ban module				                    */
/************************************************************************/


#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include "acadia.h"

void fread_ban args( ( FILE * fp, int type ) );
bool check_expire args( ( BAN_DATA * ban ) );
void dispose_ban args( ( BAN_DATA * ban, int type ) );
void free_ban args( ( BAN_DATA * pban ) );


BAN_DATA *first_ban;
BAN_DATA *last_ban;
BAN_DATA *first_ban_class;
BAN_DATA *last_ban_class;
BAN_DATA *first_ban_race;
BAN_DATA *last_ban_race;


void load_banlist( void )
{
   char buf[MAX_STRING_LENGTH];
   char *word;
   FILE *fp;
   bool fMatch = FALSE;

   if( !( fp = fopen( SYSTEM_DIR BAN_LIST, "r" ) ) )
   {
      bug( "Save_banlist: Cannot open " BAN_LIST, 0 );
      perror( BAN_LIST );
      return;
   }
   for( ;; )
   {
      word = feof( fp ) ? "END" : fread_word( fp );
      fMatch = FALSE;
      switch ( UPPER( word[0] ) )
      {
         case 'C':
            if( !str_cmp( word, "CLASS" ) )
            {
               fread_ban( fp, BAN_CLASS );
               fMatch = TRUE;
            }
            break;
         case 'E':
            if( !str_cmp( word, "END" ) )
            {
               fclose( fp );
               log_string( "Done." );
               return;
            }
         case 'R':
            if( !str_cmp( word, "RACE" ) )
            {
               fread_ban( fp, BAN_RACE );
               fMatch = TRUE;
            }
            break;
         case 'S':
            if( !str_cmp( word, "SITE" ) )
            {
               fread_ban( fp, BAN_SITE );
               fMatch = TRUE;
            }
            break;
      }
      if( !fMatch )
      {
         sprintf( buf, "Load_banlist: no match: %s", word );
         bug( buf, 0 );
      }
   }
}


void fread_ban( FILE * fp, int type )
{
   BAN_DATA *pban;
   int i = 0;
   bool fMatch = FALSE;

   CREATE( pban, BAN_DATA, 1 );

   pban->name = fread_string_nohash( fp );
   pban->user = NULL;
   pban->level = fread_number( fp );
   pban->duration = fread_number( fp );
   pban->unban_date = fread_number( fp );
   if( type == BAN_SITE )
   {
      pban->prefix = fread_number( fp );
      pban->suffix = fread_number( fp );
   }
   pban->warn = fread_number( fp );
   pban->ban_by = fread_string_nohash( fp );
   pban->ban_time = fread_string_nohash( fp );
   pban->note = fread_string( fp );


   if( type == BAN_CLASS )
      for( i = 0; i < MAX_CLASS; i++ )
      {
         if( !str_cmp( class_table[i]->who_name, pban->name ) )
         {
            fMatch = TRUE;
            break;
         }
      }
   else if( type == BAN_RACE )
      for( i = 0; i < MAX_RACE; i++ )
      {
         if( !str_cmp( race_table[i]->race_name, pban->name ) )
         {
            fMatch = TRUE;
            break;
         }
      }
   else if( type == BAN_SITE )
      for( i = 0; i < strlen( pban->name ); i++ )
      {
         if( pban->name[i] == '@' )
         {
            char *temp;
            char *temp2;

            temp = str_dup( pban->name );
            temp[i] = '\0';
            temp2 = &pban->name[i + 1];
            DISPOSE( pban->name );
            pban->name = str_dup( temp2 );
            pban->user = str_dup( temp );
            DISPOSE( temp );
            break;
         }
      }

   if( type == BAN_RACE || type == BAN_CLASS )
   {
      if( fMatch )
         pban->flag = i;
      else
      {
         bug( "Bad class structure %d.\n\r", i );
         free_ban( pban );
         return;
      }
   }
   if( type == BAN_CLASS )
      LINK( pban, first_ban_class, last_ban_class, next, prev );
   else if( type == BAN_RACE )
      LINK( pban, first_ban_race, last_ban_race, next, prev );
   else if( type == BAN_SITE )
      LINK( pban, first_ban, last_ban, next, prev );
   else
   {
      bug( "Fread_ban: Bad type %d", type );
      free_ban( pban );
   }
   return;
}


void save_banlist( void )
{
   BAN_DATA *pban;
   FILE *fp;

   fclose( fpReserve );
   if( !( fp = fopen( SYSTEM_DIR BAN_LIST, "w" ) ) )
   {
      bug( "Save_banlist: Cannot open " BAN_LIST, 0 );
      perror( BAN_LIST );
      fpReserve = fopen( NULL_FILE, "r" );
      return;
   }


   for( pban = first_ban; pban; pban = pban->next )
   {
      fprintf( fp, "SITE\n" );
      if( pban->user )
         fprintf( fp, "%s@%s~\n", pban->user, pban->name );
      else
         fprintf( fp, "%s~\n", pban->name );
      fprintf( fp, "%d %d %d %d %d %d\n", pban->level, pban->duration,
               pban->unban_date, pban->prefix, pban->suffix, pban->warn );
      fprintf( fp, "%s~\n%s~\n%s~\n", pban->ban_by, pban->ban_time, pban->note );
   }


   for( pban = first_ban_race; pban; pban = pban->next )
   {
      fprintf( fp, "RACE\n" );
      fprintf( fp, "%s~\n", pban->name );
      fprintf( fp, "%d %d %d %d\n", pban->level, pban->duration, pban->unban_date, pban->warn );
      fprintf( fp, "%s~\n%s~\n%s~\n", pban->ban_by, pban->ban_time, pban->note );
   }


   for( pban = first_ban_class; pban; pban = pban->next )
   {
      fprintf( fp, "CLASS\n" );
      fprintf( fp, "%s~\n", pban->name );
      fprintf( fp, "%d %d %d %d\n", pban->level, pban->duration, pban->unban_date, pban->warn );
      fprintf( fp, "%s~\n%s~\n%s~\n", pban->ban_by, pban->ban_time, pban->note );
   }
   fprintf( fp, "END\n" );
   fclose( fp );
   fpReserve = fopen( NULL_FILE, "r" );
   return;
}


void do_ban( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   int time = -1;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Monsters are too dumb to do that!\n\r", ch );
      return;
   }

   if( !ch->desc )
   {
      bug( "do_ban: no descriptor", 0 );
      return;
   }

   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: ban <ip>\n\r  or\n\rSyntax: ban list\n\r", ch );
      return;
   }

   if( !str_cmp( arg, "list" ) )
   {
      show_bans( ch, BAN_SITE );
      return;
   }

   sprintf( arg2, "all" );

   if( !add_ban( ch, arg, arg2, time, BAN_SITE ) )
      return;
}

/*
void do_ban ( CHAR_DATA *ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  char arg4[MAX_INPUT_LENGTH];
  char *temp;
  BAN_DATA *pban;
  int value = 0, time;

  if (IS_NPC (ch))
  {
      send_to_char ("Monsters are too dumb to do that!\n\r", ch);
      return;
  }

  if (!ch->desc)
  {
      bug ("do_ban: no descriptor", 0);
      return;
  }

  set_char_color( AT_IMMORT, ch );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );
  argument = one_argument( argument, arg4 );


  if ( arg4[0] != '\0' && is_number( arg4 ) )
	time = atoi( arg4 );
  else
	time = -1;

  
  if ( time != -1 &&  ( time < 1 || time > 1000 ) )
  {
      send_to_char("Time value is -1 (forever) or from 1 to 1000.\n\r", ch);
      return;
  }


  switch ( ch->substate ) {
  default:
      bug ("do_ban: illegal substate", 0);
      return;
  case SUB_RESTRICTED:
      send_to_char ("You cannot use this command from within another command.\n\r", ch);
      return;
  case SUB_NONE:
	ch->tempnum = SUB_NONE;
	break;


   case SUB_BAN_DESC:
	add_ban( ch, "", "",0, 0 );
	return;
  }
  if ( arg1[0] == '\0' )
	goto syntax_message;


  if ( !str_cmp ( arg1, "site" ) ) {
	if ( arg2[0] == '\0' )
	{
		show_bans( ch, BAN_SITE );
		return;
	}


	if ( get_trust(ch) < sysdata.ban_site_level ) {
		ch_printf(ch,"You must be %d level to add bans.\n\r",sysdata.ban_site_level);
		return;
	}
	if ( arg3[0] == '\0' )
		goto syntax_message;
	if ( !add_ban ( ch, arg2, arg3, time, BAN_SITE ) )
		return;
  }
  else if ( !str_cmp ( arg1, "race" ) ) {
	if ( arg2[0] == '\0' )
	{
		show_bans( ch, BAN_RACE );
		return;
	}


	if ( get_trust(ch)  < sysdata.ban_race_level ) {
		ch_printf(ch,"You must be %d level to add bans.\n\r",sysdata.ban_race_level);
		return;
	}
	if ( arg3[0] == '\0' )
		goto syntax_message;
	if ( !add_ban(ch, arg2, arg3, time, BAN_RACE ) )
		return;
  }
  else if ( !str_cmp ( arg1, "class" ) ) {
	if ( arg2[0] == '\0' )
	{
		show_bans( ch, BAN_CLASS );
		return;
	}


	if ( get_trust(ch) < sysdata.ban_class_level ) {
		ch_printf(ch,"You must be %d level to add bans.\n\r",sysdata.ban_class_level);
		return;
	}
	if ( arg3[0] == '\0' )
		goto syntax_message;
	if ( !add_ban(ch, arg2, arg3, time, BAN_CLASS ) )
		return;
  }
  else if ( !str_cmp ( arg1, "show" ) ) {
	

	if ( arg2[0] == '\0' || arg3[0] == '\0' )
		goto syntax_message;
        temp = arg3;
  	if ( arg3[0] == '#' ) 
  	{
        	temp = arg3;
        	temp++;
        	if ( !is_number( temp ) )
        	{
                	send_to_char("Which ban # to show?\n\r", ch);
                	return;
        	}
        	value = atoi(temp);
               if ( value < 1 )
        	{
			send_to_char ("You must specify a number greater than 0.\n\r",ch);
			return;
		}
	  }
	if ( !str_cmp ( arg2, "site" ) ) 
        {
		pban = first_ban;
		if ( temp[0] == '*' )
			temp++;
		if ( temp[strlen(temp)-1] == '*' )
			temp[strlen(temp)-1] = '\0';
	}
	else if ( !str_cmp ( arg2, "class" ) )
		pban = first_ban_class;
	else if ( !str_cmp ( arg2, "race" ) )
		pban = first_ban_race;
	else 
		goto syntax_message;
	for ( ; pban; pban = pban->next ) 
	  if ( value == 1 || !str_cmp ( pban->name, temp ) )
		break;
	  else if ( value > 1 )
		value--;
		
	if ( !pban ) {
		send_to_char ("No such ban.\n\r", ch);
		return;
	}	
	ch_printf(ch, "Banned by: %s\n\r", pban->ban_by );
	send_to_char ( pban->note, ch );
	return;
  }
  else
	goto syntax_message;
  return;


syntax_message:
        send_to_char("Syntax: ban site  <address> <type> <duration>\n\r", ch );
        send_to_char("Syntax: ban race  <race>    <type> <duration>\n\r", ch );
        send_to_char("Syntax: ban class <class>   <type> <duration>\n\r", ch );
	send_to_char("Syntax: ban show  <field>   <number>\n\r", ch );
	send_to_char("Ban site lists current bans.\n\r", ch );
	send_to_char("Duration is the length of the ban in days.\n\r", ch );
	send_to_char("Type can be:  newbie, mortal, all, warn or level.\n\r", ch );
	send_to_char("In ban show, the <field> is site, race or class,", ch);
	send_to_char("  and the <number> is the ban number.\n\r", ch );
        return;
}

*/

void do_allow( CHAR_DATA * ch, char *argument )
{
   BAN_DATA *pban;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char *temp = NULL;
   bool fMatch = FALSE;
   int value = 0;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Monsters are too dumb to do that!\n\r", ch );
      return;
   }

   if( !ch->desc )
   {
      bug( "do_allow: no descriptor", 0 );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   set_char_color( AT_IMMORT, ch );

   if( arg1[0] == '\0' || arg2[0] == '\0' )
      goto syntax_message;
   if( arg2[0] == '#' )
   {
      temp = arg2;
      temp++;
      if( !is_number( temp ) )
      {
         send_to_char( "Which ban # to allow?\n\r", ch );
         return;
      }
      value = atoi( temp );
   }
   if( !str_cmp( arg1, "site" ) )
   {
      if( !value )
      {
         if( strlen( arg2 ) < 2 )
         {
            send_to_char( "You have to have at least 2 chars for a ban\n\r", ch );
            send_to_char( "If you are trying to allow by number use #\n\r", ch );
            return;
         }

         temp = arg2;
         if( arg2[0] == '*' )
            temp++;
         if( temp[strlen( temp ) - 1] == '*' )
            temp[strlen( temp ) - 1] = '\0';
      }

      for( pban = first_ban; pban; pban = pban->next )
      {

         if( value == 1 || !str_cmp( pban->name, temp ) )
         {
            fMatch = TRUE;
            dispose_ban( pban, BAN_SITE );
            break;
         }
         if( value > 1 )
            value--;
      }
   }
   else if( !str_cmp( arg1, "race" ) )
   {

      arg2[0] = toupper( arg2[0] );
      for( pban = first_ban_race; pban; pban = pban->next )
      {

         if( value == 1 || !str_cmp( pban->name, arg2 ) )
         {
            fMatch = TRUE;
            dispose_ban( pban, BAN_RACE );
            break;
         }
         if( value > 1 )
            value--;
      }
   }
   else if( !str_cmp( arg1, "class" ) )
   {

      arg2[0] = toupper( arg2[0] );
      for( pban = first_ban_class; pban; pban = pban->next )
      {

         if( value == 1 || !str_cmp( pban->name, arg2 ) )
         {
            fMatch = TRUE;
            dispose_ban( pban, BAN_CLASS );
            break;
         }
         if( value > 1 )
            value--;
      }
   }
   else
      goto syntax_message;

   if( fMatch )
   {
      save_banlist(  );
      ch_printf( ch, "%s is now allowed.\n\r", arg2 );
   }
   else
      ch_printf( ch, "%s was not banned.\n\r", arg2 );
   return;


 syntax_message:
   send_to_char( "Syntax: allow site  <address>\n\r", ch );
   send_to_char( "Syntax: allow race  <race>\n\r", ch );
   send_to_char( "Syntax: allow class <class>\n\r", ch );
   return;
}

void do_warn( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_STRING_LENGTH];
   char arg2[MAX_STRING_LENGTH];
   char *name;
   int count = -1, type;
   BAN_DATA *pban, *start, *end;


   if( IS_NPC( ch ) )
   {
      send_to_char( "Monsters are too dumb to do that!\n\r", ch );
      return;
   }

   if( !ch->desc )
   {
      bug( "do_warn: no descriptor", 0 );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' || arg2[0] == '\0' )
      goto syntax_message;

   if( arg2[0] == '#' )
   {
      name = arg2;
      name++;
      if( !is_number( name ) )
         goto syntax_message;
      count = atoi( name );
      if( count < 1 )
      {
         send_to_char( "The number has to be above 0.\n\r", ch );
         return;
      }
   }

   if( !str_cmp( arg1, "class" ) )
      type = BAN_CLASS;
   else if( !str_cmp( arg1, "race" ) )
      type = BAN_RACE;
   else if( !str_cmp( arg1, "site" ) )
      type = BAN_SITE;
   else
      type = -1;


   if( type == BAN_CLASS )
   {
      pban = first_ban_class;
      start = first_ban_class;
      end = last_ban_class;
      arg2[0] = toupper( arg2[0] );
   }
   else if( type == BAN_RACE )
   {
      pban = first_ban_race;
      start = first_ban_race;
      end = last_ban_race;
      arg2[0] = toupper( arg2[0] );
   }
   else if( type == BAN_SITE )
   {
      pban = first_ban;
      start = first_ban;
      end = last_ban;
   }
   else
      goto syntax_message;
   for( ; pban && count != 0; count--, pban = pban->next )
      if( count == -1 && !str_cmp( pban->name, arg2 ) )
         break;
   if( pban )
   {

      if( pban->warn )
      {
         if( pban->level == BAN_WARN )
         {
            dispose_ban( pban, type );
            send_to_char( "Warn has been deleted.\n\r", ch );
         }
         else
         {
            pban->warn = FALSE;
            send_to_char( "Warn turned off.\n\r", ch );
         }
      }
      else
      {
         pban->warn = TRUE;
         send_to_char( "Warn turned on.\n\r", ch );
      }
      save_banlist(  );
   }
   else
   {
      ch_printf( ch, "%s was not found in the ban list.\n\r", arg2 );
      return;
   }
   return;
 syntax_message:
   send_to_char( "Syntax: warn class <field>\n\r", ch );
   send_to_char( "Syntax: warn race  <field>\n\r", ch );
   send_to_char( "Syntax: warn site  <field>\n\r", ch );
   send_to_char( "Field is either #(ban_number) or the site/class/race.\n\r", ch );
   send_to_char( "Example:  warn class #1\n\r", ch );
   return;
}


int add_ban( CHAR_DATA * ch, char *arg1, char *arg2, int time, int type )
{
   char arg[MAX_STRING_LENGTH];
   char buf[MAX_STRING_LENGTH];
   BAN_DATA *pban, *temp;
   struct tm *tms;
   char *name;
   int level, i, value;


   switch ( ch->substate )
   {
      default:
         bug( "add_ban: illegal substate", 0 );
         return 0;
      case SUB_RESTRICTED:
         send_to_char( "You cannot use this command from within another command.\n\r", ch );
         return 0;
      case SUB_NONE:
      {
         one_argument( arg1, arg );
         smash_tilde( arg );

         if( arg[0] == '\0' || arg2[0] == '\0' )
            return 0;

         if( is_number( arg2 ) )
         {
            level = atoi( arg2 );
            if( level < 0 || level > LEVEL_ADMINADVISOR )
            {
               ch_printf( ch, "Level range is from 0 to %d.\n\r", LEVEL_ADMINADVISOR );
               return 0;
            }
         }
         else if( !str_cmp( arg2, "all" ) )
            level = LEVEL_ADMINADVISOR;
         else if( !str_cmp( arg2, "newbie" ) )
            level = 1;
         else if( !str_cmp( arg2, "mortal" ) )
            level = LEVEL_AVATAR;
         else if( !str_cmp( arg2, "warn" ) )
            level = BAN_WARN;
         else
         {
            bug( "Bad string for flag in add_ban.", 0 );
            return 0;
         }

         switch ( type )
         {
            case BAN_CLASS:
               if( arg[0] == '\0' )
                  return 0;
               if( is_number( arg ) )
                  value = atoi( arg );
               else
               {
                  for( i = 0; i < MAX_CLASS; i++ )
                     if( !str_cmp( class_table[i]->who_name, arg ) )
                        break;
                  value = i;
               }
               if( value < 0 || value >= MAX_CLASS )
               {
                  send_to_char( "Unknown class.\n\r", ch );
                  return 0;
               }
               for( temp = first_ban_class; temp; temp = temp->next )
               {
                  if( temp->flag == value )
                  {
                     if( temp->level == level )
                     {
                        send_to_char( "That entry already exists.\n\r", ch );
                        return 0;
                     }
                     else
                     {
                        temp->level = level;
                        if( temp->level == BAN_WARN )
                           temp->warn = TRUE;
                        sprintf( buf, "%24.24s", ctime( &current_time ) );
                        temp->ban_time = str_dup( buf );
                        if( temp->ban_by )
                           DISPOSE( temp->ban_by );
                        temp->ban_by = str_dup( ch->name );
                        send_to_char( "Updated entry.\n\r", ch );
                        return 1;
                     }
                  }
               }
               CREATE( pban, BAN_DATA, 1 );
               pban->name = str_dup( class_table[value]->who_name );
               pban->flag = value;
               pban->level = level;
               pban->ban_by = str_dup( ch->name );
               LINK( pban, first_ban_class, last_ban_class, next, prev );
               break;
            case BAN_RACE:
               if( is_number( arg ) )
                  value = atoi( arg );
               else
               {
                  for( i = 0; i < MAX_RACE; i++ )
                     if( !str_cmp( race_table[i]->race_name, arg ) )
                        break;
                  value = i;
               }
               if( value < 0 || value >= MAX_RACE )
               {
                  send_to_char( "Unknown race.\n\r", ch );
                  return 0;
               }
               for( temp = first_ban_race; temp; temp = temp->next )
               {
                  if( temp->flag == value )
                  {
                     if( temp->level == level )
                     {
                        send_to_char( "That entry already exists.\n\r", ch );
                        return 0;
                     }
                     else
                     {
                        temp->level = level;
                        if( temp->level == BAN_WARN )
                           temp->warn = TRUE;
                        sprintf( buf, "%24.24s", ctime( &current_time ) );
                        temp->ban_time = str_dup( buf );
                        if( temp->ban_by )
                           DISPOSE( temp->ban_by );
                        temp->ban_by = str_dup( ch->name );
                        send_to_char( "Updated entry.\n\r", ch );
                        return 1;
                     }
                  }
               }
               CREATE( pban, BAN_DATA, 1 );
               pban->name = str_dup( race_table[value]->race_name );
               pban->flag = value;
               pban->level = level;
               pban->ban_by = str_dup( ch->name );
               LINK( pban, first_ban_race, last_ban_race, next, prev );
               break;
            case BAN_SITE:
            {
               bool prefix = FALSE, suffix = FALSE, user_name = FALSE;
               char *temp_host = NULL, *temp_user = NULL;
               int x;

               for( x = 0; x < strlen( arg ); x++ )
               {
                  if( arg[x] == '@' )
                  {
                     user_name = TRUE;
                     temp_host = str_dup( &arg[x + 1] );
                     arg[x] = '\0';
                     temp_user = str_dup( arg );
                     break;
                  }
               }
               if( !user_name )
                  name = arg;
               else
                  name = temp_host;
               if( name[0] == '*' )
               {
                  prefix = TRUE;
                  name++;
               }

               if( name[strlen( name ) - 1] == '*' )
               {
                  suffix = TRUE;
                  name[strlen( name ) - 1] = '\0';
               }
               for( temp = first_ban; temp; temp = temp->next )
               {
                  if( !str_cmp( temp->name, name ) )
                  {
                     if( temp->level == level && ( prefix && temp->prefix )
                         && ( suffix && temp->suffix ) && ( !user_name ||
                                                            ( user_name && !str_cmp( temp->user, temp_user ) ) ) )
                     {
                        send_to_char( "That entry already exists.\n\r", ch );
                        return 0;
                     }
                     else
                     {
                        temp->suffix = suffix;
                        temp->prefix = prefix;
                        if( temp->level == BAN_WARN )
                           temp->warn = TRUE;
                        temp->level = level;
                        sprintf( buf, "%24.24s", ctime( &current_time ) );
                        temp->ban_time = str_dup( buf );
                        if( temp->ban_by )
                           DISPOSE( temp->ban_by );
                        if( user_name )
                        {
                           DISPOSE( temp_host );
                           DISPOSE( temp_user );
                        }
                        temp->ban_by = str_dup( ch->name );
                        send_to_char( "Updated entry.\n\r", ch );
                        return 1;
                     }
                  }
               }
               CREATE( pban, BAN_DATA, 1 );
               pban->ban_by = str_dup( ch->name );
               pban->suffix = suffix;
               pban->prefix = prefix;
               pban->name = str_dup( name );
               pban->level = level;
               if( user_name )
               {
                  pban->user = str_dup( temp_user );
                  DISPOSE( temp_host );
                  DISPOSE( temp_user );
               }
               LINK( pban, first_ban, last_ban, next, prev );
               break;
            }
            default:
               bug( "Bad type in add_ban: %d.", type );
               return 0;
         }
         sprintf( buf, "%24.24s", ctime( &current_time ) );
         pban->ban_time = str_dup( buf );
         if( time > 0 )
         {
            pban->duration = time;
            tms = localtime( &current_time );
            tms->tm_mday += time;
            pban->unban_date = mktime( tms );
         }
         else
         {
            pban->duration = -1;
            pban->unban_date = -1;
         }
         if( pban->level == BAN_WARN )
            pban->warn = TRUE;
         ch->substate = SUB_BAN_DESC;
         ch->dest_buf = pban;
         if( !pban->note )
            pban->note = STRALLOC( "" );;
         start_editing( ch, pban->note );
         return 1;
      }
      case SUB_BAN_DESC:
         pban = ch->dest_buf;
         if( !pban )
         {
            bug( "do_ban: sub_ban_desc: NULL ch->dest_buf", 0 );
            ch->substate = SUB_NONE;
            return 0;
         }
         if( pban->note )
            STRFREE( pban->note );
         pban->note = copy_buffer( ch );
         stop_editing( ch );
         ch->substate = ch->tempnum;
         save_banlist(  );
         if( pban->duration > 0 )
         {
            if( !pban->user )
               ch_printf( ch, "%s banned for %d days.\n\r", pban->name, pban->duration );
            else
               ch_printf( ch, "%s@%s banned for %d days.\n\r", pban->user, pban->name, pban->duration );
         }
         else
         {
            if( !pban->user )
               ch_printf( ch, "%s banned forever.\n\r", pban->name );
            else
               ch_printf( ch, "%s@%s banned forever.\n\r", pban->user, pban->name );
         }
         return 1;
   }
   return 1;
}


void show_bans( CHAR_DATA * ch, int type )
{
   BAN_DATA *pban;
   int bnum;

   set_pager_color( AT_IMMORT, ch );

   switch ( type )
   {
      case BAN_SITE:
         send_to_pager( "Banned sites:\n\r", ch );
         send_to_pager( "[ #] Warn (Lv) Time                     By              For   Site\n\r", ch );
         send_to_pager( "---- ---- ---- ------------------------ --------------- ----  ---------------\n\r", ch );
         pban = first_ban;
         set_pager_color( AT_PLAIN, ch );
         for( bnum = 1; pban; pban = pban->next, bnum++ )
         {
            if( !pban->user )
               pager_printf( ch, "[%2d] %-4s (%2d) %-24s %-15s %4d  %c%s%c\n\r",
                             bnum, ( pban->warn ) ? "YES" : "no", pban->level,
                             pban->ban_time, pban->ban_by, pban->duration,
                             ( pban->prefix ) ? '*' : ' ', pban->name, ( pban->suffix ) ? '*' : ' ' );
            else
               pager_printf( ch, "[%2d] %-4s (%2d) %-24s %-15s %4d  %s@%c%s%c\n\r",
                             bnum, ( pban->warn ) ? "YES" : "no", pban->level,
                             pban->ban_time, pban->ban_by, pban->duration,
                             pban->user, ( pban->prefix ) ? '*' : ' ', pban->name, ( pban->suffix ) ? '*' : ' ' );
         }
         return;
      case BAN_RACE:
         send_to_pager( "Banned races:\n\r", ch );
         send_to_pager( "[ #] Warn (Lv) Time                     By              For   Race\n\r", ch );
         pban = first_ban_race;
         break;
      case BAN_CLASS:
         send_to_pager( "Banned classes:\n\r", ch );
         send_to_pager( "[ #] Warn (Lv) Time                     By              For   Class\n\r", ch );
         pban = first_ban_class;
         break;
      default:
         bug( "Bad type in show_bans: %d", type );
         return;
   }
   send_to_pager( "---- ---- ---- ------------------------ --------------- ----  ---------------\n\r", ch );
   set_pager_color( AT_PLAIN, ch );
   for( bnum = 1; pban; pban = pban->next, bnum++ )
      pager_printf( ch, "[%2d] %-4s (%2d) %-24s %-15s %4d  %s\n\r", bnum,
                    ( pban->warn ) ? "YES" : "no", pban->level, pban->ban_time, pban->ban_by, pban->duration, pban->name );
   return;
}


bool check_total_bans( DESCRIPTOR_DATA * d )
{
   BAN_DATA *pban;
   char new_host[MAX_STRING_LENGTH];
   int i;

   for( i = 0; i < ( int )strlen( d->host ); i++ )
      new_host[i] = LOWER( d->host[i] );
   new_host[i] = '\0';

   for( pban = first_ban; pban; pban = pban->next )
   {
      if( pban->level != LEVEL_ADMINADVISOR )
         continue;
      if( pban->user && str_cmp( d->user, pban->user ) )
         continue;
      if( pban->prefix && pban->suffix && strstr( pban->name, new_host ) )
      {
         if( check_expire( pban ) )
         {
            dispose_ban( pban, BAN_SITE );
            save_banlist(  );
            return FALSE;
         }
         else
            return TRUE;
      }
      if( pban->suffix && !str_prefix( pban->name, new_host ) )
      {
         if( check_expire( pban ) )
         {
            dispose_ban( pban, BAN_SITE );
            save_banlist(  );
            return FALSE;
         }
         else
            return TRUE;
      }
      if( pban->prefix && !str_suffix( pban->name, new_host ) )
      {
         if( check_expire( pban ) )
         {
            dispose_ban( pban, BAN_SITE );
            save_banlist(  );
            return FALSE;
         }
         else
            return TRUE;
      }
      if( !str_cmp( pban->name, new_host ) )
      {
         if( check_expire( pban ) )
         {
            dispose_ban( pban, BAN_SITE );
            save_banlist(  );
            return FALSE;
         }
         else
            return TRUE;
      }
   }
   return FALSE;
}


bool check_bans( CHAR_DATA * ch, int type )
{
   char buf[MAX_STRING_LENGTH];
   BAN_DATA *pban;
   char new_host[MAX_STRING_LENGTH];
   int i;
   bool fMatch = FALSE;

   switch ( type )
   {
      case BAN_RACE:
         pban = first_ban_race;
         break;
      case BAN_CLASS:
         pban = first_ban_class;
         break;
      case BAN_SITE:
         pban = first_ban;
         for( i = 0; i < ( int )( strlen( ch->desc->host ) ); i++ )
            new_host[i] = LOWER( ch->desc->host[i] );
         new_host[i] = '\0';
         break;
      default:
         bug( "Ban type in check_bans: %d.", type );
         return FALSE;
   }
   for( ; pban; pban = pban->next )
   {
      if( type == BAN_CLASS && pban->flag == ch->class )
      {
         if( check_expire( pban ) )
         {
            dispose_ban( pban, BAN_CLASS );
            save_banlist(  );
            return FALSE;
         }
         if( ch->level > pban->level )
         {
            if( pban->warn )
            {
               sprintf( buf, "%s class logging in from %s.", pban->name, ch->desc->host );
               log_string_plus( buf, LOG_WARN, sysdata.log_level );
            }
            return FALSE;
         }
         else
            return TRUE;
      }
      if( type == BAN_RACE && pban->flag == ch->race )
      {
         if( check_expire( pban ) )
         {
            dispose_ban( pban, BAN_RACE );
            save_banlist(  );
            return FALSE;
         }
         if( ch->level > pban->level )
         {
            if( pban->warn )
            {
               sprintf( buf, "%s race logging in from %s.", pban->name, ch->desc->host );
               log_string_plus( buf, LOG_WARN, sysdata.log_level );
            }
            return FALSE;
         }
         else
            return TRUE;
      }
      if( type == BAN_SITE )
      {
         if( pban->prefix && pban->suffix && strstr( pban->name, new_host ) )
            fMatch = TRUE;
         else if( pban->prefix && !str_suffix( pban->name, new_host ) )
            fMatch = TRUE;
         else if( pban->suffix && !str_prefix( pban->name, new_host ) )
            fMatch = TRUE;
         else if( !str_cmp( pban->name, new_host ) )
            fMatch = TRUE;
         if( fMatch && pban->user && str_cmp( pban->user, ch->desc->user ) )
            fMatch = FALSE;
         if( fMatch )
         {
            if( check_expire( pban ) )
            {
               dispose_ban( pban, BAN_SITE );
               save_banlist(  );
               return FALSE;
            }
            if( ch->level > pban->level )
            {
               if( pban->warn )
               {
                  sprintf( buf, "%s logging in from site %s.", ch->name, ch->desc->host );
                  log_string_plus( buf, LOG_WARN, sysdata.log_level );
               }
               return FALSE;
            }
            else
               return TRUE;
         }
      }
   }
   return FALSE;
}

bool check_expire( BAN_DATA * pban )
{
   char buf[MAX_STRING_LENGTH];

   if( pban->unban_date < 0 )
      return FALSE;
   if( pban->unban_date <= current_time )
   {
      sprintf( buf, "%s ban has expired.", pban->name );
      log_string_plus( buf, LOG_WARN, sysdata.log_level );
      return TRUE;
   }
   return FALSE;
}

void dispose_ban( BAN_DATA * pban, int type )
{
   if( !pban )
      return;

   if( type != BAN_SITE && type != BAN_CLASS && type != BAN_RACE )
   {
      bug( "Dispose_ban: Unknown Ban Type %d.", type );
      return;
   }

   switch ( type )
   {
      case BAN_SITE:
         UNLINK( pban, first_ban, last_ban, next, prev );
         break;
      case BAN_CLASS:
         UNLINK( pban, first_ban_class, last_ban_class, next, prev );
         break;
      case BAN_RACE:
         UNLINK( pban, first_ban_race, last_ban_race, next, prev );
         break;
   }
   free_ban( pban );
   return;
}

void free_ban( BAN_DATA * pban )
{
   if( pban->name )
      DISPOSE( pban->name );
   if( pban->ban_time )
      DISPOSE( pban->ban_time );
   if( pban->note )
      STRFREE( pban->note );
   if( pban->user )
      DISPOSE( pban->user );
   if( pban->ban_by )
      DISPOSE( pban->ban_by );
   if( pban->ban_time )
      DISPOSE( pban->ban_time );
   DISPOSE( pban );
}

void do_showban( CHAR_DATA * ch, char *argument )
{
   if( ch->level < LEVEL_HEADBUILD )
   {
      send_to_char( "You can't see those.", ch );
      return;
   }

   show_bans( ch, BAN_SITE );
   return;
}
