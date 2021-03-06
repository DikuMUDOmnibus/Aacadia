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
/*				 Command Interpretation module		                    */
/************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "acadia.h"
#ifdef USE_IMC
#include "icec-mercbase.h"
#endif


void subtract_times( struct timeval *etime, struct timeval *stime );
void save_sysdata args( ( SYSTEM_DATA sys ) );


bool check_social args( ( CHAR_DATA * ch, char *command, char *argument ) );
bool check_xsocial args( ( CHAR_DATA * ch, char *command, char *argument ) );
char *check_cmd_flags args( ( CHAR_DATA * ch, CMDTYPE * cmd ) );


bool fLogAll = FALSE;



CMDTYPE *command_hash[126];
SOCIALTYPE *social_index[27];
XSOCIALTYPE *xsocial_index[27];

bool check_pos( CHAR_DATA * ch, sh_int position )
{

   if( IS_NPC( ch ) && ch->position > 3 )
      return TRUE;

   if( ch->position < position )
   {
      switch ( ch->position )
      {
         case POS_DEAD:
            send_to_char( "A little difficult to do when you are DEAD...\n\r", ch );
            break;

         case POS_MORTAL:
         case POS_INCAP:
            send_to_char( "You are hurt far too bad for that.\n\r", ch );
            break;

         case POS_STUNNED:
            send_to_char( "You are too stunned to do that.\n\r", ch );
            break;

         case POS_SLEEPING:
            send_to_char( "In your dreams, or what?\n\r", ch );
            break;

         case POS_RESTING:
            send_to_char( "Nah... You feel too relaxed...\n\r", ch );
            break;

         case POS_SITTING:
            send_to_char( "You can't do that sitting down.\n\r", ch );
            break;

         case POS_FIGHTING:
            if( position <= POS_EVASIVE )
            {
               send_to_char( "This fighting style is too demanding for that!\n\r", ch );
            }
            else
            {
               send_to_char( "No way!  You are still fighting!\n\r", ch );
            }
            break;
         case POS_DEFENSIVE:
            if( position <= POS_EVASIVE )
            {
               send_to_char( "This fighting style is too demanding for that!\n\r", ch );
            }
            else
            {
               send_to_char( "No way!  You are still fighting!\n\r", ch );
            }
            break;
         case POS_AGGRESSIVE:
            if( position <= POS_EVASIVE )
            {
               send_to_char( "This fighting style is too demanding for that!\n\r", ch );
            }
            else
            {
               send_to_char( "No way!  You are still fighting!\n\r", ch );
            }
            break;
         case POS_BERSERK:
            if( position <= POS_EVASIVE )
            {
               send_to_char( "This fighting style is too demanding for that!\n\r", ch );
            }
            else
            {
               send_to_char( "No way!  You are still fighting!\n\r", ch );
            }
            break;
         case POS_EVASIVE:
            send_to_char( "No way!  You are still fighting!\n\r", ch );
            break;

      }
      return FALSE;
   }
   return TRUE;
}

extern char lastplayercmd[MAX_INPUT_LENGTH * 2];

bool valid_watch( char *logline )
{
   int len = strlen( logline );
   char c = logline[0];

   if( len == 1 && ( c == 'n' || c == 's' || c == 'e' || c == 'w' || c == 'u' || c == 'd' ) )
      return FALSE;
   if( len == 2 && c == 'n' && ( logline[1] == 'e' || logline[1] == 'w' ) )
      return FALSE;
   if( len == 2 && c == 's' && ( logline[1] == 'e' || logline[1] == 'w' ) )
      return FALSE;

   return TRUE;
}


void write_watch_files( CHAR_DATA * ch, CMDTYPE * cmd, char *logline )
{
   WATCH_DATA *pw;
   FILE *fp;
   char fname[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
   struct tm *t = localtime( &current_time );

   if( !first_watch )
      return;

   if( cmd )
   {
      char *cur_imm;
      bool found;

      pw = first_watch;
      while( pw )
      {
         found = FALSE;

         for( cur_imm = pw->imm_name; pw && !strcmp( pw->imm_name, cur_imm ); pw = pw->next )
         {

            if( !found && ch->desc && get_trust( ch ) < pw->imm_level
                && ( ( pw->target_name && !strcmp( cmd->name, pw->target_name ) )
                     || ( pw->player_site && !str_prefix( pw->player_site, ch->desc->host ) ) ) )
            {
               sprintf( fname, "%s%s", WATCH_DIR, strlower( pw->imm_name ) );
               if( !( fp = fopen( fname, "a+" ) ) )
               {
                  sprintf( buf, "%s%s", "Write_watch_files: Cannot open ", fname );
                  bug( buf, 0 );
                  perror( fname );
                  return;
               }
               sprintf( buf, "%.2d/%.2d %.2d:%.2d %s: %s\n\r",
                        t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, ch->name, logline );
               fputs( buf, fp );
               fclose( fp );
               found = TRUE;
            }
         }
      }
   }
   else
   {
      for( pw = first_watch; pw; pw = pw->next )
         if( ( ( pw->target_name && !str_cmp( pw->target_name, ch->name ) )
               || ( pw->player_site
                    && !str_prefix( pw->player_site, ch->desc->host ) ) ) && get_trust( ch ) < pw->imm_level && ch->desc )
         {
            sprintf( fname, "%s%s", WATCH_DIR, strlower( pw->imm_name ) );
            if( !( fp = fopen( fname, "a+" ) ) )
            {
               sprintf( buf, "%s%s", "Write_watch_files: Cannot open ", fname );
               bug( buf, 0 );
               perror( fname );
               return;
            }
            sprintf( buf, "%.2d/%.2d %.2d:%.2d %s: %s\n\r",
                     t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, ch->name, logline );
            fputs( buf, fp );
            fclose( fp );
         }
   }

   return;
}

void interpret( CHAR_DATA * ch, char *argument )
{
   char command[MAX_INPUT_LENGTH];
   char logline[MAX_INPUT_LENGTH];
   char logname[MAX_INPUT_LENGTH];
   char *buf;
   TIMER *timer = NULL;
   CMDTYPE *cmd = NULL;
   int trust;
   int loglvl;
   bool found;
   struct timeval time_used;
   long tmptime;


   if( !ch )
   {
      bug( "interpret: null ch!", 0 );
      return;
   }

   if( !ch->in_room )
   {
      bug( "interpret: null in_room!", 0 );
      return;
   }
   found = FALSE;
   if( ch->substate == SUB_REPEATCMD )
   {
      DO_FUN *fun;

      if( ( fun = ch->last_cmd ) == NULL )
      {
         ch->substate = SUB_NONE;
         bug( "interpret: SUB_REPEATCMD with NULL last_cmd", 0 );
         return;
      }
      else
      {
         int x;

         for( x = 0; x < 126; x++ )
         {
            for( cmd = command_hash[x]; cmd; cmd = cmd->next )
               if( cmd->do_fun == fun )
               {
                  found = TRUE;
                  break;
               }
            if( found )
               break;
         }
         if( !found )
         {
            cmd = NULL;
            bug( "interpret: SUB_REPEATCMD: last_cmd invalid", 0 );
            return;
         }
         sprintf( logline, "(%s) %s", cmd->name, argument );
      }
   }

   if( !cmd )
   {
      if( !argument || !strcmp( argument, "" ) )
      {
         bug( "interpret: null argument!", 0 );
         return;
      }

      while( isspace( *argument ) )
         argument++;
      if( argument[0] == '\0' )
         return;

      if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_FREEZE ) )
      {
         send_to_char( "You're totally frozen!\n\r", ch );
         return;
      }

      strcpy( logline, argument );
      if( !isalpha( argument[0] ) && !isdigit( argument[0] ) )
      {
         command[0] = argument[0];
         command[1] = '\0';
         argument++;
         while( isspace( *argument ) )
            argument++;
      }
      else
         argument = one_argument( argument, command );

      trust = get_trust( ch );
      for( cmd = command_hash[LOWER( command[0] ) % 126]; cmd; cmd = cmd->next )
         if( !str_prefix( command, cmd->name )
             && ( cmd->level <= trust
                  || ( !IS_NPC( ch ) && ch->pcdata->council
                       && is_name( cmd->name, ch->pcdata->council->powers )
                       && cmd->level <= ( trust + MAX_CPD ) )
                  || ( !IS_NPC( ch ) && ch->pcdata->bestowments && ch->pcdata->bestowments[0] != '\0'
                       && is_name( cmd->name, ch->pcdata->bestowments ) && cmd->level <= ( trust + sysdata.bestow_dif ) ) ) )
         {
            found = TRUE;
            break;
         }

      if( xIS_SET( ch->act, PLR_AFK ) && ( str_cmp( command, "AFK" ) ) )
      {
         xREMOVE_BIT( ch->act, PLR_AFK );

         act( AT_GREY, "$n is no longer afk.", ch, NULL, NULL, TO_CANSEE );
      }
      if( xIS_SET( ch->act, PLR_IAW ) && ( str_cmp( command, "IAW" ) ) )
      {
         xREMOVE_BIT( ch->act, PLR_IAW );

         act( AT_GREY, "$n is no longer in another window.", ch, NULL, NULL, TO_CANSEE );
      }
   }

   sprintf( lastplayercmd, "%s used %s", ch->name, logline );

   if( found && cmd->log == LOG_NEVER )
      strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );

   loglvl = found ? cmd->log : LOG_NORMAL;

   if( !IS_NPC( ch ) && ch->desc && valid_watch( logline ) )
   {
      if( found && IS_SET( cmd->flags, CMD_WATCH ) )
         write_watch_files( ch, cmd, logline );
      else if( IS_SET( ch->pcdata->flags, PCFLAG_WATCH ) )
         write_watch_files( ch, NULL, logline );
   }


   if( ( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_LOG ) )
       || fLogAll || loglvl == LOG_BUILD || loglvl == LOG_HIGH || loglvl == LOG_ALWAYS )
   {
      if( ch->desc && ch->desc->original )
         sprintf( log_buf, "Log %s (%s): %s", ch->name, ch->desc->original->name, logline );
      else
         sprintf( log_buf, "Log %s: %s", ch->name, logline );

      if( fLogAll && loglvl == LOG_NORMAL && ( IS_NPC( ch ) || !xIS_SET( ch->act, PLR_LOG ) ) )
         loglvl = LOG_ALL;

      log_string_plus( log_buf, loglvl, get_trust( ch ) );
   }

   if( ch->desc && ch->desc->snoop_by )
   {
      sprintf( logname, "%s", ch->name );
      write_to_buffer( ch->desc->snoop_by, logname, 0 );
      write_to_buffer( ch->desc->snoop_by, "% ", 2 );
      write_to_buffer( ch->desc->snoop_by, logline, 0 );
      write_to_buffer( ch->desc->snoop_by, "\n\r", 2 );
   }

   if( ( timer = get_timerptr( ch, TIMER_DO_FUN ) ) != NULL )
   {
      int tempsub;

      tempsub = ch->substate;
      ch->substate = SUB_TIMER_DO_ABORT;
      ( timer->do_fun ) ( ch, "" );
      if( char_died( ch ) )
         return;
      if( ch->substate != SUB_TIMER_CANT_ABORT )
      {
         ch->substate = tempsub;
         extract_timer( ch, timer );
      }
      else
      {
         ch->substate = tempsub;
         return;
      }
   }

   if( !found )
   {
      if( !check_skill( ch, command, argument )
// &&   !check_xsocial( ch, command, argument )
          && !check_social( ch, command, argument )
#ifdef USE_IMC
          && !icec_command_hook( ch, command, argument ) )
#else
          )
#endif
      {
         EXIT_DATA *pexit;

         if( ( pexit = find_door( ch, command, TRUE ) ) != NULL && IS_SET( pexit->exit_info, EX_xAUTO ) )
         {
            if( IS_SET( pexit->exit_info, EX_CLOSED )
                && ( !IS_AFFECTED( ch, AFF_PASS_DOOR ) || IS_SET( pexit->exit_info, EX_NOPASSDOOR ) ) )
            {
               if( !IS_SET( pexit->exit_info, EX_SECRET ) )
                  act( AT_PLAIN, "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
               else
                  send_to_char( "You cannot do that here.\n\r", ch );
               return;
            }
            move_char( ch, pexit, 0 );
            return;
         }
         send_to_char( "Huh?\n\r", ch );
      }
      return;
   }

   if( !check_pos( ch, cmd->position ) )
      return;

   buf = check_cmd_flags( ch, cmd );

   if( buf[0] != '\0' )
   {
      send_to_char_color( buf, ch );
      return;
   }


   if( !IS_NPC( ch ) && ch->pcdata->nuisance && ch->pcdata->nuisance->flags > 9
       && number_percent(  ) < ( ( ch->pcdata->nuisance->flags - 9 ) * 10 * ch->pcdata->nuisance->power ) )
   {
      send_to_char( "You can't seem to do that just now.\n\r", ch );
      return;
   }

   ch->prev_cmd = ch->last_cmd;
   ch->last_cmd = cmd->do_fun;
   start_timer( &time_used );
   ( *cmd->do_fun ) ( ch, argument );
   sysdata.lastcommand = STRALLOC( cmd->name );
   save_sysdata( sysdata );
   end_timer( &time_used );
   update_userec( &time_used, &cmd->userec );
   tmptime = UMIN( time_used.tv_sec, 19 ) * 1000000 + time_used.tv_usec;

   if( tmptime > 1500000 )
   {
#ifdef TIMEFORMAT
      sprintf( log_buf, "[*****] LAG: %s: %s %s (R:%d S:%ld.%06ld)", ch->name,
               cmd->name, ( cmd->log == LOG_NEVER ? "XXX" : argument ),
               ch->in_room ? ch->in_room->vnum : 0, time_used.tv_sec, time_used.tv_usec );
#else
      sprintf( log_buf, "[*****] LAG: %s: %s %s (R:%d S:%d.%06d)", ch->name,
               cmd->name, ( cmd->log == LOG_NEVER ? "XXX" : argument ),
               ch->in_room ? ch->in_room->vnum : 0, time_used.tv_sec, time_used.tv_usec );
#endif
      log_string_plus( log_buf, LOG_NORMAL, get_trust( ch ) );
      cmd->lag_count++;
   }

   tail_chain(  );
}

CMDTYPE *find_command( char *command )
{
   CMDTYPE *cmd;
   int hash;

   hash = LOWER( command[0] ) % 126;

   for( cmd = command_hash[hash]; cmd; cmd = cmd->next )
      if( !str_prefix( command, cmd->name ) )
         return cmd;

   return NULL;
}

SOCIALTYPE *find_social( char *command )
{
   SOCIALTYPE *social;
   int hash;

   if( command[0] < 'a' || command[0] > 'z' )
      hash = 0;
   else
      hash = ( command[0] - 'a' ) + 1;

   for( social = social_index[hash]; social; social = social->next )
      if( !str_prefix( command, social->name ) )
         return social;

   return NULL;
}

bool check_social( CHAR_DATA * ch, char *command, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   SOCIALTYPE *social;
   CHAR_DATA *remfirst, *remlast, *remtemp;

   if( ( social = find_social( command ) ) == NULL )
      return FALSE;

   if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_NO_EMOTE ) )
   {
      send_to_char( "You are anti-social!\n\r", ch );
      return TRUE;
   }

   switch ( ch->position )
   {
      case POS_DEAD:
         send_to_char( "Lie still; you are DEAD.\n\r", ch );
         return TRUE;

      case POS_INCAP:
      case POS_MORTAL:
         send_to_char( "You are hurt far too bad for that.\n\r", ch );
         return TRUE;

      case POS_STUNNED:
         send_to_char( "You are too stunned to do that.\n\r", ch );
         return TRUE;

      case POS_SLEEPING:
         if( !str_cmp( social->name, "snore" ) )
            break;
         if( !str_cmp( social->name, "drool" ) )
            break;
         send_to_char( "In your dreams, or what?\n\r", ch );
         return TRUE;

   }

   remfirst = NULL;
   remlast = NULL;
   remtemp = NULL;

   for( victim = ch->in_room->first_person; victim; victim = victim->next_in_room )
   {
      if( is_ignoring( victim, ch ) )
      {
         if( !IS_IMMORTAL( ch ) || get_trust( victim ) > get_trust( ch ) )
         {
            char_from_room( victim );
            LINK( victim, remfirst, remlast, next_in_room, prev_in_room );
         }
         else
         {
            set_char_color( AT_IGNORE, victim );
            ch_printf( victim, "You attempt to ignore %s," " but are unable to do so.\n\r", ch->name );
         }
      }
   }

   one_argument( argument, arg );
   victim = NULL;
   if( arg[0] == '\0' )
   {
      act( AT_SOCIAL, social->others_no_arg, ch, NULL, victim, TO_ROOM );
      act( AT_SOCIAL, social->char_no_arg, ch, NULL, victim, TO_CHAR );
   }
   else if( ch->level < 30 && ( ( victim = get_char_room( ch, arg ) ) == NULL ) )
   {
      for( victim = remfirst; victim; victim = victim->next_in_room )
      {
         if( nifty_is_name( victim->name, arg ) || nifty_is_name_prefix( arg, victim->name ) )
         {
            set_char_color( AT_IGNORE, ch );
            ch_printf( ch, "%s is ignoring you.\n\r", victim->name );
            break;
         }
      }

      if( !victim )
         send_to_char( "They aren't here.\n\r", ch );
   }
   else if( ch->level >= 30 && ( ( victim = get_char_area( ch, arg ) ) == NULL ) )
   {
      for( victim = remfirst; victim; victim = victim->next_in_room )
      {
         if( nifty_is_name( victim->name, arg ) || nifty_is_name_prefix( arg, victim->name ) )
         {
            set_char_color( AT_IGNORE, ch );
            ch_printf( ch, "%s is ignoring you.\n\r", victim->name );
            break;
         }
      }

      if( !victim )
         send_to_char( "They aren't here.\n\r", ch );
   }
   else if( victim == ch )
   {
      act( AT_SOCIAL, social->others_auto, ch, NULL, victim, TO_ROOM );
      act( AT_SOCIAL, social->char_auto, ch, NULL, victim, TO_CHAR );
   }
   else
   {
      act( AT_SOCIAL, social->others_found, ch, NULL, victim, TO_NOTVICT );
      act( AT_SOCIAL, social->char_found, ch, NULL, victim, TO_CHAR );
      act( AT_SOCIAL, social->vict_found, ch, NULL, victim, TO_VICT );

      if( !IS_NPC( ch ) && IS_NPC( victim )
          && !IS_AFFECTED( victim, AFF_CHARM )
          && !IS_AFFECTED( victim, AFF_JMT ) && IS_AWAKE( victim ) && !HAS_PROG( victim->pIndexData, ACT_PROG ) )
      {
         switch ( number_bits( 4 ) )
         {
            case 0:
               if( IS_EVIL( ch ) && !is_safe( victim, ch ) )
                  multi_hit( victim, ch, TYPE_UNDEFINED );
               else if( IS_NEUTRAL( ch ) )
               {
                  act( AT_ACTION, "$n slaps $N.", victim, NULL, ch, TO_NOTVICT );
                  act( AT_ACTION, "You slap $N.", victim, NULL, ch, TO_CHAR );
                  act( AT_ACTION, "$n slaps you.", victim, NULL, ch, TO_VICT );
               }
               else
               {
                  act( AT_ACTION, "$n acts like $N doesn't even exist.", victim, NULL, ch, TO_NOTVICT );
                  act( AT_ACTION, "You just ignore $N.", victim, NULL, ch, TO_CHAR );
                  act( AT_ACTION, "$n appears to be ignoring you.", victim, NULL, ch, TO_VICT );
               }
               break;

            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
               act( AT_SOCIAL, social->others_found, victim, NULL, ch, TO_NOTVICT );
               act( AT_SOCIAL, social->char_found, victim, NULL, ch, TO_CHAR );
               act( AT_SOCIAL, social->vict_found, victim, NULL, ch, TO_VICT );
               break;

            case 9:
            case 10:
            case 11:
            case 12:
               act( AT_ACTION, "$n slaps $N.", victim, NULL, ch, TO_NOTVICT );
               act( AT_ACTION, "You slap $N.", victim, NULL, ch, TO_CHAR );
               act( AT_ACTION, "$n slaps you.", victim, NULL, ch, TO_VICT );
               break;
         }
      }
   }

   for( victim = remfirst; victim; victim = remtemp )
   {
      remtemp = victim->next_in_room;
      char_to_room( victim, ch->in_room );
   }

   return TRUE;
}

XSOCIALTYPE *find_xsocial( char *command )
{
   XSOCIALTYPE *social;
   int hash;

   if( command[0] < 'a' || command[0] > 'z' )
      hash = 0;
   else
      hash = ( command[0] - 'a' ) + 1;

   for( social = xsocial_index[hash]; social; social = social->next )
      if( !str_prefix( command, social->name ) )
         return social;

   return NULL;
}

bool check_xsocial( CHAR_DATA * ch, char *command, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   XSOCIALTYPE *social;
   CHAR_DATA *remfirst, *remlast, *remtemp;
   int stage;
   int amount;
   bool is_ok = FALSE;
   bool one = FALSE;
   bool two = FALSE;

   if( ( social = find_xsocial( command ) ) == NULL )
      return FALSE;

   if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_NO_EMOTE ) )
   {
      send_to_char( "You are anti-social!\n\r", ch );
      return TRUE;
   }

   switch ( ch->position )
   {
      case POS_DEAD:
         send_to_char( "Lie still; you are DEAD.\n\r", ch );
         return TRUE;

      case POS_INCAP:
      case POS_MORTAL:
         send_to_char( "You are hurt far too bad for that.\n\r", ch );
         return TRUE;

/* case POS_STUNNED:
 * send_to_char( "You are too stunned to do that.\n\r", ch );
 * return TRUE;
 */

      case POS_SLEEPING:
         if( !str_cmp( social->name, "snore" ) )
            break;
         send_to_char( "In your dreams, or what?\n\r", ch );
         return TRUE;

   }

   remfirst = NULL;
   remlast = NULL;
   remtemp = NULL;

   for( victim = ch->in_room->first_person; victim; victim = victim->next_in_room )
   {
      if( is_ignoring( victim, ch ) )
      {
         if( !IS_IMMORTAL( ch ) || get_trust( victim ) > get_trust( ch ) )
         {
            char_from_room( victim );
            LINK( victim, remfirst, remlast, next_in_room, prev_in_room );
         }
         else
         {
            set_char_color( AT_IGNORE, victim );
            ch_printf( victim, "You attempt to ignore %s," " but are unable to do so.\n\r", ch->name );
         }
      }
   }

   one_argument( argument, arg );
   victim = NULL;

   if( xIS_SET( ch->act, TIED_UP ) )
   {
      send_to_char( "You wiggle and strain but the ropes only tighten.\n\r", ch );
      act( AT_SOCIAL, "$n strains helplessly against $m bonds.", ch, NULL, NULL, TO_ROOM );
      return FALSE;
   }
   else if( arg[0] == '\0' )
   {
      act( AT_SOCIAL, social->others_no_arg, ch, NULL, victim, TO_ROOM );
      act( AT_SOCIAL, social->char_no_arg, ch, NULL, victim, TO_CHAR );
   }
   else if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      if( IS_NPC( victim ) )
      {
         send_to_char( "You can only perform xsocials on players.\n\r", ch );
         return TRUE;
      }
      for( victim = remfirst; victim; victim = victim->next_in_room )
      {
         if( nifty_is_name( victim->name, arg ) || nifty_is_name_prefix( arg, victim->name ) )
         {
            set_char_color( AT_IGNORE, ch );
            ch_printf( ch, "%s is ignoring you.\n\r", victim->name );
            break;
         }
      }

      if( !victim )
         send_to_char( "They aren't here.\n\r", ch );
   }
   else if( victim == ch )
   {
      act( AT_SOCIAL, social->others_auto, ch, NULL, victim, TO_ROOM );
      act( AT_SOCIAL, social->char_auto, ch, NULL, victim, TO_CHAR );
   }
   else
   {
      if( social->gender == SEX_MALE && ch->sex != SEX_MALE )
      {
         send_to_char( "Only men can perform this type of social.\n\r", ch );
      }
      else if( social->gender == SEX_FEMALE && ch->sex != SEX_FEMALE )
      {
         send_to_char( "Only women can perform this type of social.\n\r", ch );
      }
      else if( social->gender == SEX_MALE && victim->sex != SEX_FEMALE )
      {
         send_to_char( "You can only perform this social on a woman.\n\r", ch );
      }
      else if( social->gender == SEX_FEMALE && victim->sex != SEX_MALE )
      {
         send_to_char( "You can only perform this social on a man.\n\r", ch );
      }
      else if( social->gender == 3 && ch->sex != SEX_FEMALE )
      {
         send_to_char( "Only females may preform this command.\n\r", ch );
      }
      else if( social->gender == 3 && victim->sex != SEX_FEMALE )
      {
         send_to_char( "You can only preform this command on a female.\n\r", ch );
      }
/*	else if (((partner = victim->pcdata->partner) == NULL || partner != ch) && (!IS_SET (victim->in_room->room_flags, ROOM_SEX)))
  	{
         	send_to_char("You cannot perform an xsocial on someone without their consent.\n\r",ch);
	 }*/
      else if( social->stage == 0 && ch->pcdata->stage[0] < 1 && ch->pcdata->stage[2] > 0 && ch->sex == 5 )
         send_to_char( "You have not yet recovered from last time!\n\r", ch );
      else if( social->stage == 0 && victim->pcdata->stage[0] < 1 && victim->pcdata->stage[2] > 0 && victim->sex == 5 )
         send_to_char( "They have not yet recovered from last time!\n\r", ch );
      else if( social->stage > 0 && ch->pcdata->stage[0] < 10 )
         send_to_char( "You are not sufficiently aroused.\n\r", ch );
      else if( social->stage > 0 && victim->pcdata->stage[0] < 10 )
         send_to_char( "They are not sufficiently aroused.\n\r", ch );
/*	else if (social->stage > 1 && ch->pcdata->stage[1] < 1)
	    send_to_char("You are not in the right position.\n\r",ch);
	else if (social->stage > 1 && victim->pcdata->stage[1] < 1)
	    send_to_char("They are not in the right position.\n\r",ch);
*/
      else
      {
         act( AT_SOCIAL, social->others_found, ch, NULL, victim, TO_NOTVICT );
         act( AT_SOCIAL, social->char_found, ch, NULL, victim, TO_CHAR );
         act( AT_SOCIAL, social->vict_found, ch, NULL, victim, TO_VICT );
         if( social->chance == 1 )
         {
/*		if (ch->sex == SEX_FEMALE && 
		    !xIS_SET(ch->act, EXTRA_PREGNANT) && number_range(1,3) == 1) 
		make_preg(ch,victim);
		else if (victim->sex == SEX_FEMALE && 
		    !xIS_SET(victim->act, EXTRA_PREGNANT) && 
		    number_range(1,3) == 1) 
		make_preg(victim,ch); */
         }
         if( !str_prefix( social->name, "x-tie" ) )
         {
            xSET_BIT( victim->act, TIED_UP );
         }
         if( !str_prefix( social->name, "x-gag" ) )
         {
            xSET_BIT( victim->act, GAGGED );
         }
         if( !str_prefix( social->name, "x-blindfold" ) )
         {
            xSET_BIT( victim->act, BLINDFOLDED );
         }
         if( social->stage == 1 )
         {
            ch->pcdata->stage[1] = social->position;
            victim->pcdata->stage[1] = social->position;
            if( !xIS_SET( ch->act, EXTRA_DONE ) )
            {
               xSET_BIT( ch->act, EXTRA_DONE );
               if( ch->sex == SEX_FEMALE )
               {
                  act( AT_SOCIAL, "You feel $n bleed as you enter $m.", ch, NULL, victim, TO_VICT );
                  act( AT_SOCIAL, "You feel yourself bleed as $N enters you.", ch, NULL, victim, TO_CHAR );
//       ch->in_room->blood += 1;
               }
            }
            if( !xIS_SET( victim->act, EXTRA_DONE ) )
            {
               xSET_BIT( victim->act, EXTRA_DONE );
               if( victim->sex == SEX_FEMALE )
               {
                  act( AT_SOCIAL, "You feel $N bleed as you enter $M.", ch, NULL, victim, TO_CHAR );
                  act( AT_SOCIAL, "You feel yourself bleed as $n enters you.", ch, NULL, victim, TO_VICT );
//       ch->in_room->blood += 1;
               }
            }
            stage = 2;
         }
         else
            stage = social->stage;
         if( stage == 2 )
            amount = ch->pcdata->stage[1];
         else
            amount = 10;
         if( social->self > 0 )
         {
            is_ok = FALSE;
            if( ch->pcdata->stage[stage] >= amount )
               is_ok = TRUE;
            ch->pcdata->stage[stage] += social->self;
            if( !is_ok && ch->pcdata->stage[stage] >= amount )
            {
               stage_update( ch, victim, stage, social->name );
               one = TRUE;
            }
         }
         if( social->other > 0 )
         {
            is_ok = FALSE;
            if( victim->pcdata->stage[stage] >= amount )
               is_ok = TRUE;
            victim->pcdata->stage[stage] += social->other;
            if( !is_ok && victim->pcdata->stage[stage] >= amount )
            {
               stage_update( victim, ch, stage, social->name );
               two = TRUE;
            }
         }
         if( one && two )
         {
            ch->pcdata->stage[0] = 0;
            victim->pcdata->stage[0] = 0;
            if( !xIS_SET( ch->act, EXTRA_EXP ) )
            {
               send_to_char( "Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r", ch );
               xSET_BIT( ch->act, EXTRA_EXP );
               ch->exp += 100000;
            }
            if( !xIS_SET( victim->act, EXTRA_EXP ) )
            {
               send_to_char( "Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r", victim );
               xSET_BIT( victim->act, EXTRA_EXP );
               victim->exp += 100000;
            }
         }
      }
   }

   return TRUE;
}

