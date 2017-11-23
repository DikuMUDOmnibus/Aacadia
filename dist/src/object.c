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
/*					   Statistic module		*/
/************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "acadia.h"
#include "bet.h"



void show_list_to_char args( ( OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing ) );
void write_corpses args( ( CHAR_DATA * ch, char *name, OBJ_DATA * objrem ) );

void get_obj args( ( CHAR_DATA * ch, OBJ_DATA * obj, OBJ_DATA * container ) );
bool remove_obj args( ( CHAR_DATA * ch, int iWear, bool fReplace ) );
void wear_obj args( ( CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace, sh_int wear_bit, bool fAll ) );

char *get_chance_verb args( ( OBJ_DATA * obj ) );
char *get_ed_number args( ( OBJ_DATA * obj, int number ) );

OBJ_DATA *recursive_note_find args( ( OBJ_DATA * obj, char *argument ) );



typedef struct gaso_struct GASO_STRUCT;

struct gaso_struct
{
   sh_int weight;
   int cost;
   int value[6];
   int count;
   int extra_flags[32];
};

int gaso_level( CHAR_DATA * ch, int level );

void do_gaso( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   int level, level_lo, level_hi, count;

   count = 0;

   if( !ch->desc )
   {
      bug( "No desc in do_gas\n\r", 0 );
   }


   one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      level = -1;
   }
   else
   {
      level = atoi( arg );
   }

   if( ( level < 0 ) || ( level > MAX_LEVEL ) )
   {
      level_lo = level_hi = level;
   }
   else
   {
      level_lo = 1;
      level_hi = LEVEL_AVATAR;
   }

   for( level = level_lo; level <= level_hi; level++ )
      count += gaso_level( ch, level );

   return;
}

int gaso_level( CHAR_DATA * ch, int level )
{
   char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
   int count, type, gcount, i;
   GASO_STRUCT stats[MAX_ITEM_TYPE + 1];  /* duh */
   OBJ_DATA *obj;
   double dcount;

   count = 0;
   gcount = 0;

   for( type = 0; type <= MAX_ITEM_TYPE; type++ )
   {
      stats[type].weight = stats[type].cost = stats[type].count = 0;
      stats[type].value[0] = 0;
      stats[type].value[1] = 0;
      stats[type].value[2] = 0;
      stats[type].value[3] = 0;
      stats[type].value[4] = 0;
      stats[type].value[5] = 0;
      for( i = 0; i < 32; i++ )
      {
         stats[type].extra_flags[i] = 0;
      }
   }

   for( obj = first_object; obj; obj = obj->next )
   {
      if( obj->level == level )
      {
         count++;
         stats[obj->item_type].count++;
         stats[obj->item_type].weight += obj->weight;
         stats[obj->item_type].cost += obj->cost;
         stats[obj->item_type].value[0] += obj->value[0];
         stats[obj->item_type].value[1] += obj->value[1];
         stats[obj->item_type].value[2] += obj->value[2];
         stats[obj->item_type].value[3] += obj->value[3];
         stats[obj->item_type].value[4] += obj->value[4];
         stats[obj->item_type].value[5] += obj->value[5];
         for( i = 0; i < 32; i++ )
         {
            if( xIS_SET( obj->pIndexData->extra_flags, i ) )
               stats[obj->item_type].extra_flags[i]++;
         }

      }
      gcount++;
   }

   if( count == 0 )
   {
      /*
       * send_to_pager("No objects in this range.\n\r",ch); 
       */
      return ( 0 );
   }

   for( type = 0; type <= MAX_ITEM_TYPE; type++ )
   {
      if( stats[type].count != 0 )
      {
#define TODUB(x) (  (double)(1.0 * x) )

         dcount = TODUB( stats[type].count );
         sprintf( buf, "%d,%d,%d,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f", level, type, stats[type].count, ( double )( 100.0 * ( dcount / TODUB( count ) ) ),   /* %-age of objs at this level */
                  ( double )( TODUB( stats[type].weight ) / dcount ),   /* average weight of this item_type for lev */
                  ( double )( TODUB( stats[type].cost ) / dcount ),  /* average cost of this item_type for lev */
                  ( double )( TODUB( stats[type].value[0] ) / dcount ),
                  ( double )( TODUB( stats[type].value[1] ) / dcount ),
                  ( double )( TODUB( stats[type].value[2] ) / dcount ),
                  ( double )( TODUB( stats[type].value[3] ) / dcount ),
                  ( double )( TODUB( stats[type].value[4] ) / dcount ),
                  ( double )( TODUB( stats[type].value[5] ) / dcount ) );
         for( i = 0; i < 32; i++ )
         {
            sprintf( buf2, ",%2.2f", ( double )( TODUB( stats[type].extra_flags[i] ) / dcount ) );
            strcat( buf, buf2 );
         }
         strcat( buf, "\n\r" );
         send_to_pager( buf, ch );
      }

   }

#undef TODUB

   return count;
}

sh_int get_obj_resistance( OBJ_DATA * obj )
{
   sh_int resist;

   resist = number_fuzzy( MAX_ITEM_IMPACT );

   if( IS_OBJ_STAT( obj, ITEM_MAGIC ) )
      resist += number_fuzzy( 12 );
   if( IS_OBJ_STAT( obj, ITEM_METAL ) )
      resist += number_fuzzy( 5 );
   if( IS_OBJ_STAT( obj, ITEM_ORGANIC ) )
      resist -= number_fuzzy( 5 );
   if( IS_OBJ_STAT( obj, ITEM_BLESS ) )
      resist += number_fuzzy( 5 );
//    if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
//      resist += 20;

   resist += ( obj->level / 10 ) - 2;

   if( obj->item_type == ITEM_ARMOR || obj->item_type == ITEM_WEAPON )
      resist += ( obj->value[0] / 2 ) - 2;

   return URANGE( 10, resist, 99 );
}

bool rare_obj( CHAR_DATA * ch, OBJ_DATA * obj )
{
   OBJ_DATA *tobj = NULL;

   if( IS_OBJ_STAT( obj, ITEM_RARE ) && !IS_IMMORTAL( ch ) )
   {
      if( ( tobj = get_obj_here2( ch, obj->name ) ) == NULL )
      {
         return TRUE;
      }
   }
   return FALSE;
}

void get_obj( CHAR_DATA * ch, OBJ_DATA * obj, OBJ_DATA * container )
{
   int weight;
   int amt;
   int lgold = 0;

   if( !CAN_WEAR( obj, ITEM_TAKE ) && ( ch->level < sysdata.level_getobjnotake ) )
   {
      send_to_char( "You can't take that.\n\r", ch );
      return;
   }

   if( IS_SET( obj->magic_flags, ITEM_PKDISARMED ) && !IS_NPC( ch ) )
   {
      if( CAN_PKILL( ch ) && !get_timer( ch, TIMER_PKILLED ) )
      {
         if( ch->level - obj->value[5] > 5 || obj->value[5] - ch->level > 5 )
         {
            send_to_char_color( "\n\r&bA godly force freezes your outstretched hand.\n\r", ch );
            return;
         }
         else
         {
            REMOVE_BIT( obj->magic_flags, ITEM_PKDISARMED );
            obj->value[5] = 0;
         }
      }
      else
      {
         send_to_char_color( "\n\r&BA godly force freezes your outstretched hand.\n\r", ch );
         return;
      }
   }

   if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) && !can_take_proto( ch ) )
   {
      send_to_char( "A godly force prevents you from getting close to it.\n\r", ch );
      return;
   }

   if( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
   {
      act( AT_PLAIN, "$d: you can't carry that many items.", ch, NULL, obj->name, TO_CHAR );
      return;
   }

   if( IS_OBJ_STAT( obj, ITEM_COVERING ) )
      weight = obj->weight;
   else
      weight = get_obj_weight( obj );

   if( ch->carry_weight + weight > can_carry_w( ch ) )
   {
      act( AT_PLAIN, "$d: you can't carry that much weight.", ch, NULL, obj->name, TO_CHAR );
      return;
   }

   if( container )
   {
      if( container->item_type == ITEM_KEYRING && !IS_OBJ_STAT( container, ITEM_COVERING ) )
      {
         act( AT_ACTION, "You remove $p from $P", ch, obj, container, TO_CHAR );
         act( AT_ACTION, "$n removes $p from $P", ch, obj, container, TO_ROOM );
      }
      else
      {
         act( AT_ACTION, IS_OBJ_STAT( container, ITEM_COVERING ) ?
              "You get $p from beneath $P." : "You get $p from $P", ch, obj, container, TO_CHAR );
         act( AT_ACTION, IS_OBJ_STAT( container, ITEM_COVERING ) ?
              "$n gets $p from beneath $P." : "$n gets $p from $P", ch, obj, container, TO_ROOM );
      }
      if( IS_OBJ_STAT( container, ITEM_CLANCORPSE ) && !IS_NPC( ch ) && str_cmp( container->name + 7, ch->name ) )
         container->value[5]++;
      obj_from_obj( obj );
   }
   else
   {
      act( AT_ACTION, "You get $p.", ch, obj, container, TO_CHAR );
      act( AT_ACTION, "$n gets $p.", ch, obj, container, TO_ROOM );
      obj_from_room( obj );
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_CLANSTOREROOM ) && ( !container || container->carried_by == NULL ) )
   {
      save_clan_storeroom( ch );
   }

   if( obj->item_type != ITEM_CONTAINER )
      check_for_trap( ch, obj, TRAP_GET );
   if( char_died( ch ) )
      return;

   if( obj->item_type == ITEM_GOLD )
   {

      amt = obj->value[0];

#ifdef GOLD_MULT
      switch ( ch->race )
      {
         case ( 1 ):
            amt *= 1.1;
            break;
         case ( 2 ):
            amt *= 0.97;
            break;
         case ( 3 ):
            amt *= 1.02;
            break;
         case ( 4 ):
            amt *= 1.08;
            break;
         case ( 6 ):
            amt *= 0.92;
            break;
         case ( 7 ):
            amt *= 0.94;
            break;
         case ( 8 ):
            amt *= 0.90;
            break;
         case ( 9 ):
            amt *= 1.04;
            break;
         case ( 10 ):
            amt *= 1.06;
            break;
      }
#endif

      if( obj->item_type == ITEM_GOLD )
      {
         if( ch->gold == MAX_GOLD )
         {
            send_to_char( "&pYou are carrying the maximum limit of katyr so the katyr is forfeit.&D\n\r", ch );
            lgold = 0;
         }
         else if( amt + ch->gold > MAX_GOLD )
         {
            ch_printf( ch, "&pYou can only take %d katyr due to the maximum katyr able to be carried.&D\n\r",
                       ( MAX_GOLD - ch->gold ) );
            lgold = ( MAX_GOLD - ch->gold );
         }
         else
         {
            lgold = amt;
         }
         ch->gold += lgold;
         extract_obj( obj );
      }
   }
   else
   {
      obj = obj_to_char( obj, ch );
   }

   if( char_died( ch ) || obj_extracted( obj ) )
      return;
   oprog_get_trigger( ch, obj );
   return;
}


