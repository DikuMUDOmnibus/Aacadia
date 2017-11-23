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
/*		    	      Information module			*/
/************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "acadia.h"

char *const where_name[] = {
   "&B<&cused as light&B>     ",
   "&B<&cworn on finger&B>    ",
   "&B<&cworn on finger&B>    ",
   "&B<&cworn around neck&B>  ",
   "&B<&cworn around neck&B>  ",
   "&B<&cworn on body&B>      ",
   "&B<&cworn on head&B>      ",
   "&B<&cworn on legs&B>      ",
   "&B<&cworn on feet&B>      ",
   "&B<&cworn on hands&B>     ",
   "&B<&cworn on arms&B>      ",
   "&B<&cworn as shield&B>    ",
   "&B<&cworn about body&B>   ",
   "&B<&cworn about waist&B>  ",
   "&B<&cworn around wrist&B> ",
   "&B<&cworn around wrist&B> ",
   "&B<&cwielded&B>           ",
   "&B<&cheld&B>              ",
   "&B<&cdual wielded&B>      ",
   "&B<&cworn on ears&B>      ",
   "&B<&cworn on eyes&B>      ",
   "&B<&cmissile wielded&B>   ",
   "&B<&cworn on back&B>      ",
   "&B<&cworn over face&B>    ",
   "&B<&cworn around ankle&B> ",
   "&B<&cworn around ankle&B> ",
   "&B<&cfloating nearby&B>   ",
   "&B<&cinsignia&B>          ",
   "&B<&clodged in arm&B>     ",
   "&B<&clodged in leg&B>     ",
   "&B<&clodged in rib&B>     ",
   "&B<&cBUG Inform Nicole&B> "
};


void show_char_to_char_0 args( ( CHAR_DATA * victim, CHAR_DATA * ch ) );
void show_char_to_char_1 args( ( CHAR_DATA * victim, CHAR_DATA * ch ) );
void show_char_to_char args( ( CHAR_DATA * list, CHAR_DATA * ch ) );
bool check_blind args( ( CHAR_DATA * ch ) );
void show_condition args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void look_window args( ( CHAR_DATA * ch, OBJ_DATA * obj ) );

sh_int str_similarity( const char *astr, const char *bstr );
sh_int str_prefix_level( const char *astr, const char *bstr );
void similar_help_files( CHAR_DATA * ch, char *argument );
int affnum2 = 0;

char *format_obj_to_char( OBJ_DATA * obj, CHAR_DATA * ch, bool fShort )
{
   static char buf[MAX_STRING_LENGTH];
   bool glowsee = FALSE;

   if( IS_OBJ_STAT( obj, ITEM_GLOW ) && IS_OBJ_STAT( obj, ITEM_INVIS )
       && !IS_AFFECTED( ch, AFF_TRUESIGHT ) && !IS_AFFECTED( ch, AFF_DETECT_INVIS ) && !IS_AFFECTED( ch, AFF_ETUDE ) )
      glowsee = TRUE;

   buf[0] = '\0';
   if( IS_OBJ_STAT( obj, ITEM_OWNED ) )
      strcat( buf, "(Owned)" );
   if( IS_OBJ_STAT( obj, ITEM_INVIS ) )
      strcat( buf, "(Invis)" );
   if( IS_OBJ_STAT( obj, ITEM_SHARP ) )
      strcat( buf, "(Sharp)" );
   if( ( IS_AFFECTED( ch, AFF_DETECT_EVIL ) || ch->class == CLASS_PALADIN ) && IS_OBJ_STAT( obj, ITEM_EVIL ) )
      strcat( buf, "(Evil)" );
   if( IS_AFFECTED( ch, AFF_DETECT_MAGIC ) && IS_OBJ_STAT( obj, ITEM_MAGIC ) )
      strcat( buf, "(Magic)" );
   if( !glowsee && IS_OBJ_STAT( obj, ITEM_GLOW ) )
      strcat( buf, "(Glow)" );
   if( IS_OBJ_STAT( obj, ITEM_HUM ) )
      strcat( buf, "(Humming)" );
   if( IS_OBJ_STAT( obj, ITEM_SILVER ) )
      strcat( buf, "(Silver)" );
   if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
      strcat( buf, "(Proto)" );
   if( IS_AFFECTED( ch, AFF_DETECTTRAPS ) && is_trapped( obj ) )
      strcat( buf, "(Trap)" );
   if( obj->value[6] > 0 )
      strcat( buf, "(HolyAura)" );
   if( obj->item_type == ITEM_WINDOW )
      strcat( buf, "(Window)" );
   if( buf[0] != '\0' )
      strcat( buf, " " );

   if( fShort )
   {
      if( glowsee && !IS_IMMORTAL( ch ) )
         strcat( buf, "the faint glow of something" );
      else if( obj->short_descr )
         strcat( buf, obj->short_descr );
   }
   else
   {
      if( glowsee )
         strcat( buf, "You see the faint glow of something nearby." );
      if( obj->description )
         strcat( buf, obj->description );
   }

   return buf;
}


char *hallucinated_object( int ms, bool fShort )
{
   int sms = URANGE( 1, ( ms + 10 ) / 5, 20 );

   if( fShort )
      switch ( number_range( 6 - URANGE( 1, sms / 2, 5 ), sms ) )
      {
         case 1:
            return "a sword";
         case 2:
            return "a stick";
         case 3:
            return "something shiny";
         case 4:
            return "something";
         case 5:
            return "something interesting";
         case 6:
            return "something colorful";
         case 7:
            return "something that looks cool";
         case 8:
            return "a nifty thing";
         case 9:
            return "a cloak of flowing colors";
         case 10:
            return "a mystical flaming sword";
         case 11:
            return "a swarm of insects";
         case 12:
            return "a deathbane";
         case 13:
            return "a figment of your imagination";
         case 14:
            return "your gravestone";
         case 15:
            return "the long lost boots of Ranger Thoric";
         case 16:
            return "a glowing tome of arcane knowledge";
         case 17:
            return "a long sought secret";
         case 18:
            return "the meaning of it all";
         case 19:
            return "the answer";
         case 20:
            return "the key to life, the universe and everything";
      }
   switch ( number_range( 6 - URANGE( 1, sms / 2, 5 ), sms ) )
   {
      case 1:
         return "A nice looking sword catches your eye.";
      case 2:
         return "The ground is covered in small sticks.";
      case 3:
         return "Something shiny catches your eye.";
      case 4:
         return "Something catches your attention.";
      case 5:
         return "Something interesting catches your eye.";
      case 6:
         return "Something colorful flows by.";
      case 7:
         return "Something that looks cool calls out to you.";
      case 8:
         return "A nifty thing of great importance stands here.";
      case 9:
         return "A cloak of flowing colors asks you to wear it.";
      case 10:
         return "A mystical flaming sword awaits your grasp.";
      case 11:
         return "A swarm of insects buzzes in your face!";
      case 12:
         return "The extremely rare Deathbane lies at your feet.";
      case 13:
         return "A figment of your imagination is at your command.";
      case 14:
         return "You notice a gravestone here... upon closer examination, it reads your name.";
      case 15:
         return "The long lost boots of Ranger Thoric lie off to the side.";
      case 16:
         return "A glowing tome of arcane knowledge hovers in the air before you.";
      case 17:
         return "A long sought secret of all mankind is now clear to you.";
      case 18:
         return "The meaning of it all, so simple, so clear... of course!";
      case 19:
         return "The answer.  One.  It's always been One.";
      case 20:
         return "The key to life, the universe and everything awaits your hand.";
   }
   return "Whoa!!!";
}


char *num_punct( int foo )
{
   int index, index_new, rest;
   char buf[16];
   static char buf_new[16];

   sprintf( buf, "%d", foo );
   rest = strlen( buf ) % 3;

   for( index = index_new = 0; index < strlen( buf ); index++, index_new++ )
   {
      if( index != 0 && ( index - rest ) % 3 == 0 )
      {
         buf_new[index_new] = ',';
         index_new++;
         buf_new[index_new] = buf[index];
      }
      else
         buf_new[index_new] = buf[index];
   }
   buf_new[index_new] = '\0';
   return strdup( buf_new );
}


void show_list_to_char_inv( OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing, int itemtype )
{
   char **prgpstrShow;
   int *prgnShow;
   int *pitShow;
   char *pstrShow;
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *obj;
   int nShow;
   int cond;
   int iShow;
   int type = 0;
   int count, offcount, tmp, cnt;
   bool fCombine;

   if( !ch->desc )
      return;

   if( itemtype > 0 )
      type = 1;
   if( !list )
   {
      if( fShowNothing )
      {
         if( IS_NPC( ch ) || xIS_SET( ch->act, PLR_COMBINE ) )
            send_to_char( "     ", ch );
         set_char_color( AT_OBJECT, ch );
         send_to_char( "Nothing.\n\r", ch );
      }
      return;
   }
   count = 0;
   for( obj = list; obj; obj = obj->next_content )
      count++;

   offcount = 0;

   if( count + offcount <= 0 )
   {
      if( fShowNothing )
      {
         if( IS_NPC( ch ) || xIS_SET( ch->act, PLR_COMBINE ) )
            send_to_char( "     ", ch );
         set_char_color( AT_OBJECT, ch );
         send_to_char( "Nothing.\n\r", ch );
      }
      return;
   }

   CREATE( prgpstrShow, char *, count + ( ( offcount > 0 ) ? offcount : 0 ) );
   CREATE( prgnShow, int, count + ( ( offcount > 0 ) ? offcount : 0 ) );
   CREATE( pitShow, int, count + ( ( offcount > 0 ) ? offcount : 0 ) );
   nShow = 0;
   tmp = ( offcount > 0 ) ? offcount : 0;
   cnt = 0;

   for( obj = list; obj; obj = obj->next_content )
   {
      if( offcount < 0 && ++cnt > ( count + offcount ) )
         break;
      if( type == 1 )
      {
         if( obj->item_type != itemtype )
            continue;
      }

      if( obj->item_type == ITEM_KEYITEM )
         continue;

      if( tmp > 0 && number_bits( 1 ) == 0 )
      {
         prgnShow[nShow] = 1;
         pitShow[nShow] = number_range( ITEM_LIGHT, ITEM_BOOK );
         nShow++;
         --tmp;
      }
      if( obj->wear_loc == WEAR_NONE
          && can_see_obj( ch, obj ) && ( obj->item_type != ITEM_TRAP || IS_AFFECTED( ch, AFF_DETECTTRAPS ) ) )
      {

         if( xIS_SET( ch->act, PLR_COND ) )
         {
            switch ( obj->item_type )
            {
               case ITEM_ARMOR:
               case ITEM_FLUTE:
                  if( obj->value[1] > 0 )
                  {
                     cond = ( int )( ( 100 * obj->value[0] / obj->value[1] ) );
                  }
                  else
                  {
                     cond = 100;
                  }
                  break;
               case ITEM_WEAPON:
                  cond = ( int )( ( 100 * obj->value[0] / 12 ) );
                  break;
               case ITEM_QUIVER:
                  cond = ( int )( ( 100 * obj->value[3] / 12 ) );
                  break;
               case ITEM_MISSILE_WEAPON:
                  cond = ( int )( ( 100 * obj->value[0] / 12 ) );
                  break;
               case ITEM_CONTAINER:
                  cond = ( int )( ( 100 * obj->value[3] / 12 ) );
                  break;
               case ITEM_LIGHT:
                  cond = ( int )( ( 100 * obj->value[0] / 12 ) );
                  break;
               case ITEM_GHOST:
                  cond = ( int )( ( 100 * obj->value[0] / 12 ) );
                  break;
               default:
                  cond = -1;
                  break;
            }
            if( cond > -1 )
            {
               sprintf( buf, "    &C<&R%3d%%&C>&g %s", cond, format_obj_to_char( obj, ch, fShort ) );
            }
            else
            {
               sprintf( buf, "    &C<&RNone&C>&g %s", format_obj_to_char( obj, ch, fShort ) );
            }
         }
         else
         {
            sprintf( buf, "       %s", format_obj_to_char( obj, ch, fShort ) );
         }
         pstrShow = buf;
         fCombine = FALSE;

         if( IS_NPC( ch ) || xIS_SET( ch->act, PLR_COMBINE ) )
         {
            for( iShow = nShow - 1; iShow >= 0; iShow-- )
            {
               if( !strcmp( prgpstrShow[iShow], pstrShow ) )
               {
                  prgnShow[iShow] += obj->count;
                  fCombine = TRUE;
                  break;
               }
            }
         }

         pitShow[nShow] = obj->item_type;
         if( !fCombine )
         {
            prgpstrShow[nShow] = str_dup( pstrShow );
            prgnShow[nShow] = obj->count;
            nShow++;
         }
      }
   }
   if( tmp > 0 )
   {
      int x;
      for( x = 0; x < tmp; x++ )
      {
         prgnShow[nShow] = 1;
         pitShow[nShow] = number_range( ITEM_LIGHT, ITEM_BOOK );
         nShow++;
      }
   }

   for( iShow = 0; iShow < nShow; iShow++ )
   {
      switch ( pitShow[iShow] )
      {
         default:
            set_char_color( AT_OBJECT, ch );
            break;
         case ITEM_BLOOD:
            set_char_color( AT_BLOOD, ch );
            break;
         case ITEM_SILVER:
            set_char_color( AT_GREY, ch );
            break;
         case ITEM_GOLD:
         case ITEM_TREASURE:
            set_char_color( AT_YELLOW, ch );
            break;
         case ITEM_COOK:
         case ITEM_FOOD:
            set_char_color( AT_HUNGRY, ch );
            break;
         case ITEM_DRINK_CON:
         case ITEM_FOUNTAIN:
            set_char_color( AT_THIRSTY, ch );
            break;
         case ITEM_COPPER:
         case ITEM_FIRE:
            set_char_color( AT_FIRE, ch );
            break;
         case ITEM_SCROLL:
         case ITEM_WAND:
         case ITEM_STAFF:
            set_char_color( AT_MAGIC, ch );
            break;
      }
      if( fShowNothing )
         send_to_char( "", ch );
      send_to_char( prgpstrShow[iShow], ch );
      {
         if( prgnShow[iShow] != 1 )
            ch_printf( ch, " (%d)", prgnShow[iShow] );
      }

      send_to_char( "\n\r", ch );
      DISPOSE( prgpstrShow[iShow] );
   }

   if( fShowNothing && nShow == 0 )
   {
      if( IS_NPC( ch ) || xIS_SET( ch->act, PLR_COMBINE ) )
         send_to_char( "     ", ch );
      set_char_color( AT_OBJECT, ch );
      send_to_char( "Nothing.\n\r", ch );
   }

   DISPOSE( prgpstrShow );
   DISPOSE( prgnShow );
   DISPOSE( pitShow );
   return;
}

void show_list_to_char_type( OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing, int itemtype )
{
   char **prgpstrShow;
   int *prgnShow;
   int *pitShow;
   char *pstrShow;
   OBJ_DATA *obj;
   int nShow;
   int iShow;
   int type = 0;
   int count, offcount, tmp, cnt;
   bool fCombine;

   if( !ch->desc )
      return;

   if( itemtype > 0 )
      type = 1;
   if( !list )
   {
      if( fShowNothing )
      {
         if( IS_NPC( ch ) || xIS_SET( ch->act, PLR_COMBINE ) )
            send_to_char( "     ", ch );
         set_char_color( AT_OBJECT, ch );
         send_to_char( "Nothing.\n\r", ch );
      }
      return;
   }
   count = 0;
   for( obj = list; obj; obj = obj->next_content )
      count++;

   offcount = 0;

   if( count + offcount <= 0 )
   {
      if( fShowNothing )
      {
         if( IS_NPC( ch ) || xIS_SET( ch->act, PLR_COMBINE ) )
            send_to_char( "     ", ch );
         set_char_color( AT_OBJECT, ch );
         send_to_char( "Nothing.\n\r", ch );
      }
      return;
   }

   CREATE( prgpstrShow, char *, count + ( ( offcount > 0 ) ? offcount : 0 ) );
   CREATE( prgnShow, int, count + ( ( offcount > 0 ) ? offcount : 0 ) );
   CREATE( pitShow, int, count + ( ( offcount > 0 ) ? offcount : 0 ) );
   nShow = 0;
   tmp = ( offcount > 0 ) ? offcount : 0;
   cnt = 0;

   for( obj = list; obj; obj = obj->next_content )
   {
      if( offcount < 0 && ++cnt > ( count + offcount ) )
         break;
      if( type == 1 )
      {
         if( obj->item_type != itemtype )
            continue;
      }
      if( obj->item_type == ITEM_KEYITEM )
         continue;

      if( tmp > 0 && number_bits( 1 ) == 0 )
      {
         prgnShow[nShow] = 1;
         pitShow[nShow] = number_range( ITEM_LIGHT, ITEM_BOOK );
         nShow++;
         --tmp;
      }
      if( obj->wear_loc == WEAR_NONE
          && can_see_obj( ch, obj ) && ( obj->item_type != ITEM_TRAP || IS_AFFECTED( ch, AFF_DETECTTRAPS ) ) )
      {
         pstrShow = format_obj_to_char( obj, ch, fShort );
         fCombine = FALSE;

         if( IS_NPC( ch ) || xIS_SET( ch->act, PLR_COMBINE ) )
         {
            for( iShow = nShow - 1; iShow >= 0; iShow-- )
            {
               if( !strcmp( prgpstrShow[iShow], pstrShow ) )
               {
                  prgnShow[iShow] += obj->count;
                  fCombine = TRUE;
                  break;
               }
            }
         }

         pitShow[nShow] = obj->item_type;
         if( !fCombine )
         {
            prgpstrShow[nShow] = str_dup( pstrShow );
            prgnShow[nShow] = obj->count;
            nShow++;
         }
      }
   }
   if( tmp > 0 )
   {
      int x;
      for( x = 0; x < tmp; x++ )
      {
         prgnShow[nShow] = 1;
         pitShow[nShow] = number_range( ITEM_LIGHT, ITEM_BOOK );
         nShow++;
      }
   }

   for( iShow = 0; iShow < nShow; iShow++ )
   {
      switch ( pitShow[iShow] )
      {
         default:
            set_char_color( AT_OBJECT, ch );
            break;
         case ITEM_BLOOD:
            set_char_color( AT_BLOOD, ch );
            break;
         case ITEM_SILVER:
            set_char_color( AT_GREY, ch );
            break;
         case ITEM_GOLD:
         case ITEM_TREASURE:
            set_char_color( AT_YELLOW, ch );
            break;
         case ITEM_COOK:
         case ITEM_FOOD:
            set_char_color( AT_HUNGRY, ch );
            break;
         case ITEM_DRINK_CON:
         case ITEM_FOUNTAIN:
            set_char_color( AT_THIRSTY, ch );
            break;
         case ITEM_COPPER:
            set_char_color( AT_ORANGE, ch );
            break;
         case ITEM_FIRE:
            set_char_color( AT_FIRE, ch );
            break;
         case ITEM_SCROLL:
         case ITEM_WAND:
         case ITEM_STAFF:
            set_char_color( AT_MAGIC, ch );
            break;
      }
      if( fShowNothing )
         send_to_char( "     ", ch );
      send_to_char( prgpstrShow[iShow], ch );
      {
         if( prgnShow[iShow] != 1 )
            ch_printf( ch, " (%d)", prgnShow[iShow] );
      }

      send_to_char( "\n\r", ch );
      DISPOSE( prgpstrShow[iShow] );
   }

   if( fShowNothing && nShow == 0 )
   {
      if( IS_NPC( ch ) || xIS_SET( ch->act, PLR_COMBINE ) )
         send_to_char( "     ", ch );
      set_char_color( AT_OBJECT, ch );
      send_to_char( "Nothing.\n\r", ch );
   }

   DISPOSE( prgpstrShow );
   DISPOSE( prgnShow );
   DISPOSE( pitShow );
   return;
}

void show_list_to_char( OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing )
{
   show_list_to_char_type( list, ch, fShort, fShowNothing, 0 );
   return;
}

void show_visible_affects_to_char( CHAR_DATA * victim, CHAR_DATA * ch )
{
   char buf[MAX_STRING_LENGTH];
   char name[MAX_STRING_LENGTH];

   if( IS_NPC( victim ) )
      strcpy( name, victim->short_descr );
   else
      strcpy( name, victim->name );
   name[0] = toupper( name[0] );

   if( ( IS_AFFECTED( victim, AFF_CHARM ) || IS_AFFECTED( victim, AFF_JMT ) ) )
   {
      set_char_color( AT_MAGIC, ch );
      ch_printf( ch, "%s wanders in a dazed, zombie-like state.\n\r", name );
   }
   if( !IS_NPC( victim ) && !victim->desc && victim->switched && IS_AFFECTED( victim->switched, AFF_POSSESS ) )
   {
      set_char_color( AT_MAGIC, ch );
      strcpy( buf, PERS( victim, ch ) );
      strcat( buf, " appears to be in a deep trance...\n\r" );
   }
}

void show_char_to_char_0( CHAR_DATA * victim, CHAR_DATA * ch )
{
   char buf[MAX_STRING_LENGTH];
   char buf1[MAX_STRING_LENGTH];

   buf[0] = '\0';

   set_char_color( AT_PERSON, ch );
   if( !IS_NPC( victim ) && !victim->desc )
   {
      if( !victim->switched )
         send_to_char_color( "&P[(Link Dead)] ", ch );
      else if( !IS_AFFECTED( victim, AFF_POSSESS ) )
         strcat( buf, "(Switched) " );
   }
   if( IS_NPC( victim ) && IS_AFFECTED( victim, AFF_POSSESS ) && IS_IMMORTAL( ch ) && victim->desc )
   {
      sprintf( buf1, "(%s)", victim->desc->original->name );
      strcat( buf, buf1 );
   }
   if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_AFK ) && !IS_AFFECTED( victim, AFF_SHAPESHIFT ) )
      strcat( buf, "(AFK) " );

   if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_IAW ) && !IS_AFFECTED( victim, AFF_SHAPESHIFT ) )
      strcat( buf, "(IAW) " );

   if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_IDLE ) && !IS_AFFECTED( victim, AFF_SHAPESHIFT ) )
      strcat( buf, "(Idling) " );

   if( ( ( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_WIZINVIS ) )
         || ( IS_NPC( victim ) && xIS_SET( victim->act, ACT_MOBINVIS ) ) && !IS_AFFECTED( victim, AFF_SHAPESHIFT ) ) )
   {
      if( !IS_NPC( victim ) )
         sprintf( buf1, "(Invis %d) ", victim->pcdata->wizinvis );
      else
         sprintf( buf1, "(Mobinvis %d) ", victim->mobinvis );
      strcat( buf, buf1 );
   }


   if( IS_AFFECTED( victim, AFF_PLAGUE ) )
      strcat( buf, "[PLAGUED] " );
   if( IS_NPC( victim ) && xIS_SET( victim->act, ACT_PROTOTYPE ) )
      strcat( buf, "(PROTO) " );
   if( IS_NPC( victim ) && xIS_SET( victim->act, ACT_SHOP ) )
      strcat( buf, "(Shop) " );
   if( IS_NPC( victim ) && ch->mount && ch->mount == victim && ch->in_room == ch->mount->in_room )
      strcat( buf, "(Mount) " );

   if( !IS_NPC( victim ) )
   {
      if( IS_AFFECTED( victim, AFF_SHAPESHIFT ) )
      {
         strcat( buf, victim->morph->morph->short_desc );
         switch ( victim->position )
         {
            case POS_DEAD:
               strcat( buf, " is DEAD!!" );
               break;
            case POS_MORTAL:
               strcat( buf, " is mortally wounded." );
               break;
            case POS_INCAP:
               strcat( buf, " is incapacitated." );
               break;
            case POS_STUNNED:
               strcat( buf, " is lying here stunned." );
               break;
            case POS_SLEEPING:
               if( ch->position == POS_SITTING || ch->position == POS_RESTING )
                  strcat( buf, " is sleeping nearby." );
               else
                  strcat( buf, " is deep in slumber here." );
               break;
            case POS_RESTING:
               if( ch->position == POS_RESTING )
                  strcat( buf, " is sprawled out alongside you." );
               else if( ch->position == POS_MOUNTED )
                  strcat( buf, " is sprawled out at the foot of your mount." );
               else
                  strcat( buf, " is sprawled out here." );
               break;
            case POS_SITTING:
               if( ch->position == POS_SITTING )
                  strcat( buf, " sits here with you." );
               else if( ch->position == POS_RESTING )
                  strcat( buf, " sits nearby as you lie around." );
               else
                  strcat( buf, " sits upright here." );
               break;
            case POS_STANDING:
               if( victim->in_room->sector_type == SECT_UNDERWATER )
                  strcat( buf, " is here in the water." );
               else if( victim->in_room->sector_type == SECT_OCEANFLOOR )
                  strcat( buf, " is standing here in the water." );
               else
                  strcat( buf, " is standing here." );
               break;
            case POS_SHOVE:
               strcat( buf, " is being shoved around." );
               break;
            case POS_DRAG:
               strcat( buf, " is being dragged around." );
               break;
            case POS_MOUNTED:
               strcat( buf, " is here, upon " );
               if( !victim->mount )
                  strcat( buf, "thin air???" );
               else if( victim->mount == ch )
                  strcat( buf, "your back." );
               else if( victim->in_room == victim->mount->in_room )
               {
                  strcat( buf, PERS( victim->mount, ch ) );
                  strcat( buf, "." );
               }
               else
                  strcat( buf, "someone who left??" );
               break;
            case POS_FIGHTING:
            case POS_EVASIVE:
            case POS_DEFENSIVE:
            case POS_AGGRESSIVE:
            case POS_BERSERK:
               strcat( buf, " is here, fighting " );
               if( !victim->fighting )
               {
                  strcat( buf, "thin air???" );

                  if( !victim->mount )
                     victim->position = POS_STANDING;
                  else
                     victim->position = POS_MOUNTED;
               }
               else if( who_fighting( victim ) == ch )
                  strcat( buf, "YOU!" );
               else if( victim->in_room == victim->fighting->who->in_room )
               {
                  strcat( buf, PERS( victim->fighting->who, ch ) );
                  strcat( buf, "." );
               }
               else
                  strcat( buf, "someone who left??" );
               break;
         }
      }
      else
      {
         switch ( victim->level )
         {
            default:
               break;
            case MAX_LEVEL - 0:
               send_to_char_color( "&P(&zHost Admin&P) ", ch );
               break;
            case MAX_LEVEL - 10:
               send_to_char_color( "&P(&zHost Senior&P) ", ch );
               break;
            case MAX_LEVEL - 20:
               send_to_char_color( "&P(&zHost&P) ", ch );
               break;
            case MAX_LEVEL - 30:
               send_to_char_color( "&P(&RHigh Templar&P) ", ch );
               break;
            case MAX_LEVEL - 40:
               send_to_char_color( "&P(&RArch Templar&P) ", ch );
               break;
            case MAX_LEVEL - 50:
               send_to_char_color( "&P(&RTemplar&P) ", ch );
               break;
            case MAX_LEVEL - 60:
               send_to_char_color( "&P(&gElder Wizard&P) ", ch );
               break;
            case MAX_LEVEL - 70:
               send_to_char_color( "&P(&gArch Wizard&P) ", ch );
               break;
            case MAX_LEVEL - 80:
               send_to_char_color( "&P(&gWizard&P) ", ch );
               break;
            case MAX_LEVEL - 90:
               send_to_char_color( "&P(&BGrand Bishop&P) ", ch );
               break;
            case MAX_LEVEL - 100:
               send_to_char_color( "&P(&BArch Bishop&P) ", ch );
               break;
            case MAX_LEVEL - 110:
               send_to_char_color( "&P(&BBishop&P) ", ch );
               break;
            case MAX_LEVEL - 120:
               send_to_char_color( "&P(&CGrand Lord&P) ", ch );
               break;
            case MAX_LEVEL - 130:
               send_to_char_color( "&P(&CLord&P) ", ch );
               break;
            case MAX_LEVEL - 140:
               send_to_char_color( "&P(&GArch Duke&P) ", ch );
               break;
            case MAX_LEVEL - 150:
               send_to_char_color( "&P(&GDuke&P) ", ch );
               break;
            case MAX_LEVEL - 160:
               send_to_char_color( "&P(&wArch Druid&P) ", ch );
               break;
            case MAX_LEVEL - 170:
               send_to_char_color( "&P(&wDruid&P) ", ch );
               break;
            case MAX_LEVEL - 180:
               send_to_char_color( "&P(&YArch Angel&P) ", ch );
               break;
            case MAX_LEVEL - 190:
               send_to_char_color( "&P(&YAngel&P) ", ch );
               break;
            case MAX_LEVEL - 200:
               send_to_char_color( "&P(&WRetired&P) ", ch );
               break;
         }

         if( NOT_AUTHED( victim ) )
            send_to_char_color( "&P(&YNot Authed&P) ", ch );

         send_to_char( "&z[", ch );
         if( IS_AFFECTED( victim, AFF_BERSERK ) )
            send_to_char_color( "&OB", ch );
         if( IS_AFFECTED( victim, AFF_FLOATING ) || IS_AFFECTED( victim, AFF_FLYING ) )
            send_to_char_color( "&BF", ch );
         if( IS_AFFECTED( victim, AFF_DETECT_EVIL )
             && IS_AFFECTED( victim, AFF_DETECT_INVIS )
             && IS_AFFECTED( victim, AFF_DETECT_MAGIC )
             && IS_AFFECTED( victim, AFF_DETECT_HIDDEN )
             && IS_AFFECTED( victim, AFF_INFRARED )
             && IS_AFFECTED( victim, AFF_DETECTTRAPS )
             && IS_AFFECTED( victim, AFF_SCRYING ) && IS_AFFECTED( victim, AFF_TRUESIGHT ) )
            send_to_char_color( "&YHY", ch );
         if( IS_AFFECTED( victim, AFF_STEEL_SKIN ) )
            send_to_char_color( "&BS", ch );
         if( IS_AFFECTED( victim, AFF_FIRESHIELD ) )
            send_to_char_color( "&RF", ch );
         if( IS_AFFECTED( victim, AFF_SHOCKSHIELD ) )
            send_to_char_color( "&YSH", ch );
         if( IS_AFFECTED( victim, AFF_ACIDMIST ) )
            send_to_char_color( "&GA", ch );
         if( IS_AFFECTED( victim, AFF_ICESHIELD ) )
            send_to_char_color( "&CIS", ch );
         if( IS_AFFECTED( victim, AFF_PASS_DOOR ) )
            send_to_char_color( "&cP", ch );
         if( IS_AFFECTED( victim, AFF_FAERIE_FIRE ) )
            send_to_char_color( "&PE", ch );
         if( ( IS_AFFECTED( victim, AFF_INVISIBLE ) || IS_AFFECTED( victim, AFF_IMPINVISIBLE ) ) )
            send_to_char_color( "&z!", ch );
         if( IS_AFFECTED( victim, AFF_SNEAK ) )
            send_to_char_color( "&z#", ch );
         if( ( IS_AFFECTED( victim, AFF_HIDE ) || IS_AFFECTED( victim, AFF_MOVEHIDE ) ) )
            send_to_char_color( "&B@", ch );
         if( IS_AFFECTED( victim, AFF_DEMONFIRE ) )
            send_to_char_color( "&R&&", ch );
         if( IS_AFFECTED( victim, AFF_SANCTUARY ) )
            send_to_char_color( "&W$", ch );
         if( IS_AFFECTED( victim, AFF_HOLYAURA ) || IS_AFFECTED( victim, AFF_KARAAURA ) )
            send_to_char_color( "&G*", ch );
         if( IS_SET( victim->pcdata->flags, PCFLAG_DEADLY ) )
            send_to_char_color( "&RPK", ch );
         send_to_char_color( "&z] ", ch );

         if( IS_SET( ch->pcdata->flags, PCFLAG_POSI ) )
         {
            send_to_char( "&z<&P", ch );
            switch ( victim->position )
            {
               case POS_DEAD:
                  send_to_char( "DEAD!!", ch );
                  break;
               case POS_MORTAL:
                  send_to_char( "wounded", ch );
                  break;
               case POS_INCAP:
                  send_to_char( "incapacitated", ch );
                  break;
               case POS_STUNNED:
                  send_to_char( "stunned", ch );
                  break;
               case POS_SLEEPING:
                  send_to_char( "asleep", ch );
                  break;
               case POS_RESTING:
                  send_to_char( "resting", ch );
                  break;
               case POS_SITTING:
                  send_to_char( "sitting", ch );
                  break;
               case POS_STANDING:
                  if( IS_IMMORTAL( victim ) )
                     send_to_char( "standing", ch );
                  else
                     if( ( victim->in_room->sector_type == SECT_UNDERWATER )
                         && !IS_AFFECTED( victim, AFF_AQUA_BREATH ) && !IS_NPC( victim ) )
                     send_to_char( "drowning", ch );
                  else if( victim->in_room->sector_type == SECT_UNDERWATER )
                     send_to_char( "swimming", ch );
                  else
                     if( ( victim->in_room->sector_type == SECT_OCEANFLOOR )
                         && !IS_AFFECTED( victim, AFF_AQUA_BREATH ) && !IS_NPC( victim ) )
                     send_to_char( "drowning", ch );
                  else if( victim->in_room->sector_type == SECT_OCEANFLOOR )
                     send_to_char( "swimming", ch );
                  else
                     send_to_char( "standing", ch );
                  break;
               case POS_SHOVE:
                  send_to_char( "shoved", ch );
                  break;
               case POS_DRAG:
                  send_to_char( "dragged", ch );
                  break;
               case POS_MOUNTED:
                  send_to_char( "mounted", ch );
                  break;
               case POS_FIGHTING:
               case POS_EVASIVE:
               case POS_DEFENSIVE:
               case POS_AGGRESSIVE:
               case POS_BERSERK:
                  send_to_char( "fighting", ch );
                  break;
            }
            send_to_char( "&z> ", ch );
         }

         set_char_color( AT_PERSON, ch );

         if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_ATTACKER ) )
            strcat( buf, "(ATTACKER) " );
         if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_KILLER ) )
            strcat( buf, "(KILLER) " );
         if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_THIEF ) )
            strcat( buf, "(THIEF) " );
         if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_LITTERBUG ) )
            strcat( buf, "(LITTERBUG) " );
         if( IS_AFFECTED( victim, AFF_PLAGUE ) )
            strcat( buf, "[PLAGUED] " );
         if( IS_NPC( victim ) && xIS_SET( victim->act, ACT_PROTOTYPE ) )
            strcat( buf, "(PROTO) " );
         if( IS_NPC( victim ) && xIS_SET( victim->act, ACT_SHOP ) )
            strcat( buf, "(Shop) " );
         if( IS_NPC( victim ) && ch->mount && ch->mount == victim && ch->in_room == ch->mount->in_room )
            strcat( buf, "(Mount) " );
         if( victim->desc && victim->desc->connected == CON_EDITING )
            strcat( buf, "(Writing) " );
         if( victim->desc && victim->desc->connected == CON_MEETING )
            strcat( buf, "(Meeting) " );
      }
   }

   if( !IS_NPC( victim ) && !IS_AFFECTED( victim, AFF_SHAPESHIFT ) )
   {
      strcat( buf, PERS( victim, ch ) );
      if( !xIS_SET( ch->act, PLR_BRIEF ) )
      {
         strcat( buf, victim->pcdata->title );
      }
   }
   else if( IS_NPC( victim ) && !IS_AFFECTED( victim, AFF_SHAPESHIFT ) )
   {
      if( victim->position == POS_STANDING )
      {
         strcat( buf, "&C" );
         strcat( buf, victim->long_descr );
      }
      else
      {
         strcat( buf, "&C" );
         strcat( buf, victim->short_descr );
      }
   }

   if( IS_NPC( victim ) )
   {
      switch ( victim->position )
      {
         case POS_DEAD:
            strcat( buf, " is DEAD!!" );
            break;
         case POS_MORTAL:
            strcat( buf, " is mortally wounded." );
            break;
         case POS_INCAP:
            strcat( buf, " is incapacitated." );
            break;
         case POS_STUNNED:
            strcat( buf, " is lying here stunned." );
            break;
         case POS_SLEEPING:
            if( ch->position == POS_SITTING || ch->position == POS_RESTING )
               strcat( buf, " is sleeping nearby." );
            else
               strcat( buf, " is deep in slumber here." );
            break;
         case POS_RESTING:
            if( ch->position == POS_RESTING )
               strcat( buf, " is sprawled out alongside you." );
            else if( ch->position == POS_MOUNTED )
               strcat( buf, " is sprawled out at the foot of your mount." );
            else
               strcat( buf, " is sprawled out here." );
            break;
         case POS_SITTING:
            if( ch->position == POS_SITTING )
               strcat( buf, " sits here with you." );
            else if( ch->position == POS_RESTING )
               strcat( buf, " sits nearby as you lie around." );
            else
               strcat( buf, " sits upright here." );
            break;
/*    case POS_STANDING:
	if ( victim->in_room->sector_type == SECT_UNDERWATER )
            strcat( buf, " is here in the water." );
	else
	if ( victim->in_room->sector_type == SECT_OCEANFLOOR )
	    strcat( buf, " is standing here in the water." );
	else
          strcat( buf, " is standing here." );
        break;*/
         case POS_SHOVE:
            strcat( buf, " is being shoved around." );
            break;
         case POS_DRAG:
            strcat( buf, " is being dragged around." );
            break;
         case POS_MOUNTED:
            strcat( buf, " is here, upon " );
            if( !victim->mount )
               strcat( buf, "thin air???" );
            else if( victim->mount == ch )
               strcat( buf, "your back." );
            else if( victim->in_room == victim->mount->in_room )
            {
               strcat( buf, PERS( victim->mount, ch ) );
               strcat( buf, "." );
            }
            else
               strcat( buf, "someone who left??" );
            break;
         case POS_FIGHTING:
         case POS_EVASIVE:
         case POS_DEFENSIVE:
         case POS_AGGRESSIVE:
         case POS_BERSERK:
            strcat( buf, " is here, fighting " );
            if( !victim->fighting )
            {
               strcat( buf, "thin air???" );

               if( !victim->mount )
                  victim->position = POS_STANDING;
               else
                  victim->position = POS_MOUNTED;
            }
            else if( who_fighting( victim ) == ch )
               strcat( buf, "YOU!" );
            else if( victim->in_room == victim->fighting->who->in_room )
            {
               strcat( buf, PERS( victim->fighting->who, ch ) );
               strcat( buf, "." );
            }
            else
               strcat( buf, "someone who left??" );
            break;
      }
      strcat( buf, "&D" );
   }

   if( IS_NPC( victim ) && !IS_AFFECTED( victim, AFF_SHAPESHIFT ) && victim->position != POS_STANDING || !IS_NPC( victim ) )
   {
      strcat( buf, "\n\r" );
   }
   buf[0] = UPPER( buf[0] );
   send_to_char( buf, ch );
   show_visible_affects_to_char( victim, ch );
   return;
}