void stage_update( CHAR_DATA * ch, CHAR_DATA * victim, int stage, char *argument )
{
   if( IS_NPC( ch ) || IS_NPC( victim ) )
      return;
   if( stage == 0 )
   {
      if( ch->sex == SEX_MALE )
      {
         send_to_char( "You get a boner.\n\r", ch );
         act( AT_SOCIAL, "You feel $n get a boner.", ch, NULL, victim, TO_VICT );
         return;
      }
      else if( ch->sex == SEX_FEMALE )
      {
         send_to_char( "You get wet.\n\r", ch );
         act( AT_SOCIAL, "You feel $n get wet.", ch, NULL, victim, TO_VICT );
         return;
      }
   }
   else if( stage == 2 )
   {
      if( ch->sex == SEX_MALE )
      {
         if( str_cmp( argument, "xm-cum" ) && str_cmp( argument, "xm-facial" ) && str_cmp( argument, "xm-canal" ) &&
             str_cmp( argument, "xm-canal" ) && str_cmp( argument, "xm-cbreasts" ) && str_cmp( argument, "xm-chair" ) &&
             str_cmp( argument, "xm-chand" ) && str_cmp( argument, "xm-cstomach" ) && str_cmp( argument, "xf-chands" ) &&
             str_cmp( argument, "xf-cbreasts" ) )
         {
            act( AT_SOCIAL, "You grit your teeth as you shoot your creamy load inside of $M.", ch, NULL, victim, TO_CHAR );
            act( AT_SOCIAL, "$n grits his teeth as he shoots his load inside of you.", ch, NULL, victim, TO_VICT );
            act( AT_SOCIAL, "$n grits his teeth as he shoots a load of cum inside of $N.", ch, NULL, victim, TO_NOTVICT );
         }
         ch->pcdata->genes[8] += 1;
         victim->pcdata->genes[8] += 1;
         save_char_obj( ch );
         save_char_obj( victim );
         if( victim->pcdata->stage[2] < 1 || victim->pcdata->stage[2] >= 250 )
         {
            ch->pcdata->stage[2] = 0;
            if( ch->pcdata->stage[0] >= 200 )
               ch->pcdata->stage[0] -= 100;
         }
/*	    else ch->pcdata->stage[2] = 200;
	    if (victim->sex == SEX_FEMALE && 
		!xIS_SET(victim->act, EXTRA_PREGNANT) && number_percent() <= 8) 
	    make_preg(victim,ch);
	    return; */
      }
      else if( ch->sex == SEX_FEMALE )
      {
         if( str_cmp( argument, "xf-cum" ) && str_cmp( argument, "xf-cface" ) )
         {
            act( AT_SOCIAL, "You whimper as you cum.", ch, NULL, victim, TO_CHAR );
            act( AT_SOCIAL, "$n whimpers as $e cums.", ch, NULL, victim, TO_ROOM );
         }
         if( victim->pcdata->stage[2] < 1 || victim->pcdata->stage[2] >= 250 )
         {
            ch->pcdata->stage[2] = 0;
            if( ch->pcdata->stage[0] >= 200 )
               ch->pcdata->stage[0] -= 100;
         }
         else
            ch->pcdata->stage[2] = 200;
         return;
      }
   }
   return;
}

