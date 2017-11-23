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
/*						Combat module		*/
/************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "acadia.h"

extern char lastplayercmd[MAX_INPUT_LENGTH];
extern CHAR_DATA *gch_prev;
extern int tkills;
extern int tdeaths;
extern int tpkills;
extern int tpdeaths;
extern int akills;
extern int adeaths;

OBJ_DATA *used_weapon;
int totalHits = 0;
int totalDamage = 0;

int multi_hitCheck( int dt, CHAR_DATA * ch );
void resetTotals(  );
void new_dam_message args( ( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, int totalHitsA, OBJ_DATA * obj ) );
void death_cry args( ( CHAR_DATA * ch ) );
void group_gain args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
int xp_compute args( ( CHAR_DATA * gch, CHAR_DATA * victim ) );
int align_compute args( ( CHAR_DATA * gch, CHAR_DATA * victim ) );
ch_ret one_hit args( ( CHAR_DATA * ch, CHAR_DATA * victim, int dt ) );
int obj_hitroll args( ( OBJ_DATA * obj ) );
void show_condition args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool check_steel_skin args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );

bool is_attack_supressed( CHAR_DATA * ch )
{
   TIMER *timer;

   if( IS_NPC( ch ) )
      return FALSE;

   timer = get_timerptr( ch, TIMER_ASUPRESSED );

   if( !timer )
      return FALSE;

   if( timer->value == -1 )
      return TRUE;

   if( timer->count >= 1 )
      return TRUE;

   return FALSE;
}

bool is_wielding_poisoned( CHAR_DATA * ch )
{
   OBJ_DATA *obj;

   if( !used_weapon )
      return FALSE;

   if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) != NULL && used_weapon == obj && IS_OBJ_STAT( obj, ITEM_POISONED ) )
      return TRUE;
   if( ( obj = get_eq_char( ch, WEAR_DUAL_WIELD ) ) != NULL && used_weapon == obj && IS_OBJ_STAT( obj, ITEM_POISONED ) )
      return TRUE;

   return FALSE;
}

bool is_wsilver( CHAR_DATA * ch )
{
   OBJ_DATA *obj;

   if( !used_weapon )
      return FALSE;

   if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) != NULL && used_weapon == obj && IS_OBJ_STAT( obj, ITEM_SILVER ) )
      return TRUE;

   if( ( obj = get_eq_char( ch, WEAR_DUAL_WIELD ) ) != NULL && used_weapon == obj && IS_OBJ_STAT( obj, ITEM_SILVER ) )
      return TRUE;

   return FALSE;
}

bool is_hunting( CHAR_DATA * ch, CHAR_DATA * victim )
{
   if( !ch->hunting || ch->hunting->who != victim )
      return FALSE;

   return TRUE;
}

bool is_hating( CHAR_DATA * ch, CHAR_DATA * victim )
{
   if( !ch->hating || ch->hating->who != victim )
      return FALSE;

   return TRUE;
}

bool is_fearing( CHAR_DATA * ch, CHAR_DATA * victim )
{
   if( !ch->fearing || ch->fearing->who != victim )
      return FALSE;

   return TRUE;
}

void stop_hunting( CHAR_DATA * ch )
{
   if( ch->hunting )
   {
      STRFREE( ch->hunting->name );
      DISPOSE( ch->hunting );
      ch->hunting = NULL;
   }
   return;
}

void stop_hating( CHAR_DATA * ch )
{
   if( ch->hating )
   {
      STRFREE( ch->hating->name );
      DISPOSE( ch->hating );
      ch->hating = NULL;
   }
   return;
}

void stop_fearing( CHAR_DATA * ch )
{
   if( ch->fearing )
   {
      STRFREE( ch->fearing->name );
      DISPOSE( ch->fearing );
      ch->fearing = NULL;
   }
   return;
}

void start_hunting( CHAR_DATA * ch, CHAR_DATA * victim )
{
   if( ch->hunting )
      stop_hunting( ch );

   CREATE( ch->hunting, HHF_DATA, 1 );
   ch->hunting->name = QUICKLINK( victim->name );
   ch->hunting->who = victim;
   return;
}

void start_hating( CHAR_DATA * ch, CHAR_DATA * victim )
{
   if( ch->hating )
      stop_hating( ch );

   CREATE( ch->hating, HHF_DATA, 1 );
   ch->hating->name = QUICKLINK( victim->name );
   ch->hating->who = victim;
   return;
}

void start_fearing( CHAR_DATA * ch, CHAR_DATA * victim )
{
   if( ch->fearing )
      stop_fearing( ch );

   CREATE( ch->fearing, HHF_DATA, 1 );
   ch->fearing->name = QUICKLINK( victim->name );
   ch->fearing->who = victim;
   return;
}

sh_int VAMP_AC( CHAR_DATA * ch )
{
   if( IS_VAMPIRE( ch ) && IS_OUTSIDE( ch ) )
   {
      switch ( time_info.sunlight )
      {
         case SUN_DARK:
            return -10;
         case SUN_RISE:
            return 5;
         case SUN_LIGHT:
            return 10;
         case SUN_SET:
            return 2;
         default:
            return 0;
      }
   }
   else
      return 0;
}

int max_fight( CHAR_DATA * ch )
{
   return 8;
}

void violence_update( void )
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *ch;
   CHAR_DATA *lst_ch;
   CHAR_DATA *victim;
   CHAR_DATA *rch, *rch_next;
   AFFECT_DATA *paf, *paf_next;
   TIMER *timer, *timer_next;
   ch_ret retcode;
   int attacktype, cnt;
   SKILLTYPE *skill;
   static int pulse = 0;

   lst_ch = NULL;
   pulse = ( pulse + 1 ) % 100;

   for( ch = last_char; ch; lst_ch = ch, ch = gch_prev )
   {
      set_cur_char( ch );

      if( ch == first_char && ch->prev )
      {
         bug( "ERROR: first_char->prev != NULL, fixing...", 0 );
         ch->prev = NULL;
      }

      gch_prev = ch->prev;

      if( gch_prev && gch_prev->next != ch )
      {
         sprintf( buf, "FATAL: violence_update: %s->prev->next doesn't point to ch.", ch->name );
         bug( buf, 0 );
         bug( "Short-cutting here", 0 );
         ch->prev = NULL;
         gch_prev = NULL;
         do_shout( ch, "Nicole says, 'Prepare for the worst!'" );
      }

      if( char_died( ch ) )
         continue;

      if( !ch->in_room || !ch->name )
      {
         log_string( "violence_update: bad ch record!  (Shortcutting.)" );
         sprintf( buf, "ch: %d  ch->in_room: %d  ch->prev: %d  ch->next: %d",
                  ( int )ch, ( int )ch->in_room, ( int )ch->prev, ( int )ch->next );
         log_string( buf );
         log_string( lastplayercmd );
         if( lst_ch )
            sprintf( buf, "lst_ch: %d  lst_ch->prev: %d  lst_ch->next: %d",
                     ( int )lst_ch, ( int )lst_ch->prev, ( int )lst_ch->next );
         else
            strcpy( buf, "lst_ch: NULL" );
         log_string( buf );
         gch_prev = NULL;
         continue;
      }

      if( ch->fighting )
         if( ( ++ch->fighting->duration % 24 ) == 0 )
            ch->fighting->xp = ( ( ch->fighting->xp * 10 ) / 10 );


      for( timer = ch->first_timer; timer; timer = timer_next )
      {
         timer_next = timer->next;
         if( --timer->count <= 0 )
         {
            if( timer->type == TIMER_ASUPRESSED )
            {
               if( timer->value == -1 )
               {
                  timer->count = 1000;
                  continue;
               }
            }
            if( timer->type == TIMER_NUISANCE )
            {
               DISPOSE( ch->pcdata->nuisance );
            }

            if( timer->type == TIMER_DO_FUN )
            {
               int tempsub;

               tempsub = ch->substate;
               ch->substate = timer->value;
               ( timer->do_fun ) ( ch, "" );
               if( char_died( ch ) )
                  break;
               ch->substate = tempsub;
            }
            extract_timer( ch, timer );
         }
      }

      if( char_died( ch ) )
         continue;

      for( paf = ch->first_affect; paf; paf = paf_next )
      {
         paf_next = paf->next;
         if( paf->duration > 0 )
            paf->duration--;
         else if( paf->duration < 0 )
            ;
         else
         {
            if( !paf_next || paf_next->type != paf->type || paf_next->duration > 0 )
            {
               skill = get_skilltype( paf->type );
               if( paf->type > 0 && skill && skill->msg_off )
               {
                  set_char_color( AT_WEAROFF, ch );
                  send_to_char( skill->msg_off, ch );
                  send_to_char( "\n\r", ch );
               }
            }
            if( paf->type == gsn_possess )
            {
               ch->desc->character = ch->desc->original;
               ch->desc->original = NULL;
               ch->desc->character->desc = ch->desc;
               ch->desc->character->switched = NULL;
               ch->desc = NULL;
            }
            affect_remove( ch, paf );
         }
      }

      if( char_died( ch ) )
         continue;

      if( ( retcode = pullcheck( ch, pulse ) ) == rCHAR_DIED || char_died( ch ) )
         continue;

      if( ( victim = who_fighting( ch ) ) == NULL || IS_AFFECTED( ch, AFF_PARALYSIS ) )
         continue;

      retcode = rNONE;

      if( ( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) ) )
      {
         sprintf( buf, "violence_update: %s fighting %s in a SAFE room.", ch->name, victim->name );
         log_string( buf );
         stop_fighting( ch, TRUE );
      }
      else if( IS_AWAKE( ch ) && ch->in_room == victim->in_room )
         retcode = multi_hit( ch, victim, TYPE_UNDEFINED );
      else
         stop_fighting( ch, FALSE );

      if( sysdata.pkill_lock == 1 && !IS_NPC( victim ) && !IS_NPC( ch ) )
      {
         send_to_char( "I'm sorry, global no pkill is on.\n\r", ch );
         sprintf( buf, "violence_update: %s fighting %s after pkill_lock turned on.", ch->name, victim->name );
         log_string( buf );
         stop_fighting( ch, TRUE );
      }

      if( sysdata.fight_lock == 1 )
      {
         send_to_char( "I'm sorry, fight lock is on.\n\r", ch );
         sprintf( buf, "violence_update: %s fighting %s after fight_lock turned on.", ch->name, victim->name );
         log_string( buf );
         stop_fighting( ch, TRUE );
      }

      if( char_died( ch ) )
         continue;

      if( retcode == rCHAR_DIED || ( victim = who_fighting( ch ) ) == NULL )
         continue;

      rprog_rfight_trigger( ch );
      if( char_died( ch ) || char_died( victim ) )
         continue;
      mprog_hitprcnt_trigger( ch, victim );
      if( char_died( ch ) || char_died( victim ) )
         continue;
      mprog_fight_trigger( ch, victim );
      if( char_died( ch ) || char_died( victim ) )
         continue;

      if( IS_NPC( ch ) )
      {
         if( !xIS_EMPTY( ch->attacks ) )
         {
            attacktype = -1;
            if( 30 + ( ch->level / 4 ) >= number_percent(  ) )
            {
               cnt = 0;
               for( ;; )
               {
                  if( cnt++ > 10 )
                  {
                     attacktype = -1;
                     break;
                  }
                  attacktype = number_range( 7, MAX_ATTACK_TYPE - 1 );
                  if( xIS_SET( ch->attacks, attacktype ) )
                     break;
               }
               switch ( attacktype )
               {
                  case ATCK_BASH:
                     do_bash( ch, "" );
                     retcode = global_retcode;
                     break;
                  case ATCK_STUN:
                     do_stun( ch, "" );
                     retcode = global_retcode;
                     break;
                  case ATCK_GOUGE:
                     do_gouge( ch, "" );
                     retcode = global_retcode;
                     break;
                  case ATCK_FEED:
                     do_gouge( ch, "" );
                     retcode = global_retcode;
                     break;
                  case ATCK_DRAIN:
                     retcode = spell_energy_drain( skill_lookup( "energy drain" ), ch->level, ch, victim );
                     break;
                  case ATCK_FIREBREATH:
                     retcode = spell_fire_breath( skill_lookup( "fire breath" ), ch->level, ch, victim );
                     break;
                  case ATCK_FROSTBREATH:
                     retcode = spell_frost_breath( skill_lookup( "frost breath" ), ch->level, ch, victim );
                     break;
                  case ATCK_ACIDBREATH:
                     retcode = spell_acid_breath( skill_lookup( "acid breath" ), ch->level, ch, victim );
                     break;
                  case ATCK_LIGHTNBREATH:
                     retcode = spell_lightning_breath( skill_lookup( "lightning breath" ), ch->level, ch, victim );
                     break;
                  case ATCK_GASBREATH:
                     retcode = spell_gas_breath( skill_lookup( "gas breath" ), ch->level, ch, victim );
                     break;
                  case ATCK_SPIRALBLAST:
                     retcode = spell_spiral_blast( skill_lookup( "spiral blast" ), ch->level, ch, victim );
                     break;
                  case ATCK_POISON:
                     retcode = spell_poison( gsn_poison, ch->level, ch, victim );
                     break;
                  case ATCK_NASTYPOISON:
                     break;
                  case ATCK_GAZE:
                     break;
                  case ATCK_BLINDNESS:
                     retcode = spell_blindness( gsn_blindness, ch->level, ch, victim );
                     break;
                  case ATCK_CAUSESERIOUS:
                     retcode = spell_cause_serious( skill_lookup( "cause serious" ), ch->level, ch, victim );
                     break;
                  case ATCK_EARTHQUAKE:
                     retcode = spell_earthquake( skill_lookup( "earthquake" ), ch->level, ch, victim );
                     break;
                  case ATCK_CAUSECRITICAL:
                     retcode = spell_cause_critical( skill_lookup( "cause critical" ), ch->level, ch, victim );
                     break;
                  case ATCK_CURSE:
                     retcode = spell_curse( skill_lookup( "curse" ), ch->level, ch, victim );
                     break;
                  case ATCK_FLAMESTRIKE:
                     retcode = spell_flamestrike( skill_lookup( "flamestrike" ), ch->level, ch, victim );
                     break;
                  case ATCK_HARM:
                     retcode = spell_harm( skill_lookup( "harm" ), ch->level, ch, victim );
                     break;
                  case ATCK_FIREBALL:
                     retcode = spell_fireball( skill_lookup( "fireball" ), ch->level, ch, victim );
                     break;
                  case ATCK_COLORSPRAY:
                     retcode = spell_colour_spray( skill_lookup( "colour spray" ), ch->level, ch, victim );
                     break;
                  case ATCK_WEAKEN:
                     retcode = spell_weaken( skill_lookup( "weaken" ), ch->level, ch, victim );
                     break;
               }
               if( attacktype != -1 && ( retcode == rCHAR_DIED || char_died( ch ) ) )
                  continue;
            }
         }
         if( !xIS_EMPTY( ch->defenses ) )
         {
            attacktype = -1;
            if( 50 + ( ch->level / 4 ) > number_percent(  ) )
            {
               cnt = 0;
               for( ;; )
               {
                  if( cnt++ > 10 )
                  {
                     attacktype = -1;
                     break;
                  }
                  attacktype = number_range( 2, MAX_DEFENSE_TYPE - 1 );
                  if( xIS_SET( ch->defenses, attacktype ) )
                     break;
               }

               switch ( attacktype )
               {
                  case DFND_CURELIGHT:
                     if( ch->hit < ch->max_hit )
                     {
                        act( AT_MAGIC, "$n mutters a few incantations...and looks a little better.", ch, NULL, NULL,
                             TO_ROOM );
                        retcode = spell_cure_light( skill_lookup( "cure light" ), ch->level, ch, ch );
                     }
                     break;
                  case DFND_CURESERIOUS:
                     if( ch->hit < ch->max_hit )
                     {
                        act( AT_MAGIC, "$n mutters a few incantations...and looks a bit better.", ch, NULL, NULL, TO_ROOM );
                        retcode = spell_cure_serious( skill_lookup( "cure serious" ), ch->level, ch, ch );
                     }
                     break;
                  case DFND_CURECRITICAL:
                     if( ch->hit < ch->max_hit )
                     {
                        act( AT_MAGIC, "$n mutters a few incantations...and looks healthier.", ch, NULL, NULL, TO_ROOM );
                        retcode = spell_cure_critical( skill_lookup( "cure critical" ), ch->level, ch, ch );
                     }
                     break;
                  case DFND_HEAL:
                     if( ch->hit < ch->max_hit )
                     {
                        act( AT_MAGIC, "$n mutters a few incantations...and looks much healthier.", ch, NULL, NULL,
                             TO_ROOM );
                        retcode = spell_heal_i( skill_lookup( "heal" ), ch->level, ch, ch );
                     }
                     break;
                  case DFND_DISPELMAGIC:
                     if( ch->first_affect )
                     {
                        act( AT_MAGIC, "$n utters an incantation...", ch, NULL, NULL, TO_ROOM );
                        retcode = spell_dispel_magic( skill_lookup( "dispel magic" ), ch->level, ch, victim );
                     }
                     break;
                  case DFND_DISPELEVIL:
                     act( AT_MAGIC, "$n utters an incantation...", ch, NULL, NULL, TO_ROOM );
                     retcode = spell_dispel_evil( skill_lookup( "dispel evil" ), ch->level, ch, victim );
                     break;
                  case DFND_TELEPORT:
                     retcode = spell_teleport( skill_lookup( "teleport" ), ch->level, ch, ch );
                     break;
                  case DFND_SHOCKSHIELD:
                     if( !IS_AFFECTED( ch, AFF_SHOCKSHIELD ) )
                     {
                        act( AT_MAGIC, "$n utters a few incantations...", ch, NULL, NULL, TO_ROOM );
                        retcode = spell_shockshield( skill_lookup( "shockshield" ), ch->level, ch, ch );
                     }
                     else
                        retcode = rNONE;
                     break;
                  case DFND_VENOMSHIELD:
                     if( !IS_AFFECTED( ch, AFF_VENOMSHIELD ) )
                     {
                        act( AT_MAGIC, "$n utters a few incantations ...", ch, NULL, NULL, TO_ROOM );
                        retcode = spell_acadia( skill_lookup( "venomshield" ), ch->level, ch, ch );
                     }
                     else
                        retcode = rNONE;
                     break;
                  case DFND_ACIDMIST:
                     if( !IS_AFFECTED( ch, AFF_ACIDMIST ) )
                     {
                        act( AT_MAGIC, "$n utters a few incantations ...", ch, NULL, NULL, TO_ROOM );
                        retcode = spell_acadia( skill_lookup( "acidmist" ), ch->level, ch, ch );
                     }
                     else
                        retcode = rNONE;
                     break;
                  case DFND_FIRESHIELD:
                     if( !IS_AFFECTED( ch, AFF_FIRESHIELD ) )
                     {
                        act( AT_MAGIC, "$n utters a few incantations...", ch, NULL, NULL, TO_ROOM );
                        retcode = spell_fireshield( skill_lookup( "fireshield" ), ch->level, ch, ch );
                     }
                     else
                        retcode = rNONE;
                     break;
                  case DFND_ICESHIELD:
                     if( !IS_AFFECTED( ch, AFF_ICESHIELD ) )
                     {
                        act( AT_MAGIC, "$n utters a few incantations...", ch, NULL, NULL, TO_ROOM );
                        retcode = spell_iceshield( skill_lookup( "iceshield" ), ch->level, ch, ch );
                     }
                     else
                        retcode = rNONE;
                     break;
                  case DFND_TRUESIGHT:
                     if( !IS_AFFECTED( ch, AFF_TRUESIGHT ) )
                        retcode = spell_true_sight( skill_lookup( "true" ), ch->level, ch, ch );
                     else
                        retcode = rNONE;
                     break;
                  case DFND_SANCTUARY:
                     if( !IS_AFFECTED( ch, AFF_SANCTUARY ) )
                     {
                        act( AT_MAGIC, "$n utters a few incantations...", ch, NULL, NULL, TO_ROOM );
                        retcode = spell_sanctuary( skill_lookup( "sanctuary" ), ch->level, ch, ch );
                     }
                     else
                        retcode = rNONE;
                     break;
               }
               if( attacktype != -1 && ( retcode == rCHAR_DIED || char_died( ch ) ) )
                  continue;
            }
         }
      }

      for( rch = ch->in_room->first_person; rch; rch = rch_next )
      {
         rch_next = rch->next_in_room;

         if( ( !IS_NPC( ch ) && !IS_NPC( rch ) )
             && ( rch != ch )
             && ( rch->fighting )
             && ( who_fighting( rch->fighting->who ) == ch )
             && ( !xIS_SET( rch->fighting->who->act, ACT_AUTONOMOUS ) ) && ( rch->style < ch->style ) )
         {
            rch->fighting->who->fighting->who = rch;
         }

         if( IS_AWAKE( rch ) && !rch->fighting )
         {
            if( ( !IS_NPC( ch ) && is_same_group( ch, rch ) ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
            {
               if( ( !IS_NPC( rch ) && rch->desc ) || IS_AFFECTED( rch, AFF_CHARM ) || IS_AFFECTED( rch, AFF_JMT ) )
               {
                  if( !is_safe( rch, victim ) )
                     multi_hit( rch, victim, TYPE_UNDEFINED );
               }
               continue;
            }

            if( IS_NPC( rch ) && ( !IS_AFFECTED( rch, AFF_CHARM ) || !IS_AFFECTED( rch, AFF_JMT ) )
                && !xIS_SET( rch->act, ACT_NOASSIST ) && !xIS_SET( rch->act, ACT_PACIFIST ) )
            {
               if( char_died( ch ) )
                  break;
               if( rch->pIndexData == ch->pIndexData || number_bits( 3 ) == 0 )
               {
                  CHAR_DATA *vch;
                  CHAR_DATA *target;
                  int number;

                  target = NULL;
                  number = 0;
                  for( vch = ch->in_room->first_person; vch; vch = vch->next )
                  {
                     if( can_see( rch, vch ) && is_same_group( vch, victim ) && number_range( 0, number ) == 0 )
                     {
                        if( vch->mount && vch->mount == rch )
                           target = NULL;
                        else
                        {
                           target = vch;
                           number++;
                        }
                     }
                  }
                  if( target && !xIS_SET( rch->act, ACT_PACIFIST ) )
                  {
                     act( AT_BLOOD, "$n screams and attacks...", rch, NULL, NULL, TO_ROOM );
                     multi_hit( rch, target, TYPE_UNDEFINED );
                  }
               }
            }
         }
      }
   }
   return;
}

int multi_hitCheck( int dt, CHAR_DATA * ch )
{
   OBJ_DATA *wield;
   static bool dual_flip = FALSE;

   if( ( wield = get_eq_char( ch, WEAR_DUAL_WIELD ) ) != NULL )
   {
      if( dual_flip == FALSE )
      {
         dual_flip = TRUE;
         wield = get_eq_char( ch, WEAR_WIELD );
      }
      else
         dual_flip = FALSE;
   }
   else
      wield = get_eq_char( ch, WEAR_WIELD );

   if( dt == TYPE_UNDEFINED )
   {
      dt = TYPE_HIT;
      if( wield && wield->item_type == ITEM_WEAPON )
         dt += wield->value[3];
   }

   return dt;
}

ch_ret multi_hit( CHAR_DATA * ch, CHAR_DATA * victim, int dt )
{
   int chance;
   int dual_bonus;
   ch_ret retcode;

   if( !IS_NPC( ch ) && !IS_NPC( victim ) )
   {
      if( xIS_SET( ch->act, PLR_NICE ) )
         return rNONE;
      add_timer( ch, TIMER_RECENTFIGHT, 11, NULL, 0 );
      add_timer( victim, TIMER_RECENTFIGHT, 11, NULL, 0 );
   }

   if( is_attack_supressed( ch ) )
      return rNONE;

   if( IS_NPC( ch ) && xIS_SET( ch->act, ACT_NOATTACK ) )
      return rNONE;

   if( ( retcode = one_hit( ch, victim, dt ) ) != rNONE )
      return retcode;

   totalHits++;

   if( who_fighting( ch ) != victim || dt == gsn_backstab || dt == gsn_circle || dt == gsn_assassinate )
      return rNONE;

   chance = IS_NPC( ch ) ? 100 : ( LEARNED( ch, gsn_berserk ) * 5 / 2 );
   if( IS_AFFECTED( ch, AFF_BERSERK ) && number_percent(  ) < chance )
   {
      totalHits++;
      if( ( retcode = one_hit( ch, victim, dt ) ) != rNONE || who_fighting( ch ) != victim )
      {
         if( victim->position != POS_DEAD )
         {
            dt = multi_hitCheck( dt, ch );
            dam_message( ch, victim, totalDamage, dt, totalHits );
            resetTotals(  );
         }
         return retcode;
      }
   }
   if( get_eq_char( ch, WEAR_DUAL_WIELD ) )
   {
      dual_bonus = IS_NPC( ch ) ? ( ch->level / 10 ) : ( LEARNED( ch, gsn_dual_wield ) * 2 );
      chance = IS_NPC( ch ) ? ch->level : LEARNED( ch, gsn_dual_wield );
      if( number_percent(  ) < chance )
      {
         learn_from_success( ch, gsn_dual_wield );
         retcode = one_hit( ch, victim, dt );
         totalHits++;

         if( retcode != rNONE || who_fighting( ch ) != victim )
         {
            if( victim->position != POS_DEAD )
            {
               dt = multi_hitCheck( dt, ch );
               dam_message( ch, victim, totalDamage, dt, totalHits );
               resetTotals(  );
            }
            return retcode;
         }
      }
      else
         learn_from_failure( ch, gsn_dual_wield );
   }
   else
      dual_bonus = 0;

   if( ch->move < 10 )
      dual_bonus = -20;

   if( IS_NPC( ch ) && ch->numattacks > 0 )
   {
      for( chance = 0; chance < ch->numattacks; chance++ )
      {
         retcode = one_hit( ch, victim, dt );
         totalHits++;
         if( retcode != rNONE || who_fighting( ch ) != victim )
         {
            if( victim->position != POS_DEAD )
            {
               dt = multi_hitCheck( dt, ch );
               dam_message( ch, victim, totalDamage, dt, totalHits );
               resetTotals(  );
            }
            return retcode;
         }
      }
      return retcode;
   }

   chance = IS_NPC( ch ) ? ch->level : ( int )( ( LEARNED( ch, gsn_second_attack ) + dual_bonus ) );
   if( number_percent(  ) < chance )
   {
      learn_from_success( ch, gsn_second_attack );
      totalHits++;
      retcode = one_hit( ch, victim, dt );
      if( retcode != rNONE || who_fighting( ch ) != victim )
      {
         if( victim->position != POS_DEAD )
         {
            dt = multi_hitCheck( dt, ch );
            dam_message( ch, victim, totalDamage, dt, totalHits );
            resetTotals(  );
         }
         return retcode;
      }
   }
   else
      learn_from_failure( ch, gsn_second_attack );

   chance = IS_NPC( ch ) ? ch->level : ( int )( ( LEARNED( ch, gsn_third_attack ) + ( dual_bonus * 1.5 ) ) );
   if( number_percent(  ) < chance )
   {
      learn_from_success( ch, gsn_third_attack );
      totalHits++;
      retcode = one_hit( ch, victim, dt );
      if( retcode != rNONE || who_fighting( ch ) != victim )
      {
         if( victim->position != POS_DEAD )
         {
            dt = multi_hitCheck( dt, ch );
            dam_message( ch, victim, totalDamage, dt, totalHits );
            resetTotals(  );
         }
         return retcode;
      }
   }
   else
      learn_from_failure( ch, gsn_third_attack );

   chance = IS_NPC( ch ) ? ch->level : ( int )( ( LEARNED( ch, gsn_fourth_attack ) + ( dual_bonus * 2 ) ) );
   if( number_percent(  ) < chance )
   {
      learn_from_success( ch, gsn_fourth_attack );
      totalHits++;
      retcode = one_hit( ch, victim, dt );
      if( retcode != rNONE || who_fighting( ch ) != victim )
      {
         if( victim->position != POS_DEAD )
         {
            dt = multi_hitCheck( dt, ch );
            dam_message( ch, victim, totalDamage, dt, totalHits );
            resetTotals(  );
         }
         return retcode;
      }
   }
   else
      learn_from_failure( ch, gsn_fourth_attack );

   chance = IS_NPC( ch ) ? ch->level : ( int )( ( LEARNED( ch, gsn_fifth_attack ) + ( dual_bonus * 3 ) ) );
   if( number_percent(  ) < chance )
   {
      learn_from_success( ch, gsn_fifth_attack );
      totalHits++;
      retcode = one_hit( ch, victim, dt );
      if( retcode != rNONE || who_fighting( ch ) != victim )
      {
         if( victim->position != POS_DEAD )
         {
            dt = multi_hitCheck( dt, ch );
            dam_message( ch, victim, totalDamage, dt, totalHits );
            resetTotals(  );
         }
         return retcode;
      }
   }
   else
      learn_from_failure( ch, gsn_fifth_attack );

   chance = IS_NPC( ch ) ? ch->level : ( int )( ( LEARNED( ch, gsn_sixth_attack ) + ( dual_bonus * 4 ) ) );
   if( number_percent(  ) < chance )
   {
      learn_from_success( ch, gsn_sixth_attack );
      totalHits++;
      retcode = one_hit( ch, victim, dt );
      if( retcode != rNONE || who_fighting( ch ) != victim )
      {
         if( victim->position != POS_DEAD )
         {
            dt = multi_hitCheck( dt, ch );
            dam_message( ch, victim, totalDamage, dt, totalHits );
            resetTotals(  );
         }
         return retcode;
      }
   }
   else
      learn_from_failure( ch, gsn_sixth_attack );

   retcode = rNONE;

   chance = IS_NPC( ch ) ? ( int )( ch->level / 2 ) : 0;
   if( number_percent(  ) < chance )
      retcode = one_hit( ch, victim, dt );
   totalHits++;

   if( retcode == rNONE )
   {
   }

   if( victim->position != POS_DEAD )
   {
      dt = multi_hitCheck( dt, ch );
      dam_message( ch, victim, totalDamage, dt, totalHits );
      resetTotals(  );
   }

   return retcode;
}

void resetTotals(  )
{
   totalDamage = 0;
   totalHits = 0;
}

int weapon_prof_bonus_check( CHAR_DATA * ch, OBJ_DATA * wield, int *gsn_ptr )
{
   int bonus;

   bonus = 0;
   *gsn_ptr = gsn_staves;

   if( !IS_NPC( ch ) && wield )
   {
      switch ( wield->value[4] )
      {
         default:
            *gsn_ptr = -1;
            break;
         case WEP_BAREHAND:
            *gsn_ptr = gsn_staves;
            break;
         case WEP_SWORD:
            *gsn_ptr = gsn_swords;
            break;
         case WEP_KATANA:
            *gsn_ptr = gsn_katana;
            break;
         case WEP_DAGGER:
            *gsn_ptr = gsn_daggers;
            break;
         case WEP_WHIP:
            *gsn_ptr = gsn_whips;
            break;
         case WEP_TALON:
            *gsn_ptr = gsn_daggers;
            break;
         case WEP_MACE:
            *gsn_ptr = gsn_maces_hammers;
            break;
         case WEP_ARCHERY:
            *gsn_ptr = gsn_archery;
            break;
         case WEP_BLOWGUN:
            *gsn_ptr = gsn_blowguns;
            break;
         case WEP_SLING:
            *gsn_ptr = gsn_slings;
            break;
         case WEP_AXE:
            *gsn_ptr = gsn_axes;
            break;
         case WEP_SPEAR:
            *gsn_ptr = gsn_spears;
            break;
         case WEP_STAFF:
            *gsn_ptr = gsn_staves;
            break;
         case WEP_ENERGY:
            *gsn_ptr = gsn_energy;
            break;
      }
      if( *gsn_ptr != -1 )
         bonus = ( int )( ( LEARNED( ch, *gsn_ptr ) - 50 ) / 10 );

      if( IS_DEVOTED( ch ) )
         bonus -= ch->pcdata->favor / -400;
   }
   return bonus;
}

int obj_hitroll( OBJ_DATA * obj )
{
   int tohit = 0;
   AFFECT_DATA *paf;

   for( paf = obj->pIndexData->first_affect; paf; paf = paf->next )
      if( paf->location == APPLY_HITROLL )
         tohit += paf->modifier;
   for( paf = obj->first_affect; paf; paf = paf->next )
      if( paf->location == APPLY_HITROLL )
         tohit += paf->modifier;
   return tohit;
}

sh_int off_shld_lvl( CHAR_DATA * ch, CHAR_DATA * victim )
{
   sh_int lvl;

   if( !IS_NPC( ch ) )
   {
      lvl = UMAX( 1, ( ch->level - 10 ) / 2 );
      if( number_percent(  ) + ( victim->level - lvl ) < 40 )
      {
         if( CAN_PKILL( ch ) && CAN_PKILL( victim ) )
            return ch->level;
         else
            return lvl;
      }
      else
         return 0;
   }
   else
   {
      lvl = ch->level / 2;
      if( number_percent(  ) + ( victim->level - lvl ) < 70 )
         return lvl;
      else
         return 0;
   }
}

ch_ret one_hit( CHAR_DATA * ch, CHAR_DATA * victim, int dt )
{
   OBJ_DATA *wield;
   int victim_ac;
   int thac0;
   int thac0_00;
   int thac0_32;
   int plusris;
   int dam;
   int diceroll;
   int attacktype, cnt;
   int prof_bonus;
   int prof_gsn = -1;
   ch_ret retcode = rNONE;
   static bool dual_flip = FALSE;

   if( victim->position == POS_DEAD || ch->in_room != victim->in_room )
      return rVICT_DIED;

   used_weapon = NULL;
   if( ( wield = get_eq_char( ch, WEAR_DUAL_WIELD ) ) != NULL )
   {
      if( dual_flip == FALSE )
      {
         dual_flip = TRUE;
         wield = get_eq_char( ch, WEAR_WIELD );
      }
      else
         dual_flip = FALSE;
   }
   else
      wield = get_eq_char( ch, WEAR_WIELD );

   used_weapon = wield;

   if( wield )
      prof_bonus = weapon_prof_bonus_check( ch, wield, &prof_gsn );
   else
      prof_bonus = 0;

   if( ch->fighting && dt == TYPE_UNDEFINED && IS_NPC( ch ) && !xIS_EMPTY( ch->attacks ) )
   {
      cnt = 0;
      for( ;; )
      {
         attacktype = number_range( 0, 6 );
         if( xIS_SET( ch->attacks, attacktype ) )
            break;
         if( cnt++ > 16 )
         {
            attacktype = -1;
            break;
         }
      }
      if( attacktype == ATCK_BACKSTAB )
         attacktype = -1;
      if( wield && number_percent(  ) > 25 )
         attacktype = -1;
      if( !wield && number_percent(  ) > 50 )
         attacktype = -1;

      switch ( attacktype )
      {
         default:
            break;
         case ATCK_BITE:
            do_bite( ch, "" );
            retcode = global_retcode;
            break;
         case ATCK_CLAWS:
            do_claw( ch, "" );
            retcode = global_retcode;
            break;
         case ATCK_TAIL:
            do_tail( ch, "" );
            retcode = global_retcode;
            break;
         case ATCK_STING:
            do_sting( ch, "" );
            retcode = global_retcode;
            break;
         case ATCK_PUNCH:
            do_punch( ch, "" );
            retcode = global_retcode;
            break;
         case ATCK_KICK:
            do_kick( ch, "" );
            retcode = global_retcode;
            break;
         case ATCK_TRIP:
            attacktype = 0;
            break;
      }
      if( attacktype >= 0 )
         return retcode;
   }

   if( dt == TYPE_UNDEFINED )
   {
      dt = TYPE_HIT;
      if( wield && wield->item_type == ITEM_WEAPON )
         dt += wield->value[3];
   }

   if( IS_NPC( ch ) )
   {
      thac0_00 = ch->mobthac0;
      thac0_32 = -45;
      thac0 = interpolate( ch->level, thac0_00, thac0_32 );
   }
   else
   {
      thac0_00 = class_table[ch->class]->thac0_00;
      thac0_32 = class_table[ch->class]->thac0_32;
      thac0 = interpolate( ch->level, thac0_00, thac0_32 );
   }

   if( GET_HITROLL( ch ) > 19999 )
   {
      if( ch->level < 100 )
      {
         thac0 = -20;
      }
      else if( ch->level == 100 )
      {
         thac0 = -10;
      }
      else if( ch->level == 200 )
      {
         thac0 = -10;
      }
      else if( ch->level == 300 )
      {
         thac0 = -5;
      }
      else if( ch->level >= 400 )
      {
         thac0 = -5;
      }
   }
   else
   {
      thac0 -= GET_HITROLL( ch );
   }

   if( !IS_NPC( ch ) && ch->level < 10 )
      thac0 /= 5.0;
   if( !IS_NPC( ch ) && ch->level > 9 && ch->level < 100 )
      thac0 /= 3.0;

   victim_ac = UMAX( -19, ( int )( GET_AC( victim ) / 10 ) );

   if( wield && !can_see_obj( victim, wield ) )
      victim_ac += 1;
   if( !can_see( ch, victim ) )
      victim_ac -= 4;

   if( ch->fighting && ch->fighting->who == victim )
   {
      sh_int times = ch->fighting->timeskilled;

      if( times )
      {
         sh_int intdiff = get_curr_int( ch ) - get_curr_int( victim );

         if( intdiff != 0 )
            victim_ac += ( intdiff * times ) / 10;
      }
   }

   victim_ac += prof_bonus;

   while( ( diceroll = number_bits( 5 ) ) >= 20 )
      ;

   if( diceroll == 0 || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
   {
      if( prof_gsn != -1 )
         learn_from_failure( ch, prof_gsn );
      damage( ch, victim, 0, dt );
      tail_chain(  );
      return rNONE;
   }


   if( !wield )
      dam = number_range( ch->barenumdie, ch->baresizedie * ch->barenumdie );
   else
      dam = number_range( wield->value[1], wield->value[2] );

   if( diceroll == 19 )
   {
      act( AT_FIRE, "You critically hit $N!", ch, NULL, victim, TO_CHAR );
      act( AT_FIRE, "$n critically hits you!", ch, NULL, victim, TO_VICT );
      act( AT_FIRE, "$n critically hits $N!", ch, NULL, victim, TO_NOTVICT );
      dam = dam * 2;
   }

   dam += GET_DAMROLL( ch );

   if( prof_bonus )
      dam += prof_bonus / 4;

   if( victim->position == POS_BERSERK )
      dam = 1.2 * dam;
   else if( victim->position == POS_AGGRESSIVE )
      dam = 1.1 * dam;
   else if( victim->position == POS_DEFENSIVE )
      dam = .85 * dam;
   else if( victim->position == POS_EVASIVE )
      dam = .8 * dam;

   if( ch->position == POS_BERSERK )
      dam = 1.2 * dam;
   else if( ch->position == POS_AGGRESSIVE )
      dam = 1.1 * dam;
   else if( ch->position == POS_DEFENSIVE )
      dam = .85 * dam;
   else if( ch->position == POS_EVASIVE )
      dam = .8 * dam;

   if( !IS_NPC( ch ) && ch->level < 10 )
      dam = 6 * dam;
   if( !IS_NPC( ch ) && ch->level > 9 && ch->level < 100 )
      dam = 3.5 * dam;


   if( !IS_NPC( ch ) && ( ch->race == 20 || ch->race == 15 || ch->race == 21 || ch->race == 22 ) )
   {
      dam = 1.6 * dam;
   }

   if( !IS_NPC( ch ) && ch->pcdata->learned[gsn_enhanced_damage] > 0 )
   {
      dam += ( int )( dam * LEARNED( ch, gsn_enhanced_damage ) / 120 );
      learn_from_success( ch, gsn_enhanced_damage );
   }

   if( !IS_AWAKE( victim ) )
      dam *= 2;
   if( dt == gsn_assassinate )
      dam *= ( 2 + URANGE( 2, ch->level - ( victim->level / 2 ), 30 ) / 4 );

   if( dt == gsn_backstab )
      dam *= ( 2 + URANGE( 2, ch->level - ( victim->level / 4 ), 30 ) / 8 );

   if( dt == gsn_circle )
      dam *= ( 2 + URANGE( 2, ch->level - ( victim->level / 4 ), 30 ) / 16 );

   if( dam <= 0 )
      dam = 1;

   plusris = 0;

   if( wield )
   {
      if( IS_OBJ_STAT( wield, ITEM_MAGIC ) )
         dam = ris_damage( victim, dam, RIS_MAGIC );
      else
         dam = ris_damage( victim, dam, RIS_NONMAGIC );

      plusris = obj_hitroll( wield );
   }
   else
      dam = ris_damage( victim, dam, RIS_NONMAGIC );

   if( dam )
   {
      int x, res, imm, sus, mod;

      if( plusris )
         plusris = RIS_PLUS1 << UMIN( plusris, 7 );

      imm = res = -1;
      sus = 1;

      for( x = RIS_PLUS1; x <= RIS_PLUS6; x <<= 1 )
      {
         if( IS_SET( victim->immune, x ) )
            imm = x;
         if( IS_SET( victim->resistant, x ) )
            res = x;
         if( IS_SET( victim->susceptible, x ) )
            sus = x;
      }
      mod = 10;
      if( imm >= plusris )
         mod -= 10;
      if( res >= plusris )
         mod -= 2;
      if( sus <= plusris )
         mod += 2;

      if( mod <= 0 )
         dam = -1;
      if( mod != 10 )
         dam = ( dam * mod ) / 10;
   }

   if( prof_gsn != -1 )
   {
      if( dam > 0 )
         learn_from_success( ch, prof_gsn );
      else
         learn_from_failure( ch, prof_gsn );
   }

   if( dam == -1 )
   {
      if( dt >= 0 && dt < top_sn )
      {
         SKILLTYPE *skill = skill_table[dt];
         bool found = FALSE;

         if( skill->imm_char && skill->imm_char[0] != '\0' )
         {
            act( AT_HIT, skill->imm_char, ch, NULL, victim, TO_CHAR );
            found = TRUE;
         }
         if( skill->imm_vict && skill->imm_vict[0] != '\0' )
         {
            act( AT_HITME, skill->imm_vict, ch, NULL, victim, TO_VICT );
            found = TRUE;
         }
         if( skill->imm_room && skill->imm_room[0] != '\0' )
         {
            act( AT_ACTION, skill->imm_room, ch, NULL, victim, TO_NOTVICT );
            found = TRUE;
         }
         if( found )
            return rNONE;
      }
      dam = 0;
   }
   if( ( retcode = damage( ch, victim, dam, dt ) ) != rNONE )
      return retcode;
   if( char_died( ch ) )
      return rCHAR_DIED;
   if( char_died( victim ) )
      return rVICT_DIED;

   retcode = rNONE;
   if( dam == 0 )
      return retcode;

   if( wield && !IS_SET( victim->immune, RIS_MAGIC ) && !xIS_SET( victim->in_room->room_flags, ROOM_NO_MAGIC ) )
   {
      AFFECT_DATA *aff;

      for( aff = wield->pIndexData->first_affect; aff; aff = aff->next )
         if( aff->location == APPLY_WEAPONSPELL && IS_VALID_SN( aff->modifier ) && skill_table[aff->modifier]->spell_fun )
            retcode = ( *skill_table[aff->modifier]->spell_fun ) ( aff->modifier, ( wield->level + 3 ) / 3, ch, victim );
      if( retcode != rNONE || char_died( ch ) || char_died( victim ) )
         return retcode;
      for( aff = wield->first_affect; aff; aff = aff->next )
         if( aff->location == APPLY_WEAPONSPELL && IS_VALID_SN( aff->modifier ) && skill_table[aff->modifier]->spell_fun )
            retcode = ( *skill_table[aff->modifier]->spell_fun ) ( aff->modifier, ( wield->level + 3 ) / 3, ch, victim );
      if( retcode != rNONE || char_died( ch ) || char_died( victim ) )
         return retcode;
   }

   if( IS_AFFECTED( victim, AFF_FIRESHIELD ) && !IS_AFFECTED( ch, AFF_FIRESHIELD ) )
      retcode = spell_acadia( skill_lookup( "flare" ), off_shld_lvl( victim, ch ), victim, ch );
   if( retcode != rNONE || char_died( ch ) || char_died( victim ) )
      return retcode;

   if( IS_AFFECTED( victim, AFF_ICESHIELD ) && !IS_AFFECTED( ch, AFF_ICESHIELD ) )
      retcode = spell_acadia( skill_lookup( "iceshard" ), off_shld_lvl( victim, ch ), victim, ch );
   if( retcode != rNONE || char_died( ch ) || char_died( victim ) )
      return retcode;

   if( IS_AFFECTED( victim, AFF_SHOCKSHIELD ) && !IS_AFFECTED( ch, AFF_SHOCKSHIELD ) )
      retcode = spell_acadia( skill_lookup( "torrent" ), off_shld_lvl( victim, ch ), victim, ch );
   if( retcode != rNONE || char_died( ch ) || char_died( victim ) )
      return retcode;

   if( IS_AFFECTED( victim, AFF_ACIDMIST ) && !IS_AFFECTED( ch, AFF_ACIDMIST ) )
      retcode = spell_acadia( skill_lookup( "acidshot" ), off_shld_lvl( victim, ch ), victim, ch );
   if( retcode != rNONE || char_died( ch ) || char_died( victim ) )
      return retcode;

   if( IS_AFFECTED( victim, AFF_VENOMSHIELD ) && !IS_AFFECTED( ch, AFF_VENOMSHIELD ) )
      retcode = spell_acadia( skill_lookup( "venomshot" ), off_shld_lvl( victim, ch ), victim, ch );
   if( retcode != rNONE || char_died( ch ) || char_died( victim ) )
      return retcode;

   tail_chain(  );
   return retcode;
}


sh_int ris_damage( CHAR_DATA * ch, sh_int dam, int ris )
{
   sh_int modifier;

   modifier = 10;
   if( IS_SET( ch->immune, ris ) && !IS_SET( ch->no_immune, ris ) )
      modifier -= 10;
   if( IS_SET( ch->resistant, ris ) && !IS_SET( ch->no_resistant, ris ) )
      modifier -= 2;
   if( IS_SET( ch->susceptible, ris ) && !IS_SET( ch->no_susceptible, ris ) )
   {
      if( IS_NPC( ch ) && IS_SET( ch->immune, ris ) )
         modifier += 0;
      else
         modifier += 2;
   }
   if( modifier <= 0 )
      return -1;
   if( modifier == 10 )
      return dam;
   return ( dam * modifier ) / 10;
}

ch_ret damage( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt )
{
   ROOM_INDEX_DATA *location;

   char buf[MAX_STRING_LENGTH];
   char buf1[MAX_STRING_LENGTH];

   char buf2[MAX_STRING_LENGTH];

   char filename[256];
   bool npcvict;
   bool loot;
   ch_ret retcode;
   sh_int dampmod;
   CHAR_DATA *gch;
   int init_gold, new_gold, gold_diff;
/*	int init_silver, new_silver, silver_diff;

	int init_copper, new_copper, copper_diff;*/

   sh_int anopc = 0;
   sh_int bnopc = 0;

   int bgold = 0;
   AFFECT_DATA af;

   int dmvpls;
   int silvdam;

   retcode = rNONE;

   if( !ch )
   {
      bug( "Damage: null ch!", 0 );
      return rERROR;
   }
   if( !victim )
   {
      bug( "Damage: null victim!", 0 );
      return rVICT_DIED;
   }

   if( victim->position == POS_DEAD )
      return rVICT_DIED;

   npcvict = IS_NPC( victim );

   if( dam && dt != TYPE_UNDEFINED )
   {
      if( IS_FIRE( dt ) )
         dam = ris_damage( victim, dam, RIS_FIRE );
      else if( IS_COLD( dt ) )
         dam = ris_damage( victim, dam, RIS_COLD );
      else if( IS_ACID( dt ) )
         dam = ris_damage( victim, dam, RIS_ACID );
      else if( IS_ELECTRICITY( dt ) )
         dam = ris_damage( victim, dam, RIS_ELECTRICITY );
      else if( IS_ENERGY( dt ) )
         dam = ris_damage( victim, dam, RIS_ENERGY );
      else if( IS_DRAIN( dt ) )
         dam = ris_damage( victim, dam, RIS_DRAIN );
      else if( dt == gsn_poison || IS_POISON( dt ) )
         dam = ris_damage( victim, dam, RIS_POISON );
      else if( dt == ( TYPE_HIT + DAM_CRUSH ) )
         dam = ris_damage( victim, dam, RIS_BLUNT );
      else if( dt == ( TYPE_HIT + DAM_STAB ) || dt == ( TYPE_HIT + DAM_PIERCE ) || dt == ( TYPE_HIT + DAM_THRUST ) )
         dam = ris_damage( victim, dam, RIS_PIERCE );
      else if( dt == ( TYPE_HIT + DAM_SLASH ) )
         dam = ris_damage( victim, dam, RIS_SLASH );
      else if( dt == ( TYPE_HIT + DAM_HACK ) )
         dam = ris_damage( victim, dam, RIS_HACK );
      else if( dt == ( TYPE_HIT + DAM_LASH ) )
         dam = ris_damage( victim, dam, RIS_LASH );

      if( dam == -1 )
      {
         if( dt >= 0 && dt < top_sn )
         {
            bool found = FALSE;
            SKILLTYPE *skill = skill_table[dt];

            if( skill->imm_char && skill->imm_char[0] != '\0' )
            {
               act( AT_HIT, skill->imm_char, ch, NULL, victim, TO_CHAR );
               found = TRUE;
            }
            if( skill->imm_vict && skill->imm_vict[0] != '\0' )
            {
               act( AT_HITME, skill->imm_vict, ch, NULL, victim, TO_VICT );
               found = TRUE;
            }
            if( skill->imm_room && skill->imm_room[0] != '\0' )
            {
               act( AT_ACTION, skill->imm_room, ch, NULL, victim, TO_NOTVICT );
               found = TRUE;
            }
            if( found )
               return rNONE;
         }
         dam = 0;
      }
   }

   if( xIS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
      dam = 0;

   if( dam && npcvict && ch != victim )
   {
      if( !xIS_SET( victim->act, ACT_SENTINEL ) )
      {
      }

      if( victim->hating )
      {
         if( victim->hating->who != ch )
         {
            STRFREE( victim->hating->name );
            victim->hating->name = QUICKLINK( ch->name );
            victim->hating->who = ch;
         }
      }
      else if( !xIS_SET( victim->act, ACT_PACIFIST ) )
         start_hating( victim, ch );
   }

   if( victim != ch )
   {
      if( is_safe( ch, victim ) )
         return rNONE;
      check_attacker( ch, victim );

      if( victim->position > POS_STUNNED )
      {
         if( !victim->fighting && victim->in_room == ch->in_room )
            set_fighting( victim, ch );

         if( IS_NPC( victim ) && victim->fighting )
            victim->position = POS_FIGHTING;
         else if( victim->fighting )
         {
            switch ( victim->style )
            {
               case ( STYLE_EVASIVE ):
                  victim->position = POS_EVASIVE;
                  break;
               case ( STYLE_DEFENSIVE ):
                  victim->position = POS_DEFENSIVE;
                  break;
               case ( STYLE_AGGRESSIVE ):
                  victim->position = POS_AGGRESSIVE;
                  break;
               case ( STYLE_BERSERK ):
                  victim->position = POS_BERSERK;
                  break;
               default:
                  victim->position = POS_FIGHTING;
            }
         }
      }

      if( victim->position > POS_STUNNED )
      {
         if( !ch->fighting && victim->in_room == ch->in_room )
            set_fighting( ch, victim );

         if( IS_NPC( ch )
             && npcvict
             && IS_AFFECTED( victim, AFF_CHARM )
             && IS_AFFECTED( victim, AFF_JMT )
             && victim->master && victim->master->in_room == ch->in_room && number_bits( 3 ) == 0 )
         {
            stop_fighting( ch, FALSE );
            retcode = multi_hit( ch, victim->master, TYPE_UNDEFINED );
            return retcode;
         }
      }

      if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_PLAGUE ) )
      {
         if( number_percent(  ) < 5 )
         {
            if( !IS_AFFECTED( victim, AFF_POISON ) )
            {
               af.type = gsn_plague;
               af.duration = 83;
               af.location = APPLY_STR;
               af.modifier = -5;
               af.bitvector = meb( AFF_PLAGUE );
               affect_join( victim, &af );
               af.type = gsn_plague;
               af.duration = 83;
               af.location = APPLY_CON;
               af.modifier = -5;
               af.bitvector = meb( AFF_PLAGUE );
               affect_join( victim, &af );
               set_char_color( AT_GREEN, victim );
               send_to_char( "You feel very sick.\n\r", victim );
               if( ch != victim )
               {
                  act( AT_GREEN, "$N shivers as your plague rips $S body.", ch, NULL, victim, TO_CHAR );
                  act( AT_GREEN, "$N shivers as $n's plague rips $S body.", ch, NULL, victim, TO_NOTVICT );
               }
            }
         }
      }
      if( victim->master == ch )
         stop_follower( victim );

      for( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
         if( is_same_group( ch, gch ) && !IS_NPC( gch ) && !IS_PKILL( gch ) && ( ch != gch ) )
            anopc++;

      for( gch = victim->in_room->first_person; gch; gch = gch->next_in_room )
         if( is_same_group( victim, gch ) && !IS_NPC( gch ) && !IS_PKILL( gch ) && ( victim != gch ) )
            bnopc++;

      if( ( bnopc > 0 && anopc > 0 ) || ( bnopc > 0 && !IS_NPC( ch ) ) || ( anopc > 0 && !IS_NPC( victim ) ) )
      {
         if( is_same_group( ch, victim ) )
         {
            act( AT_ACTION, "$n disbands from $N's group.",
                 ( ch->leader == victim ) ? victim : ch, NULL,
                 ( ch->leader == victim ) ? victim->master : ch->master, TO_NOTVICT );
            if( ch->leader == victim )
               stop_follower( victim );
            else
               stop_follower( ch );
         }
         if( ch->leader != NULL && !IS_NPC( ch->leader ) && !IS_PKILL( ch->leader ) )
         {
            act( AT_ACTION, "$n disbands from $N's group.", ch, NULL, ch->master, TO_NOTVICT );
            stop_follower( ch );
         }
         else
         {
            for( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
               if( is_same_group( gch, ch ) && !IS_NPC( gch ) && !IS_PKILL( gch ) && gch != ch )
               {
                  act( AT_ACTION, "$n disbands from $N's group.", ch, NULL, gch->master, TO_NOTVICT );
                  stop_follower( gch );
               }
         }
         if( victim->leader != NULL && !IS_NPC( victim->leader ) && !IS_PKILL( victim->leader ) )
         {
            act( AT_ACTION, "$n disbands from $N's group.", victim, NULL, victim->master, TO_NOTVICT );
            stop_follower( victim );
         }
         else
         {
            for( gch = victim->in_room->first_person; gch; gch = gch->next_in_room )
               if( is_same_group( gch, victim ) && !IS_NPC( gch ) && !IS_PKILL( gch ) && gch != victim )
               {
                  act( AT_ACTION, "$n disbands from $N's group.", gch, NULL, gch->master, TO_NOTVICT );
                  stop_follower( gch );
               }
         }
      }

      if( IS_AFFECTED( ch, AFF_INVISIBLE ) )
      {
         affect_strip( ch, gsn_invis );
         affect_strip( ch, gsn_mass_invis );
         xREMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
         act( AT_MAGIC, "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
      }

      if( IS_AFFECTED( ch, AFF_HIDE ) )
         xREMOVE_BIT( ch->affected_by, AFF_HIDE );

      if( IS_AFFECTED( victim, AFF_DEMONFIRE ) )
         dam *= 2;

      if( IS_AFFECTED( victim, AFF_DEMI ) )
         dam *= 1.2;

      if( IS_AFFECTED( victim, AFF_SANCTUARY ) )
         dam /= 2;


      if( IS_AFFECTED( victim, AFF_PROTECT ) && IS_EVIL( ch ) )
         dam -= ( int )( dam / 4 );

      if( dam < 0 )
         dam = 0;

      if( dt >= TYPE_HIT && ch->in_room == victim->in_room )
      {
         if( IS_NPC( ch ) && xIS_SET( ch->defenses, DFND_DISARM ) && ch->level > 9 && number_percent(  ) < ch->level / 3 )
            disarm( ch, victim );

         if( IS_NPC( ch ) && xIS_SET( ch->attacks, ATCK_TRIP ) && ch->level > 5 && number_percent(  ) < ch->level / 2 )
            trip( ch, victim );

         if( check_steel_skin( ch, victim ) )
            return FALSE;
         if( check_parry( ch, victim ) )
            return rNONE;
         if( check_dodge( ch, victim ) )
            return rNONE;
         if( check_tumble( ch, victim ) )
            return rNONE;
         if( check_guard( ch, victim ) )
            return rNONE;
      }

      if( IS_NPC( ch ) )
      {
         if( npcvict )
            dampmod = sysdata.dam_mob_vs_mob;
         else
            dampmod = sysdata.dam_mob_vs_plr;
      }
      else
      {
         if( npcvict )
            dampmod = sysdata.dam_plr_vs_mob;
         else
            dampmod = sysdata.dam_plr_vs_plr;
      }

      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         dmvpls = ( ( dam * ch->pcdata->deity->dampls ) / 100 );
         dam += dmvpls;
      }

      if( !IS_NPC( victim ) && LEARNED( victim, gsn_dodge_craft ) )
      {
         dam -= ( dam * .3 );
      }

      if( ( victim->class == CLASS_WEREWOLF || victim->class == CLASS_VAMPIRE ) && ( is_wsilver( ch ) ) )
      {
         silvdam = ( ( dam * 0.20 ) / 100 );
         dam += silvdam;
      }

      if( dampmod > 0 )
      {
         dam = ( dam * dampmod ) / 100;
      }

      totalDamage = totalDamage + dam;
      dam_message( ch, victim, dam, dt, 0 );
   }
   victim->hit -= dam;

   if( !IS_NPC( victim ) && victim->level >= LEVEL_IMMORTAL && victim->hit < 1 )
      victim->hit = 1;

   if( !IS_NPC( victim ) && NOT_AUTHED( victim ) && victim->hit < 1 )
      victim->hit = 1;

   if( dam > 0 && dt > TYPE_HIT
       && is_wielding_poisoned( ch )
       && !IS_AFFECTED( victim, AFF_POISON )
       && !IS_SET( victim->immune, RIS_POISON ) && !saves_poison_death( ch->level, victim ) )
   {
      af.type = gsn_poison;
      af.duration = 20;
      af.location = APPLY_STR;
      af.modifier = -2;
      af.bitvector = meb( AFF_POISON );
      affect_join( victim, &af );
   }

   if( IS_VAMPIRE( victim ) )
   {
      if( dam >= ( victim->max_hit / 10 ) )
         victim->blood -= ( 1 - ( victim->level / 20 ) );
      if( victim->hit <= ( victim->max_hit / 8 ) && victim->blood > 5 )
      {
         victim->blood -= URANGE( 3, victim->level / 10, 8 );
         victim->hit += URANGE( 4, ( victim->max_hit / 30 ), 15 );
         set_char_color( AT_BLOOD, victim );
         send_to_char( "You howl with rage as the beast within stirs!\n\r", victim );
      }
   }

   if( !npcvict && get_trust( victim ) >= LEVEL_IMMORTAL && get_trust( ch ) >= LEVEL_IMMORTAL && victim->hit < 1 )
      victim->hit = 1;
   update_pos( victim );

   switch ( victim->position )
   {
      case POS_MORTAL:
         act( AT_DYING, "$n is mortally wounded, and will die soon, if not aided.", victim, NULL, NULL, TO_ROOM );
         act( AT_DANGER, "}RYou are mortally wounded, and will die soon, if not aided.&D", victim, NULL, NULL, TO_CHAR );
         break;

      case POS_INCAP:
         act( AT_DYING, "$n is incapacitated and will slowly die, if not aided.", victim, NULL, NULL, TO_ROOM );
         act( AT_DANGER, "}RYou are incapacitated and will slowly die, if not aided.&D", victim, NULL, NULL, TO_CHAR );
         break;

      case POS_STUNNED:
         if( !IS_AFFECTED( victim, AFF_PARALYSIS ) )
         {
            act( AT_ACTION, "$n is stunned, but will probably recover.", victim, NULL, NULL, TO_ROOM );
            act( AT_HURT, "You are stunned, but will probably recover.", victim, NULL, NULL, TO_CHAR );
         }
         break;

      case POS_DEAD:
         if( dt >= 0 && dt < top_sn )
         {
            SKILLTYPE *skill = skill_table[dt];

            if( skill->die_char && skill->die_char[0] != '\0' )
               act( AT_DEAD, skill->die_char, ch, NULL, victim, TO_CHAR );
            if( skill->die_vict && skill->die_vict[0] != '\0' )
               act( AT_DEAD, skill->die_vict, ch, NULL, victim, TO_VICT );
            if( skill->die_room && skill->die_room[0] != '\0' )
               act( AT_DEAD, skill->die_room, ch, NULL, victim, TO_NOTVICT );
         }
         if( !IS_NPC( victim ) )
         {
            sprintf( buf, "&R<&BDEATH INFO&R> %s has just been killed by %s, in %s (%d).&D", victim->name,
                     ( IS_NPC( ch ) ? ch->short_descr : ch->name ), victim->in_room->name, victim->in_room->vnum );
            talk_info( AT_PLAIN, buf );
         }
         dam_message( ch, victim, totalDamage, dt, -1 );
         resetTotals(  );
         act( AT_DEAD, "$n is DEAD!!", victim, 0, 0, TO_ROOM );
         act( AT_DEAD, "You have been KILLED!!\n\r", victim, 0, 0, TO_CHAR );
         break;

      default:
         if( dam > victim->max_hit / 4 )
         {
            act( AT_HURT, "That really did HURT!", victim, 0, 0, TO_CHAR );
         }
         if( victim->hit < victim->max_hit / 4 )
         {
            act( AT_DANGER, "}RYou wish that your wounds would stop BLEEDING so much!&D", victim, 0, 0, TO_CHAR );
         }
         break;
   }

   if( !IS_AWAKE( victim ) && !IS_AFFECTED( victim, AFF_PARALYSIS ) )
   {
      if( victim->fighting && victim->fighting->who->hunting && victim->fighting->who->hunting->who == victim )
         stop_hunting( victim->fighting->who );

      if( victim->fighting && victim->fighting->who->hating && victim->fighting->who->hating->who == victim )
         stop_hating( victim->fighting->who );

      if( !npcvict && IS_NPC( ch ) )
         stop_fighting( victim, TRUE );
      else
         stop_fighting( victim, FALSE );
   }

   if( victim->position == POS_DEAD )
   {
      group_gain( ch, victim );

      if( !npcvict )
      {
         sprintf( log_buf, "%s (%d) killed by %s at %d",
                  victim->name, victim->level, ( IS_NPC( ch ) ? ch->short_descr : ch->name ), victim->in_room->vnum );
         log_string( log_buf );
         to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL, victim->level );

         if( !IS_NPC( ch ) && !IS_NPC( victim ) && IS_BOUNTIED( victim ) && victim != ch )
         {
            if( victim->pcdata->bountytype == 1 )
            {
               sprintf( buf2, "katyr" );
               if( ch->gold == MAX_GOLD )
               {
                  send_to_char( "&pYou are carrying the maximum limit of katyr so your bounty is forfeit.&D\n\r", ch );
                  bgold = 0;
               }
               else if( victim->pcdata->bounty + ch->gold > MAX_GOLD )
               {
                  ch_printf( ch, "&pYou can only take %d katyr due to the maximum katyr able to be carried.&D\n\r",
                             ( MAX_GOLD - ch->gold ) );
                  bgold = ( MAX_GOLD - ch->gold );
                  ch->gold = 99999999;
               }
               else
               {
                  bgold = victim->pcdata->bounty;
                  ch->gold += victim->pcdata->bounty;
               }
            }
            else if( victim->pcdata->bountytype == 2 )
            {
               sprintf( buf2, "experience" );
               bgold = victim->pcdata->bounty;
               gain_exp( ch, victim->pcdata->bounty );
            }
            else if( victim->pcdata->bountytype == 3 )
            {
               sprintf( buf2, "practices" );
               bgold = victim->pcdata->bounty;
               ch->practice += victim->pcdata->bounty;
            }
            sprintf( buf, "&WYou recive a &Y%d &W%s bounty, for killing %s.&D\n\r", bgold, buf2, victim->name );
            send_to_char( buf, ch );
            victim->pcdata->bounty = 0;
            victim->pcdata->bountytype = 0;
            xREMOVE_BIT( victim->act, PLR_BOUNTIED );
         }
         if( !IS_NPC( ch ) && !IS_IMMORTAL( ch ) && ch->pcdata->clan
             && ch->pcdata->clan->clan_type != CLAN_ORDER && ch->pcdata->clan->clan_type != CLAN_GUILD && victim != ch )
         {
            sprintf( filename, "%s%s.record", CLAN_DIR, ch->pcdata->clan->name );
            sprintf( log_buf, "&P(%2d) %-12s &wvs &P(%2d) %s &P%s ... &w%s",
                     ch->level,
                     ch->name,
                     victim->level,
                     !CAN_PKILL( victim ) ? "&W<Peaceful>" :
                     victim->pcdata->clan ? victim->pcdata->clan->badge :
                     "&P(&WUnclanned&P)", victim->name, ch->in_room->area->name );
            if( victim->pcdata->clan && victim->pcdata->clan->name == ch->pcdata->clan->name )
               ;
            else
               append_to_file( filename, log_buf );
         }

         if( victim->level < 100 )
         {
            if( victim->exp > exp_level( victim, victim->level ) )
            {
               gain_exp( victim, ( exp_level( victim, victim->level ) - victim->exp ) / 2 );
            }
         }
         else
         {
            if( victim->exp > exp_level( victim, victim->sublevel ) )
            {
               gain_exp( victim, ( exp_level( victim, victim->sublevel ) - victim->exp ) / 2 );
            }
         }

      }
      else if( !IS_NPC( ch ) )
         add_kill( ch, victim );

      check_killer( ch, victim );

      if( ch->in_room == victim->in_room )
         loot = legal_loot( ch, victim );
      else
         loot = FALSE;

      if( !IS_NPC( victim ) && IN_ARENA( victim ) && victim->hit < 1 )
      {
         location = get_room_index( ROOM_VNUM_ALTAR );
         victim->hit = 1;
         char_to_room( victim, location );
      }
      else
      {
         set_cur_char( victim );
         raw_kill( ch, victim, TRUE );
         victim = NULL;
      }

      if( !IS_NPC( ch ) && loot )
      {
         if( xIS_SET( ch->act, PLR_AUTOGOLD ) )
         {
            init_gold = ch->gold;
/*			init_silver = ch->silver;
			init_copper = ch->copper;*/
            do_get( ch, "'katyr' corpse" );
/*			do_get( ch, "'siam' corpse" );
			do_get( ch, "'rona' corpse" );*/
            new_gold = ch->gold;
/*			new_silver = ch->silver;
			new_copper = ch->copper;*/
            gold_diff = ( new_gold - init_gold );
/*			silver_diff = (new_silver - init_silver);
			copper_diff = (new_copper - init_copper);*/
            if( gold_diff > 0 )
            {
               sprintf( buf1, "%d katyr", gold_diff );
               do_split( ch, buf1 );
            }
/*			if (silver_diff > 0)
			{
				sprintf(buf1,"%d siam",silver_diff);
				do_split( ch, buf1 );
			}
			if (copper_diff > 0)
			{
				sprintf(buf1,"%d rona",copper_diff);
				do_split( ch, buf1 );
			} */
         }
         if( xIS_SET( ch->act, PLR_AUTOLOOT ) && victim != ch )
            do_get( ch, "all corpse" );
         else
            do_look( ch, "in corpse" );

         if( xIS_SET( ch->act, PLR_AUTOSAC ) )
            do_sacrifice( ch, "corpse" );
      }

      if( IS_SET( sysdata.save_flags, SV_KILL ) )
         save_char_obj( ch );
      return rVICT_DIED;
   }

   if( victim == ch )
      return rNONE;

   if( !npcvict && !victim->desc && !IS_SET( victim->pcdata->flags, PCFLAG_NORECALL ) )
   {
      if( number_range( 0, victim->wait ) == 0 )
      {
         do_recall( victim, "" );
         return rNONE;
      }
   }

   if( npcvict && dam > 0 )
   {
      if( ( xIS_SET( victim->act, ACT_WIMPY ) && number_bits( 1 ) == 0
            && victim->hit < victim->max_hit / 2 )
          || ( ( IS_AFFECTED( victim, AFF_CHARM ) || IS_AFFECTED( victim, AFF_JMT ) ) && victim->master
               && victim->master->in_room != victim->in_room ) )
      {
         start_fearing( victim, ch );
         stop_hunting( victim );
         do_flee( victim, "" );
      }
   }

   if( !npcvict && victim->hit > 0 && victim->hit <= victim->wimpy && victim->wait == 0 )
      do_flee( victim, "" );
   else if( !npcvict && xIS_SET( victim->act, PLR_FLEE ) )
      do_flee( victim, "" );

   tail_chain(  );
   return rNONE;
}