void do_get( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;
   OBJ_DATA *container;
   sh_int number;
   bool found;

   if( IS_ANGEL( ch ) )
   {
      send_to_char( "Angels can not get anything.\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   if( is_number( arg1 ) )
   {
      number = atoi( arg1 );
      if( number < 1 )
      {
         send_to_char( "That was easy...\n\r", ch );
         return;
      }
      if( ( ch->carry_number + number ) > can_carry_n( ch ) && !IS_AGOD( ch ) )
      {
         send_to_char( "You can't carry that many.\n\r", ch );
         return;
      }
      argument = one_argument( argument, arg1 );
   }
   else
      number = 0;
   argument = one_argument( argument, arg2 );
   if( !str_cmp( arg2, "from" ) && argument[0] != '\0' )
      argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' )
   {
      send_to_char( "Get what?\n\r", ch );
      return;
   }

   if( arg2[0] == '\0' )
   {
      if( number <= 1 && str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
      {
         obj = get_obj_list( ch, arg1, ch->in_room->first_content );
         if( !obj )
         {
            act( AT_PLAIN, "I see no $T here.", ch, NULL, arg1, TO_CHAR );
            return;
         }
         if( rare_obj( ch, obj ) )
         {
            send_to_char( "You cannot get another one of these rare items.\n\r", ch );
            return;
         }
         if( obj->owner && obj->owner[0] != '\0' )
         {
            if( str_cmp( obj->owner, ch->name ) && !IS_IMMORTAL( ch ) )
            {
               act( AT_PLAIN, "$p is owned - but not by you.", ch, obj, NULL, TO_CHAR );
               return;
            }
         }
         separate_obj( obj );
         get_obj( ch, obj, NULL );
         if( char_died( ch ) )
            return;
         if( IS_SET( sysdata.save_flags, SV_GET ) )
            save_char_obj( ch );
      }
      else
      {
         sh_int cnt = 0;
         bool fAll;
         char *chk;

         if( xIS_SET( ch->in_room->room_flags, ROOM_DONATION ) )
         {
            send_to_char( "The gods frown upon such a display of greed!\n\r", ch );
            return;
         }
         if( !str_cmp( arg1, "all" ) )
            fAll = TRUE;
         else
            fAll = FALSE;
         if( number > 1 )
            chk = arg1;
         else
            chk = &arg1[4];
         found = FALSE;
         for( obj = ch->in_room->last_content; obj; obj = obj_next )
         {
            obj_next = obj->prev_content;
            if( ( fAll || nifty_is_name( chk, obj->name ) ) && can_see_obj( ch, obj ) && ( !rare_obj( ch, obj ) ) )
            {
               found = TRUE;
               if( number && ( cnt + obj->count ) > number )
                  split_obj( obj, number - cnt );
               cnt += obj->count;
               get_obj( ch, obj, NULL );
               if( char_died( ch )
                   || ch->carry_number >= can_carry_n( ch )
                   || ch->carry_weight >= can_carry_w( ch ) || ( number && cnt >= number ) )
               {
                  if( IS_SET( sysdata.save_flags, SV_GET ) && !char_died( ch ) )
                     save_char_obj( ch );
                  return;
               }
            }
         }

         if( !found )
         {
            if( fAll )
               send_to_char( "I see nothing here.\n\r", ch );
            else
               act( AT_PLAIN, "I see no $T here.", ch, NULL, chk, TO_CHAR );
         }
         else if( IS_SET( sysdata.save_flags, SV_GET ) )
            save_char_obj( ch );
      }
   }
   else
   {
      if( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
      {
         send_to_char( "You can't do that.\n\r", ch );
         return;
      }

      if( ( container = get_obj_here( ch, arg2 ) ) == NULL )
      {
         act( AT_PLAIN, "I see no $T here.", ch, NULL, arg2, TO_CHAR );
         return;
      }

      switch ( container->item_type )
      {
         default:
            if( !IS_OBJ_STAT( container, ITEM_COVERING ) )
            {
               send_to_char( "That's not a container.\n\r", ch );
               return;
            }
            if( ch->carry_weight + container->weight > can_carry_w( ch ) )
            {
               send_to_char( "It's too heavy for you to lift.\n\r", ch );
               return;
            }
            break;

         case ITEM_CONTAINER:
         case ITEM_CORPSE_NPC:
         case ITEM_KEYRING:
         case ITEM_QUIVER:
            break;

         case ITEM_CORPSE_PC:
         {
            char name[MAX_INPUT_LENGTH];
            char *pd;

            if( IS_NPC( ch ) )
            {
               send_to_char( "You can't do that.\n\r", ch );
               return;
            }

            pd = container->short_descr;
            pd = one_argument( pd, name );
            pd = one_argument( pd, name );
            pd = one_argument( pd, name );
            pd = one_argument( pd, name );

            if( IS_OBJ_STAT( container, ITEM_CLANCORPSE )
                && !IS_NPC( ch ) && ( get_timer( ch, TIMER_PKILLED ) > 0 ) && str_cmp( name, ch->name ) )
            {
               send_to_char( "You cannot loot from that corpse...yet.\n\r", ch );
               return;
            }


            if( IS_OBJ_STAT( container, ITEM_CLANCORPSE )
                && !IS_NPC( ch ) && !IS_IMMORTAL( ch )
                && container->action_desc[0] != '\0'
                && str_cmp( name, ch->name ) && str_cmp( container->action_desc, ch->name ) )
            {
               send_to_char( "You did not inflict the death blow upon this corpse.\n\r", ch );
               return;
            }

            if( IS_OBJ_STAT( container, ITEM_CLANCORPSE )
                && !IS_NPC( ch ) && str_cmp( name, ch->name ) && container->value[5] >= 3 )
            {
               send_to_char( "Frequent looting has left this corpse protected by the gods.\n\r", ch );
               return;
            }

            if( IS_OBJ_STAT( container, ITEM_CLANCORPSE )
                && !IS_NPC( ch ) && IS_SET( ch->pcdata->flags, PCFLAG_DEADLY )
                && container->value[4] - ch->level < 6 && container->value[4] - ch->level > -6 )
               break;

/*		if ( str_cmp( name, ch->name ) && !IS_IMMORTAL(ch) )
		{
		    bool fGroup;

		    fGroup = FALSE;
		    for ( gch = first_char; gch; gch = gch->next )
		    {
			if ( !IS_NPC(gch)
			&&   is_same_group( ch, gch )
			&&   !str_cmp( name, gch->name ) )
			{
			    fGroup = TRUE;
			    break;
			}
		    }

		    if ( !fGroup )
		    {
			send_to_char( "That's someone else's corpse.\n\r", ch );
			return;
		    }
		}
	    }*/
            if( str_cmp( name, ch->name ) && !IS_IMMORTAL( ch ) )
            {
               send_to_char( "That's someone else's corpse.\n\r", ch );
               return;
            }
         }
      }

      if( !IS_OBJ_STAT( container, ITEM_COVERING ) && IS_SET( container->value[1], CONT_CLOSED ) )
      {
         act( AT_PLAIN, "The $d is closed.", ch, NULL, container->name, TO_CHAR );
         return;
      }

      if( number <= 1 && str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
      {
         obj = get_obj_list( ch, arg1, container->first_content );
         if( !obj )
         {
            act( AT_PLAIN, IS_OBJ_STAT( container, ITEM_COVERING ) ?
                 "I see nothing like that beneath the $T." : "I see nothing like that in the $T.", ch, NULL, arg2, TO_CHAR );
            return;
         }
         separate_obj( obj );
         get_obj( ch, obj, container );
//     if ( container->item_type == ITEM_CORPSE_PC )
//    write_corpses( ch, container->short_descr+14, NULL );
         check_for_trap( ch, container, TRAP_GET );
         if( char_died( ch ) )
            return;
         if( IS_SET( sysdata.save_flags, SV_GET ) )
            save_char_obj( ch );
      }
      else
      {
         int cnt = 0;
         bool fAll;
         char *chk;

         if( IS_OBJ_STAT( container, ITEM_DONATION ) )
         {
            send_to_char( "The gods frown upon such an act of greed!\n\r", ch );
            return;
         }

         if( IS_OBJ_STAT( container, ITEM_CLANCORPSE )
             && !IS_IMMORTAL( ch ) && !IS_NPC( ch ) && str_cmp( ch->name, container->name + 7 ) )
         {
            send_to_char( "The gods frown upon such wanton greed!\n\r", ch );
            return;
         }

         if( !str_cmp( arg1, "all" ) )
            fAll = TRUE;
         else
            fAll = FALSE;
         if( number > 1 )
            chk = arg1;
         else
            chk = &arg1[4];
         found = FALSE;
         for( obj = container->first_content; obj; obj = obj_next )
         {
            obj_next = obj->next_content;
            if( ( fAll || nifty_is_name( chk, obj->name ) ) && can_see_obj( ch, obj ) )
            {
               found = TRUE;
               if( number && ( cnt + obj->count ) > number )
                  split_obj( obj, number - cnt );
               cnt += obj->count;
               get_obj( ch, obj, container );
               if( char_died( ch )
                   || ch->carry_number >= can_carry_n( ch )
                   || ch->carry_weight >= can_carry_w( ch ) || ( number && cnt >= number ) )
                  return;
            }
         }

         if( !found )
         {
            if( fAll )
            {
               if( container->item_type == ITEM_KEYRING && !IS_OBJ_STAT( container, ITEM_COVERING ) )
                  act( AT_PLAIN, "The $T holds no keys.", ch, NULL, arg2, TO_CHAR );
               else
                  act( AT_PLAIN, IS_OBJ_STAT( container, ITEM_COVERING ) ?
                       "I see nothing beneath the $T." : "I see nothing in the $T.", ch, NULL, arg2, TO_CHAR );
            }
            else
            {
               if( container->item_type == ITEM_KEYRING && !IS_OBJ_STAT( container, ITEM_COVERING ) )
                  act( AT_PLAIN, "The $T does not hold that key.", ch, NULL, arg2, TO_CHAR );
               else
                  act( AT_PLAIN, IS_OBJ_STAT( container, ITEM_COVERING ) ?
                       "I see nothing like that beneath the $T." :
                       "I see nothing like that in the $T.", ch, NULL, arg2, TO_CHAR );
            }
         }
         else
            check_for_trap( ch, container, TRAP_GET );
         if( char_died( ch ) )
            return;
         if( container->item_type == ITEM_CORPSE_PC )
         {
            if( get_obj_list( ch, arg1, container->first_content ) == NULL )
            {
               container->timer = 1;
            }
//       write_corpses( ch, container->short_descr+14, NULL );
         }
         if( found && IS_SET( sysdata.save_flags, SV_GET ) )
            save_char_obj( ch );
      }
   }
   return;
}



void do_put( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   OBJ_DATA *container;
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;
   sh_int count;
   int number;
   bool save_char = FALSE;

   argument = one_argument( argument, arg1 );
   if( is_number( arg1 ) )
   {
      number = atoi( arg1 );
      if( number < 1 )
      {
         send_to_char( "That was easy...\n\r", ch );
         return;
      }
      argument = one_argument( argument, arg1 );
   }
   else
      number = 0;
   argument = one_argument( argument, arg2 );
   if( ( !str_cmp( arg2, "into" ) || !str_cmp( arg2, "inside" )
         || !str_cmp( arg2, "in" ) || !str_cmp( arg2, "under" )
         || !str_cmp( arg2, "onto" ) || !str_cmp( arg2, "on" ) ) && argument[0] != '\0' )
      argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Put what in what?\n\r", ch );
      return;
   }

   if( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
   {
      send_to_char( "You can't do that.\n\r", ch );
      return;
   }

   if( ( container = get_obj_here( ch, arg2 ) ) == NULL )
   {
      act( AT_PLAIN, "I see no $T here.", ch, NULL, arg2, TO_CHAR );
      return;
   }

   if( !container->carried_by && IS_SET( sysdata.save_flags, SV_PUT ) )
      save_char = TRUE;

   if( IS_OBJ_STAT( container, ITEM_COVERING ) )
   {
      if( ch->carry_weight + container->weight > can_carry_w( ch ) )
      {
         send_to_char( "It's too heavy for you to lift.\n\r", ch );
         return;
      }
   }
   else
   {
      if( container->item_type != ITEM_CONTAINER
          && container->item_type != ITEM_KEYRING && container->item_type != ITEM_QUIVER )
      {
         send_to_char( "That's not a container.\n\r", ch );
         return;
      }

      if( IS_SET( container->value[1], CONT_CLOSED ) )
      {
         act( AT_PLAIN, "The $d is closed.", ch, NULL, container->name, TO_CHAR );
         return;
      }
   }

   if( number <= 1 && str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
   {
      if( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
      {
         send_to_char( "You do not have that item.\n\r", ch );
         return;
      }

      if( obj == container )
      {
         send_to_char( "You can't fold it into itself.\n\r", ch );
         return;
      }

      if( !can_drop_obj( ch, obj ) )
      {
         send_to_char( "You can't let go of it.\n\r", ch );
         return;
      }

      if( container->item_type == ITEM_KEYRING && obj->item_type != ITEM_KEY )
      {
         send_to_char( "That's not a key.\n\r", ch );
         return;
      }

      if( container->item_type == ITEM_QUIVER && obj->item_type != ITEM_PROJECTILE )
      {
         send_to_char( "That's not a projectile.\n\r", ch );
         return;
      }

      if( ( IS_OBJ_STAT( container, ITEM_COVERING )
            && ( get_obj_weight( obj ) / obj->count )
            > ( ( get_obj_weight( container ) / container->count ) - container->weight ) ) )
      {
         send_to_char( "It won't fit under there.\n\r", ch );
         return;
      }

      if( ( get_real_obj_weight( obj ) / obj->count )
          + ( get_real_obj_weight( container ) / container->count ) > container->value[0] )
      {
         send_to_char( "It won't fit.\n\r", ch );
         return;
      }

      if( obj->item_type == ITEM_KEYITEM )
      {
         send_to_char( "That is a key item and cannot be placed in a container.\n\r", ch );
         return;
      }

      separate_obj( obj );
      separate_obj( container );
      obj_from_char( obj );
      obj = obj_to_obj( obj, container );
      check_for_trap( ch, container, TRAP_PUT );
      if( char_died( ch ) )
         return;
      count = obj->count;
      obj->count = 1;
      if( container->item_type == ITEM_KEYRING && !IS_OBJ_STAT( container, ITEM_COVERING ) )
      {
         act( AT_ACTION, "$n slips $p onto $P.", ch, obj, container, TO_ROOM );
         act( AT_ACTION, "You slip $p onto $P.", ch, obj, container, TO_CHAR );
      }
      else
      {
         act( AT_ACTION, IS_OBJ_STAT( container, ITEM_COVERING )
              ? "$n hides $p beneath $P." : "$n puts $p in $P.", ch, obj, container, TO_ROOM );
         act( AT_ACTION, IS_OBJ_STAT( container, ITEM_COVERING )
              ? "You hide $p beneath $P." : "You put $p in $P.", ch, obj, container, TO_CHAR );
      }
      obj->count = count;

//        if ( container->item_type == ITEM_CORPSE_PC )
//    write_corpses( NULL, container->short_descr+14, NULL );

      if( save_char )
         save_char_obj( ch );
      if( xIS_SET( ch->in_room->room_flags, ROOM_CLANSTOREROOM ) && container->carried_by == NULL )
      {
         save_clan_storeroom( ch );
      }
   }
   else
   {
      bool found = FALSE;
      int cnt = 0;
      bool fAll;
      char *chk;

      if( !str_cmp( arg1, "all" ) )
         fAll = TRUE;
      else
         fAll = FALSE;
      if( number > 1 )
         chk = arg1;
      else
         chk = &arg1[4];

      separate_obj( container );
      for( obj = ch->first_carrying; obj; obj = obj_next )
      {
         obj_next = obj->next_content;

         if( ( fAll || nifty_is_name( chk, obj->name ) )
             && can_see_obj( ch, obj )
             && obj->wear_loc == WEAR_NONE
             && obj != container
             && can_drop_obj( ch, obj )
             && ( container->item_type != ITEM_KEYRING || obj->item_type == ITEM_KEY )
             && ( container->item_type != ITEM_QUIVER || obj->item_type == ITEM_PROJECTILE )
             && get_obj_weight( obj ) + get_obj_weight( container ) <= container->value[0] )
         {
            if( obj->item_type == ITEM_KEYITEM )
               continue;

            if( number && ( cnt + obj->count ) > number )
               split_obj( obj, number - cnt );
            cnt += obj->count;
            obj_from_char( obj );
            if( container->item_type == ITEM_KEYRING )
            {
               act( AT_ACTION, "$n slips $p onto $P.", ch, obj, container, TO_ROOM );
               act( AT_ACTION, "You slip $p onto $P.", ch, obj, container, TO_CHAR );
            }
            else
            {
               act( AT_ACTION, "$n puts $p in $P.", ch, obj, container, TO_ROOM );
               act( AT_ACTION, "You put $p in $P.", ch, obj, container, TO_CHAR );
            }
            obj = obj_to_obj( obj, container );
            found = TRUE;

            check_for_trap( ch, container, TRAP_PUT );
            if( char_died( ch ) )
               return;
            if( number && cnt >= number )
               break;
         }
      }

      if( !found )
      {
         if( fAll )
            act( AT_PLAIN, "You are not carrying anything.", ch, NULL, NULL, TO_CHAR );
         else
            act( AT_PLAIN, "You are not carrying any $T.", ch, NULL, chk, TO_CHAR );
         return;
      }

      if( save_char )
         save_char_obj( ch );
//        if ( container->item_type == ITEM_CORPSE_PC )
//    write_corpses( NULL, container->short_descr+14, NULL );

      if( xIS_SET( ch->in_room->room_flags, ROOM_CLANSTOREROOM ) && container->carried_by == NULL )
      {
         save_clan_storeroom( ch );
      }
   }

   return;
}


void do_drop( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;
   bool found;
   int number;
   int type = 3;

   argument = one_argument( argument, arg );
   if( is_number( arg ) )
   {
      number = atoi( arg );
      if( number < 1 )
      {
         send_to_char( "That was easy...\n\r", ch );
         return;
      }
      argument = one_argument( argument, arg );
   }
   else
      number = 0;

   if( arg[0] == '\0' )
   {
      send_to_char( "Drop what?\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_LITTERBUG ) )
   {
      set_char_color( AT_YELLOW, ch );
      send_to_char( "A godly force prevents you from dropping anything...\n\r", ch );
      return;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_NODROP ) && ch != supermob )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "A magical force stops you!\n\r", ch );
      set_char_color( AT_TELL, ch );
      send_to_char( "Someone tells you, 'No littering here!'\n\r", ch );
      return;
   }

   if( number > 0 )
   {

      if( !str_cmp( arg, "katyr" ) )
      {
         if( !str_cmp( arg, "katyr" ) )
            type = 0;

         if( type == 0 && ch->gold < number )
         {
            send_to_char( "You haven't got that many coins.\n\r", ch );
            return;
         }

         if( type > 0 || type < 0 )
         {
            send_to_char( " <BUG> invalid type, Report to Nicole\n\r", ch );
            return;
         }
         if( type == 0 )
            ch->gold -= number;

         for( obj = ch->in_room->first_content; obj; obj = obj_next )
         {
            obj_next = obj->next_content;

            switch ( obj->pIndexData->vnum )
            {
               case OBJ_VNUM_GOLD_ONE:
                  if( type == 0 )
                  {
                     number += 1;
                     extract_obj( obj );
                  }
                  break;
               case OBJ_VNUM_GOLD_SOME:
                  if( type == 0 )
                  {
                     number += obj->value[0];
                     extract_obj( obj );
                  }
                  break;
            }
         }
         if( type == 0 )
         {
            act( AT_ACTION, "$n drops some katyr.", ch, NULL, NULL, TO_ROOM );
            obj_to_room( create_money( number, 0 ), ch->in_room );
            send_to_char( "You let the katyr slip from your hand.\n\r", ch );
         }
         if( IS_SET( sysdata.save_flags, SV_DROP ) )
            save_char_obj( ch );
         return;
      }
   }

   if( number <= 1 && str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
   {
      if( ( obj = get_obj_carry( ch, arg ) ) == NULL )
      {
         send_to_char( "You do not have that item.\n\r", ch );
         return;
      }

      if( !can_drop_obj( ch, obj ) )
      {
         send_to_char( "You can't let go of it.\n\r", ch );
         return;
      }

      if( obj->item_type == ITEM_KEYITEM )
      {
         send_to_char( "That is a key item and cannot be dropped.\n\r", ch );
         return;
      }

      separate_obj( obj );
      act( AT_ACTION, "$n drops $p.", ch, obj, NULL, TO_ROOM );
      act( AT_ACTION, "You drop $p.", ch, obj, NULL, TO_CHAR );

      obj_from_char( obj );
      if( obj->item_type == ITEM_CORPSE_PC )
      {
         obj = obj_to_room_cr( obj, ch->in_room, ch );
      }
      else
      {
         obj = obj_to_room( obj, ch->in_room );
      }
      oprog_drop_trigger( ch, obj );

      if( IS_OBJ_STAT( obj, ITEM_QOP ) && !IS_IMMORTAL( ch ) )
      {
         extract_obj( obj );
      }

      if( char_died( ch ) || obj_extracted( obj ) )
         return;

/*        if ( xIS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM) )
        {
            save_clan_storeroom( ch );
	}*/
   }
   else
   {
      int cnt = 0;
      char *chk;
      bool fAll;

      if( !str_cmp( arg, "all" ) )
         fAll = TRUE;
      else
         fAll = FALSE;
      if( number > 1 )
         chk = arg;
      else
         chk = &arg[4];
      if( xIS_SET( ch->in_room->room_flags, ROOM_NODROPALL ) )
      {
         send_to_char( "You can't seem to do that here...\n\r", ch );
         return;
      }
      found = FALSE;
      for( obj = ch->first_carrying; obj; obj = obj_next )
      {
         obj_next = obj->next_content;

         if( ( fAll || nifty_is_name( chk, obj->name ) )
             && can_see_obj( ch, obj )
             && obj->wear_loc == WEAR_NONE && obj->item_type != ITEM_KEYITEM && can_drop_obj( ch, obj ) )
         {
            found = TRUE;
            if( HAS_PROG( obj->pIndexData, DROP_PROG ) && obj->count > 1 )
            {
               ++cnt;
               separate_obj( obj );
               obj_from_char( obj );
               if( !obj_next )
                  obj_next = ch->first_carrying;
            }
            else
            {
               if( number && ( cnt + obj->count ) > number )
                  split_obj( obj, number - cnt );
               cnt += obj->count;
               obj_from_char( obj );
            }
            act( AT_ACTION, "$n drops $p.", ch, obj, NULL, TO_ROOM );
            act( AT_ACTION, "You drop $p.", ch, obj, NULL, TO_CHAR );

            if( obj->item_type == ITEM_CORPSE_PC )
            {
               obj = obj_to_room_cr( obj, ch->in_room, ch );
            }
            else
            {
               obj = obj_to_room( obj, ch->in_room );
            }
            oprog_drop_trigger( ch, obj );
            if( IS_OBJ_STAT( obj, ITEM_QOP ) && !IS_IMMORTAL( ch ) )
            {
               extract_obj( obj );
            }
            if( char_died( ch ) )
               return;
            if( number && cnt >= number )
               break;
         }
      }

      if( !found )
      {
         if( fAll )
            act( AT_PLAIN, "You are not carrying anything.", ch, NULL, NULL, TO_CHAR );
         else
            act( AT_PLAIN, "You are not carrying any $T.", ch, NULL, chk, TO_CHAR );
      }
   }
   if( IS_SET( sysdata.save_flags, SV_DROP ) )
      save_char_obj( ch );
   return;
}



void do_give( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   int type = 3;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   if( !str_cmp( arg2, "to" ) && argument[0] != '\0' )
      argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Give what to whom?\n\r", ch );
      return;
   }

   if( is_number( arg1 ) )
   {
      int amount;

      amount = atoi( arg1 );
      if( amount <= 0 || ( str_cmp( arg2, "katyr" ) ) )
      {
         send_to_char( "Sorry, you can't do that.\n\r", ch );
         return;
      }

      if( !str_cmp( arg2, "katyr" ) )
         type = 0;

      if( type > 0 || type < 0 )
      {
         send_to_char( "<BUG> invalid coin type, report to Nicole.\n\r", ch );
         return;
      }
      argument = one_argument( argument, arg2 );
      if( !str_cmp( arg2, "to" ) && argument[0] != '\0' )
         argument = one_argument( argument, arg2 );
      if( arg2[0] == '\0' )
      {
         send_to_char( "Give what to whom?\n\r", ch );
         return;
      }

      if( ( victim = get_char_room( ch, arg2 ) ) == NULL )
      {
         send_to_char( "They aren't here.\n\r", ch );
         return;
      }

      if( IS_ANGEL( ch ) )
      {
         send_to_char( "Sorry Angel's can't give anything to another.\n\r", ch );
         return;
      }

      if( !IS_IMMORTAL( ch ) && IS_ANGEL( victim ) )
      {
         send_to_char( "Sorry Angel's can't recieve items.\n\r", ch );
         return;
      }

      if( type == 0 && ch->gold < amount )
      {
         send_to_char( "Very generous of you, but you haven't got that much katyr.\n\r", ch );
         return;
      }
      if( type == 0 )
      {
         ch->gold -= amount;
         victim->gold += amount;
         strcpy( buf, "$n gives you " );
         strcat( buf, arg1 );
         strcat( buf, ( amount > 1 ) ? " katyr." : " katyr." );
         act( AT_ACTION, buf, ch, NULL, victim, TO_VICT );
         act( AT_ACTION, "$n gives $N some katyr.", ch, NULL, victim, TO_NOTVICT );
         act( AT_ACTION, "You give $N some katyr.", ch, NULL, victim, TO_CHAR );
         mprog_bribe_trigger( victim, ch, amount );
      }
      return;
   }

   if( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
   {
      send_to_char( "You do not have that item.\n\r", ch );
      return;
   }

   if( obj->wear_loc != WEAR_NONE )
   {
      send_to_char( "You must remove it first.\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg2 ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !IS_NPC( victim ) && obj->item_type == ITEM_KEYITEM )
   {
      send_to_char( "That is a key item and cannot be given to another player.\n\r", ch );
      return;
   }

   if( IS_ANGEL( ch ) )
   {
      send_to_char( "Sorry Angel's can't give anything to another.\n\r", ch );
      return;
   }

   if( !IS_IMMORTAL( ch ) && IS_ANGEL( victim ) )
   {
      send_to_char( "Sorry Angel's can't recieve items.\n\r", ch );
      return;
   }

   if( !can_drop_obj( ch, obj ) )
   {
      send_to_char( "You can't let go of it.\n\r", ch );
      return;
   }

   if( obj->owner && obj->owner[0] != '\0' && ( !IS_IMMORTAL( ch ) || !IS_IMMORTAL( victim ) ) )
   {
      send_to_char( "That is owned and can not be given.\n\r", ch );
      return;
   }

   if( IS_OBJ_STAT( obj, ITEM_QOP ) && ( !IS_IMMORTAL( victim ) && !IS_IMMORTAL( ch ) ) )
   {
      send_to_char( "This item can only be given to an immortal.\n\r", ch );
      return;
   }

   if( victim->carry_number + ( get_obj_number( obj ) / obj->count ) > can_carry_n( victim ) )
   {
      act( AT_PLAIN, "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if( victim->carry_weight + ( get_obj_weight( obj ) / obj->count ) > can_carry_w( victim ) )
   {
      act( AT_PLAIN, "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if( !can_see_obj( victim, obj ) )
   {
      act( AT_PLAIN, "$N can't see it.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) && !can_take_proto( victim ) )
   {
      act( AT_PLAIN, "You cannot give that to $N!", ch, NULL, victim, TO_CHAR );
      return;
   }

   if( rare_obj( ch, obj ) )
   {
      act( AT_PLAIN, "You cannot give a rare item away.", ch, NULL, NULL, TO_CHAR );
      return;
   }

   separate_obj( obj );
   obj_from_char( obj );
   act( AT_ACTION, "$n gives $p to $N.", ch, obj, victim, TO_NOTVICT );
   act( AT_ACTION, "$n gives you $p.", ch, obj, victim, TO_VICT );
   act( AT_ACTION, "You give $p to $N.", ch, obj, victim, TO_CHAR );
   obj = obj_to_char( obj, victim );
   mprog_give_trigger( victim, ch, obj );
   if( IS_SET( sysdata.save_flags, SV_GIVE ) && !char_died( ch ) )
      save_char_obj( ch );
   if( IS_SET( sysdata.save_flags, SV_RECEIVE ) && !char_died( victim ) )
      save_char_obj( victim );
   return;
}

obj_ret damage_obj( OBJ_DATA * obj )
{
   CHAR_DATA *ch;
   obj_ret objcode;

   ch = obj->carried_by;
   objcode = rNONE;


   separate_obj( obj );
   if( IS_OBJ_STAT( obj, ITEM_NOSCRAP ) )
   {
      return objcode;
   }
   if( !IS_NPC( ch ) && ( !IS_PKILL( ch ) || ( IS_PKILL( ch ) && !IS_SET( ch->pcdata->flags, PCFLAG_GAG ) ) ) )
      act( AT_OBJECT, "($p gets damaged)", ch, obj, NULL, TO_CHAR );
   else if( obj->in_room && ( ch = obj->in_room->first_person ) != NULL )
   {
      act( AT_OBJECT, "($p gets damaged)", ch, obj, NULL, TO_ROOM );
      act( AT_OBJECT, "($p gets damaged)", ch, obj, NULL, TO_CHAR );
      ch = NULL;
   }

   if( obj->item_type != ITEM_LIGHT )
      oprog_damage_trigger( ch, obj );
   else if( !IN_ARENA( ch ) )
      oprog_damage_trigger( ch, obj );

   if( obj_extracted( obj ) )
      return global_objcode;

   switch ( obj->item_type )
   {
      default:
         make_scraps( obj );
         objcode = rOBJ_SCRAPPED;
         break;
      case ITEM_CONTAINER:
      case ITEM_KEYRING:
      case ITEM_QUIVER:
         if( --obj->value[3] <= 0 )
         {
            if( !IN_ARENA( ch ) )
            {
               make_scraps( obj );
               objcode = rOBJ_SCRAPPED;
            }
            else
               obj->value[3] = 1;
         }
         break;
      case ITEM_LIGHT:
         if( --obj->value[0] <= 0 )
         {
            if( !IN_ARENA( ch ) )
            {
               make_scraps( obj );
               objcode = rOBJ_SCRAPPED;
            }
            else
               obj->value[0] = 1;
         }
         break;
      case ITEM_ARMOR:
         if( ch && obj->value[0] >= 1 )
            ch->armor += apply_ac( obj, obj->wear_loc );
         if( --obj->value[0] <= 0 )
         {
            if( !IS_PKILL( ch ) && !IN_ARENA( ch ) )
            {
               make_scraps( obj );
               objcode = rOBJ_SCRAPPED;
            }
            else
            {
               obj->value[0] = 1;
               ch->armor -= apply_ac( obj, obj->wear_loc );
            }
         }
         else if( ch && obj->value[0] >= 1 )
            ch->armor -= apply_ac( obj, obj->wear_loc );
         break;
      case ITEM_FLUTE:
         if( ch && obj->value[0] >= 1 )
            ch->armor += apply_ac( obj, obj->wear_loc );
         if( --obj->value[0] <= 0 )
         {
            if( !IS_PKILL( ch ) && !IN_ARENA( ch ) )
            {
               make_scraps( obj );
               objcode = rOBJ_SCRAPPED;
            }
            else
            {
               obj->value[0] = 1;
               ch->armor -= apply_ac( obj, obj->wear_loc );
            }
         }
         else if( ch && obj->value[0] >= 1 )
            ch->armor -= apply_ac( obj, obj->wear_loc );
         break;
      case ITEM_WEAPON:
         if( --obj->value[0] <= 0 )
         {
            if( !IS_PKILL( ch ) && !IN_ARENA( ch ) )
            {
               make_scraps( obj );
               objcode = rOBJ_SCRAPPED;
            }
            else
               obj->value[0] = 1;
         }
         break;
   }
   return objcode;
}


bool remove_obj( CHAR_DATA * ch, int iWear, bool fReplace )
{
   OBJ_DATA *obj, *tmpobj;

   if( ( obj = get_eq_char( ch, iWear ) ) == NULL )
      return TRUE;

   if( !fReplace && ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
   {
      act( AT_PLAIN, "$d: you can't carry that many items.", ch, NULL, obj->name, TO_CHAR );
      return FALSE;
   }

   if( !fReplace )
      return FALSE;

   if( IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
   {
      act( AT_PLAIN, "You can't remove $p.", ch, obj, NULL, TO_CHAR );
      return FALSE;
   }

   if( obj == get_eq_char( ch, WEAR_WIELD ) && ( tmpobj = get_eq_char( ch, WEAR_DUAL_WIELD ) ) != NULL )
      tmpobj->wear_loc = WEAR_WIELD;

   unequip_char( ch, obj );

   act( AT_ACTION, "You stop using $p.", ch, obj, NULL, TO_CHAR );
   oprog_remove_trigger( ch, obj );
   return TRUE;
}

bool could_dual( CHAR_DATA * ch )
{
   if( IS_NPC( ch ) || ch->pcdata->learned[gsn_dual_wield] )
      return TRUE;

   return FALSE;
}

bool can_dual( CHAR_DATA * ch )
{
   OBJ_DATA *obj = get_eq_char( ch, WEAR_WIELD );

   if( !could_dual( ch ) )
      return FALSE;

   if( IS_OBJ_STAT( obj, ITEM_TWOHAND ) )
   {
      send_to_char( "You are already wielding a two-handed weapon!\n\r", ch );
      return FALSE;
   }
   if( get_eq_char( ch, WEAR_DUAL_WIELD ) )
   {
      send_to_char( "You are already wielding two weapons!\n\r", ch );
      return FALSE;
   }
   if( get_eq_char( ch, WEAR_SHIELD ) )
   {
      send_to_char( "You cannot dual wield while holding a shield!\n\r", ch );
      return FALSE;
   }
   return TRUE;
}


bool can_layer( CHAR_DATA * ch, OBJ_DATA * obj, sh_int wear_loc )
{
   OBJ_DATA *otmp;
   sh_int bitlayers = 0;
   sh_int objlayers = obj->pIndexData->layers;

   for( otmp = ch->first_carrying; otmp; otmp = otmp->next_content )
      if( otmp->wear_loc == wear_loc )
      {
         if( !otmp->pIndexData->layers )
            return FALSE;
         else
            bitlayers |= otmp->pIndexData->layers;
      }
   if( ( bitlayers && !objlayers ) || bitlayers > objlayers )
      return FALSE;
   if( !bitlayers || ( ( bitlayers & ~objlayers ) == bitlayers ) )
      return TRUE;
   return FALSE;
}

void wear_obj( CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace, sh_int wear_bit, bool fAll )
{
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *tmpobj = NULL;
   OBJ_DATA *tmpooo = NULL;
   sh_int bit, tmp;
   AFFECT_DATA af;

   separate_obj( obj );
   if( obj->item_type == ITEM_WEAPON )
   {
      if( ( get_trust( ch ) + 5 ) < obj->level )
      {
         sprintf( buf, "You must be level %d to use this object.\n\r", obj->level - 5 );
         send_to_char( buf, ch );
         act( AT_ACTION, "$n tries to use $p, but is too inexperienced.", ch, obj, NULL, TO_ROOM );
         return;
      }
   }
   else
   {
      if( ( get_trust( ch ) + 3 ) < obj->level )
      {
         sprintf( buf, "You must be level %d to use this object.\n\r", obj->level - 3 );
         send_to_char( buf, ch );
         act( AT_ACTION, "$n tries to use $p, but is too inexperienced.", ch, obj, NULL, TO_ROOM );
         return;
      }
   }

   if( !IS_IMMORTAL( ch )
       && ( ( IS_OBJ_STAT( obj, ITEM_ANTI_WARRIOR )
              && ( ch->class == CLASS_WARRIOR
                   || ch->dualclass == CLASS_WARRIOR ) )
            || ( IS_OBJ_STAT( obj, ITEM_ANTI_MAGE )
                 && ( ch->class == CLASS_MAGE
                      || ch->dualclass == CLASS_MAGE ) )
            || ( IS_OBJ_STAT( obj, ITEM_ANTI_TAEDIEN )
                 && ( ch->class == CLASS_TAEDIEN
                      || ch->dualclass == CLASS_TAEDIEN ) )
            || ( IS_OBJ_STAT( obj, ITEM_ANTI_THIEF )
                 && ( ch->class == CLASS_THIEF
                      || ch->dualclass == CLASS_THIEF ) )
            || ( IS_OBJ_STAT( obj, ITEM_ANTI_VAMPIRE )
                 && ( ch->class == CLASS_VAMPIRE
                      || ch->dualclass == CLASS_VAMPIRE ) )
            || ( IS_OBJ_STAT( obj, ITEM_ANTI_RANGER )
                 && ( ch->class == CLASS_RANGER
                      || ch->dualclass == CLASS_RANGER ) )
            || ( IS_OBJ_STAT( obj, ITEM_ANTI_WEREWOLF )
                 && ( ch->class == CLASS_WEREWOLF
                      || ch->dualclass == CLASS_WEREWOLF ) )
            || ( IS_OBJ_STAT( obj, ITEM_ANTI_ARCHER )
                 && ( ch->class == CLASS_ARCHER
                      || ch->dualclass == CLASS_ARCHER ) )
            || ( IS_OBJ_STAT( obj, ITEM_ANTI_JEDI )
                 && ( ch->class == CLASS_JEDI
                      || ch->dualclass == CLASS_JEDI ) )
            || ( IS_OBJ_STAT( obj, ITEM_ANTI_BARBARIAN )
                 && ( ch->class == CLASS_BARBARIAN
                      || ch->dualclass == CLASS_BARBARIAN ) )
            || ( IS_OBJ_STAT( obj, ITEM_ANTI_SORCERER )
                 && ( ch->class == CLASS_SORCERER
                      || ch->dualclass == CLASS_SORCERER ) )
            || ( IS_OBJ_STAT( obj, ITEM_ANTI_PALADIN )
                 && ( ch->class == CLASS_PALADIN
                      || ch->dualclass == CLASS_PALADIN ) )
            || ( IS_OBJ_STAT( obj, ITEM_ANTI_CLERIC ) && ( ch->class == CLASS_CLERIC || ch->dualclass == CLASS_CLERIC ) ) ) )

   {
      act( AT_MAGIC, "You are forbidden to use that item.", ch, NULL, NULL, TO_CHAR );
      act( AT_ACTION, "$n tries to use $p, but is forbidden to do so.", ch, obj, NULL, TO_ROOM );
      return;
   }

   if( IS_OBJ_STAT( obj, ITEM_JEDI_ONLY ) && ch->class != CLASS_JEDI )
   {
      act( AT_MAGIC, "You are forbidden to use that item.", ch, NULL, NULL, TO_CHAR );
      act( AT_ACTION, "$n tries to use $p, but is forbidden to do so.", ch, obj, NULL, TO_ROOM );
      return;
   }

   if( IS_OBJ_STAT( obj, ITEM_SAM_ONLY ) && ch->class != CLASS_SAMURAI )
   {
      act( AT_MAGIC, "You are forbidden to use that item.", ch, NULL, NULL, TO_CHAR );
      act( AT_ACTION, "$n tries to use $p, but is forbidden to do so.", ch, obj, NULL, TO_ROOM );
      return;
   }

   if( IS_OBJ_STAT( obj, ITEM_ARCHER_ONLY ) && ch->class != CLASS_ARCHER )
   {
      act( AT_MAGIC, "You are forbidden to use that item.", ch, NULL, NULL, TO_CHAR );
      act( AT_ACTION, "$n tries to use $p, but is forbidden to do so.", ch, obj, NULL, TO_ROOM );
      return;
   }

   if( obj->item_type == ITEM_KEYITEM )
   {
      send_to_char( "Key items cannot be worn.\n\r", ch );
      return;
   }

//    if ( IS_NPC(ch) && IS_OBJ_STAT(obj, ITEM_INVENTORY) )
// return;

   if( wear_bit > -1 )
   {
      bit = wear_bit;
      if( !CAN_WEAR( obj, 1 << bit ) )
      {
         if( fReplace )
         {
            switch ( 1 << bit )
            {
               case ITEM_HOLD:
                  send_to_char( "You cannot hold that.\n\r", ch );
                  break;
               case ITEM_WIELD:
               case ITEM_MISSILE_WIELD:
                  send_to_char( "You cannot wield that.\n\r", ch );
                  break;
               default:
                  sprintf( buf, "You cannot wear that on your %s.\n\r", w_flags[bit] );
                  send_to_char( buf, ch );
            }
         }
         return;
      }
   }
   else
   {
      for( bit = -1, tmp = 1; tmp < 31; tmp++ )
      {
         if( CAN_WEAR( obj, 1 << tmp ) )
         {
            bit = tmp;
            break;
         }
      }
   }

   if( obj->item_type == ITEM_LIGHT )
   {
      if( !remove_obj( ch, WEAR_LIGHT, fReplace ) )
         return;
      if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
      {
         if( !fAll )
         {
            act( AT_ACTION, "$n holds $p as their light.", ch, obj, NULL, TO_ROOM );
         }
         act( AT_ACTION, "You hold $p as your light.", ch, obj, NULL, TO_CHAR );
      }
      equip_char( ch, obj, WEAR_LIGHT );
      oprog_wear_trigger( ch, obj );
      if( obj->value[6] > 0 )
      {
         act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
         act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
         af.type = gsn_holyaura;
         af.duration = 4 * obj->value[6];
         af.bitvector = meb( AFF_HOLYAURA );
         af.location = APPLY_HITROLL;
         af.modifier = 2000;
         affect_to_char( ch, &af );

         af.type = gsn_holyaura;
         af.duration = 4 * obj->value[6];
         af.bitvector = meb( AFF_HOLYAURA );
         af.location = APPLY_DAMROLL;
         af.modifier = 2000;
         affect_to_char( ch, &af );

         af.type = gsn_holyaura;
         af.duration = 4 * obj->value[6];
         af.bitvector = meb( AFF_HOLYAURA );
         af.location = APPLY_HIT;
         af.modifier = 10000;
         affect_to_char( ch, &af );

         af.type = gsn_holyaura;
         af.duration = 4 * obj->value[6];
         af.bitvector = meb( AFF_HOLYAURA );
         af.location = APPLY_MANA;
         af.modifier = 10000;
         affect_to_char( ch, &af );

         af.type = gsn_holyaura;
         af.duration = 4 * obj->value[6];
         af.bitvector = meb( AFF_HOLYAURA );
         af.location = APPLY_MOVE;
         af.modifier = 10000;
         affect_to_char( ch, &af );
      }
      return;
   }

   if( bit == -1 )
   {
      if( fReplace )
         send_to_char( "You can't wear, wield, or hold that.\n\r", ch );
      return;
   }

   switch ( 1 << bit )
   {
      default:
         bug( "wear_obj: uknown/unused item_wear bit %d", bit );
         if( fReplace )
            send_to_char( "You can't wear, wield, or hold that.\n\r", ch );
         return;

      case ITEM_LODGE_RIB:
         act( AT_ACTION, "$p strikes you and deeply imbeds itself in your ribs!", ch, obj, NULL, TO_CHAR );
         act( AT_ACTION, "$p strikes $n and deeply imbeds itself in $s ribs!", ch, obj, NULL, TO_ROOM );
         equip_char( ch, obj, WEAR_LODGE_RIB );
         break;

      case ITEM_LODGE_ARM:
         act( AT_ACTION, "$p strikes you and deeply imbeds itself in your arm!", ch, obj, NULL, TO_CHAR );
         act( AT_ACTION, "$p strikes $n and deeply imbeds itself in $s arm!", ch, obj, NULL, TO_ROOM );
         equip_char( ch, obj, WEAR_LODGE_ARM );
         break;

      case ITEM_LODGE_LEG:
         act( AT_ACTION, "$p strikes you and deeply imbeds itself in your leg!", ch, obj, NULL, TO_CHAR );
         act( AT_ACTION, "$p strikes $n and deeply imbeds itself in $s leg!", ch, obj, NULL, TO_ROOM );
         equip_char( ch, obj, WEAR_LODGE_LEG );
         break;

      case ITEM_WEAR_FINGER:
         if( get_eq_char( ch, WEAR_FINGER_L )
             && get_eq_char( ch, WEAR_FINGER_R )
             && !remove_obj( ch, WEAR_FINGER_L, fReplace ) && !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
            return;

         if( !get_eq_char( ch, WEAR_FINGER_L ) )
         {
            if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
               if( !fAll )
               {
                  act( AT_ACTION, "$n slips their left finger into $p.", ch, obj, NULL, TO_ROOM );
               }
               act( AT_ACTION, "You slip your left finger into $p.", ch, obj, NULL, TO_CHAR );
            }
            equip_char( ch, obj, WEAR_FINGER_L );
            oprog_wear_trigger( ch, obj );
            if( obj->value[6] > 0 )
            {
               act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
               act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_HITROLL;
               af.modifier = 2000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_DAMROLL;
               af.modifier = 2000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_HIT;
               af.modifier = 10000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_MANA;
               af.modifier = 10000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_MOVE;
               af.modifier = 10000;
               affect_to_char( ch, &af );
            }
            return;
         }

         if( !get_eq_char( ch, WEAR_FINGER_R ) )
         {
            if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
               if( !fAll )
               {
                  act( AT_ACTION, "$n slips their right finger into $p.", ch, obj, NULL, TO_ROOM );
               }
               act( AT_ACTION, "You slip your right finger into $p.", ch, obj, NULL, TO_CHAR );
            }
            equip_char( ch, obj, WEAR_FINGER_R );
            oprog_wear_trigger( ch, obj );
            return;
         }

         bug( "Wear_obj: no free finger.", 0 );
         send_to_char( "You already wear something on both fingers.\n\r", ch );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_NECK:
         if( get_eq_char( ch, WEAR_NECK_1 ) != NULL
             && get_eq_char( ch, WEAR_NECK_2 ) != NULL
             && !remove_obj( ch, WEAR_NECK_1, fReplace ) && !remove_obj( ch, WEAR_NECK_2, fReplace ) )
            return;

         if( !get_eq_char( ch, WEAR_NECK_1 ) )
         {
            if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
               if( !fAll )
               {
                  act( AT_ACTION, "$n wears $p around their neck.", ch, obj, NULL, TO_ROOM );
               }
               act( AT_ACTION, "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
            }
            equip_char( ch, obj, WEAR_NECK_1 );
            oprog_wear_trigger( ch, obj );
            if( obj->value[6] > 0 )
            {
               act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
               act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_HITROLL;
               af.modifier = 2000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_DAMROLL;
               af.modifier = 2000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_HIT;
               af.modifier = 10000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_MANA;
               af.modifier = 10000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_MOVE;
               af.modifier = 10000;
               affect_to_char( ch, &af );
            }
            return;
         }

         if( !get_eq_char( ch, WEAR_NECK_2 ) )
         {
            if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
               if( !fAll )
               {
                  act( AT_ACTION, "$n wears $p around their neck.", ch, obj, NULL, TO_ROOM );
               }
               act( AT_ACTION, "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
            }
            equip_char( ch, obj, WEAR_NECK_2 );
            oprog_wear_trigger( ch, obj );
            return;
         }

         bug( "Wear_obj: no free neck.", 0 );
         send_to_char( "You already wear two neck items.\n\r", ch );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_BODY:
         if( !remove_obj( ch, WEAR_BODY, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n fits $p on their body.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You fit $p on your body.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_BODY );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_HEAD:
         if( !remove_obj( ch, WEAR_HEAD, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n dons $p upon their head.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You don $p upon your head.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_HEAD );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_EYES:
         if( !remove_obj( ch, WEAR_EYES, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n places $p on their eyes.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You place $p on your eyes.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_EYES );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_FACE:
         if( !remove_obj( ch, WEAR_FACE, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n places $p on their face.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You place $p on your face.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_FACE );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_EARS:
         if( !remove_obj( ch, WEAR_EARS, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n wears $p on their ears.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You wear $p on your ears.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_EARS );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_LEGS:
         if( !remove_obj( ch, WEAR_LEGS, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n slips into $p.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You slip into $p.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_LEGS );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_FEET:
         if( !remove_obj( ch, WEAR_FEET, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n wears $p on their feet.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You wear $p on your feet.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_FEET );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_HANDS:
         if( !remove_obj( ch, WEAR_HANDS, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n wears $p on their hands.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You wear $p on your hands.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_HANDS );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_ARMS:
         if( !remove_obj( ch, WEAR_ARMS, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n wears $p on their arms.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You wear $p on your arms.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_ARMS );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_ABOUT:
         if( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n wears $p about their body.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You wear $p about your body.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_ABOUT );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_BACK:
         if( !remove_obj( ch, WEAR_BACK, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n slings $p on their back.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You sling $p on your back.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_BACK );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_FLOAT:
         if( !remove_obj( ch, WEAR_FLOAT, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n releases the ghost of $p and it floats nearby them.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You release the ghost of $p and it floats nearby.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_FLOAT );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_CLAN:
         if( !remove_obj( ch, WEAR_CLAN, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n don a $p.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You don a $p.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_CLAN );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_WAIST:
         if( !remove_obj( ch, WEAR_WAIST, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n wears $p about their waist.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You wear $p about your waist.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_WAIST );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_WEAR_WRIST:
         if( get_eq_char( ch, WEAR_WRIST_L )
             && get_eq_char( ch, WEAR_WRIST_R )
             && !remove_obj( ch, WEAR_WRIST_L, fReplace ) && !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
            return;

         if( !get_eq_char( ch, WEAR_WRIST_L ) )
         {
            if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
               if( !fAll )
               {
                  act( AT_ACTION, "$n fits $p around their left wrist.", ch, obj, NULL, TO_ROOM );
               }
               act( AT_ACTION, "You fit $p around your left wrist.", ch, obj, NULL, TO_CHAR );
            }
            equip_char( ch, obj, WEAR_WRIST_L );
            oprog_wear_trigger( ch, obj );
            if( obj->value[6] > 0 )
            {
               act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
               act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_HITROLL;
               af.modifier = 2000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_DAMROLL;
               af.modifier = 2000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_HIT;
               af.modifier = 10000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_MANA;
               af.modifier = 10000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_MOVE;
               af.modifier = 10000;
               affect_to_char( ch, &af );
            }
            return;
         }

         if( !get_eq_char( ch, WEAR_WRIST_R ) )
         {
            if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
               if( !fAll )
               {
                  act( AT_ACTION, "$n fits $p around their right wrist.", ch, obj, NULL, TO_ROOM );
               }
               act( AT_ACTION, "You fit $p around your right wrist.", ch, obj, NULL, TO_CHAR );
            }
            equip_char( ch, obj, WEAR_WRIST_R );
            oprog_wear_trigger( ch, obj );
            if( obj->value[6] > 0 )
            {
               act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
               act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_HITROLL;
               af.modifier = 2000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_DAMROLL;
               af.modifier = 2000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_HIT;
               af.modifier = 10000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_MANA;
               af.modifier = 10000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_MOVE;
               af.modifier = 10000;
               affect_to_char( ch, &af );
            }
            return;
         }

         bug( "Wear_obj: no free wrist.", 0 );
         send_to_char( "You already wear two wrist items.\n\r", ch );
         return;

      case ITEM_WEAR_ANKLE:
         if( get_eq_char( ch, WEAR_ANKLE_L )
             && get_eq_char( ch, WEAR_ANKLE_R )
             && !remove_obj( ch, WEAR_ANKLE_L, fReplace ) && !remove_obj( ch, WEAR_ANKLE_R, fReplace ) )
            return;

         if( !get_eq_char( ch, WEAR_ANKLE_L ) )
         {
            if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
               if( !fAll )
               {
                  act( AT_ACTION, "$n fits $p around their left ankle.", ch, obj, NULL, TO_ROOM );
               }
               act( AT_ACTION, "You fit $p around your left ankle.", ch, obj, NULL, TO_CHAR );
            }
            equip_char( ch, obj, WEAR_ANKLE_L );
            oprog_wear_trigger( ch, obj );
            if( obj->value[6] > 0 )
            {
               act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
               act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_HITROLL;
               af.modifier = 2000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_DAMROLL;
               af.modifier = 2000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_HIT;
               af.modifier = 10000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_MANA;
               af.modifier = 10000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_MOVE;
               af.modifier = 10000;
               affect_to_char( ch, &af );
            }
            return;
         }

         if( !get_eq_char( ch, WEAR_ANKLE_R ) )
         {
            if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
               if( !fAll )
               {
                  act( AT_ACTION, "$n fits $p around their right ankle.", ch, obj, NULL, TO_ROOM );
               }
               act( AT_ACTION, "You fit $p around your right ankle.", ch, obj, NULL, TO_CHAR );
            }
            equip_char( ch, obj, WEAR_ANKLE_R );
            oprog_wear_trigger( ch, obj );
            if( obj->value[6] > 0 )
            {
               act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
               act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_HITROLL;
               af.modifier = 2000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_DAMROLL;
               af.modifier = 2000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_HIT;
               af.modifier = 10000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_MANA;
               af.modifier = 10000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_MOVE;
               af.modifier = 10000;
               affect_to_char( ch, &af );
            }
            return;
         }

         bug( "Wear_obj: no free ankle.", 0 );
         send_to_char( "You already wear two ankle items.\n\r", ch );
         return;

      case ITEM_WEAR_SHIELD:

         tmpooo = get_eq_char( ch, WEAR_WIELD );

         if( get_eq_char( ch, WEAR_DUAL_WIELD )
//     ||  (tmpooo && get_eq_char(ch, WEAR_MISSILE_WIELD)) )
             || ( get_eq_char( ch, WEAR_WIELD ) && get_eq_char( ch, WEAR_MISSILE_WIELD ) ) )
         {
            send_to_char( "You can't use a shield AND two weapons!\n\r", ch );
            return;
         }
         if( tmpooo && IS_OBJ_STAT( tmpooo, ITEM_TWOHAND ) )
         {
            send_to_char( "You can't use a shield AND a two-handed weapon!\n\r", ch );
            return;
         }

         if( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n uses $p as a shield.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You use $p as a shield.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_SHIELD );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_MISSILE_WIELD:
      case ITEM_WIELD:
         if( !could_dual( ch ) || IS_OBJ_STAT( obj, ITEM_TWOHAND ) )
         {
            if( !remove_obj( ch, WEAR_MISSILE_WIELD, fReplace ) )
               return;
            if( !remove_obj( ch, WEAR_WIELD, fReplace ) )
               return;
            tmpobj = NULL;
         }
         else
         {
            OBJ_DATA *mw, *dw;

            tmpobj = get_eq_char( ch, WEAR_WIELD );
            mw = get_eq_char( ch, WEAR_MISSILE_WIELD );
            dw = get_eq_char( ch, WEAR_DUAL_WIELD );
            if( tmpobj && ( mw || dw ) )
            {
               send_to_char( "You're already wielding two weapons.\n\r", ch );
               return;
            }
         }

         if( tmpobj )
         {
            if( can_dual( ch ) )
            {
               if( !IS_AGOD( ch )
                   && ( get_obj_weight( obj ) + get_obj_weight( tmpobj ) > str_app[get_curr_str( ch )].wield ) )
               {
                  send_to_char( "It is too heavy for you to wield.\n\r", ch );
                  return;
               }
               if( IS_OBJ_STAT( obj, ITEM_NODUALW ) )
               {
                  send_to_char( "You cannot dual wield this weapon.\n\r", ch );
                  return;
               }
               if( IS_OBJ_STAT( tmpobj, ITEM_TWOHAND ) )
               {
                  send_to_char( "You cannot dual wield this weapon while wielding a two-handed weapon.\n\r", ch );
                  return;
               }
               if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
               {
                  if( !fAll )
                  {
                     act( AT_ACTION, "$n dual-wields $p.", ch, obj, NULL, TO_ROOM );
                  }
                  act( AT_ACTION, "You dual-wield $p.", ch, obj, NULL, TO_CHAR );
               }
               if( 1 << bit == ITEM_MISSILE_WIELD )
                  equip_char( ch, obj, WEAR_MISSILE_WIELD );
               else
                  equip_char( ch, obj, WEAR_DUAL_WIELD );
               oprog_wear_trigger( ch, obj );
            }
            if( obj->value[6] > 0 )
            {
               act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
               act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_HITROLL;
               af.modifier = 2000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_DAMROLL;
               af.modifier = 2000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_HIT;
               af.modifier = 10000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_MANA;
               af.modifier = 10000;
               affect_to_char( ch, &af );

               af.type = gsn_holyaura;
               af.duration = 4 * obj->value[6];
               af.bitvector = meb( AFF_HOLYAURA );
               af.location = APPLY_MOVE;
               af.modifier = 10000;
               affect_to_char( ch, &af );
            }
            return;
         }

         tmpooo = get_eq_char( ch, WEAR_SHIELD );
         if( tmpooo && IS_OBJ_STAT( obj, ITEM_TWOHAND ) )
         {
            send_to_char( "You cannot wield this weapon while holding a shield.\n\r", ch );
            return;
         }

         if( !IS_IMMORTAL( ch ) && ( get_obj_weight( obj ) > str_app[get_curr_str( ch )].wield ) )
         {
            send_to_char( "It is too heavy for you to wield.\n\r", ch );
            return;
         }

         if( !remove_obj( ch, WEAR_WIELD, fReplace ) )
            return;
         if( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n wields $p.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You wield $p.", ch, obj, NULL, TO_CHAR );
         }
         if( 1 << bit == ITEM_MISSILE_WIELD )
            equip_char( ch, obj, WEAR_MISSILE_WIELD );
         else
            equip_char( ch, obj, WEAR_WIELD );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;

      case ITEM_HOLD:
         if( !remove_obj( ch, WEAR_HOLD, fReplace ) )
            return;
         if( obj->item_type == ITEM_WAND
             || obj->item_type == ITEM_STAFF
             || obj->item_type == ITEM_FOOD
             || obj->item_type == ITEM_COOK
             || obj->item_type == ITEM_PILL
             || obj->item_type == ITEM_POTION
             || obj->item_type == ITEM_SCROLL
             || obj->item_type == ITEM_DRINK_CON
             || obj->item_type == ITEM_BLOOD
             || obj->item_type == ITEM_PIPE
             || obj->item_type == ITEM_HERB
             || obj->item_type == ITEM_KEY || !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
         {
            if( !fAll )
            {
               act( AT_ACTION, "$n holds $p in their hands.", ch, obj, NULL, TO_ROOM );
            }
            act( AT_ACTION, "You hold $p in your hands.", ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_HOLD );
         oprog_wear_trigger( ch, obj );
         if( obj->value[6] > 0 )
         {
            act( AT_IMMORT, "$n's aura flares with power!", ch, NULL, NULL, TO_ROOM );
            act( AT_IMMORT, "Your aura flare with power!", ch, NULL, NULL, TO_CHAR );
            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HITROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_DAMROLL;
            af.modifier = 2000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_HIT;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MANA;
            af.modifier = 10000;
            affect_to_char( ch, &af );

            af.type = gsn_holyaura;
            af.duration = 4 * obj->value[6];
            af.bitvector = meb( AFF_HOLYAURA );
            af.location = APPLY_MOVE;
            af.modifier = 10000;
            affect_to_char( ch, &af );
         }
         return;
   }
}


void do_wear( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   sh_int wear_bit;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   if( ( !str_cmp( arg2, "on" ) || !str_cmp( arg2, "upon" ) || !str_cmp( arg2, "around" ) ) && argument[0] != '\0' )
      argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' )
   {
      send_to_char( "Wear, wield, or hold what?\n\r", ch );
      return;
   }

   if( !str_cmp( arg1, "all" ) )
   {
      OBJ_DATA *obj_next;

      act( AT_ACTION, "$n dons their equipment.", ch, NULL, NULL, TO_ROOM );

      for( obj = ch->first_carrying; obj; obj = obj_next )
      {
         obj_next = obj->next_content;
         if( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
         {
            wear_obj( ch, obj, FALSE, -1, TRUE );
            if( char_died( ch ) )
               return;
         }
      }
      return;
   }
   else
   {
      if( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
      {
         send_to_char( "You do not have that item.\n\r", ch );
         return;
      }
      if( arg2[0] != '\0' )
         wear_bit = get_wflag( arg2 );
      else
         wear_bit = -1;
      wear_obj( ch, obj, TRUE, wear_bit, FALSE );
   }

   return;
}



void do_remove( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj, *obj_next;


   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "Remove what?\n\r", ch );
      return;
   }

   if( !str_cmp( arg, "all" ) )
   {
      for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
      {
         obj_next = obj->next_content;
         if( obj->wear_loc != WEAR_NONE && can_see_obj( ch, obj ) )
            remove_obj( ch, obj->wear_loc, TRUE );
      }
      act( AT_ACTION, "$n removes their equipment.", ch, NULL, NULL, TO_ROOM );
      return;
   }

   if( ( obj = get_obj_wear( ch, arg ) ) == NULL )
   {
      send_to_char( "You are not using that item.\n\r", ch );
      return;
   }
   if( ( obj_next = get_eq_char( ch, obj->wear_loc ) ) != obj )
   {
      act( AT_PLAIN, "You must remove $p first.", ch, obj_next, NULL, TO_CHAR );
      return;
   }

   act( AT_ACTION, "$n removes their equipment.", ch, NULL, NULL, TO_ROOM );

   remove_obj( ch, obj->wear_loc, TRUE );
   return;
}


void do_bury( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   bool shovel;
   sh_int move;

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "What do you wish to bury?\n\r", ch );
      return;
   }

   shovel = FALSE;
   for( obj = ch->first_carrying; obj; obj = obj->next_content )
      if( obj->item_type == ITEM_SHOVEL )
      {
         shovel = TRUE;
         break;
      }

   obj = get_obj_list_rev( ch, arg, ch->in_room->last_content );
   if( !obj )
   {
      send_to_char( "You can't find it.\n\r", ch );
      return;
   }

   separate_obj( obj );
   if( !CAN_WEAR( obj, ITEM_TAKE ) )
   {
      if( !IS_OBJ_STAT( obj, ITEM_CLANCORPSE ) || IS_NPC( ch ) || !IS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) )
      {
         act( AT_PLAIN, "You cannot bury $p.", ch, obj, 0, TO_CHAR );
         return;
      }
   }

   switch ( ch->in_room->sector_type )
   {
      case SECT_CITY:
      case SECT_INSIDE:
         send_to_char( "The floor is too hard to dig through.\n\r", ch );
         return;
      case SECT_WATER_SWIM:
      case SECT_WATER_NOSWIM:
      case SECT_UNDERWATER:
         send_to_char( "You cannot bury something here.\n\r", ch );
         return;
      case SECT_AIR:
         send_to_char( "What?  In the air?!\n\r", ch );
         return;
   }

   if( obj->weight > ( UMAX( 5, ( can_carry_w( ch ) / 10 ) ) ) && !shovel )
   {
      send_to_char( "You'd need a shovel to bury something that big.\n\r", ch );
      return;
   }

   move = ( obj->weight * 50 * ( shovel ? 1 : 5 ) ) / UMAX( 1, can_carry_w( ch ) );
   move = URANGE( 2, move, 1000 );
   if( move > ch->move )
   {
      send_to_char( "You don't have the energy to bury something of that size.\n\r", ch );
      return;
   }
   ch->move -= move;

   act( AT_ACTION, "You solemnly bury $p...", ch, obj, NULL, TO_CHAR );
   act( AT_ACTION, "$n solemnly buries $p...", ch, obj, NULL, TO_ROOM );
   xSET_BIT( obj->extra_flags, ITEM_BURIED );
//    WAIT_STATE( ch, URANGE( 10, move / 2, 100 ) );
   return;
}

void do_sacrifice( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   char name[50];
   OBJ_DATA *obj;
   int scost;

   if( IS_AGOD( ch ) )
   {
      send_to_char( "Can't Sacrifice to yourself. :p", ch );
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' || !str_cmp( arg, ch->name ) )
   {
      act( AT_ACTION, "$n offers $mself to $s deity, who graciously declines.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "Your deity appreciates your offer and may accept it later.\n\r", ch );
      return;
   }

   obj = get_obj_list_rev( ch, arg, ch->in_room->last_content );
   if( !obj )
   {
      send_to_char( "You can't find it.\n\r", ch );
      return;
   }

   separate_obj( obj );
   if( !CAN_WEAR( obj, ITEM_TAKE ) )
   {
      act( AT_PLAIN, "$p is not an acceptable sacrifice.", ch, obj, 0, TO_CHAR );
      return;
   }
   if( obj->owner && obj->owner[0] != '\0' )
   {
      act( AT_PLAIN, "$p is owned and can not be sacrificed..", ch, obj, 0, TO_CHAR );
      return;
   }
   if( obj->item_type == ITEM_KEYITEM )
   {
      act( AT_PLAIN, "$p is a key item and can not be sacrificed..", ch, obj, 0, TO_CHAR );
      return;
   }
   if( IS_SET( obj->magic_flags, ITEM_PKDISARMED ) && !IS_NPC( ch ) )
   {
      if( CAN_PKILL( ch ) && !get_timer( ch, TIMER_PKILLED ) )
      {
         if( ch->level - obj->value[5] > 5 || obj->value[5] - ch->level > 5 )
         {
            send_to_char_color( "\n\r&bA godly force freezes your outstretched hand.\n\r", ch );
            return;
         }
      }
   }
   if( !IS_NPC( ch ) && ch->pcdata->deity && ch->pcdata->deity->name[0] != '\0' )
   {
      strcpy( name, ch->pcdata->deity->name );
   }
   else if( !IS_NPC( ch ) && IS_GUILDED( ch ) && sysdata.guild_overseer[0] != '\0' )
   {
      strcpy( name, sysdata.guild_overseer );
   }
   else if( !IS_NPC( ch ) && ch->pcdata->clan && ch->pcdata->clan->deity[0] != '\0' )
   {
      strcpy( name, ch->pcdata->clan->deity );
   }
   else
   {
      static char *god_name_table[] = {
         "Durdyn", "Khesekh", "Nicole", "Proteus", "Ragnos", "Kianen", "Swiftsword", "Corinthian",
         "Galley", "Kinson", "Nadreck", "Shinobi"
      };
      strcpy( name, god_name_table[number_range( 0, 11 )] );
   }
   scost = ( obj->level * 2 );
   ch->gold += scost;
   sprintf( buf, "%s gives you %d katyr for your sacrifice of %s.\n\r", name, scost, obj->short_descr );
   send_to_char( buf, ch );
   sprintf( buf, "$n sacrifices $p to %s.", name );
   act( AT_ACTION, buf, ch, obj, NULL, TO_ROOM );
   oprog_sac_trigger( ch, obj );
   if( obj_extracted( obj ) )
      return;
   if( cur_obj == obj->serial )
      global_objcode = rOBJ_SACCED;
   extract_obj( obj );
   return;
}

void do_brandish( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   OBJ_DATA *staff;
   ch_ret retcode;
   int sn;

   if( ( staff = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
   {
      send_to_char( "You hold nothing in your hand.\n\r", ch );
      return;
   }

   if( staff->item_type != ITEM_STAFF )
   {
      send_to_char( "You can brandish only with a staff.\n\r", ch );
      return;
   }

   if( ( sn = staff->value[3] ) < 0 || sn >= top_sn || skill_table[sn]->spell_fun == NULL )
   {
      bug( "Do_brandish: bad sn %d.", sn );
      return;
   }

   WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

   if( staff->value[2] > 0 )
   {
      if( !oprog_use_trigger( ch, staff, NULL, NULL, NULL ) )
      {
         act( AT_MAGIC, "$n brandishes $p.", ch, staff, NULL, TO_ROOM );
         act( AT_MAGIC, "You brandish $p.", ch, staff, NULL, TO_CHAR );
      }
      for( vch = ch->in_room->first_person; vch; vch = vch_next )
      {
         vch_next = vch->next_in_room;
         if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
            continue;
         else
            switch ( skill_table[sn]->target )
            {
               default:
                  bug( "Do_brandish: bad target for sn %d.", sn );
                  return;

               case TAR_IGNORE:
                  if( vch != ch )
                     continue;
                  break;

               case TAR_CHAR_OFFENSIVE:
                  if( IS_NPC( ch ) ? IS_NPC( vch ) : !IS_NPC( vch ) )
                     continue;
                  break;

               case TAR_CHAR_DEFENSIVE:
                  if( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
                     continue;
                  break;

               case TAR_CHAR_SELF:
                  if( vch != ch )
                     continue;
                  break;
            }

         retcode = obj_cast_spell( staff->value[3], staff->value[0], ch, vch, NULL );
         if( retcode == rCHAR_DIED || retcode == rBOTH_DIED )
         {
            bug( "do_brandish: char died", 0 );
            return;
         }
      }
   }

   if( --staff->value[2] <= 0 )
   {
      act( AT_MAGIC, "$p blazes bright and vanishes from $n's hands!", ch, staff, NULL, TO_ROOM );
      act( AT_MAGIC, "$p blazes bright and is gone!", ch, staff, NULL, TO_CHAR );
      if( staff->serial == cur_obj )
         global_objcode = rOBJ_USED;
      extract_obj( staff );
   }

   return;
}



void do_zap( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *wand;
   OBJ_DATA *obj;
   ch_ret retcode;

   one_argument( argument, arg );
   if( arg[0] == '\0' && !ch->fighting )
   {
      send_to_char( "Zap whom or what?\n\r", ch );
      return;
   }

   if( ( wand = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
   {
      send_to_char( "You hold nothing in your hand.\n\r", ch );
      return;
   }

   if( wand->item_type != ITEM_WAND )
   {
      send_to_char( "You can zap only with a wand.\n\r", ch );
      return;
   }

   obj = NULL;
   if( arg[0] == '\0' )
   {
      if( ch->fighting )
      {
         victim = who_fighting( ch );
      }
      else
      {
         send_to_char( "Zap whom or what?\n\r", ch );
         return;
      }
   }
   else
   {
      if( ( victim = get_char_room( ch, arg ) ) == NULL && ( obj = get_obj_here( ch, arg ) ) == NULL )
      {
         send_to_char( "You can't find it.\n\r", ch );
         return;
      }
   }

   WAIT_STATE( ch, 1 * PULSE_VIOLENCE );

   if( wand->value[2] > 0 )
   {
      if( victim )
      {
         if( !oprog_use_trigger( ch, wand, victim, NULL, NULL ) )
         {
            act( AT_MAGIC, "$n aims $p at $N.", ch, wand, victim, TO_ROOM );
            act( AT_MAGIC, "You aim $p at $N.", ch, wand, victim, TO_CHAR );
         }
      }
      else
      {
         if( !oprog_use_trigger( ch, wand, NULL, obj, NULL ) )
         {
            act( AT_MAGIC, "$n aims $p at $P.", ch, wand, obj, TO_ROOM );
            act( AT_MAGIC, "You aim $p at $P.", ch, wand, obj, TO_CHAR );
         }
      }

      retcode = obj_cast_spell( wand->value[3], wand->value[0], ch, victim, obj );
      if( retcode == rCHAR_DIED || retcode == rBOTH_DIED )
      {
         bug( "do_zap: char died", 0 );
         return;
      }
   }

   if( --wand->value[2] <= 0 )
   {
      act( AT_MAGIC, "$p explodes into fragments.", ch, wand, NULL, TO_ROOM );
      act( AT_MAGIC, "$p explodes into fragments.", ch, wand, NULL, TO_CHAR );
      if( wand->serial == cur_obj )
         global_objcode = rOBJ_USED;
      extract_obj( wand );
   }

   return;
}

void save_clan_storeroom( CHAR_DATA * ch )
{
   FILE *fp;
   char filename[256];
   OBJ_DATA *contents;

   if( !ch )
   {
      bug( "save_clan_storeroom: Null ch pointer!", 0 );
      return;
   }

   sprintf( filename, "%s%d.vault", STORAGE_DIR, ch->in_room->vnum );
   if( ( fp = fopen( filename, "w" ) ) == NULL )
   {
      bug( "save_clan_storeroom: fopen", 0 );
      perror( filename );
   }
   else
   {
      contents = ch->in_room->last_content;
      fprintf( fp, "#VNUM %d\n", ch->in_room->vnum );
      if( contents && !IS_OBJ_STAT( contents, ITEM_CLANOBJECT ) )
         fwrite_obj( ch, contents, fp, 0, OS_CARRY );
      fprintf( fp, "#END\n" );
      fclose( fp );
      return;
   }
   return;
}

void save_donation( CHAR_DATA * ch, OBJ_DATA * obj )
{
   FILE *fp;
   char filename[256];
   OBJ_DATA *contents;

   if( !obj )
   {
      bug( "save_donation: Null obj pointer!", 0 );
      return;
   }

   sprintf( filename, "%s%d.vault", STORAGE_DIR, obj->in_room->vnum );
   if( ( fp = fopen( filename, "w" ) ) == NULL )
   {
      bug( "save_donation: fopen", 0 );
      perror( filename );
   }
   else
   {
      contents = obj->in_room->last_content;
      fprintf( fp, "#VNUM %d\n", obj->in_room->vnum );
      if( contents && !IS_OBJ_STAT( contents, ITEM_CLANOBJECT ) )
         fwrite_obj( ch, contents, fp, 0, OS_CARRY );
      fprintf( fp, "#END\n" );
      fclose( fp );
      return;
   }
   return;
}

void do_auction( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *obj;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int gbid = 0;
//    int sbid = 0;
//    int cbid = 0;
   int tmpvalue = 0;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   set_char_color( AT_LBLUE, ch );

   if( IS_NPC( ch ) )
      return;

   if( ch->level < 3 )
   {
      send_to_char( "You must be at least level three to use the auction...\n\r", ch );
      return;
   }
/*
    if ( ( time_info.hour > 18 || time_info.hour < 9 )
    && auction->item == NULL
    && !IS_IMMORTAL( ch ) )
    {
	send_to_char ("\n\rThe auctioneer works between the hours of 9 AM and 6 PM\n\r", ch );
        return;
    }
*/
   if( arg1[0] == '\0' )
   {
      if( auction->item != NULL )
      {
         AFFECT_DATA *paf;
         obj = auction->item;

         tmpvalue = auction->bet;
         gbid = tmpvalue;
         if( auction->bet > 0 )
         {
            if( gbid > 0 )
               sprintf( buf, "\n\rCurrent bid on this item is %d katyr\n\r", gbid );
            else
               sprintf( buf, "Error! report to Nicole.\n\r" );
         }
         else
            sprintf( buf, "\n\rNo bids on this item have been received.\n\r" );
         set_char_color( AT_BLUE, ch );
         send_to_char( buf, ch );

         sprintf( buf, "Object '%s' is %s, special properties: %s\n\rIts weight is %d,\n\r",
                  obj->name, aoran( item_type_name( obj ) ), extra_bit_name( &obj->extra_flags ), obj->weight );
         set_char_color( AT_LBLUE, ch );
         send_to_char( buf, ch );
         sprintf( buf, "Its value is: %d katyr Level: %d\n\r", obj->cost, obj->level );
         set_char_color( AT_LBLUE, ch );
         send_to_char( buf, ch );
         if( obj->item_type != ITEM_LIGHT && obj->wear_flags - 1 > 0 )
            ch_printf( ch, "Item's wear location: %s\n\r", flag_string( obj->wear_flags - 1, w_flags ) );

         set_char_color( AT_BLUE, ch );

         switch ( obj->item_type )
         {
            case ITEM_CONTAINER:
            case ITEM_KEYRING:
            case ITEM_QUIVER:
               ch_printf( ch, "%s appears to %s.\n\r", capitalize( obj->short_descr ),
                          obj->value[0] < 76 ? "have a small capacity" :
                          obj->value[0] < 150 ? "have a small to medium capacity" :
                          obj->value[0] < 300 ? "have a medium capacity" :
                          obj->value[0] < 500 ? "have a medium to large capacity" :
                          obj->value[0] < 751 ? "have a large capacity" : "have a giant capacity" );
               break;

            case ITEM_PILL:
            case ITEM_SCROLL:
            case ITEM_POTION:
               sprintf( buf, "Level %d spells of:", obj->value[0] );
               send_to_char( buf, ch );

               if( obj->value[1] >= 0 && obj->value[1] < top_sn )
               {
                  send_to_char( " '", ch );
                  send_to_char( skill_table[obj->value[1]]->name, ch );
                  send_to_char( "'", ch );
               }

               if( obj->value[2] >= 0 && obj->value[2] < top_sn )
               {
                  send_to_char( " '", ch );
                  send_to_char( skill_table[obj->value[2]]->name, ch );
                  send_to_char( "'", ch );
               }

               if( obj->value[3] >= 0 && obj->value[3] < top_sn )
               {
                  send_to_char( " '", ch );
                  send_to_char( skill_table[obj->value[3]]->name, ch );
                  send_to_char( "'", ch );
               }

               send_to_char( ".\n\r", ch );
               break;

            case ITEM_WAND:
            case ITEM_STAFF:
               sprintf( buf, "Has %d(%d) charges of level %d", obj->value[1], obj->value[2], obj->value[0] );
               send_to_char( buf, ch );

               if( obj->value[3] >= 0 && obj->value[3] < top_sn )
               {
                  send_to_char( " '", ch );
                  send_to_char( skill_table[obj->value[3]]->name, ch );
                  send_to_char( "'", ch );
               }

               send_to_char( ".\n\r", ch );
               break;

            case ITEM_MISSILE_WEAPON:
            case ITEM_WEAPON:
               sprintf( buf, "Damage is %d to %d (average %d).%s\n\r",
                        obj->value[1], obj->value[2],
                        ( obj->value[1] + obj->value[2] ) / 2,
                        IS_OBJ_STAT( obj, ITEM_POISONED ) ? "\n\rThis weapon is poisoned." : "" );
               send_to_char( buf, ch );
               break;

            case ITEM_ARMOR:
               sprintf( buf, "Armor class is %d.\n\r", obj->value[0] );
               send_to_char( buf, ch );
               break;

            case ITEM_FLUTE:
               sprintf( buf, "Improves innate defense by AC %d.\n\r", obj->value[0] );
               send_to_char( buf, ch );
               break;
         }

         for( paf = obj->pIndexData->first_affect; paf; paf = paf->next )
            showaffect( ch, paf );

         for( paf = obj->first_affect; paf; paf = paf->next )
            showaffect( ch, paf );
         if( ( obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_KEYRING
               || obj->item_type == ITEM_QUIVER ) && obj->first_content )
         {
            set_char_color( AT_OBJECT, ch );
            send_to_char( "Contents:\n\r", ch );
            show_list_to_char( obj->first_content, ch, TRUE, FALSE );
         }

         if( IS_IMMORTAL( ch ) )
         {
            sprintf( buf, "Seller: %s.  Bidder: %s.  Round: %d.\n\r",
                     auction->seller->name, auction->buyer->name, ( auction->going + 1 ) );
            send_to_char( buf, ch );
            sprintf( buf, "Time left in round: %d.\n\r", auction->pulse );
            send_to_char( buf, ch );
         }
         return;
      }
      else
      {
         set_char_color( AT_LBLUE, ch );
         send_to_char( "\n\rThere is nothing being auctioned right now.  What would you like to auction?\n\r", ch );
         return;
      }
   }

   if( IS_IMMORTAL( ch ) && !str_cmp( arg1, "stop" ) )
   {
      if( auction->item == NULL )
      {
         send_to_char( "There is no auction to stop.\n\r", ch );
         return;
      }
      else
      {
         set_char_color( AT_LBLUE, ch );
         sprintf( buf, "Sale of %s has been stopped by an Immortal.", auction->item->short_descr );
         talk_auction( buf );
         obj_to_char( auction->item, auction->seller );
         if( IS_SET( sysdata.save_flags, SV_AUCTION ) )
            save_char_obj( auction->seller );
         auction->item = NULL;
         if( auction->buyer != NULL && auction->buyer != auction->seller )
         {
            auction->buyer->gold += gbid;
            send_to_char( "Your money has been returned.\n\r", auction->buyer );
         }
         return;
      }
   }

   if( !str_cmp( arg1, "bid" ) )
   {
      if( auction->item != NULL )
      {
         int newbet;
         int chwealth;

         tmpvalue = auction->bet;
         gbid = tmpvalue;

         if( ch->level < auction->item->level )
         {
            send_to_char( "This object's level is too high for your use.\n\r", ch );
            return;
         }

         if( ch == auction->seller )
         {
            send_to_char( "You can't bid on your own item!\n\r", ch );
            return;
         }

         if( arg2[0] == '\0' )
         {
            send_to_char( "Bid how much?\n\r", ch );
            return;
         }

         newbet = parsebet( auction->bet, arg2 );

         if( newbet < auction->starting )
         {
            send_to_char( "You must place a bid that is higher than the starting bet.\n\r", ch );
            return;
         }


         if( newbet < ( auction->bet + 10 ) )
         {
            send_to_char( "You must at least bid 10 katyr over the current bid.\n\r", ch );
            return;
         }

         chwealth = ch->gold;
         if( newbet > chwealth )
         {
            send_to_char( "You don't have that much money!\n\r", ch );
            return;
         }
         if( newbet > 2000000000 )
         {
            send_to_char( "You can't bid over 2 billion coins.\n\r", ch );
            return;
         }

         if( auction->buyer != NULL && auction->buyer != auction->seller )
         {
            send_to_char( "The auctioneer returns your money.\n\r", auction->buyer );
            auction->buyer->gold += gbid;
         }

         tmpvalue = newbet;
         gbid = tmpvalue;
         act( AT_ACTION, "The auctioneer appears before $n, demanding some money.", ch, NULL, NULL, TO_ROOM );
         act( AT_ACTION, "The auctioneer appears before you demanding your bidded money.", ch, NULL, NULL, TO_CHAR );
         send_to_char( "You give your money to the auctioneer.\n\r", ch );
         ch->gold -= gbid;

         if( IS_SET( sysdata.save_flags, SV_AUCTION ) )
            save_char_obj( ch );
         auction->buyer = ch;
         auction->bet = newbet;
         auction->going = 0;
         auction->pulse = PULSE_AUCTION;

         if( gbid > 0 )
         {
            sprintf( buf, "&PA bid of &Y%d &Pkatyr has been received on %s.&D\n\r", gbid, auction->item->short_descr );
         }
         else
         {
            sprintf( buf, "Error! report to Nicole\n\r" );
         }
         talk_auction( buf );
         return;


      }
      else
      {
         send_to_char( "There isn't anything being auctioned right now.\n\r", ch );
         return;
      }
   }

   obj = get_obj_carry( ch, arg1 );

   if( obj == NULL )
   {
      send_to_char( "You aren't carrying that.\n\r", ch );
      return;
   }

   if( obj->timer > 0 )
   {
      send_to_char( "You can't auction objects that are decaying.\n\r", ch );
      return;
   }


   if( arg2[0] == '\0' )
   {
      auction->starting = 0;
      strcpy( arg2, "0" );
   }

   if( !is_number( arg2 ) )
   {
      send_to_char( "You must input a number at which to start the auction.\n\r", ch );
      return;
   }

   if( atoi( arg2 ) <= 0 )
   {
      send_to_char( "You can't auction something for no coins!\n\r", ch );
      return;
   }

   if( auction->item == NULL )
      switch ( obj->item_type )
      {

         default:
            act( AT_TELL, "You cannot auction $Ts.", ch, NULL, item_type_name( obj ), TO_CHAR );
            return;

         case ITEM_LIGHT:
         case ITEM_TREASURE:
         case ITEM_POTION:
         case ITEM_CONTAINER:
         case ITEM_KEYRING:
         case ITEM_QUIVER:
         case ITEM_DRINK_CON:
         case ITEM_FOOD:
         case ITEM_COOK:
         case ITEM_PEN:
         case ITEM_BOAT:
         case ITEM_PILL:
         case ITEM_PIPE:
         case ITEM_HERB_CON:
         case ITEM_INCENSE:
         case ITEM_FIRE:
         case ITEM_RUNEPOUCH:
         case ITEM_MAP:
         case ITEM_BOOK:
         case ITEM_RUNE:
         case ITEM_MATCH:
         case ITEM_HERB:
         case ITEM_WEAPON:
         case ITEM_MISSILE_WEAPON:
         case ITEM_ARMOR:
         case ITEM_FLUTE:
         case ITEM_GHOST:
         case ITEM_STAFF:
         case ITEM_WAND:
         case ITEM_SCROLL:
            separate_obj( obj );
            obj_from_char( obj );
            if( IS_SET( sysdata.save_flags, SV_AUCTION ) )
               save_char_obj( ch );
            auction->item = obj;
            auction->bet = 0;
            auction->buyer = ch;
            auction->seller = ch;
            auction->pulse = PULSE_AUCTION;
            auction->going = 0;
            auction->starting = atoi( arg2 );

            if( AUCTION_MEM > 0 )
            {
               memmove( ( char * )auction->history + sizeof( OBJ_INDEX_DATA * ),
                        auction->history, ( AUCTION_MEM - 1 ) * sizeof( OBJ_INDEX_DATA * ) );
               auction->history[0] = obj->pIndexData;
            }

            auction->hist_timer = 0;


            if( auction->starting > 0 )
               auction->bet = auction->starting;

            tmpvalue = auction->starting;
            gbid = tmpvalue;

            if( auction->starting == 0 )
            {
               sprintf( buf, "&PA new item is being auctioned&R:&P %s&P at &Y0 &Pcoins.&D", obj->short_descr );
            }
            else if( gbid > 0 )
            {
               sprintf( buf, "&PA new item is being auctioned&R:&P %s&P at &Y%d&P katyr.&D", obj->short_descr, gbid );
            }
            else
            {
               sprintf( buf, "Error! report to Nicole.\n\r" );
            }
            talk_auction( buf );

            return;

      }
   else
   {
      act( AT_TELL, "&PTry again later &R-&P $P is being auctioned right now!&D", ch, auction->item, NULL, TO_CHAR );
// if ( !IS_IMMORTAL(ch) )
//          WAIT_STATE( ch, PULSE_VIOLENCE );
      return;
   }
}




void obj_fall( OBJ_DATA * obj, bool through )
{
   EXIT_DATA *pexit;
   ROOM_INDEX_DATA *to_room;
   static int fall_count;
   char buf[MAX_STRING_LENGTH];
   static bool is_falling;

   if( !obj->in_room || is_falling )
      return;

   if( fall_count > 30 )
   {
      bug( "object falling in loop more than 30 times", 0 );
      extract_obj( obj );
      fall_count = 0;
      return;
   }

   if( xIS_SET( obj->in_room->room_flags, ROOM_NOFLOOR ) && CAN_GO( obj, DIR_DOWN ) && !IS_OBJ_STAT( obj, ITEM_MAGIC ) )
   {

      pexit = get_exit( obj->in_room, DIR_DOWN );
      to_room = pexit->to_room;

      if( through )
         fall_count++;
      else
         fall_count = 0;

      if( obj->in_room == to_room )
      {
         sprintf( buf, "Object falling into same room, room %d", to_room->vnum );
         bug( buf, 0 );
         extract_obj( obj );
         return;
      }

      if( obj->in_room->first_person )
      {
         act( AT_PLAIN, "$p falls far below...", obj->in_room->first_person, obj, NULL, TO_ROOM );
         act( AT_PLAIN, "$p falls far below...", obj->in_room->first_person, obj, NULL, TO_CHAR );
      }
      obj_from_room( obj );
      is_falling = TRUE;
      obj = obj_to_room( obj, to_room );
      is_falling = FALSE;

      if( obj->in_room->first_person )
      {
         act( AT_PLAIN, "$p falls from above...", obj->in_room->first_person, obj, NULL, TO_ROOM );
         act( AT_PLAIN, "$p falls from above...", obj->in_room->first_person, obj, NULL, TO_CHAR );
      }

      if( !xIS_SET( obj->in_room->room_flags, ROOM_NOFLOOR ) && through )
      {
         int dam = fall_count * obj->weight / 2;
         if( obj->in_room->first_person && number_percent(  ) > 15 )
         {
            CHAR_DATA *rch;
            CHAR_DATA *vch = NULL;
            int chcnt = 0;

            for( rch = obj->in_room->first_person; rch; rch = rch->next_in_room, chcnt++ )
               if( number_range( 0, chcnt ) == 0 )
                  vch = rch;
            act( AT_WHITE, "$p falls on $n!", vch, obj, NULL, TO_ROOM );
            act( AT_WHITE, "$p falls on you!", vch, obj, NULL, TO_CHAR );

            damage( vch, vch, dam * vch->level, TYPE_UNDEFINED );
         }
         switch ( obj->item_type )
         {
            case ITEM_WEAPON:
            case ITEM_ARMOR:
            case ITEM_FLUTE:
               if( ( obj->value[0] - dam ) <= 0 )
               {
                  if( obj->in_room->first_person )
                  {
                     act( AT_PLAIN, "$p is destroyed by the fall!", obj->in_room->first_person, obj, NULL, TO_ROOM );
                     act( AT_PLAIN, "$p is destroyed by the fall!", obj->in_room->first_person, obj, NULL, TO_CHAR );
                  }
                  make_scraps( obj );
               }
               else
                  obj->value[0] -= dam;
               break;
            default:
               if( ( dam * 15 ) > get_obj_resistance( obj ) )
               {
                  if( obj->in_room->first_person )
                  {
                     act( AT_PLAIN, "$p is destroyed by the fall!", obj->in_room->first_person, obj, NULL, TO_ROOM );
                     act( AT_PLAIN, "$p is destroyed by the fall!", obj->in_room->first_person, obj, NULL, TO_CHAR );
                  }
                  make_scraps( obj );
               }
               break;
         }
      }
      obj_fall( obj, TRUE );
   }
   return;
}


void do_findnote( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *obj;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      send_to_char( "You must specify at least one keyword.\n\r", ch );
      return;
   }

   obj = recursive_note_find( ch->first_carrying, argument );

   if( obj )
   {
      if( obj->in_obj )
      {
         obj_from_obj( obj );
         obj = obj_to_char( obj, ch );
      }
      wear_obj( ch, obj, TRUE, -1, FALSE );
   }
   else
      send_to_char( "Note not found.\n\r", ch );
   return;
}

OBJ_DATA *recursive_note_find( OBJ_DATA * obj, char *argument )
{
   OBJ_DATA *returned_obj;
   bool match = TRUE;
   char *argcopy;
   char *subject;

   char arg[MAX_INPUT_LENGTH];
   char subj[MAX_STRING_LENGTH];

   if( !obj )
      return NULL;

   switch ( obj->item_type )
   {
      case ITEM_PAPER:

         if( ( subject = get_extra_descr( "_subject_", obj->first_extradesc ) ) == NULL )
            break;
         sprintf( subj, "%s", strlower( subject ) );
         subject = strlower( subj );

         argcopy = argument;

         while( match )
         {
            argcopy = one_argument( argcopy, arg );

            if( arg[0] == '\0' )
               break;

            if( !strstr( subject, arg ) )
               match = FALSE;
         }


         if( match )
            return obj;
         break;

      case ITEM_CONTAINER:
      case ITEM_CORPSE_NPC:
      case ITEM_CORPSE_PC:
         if( obj->first_content )
         {
            returned_obj = recursive_note_find( obj->first_content, argument );
            if( returned_obj )
               return returned_obj;
         }
         break;

      default:
         break;
   }

   return recursive_note_find( obj->next_content, argument );
}

void do_rolldie( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *die;

   char output_string[MAX_STRING_LENGTH];
   char roll_string[MAX_STRING_LENGTH];
   char total_string[MAX_STRING_LENGTH];

   char *verb;

   int rollsum = 0;
   int roll_count = 0;

   int numsides;
   int numrolls;

   bool *face_seen_table = NULL;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   if( ( die = get_eq_char( ch, WEAR_HOLD ) ) == NULL || die->item_type != ITEM_CHANCE )
   {
      ch_printf( ch, "You must be holding an item of chance!\n\r" );
      return;
   }

   numrolls = ( is_number( argument ) ) ? atoi( argument ) : 1;
   verb = get_chance_verb( die );

   if( numrolls > 100 )
   {
      ch_printf( ch, "You can't %s more than 100 times!\n\r", verb );
      return;
   }

   numsides = die->value[0];

   if( numsides <= 1 )
   {
      ch_printf( ch, "There is no element of chance in this game!\n\r" );
      return;
   }

   if( die->value[3] == 1 )
   {
      if( numrolls > numsides )
      {
         ch_printf( ch, "Nice try, but you can only %s %d times.\n\r", verb, numsides );
         return;
      }
      face_seen_table = MudCalloc( numsides, sizeof( bool ), 2 );
      if( !face_seen_table )
      {
         bug( "do_rolldie: cannot allocate memory for face_seen_table array, terminating.\n\r", 0 );
         return;
      }
   }

   sprintf( roll_string, " " );

   while( roll_count++ < numrolls )
   {
      int current_roll;
      char current_roll_string[MAX_STRING_LENGTH];

      do
      {
         current_roll = number_range( 1, numsides );
      }
      while( die->value[3] == 1 && face_seen_table[current_roll - 1] == TRUE );

      if( die->value[3] == 1 )
         face_seen_table[current_roll - 1] = TRUE;

      rollsum += current_roll;

      if( roll_count > 1 )
         strcat( roll_string, ", " );
      if( numrolls > 1 && roll_count == numrolls )
         strcat( roll_string, "and " );

      if( die->value[1] == 1 )
      {
         char *face_name = get_ed_number( die, current_roll );
         if( face_name )
         {
            char *face_name_copy = strdup( face_name );
            sprintf( current_roll_string, "%s", strtok( face_name_copy, "\n" ) );
            MudFree( face_name_copy, 7 );
         }
         else
            sprintf( current_roll_string, "%d", current_roll );
      }
      else
         sprintf( current_roll_string, "%d", current_roll );
      strcat( roll_string, current_roll_string );
   }

   if( numrolls > 1 && die->value[2] == 1 )
   {
      sprintf( total_string, ", for a total of %d", rollsum );
      strcat( roll_string, total_string );
   }

   strcat( roll_string, ".\n\r" );

   sprintf( output_string, "You %s%s", verb, roll_string );
   act( AT_GREEN, output_string, ch, NULL, NULL, TO_CHAR );

   sprintf( output_string, "$n %s%s", verb, roll_string );
   act( AT_GREEN, output_string, ch, NULL, NULL, TO_ROOM );

   if( face_seen_table )
      MudFree( face_seen_table, 8 );
   return;
}

char *get_ed_number( OBJ_DATA * obj, int number )
{
   EXTRA_DESCR_DATA *ed;
   int count;

   for( ed = obj->first_extradesc, count = 1; ed; ed = ed->next, count++ )
   {
      if( count == number )
         return ed->description;
   }

   return NULL;
}

char *get_chance_verb( OBJ_DATA * obj )
{
   return ( obj->action_desc[0] != '\0' ) ? obj->action_desc : "roll$q";
}


void do_forge( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *first_ob;
   OBJ_DATA *new_ob;

   char arg[MAX_STRING_LENGTH];
   char buf[MAX_STRING_LENGTH];

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: forge <ore>.\n\r", ch );
      return;
   }

   if( ( first_ob = get_obj_carry( ch, arg ) ) == NULL )
   {
      sprintf( buf, "You must be holding the ore.\n\r" );
      send_to_char( buf, ch );
      return;
   }

   if( !xIS_SET( ch->in_room->room_flags, ROOM_FORGE ) )
   {
      send_to_char( "You need to be in a forge.\n\r", ch );
      return;
   }

   if( first_ob->item_type != ITEM_ORE )
   {
      send_to_char( "It needs to be an ore.\n\r", ch );
      return;
   }

   if( number_range( 1, 100 ) > 30 )
   {
      new_ob = create_object( get_obj_index( first_ob->value[2] ), first_ob->level );
      extract_obj( first_ob );
      obj_to_char( new_ob, ch );
      act( AT_ACTION, "$n begins to smith some ore...\r\n\r\n\r\n\r\n ...when is finished, made $p!", ch, new_ob, NULL,
           TO_ROOM );
      act( AT_ACTION, "You begin to smith some ore...\r\n\r\n\r\n\r\n ...when is finished, made $p!", ch, new_ob, NULL,
           TO_CHAR );
   }
   else
   {
      extract_obj( first_ob );
      act( AT_ACTION, "$n fails to forge the ore.", ch, NULL, NULL, TO_ROOM );
      act( AT_ACTION, "You try to forge the ore but fail.", ch, NULL, NULL, TO_CHAR );
      return;
   }
   return;
}

void do_connect( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *first_ob;
   OBJ_DATA *second_ob;
   OBJ_DATA *new_ob;

   char arg1[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH];
   char buf[MAX_STRING_LENGTH];

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Syntax: Connect <firstobj> <secondobj>.\n\r", ch );
      return;
   }

   if( ( first_ob = get_obj_carry( ch, arg1 ) ) == NULL )
   {
      sprintf( buf, "You must be holding both parts!!\n\r" );
      send_to_char( buf, ch );
      return;
   }

   if( ( second_ob = get_obj_carry( ch, arg2 ) ) == NULL )
   {
      sprintf( buf, "You must be holding both parts!!\n\r" );
      send_to_char( buf, ch );
      return;
   }

   if( first_ob->item_type != ITEM_PIECE || second_ob->item_type != ITEM_PIECE )
   {
      send_to_char( "Both items must be pieces of another item!\n\r", ch );
      return;
   }


   if( ( first_ob->value[1] == second_ob->pIndexData->vnum )
       && ( second_ob->value[0] == first_ob->pIndexData->vnum )
       && ( first_ob->value[2] == second_ob->value[2] ) && ( first_ob->pIndexData->vnum != second_ob->pIndexData->vnum ) )
   {
      new_ob = create_object( get_obj_index( first_ob->value[2] ), ch->level );
      extract_obj( first_ob );
      extract_obj( second_ob );
      obj_to_char( new_ob, ch );
      act( AT_ACTION, "$n jiggles some pieces together...\r\n ...suddenly they snap in place, creating $p!", ch, new_ob,
           NULL, TO_ROOM );
      act( AT_ACTION, "You jiggle the pieces together...\r\n ...suddenly they snap into place, creating $p!", ch, new_ob,
           NULL, TO_CHAR );

   }
   else
   {
      act( AT_ACTION, "$n jiggles some pieces together, but can't seem to make them connect.", ch, NULL, NULL, TO_ROOM );
      act( AT_ACTION, "You try to fit them together every which way, but they just don't want to fit together.", ch, NULL,
           NULL, TO_CHAR );

      return;
   }

   return;
}

void do_donate( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *obj;
   char arg[MAX_INPUT_LENGTH];

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "Donate what?\n\r", ch );
      return;
   }

   if( ch->position == POS_FIGHTING )
   {
      send_to_char( "You cannot donate while fighting!\n\r", ch );
      return;
   }

   if( ( obj = get_obj_carry( ch, arg ) ) == NULL )
   {
      send_to_char( "You do not have that!\n\r", ch );
      return;
   }
   else
   {
      if( !can_drop_obj( ch, obj ) && ch->level < LEVEL_IMMORTAL )
      {
         send_to_char( "You cannot donate that, it's cursed!\n\r", ch );
         return;
      }

      if( obj->item_type == ITEM_KEYITEM )
      {
         send_to_char( "You cannot donate that, its a key item.\n\r", ch );
         return;
      }

      if( ( obj->item_type == ITEM_CORPSE_NPC ) || ( obj->item_type == ITEM_CORPSE_PC ) )
      {
         send_to_char( "You cannot donate corpses!\n\r", ch );
         return;
      }

      if( obj->owner && obj->owner[0] != '\0' )
      {
         send_to_char( "That is owned and can not be donated.\n\r", ch );
         return;
      }

      if( IS_OBJ_STAT( obj, ITEM_RARE ) && !IS_IMMORTAL( ch ) )
      {
         act( AT_PLAIN, "You cannot give a rare item away.", ch, NULL, NULL, TO_CHAR );
         return;
      }

      if( obj->timer > 0 )
      {
         send_to_char( "You cannot donate that.\n\r", ch );
         return;
      }


      if( obj->level <= 10 )
      {
         act( AT_ACTION, "You donate $p, how generous of you!", ch, obj, NULL, TO_CHAR );
         separate_obj( obj );
         obj_from_char( obj );
         obj_to_room( obj, get_room_index( ROOM_VNUM_DONATION ) );
         save_char_obj( ch );
         xSET_BIT( obj->extra_flags, ITEM_DONATION );
/*        if ( xIS_SET(obj->in_room->room_flags, ROOM_CLANSTOREROOM) )
        {
            save_donation( ch, obj );
        }*/
         return;
      }
      else
      {
         act( AT_ACTION, "You donate $p, how generous of you!", ch, obj, NULL, TO_CHAR );
         separate_obj( obj );
         obj_from_char( obj );
         obj_to_room( obj, get_room_index( ROOM_VNUM_DONATIONTWO ) );
         save_char_obj( ch );
         xSET_BIT( obj->extra_flags, ITEM_DONATION );
/*        if ( xIS_SET(obj->in_room->room_flags, ROOM_CLANSTOREROOM) )
        {
            save_donation( ch, obj );
        }*/
         return;
      }
   }
}

void do_junk( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *obj, *obj_next;
   char arg[MAX_INPUT_LENGTH];
   bool found = FALSE;

   if( IS_AGOD( ch ) && ch->substate == SUB_REPEATCMD )
   {
      send_to_char( "Please type 'done' before continueing.\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "Junk what?\n\r", ch );
      return;
   }

   for( obj = ch->first_carrying; obj; obj = obj_next )
   {
      obj_next = obj->next_content;
      if( ( nifty_is_name( arg, obj->name ) ) && can_see_obj( ch, obj ) && obj->wear_loc == WEAR_NONE )
      {
         if( obj->owner && obj->owner[0] != '\0' )
         {
            found = TRUE;
            break;
         }
      }
   }
   if( found )
   {
      if( !can_drop_obj( ch, obj ) && ch->level < LEVEL_IMMORTAL )
      {
         send_to_char( "You cannot junk that, it's cursed!\n\r", ch );
         return;
      }
      separate_obj( obj );
      obj_from_char( obj );
      extract_obj( obj );
      act( AT_ACTION, "$n junks $p.", ch, obj, NULL, TO_ROOM );
      act( AT_ACTION, "You junk $p.", ch, obj, NULL, TO_CHAR );
   }
   else
   {
      send_to_char( "That is not owned and can not be junked.\n\r", ch );
   }
   return;
}

void do_tap( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *obj;
   ROOM_INDEX_DATA *location;
   CHAR_DATA *opponent;
   char arg[MAX_INPUT_LENGTH];

   argument = one_argument( argument, arg );
   location = NULL;

   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: tap <item>\r\n", ch );
      return;
   }

   if( ms_find_obj( ch ) )
      return;

   if( ( obj = get_obj_wear( ch, arg ) ) == NULL )
   {
      send_to_char( "You do not have that item.\n\r", ch );
      return;
   }

   obj = get_obj_wear( ch, arg );

   if( obj->value[4] < 1 )
   {
      send_to_char_color( "&RIt has not been Marked yet!\n\r", ch );
      return;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_NO_TAP ) )
   {
      send_to_char( "Your badge seems to be short circuited.\n\r", ch );
      return;
   }

   if( obj->value[4] > -1 )
   {
      location = get_room_index( obj->value[4] );
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
         ch_printf( ch, "You return to your clan hall from combat!  You lose %d exps.\n\r", lose );
         stop_fighting( ch, TRUE );
      }

      act( AT_ACTION, "$n taps a badge on $s shoulder and dissappears.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "\n\rYou tap your badge...\r\n", ch );
      char_from_room( ch );
      char_to_room( ch, location );

      if( ch->mount )
      {
         char_from_room( ch->mount );
         char_to_room( ch->mount, location );
      }
      act( AT_ACTION, "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
      do_look( ch, "auto" );
   }
   return;
}

void make_fire( ROOM_INDEX_DATA * in_room, sh_int timer )
{
   OBJ_DATA *fire;

   fire = create_object( get_obj_index( OBJ_VNUM_FIRE ), 0 );
   fire->timer = number_fuzzy( timer );
   obj_to_room( fire, in_room );
   return;
}

OBJ_DATA *make_trap( int v0, int v1, int v2, int v3 )
{
   OBJ_DATA *trap;

   trap = create_object( get_obj_index( OBJ_VNUM_TRAP ), 0 );
   trap->timer = 0;
   trap->value[0] = v0;
   trap->value[1] = v1;
   trap->value[2] = v2;
   trap->value[3] = v3;
   return trap;
}

void make_scraps( OBJ_DATA * obj )
{
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *scraps, *tmpobj;
   CHAR_DATA *ch = NULL;

   separate_obj( obj );
   scraps = create_object( get_obj_index( OBJ_VNUM_SCRAPS ), 0 );
   scraps->timer = number_range( 5, 15 );

   if( obj->pIndexData->vnum == OBJ_VNUM_SCRAPS )
   {
      STRFREE( scraps->short_descr );
      scraps->short_descr = STRALLOC( "some debris" );
      STRFREE( scraps->description );
      scraps->description = STRALLOC( "Bits of debris lie on the ground here." );
   }
   else
   {
      sprintf( buf, scraps->short_descr, obj->short_descr );
      STRFREE( scraps->short_descr );
      scraps->short_descr = STRALLOC( buf );
      sprintf( buf, scraps->description, obj->short_descr );
      STRFREE( scraps->description );
      scraps->description = STRALLOC( buf );
   }

   if( obj->carried_by )
   {
      act( AT_OBJECT, "$p falls to the ground in scraps!", obj->carried_by, obj, NULL, TO_CHAR );
      if( obj == get_eq_char( obj->carried_by, WEAR_WIELD )
          && ( tmpobj = get_eq_char( obj->carried_by, WEAR_DUAL_WIELD ) ) != NULL )
         tmpobj->wear_loc = WEAR_WIELD;

      obj_to_room( scraps, obj->carried_by->in_room );
   }
   else if( obj->in_room )
   {
      if( ( ch = obj->in_room->first_person ) != NULL )
      {
         act( AT_OBJECT, "$p is reduced to little more than scraps.", ch, obj, NULL, TO_ROOM );
         act( AT_OBJECT, "$p is reduced to little more than scraps.", ch, obj, NULL, TO_CHAR );
      }
      obj_to_room( scraps, obj->in_room );
   }
   if( ( obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_KEYRING
         || obj->item_type == ITEM_QUIVER || obj->item_type == ITEM_CORPSE_PC ) && obj->first_content )
   {
      if( ch && ch->in_room )
      {
         act( AT_OBJECT, "The contents of $p fall to the ground.", ch, obj, NULL, TO_ROOM );
         act( AT_OBJECT, "The contents of $p fall to the ground.", ch, obj, NULL, TO_CHAR );
      }
      if( obj->carried_by )
         empty_obj( obj, NULL, obj->carried_by->in_room );
      else if( obj->in_room )
         empty_obj( obj, NULL, obj->in_room );
      else if( obj->in_obj )
         empty_obj( obj, obj->in_obj, NULL );
   }
   extract_obj( obj );
}


void make_corpse( CHAR_DATA * ch, CHAR_DATA * killer )
{
   char buf[MAX_STRING_LENGTH];
   ROOM_INDEX_DATA *location;
   OBJ_DATA *corpse;
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;
   char *name;

   location = ch->in_room;

   if( IS_NPC( ch ) )
   {
      name = ch->short_descr;
      corpse = create_object( get_obj_index( OBJ_VNUM_CORPSE_NPC ), ch->level );
      corpse->timer = 6;
      if( ch->gold > 0 )
      {
         if( ch->in_room )
         {
            ch->in_room->area->gold_looted += ch->gold;
            sysdata.global_gold_looted += ch->gold / 100;
         }
         obj_to_obj( create_money( ch->gold, 0 ), corpse );
         ch->gold = 0;
      }

      corpse->cost = ( -( int )ch->pIndexData->vnum );
      corpse->value[2] = corpse->timer;
   }
   else
   {
      name = ch->name;
      corpse = create_object( get_obj_index( OBJ_VNUM_CORPSE_PC ), ch->level );
      if( IN_ARENA( ch ) )
         corpse->timer = 0;
      else
         corpse->timer = 40;
      corpse->value[2] = ( int )( corpse->timer / 8 );
      corpse->value[4] = ch->level;
      if( CAN_PKILL( ch ) && sysdata.pk_loot )
         xSET_BIT( corpse->extra_flags, ITEM_CLANCORPSE );
      if( !IS_NPC( ch ) && !IS_NPC( killer ) )
         corpse->value[3] = 1;
      else
         corpse->value[3] = 0;
   }

   if( CAN_PKILL( ch ) && CAN_PKILL( killer ) && ch != killer )
   {
      sprintf( buf, "%s", killer->name );
      STRFREE( corpse->action_desc );
      corpse->action_desc = STRALLOC( buf );
   }

   if( IS_NPC( ch ) )
   {
      sprintf( buf, "corpse %s", name );
      STRFREE( corpse->name );
      corpse->name = STRALLOC( buf );
   }
   else
   {
      sprintf( buf, "pcc corpse %s", name );
      STRFREE( corpse->name );
      corpse->name = STRALLOC( buf );
   }

   sprintf( buf, corpse->short_descr, name );
   STRFREE( corpse->short_descr );
   corpse->short_descr = STRALLOC( buf );

   sprintf( buf, corpse->description, name );
   STRFREE( corpse->description );
   corpse->description = STRALLOC( buf );

   for( obj = ch->first_carrying; obj; obj = obj_next )
   {
      obj_next = obj->next_content;
      obj_from_char( obj );
      if(   /*IS_OBJ_STAT( obj, ITEM_INVENTORY )
             * || */ IS_OBJ_STAT( obj, ITEM_DEATHROT ) )
         extract_obj( obj );
      else
         obj_to_obj( obj, corpse );
   }

   obj_to_room_cr( corpse, ch->in_room, ch );

   return;
}



void make_blood( CHAR_DATA * ch )
{
   OBJ_DATA *obj;

   obj = create_object( get_obj_index( OBJ_VNUM_BLOOD ), 0 );
   obj->timer = number_range( 2, 4 );
   obj->value[1] = number_range( 3, UMIN( 5, ch->level ) );
   obj_to_room( obj, ch->in_room );
}


void make_bloodstain( CHAR_DATA * ch )
{
   OBJ_DATA *obj;

   obj = create_object( get_obj_index( OBJ_VNUM_BLOODSTAIN ), 0 );
   obj->timer = number_range( 1, 2 );
   obj_to_room( obj, ch->in_room );
}

OBJ_DATA *create_money( int amount, int type )
{
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *obj;

   if( amount <= 0 )
   {
      bug( "Create_money: zero or negative money %d.", amount );
      amount = 1;
   }

   if( type > 3 || type < 0 )
   {
      bug( "Create_money: wrong type: %d!", type );
      type = 2;
   }

   if( type == 0 )
   {
      if( amount == 1 )
      {
         obj = create_object( get_obj_index( OBJ_VNUM_GOLD_ONE ), 0 );
         return obj;
      }
      else
      {
         obj = create_object( get_obj_index( OBJ_VNUM_GOLD_SOME ), 0 );
         sprintf( buf, obj->short_descr, amount );
         STRFREE( obj->short_descr );
         obj->short_descr = STRALLOC( buf );
         obj->value[0] = amount;
         return obj;
      }
   }

   if( type == 1 )
   {
      if( amount == 1 )
      {
         obj = create_object( get_obj_index( OBJ_VNUM_SILVER_ONE ), 0 );
         return obj;
      }
      else
      {
         obj = create_object( get_obj_index( OBJ_VNUM_SILVER_SOME ), 0 );
         sprintf( buf, obj->short_descr, amount );
         STRFREE( obj->short_descr );
         obj->short_descr = STRALLOC( buf );
         obj->value[0] = amount;
         return obj;
      }
   }

   if( type == 2 )
   {
      if( amount == 1 )
      {
         obj = create_object( get_obj_index( OBJ_VNUM_COPPER_ONE ), 0 );
         return obj;
      }
      else
      {
         obj = create_object( get_obj_index( OBJ_VNUM_COPPER_SOME ), 0 );
         sprintf( buf, obj->short_descr, amount );
         STRFREE( obj->short_descr );
         obj->short_descr = STRALLOC( buf );
         obj->value[0] = amount;
         return obj;
      }
   }
   obj = create_object( get_obj_index( OBJ_VNUM_COPPER_ONE ), 0 );
   return obj;
}

/*
OBJ_DATA *create_money( int amount )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;

    if ( amount <= 0 )
    {
	bug( "Create_money: zero or negative money %d.", amount );
	amount = 1;
    }

    if ( amount == 1 )
    {
	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_ONE ), 0 );
    }
    else
    {
	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_SOME ), 0 );
	sprintf( buf, obj->short_descr, amount );
	STRFREE( obj->short_descr );
	obj->short_descr = STRALLOC( buf );
	obj->value[0]	 = amount;
    }

    return obj;
}
*/