void make_preg( CHAR_DATA * mother, CHAR_DATA * father )
{
   char *strtime;
   char buf[MAX_STRING_LENGTH];

   if( IS_NPC( mother ) || IS_NPC( father ) )
      return;

   if( xIS_SET( mother->affected_by, AFF_CONTRACEPTION ) )
      return;

   strtime = ctime( &current_time );
   strtime[strlen( strtime ) - 1] = '\0';
   STRFREE( mother->pcdata->conception );
   mother->pcdata->conception = str_dup( strtime );
   sprintf( buf, "%s", father->name );
   STRFREE( mother->pcdata->cparents );
   mother->pcdata->cparents = str_dup( buf );
   xSET_BIT( mother->act, EXTRA_PREGNANT );
   mother->pcdata->genes[0] = ( mother->max_hit + father->max_hit ) * 0.5;
   mother->pcdata->genes[1] = ( mother->max_mana + father->max_mana ) * 0.5;
   mother->pcdata->genes[2] = ( mother->max_move + father->max_move ) * 0.5;
/*    if (IS_IMMUNE(mother, IMM_SLASH) && IS_IMMUNE(father, IMM_SLASH))
	SET_BIT(mother->pcdata->genes[3], IMM_SLASH);
    if (IS_IMMUNE(mother, IMM_STAB) && IS_IMMUNE(father, IMM_STAB))
	SET_BIT(mother->pcdata->genes[3], IMM_STAB);
    if (IS_IMMUNE(mother, IMM_SMASH) && IS_IMMUNE(father, IMM_SMASH))
	SET_BIT(mother->pcdata->genes[3], IMM_SMASH);
    if (IS_IMMUNE(mother, IMM_ANIMAL) && IS_IMMUNE(father, IMM_ANIMAL))
	SET_BIT(mother->pcdata->genes[3], IMM_ANIMAL);
    if (IS_IMMUNE(mother, IMM_MISC) && IS_IMMUNE(father, IMM_MISC))
	SET_BIT(mother->pcdata->genes[3], IMM_MISC);
    if (IS_IMMUNE(mother, IMM_CHARM) && IS_IMMUNE(father, IMM_CHARM))
	SET_BIT(mother->pcdata->genes[3], IMM_CHARM);
    if (IS_IMMUNE(mother, IMM_HEAT) && IS_IMMUNE(father, IMM_HEAT))
	SET_BIT(mother->pcdata->genes[3], IMM_HEAT);
    if (IS_IMMUNE(mother, IMM_COLD) && IS_IMMUNE(father, IMM_COLD))
	SET_BIT(mother->pcdata->genes[3], IMM_COLD);
    if (IS_IMMUNE(mother, IMM_LIGHTNING) && IS_IMMUNE(father, IMM_LIGHTNING))
	SET_BIT(mother->pcdata->genes[3], IMM_LIGHTNING);
    if (IS_IMMUNE(mother, IMM_ACID) && IS_IMMUNE(father, IMM_ACID))
	SET_BIT(mother->pcdata->genes[3], IMM_ACID);
    if (IS_IMMUNE(mother, IMM_VOODOO) && IS_IMMUNE(father, IMM_VOODOO))
	SET_BIT(mother->pcdata->genes[3], IMM_VOODOO);
    if (IS_IMMUNE(mother, IMM_HURL) && IS_IMMUNE(father, IMM_HURL))
	SET_BIT(mother->pcdata->genes[3], IMM_HURL);
    if (IS_IMMUNE(mother, IMM_BACKSTAB) && IS_IMMUNE(father, IMM_BACKSTAB))
	SET_BIT(mother->pcdata->genes[3], IMM_BACKSTAB);
    if (IS_IMMUNE(mother, IMM_KICK) && IS_IMMUNE(father, IMM_KICK))
	SET_BIT(mother->pcdata->genes[3], IMM_KICK);
    if (IS_IMMUNE(mother, IMM_DISARM) && IS_IMMUNE(father, IMM_DISARM))
	SET_BIT(mother->pcdata->genes[3], IMM_DISARM);
    if (IS_IMMUNE(mother, IMM_STEAL) && IS_IMMUNE(father, IMM_STEAL))
	SET_BIT(mother->pcdata->genes[3], IMM_STEAL);
    if (IS_IMMUNE(mother, IMM_SLEEP) && IS_IMMUNE(father, IMM_SLEEP))
	SET_BIT(mother->pcdata->genes[3], IMM_SLEEP);
    if (IS_IMMUNE(mother, IMM_DRAIN) && IS_IMMUNE(father, IMM_DRAIN))
	SET_BIT(mother->pcdata->genes[3], IMM_DRAIN);*/
   mother->pcdata->genes[3] = number_range( 1, 8 );
   mother->pcdata->genes[4] = number_range( 1, 2 );
   send_to_char( "You feel cum swirling inside you.\n\r", mother );
   return;
}