void show_char_to_char_1( CHAR_DATA * victim, CHAR_DATA * ch )
{
   OBJ_DATA *obj;
   int iWear;
   bool found;

   if( can_see( victim, ch ) && !IS_NPC( ch ) && !xIS_SET( ch->act, PLR_WIZINVIS ) && victim->level >= ch->level )
   {
      act( AT_ACTION, "$n looks at you.", ch, NULL, victim, TO_VICT );
      if( victim != ch )
         act( AT_ACTION, "$n looks at $N.", ch, NULL, victim, TO_NOTVICT );
      else
         act( AT_ACTION, "$n looks at $mself.", ch, NULL, victim, TO_NOTVICT );
   }

   set_char_color( AT_DGREEN, ch );

   if( victim->description[0] != '\0' )
   {
      if( victim->morph != NULL && victim->morph->morph != NULL )
         send_to_char( victim->morph->morph->description, ch );
      else
         send_to_char( victim->description, ch );
   }
   else
   {
      if( victim->morph != NULL && victim->morph->morph != NULL )
         send_to_char( victim->morph->morph->description, ch );
      else if( IS_NPC( victim ) )
         act( AT_PLAIN, "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
      else if( ch != victim )
         act( AT_PLAIN, "$E isn't much to look at...", ch, NULL, victim, TO_CHAR );
      else
         act( AT_PLAIN, "You're not much to look at...", ch, NULL, NULL, TO_CHAR );
   }

   show_race_line( ch, victim );
   show_condition( ch, victim );

   found = FALSE;
   for( iWear = 0; iWear < MAX_WEAR; iWear++ )
   {
      if( ( obj = get_eq_char( victim, iWear ) ) != NULL && can_see_obj( ch, obj ) )
      {
         if( !found )
         {
            send_to_char( "\n\r", ch );
            if( victim != ch )
               ch_printf( ch, "%s is using:\n\r",
                          ( IS_AFFECTED( victim, AFF_SHAPESHIFT ) ? victim->morph->morph->
                            short_desc : PERS( victim, ch ) ) );
            else
               act( AT_PLAIN, "You are using:", ch, NULL, NULL, TO_CHAR );
            found = TRUE;
         }
         send_to_char( where_name[iWear], ch );
         send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
         send_to_char( "\n\r", ch );
      }
   }

   if( IS_NPC( ch ) || victim == ch )
      return;

   if( IS_IMMORTAL( ch ) )
   {
      if( IS_NPC( victim ) )
         ch_printf( ch, "\n\rMobile #%d '%s' ", victim->pIndexData->vnum, victim->name );
      else
         ch_printf( ch, "\n\r%s ", victim->name );
      if( victim->level < 100 )
      {
         if( IS_DUAL( victim ) )
         {
            ch_printf( ch, "is a level %d %s %s %s.\n\r",
                       victim->level,
                       IS_NPC( victim ) ? victim->race < MAX_NPC_RACE && victim->race >= 0 ?
                       npc_race[victim->race] : "unknown" : victim->race < MAX_PC_RACE &&
                       race_table[victim->race]->race_name &&
                       race_table[victim->race]->race_name[0] != '\0' ?
                       race_table[victim->race]->race_name : "unknown",
                       IS_NPC( victim ) ? victim->class < MAX_NPC_CLASS && victim->class >= 0 ?
                       npc_class[victim->class] : "unknown" : victim->class < MAX_PC_CLASS &&
                       class_table[victim->class]->who_name &&
                       class_table[victim->class]->who_name[0] != '\0' ?
                       class_table[victim->class]->who_name : "unknown",
                       victim->dualclass < MAX_PC_CLASS && class_table[victim->dualclass]->who_name &&
                       class_table[victim->dualclass]->who_name[0] != '\0' ?
                       class_table[victim->dualclass]->who_name : "unknown" );
         }
         else
         {
            ch_printf( ch, "is a level %d %s %s.\n\r",
                       victim->level,
                       IS_NPC( victim ) ? victim->race < MAX_NPC_RACE && victim->race >= 0 ?
                       npc_race[victim->race] : "unknown" : victim->race < MAX_PC_RACE &&
                       race_table[victim->race]->race_name &&
                       race_table[victim->race]->race_name[0] != '\0' ?
                       race_table[victim->race]->race_name : "unknown",
                       IS_NPC( victim ) ? victim->class < MAX_NPC_CLASS && victim->class >= 0 ?
                       npc_class[victim->class] : "unknown" : victim->class < MAX_PC_CLASS &&
                       class_table[victim->class]->who_name &&
                       class_table[victim->class]->who_name[0] != '\0' ? class_table[victim->class]->who_name : "unknown" );
         }
      }
      else
      {
         if( IS_DUAL( victim ) )
         {
            ch_printf( ch, "is a level %d(%d) %s %s %s.\n\r",
                       victim->level,
                       HAS_AWARD( victim ),
                       IS_NPC( victim ) ? victim->race < MAX_NPC_RACE && victim->race >= 0 ?
                       npc_race[victim->race] : "unknown" : victim->race < MAX_PC_RACE &&
                       race_table[victim->race]->race_name &&
                       race_table[victim->race]->race_name[0] != '\0' ?
                       race_table[victim->race]->race_name : "unknown",
                       IS_NPC( victim ) ? victim->class < MAX_NPC_CLASS && victim->class >= 0 ?
                       npc_class[victim->class] : "unknown" : victim->class < MAX_PC_CLASS &&
                       class_table[victim->class]->who_name &&
                       class_table[victim->class]->who_name[0] != '\0' ?
                       class_table[victim->class]->who_name : "unknown",
                       victim->dualclass < MAX_PC_CLASS && class_table[victim->dualclass]->who_name &&
                       class_table[victim->dualclass]->who_name[0] != '\0' ?
                       class_table[victim->dualclass]->who_name : "unknown" );
         }
         else
         {
            ch_printf( ch, "is a level %d(%d) %s %s.\n\r",
                       victim->level,
                       HAS_AWARD( victim ),
                       IS_NPC( victim ) ? victim->race < MAX_NPC_RACE && victim->race >= 0 ?
                       npc_race[victim->race] : "unknown" : victim->race < MAX_PC_RACE &&
                       race_table[victim->race]->race_name &&
                       race_table[victim->race]->race_name[0] != '\0' ?
                       race_table[victim->race]->race_name : "unknown",
                       IS_NPC( victim ) ? victim->class < MAX_NPC_CLASS && victim->class >= 0 ?
                       npc_class[victim->class] : "unknown" : victim->class < MAX_PC_CLASS &&
                       class_table[victim->class]->who_name &&
                       class_table[victim->class]->who_name[0] != '\0' ? class_table[victim->class]->who_name : "unknown" );
         }
      }
   }

   if( number_percent(  ) < LEARNED( ch, gsn_peek ) )
   {
      ch_printf( ch, "\n\rYou peek at %s inventory:\n\r", victim->sex == 0 ? "his" : "her" );
      show_list_to_char( victim->first_carrying, ch, TRUE, TRUE );
      learn_from_success( ch, gsn_peek );
   }
   else if( ch->pcdata->learned[gsn_peek] > 0 )
      learn_from_failure( ch, gsn_peek );

   return;
}


void show_char_to_char( CHAR_DATA * list, CHAR_DATA * ch )
{
   CHAR_DATA *rch;

   for( rch = list; rch; rch = rch->next_in_room )
   {
      if( rch == ch )
         continue;

      if( can_see( ch, rch ) )
      {
         show_char_to_char_0( rch, ch );
      }
      else if( room_is_dark( ch->in_room ) && IS_AFFECTED( rch, AFF_INFRARED ) && !( !IS_NPC( rch ) && IS_IMMORTAL( rch ) ) )
      {
         set_char_color( AT_BLOOD, ch );
         send_to_char( "The red form of a living creature is here.\n\r", ch );
      }
   }

   return;
}



bool check_blind( CHAR_DATA * ch )
{
   if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_HOLYLIGHT ) )
      return TRUE;

   if( IS_AFFECTED( ch, AFF_TRUESIGHT ) )
      return TRUE;

   if( IS_AFFECTED( ch, AFF_BLIND ) )
   {
      send_to_char( "You can't see a thing!\n\r", ch );
      return FALSE;
   }

   return TRUE;
}

int get_door( char *arg )
{
   int door;

   if( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) )
      door = 0;
   else if( !str_cmp( arg, "e" ) || !str_cmp( arg, "east" ) )
      door = 1;
   else if( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) )
      door = 2;
   else if( !str_cmp( arg, "w" ) || !str_cmp( arg, "west" ) )
      door = 3;
   else if( !str_cmp( arg, "u" ) || !str_cmp( arg, "up" ) )
      door = 4;
   else if( !str_cmp( arg, "d" ) || !str_cmp( arg, "down" ) )
      door = 5;
   else if( !str_cmp( arg, "ne" ) || !str_cmp( arg, "northeast" ) )
      door = 6;
   else if( !str_cmp( arg, "nw" ) || !str_cmp( arg, "northwest" ) )
      door = 7;
   else if( !str_cmp( arg, "se" ) || !str_cmp( arg, "southeast" ) )
      door = 8;
   else if( !str_cmp( arg, "sw" ) || !str_cmp( arg, "southwest" ) )
      door = 9;
   else
      door = -1;
   return door;
}

void do_land( CHAR_DATA * ch, char *argument )
{
   AFFECT_DATA *paf;
   AFFECT_DATA *paf_next;
   bool found = FALSE;

   for( paf = ch->first_affect; paf; paf = paf_next )
   {
      paf_next = paf->next;
      if( xIS_SET( paf->bitvector, AFF_FLYING ) || xIS_SET( paf->bitvector, AFF_FLOATING ) )
      {
         affect_remove( ch, paf );
         found = TRUE;
      }
   }
   xREMOVE_BIT( ch->affected_by, AFF_FLYING );
   xREMOVE_BIT( ch->affected_by, AFF_FLOATING );
   if( found )
   {
      act( AT_PLAIN, "You land.", ch, NULL, NULL, TO_CHAR );
      act( AT_PLAIN, "$n lands.", ch, NULL, NULL, TO_ROOM );
   }
   send_to_char( "Ok.\n\r", ch );
   return;
}

