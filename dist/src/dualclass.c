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
/*						Dual Class module			                    */
/************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "acadia.h"


void do_dual( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   int attavg = 0;
   char power[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];

   argument = one_argument( argument, arg );

   power[0] = '\0';

   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: dual <class>\n\r", ch );
      return;
   }

   if( ch->level < 200 )
   {
      send_to_char( "You are not allowed to dual class yet!\n\r", ch );
      return;
   }
   if( IS_DUAL( ch ) )
   {
      send_to_char( "You are only allowed 2 classes!\n\r", ch );
      return;
   }

   attavg = number_range( 1, 13 );

   if( !str_cmp( arg, "mage" ) )
   {
      if( ch->class == 0 )
      {
         send_to_char( "You cannot dual class your own class.", ch );
         return;
      }
      ch->dualclass = 0;
      strcpy( power, "Mages" );
      sprintf( buf3, "&R<&BCLASS INFO&R> %s has decided to dual class Mage!&D", ch->name );
      talk_info( AT_PLAIN, buf3 );
   }
   if( !str_cmp( arg, "cleric" ) )
   {
      if( ch->class == 1 )
      {
         send_to_char( "You cannot dual class your own class.", ch );
         return;
      }
      ch->dualclass = 1;
      strcpy( power, "Clerics" );
      sprintf( buf3, "&R<&BCLASS INFO&R> %s has decided to dual class Cleric!&D", ch->name );
      talk_info( AT_PLAIN, buf3 );
   }
   if( !str_cmp( arg, "thief" ) )
   {
      if( ch->class == 2 )
      {
         send_to_char( "You cannot dual class your own class.", ch );
         return;
      }
      ch->dualclass = 2;
      strcpy( power, "Thieves" );
      sprintf( buf3, "&R<&BCLASS INFO&R> %s has decided to dual class Thief!&D", ch->name );
      talk_info( AT_PLAIN, buf3 );
   }
   if( !str_cmp( arg, "warrior" ) )
   {
      if( ch->class == 3 )
      {
         send_to_char( "You cannot dual class your own class.", ch );
         return;
      }
      ch->dualclass = 3;
      strcpy( power, "Warriors" );
      sprintf( buf3, "&R<&BCLASS INFO&R> %s has decided to dual class Warrior!&D", ch->name );
      talk_info( AT_PLAIN, buf3 );
   }
   if( !str_cmp( arg, "vampire" ) )
   {
      send_to_char( "You cannot dual class Vampire.", ch );
      return;
   }
   if( !str_cmp( arg, "warwiz" ) )
   {
      if( ch->class == 5 )
      {
         send_to_char( "You cannot dual class your own class.", ch );
         return;
      }
      ch->dualclass = 5;
      strcpy( power, "WarWizard" );
      sprintf( buf3, "&R<&BCLASS INFO&R> %s has decided to dual class WarWizard!&D", ch->name );
      talk_info( AT_PLAIN, buf3 );
   }
   if( !str_cmp( arg, "jedi" ) )
   {
      send_to_char( "You cannot dual class Jedika.", ch );
      return;
   }
   if( !str_cmp( arg, "wolf" ) )
   {
      if( ch->class == 7 )
      {
         send_to_char( "You cannot dual class your own class.", ch );
         return;
      }
      ch->dualclass = 7;
      strcpy( power, "Wearwolf" );
      sprintf( buf3, "&R<&BCLASS INFO&R> %s has decided to dual class Wearwolf!&D", ch->name );
      talk_info( AT_PLAIN, buf3 );
   }
   if( !str_cmp( arg, "ranger" ) )
   {
      if( ch->class == 8 )
      {
         send_to_char( "You cannot dual class your own class.", ch );
         return;
      }
      ch->dualclass = 8;
      strcpy( power, "Ranger" );
      sprintf( buf3, "&R<&BCLASS INFO&R> %s has decided to dual class Ranger!&D", ch->name );
      talk_info( AT_PLAIN, buf3 );
   }
   if( !str_cmp( arg, "taed" ) )
   {
      if( ch->class == 9 )
      {
         send_to_char( "You cannot dual class your own class.", ch );
         return;
      }
      ch->dualclass = 9;
      strcpy( power, "Taedien" );
      sprintf( buf3, "&R<&BCLASS INFO&R> %s has decided to dual class Taedien!&D", ch->name );
      talk_info( AT_PLAIN, buf3 );
   }
   if( !str_cmp( arg, "archer" ) )
   {
      send_to_char( "You cannot dual class Archer.", ch );
      return;
   }
   if( !str_cmp( arg, "barb" ) )
   {
      if( ch->class == 11 )
      {
         send_to_char( "You cannot dual class your own class.", ch );
         return;
      }
      ch->dualclass = 11;
      strcpy( power, "Barbarian" );
      sprintf( buf3, "&R<&BCLASS INFO&R> %s has decided to dual class Barbarian!&D", ch->name );
      talk_info( AT_PLAIN, buf3 );
   }
   if( !str_cmp( arg, "sorc" ) )
   {
      if( ch->class == 12 )
      {
         send_to_char( "You cannot dual class your own class.", ch );
         return;
      }
      ch->dualclass = 12;
      strcpy( power, "Sorcerer" );
      sprintf( buf3, "&R<&BCLASS INFO&R> %s has decided to dual class Sorcerer!&D", ch->name );
      talk_info( AT_PLAIN, buf3 );
   }
   if( !str_cmp( arg, "paladin" ) )
   {
      if( ch->class == 13 )
      {
         send_to_char( "You cannot dual class your own class.", ch );
         return;
      }
      ch->dualclass = 13;
      strcpy( power, "Paladin" );
      sprintf( buf3, "&R<&BCLASS INFO&R> %s has decided to dual class Paladin!&D", ch->name );
      talk_info( AT_PLAIN, buf3 );
   }
   if( !str_cmp( arg, "bard" ) )
   {
      send_to_char( "You cannot dual class Bard.", ch );
      return;
   }
   if( !str_cmp( arg, "samurai" ) )
   {
      send_to_char( "You cannot dual class Samurai.", ch );
      return;
   }

   if( power[0] == '\0' )
   {
      send_to_char( "Syntax: dual <class>\n\r", ch );
      return;
   }
   else
   {
      sprintf( buf2, "&BYou close your eyes as the power of %s rages through your body!&D", power );

      act( AT_MAGIC, buf2, ch, NULL, NULL, TO_CHAR );
      ch->exp = exp_level( ch, ch->sublevel );
      do_save( ch, "" );
      return;
   }
   return;
}


bool DUAL_SKILL( CHAR_DATA * ch, int sn )
{
   if( IS_AGOD( ch ) )
   {
      return TRUE;
   }
   else if( skill_table[sn]->type == SKILL_RACIAL )
   {
      if( ch->level >= skill_table[sn]->race_level[ch->race] && skill_table[sn]->race_level[ch->race] )
         return TRUE;
   }
   else
   {
      if( ch->level < 100 )
      {
         if( IS_DUAL( ch ) )
         {
            if( ( ch->level >= skill_table[sn]->skill_level[ch->dualclass] )
                || ( ch->level >= skill_table[sn]->skill_level[ch->class] ) )
               return TRUE;
         }
         else
         {
            if( ch->level >= skill_table[sn]->skill_level[ch->class] )
               return TRUE;
         }
      }
      else
      {
         if( IS_DUAL( ch ) )
         {
            if( ( ( ch->level >= skill_table[sn]->skill_level[ch->dualclass] )
                  || ( ch->level >= skill_table[sn]->skill_level[ch->class] )
                  && ( ch->sublevel >= skill_table[sn]->skill_slevel[ch->dualclass]
                       || ch->sublevel >= skill_table[sn]->skill_slevel[ch->class] ) ) )
/*
				if((ch->level >= skill_table[sn]->skill_level[ch->dualclass]
				&& ch->sublevel >= skill_table[sn]->skill_slevel[ch->dualclass]) 
				
				|| (ch->level>= skill_table[sn]->skill_level[ch->class]
				&& ch->sublevel >= skill_table[sn]->skill_slevel[ch->class]))*/
               return TRUE;
         }
         else
         {
            if( ch->level >= skill_table[sn]->skill_level[ch->class] )
            {
               if( ch->level > skill_table[sn]->skill_level[ch->class] )
               {
                  return TRUE;
               }
               else if( ch->sublevel >= skill_table[sn]->skill_slevel[ch->class] )
               {
                  return TRUE;
               }
            }
         }
      }
   }
   return FALSE;
}

