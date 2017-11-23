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
/*					Skill Handling module		*/
/************************************************************************/
#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "acadia.h"
extern char *const sec_flags[];
extern int get_secflag( char *flag );

char *const spell_flag[] = { "water", "earth", "air", "astral", "area", "distant", "reverse",
   "noself", "_unused2_", "accumulative", "recastable", "noscribe",
   "nobrew", "group", "object", "character", "secretskill", "pksensitive",
   "stoponfail", "nofight", "nodispel", "randomtarget", "r2", "r3", "r4",
   "r5", "r6", "r7", "r8", "r9", "r10", "r11"
};

char *const spell_saves[] = { "none", "poison_death", "wands", "para_petri", "breath", "spell_staff" };

char *const spell_save_effect[] = { "none", "negate", "eightdam", "quarterdam", "halfdam", "3qtrdam",
   "reflect", "absorb"
};

char *const spell_damage[] = { "none", "fire", "cold", "electricity", "energy", "acid", "poison", "drain" };

char *const spell_action[] = { "none", "create", "destroy", "resist", "suscept", "divinate", "obscure",
   "change"
};

char *const spell_power[] = { "none", "minor", "greater", "major" };

char *const spell_class[] = { "none", "lunar", "solar", "travel", "summon", "life", "death", "illusion" };

char *const target_type[] = { "ignore", "offensive", "defensive", "self", "objinv" };


void show_char_to_char( CHAR_DATA * list, CHAR_DATA * ch );
void show_char_to_char_0( CHAR_DATA * victim, CHAR_DATA * ch );
void show_list_to_char( OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowN );
void show_list_to_char_inv( OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing, int itemtype );

int ris_save( CHAR_DATA * ch, int chance, int ris );
bool check_illegal_psteal( CHAR_DATA * ch, CHAR_DATA * victim );

void failed_casting( struct skill_type *skill, CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * obj );

void skill_notfound( CHAR_DATA * ch, char *argument )
{
   send_to_char( "Huh?\n\r", ch );
   return;
}


int get_ssave( char *name )
{
   int x;

   for( x = 0; x < sizeof( spell_saves ) / sizeof( spell_saves[0] ); x++ )
      if( !str_cmp( name, spell_saves[x] ) )
         return x;
   return -1;
}

int get_starget( char *name )
{
   int x;

   for( x = 0; x < sizeof( target_type ) / sizeof( target_type[0] ); x++ )
      if( !str_cmp( name, target_type[x] ) )
         return x;
   return -1;
}

int get_sflag( char *name )
{
   int x;

   for( x = 0; x < sizeof( spell_flag ) / sizeof( spell_flag[0] ); x++ )
      if( !str_cmp( name, spell_flag[x] ) )
         return x;
   return -1;
}

int get_sdamage( char *name )
{
   int x;

   for( x = 0; x < sizeof( spell_damage ) / sizeof( spell_damage[0] ); x++ )
      if( !str_cmp( name, spell_damage[x] ) )
         return x;
   return -1;
}

int get_saction( char *name )
{
   int x;

   for( x = 0; x < sizeof( spell_action ) / sizeof( spell_action[0] ); x++ )
      if( !str_cmp( name, spell_action[x] ) )
         return x;
   return -1;
}

int get_ssave_effect( char *name )
{
   int x;

   for( x = 0; x < sizeof( spell_save_effect ) / sizeof( spell_save_effect[0] ); x++ )
      if( !str_cmp( name, spell_save_effect[x] ) )
         return x;
   return -1;
}

int get_spower( char *name )
{
   int x;

   for( x = 0; x < sizeof( spell_power ) / sizeof( spell_power[0] ); x++ )
      if( !str_cmp( name, spell_power[x] ) )
         return x;
   return -1;
}

int get_sclass( char *name )
{
   int x;

   for( x = 0; x < sizeof( spell_class ) / sizeof( spell_class[0] ); x++ )
      if( !str_cmp( name, spell_class[x] ) )
         return x;
   return -1;
}

bool is_legal_kill( CHAR_DATA * ch, CHAR_DATA * vch )
{
   if( IS_NPC( ch ) || IS_NPC( vch ) )
      return TRUE;
   if( !IS_PKILL( ch ) || !IS_PKILL( vch ) )
      return FALSE;
   if( ch->pcdata->clan && ch->pcdata->clan == vch->pcdata->clan )
      return FALSE;
   return TRUE;
}


extern char *target_name;

bool check_skill( CHAR_DATA * ch, char *command, char *argument )
{
   int sn;
   int first = gsn_first_skill;
   int top = gsn_first_weapon - 1;
   int mana, blood;
   struct timeval time_used;

   for( ;; )
   {
      sn = ( first + top ) >> 1;

      if( LOWER( command[0] ) == LOWER( skill_table[sn]->name[0] )
          && !str_prefix( command, skill_table[sn]->name )
          && ( skill_table[sn]->skill_fun || skill_table[sn]->spell_fun != spell_null ) && ( can_use_skill( ch, 0, sn ) ) )
         break;
      if( first >= top )
         return FALSE;
      if( strcmp( command, skill_table[sn]->name ) < 1 )
         top = sn - 1;
      else
         first = sn + 1;
   }

   if( !check_pos( ch, skill_table[sn]->minimum_position ) )
      return TRUE;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) || IS_AFFECTED( ch, AFF_POSSESS ) ) )
   {
      send_to_char( "For some reason, you seem unable to perform that...\n\r", ch );
      act( AT_GREY, "$n wanders around aimlessly.", ch, NULL, NULL, TO_ROOM );
      return TRUE;
   }

   if( skill_table[sn]->min_mana )
   {
      mana = IS_NPC( ch ) ? 0 : UMAX( skill_table[sn]->min_mana, 100 / ( 2 + ch->level - find_skill_level( ch, sn ) ) );
      blood = IS_NPC( ch ) ? 0 : UMAX( skill_table[sn]->min_mana, 100 / ( 2 + ch->level - find_skill_level( ch, sn ) ) );
      if( IS_VAMPIRE( ch ) )
      {
         if( !IS_NPC( ch ) && ch->blood < blood )
         {
            send_to_char( "You don't have enough blood power.\n\r", ch );
            return TRUE;
         }
      }
      else if( !IS_NPC( ch ) && ch->mana < mana )
      {
         send_to_char( "You don't have enough mana.\n\r", ch );
         return TRUE;
      }
   }
   else
   {
      mana = 0;
      blood = 0;
   }

   if( !skill_table[sn]->skill_fun )
   {
      ch_ret retcode = rNONE;
      void *vo = NULL;
      CHAR_DATA *victim = NULL;
      OBJ_DATA *obj = NULL;

      target_name = "";

      switch ( skill_table[sn]->target )
      {
         default:
            bug( "Check_skill: bad target for sn %d.", sn );
            send_to_char( "Something went wrong...\n\r", ch );
            return TRUE;

         case TAR_IGNORE:
            vo = NULL;
            if( argument[0] == '\0' )
            {
               if( ( victim = who_fighting( ch ) ) != NULL )
                  target_name = victim->name;
            }
            else
               target_name = argument;
            break;

         case TAR_CHAR_OFFENSIVE:
         {
            if( argument[0] == '\0' && ( victim = who_fighting( ch ) ) == NULL )
            {
               ch_printf( ch, "Confusion overcomes you as your '%s' has no target.\n\r", skill_table[sn]->name );
               return TRUE;
            }
            else if( argument[0] != '\0' && ( victim = get_char_room( ch, argument ) ) == NULL )
            {
               send_to_char( "They aren't here.\n\r", ch );
               return TRUE;
            }
         }
            if( is_safe( ch, victim ) )
               return TRUE;

            if( ch == victim && SPELL_FLAG( skill_table[sn], SF_NOSELF ) )
            {
               send_to_char( "You can't target yourself!\n\r", ch );
               return TRUE;
            }

            if( !IS_NPC( ch ) )
            {
               if( !IS_NPC( victim ) )
               {
                  if( get_timer( ch, TIMER_PKILLED ) > 0 )
                  {
                     send_to_char( "You have been killed in the last 5 minutes.\n\r", ch );
                     return TRUE;
                  }

                  if( get_timer( victim, TIMER_PKILLED ) > 0 )
                  {
                     send_to_char( "This player has been killed in the last 5 minutes.\n\r", ch );
                     return TRUE;
                  }

                  if( victim != ch )
                     send_to_char( "You really shouldn't do this to another player...\n\r", ch );
               }

               if( ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) && ch->master == victim )
               {
                  send_to_char( "You can't do that on your own follower.\n\r", ch );
                  return TRUE;
               }
            }

            check_illegal_pk( ch, victim );
            vo = ( void * )victim;
            break;

         case TAR_CHAR_DEFENSIVE:
         {
            if( argument[0] != '\0' && ( victim = get_char_room( ch, argument ) ) == NULL )
            {
               send_to_char( "They aren't here.\n\r", ch );
               return TRUE;
            }
            if( !victim )
               victim = ch;
         }

            if( ch == victim && SPELL_FLAG( skill_table[sn], SF_NOSELF ) )
            {
               send_to_char( "You can't target yourself!\n\r", ch );
               return TRUE;
            }

            vo = ( void * )victim;
            break;

         case TAR_CHAR_SELF:
            vo = ( void * )ch;
            break;

         case TAR_OBJ_INV:
         {
            if( ( obj = get_obj_carry( ch, argument ) ) == NULL )
            {
               send_to_char( "You can't find that.\n\r", ch );
               return TRUE;
            }
         }
            vo = ( void * )obj;
            break;
      }

// WAIT_STATE( ch, skill_table[sn]->beats );
      if( ( number_percent(  ) + skill_table[sn]->difficulty * 5 ) > ( IS_NPC( ch ) ? 75 : LEARNED( ch, sn ) ) )
      {
         failed_casting( skill_table[sn], ch, vo, obj );
         learn_from_failure( ch, sn );
         if( mana )
         {
            if( IS_VAMPIRE( ch ) )
               ch->blood -= blood / 2;
            else
               ch->mana -= mana / 2;
         }
         return TRUE;
      }
      if( mana )
      {
         if( IS_VAMPIRE( ch ) )
            ch->blood -= blood;
         else
            ch->mana -= mana;
      }
      start_timer( &time_used );
      retcode = ( *skill_table[sn]->spell_fun ) ( sn, ch->level, ch, vo );
      end_timer( &time_used );
      update_userec( &time_used, &skill_table[sn]->userec );

      if( retcode == rCHAR_DIED || retcode == rERROR )
         return TRUE;

      if( char_died( ch ) )
         return TRUE;

      if( retcode == rSPELL_FAILED )
      {
         learn_from_failure( ch, sn );
         retcode = rNONE;
      }
      else
         learn_from_success( ch, sn );

      if( skill_table[sn]->target == TAR_CHAR_OFFENSIVE && victim != ch && !char_died( victim ) )
      {
         CHAR_DATA *vch;
         CHAR_DATA *vch_next;

         for( vch = ch->in_room->first_person; vch; vch = vch_next )
         {
            vch_next = vch->next_in_room;
            if( victim == vch && !victim->fighting && victim->master != ch )
            {
               retcode = multi_hit( victim, ch, TYPE_UNDEFINED );
               break;
            }
         }
      }
      return TRUE;
   }

   if( mana )
   {
      if( IS_VAMPIRE( ch ) )
         ch->blood -= blood;
      else
         ch->mana -= mana;
   }
   ch->prev_cmd = ch->last_cmd;
   ch->last_cmd = skill_table[sn]->skill_fun;
   start_timer( &time_used );
   ( *skill_table[sn]->skill_fun ) ( ch, argument );
   end_timer( &time_used );
   update_userec( &time_used, &skill_table[sn]->userec );

   tail_chain(  );
   return TRUE;
}

void do_skin( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *korps;
   OBJ_DATA *corpse;
   OBJ_DATA *obj;
   OBJ_DATA *skin;
   bool found;
   char *name;
   char buf[MAX_STRING_LENGTH];
   found = FALSE;

   if( !IS_PKILL( ch ) && !IS_IMMORTAL( ch ) )
   {
      send_to_char( "Leave the hideous defilings to the killers!\n", ch );
      return;
   }
   if( argument[0] == '\0' )
   {
      send_to_char( "Whose corpse do you wish to skin?\n\r", ch );
      return;
   }
   if( ( corpse = get_obj_here( ch, argument ) ) == NULL )
   {
      send_to_char( "You cannot find that here.\n\r", ch );
      return;
   }
   if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You have no weapon with which to perform this deed.\n\r", ch );
      return;
   }
   if( corpse->item_type != ITEM_CORPSE_PC )
   {
      send_to_char( "You can only skin the bodies of player characters.\n\r", ch );
      return;
   }
   if( obj->value[3] != 1 && obj->value[3] != 2 && obj->value[3] != 3 && obj->value[3] != 11 )
   {
      send_to_char( "There is nothing you can do with this corpse.\n\r", ch );
      return;
   }
   if( get_obj_index( OBJ_VNUM_SKIN ) == NULL )
   {
      bug( "Vnum 23 (OBJ_VNUM_SKIN) not found for do_skin!", 0 );
      return;
   }
   korps = create_object( get_obj_index( OBJ_VNUM_CORPSE_PC ), 0 );
   skin = create_object( get_obj_index( OBJ_VNUM_SKIN ), 0 );
   name = IS_NPC( ch ) ? korps->short_descr : corpse->short_descr;
   sprintf( buf, skin->short_descr, name );
   STRFREE( skin->short_descr );
   skin->short_descr = STRALLOC( buf );
   sprintf( buf, skin->description, name );
   STRFREE( skin->description );
   skin->description = STRALLOC( buf );
   act( AT_BLOOD, "$n strips the skin from $p.", ch, corpse, NULL, TO_ROOM );
   act( AT_BLOOD, "You strip the skin from $p.", ch, corpse, NULL, TO_CHAR );
   obj_to_char( skin, ch );
   return;
}

void do_slookup( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   int sn;
   int iClass, iRace;
   SKILLTYPE *skill = NULL;

   one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      send_to_char( "Slookup what?\n\r", ch );
      return;
   }

   if( !str_cmp( arg, "all" ) )
   {
      for( sn = 0; sn < top_sn && skill_table[sn] && skill_table[sn]->name; sn++ )
         pager_printf( ch, "Sn: %4d Slot: %4d Skill/spell: '%-20s' Damtype: %s\n\r",
                       sn, skill_table[sn]->slot, skill_table[sn]->name, spell_damage[SPELL_DAMAGE( skill_table[sn] )] );
   }
   else if( !str_cmp( arg, "herbs" ) )
   {
      for( sn = 0; sn < top_herb && herb_table[sn] && herb_table[sn]->name; sn++ )
         pager_printf( ch, "%d) %s\n\r", sn, herb_table[sn]->name );
   }
   else
   {
      ACADIA_AFF *aff;
      int cnt = 0;

      if( arg[0] == 'h' && is_number( arg + 1 ) )
      {
         sn = atoi( arg + 1 );
         if( !IS_VALID_HERB( sn ) )
         {
            send_to_char( "Invalid herb.\n\r", ch );
            return;
         }
         skill = herb_table[sn];
      }
      else if( is_number( arg ) )
      {
         sn = atoi( arg );
         if( ( skill = get_skilltype( sn ) ) == NULL )
         {
            send_to_char( "Invalid sn.\n\r", ch );
            return;
         }
         sn %= 1000;
      }
      else if( ( sn = skill_lookup( arg ) ) >= 0 )
         skill = skill_table[sn];
      else if( ( sn = herb_lookup( arg ) ) >= 0 )
         skill = herb_table[sn];
      else
      {
         send_to_char( "No such skill, spell, proficiency or tongue.\n\r", ch );
         return;
      }
      if( !skill )
      {
         send_to_char( "Not created yet.\n\r", ch );
         return;
      }

      ch_printf( ch, "Sn: %4d Slot: %4d %s: '%-20s'\n\r", sn, skill->slot, skill_tname[skill->type], skill->name );
      if( skill->info )
         ch_printf( ch, "DamType: %s  ActType: %s   ClassType: %s   PowerType: %s\n\r",
                    spell_damage[SPELL_DAMAGE( skill )],
                    spell_action[SPELL_ACTION( skill )],
                    spell_class[SPELL_CLASS( skill )], spell_power[SPELL_POWER( skill )] );
      if( skill->flags )
      {
         int x;

         strcpy( buf, "Flags:" );
         for( x = 0; x < 32; x++ )
            if( SPELL_FLAG( skill, 1 << x ) )
            {
               strcat( buf, " " );
               strcat( buf, spell_flag[x] );
            }
         strcat( buf, "\n\r" );
         send_to_char( buf, ch );
      }
      ch_printf( ch, "Saves: %s  SaveEffect: %s\n\r",
                 spell_saves[( int )skill->saves], spell_save_effect[SPELL_SAVE( skill )] );

      if( skill->difficulty != '\0' )
         ch_printf( ch, "Difficulty: %d\n\r", ( int )skill->difficulty );

      ch_printf( ch, "Type: %s  Target: %s  Minpos: %d  Mana: %d  Beats: %d  Range: %d\n\r",
                 skill_tname[skill->type],
                 target_type[URANGE( TAR_IGNORE, skill->target, TAR_OBJ_INV )],
                 skill->minimum_position, skill->min_mana, skill->beats, skill->range );
      ch_printf( ch, "Flags: %d  Guild: %d  Value: %d  Info: %d  Code: %s\n\r",
                 skill->flags,
                 skill->guild,
                 skill->value,
                 skill->info, skill->skill_fun ? skill_name( skill->skill_fun ) : spell_name( skill->spell_fun ) );
      ch_printf( ch, "Sectors Allowed: %s\n", skill->spell_sector ? flag_string( skill->spell_sector, sec_flags ) : "All" );
      ch_printf( ch, "Dammsg: %s\n\rWearoff: %s\n", skill->noun_damage, skill->msg_off ? skill->msg_off : "(none set)" );
      if( skill->dice && skill->dice[0] != '\0' )
         ch_printf( ch, "Dice: %s\n\r", skill->dice );
      if( skill->teachers && skill->teachers[0] != '\0' )
         ch_printf( ch, "Teachers: %s\n\r", skill->teachers );
      if( skill->components && skill->components[0] != '\0' )
         ch_printf( ch, "Components: %s\n\r", skill->components );
      if( skill->participants )
         ch_printf( ch, "Participants: %d\n\r", ( int )skill->participants );
      if( skill->userec.num_uses )
         send_timer( &skill->userec, ch );
      for( aff = skill->affects; aff; aff = aff->next )
      {
         if( aff == skill->affects )
            send_to_char( "\n\r", ch );
         sprintf( buf, "Affect %d", ++cnt );
         if( aff->location )
         {
            strcat( buf, " modifies " );
            strcat( buf, a_types[aff->location % REVERSE_APPLY] );
            strcat( buf, " by '" );
            strcat( buf, aff->modifier );
            if( aff->bitvector != -1 )
               strcat( buf, "' and" );
            else
               strcat( buf, "'" );
         }
         if( aff->bitvector != -1 )
         {
            strcat( buf, " applies " );
            strcat( buf, a_flags[aff->bitvector] );
         }
         if( aff->duration[0] != '\0' && aff->duration[0] != '0' )
         {
            strcat( buf, " for '" );
            strcat( buf, aff->duration );
            strcat( buf, "' rounds" );
         }
         if( aff->location >= REVERSE_APPLY )
            strcat( buf, " (affects caster only)" );
         strcat( buf, "\n\r" );
         send_to_char( buf, ch );

         if( !aff->next )
            send_to_char( "\n\r", ch );
      }

      if( skill->hit_char && skill->hit_char[0] != '\0' )
         ch_printf( ch, "Hitchar   : %s\n\r", skill->hit_char );
      if( skill->hit_vict && skill->hit_vict[0] != '\0' )
         ch_printf( ch, "Hitvict   : %s\n\r", skill->hit_vict );
      if( skill->hit_room && skill->hit_room[0] != '\0' )
         ch_printf( ch, "Hitroom   : %s\n\r", skill->hit_room );
      if( skill->hit_dest && skill->hit_dest[0] != '\0' )
         ch_printf( ch, "Hitdest   : %s\n\r", skill->hit_dest );
      if( skill->miss_char && skill->miss_char[0] != '\0' )
         ch_printf( ch, "Misschar  : %s\n\r", skill->miss_char );
      if( skill->miss_vict && skill->miss_vict[0] != '\0' )
         ch_printf( ch, "Missvict  : %s\n\r", skill->miss_vict );
      if( skill->miss_room && skill->miss_room[0] != '\0' )
         ch_printf( ch, "Missroom  : %s\n\r", skill->miss_room );
      if( skill->die_char && skill->die_char[0] != '\0' )
         ch_printf( ch, "Diechar   : %s\n\r", skill->die_char );
      if( skill->die_vict && skill->die_vict[0] != '\0' )
         ch_printf( ch, "Dievict   : %s\n\r", skill->die_vict );
      if( skill->die_room && skill->die_room[0] != '\0' )
         ch_printf( ch, "Dieroom   : %s\n\r", skill->die_room );
      if( skill->imm_char && skill->imm_char[0] != '\0' )
         ch_printf( ch, "Immchar   : %s\n\r", skill->imm_char );
      if( skill->imm_vict && skill->imm_vict[0] != '\0' )
         ch_printf( ch, "Immvict   : %s\n\r", skill->imm_vict );
      if( skill->imm_room && skill->imm_room[0] != '\0' )
         ch_printf( ch, "Immroom   : %s\n\r", skill->imm_room );
      if( skill->type != SKILL_HERB )
      {
         if( skill->type != SKILL_RACIAL )
         {
            send_to_char( "--------------------------[CLASS USE]--------------------------\n\r", ch );
            for( iClass = 0; iClass < MAX_PC_CLASS; iClass++ )
            {
               strcpy( buf, class_table[iClass]->who_name );
               sprintf( buf + 3, ") lvl: %3d(%d) max: %2d%%",
                        skill->skill_level[iClass], skill->skill_slevel[iClass], skill->skill_adept[iClass] );
               if( iClass % 3 == 2 )
                  strcat( buf, "\n\r" );
               else
                  strcat( buf, "  " );
               send_to_char( buf, ch );
            }
         }
         else
         {
            send_to_char( "\n\r--------------------------[RACE USE]--------------------------\n\r", ch );
            for( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
            {
               sprintf( buf, "%8.8s) lvl: %3d max: %2d%%",
                        race_table[iRace]->race_name, skill->race_level[iRace], skill->race_adept[iRace] );
               if( !strcmp( race_table[iRace]->race_name, "unused" ) )
                  sprintf( buf, "                           " );
               if( ( iRace > 0 ) && ( iRace % 2 == 1 ) )
                  strcat( buf, "\n\r" );
               else
                  strcat( buf, "  " );
               send_to_char( buf, ch );
            }
         }

      }
      send_to_char( "\n\r", ch );

   }

   return;
}