bool is_number( char *arg )
{
   bool first = TRUE;
   if( *arg == '\0' )
      return FALSE;

   for( ; *arg != '\0'; arg++ )
   {
      if( first && *arg == '-' )
      {
         first = FALSE;
         continue;
      }
      if( !isdigit( *arg ) )
         return FALSE;
      first = FALSE;
   }

   return TRUE;
}

int number_argument( char *argument, char *arg )
{
   char *pdot;
   int number;

   for( pdot = argument; *pdot != '\0'; pdot++ )
   {
      if( *pdot == '.' )
      {
         *pdot = '\0';
         number = atoi( argument );
         *pdot = '.';
         strcpy( arg, pdot + 1 );
         return number;
      }
   }

   strcpy( arg, argument );
   return 1;
}

char *one_interpret( char *argument, char *arg_first )
{
   sh_int count;

   count = 0;

   if( !argument || argument[0] == '\0' )
   {
      arg_first[0] = '\0';
      return argument;
   }

   while( *argument != '\0' || ++count >= 255 )
   {
      if( *argument == ':' )
      {
         argument++;
         break;
      }
      *arg_first = *argument;
      arg_first++;
      argument++;
   }
   *arg_first = '\0';

   return argument;
}

char *one_argument( char *argument, char *arg_first )
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
   if( *argument == '\'' || *argument == '"' )
      cEnd = *argument++;

   while( *argument != '\0' || ++count >= 255 )
   {
      if( *argument == cEnd )
      {
         argument++;
         break;
      }
      *arg_first = LOWER( *argument );
      arg_first++;
      argument++;
   }
   *arg_first = '\0';

   while( isspace( *argument ) )
      argument++;

   return argument;
}