void do_affstrip( CHAR_DATA * ch, char *argument )
{
   AFFECT_DATA *paf;
   AFFECT_DATA *paf_next;
   CHAR_DATA *victim;
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   /*
    * bool found=FALSE;
    */

   if( ch->position != 12 )
   {
      send_to_char( "You must be standing to do this.\n\r", ch );
      return;
   }

   if( !IS_IMMORTAL( ch ) )
   {
      send_to_char( "You can't do this.\n\r", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      send_to_char( "Affstrip who?\n\rSyntax: affstrip <who>\n\r", ch );
      return;
   }

   strcpy( buf, "0." );
   strcat( buf, argument );
   victim = get_char_world( ch, buf );

   if( !victim )
   {
      sprintf( buf2, "%s is not logged on now.\n\r", capitalize( argument ) );
      send_to_char( buf2, ch );
      return;
   }

   do_remove( victim, "all" );
   send_to_char( "Your gear has been removed.\n\r", victim );
   act( AT_PLAIN, "$n gear has been removed.", victim, NULL, NULL, TO_ROOM );

   for( paf = victim->first_affect; paf; paf = paf_next )
   {
      paf_next = paf->next;
      affect_remove( victim, paf );
   }

   send_to_char( "Your have been cleansed.\n\r", victim );
   act( AT_PLAIN, "$n has been cleansed.", victim, NULL, NULL, TO_ROOM );
   update_aris( victim );
   return;
}

void do_look( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   char buf2[MAX_INPUT_LENGTH];
   char pname[MAX_STRING_LENGTH];
   char dir_n[50];
   char dir_e[50];
   char dir_s[50];
   char dir_w[50];
   char dir_u[50];
   char dir_d[50];
   char dir_ne[50];
   char dir_nw[50];
   char dir_se[50];
   char dir_sw[50];
   char dir_sm[50];
   char *exitcolor;
   EXIT_DATA *pexit;
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   ROOM_INDEX_DATA *original;
   char *pdesc;
   bool doexaprog;
   sh_int door;
   int number, cnt;

   strcpy( dir_n, "&z-" );
   strcpy( dir_e, "&z-" );
   strcpy( dir_s, "&z-" );
   strcpy( dir_w, "&z-" );
   strcpy( dir_u, "&z-" );
   strcpy( dir_d, "&z-" );
   strcpy( dir_ne, "&z -" );
   strcpy( dir_nw, "&z- " );
   strcpy( dir_se, "&z -" );
   strcpy( dir_sw, "&z- " );
   strcpy( dir_sm, "&z-" );

   if( !ch->desc )
      return;

   if( ch->position < POS_SLEEPING )
   {
      send_to_char( "You can't see anything but stars!\n\r", ch );
      return;
   }

   if( ch->position == POS_SLEEPING )
   {
      send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
      return;
   }

   if( !check_blind( ch ) )
      return;

   if( !IS_NPC( ch )
       && !xIS_SET( ch->act, PLR_HOLYLIGHT ) && !IS_AFFECTED( ch, AFF_TRUESIGHT ) && room_is_dark( ch->in_room ) )
   {
      set_char_color( AT_DGREY, ch );
      send_to_char( "It is pitch black ... \n\r", ch );
      show_char_to_char( ch->in_room->first_person, ch );
      return;
   }

   switch ( ch->in_room->area->plane )
   {
      default:
         break;
      case 1:
         sprintf( pname, "Terra" );
         break;
      case 2:
         sprintf( pname, "White" );
         break;
      case 3:
         sprintf( pname, "Lorur" );
         break;
      case 4:
         sprintf( pname, "Ophelia" );
         break;
      case 5:
         sprintf( pname, "Utopia" );
         break;
      case 6:
         sprintf( pname, "All" );
         break;
      case 7:
         sprintf( pname, "Angel" );
         break;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   doexaprog = str_cmp( "noprog", arg2 ) && str_cmp( "noprog", arg3 );

   if( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
   {

      sprintf( buf, "&Y[&zPlane - %s&Y] &R(&zArea - %s&R)&D", pname, ch->in_room->area->name );


      for( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
      {
         if( ( pexit->to_room
               && !IS_SET( pexit->exit_info, EX_HIDDEN )
               && !IS_SET( pexit->exit_info, EX_SECRET ) )
             || ( IS_SET( pexit->exit_info, EX_SECRET ) && !IS_SET( pexit->exit_info, EX_CLOSED ) ) )
         {
            exitcolor = "&Y";
            if( IS_SET( pexit->exit_info, EX_WINDOW ) )
               exitcolor = "&C";
            if( IS_SET( pexit->exit_info, EX_SECRET ) )
               exitcolor = "&b";
            if( IS_SET( pexit->exit_info, EX_CLOSED ) )
               exitcolor = "&z";
            if( IS_SET( pexit->exit_info, EX_LOCKED ) )
               exitcolor = "&R";

            if( pexit->vdir == DIR_NORTH )
               sprintf( dir_n, "%sN", exitcolor );

            if( pexit->vdir == DIR_EAST )
               sprintf( dir_e, "%sE", exitcolor );

            if( pexit->vdir == DIR_SOUTH )
               sprintf( dir_s, "%sS", exitcolor );

            if( pexit->vdir == DIR_WEST )
               sprintf( dir_w, "%sW", exitcolor );

            if( pexit->vdir == DIR_UP )
               sprintf( dir_u, "%sU", exitcolor );

            if( pexit->vdir == DIR_DOWN )
               sprintf( dir_d, "%sD", exitcolor );

            if( pexit->vdir == DIR_NORTHEAST )
               sprintf( dir_ne, "%sNE", exitcolor );

            if( pexit->vdir == DIR_NORTHWEST )
               sprintf( dir_nw, "%sNW", exitcolor );

            if( pexit->vdir == DIR_SOUTHEAST )
               sprintf( dir_se, "%sSE", exitcolor );

            if( pexit->vdir == DIR_SOUTHWEST )
               sprintf( dir_sw, "%sSW", exitcolor );
         }
      }

      if( WATER_SECT( ch ) )
      {
         sprintf( buf2, "&B(WATER)&w %-42.42s", ch->in_room->name );
      }
      else if( LAVA_SECT( ch ) )
      {
         sprintf( buf2, "&R(LAVA)&w %-43.43s", ch->in_room->name );
      }
      else
      {
         sprintf( buf2, "&w%-50.50s", ch->in_room->name );
      }
      ch_printf( ch, buf2 );
      strcat( dir_nw, "  " );
      ch_printf_color( ch, "         %s", dir_nw );
      strcat( dir_n, "  " );
      ch_printf_color( ch, "  %s", dir_n );
      ch_printf_color( ch, "  %s\n\r", dir_ne );
      send_to_char( "&z-<----------------------------------------------->-     ", ch );
      strcat( dir_w, "" );
      ch_printf_color( ch, "   %s", dir_w );
      send_to_char( "&z<-", ch );
      strcat( dir_u, "&z-(&Y&W*&z)" );
      ch_printf_color( ch, "%s", dir_u );
      strcat( dir_d, "&z-" );
      send_to_char( "&z-", ch );
      ch_printf_color( ch, "%s", dir_d );
      send_to_char( "&z>", ch );
      ch_printf_color( ch, "%s\n\r", dir_e );
      ch_printf( ch, "%-66.66s", buf );
      strcat( dir_sw, "  " );
      ch_printf_color( ch, "\t   %s", dir_sw );
      strcat( dir_s, "  " );
      ch_printf_color( ch, "  %s", dir_s );
      ch_printf_color( ch, "  %s", dir_se );
      send_to_char( "\n\r", ch );

      if( IS_AGOD( ch ) )
      {
         ch_printf( ch, "&z[&wRoom Value Number: &Y%d&z]&D\n\r", ch->in_room->vnum );
      }

      if( IS_IMMORTAL( ch ) && IS_SET( ch->pcdata->flags, PCFLAG_AUTOFLAGS ) )
      {
         set_char_color( AT_IMMORT, ch );
         ch_printf( ch, "[Room Flags: %s]\n\r", ext_flag_string( &ch->in_room->room_flags, r_flags ) );

         set_char_color( AT_IMMORT, ch );
         ch_printf( ch, "[Area Flags: %s]  ", flag_string( ch->in_room->area->flags, area_flags ) );
         if( !IS_SET( ch->pcdata->flags, PCFLAG_SECTORD ) && !IS_SET( ch->pcdata->flags, PCFLAG_ANAME ) )
         {
            send_to_char( "\n\r", ch );
         }
      }
      if( IS_IMMORTAL( ch ) && IS_SET( ch->pcdata->flags, PCFLAG_SECTORD ) )
      {
         set_char_color( AT_IMMORT, ch );
         ch_printf( ch, "[Sector Type: %s]  ", sec_flags[ch->in_room->sector_type] );
         if( ( IS_SET( ch->pcdata->flags, PCFLAG_AUTOFLAGS )
               && !IS_SET( ch->pcdata->flags, PCFLAG_ANAME ) )
             || ( !IS_SET( ch->pcdata->flags, PCFLAG_AUTOFLAGS ) && !IS_SET( ch->pcdata->flags, PCFLAG_ANAME ) ) )
         {
            send_to_char( "\n\r", ch );
         }
      }

      if( IS_IMMORTAL( ch ) && IS_SET( ch->pcdata->flags, PCFLAG_ANAME ) )
      {
         set_char_color( AT_IMMORT, ch );
         ch_printf( ch, "[Area filename: %s]\n\r", ch->in_room->area->filename );
      }

      set_char_color( AT_DGREEN, ch );

      if( arg1[0] == '\0' || ( !IS_NPC( ch ) && !xIS_SET( ch->act, PLR_BRIEF ) ) )
         send_to_char( ch->in_room->description, ch );

      if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_AUTOMAP ) )
      {
         if( ch->in_room->map != NULL )
         {
            do_lookmap( ch, NULL );
         }
      }

      if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_AUTOEXIT ) )
      {
         do_exits( ch, "auto" );
      }
      else
      {
         send_to_char( "&W-<----------------------------------------------->-&D", ch );
         send_to_char( "\n\r", ch );
      }

      if( IS_TRACKING( ch ) && ch->hunting && !IS_NPC( ch ) )
      {
         do_ttrack( ch, ch->hunting->name );
      }

      show_list_to_char( ch->in_room->first_content, ch, FALSE, FALSE );
      show_char_to_char( ch->in_room->first_person, ch );
      return;
   }

   if( !str_cmp( arg1, "sky" ) )
   {
      if( !IS_OUTSIDE( ch ) )
      {
         send_to_char( "You can't see the sky indoors.\n\r", ch );
         return;
      }
      else
      {
         look_sky( ch );
         return;
      }
   }

   if( !str_cmp( arg1, "under" ) )
   {
      int count;

      if( arg2[0] == '\0' )
      {
         send_to_char( "Look beneath what?\n\r", ch );
         return;
      }

      if( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
      {
         send_to_char( "You do not see that here.\n\r", ch );
         return;
      }
      if( !CAN_WEAR( obj, ITEM_TAKE ) && ch->level < sysdata.level_getobjnotake )
      {
         send_to_char( "You can't seem to get a grip on it.\n\r", ch );
         return;
      }
      if( ch->carry_weight + obj->weight > can_carry_w( ch ) )
      {
         send_to_char( "It's too heavy for you to look under.\n\r", ch );
         return;
      }
      count = obj->count;
      obj->count = 1;
      act( AT_PLAIN, "You lift $p and look beneath it:", ch, obj, NULL, TO_CHAR );
      act( AT_PLAIN, "$n lifts $p and looks beneath it:", ch, obj, NULL, TO_ROOM );
      obj->count = count;
      if( IS_OBJ_STAT( obj, ITEM_COVERING ) )
         show_list_to_char( obj->first_content, ch, TRUE, TRUE );
      else
         send_to_char( "Nothing.\n\r", ch );
      if( doexaprog )
         oprog_examine_trigger( ch, obj );
      return;
   }

   if( !str_cmp( arg1, "i" ) || !str_cmp( arg1, "in" ) )
   {
      int count;

      if( arg2[0] == '\0' )
      {
         send_to_char( "Look in what?\n\r", ch );
         return;
      }

      if( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
      {
         send_to_char( "You do not see that here.\n\r", ch );
         return;
      }

      switch ( obj->item_type )
      {
         default:
            send_to_char( "That is not a container.\n\r", ch );
            break;

         case ITEM_DRINK_CON:
            if( obj->value[1] <= 0 )
            {
               send_to_char( "It is empty.\n\r", ch );
               if( doexaprog )
                  oprog_examine_trigger( ch, obj );
               break;
            }

            ch_printf( ch, "It's %s full of a %s liquid.\n\r",
                       obj->value[1] < obj->value[0] / 4
                       ? "less than" :
                       obj->value[1] < 3 * obj->value[0] / 4 ? "about" : "more than", liq_table[obj->value[2]].liq_color );

            if( doexaprog )
               oprog_examine_trigger( ch, obj );
            break;

         case ITEM_PORTAL:
            for( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
            {
               if( pexit->vdir == DIR_PORTAL && IS_SET( pexit->exit_info, EX_PORTAL ) )
               {
                  if( room_is_private( pexit->to_room ) && get_trust( ch ) < sysdata.level_override_private )
                  {
                     set_char_color( AT_WHITE, ch );
                     send_to_char( "That room is private buster!\n\r", ch );
                     return;
                  }
                  original = ch->in_room;
                  char_from_room( ch );
                  char_to_room( ch, pexit->to_room );
                  do_look( ch, "auto" );
                  char_from_room( ch );
                  char_to_room( ch, original );
                  return;
               }
            }
            send_to_char( "You see swirling chaos...\n\r", ch );
            break;
         case ITEM_CONTAINER:
         case ITEM_QUIVER:
         case ITEM_CORPSE_NPC:
         case ITEM_CORPSE_PC:
            if( IS_SET( obj->value[1], CONT_CLOSED ) )
            {
               send_to_char( "It is closed.\n\r", ch );
               break;
            }

         case ITEM_KEYRING:
            count = obj->count;
            obj->count = 1;
            if( obj->item_type == ITEM_CONTAINER )
               act( AT_PLAIN, "$p contains:", ch, obj, NULL, TO_CHAR );
            else
               act( AT_PLAIN, "$p holds:", ch, obj, NULL, TO_CHAR );
            obj->count = count;
            show_list_to_char_type( obj->first_content, ch, TRUE, TRUE, atoi( argument ) );
            if( doexaprog )
               oprog_examine_trigger( ch, obj );
            break;
      }
      return;
   }

   if( ( pdesc = get_extra_descr( arg1, ch->in_room->first_extradesc ) ) != NULL )
   {
      send_to_char_color( pdesc, ch );
      return;
   }

   door = get_door( arg1 );
   if( ( pexit = find_door( ch, arg1, TRUE ) ) != NULL )
   {
      if( IS_SET( pexit->exit_info, EX_CLOSED ) && !IS_SET( pexit->exit_info, EX_WINDOW ) )
      {
         if( ( IS_SET( pexit->exit_info, EX_SECRET ) || IS_SET( pexit->exit_info, EX_DIG ) ) && door != -1 )
            send_to_char( "Nothing special there.\n\r", ch );
         else
            act( AT_PLAIN, "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
         return;
      }
      if( IS_SET( pexit->exit_info, EX_BASHED ) )
         act( AT_RED, "The $d has been bashed from its hinges!", ch, NULL, pexit->keyword, TO_CHAR );

      if( pexit->description && pexit->description[0] != '\0' )
         send_to_char( pexit->description, ch );
      else
         send_to_char( "Nothing special there.\n\r", ch );

      if( pexit->to_room
          && ( IS_AFFECTED( ch, AFF_SCRYING )
               || ch->class == CLASS_THIEF || IS_SET( pexit->exit_info, EX_xLOOK ) || get_trust( ch ) >= LEVEL_IMMORTAL ) )
      {
         if( !IS_SET( pexit->exit_info, EX_xLOOK ) && get_trust( ch ) < LEVEL_IMMORTAL )
         {
            set_char_color( AT_MAGIC, ch );
            send_to_char( "You attempt to scry...\n\r", ch );
            if( !IS_NPC( ch ) )
            {
               int percent = LEARNED( ch, skill_lookup( "scry" ) );
               if( !percent )
               {
                  if( ch->class == CLASS_THIEF )
                     percent = 95;
                  else
                     percent = 55;
               }

               if( number_percent(  ) > percent )
               {
                  send_to_char( "You fail.\n\r", ch );
                  return;
               }
            }
         }
         if( room_is_private( pexit->to_room ) && get_trust( ch ) < sysdata.level_override_private )
         {
            set_char_color( AT_WHITE, ch );
            send_to_char( "That room is private buster!\n\r", ch );
            return;
         }
         original = ch->in_room;
         char_from_room( ch );
         char_to_room( ch, pexit->to_room );
         do_look( ch, "auto" );
         char_from_room( ch );
         char_to_room( ch, original );
      }
      return;
   }
   else if( door != -1 )
   {
      send_to_char( "Nothing special there.\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg1 ) ) != NULL )
   {
      if( arg2[0] == '\0' )
      {
         show_char_to_char_1( victim, ch );
      }
      else if( !str_cmp( arg2, "keyitems" ) || !str_cmp( arg2, "ki" ) )
      {
         bool found;

         set_char_color( AT_RED, ch );
         ch_printf( ch, "%s currently has the following key items.\n\r", PERS( victim, ch ) );
         found = FALSE;
         set_char_color( AT_OBJECT, ch );
         for( obj = ch->first_carrying; obj; obj = obj->next_content )
         {
            if( obj->item_type == ITEM_KEYITEM )
            {
               send_to_char( "     ", ch );
               send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
               send_to_char( "\n\r", ch );
               found = TRUE;
            }
         }
         if( !found )
         {
            send_to_char( "     &zNothing&D\n\r", ch );
         }
      }
      else
      {
         show_char_to_char_1( victim, ch );
      }
      return;
   }


   number = number_argument( arg1, arg );
   for( cnt = 0, obj = ch->last_carrying; obj; obj = obj->prev_content )
   {
      if( can_see_obj( ch, obj ) )
      {
         if( ( pdesc = get_extra_descr( arg, obj->first_extradesc ) ) != NULL )
         {
            if( ( cnt += obj->count ) < number )
               continue;
            send_to_char_color( pdesc, ch );
            if( doexaprog )
               oprog_examine_trigger( ch, obj );
            return;
         }

         if( ( pdesc = get_extra_descr( arg, obj->pIndexData->first_extradesc ) ) != NULL )
         {
            if( ( cnt += obj->count ) < number )
               continue;
            send_to_char_color( pdesc, ch );
            if( doexaprog )
               oprog_examine_trigger( ch, obj );
            return;
         }
         if( nifty_is_name_prefix( arg, obj->name ) )
         {
            if( ( cnt += obj->count ) < number )
               continue;
            pdesc = get_extra_descr( obj->name, obj->pIndexData->first_extradesc );
            if( !pdesc )
               pdesc = get_extra_descr( obj->name, obj->first_extradesc );
            if( !pdesc )
               send_to_char_color( "You see nothing special.\r\n", ch );
            else
               send_to_char_color( pdesc, ch );
            if( doexaprog )
               oprog_examine_trigger( ch, obj );
            return;
         }
      }
   }

   for( obj = ch->in_room->last_content; obj; obj = obj->prev_content )
   {
      if( can_see_obj( ch, obj ) )
      {
         if( obj->item_type == ITEM_WINDOW )
         {
            look_window( ch, obj );
            return;
         }
         if( ( pdesc = get_extra_descr( arg, obj->first_extradesc ) ) != NULL )
         {
            if( ( cnt += obj->count ) < number )
               continue;
            send_to_char_color( pdesc, ch );
            if( doexaprog )
               oprog_examine_trigger( ch, obj );
            return;
         }

         if( ( pdesc = get_extra_descr( arg, obj->pIndexData->first_extradesc ) ) != NULL )
         {
            if( ( cnt += obj->count ) < number )
               continue;
            send_to_char_color( pdesc, ch );
            if( doexaprog )
               oprog_examine_trigger( ch, obj );
            return;
         }
         if( nifty_is_name_prefix( arg, obj->name ) )
         {
            if( ( cnt += obj->count ) < number )
               continue;
            pdesc = get_extra_descr( obj->name, obj->pIndexData->first_extradesc );
            if( !pdesc )
               pdesc = get_extra_descr( obj->name, obj->first_extradesc );
            if( !pdesc )
               send_to_char( "You see nothing special.\r\n", ch );
            else
               send_to_char_color( pdesc, ch );
            if( doexaprog )
               oprog_examine_trigger( ch, obj );
            return;
         }
      }
   }

   send_to_char( "You do not see that here.\n\r", ch );
   return;
}

void show_race_line( CHAR_DATA * ch, CHAR_DATA * victim )
{
   char buf[MAX_STRING_LENGTH];
   int feet, inches;


   if( !IS_NPC( victim ) && ( victim != ch ) )
   {
      feet = IS_AFFECTED( victim, AFF_SHAPESHIFT ) ? victim->morph->morph->height / 12 : victim->height / 12;
      inches = IS_AFFECTED( victim, AFF_SHAPESHIFT ) ? victim->morph->morph->height % 12 : victim->height % 12;
      sprintf( buf, "%s is %d'%d\" and weighs %d pounds.\n\r",
               ( IS_AFFECTED( victim, AFF_SHAPESHIFT ) ? victim->morph->morph->short_desc : PERS( victim, ch ) ), feet,
               inches, ( IS_AFFECTED( victim, AFF_SHAPESHIFT ) ? victim->morph->morph->weight : victim->weight ) );
      send_to_char( buf, ch );
      return;
   }
   if( !IS_NPC( victim ) && ( victim == ch ) )
   {
      feet = IS_AFFECTED( victim, AFF_SHAPESHIFT ) ? victim->morph->morph->height / 12 : victim->height / 12;
      inches = IS_AFFECTED( victim, AFF_SHAPESHIFT ) ? victim->morph->morph->height % 12 : victim->height % 12;
      sprintf( buf, "You are %d'%d\" and weigh %d pounds.\n\r", feet, inches,
               ( IS_AFFECTED( victim, AFF_SHAPESHIFT ) ? victim->morph->morph->weight : victim->weight ) );
      send_to_char( buf, ch );
      return;
   }

}


void show_condition( CHAR_DATA * ch, CHAR_DATA * victim )
{
   char buf[MAX_STRING_LENGTH];
   int percent;

   if( victim->max_hit > 0 )
      percent = ( 100 * victim->hit ) / victim->max_hit;
   else
      percent = -1;


   if( victim != ch )
   {
      strcpy( buf, ( IS_AFFECTED( victim, AFF_SHAPESHIFT ) ? victim->morph->morph->short_desc : PERS( victim, ch ) ) );
      if( percent >= 100 )
         strcat( buf, " is in perfect health.\n\r" );
      else if( percent >= 90 )
         strcat( buf, " is slightly scratched.\n\r" );
      else if( percent >= 80 )
         strcat( buf, " has a few bruises.\n\r" );
      else if( percent >= 70 )
         strcat( buf, " has some cuts.\n\r" );
      else if( percent >= 60 )
         strcat( buf, " has several wounds.\n\r" );
      else if( percent >= 50 )
         strcat( buf, " has many nasty wounds.\n\r" );
      else if( percent >= 40 )
         strcat( buf, " is bleeding freely.\n\r" );
      else if( percent >= 30 )
         strcat( buf, " is covered in blood.\n\r" );
      else if( percent >= 20 )
         strcat( buf, " is leaking guts.\n\r" );
      else if( percent >= 10 )
         strcat( buf, " is almost dead.\n\r" );
      else
         strcat( buf, " is DYING.\n\r" );
   }
   else
   {
      strcpy( buf, "You" );
      if( percent >= 100 )
         strcat( buf, " are in perfect health.\n\r" );
      else if( percent >= 90 )
         strcat( buf, " are slightly scratched.\n\r" );
      else if( percent >= 80 )
         strcat( buf, " have a few bruises.\n\r" );
      else if( percent >= 70 )
         strcat( buf, " have some cuts.\n\r" );
      else if( percent >= 60 )
         strcat( buf, " have several wounds.\n\r" );
      else if( percent >= 50 )
         strcat( buf, " have many nasty wounds.\n\r" );
      else if( percent >= 40 )
         strcat( buf, " are bleeding freely.\n\r" );
      else if( percent >= 30 )
         strcat( buf, " are covered in blood.\n\r" );
      else if( percent >= 20 )
         strcat( buf, " are leaking guts.\n\r" );
      else if( percent >= 10 )
         strcat( buf, " are almost dead.\n\r" );
      else
         strcat( buf, " are DYING.\n\r" );
   }

   buf[0] = UPPER( buf[0] );
   send_to_char( buf, ch );
   return;
}

void do_glance( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   bool brief;

   if( !ch->desc )
      return;

   if( ch->position < POS_SLEEPING )
   {
      send_to_char( "You can't see anything but stars!\n\r", ch );
      return;
   }

   if( ch->position == POS_SLEEPING )
   {
      send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
      return;
   }

   if( !check_blind( ch ) )
      return;

   set_char_color( AT_ACTION, ch );
   argument = one_argument( argument, arg1 );

   if( arg1[0] == '\0' )
   {
      if( xIS_SET( ch->act, PLR_BRIEF ) )
         brief = TRUE;
      else
         brief = FALSE;
      xSET_BIT( ch->act, PLR_BRIEF );
      do_look( ch, "auto" );
      if( !brief )
         xREMOVE_BIT( ch->act, PLR_BRIEF );
      return;
   }

   if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
      send_to_char( "They're not here.\n\r", ch );
      return;
   }
   else
   {
      if( can_see( victim, ch ) )
      {
         act( AT_ACTION, "$n glances at you.", ch, NULL, victim, TO_VICT );
         act( AT_ACTION, "$n glances at $N.", ch, NULL, victim, TO_NOTVICT );
      }
      if( IS_IMMORTAL( ch ) && victim != ch )
      {
         if( IS_NPC( victim ) )
            ch_printf( ch, "Mobile #%d '%s' ", victim->pIndexData->vnum, victim->name );
         else
            ch_printf( ch, "%s ", victim->name );
         ch_printf( ch, "is a level %d %s %s.\n\r",
                    victim->level,
                    IS_NPC( victim ) ? victim->race < MAX_NPC_RACE && victim->race >= 0 ?
                    npc_race[victim->race] : "unknown" : victim->race < MAX_PC_RACE &&
                    race_table[victim->race]->race_name &&
                    race_table[victim->race]->race_name[0] != '\0' ?
                    race_table[victim->race]->race_name : "unknown",
                    IS_NPC( victim ) ? victim->class < MAX_NPC_CLASS && victim->class >= 0 ?
                    npc_class[victim->class] : "unknown" : victim->class < MAX_PC_CLASS &&
                    class_table[victim->class]->who_name &&
                    class_table[victim->class]->who_name[0] != '\0' ? class_table[victim->class]->who_name : "unknown" );
      }
      show_condition( ch, victim );

      return;
   }

   return;
}


void do_examine( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   BOARD_DATA *board;
   sh_int dam;
   sh_int value;

   if( !argument )
   {
      bug( "do_examine: null argument.", 0 );
      return;
   }

   if( !ch )
   {
      bug( "do_examine: null ch.", 0 );
      return;
   }

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "Examine what?\n\r", ch );
      return;
   }

   sprintf( buf, "%s noprog", arg );
   do_look( ch, buf );

   if( ( obj = get_obj_here( ch, arg ) ) != NULL )
   {
      if( ( board = get_board( obj ) ) != NULL )
      {
         if( board->num_posts )
            ch_printf( ch, "There are about %d notes posted here.  Type 'note list' to list them.\n\r", board->num_posts );
         else
            send_to_char( "There aren't any notes posted here.\n\r", ch );
      }

      switch ( obj->item_type )
      {
         default:
            break;

         case ITEM_ARMOR:
            if( LEARNED( ch, gsn_armor_lore ) )
            {
               if( obj->value[1] == 0 )
                  obj->value[1] = obj->value[0];
               if( obj->value[1] == 0 )
                  obj->value[1] = 1;
               dam = ( sh_int ) ( ( obj->value[0] * 10 ) / obj->value[1] );
               strcpy( buf, "As you look more closely, you see that it is " );
               if( dam >= 10 )
                  strcat( buf, "in superb condition." );
               else if( dam == 9 )
                  strcat( buf, "in very good condition." );
               else if( dam == 8 )
                  strcat( buf, "in good shape." );
               else if( dam == 7 )
                  strcat( buf, "showing a bit of wear." );
               else if( dam == 6 )
                  strcat( buf, "a little run down." );
               else if( dam == 5 )
                  strcat( buf, "in need of repair." );
               else if( dam == 4 )
                  strcat( buf, "in great need of repair." );
               else if( dam == 3 )
                  strcat( buf, "in dire need of repair." );
               else if( dam == 2 )
                  strcat( buf, "very badly worn." );
               else if( dam == 1 )
                  strcat( buf, "practically worthless." );
               else if( dam <= 0 )
                  strcat( buf, "broken." );
               strcat( buf, "\n\r" );
            }
            else
            {
               strcpy( buf, "You don't know enough armor lore.\n\r" );
            }
            send_to_char( buf, ch );
            break;

         case ITEM_WEAPON:
            if( LEARNED( ch, gsn_weapon_lore ) )
            {
               dam = INIT_WEAPON_CONDITION - obj->value[0];
               strcpy( buf, "As you look more closely, you see that it is " );
               if( dam == 0 )
                  strcat( buf, "in superb condition." );
               else if( dam == 1 )
                  strcat( buf, "in excellent condition." );
               else if( dam == 2 )
                  strcat( buf, "in very good condition." );
               else if( dam == 3 )
                  strcat( buf, "in good shape." );
               else if( dam == 4 )
                  strcat( buf, "showing a bit of wear." );
               else if( dam == 5 )
                  strcat( buf, "a little run down." );
               else if( dam == 6 )
                  strcat( buf, "in need of repair." );
               else if( dam == 7 )
                  strcat( buf, "in great need of repair." );
               else if( dam == 8 )
                  strcat( buf, "in dire need of repair." );
               else if( dam == 9 )
                  strcat( buf, "very badly worn." );
               else if( dam == 10 )
                  strcat( buf, "practically worthless." );
               else if( dam == 11 )
                  strcat( buf, "almost broken." );
               else if( dam == 12 )
                  strcat( buf, "broken." );
               strcat( buf, "\n\r" );
            }
            else
            {
               strcpy( buf, "You don't know enough weapon lore.\n\r" );
            }
            send_to_char( buf, ch );
            break;

         case ITEM_COOK:
            strcpy( buf, "As you examine it carefully you notice that it " );
            dam = obj->value[2];
            if( dam >= 3 )
               strcat( buf, "is burned to a crisp." );
            else if( dam == 1 )
               strcat( buf, "is a little over cooked." );
            else if( dam == 1 )
               strcat( buf, "is perfectly roasted." );
            else
               strcat( buf, "is raw." );
            strcat( buf, "\n\r" );
            send_to_char( buf, ch );
         case ITEM_FOOD:
            if( obj->timer > 0 && obj->value[1] > 0 )
               dam = ( obj->timer * 10 ) / obj->value[1];
            else
               dam = 10;
            if( obj->item_type == ITEM_FOOD )
               strcpy( buf, "As you examine it carefully you notice that it " );
            else
               strcpy( buf, "Also it " );
            if( dam >= 10 )
               strcat( buf, "is fresh." );
            else if( dam == 9 )
               strcat( buf, "is nearly fresh." );
            else if( dam == 8 )
               strcat( buf, "is perfectly fine." );
            else if( dam == 7 )
               strcat( buf, "looks good." );
            else if( dam == 6 )
               strcat( buf, "looks ok." );
            else if( dam == 5 )
               strcat( buf, "is a little stale." );
            else if( dam == 4 )
               strcat( buf, "is a bit stale." );
            else if( dam == 3 )
               strcat( buf, "smells slightly off." );
            else if( dam == 2 )
               strcat( buf, "smells quite rank." );
            else if( dam == 1 )
               strcat( buf, "smells revolting!" );
            else if( dam <= 0 )
               strcat( buf, "is crawling with maggots!" );
            strcat( buf, "\n\r" );
            send_to_char( buf, ch );
            break;


         case ITEM_SWITCH:
         case ITEM_LEVER:
         case ITEM_PULLCHAIN:
            if( IS_SET( obj->value[0], TRIG_UP ) )
               send_to_char( "You notice that it is in the up position.\n\r", ch );
            else
               send_to_char( "You notice that it is in the down position.\n\r", ch );
            break;
         case ITEM_BUTTON:
            if( IS_SET( obj->value[0], TRIG_UP ) )
               send_to_char( "You notice that it is depressed.\n\r", ch );
            else
               send_to_char( "You notice that it is not depressed.\n\r", ch );
            break;


         case ITEM_CORPSE_PC:
         case ITEM_CORPSE_NPC:
         {
            sh_int timerfrac = obj->timer;
            if( obj->item_type == ITEM_CORPSE_PC )
               timerfrac = ( int )obj->timer / 8 + 1;

            switch ( timerfrac )
            {
               default:
                  send_to_char( "This corpse has recently been slain.\n\r", ch );
                  break;
               case 4:
                  send_to_char( "This corpse was slain a little while ago.\n\r", ch );
                  break;
               case 3:
                  send_to_char( "A foul smell rises from the corpse, and it is covered in flies.\n\r", ch );
                  break;
               case 2:
                  send_to_char( "A writhing mass of maggots and decay, you can barely go near this corpse.\n\r", ch );
                  break;
               case 1:
               case 0:
                  send_to_char( "Little more than bones, there isn't much left of this corpse.\n\r", ch );
                  break;
            }
         }
         case ITEM_CONTAINER:
            if( IS_OBJ_STAT( obj, ITEM_COVERING ) )
               break;
         case ITEM_DRINK_CON:
         case ITEM_QUIVER:
            send_to_char( "When you look inside, you see:\n\r", ch );
         case ITEM_KEYRING:
            value = get_otype( argument );
            if( value > 0 )
               sprintf( buf, "in %s noprog %d", arg, value );
            else
               sprintf( buf, "in %s noprog", arg );
            do_look( ch, buf );
            break;
      }
      if( IS_OBJ_STAT( obj, ITEM_COVERING ) )
      {
         sprintf( buf, "under %s noprog", arg );
         do_look( ch, buf );
      }
      oprog_examine_trigger( ch, obj );
      if( char_died( ch ) || obj_extracted( obj ) )
         return;

      check_for_trap( ch, obj, TRAP_EXAMINE );
   }
   return;
}

void do_exits( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   EXIT_DATA *pexit;
   bool found;
   bool fAuto;

   set_char_color( AT_EXITS, ch );
   buf[0] = '\0';
   fAuto = !str_cmp( argument, "auto" );

   if( !check_blind( ch ) )
      return;

   strcpy( buf, fAuto ? "&W-<" : "Obvious exits:\n\r" );

   found = FALSE;
   for( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
   {
      if( pexit->to_room
          && ( !IS_SET( pexit->exit_info, EX_WINDOW )
               || IS_SET( pexit->exit_info, EX_ISDOOR ) ) && !IS_SET( pexit->exit_info, EX_HIDDEN ) )
      {
         found = TRUE;
         if( fAuto )
         {
            if( !IS_SET( pexit->exit_info, EX_ISDOOR )
                && !IS_SET( pexit->exit_info, EX_CLOSED )
                && !IS_SET( pexit->exit_info, EX_LOCKED ) && !IS_SET( pexit->exit_info, EX_SECRET ) )
            {
               if( pexit->vdir == DIR_NORTH )
                  strcat( buf, " &YNorth" );

               if( pexit->vdir == DIR_EAST )
                  strcat( buf, " &YEast" );

               if( pexit->vdir == DIR_SOUTH )
                  strcat( buf, " &YSouth" );

               if( pexit->vdir == DIR_WEST )
                  strcat( buf, " &YWest" );

               if( pexit->vdir == DIR_UP )
                  strcat( buf, " &YUp" );

               if( pexit->vdir == DIR_DOWN )
                  strcat( buf, " &YDown" );

               if( pexit->vdir == DIR_NORTHEAST )
                  strcat( buf, " &YNorthEast" );

               if( pexit->vdir == DIR_NORTHWEST )
                  strcat( buf, " &YNorthWest" );

               if( pexit->vdir == DIR_SOUTHEAST )
                  strcat( buf, " &YSouthEast" );

               if( pexit->vdir == DIR_SOUTHWEST )
                  strcat( buf, " &YSouthWest" );
            }
            if( IS_SET( pexit->exit_info, EX_ISDOOR )
                && !IS_SET( pexit->exit_info, EX_CLOSED )
                && !IS_SET( pexit->exit_info, EX_LOCKED ) && !IS_SET( pexit->exit_info, EX_SECRET ) )
            {
               if( pexit->vdir == DIR_NORTH )
                  strcat( buf, " &Y[N]orth" );

               if( pexit->vdir == DIR_EAST )
                  strcat( buf, " &Y[E]ast" );

               if( pexit->vdir == DIR_SOUTH )
                  strcat( buf, " &Y[S]outh" );

               if( pexit->vdir == DIR_WEST )
                  strcat( buf, " &Y[W]est" );

               if( pexit->vdir == DIR_UP )
                  strcat( buf, " &Y[U]p" );

               if( pexit->vdir == DIR_DOWN )
                  strcat( buf, " &Y[D]own" );

               if( pexit->vdir == DIR_NORTHEAST )
                  strcat( buf, " &Y[N]orth[E]ast" );

               if( pexit->vdir == DIR_NORTHWEST )
                  strcat( buf, " &Y[N]orth[W]est" );

               if( pexit->vdir == DIR_SOUTHEAST )
                  strcat( buf, " &Y[S]outh[E]ast" );

               if( pexit->vdir == DIR_SOUTHWEST )
                  strcat( buf, " &Y[S]outh[W]est" );
            }
            if( IS_SET( pexit->exit_info, EX_ISDOOR )
                && IS_SET( pexit->exit_info, EX_CLOSED )
                && !IS_SET( pexit->exit_info, EX_LOCKED ) && !IS_SET( pexit->exit_info, EX_SECRET ) )
            {
               if( pexit->vdir == DIR_NORTH )
                  strcat( buf, " &z[N]orth" );

               if( pexit->vdir == DIR_EAST )
                  strcat( buf, " &z[E]ast" );

               if( pexit->vdir == DIR_SOUTH )
                  strcat( buf, " &z[S]outh" );

               if( pexit->vdir == DIR_WEST )
                  strcat( buf, " &z[W]est" );

               if( pexit->vdir == DIR_UP )
                  strcat( buf, " &z[U]p" );

               if( pexit->vdir == DIR_DOWN )
                  strcat( buf, " &z[D]own" );

               if( pexit->vdir == DIR_NORTHEAST )
                  strcat( buf, " &z[N]orth[E]ast" );

               if( pexit->vdir == DIR_NORTHWEST )
                  strcat( buf, " &z[N]orth[W]est" );

               if( pexit->vdir == DIR_SOUTHEAST )
                  strcat( buf, " &z[S]outh[E]ast" );

               if( pexit->vdir == DIR_SOUTHWEST )
                  strcat( buf, " &z[S]outh[W]est" );
            }
            if( IS_SET( pexit->exit_info, EX_ISDOOR )
                && IS_SET( pexit->exit_info, EX_CLOSED )
                && IS_SET( pexit->exit_info, EX_LOCKED ) && !IS_SET( pexit->exit_info, EX_SECRET ) )
            {
               if( pexit->vdir == DIR_NORTH )
                  strcat( buf, " &R[N]orth" );

               if( pexit->vdir == DIR_EAST )
                  strcat( buf, " &R[E]ast" );

               if( pexit->vdir == DIR_SOUTH )
                  strcat( buf, " &R[S]outh" );

               if( pexit->vdir == DIR_WEST )
                  strcat( buf, " &R[W]est" );

               if( pexit->vdir == DIR_UP )
                  strcat( buf, " &R[U]p" );

               if( pexit->vdir == DIR_DOWN )
                  strcat( buf, " &R[D]own" );

               if( pexit->vdir == DIR_NORTHEAST )
                  strcat( buf, " &R[N]orth[E]ast" );

               if( pexit->vdir == DIR_NORTHWEST )
                  strcat( buf, " &R[N]orth[W]est" );

               if( pexit->vdir == DIR_SOUTHEAST )
                  strcat( buf, " &R[S]outh[E]ast" );

               if( pexit->vdir == DIR_SOUTHWEST )
                  strcat( buf, " &R[S]outh[W]est" );
            }
         }
         else
         {
            if( IS_SET( pexit->exit_info, EX_ISDOOR )
                && IS_SET( pexit->exit_info, EX_CLOSED ) && !IS_SET( pexit->exit_info, EX_SECRET ) )
            {
               sprintf( buf + strlen( buf ), "#%-5s - %s\n\r",
                        capitalize( dir_name[pexit->vdir] ),
                        room_is_dark( pexit->to_room ) ? "Too dark to tell" : pexit->to_room->name );
            }
            else
            {
               if( !IS_SET( pexit->exit_info, EX_CLOSED ) )
                  sprintf( buf + strlen( buf ), " %-5s - %s\n\r",
                           capitalize( dir_name[pexit->vdir] ),
                           room_is_dark( pexit->to_room ) ? "Too dark to tell" : pexit->to_room->name );
            }
         }
      }
   }

   if( !found )
      strcat( buf, fAuto ? " none >-&D\n\r" : "None.\n\r" );
   else if( fAuto )
      strcat( buf, " &W>-&D\n\r" );
   send_to_char( buf, ch );
   return;
}

char *const day_name[] = {
   "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
   "the Great Gods", "the Sun"
};

char *const month_name[] = {
   "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
   "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
   "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
   "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time( CHAR_DATA * ch, char *argument )
{
   extern char str_boot_time[];
   extern char reboot_time[];
   char *suf;
   int day;

   day = time_info.day + 1;

   if( day > 4 && day < 20 )
      suf = "th";
   else if( day % 10 == 1 )
      suf = "st";
   else if( day % 10 == 2 )
      suf = "nd";
   else if( day % 10 == 3 )
      suf = "rd";
   else
      suf = "th";

   set_char_color( AT_YELLOW, ch );
   ch_printf( ch,
              "It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\r"
              "&Y        The mud started up at : &w%s\r"
              "&Y     The system time (C.S.T.) : &w%s\r"
              "&YThe system save time (C.S.T.) : &w%s\r",
              ( time_info.hour % 12 == 0 ) ? 12 : time_info.hour % 12,
              time_info.hour >= 12 ? "pm" : "am",
              day_name[day % 7],
              day, suf, month_name[time_info.month], str_boot_time, ( char * )ctime( &current_time ), sysdata.salltime );

   if( sysdata.manualb == 0 )
   {
      ch_printf( ch, "&Y       Next Reboot is set for : &w%s\r", reboot_time );
   }
   else
   {
      ch_printf( ch, "&Y       Next Reboot is set for : &wAuto Reboot Off\n\r" );
   }

   return;
}

void do_weather( CHAR_DATA * ch, char *argument )
{
   char *combo, *single;
   char buf[MAX_INPUT_LENGTH];
   int temp, precip, wind;

   if( !IS_OUTSIDE( ch ) )
   {
      ch_printf( ch, "You can't see the sky from here.\n\r" );
      return;
   }

   temp = ( ch->in_room->area->weather->temp + 3 * weath_unit - 1 ) / weath_unit;
   precip = ( ch->in_room->area->weather->precip + 3 * weath_unit - 1 ) / weath_unit;
   wind = ( ch->in_room->area->weather->wind + 3 * weath_unit - 1 ) / weath_unit;

   if( precip >= 3 )
   {
      combo = preciptemp_msg[precip][temp];
      single = wind_msg[wind];
   }
   else
   {
      combo = windtemp_msg[wind][temp];
      single = precip_msg[precip];
   }

   sprintf( buf, "%s and %s.\n\r", combo, single );

   set_char_color( AT_BLUE, ch );

   ch_printf( ch, buf );
}

HELP_DATA *get_help( CHAR_DATA * ch, char *argument )
{
   char argall[MAX_INPUT_LENGTH];
   char argone[MAX_INPUT_LENGTH];
   char argnew[MAX_INPUT_LENGTH];
   HELP_DATA *pHelp;
   int lev;

   if( argument[0] == '\0' )
      argument = "summary";

   if( isdigit( argument[0] ) )
   {
      lev = number_argument( argument, argnew );
      argument = argnew;
   }
   else
      lev = -2;
   argall[0] = '\0';
   while( argument[0] != '\0' )
   {
      argument = one_argument( argument, argone );
      if( argall[0] != '\0' )
         strcat( argall, " " );
      strcat( argall, argone );
   }

   for( pHelp = first_help; pHelp; pHelp = pHelp->next )
   {
      if( pHelp->level > get_trust( ch ) )
         continue;
      if( lev != -2 && pHelp->level != lev )
         continue;

      if( is_name( argall, pHelp->keyword ) )
         return pHelp;
   }

   return NULL;
}

void do_laws( CHAR_DATA * ch, char *argument )
{
   char buf[1024];

   if( argument == NULL )
      do_help( ch, "laws" );
   else
   {
      sprintf( buf, "\'law %s\'", argument );
      do_help( ch, buf );
   }
}

sh_int str_similarity( const char *astr, const char *bstr )
{
   sh_int matches = 0;

   if( !astr || !bstr )
      return matches;

   for( ; *astr; astr++ )
   {
      if( LOWER( *astr ) == LOWER( *bstr ) )
         matches++;

      if( ++bstr == '\0' )
         return matches;
   }

   return matches;
}

sh_int str_prefix_level( const char *astr, const char *bstr )
{
   sh_int matches = 0;

   if( !astr || !bstr )
      return matches;

   for( ; *astr; astr++ )
   {
      if( LOWER( *astr ) == LOWER( *bstr ) )
         matches++;
      else
         return matches;

      if( ++bstr == '\0' )
         return matches;
   }

   return matches;
}

void similar_help_files( CHAR_DATA * ch, char *argument )
{
   HELP_DATA *pHelp = NULL;
   char buf[MAX_STRING_LENGTH];
   char *extension;
   sh_int lvl = 0;
   bool single = FALSE;


   send_to_pager_color( "&C&BSimilar Help Files:\n\r", ch );

   for( pHelp = first_help; pHelp; pHelp = pHelp->next )
   {
      buf[0] = '\0';
      extension = pHelp->keyword;

      if( pHelp->level > get_trust( ch ) )
         continue;

      while( extension[0] != '\0' )
      {
         extension = one_argument( extension, buf );

         if( str_similarity( argument, buf ) > lvl )
         {
            lvl = str_similarity( argument, buf );
            single = TRUE;
         }
         else if( str_similarity( argument, buf ) == lvl && lvl > 0 )
         {
            single = FALSE;
         }
      }
   }

   if( lvl == 0 )
   {
      send_to_pager_color( "&C&GNo similar help files.\n\r", ch );
      return;
   }

   for( pHelp = first_help; pHelp; pHelp = pHelp->next )
   {
      buf[0] = '\0';
      extension = pHelp->keyword;

      while( extension[0] != '\0' )
      {
         extension = one_argument( extension, buf );

         if( str_similarity( argument, buf ) >= lvl && pHelp->level <= get_trust( ch ) )
         {
            if( single )
            {
               send_to_pager_color( "&C&GOpening only similar helpfile.&C\n\r", ch );
               do_help( ch, buf );
               return;
            }

            pager_printf_color( ch, "&C&G   %s\n\r", pHelp->keyword );
            break;

         }

      }
   }
   return;
}

void do_help( CHAR_DATA * ch, char *argument )
{
   HELP_DATA *pHelp;
//    char arg[MAX_INPUT_LENGTH];

   if( is_number( argument ) )
   {
      send_to_char( "You must use words to see the help files.\n\r", ch );
      return;
   }

// argument = one_argument( argument, arg );
   if( argument[0] == '\0' )
   {
      sprintf( argument, "summary" );
   }

   if( ( pHelp = get_help( ch, argument ) ) == NULL )
   {
      pager_printf_color( ch, "&C&wNo help on \'%s\' found.\n\r", argument );
      similar_help_files( ch, argument );
      return;
   }

   if( !IS_NPC( ch ) && !str_cmp( argument, "start" ) )
      SET_BIT( ch->pcdata->flags, PCFLAG_HELPSTART );

   if( pHelp->level >= 0 && str_cmp( argument, "imotd" ) && pHelp->text[0] != '.' )
   {
      send_to_pager( pHelp->keyword, ch );
      send_to_pager( "\n\r", ch );
   }

   if( pHelp->text[0] == '.' )
      send_to_pager_color( pHelp->text + 1, ch );
   else
      send_to_pager_color( pHelp->text, ch );
   return;
}


extern char *help_greeting;

void do_hedit( CHAR_DATA * ch, char *argument )
{
   HELP_DATA *pHelp;

   if( !ch->desc )
   {
      send_to_char( "You have no descriptor.\n\r", ch );
      return;
   }
   if( ch->substate == SUB_REPEATCMD )
   {
      send_to_char( "Please type 'done' before continueing.\n\r", ch );
      return;
   }

   switch ( ch->substate )
   {
      default:
         break;
      case SUB_HELP_EDIT:
         if( ( pHelp = ch->dest_buf ) == NULL )
         {
            bug( "hedit: sub_help_edit: NULL ch->dest_buf", 0 );
            stop_editing( ch );
            return;
         }
         if( help_greeting == pHelp->text )
            help_greeting = NULL;
         STRFREE( pHelp->text );
         pHelp->text = copy_buffer( ch );
         if( !help_greeting )
            help_greeting = pHelp->text;
         stop_editing( ch );
         return;
   }
   if( ( pHelp = get_help( ch, argument ) ) == NULL )
   {
      HELP_DATA *tHelp;
      char argnew[MAX_INPUT_LENGTH];
      int lev;
      bool new_help = TRUE;

      for( tHelp = first_help; tHelp; tHelp = tHelp->next )
         if( !str_cmp( argument, tHelp->keyword ) )
         {
            pHelp = tHelp;
            new_help = FALSE;
            break;
         }
      if( new_help )
      {
         if( isdigit( argument[0] ) )
         {
            lev = number_argument( argument, argnew );
            argument = argnew;
         }
         else
            lev = get_trust( ch );
         CREATE( pHelp, HELP_DATA, 1 );
         pHelp->keyword = STRALLOC( strupper( argument ) );
         pHelp->text = STRALLOC( "" );
         pHelp->level = lev;
         add_help( pHelp );
      }
   }

   ch->substate = SUB_HELP_EDIT;
   ch->dest_buf = pHelp;
   start_editing( ch, pHelp->text );
}

char *help_fix( char *text )
{
   char *fixed;

   if( !text )
      return "";
   fixed = strip_cr( text );
   if( fixed[0] == ' ' )
      fixed[0] = '.';
   return fixed;
}

/*
 * Help File Stat!             -Kianen
 */
void do_hstat( CHAR_DATA * ch, char *argument )
{
   HELP_DATA *pHelp;

   smash_tilde( argument );
   if( argument[0] == '\0' )
   {
      send_to_char( "Syntax: hstat <help file>\n\r", ch );
      return;
   }

   if( ( pHelp = get_help( ch, argument ) ) == NULL )
   {
      send_to_char( "Cannot find help on that subject.\n\r", ch );
      return;
   }

   ch_printf( ch, "&WKeywords: &w%s\n\r", pHelp->keyword );
   ch_printf( ch, "&WLevel:    &w%d\n\r", pHelp->level );
   ch_printf( ch, "&w%s\n\r", pHelp->text );
   return;

}

void do_hset( CHAR_DATA * ch, char *argument )
{
   HELP_DATA *pHelp;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];

   smash_tilde( argument );
   argument = one_argument( argument, arg1 );
   if( arg1[0] == '\0' )
   {
      send_to_char( "Syntax: hset <field> [value] [help page]\n\r", ch );
      send_to_char( "\n\r", ch );
      send_to_char( "Field being one of:\n\r", ch );
      send_to_char( "  level keyword remove save\n\r", ch );
      return;
   }

   if( !str_cmp( arg1, "save" ) )
   {
      FILE *fpout;

      log_string_plus( "Saving help.are...", LOG_NORMAL, LEVEL_BUILD );

      RENAME( "help.are", "help.are.bak" );
      fclose( fpReserve );
      if( ( fpout = fopen( "help.are", "w" ) ) == NULL )
      {
         bug( "hset save: fopen", 0 );
         perror( "help.are" );
         fpReserve = fopen( NULL_FILE, "r" );
         return;
      }

      fprintf( fpout, "#HELPS\n\n" );
      for( pHelp = first_help; pHelp; pHelp = pHelp->next )
         fprintf( fpout, "%d %s~\n%s~\n\n", pHelp->level, pHelp->keyword, help_fix( pHelp->text ) );

      fprintf( fpout, "0 $~\n\n\n#$\n" );
      fclose( fpout );
      fpReserve = fopen( NULL_FILE, "r" );
      send_to_char( "Saved.\n\r", ch );
      return;
   }
   if( str_cmp( arg1, "remove" ) )
      argument = one_argument( argument, arg2 );

   if( ( pHelp = get_help( ch, argument ) ) == NULL )
   {
      send_to_char( "Cannot find help on that subject.\n\r", ch );
      return;
   }
   if( !str_cmp( arg1, "remove" ) )
   {
      UNLINK( pHelp, first_help, last_help, next, prev );
      STRFREE( pHelp->text );
      STRFREE( pHelp->keyword );
      DISPOSE( pHelp );
      send_to_char( "Removed.\n\r", ch );
      return;
   }
   if( !str_cmp( arg1, "level" ) )
   {
      pHelp->level = atoi( arg2 );
      send_to_char( "Done.\n\r", ch );
      return;
   }
   if( !str_cmp( arg1, "keyword" ) )
   {
      STRFREE( pHelp->keyword );
      pHelp->keyword = STRALLOC( strupper( arg2 ) );
      send_to_char( "Done.\n\r", ch );
      return;
   }

   do_hset( ch, "" );
}

void do_hl( CHAR_DATA * ch, char *argument )
{
   send_to_char( "If you want to use HELPLIST, spell it out.\n\r", ch );
   return;
}

void do_helplist( CHAR_DATA * ch, char *argument )
{
   int min, max, minlimit, maxlimit, cnt;
   char arg[MAX_INPUT_LENGTH];
   HELP_DATA *help;
   bool minfound, maxfound;
   char *idx;

   maxlimit = get_trust( ch );
   minlimit = maxlimit >= LEVEL_BUILD ? -1 : 0;

   min = minlimit;
   max = maxlimit;

   idx = NULL;
   minfound = FALSE;
   maxfound = FALSE;

   for( argument = one_argument( argument, arg ); arg[0] != '\0'; argument = one_argument( argument, arg ) )
   {
      if( !isdigit( arg[0] ) )
      {
         if( idx )
         {
            set_char_color( AT_GREEN, ch );
            ch_printf( ch, "You may only use a single keyword to index the list.\n\r" );
            return;
         }
         idx = STRALLOC( arg );
      }
      else
      {
         if( !minfound )
         {
            min = URANGE( minlimit, atoi( arg ), maxlimit );
            minfound = TRUE;
         }
         else if( !maxfound )
         {
            max = URANGE( minlimit, atoi( arg ), maxlimit );
            maxfound = TRUE;
         }
         else
         {
            set_char_color( AT_GREEN, ch );
            ch_printf( ch, "You may only use two level limits.\n\r" );
            return;
         }
      }
   }

   if( min > max )
   {
      int temp = min;

      min = max;
      max = temp;
   }

   set_pager_color( AT_GREEN, ch );
   pager_printf( ch, "Help Topics in level range %d to %d:\n\r\n\r", min, max );
   for( cnt = 0, help = first_help; help; help = help->next )
      if( help->level >= min && help->level <= max && ( !idx || nifty_is_name_prefix( idx, help->keyword ) ) )
      {
         pager_printf( ch, "  %3d %s\n\r", help->level, help->keyword );
         ++cnt;
      }
   if( cnt )
      pager_printf( ch, "\n\r%d pages found.\n\r", cnt );
   else
      send_to_char( "None found.\n\r", ch );

   if( idx )
      STRFREE( idx );

   return;
}


struct whogr_s
{
   struct whogr_s *next;
   struct whogr_s *follower;
   struct whogr_s *l_follow;
   DESCRIPTOR_DATA *d;
   int indent;
}
 *first_whogr, *last_whogr;

struct whogr_s *find_whogr( DESCRIPTOR_DATA * d, struct whogr_s *first )
{
   struct whogr_s *whogr, *whogr_t;

   for( whogr = first; whogr; whogr = whogr->next )
      if( whogr->d == d )
         return whogr;
      else if( whogr->follower && ( whogr_t = find_whogr( d, whogr->follower ) ) )
         return whogr_t;
   return NULL;
}

void indent_whogr( CHAR_DATA * looker, struct whogr_s *whogr, int ilev )
{
   for( ; whogr; whogr = whogr->next )
   {
      if( whogr->follower )
      {
         int nlev = ilev;
         CHAR_DATA *wch = ( whogr->d->original ? whogr->d->original : whogr->d->character );

         if( can_see( looker, wch ) && !IS_IMMORTAL( wch ) )
            nlev += 3;
         indent_whogr( looker, whogr->follower, nlev );
      }
      whogr->indent = ilev;
   }
}

void create_whogr( CHAR_DATA * looker )
{
   DESCRIPTOR_DATA *d;
   CHAR_DATA *wch;
   struct whogr_s *whogr, *whogr_t;
   int dc = 0, wc = 0;

   while( ( whogr = first_whogr ) != NULL )
   {
      first_whogr = whogr->next;
      DISPOSE( whogr );
   }
   first_whogr = last_whogr = NULL;
   for( d = last_descriptor; d; d = d->prev )
   {
      if( d->connected != CON_PLAYING && d->connected != CON_EDITING && d->connected != CON_MEETING )
         continue;
      ++dc;
      wch = ( d->original ? d->original : d->character );
      if( !wch->leader || wch->leader == wch || !wch->leader->desc ||
          IS_NPC( wch->leader ) || IS_IMMORTAL( wch ) || IS_IMMORTAL( wch->leader ) )
      {
         CREATE( whogr, struct whogr_s, 1 );
         if( !last_whogr )
            first_whogr = last_whogr = whogr;
         else
         {
            last_whogr->next = whogr;
            last_whogr = whogr;
         }
         whogr->next = NULL;
         whogr->follower = whogr->l_follow = NULL;
         whogr->d = d;
         whogr->indent = 0;
         ++wc;
      }
   }
   while( wc < dc )
      for( d = last_descriptor; d; d = d->prev )
      {
         if( d->connected != CON_PLAYING && d->connected != CON_EDITING && d->connected != CON_MEETING )
            continue;
         if( find_whogr( d, first_whogr ) )
            continue;
         wch = ( d->original ? d->original : d->character );
         if( wch->leader && wch->leader != wch && wch->leader->desc &&
             !IS_NPC( wch->leader ) && !IS_IMMORTAL( wch ) &&
             !IS_IMMORTAL( wch->leader ) && ( whogr_t = find_whogr( wch->leader->desc, first_whogr ) ) )
         {
            CREATE( whogr, struct whogr_s, 1 );
            if( !whogr_t->l_follow )
               whogr_t->follower = whogr_t->l_follow = whogr;
            else
            {
               whogr_t->l_follow->next = whogr;
               whogr_t->l_follow = whogr;
            }
            whogr->next = NULL;
            whogr->follower = whogr->l_follow = NULL;
            whogr->d = d;
            whogr->indent = 0;
            ++wc;
         }
      }
   indent_whogr( looker, first_whogr, 0 );

   for( whogr_t = NULL, whogr = first_whogr; whogr; )
      if( whogr->l_follow )
      {
         whogr->l_follow->next = whogr;
         whogr->l_follow = NULL;
         if( whogr_t )
            whogr_t->next = whogr = whogr->follower;
         else
            first_whogr = whogr = whogr->follower;
      }
      else
      {
         whogr_t = whogr;
         whogr = whogr->next;
      }
}

void do_who( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
//    char buf2[MAX_STRING_LENGTH];
   char clan_name[MAX_INPUT_LENGTH];
   char council_name[MAX_INPUT_LENGTH];
   char invis_str[MAX_INPUT_LENGTH];
   char char_name[MAX_INPUT_LENGTH];
   char *extra_title;
   char class_text[MAX_INPUT_LENGTH];
   struct whogr_s *whogr, *whogr_p;
   DESCRIPTOR_DATA *d;
   int iClass, iRace;
   int iLevelLower;
   int iLevelUpper;
   int nNumber;
   int nMatch;
   int nMatchI;
   bool rgfClass[MAX_CLASS];
   bool rgfRace[MAX_RACE];
   bool fClassRestrict;
   bool fRaceRestrict;
   bool fImmortalOnly;
   bool fRetiredOnly;
   bool fAngelOnly;
   bool fHeroOnly;
   bool fAvatarOnly;
   bool fNeophyteOnly;
   bool fAcolyteOnly;
   bool fLeader;
   bool fPkill;
   bool fShowHomepage;
   bool fClanMatch;
   bool fCouncilMatch;
   bool fDeityMatch;
   bool fGroup;
   CLAN_DATA *pClan = NULL;
   COUNCIL_DATA *pCouncil = NULL;
   DEITY_DATA *pDeity = NULL;
   FILE *whoout = NULL;

   WHO_DATA *cur_who = NULL;
   WHO_DATA *next_who = NULL;
   WHO_DATA *first_mortal = NULL;
   WHO_DATA *first_imm = NULL;
   WHO_DATA *first_angel = NULL;
   WHO_DATA *first_hero = NULL;
   WHO_DATA *first_avatar = NULL;
   WHO_DATA *first_neophyte = NULL;
   WHO_DATA *first_acolyte = NULL;
   WHO_DATA *first_retired = NULL;
   WHO_DATA *first_deadly = NULL;
   WHO_DATA *first_grouped = NULL;
   WHO_DATA *first_groupwho = NULL;


   iLevelLower = 0;
   iLevelUpper = MAX_LEVEL;
   fClassRestrict = FALSE;
   fRaceRestrict = FALSE;
   fImmortalOnly = FALSE;
   fRetiredOnly = FALSE;
   fAngelOnly = FALSE;
   fHeroOnly = FALSE;
   fAvatarOnly = FALSE;
   fNeophyteOnly = FALSE;
   fAcolyteOnly = FALSE;
   fPkill = FALSE;
   fShowHomepage = FALSE;
   fClanMatch = FALSE;
   fCouncilMatch = FALSE;
   fDeityMatch = FALSE;
   fGroup = FALSE;
   fLeader = FALSE;
   for( iClass = 0; iClass < MAX_CLASS; iClass++ )
      rgfClass[iClass] = FALSE;
   for( iRace = 0; iRace < MAX_RACE; iRace++ )
      rgfRace[iRace] = FALSE;

   nNumber = 0;
   for( ;; )
   {
      char arg[MAX_STRING_LENGTH];

      argument = one_argument( argument, arg );
      if( arg[0] == '\0' )
         break;

      if( is_number( arg ) )
      {
         switch ( ++nNumber )
         {
            case 1:
               iLevelLower = atoi( arg );
               break;
            case 2:
               iLevelUpper = atoi( arg );
               break;
            default:
               send_to_char( "Only two level numbers allowed.\n\r", ch );
               return;
         }
      }
      else
      {
         if( strlen( arg ) < 3 )
         {
            send_to_char( "Arguments must be longer than that.\n\r", ch );
            return;
         }

         if( !str_cmp( arg, "deadly" ) || !str_cmp( arg, "pkill" ) )
            fPkill = TRUE;
         else if( !str_cmp( arg, "imm" ) || !str_cmp( arg, "gods" ) )
            fImmortalOnly = TRUE;
         else if( !str_cmp( arg, "angel" ) )
            fAngelOnly = TRUE;
         else if( !str_cmp( arg, "hero" ) )
         {
            iLevelLower = 100;
            iLevelUpper = 100;
            fHeroOnly = TRUE;
         }
         else if( !str_cmp( arg, "avatar" ) )
         {
            iLevelLower = 200;
            iLevelUpper = 200;
            fAvatarOnly = TRUE;
         }
         else if( !str_cmp( arg, "neophyte" ) )
         {
            iLevelLower = 300;
            iLevelUpper = 300;
            fNeophyteOnly = TRUE;
         }
         else if( !str_cmp( arg, "acolyte" ) )
         {
            iLevelLower = 400;
            iLevelUpper = 400;
            fAcolyteOnly = TRUE;
         }
         else if( !str_cmp( arg, "retired" ) )
            fRetiredOnly = TRUE;
         else if( !str_cmp( arg, "leader" ) )
            fLeader = TRUE;
         else if( !str_cmp( arg, "www" ) )
            fShowHomepage = TRUE;
         else if( !str_cmp( arg, "group" ) && ch )
            fGroup = TRUE;
         else if( ( pClan = get_clan( arg ) ) )
            fClanMatch = TRUE;
         else if( ( pCouncil = get_council( arg ) ) )
            fCouncilMatch = TRUE;
         else if( ( pDeity = get_deity( arg ) ) )
            fDeityMatch = TRUE;
         else
         {
            for( iClass = 0; iClass < MAX_CLASS; iClass++ )
            {
               if( !str_cmp( arg, class_table[iClass]->who_name ) )
               {
                  rgfClass[iClass] = TRUE;
                  break;
               }
            }
            if( iClass != MAX_CLASS )
               fClassRestrict = TRUE;

            for( iRace = 0; iRace < MAX_RACE; iRace++ )
            {
               if( !str_cmp( arg, race_table[iRace]->race_name ) )
               {
                  rgfRace[iRace] = TRUE;
                  break;
               }
            }
            if( iRace != MAX_RACE )
               fRaceRestrict = TRUE;

            if( iClass == MAX_CLASS && iRace == MAX_RACE
                && fClanMatch == FALSE && fCouncilMatch == FALSE && fDeityMatch == FALSE )
            {
               send_to_char( "That's not a class, race, order, guild," " council or deity.\n\r", ch );
               return;
            }
         }
      }
   }

   nMatch = 0;
   nMatchI = 0;
   buf[0] = '\0';
   if( ch )
      set_pager_color( AT_GREEN, ch );
   else
   {
      if( fShowHomepage )
         whoout = fopen( WEBWHO_FILE, "w" );
      else
         whoout = fopen( WHO_FILE, "w" );
      if( !whoout )
      {
         bug( "do_who: cannot open who file!" );
         return;
      }
   }

   if( fGroup )
   {
      create_whogr( ch );
      whogr = first_whogr;
      d = whogr->d;
   }
   else
   {
      whogr = NULL;
      d = last_descriptor;
   }
   whogr_p = NULL;
   for( ; d; whogr_p = whogr, whogr = ( fGroup ? whogr->next : NULL ),
        d = ( fGroup ? ( whogr ? whogr->d : NULL ) : d->prev ) )
   {
      CHAR_DATA *wch;
      char const *class;
      char conflag[MAX_STRING_LENGTH];
      char jgj[MAX_STRING_LENGTH];

      if( ( d->connected != CON_PLAYING && d->connected != CON_EDITING && d->connected != CON_MEETING )
          || !can_see( ch, d->character ) || d->original )
         continue;
      wch = d->original ? d->original : d->character;
      if( wch->level < iLevelLower
          || wch->level > iLevelUpper
          || IS_SET( wch->pcdata->flags, PCFLAG_BUILDING )
          || ( IS_AFFECTED( wch, AFF_SHAPESHIFT ) && !IS_IMMORTAL( ch ) )
          || ( fPkill && !CAN_PKILL( wch ) )
          || ( fImmortalOnly && wch->level < LEVEL_VISITOR )
          || ( fAngelOnly && !IS_ANGEL( wch ) )
          || ( fHeroOnly && !IS_HERO2( wch ) )
          || ( fAvatarOnly && !IS_AVA2( wch ) )
          || ( fNeophyteOnly && !IS_NEO2( wch ) )
          || ( fAcolyteOnly && !IS_ACO2( wch ) )
          || ( fRetiredOnly && !IS_RETIRED( wch ) )
          || ( fClassRestrict && !rgfClass[wch->class] )
          || ( fRaceRestrict && !rgfRace[wch->race] )
          || ( fClanMatch && ( pClan != wch->pcdata->clan ) )
          || ( fCouncilMatch && ( pCouncil != wch->pcdata->council ) )
          || ( fDeityMatch && ( pDeity != wch->pcdata->deity ) ) )
         continue;
      if( fLeader && !( wch->pcdata->council &&
                        ( ( wch->pcdata->council->head2 &&
                            !str_cmp( wch->pcdata->council->head2, wch->name ) ) ||
                          ( wch->pcdata->council->head &&
                            !str_cmp( wch->pcdata->council->head, wch->name ) ) ) ) &&
          !( wch->pcdata->clan && ( ( wch->pcdata->clan->deity &&
                                      !str_cmp( wch->pcdata->clan->deity, wch->name ) )
                                    || ( wch->pcdata->clan->leader
                                         && !str_cmp( wch->pcdata->clan->leader, wch->name ) )
                                    || ( wch->pcdata->clan->number1
                                         && !str_cmp( wch->pcdata->clan->number1, wch->name ) )
                                    || ( wch->pcdata->clan->number2
                                         && !str_cmp( wch->pcdata->clan->number2, wch->name ) ) ) ) )
         continue;

      if( fGroup && !wch->leader && !IS_SET( wch->pcdata->flags, PCFLAG_GROUPWHO ) && ( !whogr_p || !whogr_p->indent ) )
         continue;

      if( IS_AGOD( wch ) )
      {
         nMatchI++;
      }
      else
      {
         nMatch++;
      }

      if( fShowHomepage && wch->pcdata->homepage && wch->pcdata->homepage[0] != '\0' )
         sprintf( char_name, "<A HREF=\"%s\">%s</A>", show_tilde( wch->pcdata->homepage ), wch->name );
      else
         sprintf( char_name, "%s", IS_SNAME( wch ) ? wch->pcdata->sname : wch->name );

      if( wch->level >= 400 && wch->level < 601 )
      {
         sprintf( class_text, "&g[&RAcolyte     &Y%3d &G%s&g]&G", wch->sublevel, wch->sex == SEX_MALE ? "M" : "F" );
         class = class_text;
      }
      else if( wch->level >= 300 && wch->level < 400 )
      {
         sprintf( class_text, "&g[&pNeophyte    &Y%3d &G%s&g]&G", wch->sublevel, wch->sex == SEX_MALE ? "M" : "F" );
         class = class_text;
      }
      else if( wch->level >= 200 && wch->level < 300 )
      {
         sprintf( class_text, "&g[&CAvatar      &Y%3d &G%s&g]&G", wch->sublevel, wch->sex == SEX_MALE ? "M" : "F" );
         class = class_text;
      }
      else if( wch->level >= 100 && wch->level < 200 )
      {
         sprintf( class_text, "&g[&BHero        &Y%3d &G%s&g]&G", wch->sublevel, wch->sex == SEX_MALE ? "M" : "F" );
         class = class_text;
      }
      else
      {
         sprintf( class_text, "&g[&Y%2d &G%12s %s&g]&G", wch->level, class_table[wch->class]->who_name,
                  wch->sex == SEX_MALE ? "M" : "F" );
         class = class_text;
      }


      switch ( wch->level )
      {
         default:
            break;
         case MAX_LEVEL - 0:
            sprintf( class_text, "&g[&W   Host Admin   %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 10:
            sprintf( class_text, "&g[&W   Host Senior  %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 20:
            sprintf( class_text, "&g[&W      Host      %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 30:
            sprintf( class_text, "&g[&W  High Templar  %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 40:
            sprintf( class_text, "&g[&W  Arch Templar  %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 50:
            sprintf( class_text, "&g[&W     Templar    %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 60:
            sprintf( class_text, "&g[&W  Elder Wizard  %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 70:
            sprintf( class_text, "&g[&W   Arch Wizard  %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 80:
            sprintf( class_text, "&g[&W     Wizard     %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 90:
            sprintf( class_text, "&g[&W  Grand Bishop  %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 100:
            sprintf( class_text, "&g[&W  Arch Bishop   %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 110:
            sprintf( class_text, "&g[&W     Bishop     %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 120:
            sprintf( class_text, "&g[&W   Grand Lord   %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 130:
            sprintf( class_text, "&g[&W      Lord      %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 140:
            sprintf( class_text, "&g[&W   Arch Duke    %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 150:
            sprintf( class_text, "&g[&W      Duke      %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 160:
            sprintf( class_text, "&g[&W   Arch Druid   %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 170:
            sprintf( class_text, "&g[&W      Druid     %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 180:
            sprintf( class_text, "&g[&W   Arch Angel   %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 190:
            sprintf( class_text, "&g[&W      Angel     %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
         case MAX_LEVEL - 200:
            sprintf( class_text, "&g[&W     Retired    %s&g]&G", wch->sex == SEX_MALE ? "M" : "F" );
            break;
      }
      class = class_text;
      if( !str_cmp( wch->name, sysdata.guild_overseer ) )
         extra_title = " [Overseer of Guilds]";
      else if( !str_cmp( wch->name, sysdata.guild_advisor ) )
         extra_title = " [Advisor to Guilds]";
      else
         extra_title = "";

      if( wch->pcdata->clan )
      {
         CLAN_DATA *pclan = wch->pcdata->clan;
         if( pclan->clan_type == CLAN_GUILD )
            strcpy( clan_name, " <" );
         else
            strcpy( clan_name, " (" );

         if( pclan->clan_type == CLAN_ORDER )
         {
            if( !str_cmp( wch->name, pclan->deity ) )
               strcat( clan_name, "Deity, Order of " );
            else if( !str_cmp( wch->name, pclan->leader ) )
               strcat( clan_name, "Leader, Order of " );
            else if( !str_cmp( wch->name, pclan->number1 ) )
               strcat( clan_name, "Number One, Order of " );
            else if( !str_cmp( wch->name, pclan->number2 ) )
               strcat( clan_name, "Number Two, Order of " );
            else
               strcat( clan_name, "Order of " );
         }
         else if( pclan->clan_type == CLAN_GUILD )
         {
            if( !str_cmp( wch->name, pclan->leader ) )
               strcat( clan_name, "Leader, " );
            if( !str_cmp( wch->name, pclan->number1 ) )
               strcat( clan_name, "First, " );
            if( !str_cmp( wch->name, pclan->number2 ) )
               strcat( clan_name, "Second, " );
         }
         else
         {
            if( !str_cmp( wch->name, pclan->deity ) )
               strcat( clan_name, "D " );
            else if( !str_cmp( wch->name, pclan->leader ) )
               strcat( clan_name, "L " );
            else if( !str_cmp( wch->name, pclan->number1 ) )
               strcat( clan_name, "1 " );
            else if( !str_cmp( wch->name, pclan->number2 ) )
               strcat( clan_name, "2 " );
         }
         strcat( clan_name, pclan->name );
         if( pclan->clan_type == CLAN_GUILD )
            strcat( clan_name, ">" );
         else
            strcat( clan_name, ")" );
/*	  if ( wch->pcdata->rank > 0 )
	  {
	  sprintf( buf2, "%d", wch->pcdata->rank );
	  strcat( clan_name, buf2 );
	  }*/
      }
      else
         clan_name[0] = '\0';

      if( wch->desc && ( wch->desc->connected == CON_EDITING ) )
      {
         strcpy( conflag, "[WRITING] " );
      }
      else if( wch->desc && ( wch->desc->connected == CON_MEETING ) )
      {
         strcpy( conflag, "[MEETING] " );
      }
      else if( NOT_AUTHED( wch ) )
      {
         strcpy( conflag, "&p{&wUNAUTHED&p}} &G" );
      }
      else if( IS_AFFECTED( wch, AFF_SHAPESHIFT ) && IS_IMMORTAL( ch ) )
      {
         sprintf( jgj, "-SHIFTED: %s- ", wch->morph->morph->key_words );
         strcpy( conflag, jgj );
      }
      else
      {
         strcpy( conflag, "" );
      }

      if( wch->pcdata->council )
      {
         strcpy( council_name, " [" );
         if( wch->pcdata->council->head2 == NULL )
         {
            if( !str_cmp( wch->name, wch->pcdata->council->head ) )
               strcat( council_name, "Head of " );
         }
         else
         {
            if( !str_cmp( wch->name, wch->pcdata->council->head ) || !str_cmp( wch->name, wch->pcdata->council->head2 ) )
               strcat( council_name, "Co-Head of " );
         }
         strcat( council_name, wch->pcdata->council_name );
         strcat( council_name, "]" );
      }
      else
         council_name[0] = '\0';

      if( xIS_SET( wch->act, PLR_WIZINVIS ) )
         sprintf( invis_str, "(%d) ", wch->pcdata->wizinvis );
      else
         invis_str[0] = '\0';
      sprintf( buf, "%*s%-17s %s %s%s%s%s%s%s%s%s%s%s%s%s %s\n\r",
               ( fGroup ? whogr->indent : 0 ), "",
               class,
               ( wch->pcdata->spouse ) ? "&G[&WWED&G]" : "",
               invis_str,
               conflag,
               xIS_SET( wch->act, PLR_AFK ) ? "[AFK] " : "",
               xIS_SET( wch->act, PLR_IAW ) ? "[IAW] " : "",
               xIS_SET( wch->act, PLR_IDLE ) ? "&z<&RIdling&z>&G " : "",
               xIS_SET( wch->act, PLR_DEAF ) ? "{Deaf} " : "",
               xIS_SET( wch->act, PLR_ATTACKER ) ? "(ATTACKER) " : "",
               xIS_SET( wch->act, PLR_KILLER ) ? "(KILLER) " : "",
               xIS_SET( wch->act, PLR_THIEF ) ? "(THIEF) " : "", char_name, wch->pcdata->title, extra_title, clan_name );

      CREATE( cur_who, WHO_DATA, 1 );
      cur_who->text = str_dup( buf );
      if( wch->level >= 830 && IS_IMM( wch ) )
         cur_who->type = WT_IMM;
      else if( fGroup )
         if( wch->leader || ( whogr_p && whogr_p->indent ) )
            cur_who->type = WT_GROUPED;
         else
            cur_who->type = WT_GROUPWHO;
      else if( CAN_PKILL( wch ) )
         cur_who->type = WT_DEADLY;
      else if( ( wch->level == 810 || wch->level == 820 ) && IS_ANGEL( wch ) )
         cur_who->type = WT_ANGEL;
      else if( wch->level == 800 && IS_RETIRED( wch ) )
         cur_who->type = WT_RETIRED;
      else if( wch->level == 100 && IS_HERO2( wch ) )
         cur_who->type = WT_HERO;
      else if( wch->level == 200 && IS_AVA2( wch ) )
         cur_who->type = WT_AVATAR;
      else if( wch->level == 300 && IS_NEO2( wch ) )
         cur_who->type = WT_NEOPHYTE;
      else if( wch->level == 400 && IS_ACO2( wch ) )
         cur_who->type = WT_ACOLYTE;
      else
         cur_who->type = WT_MORTAL;

      switch ( cur_who->type )
      {
         case WT_MORTAL:
            cur_who->next = first_mortal;
            first_mortal = cur_who;
            break;
         case WT_DEADLY:
            cur_who->next = first_deadly;
            first_deadly = cur_who;
            break;
         case WT_GROUPED:
            cur_who->next = first_grouped;
            first_grouped = cur_who;
            break;
         case WT_GROUPWHO:
            cur_who->next = first_groupwho;
            first_groupwho = cur_who;
            break;
         case WT_IMM:
            cur_who->next = first_imm;
            first_imm = cur_who;
            break;
         case WT_ANGEL:
            cur_who->next = first_angel;
            first_angel = cur_who;
            break;
         case WT_HERO:
            cur_who->next = first_hero;
            first_hero = cur_who;
            break;
         case WT_AVATAR:
            cur_who->next = first_avatar;
            first_avatar = cur_who;
            break;
         case WT_NEOPHYTE:
            cur_who->next = first_neophyte;
            first_neophyte = cur_who;
            break;
         case WT_ACOLYTE:
            cur_who->next = first_acolyte;
            first_acolyte = cur_who;
            break;
         case WT_RETIRED:
            cur_who->next = first_retired;
            first_retired = cur_who;
            break;
      }

   }


   if( first_imm )
   {
      if( !ch )
         fprintf( whoout, "\n\r&g[ &WIMMORTALS&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
      else
         pager_printf_color( ch, "\n\r&g[ &WIMMORTALS&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
   }

   for( cur_who = first_imm; cur_who; cur_who = next_who )
   {
      if( !ch )
         fprintf( whoout, cur_who->text );
      else
         send_to_pager( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who );
   }
   if( first_angel )
   {
      if( !ch )
         fprintf( whoout, "\n\r&g[ &WANGELS&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
      else
         pager_printf_color( ch, "\n\r&g[ &WANGELS&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
   }

   for( cur_who = first_angel; cur_who; cur_who = next_who )
   {
      if( !ch )
         fprintf( whoout, cur_who->text );
      else
         send_to_pager( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who );
   }
   if( first_retired )
   {
      if( !ch )
         fprintf( whoout, "\n\r&g[ &WRETIRIES&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
      else
         pager_printf_color( ch, "\n\r&g[ &WRETIRIES&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
   }

   for( cur_who = first_retired; cur_who; cur_who = next_who )
   {
      if( !ch )
         fprintf( whoout, cur_who->text );
      else
         send_to_pager( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who );
   }

   if( first_acolyte )
   {
      if( !ch )
         fprintf( whoout, "\n\r&g[ &RACOLYTES&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
      else
         pager_printf_color( ch, "\n\r&g[ &RACOLYTES&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
   }

   for( cur_who = first_acolyte; cur_who; cur_who = next_who )
   {
      if( !ch )
         fprintf( whoout, cur_who->text );
      else
         send_to_pager( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who );
   }
   if( first_neophyte )
   {
      if( !ch )
         fprintf( whoout, "\n\r&g[ &PNEOPHYTES&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
      else
         pager_printf_color( ch, "\n\r&g[ &PNEOPHYTES&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
   }

   for( cur_who = first_neophyte; cur_who; cur_who = next_who )
   {
      if( !ch )
         fprintf( whoout, cur_who->text );
      else
         send_to_pager( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who );
   }
   if( first_avatar )
   {
      if( !ch )
         fprintf( whoout, "\n\r&g[ &cAVATARS&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
      else
         pager_printf_color( ch, "\n\r&g[ &cAVATARS&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
   }

   for( cur_who = first_avatar; cur_who; cur_who = next_who )
   {
      if( !ch )
         fprintf( whoout, cur_who->text );
      else
         send_to_pager( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who );
   }
   if( first_hero )
   {
      if( !ch )
         fprintf( whoout, "\n\r&g[ &BHEROES&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
      else
         pager_printf_color( ch, "\n\r&g[ &BHEROES&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
   }

   for( cur_who = first_hero; cur_who; cur_who = next_who )
   {
      if( !ch )
         fprintf( whoout, cur_who->text );
      else
         send_to_pager( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who );
   }
   if( first_mortal )
   {
      if( !ch )
         fprintf( whoout, "\n\r&g[ &YMORTALS&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
      else
         pager_printf_color( ch, "\n\r&g[ &YMORTALS&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
   }

   for( cur_who = first_mortal; cur_who; cur_who = next_who )
   {
      if( !ch )
         fprintf( whoout, cur_who->text );
      else
         send_to_pager( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who );
   }

   if( first_deadly )
   {
      if( !ch )
         fprintf( whoout, "\n\r&g[ &RPKILL MORTALS&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
      else
         pager_printf_color( ch, "\n\r&g[ &RPKILL MORTALS&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
   }

   for( cur_who = first_deadly; cur_who; cur_who = next_who )
   {
      if( !ch )
         fprintf( whoout, cur_who->text );
      else
         send_to_pager( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who );
   }

   if( first_grouped )
   {
      pager_printf_color( ch, "\n\r&g[ &BGROUPED MORTALS&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
   }
   for( cur_who = first_grouped; cur_who; cur_who = next_who )
   {
      send_to_pager( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who );
   }

   if( first_groupwho )
   {
      if( !ch )
         fprintf( whoout, "\n\r&g[ &cMORTALS SEEKING GROUP&G of &R%s&g]\n\r&G-----------------&D\n\r", sysdata.mud_name );
      else
         pager_printf_color( ch, "\n\r&g[ &cMORTALS SEEKING GROUP&G of &R%s&g]\n\r&G-----------------&D\n\r",
                             sysdata.mud_name );
   }
   for( cur_who = first_groupwho; cur_who; cur_who = next_who )
   {
      if( !ch )
         fprintf( whoout, cur_who->text );
      else
         send_to_pager( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who );
   }

   if( !ch )
   {
      fprintf( whoout,
               "\n\r&g[&Y%d immortal%s&g, &Y%d player%s&g] [&WWebsite:&w http://www.karastorm.org&g] [&RMax players on %d&g]&D\n\r",
               nMatchI, nMatchI == 1 ? "" : "s", nMatch, nMatch == 1 ? "" : "s", sysdata.alltimemax );
      fclose( whoout );
      return;
   }

   set_char_color( AT_YELLOW, ch );
   ch_printf( ch,
              "\n\r&g[&Y%d immortal%s&g, &Y%d player%s&g] [&WWebsite:&w http://www.karastorm.org&g] [&RMax players on %d&g]&D\n\r",
              nMatchI, nMatchI == 1 ? "" : "s", nMatch, nMatch == 1 ? "" : "s", sysdata.alltimemax );
   return;
}


void do_compare( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   OBJ_DATA *obj1;
   OBJ_DATA *obj2;
   int value1;
   int value2;
   char *msg;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   if( arg1[0] == '\0' )
   {
      send_to_char( "Compare what to what?\n\r", ch );
      return;
   }

   if( ( obj1 = get_obj_carry( ch, arg1 ) ) == NULL )
   {
      send_to_char( "You do not have that item.\n\r", ch );
      return;
   }

   if( arg2[0] == '\0' )
   {
      for( obj2 = ch->first_carrying; obj2; obj2 = obj2->next_content )
      {
         if( obj2->wear_loc != WEAR_NONE
             && can_see_obj( ch, obj2 )
             && obj1->item_type == obj2->item_type && ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE ) != 0 )
            break;
      }

      if( !obj2 )
      {
         send_to_char( "You aren't wearing anything comparable.\n\r", ch );
         return;
      }
   }
   else
   {
      if( ( obj2 = get_obj_carry( ch, arg2 ) ) == NULL )
      {
         send_to_char( "You do not have that item.\n\r", ch );
         return;
      }
   }

   msg = NULL;
   value1 = 0;
   value2 = 0;

   if( obj1 == obj2 )
   {
      msg = "You compare $p to itself.  It looks about the same.";
   }
   else if( obj1->item_type != obj2->item_type )
   {
      msg = "You can't compare $p and $P.";
   }
   else
   {
      switch ( obj1->item_type )
      {
         default:
            msg = "You can't compare $p and $P.";
            break;

         case ITEM_ARMOR:
            value1 = obj1->value[0];
            value2 = obj2->value[0];
            break;

         case ITEM_WEAPON:
            value1 = obj1->value[1] + obj1->value[2];
            value2 = obj2->value[1] + obj2->value[2];
            break;
      }
   }

   if( !msg )
   {
      if( value1 == value2 )
         msg = "$p and $P look about the same.";
      else if( value1 > value2 )
         msg = "$p looks better than $P.";
      else
         msg = "$p looks worse than $P.";
   }

   act( AT_PLAIN, msg, ch, obj1, obj2, TO_CHAR );
   return;
}



void do_where( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char pname[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   DESCRIPTOR_DATA *d;
   bool found;

   one_argument( argument, arg );

   if( arg[0] != '\0'
       && ( victim = get_char_world( ch, arg ) ) && !IS_NPC( victim )
       && IS_SET( victim->pcdata->flags, PCFLAG_DND ) && get_trust( ch ) < get_trust( victim ) )
   {
      act( AT_PLAIN, "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
      return;
   }

   switch ( ch->in_room->area->plane )
   {
      default:
         break;
      case 1:
         sprintf( pname, "&W%d - %d&P", ch->in_room->area->low_soft_range, ch->in_room->area->hi_soft_range );
         break;
      case 2:
         sprintf( pname, "&BHero&P" );
         break;
      case 3:
         sprintf( pname, "&CAvatar&P" );
         break;
      case 4:
         sprintf( pname, "&pNeophyte&P" );
         break;
      case 5:
         sprintf( pname, "&RAcolyte&P" );
         break;
      case 6:
         sprintf( pname, "&YAll&P" );
         break;
      case 7:
         sprintf( pname, "&zAngel&P" );
         break;
   }

   set_pager_color( AT_PERSON, ch );
   if( arg[0] == '\0' )
   {
      pager_printf( ch, "\n\rPlayers near you in %s (%s):\n\r", ch->in_room->area->name, pname );
      found = FALSE;
      for( d = first_descriptor; d; d = d->next )
         if( ( d->connected == CON_PLAYING || d->connected == CON_EDITING || d->connected == CON_MEETING )
             && ( victim = d->character ) != NULL
             && !IS_NPC( victim )
             && victim->in_room
             && victim->in_room->area == ch->in_room->area
             && can_see( ch, victim )
             && ( get_trust( ch ) >= get_trust( victim ) || !IS_SET( victim->pcdata->flags, PCFLAG_DND ) ) )

         {
            found = TRUE;
            pager_printf_color( ch, "&P%-13s  ", victim->name );
            if( IS_IMMORTAL( victim ) && victim->level > 600 )
               send_to_pager_color( "&P(&WImmortal&P)\t", ch );
            else if( CAN_PKILL( victim ) && victim->pcdata->clan
                     && victim->pcdata->clan->clan_type != CLAN_ORDER && victim->pcdata->clan->clan_type != CLAN_GUILD )
               pager_printf_color( ch, "%-18s\t", victim->pcdata->clan->badge );
            else if( CAN_PKILL( victim ) )
               send_to_pager_color( "(&wPlrKill&P)\t", ch );
            else
               send_to_pager( "\t\t\t", ch );
            pager_printf_color( ch, "&P%s\n\r", victim->in_room->name );
         }
      if( !found )
         send_to_char( "None\n\r", ch );
   }
   else
   {
      found = FALSE;
      for( victim = first_char; victim; victim = victim->next )
         if( victim->in_room
             && victim->in_room->area == ch->in_room->area
             && !IS_AFFECTED( victim, AFF_HIDE )
             && !IS_AFFECTED( victim, AFF_SNEAK ) && can_see( ch, victim ) && nifty_is_name( arg, victim->name ) )
         {
            found = TRUE;
            pager_printf( ch, "%-28s %s\n\r", PERS( victim, ch ), victim->in_room->name );
            break;
         }
      if( !found )
         act( AT_PLAIN, "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
   }

   return;
}




void do_consider( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   char *msg;
   int diff;

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "Consider killing whom?\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They're not here.\n\r", ch );
      return;
   }
   if( victim == ch )
   {
      send_to_char( "You decide you're pretty sure you could take yourself in a fight.\n\r", ch );
      return;
   }
   diff = victim->level - ch->level;

   if( diff <= -10 )
      msg = "You are far more experienced than $N.";
   else if( diff <= -5 )
      msg = "$N is not nearly as experienced as you.";
   else if( diff <= -2 )
      msg = "You are more experienced than $N.";
   else if( diff <= 1 )
      msg = "You are just about as experienced as $N.";
   else if( diff <= 4 )
      msg = "You are not nearly as experienced as $N.";
   else if( diff <= 9 )
      msg = "$N is far more experienced than you!";
   else
      msg = "$N would make a great teacher for you!";
   act( AT_CONSIDER, msg, ch, NULL, victim, TO_CHAR );

   diff = ( int )( victim->max_hit - ch->max_hit ) / 6;

   if( diff <= -200 )
      msg = "$N looks like a feather!";
   else if( diff <= -150 )
      msg = "You could kill $N with your hands tied!";
   else if( diff <= -100 )
      msg = "Hey! Where'd $N go?";
   else if( diff <= -50 )
      msg = "$N is a wimp.";
   else if( diff <= 0 )
      msg = "$N looks weaker than you.";
   else if( diff <= 50 )
      msg = "$N looks about as strong as you.";
   else if( diff <= 100 )
      msg = "It would take a bit of luck...";
   else if( diff <= 150 )
      msg = "It would take a lot of luck, and equipment!";
   else if( diff <= 200 )
      msg = "Why don't you dig a grave for yourself first?";
   else
      msg = "$N is built like a TANK!";
   act( AT_CONSIDER, msg, ch, NULL, victim, TO_CHAR );

   return;
}



#define CANT_PRAC "Tongue"

void do_practice( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   int sn;

   if( IS_NPC( ch ) )
      return;

   if( argument[0] == '\0' )
   {
      int col;
      sh_int lasttype, cnt;

      col = cnt = 0;
      lasttype = SKILL_SPELL;
      set_pager_color( AT_MAGIC, ch );
      for( sn = 0; sn < gsn_first_tongue; sn++ )
      {
         if( !skill_table[sn]->name )
            break;

         if( strcmp( skill_table[sn]->name, "reserved" ) == 0 && ( IS_IMMORTAL( ch ) || !IS_IMMORTAL( ch ) ) )
         {
            if( col % 2 != 0 )
               send_to_pager( "\n\r", ch );
            set_pager_color( AT_MAGIC, ch );
            send_to_pager_color( " &p----------------------------------[ &CSpells&p ]----------------------------------\n\r",
                                 ch );
            col = 0;
         }
         if( skill_table[sn]->type != lasttype )
         {
            if( !cnt )
               send_to_pager( "                                   (none)\n\r", ch );
            else if( col % 2 != 0 )
               send_to_pager( "\n\r", ch );
            set_pager_color( AT_MAGIC, ch );
            pager_printf_color( ch,
                                " &p----------------------------------[ &C%ss&p ]----------------------------------\n\r",
                                skill_tname[skill_table[sn]->type] );
            col = cnt = 0;
         }
         lasttype = skill_table[sn]->type;

         if( !IS_IMMORTAL( ch )
             && ( skill_table[sn]->guild != CLASS_NONE
                  && ( !IS_GUILDED( ch ) || ( ch->pcdata->clan->class != skill_table[sn]->guild ) ) ) )
            continue;

         if( ( ch->level < skill_table[sn]->skill_level[ch->class] && !DUAL_SKILL( ch, sn ) ) || ( !IS_IMMORTAL( ch )   //&& skill_table[sn]->skill_level[ch->class] == 0
                                                                                                   && !DUAL_SKILL( ch,
                                                                                                                   sn ) ) )
            continue;

         if( ch->pcdata->learned[sn] <= 0 && SPELL_FLAG( skill_table[sn], SF_SECRETSKILL ) )
            continue;

         ++cnt;
         switch ( find_skill_level( ch, sn ) )
         {
            case 100:
               sprintf( buf2, "&YHer&z(&R%3d&z)&B", find_skill_slevel( ch, sn ) );
               break;
            case 200:
               sprintf( buf2, "&CAva&z(&R%3d&z)&B", find_skill_slevel( ch, sn ) );
               break;
            case 300:
               sprintf( buf2, "&PNeo&z(&R%3d&z)&B", find_skill_slevel( ch, sn ) );
               break;
            case 400:
               sprintf( buf2, "&RAco&z(&R%3d&z)&B", find_skill_slevel( ch, sn ) );
               break;
            case 800:
               sprintf( buf2, "&WImm&z(&R%3d&z)&B", find_skill_level( ch, sn ) );
               break;
            default:
               sprintf( buf2, "   &z(&R%3d&z)&B", find_skill_level( ch, sn ) );
               break;
         }
         pager_printf( ch, "  %s %20.20s", buf2, skill_table[sn]->name );
         if( ch->pcdata->learned[sn] > 0 )
         {
            pager_printf( ch, " &C%3d%% ", ch->pcdata->learned[sn] );
         }
         else
         {
            pager_printf( ch, " &r%3d%%&D ", ch->pcdata->learned[sn] );
         }
         if( ++col % 2 == 0 )
            send_to_pager( "\n\r", ch );
      }

      if( col % 2 != 0 )
         send_to_pager( "\n\r", ch );
      pager_printf( ch, "&cYou have &Y%d&c practice sessions left.\n\r&D", ch->practice );
   }
   else
   {
      CHAR_DATA *mob;
      int adept;
      bool can_prac = TRUE;

      if( !IS_AWAKE( ch ) )
      {
         send_to_char( "In your dreams, or what?\n\r", ch );
         return;
      }

      for( mob = ch->in_room->first_person; mob; mob = mob->next_in_room )
         if( IS_NPC( mob ) && xIS_SET( mob->act, ACT_PRACTICE ) )
            break;

      if( !mob )
      {
         send_to_char( "You can't do that here.\n\r", ch );
         return;
      }

      if( ch->level < 3 )
      {
         send_to_char( "You must reach level 3 to practice.\n\r", ch );
         return;
      }

      if( ch->practice <= 0 )
      {
         act( AT_TELL, "$n tells you 'You must earn some more practice sessions.'", mob, NULL, ch, TO_VICT );
         return;
      }

      sn = skill_lookup( argument );

      if( can_prac && ( ( sn == -1 ) || ( !IS_NPC( ch ) && !DUAL_SKILL( ch, sn ) ) ) )
      {
         act( AT_TELL, "$n tells you 'You're not ready to learn that yet...'", mob, NULL, ch, TO_VICT );
         return;
      }

      if( is_name( skill_tname[skill_table[sn]->type], CANT_PRAC ) )
      {
         act( AT_TELL, "$n tells you 'I do not know how to teach that.'", mob, NULL, ch, TO_VICT );
         return;
      }

      if( skill_table[sn]->teachers && skill_table[sn]->teachers[0] != '\0' )
      {
         sprintf( buf, "%d", mob->pIndexData->vnum );
         if( !is_name( buf, skill_table[sn]->teachers ) )
         {
            act( AT_TELL, "$n tells you, 'I know not know how to teach that.'", mob, NULL, ch, TO_VICT );
            return;
         }
      }

      if( !IS_NPC( ch ) && skill_table[sn]->guild != CLASS_NONE )
      {
         act( AT_TELL, "$n tells you 'That is only for members of guilds...'", mob, NULL, ch, TO_VICT );
         return;
      }


      adept = dual_adept( ch, sn ); // * 0.2;

      if( ch->pcdata->learned[sn] >= adept )
      {
         sprintf( buf, "$n tells you, 'I've taught you everything I can about %s.'", skill_table[sn]->name );
         act( AT_TELL, buf, mob, NULL, ch, TO_VICT );
         act( AT_TELL, "$n tells you, 'You'll have to practice it on your own now...'", mob, NULL, ch, TO_VICT );
      }
      else
      {
         ch->practice--;
         ch->pcdata->learned[sn] += int_app[get_curr_int( ch )].learn;
         act( AT_ACTION, "You practice $T.", ch, NULL, skill_table[sn]->name, TO_CHAR );
         act( AT_ACTION, "$n practices $T.", ch, NULL, skill_table[sn]->name, TO_ROOM );
         if( ch->pcdata->learned[sn] >= adept )
         {
            ch->pcdata->learned[sn] = adept;
            act( AT_TELL, "$n tells you. 'You'll have to practice it on your own now...'", mob, NULL, ch, TO_VICT );
         }
      }
   }
   return;
}


void do_wimpy( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   int wimpy;

   set_char_color( AT_YELLOW, ch );
   one_argument( argument, arg );
   if( !str_cmp( arg, "max" ) )
   {
      if( IS_PKILL( ch ) )
         wimpy = ( int )ch->max_hit / 2.25;
      else
         wimpy = ( int )ch->max_hit / 1.2;
   }
   else if( arg[0] == '\0' )
      wimpy = ( int )ch->max_hit / 5;
   else
      wimpy = atoi( arg );

   if( wimpy < 0 )
   {
      send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
      return;
   }
   if( IS_PKILL( ch ) && wimpy > ( int )ch->max_hit / 2.25 )
   {
      send_to_char( "Such cowardice ill becomes you.\n\r", ch );
      return;
   }
   else if( wimpy > ( int )ch->max_hit / 1.2 )
   {
      send_to_char( "Such cowardice ill becomes you.\n\r", ch );
      return;
   }
   ch->wimpy = wimpy;
   ch_printf( ch, "Wimpy set to %d hit points.\n\r", wimpy );
   return;
}



void do_password( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char log_buf[MAX_STRING_LENGTH];
   char *pArg;
   char *pwdnew;
   char *p;
   char cEnd;

   if( IS_NPC( ch ) )
      return;

   pArg = arg1;
   while( isspace( *argument ) )
      argument++;

   cEnd = ' ';
   if( *argument == '\'' || *argument == '"' )
      cEnd = *argument++;

   while( *argument != '\0' )
   {
      if( *argument == cEnd )
      {
         argument++;
         break;
      }
      *pArg++ = *argument++;
   }
   *pArg = '\0';

   pArg = arg2;
   while( isspace( *argument ) )
      argument++;

   cEnd = ' ';
   if( *argument == '\'' || *argument == '"' )
      cEnd = *argument++;

   while( *argument != '\0' )
   {
      if( *argument == cEnd )
      {
         argument++;
         break;
      }
      *pArg++ = *argument++;
   }
   *pArg = '\0';

   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Syntax: password <new> <again>.\n\r", ch );
      send_to_char( "Syntax: password <new> <again>.\n\r", ch );
      return;
   }

   if( strcmp( arg1, arg2 ) )
   {
      send_to_char( "Passwords don't match try again.\n\r", ch );
      return;
   }
   if( strlen( arg2 ) < 5 )
   {
      send_to_char( "New password must be at least five characters long.\n\r", ch );
      return;
   }

   pwdnew = crypt( arg2, ch->name );
   for( p = pwdnew; *p != '\0'; p++ )
   {
      if( *p == '~' )
      {
         send_to_char( "New password not acceptable, try again.\n\r", ch );
         return;
      }
   }

   DISPOSE( ch->pcdata->pwd );
   ch->pcdata->pwd = str_dup( pwdnew );
   if( IS_SET( sysdata.save_flags, SV_PASSCHG ) )
      save_char_obj( ch );
   if( ch->desc && ch->desc->host[0] != '\0' )
      sprintf( log_buf, "%s changing password from site %s\n", ch->name, ch->desc->host );
   else
      sprintf( log_buf, "%s changing thier password with no descriptor!", ch->name );
   log_string( log_buf );
   send_to_char( "Ok.\n\r", ch );
   return;
}



void do_socials( CHAR_DATA * ch, char *argument )
{
   int iHash;
   int col = 0;
   SOCIALTYPE *social;

   set_pager_color( AT_PLAIN, ch );
   for( iHash = 0; iHash < 27; iHash++ )
      for( social = social_index[iHash]; social; social = social->next )
      {
         pager_printf( ch, "%-12s", social->name );
         if( ++col % 6 == 0 )
            send_to_pager( "\n\r", ch );
      }

   if( col % 6 != 0 )
      send_to_pager( "\n\r", ch );
   return;
}

void do_xsocials( CHAR_DATA * ch, char *argument )
{
   int iHash;
   int col = 0;
   XSOCIALTYPE *xsocial;

   set_pager_color( AT_PLAIN, ch );
   for( iHash = 0; iHash < 27; iHash++ )
      for( xsocial = xsocial_index[iHash]; xsocial; xsocial = xsocial->next )
      {
         pager_printf( ch, "%-12s", xsocial->name );
         if( ++col % 6 == 0 )
            send_to_pager( "\n\r", ch );
      }

   if( col % 6 != 0 )
      send_to_pager( "\n\r", ch );
   return;
}


void do_commands( CHAR_DATA * ch, char *argument )
{
   int col;
   bool found;
   int hash;
   CMDTYPE *command;

   col = 0;
   set_pager_color( AT_PLAIN, ch );
   if( argument[0] == '\0' )
   {
      for( hash = 0; hash < 126; hash++ )
         for( command = command_hash[hash]; command; command = command->next )
            if( command->level < LEVEL_HERO && command->level <= get_trust( ch ) && command->cshow == 1 )
            {
               pager_printf( ch, "%-12s", command->name );
               if( ++col % 6 == 0 )
                  send_to_pager( "\n\r", ch );
            }
      if( col % 6 != 0 )
         send_to_pager( "\n\r", ch );
   }
   else
   {
      found = FALSE;
      for( hash = 0; hash < 126; hash++ )
         for( command = command_hash[hash]; command; command = command->next )
            if( command->level < LEVEL_HERO
                && command->level <= get_trust( ch ) && command->cshow == 1 && !str_prefix( argument, command->name ) )
            {
               pager_printf( ch, "%-12s", command->name );
               found = TRUE;
               if( ++col % 6 == 0 )
                  send_to_pager( "\n\r", ch );
            }

      if( col % 6 != 0 )
         send_to_pager( "\n\r", ch );
      if( !found )
         ch_printf( ch, "No command found under %s.\n\r", argument );
   }
   return;
}

void do_deaf( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) )
      return;

   if( xIS_SET( ch->act, PLR_DEAF ) )
   {
      xREMOVE_BIT( ch->act, PLR_DEAF );
      xREMOVE_BIT( ch->deaf, CHANNEL_RACETALK );
      xREMOVE_BIT( ch->deaf, CHANNEL_AUCTION );
      xREMOVE_BIT( ch->deaf, CHANNEL_CHAT );
      xREMOVE_BIT( ch->deaf, CHANNEL_QUEST );
      xREMOVE_BIT( ch->deaf, CHANNEL_WARTALK );
      xREMOVE_BIT( ch->deaf, CHANNEL_PRAY );
      xREMOVE_BIT( ch->deaf, CHANNEL_TRAFFIC );
      xREMOVE_BIT( ch->deaf, CHANNEL_IC );
      xREMOVE_BIT( ch->deaf, CHANNEL_MUSIC );
      xREMOVE_BIT( ch->deaf, CHANNEL_ASK );
      xREMOVE_BIT( ch->deaf, CHANNEL_SHOUT );
      xREMOVE_BIT( ch->deaf, CHANNEL_YELL );
      xREMOVE_BIT( ch->deaf, CHANNEL_INFO );
      xREMOVE_BIT( ch->deaf, CHANNEL_FLAME );
      xREMOVE_BIT( ch->deaf, CHANNEL_GRATZ );
      xREMOVE_BIT( ch->deaf, CHANNEL_TELLS );
      xREMOVE_BIT( ch->deaf, CHANNEL_WHISPER );
      xREMOVE_BIT( ch->deaf, CHANNEL_NEWBIE );
      xREMOVE_BIT( ch->deaf, CHANNEL_BUILD );
      xREMOVE_BIT( ch->deaf, CHANNEL_MONITOR );
      xREMOVE_BIT( ch->deaf, CHANNEL_COMM );
      xREMOVE_BIT( ch->deaf, CHANNEL_LOG );
      xREMOVE_BIT( ch->deaf, CHANNEL_HBUILD );
      xREMOVE_BIT( ch->deaf, CHANNEL_IMMTALK );
      xREMOVE_BIT( ch->deaf, CHANNEL_CODER );
      xREMOVE_BIT( ch->deaf, CHANNEL_AUTH );
      xREMOVE_BIT( ch->deaf, CHANNEL_WARN );
      xREMOVE_BIT( ch->deaf, CHANNEL_HIGH );
      xREMOVE_BIT( ch->deaf, CHANNEL_HIGHGOD );

      if( ch->pcdata->clan )
         xREMOVE_BIT( ch->deaf, CHANNEL_CLAN );
      if( ch->level >= LEVEL_AVATAR )
         xREMOVE_BIT( ch->deaf, CHANNEL_AVTALK );
      if( ch->level >= LEVEL_HERO )
         xREMOVE_BIT( ch->deaf, CHANNEL_HTALK );
      if( ch->level >= LEVEL_ACOLYTE )
         xREMOVE_BIT( ch->deaf, CHANNEL_ACOTALK );
      if( ch->level >= LEVEL_NEOPHYTE )
         xREMOVE_BIT( ch->deaf, CHANNEL_NEOTALK );
      send_to_char( "Deaf toggle off.\n\r", ch );
      return;
   }
   if( !xIS_SET( ch->act, PLR_DEAF ) )
   {
      xSET_BIT( ch->act, PLR_DEAF );
      xSET_BIT( ch->deaf, CHANNEL_RACETALK );
      xSET_BIT( ch->deaf, CHANNEL_AUCTION );
      xSET_BIT( ch->deaf, CHANNEL_TRAFFIC );
      xSET_BIT( ch->deaf, CHANNEL_IC );
      xSET_BIT( ch->deaf, CHANNEL_CHAT );
      xSET_BIT( ch->deaf, CHANNEL_QUEST );
      xSET_BIT( ch->deaf, CHANNEL_PRAY );
      xSET_BIT( ch->deaf, CHANNEL_MUSIC );
      xSET_BIT( ch->deaf, CHANNEL_ASK );
      xSET_BIT( ch->deaf, CHANNEL_SHOUT );
      xSET_BIT( ch->deaf, CHANNEL_WARTALK );
      xSET_BIT( ch->deaf, CHANNEL_YELL );
      xSET_BIT( ch->deaf, CHANNEL_INFO );
      xSET_BIT( ch->deaf, CHANNEL_FLAME );
      xSET_BIT( ch->deaf, CHANNEL_GRATZ );
      xSET_BIT( ch->deaf, CHANNEL_TELLS );
      xSET_BIT( ch->deaf, CHANNEL_WHISPER );
      xSET_BIT( ch->deaf, CHANNEL_NEWBIE );
      xSET_BIT( ch->deaf, CHANNEL_BUILD );
      xSET_BIT( ch->deaf, CHANNEL_MONITOR );
      xSET_BIT( ch->deaf, CHANNEL_COMM );
      xSET_BIT( ch->deaf, CHANNEL_LOG );
      xSET_BIT( ch->deaf, CHANNEL_HBUILD );
      xSET_BIT( ch->deaf, CHANNEL_IMMTALK );
      xSET_BIT( ch->deaf, CHANNEL_CODER );
      xSET_BIT( ch->deaf, CHANNEL_AUTH );
      xSET_BIT( ch->deaf, CHANNEL_WARN );
      xSET_BIT( ch->deaf, CHANNEL_HIGH );
      xSET_BIT( ch->deaf, CHANNEL_HIGHGOD );

      if( ch->pcdata->clan )
         xSET_BIT( ch->deaf, CHANNEL_CLAN );
      if( ch->level >= LEVEL_AVATAR )
         xSET_BIT( ch->deaf, CHANNEL_AVTALK );
      if( ch->level >= LEVEL_HERO )
         xSET_BIT( ch->deaf, CHANNEL_HTALK );
      if( ch->level >= LEVEL_ACOLYTE )
         xSET_BIT( ch->deaf, CHANNEL_ACOTALK );
      if( ch->level >= LEVEL_NEOPHYTE )
         xSET_BIT( ch->deaf, CHANNEL_NEOTALK );
      send_to_char( "Deaf toggle on.\n\r", ch );
      return;
   }
}

void do_channels( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   one_argument( argument, arg );

   if( IS_NPC( ch ) )
      return;

   if( arg[0] == '\0' )
   {
      if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_SILENCE ) )
      {
         set_char_color( AT_GREEN, ch );
         send_to_char( "You are silenced.\n\r", ch );
         return;
      }

      if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_DEAF ) )
      {
         send_to_char( "You have deaf on.\n\n\r", ch );
      }

      send_to_char_color( "&YChannels&G:\n\r", ch );
      ch_printf_color( ch, "&gRacetalk  :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_RACETALK ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gTell     :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_TELLS ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gChat     :%s\n\r", !xIS_SET( ch->deaf, CHANNEL_CHAT ) ? " &GOn" : " &GOff" );
      if( get_trust( ch ) > 1 && !NOT_AUTHED( ch ) )
         ch_printf_color( ch, "&gAuction   :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_AUCTION ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gWhisper  :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_WHISPER ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gTraffic  :%s\n\r", !xIS_SET( ch->deaf, CHANNEL_TRAFFIC ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gIC        :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_IC ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gQuest    :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_QUEST ) ? " &GOn" : " &GOff" );
      if( !IS_NPC( ch ) && ch->pcdata->clan )
         ch_printf_color( ch, "&gClan     :%s", !xIS_SET( ch->deaf, CHANNEL_CLAN ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "\n\r&gFlame     :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_FLAME ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gNewbie   :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_NEWBIE ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gWartalk  :%s\n\r", !xIS_SET( ch->deaf, CHANNEL_WARTALK ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gMusic     :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_MUSIC ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gAsk      :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_ASK ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gShout    :%s\n\r", !xIS_SET( ch->deaf, CHANNEL_SHOUT ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gYell      :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_YELL ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gInfo     :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_INFO ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gGratz    :%s\n\r", !xIS_SET( ch->deaf, CHANNEL_GRATZ ) ? " &GOn" : " &GOff" );
      if( IS_AVA( ch ) )
         ch_printf_color( ch, "&gAvatar    :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_AVTALK ) ? " &GOn" : " &GOff" );
      if( IS_HERO( ch ) )
         ch_printf_color( ch, "&gHero     :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_HTALK ) ? " &GOn" : " &GOff" );
      if( IS_NEO( ch ) )
         ch_printf_color( ch, "&gNeophyte :%s\n\r", !xIS_SET( ch->deaf, CHANNEL_NEOTALK ) ? " &GOn" : " &GOff" );
      if( IS_ACO( ch ) )
         ch_printf_color( ch, "&gAcolyte   :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_ACOTALK ) ? " &GOn" : " &GOff" );
      if( IS_MARRIED( ch ) )
         ch_printf_color( ch, "&gSpTalk   :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_MARRY ) ? " &GOn" : " &GOff" );


      if( IS_ANGEL( ch ) )
      {
         ch_printf_color( ch, "\n\n\r&YAngel Channels&G:\n\r" );
         ch_printf_color( ch, "&gImmortal  :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_IMMTALK ) ? " &GOn" : " &GOff" );
         ch_printf_color( ch, "&gPray     :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_PRAY ) ? " &GOn" : " &GOff" );
         ch_printf_color( ch, "&gComm     :%s\n\r", !xIS_SET( ch->deaf, CHANNEL_COMM ) ? " &GOn" : " &GOff" );
      }

      if( IS_IMM( ch ) )
      {
         ch_printf_color( ch, "\n\r&YImmortal Channels&G:\n\r" );
         ch_printf_color( ch, "&gImmortal  :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_IMMTALK ) ? " &GOn" : " &GOff" );
         ch_printf_color( ch, "&gPray     :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_PRAY ) ? " &GOn" : " &GOff" );
         ch_printf_color( ch, "&gComm     :%s\n\r", !xIS_SET( ch->deaf, CHANNEL_COMM ) ? " &GOn" : " &GOff" );
         if( get_trust( ch ) >= sysdata.muse_level )
            ch_printf_color( ch, "&gMuse      :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_HIGHGOD ) ? " &GOn" : " &GOff" );
         ch_printf_color( ch, "&gMonitor  :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_MONITOR ) ? " &GOn" : " &GOff" );
         ch_printf_color( ch, "&gAuth     :%s\n\r", !xIS_SET( ch->deaf, CHANNEL_AUTH ) ? " &GOn" : " &GOff" );
         ch_printf_color( ch, "&gBuild     :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_BUILD ) ? " &GOn" : " &GOff" );
         if( get_trust( ch ) >= sysdata.log_level )
         {
            ch_printf_color( ch, "&gLog      :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_LOG ) ? " &GOn" : " &GOff" );
            ch_printf_color( ch, "&gWarn     :%s\n\r", !xIS_SET( ch->deaf, CHANNEL_WARN ) ? " &GOn" : " &GOff" );
            ch_printf_color( ch, "&gHeadBuild :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_HBUILD ) ? " &GOn" : " &GOff" );
            if( get_trust( ch ) >= sysdata.think_level )
               ch_printf_color( ch, "&gHigh     :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_HIGH ) ? " &GOn" : " &GOff" );
         }
         if( get_trust( ch ) >= ( MAX_LEVEL - 50 ) )
            ch_printf_color( ch, "&gCoder    :%s\n\r", !xIS_SET( ch->deaf, CHANNEL_CODER ) ? " &GOn" : " &GOff" );
         if( get_trust( ch ) >= ( MAX_LEVEL - 20 ) )
            ch_printf_color( ch, "&gAdmin     :%s\t\t", !xIS_SET( ch->deaf, CHANNEL_ADMTALK ) ? " &GOn" : " &GOff" );
      }
      send_to_char( "\n\r", ch );
   }
   else
   {
      bool fClear;
      bool ClearAll;
      int bit;

      bit = 0;
      ClearAll = FALSE;

      if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_DEAF ) && str_cmp( arg, "-all" ) )
      {
         send_to_char( "Please turn deaf off.\n\r", ch );
         return;
      }

      if( arg[0] == '+' )
         fClear = TRUE;
      else if( arg[0] == '-' )
         fClear = FALSE;
      else
      {
         send_to_char( "Channels -channel or +channel?\n\r", ch );
         return;
      }

      if( !str_cmp( arg + 1, "auction" ) )
         bit = CHANNEL_AUCTION;
      else if( !str_cmp( arg + 1, "traffic" ) )
         bit = CHANNEL_TRAFFIC;
      else if( !str_cmp( arg + 1, "ic" ) )
         bit = CHANNEL_IC;
      else if( !str_cmp( arg + 1, "chat" ) )
         bit = CHANNEL_CHAT;
      else if( !str_cmp( arg + 1, "clan" ) )
         bit = CHANNEL_CLAN;
      else if( !str_cmp( arg + 1, "council" ) )
         bit = CHANNEL_COUNCIL;
      else if( !str_cmp( arg + 1, "guild" ) )
         bit = CHANNEL_GUILD;
      else if( !str_cmp( arg + 1, "quest" ) )
         bit = CHANNEL_QUEST;
      else if( !str_cmp( arg + 1, "tells" ) )
         bit = CHANNEL_TELLS;
      else if( !str_cmp( arg + 1, "immtalk" ) )
         bit = CHANNEL_IMMTALK;
      else if( !str_cmp( arg + 1, "admtalk" ) )
         bit = CHANNEL_ADMTALK;
      else if( !str_cmp( arg + 1, "pray" ) )
         bit = CHANNEL_PRAY;
      else if( !str_cmp( arg + 1, "flame" ) )
         bit = CHANNEL_FLAME;
      else if( !str_cmp( arg + 1, "gratz" ) )
         bit = CHANNEL_GRATZ;
      else if( !str_cmp( arg + 1, "log" ) )
         bit = CHANNEL_LOG;
      else if( !str_cmp( arg + 1, "build" ) )
         bit = CHANNEL_BUILD;
      else if( !str_cmp( arg + 1, "hbuild" ) )
         bit = CHANNEL_HBUILD;
      else if( !str_cmp( arg + 1, "code" ) )
         bit = CHANNEL_CODER;
      else if( !str_cmp( arg + 1, "high" ) )
         bit = CHANNEL_HIGH;
      else if( !str_cmp( arg + 1, "pray" ) )
         bit = CHANNEL_PRAY;
      else if( !str_cmp( arg + 1, "sptalk" ) )
         bit = CHANNEL_MARRY;
      else if( !str_cmp( arg + 1, "avatar" ) )
         bit = CHANNEL_AVTALK;
      else if( !str_cmp( arg + 1, "hero" ) )
         bit = CHANNEL_HTALK;
      else if( !str_cmp( arg + 1, "neophyte" ) )
         bit = CHANNEL_NEOTALK;
      else if( !str_cmp( arg + 1, "acolyte" ) )
         bit = CHANNEL_ACOTALK;
      else if( !str_cmp( arg + 1, "monitor" ) )
         bit = CHANNEL_MONITOR;
      else if( !str_cmp( arg + 1, "auth" ) )
         bit = CHANNEL_AUTH;
      else if( !str_cmp( arg + 1, "newbie" ) )
         bit = CHANNEL_NEWBIE;
      else if( !str_cmp( arg + 1, "music" ) )
         bit = CHANNEL_MUSIC;
      else if( !str_cmp( arg + 1, "muse" ) )
         bit = CHANNEL_HIGHGOD;
      else if( !str_cmp( arg + 1, "ask" ) )
         bit = CHANNEL_ASK;
      else if( !str_cmp( arg + 1, "shout" ) )
         bit = CHANNEL_SHOUT;
      else if( !str_cmp( arg + 1, "yell" ) )
         bit = CHANNEL_YELL;
      else if( !str_cmp( arg + 1, "comm" ) )
         bit = CHANNEL_COMM;
      else if( !str_cmp( arg + 1, "warn" ) )
         bit = CHANNEL_WARN;
      else if( !str_cmp( arg + 1, "order" ) )
         bit = CHANNEL_ORDER;
      else if( !str_cmp( arg + 1, "wartalk" ) )
         bit = CHANNEL_WARTALK;
      else if( !str_cmp( arg + 1, "whisper" ) )
         bit = CHANNEL_WHISPER;
      else if( !str_cmp( arg + 1, "racetalk" ) )
         bit = CHANNEL_RACETALK;
      else if( !str_cmp( arg + 1, "info" ) )
         bit = CHANNEL_INFO;
      else if( !str_cmp( arg + 1, "all" ) )
         ClearAll = TRUE;
      else
      {
         send_to_char( "Set or clear which channel?\n\r", ch );
         return;
      }

      if( ( fClear ) && ( ClearAll ) )
      {
         xREMOVE_BIT( ch->deaf, CHANNEL_RACETALK );
         xREMOVE_BIT( ch->deaf, CHANNEL_AUCTION );
         xREMOVE_BIT( ch->deaf, CHANNEL_CHAT );
         xREMOVE_BIT( ch->deaf, CHANNEL_QUEST );
         xREMOVE_BIT( ch->deaf, CHANNEL_WARTALK );
         xREMOVE_BIT( ch->deaf, CHANNEL_PRAY );
         xREMOVE_BIT( ch->deaf, CHANNEL_TRAFFIC );
         xREMOVE_BIT( ch->deaf, CHANNEL_IC );
         xREMOVE_BIT( ch->deaf, CHANNEL_MUSIC );
         xREMOVE_BIT( ch->deaf, CHANNEL_ASK );
         xREMOVE_BIT( ch->deaf, CHANNEL_SHOUT );
         xREMOVE_BIT( ch->deaf, CHANNEL_YELL );
         xREMOVE_BIT( ch->deaf, CHANNEL_INFO );
         xREMOVE_BIT( ch->deaf, CHANNEL_FLAME );
         xREMOVE_BIT( ch->deaf, CHANNEL_GRATZ );
         xREMOVE_BIT( ch->deaf, CHANNEL_MARRY );
         xREMOVE_BIT( ch->deaf, CHANNEL_TELLS );
         xREMOVE_BIT( ch->deaf, CHANNEL_WHISPER );
         xREMOVE_BIT( ch->deaf, CHANNEL_NEWBIE );
         xREMOVE_BIT( ch->deaf, CHANNEL_BUILD );
         xREMOVE_BIT( ch->deaf, CHANNEL_MONITOR );
         xREMOVE_BIT( ch->deaf, CHANNEL_COMM );
         xREMOVE_BIT( ch->deaf, CHANNEL_LOG );
         xREMOVE_BIT( ch->deaf, CHANNEL_HBUILD );
         xREMOVE_BIT( ch->deaf, CHANNEL_IMMTALK );
         xREMOVE_BIT( ch->deaf, CHANNEL_CODER );
         xREMOVE_BIT( ch->deaf, CHANNEL_AUTH );
         xREMOVE_BIT( ch->deaf, CHANNEL_WARN );
         xREMOVE_BIT( ch->deaf, CHANNEL_HIGH );
         xREMOVE_BIT( ch->deaf, CHANNEL_HIGHGOD );

         if( ch->pcdata->clan )
            xREMOVE_BIT( ch->deaf, CHANNEL_CLAN );
         if( ch->level >= LEVEL_AVATAR )
            xREMOVE_BIT( ch->deaf, CHANNEL_AVTALK );
         if( ch->level >= LEVEL_HERO )
            xREMOVE_BIT( ch->deaf, CHANNEL_HTALK );
         if( ch->level >= LEVEL_ACOLYTE )
            xREMOVE_BIT( ch->deaf, CHANNEL_ACOTALK );
         if( ch->level >= LEVEL_NEOPHYTE )
            xREMOVE_BIT( ch->deaf, CHANNEL_NEOTALK );


      }
      else if( ( !fClear ) && ( ClearAll ) )
      {
         xSET_BIT( ch->deaf, CHANNEL_RACETALK );
         xSET_BIT( ch->deaf, CHANNEL_AUCTION );
         xSET_BIT( ch->deaf, CHANNEL_TRAFFIC );
         xSET_BIT( ch->deaf, CHANNEL_IC );
         xSET_BIT( ch->deaf, CHANNEL_CHAT );
         xSET_BIT( ch->deaf, CHANNEL_QUEST );
         xSET_BIT( ch->deaf, CHANNEL_PRAY );
         xSET_BIT( ch->deaf, CHANNEL_MUSIC );
         xSET_BIT( ch->deaf, CHANNEL_ASK );
         xSET_BIT( ch->deaf, CHANNEL_SHOUT );
         xSET_BIT( ch->deaf, CHANNEL_WARTALK );
         xSET_BIT( ch->deaf, CHANNEL_YELL );
         xSET_BIT( ch->deaf, CHANNEL_INFO );
         xSET_BIT( ch->deaf, CHANNEL_FLAME );
         xSET_BIT( ch->deaf, CHANNEL_GRATZ );
         xSET_BIT( ch->deaf, CHANNEL_MARRY );
         xSET_BIT( ch->deaf, CHANNEL_TELLS );
         xSET_BIT( ch->deaf, CHANNEL_WHISPER );
         xSET_BIT( ch->deaf, CHANNEL_NEWBIE );
         xSET_BIT( ch->deaf, CHANNEL_BUILD );
         xSET_BIT( ch->deaf, CHANNEL_MONITOR );
         xSET_BIT( ch->deaf, CHANNEL_COMM );
         xSET_BIT( ch->deaf, CHANNEL_LOG );
         xSET_BIT( ch->deaf, CHANNEL_HBUILD );
         xSET_BIT( ch->deaf, CHANNEL_IMMTALK );
         xSET_BIT( ch->deaf, CHANNEL_CODER );
         xSET_BIT( ch->deaf, CHANNEL_AUTH );
         xSET_BIT( ch->deaf, CHANNEL_WARN );
         xSET_BIT( ch->deaf, CHANNEL_HIGH );
         xSET_BIT( ch->deaf, CHANNEL_HIGHGOD );

         if( ch->pcdata->clan )
            xSET_BIT( ch->deaf, CHANNEL_CLAN );
         if( ch->level >= LEVEL_AVATAR )
            xSET_BIT( ch->deaf, CHANNEL_AVTALK );
         if( ch->level >= LEVEL_HERO )
            xSET_BIT( ch->deaf, CHANNEL_HTALK );
         if( ch->level >= LEVEL_ACOLYTE )
            xSET_BIT( ch->deaf, CHANNEL_ACOTALK );
         if( ch->level >= LEVEL_NEOPHYTE )
            xSET_BIT( ch->deaf, CHANNEL_NEOTALK );


      }
      else if( fClear )
      {
         xREMOVE_BIT( ch->deaf, bit );
      }
      else
      {
         xSET_BIT( ch->deaf, bit );
      }

      if( !( ClearAll ) )
      {
         send_to_char( "Ok\n\r", ch );
      }
   }

   return;
}


void do_wizlist( CHAR_DATA * ch, char *argument )
{
   set_pager_color( AT_IMMORT, ch );
   show_file( ch, WIZLIST_FILE );
   send_to_char( "SEE ALSO: immranks\n\r", ch );
}

void do_immlogtime( CHAR_DATA * ch, char *argument )
{
   set_pager_color( AT_IMMORT, ch );
   show_file( ch, IMM_LOGTIME );
}

void do_config( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];

   if( IS_NPC( ch ) )
      return;

   one_argument( argument, arg );

   set_char_color( AT_GREEN, ch );

   if( arg[0] == '\0' )
   {
      send_to_char( "&YConfigurations&z:\n\r", ch );
      send_to_char( "&GSyntax&z:&g config <+/-> <keyword>\n\r\n\r", ch );
      send_to_char( "&RDisplay&z:\n\r", ch );
      ch_printf_color( ch, "&gPager        :%s\t\t", IS_SET( ch->pcdata->flags, PCFLAG_PAGERON ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gGag       :%s\t\t", IS_SET( ch->pcdata->flags, PCFLAG_GAG ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gBrief	   :%s\n\r", xIS_SET( ch->act, PLR_BRIEF ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gCombine      :%s\t\t", xIS_SET( ch->act, PLR_COMBINE ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gBlank     :%s\t\t", xIS_SET( ch->act, PLR_BLANK ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gPrompt     :%s\n\r", xIS_SET( ch->act, PLR_PROMPT ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gAnsi         :%s\t\t", xIS_SET( ch->act, PLR_ANSI ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gRIP       :%s\t\t", xIS_SET( ch->act, PLR_RIP ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gShortscore :%s\n\r", xIS_SET( ch->act, PLR_SCRESHRT ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gMidscore     :%s\t\t", xIS_SET( ch->act, PLR_MIDSCORE ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gCondition :%s\t\t", xIS_SET( ch->act, PLR_COND ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gScoreaffects :%s", xIS_SET( ch->act, PLR_SCREAFF ) ? " &GOn" : " &GOff" );
      send_to_char( "\n\r&RAutos&z:\n\r", ch );
      ch_printf_color( ch, "&gAutosac      :%s\t\t", xIS_SET( ch->act, PLR_AUTOSAC ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gAutogold  :%s\t\t", xIS_SET( ch->act, PLR_AUTOGOLD ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gAutoloot   :%s\n\r", xIS_SET( ch->act, PLR_AUTOLOOT ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gAutotitle    :%s\t\t", xIS_SET( ch->act, PLR_A_TITLE ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gAutoexit  :%s\t\t", xIS_SET( ch->act, PLR_AUTOEXIT ) ? " &GOn" : " &GOff" );
      send_to_char( "\n\r&RSafeties&z:\n\r", ch );
      ch_printf_color( ch, "&gNorecall     :%s\t\t", IS_SET( ch->pcdata->flags, PCFLAG_NORECALL ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gNosummon  :%s\t\t", IS_SET( ch->pcdata->flags, PCFLAG_NOSUMMON ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gNobeep     :%s\n\r", IS_SET( ch->pcdata->flags, PCFLAG_NOBEEP ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gNice         :%s\t\t", xIS_SET( ch->act, PLR_NICE ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gDrag      :%s\t\t", xIS_SET( ch->act, PLR_SHOVEDRAG ) ? " &GOn" : " &GOff" );
      send_to_char( "\n\r&RMisc&z:\n\r", ch );
      ch_printf_color( ch, "&gGroupwho     :%s\t\t", IS_SET( ch->pcdata->flags, PCFLAG_GROUPWHO ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gPosition  :%s\t\t", IS_SET( ch->pcdata->flags, PCFLAG_POSI ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gNointro    :%s\n\r", IS_SET( ch->pcdata->flags, PCFLAG_NOINTRO ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gTelnetGA     :%s\t\t", xIS_SET( ch->act, PLR_TELNET_GA ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gNobattle  :%s\t\t", IS_SET( ch->pcdata->flags, PCFLAG_NOBATTLESPAM ) ? " &GOn" : " &GOff" );
      ch_printf_color( ch, "&gImmalt     :%s\n\r", IS_SET( ch->pcdata->flags, PCFLAG_IMMALT ) ? " &GYes" : " &GNo" );
      ch_printf_color( ch, "&gPingme       :%s", xIS_SET( ch->act, PLR_PINGME ) ? " &GOn" : " &GOff" );
      if( IS_AGOD( ch ) )
      {
         send_to_char( "\n\r&RImmortal Settings&z:\n\r", ch );
         ch_printf_color( ch, "&gRoomvnum     :%s\t\t", xIS_SET( ch->act, PLR_ROOMVNUM ) ? " &GOn" : " &GOff" );
         ch_printf_color( ch, "&gAutomap   :%s\t\t", xIS_SET( ch->act, PLR_AUTOMAP ) ? " &GOn" : " &GOff" );
         ch_printf_color( ch, "&gBuilding   :%s\n\r", IS_SET( ch->pcdata->flags, PCFLAG_BUILDING ) ? " &GOn" : " &GOff" );
         ch_printf_color( ch, "&gRoomflags    :%s\t\t", IS_SET( ch->pcdata->flags, PCFLAG_AUTOFLAGS ) ? " &GOn" : " &GOff" );
         ch_printf_color( ch, "&gSectors   :%s\t\t", IS_SET( ch->pcdata->flags, PCFLAG_SECTORD ) ? " &GOn" : " &GOff" );
         ch_printf_color( ch, "&gFilename   :%s", IS_SET( ch->pcdata->flags, PCFLAG_ANAME ) ? " &GOn" : " &GOff" );
      }

      send_to_char( "\n\r&RSettings&z:\n\r", ch );
      ch_printf_color( ch, "&gPager Length &z(&G%d&z)    &gWimpy &z(&G%d&z)     &gDeaf &z[&G%c&z]",
                       ch->pcdata->pagerlen, ch->wimpy, xIS_SET( ch->act, PLR_DEAF ) ? 'X' : ' ' );

      set_char_color( AT_DGREEN, ch );
      send_to_char( "\n\r\n\rSentences imposed on you (if any):", ch );
      set_char_color( AT_YELLOW, ch );
      ch_printf( ch, "\n\r%s%s%s%s%s%s",
                 xIS_SET( ch->act, PLR_SILENCE ) ?
                 " For your abuse of channels, you are currently silenced.\n\r" : "",
                 xIS_SET( ch->act, PLR_NO_EMOTE ) ?
                 " The gods have removed your emotes.\n\r" : "",
                 xIS_SET( ch->act, PLR_NO_TELL ) ?
                 " You are not permitted to send 'tells' to others.\n\r" : "",
                 xIS_SET( ch->act, PLR_LITTERBUG ) ?
                 " A convicted litterbug.  You cannot drop anything.\n\r" : "",
                 xIS_SET( ch->act, PLR_THIEF ) ?
                 " A proven thief, you will be hunted by the authorities.\n\r" : "",
                 xIS_SET( ch->act, PLR_KILLER ) ? " For the crime of murder you are sentenced to death...\n\r" : "" );
   }
   else
   {
      bool fSet;
      int bit = 0;

      if( arg[0] == '+' )
         fSet = TRUE;
      else if( arg[0] == '-' )
         fSet = FALSE;
      else
      {
         send_to_char( "Config -option or +option?\n\r", ch );
         return;
      }

      if( !str_prefix( arg + 1, "autoloot" ) )
         bit = PLR_AUTOLOOT;
      else if( !str_prefix( arg + 1, "autosac" ) )
         bit = PLR_AUTOSAC;
      else if( !str_prefix( arg + 1, "autokatyr" ) )
         bit = PLR_AUTOGOLD;
      else if( !str_prefix( arg + 1, "autoexit" ) )
         bit = PLR_AUTOEXIT;
      else if( !str_prefix( arg + 1, "autotitle" ) )
         bit = PLR_A_TITLE;
      else if( !str_prefix( arg + 1, "condition" ) )
         bit = PLR_COND;
      else if( !str_prefix( arg + 1, "blank" ) )
         bit = PLR_BLANK;
      else if( !str_prefix( arg + 1, "brief" ) )
         bit = PLR_BRIEF;
      else if( !str_prefix( arg + 1, "combine" ) )
         bit = PLR_COMBINE;
      else if( !str_prefix( arg + 1, "prompt" ) )
         bit = PLR_PROMPT;
      else if( !str_prefix( arg + 1, "telnetga" ) )
         bit = PLR_TELNET_GA;
      else if( !str_prefix( arg + 1, "ansi" ) )
         bit = PLR_ANSI;
      else if( !str_prefix( arg + 1, "rip" ) )
         bit = PLR_RIP;
      else if( !str_prefix( arg + 1, "scoreaffects" ) )
         bit = PLR_SCREAFF;
      else if( !str_prefix( arg + 1, "pingme" ) )
         bit = PLR_PINGME;
      else if( !str_prefix( arg + 1, "affsaves" ) )
         bit = PLR_AFFSA;
      else if( !str_prefix( arg + 1, "shortscore" ) )
         bit = PLR_SCRESHRT;
      else if( !str_prefix( arg + 1, "midscore" ) )
         bit = PLR_MIDSCORE;
      else if( !str_prefix( arg + 1, "nice" ) )
         bit = PLR_NICE;
      else if( !str_prefix( arg + 1, "drag" ) )
         bit = PLR_SHOVEDRAG;
      else if( IS_IMMORTAL( ch ) && !str_prefix( arg + 1, "vnum" ) )
         bit = PLR_ROOMVNUM;
      else if( IS_IMMORTAL( ch ) && !str_prefix( arg + 1, "map" ) )
         bit = PLR_AUTOMAP;

      if( bit )
      {
         if( ( bit == PLR_FLEE || bit == PLR_NICE || bit == PLR_SHOVEDRAG ) && IS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) )
         {
            send_to_char( "Pkill characters can not config that option.\n\r", ch );
            return;
         }
         if( bit == PLR_PINGME )
         {
            send_to_char( "You cannot set this option at this time.\n\r", ch );
            return;
         }

         if( fSet )
         {
            xSET_BIT( ch->act, bit );
            if( xIS_SET( ch->act, PLR_SCRESHRT ) && bit == PLR_MIDSCORE )
            {
               xREMOVE_BIT( ch->act, PLR_SCRESHRT );
            }
            if( xIS_SET( ch->act, PLR_MIDSCORE ) && bit == PLR_SCRESHRT )
            {
               xREMOVE_BIT( ch->act, PLR_MIDSCORE );
            }
         }
         else
         {
            xREMOVE_BIT( ch->act, bit );
         }
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      else
      {
         if( !str_prefix( arg + 1, "norecall" ) )
            bit = PCFLAG_NORECALL;
         else if( !str_prefix( arg + 1, "nointro" ) )
            bit = PCFLAG_NOINTRO;
         else if( !str_prefix( arg + 1, "nosummon" ) )
            bit = PCFLAG_NOSUMMON;
         else if( !str_prefix( arg + 1, "nobeep" ) )
            bit = PCFLAG_NOBEEP;
         else if( !str_prefix( arg + 1, "nobattle" ) )
            bit = PCFLAG_NOBATTLESPAM;
         else if( !str_prefix( arg + 1, "gag" ) )
            bit = PCFLAG_GAG;
         else if( !str_prefix( arg + 1, "pager" ) )
            bit = PCFLAG_PAGERON;
         else if( IS_IMMORTAL( ch ) && ( !str_prefix( arg + 1, "roomflags" ) ) )
            bit = PCFLAG_AUTOFLAGS;
         else if( IS_IMMORTAL( ch ) && ( !str_prefix( arg + 1, "sectortypes" ) ) )
            bit = PCFLAG_SECTORD;
         else if( IS_IMMORTAL( ch ) && ( !str_prefix( arg + 1, "filename" ) ) )
            bit = PCFLAG_ANAME;
         else if( !str_prefix( arg + 1, "groupwho" ) )
            bit = PCFLAG_GROUPWHO;
         else if( !str_prefix( arg + 1, "position" ) )
            bit = PCFLAG_POSI;
         else if( !str_prefix( arg + 1, "@hgflag_" ) )
            bit = PCFLAG_HIGHGAG;
         else
         {
            send_to_char( "Config which option?\n\r", ch );
            return;
         }

         if( fSet )
            SET_BIT( ch->pcdata->flags, bit );
         else
            REMOVE_BIT( ch->pcdata->flags, bit );

         send_to_char( "Ok.\n\r", ch );
         return;
      }
   }

   return;
}


void do_credits( CHAR_DATA * ch, char *argument )
{
   do_help( ch, "credits" );
}


extern int top_area;

void do_areas( CHAR_DATA * ch, char *argument )
{
   char *header_string1 = "\n\r   &WAuthor    &p|             &wArea"
      "                     &p| " "&WLevel Range &p|  &zPlane\n\r";
   char *header_string2 = "&p-------------+-----------------" "---------------------+----" "---------+-----------&D\n\r";
   char *print_string = "&W%-12s &p| &w%-36s &p| &W%4d &p- &W%-4d &p| &z%-12s&D\n\r";

   char *footer_string = "&RTotal Areas: &Y%d&D\n\r";


   AREA_DATA *pArea;
   int lower_bound = 0;
   int upper_bound = MAX_LEVEL + 1;
   int num = 0;
   char arg[MAX_STRING_LENGTH];
   char pname[MAX_STRING_LENGTH];
   char plvl[MAX_STRING_LENGTH];

   argument = one_argument( argument, arg );

   if( arg[0] != '\0' )
   {
      if( !is_number( arg ) )
      {
         if( !strcmp( arg, "old" ) )
         {
            set_pager_color( AT_PLAIN, ch );
            send_to_pager( header_string1, ch );
            send_to_pager( header_string2, ch );
            for( pArea = first_area; pArea; pArea = pArea->next )
            {
               switch ( pArea->plane )
               {
                  default:
                     break;
                  case 1:
                     sprintf( pname, "Terra" );
                     break;
                  case 2:
                     sprintf( pname, "White" );
                     break;
                  case 3:
                     sprintf( pname, "Lorur" );
                     break;
                  case 4:
                     sprintf( pname, "Ophelia" );
                     break;
                  case 5:
                     sprintf( pname, "Utopia" );
                     break;
                  case 6:
                     sprintf( pname, "All" );
                     break;
                  case 7:
                     sprintf( pname, "Angel" );
                     break;
               }

               if( pArea->plane == 1 )
               {
                  pager_printf( ch, print_string,
                                pArea->author, pArea->name, pArea->low_soft_range, pArea->hi_soft_range, pname );
               }
               else
               {
                  switch ( pArea->plane )
                  {
                     default:
                        break;
                     case 2:
                        sprintf( plvl, "    &YHero   " );
                        break;
                     case 3:
                        sprintf( plvl, "   &CAvatar  " );
                        break;
                     case 4:
                        sprintf( plvl, "  &PNeophyte " );
                        break;
                     case 5:
                        sprintf( plvl, "  &RAcolyte  " );
                        break;
                     case 6:
                        sprintf( plvl, "    &BAll    " );
                        break;
                     case 7:
                        sprintf( plvl, "   &zAngel   " );
                        break;
                  }

                  pager_printf( ch, "&W%-12s &p| &w%-36s &p| %11s &p| &z%-12s&D\n\r",
                                pArea->author, pArea->name, plvl, pname );
               }
               num++;
            }
            pager_printf( ch, footer_string, num );
            return;
         }
         else
         {
            send_to_char( "Area may only be followed by numbers, or 'old'.\n\r", ch );
            return;
         }
      }

      upper_bound = atoi( arg );
      lower_bound = upper_bound;

      argument = one_argument( argument, arg );

      if( arg[0] != '\0' )
      {
         if( !is_number( arg ) )
         {
            send_to_char( "Area may only be followed by numbers.\n\r", ch );
            return;
         }

         upper_bound = atoi( arg );

         argument = one_argument( argument, arg );
         if( arg[0] != '\0' )
         {
            send_to_char( "Only two level numbers allowed.\n\r", ch );
            return;
         }
      }
   }

   if( lower_bound > upper_bound )
   {
      int swap = lower_bound;
      lower_bound = upper_bound;
      upper_bound = swap;
   }

   set_pager_color( AT_PLAIN, ch );
   send_to_pager( header_string1, ch );
   send_to_pager( header_string2, ch );

   for( pArea = first_area_name; pArea; pArea = pArea->next_sort_name )
   {
      if( pArea->hi_soft_range >= lower_bound && pArea->low_soft_range <= upper_bound )
      {
         switch ( pArea->plane )
         {
            default:
               break;
            case 1:
               sprintf( pname, "Terra" );
               break;
            case 2:
               sprintf( pname, "White" );
               break;
            case 3:
               sprintf( pname, "Lorur" );
               break;
            case 4:
               sprintf( pname, "Ophelia" );
               break;
            case 5:
               sprintf( pname, "Utopia" );
               break;
            case 6:
               sprintf( pname, "All" );
               break;
            case 7:
               sprintf( pname, "Angel" );
               break;
         }

         if( pArea->plane == 1 )
         {
            pager_printf( ch, print_string, pArea->author, pArea->name, pArea->low_soft_range, pArea->hi_soft_range, pname );
         }
         else
         {
            switch ( pArea->plane )
            {
               default:
                  break;
               case 2:
                  sprintf( plvl, "    &BHero   " );
                  break;
               case 3:
                  sprintf( plvl, "   &CAvatar  " );
                  break;
               case 4:
                  sprintf( plvl, "  &PNeophyte " );
                  break;
               case 5:
                  sprintf( plvl, "  &RAcolyte  " );
                  break;
               case 6:
                  sprintf( plvl, "    &YAll    " );
                  break;
               case 7:
                  sprintf( plvl, "   &zAngel   " );
                  break;
            }

            pager_printf( ch, "&W%-12s &p| &w%-36s &p| %11s &p| &z%-12s&D\n\r", pArea->author, pArea->name, plvl, pname );
         }
         num++;
      }
   }
   pager_printf( ch, footer_string, num );
   return;
}

void do_iaw( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) )
      return;

   if( xIS_SET( ch->act, PLR_IAW ) )
   {
      xREMOVE_BIT( ch->act, PLR_IAW );
      send_to_char( "You are no longer in another window.\n\r", ch );
      act( AT_GREY, "$n is no longer in another window.", ch, NULL, NULL, TO_CANSEE );
   }
   else
   {
      xSET_BIT( ch->act, PLR_IAW );
      send_to_char( "You are now in another window.\n\r", ch );
      act( AT_GREY, "$n is now in another window.", ch, NULL, NULL, TO_CANSEE );
      return;
   }

}


void do_afk( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) )
      return;

   if( xIS_SET( ch->act, PLR_AFK ) )
   {
      xREMOVE_BIT( ch->act, PLR_AFK );
      send_to_char( "You are no longer afk.\n\r", ch );
      act( AT_GREY, "$n is no longer afk.", ch, NULL, NULL, TO_CANSEE );
   }
   else
   {
      xSET_BIT( ch->act, PLR_AFK );
      send_to_char( "You are now afk.\n\r", ch );
      act( AT_GREY, "$n is now afk.", ch, NULL, NULL, TO_CANSEE );
      return;
   }

}

void do_idle( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) )
      return;

   if( !IS_AGOD( ch ) )
   {
      send_to_char( "You cannot go into an idle state.\n\r", ch );
      return;
   }

   if( xIS_SET( ch->act, PLR_IDLE ) )
   {
      xREMOVE_BIT( ch->act, PLR_IDLE );
      send_to_char( "You are no longer idling.\n\r", ch );
      act( AT_GREY, "$n is no longer idling.", ch, NULL, NULL, TO_CANSEE );
   }
   else
   {
      xSET_BIT( ch->act, PLR_IDLE );
      send_to_char( "You are now idling.\n\r", ch );
      act( AT_GREY, "$n is now idling.", ch, NULL, NULL, TO_CANSEE );
      return;
   }

}

void do_slist( CHAR_DATA * ch, char *argument )
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
   hilev = 99;

   set_pager_color( AT_MAGIC, ch );
   if( clas == -1 )
   {
      send_to_pager( "&WSPELL & SKILL LIST For Mortal Tier\n\r", ch );
   }
   else
   {
      sprintf( buf, "&WSPELL & SKILL LIST For Mortal Tier and class %s\n\r", class_table[clas]->who_name );
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
            if( i == find_skill_level( ch, sn ) )
            {
               if( !lFound )
               {
                  if( col % 3 != 0 )
                     send_to_pager( "\n\r", ch );
                  lFound = 1;
                  pager_printf( ch,
                                "&p--[ &zLevel &W%2d&p ]-----------------------------------------------------------------------\n\r",
                                i );
                  col = 0;
               }
               pager_printf( ch, "&G %20.20s ", skill_table[sn]->name );
               if( ++col % 3 == 0 )
                  send_to_pager( "&D\n\r", ch );

            }
         }
         else
         {
            if( i == find_skill_level_target( clas, sn ) )
            {
               if( !lFound )
               {
                  if( col % 3 != 0 )
                     send_to_pager( "\n\r", ch );
                  lFound = 1;
                  pager_printf( ch,
                                "&p--[ &zLevel &W%2d&p ]-----------------------------------------------------------------------\n\r",
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
   if( col % 3 != 0 )
      send_to_pager( "\n\r", ch );
   return;
}


void do_whois( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];
   char buf4[MAX_STRING_LENGTH];
   char buf5[MAX_STRING_LENGTH];
   char buf6[MAX_STRING_LENGTH];

   buf[0] = '\0';
   buf2[0] = '\0';
   buf3[0] = '\0';
   buf4[0] = '\0';
   buf5[0] = '\0';
   buf6[0] = '\0';

   if( IS_NPC( ch ) )
      return;

   if( argument[0] == '\0' )
   {
      send_to_pager( "You must input the name of an online character.\n\r", ch );
      return;
   }

   strcat( buf, "0." );
   strcat( buf, argument );
   if( ( ( victim = get_char_world( ch, buf ) ) == NULL ) )
   {
      send_to_pager( "No such character online.\n\r", ch );
      return;
   }

   if( IS_NPC( victim ) )
   {
      send_to_pager( "That's not a player!\n\r", ch );
      return;
   }

   if( !IS_AGOD( ch ) && get_trust( victim ) > 400 )
   {
      send_to_char( "The mists of time shroud your vision.\n\r", ch );
      return;
   }

   if( !IS_IMMORTAL( ch ) )
   {
      switch ( victim->level )
      {
         case 100:
            sprintf( buf4, "Hero" );
            break;
         case 200:
            sprintf( buf4, "Avat" );
            break;
         case 300:
            sprintf( buf4, "Neop" );
            break;
         case 400:
            sprintf( buf4, "Acol" );
            break;
         default:
            sprintf( buf4, "Mort" );
            break;
      }
      if( victim->level > 400 )
      {
         sprintf( buf4, "Immo" );
      }
      if( victim->pcdata->deity_name && victim->pcdata->deity_name[0] != '\0' )
      {
         sprintf( buf6, "%3.3s", victim->pcdata->deity_name );
      }
      else
      {
         sprintf( buf6, "   " );
      }
      if( victim->level < 100 )
      {
         sprintf( buf5, "&g[&G%2d", victim->level );
      }
      else
      {
         sprintf( buf5, "&g[&G%3d(%3d)", victim->level, victim->sublevel );
      }
      if( IS_DUAL( victim ) )
      {
         sprintf( buf, "%s %s %3.3s(%3.3s) %3.3s %s %s&g] %s%s\n\r", buf5, buf4,
                  class_table[victim->class]->who_name, class_table[victim->dualclass]->who_name,
                  capitalize( race_table[victim->race]->race_name ), buf6,
                  victim->sex == SEX_MALE ? "M" : "F",
                  IS_SNAME( victim ) ? victim->pcdata->sname : victim->name, victim->pcdata->title );
      }
      else
      {
         sprintf( buf, "%s %s %3.3s %3.3s %s %s&g] %s%s\n\r", buf5, buf4,
                  class_table[victim->class]->who_name, capitalize( race_table[victim->race]->race_name ),
                  buf6, victim->sex == SEX_MALE ? "M" : "F",
                  IS_SNAME( victim ) ? victim->pcdata->sname : victim->name, victim->pcdata->title );
      }
      pager_printf( ch, buf );
      return;
   }
   else
   {
      switch ( victim->level )
      {
         case 100:
            sprintf( buf4, "Hero" );
            break;
         case 200:
            sprintf( buf4, "Avat" );
            break;
         case 300:
            sprintf( buf4, "Neop" );
            break;
         case 400:
            sprintf( buf4, "Acol" );
            break;
         default:
            sprintf( buf4, "Mort" );
            break;
      }
      if( victim->level > 400 )
      {
         sprintf( buf4, "Immo" );
      }
      if( victim->pcdata->deity_name && victim->pcdata->deity_name[0] != '\0' )
      {
         sprintf( buf6, "%3.3s", victim->pcdata->deity_name );
      }
      else
      {
         sprintf( buf6, "   " );
      }
      if( victim->level < 100 )
      {
         sprintf( buf5, "&g[&G%2d", victim->level );
      }
      else
      {
         sprintf( buf5, "&g[&G%3d(%3d)", victim->level, victim->sublevel );
      }
      if( IS_DUAL( victim ) )
      {
         sprintf( buf, "%s %s %3.3s(%3.3s) %3.3s %s %s&g] %s%s\n\r", buf5, buf4,
                  class_table[victim->class]->who_name, class_table[victim->dualclass]->who_name,
                  capitalize( race_table[victim->race]->race_name ), buf6,
                  victim->sex == SEX_MALE ? "M" : "F",
                  IS_SNAME( victim ) ? victim->pcdata->sname : victim->name, victim->pcdata->title );
      }
      else
      {
         sprintf( buf, "%s %s %3.3s %3.3s %s %s&g] %s%s\n\r", buf5, buf4,
                  class_table[victim->class]->who_name, capitalize( race_table[victim->race]->race_name ),
                  buf6, victim->sex == SEX_MALE ? "M" : "F",
                  IS_SNAME( victim ) ? victim->pcdata->sname : victim->name, victim->pcdata->title );
      }
      pager_printf( ch, buf );

      pager_printf( ch, "&W%s has killed %d mobiles, and been killed by a mobile %d times.&D\n\r",
                    victim->name, victim->pcdata->mkills, victim->pcdata->mdeaths );
      if( victim->pcdata->pkills || victim->pcdata->pdeaths )
         pager_printf( ch, "&W%s has killed %d players, and been killed by a player %d times.&D\n\r",
                       victim->name, victim->pcdata->pkills, victim->pcdata->pdeaths );
      if( victim->pcdata->illegal_pk )
         pager_printf( ch, "&W%s has committed %d illegal player kills.&D\n\r", victim->name, victim->pcdata->illegal_pk );
      if( victim->pcdata->email )
         pager_printf( ch, "&W%s has set their e-mail to %s.&D\n\r", victim->name, victim->pcdata->email );

      pager_printf( ch, "&W%s is %shelled at the moment.&D\n\r",
                    victim->name, ( victim->pcdata->release_date == 0 ) ? "not " : "" );

      if( victim->pcdata->nuisance )
      {
         pager_printf_color( ch, "&RNuisance   &cStage: (&R%d&c/%d)  Power:  &w%d  &cTime:  &w%s.&D\n\r",
                             victim->pcdata->nuisance->flags, MAX_NUISANCE_STAGE, victim->pcdata->nuisance->power,
                             ctime( &victim->pcdata->nuisance->time ) );
      }
      if( victim->pcdata->release_date != 0 )
         pager_printf( ch, "&W%s was helled by %s, and will be released on %24.24s.&D\n\r",
                       victim->sex == SEX_MALE ? "He" : "She",
                       victim->pcdata->helled_by, ctime( &victim->pcdata->release_date ) );

      if( xIS_SET( victim->act, PLR_SILENCE ) || xIS_SET( victim->act, PLR_NO_EMOTE )
          || xIS_SET( victim->act, PLR_NO_TELL ) || xIS_SET( victim->act, PLR_THIEF ) || xIS_SET( victim->act, PLR_KILLER ) )
      {
         sprintf( buf2, "&WThis player has the following flags set:" );
         if( xIS_SET( victim->act, PLR_SILENCE ) )
            strcat( buf2, " silence" );
         if( xIS_SET( victim->act, PLR_NO_EMOTE ) )
            strcat( buf2, " noemote" );
         if( xIS_SET( victim->act, PLR_NO_TELL ) )
            strcat( buf2, " notell" );
         if( xIS_SET( victim->act, PLR_THIEF ) )
            strcat( buf2, " thief" );
         if( xIS_SET( victim->act, PLR_KILLER ) )
            strcat( buf2, " killer" );
         strcat( buf2, ".&D\n\r" );
         send_to_pager( buf2, ch );
      }
      if( victim->desc && victim->desc->host[0] != '\0' )
      {
         sprintf( buf2, "&W%s's IP info: %s ", victim->name, victim->desc->host );
         if( get_trust( ch ) >= LEVEL_BUILD )
            strcat( buf2, victim->desc->user );
         strcat( buf2, "&D\n\r" );
         send_to_pager( buf2, ch );
      }
      if( IS_ADMIN( ch ) )
      {
         sprintf( buf3, "&W%s's password is %s.&D\n\r", victim->name, victim->pcdata->pwd );
         send_to_pager( buf3, ch );
      }
      return;
   }
   /*
    * set_pager_color( AT_GREY, ch );
    * if ( victim->level < 100 )
    * {
    * if ( !IS_DUAL(victim) )
    * {
    * pager_printf(ch, "\n\r'%s %s%s.&w'\n\r %s is a %s level %d %s %s, %d years of age.\n\r",
    * victim->pcdata->pretit,
    * victim->name,
    * victim->pcdata->title,
    * victim->sex == SEX_MALE ? "He" : "She",
    * victim->sex == SEX_MALE ? "male" : "female",
    * victim->level,
    * capitalize(race_table[victim->race]->race_name),
    * class_table[victim->class]->who_name,
    * get_age(victim) );
    * }
    * else
    * {
    * pager_printf(ch, "\n\r'%s %s%s.&w'\n\r %s is a %s level %d %s %s %s, %d years of age.\n\r",
    * victim->pcdata->pretit,
    * victim->name,
    * victim->pcdata->title,
    * victim->sex == SEX_MALE ? "He" : "She",
    * victim->sex == SEX_MALE ? "male" : "female",
    * victim->level, 
    * capitalize(race_table[victim->race]->race_name),
    * class_table[victim->class]->who_name,
    * class_table[victim->dualclass]->who_name,
    * get_age(victim) );
    * }
    * }
    * else
    * {
    * if ( !IS_DUAL(victim) )
    * {
    * pager_printf(ch, "\n\r'%s %s%s.&w'\n\r %s is a %s level %d(%d) %s %s, %d years of age.\n\r",
    * victim->pcdata->pretit,
    * victim->name,
    * victim->pcdata->title,
    * victim->sex == SEX_MALE ? "He" : "She",
    * victim->sex == SEX_MALE ? "male" : "female",
    * victim->level,
    * HAS_AWARD(victim),
    * capitalize(race_table[victim->race]->race_name),
    * class_table[victim->class]->who_name,
    * get_age(victim) );
    * }
    * else
    * {
    * pager_printf(ch, "\n\r'%s %s%s.&w'\n\r %s is a %s level %d(%d) %s %s %s, %d years of age.\n\r",
    * victim->pcdata->pretit,
    * victim->name,
    * victim->pcdata->title,
    * victim->sex == SEX_MALE ? "He" : "She",
    * victim->sex == SEX_MALE ? "male" : "female",
    * victim->level, 
    * HAS_AWARD(victim),
    * capitalize(race_table[victim->race]->race_name),
    * class_table[victim->class]->who_name,
    * class_table[victim->dualclass]->who_name,
    * get_age(victim) );
    * }
    * }
    * 
    * if ( victim->pcdata->spouse )
    * {
    * pager_printf(ch, " %s is married to %s.\n\r", 
    * victim->sex == SEX_MALE ? "He" : "She", victim->pcdata->spouse );
    * }
    * 
    * if ( victim->pcdata->email )
    * {
    * pager_printf(ch, " %s e-mail is %s.\n\r", 
    * victim->sex == SEX_MALE ? "His" : "Her", victim->pcdata->email );
    * }
    * 
    * pager_printf(ch, " %s is a %sdeadly player",
    * victim->sex == SEX_MALE ? "He" : "She",
    * IS_SET(victim->pcdata->flags, PCFLAG_DEADLY) ? "" : "non-");
    * 
    * if ( victim->pcdata->clan )
    * {
    * if ( victim->pcdata->clan->clan_type == CLAN_ORDER )
    * send_to_pager( ", and belongs to the Order ", ch );
    * else
    * if ( victim->pcdata->clan->clan_type == CLAN_GUILD )
    * send_to_pager( ", and belongs to the ", ch );
    * else
    * send_to_pager( ", and belongs to Clan ", ch );
    * send_to_pager( victim->pcdata->clan->name, ch );
    * }
    * send_to_pager( ".\n\r", ch );
    * 
    * if(victim->pcdata->council)
    * pager_printf(ch, " %s holds a seat on:  %s\n\r",
    * victim->sex == SEX_MALE ? "He" : "She",
    * victim->pcdata->council->name );
    * 
    * if(victim->pcdata->deity)
    * pager_printf(ch, " %s has found succor in the deity %s.\n\r",
    * victim->sex == SEX_MALE ? "He" : "She",
    * victim->pcdata->deity->name);
    * 
    * if(victim->pcdata->homepage && victim->pcdata->homepage[0] != '\0')
    * pager_printf(ch, " %s homepage can be found at %s\n\r", 
    * victim->sex == SEX_MALE ? "He" : "She",
    * show_tilde( victim->pcdata->homepage ) );
    * 
    * if(victim->pcdata->bio && victim->pcdata->bio[0] != '\0')
    * pager_printf(ch, " %s's personal bio:\n\r%s",
    * victim->name,
    * victim->pcdata->bio);
    * else
    * pager_printf(ch, " %s has yet to create a bio.\n\r",
    * victim->name );
    * 
    * if(IS_IMMORTAL(ch))
    * {  
    * send_to_pager("&G-------------------\n\r", ch);
    * send_to_pager("Info for immortals:\n\r", ch);
    * 
    * if ( victim->pcdata->authed_by && victim->pcdata->authed_by[0] != '\0' )
    * pager_printf(ch, "%s was authorized by %s.\n\r",
    * victim->name, victim->pcdata->authed_by );
    * 
    * pager_printf(ch, "%s has killed %d mobiles, and been killed by a mobile %d times.\n\r",
    * victim->name, victim->pcdata->mkills, victim->pcdata->mdeaths );
    * if ( victim->pcdata->pkills || victim->pcdata->pdeaths )
    * pager_printf(ch, "%s has killed %d players, and been killed by a player %d times.\n\r",
    * victim->name, victim->pcdata->pkills, victim->pcdata->pdeaths );
    * if ( victim->pcdata->illegal_pk )
    * pager_printf(ch, "%s has committed %d illegal player kills.\n\r",
    * victim->name, victim->pcdata->illegal_pk );
    * 
    * pager_printf(ch, "%s is %shelled at the moment.\n\r",
    * victim->name, 
    * (victim->pcdata->release_date == 0) ? "not " : "");
    * 
    * if (victim->pcdata->nuisance )
    * {
    * pager_printf_color( ch, "&RNuisance   &cStage: (&R%d&c/%d)  Power:  &w%d  &cTime:  &w%s.\n\r", victim->pcdata->nuisance->flags,
    * MAX_NUISANCE_STAGE, victim->pcdata->nuisance->power,
    * ctime(&victim->pcdata->nuisance->time));
    * }
    * if(victim->pcdata->release_date != 0)
    * pager_printf(ch, "%s was helled by %s, and will be released on %24.24s.\n\r",
    * victim->sex == SEX_MALE ? "He" : "She",
    * victim->pcdata->helled_by,
    * ctime(&victim->pcdata->release_date));
    * 
    * if(get_trust(victim) < get_trust(ch))
    * {
    * sprintf(buf2, "list %s", buf);
    * do_comment(ch, buf2);
    * }
    * 
    * if(xIS_SET(victim->act, PLR_SILENCE) || xIS_SET(victim->act, PLR_NO_EMOTE) 
    * || xIS_SET(victim->act, PLR_NO_TELL) || xIS_SET(victim->act, PLR_THIEF) 
    * || xIS_SET(victim->act, PLR_KILLER) )
    * {
    * sprintf(buf2, "This player has the following flags set:");
    * if(xIS_SET(victim->act, PLR_SILENCE)) 
    * strcat(buf2, " silence");
    * if(xIS_SET(victim->act, PLR_NO_EMOTE)) 
    * strcat(buf2, " noemote");
    * if(xIS_SET(victim->act, PLR_NO_TELL) )
    * strcat(buf2, " notell");
    * if(xIS_SET(victim->act, PLR_THIEF) )
    * strcat(buf2, " thief");
    * if(xIS_SET(victim->act, PLR_KILLER) )
    * strcat(buf2, " killer");
    * strcat(buf2, ".\n\r");
    * send_to_pager(buf2, ch);
    * }
    * if ( victim->desc && victim->desc->host[0]!='\0' ) 
    * {
    * sprintf (buf2, "%s's IP info: %s ", victim->name, victim->desc->host);
    * if (get_trust(ch) >= LEVEL_BUILD)
    * strcat (buf2, victim->desc->user);
    * strcat (buf2, "\n\r");
    * send_to_pager(buf2, ch);
    * }
    * }
    * if ( IS_ADMIN(ch) )
    * {
    * sprintf( buf3, "%s's password is %s.\n\r", victim->name, victim->pcdata->pwd );
    * send_to_pager( buf3, ch );
    * }
    */
}

void do_pager( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];

   if( IS_NPC( ch ) )
      return;
   set_char_color( AT_NOTE, ch );
   argument = one_argument( argument, arg );
   if( !*arg )
   {
      if( IS_SET( ch->pcdata->flags, PCFLAG_PAGERON ) )
      {
         send_to_char( "Pager disabled.\n\r", ch );
         do_config( ch, "-pager" );
      }
      else
      {
         ch_printf( ch, "Pager is now enabled at %d lines.\n\r", ch->pcdata->pagerlen );
         do_config( ch, "+pager" );
      }
      return;
   }
   if( !is_number( arg ) )
   {
      send_to_char( "Set page pausing to how many lines?\n\r", ch );
      return;
   }
   ch->pcdata->pagerlen = atoi( arg );
   if( ch->pcdata->pagerlen < 5 )
      ch->pcdata->pagerlen = 5;
   ch_printf( ch, "Page pausing set to %d lines.\n\r", ch->pcdata->pagerlen );
   return;
}

void do_ignore( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   IGNORE_DATA *temp, *next;
   char fname[1024];
   struct stat fst;
   CHAR_DATA *victim;

   if( IS_NPC( ch ) )
      return;

   argument = one_argument( argument, arg );

   sprintf( fname, "%s%c/%s", PLAYER_DIR, tolower( arg[0] ), capitalize( arg ) );

   victim = NULL;

   if( arg[0] == '\0' )
   {
      set_char_color( AT_DIVIDER, ch );
      ch_printf( ch, "\n\r----------------------------------------\n\r" );
      set_char_color( AT_DGREEN, ch );
      ch_printf( ch, "You are currently ignoring:\n\r" );
      set_char_color( AT_DIVIDER, ch );
      ch_printf( ch, "----------------------------------------\n\r" );
      set_char_color( AT_IGNORE, ch );

      if( !ch->pcdata->first_ignored )
      {
         ch_printf( ch, "\t    no one\n\r" );
         return;
      }

      for( temp = ch->pcdata->first_ignored; temp; temp = temp->next )
      {
         ch_printf( ch, "\t  - %s\n\r", temp->name );
      }

      return;
   }
   else if( !strcmp( arg, "none" ) )
   {
      for( temp = ch->pcdata->first_ignored; temp; temp = next )
      {
         next = temp->next;
         UNLINK( temp, ch->pcdata->first_ignored, ch->pcdata->last_ignored, next, prev );
         STRFREE( temp->name );
         DISPOSE( temp );
      }

      set_char_color( AT_IGNORE, ch );
      ch_printf( ch, "You now ignore no one.\n\r" );

      return;
   }
   else if( !strcmp( arg, "self" ) || nifty_is_name( arg, ch->name ) )
   {
      set_char_color( AT_IGNORE, ch );
      ch_printf( ch, "Did you type something?\n\r" );
      return;
   }
   else
   {
      int i;

      if( !strcmp( arg, "reply" ) )
      {
         if( !ch->reply )
         {
            set_char_color( AT_IGNORE, ch );
            ch_printf( ch, "They're not here.\n\r" );
            return;
         }
         else
         {
            strcpy( arg, ch->reply->name );
         }
      }

      for( temp = ch->pcdata->first_ignored, i = 0; temp; temp = temp->next, i++ )
      {
         if( !strcmp( temp->name, capitalize( arg ) ) )
         {
            UNLINK( temp, ch->pcdata->first_ignored, ch->pcdata->last_ignored, next, prev );
            set_char_color( AT_IGNORE, ch );
            ch_printf( ch, "You no longer ignore %s.\n\r", temp->name );
            STRFREE( temp->name );
            DISPOSE( temp );
            return;
         }
      }

      if( stat( fname, &fst ) == -1 &&
          ( !( victim = get_char_world( ch, arg ) ) || IS_NPC( victim ) || strcmp( capitalize( arg ), victim->name ) != 0 ) )
      {
         set_char_color( AT_IGNORE, ch );
         ch_printf( ch, "No player exists by that" " name.\n\r" );
         return;
      }

      if( victim )
      {
         strcpy( capitalize( arg ), victim->name );
      }

      if( i < MAX_IGN )
      {
         IGNORE_DATA *new;
         CREATE( new, IGNORE_DATA, 1 );
         new->name = STRALLOC( capitalize( arg ) );
         new->next = NULL;
         new->prev = NULL;
         LINK( new, ch->pcdata->first_ignored, ch->pcdata->last_ignored, next, prev );
         set_char_color( AT_IGNORE, ch );
         ch_printf( ch, "You now ignore %s.\n\r", new->name );
         return;
      }
      else
      {
         set_char_color( AT_IGNORE, ch );
         ch_printf( ch, "You may only ignore %d players.\n\r", MAX_IGN );
         return;
      }
   }
}

bool is_ignoring( CHAR_DATA * ch, CHAR_DATA * ign_ch )
{
   IGNORE_DATA *temp;

   if( IS_NPC( ch ) || IS_NPC( ign_ch ) )
      return FALSE;

   if( ch->pcdata->first_ignored )
      for( temp = ch->pcdata->first_ignored; temp; temp = temp->next )
      {
         if( nifty_is_name( temp->name, ign_ch->name ) )
            return TRUE;
      }

   return FALSE;
}

void do_version( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_INPUT_LENGTH];

   if( IS_NPC( ch ) )
      return;

   sprintf( buf, "%s is running on:", sysdata.mud_name );

   set_char_color( AT_YELLOW, ch );
   ch_printf( ch, "%s\n\r", buf );
   ch_printf( ch, "&Y%s version &w%s&R.&w%s ", Ke_CODEBASE_NAME, Ke_VERSION_MAJOR, Ke_VERSION_MINOR );
   ch_printf( ch, "&Ybuild &w%d\n\r", Ke_BUILD );
   ch_printf( ch, "&YCompiled on &w%s &Yat &w%s&Y.\n\r", __DATE__, __TIME__ );

   return;
}

void do_copyright( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) )
      return;

   ch_printf( ch, "&Y%s version &w%s&R.&w%s ", Ke_CODEBASE_NAME, Ke_VERSION_MAJOR, Ke_VERSION_MINOR );
   ch_printf( ch, "&Ybuild &w%d\n\r", Ke_BUILD );
   ch_printf( ch, "&WCopyright © 2003 Kara Enterprises.\n\r" );

   return;
}

void do_recall( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   ROOM_INDEX_DATA *location;
   CHAR_DATA *opponent;

   location = NULL;

   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      if( !IS_NPC( ch ) && !location )
      {
         if( ch->in_room->area->plane == 1 || ch->in_room->area->plane == 6 || ch->in_room->area->plane == 7 )
            location = get_room_index( ch->pcdata->recall );
         if( ch->in_room->area->plane == 2 )
            location = get_room_index( ROOM_VNUM_WHITE );
         if( ch->in_room->area->plane == 3 )
            location = get_room_index( ROOM_VNUM_LORUR );
         if( ch->in_room->area->plane == 4 )
            location = get_room_index( ROOM_VNUM_OPHELIA );
         if( ch->in_room->area->plane == 5 )
            location = get_room_index( ROOM_VNUM_UTOPIA );
      }

      if( ch->in_room == location )
      {
         send_to_char( "You are at your recall!\n\r", ch );
         return;
      }

      if( xIS_SET( ch->in_room->room_flags, ROOM_NO_RECALL ) )
      {
         send_to_char( "&RThis ground is cursed, you cannot recall!&D\n\r", ch );
         return;
      }

      if( IS_AFFECTED( ch, AFF_CURSE ) )
      {
         send_to_char( "You are cursed and cannot recall!\n\r", ch );
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
         ch_printf( ch, "You recall from combat!  You lose %d exps.\n\r", lose );
         stop_fighting( ch, TRUE );
      }

      ch->move -= ch->move / 4;
      act( AT_ACTION, "$n disappears in a swirl of smoke.", ch, NULL, NULL, TO_ROOM );
      char_from_room( ch );
      char_to_room( ch, location );
      if( ch->mount )
      {
         char_from_room( ch->mount );
         char_to_room( ch->mount, location );
      }
      if( ch->on )
      {
         ch->on = NULL;
         ch->position = POS_STANDING;
      }
      if( ch->position != POS_STANDING )
      {
         ch->position = POS_STANDING;
      }
      act( AT_ACTION, "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
      do_look( ch, "auto" );
      return;
   }
   else if( !str_cmp( arg, "reset" ) )
   {
      if( ch->in_room->area->plane != 1 )
      {
         send_to_char( "You cannot reset recall on this plane.\n\r", ch );
         return;
      }
      ch->pcdata->recall = ROOM_VNUM_D_RECALL;
      send_to_char( "Your recall has been reset.\n\r", ch );
      send_to_char( "Type recall set to set it to the current room you are in.\n\r", ch );
      return;
   }
   else if( !str_cmp( arg, "set" ) )
   {
      if( ch->in_room->area->plane != 1 )
      {
         send_to_char( "You cannot set recall on this plane.\n\r", ch );
         return;
      }
      if( xIS_SET( ch->in_room->room_flags, ROOM_NORECALLSET ) )
      {
         send_to_char( "You cannot set recall in this room.\n\r", ch );
         return;
      }
      ch->pcdata->recall = ch->in_room->vnum;
      send_to_char( "Your recall has been set.\n\r", ch );
      send_to_char( "Type recall reset to reset it to the default recall.\n\r", ch );
      return;
   }
   else
   {
      send_to_char( "Syntax: recall <set/reset>\n\r", ch );
      return;
   }
}

void do_classes( CHAR_DATA * ch, char *argument )
{
   int iClass = 0;

   send_to_pager( "\n\r\n\r", ch );
   send_to_pager( "&C&w-=-&C&r=-=&C&w-=-&C&r=&C&w( &C&WClasses &C&w)&C&r=&C&w-=-&C&r=-=&C&w-=-&C&w", ch );
   send_to_pager( "\n\r", ch );
   for( iClass = 0; iClass < MAX_PC_CLASS; iClass++ )
   {
      pager_printf( ch, "&C&w  %d %s\n\r", iClass, class_table[iClass]->who_name );
   }
   return;
}

void do_races( CHAR_DATA * ch, char *argument )
{
   int iRace = 0;

   send_to_pager( "\n\r\n\r", ch );
   send_to_pager( "&C&w-=-&C&r=-=&C&w-=-&C&r=&C&w( &C&WRaces &C&w)&C&r=&C&w-=-&C&r=-=&C&w-=-&C&w", ch );
   send_to_pager( "\n\r", ch );
   for( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
   {
      pager_printf( ch, "&C&w  %d %s\n\r", iRace, race_table[iRace]->race_name );
   }
   return;
}


void do_train( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   int hp_gain = 0;
   int mana_gain = 0;
   int move_gain = 0;
   sh_int *pAbility;
   char *pOutput;
   int cost = 0;
   int coststr = 0;
   int costint = 0;
   int costwis = 0;
   int costdex = 0;
   int costcha = 0;
   int costlck = 0;
   int costcon = 0;
   int pstr = 0;
   int pint = 0;
   int pwis = 0;
   int pdex = 0;
   int pcha = 0;
   int plck = 0;
   int pcon = 0;
   int dpstr = 0;
   int dpint = 0;
   int dpwis = 0;
   int dpdex = 0;
   int dpcha = 0;
   int dplck = 0;
   int dpcon = 0;
   int mpstr = 0;
   int mpint = 0;
   int mpwis = 0;
   int mpdex = 0;
   int mpcha = 0;
   int mplck = 0;
   int mpcon = 0;
   int m = 0;

   if( IS_NPC( ch ) )
      return;

   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      dpstr = ch->pcdata->deity->strpls;
      dpint = ch->pcdata->deity->intpls;
      dpwis = ch->pcdata->deity->wispls;
      dpdex = ch->pcdata->deity->dexpls;
      dpcha = ch->pcdata->deity->chapls;
      dplck = ch->pcdata->deity->lckpls;
      dpcon = ch->pcdata->deity->conpls;
   }

   if( argument[0] == '\0' )
   {
      sprintf( buf, "You have %d practice sessions.\n\r", ch->practice );
      argument = "foo";
   }

   cost = 5;
   if( class_table[ch->class]->attr_prime == APPLY_STR )
      pstr = 3;
   else
      pstr = 0;

   if( class_table[ch->class]->attr_prime == APPLY_INT )
      pint = 3;
   else
      pint = 0;

   if( class_table[ch->class]->attr_prime == APPLY_WIS )
      pwis = 3;
   else
      pwis = 0;

   if( class_table[ch->class]->attr_prime == APPLY_DEX )
      pdex = 3;
   else
      pdex = 0;

   if( class_table[ch->class]->attr_prime == APPLY_CON )
      pcon = 3;
   else
      pcon = 0;

   if( class_table[ch->class]->attr_prime == APPLY_CHA )
      pcha = 3;
   else
      pcha = 0;

   if( class_table[ch->class]->attr_prime == APPLY_LCK )
      plck = 3;
   else
      plck = 0;

   mpstr = ( 18 + race_table[ch->race]->str_plus + pstr + dpstr );
   mpint = ( 18 + race_table[ch->race]->int_plus + pint + dpint );
   mpwis = ( 18 + race_table[ch->race]->wis_plus + pwis + dpwis );
   mpdex = ( 18 + race_table[ch->race]->dex_plus + pdex + dpdex );
   mpcha = ( 18 + race_table[ch->race]->cha_plus + pcha + dpcha );
   mplck = ( 18 + race_table[ch->race]->lck_plus + plck + dplck );
   mpcon = ( 18 + race_table[ch->race]->con_plus + pcon + dpcon );

   if( !str_cmp( argument, "hp" ) )
   {
      pOutput = "number of hit points";
      cost = class_table[ch->class]->hpcost;
      hp_gain = 1;
      if( cost > ch->practice )
      {
         send_to_char( "You don't have enough practices.\n\r", ch );
         return;
      }
      if( ch->max_hit == sysdata.max_hp )
      {
         send_to_char( "You can't train any more hp.\n\r", ch );
         return;
      }
      ch->practice -= cost;
      ch->max_hit += hp_gain;
      act( AT_RED, "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
      act( AT_RED, "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
      return;
   }

   if( !str_cmp( argument, "move" ) )
   {
      pOutput = "amount of movement";
      cost = class_table[ch->class]->movecost;
      move_gain = 1;
      if( cost > ch->practice )
      {
         send_to_char( "You don't have enough practices.\n\r", ch );
         return;
      }
      if( ch->max_move == sysdata.max_move )
      {
         send_to_char( "You can't train any more moves.\n\r", ch );
         return;
      }
      ch->practice -= cost;
      ch->max_move += move_gain;
      act( AT_RED, "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
      act( AT_RED, "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
      return;
   }

   if( !str_cmp( argument, "mana" ) )
   {
      pOutput = "amount of mana";
      cost = class_table[ch->class]->manacost;
      mana_gain = 1;
      if( cost > ch->practice )
      {
         send_to_char( "You don't have enough practices.\n\r", ch );
         return;
      }
      if( ch->max_mana == sysdata.max_mana )
      {
         send_to_char( "You can't train any more mana.\n\r", ch );
         return;
      }
      ch->practice -= cost;
      ch->max_mana += mana_gain;
      act( AT_RED, "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
      act( AT_RED, "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
      return;
   }

   if( !str_cmp( argument, "blood" ) )
   {
      pOutput = "amount of blood";
      cost = class_table[ch->class]->manacost;
      mana_gain = 1;
      if( cost > ch->practice )
      {
         send_to_char( "You don't have enough practices.\n\r", ch );
         return;
      }
      if( ch->max_blood == sysdata.max_mana )
      {
         send_to_char( "You can't train any more blood.\n\r", ch );
         return;
      }
      ch->practice -= cost;
      ch->max_blood += mana_gain;
      act( AT_RED, "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
      act( AT_RED, "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
      return;
   }

   if( !str_cmp( argument, "str" ) )
   {
      if( class_table[ch->class]->attr_prime == APPLY_STR )
      {
         cost = 3;
         pstr = 3;
      }
      else
      {
         pstr = 0;
      }
      m = ( 18 + race_table[ch->race]->str_plus + pstr + dpstr );
      pAbility = &ch->perm_str;
      pOutput = "strength";
   }
   else if( !str_cmp( argument, "int" ) )
   {
      if( class_table[ch->class]->attr_prime == APPLY_INT )
      {
         cost = 3;
         pint = 3;
      }
      else
      {
         pint = 0;
      }
      m = ( 18 + race_table[ch->race]->int_plus + pint + dpint );
      pAbility = &ch->perm_int;
      pOutput = "intelligence";
   }
   else if( !str_cmp( argument, "wis" ) )
   {
      if( class_table[ch->class]->attr_prime == APPLY_WIS )
      {
         cost = 3;
         pwis = 3;
      }
      else
      {
         pwis = 0;
      }
      m = ( 18 + race_table[ch->race]->wis_plus + pwis + dpwis );
      pAbility = &ch->perm_wis;
      pOutput = "wisdom";
   }
   else if( !str_cmp( argument, "dex" ) )
   {
      if( class_table[ch->class]->attr_prime == APPLY_DEX )
      {
         cost = 3;
         pdex = 3;
      }
      else
      {
         pdex = 0;
      }
      m = ( 18 + race_table[ch->race]->dex_plus + pdex + dpdex );
      pAbility = &ch->perm_dex;
      pOutput = "dexterity";
   }
   else if( !str_cmp( argument, "con" ) )
   {
      if( class_table[ch->class]->attr_prime == APPLY_CON )
      {
         cost = 3;
         pcon = 3;
      }
      else
      {
         pcon = 0;
      }
      m = ( 18 + race_table[ch->race]->con_plus + pcon + dpcon );
      pAbility = &ch->perm_con;
      pOutput = "constitution";
   }
   else if( !str_cmp( argument, "cha" ) )
   {
      if( class_table[ch->class]->attr_prime == APPLY_CHA )
      {
         cost = 3;
         pcha = 3;
      }
      else
      {
         pcha = 0;
      }
      m = ( 18 + race_table[ch->race]->cha_plus + pcha + dpcha );
      pAbility = &ch->perm_cha;
      pOutput = "charisma";
   }
   else if( !str_cmp( argument, "lck" ) )
   {
      if( class_table[ch->class]->attr_prime == APPLY_LCK )
      {
         cost = 3;
         plck = 3;
      }
      else
      {
         plck = 0;
      }
      m = ( 18 + race_table[ch->race]->lck_plus + plck + dplck );
      pAbility = &ch->perm_lck;
      pOutput = "luck";
   }
   else
   {
      if( class_table[ch->class]->attr_prime == APPLY_STR )
         coststr = 3;
      else
         coststr = 5;

      if( class_table[ch->class]->attr_prime == APPLY_INT )
         costint = 3;
      else
         costint = 5;

      if( class_table[ch->class]->attr_prime == APPLY_WIS )
         costwis = 3;
      else
         costwis = 5;

      if( class_table[ch->class]->attr_prime == APPLY_DEX )
         costdex = 3;
      else
         costdex = 5;

      if( class_table[ch->class]->attr_prime == APPLY_CON )
         costcon = 3;
      else
         costcon = 5;

      if( class_table[ch->class]->attr_prime == APPLY_CHA )
         costcha = 3;
      else
         costcha = 5;

      if( class_table[ch->class]->attr_prime == APPLY_LCK )
         costlck = 3;
      else
         costlck = 5;


      ch_printf( ch, "&CWith &Y%d &Cpractices, you can train:\n\r", ch->practice );
      send_to_char( "&z-[ &GStat&z ]-----------------[  &GCur&z     &gMax&z  ]--[ &GCost&z ]--\n\r", ch );
      ch_printf( ch, "  &z(&Rhp&z)    Hit Points\t &W%8d &z/&w%8d\t&R%d\n\r", ch->max_hit, sysdata.max_hp,
                 class_table[ch->class]->hpcost );
      if( !IS_VAMPIRE( ch ) )
         ch_printf( ch, "  &z(&Rmana&z)  Mana Points\t &W%8d &z/&w%8d\t&R%d\n\r", ch->max_mana, sysdata.max_mana,
                    class_table[ch->class]->manacost );
      if( IS_VAMPIRE( ch ) )
         ch_printf( ch, "  &z(&Rblood&z) Blood Points\t &W%8d &z/&w%8d\t&R%d\n\r", ch->max_blood, sysdata.max_mana,
                    class_table[ch->class]->manacost );
      ch_printf( ch, "  &z(&Rmove&z)  Move Points\t &W%8d &z/&w%8d\t&R%d\n\r", ch->max_move, sysdata.max_move,
                 class_table[ch->class]->movecost );
      if( ch->perm_str < ( 18 + race_table[ch->race]->str_plus + pstr + dpstr ) )
         ch_printf( ch, "  &z(&Rstr&z)   Strength\t\t&W%d   &w%d\t\t&R%d\n\r", ch->perm_str, mpstr, coststr );
      if( ch->perm_int < ( 18 + race_table[ch->race]->int_plus + pint + dpint ) )
         ch_printf( ch, "  &z(&Rint&z)   Intelligence\t\t&W%d   &w%d\t\t&R%d\n\r", ch->perm_int, mpint, costint );
      if( ch->perm_wis < ( 18 + race_table[ch->race]->wis_plus + pwis + dpwis ) )
         ch_printf( ch, "  &z(&Rwis&z)   Wisdom\t\t&W%d   &w%d\t\t&R%d\n\r", ch->perm_wis, mpwis, costwis );
      if( ch->perm_dex < ( 18 + race_table[ch->race]->dex_plus + pdex + dpdex ) )
         ch_printf( ch, "  &z(&Rdex&z)   Dexterity\t\t&W%d   &w%d\t\t&R%d\n\r", ch->perm_dex, mpdex, costdex );
      if( ch->perm_con < ( 18 + race_table[ch->race]->con_plus + pcon + dpcon ) )
         ch_printf( ch, "  &z(&Rcon&z)   Constitution\t\t&W%d   &w%d\t\t&R%d\n\r", ch->perm_con, mpcon, costcon );
      if( ch->perm_cha < ( 18 + race_table[ch->race]->cha_plus + pcha + dpcha ) )
         ch_printf( ch, "  &z(&Rcha&z)   Charisma\t\t&W%d   &w%d\t\t&R%d\n\r", ch->perm_cha, mpcha, costcha );
      if( ch->perm_lck < ( 18 + race_table[ch->race]->lck_plus + plck + dplck ) )
         ch_printf( ch, "  &z(&Rlck&z)   Luck\t\t\t&W%d   &w%d\t\t&R%d\n\r", ch->perm_lck, mplck, costlck );
      send_to_char( "&z----------------------------------------------&D\n\r", ch );
      return;
   }

   if( *pAbility >= m )
   {
      act( AT_RED, "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
      return;
   }

   if( cost > ch->practice )
   {
      send_to_char( "You don't have enough practices.\n\r", ch );
      return;
   }

   ch->practice -= cost;
   *pAbility += 1;
   act( AT_RED, "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
   act( AT_RED, "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
   return;
}

void do_book( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   int cost;

   if( argument[0] == '\0' )
   {
      send_to_char( "&RSyntax: book sign\n\r", ch );
      send_to_char( "&RSyntax: book erase\n\r", ch );
      return;
   }


   if( xIS_SET( ch->in_room->room_flags, ROOM_BLOODROOM ) )
   {
      cost = ch->level * 10;

      if( !str_cmp( argument, "sign" ) )
      {
         if( get_age( ch ) < 18 )
         {
            send_to_char( "&rYou are too young to be a Player Killer.\n\r", ch );
            return;
         }
         else
         {
            if( CAN_PKILL( ch ) )
            {
               send_to_char( "&rYou are already a Player Killer!\n\r", ch );
               return;
            }
            else
            {
               send_to_char( "&rYou sign your name in the Book of Blood.\n\r", ch );
               send_to_char( "&rYou are now a Player Killer!\n\r", ch );
               SET_BIT( ch->pcdata->flags, PCFLAG_DEADLY );
               xREMOVE_BIT( ch->act, PLR_NICE );
               save_char_obj( ch );
//     save_finger( ch );
               saving_char = NULL;
               sprintf( buf, "&rSaving %s.\n\r", ch->name );
               send_to_char( buf, ch );
               send_to_char( "&rDone.\n\r", ch );
               return;
            }
         }
         return;
      }

      if( !str_cmp( argument, "erase" ) )
      {
         if( !CAN_PKILL( ch ) )
         {
            send_to_char( "&rYou are already peaceful!\n\r", ch );
            return;
         }
         else
         {
            if( ch->gold < cost )
            {
               send_to_char( "&rYou do not have enough katyr to erase your name!\n\r", ch );
               return;
            }
            else
            {
               ch->gold -= cost;
               REMOVE_BIT( ch->pcdata->flags, PCFLAG_DEADLY );
               xSET_BIT( ch->act, PLR_NICE );
               send_to_char( "&rYou are now a peaceful player.\n\r", ch );
               save_char_obj( ch );
               saving_char = NULL;
               send_to_char( "&rSaving $n.\n\r", ch );
               send_to_char( "&rDone.\n\r", ch );
               return;
            }
         }
         return;
      }
      return;
   }
   else
   {
      send_to_char( "&rYou can't sign that!\n\r", ch );
      return;
   }
}

void do_newaff( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   int affnum = 0;
   char buf[MAX_STRING_LENGTH];
   char snip[MAX_STRING_LENGTH];
   AFFECT_DATA *paf = NULL;
   SKILLTYPE *skill = NULL;

   if( IS_NPC( ch ) )
      return;

   argument = one_argument( argument, arg );

   send_to_char_color( "&z-=&w[ &WAffects Summary &w]&z=-&D\n\r\n\r", ch );


   if( !xIS_EMPTY( ch->affected_by ) )
   {
      send_to_char( "&wSome, all or none of these are applied by equipment.\n\r&zImbued with: ", ch );
      ch_printf( ch, "&C%s\n\r\n\r", affect_bit_name( &ch->affected_by ) );
   }
   if( LEARNED( ch, gsn_magic_lore ) && ch->level > 24 )
   {
      for( paf = ch->first_affect; paf; paf = paf->next )
      {
         if( ( skill = get_skilltype( paf->type ) ) != NULL )
         {
            if( paf->location == APPLY_AFFECT )
            {
               sprintf( snip, "for" );
            }
            else
            {
               sprintf( snip, "which modifies %s by &Y%d&B for", affect_loc_name( paf->location ), paf->modifier );
            }
            affnum++;
            ch_printf( ch, "&BSpell: '&w%s&B' %s&R %d&B rounds.&D\n\r", skill->name, snip, paf->duration );
         }
      }
   }
   else if( ch->level > 14 )
   {
      for( paf = ch->first_affect; paf; paf = paf->next )
      {
         if( ( skill = get_skilltype( paf->type ) ) != NULL )
         {
            if( paf->duration > 200 )
            {
               sprintf( snip, "for seemingly forever" );
            }
            else if( paf->duration > 149 && paf->duration < 201 )
            {
               sprintf( snip, "for a long time" );
            }
            else if( paf->duration > 99 && paf->duration < 150 )
            {
               sprintf( snip, "for a decent amount of time" );
            }
            else if( paf->duration > 19 && paf->duration < 100 )
            {
               sprintf( snip, "for a small amount of time" );
            }
            else if( paf->duration < 21 && paf->duration > 5 )
            {
               sprintf( snip, "seems to be fading fast" );
            }
            else if( paf->duration < 6 )
            {
               sprintf( snip, "seems to be wavering" );
            }
            affnum++;
            ch_printf( ch, "&BSpell: '&w%s&B' %s.&D\n\r", skill->name, snip );
         }
      }
   }
   else
   {
      for( paf = ch->first_affect; paf; paf = paf->next )
      {
         if( ( skill = get_skilltype( paf->type ) ) != NULL )
         {
            affnum++;
            ch_printf( ch, "&BSpell: '&w%s&B' &D\n\r", skill->name );
         }
      }
   }

   if( affnum == 0 )
   {
      send_to_char_color( "&R* &wNo cantrip or skill affects you.\n\r\n\r", ch );
   }
   sprintf( buf, "\n\r&WTotal of &R%d&W affects.&D\n\r", affnum );
   send_to_char( buf, ch );
   return;
}

void do_aexit( CHAR_DATA * ch, char *argument )
{
   ROOM_INDEX_DATA *room;
   int i, vnum;
   AREA_DATA *tarea;
   AREA_DATA *otherarea;
   EXIT_DATA *pexit;
#ifdef OVERLANDCODE
   ENTRANCE_DATA *enter;
#endif
   int lrange;
   int trange;
   bool found = FALSE;

   if( argument[0] == '\0' )
      tarea = ch->in_room->area;
   else
   {
      for( tarea = first_area; tarea; tarea = tarea->next )
         if( !str_cmp( tarea->filename, argument ) )
         {
            found = TRUE;
            break;
         }

      if( !found )
      {
         for( tarea = first_build; tarea; tarea = tarea->next )
            if( !str_cmp( tarea->filename, argument ) )
            {
               found = TRUE;
               break;
            }
      }

      if( !found )
      {
         send_to_char( "Area not found. Check 'zones' for the filename.\n\r", ch );
         return;
      }
   }

   trange = tarea->hi_r_vnum;
   lrange = tarea->low_r_vnum;

   for( vnum = lrange; vnum <= trange; vnum++ )
   {
      if( ( room = get_room_index( vnum ) ) == NULL )
         continue;

      if( xIS_SET( room->room_flags, ROOM_TELEPORT ) && ( room->tele_vnum < lrange || room->tele_vnum > trange ) )
      {
         pager_printf( ch, "From: %-20.20s Room: %5d To: Room: %5d (Teleport)\n\r", tarea->filename, vnum, room->tele_vnum );
      }

      for( i = 0; i < MAX_DIR + 1; i++ )
      {
         if( ( pexit = get_exit( room, i ) ) == NULL )
            continue;
#ifdef OVERLANDCODE
         if( IS_EXIT_FLAG( pexit, EX_OVERLAND ) )
         {
            pager_printf( ch, "To: Overland %4dX %4dY From: %20.20s Room: %5d (%s)\n\r",
                          pexit->coord->x, pexit->coord->y, tarea->filename, vnum, dir_name[i] );
            continue;
         }
#endif
         if( pexit->to_room->area != tarea )
         {
            pager_printf( ch, "To: %-20.20s Room: %5d From: %-20.20s Room: %5d (%s)\n\r",
                          pexit->to_room->area->filename, pexit->vnum, tarea->filename, vnum, dir_name[i] );
         }
      }
   }

   for( otherarea = first_area; otherarea; otherarea = otherarea->next )
   {
      if( tarea == otherarea )
         continue;
      trange = otherarea->hi_r_vnum;
      lrange = otherarea->low_r_vnum;
      for( vnum = lrange; vnum <= trange; vnum++ )
      {
         if( ( room = get_room_index( vnum ) ) == NULL )
            continue;

         if( xIS_SET( room->room_flags, ROOM_TELEPORT ) )
         {
            if( room->tele_vnum >= tarea->low_r_vnum && room->tele_vnum <= tarea->hi_r_vnum )
               pager_printf( ch, "From: %-20.20s Room: %5d To: %-20.20s Room: %5d (Teleport)\n\r",
                             otherarea->filename, vnum, tarea->filename, room->tele_vnum );
         }

         for( i = 0; i < MAX_DIR + 1; i++ )
         {
            if( ( pexit = get_exit( room, i ) ) == NULL )
               continue;

#ifdef OVERLANDCODE
            if( IS_EXIT_FLAG( pexit, EX_OVERLAND ) )
               continue;
#endif
            if( pexit->to_room->area == tarea )
            {
               pager_printf( ch, "From: %-20.20s Room: %5d To: %-20.20s Room: %5d (%s)\n\r",
                             otherarea->filename, vnum, pexit->to_room->area->filename, pexit->vnum, dir_name[i] );
            }
         }
      }
   }

#ifdef OVERLANDCODE
   for( enter = first_entrance; enter; enter = enter->next )
   {
      if( enter->vnum >= tarea->low_r_vnum && enter->vnum <= tarea->hi_r_vnum )
      {
         pager_printf( ch, "From: Overland %4dX %4dY To: Room: %5d\n\r", enter->here->x, enter->here->y, enter->vnum );
      }
   }
#endif
   return;
}

void do_rolldice( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   int number, sides, i, roll;

   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg2 );

   if( arg[0] == '\0' || arg2[0] == '\0' || !is_number( arg ) || !is_number( arg2 ) )
   {
      ch_printf( ch, "&RSyntax: &CRolldice <number of dice> <size of dice>\n\r" );
      return;
   }

   number = atoi( arg );
   sides = atoi( arg2 );

   if( number < 1 )
   {
      ch_printf( ch, "You have to roll at least one die.\n\r" );
      return;
   }

   if( sides < 2 )
   {
      ch_printf( ch, "You need at least two sides.\n\r" );
      return;
   }

   if( number > 10 || sides > 9999 )
   {
      ch_printf( ch, "You can only have a maximum of 10 dice with 9,999 sides each.\n\r" );
      return;
   }

   for( i = 1; i <= number; i++ )
   {
      roll = number_range( 1, sides );
      sprintf( buf, "&Y%d %d&w-sided die...the result: &Y%d&D", number, sides, roll );
      act( AT_PLAIN, "&wYou roll $T", ch, NULL, buf, TO_CHAR );
      act( AT_PLAIN, "&w$n rolls $T", ch, NULL, buf, TO_ROOM );
   }
   return;
}


void do_dice( CHAR_DATA * ch, char *argument )
{
   char arg2[MAX_INPUT_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   int number, sides, result, i;

   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg2 );


   if( IS_NPC( ch ) )
   {
      send_to_char( "Yeah.....right....\n\r", ch );
      return;
   }

   if( arg[0] == '\0' )
   {
      send_to_char_color( "&WSyntax: Dice <Number of dice> <Size of dice>\n\r", ch );
      return;
   }


   if( arg2[0] == '\0' )
   {
      send_to_char_color( "&WHow many sides will your dice have?\n\r", ch );
      return;
   }

   if( !is_number( arg ) || !is_number( arg2 ) )
   {
      send_to_char_color( "&WUse only numbers as arguments, please!\n\r", ch );
      return;
   }

   number = atoi( arg );
   sides = atoi( arg2 );


   if( number < 1 )
   {
      send_to_char_color( "&WYou have to roll at least one die!\n\r", ch );
      return;
   }

   if( sides < 2 )
   {
      send_to_char_color( "&WNow that would be kind of pointless, wouldn't it?\n\r", ch );
      return;
   }

   if( number > 10 || sides > 9999 )
   {
      send_to_char_color( "&WYou can only have a maximum of 10 dice with 9,999 sides each.\n\r", ch );
      return;
   }


   result = 0;
   for( i = 1; i <= number; i++ )
      result += number_range( 1, sides );

   sprintf( buf, "&Y%d %d&w-sided %s...the result: &Y%d&D", number, sides, number == 1 ? "die" : "dice", result );

   act( AT_PLAIN, "&wYou roll $T", ch, NULL, buf, TO_CHAR );
   act( AT_PLAIN, "&w$n rolls $T", ch, NULL, buf, TO_ROOM );

   return;
}

void do_divorce( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   CHAR_DATA *vict1;
   CHAR_DATA *vict2;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Syntax:  divorce <person 1> <person 2>\n\r", ch );
      return;
   }

   if( ( vict1 = get_char_world( ch, arg1 ) ) == NULL )
   {
      sprintf( buf, "%s is not connected.\n\r", capitalize( arg1 ) );
      send_to_char( buf, ch );
      return;
   }

   if( ( vict2 = get_char_world( ch, arg2 ) ) == NULL )
   {
      sprintf( buf, "%s is not connected.\n\r", capitalize( arg2 ) );
      send_to_char( buf, ch );
      return;
   }

   if( !IS_ADMIN( ch ) && ( IS_AGOD( vict1 ) || IS_AGOD( vict2 ) ) )
   {
      send_to_char( "You can't do this.\n\r", ch );
      return;
   }

   if( IS_NPC( vict1 ) || IS_NPC( vict2 ) )
   {
      send_to_char( "You cannot divorce mobs, silly!\n\r", ch );
      return;
   }

   if( vict1 == vict2 )
   {
      send_to_char( "You cannot do that.\n\r", ch );
      return;
   }

   if( !( vict1->pcdata->spouse ) )
   {
      sprintf( buf, "%s is not married!\n\r", vict1->name );
      send_to_char( buf, ch );
      return;
   }

   if( !( vict2->pcdata->spouse ) )
   {
      sprintf( buf, "%s is not married!\n\r", vict2->name );
      send_to_char( buf, ch );
      return;
   }

   if( ( vict1->pcdata->spouse != vict2->name || vict2->pcdata->spouse != vict1->name ) )
   {
      send_to_char( "They aren't married to each other.\n\r", ch );
      return;
   }

   STRFREE( vict1->pcdata->spouse );
   STRFREE( vict2->pcdata->spouse );

   sprintf( buf, "&R<&YWEDDING INFO&R> &W%s and %s are now divorced.&D", vict1->name, vict2->name );
   talk_info( AT_IMMORT, buf );
}

void do_marry( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   CHAR_DATA *vict1;
   CHAR_DATA *vict2;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Syntax:  marry <person 1> <person 2>\n\r", ch );
      return;
   }

   if( ( vict1 = get_char_world( ch, arg1 ) ) == NULL )
   {
      sprintf( buf, "%s is not connected.\n\r", capitalize( arg1 ) );
      send_to_char( buf, ch );
      return;
   }

   if( ( vict2 = get_char_world( ch, arg2 ) ) == NULL )
   {
      sprintf( buf, "%s is not connected.\n\r", capitalize( arg2 ) );
      send_to_char( buf, ch );
      return;
   }

   if( !IS_ADMIN( ch ) && ( IS_IMMORTAL( vict1 ) || IS_IMMORTAL( vict2 ) ) )
   {
      send_to_char( "You can't do this.\n\r", ch );
      return;
   }

   if( IS_NPC( vict1 ) || IS_NPC( vict2 ) )
   {
      send_to_char( "You cannot marry a player to a mob, silly!\n\r", ch );
      return;
   }

   if( vict1 == vict2 )
   {
      send_to_char( "You cannot do that.\n\r", ch );
      return;
   }

   if( vict1->pcdata->spouse )
   {
      sprintf( buf, "%s is already married to %s!\n\r", vict1->name, vict1->pcdata->spouse );
      send_to_char( buf, ch );
      return;
   }

   if( vict2->pcdata->spouse )
   {
      sprintf( buf, "%s is already married to %s!\n\r", vict2->name, vict2->pcdata->spouse );
      send_to_char( buf, ch );
      return;
   }
/* If i ever need to use this again. I will
	if (!str_cmp( vict1->name, "Radyn" ) )
	{
		if ( str_cmp( vict2->name, "Kianen" ) )
		{
		send_to_char( "No.\n\r", ch );
		return;
		}
	}
	if (!str_cmp( vict2->name, "Radyn" ) )
	{
		if ( str_cmp( vict1->name, "Kianen" ) )
		{
			send_to_char( "No.\n\r", ch );
			return;
		}
	}*/

   vict1->pcdata->spouse = STRALLOC( vict2->name );
   vict2->pcdata->spouse = STRALLOC( vict1->name );

   sprintf( buf, "&R<&YWEDDING INFO&R> &W%s and %s are now married.&D", vict1->name, vict2->name );
   talk_info( AT_IMMORT, buf );
}

void do_homebound( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   if( !IS_AWAKE( ch ) )
   {
      send_to_char( "In your dreams or what?\n\r", ch );
      return;
   }

   if( ch->fighting != NULL )
   {
      send_to_char( "Not while you are fighting.\n\r", ch );
      return;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_NO_RECALL ) )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   if( !ch->pcdata->spouse )
   {
      send_to_char( "But you are not married!\n\r", ch );
      return;
   }
   strcat( buf2, "0." );
   strcat( buf2, ch->pcdata->spouse );
   victim = get_char_world( ch, buf2 );

   if( !victim )
   {
      sprintf( buf, "%s is not connected.\n\r", ch->pcdata->spouse );
      send_to_char( buf, ch );
      return;
   }

   act( AT_RED, "$n steps into a rose-colored gate and disappears.", ch, NULL, NULL, TO_ROOM );
   char_from_room( ch );
   char_to_room( ch, victim->in_room );

   act( AT_RED, "$n steps into the room from a rose-colored gate.", ch, NULL, NULL, TO_ROOM );
   do_look( ch, "auto" );
}

void look_window( CHAR_DATA * ch, OBJ_DATA * obj )
{
   char buf[MAX_STRING_LENGTH];
   ROOM_INDEX_DATA *window_room;

   if( obj->value[0] == 0 )
   {
      sprintf( buf, "%s\n\r", obj->description );
      send_to_char( buf, ch );
      return;
   }

   window_room = get_room_index( obj->value[0] );

   if( window_room == NULL )
   {
      send_to_char( "!!BUG!! Window looks into a NULL room! Please report!\n\r", ch );
      bug( "Window %d looks into a null room!!!", obj->pIndexData->vnum );
      return;
   }

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Looking through the window you can see ", ch );
      send_to_char( window_room->name, ch );
      send_to_char( "\n\r&O", ch );
      send_to_char( window_room->description, ch );
      send_to_char( "\n\r", ch );
      show_list_to_char( window_room->first_content, ch, FALSE, FALSE );
      show_char_to_char( window_room->first_person, ch );
      return;
   }
}