bool is_safe( CHAR_DATA * ch, CHAR_DATA * victim )
{
   if( char_died( victim ) || char_died( ch ) )
      return TRUE;

   if( who_fighting( ch ) == ch )
      return FALSE;

   if( !victim )
   {
      bug( "Is_safe: %s opponent does not exist!", ch->name );
      return TRUE;
   }
   if( !victim->in_room )
   {
      bug( "Is_safe: %s has no physical location!", victim->name );
      return TRUE;
   }
   if( ( xIS_SET( victim->in_room->room_flags, ROOM_SAFE ) || xIS_SET( victim->in_room->room_flags, ROOM_SAFETY ) )
       || IS_AFFECTED( ch, AFF_SHAPESHIFT ) || IS_AFFECTED( victim, AFF_SHAPESHIFT ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "A magical force prevents you from attacking.\n\r", ch );
      return TRUE;
   }

   if( ( IS_PACIFIST( ch ) ) && !IN_ARENA( ch ) )
   {
      set_char_color( AT_MAGIC, ch );
      ch_printf( ch, "You are a pacifist and will not fight.\n\r" );
      return TRUE;
   }

   if( IS_PACIFIST( victim ) )
   {
      char buf[MAX_STRING_LENGTH];
      sprintf( buf, "%s is a pacifist and will not fight.\n\r", capitalize( victim->short_descr ) );
      set_char_color( AT_MAGIC, ch );
      send_to_char( buf, ch );
      return TRUE;
   }

   if( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL )
      return FALSE;

   if( !IS_NPC( ch ) && !IS_NPC( victim ) && ch != victim && IS_SET( victim->in_room->area->flags, AFLAG_NOPKILL ) )
   {
      set_char_color( AT_IMMORT, ch );
      send_to_char( "The gods have forbidden player killing in this area.\n\r", ch );
      return TRUE;
   }

   if( IS_NPC( ch ) || IS_NPC( victim ) )
      return FALSE;

   if( ch->level < 5 && !IN_ARENA( ch ) )
   {
      set_char_color( AT_WHITE, ch );
      send_to_char( "You are not yet ready, needing age or experience, if not both. \n\r", ch );
      return TRUE;
   }

   if( victim->level < 5 && !IN_ARENA( victim ) )
   {
      set_char_color( AT_WHITE, ch );
      send_to_char( "They are yet too young to die.\n\r", ch );
      return TRUE;
   }

   if( ( ch->level - victim->level > 5 || victim->level - ch->level > 5 ) && !IN_ARENA( ch ) )
   {
      set_char_color( AT_IMMORT, ch );
      send_to_char( "The gods do not allow murder when there is such a difference in level.\n\r", ch );
      return TRUE;
   }

   if( ( get_timer( victim, TIMER_PKILLED ) > 0 ) && !IN_ARENA( victim ) )
   {
      set_char_color( AT_GREEN, ch );
      send_to_char( "That character has died within the last 5 minutes.\n\r", ch );
      return TRUE;
   }

   if( ( get_timer( ch, TIMER_PKILLED ) > 0 ) && !IN_ARENA( ch ) )
   {
      set_char_color( AT_GREEN, ch );
      send_to_char( "You have been killed within the last 5 minutes.\n\r", ch );
      return TRUE;
   }
   return FALSE;
}