char *one_argument2( char *argument, char *arg_first )
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
   if( *argument == '\'' || *argument == '"' )
      cEnd = *argument++;

   while( *argument != '\0' || ++count >= 255 )
   {
      if( *argument == cEnd || *argument == '-' )
      {
         argument++;
         break;
      }
      *arg_first = LOWER( *argument );
      arg_first++;
      argument++;
   }
   *arg_first = '\0';

   while( isspace( *argument ) )
      argument++;

   return argument;
}

void do_timecmd( CHAR_DATA * ch, char *argument )
{
   struct timeval stime;
   struct timeval etime;
   static bool timing;
   extern CHAR_DATA *timechar;
   char arg[MAX_INPUT_LENGTH];

   send_to_char( "Timing\n\r", ch );
   if( timing )
      return;
   one_argument( argument, arg );
   if( !*arg )
   {
      send_to_char( "No command to time.\n\r", ch );
      return;
   }
   if( !str_cmp( arg, "update" ) )
   {
      if( timechar )
         send_to_char( "Another person is already timing updates.\n\r", ch );
      else
      {
         timechar = ch;
         send_to_char( "Setting up to record next update loop.\n\r", ch );
      }
      return;
   }
   set_char_color( AT_PLAIN, ch );
   send_to_char( "Starting timer.\n\r", ch );
   timing = TRUE;
   gettimeofday( &stime, NULL );
   interpret( ch, argument );
   gettimeofday( &etime, NULL );
   timing = FALSE;
   set_char_color( AT_PLAIN, ch );
   send_to_char( "Timing complete.\n\r", ch );
   subtract_times( &etime, &stime );
   ch_printf( ch, "Timing took %d.%06d seconds.\n\r", etime.tv_sec, etime.tv_usec );
   return;
}