void do_slearn( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   int sn;
   int iClass, iRace;
   SKILLTYPE *skill = NULL;

   one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: slearn <spell/skill>\n\r", ch );
      return;
   }

   if( ( sn = skill_lookup( arg ) ) >= 0 )
   {
      skill = skill_table[sn];
   }
   else
   {
      send_to_char( "No such skill, spell, proficiency or tongue.\n\r", ch );
      return;
   }
   if( !skill )
   {
      send_to_char( "Not created yet.\n\r", ch );
      return;
   }

   sprintf( buf,
            "&WClasses and or races that can use &G%s&W are listed below,\n\ralong with their levels atained and %% adepted.\n\n\r",
            capitalize( skill->name ) );
   send_to_char( buf, ch );

   if( skill->type != SKILL_RACIAL )
   {
      send_to_char( "--------------------------[CLASS USE]--------------------------\n\r", ch );
      if( IS_AGOD( ch ) )
      {
         for( iClass = 0; iClass < MAX_PC_CLASS; iClass++ )
         {
            sprintf( buf, "&WClass &G%2d %-12.12s&W gets it at level &R%3d(%3d)&W and adepts to &Y%d&W%%\n\r",
                     iClass, class_table[iClass]->who_name, skill->skill_level[iClass], skill->skill_slevel[iClass],
                     skill->skill_adept[iClass] );
            send_to_char( buf, ch );
         }
      }
      else
      {
         for( iClass = 0; iClass < MAX_PC_CLASS; iClass++ )
         {
            sprintf( buf, "&WClass &G%-12.12s&W gets it at level &R%3d(%3d)&W and adepts to &Y%d&W%%\n\r",
                     class_table[iClass]->who_name, skill->skill_level[iClass], skill->skill_slevel[iClass],
                     skill->skill_adept[iClass] );
            send_to_char( buf, ch );
         }
      }
   }
   else
   {
      send_to_char( "--------------------------[RACE USE]--------------------------\n\r", ch );
      if( IS_AGOD( ch ) )
      {
         for( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
         {
            sprintf( buf, "&WRace &G%2d %-12.12s&W gets it at level &R%3d&W and adepts to &Y%d&W%%\n\r",
                     iRace, race_table[iRace]->race_name, skill->race_level[iRace], skill->race_adept[iRace] );
            send_to_char( buf, ch );
         }
      }
      else
      {
         for( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
         {
            sprintf( buf, "&WRace &G%-12.12s&W gets it at level &R%3d&W and adepts to &Y%d&W%%\n\r",
                     race_table[iRace]->race_name, skill->race_level[iRace], skill->race_adept[iRace] );
            send_to_char( buf, ch );
         }
      }
   }
   send_to_char( "\n\r", ch );
   if( ch->pcdata->learned[sn] == 0 )
   {
      sprintf( buf, "&WYou have not practiced &G%s&W at all.", capitalize( skill->name ) );
      send_to_char( buf, ch );
   }
   else
   {
      sprintf( buf, "&WYou have practiced &G%s&W to &Y%d&W percent.", capitalize( skill->name ), ch->pcdata->learned[sn] );
      send_to_char( buf, ch );
   }
   send_to_char( "&D\n\n\r", ch );
   return;
}

void do_sset( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int value;
   int sn, i;
   bool fAll;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0' )
   {
      send_to_char( "Syntax: sset <victim> <skill> <value>\n\r", ch );
      send_to_char( "or:     sset <victim> all     <value>\n\r", ch );
      if( get_trust( ch ) >= LEVEL_SUBADMIN )
      {
         send_to_char( "or:     sset save skill table\n\r", ch );
         send_to_char( "or:     sset save herb table\n\r", ch );
         send_to_char( "or:     sset create skill 'new skill'\n\r", ch );
         send_to_char( "or:     sset create herb 'new herb'\n\r", ch );
         send_to_char( "or:     sset create ability 'new ability'\n\r", ch );
         send_to_char( "or:     sset <sn>     <field> <value>\n\r", ch );
         send_to_char( "\n\rField being one of:\n\r", ch );
         send_to_char( "  name code target minpos slot mana beats dammsg wearoff guild minlevel\n\r", ch );
         send_to_char( "  type damtype acttype classtype powertype seffect flag dice value difficulty\n\r", ch );
         send_to_char( "  affect rmaffect level slevel adept hit miss die imm (char/vict/room)\n\r", ch );
         send_to_char( "  components teachers racelevel raceadept\n\r", ch );
         send_to_char( "  sector\n\r", ch );
         send_to_char( "Affect having the fields: <location> <modfifier> [duration] [bitvector]\n\r", ch );
         send_to_char( "(See AFFECTTYPES for location, and AFFECTED_BY for bitvector)\n\r", ch );
      }
      send_to_char( "Skill being any skill or spell.\n\r", ch );
      return;
   }

   if( get_trust( ch ) >= LEVEL_SUBADMIN && !str_cmp( arg1, "asave" ) && !str_cmp( argument, "table" ) )
   {
      if( !str_cmp( arg2, "skill" ) )
      {
         send_to_char( "Saving skill table...\n\r", ch );
         save_skill_table(  );
         save_classes(  );
         sprintf( buf, "&R<&BINFO&R> A new skill has been added, check your slists!&D" );
         talk_info( AT_PLAIN, buf );
         return;
      }
      if( !str_cmp( arg2, "herb" ) )
      {
         send_to_char( "Saving herb table...\n\r", ch );
         save_herb_table(  );
         return;
      }
   }

   if( get_trust( ch ) >= LEVEL_SUBADMIN && !str_cmp( arg1, "save" ) && !str_cmp( argument, "table" ) )
   {
      if( !str_cmp( arg2, "skill" ) )
      {
         send_to_char( "Saving skill table...\n\r", ch );
         save_skill_table(  );
         save_classes(  );
         return;
      }
      if( !str_cmp( arg2, "herb" ) )
      {
         send_to_char( "Saving herb table...\n\r", ch );
         save_herb_table(  );
         return;
      }
   }
   if( get_trust( ch ) >= LEVEL_SUBADMIN
       && !str_cmp( arg1, "create" )
       && ( !str_cmp( arg2, "skill" ) || !str_cmp( arg2, "herb" ) || !str_cmp( arg2, "ability" ) ) )
   {
      struct skill_type *skill;
      sh_int type = SKILL_UNKNOWN;

      if( !str_cmp( arg2, "herb" ) )
      {
         type = SKILL_HERB;
         if( top_herb >= MAX_HERB )
         {
            ch_printf( ch, "The current top herb is %d, which is the maximum.  "
                       "To add more herbs,\n\rMAX_HERB will have to be "
                       "raised in mud.h, and the mud recompiled.\n\r", top_herb );
            return;
         }
      }
      else if( top_sn >= MAX_SKILL )
      {
         ch_printf( ch, "The current top sn is %d, which is the maximum.  "
                    "To add more skills,\n\rMAX_SKILL will have to be "
                    "raised in mud.h, and the mud recompiled.\n\r", top_sn );
         return;
      }
      CREATE( skill, struct skill_type, 1 );
      skill->slot = 0;
      if( type == SKILL_HERB )
      {
         int max, x;

         herb_table[top_herb++] = skill;
         for( max = x = 0; x < top_herb - 1; x++ )
            if( herb_table[x] && herb_table[x]->slot > max )
               max = herb_table[x]->slot;
         skill->slot = max + 1;
      }
      else
         skill_table[top_sn++] = skill;
      skill->min_mana = 0;
      skill->name = str_dup( argument );
      skill->noun_damage = str_dup( "" );
      skill->msg_off = str_dup( "" );
      skill->spell_fun = spell_acadia;
      skill->type = type;
      skill->spell_sector = 0;
      skill->guild = -1;
      if( !str_cmp( arg2, "ability" ) )
         skill->type = SKILL_RACIAL;

      for( i = 0; i < MAX_PC_CLASS; i++ )
      {
         skill->skill_level[i] = LEVEL_IMMORTAL;
         skill->skill_adept[i] = 95;
      }
      for( i = 0; i < MAX_PC_RACE; i++ )
      {
         skill->race_level[i] = LEVEL_IMMORTAL;
         skill->race_adept[i] = 95;
      }

      send_to_char( "Done.\n\r", ch );
      return;
   }

   if( arg1[0] == 'h' )
      sn = atoi( arg1 + 1 );
   else
      sn = atoi( arg1 );
   if( get_trust( ch ) >= LEVEL_SUBADMIN
       && ( ( arg1[0] == 'h' && is_number( arg1 + 1 ) && ( sn = atoi( arg1 + 1 ) ) >= 0 )
            || ( is_number( arg1 ) && ( sn = atoi( arg1 ) ) >= 0 ) ) )
   {
      struct skill_type *skill;

      if( arg1[0] == 'h' )
      {
         if( sn >= top_herb )
         {
            send_to_char( "Herb number out of range.\n\r", ch );
            return;
         }
         skill = herb_table[sn];
      }
      else
      {
         if( ( skill = get_skilltype( sn ) ) == NULL )
         {
            send_to_char( "Skill number out of range.\n\r", ch );
            return;
         }
         sn %= 1000;
      }

      if( !str_cmp( arg2, "difficulty" ) )
      {
         skill->difficulty = atoi( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "participants" ) )
      {
         skill->participants = atoi( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "damtype" ) )
      {
         int x = get_sdamage( argument );

         if( x == -1 )
            send_to_char( "Not a spell damage type.\n\r", ch );
         else
         {
            SET_SDAM( skill, x );
            send_to_char( "Ok.\n\r", ch );
         }
         return;
      }
      if( !str_cmp( arg2, "acttype" ) )
      {
         int x = get_saction( argument );

         if( x == -1 )
            send_to_char( "Not a spell action type.\n\r", ch );
         else
         {
            SET_SACT( skill, x );
            send_to_char( "Ok.\n\r", ch );
         }
         return;
      }
      if( !str_cmp( arg2, "classtype" ) )
      {
         int x = get_sclass( argument );

         if( x == -1 )
            send_to_char( "Not a spell class type.\n\r", ch );
         else
         {
            SET_SCLA( skill, x );
            send_to_char( "Ok.\n\r", ch );
         }
         return;
      }
      if( !str_cmp( arg2, "powertype" ) )
      {
         int x = get_spower( argument );

         if( x == -1 )
            send_to_char( "Not a spell power type.\n\r", ch );
         else
         {
            SET_SPOW( skill, x );
            send_to_char( "Ok.\n\r", ch );
         }
         return;
      }
      if( !str_cmp( arg2, "seffect" ) )
      {
         int x = get_ssave_effect( argument );

         if( x == -1 )
            send_to_char( "Not a spell save effect type.\n\r", ch );
         else
         {
            SET_SSAV( skill, x );
            send_to_char( "Ok.\n\r", ch );
         }
         return;
      }
      if( !str_cmp( arg2, "flag" ) )
      {
         int x = get_sflag( argument );

         if( x == -1 )
            send_to_char( "Not a spell flag.\n\r", ch );
         else
         {
            TOGGLE_BIT( skill->flags, 1 << x );
            send_to_char( "Ok.\n\r", ch );
         }
         return;
      }
      if( !str_cmp( arg2, "saves" ) )
      {
         int x = get_ssave( argument );

         if( x == -1 )
            send_to_char( "Not a saving type.\n\r", ch );
         else
         {
            skill->saves = x;
            send_to_char( "Ok.\n\r", ch );
         }
         return;
      }

      if( !str_cmp( arg2, "code" ) )
      {
         SPELL_FUN *spellfun;
         DO_FUN *dofun;

         if( ( spellfun = spell_function( argument ) ) != spell_notfound )
         {
            skill->spell_fun = spellfun;
            skill->skill_fun = NULL;
         }
         else if( ( dofun = skill_function( argument ) ) != skill_notfound )
         {
            skill->skill_fun = dofun;
            skill->spell_fun = NULL;
         }
         else
         {
            send_to_char( "Not a spell or skill.\n\r", ch );
            return;
         }
         send_to_char( "Ok.\n\r", ch );
         return;
      }

      if( !str_cmp( arg2, "target" ) )
      {
         int x = get_starget( argument );

         if( x == -1 )
            send_to_char( "Not a valid target type.\n\r", ch );
         else
         {
            skill->target = x;
            send_to_char( "Ok.\n\r", ch );
         }
         return;
      }
      if( !str_cmp( arg2, "minpos" ) )
      {
         skill->minimum_position = URANGE( POS_DEAD, atoi( argument ), POS_DRAG );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "minlevel" ) )
      {
         skill->min_level = URANGE( 1, atoi( argument ), MAX_LEVEL );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "sector" ) )
      {
         char tmp_arg[MAX_STRING_LENGTH];

         while( argument[0] != '\0' )
         {
            argument = one_argument( argument, tmp_arg );
            value = get_secflag( tmp_arg );
            if( value < 0 || value > MAX_SECFLAG )
               ch_printf( ch, "Unknown flag: %s\n\r", tmp_arg );
            else
               TOGGLE_BIT( skill->spell_sector, ( 1 << value ) );
         }
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "slot" ) )
      {
         skill->slot = URANGE( 0, atoi( argument ), 30000 );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "mana" ) )
      {
         skill->min_mana = URANGE( 0, atoi( argument ), 2000 );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "beats" ) )
      {
         skill->beats = URANGE( 0, atoi( argument ), 120 );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "range" ) )
      {
         skill->range = URANGE( 0, atoi( argument ), 20 );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "guild" ) )
      {
         skill->guild = atoi( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "value" ) )
      {
         skill->value = atoi( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "type" ) )
      {
         skill->type = get_skill( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "rmaffect" ) )
      {
         ACADIA_AFF *aff = skill->affects;
         ACADIA_AFF *aff_next;
         int num = atoi( argument );
         int cnt = 1;

         if( !aff )
         {
            send_to_char( "This spell has no special affects to remove.\n\r", ch );
            return;
         }
         if( num == 1 )
         {
            skill->affects = aff->next;
            DISPOSE( aff->duration );
            DISPOSE( aff->modifier );
            DISPOSE( aff );
            send_to_char( "Removed.\n\r", ch );
            return;
         }
         for( ; aff; aff = aff->next )
         {
            if( ++cnt == num && ( aff_next = aff->next ) != NULL )
            {
               aff->next = aff_next->next;
               DISPOSE( aff_next->duration );
               DISPOSE( aff_next->modifier );
               DISPOSE( aff_next );
               send_to_char( "Removed.\n\r", ch );
               return;
            }
         }
         send_to_char( "Not found.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "affect" ) )
      {
         char location[MAX_INPUT_LENGTH];
         char modifier[MAX_INPUT_LENGTH];
         char duration[MAX_INPUT_LENGTH];
         int loc, bit, tmpbit;
         ACADIA_AFF *aff;

         argument = one_argument( argument, location );
         argument = one_argument( argument, modifier );
         argument = one_argument( argument, duration );

         if( location[0] == '!' )
            loc = get_atype( location + 1 ) + REVERSE_APPLY;
         else
            loc = get_atype( location );
         if( ( loc % REVERSE_APPLY ) < 0 || ( loc % REVERSE_APPLY ) >= MAX_APPLY_TYPE )
         {
            send_to_char( "Unknown affect location.  See AFFECTTYPES.\n\r", ch );
            return;
         }
         bit = -1;
         if( argument[0] != '\0' )
         {
            if( ( tmpbit = get_aflag( argument ) ) == -1 )
               ch_printf( ch, "Unknown bitvector: %s.  See AFFECTED_BY\n\r", argument );
            else
               bit = tmpbit;
         }
         CREATE( aff, ACADIA_AFF, 1 );
         if( !str_cmp( duration, "0" ) )
            duration[0] = '\0';
         if( !str_cmp( modifier, "0" ) )
            modifier[0] = '\0';
         aff->duration = str_dup( duration );
         aff->location = loc;
         aff->modifier = str_dup( modifier );
         aff->bitvector = bit;
         aff->next = skill->affects;
         skill->affects = aff;
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "level" ) )
      {
         char arg3[MAX_INPUT_LENGTH];
         int class;

         argument = one_argument( argument, arg3 );
         class = atoi( arg3 );
         if( class >= MAX_PC_CLASS || class < 0 )
            send_to_char( "Not a valid class.\n\r", ch );
         else
            skill->skill_level[class] = URANGE( 0, atoi( argument ), MAX_LEVEL );
         return;
      }
      if( !str_cmp( arg2, "slevel" ) )
      {
         char arg3[MAX_INPUT_LENGTH];
         int class;

         argument = one_argument( argument, arg3 );
         class = atoi( arg3 );
         if( class >= MAX_PC_CLASS || class < 0 )
            send_to_char( "Not a valid class.\n\r", ch );
         else
            skill->skill_slevel[class] = URANGE( 0, atoi( argument ), 999 );
         return;
      }
      if( !str_cmp( arg2, "racelevel" ) )
      {
         char arg3[MAX_INPUT_LENGTH];
         int race;

         argument = one_argument( argument, arg3 );
         race = atoi( arg3 );
         if( race >= MAX_PC_RACE || race < 0 )
            send_to_char( "Not a valid race.\n\r", ch );
         else
            skill->race_level[race] = URANGE( 0, atoi( argument ), MAX_LEVEL );
         return;
      }
      if( !str_cmp( arg2, "adept" ) )
      {
         char arg3[MAX_INPUT_LENGTH];
         int class;

         argument = one_argument( argument, arg3 );
         class = atoi( arg3 );
         if( class >= MAX_PC_CLASS || class < 0 )
            send_to_char( "Not a valid class.\n\r", ch );
         else
            skill->skill_adept[class] = URANGE( 0, atoi( argument ), 100 );
         return;
      }
      if( !str_cmp( arg2, "raceadept" ) )
      {
         char arg3[MAX_INPUT_LENGTH];
         int race;

         argument = one_argument( argument, arg3 );
         race = atoi( arg3 );
         if( race >= MAX_PC_RACE || race < 0 )
            send_to_char( "Not a valid race.\n\r", ch );
         else
            skill->race_adept[race] = URANGE( 0, atoi( argument ), 100 );
         return;
      }


      if( !str_cmp( arg2, "name" ) )
      {
         DISPOSE( skill->name );
         skill->name = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "dammsg" ) )
      {
         DISPOSE( skill->noun_damage );
         if( !str_cmp( argument, "clear" ) )
            skill->noun_damage = str_dup( "" );
         else
            skill->noun_damage = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "wearoff" ) )
      {
         DISPOSE( skill->msg_off );
         if( str_cmp( argument, "clear" ) )
            skill->msg_off = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "hitchar" ) )
      {
         if( skill->hit_char )
            DISPOSE( skill->hit_char );
         if( str_cmp( argument, "clear" ) )
            skill->hit_char = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "hitvict" ) )
      {
         if( skill->hit_vict )
            DISPOSE( skill->hit_vict );
         if( str_cmp( argument, "clear" ) )
            skill->hit_vict = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "hitroom" ) )
      {
         if( skill->hit_room )
            DISPOSE( skill->hit_room );
         if( str_cmp( argument, "clear" ) )
            skill->hit_room = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "hitdest" ) )
      {
         if( skill->hit_dest )
            DISPOSE( skill->hit_dest );
         if( str_cmp( argument, "clear" ) )
            skill->hit_dest = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "misschar" ) )
      {
         if( skill->miss_char )
            DISPOSE( skill->miss_char );
         if( str_cmp( argument, "clear" ) )
            skill->miss_char = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "missvict" ) )
      {
         if( skill->miss_vict )
            DISPOSE( skill->miss_vict );
         if( str_cmp( argument, "clear" ) )
            skill->miss_vict = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "missroom" ) )
      {
         if( skill->miss_room )
            DISPOSE( skill->miss_room );
         if( str_cmp( argument, "clear" ) )
            skill->miss_room = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "diechar" ) )
      {
         if( skill->die_char )
            DISPOSE( skill->die_char );
         if( str_cmp( argument, "clear" ) )
            skill->die_char = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "dievict" ) )
      {
         if( skill->die_vict )
            DISPOSE( skill->die_vict );
         if( str_cmp( argument, "clear" ) )
            skill->die_vict = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "dieroom" ) )
      {
         if( skill->die_room )
            DISPOSE( skill->die_room );
         if( str_cmp( argument, "clear" ) )
            skill->die_room = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "immchar" ) )
      {
         if( skill->imm_char )
            DISPOSE( skill->imm_char );
         if( str_cmp( argument, "clear" ) )
            skill->imm_char = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "immvict" ) )
      {
         if( skill->imm_vict )
            DISPOSE( skill->imm_vict );
         if( str_cmp( argument, "clear" ) )
            skill->imm_vict = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "immroom" ) )
      {
         if( skill->imm_room )
            DISPOSE( skill->imm_room );
         if( str_cmp( argument, "clear" ) )
            skill->imm_room = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "dice" ) )
      {
         if( skill->dice )
            DISPOSE( skill->dice );
         if( str_cmp( argument, "clear" ) )
            skill->dice = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "components" ) )
      {
         if( skill->components )
            DISPOSE( skill->components );
         if( str_cmp( argument, "clear" ) )
            skill->components = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      if( !str_cmp( arg2, "teachers" ) )
      {
         if( skill->teachers )
            DISPOSE( skill->teachers );
         if( str_cmp( argument, "clear" ) )
            skill->teachers = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      do_sset( ch, "" );
      return;
   }

   if( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      if( ( sn = skill_lookup( arg1 ) ) >= 0 )
      {
         sprintf( arg1, "%d %s %s", sn, arg2, argument );
         do_sset( ch, arg1 );
      }
      else
         send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( IS_NPC( victim ) )
   {
      send_to_char( "Not on NPC's.\n\r", ch );
      return;
   }

   fAll = !str_cmp( arg2, "all" );
   sn = 0;
   if( !fAll && ( sn = skill_lookup( arg2 ) ) < 0 )
   {
      send_to_char( "No such skill or spell.\n\r", ch );
      return;
   }

   if( !is_number( argument ) )
   {
      send_to_char( "Value must be numeric.\n\r", ch );
      return;
   }

   value = atoi( argument );
   if( value < 0 || value > 100 )
   {
      send_to_char( "Value range is 0 to 100.\n\r", ch );
      return;
   }

   if( fAll )
   {
      for( sn = 0; sn < top_sn; sn++ )
      {
         if( skill_table[sn]->name
             /*
              * && ( DUAL_SKILL(ch, sn) 
              * || value == 0 )
              */  )
         {
            if( value == 100 && !IS_IMMORTAL( victim ) )
               victim->pcdata->learned[sn] = GET_ADEPT( victim, sn );
            else
               victim->pcdata->learned[sn] = value;
         }
      }
   }
   else
      victim->pcdata->learned[sn] = value;

   return;
}


void learn_from_success( CHAR_DATA * ch, int sn )
{
   int adept, gain, sklvl, learn, percent, chance;

   if( IS_NPC( ch ) || ch->pcdata->learned[sn] <= 0 )
      return;
   adept = dual_adept( ch, sn );
   sklvl = find_skill_level( ch, sn );
   if( sklvl == 0 )
      sklvl = ch->level;
   if( ch->pcdata->learned[sn] < adept )
   {
      chance = ch->pcdata->learned[sn] + ( 5 * skill_table[sn]->difficulty );
      percent = number_percent(  );
      if( percent >= chance )
         learn = 2;
      else if( chance - percent > 25 )
         return;
      else
         learn = 1;
      ch->pcdata->learned[sn] = UMIN( adept, ch->pcdata->learned[sn] + learn );
      if( ch->pcdata->learned[sn] == adept )
      {
         gain = 2 * sklvl;
         set_char_color( AT_WHITE, ch );
         ch_printf( ch, "You are now an adept of %s!  You gain %d bonus experience!\n\r", skill_table[sn]->name, gain );
      }
      else
      {
         gain = sklvl;
         if( !ch->fighting && sn != gsn_hide && sn != gsn_sneak )
         {
            set_char_color( AT_WHITE, ch );
            ch_printf( ch, "You gain %d experience points from your success at %s!\n\r", gain, skill_table[sn]->name );
         }
      }
      if( !IS_MAXED( ch ) )
         gain_exp( ch, gain );
   }
}


void learn_from_failure( CHAR_DATA * ch, int sn )
{
   int adept, chance;

   if( IS_NPC( ch ) || ch->pcdata->learned[sn] <= 0 )
      return;
   chance = ch->pcdata->learned[sn] + ( 5 * skill_table[sn]->difficulty );
   if( chance - number_percent(  ) > 25 )
      return;
   adept = dual_adept( ch, sn );
   if( ch->pcdata->learned[sn] < ( adept - 1 ) )
      ch->pcdata->learned[sn] = UMIN( adept, ch->pcdata->learned[sn] + 1 );
}


void do_gouge( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   AFFECT_DATA af;
   sh_int dam;
   int chance;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( !can_use_skill( ch, 0, gsn_gouge ) )
   {
      send_to_char( "You do not yet know of this skill.\n\r", ch );
      return;
   }

   if( ch->mount )
   {
      send_to_char( "You can't get close enough while mounted.\n\r", ch );
      return;
   }

   if( ( victim = who_fighting( ch ) ) == NULL )
   {
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
   }

   chance = ( ( get_curr_dex( victim ) - get_curr_dex( ch ) ) * 10 ) + 10;
   if( !IS_NPC( ch ) && !IS_NPC( victim ) )
      chance += sysdata.gouge_plr_vs_plr;
   if( victim->fighting && victim->fighting->who != ch )
      chance += sysdata.gouge_nontank;
   if( can_use_skill( ch, ( number_percent(  ) + chance ), gsn_gouge ) )
   {
      dam = number_range( 5, ch->level );
      global_retcode = damage( ch, victim, dam, gsn_gouge );
      if( global_retcode == rNONE )
      {
         if( !IS_AFFECTED( victim, AFF_BLIND ) )
         {
            af.type = gsn_blindness;
            af.location = APPLY_HITROLL;
            af.modifier = -6;
            if( !IS_NPC( victim ) && !IS_NPC( ch ) )
               af.duration = ( ch->level + 10 ) / get_curr_con( victim );
            else
               af.duration = 3 + ( ch->level / 15 );
            af.bitvector = meb( AFF_BLIND );
            affect_to_char( victim, &af );
            act( AT_SKILL, "You can't see a thing!", victim, NULL, NULL, TO_CHAR );
         }
//    WAIT_STATE( ch,     PULSE_VIOLENCE );
         if( !IS_NPC( ch ) && !IS_NPC( victim ) )
         {
            if( number_bits( 1 ) == 0 )
            {
               ch_printf( ch, "%s looks momentarily dazed.\n\r", victim->name );
               send_to_char( "You are momentarily dazed ...\n\r", victim );
               WAIT_STATE( victim, PULSE_VIOLENCE );
            }
         }
         else
            WAIT_STATE( victim, PULSE_VIOLENCE );
      }
      else if( global_retcode == rVICT_DIED )
      {
         act( AT_BLOOD, "Your fingers plunge into your victim's brain, causing immediate death!", ch, NULL, NULL, TO_CHAR );
      }
      if( global_retcode != rCHAR_DIED && global_retcode != rBOTH_DIED )
         learn_from_success( ch, gsn_gouge );
   }
   else
   {
// WAIT_STATE( ch, skill_table[gsn_gouge]->beats );
      global_retcode = damage( ch, victim, 0, gsn_gouge );
      learn_from_failure( ch, gsn_gouge );
   }

   return;
}

void do_detrap( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   OBJ_DATA *trap;
   int percent;
   bool found = FALSE;

   switch ( ch->substate )
   {
      default:
         if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
         {
            send_to_char( "You can't concentrate enough for that.\n\r", ch );
            return;
         }
         argument = one_argument( argument, arg );
         if( !can_use_skill( ch, 0, gsn_detrap ) )
         {
            send_to_char( "You do not yet know of this skill.\n\r", ch );
            return;
         }
         if( arg[0] == '\0' )
         {
            send_to_char( "Detrap what?\n\r", ch );
            return;
         }
         found = FALSE;
         if( ch->mount )
         {
            send_to_char( "You can't do that while mounted.\n\r", ch );
            return;
         }
         if( !ch->in_room->first_content )
         {
            send_to_char( "You can't find that here.\n\r", ch );
            return;
         }
         for( obj = ch->in_room->first_content; obj; obj = obj->next_content )
         {
            if( can_see_obj( ch, obj ) && nifty_is_name( arg, obj->name ) )
            {
               found = TRUE;
               break;
            }
         }
         if( !found )
         {
            send_to_char( "You can't find that here.\n\r", ch );
            return;
         }
         act( AT_ACTION, "You carefully begin your attempt to remove a trap from $p...", ch, obj, NULL, TO_CHAR );
         act( AT_ACTION, "$n carefully attempts to remove a trap from $p...", ch, obj, NULL, TO_ROOM );
         ch->alloc_ptr = str_dup( obj->name );
         add_timer( ch, TIMER_DO_FUN, 3, do_detrap, 1 );
         return;
      case 1:
         if( !ch->alloc_ptr )
         {
            send_to_char( "Your detrapping was interrupted!\n\r", ch );
            bug( "do_detrap: ch->alloc_ptr NULL!", 0 );
            return;
         }
         strcpy( arg, ch->alloc_ptr );
         DISPOSE( ch->alloc_ptr );
         ch->alloc_ptr = NULL;
         ch->substate = SUB_NONE;
         break;
      case SUB_TIMER_DO_ABORT:
         DISPOSE( ch->alloc_ptr );
         ch->substate = SUB_NONE;
         send_to_char( "You carefully stop what you were doing.\n\r", ch );
         return;
   }

   if( !ch->in_room->first_content )
   {
      send_to_char( "You can't find that here.\n\r", ch );
      return;
   }
   for( obj = ch->in_room->first_content; obj; obj = obj->next_content )
   {
      if( can_see_obj( ch, obj ) && nifty_is_name( arg, obj->name ) )
      {
         found = TRUE;
         break;
      }
   }
   if( !found )
   {
      send_to_char( "You can't find that here.\n\r", ch );
      return;
   }
   if( ( trap = get_trap( obj ) ) == NULL )
   {
      send_to_char( "You find no trap on that.\n\r", ch );
      return;
   }

   percent = number_percent(  ) - ( ch->level / 15 ) - ( get_curr_lck( ch ) - 16 );

   separate_obj( obj );
   if( can_use_skill( ch, percent, gsn_detrap ) )
   {
      send_to_char( "Ooops!\n\r", ch );
      spring_trap( ch, trap );
      learn_from_failure( ch, gsn_detrap );
      return;
   }

   extract_obj( trap );

   send_to_char( "You successfully remove a trap.\n\r", ch );
   learn_from_success( ch, gsn_detrap );
   return;
}

void do_dig( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   OBJ_DATA *startobj;
   bool found, shovel;
   EXIT_DATA *pexit;

   switch ( ch->substate )
   {
      default:
         if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
         {
            send_to_char( "You can't concentrate enough for that.\n\r", ch );
            return;
         }
         if( ch->mount )
         {
            send_to_char( "You can't do that while mounted.\n\r", ch );
            return;
         }
         one_argument( argument, arg );
         if( arg[0] != '\0' )
         {
            if( ( pexit = find_door( ch, arg, TRUE ) ) == NULL && get_dir( arg ) == -1 )
            {
               send_to_char( "What direction is that?\n\r", ch );
               return;
            }
            if( pexit )
            {
               if( !IS_SET( pexit->exit_info, EX_DIG ) && !IS_SET( pexit->exit_info, EX_CLOSED ) )
               {
                  send_to_char( "There is no need to dig out that exit.\n\r", ch );
                  return;
               }
            }
         }
         else
         {
            switch ( ch->in_room->sector_type )
            {
               case SECT_CITY:
               case SECT_INSIDE:
                  send_to_char( "The floor is too hard to dig through.\n\r", ch );
                  return;
               case SECT_WATER_SWIM:
               case SECT_WATER_NOSWIM:
               case SECT_UNDERWATER:
                  send_to_char( "You cannot dig here.\n\r", ch );
                  return;
               case SECT_AIR:
                  send_to_char( "What?  In the air?!\n\r", ch );
                  return;
            }
         }
         add_timer( ch, TIMER_DO_FUN, UMIN( skill_table[gsn_dig]->beats / 10, 3 ), do_dig, 1 );
         ch->alloc_ptr = str_dup( arg );
         send_to_char( "You begin digging...\n\r", ch );
         act( AT_PLAIN, "$n begins digging...", ch, NULL, NULL, TO_ROOM );
         return;

      case 1:
         if( !ch->alloc_ptr )
         {
            send_to_char( "Your digging was interrupted!\n\r", ch );
            act( AT_PLAIN, "$n's digging was interrupted!", ch, NULL, NULL, TO_ROOM );
            bug( "do_dig: alloc_ptr NULL", 0 );
            return;
         }
         strcpy( arg, ch->alloc_ptr );
         DISPOSE( ch->alloc_ptr );
         break;

      case SUB_TIMER_DO_ABORT:
         DISPOSE( ch->alloc_ptr );
         ch->substate = SUB_NONE;
         send_to_char( "You stop digging...\n\r", ch );
         act( AT_PLAIN, "$n stops digging...", ch, NULL, NULL, TO_ROOM );
         return;
   }

   ch->substate = SUB_NONE;

   shovel = FALSE;
   for( obj = ch->first_carrying; obj; obj = obj->next_content )
      if( obj->item_type == ITEM_SHOVEL )
      {
         shovel = TRUE;
         break;
      }

   if( arg[0] != '\0' )
   {
      if( ( pexit = find_door( ch, arg, TRUE ) ) != NULL
          && IS_SET( pexit->exit_info, EX_DIG ) && IS_SET( pexit->exit_info, EX_CLOSED ) )
      {
         if( can_use_skill( ch, ( number_percent(  ) * ( shovel ? 1 : 4 ) ), gsn_dig ) )
         {
            REMOVE_BIT( pexit->exit_info, EX_CLOSED );
            send_to_char( "You dig open a passageway!\n\r", ch );
            act( AT_PLAIN, "$n digs open a passageway!", ch, NULL, NULL, TO_ROOM );
            learn_from_success( ch, gsn_dig );
            return;
         }
      }
      learn_from_failure( ch, gsn_dig );
      send_to_char( "Your dig did not discover any exit...\n\r", ch );
      act( AT_PLAIN, "$n's dig did not discover any exit...", ch, NULL, NULL, TO_ROOM );
      return;
   }

   startobj = ch->in_room->first_content;
   found = FALSE;

   for( obj = startobj; obj; obj = obj->next_content )
   {
      if( IS_OBJ_STAT( obj, ITEM_BURIED ) && ( can_use_skill( ch, ( number_percent(  ) * ( shovel ? 1 : 2 ) ), gsn_dig ) ) )
      {
         found = TRUE;
         break;
      }
   }

   if( !found )
   {
      send_to_char( "Your dig uncovered nothing.\n\r", ch );
      act( AT_PLAIN, "$n's dig uncovered nothing.", ch, NULL, NULL, TO_ROOM );
      learn_from_failure( ch, gsn_dig );
      return;
   }

   separate_obj( obj );
   xREMOVE_BIT( obj->extra_flags, ITEM_BURIED );
   act( AT_SKILL, "Your dig uncovered $p!", ch, obj, NULL, TO_CHAR );
   act( AT_SKILL, "$n's dig uncovered $p!", ch, obj, NULL, TO_ROOM );
   learn_from_success( ch, gsn_dig );

   return;
}


void do_search( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   OBJ_DATA *container;
   OBJ_DATA *startobj;
   int percent, door;

   door = -1;
   switch ( ch->substate )
   {
      default:
         if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
         {
            send_to_char( "You can't concentrate enough for that.\n\r", ch );
            return;
         }
         if( ch->mount )
         {
            send_to_char( "You can't do that while mounted.\n\r", ch );
            return;
         }
         argument = one_argument( argument, arg );
         if( arg[0] != '\0' && ( door = get_door( arg ) ) == -1 )
         {
            container = get_obj_here( ch, arg );
            if( !container )
            {
               send_to_char( "You can't find that here.\n\r", ch );
               return;
            }
            if( container->item_type != ITEM_CONTAINER )
            {
               send_to_char( "You can't search in that!\n\r", ch );
               return;
            }
            if( IS_SET( container->value[1], CONT_CLOSED ) )
            {
               send_to_char( "It is closed.\n\r", ch );
               return;
            }
         }
         add_timer( ch, TIMER_DO_FUN, UMIN( skill_table[gsn_search]->beats / 10, 3 ), do_search, 1 );
         send_to_char( "You begin your search...\n\r", ch );
         ch->alloc_ptr = str_dup( arg );
         return;

      case 1:
         if( !ch->alloc_ptr )
         {
            send_to_char( "Your search was interrupted!\n\r", ch );
            bug( "do_search: alloc_ptr NULL", 0 );
            return;
         }
         strcpy( arg, ch->alloc_ptr );
         DISPOSE( ch->alloc_ptr );
         break;
      case SUB_TIMER_DO_ABORT:
         DISPOSE( ch->alloc_ptr );
         ch->substate = SUB_NONE;
         send_to_char( "You stop your search...\n\r", ch );
         return;
   }
   ch->substate = SUB_NONE;
   if( arg[0] == '\0' )
      startobj = ch->in_room->first_content;
   else
   {
      if( ( door = get_door( arg ) ) != -1 )
         startobj = NULL;
      else
      {
         container = get_obj_here( ch, arg );
         if( !container )
         {
            send_to_char( "You can't find that here.\n\r", ch );
            return;
         }
         startobj = container->first_content;
      }
   }

   if( ( !startobj && door == -1 ) || IS_NPC( ch ) )
   {
      send_to_char( "You find nothing.\n\r", ch );
      learn_from_failure( ch, gsn_search );
      return;
   }

   percent = number_percent(  ) + number_percent(  ) - ( ch->level / 10 );

   if( door != -1 )
   {
      EXIT_DATA *pexit;

      if( ( pexit = get_exit( ch->in_room, door ) ) != NULL
          && IS_SET( pexit->exit_info, EX_SECRET )
          && IS_SET( pexit->exit_info, EX_xSEARCHABLE ) && can_use_skill( ch, percent, gsn_search ) )
      {
         act( AT_SKILL, "Your search reveals the $d!", ch, NULL, pexit->keyword, TO_CHAR );
         act( AT_SKILL, "$n finds the $d!", ch, NULL, pexit->keyword, TO_ROOM );
         REMOVE_BIT( pexit->exit_info, EX_SECRET );
         learn_from_success( ch, gsn_search );
         return;
      }
   }
   else
      for( obj = startobj; obj; obj = obj->next_content )
      {
         if( IS_OBJ_STAT( obj, ITEM_HIDDEN ) && can_use_skill( ch, percent, gsn_search ) )
         {
            separate_obj( obj );
            xREMOVE_BIT( obj->extra_flags, ITEM_HIDDEN );
            act( AT_SKILL, "Your search reveals $p!", ch, obj, NULL, TO_CHAR );
            act( AT_SKILL, "$n finds $p!", ch, obj, NULL, TO_ROOM );
            learn_from_success( ch, gsn_search );
            return;
         }
      }

   send_to_char( "You find nothing.\n\r", ch );
   learn_from_failure( ch, gsn_search );
   return;
}

void do_fletch( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];
   OBJ_DATA *obj = NULL;
   OBJ_INDEX_DATA *oi;
   int count = 0;
   int counter;
   int type = 0;

   if( !LEARNED( ch, gsn_forest_lore ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "You don't know enough forest lore to fletch arrows.\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      type = 13;
      sprintf( arg, "standard" );
   }
   else if( !str_cmp( arg, "flaming" ) )
   {
      if( !LEARNED( ch, gsn_flaming_arrow ) )
      {
         send_to_char( "You are not ready for this type of arrow.\n\r", ch );
         return;
      }
      type = 8;
   }
   else if( !str_cmp( arg, "barbed" ) )
   {
      if( !LEARNED( ch, gsn_barbed_arrow ) )
      {
         send_to_char( "You are not ready for this type of arrow.\n\r", ch );
         return;
      }
      type = 11;
   }
   else if( !str_cmp( arg, "shadow" ) )
   {
      if( !LEARNED( ch, gsn_shadow_arrow ) )
      {
         send_to_char( "You are not ready for this type of arrow.\n\r", ch );
         return;
      }
      type = 10;
   }
   else if( !str_cmp( arg, "piercing" ) )
   {
      if( !LEARNED( ch, gsn_piercing_arrow ) )
      {
         send_to_char( "You are not ready for this type of arrow.\n\r", ch );
         return;
      }
      type = 9;
   }
   else if( !str_cmp( arg, "camo" ) )
   {
      if( !LEARNED( ch, gsn_camouflaged_arrow ) )
      {
         send_to_char( "You are not ready for this type of arrow.\n\r", ch );
         return;
      }
      type = 12;
   }
   else
   {
      type = 13;
      sprintf( arg, "standard" );
   }

   count = number_range( 8, 20 );

   for( counter = 0; counter < count; counter++ )
   {
      if( ( oi = get_obj_index( OBJ_VNUM_ARROW_MAKE ) ) == NULL || ( obj = create_object( oi, 1 ) ) == NULL )
      {
         send_to_char( "A procedural error interrupts you.\n\r", ch );
         return;
      }

      separate_obj( obj );

      sprintf( buf, "%s arrow projectile", arg );
      sprintf( buf2, "a %s arrow", arg );
      STRFREE( obj->name );
      STRFREE( obj->short_descr );
      obj->name = STRALLOC( buf );
      obj->short_descr = STRALLOC( buf2 );

      if( ch->level < 100 )
      {
         obj->level = ch->level;
         obj->value[1] = ch->level;
         obj->value[2] = ch->level * 2;
         obj->value[3] = type;

      }
      else
      {
         obj->level = ch->level;
         obj->value[1] = ch->level + ch->sublevel;
         obj->value[2] = ( ch->level + ch->sublevel ) * 2;
         obj->value[3] = type;

      }

      if( CAN_WEAR( obj, ITEM_TAKE ) )
         obj_to_char( obj, ch );
      else
         obj_to_room( obj, ch->in_room );
   }
   if( !IS_IMMORTAL( ch ) )
   {
      WAIT_STATE( ch, 20 );
   }
   sprintf( buf3, "You fletch %d $ps.", count );
   act( AT_ACTION, buf3, ch, obj, NULL, TO_CHAR );
   act( AT_ACTION, "$n fletches some $ps.", ch, obj, NULL, TO_ROOM );
   save_char_obj( ch );
   return;
}

void do_quiver( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   char log_buf[MAX_STRING_LENGTH];
   char logline[MAX_STRING_LENGTH];
   OBJ_DATA *obj;
   OBJ_INDEX_DATA *oi;

   strcpy( arg, argument );
   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: quiver <keywords>\n\r", ch );
      return;
   }

   if( !LEARNED( ch, gsn_forest_lore ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "You don't know enough forest lore to make a quiver.\n\r", ch );
      return;
   }

   if( ( oi = get_obj_index( OBJ_VNUM_QUIVER_MAKE ) ) == NULL || ( obj = create_object( oi, 1 ) ) == NULL )
   {
      send_to_char( "A procedural error interrupts you.\n\r", ch );
      return;
   }
   sprintf( logline, "quiver %s", arg );

   separate_obj( obj );

   STRFREE( obj->name );
   STRFREE( obj->short_descr );
   obj->name = STRALLOC( arg );
   obj->short_descr = STRALLOC( arg );

   if( !IS_IMMORTAL( ch ) )
   {
      obj->level = ch->level;
      obj->value[0] = ch->level + 100;
   }
   else
   {
      obj->level = ch->level;
      obj->value[0] = ch->level + 200;
   }

   if( CAN_WEAR( obj, ITEM_TAKE ) )
      obj_to_char( obj, ch );
   else
      obj_to_room( obj, ch->in_room );

   act( AT_ACTION, "You make a $p.", ch, obj, NULL, TO_CHAR );
   act( AT_ACTION, "$n makes a $p.", ch, obj, NULL, TO_ROOM );
   sprintf( log_buf, "%s: %s", ch->name, logline );
   log_string( log_buf );
   save_char_obj( ch );
   return;
}