int dual_adept( CHAR_DATA * ch, int sn )
{
   int max = 0;

   if( ch->level < 100 )
   {
      if( skill_table[( sn )]->type == SKILL_RACIAL )
         max = skill_table[( sn )]->race_adept[ch->race];
      else if( skill_table[( sn )]->skill_level[ch->class] <= LEVEL_IMMORTAL )
         max = skill_table[( sn )]->skill_adept[ch->class];
      if( !IS_DUAL( ch ) )
         return max;

      if( ( skill_table[( sn )]->skill_adept[ch->dualclass] > max )
          && ( skill_table[( sn )]->skill_level[ch->dualclass] <= LEVEL_IMMORTAL ) )
         max = skill_table[( sn )]->skill_adept[ch->dualclass];

      return max;
   }
   else
   {
      if( skill_table[( sn )]->type == SKILL_RACIAL )
         max = skill_table[( sn )]->race_adept[ch->race];
      else
         if( skill_table[( sn )]->skill_level[ch->class] <= LEVEL_IMMORTAL
             || skill_table[( sn )]->skill_slevel[ch->class] <= LEVEL_IMMORTAL )
         max = skill_table[( sn )]->skill_adept[ch->class];
      if( !IS_DUAL( ch ) )
         return max;

      if( ( skill_table[( sn )]->skill_adept[ch->dualclass] > max )
          && ( skill_table[( sn )]->skill_level[ch->dualclass] <= LEVEL_IMMORTAL
               || skill_table[( sn )]->skill_slevel[ch->dualclass] <= LEVEL_IMMORTAL ) )
         max = skill_table[( sn )]->skill_adept[ch->dualclass];

      return max;
   }
}

