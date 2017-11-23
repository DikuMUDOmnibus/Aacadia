/************************************************************************/
/*        Acadia code team (C) 2002-2005 Kara Enterprises                   */
/************************************************************************/
/*			  New Commands module  		                    */
/************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "acadia.h"

void do_seedate( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];

   sprintf( buf, "testing date -> %s @@@\n\r", ( char * )friendly_ctime( &current_time ) );
   send_to_char( buf, ch );
   return;
}

void do_mudschool( CHAR_DATA * ch, char *argument )
{
   ROOM_INDEX_DATA *location;
   CHAR_DATA *opponent;

   location = NULL;

   if( get_trust( ch ) > 7 )
   {
      send_to_char( "You cannot enter the school.", ch );
      return;
   }

   if( !IS_NPC( ch ) && !location )
   {
      location = get_room_index( ROOM_VNUM_SCHOOL );
   }

   if( ch->in_room == location )
   {
      send_to_char( "You are at the school!\n\r", ch );
      return;
   }

   if( ( opponent = who_fighting( ch ) ) != NULL )
   {
      int lose;

      if( number_bits( 1 ) == 0 || ( !IS_NPC( opponent ) && number_bits( 3 ) > 1 ) )
      {
         WAIT_STATE( ch, 4 );
         lose = ( exp_level( ch, ch->level + 1 ) - exp_level( ch, ch->level ) ) * 0.1;
         if( ch->desc )
            lose /= 2;
         gain_exp( ch, 0 - lose );
         ch_printf( ch, "You failed!  You lose %d exps.\n\r", lose );
         return;
      }

      lose = ( exp_level( ch, ch->level + 1 ) - exp_level( ch, ch->level ) ) * 0.2;
      if( ch->desc )
         lose /= 2;
      gain_exp( ch, 0 - lose );
      ch_printf( ch, "You escape from combat to the school!  You lose %d exps.\n\r", lose );
      stop_fighting( ch, TRUE );
   }

   act( AT_ACTION, "$n seaks higher knowledge.", ch, NULL, NULL, TO_ROOM );
   char_from_room( ch );
   char_to_room( ch, location );
   if( ch->mount )
   {
      char_from_room( ch->mount );
      char_to_room( ch->mount, location );
   }
   act( AT_ACTION, "$n has come to learn!", ch, NULL, NULL, TO_ROOM );
   do_look( ch, "auto" );
   return;
}

void do_sanctuary( CHAR_DATA * ch, char *argument )
{
   ROOM_INDEX_DATA *location;
   CHAR_DATA *opponent;

   location = NULL;

   if( !IS_NPC( ch ) && !location )
   {
      location = get_room_index( ROOM_VNUM_SANCTUARY );
   }

   if( ( ch->in_room->area->plane == 2
         || ch->in_room->area->plane == 3
         || ch->in_room->area->plane == 4
         || ch->in_room->area->plane == 5 ) || xIS_SET( ch->in_room->room_flags, ROOM_NOSANC ) )
   {
      send_to_char( "You cannot enter sanctuary from here.\n\r", ch );
      return;
   }

   if( ch->in_room == location )
   {
      send_to_char( "You are in Syluthia's Sanctuary.\n\r", ch );
      return;
   }

   if( ( opponent = who_fighting( ch ) ) != NULL )
   {
      int lose;

      if( number_bits( 1 ) == 0 || ( !IS_NPC( opponent ) && number_bits( 3 ) > 1 ) )
      {
         WAIT_STATE( ch, 4 );
         lose = ( exp_level( ch, ch->level + 1 ) - exp_level( ch, ch->level ) ) * 0.1;
         if( ch->desc )
            lose /= 2;
         gain_exp( ch, 0 - lose );
         ch_printf( ch, "You failed!  You lose %d exps.\n\r", lose );
         return;
      }

      lose = ( exp_level( ch, ch->level + 1 ) - exp_level( ch, ch->level ) ) * 0.2;
      if( ch->desc )
         lose /= 2;
      gain_exp( ch, 0 - lose );
      ch_printf( ch, "You escape from combat to the sanctuary!  You lose %d exps.\n\r", lose );
      stop_fighting( ch, TRUE );
   }

   act( AT_ACTION, "$n seaks safety with Syluthia.", ch, NULL, NULL, TO_ROOM );
   char_from_room( ch );
   char_to_room( ch, location );
   if( ch->mount )
   {
      char_from_room( ch->mount );
      char_to_room( ch->mount, location );
   }
   act( AT_ACTION, "$n seeked safety with Syluthia.", ch, NULL, NULL, TO_ROOM );
   do_look( ch, "auto" );
   return;
}

void do_hlist( CHAR_DATA * ch, char *argument )
{
   int sn, i, lFound;
   char arg[MAX_INPUT_LENGTH];
   int lowlev, hilev;
   int col = 0;
   int clas = -1;
   sh_int lasttype = SKILL_SPELL;
   char buf[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) )
      return;

   argument = one_argument( argument, arg );

   if( arg[0] != '\0' )
   {
      clas = atoi( arg );
      if( clas < 0 || clas > MAX_PC_CLASS )
      {
         send_to_pager( "Please use a class number. (type classes for the list)\n\r", ch );
         return;
      }
   }
   lowlev = 1;
   hilev = 999;

   set_pager_color( AT_MAGIC, ch );
   if( clas == -1 )
   {
      send_to_pager( "&WSPELL & SKILL LIST For Hero Tier\n\r", ch );
   }
   else
   {
      sprintf( buf, "&WSPELL & SKILL LIST For Hero Tier and class %s\n\r", class_table[clas]->who_name );
      send_to_pager( buf, ch );
   }

   for( i = lowlev; i <= hilev; i++ )
   {
      lFound = 0;
      for( sn = 0; sn < top_sn; sn++ )
      {
         if( !skill_table[sn]->name )
            break;

         if( skill_table[sn]->type != lasttype )
         {
            lasttype = skill_table[sn]->type;
         }

         if( ch->pcdata->learned[sn] <= 0 && SPELL_FLAG( skill_table[sn], SF_SECRETSKILL ) )
            continue;

         if( clas == -1 )
         {
            if( find_skill_level( ch, sn ) == 100 )
            {
               if( i == find_skill_slevel( ch, sn ) )
               {
                  if( !lFound )
                  {
                     if( col % 3 != 0 )
                        send_to_pager( "\n\r", ch );
                     lFound = 1;
                     pager_printf( ch,
                                   "&p--[ &zLevel &W%3d&p ]-----------------------------------------------------------------------\n\r",
                                   i );
                     col = 0;
                  }
                  pager_printf( ch, "&G %20.20s ", skill_table[sn]->name );
                  if( ++col % 3 == 0 )
                     send_to_pager( "&D\n\r", ch );

               }
            }
         }
         else
         {
            if( find_skill_level_target( clas, sn ) == 100 )
            {
               if( i == find_skill_slevel_target( clas, sn ) )
               {
                  if( !lFound )
                  {
                     if( col % 3 != 0 )
                        send_to_pager( "\n\r", ch );
                     lFound = 1;
                     pager_printf( ch,
                                   "&p--[ &zLevel &W%3d&p ]-----------------------------------------------------------------------\n\r",
                                   i );
                     col = 0;
                  }
                  pager_printf( ch, "&G %20.20s ", skill_table[sn]->name );
                  if( ++col % 3 == 0 )
                     send_to_pager( "&D\n\r", ch );

               }
            }
         }
      }
   }
   if( col % 3 != 0 )
      send_to_pager( "\n\r", ch );
   return;
}


void do_motd( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];

   argument = one_argument( argument, arg );
   if( !str_cmp( arg, "mort" ) )
   {
      do_help( ch, "motd" );
   }
   else if( !str_cmp( arg, "ava" ) )
   {
      do_help( ch, "amotd" );
   }
   else if( !str_cmp( arg, "imm" ) && IS_AGOD( ch ) )
   {
      do_help( ch, "imotd" );
   }
   else
   {
      send_to_char( "Syntax: motd <mort/ava/imm>\n\r", ch );
   }
   return;
}

void do_autos( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];

   argument = one_argument( argument, arg );
   if( !str_cmp( arg, "katyr" ) || !str_cmp( arg, "coins" ) )
   {
      if( xIS_SET( ch->act, PLR_AUTOGOLD ) )
      {
         xREMOVE_BIT( ch->act, PLR_AUTOGOLD );
         send_to_char( "Done.\n\r", ch );
      }
      else
      {
         xSET_BIT( ch->act, PLR_AUTOGOLD );
         send_to_char( "Done.\n\r", ch );
      }
   }
   else if( !str_cmp( arg, "loot" ) )
   {
      if( xIS_SET( ch->act, PLR_AUTOLOOT ) )
      {
         xREMOVE_BIT( ch->act, PLR_AUTOLOOT );
         send_to_char( "Done.\n\r", ch );
      }
      else
      {
         xSET_BIT( ch->act, PLR_AUTOLOOT );
         send_to_char( "Done.\n\r", ch );
      }
   }
   else if( !str_cmp( arg, "exit" ) )
   {
      if( xIS_SET( ch->act, PLR_AUTOEXIT ) )
      {
         xREMOVE_BIT( ch->act, PLR_AUTOEXIT );
         send_to_char( "Done.\n\r", ch );
      }
      else
      {
         xSET_BIT( ch->act, PLR_AUTOEXIT );
         send_to_char( "Done.\n\r", ch );
      }
   }
   else if( !str_cmp( arg, "sac" ) )
   {
      if( xIS_SET( ch->act, PLR_AUTOSAC ) )
      {
         xREMOVE_BIT( ch->act, PLR_AUTOSAC );
         send_to_char( "Done.\n\r", ch );
      }
      else
      {
         xSET_BIT( ch->act, PLR_AUTOSAC );
         send_to_char( "Done.\n\r", ch );
      }
   }
   else if( !str_cmp( arg, "title" ) )
   {
      if( xIS_SET( ch->act, PLR_A_TITLE ) )
      {
         xREMOVE_BIT( ch->act, PLR_A_TITLE );
         send_to_char( "Done.\n\r", ch );
      }
      else
      {
         xSET_BIT( ch->act, PLR_A_TITLE );
         send_to_char( "Done.\n\r", ch );
      }
   }
   else
   {
      send_to_char( "Syntax: autos <katyr/coins> - Gets coins from mobiles.\n\r", ch );
      send_to_char( "Syntax: autos loot          - Gets inv from mobiles.\n\r", ch );
      send_to_char( "Syntax: autos exit          - Sets if you want to see horizontal exit info.\n\r", ch );
      send_to_char( "Syntax: autos sac           - Sac the corpses.\n\r", ch );
      send_to_char( "Syntax: autos title         - Changes title on level.\n\n\r", ch );
      sprintf( buf, "&GAUTOS&g: katyr: &W%-3s&g loot: &W%-3s&g sac: &W%-3s&g title: &W%-3s&g exit: &W%-3s&g\n\r",
               xIS_SET( ch->act, PLR_AUTOGOLD ) ? "Yes" : "No", xIS_SET( ch->act, PLR_AUTOLOOT ) ? "Yes" : "No",
               xIS_SET( ch->act, PLR_AUTOSAC ) ? "Yes" : "No", xIS_SET( ch->act, PLR_A_TITLE ) ? "Yes" : "No",
               xIS_SET( ch->act, PLR_AUTOEXIT ) ? "Yes" : "No" );
      send_to_char( buf, ch );
   }
   return;
}

void do_tier( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char trcl[MAX_STRING_LENGTH];
   int iLevel = 0;
   int chance = 0;
   int opi = 0;
   int oldLevel = 0;

   set_char_color( AT_IMMORT, ch );

   if( IS_NPC( ch ) )
   {
      send_to_char( "A mobile has no need of tier.\n\r", ch );
      return;
   }

   if( IS_AGOD( ch ) )
   {
      send_to_char( "An immortal has no need of tier.\n\r", ch );
      return;
   }

   if( ch->level >= 100 )
   {
      if( ch->sublevel < ( SUB_NEOPHYTE - 500 ) )
      {
         send_to_char( "You aren't ready to tier yet.\n\r", ch );
         return;
      }
      if( ch->level == 100 )
      {
         iLevel = 200;
         sprintf( trcl, "Avatar" );
      }
      if( ch->level == 200 )
      {
         iLevel = 300;
         sprintf( trcl, "Neophyte" );
      }
      if( ch->level == 300 )
      {
         iLevel = 400;
         sprintf( trcl, "Acolyte" );
      }
      if( ch->level == 400 )
      {
         send_to_char( "You are already at Acolyte tier!\n\r", ch );
         return;
      }
   }
   else
   {
      send_to_char( "You need to be at Hero, Avatar, or Neophyte tier to tier further!\n\r", ch );
      return;
   }

   opi = UMAX( 500, ch->sublevel ) / 10;

   chance = number_range( 50, 100 );
   if( chance < opi )
   {
      if( ch->class == CLASS_WARRIOR || ch->dualclass == CLASS_WARRIOR
          || ch->class == CLASS_THIEF || ch->dualclass == CLASS_THIEF )
      {
         ch->max_mana += 500;
      }

      ch->level = iLevel;
      ch->sublevel = 1;
      ch->exp = exp_level( ch, ch->sublevel );
      ch->trust = 0;
      if( race_table[ch->race]->evolve != -1 )
      {
         evolve_char( ch );
      }
      sprintf( buf, "&RThe sky reddens as the Storm swirls hard and fast as %s transforms into an %s.....&D", ch->name,
               trcl );
      talk_info( AT_PLAIN, buf );
      return;
   }
   else
   {
      oldLevel = number_range( 20, 500 );
      ch->pcdata->wasat = ch->sublevel - oldLevel;
      ch->sublevel = oldLevel;
      ch->exp = exp_level( ch, ch->sublevel );
      ch->trust = 0;
      sprintf( buf, "&zThe sky blackens as %s fails to meld into the Storm.....&D", ch->name );
      talk_info( AT_PLAIN, buf );
      return;
   }
}


void do_spells( CHAR_DATA * ch, char *argument )
{
   int sn;
   int col;
   sh_int cnt;
   int mana = 0;
   int blood = 0;

   if( IS_NPC( ch ) )
      return;

   set_pager_color( AT_MAGIC, ch );
   send_to_pager_color( " &B-------------------------------[&CCost of Spells&B]--------------------------------&D\n\r", ch );

   col = cnt = 0;
   set_pager_color( AT_MAGIC, ch );
   for( sn = 0; sn < top_sn; sn++ )
   {
      if( skill_table[sn]->type == SKILL_SPELL )
      {
         if( !skill_table[sn]->name )
            break;

         if( !IS_IMMORTAL( ch )
             && ( skill_table[sn]->guild != CLASS_NONE
                  && ( !IS_GUILDED( ch ) || ( ch->pcdata->clan->class != skill_table[sn]->guild ) ) ) )
            continue;

         if( ( ch->level < skill_table[sn]->skill_level[ch->class] && !DUAL_SKILL( ch, sn ) ) || ( !IS_IMMORTAL( ch )   //&& skill_table[sn]->skill_level[ch->class] == 0
                                                                                                   && !DUAL_SKILL( ch,
                                                                                                                   sn ) ) )
            continue;

         if( IS_VAMPIRE( ch ) )
            blood = skill_table[sn]->min_mana;
         else
            mana = skill_table[sn]->min_mana;

         if( ch->pcdata->learned[sn] <= 0 && SPELL_FLAG( skill_table[sn], SF_SECRETSKILL ) )
            continue;

         ++cnt;
         set_pager_color( AT_MAGIC, ch );
         pager_printf( ch, "&B%20.20s", skill_table[sn]->name );
         set_pager_color( AT_SCORE, ch );
         pager_printf( ch, " &C%3d ", IS_VAMPIRE( ch ) ? blood : mana );
         if( ++col % 3 == 0 )
            send_to_pager( "\n\r", ch );
      }
   }
   send_to_pager( "\n\r", ch );
   return;
}

void do_avlist( CHAR_DATA * ch, char *argument )
{
   int sn, i, lFound;
   char arg[MAX_INPUT_LENGTH];
   int lowlev, hilev;
   int col = 0;
   int clas = -1;
   sh_int lasttype = SKILL_SPELL;
   char buf[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) )
      return;

   argument = one_argument( argument, arg );

   if( arg[0] != '\0' )
   {
      clas = atoi( arg );
      if( clas < 0 || clas > MAX_PC_CLASS )
      {
         send_to_pager( "Please use a class number. (type classes for the list)\n\r", ch );
         return;
      }
   }
   lowlev = 1;
   hilev = 999;

   set_pager_color( AT_MAGIC, ch );
   if( clas == -1 )
   {
      send_to_pager( "&WSPELL & SKILL LIST For Avatar Tier\n\r", ch );
   }
   else
   {
      sprintf( buf, "&WSPELL & SKILL LIST For Avatar Tier and class %s\n\r", class_table[clas]->who_name );
      send_to_pager( buf, ch );
   }

   for( i = lowlev; i <= hilev; i++ )
   {
      lFound = 0;
      for( sn = 0; sn < top_sn; sn++ )
      {
         if( !skill_table[sn]->name )
            break;

         if( skill_table[sn]->type != lasttype )
         {
            lasttype = skill_table[sn]->type;
         }

         if( ch->pcdata->learned[sn] <= 0 && SPELL_FLAG( skill_table[sn], SF_SECRETSKILL ) )
            continue;

         if( clas == -1 )
         {
            if( find_skill_level( ch, sn ) == 200 )
            {
               if( i == find_skill_slevel( ch, sn ) )
               {
                  if( !lFound )
                  {
                     if( col % 3 != 0 )
                        send_to_pager( "\n\r", ch );
                     lFound = 1;
                     pager_printf( ch,
                                   "&p--[ &zLevel &W%3d&p ]-----------------------------------------------------------------------\n\r",
                                   i );
                     col = 0;
                  }
                  pager_printf( ch, "&G %20.20s ", skill_table[sn]->name );
                  if( ++col % 3 == 0 )
                     send_to_pager( "&D\n\r", ch );

               }
            }
         }
         else
         {
            if( find_skill_level_target( clas, sn ) == 200 )
            {
               if( i == find_skill_slevel_target( clas, sn ) )
               {
                  if( !lFound )
                  {
                     if( col % 3 != 0 )
                        send_to_pager( "\n\r", ch );
                     lFound = 1;
                     pager_printf( ch,
                                   "&p--[ &zLevel &W%3d&p ]-----------------------------------------------------------------------\n\r",
                                   i );
                     col = 0;
                  }
                  pager_printf( ch, "&G %20.20s ", skill_table[sn]->name );
                  if( ++col % 3 == 0 )
                     send_to_pager( "&D\n\r", ch );

               }
            }
         }
      }
   }
   if( col % 3 != 0 )
      send_to_pager( "\n\r", ch );
   return;
}

void do_nelist( CHAR_DATA * ch, char *argument )
{
   int sn, i, lFound;
   char arg[MAX_INPUT_LENGTH];
   int lowlev, hilev;
   int col = 0;
   int clas = -1;
   sh_int lasttype = SKILL_SPELL;
   char buf[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) )
      return;

   argument = one_argument( argument, arg );

   if( arg[0] != '\0' )
   {
      clas = atoi( arg );
      if( clas < 0 || clas > MAX_PC_CLASS )
      {
         send_to_pager( "Please use a class number. (type classes for the list)\n\r", ch );
         return;
      }
   }
   lowlev = 1;
   hilev = 999;

   set_pager_color( AT_MAGIC, ch );
   if( clas == -1 )
   {
      send_to_pager( "&WSPELL & SKILL LIST For Neophyte Tier\n\r", ch );
   }
   else
   {
      sprintf( buf, "&WSPELL & SKILL LIST For Neophyte Tier and class %s\n\r", class_table[clas]->who_name );
      send_to_pager( buf, ch );
   }

   for( i = lowlev; i <= hilev; i++ )
   {
      lFound = 0;
      for( sn = 0; sn < top_sn; sn++ )
      {
         if( !skill_table[sn]->name )
            break;

         if( skill_table[sn]->type != lasttype )
         {
            lasttype = skill_table[sn]->type;
         }

         if( ch->pcdata->learned[sn] <= 0 && SPELL_FLAG( skill_table[sn], SF_SECRETSKILL ) )
            continue;

         if( clas == -1 )
         {
            if( find_skill_level( ch, sn ) == 300 )
            {
               if( i == find_skill_slevel( ch, sn ) )
               {
                  if( !lFound )
                  {
                     if( col % 3 != 0 )
                        send_to_pager( "\n\r", ch );
                     lFound = 1;
                     pager_printf( ch,
                                   "&p--[ &zLevel &W%3d&p ]-----------------------------------------------------------------------\n\r",
                                   i );
                     col = 0;
                  }
                  pager_printf( ch, "&G %20.20s ", skill_table[sn]->name );
                  if( ++col % 3 == 0 )
                     send_to_pager( "&D\n\r", ch );

               }
            }
         }
         else
         {
            if( find_skill_level_target( clas, sn ) == 300 )
            {
               if( i == find_skill_slevel_target( clas, sn ) )
               {
                  if( !lFound )
                  {
                     if( col % 3 != 0 )
                        send_to_pager( "\n\r", ch );
                     lFound = 1;
                     pager_printf( ch,
                                   "&p--[ &zLevel &W%3d&p ]-----------------------------------------------------------------------\n\r",
                                   i );
                     col = 0;
                  }
                  pager_printf( ch, "&G %20.20s ", skill_table[sn]->name );
                  if( ++col % 3 == 0 )
                     send_to_pager( "&D\n\r", ch );

               }
            }
         }
      }
   }
   if( col % 3 != 0 )
      send_to_pager( "\n\r", ch );
   return;
}

void do_aclist( CHAR_DATA * ch, char *argument )
{
   int sn, i, lFound;
   char arg[MAX_INPUT_LENGTH];
   int lowlev, hilev;
   int col = 0;
   int clas = -1;
   sh_int lasttype = SKILL_SPELL;
   char buf[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) )
      return;

   argument = one_argument( argument, arg );

   if( arg[0] != '\0' )
   {
      clas = atoi( arg );
      if( clas < 0 || clas > MAX_PC_CLASS )
      {
         send_to_pager( "Please use a class number. (type classes for the list)\n\r", ch );
         return;
      }
   }
   lowlev = 1;
   hilev = 999;

   set_pager_color( AT_MAGIC, ch );
   if( clas == -1 )
   {
      send_to_pager( "&WSPELL & SKILL LIST For Acolyte Tier\n\r", ch );
   }
   else
   {
      sprintf( buf, "&WSPELL & SKILL LIST For Acolyte Tier and class %s\n\r", class_table[clas]->who_name );
      send_to_pager( buf, ch );
   }

   for( i = lowlev; i <= hilev; i++ )
   {
      lFound = 0;
      for( sn = 0; sn < top_sn; sn++ )
      {
         if( !skill_table[sn]->name )
            break;

         if( skill_table[sn]->type != lasttype )
         {
            lasttype = skill_table[sn]->type;
         }

         if( ch->pcdata->learned[sn] <= 0 && SPELL_FLAG( skill_table[sn], SF_SECRETSKILL ) )
            continue;

         if( clas == -1 )
         {
            if( find_skill_level( ch, sn ) == 400 )
            {
               if( i == find_skill_slevel( ch, sn ) )
               {
                  if( !lFound )
                  {
                     if( col % 3 != 0 )
                        send_to_pager( "\n\r", ch );
                     lFound = 1;
                     pager_printf( ch,
                                   "&p--[ &zLevel &W%3d&p ]-----------------------------------------------------------------------\n\r",
                                   i );
                     col = 0;
                  }
                  pager_printf( ch, "&G %20.20s ", skill_table[sn]->name );
                  if( ++col % 3 == 0 )
                     send_to_pager( "&D\n\r", ch );

               }
            }
         }
         else
         {
            if( find_skill_level_target( clas, sn ) == 400 )
            {
               if( i == find_skill_slevel_target( clas, sn ) )
               {
                  if( !lFound )
                  {
                     if( col % 3 != 0 )
                        send_to_pager( "\n\r", ch );
                     lFound = 1;
                     pager_printf( ch,
                                   "&p--[ &zLevel &W%3d&p ]-----------------------------------------------------------------------\n\r",
                                   i );
                     col = 0;
                  }
                  pager_printf( ch, "&G %20.20s ", skill_table[sn]->name );
                  if( ++col % 3 == 0 )
                     send_to_pager( "&D\n\r", ch );

               }
            }
         }
      }
   }
   if( col % 3 != 0 )
      send_to_pager( "\n\r", ch );
   return;
}

void do_rebuild( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *obj;

   if( IS_NPC( ch ) )
   {
      send_to_char( "NPC's don't need to rebuild.\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg1 );
   if( arg[0] == '\0' || arg1[0] == '\0' )
   {
      send_to_char( "Syntax: rebuild <class> <password>\n\r", ch );
      send_to_char( "\n\r", ch );
      send_to_char( "This command allows you to rebuild into one of 4 classes which\n\r", ch );
      send_to_char( "are unselectable in normal conditions. The following are the\n\r", ch );
      send_to_char( "classes available through rebuild and the level needed to rebuild.\n\r", ch );
      send_to_char
         ( "&RPaladin&g\t\t300(100)\n\r&RSorcerer&g\t300(100)\n\r&RBard&g\t\t400(200)\n\r&RBarbarian&g\t400(200)&D\n\r",
           ch );
      return;
   }
   else if( !str_cmp( arg, "Sorcerer" ) || !str_cmp( arg, "sorcerer" ) )
   {
      if( ch->level != 300 )
      {
         send_to_char( "You are not ready to rebuild.\n\r", ch );
         return;
      }
      if( ch->sublevel < 100 )
      {
         send_to_char( "You are not ready to rebuild.\n\r", ch );
         return;
      }
      if( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
      {
         send_to_char( "Wrong password.\n\r", ch );
         return;
      }
      ch->class = 12;

      for( obj = ch->first_carrying; obj; obj = obj->next_content )
      {
         separate_obj( obj );
         extract_obj( obj );
      }

      ch->pcdata->clan_name = STRALLOC( "" );
      ch->pcdata->clan = NULL;

      ch->perm_str = 13;
      ch->perm_dex = 13;
      ch->perm_wis = 13;
      ch->perm_int = 13;
      ch->perm_con = 13;
      ch->perm_cha = 13;
      ch->perm_lck = 13;

      switch ( class_table[ch->class]->attr_prime )
      {
         case APPLY_STR:
            ch->perm_str = 16;
            break;
         case APPLY_INT:
            ch->perm_int = 16;
            break;
         case APPLY_WIS:
            ch->perm_wis = 16;
            break;
         case APPLY_DEX:
            ch->perm_dex = 16;
            break;
         case APPLY_CON:
            ch->perm_con = 16;
            break;
         case APPLY_CHA:
            ch->perm_cha = 16;
            break;
         case APPLY_LCK:
            ch->perm_lck = 16;
            break;
      }

      ch->perm_str += race_table[ch->race]->str_plus;
      ch->perm_int += race_table[ch->race]->int_plus;
      ch->perm_wis += race_table[ch->race]->wis_plus;
      ch->perm_dex += race_table[ch->race]->dex_plus;
      ch->perm_con += race_table[ch->race]->con_plus;
      ch->perm_cha += race_table[ch->race]->cha_plus;
      ch->perm_lck += race_table[ch->race]->lck_plus;

      if( ch->pcdata->deity )
      {
         ch->perm_str += ch->pcdata->deity->strpls;
         ch->perm_int += ch->pcdata->deity->intpls;
         ch->perm_wis += ch->pcdata->deity->wispls;
         ch->perm_dex += ch->pcdata->deity->dexpls;
         ch->perm_con += ch->pcdata->deity->conpls;
         ch->perm_cha += ch->pcdata->deity->chapls;
         ch->perm_lck += ch->pcdata->deity->lckpls;
      }

      ch->armor += race_table[ch->race]->ac_plus;
      ch->alignment += race_table[ch->race]->alignment;
      ch->affected_by = race_table[ch->race]->affected;
      ch->attacks = race_table[ch->race]->attacks;
      ch->defenses = race_table[ch->race]->defenses;
      ch->saving_poison_death = race_table[ch->race]->saving_poison_death;
      ch->saving_wand = race_table[ch->race]->saving_wand;
      ch->saving_para_petri = race_table[ch->race]->saving_para_petri;
      ch->saving_breath = race_table[ch->race]->saving_breath;
      ch->saving_spell_staff = race_table[ch->race]->saving_spell_staff;

      ch->height = number_range( race_table[ch->race]->height * .9, race_table[ch->race]->height * 1.1 );
      ch->weight = number_range( race_table[ch->race]->weight * .9, race_table[ch->race]->weight * 1.1 );

      if( ch->class == CLASS_PALADIN )
         ch->alignment = 1000;

      ch->level = 2;
      ch->dualclass = -1;
      ch->sublevel = 0;
      ch->exp = 0;
      ch->exp += ( 1 * class_table[ch->class]->exp_tnl );
      ch->max_hit = 20 + race_table[ch->race]->hit;
      ch->max_mana = race_table[ch->race]->mana;
      ch->max_move = 100;
      ch->max_blood = race_table[ch->race]->mana;
      ch->max_hit += race_table[ch->race]->hit;
      ch->max_mana += race_table[ch->race]->mana;
      ch->max_blood += race_table[ch->race]->mana;
      ch->hit = UMAX( 1, ch->max_hit );
      ch->mana = UMAX( 1, ch->max_mana );
      ch->blood = UMAX( 1, ch->max_blood );
      ch->move = ch->max_move;
      ch->pcdata->pretiti = 0;
      ch->practice = 45;
      sprintf( buf, "the %s", title_table[ch->class][ch->level][ch->sex == SEX_FEMALE ? 1 : 0] );
      set_title( ch, buf );

      xSET_BIT( ch->act, PLR_AUTOGOLD );
      xSET_BIT( ch->act, PLR_AUTOLOOT );
      xSET_BIT( ch->act, PLR_A_TITLE );
      SET_BIT( ch->pcdata->flags, PCFLAG_POSI );
      ch->mod_str = 7;
      ch->mod_int = 7;
      ch->mod_wis = 7;
      ch->mod_dex = 7;
      ch->mod_con = 7;
      ch->mod_cha = 7;
      ch->mod_lck = 7;
      {
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_VEST ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_GHOST ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SHIELD ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_BANNER ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );

         if( ( ch->class == CLASS_MAGE ) || ( ch->class == CLASS_THIEF ) || ( ch->class == CLASS_BARD )
             || ( ch->class == CLASS_TAEDIEN ) || ( ch->class == CLASS_VAMPIRE ) )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_DAGGER ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_CLERIC )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_MACE ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_JEDI )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_LIGHTSABER ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_RANGER )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_BARBARIAN )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_SAMURAI )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD_TWO ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_SORCERER )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_PALADIN )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ( ch->class == CLASS_WARRIOR ) || ( ch->class == CLASS_WARWIZARD ) || ( ch->class == CLASS_WEREWOLF ) )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
      }
      ch->exp = exp_level( ch, ch->level );
      ch->trust = 0;
      ch->pcdata->recall = 100000;
      send_to_char( "A mystical force ripples through your body as the Storm reintertwines\n\r", ch );
      send_to_char( "with you. Painful jolts of electricity arc from your eyes and mouth as\n\r", ch );
      send_to_char( "you begin to change....\n\n\n\n\r", ch );
      send_to_char( "As you emerge from the cacoon you were in, a bright light shines from\n\r", ch );
      send_to_char( "you. There is a thunder as the Storm recedes from you as your new body\n\r", ch );
      send_to_char( "becomes visible.\n\r", ch );
      sprintf( buf, "&R<&BLEVEL INFO&R> %s has just rebuilt into a Sorcerer!&D", ch->name );
      talk_info( AT_PLAIN, buf );
      if( IS_SET( ch->pcdata->flags, PCFLAG_EXP ) )
      {
         char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL_ADV ) );
      }
      else
      {
         char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
      }
      return;
   }
   else if( !str_cmp( arg, "Barbarian" ) || !str_cmp( arg, "barbarian" ) )
   {
      if( ch->level != 400 )
      {
         send_to_char( "You are not ready to rebuild.\n\r", ch );
         return;
      }
      if( ch->sublevel < 200 )
      {
         send_to_char( "You are not ready to rebuild.\n\r", ch );
         return;
      }
      if( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
      {
         send_to_char( "Wrong password.\n\r", ch );
         return;
      }
      ch->class = 11;

      for( obj = ch->first_carrying; obj; obj = obj->next_content )
      {
         separate_obj( obj );
         extract_obj( obj );
      }

      ch->pcdata->clan_name = STRALLOC( "" );
      ch->pcdata->clan = NULL;

      ch->perm_str = 13;
      ch->perm_dex = 13;
      ch->perm_wis = 13;
      ch->perm_int = 13;
      ch->perm_con = 13;
      ch->perm_cha = 13;
      ch->perm_lck = 13;

      switch ( class_table[ch->class]->attr_prime )
      {
         case APPLY_STR:
            ch->perm_str = 16;
            break;
         case APPLY_INT:
            ch->perm_int = 16;
            break;
         case APPLY_WIS:
            ch->perm_wis = 16;
            break;
         case APPLY_DEX:
            ch->perm_dex = 16;
            break;
         case APPLY_CON:
            ch->perm_con = 16;
            break;
         case APPLY_CHA:
            ch->perm_cha = 16;
            break;
         case APPLY_LCK:
            ch->perm_lck = 16;
            break;
      }

      ch->perm_str += race_table[ch->race]->str_plus;
      ch->perm_int += race_table[ch->race]->int_plus;
      ch->perm_wis += race_table[ch->race]->wis_plus;
      ch->perm_dex += race_table[ch->race]->dex_plus;
      ch->perm_con += race_table[ch->race]->con_plus;
      ch->perm_cha += race_table[ch->race]->cha_plus;
      ch->perm_lck += race_table[ch->race]->lck_plus;

      if( ch->pcdata->deity )
      {
         ch->perm_str += ch->pcdata->deity->strpls;
         ch->perm_int += ch->pcdata->deity->intpls;
         ch->perm_wis += ch->pcdata->deity->wispls;
         ch->perm_dex += ch->pcdata->deity->dexpls;
         ch->perm_con += ch->pcdata->deity->conpls;
         ch->perm_cha += ch->pcdata->deity->chapls;
         ch->perm_lck += ch->pcdata->deity->lckpls;
      }

      ch->armor += race_table[ch->race]->ac_plus;
      ch->alignment += race_table[ch->race]->alignment;
      ch->affected_by = race_table[ch->race]->affected;
      ch->attacks = race_table[ch->race]->attacks;
      ch->defenses = race_table[ch->race]->defenses;
      ch->saving_poison_death = race_table[ch->race]->saving_poison_death;
      ch->saving_wand = race_table[ch->race]->saving_wand;
      ch->saving_para_petri = race_table[ch->race]->saving_para_petri;
      ch->saving_breath = race_table[ch->race]->saving_breath;
      ch->saving_spell_staff = race_table[ch->race]->saving_spell_staff;

      ch->height = number_range( race_table[ch->race]->height * .9, race_table[ch->race]->height * 1.1 );
      ch->weight = number_range( race_table[ch->race]->weight * .9, race_table[ch->race]->weight * 1.1 );

      if( ch->class == CLASS_PALADIN )
         ch->alignment = 1000;

      ch->level = 2;
      ch->sublevel = 0;
      ch->dualclass = -1;
      ch->exp = 0;
      ch->exp += ( 1 * class_table[ch->class]->exp_tnl );
      ch->max_hit = 20 + race_table[ch->race]->hit;
      ch->max_mana = race_table[ch->race]->mana;
      ch->max_move = 100;
      ch->max_blood = race_table[ch->race]->mana;
      ch->max_hit += race_table[ch->race]->hit;
      ch->max_mana += race_table[ch->race]->mana;
      ch->max_blood += race_table[ch->race]->mana;
      ch->hit = UMAX( 1, ch->max_hit );
      ch->mana = UMAX( 1, ch->max_mana );
      ch->blood = UMAX( 1, ch->max_blood );
      ch->move = ch->max_move;
      ch->pcdata->pretiti = 0;
      ch->practice = 45;
      sprintf( buf, "the %s", title_table[ch->class][ch->level][ch->sex == SEX_FEMALE ? 1 : 0] );
      set_title( ch, buf );

      xSET_BIT( ch->act, PLR_AUTOGOLD );
      xSET_BIT( ch->act, PLR_AUTOLOOT );
      xSET_BIT( ch->act, PLR_A_TITLE );
      SET_BIT( ch->pcdata->flags, PCFLAG_POSI );
      ch->mod_str = 7;
      ch->mod_int = 7;
      ch->mod_wis = 7;
      ch->mod_dex = 7;
      ch->mod_con = 7;
      ch->mod_cha = 7;
      ch->mod_lck = 7;
      {
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_VEST ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_GHOST ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SHIELD ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_BANNER ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );

         if( ( ch->class == CLASS_MAGE ) || ( ch->class == CLASS_THIEF ) || ( ch->class == CLASS_BARD )
             || ( ch->class == CLASS_TAEDIEN ) || ( ch->class == CLASS_VAMPIRE ) )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_DAGGER ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_SAMURAI )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD_TWO ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_CLERIC )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_MACE ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_JEDI )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_LIGHTSABER ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_RANGER )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_BARBARIAN )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_SORCERER )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_PALADIN )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ( ch->class == CLASS_WARRIOR ) || ( ch->class == CLASS_WARWIZARD ) || ( ch->class == CLASS_WEREWOLF ) )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
      }
      ch->exp = exp_level( ch, ch->level );
      ch->trust = 0;
      ch->pcdata->recall = 100000;
      send_to_char( "A mystical force ripples through your body as the Storm reintertwines\n\r", ch );
      send_to_char( "with you. Painful jolts of electricity arc from your eyes and mouth as\n\r", ch );
      send_to_char( "you begin to change....\n\n\n\n\r", ch );
      send_to_char( "As you emerge from the cacoon you were in, a bright light shines from\n\r", ch );
      send_to_char( "you. There is a thunder as the Storm recedes from you as your new body\n\r", ch );
      send_to_char( "becomes visible.\n\r", ch );
      sprintf( buf, "&R<&BLEVEL INFO&R> %s has just rebuilt into a Barbarian!&D", ch->name );
      talk_info( AT_PLAIN, buf );
      if( IS_SET( ch->pcdata->flags, PCFLAG_EXP ) )
      {
         char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL_ADV ) );
      }
      else
      {
         char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
      }
      return;
   }
   else if( !str_cmp( arg, "Paladin" ) || !str_cmp( arg, "paladin" ) )
   {
      if( ch->level != 300 )
      {
         send_to_char( "You are not ready to rebuild.\n\r", ch );
         return;
      }
      if( ch->sublevel < 100 )
      {
         send_to_char( "You are not ready to rebuild.\n\r", ch );
         return;
      }
      if( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
      {
         send_to_char( "Wrong password.\n\r", ch );
         return;
      }
      ch->class = 13;

      for( obj = ch->first_carrying; obj; obj = obj->next_content )
      {
         separate_obj( obj );
         extract_obj( obj );
      }

      ch->pcdata->clan_name = STRALLOC( "" );
      ch->pcdata->clan = NULL;

      ch->perm_str = 13;
      ch->perm_dex = 13;
      ch->perm_wis = 13;
      ch->perm_int = 13;
      ch->perm_con = 13;
      ch->perm_cha = 13;
      ch->perm_lck = 13;

      switch ( class_table[ch->class]->attr_prime )
      {
         case APPLY_STR:
            ch->perm_str = 16;
            break;
         case APPLY_INT:
            ch->perm_int = 16;
            break;
         case APPLY_WIS:
            ch->perm_wis = 16;
            break;
         case APPLY_DEX:
            ch->perm_dex = 16;
            break;
         case APPLY_CON:
            ch->perm_con = 16;
            break;
         case APPLY_CHA:
            ch->perm_cha = 16;
            break;
         case APPLY_LCK:
            ch->perm_lck = 16;
            break;
      }

      ch->perm_str += race_table[ch->race]->str_plus;
      ch->perm_int += race_table[ch->race]->int_plus;
      ch->perm_wis += race_table[ch->race]->wis_plus;
      ch->perm_dex += race_table[ch->race]->dex_plus;
      ch->perm_con += race_table[ch->race]->con_plus;
      ch->perm_cha += race_table[ch->race]->cha_plus;
      ch->perm_lck += race_table[ch->race]->lck_plus;

      if( ch->pcdata->deity )
      {
         ch->perm_str += ch->pcdata->deity->strpls;
         ch->perm_int += ch->pcdata->deity->intpls;
         ch->perm_wis += ch->pcdata->deity->wispls;
         ch->perm_dex += ch->pcdata->deity->dexpls;
         ch->perm_con += ch->pcdata->deity->conpls;
         ch->perm_cha += ch->pcdata->deity->chapls;
         ch->perm_lck += ch->pcdata->deity->lckpls;
      }

      ch->armor += race_table[ch->race]->ac_plus;
      ch->alignment += race_table[ch->race]->alignment;
      ch->affected_by = race_table[ch->race]->affected;
      ch->attacks = race_table[ch->race]->attacks;
      ch->defenses = race_table[ch->race]->defenses;
      ch->saving_poison_death = race_table[ch->race]->saving_poison_death;
      ch->saving_wand = race_table[ch->race]->saving_wand;
      ch->saving_para_petri = race_table[ch->race]->saving_para_petri;
      ch->saving_breath = race_table[ch->race]->saving_breath;
      ch->saving_spell_staff = race_table[ch->race]->saving_spell_staff;

      ch->height = number_range( race_table[ch->race]->height * .9, race_table[ch->race]->height * 1.1 );
      ch->weight = number_range( race_table[ch->race]->weight * .9, race_table[ch->race]->weight * 1.1 );

      if( ch->class == CLASS_PALADIN )
         ch->alignment = 1000;

      ch->level = 2;
      ch->sublevel = 0;
      ch->dualclass = -1;
      ch->exp = 0;
      ch->exp += ( 1 * class_table[ch->class]->exp_tnl );
      ch->max_hit = 20 + race_table[ch->race]->hit;
      ch->max_mana = race_table[ch->race]->mana;
      ch->max_move = 100;
      ch->max_blood = race_table[ch->race]->mana;
      ch->max_hit += race_table[ch->race]->hit;
      ch->max_mana += race_table[ch->race]->mana;
      ch->max_blood += race_table[ch->race]->mana;
      ch->hit = UMAX( 1, ch->max_hit );
      ch->mana = UMAX( 1, ch->max_mana );
      ch->blood = UMAX( 1, ch->max_blood );
      ch->move = ch->max_move;
      ch->pcdata->pretiti = 0;
      ch->practice = 45;
      sprintf( buf, "the %s", title_table[ch->class][ch->level][ch->sex == SEX_FEMALE ? 1 : 0] );
      set_title( ch, buf );

      xSET_BIT( ch->act, PLR_AUTOGOLD );
      xSET_BIT( ch->act, PLR_AUTOLOOT );
      xSET_BIT( ch->act, PLR_A_TITLE );
      SET_BIT( ch->pcdata->flags, PCFLAG_POSI );
      ch->mod_str = 7;
      ch->mod_int = 7;
      ch->mod_wis = 7;
      ch->mod_dex = 7;
      ch->mod_con = 7;
      ch->mod_cha = 7;
      ch->mod_lck = 7;
      {
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_VEST ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_GHOST ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SHIELD ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_BANNER ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );

         if( ( ch->class == CLASS_MAGE ) || ( ch->class == CLASS_THIEF ) || ( ch->class == CLASS_BARD )
             || ( ch->class == CLASS_TAEDIEN ) || ( ch->class == CLASS_VAMPIRE ) )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_DAGGER ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_SAMURAI )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD_TWO ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_CLERIC )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_MACE ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_JEDI )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_LIGHTSABER ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_RANGER )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_BARBARIAN )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_SORCERER )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_PALADIN )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ( ch->class == CLASS_WARRIOR ) || ( ch->class == CLASS_WARWIZARD ) || ( ch->class == CLASS_WEREWOLF ) )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
      }
      ch->exp = exp_level( ch, ch->level );
      ch->trust = 0;
      ch->pcdata->recall = 100000;
      send_to_char( "A mystical force ripples through your body as the Storm reintertwines\n\r", ch );
      send_to_char( "with you. Painful jolts of electricity arc from your eyes and mouth as\n\r", ch );
      send_to_char( "you begin to change....\n\n\n\n\r", ch );
      send_to_char( "As you emerge from the cacoon you were in, a bright light shines from\n\r", ch );
      send_to_char( "you. There is a thunder as the Storm recedes from you as your new body\n\r", ch );
      send_to_char( "becomes visible.\n\r", ch );
      sprintf( buf, "&R<&BLEVEL INFO&R> %s has just rebuilt into a Paladin!&D", ch->name );
      talk_info( AT_PLAIN, buf );
      if( IS_SET( ch->pcdata->flags, PCFLAG_EXP ) )
      {
         char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL_ADV ) );
      }
      else
      {
         char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
      }
      return;
   }
   else if( !str_cmp( arg, "Bard" ) || !str_cmp( arg, "bard" ) )
   {
      if( ch->level != 400 )
      {
         send_to_char( "You are not ready to rebuild.\n\r", ch );
         return;
      }
      if( ch->sublevel < 200 )
      {
         send_to_char( "You are not ready to rebuild.\n\r", ch );
         return;
      }
      if( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
      {
         send_to_char( "Wrong password.\n\r", ch );
         return;
      }
      ch->class = 14;

      for( obj = ch->first_carrying; obj; obj = obj->next_content )
      {
         separate_obj( obj );
         extract_obj( obj );
      }

      ch->pcdata->clan_name = STRALLOC( "" );
      ch->pcdata->clan = NULL;

      ch->perm_str = 13;
      ch->perm_dex = 13;
      ch->perm_wis = 13;
      ch->perm_int = 13;
      ch->perm_con = 13;
      ch->perm_cha = 13;
      ch->perm_lck = 13;

      switch ( class_table[ch->class]->attr_prime )
      {
         case APPLY_STR:
            ch->perm_str = 16;
            break;
         case APPLY_INT:
            ch->perm_int = 16;
            break;
         case APPLY_WIS:
            ch->perm_wis = 16;
            break;
         case APPLY_DEX:
            ch->perm_dex = 16;
            break;
         case APPLY_CON:
            ch->perm_con = 16;
            break;
         case APPLY_CHA:
            ch->perm_cha = 16;
            break;
         case APPLY_LCK:
            ch->perm_lck = 16;
            break;
      }

      ch->perm_str += race_table[ch->race]->str_plus;
      ch->perm_int += race_table[ch->race]->int_plus;
      ch->perm_wis += race_table[ch->race]->wis_plus;
      ch->perm_dex += race_table[ch->race]->dex_plus;
      ch->perm_con += race_table[ch->race]->con_plus;
      ch->perm_cha += race_table[ch->race]->cha_plus;
      ch->perm_lck += race_table[ch->race]->lck_plus;

      if( ch->pcdata->deity )
      {
         ch->perm_str += ch->pcdata->deity->strpls;
         ch->perm_int += ch->pcdata->deity->intpls;
         ch->perm_wis += ch->pcdata->deity->wispls;
         ch->perm_dex += ch->pcdata->deity->dexpls;
         ch->perm_con += ch->pcdata->deity->conpls;
         ch->perm_cha += ch->pcdata->deity->chapls;
         ch->perm_lck += ch->pcdata->deity->lckpls;
      }

      ch->armor += race_table[ch->race]->ac_plus;
      ch->alignment += race_table[ch->race]->alignment;
      ch->affected_by = race_table[ch->race]->affected;
      ch->attacks = race_table[ch->race]->attacks;
      ch->defenses = race_table[ch->race]->defenses;
      ch->saving_poison_death = race_table[ch->race]->saving_poison_death;
      ch->saving_wand = race_table[ch->race]->saving_wand;
      ch->saving_para_petri = race_table[ch->race]->saving_para_petri;
      ch->saving_breath = race_table[ch->race]->saving_breath;
      ch->saving_spell_staff = race_table[ch->race]->saving_spell_staff;

      ch->height = number_range( race_table[ch->race]->height * .9, race_table[ch->race]->height * 1.1 );
      ch->weight = number_range( race_table[ch->race]->weight * .9, race_table[ch->race]->weight * 1.1 );

      if( ch->class == CLASS_PALADIN )
         ch->alignment = 1000;

      ch->level = 2;
      ch->sublevel = 0;
      ch->dualclass = -1;
      ch->exp = 0;
      ch->exp += ( 1 * class_table[ch->class]->exp_tnl );
      ch->max_hit = 20 + race_table[ch->race]->hit;
      ch->max_mana = race_table[ch->race]->mana;
      ch->max_move = 100;
      ch->max_blood = race_table[ch->race]->mana;
      ch->max_hit += race_table[ch->race]->hit;
      ch->max_mana += race_table[ch->race]->mana;
      ch->max_blood += race_table[ch->race]->mana;
      ch->hit = UMAX( 1, ch->max_hit );
      ch->mana = UMAX( 1, ch->max_mana );
      ch->blood = UMAX( 1, ch->max_blood );
      ch->move = ch->max_move;
      ch->pcdata->pretiti = 0;
      ch->practice = 45;
      sprintf( buf, "the %s", title_table[ch->class][ch->level][ch->sex == SEX_FEMALE ? 1 : 0] );
      set_title( ch, buf );

      xSET_BIT( ch->act, PLR_AUTOGOLD );
      xSET_BIT( ch->act, PLR_AUTOLOOT );
      xSET_BIT( ch->act, PLR_A_TITLE );
      SET_BIT( ch->pcdata->flags, PCFLAG_POSI );
      ch->mod_str = 7;
      ch->mod_int = 7;
      ch->mod_wis = 7;
      ch->mod_dex = 7;
      ch->mod_con = 7;
      ch->mod_cha = 7;
      ch->mod_lck = 7;
      {
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_VEST ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_GHOST ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SHIELD ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );
         obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_BANNER ), 1 );
         if( IS_ARM_TYPE( obj ) )
         {
            basestat( obj );
         }
         obj_to_char( obj, ch );

         if( ( ch->class == CLASS_MAGE ) || ( ch->class == CLASS_THIEF ) || ( ch->class == CLASS_BARD )
             || ( ch->class == CLASS_TAEDIEN ) || ( ch->class == CLASS_VAMPIRE ) )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_DAGGER ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_SAMURAI )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD_TWO ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_CLERIC )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_MACE ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_JEDI )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_LIGHTSABER ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_RANGER )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_BARBARIAN )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_SORCERER )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ch->class == CLASS_PALADIN )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
         else if( ( ch->class == CLASS_WARRIOR ) || ( ch->class == CLASS_WARWIZARD ) || ( ch->class == CLASS_WEREWOLF ) )
         {
            obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
            if( IS_ARM_TYPE( obj ) )
            {
               basestat( obj );
            }
            obj_to_char( obj, ch );
         }
      }
      ch->exp = exp_level( ch, ch->level );
      ch->trust = 0;
      ch->pcdata->recall = 100000;
      send_to_char( "A mystical force ripples through your body as the Storm reintertwines\n\r", ch );
      send_to_char( "with you. Painful jolts of electricity arc from your eyes and mouth as\n\r", ch );
      send_to_char( "you begin to change....\n\n\n\n\r", ch );
      send_to_char( "As you emerge from the cacoon you were in, a bright light shines from\n\r", ch );
      send_to_char( "you. There is a thunder as the Storm recedes from you as your new body\n\r", ch );
      send_to_char( "becomes visible.\n\r", ch );
      sprintf( buf, "&R<&BLEVEL INFO&R> %s has just rebuilt into a Bard!&D", ch->name );
      talk_info( AT_PLAIN, buf );
      if( IS_SET( ch->pcdata->flags, PCFLAG_EXP ) )
      {
         char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL_ADV ) );
      }
      else
      {
         char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
      }
      return;
   }
}

void do_rework( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
// char buf[MAX_STRING_LENGTH];
   AFFECT_DATA *paf;
   AFFECT_DATA *paf_next;
   OBJ_DATA *obj, *obj_next;

   if( IS_NPC( ch ) )
   {
      send_to_char( "NPC's don't need to rebuild.\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg1 );
   if( arg[0] == '\0' ) //|| arg1[0] == '\0' )
   {
      send_to_char( "Syntax: rework <class> <password>\n\r", ch );
      return;
   }
   else if( !str_cmp( arg, "Warrior" ) || !str_cmp( arg, "warrior" ) )
   {
/*		if (strcmp (crypt (arg1, ch->pcdata->pwd), ch->pcdata->pwd))
		{
			send_to_char ("Wrong password.\n\r", ch);
			return;
		}*/
      ch->class = 3;

      for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
      {
         obj_next = obj->next_content;
         if( obj->wear_loc != WEAR_NONE && can_see_obj( ch, obj ) )
            remove_obj( ch, obj->wear_loc, TRUE );
      }
      for( paf = ch->first_affect; paf; paf = paf_next )
      {
         paf_next = paf->next;
         affect_remove( ch, paf );
      }

      ch->perm_str = 13;
      ch->perm_dex = 13;
      ch->perm_wis = 13;
      ch->perm_int = 13;
      ch->perm_con = 13;
      ch->perm_cha = 13;
      ch->perm_lck = 13;

      switch ( class_table[ch->class]->attr_prime )
      {
         case APPLY_STR:
            ch->perm_str = 16;
            break;
         case APPLY_INT:
            ch->perm_int = 16;
            break;
         case APPLY_WIS:
            ch->perm_wis = 16;
            break;
         case APPLY_DEX:
            ch->perm_dex = 16;
            break;
         case APPLY_CON:
            ch->perm_con = 16;
            break;
         case APPLY_CHA:
            ch->perm_cha = 16;
            break;
         case APPLY_LCK:
            ch->perm_lck = 16;
            break;
      }

      ch->perm_str += race_table[ch->race]->str_plus;
      ch->perm_int += race_table[ch->race]->int_plus;
      ch->perm_wis += race_table[ch->race]->wis_plus;
      ch->perm_dex += race_table[ch->race]->dex_plus;
      ch->perm_con += race_table[ch->race]->con_plus;
      ch->perm_cha += race_table[ch->race]->cha_plus;
      ch->perm_lck += race_table[ch->race]->lck_plus;

      if( ch->pcdata->deity )
      {
         ch->perm_str += ch->pcdata->deity->strpls;
         ch->perm_int += ch->pcdata->deity->intpls;
         ch->perm_wis += ch->pcdata->deity->wispls;
         ch->perm_dex += ch->pcdata->deity->dexpls;
         ch->perm_con += ch->pcdata->deity->conpls;
         ch->perm_cha += ch->pcdata->deity->chapls;
         ch->perm_lck += ch->pcdata->deity->lckpls;
      }

      ch->practice += 350;
      ch->trust = 0;
      ch->pcdata->recall = 100000;
      send_to_char( "Thank you for making this choice, we are sorry for this inconvenience.\n\r", ch );
      update_aris( ch );
      return;
   }
   else if( !str_cmp( arg, "Thief" ) || !str_cmp( arg, "thief" ) )
   {
      if( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
      {
         send_to_char( "Wrong password.\n\r", ch );
         return;
      }
      ch->class = 2;

      for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
      {
         obj_next = obj->next_content;
         if( obj->wear_loc != WEAR_NONE && can_see_obj( ch, obj ) )
            remove_obj( ch, obj->wear_loc, TRUE );
      }
      for( paf = ch->first_affect; paf; paf = paf_next )
      {
         paf_next = paf->next;
         affect_remove( ch, paf );
      }

      ch->perm_str = 13;
      ch->perm_dex = 13;
      ch->perm_wis = 13;
      ch->perm_int = 13;
      ch->perm_con = 13;
      ch->perm_cha = 13;
      ch->perm_lck = 13;

      switch ( class_table[ch->class]->attr_prime )
      {
         case APPLY_STR:
            ch->perm_str = 16;
            break;
         case APPLY_INT:
            ch->perm_int = 16;
            break;
         case APPLY_WIS:
            ch->perm_wis = 16;
            break;
         case APPLY_DEX:
            ch->perm_dex = 16;
            break;
         case APPLY_CON:
            ch->perm_con = 16;
            break;
         case APPLY_CHA:
            ch->perm_cha = 16;
            break;
         case APPLY_LCK:
            ch->perm_lck = 16;
            break;
      }

      ch->perm_str += race_table[ch->race]->str_plus;
      ch->perm_int += race_table[ch->race]->int_plus;
      ch->perm_wis += race_table[ch->race]->wis_plus;
      ch->perm_dex += race_table[ch->race]->dex_plus;
      ch->perm_con += race_table[ch->race]->con_plus;
      ch->perm_cha += race_table[ch->race]->cha_plus;
      ch->perm_lck += race_table[ch->race]->lck_plus;

      if( ch->pcdata->deity )
      {
         ch->perm_str += ch->pcdata->deity->strpls;
         ch->perm_int += ch->pcdata->deity->intpls;
         ch->perm_wis += ch->pcdata->deity->wispls;
         ch->perm_dex += ch->pcdata->deity->dexpls;
         ch->perm_con += ch->pcdata->deity->conpls;
         ch->perm_cha += ch->pcdata->deity->chapls;
         ch->perm_lck += ch->pcdata->deity->lckpls;
      }

      ch->practice += 350;
      ch->trust = 0;
      ch->pcdata->recall = 100000;
      send_to_char( "Thank you for making this choice, we are sorry for this inconvenience.\n\r", ch );
      update_aris( ch );
      return;
   }
   else if( !str_cmp( arg, "Ranger" ) || !str_cmp( arg, "ranger" ) )
   {
      if( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
      {
         send_to_char( "Wrong password.\n\r", ch );
         return;
      }
      ch->class = 8;

      for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
      {
         obj_next = obj->next_content;
         if( obj->wear_loc != WEAR_NONE && can_see_obj( ch, obj ) )
            remove_obj( ch, obj->wear_loc, TRUE );
      }
      for( paf = ch->first_affect; paf; paf = paf_next )
      {
         paf_next = paf->next;
         affect_remove( ch, paf );
      }

      ch->perm_str = 13;
      ch->perm_dex = 13;
      ch->perm_wis = 13;
      ch->perm_int = 13;
      ch->perm_con = 13;
      ch->perm_cha = 13;
      ch->perm_lck = 13;

      switch ( class_table[ch->class]->attr_prime )
      {
         case APPLY_STR:
            ch->perm_str = 16;
            break;
         case APPLY_INT:
            ch->perm_int = 16;
            break;
         case APPLY_WIS:
            ch->perm_wis = 16;
            break;
         case APPLY_DEX:
            ch->perm_dex = 16;
            break;
         case APPLY_CON:
            ch->perm_con = 16;
            break;
         case APPLY_CHA:
            ch->perm_cha = 16;
            break;
         case APPLY_LCK:
            ch->perm_lck = 16;
            break;
      }

      ch->perm_str += race_table[ch->race]->str_plus;
      ch->perm_int += race_table[ch->race]->int_plus;
      ch->perm_wis += race_table[ch->race]->wis_plus;
      ch->perm_dex += race_table[ch->race]->dex_plus;
      ch->perm_con += race_table[ch->race]->con_plus;
      ch->perm_cha += race_table[ch->race]->cha_plus;
      ch->perm_lck += race_table[ch->race]->lck_plus;

      if( ch->pcdata->deity )
      {
         ch->perm_str += ch->pcdata->deity->strpls;
         ch->perm_int += ch->pcdata->deity->intpls;
         ch->perm_wis += ch->pcdata->deity->wispls;
         ch->perm_dex += ch->pcdata->deity->dexpls;
         ch->perm_con += ch->pcdata->deity->conpls;
         ch->perm_cha += ch->pcdata->deity->chapls;
         ch->perm_lck += ch->pcdata->deity->lckpls;
      }

      ch->practice += 350;
      ch->trust = 0;
      ch->pcdata->recall = 100000;
      send_to_char( "Thank you for making this choice, we are sorry for this inconvenience.\n\r", ch );
      update_aris( ch );
      return;
   }
   else if( !str_cmp( arg, "Samurai" ) || !str_cmp( arg, "samurai" ) )
   {
      if( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
      {
         send_to_char( "Wrong password.\n\r", ch );
         return;
      }
      ch->class = 15;

      for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
      {
         obj_next = obj->next_content;
         if( obj->wear_loc != WEAR_NONE && can_see_obj( ch, obj ) )
            remove_obj( ch, obj->wear_loc, TRUE );
      }
      for( paf = ch->first_affect; paf; paf = paf_next )
      {
         paf_next = paf->next;
         affect_remove( ch, paf );
      }

      ch->perm_str = 13;
      ch->perm_dex = 13;
      ch->perm_wis = 13;
      ch->perm_int = 13;
      ch->perm_con = 13;
      ch->perm_cha = 13;
      ch->perm_lck = 13;

      switch ( class_table[ch->class]->attr_prime )
      {
         case APPLY_STR:
            ch->perm_str = 16;
            break;
         case APPLY_INT:
            ch->perm_int = 16;
            break;
         case APPLY_WIS:
            ch->perm_wis = 16;
            break;
         case APPLY_DEX:
            ch->perm_dex = 16;
            break;
         case APPLY_CON:
            ch->perm_con = 16;
            break;
         case APPLY_CHA:
            ch->perm_cha = 16;
            break;
         case APPLY_LCK:
            ch->perm_lck = 16;
            break;
      }

      ch->perm_str += race_table[ch->race]->str_plus;
      ch->perm_int += race_table[ch->race]->int_plus;
      ch->perm_wis += race_table[ch->race]->wis_plus;
      ch->perm_dex += race_table[ch->race]->dex_plus;
      ch->perm_con += race_table[ch->race]->con_plus;
      ch->perm_cha += race_table[ch->race]->cha_plus;
      ch->perm_lck += race_table[ch->race]->lck_plus;

      if( ch->pcdata->deity )
      {
         ch->perm_str += ch->pcdata->deity->strpls;
         ch->perm_int += ch->pcdata->deity->intpls;
         ch->perm_wis += ch->pcdata->deity->wispls;
         ch->perm_dex += ch->pcdata->deity->dexpls;
         ch->perm_con += ch->pcdata->deity->conpls;
         ch->perm_cha += ch->pcdata->deity->chapls;
         ch->perm_lck += ch->pcdata->deity->lckpls;
      }

      ch->practice += 350;
      ch->trust = 0;
      ch->pcdata->recall = 100000;
      send_to_char( "Thank you for making this choice, we are sorry for this inconvenience.\n\r", ch );
      update_aris( ch );
      return;
   }
}

void do_midscore( CHAR_DATA * ch, char *argument )
{
   if( xIS_SET( ch->act, PLR_MIDSCORE ) )
   {
      xREMOVE_BIT( ch->act, PLR_MIDSCORE );
      send_to_char( "Your score is now in the normal display mode.\n\r", ch );
      do_score( ch, "" );
      return;
   }
   else
   {
      if( xIS_SET( ch->act, PLR_SCRESHRT ) )
      {
         xREMOVE_BIT( ch->act, PLR_SCRESHRT );
      }

      xSET_BIT( ch->act, PLR_MIDSCORE );
      send_to_char( "Your score is now in the middle display mode.\n\r", ch );
      do_score( ch, "" );
      return;
   }
}

void do_shortscore( CHAR_DATA * ch, char *argument )
{
   if( xIS_SET( ch->act, PLR_SCRESHRT ) )
   {
      xREMOVE_BIT( ch->act, PLR_SCRESHRT );
      send_to_char( "Your score is now in the normal display mode.\n\r", ch );
      do_score( ch, "" );
      return;
   }
   else
   {
      if( xIS_SET( ch->act, PLR_MIDSCORE ) )
      {
         xREMOVE_BIT( ch->act, PLR_MIDSCORE );
      }

      xSET_BIT( ch->act, PLR_SCRESHRT );
      send_to_char( "Your score is now in the short display mode.\n\r", ch );
      do_score( ch, "" );
      return;
   }
}

void do_urna( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *obj;

   smash_tilde( argument );

   argument = one_argument( argument, arg1 );
   strcpy( arg2, argument );
   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Syntax: urna <obj> <string>.\n\r", ch );
      return;
   }

   if( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
   {
      send_to_char( "You can't find that here.\n\r", ch );
      return;
   }

   if( sysdata.spell_lock == 1 )
   {
      send_to_char( "I'm sorry, global spell lock is on.\n\r", ch );
      return;
   }

   if( ch->position != 12 )
   {
      send_to_char( "You must be standing to do this.\n\r", ch );
      return;
   }

   if( !IS_VAMPIRE( ch ) && !IS_IMMORTAL( ch ) )
   {
      if( ch->mana < 2500 )
      {
         send_to_char( "You don't have enough mana.\n\r", ch );
         return;
      }
   }

   if( IS_VAMPIRE( ch ) && !IS_IMMORTAL( ch ) )
   {
      if( ch->blood < 2500 )
      {
         send_to_char( "You don't have enough blood.\n\r", ch );
         return;
      }
   }

   separate_obj( obj );

   STRFREE( obj->short_descr );
   obj->short_descr = STRALLOC( arg2 );
   if( str_infix( "rename", obj->name ) )
   {
      sprintf( buf, "%s %s", obj->name, "rename" );
      STRFREE( obj->name );
      obj->name = STRALLOC( buf );
   }
   if( !IS_IMMORTAL( ch ) )
   {
      if( IS_VAMPIRE( ch ) )
      {
         ch->blood -= 2500;
      }
      else
      {
         ch->mana -= 2500;
      }
   }
   act( AT_MAGIC, "Your magic twists and winds around $p and the look has changed.", ch, obj, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's magic twists and winds around $p and the look has changed.", ch, obj, NULL, TO_NOTVICT );
   WAIT_STATE( ch, 70 );
   return;
}

CHAR_DATA *find_undertaker( CHAR_DATA * ch )
{
   CHAR_DATA *undertaker = NULL;

   for( undertaker = ch->in_room->first_person; undertaker; undertaker = undertaker->next_in_room )
      if( IS_NPC( undertaker ) && xIS_SET( undertaker->act, ACT_UNDERTAKER ) )
         break;

   return undertaker;
}

void do_corpse( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj, *outer_obj;
   CHAR_DATA *mob;
   bool found = FALSE;
   int cost = 0;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Mobs cannot retreive corpses.\n\r", ch );
      return;
   }

   if( !( mob = find_undertaker( ch ) ) )
   {
      send_to_char( "There's no undertaker here!\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      sprintf( buf2, "%s Ooo Yesss ... I can helpss you.", ch->name );
      do_say_to_char( mob, buf2 );
      sprintf( buf2, "%s It will cost you 20 katyr per your level....", ch->name );
      do_say_to_char( mob, buf2 );
/*		sprintf( buf2, "%s This service will stay available until you are level 35...", ch->name );
		do_say_to_char( mob, buf2 );*/
      sprintf( buf2, "%s If still interested... type corpse retrieve...", ch->name );
      do_say_to_char( mob, buf2 );
      return;
   }

   if( !str_cmp( arg, "retrieve" ) /* && ch->level < 36 */  )
   {
      cost = 20 * ch->level;
   }
   else
   {
      sprintf( buf2, "%s Type 'corpse' for help on what I do.", ch->name );
      do_say_to_char( mob, buf2 );
      return;
   }

   if( cost > ch->gold )
   {
      sprintf( buf2, "%s Pah! You do not have enough katyr for my services!", ch->name );
      do_say_to_char( mob, buf2 );
      return;
   }

   strcpy( buf, "the corpse of " );
   strcat( buf, ch->name );
   for( obj = first_object; obj; obj = obj->next )
   {
      if( !nifty_is_name( buf, obj->short_descr ) )
         continue;

      if( obj->item_type == ITEM_CORPSE_NPC )
         continue;

      found = TRUE;

      outer_obj = obj;
      while( outer_obj->in_obj )
         outer_obj = outer_obj->in_obj;

      separate_obj( outer_obj );
      obj_from_room( outer_obj );
      obj_to_room_cr( outer_obj, ch->in_room, ch );

      ch->gold -= cost;
      act( AT_PLAIN, "$N creepily carts in your corpse.", ch, NULL, mob, TO_CHAR );
      act( AT_PLAIN, "$n creepily carts in the $T.", mob, NULL, buf, TO_ROOM );
   }

   if( !found )
      sprintf( buf2, "%s Sorry I can't find your corpse. There's nothing more I can do.", ch->name );
   do_say_to_char( mob, buf2 );

   return;
}

void do_bountylist( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;

   sprintf( buf,
            "&GThese are those who have been bountied...\n\rWho          &z|&G Ammount   &z|&G Type       \n\r&z-------------+-----------+-------------------\n\r" );
   send_to_char( buf, ch );
   for( vch = first_char; vch; vch = vch_next )
   {
      vch_next = vch->next;

      if( !IS_NPC( vch ) )
      {
         if( IS_BOUNTIED( vch ) && can_see( ch, vch ) )
         {
            if( vch->pcdata->bountytype == 1 )
            {
               sprintf( buf3, "katyr" );
            }
            else if( vch->pcdata->bountytype == 2 )
            {
               sprintf( buf3, "experience" );
            }
            else if( vch->pcdata->bountytype == 3 )
            {
               sprintf( buf3, "practices" );
            }
            sprintf( buf2, "&W%-12.12s &z|&R %-9d &z|&G %s\n\r", vch->name, vch->pcdata->bounty, buf3 );
            send_to_char( buf2, ch );
         }
      }
   }
   return;
}

void do_movein( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) )
      return;

   set_char_color( AT_SCORE, ch );

   if( IS_SET( ch->pcdata->flags, PCFLAG_NOTITLE ) )
   {
      set_char_color( AT_IMMORT, ch );
      send_to_char( "The Gods prohibit you from changing your title.\n\r", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      if( ch->pcdata->movein && ch->pcdata->movein[0] != '\0' )
      {
         send_to_char( ch->pcdata->movein, ch );
         send_to_char( "\n\r", ch );
      }
      else
      {
         send_to_char( "Set your movein to what?\n\r", ch );
      }
      return;
   }

   if( strlen( argument ) > 80 )
      argument[80] = '\0';

   smash_tilde( argument );
   set_movein( ch, argument );
   send_to_char( "Ok.\n\r", ch );
}

void do_moveout( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) )
      return;

   set_char_color( AT_SCORE, ch );

   if( IS_SET( ch->pcdata->flags, PCFLAG_NOTITLE ) )
   {
      set_char_color( AT_IMMORT, ch );
      send_to_char( "The Gods prohibit you from changing your title.\n\r", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      if( ch->pcdata->moveout && ch->pcdata->moveout[0] != '\0' )
      {
         send_to_char( ch->pcdata->moveout, ch );
         send_to_char( "\n\r", ch );
      }
      else
      {
         send_to_char( "Set your moveout to what?\n\r", ch );
      }
      return;
   }

   if( strlen( argument ) > 80 )
      argument[80] = '\0';

   smash_tilde( argument );
   set_moveout( ch, argument );
   send_to_char( "Ok.\n\r", ch );
}

void do_monitor( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *targ;

   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: monitor <who>\n\r", ch );
      return;
   }

   if( ( targ = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "That player is not here.\n\r", ch );
      return;
   }

   if( !is_same_group( targ, ch ) )
   {
      send_to_char( "That player is not in the group.\n\r", ch );
      return;
   }

   if( IS_NPC( targ ) )
   {
      send_to_char( "Not on NPC's.\n\r", ch );
      return;
   }

   if( targ == ch )
   {
      ch->pcdata->monitor = NULL;
      send_to_char( "Monitor turned off.\n\r", ch );
      return;
   }

   ch->pcdata->monitor = targ;
   ch->pcdata->monitoring = TRUE;
   sprintf( buf, "Ok, monitoring %s.\n\r", targ->name );
   send_to_char( buf, ch );
   return;
}