bool legal_loot( CHAR_DATA * ch, CHAR_DATA * victim )
{
   if( IS_NPC( victim ) )
      return TRUE;
   if( IS_NPC( ch ) && !ch->master )
      return TRUE;
   if( !IS_NPC( ch ) && !IS_NPC( victim )
       && IS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) && IS_SET( victim->pcdata->flags, PCFLAG_DEADLY ) )
      return TRUE;
   return FALSE;
}

void check_killer( CHAR_DATA * ch, CHAR_DATA * victim )
{
   if( IS_NPC( victim ) )
   {
      if( !IS_NPC( ch ) )
      {
         int level_ratio;
         level_ratio = URANGE( 1, ch->level / victim->level, 50 );
         if( ch->pcdata->clan )
            ch->pcdata->clan->mkills++;
         ch->pcdata->mkills++;

         tkills++;
         ch->in_room->area->mkills++;
      }
      return;
   }

   if( ch == victim || ch->level >= LEVEL_IMMORTAL )
      return;

   if( IN_ARENA( ch ) )
   {
      if( !IS_NPC( ch ) && !IS_NPC( victim ) )
      {
         ch->pcdata->pkills++;

         akills++;
         victim->pcdata->pdeaths++;

         adeaths++;
      }
      return;
   }

   if( xIS_SET( victim->act, PLR_KILLER ) || xIS_SET( victim->act, PLR_THIEF ) )
   {
      if( !IS_NPC( ch ) )
      {
         if( ch->pcdata->clan )
         {
            if( victim->level < 10 )
               ch->pcdata->clan->pkills[0]++;
            else if( victim->level < 15 )
               ch->pcdata->clan->pkills[1]++;
            else if( victim->level < 20 )
               ch->pcdata->clan->pkills[2]++;
            else if( victim->level < 30 )
               ch->pcdata->clan->pkills[3]++;
            else if( victim->level < 40 )
               ch->pcdata->clan->pkills[4]++;
            else if( victim->level < 50 )
               ch->pcdata->clan->pkills[5]++;
            else
               ch->pcdata->clan->pkills[6]++;
         }
         ch->pcdata->pkills++;

         tpkills++;
         ch->in_room->area->pkills++;
      }
      return;
   }

   if( !IS_NPC( ch ) && !IS_NPC( victim )
       && IS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) && IS_SET( victim->pcdata->flags, PCFLAG_DEADLY ) )
   {
      if( !ch->pcdata->clan
          || !victim->pcdata->clan
          || ( ch->pcdata->clan->clan_type != CLAN_NOKILL
               && victim->pcdata->clan->clan_type != CLAN_NOKILL && ch->pcdata->clan != victim->pcdata->clan ) )
      {
         if( ch->pcdata->clan )
         {
            if( victim->level < 10 )
               ch->pcdata->clan->pkills[0]++;
            else if( victim->level < 15 )
               ch->pcdata->clan->pkills[1]++;
            else if( victim->level < 20 )
               ch->pcdata->clan->pkills[2]++;
            else if( victim->level < 30 )
               ch->pcdata->clan->pkills[3]++;
            else if( victim->level < 40 )
               ch->pcdata->clan->pkills[4]++;
            else if( victim->level < 50 )
               ch->pcdata->clan->pkills[5]++;
            else
               ch->pcdata->clan->pkills[6]++;
         }
         ch->pcdata->pkills++;

         tpkills++;
         ch->hit = ch->max_hit;
         ch->mana = ch->max_mana;
         ch->blood = ch->max_blood;
         ch->move = ch->max_move;
         update_pos( victim );
         if( victim != ch )
         {
            act( AT_MAGIC, "Bolts of blue energy rise from the corpse, seeping into $n.", ch, victim->name, NULL, TO_ROOM );
            act( AT_MAGIC, "Bolts of blue energy rise from the corpse, seeping into you.", ch, victim->name, NULL, TO_CHAR );
         }
         if( victim->pcdata->clan )
         {
            if( ch->level < 10 )
               victim->pcdata->clan->pdeaths[0]++;
            else if( ch->level < 15 )
               victim->pcdata->clan->pdeaths[1]++;
            else if( ch->level < 20 )
               victim->pcdata->clan->pdeaths[2]++;
            else if( ch->level < 30 )
               victim->pcdata->clan->pdeaths[3]++;
            else if( ch->level < 40 )
               victim->pcdata->clan->pdeaths[4]++;
            else if( ch->level < 50 )
               victim->pcdata->clan->pdeaths[5]++;
            else
               victim->pcdata->clan->pdeaths[6]++;
         }
         victim->pcdata->pdeaths++;

         tpdeaths++;
         add_timer( victim, TIMER_PKILLED, 115, NULL, 0 );
         return;
      }
   }

   if( ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      if( !ch->master )
      {
         char buf[MAX_STRING_LENGTH];

         sprintf( buf, "Check_killer: %s bad AFF_CHARM", IS_NPC( ch ) ? ch->short_descr : ch->name );
         bug( buf, 0 );
         affect_strip( ch, gsn_charm_person );
         affect_strip( ch, gsn_jmt );
         xREMOVE_BIT( ch->affected_by, AFF_CHARM );
         xREMOVE_BIT( ch->affected_by, AFF_JMT );
         return;
      }

      if( ch->master )
         check_killer( ch->master, victim );
      return;
   }

   if( IS_NPC( ch ) )
   {
      if( !IS_NPC( victim ) )
      {
         int level_ratio;
         if( victim->pcdata->clan )
            victim->pcdata->clan->mdeaths++;
         victim->pcdata->mdeaths++;

         tdeaths++;
         victim->in_room->area->mdeaths++;
         level_ratio = URANGE( 1, ch->level / victim->level, 50 );
      }
      return;
   }


   if( !IS_NPC( ch ) )
   {
      if( ch->pcdata->clan )
         ch->pcdata->clan->illegal_pk++;
      ch->pcdata->illegal_pk++;
      ch->in_room->area->illegal_pk++;
   }
   if( !IS_NPC( victim ) )
   {
      if( victim->pcdata->clan )
      {
         if( ch->level < 10 )
            victim->pcdata->clan->pdeaths[0]++;
         else if( ch->level < 15 )
            victim->pcdata->clan->pdeaths[1]++;
         else if( ch->level < 20 )
            victim->pcdata->clan->pdeaths[2]++;
         else if( ch->level < 30 )
            victim->pcdata->clan->pdeaths[3]++;
         else if( ch->level < 40 )
            victim->pcdata->clan->pdeaths[4]++;
         else if( ch->level < 50 )
            victim->pcdata->clan->pdeaths[5]++;
         else
            victim->pcdata->clan->pdeaths[6]++;
      }
      victim->pcdata->pdeaths++;

      tpdeaths++;
      victim->in_room->area->pdeaths++;
   }

   if( xIS_SET( ch->act, PLR_KILLER ) || IS_BOUNTIED( victim ) || IS_BOUNTIED( ch ) )
      return;

   set_char_color( AT_WHITE, ch );
   send_to_char( "A strange feeling grows deep inside you, and a tingle goes up your spine...\n\r", ch );
   set_char_color( AT_IMMORT, ch );
   send_to_char( "A deep voice booms inside your head, 'Thou shall now be known as a deadly murderer!!!'\n\r", ch );
   set_char_color( AT_WHITE, ch );
   send_to_char( "You feel as if your soul has been revealed for all to see.\n\r", ch );
   xSET_BIT( ch->act, PLR_KILLER );
   if( xIS_SET( ch->act, PLR_ATTACKER ) )
      xREMOVE_BIT( ch->act, PLR_ATTACKER );
   save_char_obj( ch );
   return;
}