void start_timer( struct timeval *stime )
{
   if( !stime )
   {
      bug( "Start_timer: NULL stime.", 0 );
      return;
   }
   gettimeofday( stime, NULL );
   return;
}

time_t end_timer( struct timeval * stime )
{
   struct timeval etime;

   gettimeofday( &etime, NULL );
   if( !stime || ( !stime->tv_sec && !stime->tv_usec ) )
   {
      bug( "End_timer: bad stime.", 0 );
      return 0;
   }
   subtract_times( &etime, stime );
   *stime = etime;
   return ( etime.tv_sec * 1000000 ) + etime.tv_usec;
}

void send_timer( struct timerset *vtime, CHAR_DATA * ch )
{
   struct timeval ntime;
   int carry;

   if( vtime->num_uses == 0 )
      return;
   ntime.tv_sec = vtime->total_time.tv_sec / vtime->num_uses;
   carry = ( vtime->total_time.tv_sec % vtime->num_uses ) * 1000000;
   ntime.tv_usec = ( vtime->total_time.tv_usec + carry ) / vtime->num_uses;
   ch_printf( ch, "Has been used %d times this boot.\n\r", vtime->num_uses );
   ch_printf( ch, "Time (in secs): min %d.%0.6d; avg: %d.%0.6d; max %d.%0.6d"
              "\n\r", vtime->min_time.tv_sec, vtime->min_time.tv_usec, ntime.tv_sec,
              ntime.tv_usec, vtime->max_time.tv_sec, vtime->max_time.tv_usec );
   return;
}

