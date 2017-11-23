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
/*					 Automated Quest module	        */
/************************************************************************/
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "acadia.h"
struct reward_type
{
   char *name;
   char *keyword;
   int cost;
   bool object;
   int value;
   void *where;
};

struct quest_desc_type
{
   char *name;
   char *short_descr;
   char *long_descr;
};

const struct quest_desc_type quest_desc[] = {
   {"quest sword", "the Sword of the Gods",
    "&G[&RTARGET&G]&W The Sword of the Gods is lying here, waiting to be returned to its owner."},

   {"quest crown", "the Crown of the Ancient One",
    "&G[&RTARGET&G]&W The Crown of the Ancient One is lying here, waiting to be returned to its owner."},

   {"quest ring", "the Ring of the Gods",
    "&G[TARGET&G]&W The Ring of the Gods is lying here, waiting to be returned to its owner."},

   {"quest dagger", "the Dagger of the Ancient One",
    "&G[&RTARGET&G]&W The Dagger of the Ancient One is lying here, waiting to be returned to its owner."},

   {NULL, NULL, NULL}
};

int quest_num;

void generate_quest args( ( CHAR_DATA * ch, CHAR_DATA * questman ) );
void quest_update args( ( void ) );
bool quest_level_diff args( ( int clevel, int mlevel ) );
int QUEST_OBJ args( ( void ) );
void quest_death_check args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
AREA_DATA *find_area( int vnum, sh_int type );