void check_attacker( CHAR_DATA * ch, CHAR_DATA * victim )
{

   if( IS_NPC( victim ) || xIS_SET( victim->act, PLR_KILLER ) || xIS_SET( victim->act, PLR_THIEF ) )
      return;

   if( !IS_NPC( ch ) && !IS_NPC( victim ) && CAN_PKILL( ch ) && CAN_PKILL( victim ) )
      return;

   if( ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      if( !ch->master )
      {
         char buf[MAX_STRING_LENGTH];

         sprintf( buf, "Check_attacker: %s bad AFF_CHARM", IS_NPC( ch ) ? ch->short_descr : ch->name );
         bug( buf, 0 );
         affect_strip( ch, gsn_charm_person );
         xREMOVE_BIT( ch->affected_by, AFF_CHARM );
         xREMOVE_BIT( ch->affected_by, AFF_JMT );
         return;
      }
      return;
   }

   if( IS_NPC( ch )
       || ch == victim
       || ch->level >= LEVEL_IMMORTAL
       || xIS_SET( ch->act, PLR_ATTACKER ) || xIS_SET( ch->act, PLR_KILLER ) || IN_ARENA( ch ) )
      return;

   xSET_BIT( ch->act, PLR_ATTACKER );
   save_char_obj( ch );
   return;
}


