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
/*					Update Handling module          */
/************************************************************************/
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "acadia.h"
int yesd;

extern void start_arena(  );
extern void do_game(  );
extern int in_start_arena;
extern int ppl_in_arena;
extern int ppl_challenged;
extern int num_in_arena(  );
extern int tlevels;

int global_exp;
int global2_exp;
sh_int display;

int hit_gain args( ( CHAR_DATA * ch ) );
int mana_gain args( ( CHAR_DATA * ch ) );
int blood_gain args( ( CHAR_DATA * ch ) );
int move_gain args( ( CHAR_DATA * ch ) );
void mobile_update args( ( void ) );
void weather_update args( ( void ) );
void time_update args( ( void ) );
void char_update args( ( void ) );
void obj_update args( ( void ) );
void aggr_update args( ( void ) );
void room_act_update args( ( void ) );
void obj_act_update args( ( void ) );
void char_check args( ( void ) );
void plague args( ( CHAR_DATA * ch ) );
void drunk_randoms args( ( CHAR_DATA * ch ) );
void hallucinations args( ( CHAR_DATA * ch ) );
void subtract_times args( ( struct timeval * etime, struct timeval * stime ) );

void adjust_vectors args( ( WEATHER_DATA * weather ) );
void get_weather_echo args( ( WEATHER_DATA * weather ) );
void get_time_echo args( ( WEATHER_DATA * weather ) );
bool write_to_descriptor args( ( int desc, char *txt, int length ) );

CHAR_DATA *gch_prev;
OBJ_DATA *gobj_prev;

CHAR_DATA *timechar;

char *corpse_descs[] = {
   "The corpse of %s is in the last stages of decay.",
   "The corpse of %s is crawling with vermin.",
   "The corpse of %s fills the air with a foul stench.",
   "The corpse of %s is buzzing with flies.",
   "The corpse of %s lies here."
};

extern int top_exit;