void do_bowcraft( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   char bname[MAX_STRING_LENGTH];
   char log_buf[MAX_STRING_LENGTH];
   char logline[MAX_STRING_LENGTH];
   AFFECT_DATA *paf;
   OBJ_DATA *obj;
   OBJ_INDEX_DATA *oi;

   strcpy( arg, argument );
   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: bowcraft <keywords>\n\r", ch );
      return;
   }

   if( !LEARNED( ch, gsn_forest_lore ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "You don't know enough forest lore to make a bow.\n\r", ch );
      return;
   }

   if( ( oi = get_obj_index( OBJ_VNUM_BOW_MAKE ) ) == NULL || ( obj = create_object( oi, 1 ) ) == NULL )
   {
      send_to_char( "A procedural error interrupts you.\n\r", ch );
      return;
   }
   sprintf( logline, "bowcraft %s", arg );

   sprintf( bname, "bow %s", arg );

   separate_obj( obj );

   STRFREE( obj->name );
   STRFREE( obj->short_descr );
   obj->name = STRALLOC( bname );
   obj->short_descr = STRALLOC( arg );

   if( !IS_IMMORTAL( ch ) )
   {
      if( ch->level < 100 )
      {
         obj->level = ch->level;
         obj->value[1] = ch->level * .8;
         obj->value[2] = ch->level * .9;

         CREATE( paf, AFFECT_DATA, 1 );
         paf->type = -1;
         paf->duration = -1;
         paf->location = APPLY_HITROLL;
         paf->modifier = ch->level - 10;
         xCLEAR_BITS( paf->bitvector );
         LINK( paf, obj->first_affect, obj->last_affect, next, prev );

         CREATE( paf, AFFECT_DATA, 1 );
         paf->type = -1;
         paf->duration = -1;
         paf->location = APPLY_DAMROLL;
         paf->modifier = ch->level - 5;
         xCLEAR_BITS( paf->bitvector );
         LINK( paf, obj->first_affect, obj->last_affect, next, prev );
      }
      else
      {
         obj->level = ch->level;
         obj->value[1] = ch->level;
         obj->value[2] = ch->level + 20;

         CREATE( paf, AFFECT_DATA, 1 );
         paf->type = -1;
         paf->duration = -1;
         paf->location = APPLY_HITROLL;
         paf->modifier = ch->level - 10;
         xCLEAR_BITS( paf->bitvector );
         LINK( paf, obj->first_affect, obj->last_affect, next, prev );

         CREATE( paf, AFFECT_DATA, 1 );
         paf->type = -1;
         paf->duration = -1;
         paf->location = APPLY_DAMROLL;
         paf->modifier = ch->level - 5;
         xCLEAR_BITS( paf->bitvector );
         LINK( paf, obj->first_affect, obj->last_affect, next, prev );
      }
   }
   else
   {
      obj->level = ch->level;
      obj->value[1] = ch->level * 200;
      obj->value[2] = ch->level * 400;

      CREATE( paf, AFFECT_DATA, 1 );
      paf->type = -1;
      paf->duration = -1;
      paf->location = APPLY_HITROLL;
      paf->modifier = ch->level * 20;
      xCLEAR_BITS( paf->bitvector );
      LINK( paf, obj->first_affect, obj->last_affect, next, prev );

      CREATE( paf, AFFECT_DATA, 1 );
      paf->type = -1;
      paf->duration = -1;
      paf->location = APPLY_DAMROLL;
      paf->modifier = ch->level * 40;
      xCLEAR_BITS( paf->bitvector );
      LINK( paf, obj->first_affect, obj->last_affect, next, prev );
   }

   if( CAN_WEAR( obj, ITEM_TAKE ) )
      obj_to_char( obj, ch );
   else
      obj_to_room( obj, ch->in_room );

   act( AT_ACTION, "You make a $p.", ch, obj, NULL, TO_CHAR );
   act( AT_ACTION, "$n makes a $p.", ch, obj, NULL, TO_ROOM );
   sprintf( log_buf, "%s: %s", ch->name, logline );
   log_string( log_buf );
   save_char_obj( ch );
   return;
}

void do_craft( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   char log_buf[MAX_STRING_LENGTH];
   char logline[MAX_STRING_LENGTH];
   AFFECT_DATA *paf;
   OBJ_DATA *obj;
   OBJ_INDEX_DATA *oi;

   strcpy( arg, argument );
   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: craft <keywords>\n\r", ch );
      return;
   }

   if( !LEARNED( ch, gsn_force_lore ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "A magical force interrupts you.\n\r", ch );
      return;
   }

   if( ( oi = get_obj_index( OBJ_VNUM_LIGHTSABER_MAKE ) ) == NULL || ( obj = create_object( oi, 1 ) ) == NULL )
   {
      send_to_char( "A procedural error interrupts you.\n\r", ch );
      return;
   }
   sprintf( logline, "craft %s", arg );

   separate_obj( obj );

   STRFREE( obj->name );
   STRFREE( obj->short_descr );
   obj->name = STRALLOC( arg );
   obj->short_descr = STRALLOC( arg );

   if( !IS_IMMORTAL( ch ) )
   {
      if( ch->level < 100 )
      {
         obj->level = ch->level;
         obj->value[1] = ch->level * .8;
         obj->value[2] = ch->level * .9;

         CREATE( paf, AFFECT_DATA, 1 );
         paf->type = -1;
         paf->duration = -1;
         paf->location = APPLY_HITROLL;
         paf->modifier = ch->level - 10;
         xCLEAR_BITS( paf->bitvector );
         LINK( paf, obj->first_affect, obj->last_affect, next, prev );

         CREATE( paf, AFFECT_DATA, 1 );
         paf->type = -1;
         paf->duration = -1;
         paf->location = APPLY_DAMROLL;
         paf->modifier = ch->level - 5;
         xCLEAR_BITS( paf->bitvector );
         LINK( paf, obj->first_affect, obj->last_affect, next, prev );
      }
      else
      {
         obj->level = ch->level;
         obj->value[1] = ch->level;
         obj->value[2] = ch->level + 20;

         CREATE( paf, AFFECT_DATA, 1 );
         paf->type = -1;
         paf->duration = -1;
         paf->location = APPLY_HITROLL;
         paf->modifier = ch->level - 10;
         xCLEAR_BITS( paf->bitvector );
         LINK( paf, obj->first_affect, obj->last_affect, next, prev );

         CREATE( paf, AFFECT_DATA, 1 );
         paf->type = -1;
         paf->duration = -1;
         paf->location = APPLY_DAMROLL;
         paf->modifier = ch->level - 5;
         xCLEAR_BITS( paf->bitvector );
         LINK( paf, obj->first_affect, obj->last_affect, next, prev );
      }
   }
   else
   {
      obj->level = ch->level;
      obj->value[1] = ch->level * 200;
      obj->value[2] = ch->level * 400;

      CREATE( paf, AFFECT_DATA, 1 );
      paf->type = -1;
      paf->duration = -1;
      paf->location = APPLY_HITROLL;
      paf->modifier = ch->level * 20;
      xCLEAR_BITS( paf->bitvector );
      LINK( paf, obj->first_affect, obj->last_affect, next, prev );

      CREATE( paf, AFFECT_DATA, 1 );
      paf->type = -1;
      paf->duration = -1;
      paf->location = APPLY_DAMROLL;
      paf->modifier = ch->level * 40;
      xCLEAR_BITS( paf->bitvector );
      LINK( paf, obj->first_affect, obj->last_affect, next, prev );
   }

   if( CAN_WEAR( obj, ITEM_TAKE ) )
      obj_to_char( obj, ch );
   else
      obj_to_room( obj, ch->in_room );

   act( AT_ACTION, "Congratulations young learner, you have made a $p.", ch, obj, NULL, TO_CHAR );
   act( AT_ACTION, "$n makes a $p.", ch, obj, NULL, TO_ROOM );
   sprintf( log_buf, "%s: %s", ch->name, logline );
   log_string( log_buf );
   save_char_obj( ch );
   return;
}