void update_userec( struct timeval *time_used, struct timerset *userec )
{
   userec->num_uses++;
   if( !timerisset( &userec->min_time ) || timercmp( time_used, &userec->min_time, < ) )
   {
      userec->min_time.tv_sec = time_used->tv_sec;
      userec->min_time.tv_usec = time_used->tv_usec;
   }
   if( !timerisset( &userec->max_time ) || timercmp( time_used, &userec->max_time, > ) )
   {
      userec->max_time.tv_sec = time_used->tv_sec;
      userec->max_time.tv_usec = time_used->tv_usec;
   }
   userec->total_time.tv_sec += time_used->tv_sec;
   userec->total_time.tv_usec += time_used->tv_usec;
   while( userec->total_time.tv_usec >= 1000000 )
   {
      userec->total_time.tv_sec++;
      userec->total_time.tv_usec -= 1000000;
   }
   return;
}

char cmd_flag_buf[MAX_STRING_LENGTH];

char *check_cmd_flags( CHAR_DATA * ch, CMDTYPE * cmd )
{

   if( IS_AFFECTED( ch, AFF_POSSESS ) && IS_SET( cmd->flags, CMD_FLAG_POSSESS ) )
      sprintf( cmd_flag_buf, "You can't %s while you are possessing someone!\n\r", cmd->name );
   else if( ch->morph != NULL && IS_SET( cmd->flags, CMD_FLAG_POLYMORPHED ) )
      sprintf( cmd_flag_buf, "You can't %s while you are polymorphed!\n\r", cmd->name );
   else
      cmd_flag_buf[0] = '\0';

   return cmd_flag_buf;
}