void update_pos( CHAR_DATA * victim )
{
   if( !victim )
   {
      bug( "update_pos: null victim", 0 );
      return;
   }

   if( victim->hit > 0 )
   {
      if( victim->position <= POS_STUNNED )
         victim->position = POS_STANDING;
      if( IS_AFFECTED( victim, AFF_PARALYSIS ) )
         victim->position = POS_STUNNED;
      return;
   }

   if( IS_NPC( victim ) || victim->hit <= -11 )
   {
      if( victim->mount )
      {
         act( AT_ACTION, "$n falls from $N.", victim, NULL, victim->mount, TO_ROOM );
         xREMOVE_BIT( victim->mount->act, ACT_MOUNTED );
         victim->mount = NULL;
      }
      victim->position = POS_DEAD;
      return;
   }

   if( victim->hit <= -6 )
      victim->position = POS_MORTAL;
   else if( victim->hit <= -3 )
      victim->position = POS_INCAP;
   else
      victim->position = POS_STUNNED;

   if( victim->position > POS_STUNNED && IS_AFFECTED( victim, AFF_PARALYSIS ) )
      victim->position = POS_STUNNED;

   if( victim->mount )
   {
      act( AT_ACTION, "$n falls unconscious from $N.", victim, NULL, victim->mount, TO_ROOM );
      xREMOVE_BIT( victim->mount->act, ACT_MOUNTED );
      victim->mount = NULL;
   }
   return;
}