void do_racial( CHAR_DATA * ch, char *argument )
{
   char afby[MAX_STRING_LENGTH];
   char atks[MAX_STRING_LENGTH];
   char dfns[MAX_STRING_LENGTH];
   bool rFound = FALSE;

   if( IS_NPC( ch ) )
      return;

   pager_printf( ch, "&BRacial Information for %s\n\r\n\r", capitalize( race_table[ch->race]->race_name ) );
   strcpy( afby, affect_bit_name( &race_table[ch->race]->affected ) );
   strcpy( atks, ext_flag_string( &race_table[ch->race]->attacks, attack_flags ) );
   strcpy( dfns, ext_flag_string( &race_table[ch->race]->defenses, defense_flags ) );

   if( afby[0] != '\0' && str_cmp( afby, "none" ) )
   {
      pager_printf( ch, "&GAffected: %s&D\n\r", afby );
      rFound = TRUE;
   }
   if( atks[0] != '\0' )
   {
      pager_printf( ch, "   &cAttacks: %s&D\n\r", atks );
      rFound = TRUE;
   }
   if( dfns[0] != '\0' )
   {
      pager_printf( ch, "  &RDefenses: %s&D\n\r", dfns );
      rFound = TRUE;
   }

   if( !rFound )
      send_to_pager( "&w(No Information Found)&D\n\r", ch );

   return;
}

void do_kar( CHAR_DATA * ch, char *argument )
{
   AFFECT_DATA af;

   if( sysdata.kaura == FALSE )
   {
      send_to_char( "Kara's Aura is not available at this time. Please do not ask for it.\n\r", ch );
      return;
   }

   if( IS_AFFECTED( ch, AFF_KARAAURA ) )
   {
      send_to_char( "You are already touched by Kara's Aura.\n\r", ch );
      return;
   }

   af.type = gsn_karaaura;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_KARAAURA );
   af.location = APPLY_HITROLL;
   af.modifier = 20000;
   affect_to_char( ch, &af );

   af.type = gsn_karaaura;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_KARAAURA );
   af.location = APPLY_DAMROLL;
   af.modifier = 20000;
   affect_to_char( ch, &af );

   act( AT_IMMORT, "Kara's Aura touches $n's soul!", ch, NULL, NULL, TO_ROOM );
   act( AT_IMMORT, "Kara's Aura touches your soul!", ch, NULL, NULL, TO_CHAR );
   return;
}

void do_cribsheet( CHAR_DATA * ch, char *argument )
{
   do_help( ch, "command_cribsheet" );
   return;
}