void do_qwest( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *questman;
   OBJ_DATA *obj = NULL, *obj_next;
   OBJ_INDEX_DATA *questinfoobj;
   char buf[MAX_STRING_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   int i;

   const struct reward_type reward_table[] = {
      {"4 Practices", "practices", 10, FALSE, 4, &ch->practice},
      {"350,000 katyr Pieces", "350000 katyr", 150, FALSE, 350000, &ch->gold},
      {"Jur's Cloak", "jur cloak", 15000, TRUE, 1006, 0},
      {"Jir's Blade", "jir blade", 15000, TRUE, 1007, 0},
      {"Jer's Armor", "jer armor", 15000, TRUE, 1022, 0},
      {NULL, NULL, 0, FALSE, 0, 0}
   };


   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( IS_NPC( ch ) )
   {
      send_to_char( "NPC's can't quest.\n\r", ch );
      return;
   }

   if( arg1[0] == '\0' )
   {
      send_to_char( "Qwest commands: Info, Time, Request, Complete, Quit, List, and Buy.\n\r", ch );
//        send_to_char("For more information, type 'Help Qwest'.\n\r",ch);
      return;
   }

   if( !strcmp( arg1, "info" ) )
   {
      MOB_INDEX_DATA *temp;
      ROOM_INDEX_DATA *room;
      char roomname[MSL];
      bool mobquest = FALSE;
      bool objquest = FALSE;

      if( xIS_SET( ch->act, PLR_QUESTING ) )
      {
         send_to_char( "Quest Info\n\r", ch );
         send_to_char( "==========\n\r", ch );

         if( ch->pcdata->questmob > 0 )
         {
            temp = get_mob_index( ch->pcdata->questmob );
            sprintf( buf, "You are Looking for: %s\n\r", temp ? temp->short_descr : "(ERROR)" );
            send_to_char( buf, ch );
            mobquest = TRUE;
         }
         else if( ch->pcdata->questobj > 0 )
         {
            questinfoobj = get_obj_index( ch->pcdata->questobj );
            sprintf( buf, "You are Looking for: %s\n\r", questinfoobj ? questinfoobj->short_descr : "(ERROR)" );
            send_to_char( buf, ch );
            objquest = TRUE;
         }

         if( objquest || mobquest )
         {
            room = get_room_index( ch->pcdata->questinroom );
            sprintf( roomname, "%s", room ? room->name : "Unknown" );
         }

         if( objquest )
         {
            sprintf( buf, "Last Known Sighting: %s\n\r", roomname[0] != '\0' ? roomname : "(ERROR)" );
            send_to_char( buf, ch );
         }
         else if( mobquest )
         {
            sprintf( buf, "Last Known Signting: %s\n\r", roomname[0] != '\0' ? roomname : "(ERROR)" );
            send_to_char( buf, ch );
         }

      }
      else
         send_to_char( "You aren't currently on a quest.\n\r", ch );
      return;
   }
   else if( !strcmp( arg1, "time" ) )
   {
      if( !xIS_SET( ch->act, PLR_QUESTING ) )
      {
         send_to_char( "You aren't currently on a quest.\n\r", ch );
         if( ch->pcdata->nextquest > 1 )
         {
            sprintf( buf, "There are %d minutes remaining until you can "
                     "go on another quest.\n\r", ch->pcdata->nextquest );
            send_to_char( buf, ch );
         }
         else if( ch->pcdata->nextquest == 1 )
         {
            sprintf( buf, "There is less than a minute remaining until" "you can go on another quest.\n\r" );
            send_to_char( buf, ch );
         }
      }
      else if( ch->pcdata->countdown > 0 )
      {
         sprintf( buf, "Time left for current quest: %d\n\r", ch->pcdata->countdown );
         send_to_char( buf, ch );
      }
      return;
   }

   for( questman = ch->in_room->first_person; questman != NULL; questman = questman->next_in_room )
   {
      if( !IS_NPC( questman ) )
         continue;
      if( questman->spec_fun == spec_lookup( "spec_questmaster" ) )
         break;
   }

   if( questman == NULL || questman->spec_fun != spec_lookup( "spec_questmaster" ) )
   {
      send_to_char( "You can't do that here.\n\r", ch );
      return;
   }

   if( who_fighting( questman ) != NULL )
   {
      send_to_char( "Wait until the fighting stops.\n\r", ch );
      return;
   }

   ch->pcdata->questgiver = questman;

   if( !strcmp( arg1, "list" ) )
   {
      act( AT_GREY, "$n asks $N for a list of quest items.", ch, NULL, questman, TO_ROOM );
      act( AT_GREY, "You ask $N for a list of quest items.", ch, NULL, questman, TO_CHAR );
      send_to_char( "Current Quest Items available for Purchase:\n\r", ch );
      if( reward_table[0].name == NULL )
         send_to_char( "  Nothing.\n\r", ch );
      else
      {
         send_to_char( "  [&WCost&w]     [&BName&w]\n\r", ch );
         for( i = 0; reward_table[i].name != NULL; i++ )
         {
            sprintf( buf, "   &W%-4d&w       &b%s&w\n\r", reward_table[i].cost, reward_table[i].name );
            send_to_char( buf, ch );
         }
// send_to_char("\n\rPlease see HELP GLORYLIST of other things\n\ryou can buy\n\r",ch);
         send_to_char( "\n\rTo buy an item, type 'Qwest buy <item>'.\n\r", ch );
         return;
      }
   }

   else if( !strcmp( arg1, "buy" ) )
   {
      bool found = FALSE;
      if( arg2[0] == '\0' )
      {
         send_to_char( "To buy an item, type 'Qwest buy <item>'.\n\r", ch );
         return;
      }
      for( i = 0; reward_table[i].name != NULL; i++ )
         if( is_name( arg2, reward_table[i].keyword ) )
         {
            found = TRUE;
            if( ch->pcdata->quest_curr >= reward_table[i].cost )
            {
               ch->pcdata->quest_curr -= reward_table[i].cost;
               if( reward_table[i].object )
                  obj = create_object( get_obj_index( reward_table[i].value ), ch->level );
               else
               {
                  sprintf( buf, "In exchange for %d glory, %s gives you %s.\n\r",
                           reward_table[i].cost, questman->short_descr, reward_table[i].name );
                  send_to_char( buf, ch );
                  *( int * )reward_table[i].where += reward_table[i].value;
               }
               break;
            }
            else
            {
               sprintf( buf, "%s Sorry, %s, but you don't have enough glory for that.", ch->name, ch->name );
               do_say_to_char( questman, buf );
               return;
            }
         }
      if( !found )
      {
         sprintf( buf, "%s I don't have that item, %s.", ch->name, ch->name );
         do_say_to_char( questman, buf );
      }
      if( obj != NULL )
      {
         sprintf( buf, "In exchange for %d glory, %s gives you %s.\n\r",
                  reward_table[i].cost, questman->short_descr, obj->short_descr );
         send_to_char( buf, ch );
         obj_to_char( obj, ch );
      }
      return;
   }
   else if( !strcmp( arg1, "request" ) )
   {
      act( AT_GREY, "$n asks $N for a quest.", ch, NULL, questman, TO_ROOM );
      act( AT_GREY, "You ask $N for a quest.", ch, NULL, questman, TO_CHAR );
      if( xIS_SET( ch->act, PLR_QUESTING ) )
      {
         sprintf( buf, "%s But you're already on a quest!", ch->name );
         do_say_to_char( questman, buf );
         return;
      }
      if( ch->pcdata->nextquest > 0 )
      {
         sprintf( buf, "%s You're very brave, %s, but let someone else have a chance.", ch->name, ch->name );
         do_say_to_char( questman, buf );
         sprintf( buf, "%s Come back later.", ch->name );
         do_say_to_char( questman, buf );
         return;
      }

      sprintf( buf, "%s Thank you, brave %s!", ch->name, ch->name );
      do_say_to_char( questman, buf );
      ch->pcdata->questmob = 0;
      ch->pcdata->questobj = 0;
      generate_quest( ch, questman );

      if( ch->pcdata->questmob > 0 || ch->pcdata->questobj > 0 )
      {
         ch->pcdata->countdown = number_range( 10, 30 );
         xSET_BIT( ch->act, PLR_QUESTING );
         sprintf( buf, "%s You have %d minutes to complete this quest.", ch->name, ch->pcdata->countdown );
         do_say_to_char( questman, buf );
         sprintf( buf, "%s May the gods go with you!", ch->name );
         do_say_to_char( questman, buf );
      }
      return;
   }

   else if( !str_cmp( arg1, "complete" ) )
   {
      act( AT_GREY, "$n informs $N $e has completed $s quest.", ch, NULL, questman, TO_ROOM );
      act( AT_GREY, "You inform $N you have completed $s quest.", ch, NULL, questman, TO_CHAR );
      if( ch->pcdata->questgiver != questman )
      {
         sprintf( buf, "%s I never sent you on a quest! Perhaps you're thinking of someone else.", ch->name );
         do_say_to_char( questman, buf );
         return;
      }

      if( xIS_SET( ch->act, PLR_QUESTING ) )
      {
         bool obj_found = FALSE;
         if( ch->pcdata->questobj > 0 && ch->pcdata->countdown > 0 )
         {
            for( obj = ch->first_carrying; obj; obj = obj_next )
            {
               obj_next = obj->next_content;

               if( obj && obj->pIndexData->vnum == ch->pcdata->questobj )
               {

                  obj_found = TRUE;
                  obj_from_char( obj );
                  extract_obj( obj );
                  break;
               }
            }
         }

         if( ( ch->pcdata->questmob == -1 || ( ch->pcdata->questobj && obj_found ) ) && ch->pcdata->countdown > 0 )
         {
            long reward = 0;
            int pointreward = 0;

            reward = number_range( 2000, 25000 );
            pointreward = number_range( 40, 160 );

            sprintf( buf, "%s Congratulations on completing your quest!", ch->name );
            do_say_to_char( questman, buf );
            sprintf( buf, "%s As a reward, I am giving you %d glory points, and %ld katyr.", ch->name, pointreward, reward );
            do_say_to_char( questman, buf );

            xREMOVE_BIT( ch->act, PLR_QUESTING );
            ch->pcdata->questgiver = NULL;
            ch->pcdata->countdown = 0;
            ch->pcdata->questmob = 0;
            ch->pcdata->questobj = 0;
            ch->pcdata->nextquest = 10;
            ch->gold += reward;
            ch->pcdata->quest_curr += pointreward;
            ch->pcdata->quest_accum += pointreward;
            return;
         }
         else if( ( ch->pcdata->questmob > 0 || ch->pcdata->questobj > 0 ) && ch->pcdata->countdown > 0 )
         {
            sprintf( buf, "%s You haven't completed the quest yet, but there is still time!", ch->name );
            do_say_to_char( questman, buf );
            return;
         }
      }
      if( ch->pcdata->nextquest > 0 )
         sprintf( buf, "But you didn't complete your quest in time!" );
      else
         sprintf( buf, "%s You have to request a quest first, %s.", ch->name, ch->name );
      do_say_to_char( questman, buf );
      return;
   }

   else if( !strcmp( arg1, "quit" ) )
   {
      act( AT_GREY, "$n informs $N $e wishes to quit $s quest.", ch, NULL, questman, TO_ROOM );
      act( AT_GREY, "You inform $N you wish to quit $s quest.", ch, NULL, questman, TO_CHAR );
      if( ch->pcdata->questgiver != questman )
      {
         sprintf( buf, "%s I never sent you on a quest! Perhaps you're thinking of someone else.", ch->name );
         do_say_to_char( questman, buf );
         return;
      }

      if( xIS_SET( ch->act, PLR_QUESTING ) )
      {
         xREMOVE_BIT( ch->act, PLR_QUESTING );
         ch->pcdata->questgiver = NULL;
         ch->pcdata->countdown = 0;
         ch->pcdata->questmob = 0;
         ch->pcdata->questobj = 0;
         ch->pcdata->nextquest = 30;
         sprintf( buf, "%s Your quest is over, but for your cowardly behavior, you may not quest again for 30 minutes.",
                  ch->name );
         do_say_to_char( questman, buf );
         return;
      }
      else
      {
         send_to_char( "You aren't on a quest!", ch );
         return;
      }
   }


   send_to_char( "Qwest commands: Info, Time, Request, Complete, Quit, List, and Buy.\n\r", ch );
//    send_to_char("For more information, type 'Help Qwest'.\n\r",ch);
   return;
}

void generate_quest( CHAR_DATA * ch, CHAR_DATA * questman )
{
   CHAR_DATA *victim;
   MOB_INDEX_DATA *vsearch;
   ROOM_INDEX_DATA *room;
   OBJ_DATA *questitem;
   char buf[MAX_STRING_LENGTH];
   long mcounter;
   int mob_vnum;


   for( mcounter = 0; mcounter < 99999; mcounter++ )
   {
      mob_vnum = number_range( 100, 32600 );

      if( ( vsearch = get_mob_index( mob_vnum ) ) != NULL )
      {
         if( quest_level_diff( ch->level, vsearch->level ) == TRUE
             && vsearch->pShop == NULL
             && !xIS_SET( vsearch->act, ACT_PRACTICE )
             && !xIS_SET( vsearch->act, ACT_PET )
             && !IS_AFFECTED( vsearch, AFF_CHARM ) && !IS_AFFECTED( vsearch, AFF_INVISIBLE ) && number_percent(  ) < 40 )
            break;
         else
            vsearch = NULL;
      }
   }

   if( vsearch == NULL || ( victim = get_char_world( ch, vsearch->player_name ) ) == NULL
       || ( victim && !IS_NPC( victim ) ) )
   {
      sprintf( buf, "%s I'm sorry, but I don't have any quests for you at this time.", ch->name );
      do_say_to_char( questman, buf );
      sprintf( buf, "%s Try again later.", ch->name );
      do_say_to_char( questman, buf );
      return;
   }

   if( ( ( room = find_location( ch, victim->name ) ) == NULL )
       || xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) || !room->area )
   {
      sprintf( buf, "%s I'm sorry, but I don't have any quests for you at this time.", ch->name );
      do_say_to_char( questman, buf );
      sprintf( buf, "%s Try again later.", ch->name );
      do_say_to_char( questman, buf );
      return;
   }

   xSET_BIT( room->room_flags, ROOM_NO_ASTRAL );
   xSET_BIT( room->room_flags, ROOM_NO_SUMMON );

   if( number_percent(  ) < 40 )
   {
      int numobjs = 0;
      int descnum = 0;

      for( numobjs = 0; quest_desc[numobjs].name != NULL; numobjs++ )
         ;
      numobjs--;
      descnum = number_range( 0, numobjs );
      quest_num = QUEST_OBJ(  );
      questitem = create_object( get_obj_index( quest_num ), ch->level );

      if( !questitem )
      {
         sprintf( buf, "%s Sorry, come back later", ch->name );
         do_say_to_char( questman, buf );
         bug( "Quest master cannot find quest item!", 0 );
         return;
      }

      if( descnum > -1 )
      {
         if( questitem->short_descr )
            STRFREE( questitem->short_descr );
         if( questitem->description )
            STRFREE( questitem->description );
         if( questitem->name )
            STRFREE( questitem->name );

         questitem->name = STRALLOC( quest_desc[descnum].name );
         questitem->description = STRALLOC( quest_desc[descnum].long_descr );
         questitem->short_descr = STRALLOC( quest_desc[descnum].short_descr );
      }
      obj_to_room( questitem, room );
      ch->pcdata->questobj = questitem->pIndexData->vnum;
      ch->pcdata->questinroom = room->vnum;
      sprintf( buf, "%s Thieves have stolen %s from the Temple!", ch->name, questitem->short_descr );
      do_say_to_char( questman, buf );
      sprintf( buf, "%s The Gods have told me its location.", ch->name );
      do_say_to_char( questman, buf );

      sprintf( buf, "%s Look in %s for %s!, there you will find it!", ch->name, room->area->name, room->name );
      do_say_to_char( questman, buf );
      return;
   }

   else
   {
      sprintf( buf, "%s %s, has upset the Gods!", ch->name, victim->short_descr );
      do_say_to_char( questman, buf );
      sprintf( buf, "%s They have sentanced Death for this vile creature!", ch->name );
      do_say_to_char( questman, buf );

      if( room->name != NULL )
      {
         sprintf( buf, "%s Find %s in %s! And carry out the Sentance!", ch->name, victim->short_descr, room->name );
         do_say_to_char( questman, buf );

         sprintf( buf, "%s That location is in the general area of %s.", ch->name, room->area->name );
         do_say_to_char( questman, buf );
      }
      ch->pcdata->questmob = victim->pIndexData->vnum;
      ch->pcdata->questinroom = victim->in_room->vnum;
   }

   return;
}