void do_makepotion( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   char buf[MAX_STRING_LENGTH];
   char log_buf[MAX_STRING_LENGTH];
   char logline[MAX_STRING_LENGTH];
   OBJ_DATA *obj;
   OBJ_INDEX_DATA *oi;
   int sn = 0;
   int x = 0;
   int hloss = 0;

   hloss = 250;

   strcpy( arg, argument );
   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: makepotion <keywords>\n\r", ch );
      return;
   }

   if( !LEARNED( ch, gsn_magic_lore ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "A magical force interrupts you.\n\r", ch );
      return;
   }

   if( ch->hit <= hloss )
   {
      send_to_char( "You don't have enough hp for this.\n\r", ch );
      return;
   }

   if( ( oi = get_obj_index( OBJ_VNUM_POTION_MAKE ) ) == NULL || ( obj = create_object( oi, 1 ) ) == NULL )
   {
      send_to_char( "A procedural error interrupts you.\n\r", ch );
      return;
   }
   sprintf( logline, "makepotion %s", arg );

   x = number_range( 1, 3 );
   if( x == 1 )
   {
      sn = gsn_heal_ii;
   }
   else if( x == 2 )
   {
      sn = gsn_heal_iii;
   }
   else if( x == 3 )
   {
      sn = gsn_heal_iv;
   }
   if( IS_IMMORTAL( ch ) )
   {
      sn = gsn_heal_v;
   }

   separate_obj( obj );

   sprintf( buf, "%s potion", arg );
   STRFREE( obj->name );
   STRFREE( obj->short_descr );
   obj->name = STRALLOC( buf );
   obj->short_descr = STRALLOC( buf );

   obj->level = ch->level;
   obj->value[0] = ch->level;
   obj->value[1] = sn;
   if( ch->level >= 300 )
   {
      obj->value[2] = sn;
   }
   if( ch->level >= 400 )
   {
      obj->value[3] = sn;
   }

   if( CAN_WEAR( obj, ITEM_TAKE ) )
      obj_to_char( obj, ch );
   else
      obj_to_room( obj, ch->in_room );


   ch->hit -= hloss;
   WAIT_STATE( ch, skill_table[gsn_makepotion]->beats );
   act( AT_ACTION, "You have made a $p.", ch, obj, NULL, TO_CHAR );
   act( AT_ACTION, "$n makes a $p.", ch, obj, NULL, TO_ROOM );
   sprintf( log_buf, "%s: %s", ch->name, logline );
   log_string( log_buf );
   save_char_obj( ch );
   return;
}

void do_force_throw( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_STRING_LENGTH];
   char arg2[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   int percent;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Syntax: forcethrow <what> <target>\n\r", ch );
      return;
   }

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return;
   }

   if( !LEARNED( ch, gsn_force_lore ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "A magical force interrupts you.\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg2 ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "A magical force interrupts you.\n\r", ch );
      return;
   }

   if( ( !IS_NPC( ch ) && !IS_NPC( victim ) ) && ( !IS_PKILL( ch ) && !IS_PKILL( victim ) ) )
   {
      set_char_color( AT_IMMORT, ch );
      send_to_char( "The gods forbid that between non-player killers.\n\r", ch );
      return;
   }

   if( xIS_SET( victim->act, ACT_PACIFIST ) )
   {
      send_to_char( "They are a pacifist - Shame on you!\n\r", ch );
      return;
   }

   if( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
   {
      send_to_char( "You can't seem to find it.\n\r", ch );
      learn_from_failure( ch, gsn_force_throw );
      return;
   }

//    WAIT_STATE( ch, skill_table[gsn_force_throw]->beats );
   percent = number_percent(  ) + ( IS_AWAKE( victim ) ? 10 : -50 )
      - ( get_curr_lck( ch ) - 15 ) + ( get_curr_lck( victim ) - 13 );

   separate_obj( obj );
   if( !can_use_skill( ch, percent, gsn_force_throw ) )
   {
      obj_to_room( obj, ch->in_room );
      send_to_char( "You missed! Remember young learner, do or do not, there is no try.\n\r", ch );
      act( AT_ACTION, "$n force throws $p at you, but missed!\n\r", ch, obj, victim, TO_VICT );
      act( AT_ACTION, "$n force throws $p at $N and misses badly!\n\r", ch, obj, victim, TO_NOTVICT );
      learn_from_failure( ch, gsn_force_throw );
      global_retcode = damage( ch, victim, 0, gsn_force_throw );
      check_illegal_pk( ch, victim );
   }
   else
   {
      extract_obj( obj );
      act( AT_ACTION, "$n force throws $p at you!\n\r", ch, obj, victim, TO_VICT );
      act( AT_ACTION, "Congratulations young learner, you force throw $p at $N!", ch, obj, victim, TO_CHAR );
      act( AT_ACTION, "$n force throws $p at $N!\n\r", ch, obj, victim, TO_NOTVICT );
      learn_from_success( ch, gsn_force_throw );
      global_retcode = multi_hit( ch, victim, gsn_force_throw );
      check_illegal_pk( ch, victim );
   }
   return;
}

void do_steal( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   CHAR_DATA *victim, *mst;
   OBJ_DATA *obj;
   int percent;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( ch->mount )
   {
      send_to_char( "You can't do that while mounted.\n\r", ch );
      return;
   }

   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Steal what from whom?\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg2 ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "That's pointless.\n\r", ch );
      return;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "A magical force interrupts you.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !IS_NPC( victim ) )
   {
      set_char_color( AT_IMMORT, ch );
      send_to_char( "The gods forbid theft between players.\n\r", ch );
      return;
   }

   if( xIS_SET( victim->act, ACT_PACIFIST ) )
   {
      send_to_char( "They are a pacifist - Shame on you!\n\r", ch );
      return;
   }


//    WAIT_STATE( ch, skill_table[gsn_steal]->beats );
   percent = number_percent(  ) + ( IS_AWAKE( victim ) ? 10 : -50 )
      - ( get_curr_lck( ch ) - 15 ) + ( get_curr_lck( victim ) - 13 );

   if( ch->level + 10 < victim->level )
   {
      send_to_char( "You really don't want to try that!\n\r", ch );
      return;
   }

   if( victim->position == POS_FIGHTING || !can_use_skill( ch, percent, gsn_steal ) )
   {
      send_to_char( "Oops...\n\r", ch );
      act( AT_ACTION, "$n tried to steal from you!\n\r", ch, NULL, victim, TO_VICT );
      act( AT_ACTION, "$n tried to steal from $N.\n\r", ch, NULL, victim, TO_NOTVICT );

      sprintf( buf, "%s is a bloody thief!", ch->name );
      do_yell( victim, buf );

      learn_from_failure( ch, gsn_steal );
      if( !IS_NPC( ch ) )
      {
         if( legal_loot( ch, victim ) )
         {
            global_retcode = multi_hit( victim, ch, TYPE_UNDEFINED );
         }
         else
         {
            if( IS_NPC( ch ) )
            {
               if( ( mst = ch->master ) == NULL )
                  return;
            }
            else
               mst = ch;
            if( IS_NPC( mst ) )
               return;
            if( !xIS_SET( mst->act, PLR_THIEF ) )
            {
               xSET_BIT( mst->act, PLR_THIEF );
               set_char_color( AT_WHITE, ch );
               send_to_char( "A strange feeling grows deep inside you, and a tingle goes up your spine...\n\r", ch );
               set_char_color( AT_IMMORT, ch );
               send_to_char( "A deep voice booms inside your head, 'Thou shall now be known as a lowly thief!'\n\r", ch );
               set_char_color( AT_WHITE, ch );
               send_to_char( "You feel as if your soul has been revealed for all to see.\n\r", ch );
               save_char_obj( mst );
            }
         }
      }

      return;
   }

   if( !str_cmp( arg1, "coin" ) || !str_cmp( arg1, "coins" ) || !str_cmp( arg1, "katyr" ) )
   {
      int amount;

      amount = ( int )( victim->gold * number_range( 1, 10 ) / 100 );
      if( amount <= 0 )
      {
         send_to_char( "You couldn't get any katyr.\n\r", ch );
         learn_from_failure( ch, gsn_steal );
         return;
      }

      ch->gold += amount;
      victim->gold -= amount;
      ch_printf( ch, "Aha!  You got %d katyr.\n\r", amount );
      learn_from_success( ch, gsn_steal );
      return;
   }

   if( ( obj = get_obj_carry( victim, arg1 ) ) == NULL )
   {
      send_to_char( "You can't seem to find it.\n\r", ch );
      learn_from_failure( ch, gsn_steal );
      return;
   }

   if( !can_drop_obj( ch, obj )
       || IS_OBJ_STAT( obj, ITEM_INVENTORY ) || IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) || obj->level > ch->level )
   {
      send_to_char( "You can't manage to pry it away.\n\r", ch );
      learn_from_failure( ch, gsn_steal );
      return;
   }

   if( ch->carry_number + ( get_obj_number( obj ) / obj->count ) > can_carry_n( ch ) )
   {
      send_to_char( "You have your hands full.\n\r", ch );
      learn_from_failure( ch, gsn_steal );
      return;
   }

   if( ch->carry_weight + ( get_obj_weight( obj ) / obj->count ) > can_carry_w( ch ) )
   {
      send_to_char( "You can't carry that much weight.\n\r", ch );
      learn_from_failure( ch, gsn_steal );
      return;
   }

   separate_obj( obj );
   obj_from_char( obj );
   obj_to_char( obj, ch );
   send_to_char( "Ok.\n\r", ch );
   learn_from_success( ch, gsn_steal );
   return;
}

void do_backstab( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj = NULL;
   int percent;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't do that right now.\n\r", ch );
      return;
   }

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( ch->mount )
   {
      send_to_char( "You can't get close enough while mounted.\n\r", ch );
      return;
   }

   if( arg[0] == '\0' )
   {
      send_to_char( "Backstab whom?\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "How can you sneak up on yourself?\n\r", ch );
      return;
   }

   if( is_safe( ch, victim ) )
      return;

   if( !IS_NPC( ch ) && !IS_NPC( victim ) && xIS_SET( ch->act, PLR_NICE ) )
   {
      send_to_char( "You are too nice to do that.\n\r", ch );
      return;
   }

   if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You need to wield a piercing or stabbing weapon.\n\r", ch );
      return;
   }

   if( obj->value[4] != WEP_DAGGER )
   {
      if( ( obj->value[4] == WEP_SWORD && obj->value[3] != DAM_PIERCE ) || obj->value[4] != WEP_SWORD )
      {
         send_to_char( "You need to wield a piercing or stabbing weapon.\n\r", ch );
         return;
      }
   }

   if( victim->fighting )
   {
      send_to_char( "You can't backstab someone who is in combat.\n\r", ch );
      return;
   }

   if( victim->hit < victim->max_hit && IS_AWAKE( victim ) )
   {
      act( AT_PLAIN, "$N is hurt and suspicious ... you can't sneak up.", ch, NULL, victim, TO_CHAR );
      return;
   }

   percent = number_range( 1, 100 );

   check_attacker( ch, victim );
//    WAIT_STATE( ch, skill_table[gsn_backstab]->beats );
   if( !IS_IMMORTAL( ch ) && percent < 5 )
   {
      learn_from_failure( ch, gsn_backstab );
      global_retcode = damage( ch, victim, 0, gsn_backstab );
      check_illegal_pk( ch, victim );
      return;
   }
   learn_from_success( ch, gsn_backstab );
   global_retcode = multi_hit( ch, victim, gsn_backstab );
   check_illegal_pk( ch, victim );
   return;
}

void do_assassinate( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj = NULL;
   int percent;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't do that right now.\n\r", ch );
      return;
   }

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( ch->mount )
   {
      send_to_char( "You can't get close enough while mounted.\n\r", ch );
      return;
   }

   if( arg[0] == '\0' )
   {
      send_to_char( "Assassinate whom?\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "How can you sneak up on yourself?\n\r", ch );
      return;
   }

   if( is_safe( ch, victim ) )
      return;

   if( !IS_NPC( ch ) && !IS_NPC( victim ) && xIS_SET( ch->act, PLR_NICE ) )
   {
      send_to_char( "You are too nice to do that.\n\r", ch );
      return;
   }

   if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You need to wield a piercing or stabbing weapon.\n\r", ch );
      return;
   }

   if( obj->value[4] != WEP_DAGGER )
   {
      if( ( obj->value[4] == WEP_SWORD && obj->value[3] != DAM_PIERCE ) || obj->value[4] != WEP_SWORD )
      {
         send_to_char( "You need to wield a piercing or stabbing weapon.\n\r", ch );
         return;
      }
   }

   if( victim->fighting )
   {
      send_to_char( "You can't assassinate someone who is in combat.\n\r", ch );
      return;
   }

   if( victim->hit < victim->max_hit && IS_AWAKE( victim ) )
   {
      act( AT_PLAIN, "$N is hurt and suspicious ... you can't sneak up.", ch, NULL, victim, TO_CHAR );
      return;
   }

   percent = number_range( 1, 100 );

   check_attacker( ch, victim );
//    WAIT_STATE( ch, skill_table[gsn_backstab]->beats );
   if( !IS_IMMORTAL( ch ) && percent < 10 )
   {
      learn_from_failure( ch, gsn_assassinate );
      global_retcode = damage( ch, victim, 0, gsn_assassinate );
      check_illegal_pk( ch, victim );
      return;
   }
   learn_from_success( ch, gsn_assassinate );
   global_retcode = multi_hit( ch, victim, gsn_assassinate );
   check_illegal_pk( ch, victim );
   return;
}

void do_rescue( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   CHAR_DATA *fch;
   int percent;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( IS_AFFECTED( ch, AFF_BERSERK ) )
   {
      send_to_char( "You aren't thinking clearly...\n\r", ch );
      return;
   }

   one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      send_to_char( "Rescue whom?\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "How about fleeing instead?\n\r", ch );
      return;
   }

   if( ch->mount )
   {
      send_to_char( "You can't do that while mounted.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && IS_NPC( victim ) )
   {
      send_to_char( "They don't need your help!\n\r", ch );
      return;
   }

   if( !ch->fighting )
   {
      send_to_char( "Too late...\n\r", ch );
      return;
   }

   if( ( fch = who_fighting( victim ) ) == NULL )
   {
      send_to_char( "They are not fighting right now.\n\r", ch );
      return;
   }

   if( who_fighting( victim ) == ch )
   {
      send_to_char( "Just running away would be better...\n\r", ch );
      return;
   }

   if( IS_AFFECTED( victim, AFF_BERSERK ) )
   {
      send_to_char( "Stepping in front of a berserker would not be an intelligent decision.\n\r", ch );
      return;
   }

   percent = number_percent(  ) - ( get_curr_lck( ch ) - 14 ) - ( get_curr_lck( victim ) - 16 );

   if( !can_use_skill( ch, percent, gsn_rescue ) )
   {
      send_to_char( "You fail the rescue.\n\r", ch );
      act( AT_SKILL, "$n tries to rescue you!", ch, NULL, victim, TO_VICT );
      act( AT_SKILL, "$n tries to rescue $N!", ch, NULL, victim, TO_NOTVICT );
      WAIT_STATE( ch, skill_table[gsn_rescue]->beats );
      learn_from_failure( ch, gsn_rescue );
      return;
   }

   act( AT_SKILL, "You rescue $N!", ch, NULL, victim, TO_CHAR );
   act( AT_SKILL, "$n rescues you!", ch, NULL, victim, TO_VICT );
   act( AT_SKILL, "$n moves in front of $N!", ch, NULL, victim, TO_NOTVICT );

   learn_from_success( ch, gsn_rescue );
   stop_fighting( fch, FALSE );
   stop_fighting( victim, FALSE );
   if( ch->fighting )
      stop_fighting( ch, FALSE );

   set_fighting( ch, fch );
   set_fighting( fch, ch );
   return;
}



void do_kick( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_kick ) )
   {
      send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
      return;
   }

   if( ( victim = who_fighting( ch ) ) == NULL )
   {
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
   }

//    WAIT_STATE( ch, skill_table[gsn_kick]->beats );
   if( can_use_skill( ch, number_percent(  ), gsn_kick ) )
   {
      learn_from_success( ch, gsn_kick );
      global_retcode = damage( ch, victim, number_range( 50, 150 ), gsn_kick );
      if( !IS_NPC( ch ) )
      {
         if( LEARNED( ch, gsn_firekick ) && ch->class == CLASS_MONK )
         {
            learn_from_success( ch, gsn_firekick );
            global_retcode = damage( ch, victim, number_range( 150, 250 ), gsn_firekick );
         }
         if( LEARNED( ch, gsn_icekick ) && ch->class == CLASS_MONK )
         {
            learn_from_success( ch, gsn_icekick );
            global_retcode = damage( ch, victim, number_range( 250, 350 ), gsn_icekick );
         }
         if( LEARNED( ch, gsn_lightningkick ) && ch->class == CLASS_MONK )
         {
            learn_from_success( ch, gsn_lightningkick );
            global_retcode = damage( ch, victim, number_range( 350, 450 ), gsn_lightningkick );
         }
      }
   }
   else
   {
      learn_from_failure( ch, gsn_kick );
      global_retcode = damage( ch, victim, 0, gsn_kick );
   }
   return;
}

void do_tachi_kashi( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( ch->mount )
   {
      send_to_char( "You can't get close enough while mounted.\n\r", ch );
      return;
   }

   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: tachikashi <who>\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "How can you sneak up on yourself?\n\r", ch );
      return;
   }

   if( is_safe( ch, victim ) )
      return;

   if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You need to wield a katana.\n\r", ch );
      return;
   }

   if( obj->value[4] != WEP_KATANA )
   {
      send_to_char( "You need to wield a katana.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_tachi_kashi ) )
   {
      send_to_char( "You better leave the weapon skills to the Samurai.\n\r", ch );
      return;
   }

   if( can_use_skill( ch, number_percent(  ), gsn_tachi_kashi ) )
   {
      learn_from_success( ch, gsn_tachi_kashi );
      global_retcode = damage( ch, victim, number_range( 450, 600 ), gsn_tachi_kashi );
      global_retcode = damage( ch, victim, number_range( 450, 600 ), gsn_tachi_kashi );
      global_retcode = damage( ch, victim, number_range( 450, 600 ), gsn_tachi_kashi );
   }
   else
   {
      learn_from_failure( ch, gsn_tachi_kashi );
      global_retcode = damage( ch, victim, 0, gsn_tachi_kashi );
   }
   return;
}

void do_tachi_haboku( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   int x = 0;
   int y = 0;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( ch->mount )
   {
      send_to_char( "You can't get close enough while mounted.\n\r", ch );
      return;
   }

   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: tachihaboku <who>\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "How can you sneak up on yourself?\n\r", ch );
      return;
   }

   if( is_safe( ch, victim ) )
      return;

   if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You need to wield a katana.\n\r", ch );
      return;
   }

   if( obj->value[4] != WEP_KATANA )
   {
      send_to_char( "You need to wield a katana.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_tachi_haboku ) )
   {
      send_to_char( "You better leave the weapon skills to the Samurai.\n\r", ch );
      return;
   }

   if( can_use_skill( ch, number_percent(  ), gsn_tachi_haboku ) )
   {
      learn_from_success( ch, gsn_tachi_haboku );
      x = number_range( 150, 300 );
      y = ( x / 2 );
      ch->hit += y;
      if( ch->hit > ch->max_hit )
      {
         ch->hit = ch->max_hit;
      }
      sprintf( buf, "%d hp drained from $N.", y );
      act( AT_PLAIN, buf, ch, NULL, victim, TO_CHAR );
      act( AT_PLAIN, buf, ch, NULL, victim, TO_ROOM );
      global_retcode = damage( ch, victim, x, gsn_tachi_haboku );
   }
   else
   {
      learn_from_failure( ch, gsn_tachi_haboku );
      global_retcode = damage( ch, victim, 0, gsn_tachi_haboku );
   }
   return;
}

void do_tachi_jinpu( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( ch->mount )
   {
      send_to_char( "You can't get close enough while mounted.\n\r", ch );
      return;
   }

   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: tachijinpu <who>\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "How can you sneak up on yourself?\n\r", ch );
      return;
   }

   if( is_safe( ch, victim ) )
      return;

   if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You need to wield a katana.\n\r", ch );
      return;
   }

   if( obj->value[4] != WEP_KATANA )
   {
      send_to_char( "You need to wield a katana.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_tachi_jinpu ) )
   {
      send_to_char( "You better leave the weapon skills to the Samurai.\n\r", ch );
      return;
   }

   if( can_use_skill( ch, number_percent(  ), gsn_tachi_jinpu ) )
   {
      learn_from_success( ch, gsn_tachi_jinpu );
      global_retcode = damage( ch, victim, number_range( 150, 300 ), gsn_tachi_jinpu );
      global_retcode = damage( ch, victim, number_range( 150, 300 ), gsn_tachi_jinpu );
      global_retcode = damage( ch, victim, number_range( 150, 300 ), gsn_tachi_jinpu );
   }
   else
   {
      learn_from_failure( ch, gsn_tachi_jinpu );
      global_retcode = damage( ch, victim, 0, gsn_tachi_jinpu );
   }
   return;
}

void do_tachi_koki( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   int dam;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( ch->mount )
   {
      send_to_char( "You can't get close enough while mounted.\n\r", ch );
      return;
   }

   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: tachikoki <who>\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "How can you sneak up on yourself?\n\r", ch );
      return;
   }

   if( is_safe( ch, victim ) )
      return;

   if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You need to wield a katana.\n\r", ch );
      return;
   }

   if( obj->value[4] != WEP_KATANA )
   {
      send_to_char( "You need to wield a katana.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_tachi_koki ) )
   {
      send_to_char( "You better leave the weapon skills to the Samurai.\n\r", ch );
      return;
   }

   if( can_use_skill( ch, number_percent(  ), gsn_tachi_koki ) )
   {
      dam = dice( 10, ch->level ) * 1.7;
      learn_from_success( ch, gsn_tachi_koki );
      global_retcode = damage( ch, victim, number_range( 550, 700 ), gsn_tachi_koki );
      global_retcode = damage( ch, victim, dam, gsn_tachi_koki );
      global_retcode = damage( ch, victim, number_range( 550, 700 ), gsn_tachi_koki );
      global_retcode = damage( ch, victim, dam, gsn_tachi_koki );
   }
   else
   {
      learn_from_failure( ch, gsn_tachi_koki );
      global_retcode = damage( ch, victim, 0, gsn_tachi_koki );
   }
   return;
}

void do_tachi_gekko( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( ch->mount )
   {
      send_to_char( "You can't get close enough while mounted.\n\r", ch );
      return;
   }

   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: tachigekko <who>\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "How can you sneak up on yourself?\n\r", ch );
      return;
   }

   if( is_safe( ch, victim ) )
      return;

   if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You need to wield a katana.\n\r", ch );
      return;
   }

   if( obj->value[4] != WEP_KATANA )
   {
      send_to_char( "You need to wield a katana.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_tachi_gekko ) )
   {
      send_to_char( "You better leave the weapon skills to the Samurai.\n\r", ch );
      return;
   }

   if( can_use_skill( ch, number_percent(  ), gsn_tachi_gekko ) )
   {
      learn_from_success( ch, gsn_tachi_gekko );
      global_retcode = damage( ch, victim, number_range( 250, 350 ), gsn_tachi_gekko );
      global_retcode = damage( ch, victim, number_range( 250, 350 ), gsn_tachi_gekko );
   }
   else
   {
      learn_from_failure( ch, gsn_tachi_gekko );
      global_retcode = damage( ch, victim, 0, gsn_tachi_gekko );
   }
   return;
}

void do_tachi_enpi( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( ch->mount )
   {
      send_to_char( "You can't get close enough while mounted.\n\r", ch );
      return;
   }

   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: tachienpi <who>\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "How can you sneak up on yourself?\n\r", ch );
      return;
   }

   if( is_safe( ch, victim ) )
      return;

   if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You need to wield a katana.\n\r", ch );
      return;
   }

   if( obj->value[4] != WEP_KATANA )
   {
      send_to_char( "You need to wield a katana.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_tachi_enpi ) )
   {
      send_to_char( "You better leave the weapon skills to the Samurai.\n\r", ch );
      return;
   }

   if( can_use_skill( ch, number_percent(  ), gsn_tachi_enpi ) )
   {
      learn_from_success( ch, gsn_tachi_enpi );
      global_retcode = damage( ch, victim, number_range( 150, 300 ), gsn_tachi_enpi );
   }
   else
   {
      learn_from_failure( ch, gsn_tachi_enpi );
      global_retcode = damage( ch, victim, 0, gsn_tachi_enpi );
   }
   return;
}

void do_tachi_kagero( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   int damx;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return;
   }
   one_argument( argument, arg );

   if( ch->mount )
   {
      send_to_char( "You can't get close enough while mounted.\n\r", ch );
      return;
   }

   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: tachikagero <who>\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "How can you sneak up on yourself?\n\r", ch );
      return;
   }

   if( is_safe( ch, victim ) )
      return;

   if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You need to wield a katana.\n\r", ch );
      return;
   }

   if( obj->value[4] != WEP_KATANA )
   {
      send_to_char( "You need to wield a katana.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_tachi_kagero ) )
   {
      send_to_char( "You better leave the weapon skills to the Samurai.\n\r", ch );
      return;
   }

   if( can_use_skill( ch, number_percent(  ), gsn_tachi_kagero ) )
   {
      learn_from_success( ch, gsn_tachi_kagero );
      damx = dice( 10, ch->level ) * 2.1;
      global_retcode = damage( ch, victim, damx, gsn_tachi_kagero );
      global_retcode = damage( ch, victim, damx, gsn_tachi_kagero );
      global_retcode = damage( ch, victim, damx, gsn_tachi_kagero );
   }
   else
   {
      learn_from_failure( ch, gsn_tachi_kagero );
      global_retcode = damage( ch, victim, 0, gsn_tachi_kagero );
   }
   return;
}


void do_punch( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_punch ) )
   {
      send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
      return;
   }

   if( ( victim = who_fighting( ch ) ) == NULL )
   {
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
   }