void set_fighting( CHAR_DATA * ch, CHAR_DATA * victim )
{
   FIGHT_DATA *fight;
   CHAR_DATA *gch;
   int group_levels;

   if( ch->fighting )
   {
      char buf[MAX_STRING_LENGTH];

      sprintf( buf, "Set_fighting: %s -> %s (already fighting %s)", ch->name, victim->name, ch->fighting->who->name );
      bug( buf, 0 );
      return;
   }

   if( IS_AFFECTED( ch, AFF_SLEEP ) )
      affect_strip( ch, gsn_sleep );

   if( victim->num_fighting > max_fight( victim ) )
   {
      send_to_char( "There are too many people fighting for you to join in.\n\r", ch );
      return;
   }

   group_levels = 0;

   for( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
   {
      if( is_same_group( gch, ch ) )
      {
         group_levels += IS_NPC( gch ) ? gch->level / 2 : gch->level;
      }
   }

   CREATE( fight, FIGHT_DATA, 1 );
   fight->who = victim;
   fight->xp = ( int )xp_compute( ch, victim ) * 0.85;
   fight->align = align_compute( ch, victim );
   if( !IS_NPC( ch ) && IS_NPC( victim ) )
      fight->timeskilled = times_killed( ch, victim );
   ch->num_fighting = 1;
   ch->fighting = fight;
   if( IS_NPC( ch ) )
      ch->position = POS_FIGHTING;
   else
      switch ( ch->style )
      {
         case ( STYLE_EVASIVE ):
            ch->position = POS_EVASIVE;
            break;
         case ( STYLE_DEFENSIVE ):
            ch->position = POS_DEFENSIVE;
            break;
         case ( STYLE_AGGRESSIVE ):
            ch->position = POS_AGGRESSIVE;
            break;
         case ( STYLE_BERSERK ):
            ch->position = POS_BERSERK;
            break;
         default:
            ch->position = POS_FIGHTING;
      }
   victim->num_fighting++;
   if( victim->switched && IS_AFFECTED( victim->switched, AFF_POSSESS ) )
   {
      send_to_char( "You are disturbed!\n\r", victim->switched );
      do_return( victim->switched, "" );
   }
   return;
}


CHAR_DATA *who_fighting( CHAR_DATA * ch )
{
   if( !ch )
   {
      bug( "who_fighting: null ch", 0 );
      return NULL;
   }
   if( !ch->fighting )
      return NULL;
   return ch->fighting->who;
}

void free_fight( CHAR_DATA * ch )
{
   if( !ch )
   {
      bug( "Free_fight: null ch!", 0 );
      return;
   }
   if( ch->fighting )
   {
      if( !char_died( ch->fighting->who ) )
         --ch->fighting->who->num_fighting;
      DISPOSE( ch->fighting );
   }
   ch->fighting = NULL;
   if( ch->mount )
      ch->position = POS_MOUNTED;
   else
      ch->position = POS_STANDING;
   if( IS_AFFECTED( ch, AFF_BERSERK ) )
   {
      affect_strip( ch, gsn_berserk );
      set_char_color( AT_WEAROFF, ch );
      send_to_char( skill_table[gsn_berserk]->msg_off, ch );
      send_to_char( "\n\r", ch );
   }
   return;
}

void stop_fighting( CHAR_DATA * ch, bool fBoth )
{
   CHAR_DATA *fch;

   free_fight( ch );
   update_pos( ch );

   if( !fBoth )
      return;

   for( fch = first_char; fch; fch = fch->next )
   {
      if( who_fighting( fch ) == ch )
      {
         free_fight( fch );
         update_pos( fch );
      }
   }
   return;
}

int part_vnums[] = { 12,
   14,
   15,
   13,
   44,
   16,
   45,
   46,
   47,
   48,
   49,
   50,
   51,
   52,
   53,
   54,
   55,
   56,
   59,
   87,
   58,
   57,
   55,
   85,
   84,
   86,
   83,
   82,
   81,
   80,
   0,
   0
};

char *part_messages[] = {
   "$n's severed head plops from its neck.",
   "$n's arm is sliced from $s dead body.",
   "$n's leg is sliced from $s dead body.",
   "$n's heart is torn from $s chest.",
   "$n's brains spill grotesquely from $s head.",
   "$n's guts spill grotesquely from $s torso.",
   "$n's hand is sliced from $s dead body.",
   "$n's foot is sliced from $s dead body.",
   "A finger is sliced from $n's dead body.",
   "$n's ear is sliced from $s dead body.",
   "$n's eye is gouged from its socket.",
   "$n's tongue is torn from $s mouth.",
   "An eyestalk is sliced from $n's dead body.",
   "A tentacle is severed from $n's dead body.",
   "A fin is sliced from $n's dead body.",
   "A wing is severed from $n's dead body.",
   "$n's tail is sliced from $s dead body.",
   "A scale falls from the body of $n.",
   "A claw is torn from $n's dead body.",
   "$n's fangs are torn from $s mouth.",
   "A horn is wrenched from the body of $n.",
   "$n's tusk is torn from $s dead body.",
   "$n's tail is sliced from $s dead body.",
   "A ridged scale falls from the body of $n.",
   "$n's beak is sliced from $s dead body.",
   "$n's haunches are sliced from $s dead body.",
   "A hoof is sliced from $n's dead body.",
   "A paw is sliced from $n's dead body.",
   "$n's foreleg is sliced from $s dead body.",
   "Some feathers fall from $n's dead body.",
   "r1 message.",
   "r2 message."
};

void death_cry( CHAR_DATA * ch )
{
   ROOM_INDEX_DATA *was_in_room;
   char *msg;
   EXIT_DATA *pexit;
   int vnum, shift, index, i;

   if( !ch )
   {
      bug( "DEATH_CRY: null ch!", 0 );
      return;
   }

   vnum = 0;
   msg = NULL;

   switch ( number_range( 0, 5 ) )
   {
      default:
         msg = "You hear $n's death cry.";
         break;
      case 0:
         msg = "$n screams furiously as $e falls to the ground in a heap!";
         break;
      case 1:
         msg = "$n hits the ground ... DEAD.";
         break;
      case 2:
         msg = "$n catches $s guts in $s hands as they pour through $s fatal" " wound!";
         break;
      case 3:
         msg = "$n splatters blood on your armor.";
         break;
      case 4:
         msg = "$n gasps $s last breath and blood spurts out of $s " "mouth and ears.";
         break;
      case 5:
         shift = number_range( 0, 31 );
         index = 1 << shift;

         for( i = 0; i < 32 && ch->xflags; i++ )
         {
            if( HAS_BODYPART( ch, index ) )
            {
               msg = part_messages[shift];
               vnum = part_vnums[shift];
               break;
            }
            else
            {
               shift = number_range( 0, 31 );
               index = 1 << shift;
            }
         }

         if( !msg )
            msg = "You hear $n's death cry.";
         break;
   }

   act( AT_CARNAGE, msg, ch, NULL, NULL, TO_ROOM );

   if( vnum )
   {
      char dcb[MAX_STRING_LENGTH];

      char buf[MAX_STRING_LENGTH];

      OBJ_DATA *obj;
      char *name;

      if( !get_obj_index( vnum ) )
      {

         sprintf( dcb, "death_cry: invalid vnum %d", vnum );
         bug( dcb, 0 );
         return;
      }

      name = IS_NPC( ch ) ? ch->short_descr : ch->name;
      obj = create_object( get_obj_index( vnum ), 0 );
      obj->timer = number_range( 4, 7 );
      if( IS_AFFECTED( ch, AFF_POISON ) )
         obj->value[3] = 10;

      sprintf( buf, obj->short_descr, name );
      STRFREE( obj->short_descr );
      obj->short_descr = STRALLOC( buf );

      sprintf( buf, obj->description, name );
      STRFREE( obj->description );
      obj->description = STRALLOC( buf );

      obj = obj_to_room( obj, ch->in_room );
   }

   if( IS_NPC( ch ) )
      msg = "You hear something's death cry.";
   else
      msg = "You hear someone's death cry.";

   was_in_room = ch->in_room;
   for( pexit = was_in_room->first_exit; pexit; pexit = pexit->next )
   {
      if( pexit->to_room && pexit->to_room != was_in_room )
      {
         ch->in_room = pexit->to_room;
         act( AT_CARNAGE, msg, ch, NULL, NULL, TO_ROOM );
      }
   }
   ch->in_room = was_in_room;

   return;
}



void raw_kill( CHAR_DATA * ch, CHAR_DATA * victim, bool spirit )
{
   OBJ_DATA *obj = NULL;
   ROOM_INDEX_DATA *loc;

   if( !victim )
   {
      bug( "raw_kill: null victim!", 0 );
      return;
   }
   if( NOT_AUTHED( victim ) )
   {
      bug( "raw_kill: killing unauthed", 0 );
      return;
   }

   stop_fighting( victim, TRUE );

   if( victim->morph )
   {
      do_unmorph_char( victim );
      raw_kill( ch, victim, TRUE );
      return;
   }

   mprog_death_trigger( ch, victim );
   if( char_died( victim ) )
      return;

   rprog_death_trigger( ch, victim );
   if( char_died( victim ) )
      return;

   make_corpse( victim, ch );
   if( victim->in_room->sector_type == SECT_OCEANFLOOR
       || victim->in_room->sector_type == SECT_UNDERWATER
       || victim->in_room->sector_type == SECT_WATER_SWIM || victim->in_room->sector_type == SECT_WATER_NOSWIM )
      act( AT_BLOOD, "$n's blood slowly clouds the surrounding water.", victim, NULL, NULL, TO_ROOM );
   else if( victim->in_room->sector_type == SECT_AIR )
      act( AT_BLOOD, "$n's blood sprays wildly through the air.", victim, NULL, NULL, TO_ROOM );
   else
      make_blood( victim );

   if( IS_NPC( victim ) )
   {
      loc = victim->in_room;

      victim->pIndexData->killed++;
      extract_char( victim, TRUE );

      if( spirit = TRUE )
         check_spirit( ch, victim, loc );
      victim = NULL;
      return;
   }

   set_char_color( AT_DIEMSG, victim );
   do_help( victim, "_DIEMSG_" );

   extract_char( victim, FALSE );
   if( !victim )
   {
      bug( "oops! raw_kill: extract_char destroyed pc char", 0 );
      return;
   }
   while( victim->first_affect )
      affect_remove( victim, victim->first_affect );
   victim->affected_by = race_table[victim->race]->affected;
   victim->resistant = 0;
   victim->susceptible = 0;
   victim->immune = 0;
   victim->carry_weight = 0;
   victim->armor = 100;
   victim->armor += race_table[victim->race]->ac_plus;
   victim->attacks = race_table[victim->race]->attacks;
   victim->defenses = race_table[victim->race]->defenses;
   victim->mod_str = 0;
   victim->mod_dex = 0;
   victim->mod_wis = 0;
   victim->mod_int = 0;
   victim->mod_con = 0;
   victim->mod_cha = 0;
   victim->mod_lck = 0;
   victim->damroll = 0;
   victim->hitroll = 0;
   victim->alignment = URANGE( -1000, victim->alignment, 1000 );

   victim->saving_poison_death = race_table[victim->race]->saving_poison_death;
   victim->saving_wand = race_table[victim->race]->saving_wand;
   victim->saving_para_petri = race_table[victim->race]->saving_para_petri;
   victim->saving_breath = race_table[victim->race]->saving_breath;
   victim->saving_spell_staff = race_table[victim->race]->saving_spell_staff;
   victim->position = POS_RESTING;
   victim->hit = 1;
   if( IS_VAMPIRE( victim ) )
   {
      victim->blood = 1;
   }
   else
   {
      victim->mana = 1;
   }
   victim->move = 1;

   if( xIS_SET( victim->act, PLR_KILLER ) )
   {
      xREMOVE_BIT( victim->act, PLR_KILLER );
      send_to_char( "The gods have pardoned you for your murderous acts.\n\r", victim );
   }
   if( xIS_SET( victim->act, PLR_THIEF ) )
   {
      xREMOVE_BIT( victim->act, PLR_THIEF );
      send_to_char( "The gods have pardoned you for your thievery.\n\r", victim );
   }
   victim->pcdata->condition[COND_FULL] = 12;
   victim->pcdata->condition[COND_THIRST] = 12;

   obj = create_object( get_obj_index( OBJ_VNUM_CLIGHT ), 1 );
   if( IS_ARM_TYPE( obj ) )
   {
      basestat( obj );
   }
   obj_to_char( obj, victim );

   if( IS_SET( sysdata.save_flags, SV_DEATH ) )
      save_char_obj( victim );
   return;
}

void group_gain( CHAR_DATA * ch, CHAR_DATA * victim )
{
   CHAR_DATA *gch;
   CHAR_DATA *lch;
   int xp;
   int members;
   int group_levels;

   if( IS_NPC( ch ) || victim == ch )
      return;

   members = 0;
   group_levels = 0;

   for( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
   {
      if( is_same_group( gch, ch ) )
      {
         members++;
         group_levels += IS_NPC( gch ) ? gch->level / 2 : gch->level;
      }
   }

   if( members == 0 )
   {
      bug( "Group_gain: members.", members );
      members = 1;
   }

   lch = ch->leader ? ch->leader : ch;

   for( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
   {
      OBJ_DATA *obj;
      OBJ_DATA *obj_next;

      if( !is_same_group( gch, ch ) )
         continue;

      if( gch->level - lch->level > 20 )
      {
         send_to_char( "You are too high for this group.\n\r", gch );
         continue;
      }

      if( gch->level - lch->level < -20 )
      {
         send_to_char( "You are too low for this group.\n\r", gch );
         continue;
      }

      xp = xp_compute( gch, victim );


      gch->alignment = align_compute( gch, victim );

      if( !IS_MAXED( ch ) )
         gain_exp( gch, xp );

      for( obj = ch->first_carrying; obj; obj = obj_next )
      {
         obj_next = obj->next_content;
         if( obj->wear_loc == WEAR_NONE )
            continue;

         if( ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL ) && IS_EVIL( ch ) )
             || ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD ) && IS_GOOD( ch ) )
             || ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL ) && IS_NEUTRAL( ch ) ) )
         {
            act( AT_MAGIC, "You are zapped by $p.", ch, obj, NULL, TO_CHAR );
            act( AT_MAGIC, "$n is zapped by $p.", ch, obj, NULL, TO_ROOM );

            obj_from_char( obj );
            obj = obj_to_room( obj, ch->in_room );
            oprog_zap_trigger( ch, obj );
            if( char_died( ch ) )
               return;
         }
      }
      if( ( !IS_NPC( gch ) && xIS_SET( gch->act, PLR_QUESTING ) ) && IS_NPC( victim ) )
      {
         if( gch->pcdata->questmob == victim->pIndexData->vnum )
         {
            send_to_char( "You have almost completed your quest!\n\r", gch );
            send_to_char( "Return to the questmaster before your time runs of time.\n\r", gch );
            ch->pcdata->questmob = -1;
         }
      }
   }

   return;
}


int align_compute( CHAR_DATA * gch, CHAR_DATA * victim )
{
   int align, newalign, divalign;

   align = gch->alignment - victim->alignment;

   if( gch->alignment > -350 && gch->alignment < 350 )
      divalign = 4;
   else
      divalign = 20;

   if( align > 500 )
      newalign = UMIN( gch->alignment + ( align - 500 ) / divalign, 1000 );
   else if( align < -500 )
      newalign = UMAX( gch->alignment + ( align + 500 ) / divalign, -1000 );
   else
      newalign = gch->alignment - ( int )( gch->alignment / divalign );

   if( gch->race == 5 || gch->race == 29 || gch->race == 30 || gch->race == 31 || gch->race == 32 )
   {
      newalign = -1000;
   }
   if( gch->class == CLASS_SORCERER )
   {
      newalign = -1000;
   }
   return newalign;
}

int xp_compute( CHAR_DATA * gch, CHAR_DATA * victim )
{
   int xp = 0;
   int bonus = 0;
   int xppls = 0;

   switch ( victim->exptier )
   {
      case 0:
         if( gch->level < 100 )
         {
            if( ( victim->level <= ( gch->level - 5 ) ) || ( ( victim->level - gch->level ) >= 40 ) || victim->level > 99 )
            {
               xp = 0;
            }
            else
            {
               if( gch->level < 10 )
               {
                  xp = ( victim->level * 15 ) / sysdata.exp1;
                  xp *= 3;
               }
               else if( gch->level > 9 && gch->level < 20 )
               {
                  xp = ( victim->level * 15 ) / sysdata.exp2;
               }
               else if( gch->level > 19 && gch->level < 30 )
               {
                  xp = ( victim->level * 15 ) / sysdata.exp3;
               }
               else if( gch->level > 29 && gch->level < 40 )
               {
                  xp = ( victim->level * 15 ) / sysdata.exp4;
               }
               else if( gch->level > 39 && gch->level < 50 )
               {
                  xp = ( victim->level * 15 ) / sysdata.exp5;
               }
               else if( gch->level > 49 && gch->level < 60 )
               {
                  xp = ( victim->level * 15 ) / sysdata.exp6;
               }
               else if( gch->level > 59 && gch->level < 70 )
               {
                  xp = ( victim->level * 15 ) / sysdata.exp7;
               }
               else if( gch->level > 69 && gch->level < 80 )
               {
                  xp = ( victim->level * 15 ) / sysdata.exp8;
               }
               else if( gch->level > 79 && gch->level < 100 )
               {
                  xp = ( victim->level * 15 ) / sysdata.exp9;
               }
            }
         }
         else
         {
            xp = 0;
         }
         break;
      case 1:
         if( gch->level == 100 )
         {
            if( victim->level > 99 && victim->level < 199 )
            {
               xp = ( victim->level * 15 ) / 18;
            }
            else
            {
               xp = 0;
            }
         }
         else
         {
            xp = 0;
         }
         break;
      case 2:
         if( gch->level == 200 )
         {
            if( victim->level > 199 && victim->level < 299 )
            {
               xp = ( victim->level * 15 ) / 25;
            }
            else
            {
               xp = 0;
            }
         }
         else
         {
            xp = 0;
         }
         break;
      case 3:
         if( gch->level == 300 )
         {
            if( victim->level > 299 && victim->level < 399 )
            {
               xp = ( victim->level * 15 ) / 30;
            }
            else
            {
               xp = 0;
            }
         }
         else
         {
            xp = 0;
         }
         break;
      case 4:
         if( gch->level == 400 )
         {
/*			if ( victim->level > 399 &&  victim->level < 499 )
			{*/
            xp = ( victim->level * 15 ) / 40;
/*			}
			else
			{
				xp = 0;
			}*/
         }
         else
         {
            xp = 0;
         }
         break;
      case 5:
         xp = ( victim->level * 15 ) / 6;
         break;
   }

   if( !IS_NPC( gch ) && gch->pcdata->deity )
   {
      xppls = ( ( xp * gch->pcdata->deity->exppls ) / 100 );
      xp += xppls;
   }

   bonus = xp;
   if( double_exp )
   {
      xp += bonus;
   }
   if( half_exp )
   {
      xp /= 2;
   }
   return xp;
}