void advance_level( CHAR_DATA * ch, bool broadcast )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];
   CHAR_DATA *crier;
   CHAR_DATA *crier_next;
   int add_hp;
   int add_mana;
   int add_blood;
   int add_move;
   int add_prac;

   if( xIS_SET( ch->act, PLR_A_TITLE ) )
   {
//     sprintf( buf, "the %s",
//    title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0] );
//     if ( !str_cmp( title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0], "Not Set" )
      sprintf( buf, "the %s", class_table[ch->class]->who_name );
      set_title( ch, buf );
   }

   add_hp = con_app[get_curr_con( ch )].hitp + number_range( hp_min( ch ), hp_max( ch ) );
   add_mana = use_mana( ch ) ? number_range( 2, ( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 2 ) : 0;
   add_blood = number_range( 2, ( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 2 );
   add_move = number_range( 5, ( get_curr_con( ch ) + get_curr_dex( ch ) ) / 4 );
   add_prac = wis_app[get_curr_wis( ch )].practice;

   if( ch->pcdata->deity )
   {
      add_hp += ch->pcdata->deity->hitgain;
      add_mana += ch->pcdata->deity->managain;
      add_blood += ch->pcdata->deity->managain;
      add_move += ch->pcdata->deity->movegain;
   }

   add_hp = UMAX( 1, add_hp );
   add_mana = UMAX( 0, add_mana );
   add_move = UMAX( 10, add_move );
   if( IS_VAMPIRE( ch ) )
      add_blood = UMAX( 0, add_blood );

   if( IS_PKILL( ch ) )
   {
      add_mana = add_mana + add_mana * .3;
      add_move = add_move + add_move * .3;
      add_hp += 1;
      sprintf( buf, "Gravoc's Pandect steels your sinews.\n\r" );
   }
   if( ch->level == 100
       && ( ch->class == CLASS_WARRIOR || ch->class == CLASS_THIEF || ch->class == CLASS_SAMURAI
            || ch->class == CLASS_ARCHER || ch->class == CLASS_BARBARIAN ) )
   {
      add_mana = 700;
   }

   ch->max_hit += add_hp;
   ch->max_mana += add_mana;
   ch->max_move += add_move;
   ch->practice += add_prac;
   if( IS_VAMPIRE( ch ) )
      ch->max_blood += add_blood;

   if( !IS_NPC( ch ) )
      xREMOVE_BIT( ch->act, PLR_BOUGHT_PET );

   tlevels++;
   if( ch->level < LEVEL_IMMORTAL )
   {
      if( IS_VAMPIRE( ch ) )
         sprintf( buf,
                  "Your gain is: %d/%d hp, %d/%d bp, %d/%d mv %d/%d prac.\n\r",
                  add_hp, ch->max_hit, add_blood, ch->max_blood, add_move, ch->max_move, add_prac, ch->practice );
      else
         sprintf( buf,
                  "Your gain is: %d/%d hp, %d/%d mana, %d/%d mv %d/%d prac.\n\r",
                  add_hp, ch->max_hit, add_mana, ch->max_mana, add_move, ch->max_move, add_prac, ch->practice );
      set_char_color( AT_WHITE, ch );
      send_to_char( buf, ch );

      if( ch->level == 3 )
      {
/*		  OBJ_DATA *obj;

		  if ( ( obj = get_eq_char( ch, WEAR_FLOAT ) ) != NULL
			  && obj->item_type == ITEM_GHOST )
		  {
			  send_to_char( "&BDyrthianua's presence fades as the crysstal shatters!&D\n\r", ch );
			  extract_obj( obj );
		  }*/
         de_equip_char( ch );
         ch->mod_str = 0;
         ch->mod_int = 0;
         ch->mod_wis = 0;
         ch->mod_dex = 0;
         ch->mod_con = 0;
         ch->mod_cha = 0;
         ch->mod_lck = 0;
         re_equip_char( ch );
         send_to_char( "&pYou feel Nicole's presence fade.&D\n\r", ch );
         send_to_char( "You are now able to use more channels like chat, type channels to see what channels there is.\n\r",
                       ch );
      }
      if( ch->level == 5 )
      {
         OBJ_DATA *obj;

         if( ( obj = get_eq_char( ch, WEAR_FLOAT ) ) != NULL && obj->item_type == ITEM_GHOST )
         {
            send_to_char( "&BDyrthianua's presence fades as the crysstal shatters!&D\n\r", ch );
            extract_obj( obj );
         }
      }
      if( ch->level == 10 )
      {
         send_to_char( "&W+---------------------------------------------------------------------+\n\r", ch );
         send_to_char( "&W| CONGRATULATIONS: You have reached a level of decision. You can now  |\n\r", ch );
         send_to_char( "&W| choose to worship a deity. To see what deities there are to devote  |\n\r", ch );
         send_to_char( "&W| to type &Pdeities&W. But be carefull, it takes 200 practices to     |\n\r", ch );
         send_to_char( "&W| undevote.                                                           |\n\r", ch );
         send_to_char( "&W+---------------------------------------------------------------------+\n\r", ch );
      }
      if( is_affected( ch, gsn_karaaura ) )
      {
         affect_strip( ch, gsn_karaaura );
         send_to_char( "Kara's Aura leaves your soul feeling empty.\n\r", ch );
      }

      if( broadcast && !IS_IMMORTAL( ch ) )
      {
         sprintf( buf, "&R<&BLEVEL INFO&R> %s has just reached level %d!", ch->name, ch->level );
         talk_info( AT_BLUE, buf );
//      }

         for( crier = first_char; crier != NULL; crier = crier_next )
         {
            crier_next = crier->next;

            if( !IS_NPC( crier ) )
               continue;
            if( crier->spec_fun == spec_lookup( "spec_crier" ) )
               break;
         }

         if( crier == NULL || crier->spec_fun != spec_lookup( "spec_crier" ) )
         {
            bug( "NO CRIER.\n\r", 0 );
         }
         else
         {
            sprintf( buf3, "%s on reaching level %d!!", ch->name, ch->level );
            do_gratz( crier, buf3 );
         }
      }

      if( ch->level == 100 )
      {
         do_help( ch, "NEW_HERO" );
      }
      if( xIS_SET( ch->act, PLR_BOOSTED ) )
      {
         xREMOVE_BIT( ch->act, PLR_BOOSTED );
         ch->mod_str = 0;
         ch->mod_int = 0;
         ch->mod_wis = 0;
         ch->mod_dex = 0;
         ch->mod_con = 0;
         ch->mod_cha = 0;
         ch->mod_lck = 0;
         send_to_char( "&pYou feel Nicole's presence fade.&D\n\r", ch );
      }

      if( IS_SET( sysdata.save_flags, SV_LEVEL ) )
         save_char_obj( ch );

      if( !IS_NPC( ch ) )
      {
         sprintf( buf2, "&G%-13s  ->&w%-2d  &G-&w  %-5d&G   Rvnum: %-5d   %s %s",
                  ch->name,
                  ch->level,
                  get_age( ch ),
                  ch->in_room == NULL ? 0 : ch->in_room->vnum,
                  capitalize( race_table[ch->race]->race_name ), class_table[ch->class]->who_name );
         append_to_file( PLEVEL_FILE, buf2 );
      }
   }
   return;
}

void advance_slevel( CHAR_DATA * ch, bool broadcast, int gain )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];
   CHAR_DATA *crier;
   CHAR_DATA *crier_next;
   int add_hp;
   int add_mana;
   int add_blood;
   int add_move;
   int add_prac;
   int templ;

   if( ch->pcdata->wasat == 0 )
   {
      add_hp = con_app[get_curr_con( ch )].hitp + number_range( hp_min( ch ), hp_max( ch ) );
      add_mana =
         use_mana( ch ) ? number_range( 2, ( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 8 ) : number_range( 0, 2 );
      add_blood = number_range( 2, ( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 8 );
      add_move = number_range( 5, ( get_curr_con( ch ) + get_curr_dex( ch ) ) / 4 );
      add_prac = wis_app[get_curr_wis( ch )].practice;

      if( ch->pcdata->deity )
      {
         add_hp += ch->pcdata->deity->hitgain;
         add_mana += ch->pcdata->deity->managain;
         add_blood += ch->pcdata->deity->managain;
         add_move += ch->pcdata->deity->movegain;
      }

      if( ch->level == 100 )
      {
         add_hp = add_hp / 2;
         add_mana = add_mana / 2;
         add_blood = add_blood / 2;
         add_move = add_move / 2;
         add_prac = add_prac / 2;
      }

      add_hp = UMAX( 1, add_hp );
      add_hp = ( add_hp * gain );
      add_mana = UMAX( 0, add_mana );
      add_mana = ( add_mana * gain );
      add_move = UMAX( 10, add_move );
      add_move = ( add_move * gain );
      add_prac = UMAX( 0, add_prac );
      add_prac = ( add_prac * gain );
      if( IS_VAMPIRE( ch ) )
      {
         add_blood = UMAX( 0, add_blood );
         add_blood = ( add_blood * gain );
      }

      if( IS_PKILL( ch ) )
      {
         add_mana = add_mana + add_mana * .3;
         add_move = add_move + add_move * .3;
         add_hp += 1;
         sprintf( buf, "Gravoc's Pandect steels your sinews.\n\r" );
      }

      ch->max_hit += add_hp;
      ch->max_mana += add_mana;
      ch->max_move += add_move;
      ch->practice += add_prac;
      if( IS_VAMPIRE( ch ) )
         ch->max_blood += add_blood;
   }
   else
   {
      if( gain >= ch->pcdata->wasat )
      {
         templ = gain - ch->pcdata->wasat;
         add_hp = con_app[get_curr_con( ch )].hitp + number_range( hp_min( ch ), hp_max( ch ) );
         add_mana = use_mana( ch ) ? number_range( 2, ( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 8 ) : 0;
         add_blood = number_range( 2, ( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 8 );
         add_move = number_range( 5, ( get_curr_con( ch ) + get_curr_dex( ch ) ) / 4 );
         add_prac = wis_app[get_curr_wis( ch )].practice + 2;

         if( ch->level == 100 )
         {
            add_hp = add_hp / 2;
            add_mana = add_mana / 2;
            add_blood = add_blood / 2;
            add_move = add_move / 2;
            add_prac = add_prac / 2;
         }

         add_hp = UMAX( 1, add_hp );
         add_hp = ( add_hp * templ );
         add_mana = UMAX( 0, add_mana );
         add_mana = ( add_mana * templ );
         add_move = UMAX( 10, add_move );
         add_move = ( add_move * templ );
         add_prac = UMAX( 0, add_prac );
         add_prac = ( add_prac * templ );
         if( IS_VAMPIRE( ch ) )
         {
            add_blood = UMAX( 0, add_blood );
            add_blood = ( add_blood * templ );
         }
         ch->pcdata->wasat = 0;
      }
      else
      {
         add_hp = 0;
         add_mana = 0;
         add_blood = 0;
         add_move = 0;
         add_prac = 0;
         ch->pcdata->wasat--;
      }
   }

   if( !IS_NPC( ch ) )
      xREMOVE_BIT( ch->act, PLR_BOUGHT_PET );

   tlevels++;
   if( ch->sublevel < 1000 )
   {
      if( IS_VAMPIRE( ch ) )
         sprintf( buf,
                  "Your gain is: %d/%d hp, %d/%d bp, %d/%d mv %d/%d prac.\n\r",
                  add_hp, ch->max_hit, add_blood, ch->max_blood, add_move, ch->max_move, add_prac, ch->practice );
      else
         sprintf( buf,
                  "Your gain is: %d/%d hp, %d/%d mana, %d/%d mv %d/%d prac.\n\r",
                  add_hp, ch->max_hit, add_mana, ch->max_mana, add_move, ch->max_move, add_prac, ch->practice );
      set_char_color( AT_WHITE, ch );
      if( broadcast );
      send_to_char( buf, ch );

      if( is_affected( ch, gsn_karaaura ) )
      {
         affect_strip( ch, gsn_karaaura );
         send_to_char( "Kara's Aura leaves your soul feeling empty.\n\r", ch );
      }
      if( broadcast && !IS_IMMORTAL( ch ) )
      {
         sprintf( buf, "&R<&BLEVEL INFO&R> %s has just attained power level %d!", ch->name, ch->sublevel );
         talk_info( AT_BLUE, buf );
      }

      for( crier = first_char; crier != NULL; crier = crier_next )
      {
         crier_next = crier->next;

         if( !IS_NPC( crier ) )
            continue;
         if( crier->spec_fun == spec_lookup( "spec_crier" ) )
            break;
      }

      if( crier == NULL || crier->spec_fun != spec_lookup( "spec_crier" ) )
      {
         bug( "NO CRIER.\n\r", 0 );
      }
      else
      {
         sprintf( buf3, "%s on reaching sublevel %d!!", ch->name, ch->sublevel );
         do_gratz( crier, buf3 );
      }
      if( xIS_SET( ch->act, PLR_BOOSTED ) )
      {
         xREMOVE_BIT( ch->act, PLR_BOOSTED );
         ch->mod_str = 0;
         ch->mod_int = 0;
         ch->mod_wis = 0;
         ch->mod_dex = 0;
         ch->mod_con = 0;
         ch->mod_cha = 0;
         ch->mod_lck = 0;
         send_to_char( "&pYou feel Nicole's presence fade.&D\n\r", ch );
      }

      if( IS_SET( sysdata.save_flags, SV_LEVEL ) )
         save_char_obj( ch );

      if( !IS_NPC( ch ) )
      {
         sprintf( buf2, "&G%-13s  ->&w%-2d  &G-&w  %-5d&G   Rvnum: %-5d   %s %s",
                  ch->name,
                  ch->sublevel,
                  get_age( ch ),
                  ch->in_room == NULL ? 0 : ch->in_room->vnum,
                  capitalize( race_table[ch->race]->race_name ), class_table[ch->class]->who_name );
         append_to_file( PLEVEL_FILE, buf2 );
      }
   }
   return;
}

void advance_level_down( CHAR_DATA * ch, bool broadcast )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   int add_hp;
   int add_mana;
   int add_blood;
   int add_move;
   int add_prac;

   if( xIS_SET( ch->act, PLR_A_TITLE ) )
   {
      sprintf( buf, "the %s", title_table[ch->class][ch->level][ch->sex == SEX_FEMALE ? 1 : 0] );
      set_title( ch, buf );
   }

   add_hp = con_app[get_curr_con( ch )].hitp + number_range( hp_min( ch ), hp_max( ch ) );
   add_mana = use_mana( ch ) ? number_range( 2, ( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 8 ) : 0;
   add_blood = number_range( 2, ( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 8 );
   add_move = number_range( 5, ( get_curr_con( ch ) + get_curr_dex( ch ) ) / 4 );
   add_prac = wis_app[get_curr_wis( ch )].practice + 2;

   add_hp = UMAX( 1, add_hp );
   add_mana = UMAX( 0, add_mana );
   add_move = UMAX( 10, add_move );
   if( IS_VAMPIRE( ch ) )
      add_blood = UMAX( 0, add_blood );

   if( IS_PKILL( ch ) )
   {
      add_mana = add_mana + add_mana * .3;
      add_move = add_move + add_move * .3;
      add_hp += 1;
      sprintf( buf, "Gravoc's Pandect steels your sinews.\n\r" );
   }
   if( ch->level == 100 && ( ch->class == CLASS_WARRIOR || ch->class == CLASS_THIEF ) )
   {
      add_mana = 500;
   }

   ch->max_hit -= add_hp;
   ch->max_mana -= add_mana;
   ch->max_move -= add_move;
   ch->practice -= add_prac;
   if( IS_VAMPIRE( ch ) )
      ch->max_blood -= add_blood;

   if( !IS_NPC( ch ) )
      xREMOVE_BIT( ch->act, PLR_BOUGHT_PET );

   if( ch->level < LEVEL_IMMORTAL )
   {
      if( IS_VAMPIRE( ch ) )
         sprintf( buf,
                  "Your loss is: %d/%d hp, %d/%d bp, %d/%d mv %d/%d prac.\n\r",
                  add_hp, ch->max_hit, add_blood, ch->max_blood, add_move, ch->max_move, add_prac, ch->practice );
      else
         sprintf( buf,
                  "Your loss is: %d/%d hp, %d/%d mana, %d/%d mv %d/%d prac.\n\r",
                  add_hp, ch->max_hit, add_mana, ch->max_mana, add_move, ch->max_move, add_prac, ch->practice );
      set_char_color( AT_WHITE, ch );
      send_to_char( buf, ch );

      if( ch->level == 3 )
      {
         ch->mod_str = 3;
         ch->mod_int = 3;
         ch->mod_wis = 3;
         ch->mod_dex = 3;
         ch->mod_con = 3;
         ch->mod_cha = 3;
         ch->mod_lck = 3;
         send_to_char( "&pYou feel Nicole's presence again.&D\n\r", ch );
      }

      if( broadcast && !IS_IMMORTAL( ch ) )
      {
         sprintf( buf, "&R<&BLEVEL INFO&R> %s has just reached level %d!", ch->name, ch->level );
         talk_info( AT_BLUE, buf );
      }

      if( IS_SET( sysdata.save_flags, SV_LEVEL ) )
         save_char_obj( ch );

      if( !IS_NPC( ch ) )
      {
         sprintf( buf2, "&G%-13s  ->&w%-2d  &G-&w  %-5d&G   Rvnum: %-5d   %s %s",
                  ch->name,
                  ch->level,
                  get_age( ch ),
                  ch->in_room == NULL ? 0 : ch->in_room->vnum,
                  capitalize( race_table[ch->race]->race_name ), class_table[ch->class]->who_name );
         append_to_file( PLEVEL_FILE, buf2 );
      }
   }
   return;
}

void advance_slevel_down( CHAR_DATA * ch, bool broadcast, int gain )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   int add_hp;
   int add_mana;
   int add_blood;
   int add_move;
   int add_prac;

   add_hp = con_app[get_curr_con( ch )].hitp + number_range( hp_min( ch ), hp_max( ch ) );
   add_mana = use_mana( ch ) ? number_range( 2, ( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 8 ) : 0;
   add_blood = number_range( 2, ( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 8 );
   add_move = number_range( 5, ( get_curr_con( ch ) + get_curr_dex( ch ) ) / 4 );
   add_prac = wis_app[get_curr_wis( ch )].practice + 2;

   if( ch->level == 100 )
   {
      add_hp = add_hp / 2;
      add_mana = add_mana / 2;
      add_blood = add_blood / 2;
      add_move = add_move / 2;
      add_prac = add_prac / 2;
   }

   add_hp = UMAX( 1, add_hp );
   add_hp = ( add_hp * gain );
   add_mana = UMAX( 0, add_mana );
   add_mana = ( add_mana * gain );
   add_move = UMAX( 10, add_move );
   add_move = ( add_move * gain );
   if( IS_VAMPIRE( ch ) )
   {
      add_blood = UMAX( 0, add_blood );
      add_blood = ( add_blood * gain );
   }

   if( IS_PKILL( ch ) )
   {
      add_mana = add_mana + add_mana * .3;
      add_move = add_move + add_move * .3;
      add_hp += 1;
      sprintf( buf, "Gravoc's Pandect steels your sinews.\n\r" );
   }

   ch->max_hit -= add_hp;
   ch->max_mana -= add_mana;
   ch->max_move -= add_move;
   ch->practice -= add_prac;
   if( IS_VAMPIRE( ch ) )
      ch->max_blood -= add_blood;

   if( !IS_NPC( ch ) )
      xREMOVE_BIT( ch->act, PLR_BOUGHT_PET );

   if( ch->sublevel < 1000 )
   {
      if( IS_VAMPIRE( ch ) )
         sprintf( buf,
                  "Your loss is: %d/%d hp, %d/%d bp, %d/%d mv %d/%d prac.\n\r",
                  add_hp, ch->max_hit, add_blood, ch->max_blood, add_move, ch->max_move, add_prac, ch->practice );
      else
         sprintf( buf,
                  "Your loss is: %d/%d hp, %d/%d mana, %d/%d mv %d/%d prac.\n\r",
                  add_hp, ch->max_hit, add_mana, ch->max_mana, add_move, ch->max_move, add_prac, ch->practice );
      set_char_color( AT_WHITE, ch );
      if( broadcast );
      send_to_char( buf, ch );

      if( broadcast && !IS_IMMORTAL( ch ) )
      {
         sprintf( buf, "&R<&BLEVEL INFO&R> %s has just attained power level %d!", ch->name, ch->sublevel );
         talk_info( AT_BLUE, buf );
      }

      if( IS_SET( sysdata.save_flags, SV_LEVEL ) )
         save_char_obj( ch );

      if( !IS_NPC( ch ) )
      {
         sprintf( buf2, "&G%-13s  ->&w%-2d  &G-&w  %-5d&G   Rvnum: %-5d   %s %s",
                  ch->name,
                  ch->sublevel,
                  get_age( ch ),
                  ch->in_room == NULL ? 0 : ch->in_room->vnum,
                  capitalize( race_table[ch->race]->race_name ), class_table[ch->class]->who_name );
         append_to_file( PLEVEL_FILE, buf2 );
      }
   }
   return;
}

void gain_exp( CHAR_DATA * ch, int gain )
{
   int modgain;
   int neggain;
   char buf[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) || ch->level >= LEVEL_MAXGAIN || NOT_AUTHED( ch ) || ch->tmplevel != 0 )
      return;

   modgain = gain;

//    modgain *= ( race_table[ch->race]->exp_multiplier/200.0);

   modgain *= 100 / 200.0;

   if( ch->level < 100 )
   {
      modgain = UMIN( modgain, exp_level( ch, ch->level + 2 ) - exp_level( ch, ch->level + 1 ) );
   }
   else
   {
      modgain = UMIN( modgain, exp_level( ch, ch->sublevel + 2 ) - exp_level( ch, ch->sublevel + 1 ) );
   }

   if( NOT_AUTHED( ch ) && ch->exp >= exp_level( ch, ch->level + 1 ) )
   {
      send_to_char( "You can not ascend to a higher level until you are authorized.\n\r", ch );
      ch->exp = ( exp_level( ch, ( ch->level + 1 ) ) - 1 );
      return;
   }

   if( modgain < 0 )
   {
      neggain = ( modgain + modgain - modgain );
      sprintf( buf, "&RYou lost %d experience points.&D\n\r", neggain );
   }
   else
   {
      sprintf( buf, "&RYou receive %d experience points.&D\n\r", modgain );
   }
   send_to_char( buf, ch );

   ch->exp = UMAX( 0, ch->exp + modgain );

   if( ch->level < 100 )
   {
      while( ch->level < LEVEL_HERO && ch->exp >= exp_level( ch, ch->level + 1 ) )
      {
         if( ch->level == 100 )
         {
            return;
         }
         set_char_color( AT_WHITE, ch );
         ch_printf( ch, "You have now obtained experience level %d!\n\r", ++ch->level );
         advance_level( ch, TRUE );
         if( ch->level == 100 )
         {
//          do_help( ch, "NEW_HERO" );
            ch->sublevel = 1;
            ch->exp = exp_level( ch, ch->sublevel );
            return;
         }
      }
      return;
   }
   else if( ch->level == 100 )
   {
      while( ch->sublevel <= SUB_HERO && ch->exp >= exp_level( ch, ch->sublevel + 1 ) )
      {
         if( ch->sublevel == 999 )
         {
            return;
         }
         set_char_color( AT_WHITE, ch );
         ch_printf( ch, "You have now obtained experience level %d!\n\r", ++ch->sublevel );
         advance_slevel( ch, TRUE, 1 );
      }
      return;
   }
   else if( ch->level == 200 )
   {
      while( ch->sublevel <= SUB_AVATAR && ch->exp >= exp_level( ch, ch->sublevel + 1 ) )
      {
         if( ch->sublevel == 999 )
         {
            return;
         }
         set_char_color( AT_WHITE, ch );
         ch_printf( ch, "You have now obtained experience level %d!\n\r", ++ch->sublevel );
         advance_slevel( ch, TRUE, 1 );
      }
      return;
   }
   else if( ch->level == 300 )
   {
      while( ch->sublevel <= SUB_NEOPHYTE && ch->exp >= exp_level( ch, ch->sublevel + 1 ) )
      {
         if( ch->sublevel == 999 )
         {
            return;
         }
         set_char_color( AT_WHITE, ch );
         ch_printf( ch, "You have now obtained experience level %d!\n\r", ++ch->sublevel );
         advance_slevel( ch, TRUE, 1 );
      }
      return;
   }
   else if( ch->level == 400 )
   {
      while( ch->sublevel <= SUB_ACOLYTE && ch->exp >= exp_level( ch, ch->sublevel + 1 ) )
      {
         if( ch->sublevel == 999 )
         {
            return;
         }
         set_char_color( AT_WHITE, ch );
         ch_printf( ch, "You have now obtained experience level %d!\n\r", ++ch->sublevel );
         advance_slevel( ch, TRUE, 1 );
      }
      return;
   }

   return;
}

int hit_gain( CHAR_DATA * ch )
{
   int gain;

   if( IS_NPC( ch ) )
   {
      gain = ch->level * 3 / 2;
   }
   else
   {
      gain = 160;

      if( IS_AFFECTED( ch, AFF_REGEN ) )
         gain = gain * 2.5;

      switch ( ch->position )
      {
         case POS_DEAD:
            return 0;
         case POS_MORTAL:
            return -1;
         case POS_INCAP:
            return -1;
         case POS_STUNNED:
            return 1;
         case POS_FIGHTING:
            return 1;
         case POS_AGGRESSIVE:
            return 1;
         case POS_BERSERK:
            return 1;
         case POS_DEFENSIVE:
            return 1;
         case POS_EVASIVE:
            return 1;
         case POS_STANDING:
            return 1;
         case POS_SLEEPING:
            gain += get_curr_con( ch );
            break;
         case POS_RESTING:
            gain += get_curr_con( ch ) / 2;
            break;
      }

      if( LEARNED( ch, gsn_fast_healing5 ) )
      {
         gain = gain * 10;
      }
      else if( LEARNED( ch, gsn_fast_healing4 ) )
      {
         gain = gain * 8;
      }
      else if( LEARNED( ch, gsn_fast_healing3 ) )
      {
         gain = gain * 6;
      }
      else if( LEARNED( ch, gsn_fast_healing2 ) )
      {
         gain = gain * 4;
      }
      else if( LEARNED( ch, gsn_fast_healing ) )
      {
         gain = gain * 2;
      }

      if( IS_VAMPIRE( ch ) )
      {
         if( IS_OUTSIDE( ch ) )
         {
            switch ( time_info.sunlight )
            {
               case SUN_RISE:
               case SUN_SET:
                  gain /= 2;
                  break;
               case SUN_LIGHT:
                  gain /= 4;
                  break;
            }
         }
      }

   }

   if( IS_AFFECTED( ch, AFF_POISON ) || IS_AFFECTED( ch, AFF_BLACKFLAME ) )
      gain /= 4;

// gain = ( ( gain * race_table[ch->race]->hp_regen ) / 100 );

   if( gain > ch->max_hit - ch->hit )
   {
      gain = ch->max_hit - ch->hit;
   }
   if( !IS_NPC( ch ) && sysdata.showhitgains == TRUE )
   {
      sprintf( log_buf, "%s healed %d hp.", ch->name, gain );
      log_string( log_buf );
   }
   return gain;
}

int mana_gain( CHAR_DATA * ch )
{
   int gain;

   if( IS_NPC( ch ) )
   {
      gain = ch->level;
   }
   else
   {
      gain = 80;

      if( IS_AFFECTED( ch, AFF_REGEN ) )
         gain = gain * 1.5;

      switch ( ch->position )
      {
         case POS_DEAD:
            return 0;
         case POS_MORTAL:
            return -1;
         case POS_INCAP:
            return -1;
         case POS_STUNNED:
            return 1;
         case POS_FIGHTING:
            return 1;
         case POS_AGGRESSIVE:
            return 1;
         case POS_BERSERK:
            return 1;
         case POS_DEFENSIVE:
            return 1;
         case POS_EVASIVE:
            return 1;
         case POS_STANDING:
            return 1;
         case POS_SLEEPING:
            gain += get_curr_int( ch );
            break;
         case POS_RESTING:
            gain += get_curr_int( ch ) / 2;
            break;
      }

      if( LEARNED( ch, gsn_meditation5 ) )
      {
         gain = gain * 10;
      }
      else if( LEARNED( ch, gsn_meditation4 ) )
      {
         gain = gain * 8;
      }
      else if( LEARNED( ch, gsn_meditation3 ) )
      {
         gain = gain * 6;
      }
      else if( LEARNED( ch, gsn_meditation2 ) )
      {
         gain = gain * 4;
      }
      else if( LEARNED( ch, gsn_meditation ) )
      {
         gain = gain * 2;
      }

   }

   if( IS_AFFECTED( ch, AFF_POISON ) || IS_AFFECTED( ch, AFF_BLACKFLAME ) )
      gain /= 4;

// gain = ( ( gain * race_table[ch->race]->mana_regen ) / 100 );

   if( gain > ch->max_mana - ch->mana )
   {
      gain = ch->max_mana - ch->mana;
   }
   return gain;
}

int blood_gain( CHAR_DATA * ch )
{
   int gain;

   if( IS_NPC( ch ) )
   {
      gain = ch->level;
   }
   else
   {
      gain = 80;

      if( IS_AFFECTED( ch, AFF_REGEN ) )
         gain = gain * 1.5;

      switch ( ch->position )
      {
         case POS_DEAD:
            return 0;
         case POS_MORTAL:
            return -1;
         case POS_INCAP:
            return -1;
         case POS_STUNNED:
            return 1;
         case POS_FIGHTING:
            return 1;
         case POS_AGGRESSIVE:
            return 1;
         case POS_BERSERK:
            return 1;
         case POS_DEFENSIVE:
            return 1;
         case POS_EVASIVE:
            return 1;
         case POS_STANDING:
            return 1;
         case POS_SLEEPING:
            gain += get_curr_int( ch );
            break;
         case POS_RESTING:
            gain += get_curr_int( ch ) / 2;
            break;
      }

   }

   if( LEARNED( ch, gsn_meditation5 ) )
   {
      gain = gain * 10;
   }
   else if( LEARNED( ch, gsn_meditation4 ) )
   {
      gain = gain * 8;
   }
   else if( LEARNED( ch, gsn_meditation3 ) )
   {
      gain = gain * 6;
   }
   else if( LEARNED( ch, gsn_meditation2 ) )
   {
      gain = gain * 4;
   }
   else if( LEARNED( ch, gsn_meditation ) )
   {
      gain = gain * 2;
   }

   if( IS_AFFECTED( ch, AFF_POISON ) || IS_AFFECTED( ch, AFF_BLACKFLAME ) )
      gain /= 4;

// gain = ( ( gain * race_table[ch->race]->mana_regen ) / 100 );

   if( gain > ch->max_blood - ch->blood )
   {
      gain = ch->max_blood - ch->blood;
   }
   return gain;
}

int move_gain( CHAR_DATA * ch )
{
   int gain;

   if( IS_NPC( ch ) )
   {
      gain = ch->level;
   }
   else
   {
      gain = 100;

      if( IS_AFFECTED( ch, AFF_REGEN ) )
         gain = gain * 2.5;

      switch ( ch->position )
      {
         case POS_DEAD:
            return 0;
         case POS_MORTAL:
            return -1;
         case POS_INCAP:
            return -1;
         case POS_STUNNED:
            return 1;
         case POS_FIGHTING:
            return 1;
         case POS_AGGRESSIVE:
            return 1;
         case POS_BERSERK:
            return 1;
         case POS_DEFENSIVE:
            return 1;
         case POS_EVASIVE:
            return 1;
         case POS_STANDING:
            return 1;
         case POS_SLEEPING:
            gain += get_curr_dex( ch );
            break;
         case POS_RESTING:
            gain += get_curr_dex( ch ) / 2;
            break;
      }

      if( IS_VAMPIRE( ch ) )
      {
         if( IS_OUTSIDE( ch ) )
         {
            switch ( time_info.sunlight )
            {
               case SUN_RISE:
               case SUN_SET:
                  gain /= 2;
                  break;
               case SUN_LIGHT:
                  gain /= 4;
                  break;
            }
         }
      }

   }

   if( IS_AFFECTED( ch, AFF_POISON ) || IS_AFFECTED( ch, AFF_BLACKFLAME ) )
      gain /= 4;

   if( gain > ch->max_move - ch->move )
   {
      gain = ch->max_move - ch->move;
   }
   return gain;
}


void gain_condition( CHAR_DATA * ch, int iCond, int value )
{
   int condition;
   ch_ret retcode = rNONE;

   if( value == 0 || IS_NPC( ch ) || ch->level >= LEVEL_IMMORTAL || NOT_AUTHED( ch ) )
      return;

   condition = ch->pcdata->condition[iCond];
   if( iCond == COND_BLOODTHIRST )
      ch->pcdata->condition[iCond] = URANGE( 0, condition + value, ch->max_blood );
   else
      ch->pcdata->condition[iCond] = URANGE( 0, condition + value, 48 );

   if( ch->pcdata->condition[iCond] == 0 )
   {
      switch ( iCond )
      {
         case COND_BLOODTHIRST:
            if( ch->level < LEVEL_AVATAR )
            {
               set_char_color( AT_BLOOD, ch );
               send_to_char( "You are starved to feast on blood!\n\r", ch );
               act( AT_BLOOD, "$n is suffering from lack of blood!", ch, NULL, NULL, TO_ROOM );
               retcode = damage( ch, ch, ch->max_hit / 20, TYPE_UNDEFINED );
            }
            break;
         case COND_DRUNK:
            if( condition != 0 )
            {
               set_char_color( AT_SOBER, ch );
               send_to_char( "You are sober.\n\r", ch );
            }
            retcode = rNONE;
            break;
         default:
            bug( "Gain_condition: invalid condition type %d", iCond );
            retcode = rNONE;
            break;
      }
   }

   if( retcode != rNONE )
      return;

   if( ch->pcdata->condition[iCond] == 1 )
   {
      switch ( iCond )
      {
         case COND_BLOODTHIRST:
            if( ch->level < LEVEL_AVATAR )
            {
               set_char_color( AT_BLOOD, ch );
               send_to_char( "You have a growing need to feast on blood!\n\r", ch );
               act( AT_BLOOD, "$n gets a strange look in $s eyes...", ch, NULL, NULL, TO_ROOM );
            }
            break;
         case COND_DRUNK:
            if( condition != 0 )
            {
               set_char_color( AT_SOBER, ch );
               send_to_char( "You are feeling a little less light headed.\n\r", ch );
            }
            break;
      }
   }


   if( ch->pcdata->condition[iCond] == 2 )
   {
      switch ( iCond )
      {
         case COND_BLOODTHIRST:
            if( ch->level < LEVEL_AVATAR )
            {
               set_char_color( AT_BLOOD, ch );
               send_to_char( "You feel an urgent need for blood.\n\r", ch );
            }
            break;
      }
   }

   if( ch->pcdata->condition[iCond] == 3 )
   {
      switch ( iCond )
      {
         case COND_BLOODTHIRST:
            if( ch->level < LEVEL_AVATAR )
            {
               set_char_color( AT_BLOOD, ch );
               send_to_char( "You feel an aching in your fangs.\n\r", ch );
            }
            break;
      }
   }


   return;
}


void check_alignment( CHAR_DATA * ch )
{
   if( ch->alignment < race_table[ch->race]->minalign )
   {
      set_char_color( AT_BLOOD, ch );
      send_to_char( "Your actions have been incompatible with the ideals of your race.  This troubles you.", ch );
   }

   if( ch->alignment > race_table[ch->race]->maxalign )
   {
      set_char_color( AT_BLOOD, ch );
      send_to_char( "Your actions have been incompatible with the ideals of your race.  This troubles you.", ch );
   }

   if( ch->class == CLASS_NEPHANDI )
   {
      if( ch->alignment > -250 )
      {
         set_char_color( AT_BLOOD, ch );
         send_to_char( "Damn you heathen! Go forth and do evil or suffer the consequences!\n\r", ch );
         return;
      }
   }

   if( ch->class == CLASS_PALADIN )
   {
      if( ch->alignment < 250 )
      {
         set_char_color( AT_BLOOD, ch );
         send_to_char( "You are wracked with guilt and remorse for your craven actions!\n\r", ch );
         act( AT_BLOOD, "$n prostrates $mself, seeking forgiveness from $s Lord.", ch, NULL, NULL, TO_ROOM );
         return;
      }
      if( ch->alignment < 500 )
      {
         set_char_color( AT_BLOOD, ch );
         send_to_char( "As you betray your faith, your mind begins to betray you.\n\r", ch );
         act( AT_BLOOD, "$n shudders, judging $s actions unworthy of a Paladin.", ch, NULL, NULL, TO_ROOM );
         return;
      }
   }
}

void mobile_update( void )
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *ch;
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   EXIT_DATA *pexit;
   int door;
   ch_ret retcode;

   retcode = rNONE;

   for( ch = last_char; ch; ch = gch_prev )
   {
      set_cur_char( ch );
      if( ch == first_char && ch->prev )
      {
         bug( "mobile_update: first_char->prev != NULL... fixed", 0 );
         ch->prev = NULL;
      }

      gch_prev = ch->prev;

      if( gch_prev && gch_prev->next != ch )
      {
         sprintf( buf, "FATAL: Mobile_update: %s->prev->next doesn't point to ch.", ch->name );
         bug( buf, 0 );
         bug( "Short-cutting here", 0 );
         gch_prev = NULL;
         ch->prev = NULL;
         do_shout( ch, "Nicole says, 'Prepare for the worst!'" );
      }

      if( !IS_NPC( ch ) )
      {
         drunk_randoms( ch );
         continue;
      }

      if( !ch->in_room || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) || IS_AFFECTED( ch, AFF_PARALYSIS ) )
         continue;

      if( IS_AFFECTED( ch, AFF_BLACKFLAME ) )
      {
         if( ch->in_room->first_person )
            act( AT_FIRE, "$n sweats and suffers.", ch, NULL, NULL, TO_ROOM );
         damage( ch, ch, 16, gsn_blackflame );
         continue;
      }

      if( ch->pIndexData->vnum == 5 && ( !IS_AFFECTED( ch, AFF_CHARM ) || !IS_AFFECTED( ch, AFF_JMT ) ) )
      {
         if( ch->in_room->first_person )
            act( AT_MAGIC, "$n returns to the dust from whence $e came.", ch, NULL, NULL, TO_ROOM );

         if( IS_NPC( ch ) )
            extract_char( ch, TRUE );
         continue;
      }

      if( !xIS_SET( ch->act, ACT_RUNNING ) && !xIS_SET( ch->act, ACT_SENTINEL ) && !ch->fighting && ch->hunting )
      {
         WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
         hunt_victim( ch );
         continue;
      }

      if( !xIS_SET( ch->act, ACT_RUNNING ) && ch->spec_fun )
      {
         if( ( *ch->spec_fun ) ( ch ) )
            continue;
         if( char_died( ch ) )
            continue;
      }

      if( HAS_PROG( ch->pIndexData, SCRIPT_PROG ) )
      {
         mprog_script_trigger( ch );
         continue;
      }

      if( ch != cur_char )
      {
         bug( "Mobile_update: ch != cur_char after spec_fun", 0 );
         continue;
      }

      if( ch->position != POS_STANDING )
         continue;

      if( xIS_SET( ch->act, ACT_MOUNTED ) )
      {
         if( xIS_SET( ch->act, ACT_AGGRESSIVE )
             /*
              * ||   xIS_SET(ch->act, ACT_META_AGGR)
              */  )
            do_emote( ch, "snarls and growls." );
         continue;
      }

      if( ch->timer > 0 )
      {
         ch->timer--;
         if( ch->timer == 0 )
         {
            if( xIS_SET( ch->act, ACT_DRAGOON ) )
            {
               act( AT_YELLOW, "All of a sudden, $n rears back and flies off into the horizon.", ch, NULL, NULL, TO_ROOM );
               for( vch = ch->in_room->first_person; vch; vch = vch_next )
               {
                  vch_next = vch->next_in_room;
                  if( vch->mount && vch->mount == ch )
                  {
                     xREMOVE_BIT( vch->mount->act, ACT_MOUNTED );
                     vch->mount = NULL;
                     vch->position = POS_STANDING;
                     send_to_char( "No longer upon your mount, you fall to the ground...\n\rOUCH!\n\r", vch );
                  }
               }
            }
            extract_char( ch, TRUE );
            continue;
         }
      }

      if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
      {
         if( ( xIS_SET( ch->act, ACT_AGGRESSIVE ) /*|| xIS_SET(ch->act, ACT_META_AGGR) */  )
             && sysdata.fight_lock == 1 )
            do_emote( ch, "glares around and snarls." );
      }


      if( ch->in_room->area->nplayer > 0 )
      {
         mprog_random_trigger( ch );
         if( char_died( ch ) )
            continue;
         if( ch->position < POS_STANDING )
            continue;
      }

      mprog_hour_trigger( ch );

      if( char_died( ch ) )
         continue;

      rprog_hour_trigger( ch );
      if( char_died( ch ) )
         continue;

      if( ch->position < POS_STANDING )
         continue;

      if( xIS_SET( ch->act, ACT_SCAVENGER ) && ch->in_room->first_content && number_bits( 2 ) == 0 )
      {
         OBJ_DATA *obj;
         OBJ_DATA *obj_best;
         int max;

         max = 1;
         obj_best = NULL;
         for( obj = ch->in_room->first_content; obj; obj = obj->next_content )
         {
            if( CAN_WEAR( obj, ITEM_TAKE ) && obj->cost > max && !IS_OBJ_STAT( obj, ITEM_BURIED ) )
            {
               obj_best = obj;
               max = obj->cost;
            }
         }

         if( obj_best )
         {
            obj_from_room( obj_best );
            obj_to_char( obj_best, ch );
            act( AT_ACTION, "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
         }
      }

      if( !xIS_SET( ch->act, ACT_RUNNING )
          && !xIS_SET( ch->act, ACT_SENTINEL )
          && !xIS_SET( ch->act, ACT_PROTOTYPE )
          && ( door = number_bits( 5 ) ) <= 9
          && ( pexit = get_exit( ch->in_room, door ) ) != NULL
          && pexit->to_room
          && !IS_SET( pexit->exit_info, EX_CLOSED )
          && !xIS_SET( pexit->to_room->room_flags, ROOM_NO_MOB )
          && !xIS_SET( pexit->to_room->room_flags, ROOM_DEATH )
          && ( !xIS_SET( ch->act, ACT_STAY_AREA ) || pexit->to_room->area == ch->in_room->area ) )
      {
         retcode = move_char( ch, pexit, 0 );
         if( char_died( ch ) )
            continue;
         if( retcode != rNONE || xIS_SET( ch->act, ACT_SENTINEL ) || ch->position < POS_STANDING )
            continue;
      }

      if( ch->hit < ch->max_hit / 2
          && ( door = number_bits( 4 ) ) <= 9
          && ( pexit = get_exit( ch->in_room, door ) ) != NULL
          && pexit->to_room
          && !IS_SET( pexit->exit_info, EX_CLOSED ) && !xIS_SET( pexit->to_room->room_flags, ROOM_NO_MOB ) )
      {
         CHAR_DATA *rch;
         bool found;

         found = FALSE;
         for( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
         {
            if( is_fearing( ch, rch ) )
            {
               switch ( number_bits( 2 ) )
               {
                  case 0:
                     sprintf( buf, "Get away from me, %s!", rch->name );
                     break;
                  case 1:
                     sprintf( buf, "Leave me be, %s!", rch->name );
                     break;
                  case 2:
                     sprintf( buf, "%s is trying to kill me!  Help!", rch->name );
                     break;
                  case 3:
                     sprintf( buf, "Someone save me from %s!", rch->name );
                     break;
               }
               do_yell( ch, buf );
               found = TRUE;
               break;
            }
         }
         if( found )
            retcode = move_char( ch, pexit, 0 );
      }
   }

   return;
}

void char_update( void )
{
//    char buf[MAX_STRING_LENGTH];
   CHAR_DATA *ch;
   CHAR_DATA *ch_save;
   sh_int save_count = 0;

   ch_save = NULL;
   for( ch = last_char; ch; ch = gch_prev )
   {
      if( ch == first_char && ch->prev )
      {
         bug( "char_update: first_char->prev != NULL... fixed", 0 );
         ch->prev = NULL;
      }
      gch_prev = ch->prev;
      set_cur_char( ch );
      if( gch_prev && gch_prev->next != ch )
      {
         bug( "char_update: ch->prev->next != ch", 0 );
         return;
      }

      if( !IS_NPC( ch ) )
         rprog_random_trigger( ch );

      if( char_died( ch ) )
         continue;

      if( IS_NPC( ch ) )
      {
         mprog_time_trigger( ch );
      }

      if( char_died( ch ) )
         continue;

      rprog_time_trigger( ch );

      if( char_died( ch ) )
         continue;

      if( !IS_NPC( ch )
          && ( !ch->desc || ch->desc->connected == CON_PLAYING )
          && ch->level >= 2 && current_time - ch->save_time > ( sysdata.save_frequency * 60 ) )
         ch_save = ch;
      else
         ch_save = NULL;

      if( ch->position >= POS_STUNNED )
      {
         if( ch->hit < ch->max_hit )
            ch->hit += hit_gain( ch );

         if( ch->mana < ch->max_mana )
            ch->mana += mana_gain( ch );

         if( ch->blood < ch->max_blood )
            ch->blood += blood_gain( ch );

         if( ch->move < ch->max_move )
            ch->move += move_gain( ch );
      }

      if( ch->position == POS_STUNNED )
         update_pos( ch );

      if( ch->morph )
      {
         if( ch->morph->timer > 0 )
         {
            ch->morph->timer--;
            if( ch->morph->timer == 0 )
               do_unmorph_char( ch );
         }
      }

      if( ch->morph && !IS_IMMORTAL( ch ) )
      {
         if( ch->morph->timer > 0 )
         {
            ch->morph->timer--;
         }
         if( ch->morph->timer == 0 )
         {
            if( IS_AFFECTED( ch, AFF_SHAPESHIFT ) )
            {
               affect_strip( ch, gsn_shapeshift );
               xREMOVE_BIT( ch->affected_by, AFF_SHAPESHIFT );
            }
            do_unmorph_char( ch );
         }
      }

      if( !IS_NPC( ch ) && ch->pcdata->nuisance )
      {
         long int temp;

         if( ch->pcdata->nuisance->flags < MAX_NUISANCE_STAGE )
         {
            temp = ch->pcdata->nuisance->max_time - ch->pcdata->nuisance->time;
            temp *= ch->pcdata->nuisance->flags;
            temp /= MAX_NUISANCE_STAGE;
            temp += ch->pcdata->nuisance->time;
            if( temp < current_time )
               ch->pcdata->nuisance->flags++;
         }
      }

      if( !IS_NPC( ch ) && ch->level < LEVEL_IMMORTAL )
      {
         OBJ_DATA *obj;

         if( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] > 0 )
         {
            if( --obj->value[2] == 0 && ch->in_room )
            {
               ch->in_room->light -= obj->count;
               if( ch->in_room->light < 0 )
                  ch->in_room->light = 0;
               act( AT_ACTION, "$p goes out.", ch, obj, NULL, TO_ROOM );
               act( AT_ACTION, "$p goes out.", ch, obj, NULL, TO_CHAR );
               if( obj->serial == cur_obj )
                  global_objcode = rOBJ_EXPIRED;
               extract_obj( obj );
            }
         }


         if( ++ch->timer >= 12 )
         {
            if( !IS_IMMORTAL( ch ) || !IS_NPC( ch ) )
            {
               if( !IS_IDLE( ch ) )
               {
                  if( ch->fighting )
                     stop_fighting( ch, TRUE );
                  act( AT_ACTION, "$n disappears into the void.", ch, NULL, NULL, TO_ROOM );
                  send_to_char( "You disappear into the void.\n\r", ch );
                  if( IS_SET( sysdata.save_flags, SV_IDLE ) )
                     save_char_obj( ch );
                  SET_BIT( ch->pcdata->flags, PCFLAG_IDLE );
                  char_from_room( ch );
                  char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
               }
            }
         }
         if( ch->pcdata->condition[COND_DRUNK] > 8 )
            check_alignment( ch );
         gain_condition( ch, COND_DRUNK, -1 );
         if( !IS_NPC( ch ) && ch->pcdata->nuisance )
         {
            int value;

            value = ( ( 0 - ch->pcdata->nuisance->flags ) * ch->pcdata->nuisance->power );
            gain_condition( ch, COND_THIRST, value );
            gain_condition( ch, COND_FULL, --value );
         }
      }
      if( !IS_NPC( ch ) && !IS_IMMORTAL( ch ) && ch->pcdata->release_date > 0 && ch->pcdata->release_date <= current_time )
      {
         ROOM_INDEX_DATA *location;
         if( ch->pcdata->clan )
            location = get_room_index( ch->pcdata->clan->recall );
         else
            location = get_room_index( ROOM_VNUM_TEMPLE );
         if( !location )
            location = ch->in_room;
         MOBtrigger = FALSE;
         char_from_room( ch );
         char_to_room( ch, location );
         send_to_char( "The gods have released you from hell as your sentance is up!\n\r", ch );
         do_look( ch, "auto" );
         STRFREE( ch->pcdata->helled_by );
         ch->pcdata->helled_by = NULL;
         ch->pcdata->release_date = 0;
         xREMOVE_BIT( ch->act, PLR_SILENCE );
         save_char_obj( ch );
      }
      if( !IS_NPC( ch ) && !IS_IMMORTAL( ch ) && ch->pcdata->silence_release_date > 0 &&
          ch->pcdata->silence_release_date <= current_time )
      {
         send_to_char( "You are now able to speak again. \n\r", ch );
         xREMOVE_BIT( ch->act, PLR_SILENCE );
         ch->pcdata->silence_release_date = 0;
         save_char_obj( ch );
      }
      if( !char_died( ch ) )
      {
         OBJ_DATA *arrow = NULL;
         int dam = 0;

         if( ( arrow = get_eq_char( ch, WEAR_LODGE_RIB ) ) != NULL )
         {
            dam = number_range( ( 2 * arrow->value[1] ), ( 2 * arrow->value[2] ) );
            act( AT_CARNAGE, "$n suffers damage from $p stuck in $s rib.", ch, arrow, NULL, TO_ROOM );
            act( AT_CARNAGE, "You suffer damage from $p stuck in your rib.", ch, arrow, NULL, TO_CHAR );
            damage( ch, ch, dam, TYPE_UNDEFINED );
         }
         else if( ( arrow = get_eq_char( ch, WEAR_LODGE_LEG ) ) != NULL )
         {
            dam = number_range( arrow->value[1], arrow->value[2] );
            act( AT_CARNAGE, "$n suffers damage from $p stuck in $s leg.", ch, arrow, NULL, TO_ROOM );
            act( AT_CARNAGE, "You suffer damage from $p stuck in your leg.", ch, arrow, NULL, TO_CHAR );
            damage( ch, ch, dam, TYPE_UNDEFINED );
         }
         else if( ( arrow = get_eq_char( ch, WEAR_LODGE_ARM ) ) != NULL )
         {
            dam = number_range( arrow->value[1], arrow->value[2] );
            act( AT_CARNAGE, "$n suffers damage from $p stuck in $s arm.", ch, arrow, NULL, TO_ROOM );
            act( AT_CARNAGE, "You suffer damage from $p stuck in your arm.", ch, arrow, NULL, TO_CHAR );
            damage( ch, ch, dam, TYPE_UNDEFINED );
         }

         if( char_died( ch ) )
            continue;
      }

      if( !char_died( ch ) )
      {
         if( IS_AFFECTED( ch, AFF_PLAGUE ) )
         {
            act( AT_POISON, "$n convulses and suffers.", ch, NULL, NULL, TO_ROOM );
            act( AT_POISON, "You convulse and suffer.", ch, NULL, NULL, TO_CHAR );
            damage( ch, ch, 16, gsn_plague );
            plague( ch );
         }
         else if( IS_AFFECTED( ch, AFF_POISON ) )
         {
            act( AT_POISON, "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
            act( AT_POISON, "You shiver and suffer.", ch, NULL, NULL, TO_CHAR );
            damage( ch, ch, 6, gsn_poison );
         }
         else if( IS_AFFECTED( ch, AFF_BLACKFLAME ) )
         {
            act( AT_FIRE, "$n sweats and suffers.", ch, NULL, NULL, TO_ROOM );
            act( AT_FIRE, "You sweat and suffer.", ch, NULL, NULL, TO_CHAR );
            damage( ch, ch, 16, gsn_blackflame );
         }
         else if( ch->position == POS_INCAP )
            damage( ch, ch, 1, TYPE_UNDEFINED );
         else if( ch->position == POS_MORTAL )
            damage( ch, ch, 4, TYPE_UNDEFINED );
         if( char_died( ch ) )
            continue;

         if( IS_AFFECTED( ch, AFF_RECURRINGSPELL ) )
         {
            AFFECT_DATA *paf, *paf_next;
            SKILLTYPE *skill;
            bool found = FALSE, died = FALSE;

            for( paf = ch->first_affect; paf; paf = paf_next )
            {
               paf_next = paf->next;
               if( paf->location == APPLY_RECURRINGSPELL )
               {
                  found = TRUE;
                  if( IS_VALID_SN( paf->modifier )
                      && ( skill = skill_table[paf->modifier] ) != NULL && skill->type == SKILL_SPELL )
                  {
                     if( ( *skill->spell_fun ) ( paf->modifier, ch->level, ch, ch ) == rCHAR_DIED || char_died( ch ) )
                     {
                        died = TRUE;
                        break;
                     }
                  }
               }
            }
            if( died )
               continue;
            if( !found )
               xREMOVE_BIT( ch->affected_by, AFF_RECURRINGSPELL );
         }
         if( ch->timer > 24 )
            char_quit( ch, FALSE, "auto" );
         else if( ch == ch_save && IS_SET( sysdata.save_flags, SV_AUTO ) && ++save_count < 10 )
            save_char_obj( ch );
      }
   }
   if( global_exp-- > 1 )
   {
      display++;

/*      if (display >= 3)
      {        
        sprintf( buf, "&R<&BINFO&R> &CThere are %d ticks of double exp left.", global_exp);
        talk_info( AT_PLAIN, buf );    
      }*/
   }
   if( global_exp == 0 )
   {
//        sprintf( buf, "&R<&BINFO&R> &CDouble exp has run out!.&w&D");
//        talk_info( AT_PLAIN, buf );
      log_string( "Double exp has run out." );
      double_exp = FALSE;
      display = 0;
   }

   if( global2_exp-- > 1 )
   {
      display++;

/*      if (display >= 3)
      {        
        sprintf( buf, "&R<&BINFO&R> &CThere are %d ticks of half exp left.", global2_exp);
        talk_info( AT_PLAIN, buf );    
      }*/
   }

   if( global2_exp == 0 )
   {
//        sprintf( buf, "&R<&BINFO&R> &CHalf exp has run out!.&w&D");
//        talk_info( AT_PLAIN, buf );
      log_string( "Half exp has run out." );
      half_exp = FALSE;
      display = 0;
   }

   return;
}

void obj_update( void )
{
   OBJ_DATA *obj;
   sh_int AT_TEMP;

   for( obj = last_object; obj; obj = gobj_prev )
   {
      CHAR_DATA *rch;
      char *message;

      if( obj == first_object && obj->prev )
      {
         bug( "obj_update: first_object->prev != NULL... fixed", 0 );
         obj->prev = NULL;
      }
      gobj_prev = obj->prev;
      if( gobj_prev && gobj_prev->next != obj )
      {
         bug( "obj_update: obj->prev->next != obj", 0 );
         return;
      }
      set_cur_obj( obj );
      if( obj->carried_by )
         oprog_random_trigger( obj );
      else if( obj->in_room && obj->in_room->area->nplayer > 0 )
         oprog_random_trigger( obj );

      if( obj_extracted( obj ) )
         continue;

      if( obj->item_type == ITEM_PIPE )
      {
         if( IS_SET( obj->value[3], PIPE_LIT ) )
         {
            if( --obj->value[1] <= 0 )
            {
               obj->value[1] = 0;
               REMOVE_BIT( obj->value[3], PIPE_LIT );
            }
            else if( IS_SET( obj->value[3], PIPE_HOT ) )
               REMOVE_BIT( obj->value[3], PIPE_HOT );
            else
            {
               if( IS_SET( obj->value[3], PIPE_GOINGOUT ) )
               {
                  REMOVE_BIT( obj->value[3], PIPE_LIT );
                  REMOVE_BIT( obj->value[3], PIPE_GOINGOUT );
               }
               else
                  SET_BIT( obj->value[3], PIPE_GOINGOUT );
            }
            if( !IS_SET( obj->value[3], PIPE_LIT ) )
               SET_BIT( obj->value[3], PIPE_FULLOFASH );
         }
         else
            REMOVE_BIT( obj->value[3], PIPE_HOT );
      }


      if( obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC )
      {
         sh_int timerfrac = UMAX( 1, obj->timer - 1 );
         if( obj->item_type == ITEM_CORPSE_PC )
            timerfrac = ( int )( obj->timer / 8 + 1 );

         if( obj->timer > 0 && obj->value[2] > timerfrac )
         {
            char buf[MAX_STRING_LENGTH];
            char name[MAX_STRING_LENGTH];
            char *bufptr;
            bufptr = one_argument( obj->short_descr, name );
            bufptr = one_argument( bufptr, name );
            bufptr = one_argument( bufptr, name );

            separate_obj( obj );
            obj->value[2] = timerfrac;
            sprintf( buf, corpse_descs[UMIN( timerfrac - 1, 4 )], bufptr );

            STRFREE( obj->description );
            obj->description = STRALLOC( buf );
         }
      }

      if( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
         continue;

      if( IS_OBJ_STAT( obj, ITEM_GROUNDROT ) && !obj->in_room )
         continue;

      if( ( obj->timer <= 0 || --obj->timer > 0 ) )
         continue;

      AT_TEMP = AT_PLAIN;
      switch ( obj->item_type )
      {
         default:
            message = "$p mysteriously vanishes.";
            AT_TEMP = AT_PLAIN;
            break;
         case ITEM_CONTAINER:
            message = "$p falls apart, tattered from age.";
            AT_TEMP = AT_OBJECT;
            break;
         case ITEM_PORTAL:
            message = "$p unravels and winks from existence.";
            remove_portal( obj );
            obj->item_type = ITEM_TRASH;
            AT_TEMP = AT_MAGIC;
            break;
         case ITEM_FOUNTAIN:
            message = "$p dries up.";
            AT_TEMP = AT_BLUE;
            break;
         case ITEM_CORPSE_NPC:
            message = "$p decays into dust and blows away.";
            AT_TEMP = AT_OBJECT;
            break;
         case ITEM_CORPSE_PC:
            message = "$p is sucked into a swirling vortex of colors...";
            AT_TEMP = AT_MAGIC;
            break;
         case ITEM_COOK:
         case ITEM_FOOD:
            message = "$p is devoured by a swarm of maggots.";
            AT_TEMP = AT_HUNGRY;
            break;
         case ITEM_BLOOD:
            message = "$p slowly seeps into the ground.";
            AT_TEMP = AT_BLOOD;
            break;
         case ITEM_BLOODSTAIN:
            message = "$p dries up into flakes and blows away.";
            AT_TEMP = AT_BLOOD;
            break;
         case ITEM_SCRAPS:
            message = "$p crumble and decay into nothing.";
            AT_TEMP = AT_OBJECT;
            break;
         case ITEM_FIRE:
            message = "$p burns out.";
            AT_TEMP = AT_FIRE;
      }

      if( obj->carried_by )
      {
         act( AT_TEMP, message, obj->carried_by, obj, NULL, TO_CHAR );
      }
      else if( obj->in_room && ( rch = obj->in_room->first_person ) != NULL && !IS_OBJ_STAT( obj, ITEM_BURIED ) )
      {
         act( AT_TEMP, message, rch, obj, NULL, TO_ROOM );
         act( AT_TEMP, message, rch, obj, NULL, TO_CHAR );
      }

      if( obj->serial == cur_obj )
      {
         global_objcode = rOBJ_EXPIRED;
      }
/*	if ( xIS_SET(obj->in_room->room_flags, ROOM_SAFETY))
	{
		xREMOVE_BIT(obj->in_room->room_flags, ROOM_SAFETY);
	}*/
      extract_obj( obj );
   }
   return;
}

void char_check( void )
{
   CHAR_DATA *ch, *ch_next;
   OBJ_DATA *obj;
   EXIT_DATA *pexit;
   static int cnt = 0;
   int door, retcode;
// char cmd[1024];

   cnt = ( cnt + 1 ) % SECONDS_PER_TICK;

   for( ch = first_char; ch; ch = ch_next )
   {
      set_cur_char( ch );
      ch_next = ch->next;
      will_fall( ch, 0 );

      if( char_died( ch ) )
         continue;

      if( IS_NPC( ch ) )
      {
         if( ( cnt & 1 ) )
            continue;

         if( xIS_SET( ch->act, ACT_RUNNING ) )
         {
            if( !xIS_SET( ch->act, ACT_SENTINEL ) && !ch->fighting && ch->hunting )
            {
               WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
               hunt_victim( ch );
               continue;
            }

            if( ch->spec_fun )
            {
               if( ( *ch->spec_fun ) ( ch ) )
                  continue;
               if( char_died( ch ) )
                  continue;
            }

            if( !xIS_SET( ch->act, ACT_SENTINEL )
                && !xIS_SET( ch->act, ACT_PROTOTYPE )
                && ( door = number_bits( 4 ) ) <= 9
                && ( pexit = get_exit( ch->in_room, door ) ) != NULL
                && pexit->to_room
                && !IS_SET( pexit->exit_info, EX_CLOSED )
                && !xIS_SET( pexit->to_room->room_flags, ROOM_NO_MOB )
                && !xIS_SET( pexit->to_room->room_flags, ROOM_DEATH )
                && ( !xIS_SET( ch->act, ACT_STAY_AREA ) || pexit->to_room->area == ch->in_room->area ) )
            {
               retcode = move_char( ch, pexit, 0 );
               if( char_died( ch ) )
                  continue;
               if( retcode != rNONE || xIS_SET( ch->act, ACT_SENTINEL ) || ch->position < POS_STANDING )
                  continue;
            }
         }
         continue;
      }
      else
      {
         if( ch->mount && ch->in_room != ch->mount->in_room )
         {
            xREMOVE_BIT( ch->mount->act, ACT_MOUNTED );
            ch->mount = NULL;
            ch->position = POS_STANDING;
            send_to_char( "No longer upon your mount, you fall to the ground...\n\rOUCH!\n\r", ch );
         }
/*
		if ( xIS_SET( ch->act, PLR_PINGME ) )
		{
			sprintf( cmd, "ping %s", ch->desc->host );
			system( cmd );
		}
*/
         if( ( ch->in_room && ch->in_room->sector_type == SECT_UNDERWATER )
             || ( ch->in_room && ch->in_room->sector_type == SECT_OCEANFLOOR ) )
         {
            if( !IS_AFFECTED( ch, AFF_AQUA_BREATH ) )
            {
               if( ch->level < LEVEL_IMMORTAL )
               {
                  int dam;

                  dam = number_range( ch->max_hit / 100, ch->max_hit / 50 );
                  dam = UMAX( 1, dam );
                  if( number_bits( 3 ) == 0 )
                     send_to_char( "You cough and choke as you try to breathe water!\n\r", ch );
                  damage( ch, ch, dam, TYPE_UNDEFINED );
               }
            }
         }

         if( char_died( ch ) )
            continue;

         if( ch->in_room
             && ( ( ch->in_room->sector_type == SECT_WATER_NOSWIM ) || ( ch->in_room->sector_type == SECT_WATER_SWIM ) ) )
         {
            if( !IS_AFFECTED( ch, AFF_FLYING )
                && !IS_AFFECTED( ch, AFF_FLOATING ) && !IS_AFFECTED( ch, AFF_AQUA_BREATH ) && !ch->mount )
            {
               for( obj = ch->first_carrying; obj; obj = obj->next_content )
                  if( obj->item_type == ITEM_BOAT )
                     break;

               if( !obj )
               {
                  if( ch->level < LEVEL_IMMORTAL )
                  {
                     int mov;
                     int dam;

                     if( ch->move > 0 )
                     {
                        mov = number_range( ch->max_move / 20, ch->max_move / 5 );
                        mov = UMAX( 1, mov );

                        if( ch->move - mov < 0 )
                           ch->move = 0;
                        else
                           ch->move -= mov;
                     }
                     else
                     {
                        dam = number_range( ch->max_hit / 20, ch->max_hit / 5 );
                        dam = UMAX( 1, dam );

                        if( number_bits( 3 ) == 0 )
                           send_to_char( "Struggling with exhaustion, you choke on a mouthful of water.\n\r", ch );
                        damage( ch, ch, dam, TYPE_UNDEFINED );
                     }
                  }
               }
            }
         }

         if( !ch->desc )
         {
            CHAR_DATA *wch, *wch_next;

            for( wch = ch->in_room->first_person; wch; wch = wch_next )
            {
               wch_next = wch->next_in_room;

               if( !IS_NPC( wch )
                   || wch->fighting
                   || IS_AFFECTED( wch, AFF_CHARM )
                   || IS_AFFECTED( wch, AFF_JMT )
                   || !IS_AWAKE( wch ) || ( xIS_SET( wch->act, ACT_WIMPY ) && IS_AWAKE( ch ) ) || !can_see( wch, ch ) )
                  continue;

               if( is_hating( wch, ch ) )
               {
                  found_prey( wch, ch );
                  continue;
               }

               if( !xIS_SET( wch->act, ACT_AGGRESSIVE )
                   || xIS_SET( wch->act, ACT_MOUNTED )
                   || xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
                  continue;
               global_retcode = multi_hit( wch, ch, TYPE_UNDEFINED );
            }
         }
      }
   }
}

void aggr_update( void )
{
   DESCRIPTOR_DATA *d, *dnext;
   CHAR_DATA *wch;
   CHAR_DATA *ch;
   CHAR_DATA *ch_next;
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   CHAR_DATA *victim;
   struct act_prog_data *apdtmp;
   int zxr = 60;

#ifdef UNDEFD
   if( IS_NPC( wch ) && wch->mpactnum > 0 && wch->in_room->area->nplayer > 0 )
   {
      MPROG_ACT_LIST *tmp_act, *tmp2_act;
      for( tmp_act = wch->mpact; tmp_act; tmp_act = tmp_act->next )
      {
         oprog_wordlist_check( tmp_act->buf, wch, tmp_act->ch, tmp_act->obj, tmp_act->vo, ACT_PROG );
         DISPOSE( tmp_act->buf );
      }
      for( tmp_act = wch->mpact; tmp_act; tmp_act = tmp2_act )
      {
         tmp2_act = tmp_act->next;
         DISPOSE( tmp_act );
      }
      wch->mpactnum = 0;
      wch->mpact = NULL;
   }
#endif

   if( sysdata.fight_lock == 1 )
   {
      return;
   }

   while( ( apdtmp = mob_act_list ) != NULL )
   {
      wch = mob_act_list->vo;
      if( !char_died( wch ) && wch->mpactnum > 0 )
      {
         MPROG_ACT_LIST *tmp_act;

         while( ( tmp_act = wch->mpact ) != NULL )
         {
            if( tmp_act->obj && obj_extracted( tmp_act->obj ) )
               tmp_act->obj = NULL;
            if( tmp_act->ch && !char_died( tmp_act->ch ) )
               mprog_wordlist_check( tmp_act->buf, wch, tmp_act->ch, tmp_act->obj, tmp_act->vo, ACT_PROG );
            wch->mpact = tmp_act->next;
            DISPOSE( tmp_act->buf );
            DISPOSE( tmp_act );
         }
         wch->mpactnum = 0;
         wch->mpact = NULL;
      }
      mob_act_list = apdtmp->next;
      DISPOSE( apdtmp );
   }

   for( d = first_descriptor; d; d = dnext )
   {
      dnext = d->next;
      if( d->connected != CON_PLAYING || ( wch = d->character ) == NULL )
         continue;

      if( char_died( wch ) || IS_NPC( wch ) || wch->level >= LEVEL_IMMORTAL || !wch->in_room )
         continue;

      for( ch = wch->in_room->first_person; ch; ch = ch_next )
      {
         int count;

         ch_next = ch->next_in_room;

         if( !IS_NPC( ch )
             || ch->fighting
             || IS_AFFECTED( ch, AFF_CHARM )
             || IS_AFFECTED( ch, AFF_JMT )
             || !IS_AWAKE( ch ) || ( xIS_SET( ch->act, ACT_WIMPY ) && IS_AWAKE( wch ) ) || !can_see( ch, wch ) )
            continue;

         if( is_hating( ch, wch ) )
         {
            found_prey( ch, wch );
            continue;
         }

         if( !xIS_SET( ch->act, ACT_AGGRESSIVE )
             || xIS_SET( ch->act, ACT_MOUNTED )
             || xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
            continue;

         count = 0;
         victim = NULL;
         for( vch = wch->in_room->first_person; vch; vch = vch_next )
         {
            vch_next = vch->next_in_room;

            if( ( !IS_NPC( vch )
                  || xIS_SET( vch->act, ACT_ANNOYING ) )
                && vch->level < LEVEL_IMMORTAL
                && ( !xIS_SET( ch->act, ACT_WIMPY ) || !IS_AWAKE( vch ) ) && can_see( ch, vch ) )
            {
               if( number_range( 0, count ) == 0 )
                  victim = vch;
               count++;
            }
         }

         if( !victim )
         {
            bug( "Aggr_update: null victim.", count );
            continue;
         }

         while( zxr > 0 )
         {
            if( zxr == 1 )
            {
               if( IS_NPC( ch ) && xIS_SET( ch->attacks, ATCK_BACKSTAB ) )
               {
                  OBJ_DATA *obj;

                  if( !ch->mount
                      && ( obj = get_eq_char( ch, WEAR_WIELD ) ) != NULL
                      && ( obj->value[4] == WEP_DAGGER ) && !victim->fighting && victim->hit >= victim->max_hit )
                  {
                     check_attacker( ch, victim );
                     WAIT_STATE( ch, skill_table[gsn_backstab]->beats );
                     if( !IS_AWAKE( victim ) || number_percent(  ) + 5 < ch->level )
                     {
                        global_retcode = multi_hit( ch, victim, gsn_backstab );
                        continue;
                     }
                     else
                     {
                        global_retcode = damage( ch, victim, 0, gsn_backstab );
                        continue;
                     }
                  }
               }
               global_retcode = multi_hit( ch, victim, TYPE_UNDEFINED );
            }
            zxr--;
         }
      }
   }

   return;
}

bool check_social args( ( CHAR_DATA * ch, char *command, char *argument ) );

void drunk_randoms( CHAR_DATA * ch )
{
   CHAR_DATA *rvch = NULL;
   CHAR_DATA *vch;
   sh_int drunk;
   sh_int position;

   if( IS_NPC( ch ) || ch->pcdata->condition[COND_DRUNK] <= 0 )
      return;

   if( number_percent(  ) < 30 )
      return;

   drunk = ch->pcdata->condition[COND_DRUNK];
   position = ch->position;
   ch->position = POS_STANDING;

   if( number_percent(  ) < ( 2 * drunk / 20 ) )
      check_social( ch, "burp", "" );
   else if( number_percent(  ) < ( 2 * drunk / 20 ) )
      check_social( ch, "hiccup", "" );
   else if( number_percent(  ) < ( 2 * drunk / 20 ) )
      check_social( ch, "drool", "" );
   else if( number_percent(  ) < ( 2 * drunk / 20 ) )
      check_social( ch, "fart", "" );
   else if( drunk > ( 10 + ( get_curr_con( ch ) / 5 ) ) && number_percent(  ) < ( 2 * drunk / 18 ) )
   {
      for( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
         if( number_percent(  ) < 10 )
            rvch = vch;
      check_social( ch, "puke", ( rvch ? rvch->name : "" ) );
   }

   ch->position = position;
   return;
}

void hallucinations( CHAR_DATA * ch )
{
   if( ch->mental_state >= 30 && number_bits( 5 - ( ch->mental_state >= 50 ) - ( ch->mental_state >= 75 ) ) == 0 )
   {
      char *t;

      switch ( number_range( 1, UMIN( 21, ( ch->mental_state + 5 ) / 5 ) ) )
      {
         default:
         case 1:
            t = "You feel very restless... you can't sit still.\n\r";
            break;
         case 2:
            t = "You're tingling all over.\n\r";
            break;
         case 3:
            t = "Your skin is crawling.\n\r";
            break;
         case 4:
            t = "You suddenly feel that something is terribly wrong.\n\r";
            break;
         case 5:
            t = "Those damn little fairies keep laughing at you!\n\r";
            break;
         case 6:
            t = "You can hear your mother crying...\n\r";
            break;
         case 7:
            t = "Have you been here before, or not?  You're not sure...\n\r";
            break;
         case 8:
            t = "Painful childhood memories flash through your mind.\n\r";
            break;
         case 9:
            t = "You hear someone call your name in the distance...\n\r";
            break;
         case 10:
            t = "Your head is pulsating... you can't think straight.\n\r";
            break;
         case 11:
            t = "The ground... seems to be squirming...\n\r";
            break;
         case 12:
            t = "You're not quite sure what is real anymore.\n\r";
            break;
         case 13:
            t = "It's all a dream... or is it?\n\r";
            break;
         case 14:
            t = "You hear your grandchildren praying for you to watch over them.\n\r";
            break;
         case 15:
            t = "They're coming to get you... coming to take you away...\n\r";
            break;
         case 16:
            t = "You begin to feel all powerful!\n\r";
            break;
         case 17:
            t = "You're light as air... the heavens are yours for the taking.\n\r";
            break;
         case 18:
            t = "Your whole life flashes by... and your future...\n\r";
            break;
         case 19:
            t = "You are everywhere and everything... you know all and are all!\n\r";
            break;
         case 20:
            t = "You feel immortal!\n\r";
            break;
         case 21:
            t = "Ahh... the power of a Supreme Entity... what to do...\n\r";
            break;
      }
      send_to_char( t, ch );
   }
   return;
}

void tele_update( void )
{
   TELEPORT_DATA *tele, *tele_next;

   if( !first_teleport )
      return;

   for( tele = first_teleport; tele; tele = tele_next )
   {
      tele_next = tele->next;
      if( --tele->timer <= 0 )
      {
         if( tele->room->first_person )
         {
            teleport( tele->room->first_person, tele->room->tele_vnum, TELE_TRANSALL );
         }
         UNLINK( tele, first_teleport, last_teleport, next, prev );
         DISPOSE( tele );
      }
   }
}

#if FALSE
void auth_update( void )
{
   CHAR_DATA *victim;
   DESCRIPTOR_DATA *d;
   char log_buf[MAX_INPUT_LENGTH];
   bool first_time = TRUE;

   for( d = first_descriptor; d; d = d->next )
   {
      victim = d->character;
      if( victim && IS_WAITING_FOR_AUTH( victim ) && IS_SET( victim->pcdata->flags, PCFLAG_EXP ) )
      {
         if( first_time )
         {
            first_time = FALSE;
         }
         strcpy( log_buf, "Pending authorizations:" );
         to_channel( log_buf, CHANNEL_AUTH, "Auth", 1, 1 );
         sprintf( log_buf, " %s @ %s new %s %s", victim->name,
                  victim->desc->host, race_table[victim->race]->race_name, class_table[victim->class]->who_name );
         to_channel( log_buf, CHANNEL_AUTH, "Auth", 1, 1 );
      }
   }
}
#endif

void auth_update( void )
{
   CHAR_DATA *victim;
   DESCRIPTOR_DATA *d;
   char buf[MAX_INPUT_LENGTH], log_buf[MAX_INPUT_LENGTH];
   bool found_hit = FALSE;

   strcpy( log_buf, "Pending authorizations:\n\r" );
   for( d = first_descriptor; d; d = d->next )
   {
      if( ( victim = d->character ) && IS_WAITING_FOR_AUTH( victim ) && IS_SET( victim->pcdata->flags, PCFLAG_EXP ) )
      {
         found_hit = TRUE;
         sprintf( buf, " %s @ %s new %s %s\n\r", victim->name,
                  victim->desc->host, race_table[victim->race]->race_name, class_table[victim->class]->who_name );
         strcat( log_buf, buf );
      }
   }
   if( found_hit )
   {
      to_channel( log_buf, CHANNEL_AUTH, "Auth", 1, 1 );
   }
}

void update_handler( void )
{
   static int pulse_area;
   static int pulse_mobile;
   static int pulse_violence;
   static int pulse_point;
   static int pulse_wet;
   static int pulse_second;
   static int pulse_start_arena = PULSE_ARENA;
   static int pulse_arena = PULSE_ARENA;
   struct timeval stime;
   struct timeval etime;

   if( timechar )
   {
      set_char_color( AT_PLAIN, timechar );
      send_to_char( "Starting update timer.\n\r", timechar );
      gettimeofday( &stime, NULL );
   }

   if( --pulse_area <= 0 )
   {
      pulse_area = number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 );
      area_update(  );
   }


   if( --pulse_mobile <= 0 )
   {
      pulse_mobile = PULSE_MOBILE;
      mobile_update(  );
   }

   if( --pulse_violence <= 0 )
   {
      pulse_violence = PULSE_VIOLENCE;
      violence_update(  );
   }

   if( --pulse_point <= 0 )
   {
      pulse_point = number_range( PULSE_TICK * 0.75, PULSE_TICK * 1.25 );

      auth_update(  );
      time_update(  );
      //weather_update  ( );
      char_update(  );
      obj_update(  );
      quest_update(  );
      clear_vrooms(  );
   }

   if( --pulse_wet <= 0 )
   {
      pulse_wet = PULSE_TICK + number_range( 50, 80 );
      weather_update(  );
   }

   if( --pulse_second <= 0 )
   {
      pulse_second = PULSE_PER_SECOND;
      char_check(  );
      check_pfiles( 0 );
      reboot_check( 0 );
   }

   if( --auction->pulse <= 0 )
   {
      auction->pulse = PULSE_AUCTION;
      auction_update(  );
   }

   if( in_start_arena || ppl_challenged )
      if( --pulse_start_arena <= 0 )
      {
         pulse_start_arena = PULSE_ARENA;
         start_arena(  );
      }

   if( ppl_in_arena )
      if( ( --pulse_arena <= 0 ) || ( num_in_arena(  ) == 1 ) )
      {
         pulse_arena = PULSE_ARENA;
         do_game(  );
      }

   mpsleep_update(  );
   tele_update(  );
   aggr_update(  );
   obj_act_update(  );
   room_act_update(  );
   clean_obj_queue(  );
   clean_char_queue(  );
   if( timechar )
   {
      gettimeofday( &etime, NULL );
      set_char_color( AT_PLAIN, timechar );
      send_to_char( "Update timing complete.\n\r", timechar );
      subtract_times( &etime, &stime );
      ch_printf( timechar, "Timing took %d.%06d seconds.\n\r", etime.tv_sec, etime.tv_usec );
      timechar = NULL;
   }
   tail_chain(  );
   return;
}


void remove_portal( OBJ_DATA * portal )
{
   ROOM_INDEX_DATA *fromRoom, *toRoom;
   EXIT_DATA *pexit;
   bool found;

   if( !portal )
   {
      bug( "remove_portal: portal is NULL", 0 );
      return;
   }

   fromRoom = portal->in_room;
   found = FALSE;
   if( !fromRoom )
   {
      bug( "remove_portal: portal->in_room is NULL", 0 );
      return;
   }

   for( pexit = fromRoom->first_exit; pexit; pexit = pexit->next )
      if( IS_SET( pexit->exit_info, EX_PORTAL ) )
      {
         found = TRUE;
         break;
      }

   if( !found )
   {
      bug( "remove_portal: portal not found in room %d!", fromRoom->vnum );
      return;
   }

   if( pexit->vdir != DIR_PORTAL )
      bug( "remove_portal: exit in dir %d != DIR_PORTAL", pexit->vdir );

   if( ( toRoom = pexit->to_room ) == NULL )
      bug( "remove_portal: toRoom is NULL", 0 );

   extract_exit( fromRoom, pexit );

   return;
}

void reboot_check( time_t reset )
{
   static char *tmsg[] = {
      "Prepare yourself, the mud is going to warmboot automatically \n\r"
         "Please find a save place to hide so you do not die"
   };

/*    "You feel the ground shake as the end comes near!", 			*/
/*    "Lightning crackles in the sky above!", 					*/
/*    "Crashes of thunder sound across the land!", 				*/
/*    "The sky has suddenly turned midnight black.", 				*/
/*    "You notice the life forms around you slowly dwindling away.", 		*/
/*    "The seas across the realm have turned frigid.", 				*/
/*    "The aura of magic that surrounds the realms seems slightly unstable.", 	*/
/*    "You sense a change in the magical forces surrounding you." 		*/

/* }; */

   static const int times[] = { 60, 120, 180, 240, 300, 600, 900, 1800 };
   static const int timesize = UMIN( sizeof( times ) / sizeof( *times ), sizeof( tmsg ) / sizeof( *tmsg ) );
   char buf[MAX_STRING_LENGTH];
   static int trun;
   static bool init = FALSE;

   if( sysdata.manualb == 1 )
      return;

   if( !init || reset >= current_time )
   {
      for( trun = timesize - 1; trun >= 0; trun-- )
         if( reset >= current_time + times[trun] )
            break;
      init = TRUE;
      return;
   }

   if( ( current_time % 1800 ) == 0 )
   {
      sprintf( buf, "%.24s: %d players", ctime( &current_time ), num_descriptors );
      append_to_file( USAGE_FILE, buf );
      sprintf( buf, "%.24s:  %dptn  %dpll  %dsc %dbr  %d katyr loot ",
               ctime( &current_time ),
               sysdata.upotion_val,
               sysdata.upill_val, sysdata.scribed_used, sysdata.brewed_used, sysdata.global_gold_looted );
      append_to_file( ECONOMY_FILE, buf );
   }

   if( new_boot_time_t - boot_time < 60 * 60 * 18 && !set_boot_time->manual )
      return;

   if( trun != -1 && new_boot_time_t - current_time <= times[trun] )
   {
      echo_to_all( AT_YELLOW, tmsg[trun], ECHOTAR_ALL );
      if( trun <= 5 )
         sysdata.DENY_NEW_PLAYERS = TRUE;
      --trun;
      return;
   }

   if( new_boot_time_t <= current_time )
   {
      CHAR_DATA *vch;
      extern bool mud_down;

      if( auction->item )
      {
         sprintf( buf, "Sale of %s has been stopped by mud.", auction->item->short_descr );
         talk_auction( buf );
         obj_to_char( auction->item, auction->seller );
         auction->item = NULL;
         if( auction->buyer && auction->buyer != auction->seller )
         {
            auction->buyer->gold += auction->bet;
            send_to_char( "Your money has been returned.\n\r", auction->buyer );
         }
      }

      if( sysdata.warmboot )
      {
         sprintf( buf,
                  ANSI_RED "\n\r<" ANSI_BLUE "INFO" ANSI_RED ">" ANSI_LBLUE " Warmboot of the MUD initiated by The MUD.\n\r"
                  ANSI_GREY );


         copyover( buf );

         sprintf( buf, "&R<&BINFO&R> &CRebooting MUD now! System Warmboot Failed!&D" );
      }
      else
      {
         sprintf( buf, "&R<&BINFO&R> &CRebooting MUD now!&D" );
      }
      echo_to_all( AT_YELLOW, "You are forced from these realms by a strong "
                   "magical presence\n\ras life here is reconstructed.", ECHOTAR_ALL );
      log_string( "Automatic Reboot" );

      talk_info( AT_PLAIN, buf );
      for( vch = first_char; vch; vch = vch->next )
         if( !IS_NPC( vch ) )
         {
            save_char_obj( vch );
//                save_finger(vch);
         }

      mud_down = TRUE;


   }

   return;
}

/*  
void auction_update(void)
{
    int tax, pay;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int gbid = 0;
    int sbid = 0;
    int cbid = 0;
    int wealth = 0;
    int tmpvalue = 0;
    
    if(!auction->item)
    {
    	if(AUCTION_MEM > 0 && auction->history[0] &&
    			++auction->hist_timer == 6*AUCTION_MEM)
    	{
    		int i;
    		
    		for(i = AUCTION_MEM - 1; i >= 0; i--)
    		{
    			if(auction->history[i])
    			{
    				auction->history[i] = NULL;
    				auction->hist_timer = 0;
    				break;
    			}
    		}
    	}
    	return;
    }

    switch (++auction->going) 
    {
	case 1 : 
	case 2 :
	    if (auction->bet > auction->starting){
	    tmpvalue = auction->bet;  
	    gbid = tmpvalue/10000;
	    tmpvalue=tmpvalue%10000;
	    sbid = tmpvalue/100;
	    tmpvalue=tmpvalue%100;		
	    cbid = tmpvalue;
		sprintf (buf, "%s&P: going %s for ", auction->item->short_descr,
			((auction->going == 1) ? "once" : "twice") );
			if( gbid > 0 && sbid > 0 && cbid > 0)
			sprintf( buf2, "&Y%d&P katyr, &Y%d&P siam, and &Y%d&P rona.",gbid,sbid,cbid);
			else if( gbid > 0 && sbid > 0 && cbid <= 0)
			sprintf( buf2, "&Y%d&P katyr and &Y%d&P siam.",gbid,sbid);
			else if( gbid > 0 && sbid <= 0 && cbid > 0)
			sprintf( buf2, "&Y%d&P katyr and &Y%d&P rona.",gbid,cbid);
			else if( gbid > 0 && sbid <= 0 && cbid <= 0)
			sprintf( buf2, "&Y%d&P katyr.",gbid);
			else if( gbid <= 0 && sbid > 0 && cbid <= 0)
			sprintf( buf2, "&Y%d&P siam.",sbid);
			else if( gbid <= 0 && sbid <= 0 && cbid > 0)
			sprintf( buf2, "&Y%d&P rona.",cbid);
			else if( gbid <= 0 && sbid > 0 && cbid > 0)
			sprintf( buf2, "&Y%d&P siam and &Y%d&P rona.",sbid,cbid);
			else sprintf( buf2, "Error in update_auction report to Nicole!");
			strcat(buf,buf2);
	    } else
		sprintf (buf, "%s: going %s (bid not received yet).",  auction->item->short_descr,
			((auction->going == 1) ? "once" : "twice"));

	    talk_auction (buf);
	    break;

	case 3 :
	    if (!auction->buyer && auction->bet)
	    {
		bug( "Auction code reached SOLD, with NULL buyer, but %d katyr bid", auction->bet );
		auction->bet = 0;
	    }
	    if (auction->bet > 0 && auction->buyer != auction->seller)
	    {
	    tmpvalue = auction->bet;  
	    gbid = tmpvalue/10000;
	    tmpvalue=tmpvalue%10000;
	    sbid = tmpvalue/100;
	    tmpvalue=tmpvalue%100;		
	    cbid = tmpvalue;
	    
		sprintf (buf, "%s sold to %s for ",
			auction->item->short_descr,
			IS_NPC(auction->buyer) ? auction->buyer->short_descr : auction->buyer->name);
		if( gbid > 0 && sbid > 0 && cbid > 0)
			sprintf( buf2, "&Y%d&P katyr, &Y%d&P siam, and &Y%d&P rona.",gbid,sbid,cbid);
			else if( gbid > 0 && sbid > 0 && cbid <= 0)
			sprintf( buf2, "&Y%d&P katyr and &Y%d&P siam.",gbid,sbid);
			else if( gbid > 0 && sbid <= 0 && cbid > 0)
			sprintf( buf2, "&Y%d&P katyr and &Y%d&P rona.",gbid,cbid);
			else if( gbid > 0 && sbid <= 0 && cbid <= 0)
			sprintf( buf2, "&Y%d&P katyr.",gbid);
			else if( gbid <= 0 && sbid > 0 && cbid <= 0)
			sprintf( buf2, "&Y%d&P siam.",sbid);
			else if( gbid <= 0 && sbid <= 0 && cbid > 0)
			sprintf( buf2, "&Y%d&P rona.",cbid);
			else if( gbid <= 0 && sbid > 0 && cbid > 0)
			sprintf( buf2, "&Y%d&P siam and &Y%d&P rona.",sbid,cbid);
			else sprintf( buf2, "Error in update_auction report to Nicole!");
			strcat(buf,buf2);
		talk_auction(buf);

		act(AT_ACTION, "The auctioneer materializes before you, and hands you $p.",
			auction->buyer, auction->item, NULL, TO_CHAR);
		act(AT_ACTION, "The auctioneer materializes before $n, and hands $m $p.",
			auction->buyer, auction->item, NULL, TO_ROOM);

		if ( (auction->buyer->carry_weight 
		+     get_obj_weight( auction->item ))
		>     can_carry_w( auction->buyer ) )
		{
		    act( AT_PLAIN, "$p is too heavy for you to carry with your current inventory.", auction->buyer, auction->item, NULL, TO_CHAR );
    		    act( AT_PLAIN, "$n is carrying too much to also carry $p, and $e drops it.", auction->buyer, auction->item, NULL, TO_ROOM );
		    obj_to_room( auction->item, auction->buyer->in_room );
		}
		else
		    obj_to_char( auction->item, auction->buyer );
	        pay = (int)auction->bet * 0.9;
		tax = (int)auction->bet * 0.1;
		tmpvalue = pay;  
	    gbid = tmpvalue/10000;
	    tmpvalue=tmpvalue%10000;
	    sbid = tmpvalue/100;
	    tmpvalue=tmpvalue%100;		
	    cbid = tmpvalue;
		boost_economy( auction->seller->in_room->area, tax );
                auction->seller->gold += gbid; 
                auction->seller->silver += sbid;
                auction->seller->copper += cbid;
		ch_printf( auction->seller, "The auctioneer pays you ");
		if( gbid > 0 && sbid > 0 && cbid > 0)
			ch_printf( auction->seller, "%d katyr, %d siam, and %d rona.\n\r",gbid,sbid,cbid);
			else if( gbid > 0 && sbid > 0 && cbid <= 0)
		ch_printf( auction->seller, "%d katyr and %d siam.\n\r",gbid,sbid);
			else if( gbid > 0 && sbid <= 0 && cbid > 0)
			ch_printf( auction->seller, "%d katyr and %d rona.\n\r",gbid,cbid);
			else if( gbid > 0 && sbid <= 0 && cbid <= 0)
			ch_printf( auction->seller, "%d katyr.\n\r",gbid);
			else if( gbid <= 0 && sbid > 0 && cbid <= 0)
			ch_printf( auction->seller, "%d siam.\n\r",sbid);
			else if( gbid <= 0 && sbid <= 0 && cbid > 0)
		ch_printf( auction->seller, "%d rona.\n\r",cbid);
			else if( gbid <= 0 && sbid > 0 && cbid > 0)
			ch_printf( auction->seller, "%d siam and %d rona.\n\r",sbid,cbid);
			else bug("Error in update_auction report to Nicole!");
		 tmpvalue = tax;  
	    gbid = tmpvalue/10000;
	    tmpvalue=tmpvalue%10000;
	    sbid = tmpvalue/100;
	    tmpvalue=tmpvalue%100;		
	    cbid = tmpvalue;
		 ch_printf(auction->seller,"The auctioneer charged a fee of ");
		if( gbid > 0 && sbid > 0 && cbid > 0)
			ch_printf( auction->seller, "%d katyr, %d siam, and %d rona.\n\r",gbid,sbid,cbid);
			else if( gbid > 0 && sbid > 0 && cbid <= 0)
		  ch_printf( auction->seller, "%d katyr and %d siam.\n\r",gbid,sbid);
			else if( gbid > 0 && sbid <= 0 && cbid > 0)
			ch_printf( auction->seller, "%d katyr and %d rona.\n\r",gbid,cbid);
			else if( gbid > 0 && sbid <= 0 && cbid <= 0)
			ch_printf( auction->seller, "%d katyr.\n\r",gbid);
			else if( gbid <= 0 && sbid > 0 && cbid <= 0)
			ch_printf( auction->seller, "%d siam.\n\r",sbid);
			else if( gbid <= 0 && sbid <= 0 && cbid > 0)
		  ch_printf( auction->seller, "%d rona.\n\r",cbid);
			else if( gbid <= 0 && sbid > 0 && cbid > 0)
			ch_printf( auction->seller, "%d siam and %d rona.\n\r",sbid,cbid);
			else bug("Error in update_auction report to Nicole!");
		
                auction->item = NULL;
		if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
		{
		    save_char_obj( auction->buyer );
		    save_char_obj( auction->seller );
		}
            }
            else 
            {
                sprintf (buf, "No bids received for %s - removed from auction.\n\r",auction->item->short_descr);
                talk_auction(buf);
                act (AT_ACTION, "The auctioneer appears before you to return $p to you.",
                      auction->seller,auction->item,NULL,TO_CHAR);
                act (AT_ACTION, "The auctioneer appears before $n to return $p to $m.",
                      auction->seller,auction->item,NULL,TO_ROOM);
		if ( (auction->seller->carry_weight
		+     get_obj_weight( auction->item ))
		>     can_carry_w( auction->seller ) )
		{
		    act( AT_PLAIN, "You drop $p as it is just too much to carry"
			" with everything else you're carrying.", auction->seller,
			auction->item, NULL, TO_CHAR );
		    act( AT_PLAIN, "$n drops $p as it is too much extra weight"
			" for $m with everything else.", auction->seller,
			auction->item, NULL, TO_ROOM );
		    obj_to_room( auction->item, auction->seller->in_room );
		}
		else
		    obj_to_char (auction->item,auction->seller);
		tax = (int)get_value(auction->item->gold_cost, auction->item->silver_cost, auction->item->copper_cost) * 0.05;
		boost_economy( auction->seller->in_room->area, tax );
		ch_printf(auction->seller, "The auctioneer charges you an auction fee of ");
		tmpvalue = tax;  
	    gbid = tmpvalue/10000;
	    tmpvalue=tmpvalue%10000;
	    sbid = tmpvalue/1000;
	    tmpvalue=tmpvalue%1000;		
	    cbid = tmpvalue;
		if( gbid > 0 && sbid > 0 && cbid > 0)
			ch_printf( auction->seller, "%d katyr, %d siam, and %d rona.\n\r",gbid,sbid,cbid);
			else if( gbid > 0 && sbid > 0 && cbid <= 0)
		  ch_printf( auction->seller, "%d katyr and %d siam.\n\r",gbid,sbid);
			else if( gbid > 0 && sbid <= 0 && cbid > 0)
			ch_printf( auction->seller, "%d katyr and %d rona.\n\r",gbid,cbid);
			else if( gbid > 0 && sbid <= 0 && cbid <= 0)
			ch_printf( auction->seller, "%d katyr.\n\r",gbid);
			else if( gbid <= 0 && sbid > 0 && cbid <= 0)
			ch_printf( auction->seller, "%d siam.\n\r",sbid);
			else if( gbid <= 0 && sbid <= 0 && cbid > 0)
		  ch_printf( auction->seller, "%d rona.\n\r",cbid);
			else if( gbid <= 0 && sbid > 0 && cbid > 0)
			ch_printf( auction->seller, "%d siam and %d rona.\n\r",sbid,cbid);
			else bug("Error in update_auction report to Nicole!");
		wealth = get_value(auction->seller->gold, auction->seller->silver, auction->seller->copper);
		if ((wealth - tax) < 0){
		  auction->seller->gold = 0;
		  auction->seller->silver = 0;
		  auction->seller->copper = 0;
		} 
		else if(auction->seller->gold < gbid 
		   || auction->seller->silver < sbid 
		   || auction->seller->copper < cbid){
    tmpvalue = wealth - tax;
		conv_currency( auction->seller, tmpvalue);
    send_to_char("You hand your coins to the auctioneer who quickly makes change.\n\r",auction->seller);
	  } else {
	  auction->seller->gold -= gbid;
	  auction->seller->silver -= sbid;
	  auction->seller->copper -= cbid;
	  }
		if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
		    save_char_obj( auction->seller );
	    } 
	    auction->item = NULL;
    } 
} 
*/
void auction_update( void )
{
   int tax, pay;
   char buf[MAX_STRING_LENGTH];

   if( !auction->item )
   {
      if( AUCTION_MEM > 0 && auction->history[0] && ++auction->hist_timer == 6 * AUCTION_MEM )
      {
         int i;

         for( i = AUCTION_MEM - 1; i >= 0; i-- )
         {
            if( auction->history[i] )
            {
               auction->history[i] = NULL;
               auction->hist_timer = 0;
               break;
            }
         }
      }
      return;
   }

   switch ( ++auction->going )
   {
      case 1:
      case 2:
         if( auction->bet > auction->starting )
            sprintf( buf, "%s&P: going %s for &Y%s&P.&D", auction->item->short_descr,
                     ( ( auction->going == 1 ) ? "once" : "twice" ), num_punct( auction->bet ) );
         else
            sprintf( buf, "%s&P: going %s &R(&Pbid not received yet&R)&P.&D", auction->item->short_descr,
                     ( ( auction->going == 1 ) ? "once" : "twice" ) );

         talk_auction( buf );
         break;

      case 3:
         if( !auction->buyer && auction->bet )
         {
            bug( "Auction code reached SOLD, with NULL buyer, but %d katyr bid", auction->bet );
            auction->bet = 0;
         }
         if( auction->bet > 0 && auction->buyer != auction->seller )
         {
            sprintf( buf, "%s&P sold to %s for &Y%s&P.&D",
                     auction->item->short_descr,
                     IS_NPC( auction->buyer ) ? auction->buyer->short_descr : auction->buyer->name,
                     num_punct( auction->bet ) );
            talk_auction( buf );

            act( AT_ACTION, "The auctioneer materializes before you, and hands you $p.",
                 auction->buyer, auction->item, NULL, TO_CHAR );
            act( AT_ACTION, "The auctioneer materializes before $n, and hands $m $p.",
                 auction->buyer, auction->item, NULL, TO_ROOM );

            if( ( auction->buyer->carry_weight + get_obj_weight( auction->item ) ) > can_carry_w( auction->buyer ) )
            {
               act( AT_PLAIN, "$p is too heavy for you to carry with your current inventory.", auction->buyer, auction->item,
                    NULL, TO_CHAR );
               act( AT_PLAIN, "$n is carrying too much to also carry $p, and $e drops it.", auction->buyer, auction->item,
                    NULL, TO_ROOM );
               obj_to_room( auction->item, auction->buyer->in_room );
            }
            else
               obj_to_char( auction->item, auction->buyer );
            pay = ( int )auction->bet * 0.9;
            tax = ( int )auction->bet * 0.1;
            boost_economy( auction->seller->in_room->area, tax );
            auction->seller->gold += pay;
            sprintf( buf, "The auctioneer pays you %s katyr, charging an auction fee of %s.\n\r",
                     num_punct( pay ), num_punct( tax ) );
            send_to_char( buf, auction->seller );
            auction->item = NULL;
            if( IS_SET( sysdata.save_flags, SV_AUCTION ) )
            {
               save_char_obj( auction->buyer );
               save_char_obj( auction->seller );
            }
         }
         else
         {
            sprintf( buf, "&PNo bids received for %s &R-&P removed from auction.&D\n\r", auction->item->short_descr );
            talk_auction( buf );
            act( AT_ACTION, "The auctioneer appears before you to return $p to you.",
                 auction->seller, auction->item, NULL, TO_CHAR );
            act( AT_ACTION, "The auctioneer appears before $n to return $p to $m.",
                 auction->seller, auction->item, NULL, TO_ROOM );
            if( ( auction->seller->carry_weight + get_obj_weight( auction->item ) ) > can_carry_w( auction->seller ) )
            {
               act( AT_PLAIN, "You drop $p as it is just too much to carry"
                    " with everything else you're carrying.", auction->seller, auction->item, NULL, TO_CHAR );
               act( AT_PLAIN, "$n drops $p as it is too much extra weight"
                    " for $m with everything else.", auction->seller, auction->item, NULL, TO_ROOM );
               obj_to_room( auction->item, auction->seller->in_room );
            }
            else
               obj_to_char( auction->item, auction->seller );
            tax = ( int )auction->item->cost * 0.05;
            boost_economy( auction->seller->in_room->area, tax );
            sprintf( buf, "The auctioneer charges you an auction fee of %s.\n\r", num_punct( tax ) );
            send_to_char( buf, auction->seller );
            if( ( auction->seller->gold - tax ) < 0 )
               auction->seller->gold = 0;
            else
               auction->seller->gold -= tax;
            if( IS_SET( sysdata.save_flags, SV_AUCTION ) )
               save_char_obj( auction->seller );
         }
         auction->item = NULL;
   }
}

void subtract_times( struct timeval *etime, struct timeval *stime )
{
   etime->tv_sec -= stime->tv_sec;
   etime->tv_usec -= stime->tv_usec;
   while( etime->tv_usec < 0 )
   {
      etime->tv_usec += 1000000;
      etime->tv_sec--;
   }
   return;
}

void adjust_vectors( WEATHER_DATA * weather )
{
   NEIGHBOR_DATA *neigh;
   double dT, dP, dW;

   if( !weather )
   {
      bug( "adjust_vectors: NULL weather data.", 0 );
      return;
   }

   dT = 0;
   dP = 0;
   dW = 0;

   dT += number_range( -rand_factor, rand_factor );
   dP += number_range( -rand_factor, rand_factor );
   dW += number_range( -rand_factor, rand_factor );

   dT += climate_factor * ( ( ( weather->climate_temp - 2 ) * weath_unit ) - ( weather->temp ) ) / weath_unit;
   dP += climate_factor * ( ( ( weather->climate_precip - 2 ) * weath_unit ) - ( weather->precip ) ) / weath_unit;
   dW += climate_factor * ( ( ( weather->climate_wind - 2 ) * weath_unit ) - ( weather->wind ) ) / weath_unit;


   for( neigh = weather->first_neighbor; neigh; neigh = neigh->next )
   {
      if( !neigh->address )
      {
         neigh->address = get_area( neigh->name );

         if( !neigh->address )
         {
            NEIGHBOR_DATA *temp;
            bug( "adjust_weather: " "invalid area name.", 0 );
            temp = neigh->prev;
            UNLINK( neigh, weather->first_neighbor, weather->last_neighbor, next, prev );
            STRFREE( neigh->name );
            DISPOSE( neigh );
            neigh = temp;
            continue;
         }
      }

      dT += ( neigh->address->weather->temp - weather->temp ) / neigh_factor;
      dP += ( neigh->address->weather->precip - weather->precip ) / neigh_factor;
      dW += ( neigh->address->weather->wind - weather->wind ) / neigh_factor;
   }

   weather->temp_vector += ( int )dT;
   weather->precip_vector += ( int )dP;
   weather->wind_vector += ( int )dW;

   weather->temp_vector = URANGE( -max_vector, weather->temp_vector, max_vector );
   weather->precip_vector = URANGE( -max_vector, weather->precip_vector, max_vector );
   weather->wind_vector = URANGE( -max_vector, weather->wind_vector, max_vector );

   return;
}

void weather_update(  )
{
   AREA_DATA *pArea;
   DESCRIPTOR_DATA *d;
   int limit;

   limit = 3 * weath_unit;

   for( pArea = first_area; pArea; pArea = ( pArea == last_area ) ? first_build : pArea->next )
   {
      pArea->weather->temp += pArea->weather->temp_vector;
      pArea->weather->precip += pArea->weather->precip_vector;
      pArea->weather->wind += pArea->weather->wind_vector;

      pArea->weather->temp = URANGE( -limit, pArea->weather->temp, limit );
      pArea->weather->precip = URANGE( -limit, pArea->weather->precip, limit );
      pArea->weather->wind = URANGE( -limit, pArea->weather->wind, limit );

      get_weather_echo( pArea->weather );
   }

   for( pArea = first_area; pArea; pArea = ( pArea == last_area ) ? first_build : pArea->next )
   {
      adjust_vectors( pArea->weather );
   }

   for( d = first_descriptor; d; d = d->next )
   {
      WEATHER_DATA *weath;

      if( d->connected == CON_PLAYING &&
          IS_OUTSIDE( d->character ) && !IS_BLIND( d->character ) && IS_AWAKE( d->character ) )
      {
         weath = d->character->in_room->area->weather;
         if( !weath->echo )
            continue;
         set_char_color( weath->echo_color, d->character );
         ch_printf( d->character, weath->echo );
      }
   }

   return;
}

void get_weather_echo( WEATHER_DATA * weath )
{
   int n;
   int temp, precip, wind;
   int dT, dP, dW;
   int tindex, pindex, windex;

   weath->echo = NULL;
   weath->echo_color = AT_GREY;

   n = number_bits( 2 );

   temp = weath->temp;
   precip = weath->precip;
   wind = weath->wind;

   dT = weath->temp_vector;
   dP = weath->precip_vector;
   dW = weath->wind_vector;

   tindex = ( temp + 3 * weath_unit - 1 ) / weath_unit;
   pindex = ( precip + 3 * weath_unit - 1 ) / weath_unit;
   windex = ( wind + 3 * weath_unit - 1 ) / weath_unit;

   switch ( pindex )
   {
      case 0:
         if( precip - dP > -2 * weath_unit )
         {
            char *echo_strings[4] = {
               "The clouds disappear.\n\r",
               "The clouds disappear.\n\r",
               "The sky begins to break through " "the clouds.\n\r",
               "The clouds are slowly " "evaporating.\n\r"
            };

            weath->echo = echo_strings[n];
            weath->echo_color = AT_WHITE;
         }
         break;

      case 1:
         if( precip - dP <= -2 * weath_unit )
         {
            char *echo_strings[4] = {
               "The sky is getting cloudy.\n\r",
               "The sky is getting cloudy.\n\r",
               "Light clouds cast a haze over " "the sky.\n\r",
               "Billows of clouds spread through " "the sky.\n\r"
            };
            weath->echo = echo_strings[n];
            weath->echo_color = AT_GREY;
         }
         break;

      case 2:
         if( precip - dP > 0 )
         {
            if( tindex > 1 )
            {
               char *echo_strings[4] = {
                  "The rain stops.\n\r",
                  "The rain stops.\n\r",
                  "The rainstorm tapers " "off.\n\r",
                  "The rain's intensity " "breaks.\n\r"
               };
               weath->echo = echo_strings[n];
               weath->echo_color = AT_CYAN;
            }
            else
            {
               char *echo_strings[4] = {
                  "The snow stops.\n\r",
                  "The snow stops.\n\r",
                  "The snow showers taper " "off.\n\r",
                  "The snow flakes disappear " "from the sky.\n\r"
               };
               weath->echo = echo_strings[n];
               weath->echo_color = AT_WHITE;
            }
         }
         break;

      case 3:
         if( precip - dP <= 0 )
         {
            if( tindex > 1 )
            {
               char *echo_strings[4] = {
                  "It starts to rain.\n\r",
                  "It starts to rain.\n\r",
                  "A droplet of rain falls " "upon you.\n\r",
                  "The rain begins to " "patter.\n\r"
               };
               weath->echo = echo_strings[n];
               weath->echo_color = AT_CYAN;
            }
            else
            {
               char *echo_strings[4] = {
                  "It starts to snow.\n\r",
                  "It starts to snow.\n\r",
                  "Crystal flakes begin to " "fall from the " "sky.\n\r",
                  "Snow flakes drift down " "from the clouds.\n\r"
               };
               weath->echo = echo_strings[n];
               weath->echo_color = AT_WHITE;
            }
         }
         else if( tindex < 2 && temp - dT > -weath_unit )
         {
            char *echo_strings[4] = {
               "The temperature drops and the rain " "becomes a light snow.\n\r",
               "The temperature drops and the rain " "becomes a light snow.\n\r",
               "Flurries form as the rain freezes.\n\r",
               "Large snow flakes begin to fall " "with the rain.\n\r"
            };
            weath->echo = echo_strings[n];
            weath->echo_color = AT_WHITE;
         }
         else if( tindex > 1 && temp - dT <= -weath_unit )
         {
            char *echo_strings[4] = {
               "The snow flurries are gradually " "replaced by pockets of rain.\n\r",
               "The snow flurries are gradually " "replaced by pockets of rain.\n\r",
               "The falling snow turns to a cold drizzle.\n\r",
               "The snow turns to rain as the air warms.\n\r"
            };
            weath->echo = echo_strings[n];
            weath->echo_color = AT_CYAN;
         }
         break;

      case 4:
         if( precip - dP > 2 * weath_unit )
         {
            if( tindex > 1 )
            {
               char *echo_strings[4] = {
                  "The lightning has stopped.\n\r",
                  "The lightning has stopped.\n\r",
                  "The sky settles, and the " "thunder surrenders.\n\r",
                  "The lightning bursts fade as " "the storm weakens.\n\r"
               };
               weath->echo = echo_strings[n];
               weath->echo_color = AT_GREY;
            }
         }
         else if( tindex < 2 && temp - dT > -weath_unit )
         {
            char *echo_strings[4] = {
               "The cold rain turns to snow.\n\r",
               "The cold rain turns to snow.\n\r",
               "Snow flakes begin to fall " "amidst the rain.\n\r",
               "The driving rain begins to freeze.\n\r"
            };
            weath->echo = echo_strings[n];
            weath->echo_color = AT_WHITE;
         }
         else if( tindex > 1 && temp - dT <= -weath_unit )
         {
            char *echo_strings[4] = {
               "The snow becomes a freezing rain.\n\r",
               "The snow becomes a freezing rain.\n\r",
               "A cold rain beats down on you " "as the snow begins to melt.\n\r",
               "The snow is slowly replaced by a heavy " "rain.\n\r"
            };
            weath->echo = echo_strings[n];
            weath->echo_color = AT_CYAN;
         }
         break;

      case 5:
         if( precip - dP <= 2 * weath_unit )
         {
            if( tindex > 1 )
            {
               char *echo_strings[4] = {
                  "Lightning flashes in the " "sky.\n\r",
                  "Lightning flashes in the " "sky.\n\r",
                  "A flash of lightning splits " "the sky.\n\r",
                  "The sky flashes, and the " "ground trembles with " "thunder.\n\r"
               };
               weath->echo = echo_strings[n];
               weath->echo_color = AT_YELLOW;
            }
         }
         else if( tindex > 1 && temp - dT <= -weath_unit )
         {
            char *echo_strings[4] = {
               "The sky rumbles with thunder as " "the snow changes to rain.\n\r",
               "The sky rumbles with thunder as " "the snow changes to rain.\n\r",
               "The falling turns to freezing rain " "amidst flashes of " "lightning.\n\r",
               "The falling snow begins to melt as " "thunder crashes overhead.\n\r"
            };
            weath->echo = echo_strings[n];
            weath->echo_color = AT_WHITE;
         }
         else if( tindex < 2 && temp - dT > -weath_unit )
         {
            char *echo_strings[4] = {
               "The lightning stops as the rainstorm " "becomes a blinding " "blizzard.\n\r",
               "The lightning stops as the rainstorm " "becomes a blinding " "blizzard.\n\r",
               "The thunder dies off as the " "pounding rain turns to " "heavy snow.\n\r",
               "The cold rain turns to snow and " "the lightning stops.\n\r"
            };
            weath->echo = echo_strings[n];
            weath->echo_color = AT_CYAN;
         }
         break;

      default:
         bug( "echo_weather: invalid precip index" );
         weath->precip = 0;
         break;
   }

   return;
}

void get_time_echo( WEATHER_DATA * weath )
{
   int n;
   int pindex;

   n = number_bits( 2 );
   pindex = ( weath->precip + 3 * weath_unit - 1 ) / weath_unit;
   weath->echo = NULL;
   weath->echo_color = AT_GREY;

   switch ( time_info.hour )
   {
      case 5:
      {
         char *echo_strings[4] = {
            "The day has begun.\n\r",
            "The day has begun.\n\r",
            "The sky slowly begins to glow.\n\r",
            "The sun slowly embarks upon a new day.\n\r"
         };
         time_info.sunlight = SUN_RISE;
         weath->echo = echo_strings[n];
         weath->echo_color = AT_YELLOW;
         break;
      }
      case 6:
      {
         char *echo_strings[4] = {
            "The sun rises in the east.\n\r",
            "The sun rises in the east.\n\r",
            "The hazy sun rises over the horizon.\n\r",
            "Day breaks as the sun lifts into the sky.\n\r"
         };
         time_info.sunlight = SUN_LIGHT;
         weath->echo = echo_strings[n];
         weath->echo_color = AT_ORANGE;
         break;
      }
      case 12:
      {
         if( pindex > 0 )
         {
            weath->echo = "It's noon.\n\r";
         }
         else
         {
            char *echo_strings[2] = {
               "The intensity of the sun " "heralds the noon hour.\n\r",
               "The sun's bright rays beat down " "upon your shoulders.\n\r"
            };
            weath->echo = echo_strings[n % 2];
         }
         time_info.sunlight = SUN_LIGHT;
         weath->echo_color = AT_WHITE;
         break;
      }
      case 19:
      {
         char *echo_strings[4] = {
            "The sun slowly disappears in the west.\n\r",
            "The reddish sun sets past the horizon.\n\r",
            "The sky turns a reddish orange as the sun " "ends its journey.\n\r",
            "The sun's radiance dims as it sinks in the " "sky.\n\r"
         };
         time_info.sunlight = SUN_SET;
         weath->echo = echo_strings[n];
         weath->echo_color = AT_RED;
         break;
      }
      case 20:
      {
         if( pindex > 0 )
         {
            char *echo_strings[2] = {
               "The night begins.\n\r",
               "Twilight descends around you.\n\r"
            };
            weath->echo = echo_strings[n % 2];
         }
         else
         {
            char *echo_strings[2] = {
               "The moon's gentle glow diffuses " "through the night sky.\n\r",
               "The night sky gleams with " "glittering starlight.\n\r"
            };
            weath->echo = echo_strings[n % 2];
         }
         time_info.sunlight = SUN_DARK;
         weath->echo_color = AT_DBLUE;
         break;
      }
   }

   return;
}

void time_update(  )
{
   AREA_DATA *pArea;
   DESCRIPTOR_DATA *d;
   WEATHER_DATA *weath;

   switch ( ++time_info.hour )
   {
      case 5:
      case 6:
      case 12:
      case 19:
      case 20:
         for( pArea = first_area; pArea; pArea = ( pArea == last_area ) ? first_build : pArea->next )
         {
            get_time_echo( pArea->weather );
         }

         for( d = first_descriptor; d; d = d->next )
         {
            if( d->connected == CON_PLAYING && IS_OUTSIDE( d->character ) && IS_AWAKE( d->character ) )
            {
               weath = d->character->in_room->area->weather;
               if( !weath->echo )
                  continue;
               set_char_color( weath->echo_color, d->character );
               ch_printf( d->character, weath->echo );
            }
         }
         break;
      case 24:
         time_info.hour = 0;
         time_info.day++;
         break;
   }

   if( time_info.day >= 30 )
   {
      time_info.day = 0;
      time_info.month++;
   }

   if( time_info.month >= 17 )
   {
      time_info.month = 0;
      time_info.year++;
   }

   return;
}


void plague( CHAR_DATA * ch )
{
   CHAR_DATA *vch;
   AFFECT_DATA af;


   if( number_percent(  ) < 15 )
   {
      for( vch = first_char; vch; vch = vch->next )
      {
         if( vch->in_room == ch->in_room )
         {
            if( !IS_AFFECTED( vch, AFF_PLAGUE ) && vch != ch )
            {
               af.type = gsn_plague;
               af.duration = 83;
               af.location = APPLY_STR;
               af.modifier = -5;
               af.bitvector = meb( AFF_PLAGUE );
               affect_to_char( vch, &af );

               af.type = gsn_plague;
               af.duration = 83;
               af.location = APPLY_CON;
               af.modifier = -5;
               af.bitvector = meb( AFF_PLAGUE );
               affect_to_char( vch, &af );

               set_char_color( AT_GREEN, vch );
               send_to_char( "You feel very sick.\n\r", vch );
               if( ch != vch )
               {
                  act( AT_GREEN, "$N shivers as your plague rips $S body.", ch, NULL, vch, TO_CHAR );
                  act( AT_GREEN, "$N shivers as $n's plague rips $S body.", ch, NULL, vch, TO_NOTVICT );
               }
            }
         }
      }
   }
   return;
}