//    WAIT_STATE( ch, skill_table[gsn_punch]->beats );
   if( can_use_skill( ch, number_percent(  ), gsn_punch ) )
   {
      learn_from_success( ch, gsn_punch );
      global_retcode = damage( ch, victim, number_range( 1, ch->level ), gsn_punch );
   }
   else
   {
      learn_from_failure( ch, gsn_punch );
      global_retcode = damage( ch, victim, 0, gsn_punch );
   }
   return;
}


void do_bite( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_bite ) )
   {
      send_to_char( "That isn't quite one of your natural skills.\n\r", ch );
      return;
   }

   if( ( victim = who_fighting( ch ) ) == NULL )
   {
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
   }

//    WAIT_STATE( ch, skill_table[gsn_bite]->beats );
   if( can_use_skill( ch, number_percent(  ), gsn_bite ) )
   {
      learn_from_success( ch, gsn_bite );
      global_retcode = damage( ch, victim, number_range( 1, ch->level ), gsn_bite );
   }
   else
   {
      learn_from_failure( ch, gsn_bite );
      global_retcode = damage( ch, victim, 0, gsn_bite );
   }
   return;
}


void do_claw( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_claw ) )
   {
      send_to_char( "That isn't quite one of your natural skills.\n\r", ch );
      return;
   }

   if( ( victim = who_fighting( ch ) ) == NULL )
   {
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
   }

//    WAIT_STATE( ch, skill_table[gsn_claw]->beats );
   if( can_use_skill( ch, number_percent(  ), gsn_claw ) )
   {
      learn_from_success( ch, gsn_claw );
      global_retcode = damage( ch, victim, number_range( 1, ch->level ), gsn_claw );
   }
   else
   {
      learn_from_failure( ch, gsn_claw );
      global_retcode = damage( ch, victim, 0, gsn_claw );
   }
   return;
}


void do_sting( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_sting ) )
   {
      send_to_char( "That isn't quite one of your natural skills.\n\r", ch );
      return;
   }

   if( ( victim = who_fighting( ch ) ) == NULL )
   {
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
   }

//    WAIT_STATE( ch, skill_table[gsn_sting]->beats );
   if( can_use_skill( ch, number_percent(  ), gsn_sting ) )
   {
      learn_from_success( ch, gsn_sting );
      global_retcode = damage( ch, victim, number_range( 1, ch->level ), gsn_sting );
   }
   else
   {
      learn_from_failure( ch, gsn_sting );
      global_retcode = damage( ch, victim, 0, gsn_sting );
   }
   return;
}


void do_tail( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_tail ) )
   {
      send_to_char( "That isn't quite one of your natural skills.\n\r", ch );
      return;
   }

   if( ( victim = who_fighting( ch ) ) == NULL )
   {
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
   }

//    WAIT_STATE( ch, skill_table[gsn_tail]->beats );
   if( can_use_skill( ch, number_percent(  ), gsn_tail ) )
   {
      learn_from_success( ch, gsn_tail );
      global_retcode = damage( ch, victim, number_range( 1, ch->level ), gsn_tail );
   }
   else
   {
      learn_from_failure( ch, gsn_tail );
      global_retcode = damage( ch, victim, 0, gsn_tail );
   }
   return;
}

void do_force_push( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( !LEARNED( ch, gsn_force_lore ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "A magical force interrupts you.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_force_push ) )
   {
      send_to_char( "That isn't quite one of your natural skills.\n\r", ch );
      return;
   }

   if( ( victim = who_fighting( ch ) ) == NULL )
   {
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
   }

//    WAIT_STATE( ch, skill_table[gsn_force_push]->beats );
   if( can_use_skill( ch, number_percent(  ), gsn_force_push ) )
   {
      learn_from_success( ch, gsn_force_push );
      global_retcode = damage( ch, victim, number_range( 1, ch->level ), gsn_force_push );
   }
   else
   {
      learn_from_failure( ch, gsn_force_push );
      global_retcode = damage( ch, victim, 0, gsn_force_push );
   }
   return;
}

void do_ferralclaw( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   int chance;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_fclaw ) )
   {
      send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      if( ( victim = who_fighting( ch ) ) == NULL )
      {
         send_to_char( "You aren't fighting anyone.\n\r", ch );
         return;
      }
   }
   else
   {
      if( ( victim = get_char_room( ch, arg ) ) == NULL )
      {
         send_to_char( "They aren't here.\n\r", ch );
         return;
      }
   }

   chance = ( ( ( get_curr_dex( victim ) + get_curr_str( victim ) )
                - ( get_curr_dex( ch ) + get_curr_str( ch ) ) ) * 10 ) + 10;

   if( !IS_NPC( ch ) && !IS_NPC( victim ) )
      chance += sysdata.bash_plr_vs_plr;
   if( victim->fighting && victim->fighting->who != ch )
      chance += sysdata.bash_nontank;
   if( can_use_skill( ch, ( number_percent(  ) + chance ), gsn_fclaw ) )
   {
      learn_from_success( ch, gsn_fclaw );
      WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
      victim->position = POS_SITTING;
      global_retcode = damage( ch, victim, number_range( ch->level, 150 ), gsn_fclaw );
   }
   else
   {
      WAIT_STATE( ch, skill_table[gsn_fclaw]->beats );
      learn_from_failure( ch, gsn_fclaw );
      global_retcode = damage( ch, victim, 0, gsn_fclaw );
   }
   return;
}

void do_bash( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   int chance;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_bash ) )
   {
      send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
      return;
   }

   if( ( victim = who_fighting( ch ) ) == NULL )
   {
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
   }

   chance = ( ( ( get_curr_dex( victim ) + get_curr_str( victim ) )
                - ( get_curr_dex( ch ) + get_curr_str( ch ) ) ) * 10 ) + 10;
   if( !IS_NPC( ch ) && !IS_NPC( victim ) )
      chance += sysdata.bash_plr_vs_plr;
   if( victim->fighting && victim->fighting->who != ch )
      chance += sysdata.bash_nontank;
   if( can_use_skill( ch, ( number_percent(  ) + chance ), gsn_bash ) )
   {
      learn_from_success( ch, gsn_bash );
      WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
      victim->position = POS_SITTING;
      global_retcode = damage( ch, victim, number_range( 1, ch->level ), gsn_bash );
   }
   else
   {
      WAIT_STATE( ch, skill_table[gsn_bash]->beats );
      learn_from_failure( ch, gsn_bash );
      global_retcode = damage( ch, victim, 0, gsn_bash );
   }
   return;
}


void do_stun( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   AFFECT_DATA af;
   int chance;
   bool fail;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_stun ) )
   {
      send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
      return;
   }

   if( ( victim = who_fighting( ch ) ) == NULL )
   {
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && ch->move < ch->max_move / 10 )
   {
      set_char_color( AT_SKILL, ch );
      send_to_char( "You are far too tired to do that.\n\r", ch );
      return;
   }

//    WAIT_STATE( ch, skill_table[gsn_stun]->beats );
   fail = FALSE;
   chance = ris_save( victim, ch->level, RIS_PARALYSIS );
   if( chance == 1000 )
      fail = TRUE;
   else
      fail = saves_para_petri( chance, victim );

   chance = ( ( ( get_curr_dex( victim ) + get_curr_str( victim ) )
                - ( get_curr_dex( ch ) + get_curr_str( ch ) ) ) * 10 ) + 10;
   if( !IS_NPC( ch ) && !IS_NPC( victim ) )
      chance += sysdata.stun_plr_vs_plr;
   else
      chance += sysdata.stun_regular;
   if( !fail && can_use_skill( ch, ( number_percent(  ) + chance ), gsn_stun ) )
   {
      learn_from_success( ch, gsn_stun );
      if( !IS_NPC( ch ) )
         ch->move -= ch->max_move / 10;
// WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
      WAIT_STATE( victim, PULSE_VIOLENCE );
      act( AT_SKILL, "$N smashes into you, leaving you stunned!", victim, NULL, ch, TO_CHAR );
      act( AT_SKILL, "You smash into $N, leaving $M stunned!", ch, NULL, victim, TO_CHAR );
      act( AT_SKILL, "$n smashes into $N, leaving $M stunned!", ch, NULL, victim, TO_NOTVICT );
      if( !IS_AFFECTED( victim, AFF_PARALYSIS ) )
      {
         af.type = gsn_stun;
         af.location = APPLY_AC;
         af.modifier = 20;
         af.duration = 1;
         af.bitvector = meb( AFF_PARALYSIS );
         affect_to_char( victim, &af );
         update_pos( victim );
      }
   }
   else
   {
      WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
      if( !IS_NPC( ch ) )
         ch->move -= ch->max_move / 15;
      learn_from_failure( ch, gsn_stun );
      act( AT_SKILL, "$n charges at you screaming, but you dodge out of the way.", ch, NULL, victim, TO_VICT );
      act( AT_SKILL, "You try to stun $N, but $E dodges out of the way.", ch, NULL, victim, TO_CHAR );
      act( AT_SKILL, "$n charges screaming at $N, but keeps going right on past.", ch, NULL, victim, TO_NOTVICT );
   }
   return;
}

void do_bloodlet( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *obj;

   if( IS_NPC( ch ) || !IS_VAMPIRE( ch ) )
      return;

   if( ch->fighting )
   {
      send_to_char( "You're too busy fighting...\n\r", ch );
      return;
   }
   if( ch->blood < 10 )
   {
      send_to_char( "You are too drained to offer any blood...\n\r", ch );
      return;
   }

//    WAIT_STATE( ch, PULSE_VIOLENCE );
   if( can_use_skill( ch, number_percent(  ), gsn_bloodlet ) )
   {
      ch->blood -= 7;
      act( AT_BLOOD, "Tracing a sharp nail over your skin, you let your blood spill.", ch, NULL, NULL, TO_CHAR );
      act( AT_BLOOD, "$n traces a sharp nail over $s skin, spilling a quantity of blood to the ground.", ch, NULL, NULL,
           TO_ROOM );
      learn_from_success( ch, gsn_bloodlet );
      obj = create_object( get_obj_index( OBJ_VNUM_BLOODLET ), 0 );
      obj->timer = 1;
      obj->value[1] = 6;
      obj_to_room( obj, ch->in_room );
   }
   else
   {
      act( AT_BLOOD, "You cannot manage to draw much blood...", ch, NULL, NULL, TO_CHAR );
      act( AT_BLOOD, "$n slices open $s skin, but no blood is spilled...", ch, NULL, NULL, TO_ROOM );
      learn_from_failure( ch, gsn_bloodlet );
   }
   return;
}

void do_feed( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   sh_int dam;
   int bpx;


   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !IS_VAMPIRE( ch ) )
   {
      send_to_char( "It is not of your nature to feed on living creatures.\n\r", ch );
      return;
   }
   if( !can_use_skill( ch, 0, gsn_feed ) )
   {
      send_to_char( "You have not yet practiced your new teeth.\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      if( ( victim = who_fighting( ch ) ) == NULL )
      {
         send_to_char( "You aren't fighting anyone.\n\r", ch );
         return;
      }
   }
   else
   {
      if( ( victim = get_char_room( ch, arg ) ) == NULL )
      {
         send_to_char( "They aren't here.\n\r", ch );
         return;
      }
   }

   if( ch->mount )
   {
      send_to_char( "You can't do that while mounted.\n\r", ch );
      return;
   }

// WAIT_STATE( ch, skill_table[gsn_feed]->beats );
   if( can_use_skill( ch, number_percent(  ), gsn_feed ) )
   {
      dam = number_range( 1, ch->level );
      global_retcode = damage( ch, victim, dam, gsn_feed );
      if( global_retcode == rNONE && !IS_NPC( ch ) && dam && ch->fighting && ch->blood < ch->max_blood )
      {
         bpx = ch->max_blood * 1.25;
         ch->blood += victim->max_hit / 3;
         if( ch->blood > bpx )
         {
            ch->blood = bpx;
         }
         if( ch->pcdata->condition[COND_FULL] <= 37 )
            gain_condition( ch, COND_FULL, 2 );
         gain_condition( ch, COND_THIRST, 2 );
         act( AT_BLOOD, "You manage to suck a little life out of $N.", ch, NULL, victim, TO_CHAR );
         act( AT_BLOOD, "$n sucks some of your blood!", ch, NULL, victim, TO_VICT );
         learn_from_success( ch, gsn_feed );
      }
   }
   else
   {
      global_retcode = damage( ch, victim, 0, gsn_feed );
      if( global_retcode == rNONE && !IS_NPC( ch ) && ch->fighting && ch->blood < ch->max_blood )
      {
         act( AT_BLOOD, "The smell of $N's blood is driving you insane!", ch, NULL, victim, TO_CHAR );
         act( AT_BLOOD, "$n is lusting after your blood!", ch, NULL, victim, TO_VICT );
         learn_from_failure( ch, gsn_feed );
      }
   }
   return;
}

void disarm( CHAR_DATA * ch, CHAR_DATA * victim )
{
   OBJ_DATA *obj, *tmpobj;

   if( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
      return;

   if( ( tmpobj = get_eq_char( victim, WEAR_DUAL_WIELD ) ) != NULL && number_bits( 1 ) == 0 )
      obj = tmpobj;

   if( get_eq_char( ch, WEAR_WIELD ) == NULL && number_bits( 1 ) == 0 )
   {
      learn_from_failure( ch, gsn_disarm );
      return;
   }

   if( IS_NPC( ch ) && !can_see_obj( ch, obj ) && number_bits( 1 ) == 0 )
   {
      learn_from_failure( ch, gsn_disarm );
      return;
   }

   if( check_grip( ch, victim ) )
   {
      learn_from_failure( ch, gsn_disarm );
      return;
   }

   act( AT_SKILL, "$n DISARMS you!", ch, NULL, victim, TO_VICT );
   act( AT_SKILL, "You disarm $N!", ch, NULL, victim, TO_CHAR );
   act( AT_SKILL, "$n disarms $N!", ch, NULL, victim, TO_NOTVICT );
   learn_from_success( ch, gsn_disarm );

   if( obj == get_eq_char( victim, WEAR_WIELD ) && ( tmpobj = get_eq_char( victim, WEAR_DUAL_WIELD ) ) != NULL )
      tmpobj->wear_loc = WEAR_WIELD;

   obj_from_char( obj );
   if( !IS_NPC( victim ) && CAN_PKILL( victim ) && !IS_OBJ_STAT( obj, ITEM_LOYAL ) )
   {
      SET_BIT( obj->magic_flags, ITEM_PKDISARMED );
      obj->value[5] = victim->level;
   }
   if( IS_NPC( victim ) || ( IS_OBJ_STAT( obj, ITEM_LOYAL ) && IS_PKILL( victim ) && !IS_NPC( ch ) ) )
      obj_to_char( obj, victim );
   else
      obj_to_room( obj, victim->in_room );

   return;
}


void do_disarm( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   int percent;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_disarm ) )
   {
      send_to_char( "You don't know how to disarm opponents.\n\r", ch );
      return;
   }

   if( get_eq_char( ch, WEAR_WIELD ) == NULL )
   {
      send_to_char( "You must wield a weapon to disarm.\n\r", ch );
      return;
   }

   if( ( victim = who_fighting( ch ) ) == NULL )
   {
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
   }

   if( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "Your opponent is not wielding a weapon.\n\r", ch );
      return;
   }

//    WAIT_STATE( ch, skill_table[gsn_disarm]->beats );
   percent = number_percent(  ) + victim->level - ch->level - ( get_curr_lck( ch ) - 15 ) + ( get_curr_lck( victim ) - 15 );
   if( !can_see_obj( ch, obj ) )
      percent += 10;
   if( can_use_skill( ch, ( percent * 3 / 2 ), gsn_disarm ) )
      disarm( ch, victim );
   else
   {
      send_to_char( "You failed.\n\r", ch );
      learn_from_failure( ch, gsn_disarm );
   }
   return;
}

void trip( CHAR_DATA * ch, CHAR_DATA * victim )
{
   if( IS_AFFECTED( victim, AFF_FLYING ) || IS_AFFECTED( victim, AFF_FLOATING ) )
      return;
   if( victim->mount )
   {
      if( IS_AFFECTED( victim->mount, AFF_FLYING ) || IS_AFFECTED( victim->mount, AFF_FLOATING ) )
         return;
      act( AT_SKILL, "$n trips your mount and you fall off!", ch, NULL, victim, TO_VICT );
      act( AT_SKILL, "You trip $N's mount and $N falls off!", ch, NULL, victim, TO_CHAR );
      act( AT_SKILL, "$n trips $N's mount and $N falls off!", ch, NULL, victim, TO_NOTVICT );
      xREMOVE_BIT( victim->mount->act, ACT_MOUNTED );
      victim->mount = NULL;
      WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
      WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
      victim->position = POS_RESTING;
      return;
   }
   if( victim->wait == 0 )
   {
      act( AT_SKILL, "$n trips you and you go down!", ch, NULL, victim, TO_VICT );
      act( AT_SKILL, "You trip $N and $N goes down!", ch, NULL, victim, TO_CHAR );
      act( AT_SKILL, "$n trips $N and $N goes down!", ch, NULL, victim, TO_NOTVICT );

      WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
      WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
      victim->position = POS_RESTING;
   }

   return;
}

void do_broach( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   EXIT_DATA *pexit;

   set_char_color( AT_DGREEN, ch );

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }
   one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      send_to_char( "Attempt this in which direction?\n\r", ch );
      return;
   }
   if( ch->mount )
   {
      send_to_char( "You should really dismount first.\n\r", ch );
      return;
   }
//    WAIT_STATE( ch, skill_table[gsn_broach]->beats );
   if( ( pexit = find_door( ch, arg, TRUE ) ) != NULL )
   {
      EXIT_DATA *pexit_rev;
      if( !IS_SET( pexit->exit_info, EX_CLOSED )
          || !IS_SET( pexit->exit_info, EX_LOCKED )
          || IS_SET( pexit->exit_info, EX_PICKPROOF ) || can_use_skill( ch, number_percent(  ), gsn_broach ) )
      {
         send_to_char( "Your attempt fails.\n\r", ch );
         learn_from_failure( ch, gsn_broach );
         check_room_for_traps( ch, TRAP_PICK | trap_door[pexit->vdir] );
         return;
      }
      REMOVE_BIT( pexit->exit_info, EX_LOCKED );
      send_to_char( "You successfully broach the exit...\n\r", ch );
      learn_from_success( ch, gsn_broach );
      if( ( pexit_rev = pexit->rexit ) != NULL && pexit_rev->to_room == ch->in_room )
      {
         REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
      }
      check_room_for_traps( ch, TRAP_PICK | trap_door[pexit->vdir] );
      return;
   }
   send_to_char( "Your attempt fails.\n\r", ch );
   return;
}

void do_pick( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *gch;
   OBJ_DATA *obj;
   EXIT_DATA *pexit;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "Pick what?\n\r", ch );
      return;
   }

   if( ch->mount )
   {
      send_to_char( "You can't do that while mounted.\n\r", ch );
      return;
   }

