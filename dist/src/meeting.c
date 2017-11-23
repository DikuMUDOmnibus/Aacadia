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
/*						Meeting module				                    */
/************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "acadia.h"

int meeting_count;
time_t start_time;
bool existing_meeting;
bool transcribe_meeting;
int yes, no, abstain;
bool silence;
char subject[MAX_INPUT_LENGTH];
int subji;

void exit_meeting( DESCRIPTOR_DATA * d );

void do_smeeting( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char log_buf[MAX_INPUT_LENGTH];
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;

   argument = one_argument( argument, arg );
   one_argument( argument, arg1 );
   if( arg[0] == '\0' || arg1[0] == '\0' )
   {
      send_to_char( "Syntax: smeeting <type> <subject>", ch );
      return;
   }

   if( atoi( arg ) == 6 )
   {
      subji = 6;
      sprintf( subject, arg1 );
   }
   else if( atoi( arg ) == 5 )
   {
      subji = 5;
      sprintf( subject, arg1 );
   }
   else if( atoi( arg ) == 4 )
   {
      subji = 4;
      sprintf( subject, arg1 );
   }
   else if( atoi( arg ) == 3 )
   {
      subji = 3;
      sprintf( subject, arg1 );
   }
   else if( atoi( arg ) == 2 )
   {
      subji = 2;
      sprintf( subject, arg1 );
   }
   else if( atoi( arg ) == 1 )
   {
      subji = 1;
      sprintf( subject, arg1 );
   }
   else if( atoi( arg ) == 0 )
   {
      subji = 0;
      sprintf( subject, arg1 );
   }

   if( existing_meeting )
   {
      send_to_char( "&RMeeting already exists.  Cannot start a new meeting.&w\n\r", ch );
      return;
   }

   existing_meeting = TRUE;
   transcribe_meeting = TRUE;
   silence = FALSE;
   meeting_count = 0;
   start_time = current_time;
   send_to_char( "&GMeeting started successfully.&w\n\r", ch );
   sprintf( log_buf, "%s created a meeting.", ch->name );
   log_string_plus( log_buf, LOG_COMM, LEVEL_IMMORTAL );
   if( subji == 6 )
   {
      for( vch = first_char; vch; vch = vch_next )
      {
         vch_next = vch->next;

         if( !IS_NPC( vch ) && vch->level >= 980 )
         {
            send_to_char
               ( "&R<&BINFO&R> &pA meeting has been started. Type meeting info for info, or meeting join to join.&D", vch );
         }
      }
   }
   else if( subji == 5 )
   {
      for( vch = first_char; vch; vch = vch_next )
      {
         vch_next = vch->next;

         if( !IS_NPC( vch ) && vch->level >= 800 )
         {
            send_to_char
               ( "&R<&BINFO&R> &pA meeting has been started. Type meeting info for info, or meeting join to join.&D", vch );
         }
      }
   }
   else if( subji == 4 )
   {
      for( vch = first_char; vch; vch = vch_next )
      {
         vch_next = vch->next;

         if( !IS_NPC( vch ) && vch->level >= 400 )
         {
            send_to_char
               ( "&R<&BINFO&R> &pA meeting has been started. Type meeting info for info, or meeting join to join.&D", vch );
         }
      }
   }
   else if( subji == 3 )
   {
      for( vch = first_char; vch; vch = vch_next )
      {
         vch_next = vch->next;

         if( !IS_NPC( vch ) && vch->level >= 300 )
         {
            send_to_char
               ( "&R<&BINFO&R> &pA meeting has been started. Type meeting info for info, or meeting join to join.&D", vch );
         }
      }
   }
   else if( subji == 2 )
   {
      for( vch = first_char; vch; vch = vch_next )
      {
         vch_next = vch->next;

         if( !IS_NPC( vch ) && vch->level >= 200 )
         {
            send_to_char
               ( "&R<&BINFO&R> &pA meeting has been started. Type meeting info for info, or meeting join to join.&D", vch );
         }
      }
   }
   else if( subji == 1 )
   {
      for( vch = first_char; vch; vch = vch_next )
      {
         vch_next = vch->next;

         if( !IS_NPC( vch ) && vch->level >= 100 )
         {
            send_to_char
               ( "&R<&BINFO&R> &pA meeting has been started. Type meeting info for info, or meeting join to join.&D", vch );
         }
      }
   }
   else
   {
      send_to_char( "&R<&BINFO&R> &pA meeting has been started. Type meeting info for info, or meeting join to join.&D",
                    ch );
   }
   return;
}

void do_cmeeting( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_INPUT_LENGTH];
   char log_buf[MAX_INPUT_LENGTH];
   DESCRIPTOR_DATA *d;
   CHAR_DATA *vch = NULL;
   CHAR_DATA *vch_next = NULL;

   if( !existing_meeting )
   {
      send_to_char( "&RNo meeting exists!&w\n\r", ch );
      return;
   }

   if( meeting_count > 0 )
   {
      for( d = first_descriptor; d; d = d->next )
         if( d->connected == CON_MEETING )
         {
            send_to_char( "&RMeeting is closed.&w\n\r", d->character );
            exit_meeting( d );
         }

   }

   if( transcribe_meeting )
   {
      sprintf( buf, "\n\rMeeting Closed: %s\n\r", ( char * )ctime( &current_time ) );
      append_to_file( MEETING_FILE, buf );
   }
   meeting_count = 0;
   transcribe_meeting = FALSE;
   existing_meeting = FALSE;

   send_to_char( "&GMeeting closed successfully.&w\n\r", ch );
   sprintf( log_buf, "%s closed the meeting.", ch->name );
   log_string_plus( log_buf, LOG_COMM, LEVEL_IMMORTAL );
   if( subji == 6 )
   {
      for( vch = first_char; vch; vch = vch_next )
      {
         vch_next = vch->next;

         if( !IS_NPC( vch ) && vch->level >= 980 )
         {
            send_to_char( "&R<&BINFO&R> &pThe meeting has been closed.&D", vch );
         }
      }
   }
   else if( subji == 5 )
   {
      for( vch = first_char; vch; vch = vch_next )
      {
         vch_next = vch->next;

         if( !IS_NPC( vch ) && vch->level >= 800 )
         {
            send_to_char( "&R<&BINFO&R> &pThe meeting has been closed.&D", vch );
         }
      }
   }
   else if( subji == 4 )
   {
      for( vch = first_char; vch; vch = vch_next )
      {
         vch_next = vch->next;

         if( !IS_NPC( vch ) && vch->level >= 400 )
         {
            send_to_char( "&R<&BINFO&R> &pThe meeting has been closed.&D", vch );
         }
      }
   }
   else if( subji == 3 )
   {
      for( vch = first_char; vch; vch = vch_next )
      {
         vch_next = vch->next;

         if( !IS_NPC( vch ) && vch->level >= 300 )
         {
            send_to_char( "&R<&BINFO&R> &pThe meeting has been closed.&D", vch );
         }
      }
   }
   else if( subji == 2 )
   {
      for( vch = first_char; vch; vch = vch_next )
      {
         vch_next = vch->next;

         if( !IS_NPC( vch ) && vch->level >= 200 )
         {
            send_to_char( "&R<&BINFO&R> &pThe meeting has been closed.&D", vch );
         }
      }
   }
   else if( subji == 1 )
   {
      for( vch = first_char; vch; vch = vch_next )
      {
         vch_next = vch->next;

         if( !IS_NPC( vch ) && vch->level >= 100 )
         {
            send_to_char( "&R<&BINFO&R> &pThe meeting has been closed.&D", vch );
         }
      }
   }
   else
   {
      for( vch = first_char; vch; vch = vch_next )
      {
         vch_next = vch->next;

         send_to_char( "&R<&BINFO&R> &pThe meeting has been closed.&D", vch );
      }
   }
   return;
}

void do_meeting( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   DESCRIPTOR_DATA *e;
   char formsubj[MAX_INPUT_LENGTH];

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "&YSyntax: meeting <join | info>&w\n\r", ch );
      return;
   }

   if( existing_meeting )
   {

      if( !str_cmp( arg, "join" ) )
      {
         if( subji == 6 && ch->level < 980 )
         {
            send_to_char( "You cannot join this meeting.\n\r", ch );
            return;
         }
         else if( subji == 5 && ch->level < 800 )
         {
            send_to_char( "You cannot join this meeting.\n\r", ch );
            return;
         }
         else if( subji == 4 && ch->level < 400 )
         {
            send_to_char( "You cannot join this meeting.\n\r", ch );
            return;
         }
         else if( subji == 3 && ch->level < 300 )
         {
            send_to_char( "You cannot join this meeting.\n\r", ch );
            return;
         }
         else if( subji == 2 && ch->level < 200 )
         {
            send_to_char( "You cannot join this meeting.\n\r", ch );
            return;
         }
         else if( subji == 1 && ch->level < 100 )
         {
            send_to_char( "You cannot join this meeting.\n\r", ch );
            return;
         }
         ch->premeeting_vnum = ch->in_room->vnum;
         char_from_room( ch );
         char_to_room( ch, get_room_index( ROOM_VNUM_MEETING ) );
         for( e = first_descriptor; e; e = e->next )
            if( e->connected == CON_MEETING )
            {
               sprintf( buf, "&W%s has joined the meeting.&w\n\r", ch->name );
               send_to_char( buf, e->character );
            }
         ch->desc->connected = CON_MEETING;
         ch->voted = TRUE;
         send_to_char( "&GJoining the meeting...&w\n\r", ch );
         meeting_count++;
         return;
      }

      if( subji == 6 )
      {
         sprintf( formsubj, "Host" );
      }
      else if( subji == 5 )
      {
         sprintf( formsubj, "Immortal" );
      }
      else if( subji == 4 )
      {
         sprintf( formsubj, "Acolyte" );
      }
      else if( subji == 3 )
      {
         sprintf( formsubj, "Neophyte" );
      }
      else if( subji == 2 )
      {
         sprintf( formsubj, "Avatar" );
      }
      else if( subji == 1 )
      {
         sprintf( formsubj, "Hero" );
      }
      else
      {
         sprintf( formsubj, "All" );
      }
      if( !str_cmp( arg, "info" ) )
      {
         sprintf( buf, "&BCurrent Meeting Info\n\r--------------------\n\r" );
         sprintf( buf, "%s&YStart Time: &W%s\n\r", buf, ( char * )ctime( &start_time ) );
         sprintf( buf, "%s&YMeeting Target: &W%s\n\r", buf, formsubj );
         sprintf( buf, "%s&YMeeting Subject: &W%s\n\r", buf, subject );
         sprintf( buf, "%s&YCurrent Attendance: &W%2d\n\r", buf, meeting_count );
         sprintf( buf, "%s&YTranscribing: &W%s\n\r", buf, transcribe_meeting ? "Yes" : "No" );

         send_to_char( buf, ch );
         return;
      }
   }
   else
   {
      send_to_char( "&RThere is not a meeting currently open.&w\n\r", ch );
      return;
   }

}

void exit_meeting( DESCRIPTOR_DATA * d )
{
   CHAR_DATA *ch = d->character;

   send_to_char( "&RExiting the meeting...&w\n\r", ch );
   ch->desc->connected = CON_PLAYING;
   meeting_count--;

   char_from_room( ch );
   char_to_room( ch, get_room_index( ROOM_VNUM_MEETING ) );
}

void meeting_interpret( CHAR_DATA * ch, char *argument )
{
   DESCRIPTOR_DATA *d, *e;
   CHAR_DATA *victim;
   char buf[MAX_INPUT_LENGTH];
   char filebuf[MAX_INPUT_LENGTH];
   char arg[MAX_INPUT_LENGTH];

   d = ch->desc;
   if( d == NULL )
   {
      send_to_char( "You have no descriptor.\n\r", ch );
      return;
   }

   if( d->connected != CON_MEETING )
   {
      send_to_char( "You can't do that!\n\r", ch );
      bug( "Meeting_interpret: d->connected != CON_MEETING", 0 );
      return;
   }

   if( argument[0] == '`' )
   {
      argument = one_argument( argument, arg );

      if( !str_cmp( arg, "`exit" ) )
      {
         exit_meeting( d );
         for( e = first_descriptor; e; e = e->next )
            if( e->connected == CON_MEETING )
            {
               sprintf( buf, "&W%s has left the meeting.&w\n\r", ch->name );
               send_to_char( buf, e->character );
            }
         return;
      }
      else if( !str_cmp( arg, "`info" ) )
      {
         do_meeting( ch, "info" );
         return;
      }
      else if( !str_cmp( arg, "`hand" ) )
      {
         send_to_char( "&YRequest for the floor sent.&w\n\r", ch );
         for( e = first_descriptor; e; e = e->next )
         {
            if( ( e->connected == CON_MEETING ) && ( e->character->level >= LEVEL_HEADBUILD ) )
            {
               sprintf( buf, "&C%s &Ywishes to say something.&w\n\r", ch->name );
               send_to_char( buf, e->character );
            }
         }
         return;
      }
      else if( !str_cmp( arg, "`who" ) )
      {
         sprintf( buf, "&BCurrent Meeting Attendance Roster\n\r---------------------------------&Y\n\r" );

         for( e = first_descriptor; e; e = e->next )
            if( e->connected == CON_MEETING )
               sprintf( buf, "%s%s\n\r", buf, e->character->name );

         sprintf( buf, "%s&w", buf );
         send_to_char( buf, ch );
         return;
      }
      else if( ( !str_cmp( arg, "`kick" ) ) && ( ch->level >= LEVEL_HEADBUILD ) )
      {
         victim = get_char_world( ch, argument );
         if( ( victim == NULL ) || ( victim->desc->connected != CON_MEETING ) )
         {
            send_to_char( "&YThey aren't here.&w\n\r", ch );
            return;
         }

         if( get_trust( ch ) <= get_trust( victim ) )
         {
            send_to_char( "&YYou cannot do that.&w\n\r", ch );
            return;
         }

         sprintf( buf, "&YYou have been kicked from the meeting by %s.\n\r", ch->name );
         sprintf( buf, "&YPlease do not attempt to re-enter the meeting or other disciplinary actions will follow.&w\n\r" );
         send_to_char( buf, victim );
         exit_meeting( victim->desc );
         return;
      }
      else if( !str_cmp( arg, "`vote" ) )
      {
         if( !ch->voted )
            if( !str_cmp( argument, "yes" ) )
            {
               yes++;
               send_to_char( "&YVote recorded.&w\n\r", ch );
               ch->voted = TRUE;
               return;
            }
            else if( !str_cmp( argument, "no" ) )
            {
               no++;
               send_to_char( "&YVote recorded.&w\n\r", ch );
               ch->voted = TRUE;
               return;
            }
            else if( !str_cmp( argument, "abstain" ) )
            {
               abstain++;
               send_to_char( "&YVote recorded.&w\n\r", ch );
               ch->voted = TRUE;
               return;
            }
            else
            {
               send_to_char( "Syntax: `vote <yes | no | abstain>\n\r", ch );
               return;
            }
         else
            send_to_char( "&YYou've already voted on this issue!&w\n\r", ch );
      }
      else if( ( !str_cmp( arg, "`voteshow" ) ) && ( ch->level >= LEVEL_HEADBUILD ) )
      {
         for( e = first_descriptor; e; e = e->next )
            if( e->connected == CON_MEETING )
            {
               sprintf( buf, "&BResults of the Last Vote:\n\r" );
               sprintf( buf, "%s    &GYes:   &W%2d\n\r", buf, yes );
               sprintf( buf, "%s     &RNo:   &W%2d\n\r", buf, no );
               sprintf( buf, "%s&YAbstain:   &W%2d&w\n\r", buf, abstain );
               send_to_char( buf, e->character );
            }
         if( transcribe_meeting )
            sprintf( filebuf, "Results of the Last Vote:\n\r" );
         sprintf( filebuf, "%s    Yes:   %2d\n\r", filebuf, yes );
         sprintf( filebuf, "%s     No:   %2d\n\r", filebuf, no );
         sprintf( filebuf, "%sAbstain:   %2d\n\r", filebuf, abstain );

         append_to_file( MEETING_FILE, filebuf );
         return;
      }
      else if( ( !str_cmp( arg, "`voteclear" ) ) && ( ch->level >= LEVEL_HEADBUILD ) )
      {
         yes = 0;
         no = 0;
         abstain = 0;
         for( e = first_descriptor; e; e = e->next )
            if( e->connected == CON_MEETING )
               e->character->voted = FALSE;
         send_to_char( "Vote Cleared.\n\r", ch );
         return;
      }
      else if( ( !str_cmp( arg, "`silence" ) ) && ( ch->level >= LEVEL_HEADBUILD ) )
      {
         silence = !silence;
         if( silence )
            send_to_char( "&YSilent mode on.&w\n\r", ch );
         else
            send_to_char( "&YSilent mode off.&w\n\r", ch );
         return;
      }
      else if( ( !str_cmp( arg, "`transcribe" ) ) && ( ch->level >= LEVEL_HEADBUILD ) )
      {
         transcribe_meeting = !transcribe_meeting;
         if( transcribe_meeting )
            send_to_char( "&YNow transcribing...&w\n\r", ch );
         else
            send_to_char( "&YNo longer transcribing.&w\n\r", ch );
         return;
      }
      else if( !str_cmp( arg, "`help" ) )
      {
         sprintf( buf, "&RMeeting Commands:\n\r" );
         sprintf( buf, "%s  &W`help:\t\t&YThis help page.\n\r", buf );
         sprintf( buf, "%s  &W`exit:\t\t&YExit the meeting.\n\r", buf );
         sprintf( buf, "%s  &W`info:\t\t&YMeeting Information.\n\r", buf );
         sprintf( buf, "%s  &W`hand:\t\t&YRequest to speak.\n\r", buf );
         sprintf( buf, "%s  &W`who :\t\t&YMeeting Attendee List.\n\r", buf );
         sprintf( buf, "%s  &W`vote:\t\t&Yyes, no, or abstain.&w\n\r", buf );

         if( ch->level >= LEVEL_HEADBUILD )
         {
            sprintf( buf, "%s &G&W`voteshow:\t&YShow current vote totals.\n\r", buf );
            sprintf( buf, "%s &W`voteclear:\t&YClear current vote totals.\n\r", buf );
            sprintf( buf, "%s &W`transcribe:\t&YToggle meeting logging.\n\r", buf );
            sprintf( buf, "%s &W`silence:\t&YToggle silence mode.\n\r", buf );
            sprintf( buf, "%s &W`kick:\t\t&YKick specified character from meeting.&w\n\r", buf );
         }

         send_to_char( buf, ch );
         return;
      }
      else
      {
         send_to_char( "&YInvalid command.&w\n\r", ch );
         return;
      }

   }
   else if( !silence )
   {
      for( e = first_descriptor; e; e = e->next )
         if( e->connected == CON_MEETING )
         {
            switch ( ch->level )
            {
               case LEVEL_NEOPHYTE:
                  sprintf( buf, "&G&W(&B%s&G&W) &Y%s&w\n\r", ch->name, argument );
                  break;
               case LEVEL_GODS:
                  sprintf( buf, "&G&W(&C%s&G&W) &Y%s&w\n\r", ch->name, argument );
                  break;
               case LEVEL_HEADBUILD:
                  sprintf( buf, "&G&W(&C%s&G&W) &Y%s&w\n\r", ch->name, argument );
                  break;
               case LEVEL_ANGEL:
                  sprintf( buf, "&G&W(&p%s&G&W) &Y%s&w\n\r", ch->name, argument );
                  break;
               case LEVEL_ADMIN:
                  sprintf( buf, "&G&W(&R%s&G&W) &Y%s&w\n\r", ch->name, argument );
                  break;
               case LEVEL_ADVBUILD:
                  sprintf( buf, "&G&W(&G%s&G&W) &Y%s&w\n\r", ch->name, argument );
                  break;
               case LEVEL_VISITOR:
                  sprintf( buf, "&G&W(&G%s&G&W) &Y%s&w\n\r", ch->name, argument );
                  break;
               case LEVEL_BUILD:
                  sprintf( buf, "&G&W(&G%s&G&W) &Y%s&w\n\r", ch->name, argument );
                  break;
               case LEVEL_AVATAR:
                  sprintf( buf, "&G&W(&g%s&G&W) &Y%s&w\n\r", ch->name, argument );
                  break;
               case LEVEL_ADMINADVISOR:
                  sprintf( buf, "&G&W(&c%s&G&W) &Y%s&w\n\r", ch->name, argument );
                  break;
               case LEVEL_HERO:
                  sprintf( buf, "&G&W(&P%s&G&W) &Y%s&w\n\r", ch->name, argument );
                  break;
               case LEVEL_ACOLYTE:
                  sprintf( buf, "&G&W(&O%s&G&W) &Y%s&w\n\r", ch->name, argument );
                  break;
               default:
                  sprintf( buf, "&G&W(&z%s&W) &Y%s&w\n\r", ch->name, argument );
                  break;
            }

            send_to_char( buf, e->character );
         }
      if( transcribe_meeting )
         sprintf( filebuf, "(%s) %s\n\r", ch->name, argument );
      append_to_file( MEETING_FILE, filebuf );

      return;
   }
   else
   {
      send_to_char( "&RThe Meeting is in Silent Mode.  For a list of commands type `help.\n\r", ch );
      return;
   }

}
