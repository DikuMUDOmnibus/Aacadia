/************************************************************************/
/*        Acadia code team (C) 2002-2005 Kara Enterprises                   */
/************************************************************************/
/*					Bard Handling module			                    */
/************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#ifdef sun
#include <strings.h>
#endif
#include <time.h>
#include "acadia.h"

void say_spell args( ( CHAR_DATA * ch, int sn ) );
ch_ret spell_affect args( ( int sn, int level, CHAR_DATA * ch, void *vo ) );
ch_ret spell_affectchar args( ( int sn, int level, CHAR_DATA * ch, void *vo ) );
int dispel_casting( AFFECT_DATA * paf, CHAR_DATA * ch, CHAR_DATA * victim, int affect, bool dispel );
bool can_charm( CHAR_DATA * ch );

void do_flutecraft( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   char arg2[MAX_STRING_LENGTH];
   char log_buf[MAX_STRING_LENGTH];
   char logline[MAX_STRING_LENGTH];
   OBJ_DATA *obj;
   OBJ_INDEX_DATA *oi;

   strcpy( arg, argument );
   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: flutecraft <keywords>\n\r", ch );
      return;
   }

   if( !LEARNED( ch, gsn_elem_lore ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "A magical force interrupts you.\n\r", ch );
      return;
   }

   if( ( oi = get_obj_index( OBJ_VNUM_FLUTE_MAKE ) ) == NULL || ( obj = create_object( oi, 1 ) ) == NULL )
   {
      send_to_char( "A procedural error interrupts you.\n\r", ch );
      return;
   }
   sprintf( logline, "flutecraft %s", arg );

   separate_obj( obj );

   sprintf( arg2, "%s flute", arg );

   STRFREE( obj->name );
   STRFREE( obj->short_descr );
   obj->name = STRALLOC( arg2 );
   obj->short_descr = STRALLOC( arg2 );

   obj->level = ch->level;
   if( IS_ARM_TYPE( obj ) )
   {
      basestat( obj );
   }

   if( CAN_WEAR( obj, ITEM_TAKE ) )
      obj_to_char( obj, ch );
   else
      obj_to_room( obj, ch->in_room );

   act( AT_ACTION, "You have made a $p.", ch, obj, NULL, TO_CHAR );
   act( AT_ACTION, "$n makes a $p.", ch, obj, NULL, TO_ROOM );
   sprintf( log_buf, "%s: %s", ch->name, logline );
   log_string( log_buf );
   save_char_obj( ch );
   return;
}

void do_tune( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   static char staticbuf[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   OBJ_DATA *obj2;
   void *vo = NULL;
   int mana = 0;
   int blood = 0;
   int sn;
   ch_ret retcode;
   bool dont_wait = FALSE;
   SKILLTYPE *skill = NULL;
   struct timeval time_used;

   retcode = rNONE;

   switch ( ch->substate )
   {
      default:

         if( ch->class != CLASS_BARD )
         {
            send_to_char( "You can't do that.\n\r", ch );
            return;
         }

         obj2 = get_eq_char( ch, WEAR_HOLD );

         if( ( obj2 && obj2->item_type != ITEM_FLUTE ) || !obj2 )
         {
            send_to_char( "You don't have a flute equiped.\n\r", ch );
            return;
         }

         if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_POSSESS ) ) )
         {
            send_to_char( "You can't seem to do that right now...\n\r", ch );
            return;
         }

         if( sysdata.spell_lock == 1 )
         {
            send_to_char( "I'm sorry, global spell lock is on.\n\r", ch );
            return;
         }

         if( xIS_SET( ch->in_room->room_flags, ROOM_NO_MAGIC ) )
         {
            set_char_color( AT_MAGIC, ch );
            send_to_char( "You failed.\n\r", ch );
            return;
         }

         target_name = one_argument( argument, arg1 );
         one_argument( target_name, arg2 );

         if( arg1[0] == '\0' )
         {
            send_to_char( "Play what tune?\n\r", ch );
            return;
         }

         if( get_trust( ch ) < LEVEL_BUILD )
         {
            if( ( sn = find_song( ch, arg1, TRUE ) ) < 0 || ( !IS_NPC( ch ) && !DUAL_SKILL( ch, sn ) ) )
            {
               send_to_char( "&BYou can't do that.\n\r", ch );
               return;
            }
            if( ( skill = get_skilltype( sn ) ) == NULL )
            {
               send_to_char( "You can't do that right now...\n\r", ch );
               return;
            }
         }
         else
         {
            if( ( sn = skill_lookup( arg1 ) ) < 0 )
            {
               send_to_char( "We didn't create that yet...\n\r", ch );
               return;
            }
            if( sn >= MAX_SKILL )
            {
               send_to_char( "Hmm... that might hurt.\n\r", ch );
               return;
            }
            if( ( skill = get_skilltype( sn ) ) == NULL )
            {
               send_to_char( "Something is severely wrong with that one...\n\r", ch );
               return;
            }
            if( skill->type != SKILL_SONG )
            {
               send_to_char( "That isn't a song.\n\r", ch );
               return;
            }
            if( !skill->spell_fun )
            {
               send_to_char( "We didn't finish that one yet...\n\r", ch );
               return;
            }
         }

         if( ch->position < skill->minimum_position && !IS_NPC( ch ) )
         {
            switch ( ch->position )
            {
               default:
                  send_to_char( "You can't concentrate enough.\n\r", ch );
                  break;
               case POS_SITTING:
                  send_to_char( "You can't summon enough energy sitting down.\n\r", ch );
                  break;
               case POS_RESTING:
                  send_to_char( "You're too relaxed to cast that spell.\n\r", ch );
                  break;
               case POS_FIGHTING:
                  if( skill->minimum_position <= POS_EVASIVE )
                  {
                     send_to_char( "This fighting style is too demanding for that!\n\r", ch );
                  }
                  else
                  {
                     send_to_char( "No way!  You are still fighting!\n\r", ch );
                  }
                  break;
               case POS_DEFENSIVE:
                  if( skill->minimum_position <= POS_EVASIVE )
                  {
                     send_to_char( "This fighting style is too demanding for that!\n\r", ch );
                  }
                  else
                  {
                     send_to_char( "No way!  You are still fighting!\n\r", ch );
                  }
                  break;
               case POS_AGGRESSIVE:
                  if( skill->minimum_position <= POS_EVASIVE )
                  {
                     send_to_char( "This fighting style is too demanding for that!\n\r", ch );
                  }
                  else
                  {
                     send_to_char( "No way!  You are still fighting!\n\r", ch );
                  }
                  break;
               case POS_BERSERK:
                  if( skill->minimum_position <= POS_EVASIVE )
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
               case POS_SLEEPING:
                  send_to_char( "You dream about great feats of magic.\n\r", ch );
                  break;
            }
            return;
         }

         if( skill->spell_fun == spell_null )
         {
            send_to_char( "That's not a spell!\n\r", ch );
            return;
         }

         if( !skill->spell_fun )
         {
            send_to_char( "You cannot cast that... yet.\n\r", ch );
            return;
         }

         if( !IS_NPC( ch )
             && !IS_AGOD( ch )
             && skill->guild != CLASS_NONE && ( !ch->pcdata->clan || skill->guild != ch->pcdata->clan->class ) )
         {
            send_to_char( "That is only available to members of a certain guild.\n\r", ch );
            return;
         }

         if( !ch->in_room || ( skill->spell_sector && !IS_SET( skill->spell_sector, ( 1 << ch->in_room->sector_type ) ) ) )
         {
            send_to_char( "You can not cast that here.\n\r", ch );
            return;
         }

         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            if( IS_VAMPIRE( ch ) )
            {
               if( !IS_AGOD( ch ) || !IS_NPC( ch ) )
               {
                  if( skill->min_mana > 0 )
                  {
                     blood = UMAX( 5, skill->min_mana + ch->pcdata->deity->splcst );
                  }
                  else
                  {
                     blood = 0;
                  }
               }
               else
               {
                  blood = 0;
               }
            }
            else
            {
               if( !IS_AGOD( ch ) || !IS_NPC( ch ) )
               {
                  if( skill->min_mana > 0 )
                  {
                     mana = UMAX( 5, skill->min_mana + ch->pcdata->deity->splcst );
                  }
                  else
                  {
                     mana = 0;
                  }
               }
               else
               {
                  mana = 0;
               }
            }
         }
         else
         {
            if( IS_VAMPIRE( ch ) )
            {
               if( !IS_AGOD( ch ) || !IS_NPC( ch ) )
                  blood = skill->min_mana;
               else
                  blood = 0;
            }
            else
            {
               if( !IS_AGOD( ch ) || !IS_NPC( ch ) )
                  mana = skill->min_mana;
               else
                  mana = 0;
            }
         }

         vo = locate_targets( ch, arg2, sn, &victim, &obj );
         if( vo == &pAbort )
            return;

         if( !IS_IMMORTAL( ch ) )
         {
            if( IS_VAMPIRE( ch ) )
            {
               if( !IS_NPC( ch ) && ch->blood < blood )
               {
                  send_to_char( "You don't have enough blood power.\n\r", ch );
                  return;
               }
            }
            else
            {
               if( !IS_NPC( ch ) && ch->mana < mana )
               {
                  send_to_char( "You don't have enough mana.\n\r", ch );
                  return;
               }
            }
         }

         if( skill->participants <= 1 )
            break;

         add_timer( ch, TIMER_DO_FUN, UMIN( skill->beats / 10, 3 ), do_cast, 1 );
         act( AT_MAGIC, "You begin to chant...", ch, NULL, NULL, TO_CHAR );
         act( AT_MAGIC, "$n begins to chant...", ch, NULL, NULL, TO_ROOM );
         sprintf( staticbuf, "%s %s", arg2, target_name );
         ch->alloc_ptr = str_dup( staticbuf );
         ch->tempnum = sn;
         return;
      case SUB_TIMER_DO_ABORT:
         DISPOSE( ch->alloc_ptr );
         if( IS_VALID_SN( ( sn = ch->tempnum ) ) )
         {
            if( ( skill = get_skilltype( sn ) ) == NULL )
            {
               send_to_char( "Something went wrong...\n\r", ch );
               bug( "do_cast: SUB_TIMER_DO_ABORT: bad sn %d", sn );
               return;
            }

            if( !IS_IMMORTAL( ch ) || !IS_NPC( ch ) )
            {
               if( !IS_NPC( ch ) && ch->pcdata->deity )
               {
                  if( IS_VAMPIRE( ch ) )
                  {
                     blood = skill->min_mana + ch->pcdata->deity->splcst;
                  }
                  else
                  {
                     mana = skill->min_mana + ch->pcdata->deity->splcst;
                  }
               }
               else
               {
                  if( IS_VAMPIRE( ch ) )
                  {
                     blood = skill->min_mana;
                  }
                  else
                  {
                     mana = skill->min_mana;
                  }
               }

               if( IS_VAMPIRE( ch ) )
                  ch->blood -= blood / 3;
               else
                  ch->mana -= mana / 3;
            }
         }
         set_char_color( AT_MAGIC, ch );
         send_to_char( "You stop chanting...\n\r", ch );
         return;
      case 1:
         sn = ch->tempnum;
         if( ( skill = get_skilltype( sn ) ) == NULL )
         {
            send_to_char( "Something went wrong...\n\r", ch );
            bug( "do_cast: substate 1: bad sn %d", sn );
            return;
         }
         if( !ch->alloc_ptr || !IS_VALID_SN( sn ) || skill->type != SKILL_SPELL )
         {
            send_to_char( "Something cancels out the spell!\n\r", ch );
            bug( "do_cast: ch->alloc_ptr NULL or bad sn (%d)", sn );
            return;
         }
         if( !IS_IMMORTAL( ch ) || !IS_NPC( ch ) )
         {
            if( ch->pcdata->deity )
            {
               if( IS_VAMPIRE( ch ) )
               {
                  blood = skill->min_mana + ch->pcdata->deity->splcst;
               }
               else
               {
                  mana = skill->min_mana + ch->pcdata->deity->splcst;
               }
            }
            else
            {
               if( IS_VAMPIRE( ch ) )
               {
                  blood = skill->min_mana;
               }
               else
               {
                  mana = skill->min_mana;
               }
            }
         }
         strcpy( staticbuf, ch->alloc_ptr );
         target_name = one_argument( staticbuf, arg2 );
         DISPOSE( ch->alloc_ptr );
         ch->substate = SUB_NONE;
   }


   if( str_cmp( skill->name, "ventriloquate" ) )
      say_spell( ch, sn );

   if( !dont_wait )
      WAIT_STATE( ch, skill->beats );

   if( !process_spell_components( ch, sn ) )
   {
      if( !IS_IMMORTAL( ch ) )
      {
         if( IS_VAMPIRE( ch ) )
            ch->blood -= blood / 2;
         else
            ch->mana -= mana / 2;
      }
      learn_from_failure( ch, sn );
      return;
   }

   if( !IS_NPC( ch ) && !IS_AGOD( ch ) && ( number_percent(  ) + skill->difficulty * 5 ) > ch->pcdata->learned[sn] )
   {
      switch ( number_bits( 2 ) )
      {
         case 0:
            if( ch->fighting )
               send_to_char( "This round of battle is too hectic to concentrate properly.\n\r", ch );
            else
               send_to_char( "You lost your concentration.\n\r", ch );
            break;
         case 1:
            if( number_bits( 2 ) == 0 )
            {
               switch ( number_bits( 2 ) )
               {
                  case 0:
                     send_to_char( "A tickle in your nose prevents you from keeping your concentration.\n\r", ch );
                     break;
                  case 1:
                     send_to_char( "An itch on your leg keeps you from properly casting your spell.\n\r", ch );
                     break;
                  case 2:
                     send_to_char( "Something in your throat prevents you from uttering the proper phrase.\n\r", ch );
                     break;
                  case 3:
                     send_to_char( "A twitch in your eye disrupts your concentration for a moment.\n\r", ch );
                     break;
               }
            }
            else
               send_to_char( "Something distracts you, and you lose your concentration.\n\r", ch );
            break;
         case 2:
            if( ch->fighting )
               send_to_char( "There wasn't enough time this round to complete the casting.\n\r", ch );
            else
               send_to_char( "You lost your concentration.\n\r", ch );
            break;
         case 3:
            send_to_char( "You get a mental block mid-way through the casting.\n\r", ch );
            break;
      }
      if( !IS_IMMORTAL( ch ) )
      {
         if( IS_VAMPIRE( ch ) )
            ch->blood -= blood / 2;
         else
            ch->mana -= mana / 2;
      }
      learn_from_failure( ch, sn );
      return;
   }
   else
   {
      if( !IS_IMMORTAL( ch ) )
      {
         if( IS_VAMPIRE( ch ) )
            ch->blood -= blood;
         else
            ch->mana -= mana;
      }
      if( ( ( skill->target == TAR_CHAR_DEFENSIVE
              || skill->target == TAR_CHAR_SELF ) && victim && IS_SET( victim->immune, RIS_MAGIC ) ) )
      {
         immune_casting( skill, ch, victim, NULL );
         retcode = rSPELL_FAILED;
      }
      else
      {
         start_timer( &time_used );
         retcode = ( *skill->spell_fun ) ( sn, ch->level, ch, vo );
         end_timer( &time_used );
         update_userec( &time_used, &skill->userec );
      }
   }

   if( ch->in_room && IS_SET( ch->in_room->area->flags, AFLAG_SPELLLIMIT ) )
      ch->in_room->area->curr_spell_count++;

   if( retcode == rCHAR_DIED || retcode == rERROR || char_died( ch ) )
      return;

   if( retcode != rSPELL_FAILED )
      learn_from_success( ch, sn );
   else
      learn_from_failure( ch, sn );


   if( skill->target == TAR_CHAR_OFFENSIVE && victim && !char_died( victim ) && victim != ch )
   {
      CHAR_DATA *vch, *vch_next;

      if( sysdata.fight_lock == 1 )
      {
         send_to_char( "I'm sorry, fight lock is on.\n\r", ch );
         return;
      }

      for( vch = ch->in_room->first_person; vch; vch = vch_next )
      {
         vch_next = vch->next_in_room;

         if( vch == victim )
         {
            if( vch->master != ch && !vch->fighting )
               retcode = multi_hit( vch, ch, TYPE_UNDEFINED );
            break;
         }
      }
   }

   return;
}

ch_ret spell_kn_minne( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *gch;
   AFFECT_DATA af;

   if( !LEARNED( ch, gsn_lore ) )
   {
      send_to_char( "You don't know enough lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( !LEARNED( ch, gsn_elem_lore ) )
   {
      send_to_char( "You don't know enough song lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   for( gch = first_char; gch; gch = gch->next )
   {
      if( is_same_group( gch, ch ) )
      {
         if( IS_AFFECTED( gch, AFF_MINNE ) )
            continue;

         af.type = sn;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.bitvector = meb( AFF_MINNE );
         af.location = APPLY_AC;
         af.modifier = -50;
         affect_to_char( gch, &af );

         act( AT_MAGIC, "Your armor glows.", gch, NULL, NULL, TO_CHAR );
         act( AT_MAGIC, "$n's armor glows.", gch, NULL, NULL, TO_ROOM );
      }
   }
   return rNONE;
}

ch_ret spell_kn_minne2( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *gch;
   AFFECT_DATA af;

   if( !LEARNED( ch, gsn_lore ) )
   {
      send_to_char( "You don't know enough lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( !LEARNED( ch, gsn_elem_lore ) )
   {
      send_to_char( "You don't know enough song lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   for( gch = first_char; gch; gch = gch->next )
   {
      if( is_same_group( gch, ch ) )
      {
         if( IS_AFFECTED( gch, AFF_MINNE2 ) )
            continue;

         af.type = sn;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.bitvector = meb( AFF_MINNE2 );
         af.location = APPLY_AC;
         af.modifier = -100;
         affect_to_char( gch, &af );

         act( AT_MAGIC, "Your armor glows.", gch, NULL, NULL, TO_CHAR );
         act( AT_MAGIC, "$n's armor glows.", gch, NULL, NULL, TO_ROOM );
      }
   }
   return rNONE;
}

ch_ret spell_kn_minne3( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *gch;
   AFFECT_DATA af;

   if( !LEARNED( ch, gsn_lore ) )
   {
      send_to_char( "You don't know enough lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( !LEARNED( ch, gsn_elem_lore ) )
   {
      send_to_char( "You don't know enough song lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   for( gch = first_char; gch; gch = gch->next )
   {
      if( is_same_group( gch, ch ) )
      {
         if( IS_AFFECTED( gch, AFF_MINNE3 ) )
            continue;

         af.type = sn;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.bitvector = meb( AFF_MINNE3 );
         af.location = APPLY_AC;
         af.modifier = -150;
         affect_to_char( gch, &af );

         act( AT_MAGIC, "Your armor glows.", gch, NULL, NULL, TO_CHAR );
         act( AT_MAGIC, "$n's armor glows.", gch, NULL, NULL, TO_ROOM );
      }
   }
   return rNONE;
}

ch_ret spell_kn_minne4( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *gch;
   AFFECT_DATA af;

   if( !LEARNED( ch, gsn_lore ) )
   {
      send_to_char( "You don't know enough lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( !LEARNED( ch, gsn_elem_lore ) )
   {
      send_to_char( "You don't know enough song lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   for( gch = first_char; gch; gch = gch->next )
   {
      if( is_same_group( gch, ch ) )
      {
         if( IS_AFFECTED( gch, AFF_MINNE4 ) )
            continue;

         af.type = sn;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.bitvector = meb( AFF_MINNE4 );
         af.location = APPLY_AC;
         af.modifier = -200;
         affect_to_char( gch, &af );

         act( AT_MAGIC, "Your armor glows.", gch, NULL, NULL, TO_CHAR );
         act( AT_MAGIC, "$n's armor glows.", gch, NULL, NULL, TO_ROOM );
      }
   }
   return rNONE;
}

ch_ret spell_val_minuet( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *gch;
   AFFECT_DATA af;

   if( !LEARNED( ch, gsn_lore ) )
   {
      send_to_char( "You don't know enough lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( !LEARNED( ch, gsn_elem_lore ) )
   {
      send_to_char( "You don't know enough song lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   for( gch = first_char; gch; gch = gch->next )
   {
      if( is_same_group( gch, ch ) )
      {
         if( IS_AFFECTED( gch, AFF_MINUET ) )
            continue;

         af.type = sn;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.bitvector = meb( AFF_MINUET );
         af.location = APPLY_HITROLL;
         af.modifier = 30;
         affect_to_char( gch, &af );

         af.type = sn;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.bitvector = meb( AFF_MINUET );
         af.location = APPLY_DAMROLL;
         af.modifier = 30;
         affect_to_char( gch, &af );

         act( AT_MAGIC, "Your weapons glows.", gch, NULL, NULL, TO_CHAR );
         act( AT_MAGIC, "$n's weapons glows.", gch, NULL, NULL, TO_ROOM );
      }
   }
   return rNONE;
}

ch_ret spell_val_minuet2( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *gch;
   AFFECT_DATA af;

   if( !LEARNED( ch, gsn_lore ) )
   {
      send_to_char( "You don't know enough lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( !LEARNED( ch, gsn_elem_lore ) )
   {
      send_to_char( "You don't know enough song lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   for( gch = first_char; gch; gch = gch->next )
   {
      if( is_same_group( gch, ch ) )
      {
         if( IS_AFFECTED( gch, AFF_MINUET2 ) )
            continue;

         af.type = sn;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.bitvector = meb( AFF_MINUET2 );
         af.location = APPLY_HITROLL;
         af.modifier = 50;
         affect_to_char( gch, &af );

         af.type = sn;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.bitvector = meb( AFF_MINUET2 );
         af.location = APPLY_DAMROLL;
         af.modifier = 50;
         affect_to_char( gch, &af );

         act( AT_MAGIC, "Your weapons glows.", gch, NULL, NULL, TO_CHAR );
         act( AT_MAGIC, "$n's weapons glows.", gch, NULL, NULL, TO_ROOM );
      }
   }
   return rNONE;
}

ch_ret spell_ice_threnody( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *gch;
   AFFECT_DATA af;

   if( !LEARNED( ch, gsn_lore ) )
   {
      send_to_char( "You don't know enough lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( !LEARNED( ch, gsn_elem_lore ) )
   {
      send_to_char( "You don't know enough song lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   for( gch = first_char; gch; gch = gch->next )
   {
      if( is_same_group( gch, ch ) )
      {
         af.type = sn;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.location = APPLY_AFFECT;
         af.modifier = 0;
         af.bitvector = meb( AFF_ICESHIELD );
         affect_to_char( gch, &af );

         act( AT_MAGIC, "You gain a shield of ice.", gch, NULL, NULL, TO_CHAR );
         act( AT_MAGIC, "$n's gains a shield of ice.", gch, NULL, NULL, TO_ROOM );
      }
   }
   return rNONE;
}

ch_ret spell_fire_threnody( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *gch;
   AFFECT_DATA af;

   if( !LEARNED( ch, gsn_lore ) )
   {
      send_to_char( "You don't know enough lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( !LEARNED( ch, gsn_elem_lore ) )
   {
      send_to_char( "You don't know enough song lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   for( gch = first_char; gch; gch = gch->next )
   {
      if( is_same_group( gch, ch ) )
      {
         af.type = sn;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.bitvector = meb( AFF_FIRESHIELD );
         af.location = APPLY_AFFECT;
         af.modifier = 0;
         affect_to_char( gch, &af );

         act( AT_MAGIC, "You gain a shield of fire.", gch, NULL, NULL, TO_CHAR );
         act( AT_MAGIC, "$n's gains a shield of fire.", gch, NULL, NULL, TO_ROOM );
      }
   }
   return rNONE;
}

ch_ret spell_shock_threnody( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *gch;
   AFFECT_DATA af;

   if( !LEARNED( ch, gsn_lore ) )
   {
      send_to_char( "You don't know enough lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( !LEARNED( ch, gsn_elem_lore ) )
   {
      send_to_char( "You don't know enough song lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   for( gch = first_char; gch; gch = gch->next )
   {
      if( is_same_group( gch, ch ) )
      {
         af.type = sn;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.bitvector = meb( AFF_SHOCKSHIELD );
         af.location = APPLY_AFFECT;
         af.modifier = 0;
         affect_to_char( gch, &af );

         act( AT_MAGIC, "You gain a shield of lightning.", gch, NULL, NULL, TO_CHAR );
         act( AT_MAGIC, "$n's gains a shield of lightning.", gch, NULL, NULL, TO_ROOM );
      }
   }
   return rNONE;
}

ch_ret spell_optic_etude( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *gch;
   AFFECT_DATA af;

   if( !LEARNED( ch, gsn_lore ) )
   {
      send_to_char( "You don't know enough lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( !LEARNED( ch, gsn_elem_lore ) )
   {
      send_to_char( "You don't know enough song lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   for( gch = first_char; gch; gch = gch->next )
   {
      if( is_same_group( gch, ch ) )
      {
         af.type = sn;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.bitvector = meb( AFF_ETUDE );
         af.location = APPLY_AFFECT;
         af.modifier = 0;
         affect_to_char( gch, &af );

         act( AT_MAGIC, "You start seeing things more clearly.", gch, NULL, NULL, TO_CHAR );
         act( AT_MAGIC, "$n sees more clearly.", gch, NULL, NULL, TO_ROOM );
      }
   }
   return rNONE;
}