//    WAIT_STATE( ch, skill_table[gsn_pick_lock]->beats );

   for( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
   {
      if( IS_NPC( gch ) && IS_AWAKE( gch ) && ch->level + 5 < gch->level )
      {
         act( AT_PLAIN, "$N is standing too close to the lock.", ch, NULL, gch, TO_CHAR );
         return;
      }
   }

   if( !can_use_skill( ch, number_percent(  ), gsn_pick_lock ) )
   {
      send_to_char( "You failed.\n\r", ch );
      learn_from_failure( ch, gsn_pick_lock );
      return;
   }

   if( ( pexit = find_door( ch, arg, TRUE ) ) != NULL )
   {
      EXIT_DATA *pexit_rev;

      if( !IS_SET( pexit->exit_info, EX_CLOSED ) )
      {
         send_to_char( "It's not closed.\n\r", ch );
         return;
      }
      if( pexit->key < 0 )
      {
         send_to_char( "It can't be picked.\n\r", ch );
         return;
      }
      if( !IS_SET( pexit->exit_info, EX_LOCKED ) )
      {
         send_to_char( "It's already unlocked.\n\r", ch );
         return;
      }
      if( IS_SET( pexit->exit_info, EX_PICKPROOF ) )
      {
         send_to_char( "You failed.\n\r", ch );
         learn_from_failure( ch, gsn_pick_lock );
         check_room_for_traps( ch, TRAP_PICK | trap_door[pexit->vdir] );
         return;
      }

      REMOVE_BIT( pexit->exit_info, EX_LOCKED );
      send_to_char( "*Click*\n\r", ch );
      act( AT_ACTION, "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );
      learn_from_success( ch, gsn_pick_lock );
      if( ( pexit_rev = pexit->rexit ) != NULL && pexit_rev->to_room == ch->in_room )
      {
         REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
      }
      check_room_for_traps( ch, TRAP_PICK | trap_door[pexit->vdir] );
      return;
   }

   if( ( obj = get_obj_here( ch, arg ) ) != NULL )
   {
      if( obj->item_type != ITEM_CONTAINER )
      {
         send_to_char( "That's not a container.\n\r", ch );
         return;
      }
      if( !IS_SET( obj->value[1], CONT_CLOSED ) )
      {
         send_to_char( "It's not closed.\n\r", ch );
         return;
      }
      if( obj->value[2] < 0 )
      {
         send_to_char( "It can't be unlocked.\n\r", ch );
         return;
      }
      if( !IS_SET( obj->value[1], CONT_LOCKED ) )
      {
         send_to_char( "It's already unlocked.\n\r", ch );
         return;
      }
      if( IS_SET( obj->value[1], CONT_PICKPROOF ) )
      {
         send_to_char( "You failed.\n\r", ch );
         learn_from_failure( ch, gsn_pick_lock );
         check_for_trap( ch, obj, TRAP_PICK );
         return;
      }

      separate_obj( obj );
      REMOVE_BIT( obj->value[1], CONT_LOCKED );
      send_to_char( "*Click*\n\r", ch );
      act( AT_ACTION, "$n picks $p.", ch, obj, NULL, TO_ROOM );
      learn_from_success( ch, gsn_pick_lock );
      check_for_trap( ch, obj, TRAP_PICK );
      return;
   }

   ch_printf( ch, "You see no %s here.\n\r", arg );
   return;
}

void do_sneak( CHAR_DATA * ch, char *argument )
{
   AFFECT_DATA af;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( ch->mount )
   {
      send_to_char( "You can't do that while mounted.\n\r", ch );
      return;
   }

   send_to_char( "You attempt to move silently.\n\r", ch );
   affect_strip( ch, gsn_sneak );

   if( can_use_skill( ch, number_percent(  ), gsn_sneak ) )
   {
      af.type = gsn_sneak;
      af.duration = ch->level * DUR_CONV;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.bitvector = meb( AFF_SNEAK );
      affect_to_char( ch, &af );
      learn_from_success( ch, gsn_sneak );
   }
   else
      learn_from_failure( ch, gsn_sneak );

   return;
}

void do_move_hidden( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( ch->mount )
   {
      send_to_char( "You can't do that while mounted.\n\r", ch );
      return;
   }

   send_to_char( "You attempt to mask your movements.\n\r", ch );

   if( IS_AFFECTED( ch, AFF_MOVEHIDE ) )
      xREMOVE_BIT( ch->affected_by, AFF_MOVEHIDE );

   if( can_use_skill( ch, number_percent(  ), gsn_movehide ) )
   {
      xSET_BIT( ch->affected_by, AFF_MOVEHIDE );
      learn_from_success( ch, gsn_movehide );
   }
   else
      learn_from_failure( ch, gsn_movehide );
   return;
}

void do_hide( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( ch->mount )
   {
      send_to_char( "You can't do that while mounted.\n\r", ch );
      return;
   }

   send_to_char( "You attempt to hide.\n\r", ch );

   if( IS_AFFECTED( ch, AFF_HIDE ) )
      xREMOVE_BIT( ch->affected_by, AFF_HIDE );

   if( can_use_skill( ch, number_percent(  ), gsn_hide ) )
   {
      xSET_BIT( ch->affected_by, AFF_HIDE );
      learn_from_success( ch, gsn_hide );
   }
   else
      learn_from_failure( ch, gsn_hide );
   return;
}

void do_alertness( CHAR_DATA * ch, char *argument )
{
   AFFECT_DATA af;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   if( ch->mount )
   {
      send_to_char( "You can't do that while mounted.\n\r", ch );
      return;
   }

   if( can_use_skill( ch, number_percent(  ), gsn_alertness ) )
   {
      af.type = gsn_alertness;
      af.duration = ch->level * 2;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.bitvector = meb( AFF_ALERTNESS );
      affect_to_char( ch, &af );
      learn_from_success( ch, gsn_alertness );
      send_to_char( "You become more alert.\n\r", ch );
   }
   else
      learn_from_failure( ch, gsn_alertness );
   return;
}

void do_visible( CHAR_DATA * ch, char *argument )
{
   affect_strip( ch, gsn_invis );
   affect_strip( ch, gsn_impinvis );
   affect_strip( ch, gsn_mass_invis );
   affect_strip( ch, gsn_sneak );
   xREMOVE_BIT( ch->affected_by, AFF_HIDE );
   xREMOVE_BIT( ch->affected_by, AFF_MOVEHIDE );
   xREMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
   xREMOVE_BIT( ch->affected_by, AFF_IMPINVISIBLE );
   xREMOVE_BIT( ch->affected_by, AFF_SNEAK );
   send_to_char( "Ok.\n\r", ch );
   return;
}


void do_aid( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int percent;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      send_to_char( "Aid whom?\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( IS_NPC( victim ) )
   {
      send_to_char( "Not on mobs.\n\r", ch );
      return;
   }

   if( ch->mount )
   {
      send_to_char( "You can't do that while mounted.\n\r", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "Aid yourself?\n\r", ch );
      return;
   }

   if( victim->position > POS_STUNNED )
   {
      act( AT_PLAIN, "$N doesn't need your help.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if( victim->hit <= -6 )
   {
      act( AT_PLAIN, "$N's condition is beyond your aiding ability.", ch, NULL, victim, TO_CHAR );
      return;
   }

   percent = number_percent(  ) - ( get_curr_lck( ch ) - 13 );
//    WAIT_STATE( ch, skill_table[gsn_aid]->beats );
   if( !can_use_skill( ch, percent, gsn_aid ) )
   {
      send_to_char( "You fail.\n\r", ch );
      learn_from_failure( ch, gsn_aid );
      return;
   }

   act( AT_SKILL, "You aid $N!", ch, NULL, victim, TO_CHAR );
   act( AT_SKILL, "$n aids $N!", ch, NULL, victim, TO_NOTVICT );
   learn_from_success( ch, gsn_aid );
   if( victim->hit < 1 )
      victim->hit = 1;

   update_pos( victim );
   act( AT_SKILL, "$n aids you!", ch, NULL, victim, TO_VICT );
   return;
}


void do_mount( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   ROOM_INDEX_DATA *to_room;

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_mount ) )
   {
      send_to_char( "I don't think that would be a good idea...\n\r", ch );
      return;
   }

   if( ch->mount )
   {
      send_to_char( "You're already mounted!\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, argument ) ) == NULL )
   {
      send_to_char( "You can't find that here.\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) || !xIS_SET( victim->act, ACT_MOUNTABLE ) )
   {
      send_to_char( "You can't mount that!\n\r", ch );
      return;
   }

   if( xIS_SET( victim->act, ACT_MOUNTED ) )
   {
      send_to_char( "That mount already has a rider.\n\r", ch );
      return;
   }

   if( victim->position < POS_STANDING )
   {
      send_to_char( "Your mount must be standing.\n\r", ch );
      return;
   }

   if( victim->position == POS_FIGHTING || victim->fighting )
   {
      send_to_char( "Your mount is moving around too much.\n\r", ch );
      return;
   }

//    WAIT_STATE( ch, skill_table[gsn_mount]->beats );
   if( can_use_skill( ch, number_percent(  ), gsn_mount ) )
   {
      xSET_BIT( victim->act, ACT_MOUNTED );
      ch->mount = victim;
      act( AT_SKILL, "You mount $N.", ch, NULL, victim, TO_CHAR );
      act( AT_SKILL, "$n skillfully mounts $N.", ch, NULL, victim, TO_NOTVICT );
      act( AT_SKILL, "$n mounts you.", ch, NULL, victim, TO_VICT );
      learn_from_success( ch, gsn_mount );
      ch->position = POS_MOUNTED;
      if( xIS_SET( victim->act, ACT_DRAGOON ) )
      {
         to_room = get_room_index( ROOM_VNUM_DRAGOON );
         act( AT_SKILL, "You are lifted into the sky on $N.", ch, NULL, victim, TO_CHAR );
         act( AT_SKILL, "$n is swept clear into the horizon by $N.", ch, NULL, victim, TO_NOTVICT );
         char_from_room( ch );
         char_from_room( victim );
         char_to_room( victim, to_room );
         char_to_room( ch, to_room );
         act( AT_SKILL, "You dismount $N.", ch, NULL, victim, TO_CHAR );
         act( AT_SKILL, "$n skillfully dismounts $N.", ch, NULL, victim, TO_NOTVICT );
         act( AT_SKILL, "$n dismounts you.  Whew!", ch, NULL, victim, TO_VICT );
         xREMOVE_BIT( victim->act, ACT_MOUNTED );
         ch->mount = NULL;
         ch->position = POS_STANDING;
         do_look( ch, "auto" );
         act( AT_YELLOW, "All of a sudden, $N rears back and flies off into the horizon.", ch, NULL, victim, TO_ROOM );
         act( AT_YELLOW, "All of a sudden, $N rears back and flies off into the horizon.", ch, NULL, victim, TO_CHAR );
         extract_char( victim, TRUE );
      }
   }
   else
   {
      act( AT_SKILL, "You unsuccessfully try to mount $N.", ch, NULL, victim, TO_CHAR );
      act( AT_SKILL, "$n unsuccessfully attempts to mount $N.", ch, NULL, victim, TO_NOTVICT );
      act( AT_SKILL, "$n tries to mount you.", ch, NULL, victim, TO_VICT );
      learn_from_failure( ch, gsn_mount );
   }
   return;
}


void do_dismount( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;

   if( ( victim = ch->mount ) == NULL )
   {
      send_to_char( "You're not mounted.\n\r", ch );
      return;
   }

//    WAIT_STATE( ch, skill_table[gsn_mount]->beats );
   if( can_use_skill( ch, number_percent(  ), gsn_mount ) )
   {
      act( AT_SKILL, "You dismount $N.", ch, NULL, victim, TO_CHAR );
      act( AT_SKILL, "$n skillfully dismounts $N.", ch, NULL, victim, TO_NOTVICT );
      act( AT_SKILL, "$n dismounts you.  Whew!", ch, NULL, victim, TO_VICT );
      xREMOVE_BIT( victim->act, ACT_MOUNTED );
      ch->mount = NULL;
      ch->position = POS_STANDING;
      learn_from_success( ch, gsn_mount );
   }
   else
   {
      act( AT_SKILL, "You fall off while dismounting $N.  Ouch!", ch, NULL, victim, TO_CHAR );
      act( AT_SKILL, "$n falls off of $N while dismounting.", ch, NULL, victim, TO_NOTVICT );
      act( AT_SKILL, "$n falls off your back.", ch, NULL, victim, TO_VICT );
      learn_from_failure( ch, gsn_mount );
      xREMOVE_BIT( victim->act, ACT_MOUNTED );
      ch->mount = NULL;
      ch->position = POS_SITTING;
      global_retcode = damage( ch, ch, 1, TYPE_UNDEFINED );
   }
   return;
}

bool check_parry( CHAR_DATA * ch, CHAR_DATA * victim )
{
   int chances;

   if( !IS_AWAKE( victim ) )
      return FALSE;

   if( IS_NPC( victim ) && !xIS_SET( victim->defenses, DFND_PARRY ) )
      return FALSE;

   if( IS_NPC( victim ) )
   {
      chances = victim->level / sysdata.parry_mod;;
   }
   else
   {
      if( get_eq_char( victim, WEAR_WIELD ) == NULL )
         return FALSE;
      chances = ( int )( LEARNED( victim, gsn_parry ) / sysdata.parry_mod );
   }

   if( chances != 0 && victim->morph )
      chances += victim->morph->parry;

   if( !chance( victim, chances + victim->level - ch->level ) )
   {
      learn_from_failure( victim, gsn_parry );
      return FALSE;
   }
   if( !IS_NPC( victim ) && !IS_SET( victim->pcdata->flags, PCFLAG_GAG ) )
      act( AT_SKILL, "You parry $n's attack.", ch, NULL, victim, TO_VICT );

   if( !IS_NPC( ch ) && !IS_SET( ch->pcdata->flags, PCFLAG_GAG ) )
      act( AT_SKILL, "$N parries your attack.", ch, NULL, victim, TO_CHAR );

   learn_from_success( victim, gsn_parry );
   return TRUE;
}

bool check_steel_skin( CHAR_DATA * ch, CHAR_DATA * victim )
{

   if( !IS_AFFECTED( victim, AFF_STEEL_SKIN ) )
      return FALSE;

   if( victim->subtype > 0 )
   {
      act( AT_RED, "$n's attack bounces off your steel skin.", ch, NULL, victim, TO_VICT );
      act( AT_RED, "Your attack bounces off $N's steel skin.", ch, NULL, victim, TO_CHAR );
      victim->subtype--;
      if( victim->subtype <= 0 )
      {
         send_to_char( "Your skin feels soft once again.\n\r", victim );
         affect_strip( victim, skill_lookup( "steel skin" ) );
      }
      return TRUE;
   }
   return FALSE;
}

bool check_dodge( CHAR_DATA * ch, CHAR_DATA * victim )
{
   int chances;

   if( !IS_AWAKE( victim ) )
      return FALSE;

   if( IS_NPC( victim ) && !xIS_SET( victim->defenses, DFND_DODGE ) )
      return FALSE;

   if( IS_NPC( victim ) )
      chances = victim->level / sysdata.dodge_mod;
   else
      chances = ( int )( LEARNED( victim, gsn_dodge ) / sysdata.dodge_mod );

   if( chances != 0 && victim->morph != NULL )
      chances += victim->morph->dodge;

   if( !chance( victim, chances + victim->level - ch->level ) )
   {
      learn_from_failure( victim, gsn_dodge );
      return FALSE;
   }

   if( !IS_NPC( victim ) && !IS_SET( victim->pcdata->flags, PCFLAG_GAG ) )
      act( AT_SKILL, "You dodge $n's attack.", ch, NULL, victim, TO_VICT );

   if( !IS_NPC( ch ) && !IS_SET( ch->pcdata->flags, PCFLAG_GAG ) )
      act( AT_SKILL, "$N dodges your attack.", ch, NULL, victim, TO_CHAR );

   learn_from_success( victim, gsn_dodge );
   return TRUE;
}

bool check_tumble( CHAR_DATA * ch, CHAR_DATA * victim )
{
   int chances;

   if( victim->class != CLASS_THIEF || !IS_AWAKE( victim ) )
      return FALSE;
   if( !IS_NPC( victim ) && !victim->pcdata->learned[gsn_tumble] > 0 )
      return FALSE;
   if( IS_NPC( victim ) )
      chances = UMIN( 20, victim->level );
   else
      chances = ( int )( LEARNED( victim, gsn_tumble ) / sysdata.tumble_mod + ( get_curr_dex( victim ) - 13 ) );
   if( chances != 0 && victim->morph )
      chances += victim->morph->tumble;
   if( !chance( victim, chances + victim->level - ch->level ) )
      return FALSE;
   if( !IS_NPC( victim ) && !IS_SET( victim->pcdata->flags, PCFLAG_GAG ) )
      act( AT_SKILL, "You tumble away from $n's attack.", ch, NULL, victim, TO_VICT );
   if( !IS_NPC( ch ) && !IS_SET( ch->pcdata->flags, PCFLAG_GAG ) )
      act( AT_SKILL, "$N tumbles away from your attack.", ch, NULL, victim, TO_CHAR );
   learn_from_success( victim, gsn_tumble );
   return TRUE;
}

bool check_guard( CHAR_DATA * ch, CHAR_DATA * victim )
{
   int chances;

   if( victim->class != CLASS_MONK || !IS_AWAKE( victim ) )
      return FALSE;

   if( !IS_NPC( victim ) && !victim->pcdata->learned[gsn_guard] > 0 )
      return FALSE;

   if( IS_NPC( victim ) )
      chances = UMIN( 20, victim->level );
   else
      chances = number_range( LEARNED( victim, gsn_guard ), 100 );

   if( chances != 0 && victim->morph )
      chances += victim->morph->tumble;

   if( chances < 65 )
      return FALSE;

   if( !IS_NPC( victim ) && !IS_SET( victim->pcdata->flags, PCFLAG_GAG ) )
      act( AT_SKILL, "You tumble away from $n's attack.", ch, NULL, victim, TO_VICT );

   if( !IS_NPC( ch ) && !IS_SET( ch->pcdata->flags, PCFLAG_GAG ) )
      act( AT_SKILL, "$N tumbles away from your attack.", ch, NULL, victim, TO_CHAR );

   learn_from_success( victim, gsn_guard );
   return TRUE;
}


void do_poison_weapon( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *obj;
   OBJ_DATA *pobj;
   OBJ_DATA *wobj;
   char arg[MAX_INPUT_LENGTH];
   int percent;
//  int tmpvalue = 0;

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_poison_weapon ) )
   {
      send_to_char( "What do you think you are, a thief?\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "What are you trying to poison?\n\r", ch );
      return;
   }
   if( ch->fighting )
   {
      send_to_char( "While you're fighting?  Nice try.\n\r", ch );
      return;
   }

   if( !( obj = get_obj_carry( ch, arg ) ) )
   {
      send_to_char( "You do not have that weapon.\n\r", ch );
      return;
   }
   if( obj->item_type != ITEM_WEAPON )
   {
      send_to_char( "That item is not a weapon.\n\r", ch );
      return;
   }
   if( IS_OBJ_STAT( obj, ITEM_POISONED ) )
   {
      send_to_char( "That weapon is already poisoned.\n\r", ch );
      return;
   }
   if( IS_OBJ_STAT( obj, ITEM_CLANOBJECT ) )
   {
      send_to_char( "It doesn't appear to be fashioned of a poisonable material.\n\r", ch );
      return;
   }
   for( pobj = ch->first_carrying; pobj; pobj = pobj->next_content )
   {
      if( pobj->pIndexData->vnum == OBJ_VNUM_BLACK_POWDER )
         break;
   }
   if( !pobj )
   {
      send_to_char( "You do not have the black poison powder.\n\r", ch );
      return;
   }
   for( wobj = ch->first_carrying; wobj; wobj = wobj->next_content )
   {
      if( wobj->item_type == ITEM_DRINK_CON && wobj->value[1] > 0 && wobj->value[2] == 0 )
         break;
   }
   if( !wobj )
   {
      send_to_char( "You have no water to mix with the powder.\n\r", ch );
      return;
   }
   if( !IS_NPC( ch ) && get_curr_wis( ch ) < 16 )
   {
      send_to_char( "You can't quite remember what to do...\n\r", ch );
      return;
   }
   if( !IS_NPC( ch ) && ( ( get_curr_dex( ch ) < 13 ) || ch->pcdata->condition[COND_DRUNK] > 0 ) )
   {
      send_to_char( "Your hands aren't steady enough to properly mix the poison.\n\r", ch );
      return;
   }
//    WAIT_STATE( ch, skill_table[gsn_poison_weapon]->beats );

   percent = ( number_percent(  ) - get_curr_lck( ch ) - 14 );

   separate_obj( pobj );
   separate_obj( wobj );
   if( !can_use_skill( ch, percent, gsn_poison_weapon ) )
   {
      set_char_color( AT_RED, ch );
      send_to_char( "You failed and spill some on yourself.  Ouch!\n\r", ch );
      set_char_color( AT_GREY, ch );
      damage( ch, ch, ch->level, gsn_poison_weapon );
      act( AT_RED, "$n spills the poison all over!", ch, NULL, NULL, TO_ROOM );
      extract_obj( pobj );
      extract_obj( wobj );
      learn_from_failure( ch, gsn_poison_weapon );
      return;
   }
   separate_obj( obj );
   act( AT_RED, "You mix $p in $P, creating a deadly poison!", ch, pobj, wobj, TO_CHAR );
   act( AT_RED, "$n mixes $p in $P, creating a deadly poison!", ch, pobj, wobj, TO_ROOM );
   act( AT_GREEN, "You pour the poison over $p, which glistens wickedly!", ch, obj, NULL, TO_CHAR );
   act( AT_GREEN, "$n pours the poison over $p, which glistens wickedly!", ch, obj, NULL, TO_ROOM );
   xSET_BIT( obj->extra_flags, ITEM_POISONED );
   obj->timer = UMIN( obj->level, ch->level );

   if( IS_OBJ_STAT( obj, ITEM_BLESS ) )
      obj->timer *= 2;

   if( IS_OBJ_STAT( obj, ITEM_MAGIC ) )
      obj->timer *= 2;

   act( AT_BLUE, "The remainder of the poison eats through $p.", ch, wobj, NULL, TO_CHAR );
   act( AT_BLUE, "The remainder of the poison eats through $p.", ch, wobj, NULL, TO_ROOM );
   extract_obj( pobj );
   extract_obj( wobj );
   learn_from_success( ch, gsn_poison_weapon );
   return;
}

void do_sharpen( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *obj;
   OBJ_DATA *pobj;
   char arg[MAX_INPUT_LENGTH];
   int percent;
   int level;
   int msharp = 0;

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_sharpen ) )
   {
      send_to_char( "I am sorry, you cannot do that.\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "What do you wish to sharpen?\n\r", ch );
      return;
   }

   if( !( obj = get_obj_carry( ch, arg ) ) )
   {
      send_to_char( "You do not have that weapon.\n\r", ch );
      return;
   }

   if( obj->item_type != ITEM_WEAPON )
   {
      send_to_char( "You can't sharpen something that's not a weapon.\n\r", ch );
      return;
   }

   if( obj->value[4] == WEP_BAREHAND
       || obj->value[4] == WEP_WHIP
       || obj->value[4] == WEP_TALON
       || obj->value[4] == WEP_MACE
       || obj->value[4] == WEP_ARCHERY
       || obj->value[4] == WEP_BLOWGUN
       || obj->value[4] == WEP_STAFF || obj->value[4] == WEP_ENERGY || obj->value[4] == WEP_SLING )
   {
      send_to_char( "You can't sharpen that type of weapon!\n\r", ch );
      return;
   }

   msharp = 0;
   if( ( ( obj->value[2] - obj->value[1] <= msharp ) || ( xIS_SET( obj->extra_flags, ITEM_SHARP ) ) ) && !IS_IMMORTAL( ch ) )
   {
      send_to_char( "It is already as sharp as it's going to get.\n\r", ch );
      return;
   }

   for( pobj = ch->first_carrying; pobj; pobj = pobj->next_content )
   {
      if( pobj->pIndexData->vnum == OBJ_VNUM_SHARPEN )
         break;
   }

   if( !pobj )
   {
      send_to_char( "You do not have a sharpening stone.\n\r", ch );
      return;
   }

   WAIT_STATE( ch, skill_table[gsn_sharpen]->beats );
   if( !IS_IMMORTAL( ch ) && !IS_NPC( ch ) && get_curr_dex( ch ) < 15 )
   {
      separate_obj( obj );
      if( obj->value[0] <= 1 )
      {
         act( AT_OBJECT, "$p breaks apart and falls to the ground in pieces!.", ch, obj, NULL, TO_CHAR );
         extract_obj( obj );
         learn_from_failure( ch, gsn_sharpen );
         return;
      }
      else
      {
         obj->value[0]--;
         act( AT_GREEN, "You clumsily slip and damage $p!", ch, obj, NULL, TO_CHAR );
         return;
      }
   }

   percent = ( number_percent(  ) - get_curr_lck( ch ) - 15 );

   separate_obj( pobj );
   if( !IS_IMMORTAL( ch ) && !IS_NPC( ch ) && percent > ch->pcdata->learned[gsn_sharpen] )
   {
      act( AT_OBJECT, "You fail to sharpen $p correctly, damaging the stone.", ch, obj, NULL, TO_CHAR );
      if( pobj->value[0] <= 1 )
      {
         act( AT_OBJECT, "The sharpening stone crumbles apart from misuse.", ch, pobj, NULL, TO_CHAR );
         extract_obj( pobj );
         learn_from_failure( ch, gsn_sharpen );
         return;
      }
      pobj->value[0]--;
      learn_from_failure( ch, gsn_sharpen );
      return;
   }
   level = ch->level;
   separate_obj( obj );
   act( AT_SKILL, "With skill and precision, you sharpen $p to a fine edge.", ch, obj, NULL, TO_CHAR );
   act( AT_SKILL, "With skill and precision, $n sharpens $p.", ch, obj, NULL, TO_ROOM );
   obj->value[1] = obj->value[1] + 5;
   if( obj->value[1] >= obj->value[2] && !IS_IMMORTAL( ch ) )
      obj->value[1] = obj->value[2];
   if( obj->value[1] == obj->value[2] )
      if( !xIS_SET( obj->extra_flags, ITEM_SHARP ) )
      {
         xSET_BIT( obj->extra_flags, ITEM_SHARP );
      }
   learn_from_success( ch, gsn_sharpen );
   return;
}

void do_scribe( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *scroll;
   int sn;
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];
   int mana;

   if( IS_NPC( ch ) )
      return;

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_scribe ) )
   {
      send_to_char( "A skill such as this requires more magical ability than that of your class.\n\r", ch );
      return;
   }

   if( argument[0] == '\0' || !str_cmp( argument, "" ) )
   {
      send_to_char( "Scribe what?\n\r", ch );
      return;
   }

   if( ( sn = find_spell( ch, argument, TRUE ) ) < 0 )
   {
      send_to_char( "You have not learned that spell.\n\r", ch );
      return;
   }

   if( skill_table[sn]->spell_fun == spell_null )
   {
      send_to_char( "That's not a spell!\n\r", ch );
      return;
   }

   if( SPELL_FLAG( skill_table[sn], SF_NOSCRIBE ) )
   {
      send_to_char( "You cannot scribe that spell.\n\r", ch );
      return;
   }

   mana = IS_NPC( ch ) ? 0 : UMAX( skill_table[sn]->min_mana, 100 / ( 2 + ch->level - find_skill_level( ch, sn ) ) );

   mana *= 5;

   if( !IS_NPC( ch ) && ch->mana < mana )
   {
      send_to_char( "You don't have enough mana.\n\r", ch );
      return;
   }

   if( ( scroll = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
   {
      send_to_char( "You must be holding a blank scroll to scribe it.\n\r", ch );
      return;
   }

   if( scroll->pIndexData->vnum != OBJ_VNUM_SCROLL_SCRIBING )
   {
      send_to_char( "You must be holding a blank scroll to scribe it.\n\r", ch );
      return;
   }

   if( ( scroll->value[1] != -1 ) && ( scroll->pIndexData->vnum == OBJ_VNUM_SCROLL_SCRIBING ) )
   {
      send_to_char( "That scroll has already been inscribed.\n\r", ch );
      return;
   }

   if( !process_spell_components( ch, sn ) )
   {
      learn_from_failure( ch, gsn_scribe );
      ch->mana -= ( mana / 2 );
      return;
   }

   if( !can_use_skill( ch, number_percent(  ), gsn_scribe ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "You failed.\n\r", ch );
      learn_from_failure( ch, gsn_scribe );
      ch->mana -= ( mana / 2 );
      return;
   }

   scroll->value[1] = sn;
   scroll->value[0] = ch->level;
   sprintf( buf1, "%s scroll", skill_table[sn]->name );
   STRFREE( scroll->short_descr );
   scroll->short_descr = STRALLOC( aoran( buf1 ) );

   sprintf( buf2, "A glowing scroll inscribed '%s' lies in the dust.", skill_table[sn]->name );

   STRFREE( scroll->description );
   scroll->description = STRALLOC( buf2 );

   sprintf( buf3, "scroll scribing %s", skill_table[sn]->name );
   STRFREE( scroll->name );
   scroll->name = STRALLOC( buf3 );

   act( AT_MAGIC, "$n magically scribes $p.", ch, scroll, NULL, TO_ROOM );
   act( AT_MAGIC, "You magically scribe $p.", ch, scroll, NULL, TO_CHAR );

   learn_from_success( ch, gsn_scribe );

   ch->mana -= mana;

}

void do_brew( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *potion;
   OBJ_DATA *fire;
   int sn = 0;
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];
   int mana;
   bool found;

   if( IS_NPC( ch ) )
      return;

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, sn ) )
   {
      send_to_char( "A skill such as this requires more magical ability than that of your class.\n\r", ch );
      return;
   }

   if( argument[0] == '\0' || !str_cmp( argument, "" ) )
   {
      send_to_char( "Brew what?\n\r", ch );
      return;
   }

   if( ( sn = find_spell( ch, argument, TRUE ) ) < 0 )
   {
      send_to_char( "You have not learned that spell.\n\r", ch );
      return;
   }

   if( skill_table[sn]->spell_fun == spell_null )
   {
      send_to_char( "That's not a spell!\n\r", ch );
      return;
   }

   if( SPELL_FLAG( skill_table[sn], SF_NOBREW ) )
   {
      send_to_char( "You cannot brew that spell.\n\r", ch );
      return;
   }

   mana = IS_NPC( ch ) ? 0 : UMAX( skill_table[sn]->min_mana, 100 / ( 2 + ch->level - find_skill_level( ch, sn ) ) );

   mana *= 4;

   if( !IS_NPC( ch ) && ch->mana < mana )
   {
      send_to_char( "You don't have enough mana.\n\r", ch );
      return;
   }

   found = FALSE;

   for( fire = ch->in_room->first_content; fire; fire = fire->next_content )
   {
      if( fire->item_type == ITEM_FIRE )
      {
         found = TRUE;
         break;
      }
   }

   if( !found )
   {
      send_to_char( "There must be a fire in the room to brew a potion.\n\r", ch );
      return;
   }

   if( ( potion = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
   {
      send_to_char( "You must be holding an empty flask to brew a potion.\n\r", ch );
      return;
   }

   if( potion->pIndexData->vnum != OBJ_VNUM_FLASK_BREWING )
   {
      send_to_char( "You must be holding an empty flask to brew a potion.\n\r", ch );
      return;
   }

   if( ( potion->value[1] != -1 ) && ( potion->pIndexData->vnum == OBJ_VNUM_FLASK_BREWING ) )
   {
      send_to_char( "That's not an empty flask.\n\r", ch );
      return;
   }

   if( !process_spell_components( ch, sn ) )
   {
      learn_from_failure( ch, gsn_brew );
      ch->mana -= ( mana / 2 );
      return;
   }

   if( !can_use_skill( ch, number_percent(  ), gsn_brew ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "You failed.\n\r", ch );
      learn_from_failure( ch, gsn_brew );
      ch->mana -= ( mana / 2 );
      return;
   }

   potion->value[1] = sn;
   potion->value[0] = ch->level;
   sprintf( buf1, "%s potion", skill_table[sn]->name );
   STRFREE( potion->short_descr );
   potion->short_descr = STRALLOC( aoran( buf1 ) );

   sprintf( buf2, "A strange potion labelled '%s' sizzles in a glass flask.", skill_table[sn]->name );

   STRFREE( potion->description );
   potion->description = STRALLOC( buf2 );

   sprintf( buf3, "flask potion %s", skill_table[sn]->name );
   STRFREE( potion->name );
   potion->name = STRALLOC( buf3 );

   act( AT_MAGIC, "$n brews up $p.", ch, potion, NULL, TO_ROOM );
   act( AT_MAGIC, "You brew up $p.", ch, potion, NULL, TO_CHAR );

   learn_from_success( ch, gsn_brew );

   ch->mana -= mana;

}

bool check_grip( CHAR_DATA * ch, CHAR_DATA * victim )
{
   int chance;

   if( !IS_AWAKE( victim ) )
      return FALSE;

   if( IS_NPC( victim ) && !xIS_SET( victim->defenses, DFND_GRIP ) )
      return FALSE;

   if( IS_NPC( victim ) )
      chance = UMIN( 60, 2 * victim->level );
   else
      chance = ( int )( LEARNED( victim, gsn_grip ) / 2 );

   chance += ( 2 * ( get_curr_lck( victim ) - 13 ) );

   if( number_percent(  ) >= chance + victim->level - ch->level )
   {
      learn_from_failure( victim, gsn_grip );
      return FALSE;
   }
   act( AT_SKILL, "You evade $n's attempt to disarm you.", ch, NULL, victim, TO_VICT );
   act( AT_SKILL, "$N holds $S weapon strongly, and is not disarmed.", ch, NULL, victim, TO_CHAR );
   learn_from_success( victim, gsn_grip );
   return TRUE;
}

void do_circle( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   int percent;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You can't concentrate enough for that.\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( ch->mount )
   {
      send_to_char( "You can't circle while mounted.\n\r", ch );
      return;
   }

   if( arg[0] == '\0' )
   {
      send_to_char( "Circle around whom?\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "How can you sneak up on yourself?\n\r", ch );
      return;
   }

   if( is_safe( ch, victim ) )
      return;

   if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You need to wield a piercing or stabbing weapon.\n\r", ch );
      return;
   }

   if( obj->value[4] != WEP_DAGGER )
   {
      if( ( obj->value[4] == WEP_SWORD && obj->value[3] != DAM_PIERCE ) || obj->value[4] != WEP_SWORD )
      {
         send_to_char( "You need to wield a piercing or stabbing weapon.\n\r", ch );
         return;
      }
   }

   if( !ch->fighting )
   {
      send_to_char( "You can't circle when you aren't fighting.\n\r", ch );
      return;
   }

   if( !victim->fighting )
   {
      send_to_char( "You can't circle around a person who is not fighting.\n\r", ch );
      return;
   }

   if( victim->num_fighting < 2 )
   {
      act( AT_PLAIN, "You can't circle around them without a distraction.", ch, NULL, victim, TO_CHAR );
      return;
   }

   percent = number_percent(  ) - ( get_curr_lck( ch ) - 16 ) + ( get_curr_lck( victim ) - 13 );

   check_attacker( ch, victim );
//    WAIT_STATE( ch, skill_table[gsn_circle]->beats );
   if( can_use_skill( ch, percent, gsn_circle ) )
   {
      learn_from_success( ch, gsn_circle );
// WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
      global_retcode = multi_hit( ch, victim, gsn_circle );
      check_illegal_pk( ch, victim );
   }
   else
   {
      learn_from_failure( ch, gsn_circle );
      WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
      global_retcode = damage( ch, victim, 0, gsn_circle );
   }
   return;
}

void do_berserk( CHAR_DATA * ch, char *argument )
{
   sh_int percent;
   AFFECT_DATA af;

   if( !ch->fighting )
   {
      send_to_char( "But you aren't fighting!\n\r", ch );
      return;
   }

   if( IS_AFFECTED( ch, AFF_BERSERK ) )
   {
      send_to_char( "Your rage is already at its peak!\n\r", ch );
      return;
   }

   percent = LEARNED( ch, gsn_berserk );
//  WAIT_STATE(ch, skill_table[gsn_berserk]->beats);
   if( !chance( ch, percent ) )
   {
      send_to_char( "You couldn't build up enough rage.\n\r", ch );
      learn_from_failure( ch, gsn_berserk );
      return;
   }
   af.type = gsn_berserk;
   af.duration = number_range( ch->level / 5, ch->level * 2 / 5 );
   af.location = APPLY_STR;
   af.modifier = 1;
   af.bitvector = meb( AFF_BERSERK );
   affect_to_char( ch, &af );
   send_to_char( "You start to lose control..\n\r", ch );
   learn_from_success( ch, gsn_berserk );
   return;
}

ch_ret one_hit args( ( CHAR_DATA * ch, CHAR_DATA * victim, int dt ) );
void do_hitall( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   sh_int nvict = 0;
   sh_int nhit = 0;
   sh_int percent;

   if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
   {
      send_to_char_color( "&BA godly force prevents you.\n\r", ch );
      return;
   }

   if( !ch->in_room->first_person )
   {
      send_to_char( "There's no one else here!\n\r", ch );
      return;
   }
   percent = LEARNED( ch, gsn_hitall );
   for( vch = ch->in_room->first_person; vch; vch = vch_next )
   {
      vch_next = vch->next_in_room;
      if( is_same_group( ch, vch ) || !is_legal_kill( ch, vch ) || !can_see( ch, vch ) || is_safe( ch, vch ) )
         continue;
      if( ++nvict > ch->level / 5 )
         break;
      check_illegal_pk( ch, vch );
      if( chance( ch, percent ) )
      {
         nhit++;
         global_retcode = one_hit( ch, vch, TYPE_UNDEFINED );
      }
      else
         global_retcode = damage( ch, vch, 0, TYPE_UNDEFINED );
      if( global_retcode == rCHAR_DIED || global_retcode == rBOTH_DIED || char_died( ch ) )
         return;
   }
   if( !nvict )
   {
      send_to_char( "There's no one else here!\n\r", ch );
      return;
   }
   ch->move = UMAX( 0, ch->move - nvict * 3 + nhit );
   if( nhit )
      learn_from_success( ch, gsn_hitall );
   else
      learn_from_failure( ch, gsn_hitall );
   return;
}



bool check_illegal_psteal( CHAR_DATA * ch, CHAR_DATA * victim )
{
   if( !IS_NPC( victim ) && !IS_NPC( ch ) )
   {
      if( ( !IS_SET( victim->pcdata->flags, PCFLAG_DEADLY )
            || ch->level - victim->level > 10
            || !IS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) )
          && ( ch->in_room->vnum < 29 || ch->in_room->vnum > 43 ) && ch != victim )
      {
         return TRUE;
      }
   }
   return FALSE;
}

int scan_room( CHAR_DATA * ch, const ROOM_INDEX_DATA * room, char *buf, char *distance )
{
   CHAR_DATA *target = room->first_person;
   char buf1[MAX_STRING_LENGTH] = "";
   char buf2[MAX_STRING_LENGTH] = "";

   int number_found = 0;

   if( distance != NULL )
   {
      sprintf( buf1, "%s", distance );
      strcat( buf, buf1 );
   }
   while( target != NULL )
   {
      if( can_see( ch, target ) && target != ch )
      {
         if( IS_NPC( target ) )
         {
            sprintf( buf2, "%s&D", target->long_descr );
         }
         else if( IS_AFFECTED( target, AFF_SHAPESHIFT ) )
         {
            sprintf( buf2, "%s is here.\n\r&D", target->morph->morph->short_desc );
         }
         else
         {
            sprintf( buf2, "%s %s&D\n\r", PERS( target, ch ), can_see( ch, target ) ? target->pcdata->title : "is here." );
         }
         sprintf( buf1, "    %s%s", IS_NPC( target ) ? "&C" : "&D", buf2 );
         strcat( buf, buf1 );
         number_found++;
      }
      target = target->next_in_room;
   }

   return number_found;
}

int scan_room2( CHAR_DATA * ch, const ROOM_INDEX_DATA * room, char *buf, char *distance )
{
   CHAR_DATA *target = room->first_person;
   char buf1[MAX_STRING_LENGTH] = "";
   char buf2[MAX_STRING_LENGTH] = "";

   int number_found = 0;

   if( distance != NULL )
   {
      sprintf( buf1, "%s", distance );
      strcat( buf, buf1 );
   }
   while( target != NULL )
   {
      if( can_see( ch, target ) && target != ch )
      {
         if( IS_NPC( target ) )
         {
            sprintf( buf2, "%s&D", target->long_descr );
         }
         else if( IS_AFFECTED( target, AFF_SHAPESHIFT ) )
         {
            sprintf( buf2, "%s is here.\n\r&D", target->morph->morph->short_desc );
         }
         else
         {
            sprintf( buf2, "%s %s&D\n\r", PERS( target, ch ), can_see( ch, target ) ? target->pcdata->title : "is here." );
         }
         sprintf( buf1, "    %s%s", IS_NPC( target ) ? "&C" : "&D", buf2 );
         strcat( buf, buf1 );
         number_found++;
      }
      target = target->next_in_room;
   }

   return number_found;
}

void do_scan( CHAR_DATA * ch, char *argument )
{
   EXIT_DATA *pexit;
   ROOM_INDEX_DATA *was_in_room;
   extern char *const dir_name[];
   char buf[MAX_STRING_LENGTH];
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   sh_int dir;
   sh_int dist;
   sh_int max_dist;
   int totalppl, ppl;

   if( ch->position == POS_SLEEPING && !IS_AGOD( ch ) )
   {
      send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
      return;
   }

   act( AT_PLAIN, "$n scans for trouble...", ch, NULL, NULL, TO_CANSEE );
   if( LEARNED( ch, gsn_longsight ) )
   {
      was_in_room = ch->in_room;
      send_to_char( "Scanning ... \n\r", ch );

      if( WATER_SECT( ch ) )
      {
         sprintf( buf2, "&B(WATER)&D" );
      }
      else if( LAVA_SECT( ch ) )
      {
         sprintf( buf2, "&R(LAVA)&D" );
      }
      else
      {
         buf2[0] = '\0';
      }
      sprintf( buf1, "&p[&Bright here&p] %s\n\r", buf2 );
      buf[0] = '\0';
      if( ( totalppl = scan_room( ch, ch->in_room, buf, buf1 ) ) != 0 )
         send_to_char( buf, ch );

      for( dir = 0; dir < 10; dir++ )
      {
         if( ( pexit = get_exit( ch->in_room, dir ) ) == NULL )
         {
            continue;
         }
         max_dist = 3;
         if( ch->level < 100 )
            --max_dist;

         for( dist = 1; dist <= max_dist; )
         {
            if( IS_SET( pexit->exit_info, EX_CLOSED ) )
               break;
            if( room_is_private( pexit->to_room ) && ch->level < LEVEL_BUILD )
               break;

            char_from_room( ch );
            char_to_room( ch, pexit->to_room );

            if( WATER_SECT( ch ) )
            {
               sprintf( buf2, "&B(WATER)&D" );
            }
            else if( LAVA_SECT( ch ) )
            {
               sprintf( buf2, "&R(LAVA)&D" );
            }
            else
            {
               buf2[0] = '\0';
            }
            switch ( dist )
            {
               default:
                  break;
               case 1:
                  sprintf( buf, "&p[&Bimmediately to the %s&p] %s\n\r", dir_name[dir], buf2 );
                  break;
               case 2:
                  sprintf( buf, "&p[&Bvery near to the %s&p] %s\n\r", dir_name[dir], buf2 );
                  break;
               case 3:
                  sprintf( buf, "&p[&Bnearby to the %s&p] %s\n\r", dir_name[dir], buf2 );
                  break;
               case 4:
                  sprintf( buf, "&p[&Ba ways to the %s&p] %s\n\r", dir_name[dir], buf2 );
                  break;
               case 5:
                  sprintf( buf, "&p[&Ba long way to the %s&p] %s\n\r", dir_name[dir], buf2 );
                  break;
               case 6:
                  sprintf( buf, "&p[&Ba very long way to the %s&p] %s\n\r", dir_name[dir], buf2 );
                  break;
               case 7:
                  sprintf( buf, "&p[&Ba very long way to the %s&p] %s\n\r", dir_name[dir], buf2 );
                  break;
               case 8:
                  sprintf( buf, "&p[&Ba very long way to the %s&p] %s\n\r", dir_name[dir], buf2 );
                  break;
            }

            sprintf( buf1, "%s", buf );

            buf[0] = '\0';
            if( ( ppl = scan_room2( ch, ch->in_room, buf, buf1 ) ) != 0 )
               send_to_char( buf, ch );

            totalppl += ppl;

            switch ( ch->in_room->sector_type )
            {
               default:
                  dist++;
                  break;
               case SECT_AIR:
                  if( number_percent(  ) < 80 )
                     dist++;
                  break;
               case SECT_INSIDE:
               case SECT_FIELD:
               case SECT_UNDERGROUND:
                  dist++;
                  break;
               case SECT_FOREST:
               case SECT_CITY:
               case SECT_DESERT:
               case SECT_HILLS:
                  dist += 2;
                  break;
               case SECT_WATER_SWIM:
               case SECT_WATER_NOSWIM:
                  dist += 3;
                  break;
               case SECT_MOUNTAIN:
               case SECT_UNDERWATER:
               case SECT_OCEANFLOOR:
                  dist += 4;
                  break;
            }

            if( dist >= max_dist )
            {
               break;
            }
            if( ( pexit = get_exit( ch->in_room, dir ) ) == NULL )
            {
               break;
            }
         }

         char_from_room( ch );
         char_to_room( ch, was_in_room );

      }
   }
   else
   {
      was_in_room = ch->in_room;
      send_to_char( "Scanning ... \n\r", ch );

      if( WATER_SECT( ch ) )
      {
         sprintf( buf2, "&B(WATER)&D" );
      }
      else if( LAVA_SECT( ch ) )
      {
         sprintf( buf2, "&R(LAVA)&D" );
      }
      else
      {
         buf2[0] = '\0';
      }
      sprintf( buf1, "&p[&wright here&p] %s\n\r", buf2 );
      buf[0] = '\0';
      if( ( totalppl = scan_room( ch, ch->in_room, buf, buf1 ) ) != 0 )
         send_to_char( buf, ch );

      for( dir = 0; dir < 10; dir++ )
      {
         if( ( pexit = get_exit( ch->in_room, dir ) ) == NULL )
         {
            continue;
         }
         max_dist = 1;

         for( dist = 1; dist <= max_dist; )
         {
            if( IS_SET( pexit->exit_info, EX_CLOSED ) )
               break;
            if( room_is_private( pexit->to_room ) && ch->level < LEVEL_BUILD )
               break;

            char_from_room( ch );
            char_to_room( ch, pexit->to_room );

            if( WATER_SECT( ch ) )
            {
               sprintf( buf2, "&B(WATER)&D" );
            }
            else if( LAVA_SECT( ch ) )
            {
               sprintf( buf2, "&R(LAVA)&D" );
            }
            else
            {
               buf2[0] = '\0';
            }
            sprintf( buf, "&p[&w%s&p] %s\n\r", dir_name[dir], buf2 );

            sprintf( buf1, "%s", buf );

            buf[0] = '\0';
            if( ( ppl = scan_room( ch, ch->in_room, buf, buf1 ) ) != 0 )
               send_to_char( buf, ch );

            totalppl += ppl;

            switch ( ch->in_room->sector_type )
            {
               default:
                  dist++;
                  break;
               case SECT_AIR:
                  if( number_percent(  ) < 80 )
                     dist++;
                  break;
               case SECT_INSIDE:
               case SECT_FIELD:
               case SECT_UNDERGROUND:
                  dist++;
                  break;
               case SECT_FOREST:
               case SECT_CITY:
               case SECT_DESERT:
               case SECT_HILLS:
                  dist += 2;
                  break;
               case SECT_WATER_SWIM:
               case SECT_WATER_NOSWIM:
                  dist += 3;
                  break;
               case SECT_MOUNTAIN:
               case SECT_UNDERWATER:
               case SECT_OCEANFLOOR:
                  dist += 4;
                  break;
            }

            if( dist >= max_dist )
            {
               break;
            }
            if( ( pexit = get_exit( ch->in_room, dir ) ) == NULL )
            {
               break;
            }
         }

         char_from_room( ch );
         char_to_room( ch, was_in_room );

      }
   }
   if( totalppl == 0 )
   {
      send_to_char( "noone", ch );
   }

}

void do_slice( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *corpse;
   OBJ_DATA *obj;
   OBJ_DATA *slice;
   bool found;
   MOB_INDEX_DATA *pMobIndex;
   char buf[MAX_STRING_LENGTH];
   char buf1[MAX_STRING_LENGTH];
   found = FALSE;

   if( !IS_NPC( ch ) && !IS_IMMORTAL( ch ) && !DUAL_SKILL( ch, gsn_slice ) )
   {
      send_to_char( "You are not learned in this skill.\n\r", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      send_to_char( "From what do you wish to slice meat?\n\r", ch );
      return;
   }


   if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
       || ( obj->value[3] != 1 && obj->value[3] != 2 && obj->value[3] != 3 && obj->value[3] != 11 ) )
   {
      send_to_char( "You need to wield a sharp weapon.\n\r", ch );
      return;
   }

   if( ( corpse = get_obj_here( ch, argument ) ) == NULL )
   {
      send_to_char( "You can't find that here.\n\r", ch );
      return;
   }

   if( corpse->item_type != ITEM_CORPSE_NPC || corpse->value[3] < 75 )
   {
      send_to_char( "That is not a suitable source of meat.\n\r", ch );
      return;
   }

   if( ( pMobIndex = get_mob_index( ( int )-( corpse->value[2] ) ) ) == NULL )
   {
      bug( "Can not find mob for value[2] of corpse, do_slice", 0 );
      return;
   }

   if( get_obj_index( OBJ_VNUM_SLICE ) == NULL )
   {
      bug( "Vnum 24 not found for do_slice!", 0 );
      return;
   }

   if( !can_use_skill( ch, number_percent(  ), gsn_slice ) && !IS_IMMORTAL( ch ) )
   {
      send_to_char( "You fail to slice the meat properly.\n\r", ch );
      learn_from_failure( ch, gsn_slice );
      if( number_percent(  ) + ( get_curr_dex( ch ) - 13 ) < 10 )
      {
         act( AT_BLOOD, "You cut yourself!", ch, NULL, NULL, TO_CHAR );
         damage( ch, ch, ch->level, gsn_slice );
      }
      return;
   }

   slice = create_object( get_obj_index( OBJ_VNUM_SLICE ), 0 );

   sprintf( buf, "meat fresh slice %s", pMobIndex->player_name );
   STRFREE( slice->name );
   slice->name = STRALLOC( buf );

   sprintf( buf, "a slice of raw meat from %s", pMobIndex->short_descr );
   STRFREE( slice->short_descr );
   slice->short_descr = STRALLOC( buf );

   sprintf( buf1, "A slice of raw meat from %s lies on the ground.", pMobIndex->short_descr );
   STRFREE( slice->description );
   slice->description = STRALLOC( buf1 );

   act( AT_BLOOD, "$n cuts a slice of meat from $p.", ch, corpse, NULL, TO_ROOM );
   act( AT_BLOOD, "You cut a slice of meat from $p.", ch, corpse, NULL, TO_CHAR );

   obj_to_char( slice, ch );
   corpse->value[3] -= 25;
   learn_from_success( ch, gsn_slice );
   return;
}

void do_style( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];

   if( IS_NPC( ch ) )
      return;

   one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      ch_printf_color( ch, "&wAdopt which fighting style?  (current:  %s&w)\n\r",
                       ch->style == STYLE_BERSERK ? "&Rberserk" :
                       ch->style == STYLE_AGGRESSIVE ? "&Raggressive" :
                       ch->style == STYLE_DEFENSIVE ? "&Ydefensive" :
                       ch->style == STYLE_EVASIVE ? "&Yevasive" : "standard" );
      return;
   }

   if( !str_prefix( arg, "evasive" ) )
   {
      if( !DUAL_SKILL( ch, gsn_style_evasive ) )
      {
         send_to_char( "You have not yet learned enough to fight evasively.\n\r", ch );
         return;
      }
//      WAIT_STATE( ch, skill_table[gsn_style_evasive]->beats );
      if( number_percent(  ) < LEARNED( ch, gsn_style_evasive ) )
      {
         if( ch->fighting )
         {
            ch->position = POS_EVASIVE;
            learn_from_success( ch, gsn_style_evasive );
            if( IS_PKILL( ch ) )
               act( AT_ACTION, "$n falls back into an evasive stance.", ch, NULL, NULL, TO_ROOM );
         }
         ch->style = STYLE_EVASIVE;
         send_to_char( "You adopt an evasive fighting style.\n\r", ch );
         return;
      }
      else
      {
         send_to_char( "You nearly trip in a lame attempt to adopt an evasive fighting style.\n\r", ch );
         return;
      }
   }
   else if( !str_prefix( arg, "defensive" ) )
   {
      if( !DUAL_SKILL( ch, gsn_style_defensive ) )
      {
         send_to_char( "You have not yet learned enough to fight defensively.\n\r", ch );
         return;
      }
//      WAIT_STATE( ch, skill_table[gsn_style_defensive]->beats );
      if( number_percent(  ) < LEARNED( ch, gsn_style_defensive ) )
      {
         if( ch->fighting )
         {
            ch->position = POS_DEFENSIVE;
            learn_from_success( ch, gsn_style_defensive );
            if( IS_PKILL( ch ) )
               act( AT_ACTION, "$n moves into a defensive posture.", ch, NULL, NULL, TO_ROOM );
         }
         ch->style = STYLE_DEFENSIVE;
         send_to_char( "You adopt a defensive fighting style.\n\r", ch );
         return;
      }
      else
      {
         send_to_char( "You nearly trip in a lame attempt to adopt a defensive fighting style.\n\r", ch );
         return;
      }
   }
   else if( !str_prefix( arg, "standard" ) )
   {
      if( !DUAL_SKILL( ch, gsn_style_standard ) )
      {
         send_to_char( "You have not yet learned enough to fight in the standard style.\n\r", ch );
         return;
      }
//      WAIT_STATE( ch, skill_table[gsn_style_standard]->beats );
      if( number_percent(  ) < LEARNED( ch, gsn_style_standard ) )
      {
         if( ch->fighting )
         {
            ch->position = POS_FIGHTING;
            learn_from_success( ch, gsn_style_standard );
            if( IS_PKILL( ch ) )
               act( AT_ACTION, "$n switches to a standard fighting style.", ch, NULL, NULL, TO_ROOM );
         }
         ch->style = STYLE_FIGHTING;
         send_to_char( "You adopt a standard fighting style.\n\r", ch );
         return;
      }
      else
      {
         send_to_char( "You nearly trip in a lame attempt to adopt a standard fighting style.\n\r", ch );
         return;
      }
   }
   else if( !str_prefix( arg, "aggressive" ) )
   {
      if( !DUAL_SKILL( ch, gsn_style_aggressive ) )
      {
         send_to_char( "You have not yet learned enough to fight aggressively.\n\r", ch );
         return;
      }
//      WAIT_STATE( ch, skill_table[gsn_style_aggressive]->beats );
      if( number_percent(  ) < LEARNED( ch, gsn_style_aggressive ) )
      {
         if( ch->fighting )
         {
            ch->position = POS_AGGRESSIVE;
            learn_from_success( ch, gsn_style_aggressive );
            if( IS_PKILL( ch ) )
               act( AT_ACTION, "$n assumes an aggressive stance.", ch, NULL, NULL, TO_ROOM );
         }
         ch->style = STYLE_AGGRESSIVE;
         send_to_char( "You adopt an aggressive fighting style.\n\r", ch );
         return;
      }
      else
      {
         send_to_char( "You nearly trip in a lame attempt to adopt an aggressive fighting style.\n\r", ch );
         return;
      }
   }
   else if( !str_prefix( arg, "berserk" ) )
   {
      if( !DUAL_SKILL( ch, gsn_style_berserk ) )
      {
         send_to_char( "You have not yet learned enough to fight as a berserker.\n\r", ch );
         return;
      }
//      WAIT_STATE( ch, skill_table[gsn_style_berserk]->beats );
      if( number_percent(  ) < LEARNED( ch, gsn_style_berserk ) )
      {
         if( ch->fighting )
         {
            ch->position = POS_BERSERK;
            learn_from_success( ch, gsn_style_berserk );
            if( IS_PKILL( ch ) )
               act( AT_ACTION, "$n enters a wildly aggressive style.", ch, NULL, NULL, TO_ROOM );
         }
         ch->style = STYLE_BERSERK;
         send_to_char( "You adopt a berserk fighting style.\n\r", ch );
         return;
      }
      else
      {
         send_to_char( "You nearly trip in a lame attempt to adopt a berserk fighting style.\n\r", ch );
         return;
      }
   }

   send_to_char( "Adopt which fighting style?\n\r", ch );

   return;
}