int find_skill_level( CHAR_DATA * ch, int sn )
{
   int level;

   level = skill_table[sn]->skill_level[ch->class];

   if( skill_table[( sn )]->type == SKILL_RACIAL )
      return skill_table[( sn )]->race_level[ch->race];
   else if( !IS_DUAL( ch ) )
      return level;

   if( skill_table[sn]->skill_level[ch->dualclass] < level )
      level = skill_table[sn]->skill_level[ch->dualclass];
   return level;
}

int find_skill_slevel( CHAR_DATA * ch, int sn )
{
   int level;

   level = skill_table[sn]->skill_slevel[ch->class];

   if( skill_table[( sn )]->type == SKILL_RACIAL )
      return skill_table[( sn )]->race_level[ch->race];
   else if( !IS_DUAL( ch ) )
      return level;

   if( skill_table[sn]->skill_slevel[ch->dualclass] < level )
      level = skill_table[sn]->skill_slevel[ch->dualclass];
   return level;
}

int find_skill_level_target( int clas, int sn )
{
   int level;

   level = skill_table[sn]->skill_level[clas];

   return level;
}

int find_skill_slevel_target( int clas, int sn )
{
   int level;

   level = skill_table[sn]->skill_slevel[clas];

   return level;
}

int hp_max( CHAR_DATA * ch )
{

   if( !IS_DUAL( ch ) )
      return class_table[ch->class]->hp_max;
   else if( class_table[ch->dualclass]->hp_max > class_table[ch->class]->hp_max )
      return class_table[ch->dualclass]->hp_max;

   return class_table[ch->class]->hp_max;
}

int hp_min( CHAR_DATA * ch )
{

   if( !IS_DUAL( ch ) )
      return class_table[ch->class]->hp_min;
   else if( class_table[ch->dualclass]->hp_min < class_table[ch->class]->hp_min )
      return class_table[ch->dualclass]->hp_min;

   return class_table[ch->class]->hp_min;
}

bool use_mana( CHAR_DATA * ch )
{
   if( class_table[ch->class]->fMana )
      return TRUE;
   if( !IS_DUAL( ch ) )
      return FALSE;
   if( class_table[ch->dualclass]->fMana )
      return TRUE;
   return FALSE;
}