void new_dam_message( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, int totalHitsA, OBJ_DATA * obj )
{
   char buf1[256], buf2[256], buf3[256];
   char bugbuf[MAX_STRING_LENGTH];
// CHAR_DATA *vch;
   const char *vs;
   const char *vp;
   const char *vt;
   const char *attack;
   char punct;
   sh_int dampc;
   struct skill_type *skill = NULL;
   bool gcflag = FALSE;
   bool gvflag = FALSE;
   int d_index, w_index, t_index;
//    ROOM_INDEX_DATA *was_in_room;

   if( !IS_NPC( ch ) && dam > ch->pcdata->damhigh )
   {
      ch->pcdata->damhigh = dam;
      send_to_char( "&CYou have set a new personal damage record!&D\n\r", ch );
   }

   if( !dam )
      dampc = 0;
   else
      dampc = ( ( dam * 1000 ) / victim->max_hit ) + ( 50 - ( ( victim->hit * 50 ) / victim->max_hit ) );

   /*
    * if ( ch->in_room != victim->in_room )
    * {
    * was_in_room = ch->in_room;
    * char_from_room(ch);
    * char_to_room(ch, victim->in_room);
    * }
    * else
    * was_in_room = NULL;
    */

   if( dt > 0 && dt < top_sn )
   {
      w_index = 0;
   }
   else if( dt >= TYPE_HIT && dt < TYPE_HIT + sizeof( attack_table ) / sizeof( attack_table[0] ) )
   {
      w_index = dt - TYPE_HIT;
   }
   else
   {
      sprintf( bugbuf, "Dam_message: bad dt %d from %s in %d.", dt, ch->name, ch->in_room->vnum );
      bug( bugbuf, 0 );
      dt = TYPE_HIT;
      w_index = 0;
   }

   if( dam == 0 )
      d_index = 0;
   else if( dampc < 0 )
      d_index = 1;
   else if( dampc <= 100 )
      d_index = 1 + dampc / 10;
   else if( dampc <= 200 )
      d_index = 11 + ( dampc - 100 ) / 20;
   else if( dampc <= 900 )
      d_index = 16 + ( dampc - 200 ) / 100;
   else if( dampc <= 1500 )
      d_index = 23 + ( dampc - 700 ) / 200;
   else
      d_index = 27;


   if( dam == 0 )
      t_index = 0;
   else
      t_index = number_range( 1, 5 );

   vs = s_message_table[w_index][d_index];
   vp = p_message_table[w_index][d_index];
   vt = ft_end_msg[t_index];

   punct = ( dampc <= 30 ) ? '.' : '!';

   if( dam == 0 && ( !IS_NPC( ch ) && ( IS_SET( ch->pcdata->flags, PCFLAG_GAG ) ) ) )
      gcflag = TRUE;

   if( dam == 0 && ( !IS_NPC( victim ) && ( IS_SET( victim->pcdata->flags, PCFLAG_GAG ) ) ) )
      gvflag = TRUE;

   if( dt >= 0 && dt < top_sn )
      skill = skill_table[dt];

   if( dt == TYPE_HIT && !IS_NPC( ch ) && IS_SET( ch->pcdata->flags, PCFLAG_NOBATTLESPAM ) && ( totalHitsA != 0 )
       && ( totalHitsA != -1 ) )
   {
      sprintf( buf1, "$n hits $N %d times %s %s%c &W[&R%d&W]&D", totalHitsA, vp, vt, punct, dam );
      sprintf( buf2, "You hit $N %d times %s %s%c &W[&R%d&W]&D", totalHitsA, vs, vt, punct, dam );
      sprintf( buf3, "$n hits you %d times %s %s%c &W[&R%d&W]&D", totalHitsA, vp, vt, punct, dam );
      act( AT_ACTION, buf1, ch, NULL, victim, TO_NOTVICT );
      if( !gcflag )
         act( AT_HIT, buf2, ch, NULL, victim, TO_CHAR );
      if( !gvflag )
         act( AT_HITME, buf3, ch, NULL, victim, TO_VICT );
   }
   else if( dt == TYPE_HIT && IS_NPC( ch ) && ( totalHitsA == 0 ) )
   {
      sprintf( buf1, "$n hits $N %s %s%c &W[&R%d&W]&D", vp, vt, punct, dam );
      sprintf( buf2, "You hit $N %s %s%c &W[&R%d&W]&D", vs, vt, punct, dam );
      sprintf( buf3, "$n hits you %s %s%c &W[&R%d&W]&D", vp, vt, punct, dam );
      act( AT_ACTION, buf1, ch, NULL, victim, TO_NOTVICT );
      if( !gcflag )
         act( AT_HIT, buf2, ch, NULL, victim, TO_CHAR );
      if( !gvflag )
         act( AT_HITME, buf3, ch, NULL, victim, TO_VICT );
   }
   else if( dt == TYPE_HIT && !IS_NPC( ch ) && !IS_SET( ch->pcdata->flags, PCFLAG_NOBATTLESPAM ) && ( totalHitsA == 0 ) )
   {
      sprintf( buf1, "$n hits $N %s %s%c &W[&R%d&W]&D", vp, vt, punct, dam );
      sprintf( buf2, "You hit $N %s %s%c &W[&R%d&W]&D", vs, vt, punct, dam );
      sprintf( buf3, "$n hits you %s %s%c &W[&R%d&W]&D", vp, vt, punct, dam );
      if( IS_NPC( victim ) )
      {
         act( AT_ACTION, buf1, ch, NULL, victim, TO_NOTVICT );
         if( !gcflag )
            act( AT_HIT, buf2, ch, NULL, victim, TO_CHAR );
      }
      else
      {
         act( AT_ACTION, buf1, ch, NULL, victim, TO_NOTVICT );
         if( !gcflag )
            act( AT_HIT, buf2, ch, NULL, victim, TO_CHAR );
         if( !gvflag )
            act( AT_HITME, buf3, ch, NULL, victim, TO_VICT );
      }
   }
   else if( dt == TYPE_HIT && !IS_NPC( ch ) && IS_SET( ch->pcdata->flags, PCFLAG_NOBATTLESPAM ) && ( totalHitsA == -1 ) )
   {
      sprintf( buf1, "$n hits $N %d times with terminal %s%c &W[&R%d&W]&D", totalHits, vt, punct, dam );
      sprintf( buf2, "You hit $N %d times with terminal %s%c &W[&R%d&W]&D", totalHits, vt, punct, dam );
      sprintf( buf3, "$n hits you %d times with terminal %s%c &W[&R%d&W]&D", totalHits, vt, punct, dam );
      if( IS_NPC( victim ) )
      {
         act( AT_ACTION, buf1, ch, NULL, victim, TO_NOTVICT );
         if( !gcflag )
            act( AT_HIT, buf2, ch, NULL, victim, TO_CHAR );
      }
      else
      {
         act( AT_ACTION, buf1, ch, NULL, victim, TO_NOTVICT );
         if( !gcflag )
            act( AT_HIT, buf2, ch, NULL, victim, TO_CHAR );
         if( !gvflag )
            act( AT_HITME, buf3, ch, NULL, victim, TO_VICT );
      }
   }
   else if( dt > TYPE_HIT && is_wielding_poisoned( ch ) )
   {
      if( dt < TYPE_HIT + sizeof( attack_table ) / sizeof( attack_table[0] ) )
         attack = attack_table[dt - TYPE_HIT];
      else
      {
         sprintf( bugbuf, "Dam_message: bad dt %d from %s in %d.", dt, ch->name, ch->in_room->vnum );
         bug( bugbuf, 0 );
         dt = TYPE_HIT;
         attack = attack_table[0];
      }

      sprintf( buf1, "$n's poisoned %s hits $N %s %s%c &W[&R%d&W]&D", attack, vp, vt, punct, dam );
      sprintf( buf2, "Your poisoned %s hits $N %s %s%c &W[&R%d&W]&D", attack, vp, vt, punct, dam );
      sprintf( buf3, "$n's poisoned %s hits you %s %s%c &W[&R%d&W]&D", attack, vp, vt, punct, dam );
      act( AT_ACTION, buf1, ch, NULL, victim, TO_NOTVICT );
      if( !gcflag )
         act( AT_HIT, buf2, ch, NULL, victim, TO_CHAR );
      if( !gvflag )
         act( AT_HITME, buf3, ch, NULL, victim, TO_VICT );
   }
   else
   {
      if( skill )
      {
         attack = skill->noun_damage;
         if( dam == 0 && ( totalDamage != 0 ) && ( totalHits != 0 ) && ( victim->position != POS_DEAD ) )
         {
            bool found = FALSE;

            if( skill->miss_char && skill->miss_char[0] != '\0' )
            {
               act( AT_HIT, skill->miss_char, ch, NULL, victim, TO_CHAR );
               found = TRUE;
            }
            if( skill->miss_vict && skill->miss_vict[0] != '\0' )
            {
               act( AT_HITME, skill->miss_vict, ch, NULL, victim, TO_VICT );
               found = TRUE;
            }
            if( skill->miss_room && skill->miss_room[0] != '\0' )
            {
               if( strcmp( skill->miss_room, "supress" ) )
                  act( AT_ACTION, skill->miss_room, ch, NULL, victim, TO_NOTVICT );
               found = TRUE;
            }
            /*
             * if ( found )
             * {
             * if ( was_in_room )
             * {
             * char_from_room(ch);
             * char_to_room(ch, was_in_room);
             * }
             * return;
             * }
             */
         }
         else
         {
            if( skill->hit_char && skill->hit_char[0] != '\0' )
               act( AT_HIT, skill->hit_char, ch, NULL, victim, TO_CHAR );
            if( skill->hit_vict && skill->hit_vict[0] != '\0' )
               act( AT_HITME, skill->hit_vict, ch, NULL, victim, TO_VICT );
            if( skill->hit_room && skill->hit_room[0] != '\0' )
               act( AT_ACTION, skill->hit_room, ch, NULL, victim, TO_NOTVICT );
         }
      }
      else if( dt >= TYPE_HIT && dt < TYPE_HIT + sizeof( attack_table ) / sizeof( attack_table[0] ) )
      {
         if( obj )
            attack = obj->short_descr;
         else
            attack = attack_table[dt - TYPE_HIT];
      }
      else
      {
         sprintf( bugbuf, "Dam_message: bad dt %d from %s in %d.", dt, ch->name, ch->in_room->vnum );
         bug( bugbuf, 0 );
         dt = TYPE_HIT;
         attack = attack_table[0];
      }

      if( !IS_NPC( ch ) && IS_SET( ch->pcdata->flags, PCFLAG_NOBATTLESPAM ) && ( totalHitsA != 0 ) && ( totalHitsA != -1 ) )
      {
         sprintf( buf1, "$n's %s hits $N %d times %s %s%c &W[&R%d&W]&D", attack, totalHitsA, vp, vt, punct, dam );
         sprintf( buf2, "Your %s hits $N %d times %s %s%c &W[&R%d&W]&D", attack, totalHitsA, vs, vt, punct, dam );
         sprintf( buf3, "$n's %s hits you %d times %s %s%c &W[&R%d&W]&D", attack, totalHitsA, vp, vt, punct, dam );
         act( AT_ACTION, buf1, ch, NULL, victim, TO_NOTVICT );
         if( !gcflag )
            act( AT_HIT, buf2, ch, NULL, victim, TO_CHAR );
         if( !gvflag )
            act( AT_HITME, buf3, ch, NULL, victim, TO_VICT );
      }
      else if( IS_NPC( ch ) && ( totalHitsA == 0 ) )
      {
         sprintf( buf1, "$n's %s hits $N %s %s%c &W[&R%d&W]&D", attack, vp, vt, punct, dam );
         sprintf( buf2, "Your %s hits $N %s %s%c &W[&R%d&W]&D", attack, vs, vt, punct, dam );
         sprintf( buf3, "$n's %s hits you %s %s%c &W[&R%d&W]&D", attack, vp, vt, punct, dam );
         act( AT_ACTION, buf1, ch, NULL, victim, TO_NOTVICT );
         if( !gcflag )
            act( AT_HIT, buf2, ch, NULL, victim, TO_CHAR );
         if( !gvflag )
            act( AT_HITME, buf3, ch, NULL, victim, TO_VICT );
      }
      else if( !IS_NPC( ch ) && !IS_SET( ch->pcdata->flags, PCFLAG_NOBATTLESPAM ) && ( totalHitsA == 0 ) )
      {
         sprintf( buf1, "$n's %s hits $N %s %s%c &W[&R%d&W]&D", attack, vp, vt, punct, dam );
         sprintf( buf2, "Your %s hits $N %s %s%c &W[&R%d&W]&D", attack, vs, vt, punct, dam );
         sprintf( buf3, "$n's %s hits you %s %s%c &W[&R%d&W]&D", attack, vp, vt, punct, dam );
         if( IS_NPC( victim ) )
         {
            act( AT_ACTION, buf1, ch, NULL, victim, TO_NOTVICT );
            if( !gcflag )
               act( AT_HIT, buf2, ch, NULL, victim, TO_CHAR );
         }
         else
         {
            act( AT_ACTION, buf1, ch, NULL, victim, TO_NOTVICT );
            if( !gcflag )
               act( AT_HIT, buf2, ch, NULL, victim, TO_CHAR );
            if( !gvflag )
               act( AT_HITME, buf3, ch, NULL, victim, TO_VICT );
         }
      }
      else if( !IS_NPC( ch ) && IS_SET( ch->pcdata->flags, PCFLAG_NOBATTLESPAM ) && ( totalHitsA == -1 ) )
      {
         sprintf( buf1, "$n's %s hits $N %d times with terminal %s%c &W[&R%d&W]&D", attack, totalHits, vt, punct, dam );
         sprintf( buf2, "Your %s hits $N %d times with terminal %s%c &W[&R%d&W]&D", attack, totalHits, vt, punct, dam );
         sprintf( buf3, "$n's %s hits you %d times with terminal %s%c &W[&R%d&W]&D", attack, totalHits, vt, punct, dam );
         if( IS_NPC( victim ) )
         {
            act( AT_ACTION, buf1, ch, NULL, victim, TO_NOTVICT );
            if( !gcflag )
               act( AT_HIT, buf2, ch, NULL, victim, TO_CHAR );
         }
         else
         {
            act( AT_ACTION, buf1, ch, NULL, victim, TO_NOTVICT );
            if( !gcflag )
               act( AT_HIT, buf2, ch, NULL, victim, TO_CHAR );
            if( !gvflag )
               act( AT_HITME, buf3, ch, NULL, victim, TO_VICT );
         }
      }
   }


   /*
    * if ( was_in_room )
    * {
    * char_from_room(ch);
    * char_to_room(ch, was_in_room);
    * }
    */
   return;
}

#ifndef dam_message
void dam_message( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, int totalHitsA )
{
   new_dam_message( ch, victim, dam, dt, totalHitsA );
}
#endif