bool can_use_skill( CHAR_DATA * ch, int percent, int gsn )
{
   bool check = FALSE;
   if( IS_NPC( ch ) && percent < 85 )
      check = TRUE;
   else if( !IS_NPC( ch ) && percent < LEARNED( ch, gsn ) )
      check = TRUE;
   else if( ch->morph && ch->morph->morph && ch->morph->morph->skills &&
            ch->morph->morph->skills[0] != '\0' &&
            is_name( skill_table[gsn]->name, ch->morph->morph->skills ) && percent < 85 )
      check = TRUE;
   if( ch->morph && ch->morph->morph && ch->morph->morph->no_skills &&
       ch->morph->morph->no_skills[0] != '\0' && is_name( skill_table[gsn]->name, ch->morph->morph->no_skills ) )
      check = FALSE;
   return check;
}

void do_cook( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *food, *fire;
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];

   one_argument( argument, arg );
   if( IS_NPC( ch ) || DUAL_SKILL( ch, gsn_cook ) )
   {
      send_to_char( "That skill is beyond your understanding.\n\r", ch );
      return;
   }
   if( arg[0] == '\0' )
   {
      send_to_char( "Cook what?\n\r", ch );
      return;
   }

   if( ( food = get_obj_carry( ch, arg ) ) == NULL )
   {
      send_to_char( "You do not have that item.\n\r", ch );
      return;
   }
   if( food->item_type != ITEM_COOK )
   {
      send_to_char( "How can you cook that?\n\r", ch );
      return;
   }
   if( food->value[2] > 2 )
   {
      send_to_char( "That is already burnt to a crisp.\n\r", ch );
      return;
   }
   for( fire = ch->in_room->first_content; fire; fire = fire->next_content )
   {
      if( fire->item_type == ITEM_FIRE )
         break;
   }
   if( !fire )
   {
      send_to_char( "There is no fire here!\n\r", ch );
      return;
   }
   separate_obj( food );
   if( number_percent(  ) > LEARNED( ch, gsn_cook ) )
   {
      food->timer = food->timer / 2;
      food->value[0] = 0;
      food->value[2] = 3;
      act( AT_MAGIC, "$p catches on fire burning it to a crisp!\n\r", ch, food, NULL, TO_CHAR );
      act( AT_MAGIC, "$n catches $p on fire burning it to a crisp.", ch, food, NULL, TO_ROOM );
      sprintf( buf, "a burnt %s", food->pIndexData->name );
      STRFREE( food->short_descr );
      food->short_descr = STRALLOC( buf );
      sprintf( buf, "A burnt %s.", food->pIndexData->name );
      STRFREE( food->description );
      food->description = STRALLOC( buf );
      return;
   }

   if( number_percent(  ) > 85 )
   {
      food->timer = food->timer * 3;
      food->value[2] += 2;
      act( AT_MAGIC, "$n overcooks a $p.", ch, food, NULL, TO_ROOM );
      act( AT_MAGIC, "You overcook a $p.", ch, food, NULL, TO_CHAR );
      sprintf( buf, "an overcooked %s", food->pIndexData->name );
      STRFREE( food->short_descr );
      food->short_descr = STRALLOC( buf );
      sprintf( buf, "An overcooked %s.", food->pIndexData->name );
      STRFREE( food->description );
      food->description = STRALLOC( buf );
   }
   else
   {
      food->timer = food->timer * 4;
      food->value[0] *= 2;
      act( AT_MAGIC, "$n roasts a $p.", ch, food, NULL, TO_ROOM );
      act( AT_MAGIC, "You roast a $p.", ch, food, NULL, TO_CHAR );
      sprintf( buf, "a roasted %s", food->pIndexData->name );
      STRFREE( food->short_descr );
      food->short_descr = STRALLOC( buf );
      sprintf( buf, "A roasted %s.", food->pIndexData->name );
      STRFREE( food->description );
      food->description = STRALLOC( buf );
      food->value[2]++;
   }
   learn_from_success( ch, gsn_cook );
}