bool quest_level_diff( int clevel, int mlevel )
{
   if( clevel < 9 && mlevel < clevel + 2 )
      return TRUE;
   else if( clevel <= 9 && mlevel < clevel + 3 && mlevel > clevel - 5 )
      return TRUE;
   else if( clevel <= 100 && mlevel < clevel + 4 && mlevel > clevel - 5 )
      return TRUE;
   else if( clevel <= 200 && mlevel < clevel + 5 && mlevel > clevel - 4 )
      return TRUE;
   else if( clevel <= 300 && mlevel < clevel + 6 && mlevel > clevel - 3 )
      return TRUE;
   else if( clevel <= 400 && mlevel < clevel + 7 && mlevel > clevel - 2 )
      return TRUE;
   else if( clevel <= 600 && mlevel < clevel + 8 && mlevel > clevel - 1 )
      return TRUE;
   else if( clevel > 600 )
      return TRUE;
   else
      return FALSE;
}

void quest_update( void )
{
   DESCRIPTOR_DATA *d;
   CHAR_DATA *ch;
   char buf[MSL];

   for( d = first_descriptor; d != NULL; d = d->next )
   {
      if( d->character != NULL && d->connected == CON_PLAYING )
      {
         ch = d->character;
         if( IS_NPC( ch ) )
            continue;
         if( ch->pcdata->nextquest > 0 )
         {
            ch->pcdata->nextquest--;
            if( ch->pcdata->nextquest == 0 )
            {
               send_to_char( "You may now quest again.\n\r", ch );
               return;
            }
         }
         else if( xIS_SET( ch->act, PLR_QUESTING ) )
         {
            if( --ch->pcdata->countdown <= 0 )
            {
               ch->pcdata->nextquest = 10;
               sprintf( buf, "You have run out of time for your"
                        "quest!\n\rYou may quest again in %d minutes.\n\r", ch->pcdata->nextquest );
               send_to_char( buf, ch );
               xREMOVE_BIT( ch->act, PLR_QUESTING );
               ch->pcdata->questgiver = NULL;
               ch->pcdata->countdown = 0;
               ch->pcdata->questmob = 0;
               ch->pcdata->questobj = 0;
            }
            if( ch->pcdata->countdown > 0 && ch->pcdata->countdown < 6 )
            {
               send_to_char( "Better hurry, you're almost out of time for your quest!\n\r", ch );
               return;
            }
         }
      }
   }
   return;
}

int QUEST_OBJ( void )
{

   switch ( number_range( 1, 4 ) )
   {

      case 1:
         quest_num = 60;
         break;
      case 2:
         quest_num = 61;
         break;
      case 3:
         quest_num = 62;
         break;
      case 4:
         quest_num = 63;
         break;
   }

   if( quest_num == 0 )
      bug( "Quest Num == 0! (QUEST_OBJ: quest-master.c)", 0 );

   return quest_num;
}

void quest_death_check( CHAR_DATA * ch, CHAR_DATA * victim )
{

   if( !ch || !victim )
      return;

   if( IS_NPC( ch ) || !IS_NPC( victim ) || ch->pcdata->questmob == 0 )
      return;

   if( victim->pIndexData->vnum == ch->pcdata->questmob )
   {
      ch->pcdata->questmob = -1;
      send_to_char( "\n\rYou feel fulfilled, as you complete your quest!\n\r", ch );
      send_to_char( "Find your QuestMaster! Type qwest complete when your there!\n\r", ch );
      return;
   }

   return;
}