void do_kill( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   one_argument( argument, arg );



   if( arg[0] == '\0' )
   {
      send_to_char( "Kill whom?\n\r", ch );
      return;
   }

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return;
   }

   if( strlen( arg ) < 3 )
   {
      send_to_char( "Tut tut tut... You must use at least 3 characters in the argument.\n\r", ch );
      return;
   }
   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They are not here.\n\r", ch );
      return;
   }

   if( IS_NPC( victim ) && victim->morph )
   {
      send_to_char( "This creature appears strange to you.  Look upon it more closely before attempting to kill it.", ch );
      return;
   }

   if( ch->substate != SUB_NONE )
   {
      send_to_char( "Please type done before continueing.\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) && !IN_ARENA( victim ) )
   {
      if( !xIS_SET( victim->act, PLR_KILLER ) && !xIS_SET( victim->act, PLR_THIEF ) )
      {
         send_to_char( "You must MURDER a player.\n\r", ch );
         return;
      }
   }


   if( victim == ch )
   {
      send_to_char( "You hit yourself.  Ouch!\n\r", ch );
      multi_hit( ch, ch, TYPE_UNDEFINED );
      return;
   }

   if( is_safe( ch, victim ) )
      return;

   if( ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) && ch->master == victim )
   {
      act( AT_PLAIN, "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if( ch->position == POS_FIGHTING
       || ch->position == POS_EVASIVE
       || ch->position == POS_DEFENSIVE || ch->position == POS_AGGRESSIVE || ch->position == POS_BERSERK )
   {
      send_to_char( "You do the best you can!\n\r", ch );
      return;
   }

   sprintf( buf, "&pYou start fighting %s!\n\r", ( IS_NPC( victim ) ? victim->short_descr : victim->name ) );
   send_to_char( buf, ch );
   check_attacker( ch, victim );
   multi_hit( ch, victim, TYPE_UNDEFINED );
   return;
}



void do_murde( CHAR_DATA * ch, char *argument )
{
   send_to_char( "If you want to MURDER, spell it out.\n\r", ch );
   return;
}



void do_murder( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   bool cpk = FALSE;

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "Murder whom?\n\r", ch );
      return;
   }

   if( sysdata.pkill_lock == 1 )
   {
      send_to_char( "I'm sorry, global no pkill is on.\n\r", ch );
      return;
   }

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I'm sorry, fight lock is on.\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "Suicide is a mortal sin.\n\r", ch );
      return;
   }

   if( is_safe( ch, victim ) )
      return;

   if( ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      if( ch->master == victim )
      {
         act( AT_PLAIN, "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
         return;
      }
      else
      {
         if( ch->master )
            xSET_BIT( ch->master->act, PLR_ATTACKER );
      }
   }

   if( ch->position == POS_FIGHTING
       || ch->position == POS_EVASIVE
       || ch->position == POS_DEFENSIVE || ch->position == POS_AGGRESSIVE || ch->position == POS_BERSERK )
   {
      send_to_char( "You do the best you can!\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) && xIS_SET( ch->act, PLR_NICE ) )
   {
      send_to_char( "You feel too nice to do that!\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) )
   {
      sprintf( log_buf, "%s: murder %s.", ch->name, victim->name );
      log_string_plus( log_buf, LOG_NORMAL, ch->level );
   }

   sprintf( buf, "Help!  I am being attacked by %s!", IS_NPC( ch ) ? ch->short_descr : ch->name );

   if( !IS_PKILL( victim ) )
   {
      cpk = check_illegal_pk( ch, victim );
      if( cpk )
      {
         send_to_char( "You cannot kill a non-pk char.\n\rNow time to pay the piper.\n\r", ch );
         ch->hit = -10;
         ch->position = POS_MORTAL;
         update_pos( ch );
         return;
      }
      else
      {
         do_wartalk( victim, buf );
         multi_hit( ch, victim, TYPE_UNDEFINED );
      }
   }
   return;
}

bool check_illegal_pk( CHAR_DATA * ch, CHAR_DATA * victim )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   bool bnt = FALSE;

   if( !IS_NPC( victim ) && !IS_NPC( ch ) )
   {
      if( !IS_BOUNTIED( victim ) || !IS_BOUNTIED( ch ) )
      {
         bnt = TRUE;
      }

      if( bnt )
      {
         return FALSE;
      }
      else
      {
         if( IS_NPC( ch ) )
            sprintf( buf, " (%s)", ch->name );
         if( IS_NPC( victim ) )
            sprintf( buf2, " (%s)", victim->name );
         sprintf( log_buf, "&p%s on %s%s in &W***&rILLEGAL PKILL&W*** &pattempt at %d",
                  ( lastplayercmd ),
                  ( IS_NPC( victim ) ? victim->short_descr : victim->name ),
                  ( IS_NPC( victim ) ? buf2 : "" ), victim->in_room->vnum );
         last_pkroom = victim->in_room->vnum;
         log_string( log_buf );
         to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL, ch->level );
         stop_fighting( ch, TRUE );
         return TRUE;
      }
   }
   return FALSE;
}


void do_flee( CHAR_DATA * ch, char *argument )
{
   ROOM_INDEX_DATA *was_in;
   ROOM_INDEX_DATA *now_in;
   char buf[MAX_STRING_LENGTH];
   int attempt, los;
   sh_int door;
   EXIT_DATA *pexit;

   if( !who_fighting( ch ) )
   {
      if( ch->position == POS_FIGHTING
          || ch->position == POS_EVASIVE
          || ch->position == POS_DEFENSIVE || ch->position == POS_AGGRESSIVE || ch->position == POS_BERSERK )
      {
         if( ch->mount )
            ch->position = POS_MOUNTED;
         else
            ch->position = POS_STANDING;
      }
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
   }
   if( IS_AFFECTED( ch, AFF_BERSERK ) )
   {
      send_to_char( "Flee while berserking?  You aren't thinking very clearly...\n\r", ch );
      return;
   }
   if( ch->move <= 0 )
   {
      send_to_char( "You're too exhausted to flee from combat!\n\r", ch );
      return;
   }
   if( !IS_NPC( ch ) && ch->position < POS_FIGHTING )
   {
      send_to_char( "You can't flee in an aggressive stance...\n\r", ch );
      return;
   }
   if( IS_NPC( ch ) && ch->position <= POS_SLEEPING )
      return;
   was_in = ch->in_room;
   for( attempt = 0; attempt < 8; attempt++ )
   {
      door = number_door(  );
      if( ( pexit = get_exit( was_in, door ) ) == NULL
          || !pexit->to_room
          || IS_SET( pexit->exit_info, EX_NOFLEE )
          || ( IS_SET( pexit->exit_info, EX_CLOSED )
               && !IS_AFFECTED( ch, AFF_PASS_DOOR ) )
          || ( IS_NPC( ch ) && xIS_SET( pexit->to_room->room_flags, ROOM_NO_MOB ) ) )
         continue;
      affect_strip( ch, gsn_sneak );
      xREMOVE_BIT( ch->affected_by, AFF_SNEAK );
      if( ch->mount && ch->mount->fighting )
         stop_fighting( ch->mount, TRUE );
      move_char( ch, pexit, 0 );
      if( ( now_in = ch->in_room ) == was_in )
         continue;
      ch->in_room = was_in;
      act( AT_FLEE, "$n flees head over heels!", ch, NULL, NULL, TO_ROOM );
      ch->in_room = now_in;
      act( AT_FLEE, "$n glances around for signs of pursuit.", ch, NULL, NULL, TO_ROOM );
      if( !IS_NPC( ch ) )
      {
         act( AT_FLEE, "You flee head over heels from combat!", ch, NULL, NULL, TO_CHAR );
         los = ( exp_level( ch, ch->level + 1 ) - exp_level( ch, ch->level ) ) * 0.02;
         if( ch->level < 50 )
         {
            sprintf( buf, "Curse the gods, you've lost %d experience!", los );
            act( AT_FLEE, buf, ch, NULL, NULL, TO_CHAR );
            gain_exp( ch, 0 - los );
         }
      }
      stop_fighting( ch, TRUE );
      return;
   }
   los = ( exp_level( ch, ch->level + 1 ) - exp_level( ch, ch->level ) ) * 0.01;
   act( AT_FLEE, "You attempt to flee from combat but can't escape!", ch, NULL, NULL, TO_CHAR );
   if( ch->level < 50 && number_bits( 3 ) == 1 )
   {
      sprintf( buf, "Curse the gods, you've lost %d experience!\n\r", los );
      act( AT_FLEE, buf, ch, NULL, NULL, TO_CHAR );
      gain_exp( ch, 0 - los );
   }
   return;
}


void do_sla( CHAR_DATA * ch, char *argument )
{
   send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
   return;
}


void do_slay( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];

   argument = one_argument( argument, arg );
   one_argument( argument, arg2 );
   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: slay [Char] [Type]\n\r", ch );
      send_to_char( "Types: Skin, Slit, Immolate, Demon, Shatter, 9mm, Deheart, Pounce, Fslay, Arch, Nici.\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( ch == victim )
   {
      send_to_char( "Suicide is a mortal sin.\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) && get_trust( victim ) >= get_trust( ch ) )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   if( !str_cmp( arg2, "skin" ) )
   {
      act( AT_IMMORT, "You rip the flesh from $N and send his soul to the fiery depths of hell.", ch, NULL, victim,
           TO_CHAR );
      act( AT_IMMORT,
           "Your flesh has been torn from your bones and your bodyless soul now watches your bones incenerate in the fires of hell.",
           ch, NULL, victim, TO_VICT );
      act( AT_IMMORT, "$n rips the flesh off of $N, releasing his soul into the fiery depths of hell.", ch, NULL, victim,
           TO_NOTVICT );
   }

   else if( !str_cmp( arg2, "9mm" ) && get_trust( ch ) == MAX_LEVEL )
   {
      act( AT_IMMORT, "You pull out your 9mm and bust a cap in $N's ass.", ch, NULL, victim, TO_CHAR );
      act( AT_IMMORT, "$n pulls out $s 9mm and busts a cap in your ass.", ch, NULL, victim, TO_VICT );
      act( AT_IMMORT, "$n pulls out $s 9mm and busts a cap in $N's ass.", ch, NULL, victim, TO_NOTVICT );
   }

   else if( !str_cmp( arg2, "deheart" ) )
   {
      act( AT_IMMORT, "You rip through $N's chest and pull out $M beating heart in your hand.", ch, NULL, victim, TO_CHAR );
      act( AT_IMMORT, "You feel a sharp pain as $n rips into your chest and pulls our your beating heart in $M hand.", ch,
           NULL, victim, TO_VICT );
      act( AT_IMMORT, "Specks of blood hit your face as $n rips through $N's chest pulling out $M's beating heart.", ch,
           NULL, victim, TO_NOTVICT );
   }

   else if( !str_cmp( arg2, "immolate" ) )
   {
      act( AT_IMMORT, "Your fireball turns $N into a blazing inferno.", ch, NULL, victim, TO_CHAR );
      act( AT_IMMORT, "$n releases a searing fireball in your direction.", ch, NULL, victim, TO_VICT );
      act( AT_IMMORT, "$n points at $N, who bursts into a flaming inferno.", ch, NULL, victim, TO_NOTVICT );
   }

   else if( !str_cmp( arg2, "shatter" ) )
   {
      act( AT_IMMORT, "You freeze $N with a glance and shatter the frozen corpse into tiny shards.", ch, NULL, victim,
           TO_CHAR );
      act( AT_IMMORT, "$n freezes you with a glance and shatters your frozen body into tiny shards.", ch, NULL, victim,
           TO_VICT );
      act( AT_IMMORT, "$n freezes $N with a glance and shatters the frozen body into tiny shards.", ch, NULL, victim,
           TO_NOTVICT );
   }

   else if( !str_cmp( arg2, "demon" ) )
   {
      act( AT_IMMORT,
           "You gesture, and a slavering demon appears.  With a horrible grin, the foul creature turns on $N, who screams in panic before being eaten alive.",
           ch, NULL, victim, TO_CHAR );
      act( AT_IMMORT,
           "$n gestures, and a slavering demon appears.  The foul creature turns on you with a horrible grin.   You scream in panic before being eaten alive.",
           ch, NULL, victim, TO_VICT );
      act( AT_IMMORT,
           "$n gestures, and a slavering demon appears.  With a horrible grin, the foul creature turns on $N, who screams in panic before being eaten alive.",
           ch, NULL, victim, TO_NOTVICT );
   }

   else if( !str_cmp( arg2, "pounce" ) )
   {
      act( AT_IMMORT, "Leaping upon $N with bared fangs, you tear open $S throat and toss the corpse to the ground...", ch,
           NULL, victim, TO_CHAR );
      act( AT_IMMORT,
           "In a heartbeat, $n rips $s fangs through your throat!  Your blood sprays and pours to the ground as your life ends...",
           ch, NULL, victim, TO_VICT );
      act( AT_IMMORT,
           "Leaping suddenly, $n sinks $s fangs into $N's throat.  As blood sprays and gushes to the ground, $n tosses $N's dying body away.",
           ch, NULL, victim, TO_NOTVICT );
   }

   else if( !str_cmp( arg2, "slit" ) )
   {
      act( AT_IMMORT, "You calmly slit $N's throat.", ch, NULL, victim, TO_CHAR );
      act( AT_IMMORT, "$n reaches out with a clawed finger and calmly slits your throat.", ch, NULL, victim, TO_VICT );
      act( AT_IMMORT, "A claw extends from $n's hand as $M calmly slits $N's throat.", ch, NULL, victim, TO_NOTVICT );
   }

   else if( !str_cmp( arg2, "fslay" ) )
   {
      act( AT_IMMORT, "You point at $N and fall down laughing.", ch, NULL, victim, TO_CHAR );
      act( AT_IMMORT, "$n points at you and falls down laughing. How embaressing!.", ch, NULL, victim, TO_VICT );
      act( AT_IMMORT, "$n points at $N and falls down laughing.", ch, NULL, victim, TO_NOTVICT );
   }

   else if( !str_cmp( arg2, "arch" ) )
   {
      act( AT_IMMORT,
           "You make a cross symbol with your finger at $N and the Archangel Gabriel appears and slices $M in half.", ch,
           NULL, victim, TO_CHAR );
      act( AT_IMMORT,
           "$n makes a cross symbol with $s finger toward you and the Archangel Gabriel appears and slices you in half.", ch,
           NULL, victim, TO_VICT );
      act( AT_IMMORT,
           "$n makes a cross symbol with $s finger toward $N and and the Archangel Gabriel appears and slices $M in half.",
           ch, NULL, victim, TO_NOTVICT );
   }

   else if( !str_cmp( arg2, "nici" ) )
   {
      act( AT_IMMORT, "&RYou pray for the &WD&ze&Wa&zt&Wh &zT&We&zm&Wp&zl&Wa&zr&R to take $N to $S final resting place.&D",
           ch, NULL, victim, TO_CHAR );
      act( AT_IMMORT,
           "&R$n prays for the &WD&ze&Wa&zt&Wh &zT&We&zm&Wp&zl&Wa&zr&R to take you to your final resting place.&D", ch, NULL,
           victim, TO_VICT );
      act( AT_IMMORT, "&R$n prays for the &WD&ze&Wa&zt&Wh &zT&We&zm&Wp&zl&Wa&zr&R to take $N to $S final resting place.&D",
           ch, NULL, victim, TO_NOTVICT );
   }

   else
   {
      act( AT_IMMORT, "You slay $N in cold blood!", ch, NULL, victim, TO_CHAR );
      act( AT_IMMORT, "$n slays you in cold blood!", ch, NULL, victim, TO_VICT );
      act( AT_IMMORT, "$n slays $N in cold blood!", ch, NULL, victim, TO_NOTVICT );
   }

   set_cur_char( victim );
   raw_kill( ch, victim, FALSE );
   return;
}


void do_sic( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   char arg[MAX_INPUT_LENGTH];
   char buf1[MSL], buf2[MSL], buf3[MSL], buf4[MSL], buf5[MSL], buf6[MSL], buf7[MSL];
   OBJ_DATA *obj, *obj_next;

   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: sic <char>\n\r", ch );
      return;
   }

   if( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( ch == victim )
   {
      send_to_char( "Suicide is a mortal sin.\n\r", ch );
      return;
   }

   if( !IS_BUILD( ch ) )
   {
      send_to_char( "You cannot use the sic command.\n\r", ch );
      return;
   }

   if( IS_NPC( victim ) )
   {
      send_to_char( "You cannot sic a mobile.\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) && get_trust( victim ) >= get_trust( ch ) )
   {
      send_to_char( "You cannot sic a higher level person.\n\r", ch );
      return;
   }

   sprintf( buf1, "The Paladin of %s appears in a puff of smoke before $N.", ch->name );
   sprintf( buf2, "Your Paladin appears in a puff of smoke before $N." );
   sprintf( buf3, "The Paladin of %s appears in a puff of smoke before you.", ch->name );

   act( AT_RED, buf1, ch, NULL, victim, TO_NOTVICT );
   act( AT_RED, buf2, ch, NULL, victim, TO_CHAR );
   act( AT_RED, buf3, ch, NULL, victim, TO_VICT );

   sprintf( buf4, "The Paladin of %s's slash hits $N &wwith >>*<<*>> IMMORAL <<*>>*<< power! &W[&R1000000&W]&D", ch->name );
   sprintf( buf5, "Your Paladin's slash hits $N with >>*<<*>> IMMORAL <<*>>*<< power! &W[&R1000000&W]&D" );
   sprintf( buf6, "The Paladin of %s's slash hits you with >>*<<*>> IMMORAL <<*>>*<< power! &W[&R1000000&W]&D", ch->name );

   act( AT_ACTION, buf4, ch, NULL, victim, TO_NOTVICT );
   act( AT_HIT, buf5, ch, NULL, victim, TO_CHAR );
   act( AT_HITME, buf6, ch, NULL, victim, TO_VICT );

   sprintf( buf7, "&R<&BDEATH INFO&R> %s has just been killed by The Paladin of %s, in %s (%d).&D",
            victim->name, ch->name, victim->in_room->name, victim->in_room->vnum );
   talk_info( AT_PLAIN, buf7 );

   if( IS_AGOD( victim ) )
   {
      for( obj = victim->first_carrying; obj != NULL; obj = obj_next )
      {
         obj_next = obj->next_content;
         if( obj->wear_loc != WEAR_NONE )
            remove_obj( victim, obj->wear_loc, TRUE );
      }
      affect_remove( victim, victim->first_affect );
      victim->affected_by = race_table[victim->race]->affected;
      victim->resistant = 0;
      victim->susceptible = 0;
      victim->immune = 0;
      victim->carry_weight = 0;
      victim->armor = 100;
      victim->armor += race_table[victim->race]->ac_plus;
      victim->attacks = race_table[victim->race]->attacks;
      victim->defenses = race_table[victim->race]->defenses;
      victim->mod_str = 0;
      victim->mod_dex = 0;
      victim->mod_wis = 0;
      victim->mod_int = 0;
      victim->mod_con = 0;
      victim->mod_cha = 0;
      victim->mod_lck = 0;
      victim->damroll = 0;
      victim->hitroll = 0;
      victim->alignment = URANGE( -1000, victim->alignment, 1000 );

      victim->saving_poison_death = race_table[victim->race]->saving_poison_death;
      victim->saving_wand = race_table[victim->race]->saving_wand;
      victim->saving_para_petri = race_table[victim->race]->saving_para_petri;
      victim->saving_breath = race_table[victim->race]->saving_breath;
      victim->saving_spell_staff = race_table[victim->race]->saving_spell_staff;
      victim->hit = 1;
      victim->mana = 0;
      victim->move = 0;
      victim->blood = 0;
      victim->position = POS_RESTING;

      if( IS_SET( sysdata.save_flags, SV_DEATH ) )
         save_char_obj( victim );
   }
   else
   {
      if( victim->level < 100 )
      {
         if( victim->exp > exp_level( victim, victim->level ) )
         {
            gain_exp( victim, ( exp_level( victim, victim->level ) - victim->exp ) / 2 );
         }
      }
      else
      {
         if( victim->exp > exp_level( victim, victim->sublevel ) )
         {
            gain_exp( victim, ( exp_level( victim, victim->sublevel ) - victim->exp ) / 2 );
         }
      }
      act( AT_DEAD, "$n is DEAD!!", victim, 0, 0, TO_ROOM );
      act( AT_DEAD, "You have been KILLED!!\n\r", victim, 0, 0, TO_CHAR );

      set_cur_char( victim );
      raw_kill( ch, victim, FALSE );
   }
   return;
}