/*
void do_assassinate( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int percent;

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Assassinate whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( IS_IMMORTAL(victim) )
    {
	send_to_char( "You might make them angry if you tryed.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "choke" ) )
    {
      act( AT_WHITE, "You wrap your meaty arms around $N's neck choking out the life.",  ch, NULL, victim, TO_CHAR    );
      act( AT_WHITE, "$n wrapped meaty arms around your neck and choked you to death.", ch, NULL, victim, TO_VICT    );
      act( AT_WHITE, "$n wraps meaty arms around $N's neck choking out the life.",  ch, NULL, victim, TO_NOTVICT );
    }

    else if ( !str_cmp( arg2, "poison" ) )
    {
      act( AT_LBLUE, "You blow posion powder in $N's face causing them to convulse as their bodies blood vessels explode.",  ch, NULL, victim, TO_CHAR    );
      act( AT_LBLUE, "$n blew poison powder in your face, causing you to convulse as your blood vessels explode.", ch, NULL, victim, TO_VICT    );
      act( AT_LBLUE, "$n blows poison powder in $N's face causing convulsion as $N's blood vessels explode.",  ch, NULL, victim, TO_NOTVICT );
    }
	else if (!str_cmp( arg2, "backstab" ) )
	{
      act( AT_RED, "Your backstab finds its mark between $N's shoulder blades.",  ch, NULL, victim, TO_CHAR );
      act( AT_RED, "$n's backstab hits you square inbetween the shoulder blades, stealing your life.",  ch, NULL, victim, TO_VICT );
      act( AT_RED, "$n backstabs $N in square inbetween the shoulder blades, stealing $N's life.",  ch, NULL, victim, TO_NOTVICT );
     }
    else
	{ 
		send_to_char( "What kind of assassination attemp: poison, choke, backstab?\n\r", ch );
		return;
	  } 
	percent = number_percent( ) - (get_curr_lck(ch) - 14) 
	      + (get_curr_lck(victim) - 13);
    if ( !IS_AWAKE(victim)
      || can_use_skill( ch, percent, gsn_backstab ) )
{
	learn_from_success( ch, gsn_assassinate );
	check_illegal_pk( ch, victim );
    set_cur_char(victim);
 	raw_kill( ch, victim );
}
 else
    {
	learn_from_failure( ch, gsn_assassinate );
	check_illegal_pk( ch, victim );
    }
    
    return;
    
}*/

void do_reglist( CHAR_DATA * ch, char *argument )
{
   DIR *dp;
   struct dirent *de;

   pager_printf_color( ch, "These are the players who are registered.\n\r" );
   if( !( dp = opendir( REGISTER_DIR ) ) )
   {
      bug( "Load_corpses: can't open REGISTER_DIR", 0 );
      perror( CORPSE_DIR );
      return;
   }

   while( ( de = readdir( dp ) ) != NULL )
   {
      if( de->d_name[0] != '.' )
      {
         pager_printf_color( ch, "%s\n\r", de->d_name );
      }
   }
   closedir( dp );
   return;
}

void do_register( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *mob;
   OBJ_DATA *obj;
   char outbuf[MAX_STRING_LENGTH];
   char regsave[MAX_INPUT_LENGTH];
   char arg1[MAX_STRING_LENGTH];
   FILE *Rfp;
   int Rcost = 1000;

   argument = one_argument( argument, arg1 );

   if( !ch )
   {
      bug( "do_register: NULL ch!", 0 );
      return;
   }

   if( IS_NPC( ch ) )
      return;

   if( ch->level < 50 )
   {
      send_to_char( "You are not able to do that.\n\r", ch );
      return;
   }

   for( mob = ch->in_room->first_person; mob; mob = mob->next_in_room )
      if( IS_NPC( mob ) && xIS_SET( mob->act, ACT_REGISTRAR ) )
         break;


   if( arg1[0] != '\0' && !str_cmp( arg1, "cost" ) )
   {
      sprintf( outbuf, "It will cost %s katyr to register your equipment %s.", num_punct( Rcost ), ch->name );

      if( mob )
      {
         do_say( mob, outbuf );
      }
      else if( IS_IMMORTAL( ch ) )
      {
         send_to_char( outbuf, ch );
      }
      else
      {
         send_to_char( "You can't do that here.\n\r", ch );
      }
      return;
   }
   else if( arg1[0] != '\0' && !IS_IMMORTAL( ch ) )
   {
      send_to_char( "Excuse me?\n\r", ch );
      return;
   }

   if( !mob && !IS_IMMORTAL( ch ) )
   {
      send_to_char( "You can't do that here.\n\r", ch );
      return;
   }

   if( ch->gold < Rcost )
   {
      if( mob )
      {
         sprintf( outbuf, "You don't have enough katyr to do that %s", ch->name );
         do_say( mob, outbuf );
         return;
      }
      else
      {
         if( !IS_IMMORTAL( ch ) )
         {
            return;
         }
      }
   }
   else
   {
      if( !mob )
      {
         if( !IS_IMMORTAL( ch ) )
         {
            return;
         }
      }
      else
      {
         sprintf( outbuf, "Thank you %s.", ch->name );
         do_say( mob, outbuf );
         ch->gold -= Rcost;
         boost_economy( mob->in_room->area, Rcost );
         act( AT_ACTION, "$n picks up a quill and begins writing in a book.", mob, NULL, ch, TO_ROOM );
      }
   }

   de_equip_char( ch );

   for( obj = ch->first_carrying; obj; obj = obj->next_content )
   {
      obj->owner = STRALLOC( ch->name );
      xSET_BIT( obj->extra_flags, ITEM_OWNED );
   }

   sprintf( regsave, "%s%s", REGISTER_DIR, capitalize( ch->pcdata->filename ) );

   if( ( Rfp = fopen( regsave, "w" ) ) == NULL )
   {
      perror( regsave );
      bug( "do_register: fopen", 0 );
   }
   else
   {
      bool ferr;
      if( ch->first_carrying )
         fwrite_obj( ch, ch->last_carrying, Rfp, 0, OS_CARRY );

      fprintf( Rfp, "#END\n" );
      ferr = ferror( Rfp );
      fclose( Rfp );
      if( ferr )
      {
         perror( regsave );
         bug( "Error writing temp file for %s -- not copying", regsave );
      }
   }

   re_equip_char( ch );
   sprintf( outbuf, "Your equipment has now been registered." );
   if( !mob )
      send_to_char( outbuf, ch );
   else
      do_say( mob, outbuf );
   return;
}

void do_reimb( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   char arg[MAX_STRING_LENGTH];
   char regsave[MAX_INPUT_LENGTH];
   FILE *Rfp;
   int i, x;

   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      send_to_char( "Reimb who?\n\r", ch );
      return;
   }
   else
   {
      if( ( victim = get_char_world( ch, arg ) ) != NULL )
      {
         if( IS_NPC( victim ) )
         {
            send_to_char( "Not on Mobiles.\n\r", ch );
            return;
         }
         sprintf( regsave, "%s%s", REGISTER_DIR, capitalize( victim->pcdata->filename ) );
         if( ( Rfp = fopen( regsave, "r" ) ) != NULL )
         {
            de_equip_char( victim );
            while( ( obj = victim->first_carrying ) != NULL )
            {
               obj_from_char( obj );
            }

            for( x = 0; x < MAX_WEAR; x++ )
               for( i = 0; i < MAX_LAYERS; i++ )
                  save_equipment[x][i] = NULL;

            for( ;; )
            {
               char letter;
               char *word;
               letter = fread_letter( Rfp );
               if( letter == '*' )
               {
                  fread_to_eol( Rfp );
                  continue;
               }
               if( letter != '#' )
               {
                  bug( "do_reimb: no # found", 0 );
                  break;
               }
               word = fread_word( Rfp );
               if( !str_cmp( word, "OBJECT" ) )
               {
                  fread_obj( victim, Rfp, OS_REGISTER );
               }
               else if( !str_cmp( word, "END" ) )
               {
                  break;
               }
               else
               {
                  bug( "do_reimb: bad section", 0 );
                  break;
               }
            }
            fclose( Rfp );
/*                for ( i = 0; i < MAX_WEAR; i++ ) 
				{
					for ( x = 0; x < MAX_LAYERS; x++ ) 
					{
						if ( save_equipment[i][x] )
						{
							equip_char( ch, save_equipment[i][x], i);
							save_equipment[i][x] = NULL;
						}
						else 
						{
							break;
						}
					}
				}*/
            send_to_char( "Your equipment has been restored.\n\r", victim );
            send_to_char( "Registration restored.\n\r", ch );
         }
         else
         {
            send_to_char( "No registration found.\n\r", ch );
         }
      }
      else
      {
         send_to_char( "They are not here.\n\r", ch );
      }
      return;
   }
   return;
}


void do_heighten( CHAR_DATA * ch, char *argument )
{
   AFFECT_DATA af;

   if( IS_AFFECTED( ch, AFF_DETECT_INVIS ) && IS_AFFECTED( ch, AFF_DETECT_HIDDEN ) )
   {
      send_to_char( "You fail to heighten since other augments prevent you from seeing hidden and invis.\n\r", ch );
      return;
   }

   if( IS_AFFECTED( ch, AFF_DETECT_INVIS ) )
   {
      af.type = gsn_heighten;
      af.duration = 400;
      af.location = APPLY_AFFECT;
      af.modifier = 0;
      af.bitvector = meb( AFF_DETECT_HIDDEN );
      affect_to_char( ch, &af );
      send_to_char( "Other augmentation prevents you from detecting invisible right now.\n\r", ch );
      send_to_char( "Your senses are partialy heightened.\n\r", ch );
      return;
   }

   if( IS_AFFECTED( ch, AFF_DETECT_HIDDEN ) )
   {
      af.type = gsn_heighten;
      af.duration = 400;
      af.location = APPLY_AFFECT;
      af.modifier = 0;
      af.bitvector = meb( AFF_DETECT_INVIS );
      affect_to_char( ch, &af );
      send_to_char( "Other augmentation prevents you from detecting hidden right now.\n\r", ch );
      send_to_char( "Your senses are partialy heightened.\n\r", ch );
      return;
   }

   af.type = gsn_heighten;
   af.duration = 400;
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_DETECT_HIDDEN );
   affect_to_char( ch, &af );

   af.type = gsn_heighten;
   af.duration = 400;
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_DETECT_INVIS );
   affect_to_char( ch, &af );
   send_to_char( "Your senses are completely heightened.\n\r", ch );
   return;
}

void do_shapeshift( CHAR_DATA * ch, char *argument )
{
   MORPH_DATA *morph;
   sh_int percent;
   AFFECT_DATA af;
   char arg[MAX_STRING_LENGTH];
   bool fForm = FALSE;

   if( !ch )
   {
      bug( "do_shapeshift: NULL ch!", 0 );
      return;
   }
   if( IS_NPC( ch ) )
   {
      send_to_char( "Mobiles can't do that.\n\r", ch );
      return;
   }
   if( ch->fighting )
   {
      send_to_char( "Not while your still fighting...\n\r", ch );
      return;
   }
/*    if ( IS_AFFECTED(ch, AFF_SHAPESHIFT) )
    {
        send_to_char( "You are already using an alternative form!\n\r", ch );
        return;
    }*/
   if( global_retcode == rCHAR_DIED || global_retcode == rBOTH_DIED || char_died( ch ) )
      return;

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      int mcnt = 0;
      send_to_pager( "\n\r&CThe Following Forms are Available:\n\r&W-[ &GKey Words&W ]-- -[&G Form Description &W]--&w\n\r",
                     ch );
      if( !morph_start )
      {
         send_to_pager( "&R (None Available) &w\n\r", ch );
         return;
      }
      for( morph = morph_start; morph; morph = morph->next )
      {
         if( ( morph->level <= ch->level && ( morph->class != 0 && IS_SET( morph->class, ( 1 << ch->class ) ) ) )
             || IS_IMMORTAL( ch ) || ( morph->race != 0 && IS_SET( morph->race, ( 1 << ch->race ) ) ) )
         {
            mcnt++;
            pager_printf_color( ch, "&C%-15.15s &W:&w%s&w\n\r", morph->key_words, morph->short_desc );
         }
      }

      if( mcnt == 0 )
         send_to_pager( "&R (None Available)&w\n\r", ch );

      send_to_char( "&W----------------------------------------&w\n\r", ch );
   }
   else
   {
      char buf[MAX_STRING_LENGTH];
      if( !morph_start )
      {
         send_to_pager( "&R(No Forms Available)&w\n\r", ch );
         return;
      }
      for( morph = morph_start; morph; morph = morph->next )
      {
         if( ( ( morph->level <= ch->level && ( morph->class != 0 && IS_SET( morph->class, ( 1 << ch->class ) ) ) )
               || IS_IMMORTAL( ch )
               || ( morph->race != 0 && IS_SET( morph->race, ( 1 << ch->race ) ) ) )
             && nifty_is_name( arg, morph->key_words ) )
         {
            fForm = TRUE;
            break;
         }
      }

      if( fForm )
      {
         sprintf( buf, "%d", morph->vnum );
         do_imm_morph( ch, buf );
      }

      if( ch->morph == NULL || ch->morph->morph == NULL )
         fForm = FALSE;

      percent = LEARNED( ch, gsn_shapeshift );
      WAIT_STATE( ch, skill_table[gsn_shapeshift]->beats );
      if( IS_AFFECTED( ch, AFF_SHAPESHIFT ) || !fForm )
      {
         send_to_char( "&YYou fail to assume an alternative form.&w\n\r", ch );
         learn_from_failure( ch, gsn_shapeshift );
         return;
      }
      af.type = gsn_shapeshift;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = ( sdur * 4 ) + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur * 4;
      }
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.bitvector = meb( AFF_SHAPESHIFT );
      affect_to_char( ch, &af );
      learn_from_success( ch, gsn_shapeshift );
      return;
   }
   return;
}

void do_unshift( CHAR_DATA * ch, char *argument )
{
   if( ( skill_table[gsn_shapeshift]->skill_level[ch->class] == -1 && !IS_IMMORTAL( ch ) )
       && ( LEARNED( ch, gsn_shapeshift ) < 1 ) )
   {
      send_to_char( "Huh!\n\r", ch );
      return;
   }
   if( IS_AFFECTED( ch, AFF_SHAPESHIFT ) )
   {
      affect_strip( ch, gsn_shapeshift );
      xREMOVE_BIT( ch->affected_by, AFF_SHAPESHIFT );
      do_unmorph_char( ch );
   }
   else
      send_to_char( "You are already in your true form.\n\r", ch );
   return;
}

void do_throw( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   AFFECT_DATA af;

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      ch_printf( ch, "You can't concentrate enough for that.\n\r" );
      return;
   }

   if( !IS_NPC( ch ) && !DUAL_SKILL( ch, gsn_throw ) )
   {
      ch_printf( ch, "You better leave the martial arts to fighters.\n\r" );
      return;
   }

   if( ( victim = who_fighting( ch ) ) == NULL )
   {
      if( ( victim = get_char_room( ch, argument ) ) == NULL )
      {
         ch_printf( ch, "You aren't fighting anyone.\n\r" );
         return;
      }
   }
   if( !IS_NPC( victim ) )
   {
      ch_printf( ch, "You cannot use this on players.\n\r" );
      return;
   }

   WAIT_STATE( ch, skill_table[gsn_throw]->beats );

   if( can_use_skill( ch, number_percent(  ), gsn_throw ) )
   {
      learn_from_success( ch, gsn_throw );
      if( ( number_range( 1, 100 ) < ( LEARNED( ch, gsn_throw ) + 10 ) ) && !IS_AFFECTED( victim, AFF_PARALYSIS ) )
      {
         ch_printf( ch, "You throw %s to the ground.\n\r", victim->short_descr );
         ch_printf( ch, "  ... stunning your victim.\n\r" );
         global_retcode = damage( ch, victim, number_range( 100, 200 ), gsn_throw );
         af.type = gsn_throw;
         af.location = APPLY_AC;
         af.modifier = 20;
         af.duration = 1;
         af.bitvector = meb( AFF_PARALYSIS );
         affect_to_char( victim, &af );
         update_pos( victim );
      }
      else
         global_retcode = damage( ch, victim, number_range( 80, 180 ), gsn_throw );
   }
   else
   {
      learn_from_failure( ch, gsn_throw );
      global_retcode = damage( ch, victim, 0, gsn_throw );
   }
   return;
}
