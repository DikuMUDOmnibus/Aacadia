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
/*			   Player communication module			*/
/************************************************************************/


#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef REGEX
#include <regex.h>
#endif

#ifdef FREEBSD
#include <unistd.h>
#include <regex.h>
#endif
#include "acadia.h"


#ifdef REGEX
extern int re_exec _RE_ARGS( ( const char * ) );
#endif

void send_obj_page_to_char( CHAR_DATA * ch, OBJ_INDEX_DATA * idx, char page );
void send_room_page_to_char( CHAR_DATA * ch, ROOM_INDEX_DATA * idx, char page );
void send_page_to_char( CHAR_DATA * ch, MOB_INDEX_DATA * idx, char page );
void send_control_page_to_char( CHAR_DATA * ch, char page );
void sportschan( char * );

void talk_channel args( ( CHAR_DATA * ch, char *argument, int channel, const char *verb ) );

char *scramblechan args( ( const char *argument, int modifier ) );
char *drunk_speech args( ( const char *argument, CHAR_DATA * ch ) );

void add_profane_word( char *word );
//int is_profane (char *what);
bool is_profane args( ( char *what ) );

char *bigregex = NULL;
char *preg;



char *scramblechan( const char *argument, int modifier )
{
   static char arg[MAX_INPUT_LENGTH];
   sh_int position;
   sh_int conversion = 0;

   modifier %= number_range( 80, 300 );
   for( position = 0; position < MAX_INPUT_LENGTH; position++ )
   {
      if( argument[position] == '\0' )
      {
         arg[position] = '\0';
         return arg;
      }
      else if( argument[position] >= 'A' && argument[position] <= 'Z' )
      {
         conversion = -conversion + position - modifier + argument[position] - 'A';
         conversion = number_range( conversion - 5, conversion + 5 );
         while( conversion > 25 )
            conversion -= 26;
         while( conversion < 0 )
            conversion += 26;
         arg[position] = conversion + 'A';
      }
      else if( argument[position] >= 'a' && argument[position] <= 'z' )
      {
         conversion = -conversion + position - modifier + argument[position] - 'a';
         conversion = number_range( conversion - 5, conversion + 5 );
         while( conversion > 25 )
            conversion -= 26;
         while( conversion < 0 )
            conversion += 26;
         arg[position] = conversion + 'a';
      }
      else if( argument[position] >= '0' && argument[position] <= '9' )
      {
         conversion = -conversion + position - modifier + argument[position] - '0';
         conversion = number_range( conversion - 2, conversion + 2 );
         while( conversion > 9 )
            conversion -= 10;
         while( conversion < 0 )
            conversion += 10;
         arg[position] = conversion + '0';
      }
      else
         arg[position] = argument[position];
   }
   arg[position] = '\0';
   return arg;
}


LANG_DATA *get_lang( const char *name )
{
   LANG_DATA *lng;

   for( lng = first_lang; lng; lng = lng->next )
      if( !str_cmp( lng->name, name ) )
         return lng;
   return NULL;
}

char *translate( int percent, const char *in, const char *name )
{
   LCNV_DATA *cnv;
   static char buf[256];
   char buf2[256];
   const char *pbuf;
   char *pbuf2 = buf2;
   LANG_DATA *lng;

   if( percent > 99 || !str_cmp( name, "common" ) )
      return ( char * )in;

   if( !( lng = get_lang( name ) ) )
      if( !( lng = get_lang( "default" ) ) )
         return ( char * )in;

   for( pbuf = in; *pbuf; )
   {
      for( cnv = lng->first_precnv; cnv; cnv = cnv->next )
      {
         if( !str_prefix( cnv->old, pbuf ) )
         {
            if( percent && ( rand(  ) % 100 ) < percent )
            {
               strncpy( pbuf2, pbuf, cnv->olen );
               pbuf2[cnv->olen] = '\0';
               pbuf2 += cnv->olen;
            }
            else
            {
               strcpy( pbuf2, cnv->new );
               pbuf2 += cnv->nlen;
            }
            pbuf += cnv->olen;
            break;
         }
      }
      if( !cnv )
      {
         if( isalpha( *pbuf ) && ( !percent || ( rand(  ) % 100 ) > percent ) )
         {
            *pbuf2 = lng->alphabet[LOWER( *pbuf ) - 'a'];
            if( isupper( *pbuf ) )
               *pbuf2 = UPPER( *pbuf2 );
         }
         else
            *pbuf2 = *pbuf;
         pbuf++;
         pbuf2++;
      }
   }
   *pbuf2 = '\0';
   for( pbuf = buf2, pbuf2 = buf; *pbuf; )
   {
      for( cnv = lng->first_cnv; cnv; cnv = cnv->next )
         if( !str_prefix( cnv->old, pbuf ) )
         {
            strcpy( pbuf2, cnv->new );
            pbuf += cnv->olen;
            pbuf2 += cnv->nlen;
            break;
         }
      if( !cnv )
         *( pbuf2++ ) = *( pbuf++ );
   }
   *pbuf2 = '\0';
#if 0
   for( pbuf = in, pbuf2 = buf; *pbuf && *pbuf2; pbuf++, pbuf2++ )
      if( isupper( *pbuf ) )
         *pbuf2 = UPPER( *pbuf2 );
      else if( isspace( *pbuf ) )
         while( *pbuf2 && !isspace( *pbuf2 ) )
            pbuf2++;
#endif
   return buf;
}

char *drunk_speech( const char *argument, CHAR_DATA * ch )
{
   const char *arg = argument;
   static char buf[MAX_INPUT_LENGTH * 2];
   char buf1[MAX_INPUT_LENGTH * 2];
   sh_int drunk;
   char *txt;
   char *txt1;

   if( IS_NPC( ch ) || !ch->pcdata )
      return ( char * )argument;

   drunk = ch->pcdata->condition[COND_DRUNK];

   if( drunk <= 0 )
      return ( char * )argument;

   buf[0] = '\0';
   buf1[0] = '\0';

   if( !argument )
   {
      bug( "Drunk_speech: NULL argument", 0 );
      return "";
   }


   txt = buf;
   txt1 = buf1;

   while( *arg != '\0' )
   {
      if( toupper( *arg ) == 'T' )
      {
         if( number_percent(  ) < ( drunk * 2 ) )
         {
            *txt++ = *arg;
            *txt++ = 'h';
         }
         else
            *txt++ = *arg;
      }
      else if( toupper( *arg ) == 'X' )
      {
         if( number_percent(  ) < ( drunk * 2 / 2 ) )
         {
            *txt++ = 'c', *txt++ = 's', *txt++ = 'h';
         }
         else
            *txt++ = *arg;
      }
      else if( number_percent(  ) < ( drunk * 2 / 5 ) )
      {
         sh_int slurn = number_range( 1, 2 );
         sh_int currslur = 0;

         while( currslur < slurn )
            *txt++ = *arg, currslur++;
      }
      else
         *txt++ = *arg;

      arg++;
   };

   *txt = '\0';

   txt = buf;

   while( *txt != '\0' )
   {
      if( number_percent(  ) < ( 2 * drunk / 2.5 ) )
      {
         if( isupper( *txt ) )
            *txt1 = tolower( *txt );
         else if( islower( *txt ) )
            *txt1 = toupper( *txt );
         else
            *txt1 = *txt;
      }
      else
         *txt1 = *txt;

      txt1++, txt++;
   };

   *txt1 = '\0';
   txt1 = buf1;
   txt = buf;

   while( *txt1 != '\0' )
   {
      if( *txt1 == ' ' )
      {

         while( *txt1 == ' ' )
            *txt++ = *txt1++;

         if( ( number_percent(  ) < ( 2 * drunk / 4 ) ) && *txt1 != '\0' )
         {
            sh_int offset = number_range( 0, 2 );
            sh_int pos = 0;

            while( *txt1 != '\0' && pos < offset )
               *txt++ = *txt1++, pos++;

            if( *txt1 == ' ' )
            {
               *txt++ = *txt1++;
               continue;
            }

            pos = 0;
            offset = number_range( 2, 4 );
            while( *txt1 != '\0' && pos < offset )
            {
               *txt++ = *txt1;
               pos++;
               if( *txt1 == ' ' || pos == offset )
               {
                  txt1--;
                  break;
               }
               *txt++ = '-';
            }
            if( *txt1 != '\0' )
               txt1++;
         }
      }
      else
         *txt++ = *txt1++;
   }

   *txt = '\0';

   return buf;
}

void talk_channel( CHAR_DATA * ch, char *argument, int channel, const char *verb )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   DESCRIPTOR_DATA *d;
   int position;
   bool EMOTE = FALSE;
   struct tm *tms;
#ifndef SCRAMBLE
   int speaking = -1, lang;

   for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
      if( ch->speaking & lang_array[lang] )
      {
         speaking = lang;
         break;
      }
#endif

   if( IS_NPC( ch ) && channel == CHANNEL_CLAN )
   {
      send_to_char( "Mobs can't be in clans.\n\r", ch );
      return;
   }
   if( IS_NPC( ch ) && channel == CHANNEL_ORDER )
   {
      send_to_char( "Mobs can't be in orders.\n\r", ch );
      return;
   }

   if( IS_NPC( ch ) && channel == CHANNEL_COUNCIL )
   {
      send_to_char( "Mobs can't be in councils.\n\r", ch );
      return;
   }

   if( IS_NPC( ch ) && channel == CHANNEL_GUILD )
   {
      send_to_char( "Mobs can't be in guilds.\n\r", ch );
      return;
   }

   if( !IS_PKILL( ch ) && channel == CHANNEL_WARTALK )
   {
      if( !IS_IMMORTAL( ch ) )
      {
         send_to_char( "Peacefuls have no need to use wartalk.\n\r", ch );
         return;
      }
   }
   if( is_profane( argument ) )
   {
      if( !IS_IMMORTAL( ch ) /*|| !str_cmp( ch->name, "Neo" ) */  )
      {
         send_to_char( "You have been caught swearing and will automatically be silenced\n\rfor half an hour.\n\r", ch );
         tms = localtime( &current_time );
         tms->tm_hour += 1;
         ch->pcdata->silence_release_date = mktime( tms );
         xSET_BIT( ch->act, PLR_SILENCE );
         save_char_obj( ch );
         return;
      }
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
   {
      send_to_char( "You can't do that here.\n\r", ch );
      return;
   }

   if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      if( ch->master )
         send_to_char( "I don't think so...\n\r", ch->master );
      return;
   }

   if( argument[0] == '\0' )
   {
      sprintf( buf, "%s what?\n\r", verb );
      buf[0] = UPPER( buf[0] );
      send_to_char( buf, ch );
      return;
   }

   if( argument[0] == ',' || ( argument[0] == '*' && argument[strlen( argument ) - 1] == '*' ) )
   {
      if( argument[strlen( argument ) - 1] == '*' )
         argument[strlen( argument ) - 1] = '\0';
      if( !str_cmp( argument, "*rofl" ) )
         sprintf( argument, ",rolls on the floor laughing." );
      if( argument[1] == '\0' )
      {
         sprintf( buf, "Emote what over %s?\n\r", verb );
         send_to_char( buf, ch );
         return;
      }
      EMOTE = TRUE;
   }

   if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_SILENCE ) )
   {
      ch_printf( ch, "You can't %s.\n\r", capitalize( verb ) );
      return;
   }

   xREMOVE_BIT( ch->deaf, channel );

   if( EMOTE == FALSE )
   {
      switch ( channel )
      {
         default:
            sprintf( buf, "&C[%s] $n&C> $t&D", capitalize( verb ) );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( genLOG_FILE, buf2 );
            break;
         case CHANNEL_CLAN:
            sprintf( buf, "&W<%s> $n&W> $t&D", capitalize( verb ) );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            break;
         case CHANNEL_RACETALK:
            sprintf( buf, "&g[%s] $n&g> $t&D", capitalize( verb ) );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( genLOG_FILE, buf2 );
            break;
         case CHANNEL_TRAFFIC:
            sprintf( buf, "&C[%s] $n&C> $t&D", capitalize( verb ) );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( genLOG_FILE, buf2 );
            break;
         case CHANNEL_CHAT:
            sprintf( buf, "&C[%s] $n&C> $t&D", capitalize( verb ) );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( chatLOG_FILE, buf2 );
            break;
         case CHANNEL_IC:
            sprintf( buf, "&C[IC>> $n&C> $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( genLOG_FILE, buf2 );
            break;
         case CHANNEL_GRATZ:
            sprintf( buf, "&W$n &Wcongratulates $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            break;
         case CHANNEL_FLAME:
            sprintf( buf, "&R[Flame] $n&R> $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( genLOG_FILE, buf2 );
            break;
         case CHANNEL_WARTALK:
            sprintf( buf, "&R[%s] $n&R> $t&D", capitalize( verb ) );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( genLOG_FILE, buf2 );
            break;
         case CHANNEL_NS:
            sprintf( buf, "&w(NS) $n&w> $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            break;
         case CHANNEL_IMMTALK:
            sprintf( buf, "&w(Immortal) $n&w> $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( immLOG_FILE, buf2 );
            break;
         case CHANNEL_HBUILD:
            sprintf( buf, "&w(HeadBuild) $n&w> $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( hbLOG_FILE, buf2 );
            break;
         case CHANNEL_AVTALK:
            sprintf( buf, "&c[Avatar] $n&c> $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( avaLOG_FILE, buf2 );
            break;
         case CHANNEL_HTALK:
            sprintf( buf, "&Y[Hero] $n&Y> $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( heroLOG_FILE, buf2 );
            break;
         case CHANNEL_NEOTALK:
            sprintf( buf, "&G[Neophyte] $n&G> $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( neoLOG_FILE, buf2 );
            break;
         case CHANNEL_ACOTALK:
            sprintf( buf, "&g[Acolyte] $n&g> $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( acoLOG_FILE, buf2 );
            break;
         case CHANNEL_PRAY:
            sprintf( buf, "&P(Pray) $n&P> $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( immLOG_FILE, buf2 );
            break;
         case CHANNEL_ADMTALK:
            sprintf( buf, "&w(Admin) $n&w> $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( admLOG_FILE, buf2 );
            break;
         case CHANNEL_CODER:
            sprintf( buf, "&w(Coder) $n&w> $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( cdLOG_FILE, buf2 );
            break;
      }
   }
   else if( EMOTE == TRUE )
   {
      switch ( channel )
      {
         default:
            sprintf( buf, "&C[%s] $n $t&D", capitalize( verb ) );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( genLOG_FILE, buf2 );
            break;
         case CHANNEL_CLAN:
            sprintf( buf, "&W<%s> $n $t&D", capitalize( verb ) );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument, NULL, TO_CHAR );
            ch->position = position;
            break;
         case CHANNEL_RACETALK:
            sprintf( buf, "&g[%s] $n $t&D", capitalize( verb ) );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( genLOG_FILE, buf2 );
            break;
         case CHANNEL_TRAFFIC:
            sprintf( buf, "&C[%s] $n $t&D", capitalize( verb ) );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( genLOG_FILE, buf2 );
            break;
         case CHANNEL_CHAT:
            sprintf( buf, "&C[%s] $n $t&D", capitalize( verb ) );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( chatLOG_FILE, buf2 );
            break;
         case CHANNEL_IC:
            sprintf( buf, "&C[IC>> $n $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( genLOG_FILE, buf2 );
            break;
         case CHANNEL_GRATZ:
            sprintf( buf, "&W$n &Wcongratulates $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            break;
         case CHANNEL_FLAME:
            sprintf( buf, "&R[Flame] $n $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( genLOG_FILE, buf2 );
            break;
         case CHANNEL_WARTALK:
            sprintf( buf, "&R[%s] $n $t&D", capitalize( verb ) );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_GOSSIP, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( genLOG_FILE, buf2 );
            break;
         case CHANNEL_NS:
            sprintf( buf, "&w(NS) $n $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            break;
         case CHANNEL_IMMTALK:
            sprintf( buf, "&w(Immortal) $n $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( immLOG_FILE, buf2 );
            break;
         case CHANNEL_HBUILD:
            sprintf( buf, "&w(HeadBuild) $n $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( hbLOG_FILE, buf2 );
            break;
         case CHANNEL_AVTALK:
            sprintf( buf, "&c[Avatar] $n $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( avaLOG_FILE, buf2 );
            break;
         case CHANNEL_HTALK:
            sprintf( buf, "&Y[Hero] $n $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( heroLOG_FILE, buf2 );
            break;
         case CHANNEL_NEOTALK:
            sprintf( buf, "&G[Neophyte] $n $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( neoLOG_FILE, buf2 );
            break;
         case CHANNEL_ACOTALK:
            sprintf( buf, "&g[Acolyte] $n $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( acoLOG_FILE, buf2 );
            break;
         case CHANNEL_PRAY:
            sprintf( buf, "&P(Pray) $n $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( immLOG_FILE, buf2 );
            break;
         case CHANNEL_ADMTALK:
            sprintf( buf, "&w(Admin) $n $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( admLOG_FILE, buf2 );
            break;
         case CHANNEL_CODER:
            sprintf( buf, "&w(Coder) $n $t&D" );
            position = ch->position;
            ch->position = POS_STANDING;
            act( AT_IMMORT, buf, ch, argument + 1, NULL, TO_CHAR );
            ch->position = position;
            sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
            append_to_file( cdLOG_FILE, buf2 );
            break;
      }
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
   {
      sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
      append_to_file( LOG_FILE, buf2 );
   }


#ifdef HMM
   if( is_profane( argument ) )
   {
      sprintf( buf2, "%s Profanity warning: %s: %s (%s)", "say", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );

      puff = get_char_world( ch, "Puff" );
      if( puff != NULL )
      {
         if( ( location = get_room_index( 1 ) ) != NULL )
         {
            original = puff->in_room;
            char_from_room( puff );
            char_to_room( puff, location );
            interpret( puff, buf2 );
            char_to_room( puff, original );
         }
      }
   }
#endif

   for( d = first_descriptor; d; d = d->next )
   {
      CHAR_DATA *och;
      CHAR_DATA *vch;

      och = d->original ? d->original : d->character;
      vch = d->character;

      if( d->connected == CON_PLAYING && vch != ch && !xIS_SET( och->deaf, channel ) )
      {
         char *sbuf = "";
         char lbuf[MAX_INPUT_LENGTH + 4];
         if( ( EMOTE == FALSE ) )
            sbuf = argument;
         if( ( EMOTE == TRUE ) )
            sbuf = argument + 1;

         if( is_ignoring( och, ch ) && get_trust( ch ) <= get_trust( och ) )
            continue;

         if( IS_SET( och->pcdata->flags, PCFLAG_BUILDING ) )
            continue;
         if( xIS_SET( och->act, PLR_DEAF ) )
            continue;
         if( channel != CHANNEL_NEWBIE && NOT_AUTHED( och ) )
            continue;
         if( channel == CHANNEL_IMMTALK && !IS_AGOD( och ) )
            continue;
         if( channel == CHANNEL_HBUILD && !IS_HBUILD( och ) )
            continue;
         if( channel == CHANNEL_ADMTALK && ( get_trust( och ) < ( MAX_LEVEL - 20 ) ) )
            continue;
         if( channel == CHANNEL_CHAT && ( get_trust( och ) < 3 ) )
            continue;
         if( channel == CHANNEL_NS && ( get_trust( och ) < MAX_LEVEL ) )
            continue;
         if( channel == CHANNEL_CODER && ( get_trust( och ) < ( MAX_LEVEL - 50 ) ) )
            continue;
         if( channel == CHANNEL_WARTALK && NOT_AUTHED( och ) )
            continue;
         if( channel == CHANNEL_AVTALK && !IS_AVA( och ) )
            continue;
         if( channel == CHANNEL_HTALK && !IS_HERO( och ) )
            continue;
         if( channel == CHANNEL_ACOTALK && !IS_ACO( och ) )
            continue;
         if( channel == CHANNEL_NEOTALK && !IS_NEO( och ) )
            continue;
         if( channel == CHANNEL_HIGHGOD && get_trust( och ) < sysdata.muse_level )
            continue;
         if( channel == CHANNEL_HIGH && get_trust( och ) < sysdata.think_level )
            continue;

         if( channel == CHANNEL_TRAFFIC && !IS_IMMORTAL( och ) && !IS_IMMORTAL( ch ) )
         {
            if( ( IS_HERO( ch ) && !IS_HERO( och ) ) || ( !IS_HERO( ch ) && IS_HERO( och ) ) )
               continue;
         }

         if( channel == CHANNEL_NEWBIE && !IS_READY( och ) )
            continue;
         if( xIS_SET( vch->in_room->room_flags, ROOM_SILENCE ) )
            continue;
         if( channel == CHANNEL_YELL && vch->in_room->area != ch->in_room->area )
            continue;

         if( channel == CHANNEL_CLAN || channel == CHANNEL_ORDER || channel == CHANNEL_GUILD )
         {
            if( IS_NPC( vch ) )
               continue;
            if( vch->pcdata->clan != ch->pcdata->clan )
               continue;
         }

         if( channel == CHANNEL_COUNCIL )
         {
            if( IS_NPC( vch ) )
               continue;
            if( vch->pcdata->council != ch->pcdata->council )
               continue;
         }


         if( channel == CHANNEL_RACETALK )
            if( vch->race != ch->race )
               continue;

         if( xIS_SET( ch->act, PLR_WIZINVIS ) && can_see( vch, ch ) && IS_AGOD( vch ) )
         {
            sprintf( lbuf, "(%d) ", ( !IS_NPC( ch ) ) ? ch->pcdata->wizinvis : ch->mobinvis );
         }
         else
         {
            lbuf[0] = '\0';
         }

         position = vch->position;
         if( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL )
            vch->position = POS_STANDING;
#ifndef SCRAMBLE
         if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
         {
            int speakswell = UMIN( knows_language( vch, ch->speaking, ch ),
                                   knows_language( ch, ch->speaking, vch ) );

            if( speakswell < 85 )
               sbuf = translate( speakswell, argument, lang_names[speaking] );
         }
#else
         if( !knows_language( vch, ch->speaking, ch ) && ( !IS_NPC( ch ) || ch->speaking != 0 ) )
            sbuf = scramblechan( argument, ch->speaking );
#endif

         if( !IS_NPC( ch ) && ch->pcdata->nuisance
             && ch->pcdata->nuisance->flags > 7
             && ( number_percent(  ) < ( ( ch->pcdata->nuisance->flags - 7 ) * 10 * ch->pcdata->nuisance->power ) ) )
            sbuf = scramblechan( argument, number_range( 1, 10 ) );

         if( !IS_NPC( vch ) && vch->pcdata->nuisance &&
             vch->pcdata->nuisance->flags > 7
             && ( number_percent(  ) < ( ( vch->pcdata->nuisance->flags - 7 ) * 10 * vch->pcdata->nuisance->power ) ) )
            sbuf = scramblechan( argument, number_range( 1, 10 ) );

         MOBtrigger = FALSE;
         if( channel == CHANNEL_IMMTALK || channel == CHANNEL_AVTALK )
            act( AT_IMMORT, strcat( lbuf, buf ), ch, sbuf, vch, TO_VICT );
         else if( channel == CHANNEL_WARTALK )
            act( AT_WARTALK, strcat( lbuf, buf ), ch, sbuf, vch, TO_VICT );
         else if( channel == CHANNEL_RACETALK )
            act( AT_RACETALK, strcat( lbuf, buf ), ch, sbuf, vch, TO_VICT );
         else
            act( AT_GOSSIP, strcat( lbuf, buf ), ch, sbuf, vch, TO_VICT );
         vch->position = position;
      }
   }
/*
    if ( ( ch->level<51 )
    && ( channel != CHANNEL_WARTALK )
    && ( channel != CHANNEL_CLAN ) )
       WAIT_STATE( ch, 6 );
*/
   return;
}

void talk_info( sh_int AT_COLOR, char *argument )
{
   DESCRIPTOR_DATA *d;
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *original;
   int position;

   sprintf( buf, "%s&D", argument );

   for( d = first_descriptor; d; d = d->next )
   {
      original = d->original ? d->original : d->character;
      if( ( d->connected == CON_PLAYING ) && !xIS_SET( original->deaf, CHANNEL_INFO )
          && !xIS_SET( original->in_room->room_flags, ROOM_SILENCE ) && !xIS_SET( original->act, PLR_DEAF )
          && !IS_SET( original->pcdata->flags, PCFLAG_BUILDING ) && !NOT_AUTHED( original ) )
      {
         position = original->position;
         original->position = POS_STANDING;
         act( AT_COLOR, buf, original, NULL, NULL, TO_CHAR );
         original->position = position;
      }
   }
}


void to_channel( const char *argument, int channel, const char *verb, sh_int level, sh_int ulvl )
{
   char buf[MAX_STRING_LENGTH];
   DESCRIPTOR_DATA *d;

   if( !first_descriptor || argument[0] == '\0' )
      return;

   sprintf( buf, "%s: %s\r\n", verb, argument );

   for( d = first_descriptor; d; d = d->next )
   {
      CHAR_DATA *och;
      CHAR_DATA *vch;

      och = d->original ? d->original : d->character;
      vch = d->character;

      if( !och || !vch )
         continue;
      if( !IS_IMMORTAL( vch )
          || IS_SET( vch->pcdata->flags, PCFLAG_BUILDING )
          || xIS_SET( vch->act, PLR_DEAF )
          || ( get_trust( vch ) < sysdata.build_level && channel == CHANNEL_BUILD )
          || ( ( get_trust( vch ) < sysdata.log_level
                 || ( ulvl == MAX_LEVEL && get_trust( vch ) < MAX_LEVEL ) )
               && ( channel == CHANNEL_LOG || channel == CHANNEL_HIGH ||
                    channel == CHANNEL_WARN || channel == CHANNEL_COMM ) ) )
         continue;

      if( ( d->connected == CON_PLAYING
            || d->connected == CON_MEETING ) && !xIS_SET( och->deaf, channel ) && get_trust( vch ) >= level )
      {
         set_char_color( AT_LOG, vch );
         send_to_char_color( buf, vch );
      }
   }

   return;
}


void do_info( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];

   if( !IS_ADMIN( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   sprintf( buf, "&R<&BINFO&R> &p%s&D", argument );
   talk_info( AT_PLAIN, buf );
   return;
}

void do_chat( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) || ch->level < 3 )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_CHAT, "chat" );
   return;
}

void do_flame( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_FLAME, "flame" );
   return;
}

void do_gratz( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_GRATZ, "gratz" );
   return;
}

void do_clantalk( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   if( IS_NPC( ch ) || !ch->pcdata->clan
       || ch->pcdata->clan->clan_type == CLAN_ORDER || ch->pcdata->clan->clan_type == CLAN_GUILD )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_CLAN, ch->pcdata->clan->longname );
   return;
}

void do_newbiechat( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) || !IS_READY( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_NEWBIE, "newbiechat" );
   return;
}

void do_ot( CHAR_DATA * ch, char *argument )
{
   do_ordertalk( ch, argument );
}

void do_ordertalk( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   if( IS_NPC( ch ) || !ch->pcdata->clan || ch->pcdata->clan->clan_type != CLAN_ORDER )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_ORDER, ch->pcdata->clan->longname );
   return;
}

void do_counciltalk( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   if( IS_NPC( ch ) || !ch->pcdata->council )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_COUNCIL, "counciltalk" );
   return;
}

void do_guildtalk( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   if( IS_NPC( ch ) || !ch->pcdata->clan || ch->pcdata->clan->clan_type != CLAN_GUILD )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_GUILD, "guildtalk" );
   return;
}

void do_music( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_MUSIC, "music" );
   return;
}


void do_quest( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_QUEST, "quest" );
   return;
}

void do_ask( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_ASK, "ask" );
   return;
}



void do_answer( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_ASK, "answer" );
   return;
}


void do_ic( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_IC, "ic" );
   return;
}


void do_shout( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_SHOUT, "shout" );
//  WAIT_STATE( ch, 12 );
   return;
}



void do_yell( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_YELL, "yell" );
   return;
}

void do_pray( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   if( ch->level < 601 )
   {
      ch_printf( ch, "&P( %s &P) %s&D\n\r", IS_SNAME( ch ) ? ch->pcdata->sname : ch->name, argument );
      return;
   }
   else
   {
      talk_channel( ch, argument, CHANNEL_PRAY, "pray" );
      return;
   }
}

void do_hbuild( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   talk_channel( ch, argument, CHANNEL_HBUILD, "hbuild" );
   return;
}

void do_htalk( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   talk_channel( ch, argument, CHANNEL_HTALK, "htalk" );
   return;
}

void do_neotalk( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   talk_channel( ch, argument, CHANNEL_NEOTALK, "neotalk" );
   return;
}

void do_acotalk( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   talk_channel( ch, argument, CHANNEL_ACOTALK, "acotalk" );
   return;
}

void do_admtalk( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   talk_channel( ch, argument, CHANNEL_ADMTALK, "admtalk" );
   return;
}

void do_nstalk( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   talk_channel( ch, argument, CHANNEL_NS, "ns" );
   return;
}

void do_codetalk( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   talk_channel( ch, argument, CHANNEL_CODER, "codetalk" );
   return;
}


void do_immtalk( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   talk_channel( ch, argument, CHANNEL_IMMTALK, "immtalk" );
   return;
}


void do_muse( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_HIGHGOD, "muse" );
   return;
}


void do_think( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_CHAT, "think" );
   return;
}


void do_avtalk( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_AVTALK, "avtalk" );
   return;
}


void do_say( CHAR_DATA * ch, char *argument )
{
   char last_char;
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   CHAR_DATA *vch;
   EXT_BV actflags;
   int arglen;
#ifndef SCRAMBLE
   int speaking = -1, lang;

   for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
      if( ch->speaking & lang_array[lang] )
      {
         speaking = lang;
         break;
      }
#endif

   if( argument[0] == '\0' )
   {
      send_to_char( "Say what?\n\r", ch );
      return;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
   {
      send_to_char( "You can't do that here.\n\r", ch );
      return;
   }

   arglen = strlen( argument ) - 1;
   while( argument[arglen] == ' ' || argument[arglen] == '\t' )
      --arglen;
   last_char = argument[arglen];

   actflags = ch->act;
   if( IS_NPC( ch ) )
      xREMOVE_BIT( ch->act, ACT_SECRETIVE );
   for( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
   {
      char *sbuf = argument;

      if( vch == ch )
         continue;

      if( is_ignoring( vch, ch ) )
      {
         if( !IS_IMMORTAL( ch ) || get_trust( vch ) > get_trust( ch ) )
            continue;
         else
         {
            set_char_color( AT_IGNORE, vch );
            ch_printf( vch, "You attempt to ignore %s, but" " are unable to do so.\n\r", ch->name );
         }
      }

#ifndef SCRAMBLE
      if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
      {
         int speakswell = UMIN( knows_language( vch, ch->speaking, ch ),
                                knows_language( ch, ch->speaking, vch ) );

         if( speakswell < 75 )
            sbuf = translate( speakswell, argument, lang_names[speaking] );
      }
#else
      if( !knows_language( vch, ch->speaking, ch ) && ( !IS_NPC( ch ) || ch->speaking != 0 ) )
         sbuf = scramblechan( argument, ch->speaking );
#endif
      sbuf = drunk_speech( sbuf, ch );

      MOBtrigger = FALSE;
// act( AT_SAY, "$n&R says '$t'&D", ch, sbuf, vch, TO_VICT );
   }
   ch->act = actflags;
   MOBtrigger = FALSE;

   switch ( last_char )
   {
      case '?':
         act( AT_SAY, "&RYou ask '&w$t&R'&D", ch, drunk_speech( argument, ch ), NULL, TO_CHAR );
         act( AT_SAY, "$n&R asks '&w$t&R'&D", ch, drunk_speech( argument, ch ), NULL, TO_ROOM );
         break;

      case '!':
         act( AT_SAY, "&RYou exclaim '&w$t&R'&D", ch, drunk_speech( argument, ch ), NULL, TO_CHAR );
         act( AT_SAY, "$n&R exclaims '&w$t&R'&D", ch, drunk_speech( argument, ch ), NULL, TO_ROOM );
         break;

      default:
         act( AT_SAY, "&RYou say '&w$t&R'&D", ch, drunk_speech( argument, ch ), NULL, TO_CHAR );
         act( AT_SAY, "$n&R says '&w$t&R'&D", ch, drunk_speech( argument, ch ), NULL, TO_ROOM );
         break;
   }
   if( !IS_NPC( ch ) )
   {
      sprintf( buf2, "%s: %s (say)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument );
      append_to_file( sayLOG_FILE, buf2 );
   }

// act( AT_SAY, "You say '$T'", ch, NULL, drunk_speech( argument, ch ), TO_CHAR ); 
   if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
   {
      sprintf( buf, "%s: %s", IS_NPC( ch ) ? ch->short_descr : ch->name, argument );
      append_to_file( LOG_FILE, buf );
   }
   mprog_speech_trigger( argument, ch );
   if( char_died( ch ) )
      return;
   oprog_speech_trigger( argument, ch );
   if( char_died( ch ) )
      return;
   rprog_speech_trigger( argument, ch );
   return;
}

void do_say_to_char( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH], last_char;
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   CHAR_DATA *vch;
   CHAR_DATA *victim;
   EXT_BV actflags;
   int arglen;
#ifndef SCRAMBLE
   int speaking = -1, lang;

   for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
      if( ch->speaking & lang_array[lang] )
      {
         speaking = lang;
         break;
      }
#endif

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' || argument[0] == '\0' )
   {
      send_to_char( "Say what to whom?\n\r", ch );
      return;
   }
   if( ( victim = get_char_world( ch, arg ) ) == NULL
       || ( IS_NPC( victim ) && victim->in_room != ch->in_room )
       || ( !NOT_AUTHED( ch ) && NOT_AUTHED( victim ) && !IS_IMMORTAL( ch ) ) )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
   {
      send_to_char( "You can't do that here.\n\r", ch );
      return;
   }

   arglen = strlen( argument ) - 1;
   while( argument[arglen] == ' ' || argument[arglen] == '\t' )
      --arglen;
   last_char = argument[arglen];

   actflags = ch->act;
   if( IS_NPC( ch ) )
      xREMOVE_BIT( ch->act, ACT_SECRETIVE );
   for( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
   {
      char *sbuf = argument;

      if( vch == ch )
         continue;

      if( is_ignoring( vch, ch ) )
      {
         if( !IS_IMMORTAL( ch ) || get_trust( vch ) > get_trust( ch ) )
            continue;
         else
         {
            set_char_color( AT_IGNORE, vch );
            ch_printf( vch, "You attempt to ignore %s, but are unable to do so.\n\r", ch->name );
         }
      }

#ifndef SCRAMBLE
      if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
      {
         int speakswell = UMIN( knows_language( vch, ch->speaking, ch ), knows_language( ch, ch->speaking, vch ) );
         if( speakswell < 75 )
            sbuf = translate( speakswell, argument, lang_names[speaking] );
      }
#else
      if( !knows_language( vch, ch->speaking, ch ) && ( !IS_NPC( ch ) || ch->speaking != 0 ) )
         sbuf = scramblechan( argument, ch->speaking );
#endif
      sbuf = drunk_speech( sbuf, ch );

      MOBtrigger = FALSE;

   }
   ch->act = actflags;
   MOBtrigger = FALSE;

   switch ( last_char )
   {
      case '?':
         act( AT_SAY, "&RYou ask $N, '&w$t&R'&D", ch, drunk_speech( argument, ch ), victim, TO_CHAR );
         act( AT_SAY, "$n&R asks $N, '&w$t&R'&D", ch, drunk_speech( argument, ch ), victim, TO_NOTVICT );
         act( AT_SAY, "$n&R asks you '&w$t&R'&D", ch, drunk_speech( argument, ch ), victim, TO_VICT );
         break;

      case '!':
         act( AT_SAY, "&RYou exclaim at $N, '&w$t&R'&D", ch, drunk_speech( argument, ch ), victim, TO_CHAR );
         act( AT_SAY, "$n&R exclaims at $N, '&w$t&R'&D", ch, drunk_speech( argument, ch ), victim, TO_NOTVICT );
         act( AT_SAY, "$n&R exclaims to you, '&w$t&R'&D", ch, drunk_speech( argument, ch ), victim, TO_VICT );
         break;

      default:
         act( AT_SAY, "&RYou say to $N '&w$t&R'&D", ch, drunk_speech( argument, ch ), victim, TO_CHAR );
         act( AT_SAY, "$n&R says to $N '&w$t&R'&D", ch, drunk_speech( argument, ch ), victim, TO_NOTVICT );
         act( AT_SAY, "$n&R says to you '&w$t&R'&D", ch, drunk_speech( argument, ch ), victim, TO_VICT );
         break;
   }
   sprintf( buf2, "%s: %s (say)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument );
   append_to_file( sayLOG_FILE, buf2 );

   if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
   {
      sprintf( buf, "%s: %s", IS_NPC( ch ) ? ch->short_descr : ch->name, argument );
      append_to_file( LOG_FILE, buf );
   }
   mprog_speech_trigger( argument, ch );
   if( char_died( ch ) )
      return;
   oprog_speech_trigger( argument, ch );
   if( char_died( ch ) )
      return;
   rprog_speech_trigger( argument, ch );
   return;
}


void do_whisper( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int position;
   int speaking = -1, lang;
#ifndef SCRAMBLE

   for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
      if( ch->speaking & lang_array[lang] )
      {
         speaking = lang;
         break;
      }
#endif

   xREMOVE_BIT( ch->deaf, CHANNEL_WHISPER );

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' || argument[0] == '\0' )
   {
      send_to_char( "Whisper to whom what?\n\r", ch );
      return;
   }


   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( ch == victim )
   {
      send_to_char( "You have a nice little chat with yourself.\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) && ( victim->switched ) && !IS_AFFECTED( victim->switched, AFF_POSSESS ) )
   {
      send_to_char( "That player is switched.\n\r", ch );
      return;
   }
   else if( !IS_NPC( victim ) && ( !victim->desc ) )
   {
      send_to_char( "That player is link-dead.\n\r", ch );
      return;
   }
   if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_AFK ) )
   {
      send_to_char( "That player is afk.\n\r", ch );
      return;
   }
   if( xIS_SET( victim->deaf, CHANNEL_WHISPER ) && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
   {
      act( AT_PLAIN, "$E has $S whispers turned off.", ch, NULL, victim, TO_CHAR );
      return;
   }
   if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_SILENCE ) )
      send_to_char( "That player is silenced.  They will receive your message but can not respond.\n\r", ch );

   if( victim->desc && victim->desc->connected == CON_EDITING && get_trust( ch ) < LEVEL_BUILD )
   {
      act( AT_PLAIN, "$E is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
      return;
   }

   if( victim->desc && victim->desc->connected == CON_MEETING && get_trust( ch ) < LEVEL_IMMORTAL )
   {
      act( AT_PLAIN, "$E is currently in a meeting.  Please try again later.", ch, 0, victim, TO_CHAR );
      return;
   }

   if( is_ignoring( victim, ch ) )
   {
      if( !IS_IMMORTAL( ch ) || get_trust( victim ) > get_trust( ch ) )
      {
         set_char_color( AT_IGNORE, ch );
         ch_printf( ch, "%s is ignoring you.\n\r", victim->name );
         return;
      }
      else
      {
         set_char_color( AT_IGNORE, victim );
         ch_printf( victim, "You attempt to ignore %s, but " "are unable to do so.\n\r", ch->name );
      }
   }

   act( AT_WHISPER, "You whisper to $N '$t'", ch, argument, victim, TO_CHAR );
   position = victim->position;
   victim->position = POS_STANDING;

   if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
   {
#ifndef SCRAMBLE
      int speakswell = UMIN( knows_language( victim, ch->speaking, ch ),
                             knows_language( ch, ch->speaking, victim ) );

      if( speakswell < 85 )
         act( AT_WHISPER, "$n whispers to you '$t'", ch,
              translate( speakswell, argument, lang_names[speaking] ), victim, TO_VICT );
#else
      if( !knows_language( vch, ch->speaking, ch ) && ( !IS_NPC( ch ) || ch->speaking != 0 ) )
         act( AT_WHISPER, "$n whispers to you '$t'", ch, translate( speakswell, argument, lang_names[speaking] ), victim,
              TO_VICT );
#endif
      else
         act( AT_WHISPER, "$n whispers to you '$t'", ch, argument, victim, TO_VICT );
   }

   else
      act( AT_WHISPER, "$n whispers to you '$t'", ch, argument, victim, TO_VICT );


   if( !xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
      act( AT_WHISPER, "$n whispers something to $N.", ch, argument, victim, TO_NOTVICT );

   victim->position = position;
   if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
   {
      sprintf( buf, "%s: %s (whisper to) %s.",
               IS_NPC( ch ) ? ch->short_descr : ch->name, argument, IS_NPC( victim ) ? victim->short_descr : victim->name );
      append_to_file( LOG_FILE, buf );
   }

   mprog_speech_trigger( argument, ch );
   return;
}

void do_tell( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int position;
   CHAR_DATA *switched_victim = NULL;
#ifndef SCRAMBLE
   int speaking = -1, lang;

   for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
      if( ch->speaking & lang_array[lang] )
      {
         speaking = lang;
         break;
      }
#endif

   xREMOVE_BIT( ch->deaf, CHANNEL_TELLS );
   if( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
   {
      send_to_char( "You can't do that here.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && ( xIS_SET( ch->act, PLR_SILENCE ) || xIS_SET( ch->act, PLR_NO_TELL ) ) )
   {
      send_to_char( "You can't do that.\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' || argument[0] == '\0' )
   {
      send_to_char( "Tell whom what?\n\r", ch );
      return;
   }

   if( ( victim = get_char_world( ch, arg ) ) == NULL
       || ( IS_NPC( victim ) && victim->in_room != ch->in_room )
       || ( !NOT_AUTHED( ch ) && NOT_AUTHED( victim ) && !IS_IMMORTAL( ch ) ) )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( ch == victim )
   {
      send_to_char( "You have a nice little chat with yourself.\n\r", ch );
      return;
   }

   if( NOT_AUTHED( ch ) && !NOT_AUTHED( victim ) && !IS_IMMORTAL( victim ) )
   {
      send_to_char( "They can't hear you because you are not authorized.\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) && ( victim->switched )
       && ( get_trust( ch ) > LEVEL_AVATAR ) && !IS_AFFECTED( victim->switched, AFF_POSSESS ) )
   {
      send_to_char( "That player is switched.\n\r", ch );
      return;
   }

   else if( !IS_NPC( victim ) && ( victim->switched ) && IS_AFFECTED( victim->switched, AFF_POSSESS ) )
      switched_victim = victim->switched;

   else if( !IS_NPC( victim ) && ( !victim->desc ) )
   {
      send_to_char( "That player is link-dead.\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_AFK ) )
   {
      send_to_char( "That player is afk, but you leave a message.\n\r", ch );
   }

   if( xIS_SET( victim->deaf, CHANNEL_TELLS ) || xIS_SET( victim->act, PLR_DEAF )
       && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
   {
      act( AT_PLAIN, "$E has $S tells turned off.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_SILENCE ) )
      send_to_char( "That player is silenced.  They will receive your message but can not respond.\n\r", ch );

   if( ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) ) )
   {
      act( AT_PLAIN, "$E is too tired to discuss such matters with you now.", ch, 0, victim, TO_CHAR );
      return;
   }

   if( !IS_NPC( victim ) && xIS_SET( victim->in_room->room_flags, ROOM_SILENCE ) )
   {
      act( AT_PLAIN, "A magic force prevents your message from being heard.", ch, 0, victim, TO_CHAR );
      return;
   }

   if( victim->desc && victim->desc->connected == CON_EDITING && get_trust( ch ) < LEVEL_BUILD )
   {
      act( AT_PLAIN, "$E is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
      return;
   }

   if( victim->desc && victim->desc->connected == CON_MEETING && get_trust( ch ) < LEVEL_IMMORTAL )
   {
      act( AT_PLAIN, "$E is currently in a meeting.  Please try again later.", ch, 0, victim, TO_CHAR );
      return;
   }

   if( is_ignoring( victim, ch ) )
   {
      if( !IS_IMMORTAL( ch ) || get_trust( victim ) > get_trust( ch ) )
      {
         set_char_color( AT_IGNORE, ch );
         ch_printf( ch, "%s is ignoring you.\n\r", victim->name );
         return;
      }
      else
      {
         set_char_color( AT_IGNORE, victim );
         ch_printf( victim, "You attempt to ignore %s, but " "are unable to do so.\n\r", ch->name );
      }
   }

   ch->retell = victim;

   if( !IS_NPC( victim ) && IS_IMMORTAL( victim ) && victim->pcdata->tell_history &&
       isalpha( IS_NPC( ch ) ? ch->short_descr[0] : ch->name[0] ) )
   {
      sprintf( buf, "%s told you '%s'\n\r", capitalize( IS_NPC( ch ) ? ch->short_descr : ch->name ), argument );

      victim->pcdata->lt_index = tolower( IS_NPC( ch ) ? ch->short_descr[0] : ch->name[0] ) - 'a';

      if( victim->pcdata->tell_history[victim->pcdata->lt_index] )
         STRFREE( victim->pcdata->tell_history[victim->pcdata->lt_index] );

      victim->pcdata->tell_history[victim->pcdata->lt_index] = STRALLOC( buf );
   }

   if( switched_victim )
      victim = switched_victim;
   position = ch->position;
   ch->position = POS_STANDING;
   act( AT_TELL, "You tell $N &P'$t'", ch, argument, victim, TO_CHAR );
   ch->position = position;
   position = victim->position;
   victim->position = POS_STANDING;
   if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
   {
      int speakswell = UMIN( knows_language( victim, ch->speaking, ch ),
                             knows_language( ch, ch->speaking, victim ) );

      if( speakswell < 85 )
         act( AT_TELL, "$n&P tells you '$t'", ch, translate( speakswell, argument, lang_names[speaking] ), victim, TO_VICT );
      else
         act( AT_TELL, "$n&P tells you '$t'", ch, argument, victim, TO_VICT );
   }
   else
      act( AT_TELL, "$n&P tells you '$t'", ch, argument, victim, TO_VICT );

   victim->position = position;
   victim->reply = ch;
   if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
   {
      sprintf( buf, "%s: %s (tell to) %s.",
               IS_NPC( ch ) ? ch->short_descr : ch->name, argument, IS_NPC( victim ) ? victim->short_descr : victim->name );
      append_to_file( LOG_FILE, buf );
   }
   sprintf( buf, "%s: %s (tell to) %s.",
            IS_NPC( ch ) ? ch->short_descr : ch->name, argument, IS_NPC( victim ) ? victim->short_descr : victim->name );
   append_to_file( tellLOG_FILE, buf );

   mprog_speech_trigger( argument, ch );
   return;
}

void do_forget( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "&RSyntax:&g forget <who>&D\n\r", ch );
      return;
   }

   if( !IS_IMMORTAL( ch ) )
   {
      send_to_char( "You cannot do that!\n\r", ch );
      return;
   }

   if( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( ch == victim )
   {
      send_to_char( "You forget yourself!!!\n\r", ch );
      return;
   }

   victim->reply = NULL;
   sprintf( buf, "%s is terminating this conversation. If you attempt to continue you might be silenced.\n\r",
            PERS( ch, victim ) );
   send_to_char( buf, victim );
   send_to_char( "Done.\n\r", ch );
   return;
}

void do_spousetalk( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   int position;
#ifndef SCRAMBLE
   int speaking = -1, lang;

   for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
      if( ch->speaking & lang_array[lang] )
      {
         speaking = lang;
         break;
      }
#endif


   xREMOVE_BIT( ch->deaf, CHANNEL_MARRY );
   if( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
   {
      send_to_char( "You can't do that here.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_SILENCE ) )
   {
      send_to_char( "Your message didn't get through.\n\r", ch );
      return;
   }

   if( !IS_MARRIED( ch ) )
   {
      send_to_char( "You cannot do that.\n\r", ch );
      return;
   }

   if( ( victim = get_char_world( ch, ch->pcdata->spouse ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) && ( victim->switched ) && can_see( ch, victim ) && ( get_trust( ch ) > LEVEL_AVATAR ) )
   {
      send_to_char( "That player is switched.\n\r", ch );
      return;
   }
   else if( !IS_NPC( victim ) && ( !victim->desc ) )
   {
      send_to_char( "That player is link-dead.\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_AFK ) )
   {
      send_to_char( "That player is afk, but you leave a message.\n\r", ch );
   }

   if( xIS_SET( victim->deaf, CHANNEL_MARRY ) && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
   {
      act( AT_PLAIN, "$E has spousetalk turned off.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if(   /* ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
          * || */ ( !IS_NPC( victim ) && xIS_SET( victim->in_room->room_flags, ROOM_SILENCE ) ) )
   {
      act( AT_PLAIN, "$E can't hear you.", ch, 0, victim, TO_CHAR );
      return;
   }

   if( victim->desc && victim->desc->connected == CON_EDITING && get_trust( ch ) < LEVEL_BUILD )
   {
      act( AT_PLAIN, "$E is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
      return;
   }

   if( victim->desc && victim->desc->connected == CON_MEETING && get_trust( ch ) < LEVEL_IMMORTAL )
   {
      act( AT_PLAIN, "$E is currently in a meeting.  Please try again later.", ch, 0, victim, TO_CHAR );
      return;
   }

   if( is_ignoring( victim, ch ) )
   {
      if( !IS_IMMORTAL( ch ) || get_trust( victim ) > get_trust( ch ) )
      {
         set_char_color( AT_IGNORE, ch );
         ch_printf( ch, "%s is ignoring you.\n\r", victim->name );
         return;
      }
      else
      {
         set_char_color( AT_IGNORE, victim );
         ch_printf( victim, "You attempt to ignore %s, but " "are unable to do so.\n\r", ch->name );
      }
   }

   act( AT_TELL, "&w(&RSpouseTalk&w) $n&w> $t&D", ch, argument, NULL, TO_CHAR );
   position = victim->position;
   victim->position = POS_STANDING;
#ifndef SCRAMBLE
   if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
   {
      int speakswell = UMIN( knows_language( victim, ch->speaking, ch ),
                             knows_language( ch, ch->speaking, victim ) );

      if( speakswell < 85 )
         act( AT_TELL, "&w(&RSpouseTalk&w) $n&w> $t&D", ch, translate( speakswell, argument, lang_names[speaking] ), victim,
              TO_VICT );
      else
         act( AT_TELL, "&w(&RSpouseTalk&w) $n&w> $t&D", ch, argument, victim, TO_VICT );
   }
   else
      act( AT_TELL, "$&w(&RSpouseTalk&w) $n&w> $t&D", ch, argument, victim, TO_VICT );
#else
   if( knows_language( victim, ch->speaking, ch ) || ( IS_NPC( ch ) && !ch->speaking ) )
      act( AT_TELL, "&w(&RSpouseTalk&w) $n&w> $t&D", ch, argument, victim, TO_VICT );
   else
      act( AT_TELL, "&w(&RSpouseTalk&w) $n&w> $t&D", ch, scramblechan( argument, ch->speaking ), victim, TO_VICT );
#endif
   victim->position = position;
   victim->reply = ch;
   ch->retell = victim;
   if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
   {
      sprintf( buf, "%s: %s (spousetalk to) %s.",
               IS_NPC( ch ) ? ch->short_descr : ch->name, argument, IS_NPC( victim ) ? victim->short_descr : victim->name );
      append_to_file( LOG_FILE, buf );
   }


   return;
}

void do_reply( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   int position;
#ifndef SCRAMBLE
   int speaking = -1, lang;

   for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
      if( ch->speaking & lang_array[lang] )
      {
         speaking = lang;
         break;
      }
#endif


   xREMOVE_BIT( ch->deaf, CHANNEL_TELLS );
   if( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
   {
      send_to_char( "You can't do that here.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_SILENCE ) )
   {
      send_to_char( "Your message didn't get through.\n\r", ch );
      return;
   }

   if( ( victim = ch->reply ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) && ( victim->switched ) && can_see( ch, victim ) && ( get_trust( ch ) > LEVEL_AVATAR ) )
   {
      send_to_char( "That player is switched.\n\r", ch );
      return;
   }
   else if( !IS_NPC( victim ) && ( !victim->desc ) )
   {
      send_to_char( "That player is link-dead.\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_AFK ) )
   {
      send_to_char( "That player is afk, but you leave a message.\n\r", ch );
   }

   if( xIS_SET( victim->deaf, CHANNEL_TELLS ) || xIS_SET( victim->act, PLR_DEAF )
       && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
   {
      act( AT_PLAIN, "$E has $S tells turned off.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if(   /*( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
          * || */ ( !IS_NPC( victim ) && xIS_SET( victim->in_room->room_flags, ROOM_SILENCE ) ) )
   {
      act( AT_PLAIN, "$E can't hear you.", ch, 0, victim, TO_CHAR );
      return;
   }

   if( victim->desc && victim->desc->connected == CON_EDITING && get_trust( ch ) < LEVEL_BUILD )
   {
      act( AT_PLAIN, "$E is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
      return;
   }

   if( victim->desc && victim->desc->connected == CON_MEETING && get_trust( ch ) < LEVEL_IMMORTAL )
   {
      act( AT_PLAIN, "$E is currently in a meeting.  Please try again later.", ch, 0, victim, TO_CHAR );
      return;
   }

   if( is_ignoring( victim, ch ) )
   {
      if( !IS_IMMORTAL( ch ) || get_trust( victim ) > get_trust( ch ) )
      {
         set_char_color( AT_IGNORE, ch );
         ch_printf( ch, "%s is ignoring you.\n\r", victim->name );
         return;
      }
      else
      {
         set_char_color( AT_IGNORE, victim );
         ch_printf( victim, "You attempt to ignore %s, but " "are unable to do so.\n\r", ch->name );
      }
   }

   position = ch->position;
   ch->position = POS_STANDING;
   act( AT_TELL, "You tell $N &P'$t'", ch, argument, victim, TO_CHAR );
   ch->position = position;
   position = victim->position;
   victim->position = POS_STANDING;
#ifndef SCRAMBLE
   if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
   {
      int speakswell = UMIN( knows_language( victim, ch->speaking, ch ),
                             knows_language( ch, ch->speaking, victim ) );

      if( speakswell < 85 )
         act( AT_TELL, "$n&P tells you '$t'", ch, translate( speakswell, argument, lang_names[speaking] ), victim, TO_VICT );
      else
         act( AT_TELL, "$n&P tells you '$t'", ch, argument, victim, TO_VICT );
   }
   else
      act( AT_TELL, "$n&P tells you '$t'", ch, argument, victim, TO_VICT );
#else
   if( knows_language( victim, ch->speaking, ch ) || ( IS_NPC( ch ) && !ch->speaking ) )
      act( AT_TELL, "$n&P tells you '$t'", ch, argument, victim, TO_VICT );
   else
      act( AT_TELL, "$n&P tells you '$t'", ch, scramblechan( argument, ch->speaking ), victim, TO_VICT );
#endif
   victim->position = position;
   victim->reply = ch;
   ch->retell = victim;
   if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
   {
      sprintf( buf, "%s: %s (reply to) %s.",
               IS_NPC( ch ) ? ch->short_descr : ch->name, argument, IS_NPC( victim ) ? victim->short_descr : victim->name );
      append_to_file( LOG_FILE, buf );
   }
   sprintf( buf, "%s: %s (tell to) %s.",
            IS_NPC( ch ) ? ch->short_descr : ch->name, argument, IS_NPC( victim ) ? victim->short_descr : victim->name );
   append_to_file( tellLOG_FILE, buf );

   if( !IS_NPC( victim ) && IS_IMMORTAL( victim ) && victim->pcdata->tell_history &&
       isalpha( IS_NPC( ch ) ? ch->short_descr[0] : ch->name[0] ) )
   {
      sprintf( buf, "%s told you '%s'\n\r", capitalize( IS_NPC( ch ) ? ch->short_descr : ch->name ), argument );

      victim->pcdata->lt_index = tolower( IS_NPC( ch ) ? ch->short_descr[0] : ch->name[0] ) - 'a';

      if( victim->pcdata->tell_history[victim->pcdata->lt_index] )
         STRFREE( victim->pcdata->tell_history[victim->pcdata->lt_index] );

      victim->pcdata->tell_history[victim->pcdata->lt_index] = STRALLOC( buf );
   }

   return;
}

void do_retell( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int position;
   CHAR_DATA *switched_victim = NULL;
#ifndef SCRAMBLE
   int speaking = -1, lang;

   for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
      if( ch->speaking & lang_array[lang] )
      {
         speaking = lang;
         break;
      }
#endif
   xREMOVE_BIT( ch->deaf, CHANNEL_TELLS );
   if( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
   {
      send_to_char( "You can't do that here.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && ( xIS_SET( ch->act, PLR_SILENCE ) || xIS_SET( ch->act, PLR_NO_TELL ) ) )
   {
      send_to_char( "You can't do that.\n\r", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      ch_printf( ch, "What message do you wish to send?\n\r" );
      return;
   }

   victim = ch->retell;

   if( !victim )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) && ( victim->switched ) &&
       ( get_trust( ch ) > LEVEL_AVATAR ) && !IS_AFFECTED( victim->switched, AFF_POSSESS ) )
   {
      send_to_char( "That player is switched.\n\r", ch );
      return;
   }
   else if( !IS_NPC( victim ) && ( victim->switched ) && IS_AFFECTED( victim->switched, AFF_POSSESS ) )
   {
      switched_victim = victim->switched;
   }
   else if( !IS_NPC( victim ) && ( !victim->desc ) )
   {
      send_to_char( "That player is link-dead.\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_AFK ) )
   {
      send_to_char( "That player is afk, but you leave a message.\n\r", ch );
   }

   if( xIS_SET( victim->deaf, CHANNEL_TELLS ) || xIS_SET( victim->act, PLR_DEAF )
       && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
   {
      act( AT_PLAIN, "$E has $S tells turned off.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_SILENCE ) )
      send_to_char( "That player is silenced. They will receive your message, but can not respond.\n\r", ch );

   if( ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) ) ||
       ( !IS_NPC( victim ) && xIS_SET( victim->in_room->room_flags, ROOM_SILENCE ) ) )
   {
      act( AT_PLAIN, "$E can't hear you.", ch, 0, victim, TO_CHAR );
      return;
   }

   if( victim->desc && victim->desc->connected == CON_EDITING && get_trust( ch ) < LEVEL_BUILD )
   {
      act( AT_PLAIN, "$E is currently in a writing buffer. Please " "try again in a few minutes.", ch, 0, victim, TO_CHAR );
      return;
   }

   if( victim->desc && victim->desc->connected == CON_MEETING && get_trust( ch ) < LEVEL_IMMORTAL )
   {
      act( AT_PLAIN, "$E is currently in a meeting.  Please try again later.", ch, 0, victim, TO_CHAR );
      return;
   }

   if( is_ignoring( victim, ch ) )
   {
      if( !IS_IMMORTAL( ch ) || get_trust( victim ) > get_trust( ch ) )
      {
         set_char_color( AT_IGNORE, ch );
         ch_printf( ch, "%s is ignoring you.\n\r", victim->name );
         return;
      }
      else
      {
         set_char_color( AT_IGNORE, victim );
         ch_printf( victim, "You attempy to ignore %s, but " "are unable to do so.\n\r", ch->name );
      }
   }

   if( !IS_NPC( victim ) && IS_IMMORTAL( victim ) && victim->pcdata->tell_history &&
       isalpha( IS_NPC( ch ) ? ch->short_descr[0] : ch->name[0] ) )
   {
      sprintf( buf, "%s told you '%s'\n\r", capitalize( IS_NPC( ch ) ? ch->short_descr : ch->name ), argument );

      victim->pcdata->lt_index = tolower( IS_NPC( ch ) ? ch->short_descr[0] : ch->name[0] ) - 'a';

      if( victim->pcdata->tell_history[victim->pcdata->lt_index] )
         STRFREE( victim->pcdata->tell_history[victim->pcdata->lt_index] );

      victim->pcdata->tell_history[victim->pcdata->lt_index] = STRALLOC( buf );
   }

   if( switched_victim )
      victim = switched_victim;

   act( AT_TELL, "You tell $N &P'$t'", ch, argument, victim, TO_CHAR );
   position = victim->position;
   victim->position = POS_STANDING;
#ifndef SCRAMBLE
   if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
   {
      int speakswell = UMIN( knows_language( victim, ch->speaking, ch ),
                             knows_language( ch, ch->speaking, victim ) );

      if( speakswell < 85 )
         act( AT_TELL, "$n&P tells you '$t'", ch, translate( speakswell, argument, lang_names[speaking] ), victim, TO_VICT );
      else
         act( AT_TELL, "$n&P tells you '$t'", ch, argument, victim, TO_VICT );
   }
   else
      act( AT_TELL, "$n&P tells you '$t'", ch, argument, victim, TO_VICT );
#else
   if( knows_language( victim, ch->speaking, ch ) || ( IS_NPC( ch ) && !ch->speaking ) )
   {
      act( AT_TELL, "$n&P tells you '$t'", ch, argument, victim, TO_VICT );
   }
   else
   {
      act( AT_TELL, "$n&P tells you '$t'", ch, scramblechan( argument, ch->speaking ), victim, TO_VICT );
   }
#endif
   victim->position = position;
   victim->reply = ch;
   if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
   {
      sprintf( buf, "%s: %s (retell to) %s.",
               IS_NPC( ch ) ? ch->short_descr : ch->name, argument, IS_NPC( victim ) ? victim->short_descr : victim->name );
      append_to_file( LOG_FILE, buf );
   }

   mprog_speech_trigger( argument, ch );
   return;
}

void do_repeat( CHAR_DATA * ch, char *argument )
{
   int index;

   if( IS_NPC( ch ) || !IS_IMMORTAL( ch ) || !ch->pcdata->tell_history )
   {
      ch_printf( ch, "Huh?\n\r" );
      return;
   }

   if( argument[0] == '\0' )
   {
      index = ch->pcdata->lt_index;
   }
   else if( isalpha( argument[0] ) && argument[1] == '\0' )
   {
      index = tolower( argument[0] ) - 'a';
   }
   else
   {
      ch_printf( ch, "You may only index your tell history using " "a single letter.\n\r" );
      return;
   }

   if( ch->pcdata->tell_history[index] )
   {
      set_char_color( AT_TELL, ch );
      ch_printf( ch, ch->pcdata->tell_history[index] );
   }
   else
   {
      ch_printf( ch, "No one like that has sent you a tell.\n\r" );
   }

   return;
}


void do_emote( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char *plast;
   CHAR_DATA *vch;
   EXT_BV actflags;
#ifndef SCRAMBLE
   int speaking = -1, lang;

   for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
      if( ch->speaking & lang_array[lang] )
      {
         speaking = lang;
         break;
      }
#endif

   if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_NO_EMOTE ) )
   {
      send_to_char( "You can't show your emotions.\n\r", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      send_to_char( "Emote what?\n\r", ch );
      return;
   }

   actflags = ch->act;
   if( IS_NPC( ch ) )
      xREMOVE_BIT( ch->act, ACT_SECRETIVE );
   for( plast = argument; *plast != '\0'; plast++ )
      ;

   strcpy( buf, argument );
   if( isalpha( plast[-1] ) )
      strcat( buf, "." );
   for( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
   {
      char *sbuf = buf;

      if( is_ignoring( vch, ch ) )
      {
         if( !IS_IMMORTAL( ch ) || get_trust( vch ) > get_trust( ch ) )
            continue;
         else
         {
            set_char_color( AT_IGNORE, vch );
            ch_printf( vch, "You attempt to ignore %s, but" " are unable to do so.\n\r", ch->name );
         }
      }
#ifndef SCRAMBLE
      if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
      {
         int speakswell = UMIN( knows_language( vch, ch->speaking, ch ),
                                knows_language( ch, ch->speaking, vch ) );

         if( speakswell < 85 )
            sbuf = translate( speakswell, argument, lang_names[speaking] );
      }
#else
      if( !knows_language( vch, ch->speaking, ch ) && ( !IS_NPC( ch ) && ch->speaking != 0 ) )
         sbuf = scramblechan( buf, ch->speaking );
#endif
      MOBtrigger = FALSE;
      act( AT_ACTION, "$n $t", ch, sbuf, vch, ( vch == ch ? TO_CHAR : TO_VICT ) );
   }
   ch->act = actflags;
   if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
   {
      sprintf( buf, "%s %s (emote)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument );
      append_to_file( LOG_FILE, buf );
   }
   return;
}

/*
void do_bug( CHAR_DATA *ch, char *argument )
{
    char    buf[MAX_STRING_LENGTH];
    struct  tm *t = localtime(&current_time);

    set_char_color( AT_PLAIN, ch );
    if ( argument[0] == '\0' ) {
        send_to_char( "\n\rUsage:  'bug <message>'  (your location is automatically recorded)\n\r", ch );
        return;
    }
    sprintf( buf, "(%-2.2d/%-2.2d):  %s",
	t->tm_mon+1, t->tm_mday, argument );
    append_file( ch, PBUG_FILE, buf );
    send_to_char( "Thanks, your bug notice has been recorded.\n\r", ch );
    return;
}
*/
void do_ide( CHAR_DATA * ch, char *argument )
{
   set_char_color( AT_PLAIN, ch );
   send_to_char( "\n\rIf you want to send an idea, type 'idea <message>'.\n\r", ch );
   send_to_char( "If you want to identify an object, use the identify spell.\n\r", ch );
   return;
}

void do_idea( CHAR_DATA * ch, char *argument )
{
   set_char_color( AT_PLAIN, ch );
   if( argument[0] == '\0' )
   {
      send_to_char( "\n\rUsage:  'idea <message>'\n\r", ch );
      return;
   }
   append_file( ch, IDEA_FILE, argument );
   send_to_char( "Thanks, your idea has been recorded.\n\r", ch );
   return;
}

void do_typo( CHAR_DATA * ch, char *argument )
{
   set_char_color( AT_PLAIN, ch );
   if( argument[0] == '\0' )
   {
      send_to_char( "\n\rUsage:  'typo <message>'  (your location is automatically recorded)\n\r", ch );
      if( get_trust( ch ) >= LEVEL_BUILD )
         send_to_char( "Usage:  'typo list' or 'typo clear now'\n\r", ch );
      return;
   }
   if( !str_cmp( argument, "clear now" ) && get_trust( ch ) >= LEVEL_BUILD )
   {
      FILE *fp = fopen( TYPO_FILE, "w" );
      if( fp )
         fclose( fp );
      send_to_char( "Typo file cleared.\n\r", ch );
      return;
   }
   if( !str_cmp( argument, "list" ) && get_trust( ch ) >= LEVEL_BUILD )
   {
      send_to_char( "\n\r VNUM \n\r.......\n\r", ch );
      show_file( ch, TYPO_FILE );
   }
   else
   {
      append_file( ch, TYPO_FILE, argument );
      send_to_char( "Thanks, your typo notice has been recorded.\n\r", ch );
   }
   return;
}


void do_qui( CHAR_DATA * ch, char *argument )
{
   set_char_color( AT_RED, ch );
   send_to_char( "If you want to QUIT, you have to spell it out.\n\r", ch );
   return;
}

void do_quit( CHAR_DATA * ch, char *argument )
{
   char_quit( ch, TRUE, argument );
}

void char_quit( CHAR_DATA * ch, bool broad_quit, char *argument )
{
//    int x, y;
//    int level;
   char buf[MAX_STRING_LENGTH];
   char qbuf[MAX_INPUT_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   sh_int room_chance;
   int level = ch->level, rentcost = 0;
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;

   if( IS_NPC( ch ) )
      return;

   for( obj = ch->first_carrying; obj; obj = obj->next_content )
      rent_calculate( ch, obj, &rentcost );

   argument = one_argument( argument, arg1 );

   if( !str_cmp( arg1, "auto" ) && sysdata.RENT && rentcost > 0 )
   {
      room_chance = number_range( 1, 3 );

      if( room_chance > 2 )
      {
         sprintf( log_buf, "%s has failed autorent, setting autorent flag.", ch->name );
         log_string_plus( log_buf, LOG_COMM, level );
         char_leaving( ch, 2, rentcost );
      }
      else
      {
         sprintf( log_buf, "%s has autorented safely.", ch->name );
         log_string_plus( log_buf, LOG_COMM, level );
         char_leaving( ch, 3, rentcost );
      }
      return;
   }

   if( ( arg1[0] == '\0' || str_cmp( arg1, "yes" ) ) && sysdata.RENT && rentcost > 0 )
   {
      do_help( ch, "quit" );
      return;
   }

   if( ch->position == POS_FIGHTING
       || ch->position == POS_EVASIVE
       || ch->position == POS_DEFENSIVE || ch->position == POS_AGGRESSIVE || ch->position == POS_BERSERK )
   {
      set_char_color( AT_RED, ch );
      send_to_char( "No way! You are fighting.\n\r", ch );
      return;
   }

   if( ch->position < POS_STUNNED )
   {
      set_char_color( AT_BLOOD, ch );
      send_to_char( "You're not DEAD yet.\n\r", ch );
      return;
   }

   if( get_timer( ch, TIMER_RECENTFIGHT ) > 0 && !IS_IMMORTAL( ch ) )
   {
      set_char_color( AT_RED, ch );
      send_to_char( "Your adrenaline is pumping too hard to quit now!\n\r", ch );
      return;
   }

   if( auction->item != NULL && ( ( ch == auction->buyer ) || ( ch == auction->seller ) ) )
   {
      send_to_char( "Wait until you have bought/sold the item on auction.\n\r", ch );
      return;

   }

   if( ch->level > 799 )
   {
      immlog_onoff( ch, "Off" );
   }

   if( ch->challenged )
   {
      sprintf( qbuf, "%s has quit! Challenge is void. WHAT A WUSS!", ch->name );
      ch->challenged = NULL;
      sportschan( qbuf );
   }

   if( IS_PKILL( ch ) && ch->wimpy > ( int )ch->max_hit / 2.25 )
   {
      send_to_char( "Your wimpy has been adjusted to the maximum level for deadlies.\n\r", ch );
      do_wimpy( ch, "max" );
   }
   if( IS_TRACKING( ch ) || ch->hunting )
   {
      if( ch->hunting )
      {
         stop_hunting( ch );
      }
      affect_strip( ch, gsn_track );

      xREMOVE_BIT( ch->act, PLR_TRACK );
   }
   if( ch->position == POS_MOUNTED )
      do_dismount( ch, "" );
   set_char_color( AT_WHITE, ch );
   if( sysdata.RENT && rentcost > 0 )
      send_to_char( "As you leave, your equipment falls to the floor!\n\r", ch );
   if( is_affected( ch, gsn_karaaura ) )
   {
      affect_strip( ch, gsn_karaaura );
      send_to_char( "Kara's Aura leaves your soul feeling empty.\n\r", ch );
   }
   send_to_char( "The Storm swallows you as you fade from the realm....\n\n\n\r", ch );
   act( AT_BYE, "$n has left the game.", ch, NULL, NULL, TO_CANSEE );
   set_char_color( AT_GREY, ch );

   if( broad_quit && !IS_IMMORTAL( ch ) )
   {
      sprintf( buf, "&R<&WLOG INFO&R>&D %s has left %s...", ch->name, sysdata.mud_name );
      talk_info( AT_BLUE, buf );
   }
   for( vch = first_char; vch; vch = vch_next )
   {
      vch_next = vch->next;

      if( !IS_NPC( vch ) )
      {
         if( vch->pcdata->monitor == ch )
         {
            do_monitor( vch, "self" );
         }
      }
   }

   sprintf( log_buf, "%s has quit (Room %d).", ch->name, ( ch->in_room ? ch->in_room->vnum : -1 ) );
   log_string_plus( log_buf, LOG_COMM, level );

   if( sysdata.RENT )
      char_leaving( ch, 4, rentcost );
   else
      char_leaving( ch, 0, rentcost );

   return;
}

void do_pqui( CHAR_DATA * ch, char *argument )
{
   set_char_color( AT_RED, ch );
   send_to_char( "Please spell pquit out.\n\r", ch );
   return;
}

void do_pquit( CHAR_DATA * ch, char *argument )
{
//    int x, y;
//    int level;
   char buf[MAX_STRING_LENGTH];
   char qbuf[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   sh_int room_chance;
   int level = ch->level, rentcost = 0;

   set_char_color( AT_IMMORT, ch );

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' )
   {
      send_to_char( "Syntax: pquit <who>\n\r", ch );
      return;
   }

   if( !( victim = get_char_world( ch, arg1 ) ) )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( IS_NPC( victim ) )
   {
      send_to_char( "Not on NPC's.\n\r", ch );
      return;
   }

   for( obj = victim->first_carrying; obj; obj = obj->next_content )
      rent_calculate( victim, obj, &rentcost );


   if( !str_cmp( arg2, "auto" ) && sysdata.RENT && rentcost > 0 )
   {
      room_chance = number_range( 1, 3 );

      if( room_chance > 2 )
      {
         sprintf( log_buf, "%s has failed autorent, setting autorent flag.", victim->name );
         log_string_plus( log_buf, LOG_COMM, level );
         char_leaving( victim, 2, rentcost );
      }
      else
      {
         sprintf( log_buf, "%s has autorented safely.", victim->name );
         log_string_plus( log_buf, LOG_COMM, level );
         char_leaving( victim, 3, rentcost );
      }
      return;
   }

   set_char_color( AT_IMMORT, victim );
   send_to_char( "The MUD administrators had you quit due to linkdead state or other reason.\n\r", victim );
   if( victim->fighting )
      stop_fighting( victim, TRUE );

   if( auction->item != NULL && ( ( victim == auction->buyer ) || ( victim == auction->seller ) ) )
   {
      send_to_char( "Wait until your victim has bought/sold the item on auction.\n\r", ch );
      return;

   }

   if( victim->challenged )
   {
      sprintf( qbuf, "%s has quit! Challenge is void. WHAT A WUSS!", victim->name );
      victim->challenged = NULL;
      sportschan( qbuf );
   }

   if( IS_PKILL( victim ) && victim->wimpy > ( int )victim->max_hit / 2.25 )
   {
      send_to_char( "Your wimpy has been adjusted to the maximum level for deadlies.\n\r", victim );
      do_wimpy( victim, "max" );
   }
   if( victim->position == POS_MOUNTED )
      do_dismount( victim, "" );
   set_char_color( AT_WHITE, victim );
   if( sysdata.RENT && rentcost > 0 )
      send_to_char( "As you leave, your equipment falls to the floor!\n\r", victim );
   if( is_affected( victim, gsn_karaaura ) )
   {
      affect_strip( victim, gsn_karaaura );
      send_to_char( "Kara's Aura leaves your soul feeling empty.\n\r", victim );
   }
   send_to_char( "The Storm swallows you as you fade from the realm....\n\n\n\r", victim );
   act( AT_BYE, "$n has left the game.", victim, NULL, NULL, TO_CANSEE );
   set_char_color( AT_GREY, victim );

   if( !IS_IMMORTAL( victim ) )
   {
      sprintf( buf, "&R<&WLOG INFO&R>&D %s has left %s...", victim->name, sysdata.mud_name );
      talk_info( AT_BLUE, buf );
   }

   sprintf( log_buf, "%s has quit (Room %d).", victim->name, ( victim->in_room ? victim->in_room->vnum : -1 ) );
   log_string_plus( log_buf, LOG_COMM, level );

   if( sysdata.RENT )
      char_leaving( victim, 4, rentcost );
   else
      char_leaving( victim, 0, rentcost );

   send_to_char( "Done.\n\r", ch );
   return;
}


void send_rip_screen( CHAR_DATA * ch )
{
   FILE *rpfile;
   int num = 0;
   char BUFF[MAX_STRING_LENGTH * 2];

   if( ( rpfile = fopen( RIPSCREEN_FILE, "r" ) ) != NULL )
   {
      while( ( BUFF[num] = fgetc( rpfile ) ) != EOF )
         num++;
      fclose( rpfile );
      BUFF[num] = 0;
      write_to_buffer( ch->desc, BUFF, num );
   }
}

void send_rip_title( CHAR_DATA * ch )
{
   FILE *rpfile;
   int num = 0;
   char BUFF[MAX_STRING_LENGTH * 2];

   if( ( rpfile = fopen( RIPTITLE_FILE, "r" ) ) != NULL )
   {
      while( ( BUFF[num] = fgetc( rpfile ) ) != EOF )
         num++;
      fclose( rpfile );
      BUFF[num] = 0;
      write_to_buffer( ch->desc, BUFF, num );
   }
}

void send_ansi_title( CHAR_DATA * ch )
{
   FILE *rpfile;
   int num = 0;
   char BUFF[MAX_STRING_LENGTH * 2];

   if( ( rpfile = fopen( ANSITITLE_FILE, "r" ) ) != NULL )
   {
      while( ( BUFF[num] = fgetc( rpfile ) ) != EOF )
         num++;
      fclose( rpfile );
      BUFF[num] = 0;
      write_to_buffer( ch->desc, BUFF, num );
   }
}

void send_ascii_title( CHAR_DATA * ch )
{
   FILE *rpfile;
   int num = 0;
   char BUFF[MAX_STRING_LENGTH];

   if( ( rpfile = fopen( ASCTITLE_FILE, "r" ) ) != NULL )
   {
      while( ( BUFF[num] = fgetc( rpfile ) ) != EOF )
         num++;
      fclose( rpfile );
      BUFF[num] = 0;
      write_to_buffer( ch->desc, BUFF, num );
   }
}


void do_rip( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "Rip ON or OFF?\n\r", ch );
      return;
   }
   if( ( strcmp( arg, "on" ) == 0 ) || ( strcmp( arg, "ON" ) == 0 ) )
   {
      send_rip_screen( ch );
      xSET_BIT( ch->act, PLR_RIP );
      xSET_BIT( ch->act, PLR_ANSI );
      return;
   }

   if( ( strcmp( arg, "off" ) == 0 ) || ( strcmp( arg, "OFF" ) == 0 ) )
   {
      xREMOVE_BIT( ch->act, PLR_RIP );
      send_to_char( "!|*\n\rRIP now off...\n\r", ch );
      return;
   }
}

void do_ansi( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "ANSI ON or OFF?\n\r", ch );
      return;
   }
   if( ( strcmp( arg, "on" ) == 0 ) || ( strcmp( arg, "ON" ) == 0 ) )
   {
      xSET_BIT( ch->act, PLR_ANSI );
      set_char_color( AT_WHITE, ch );
      send_to_char( "ANSI ON!!!\n\r", ch );
      return;
   }

   if( ( strcmp( arg, "off" ) == 0 ) || ( strcmp( arg, "OFF" ) == 0 ) )
   {
      xREMOVE_BIT( ch->act, PLR_ANSI );
      send_to_char( "Okay... ANSI support is now off\n\r", ch );
      return;
   }
}

void do_save( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) )
      return;
   if( ch->level < 3 )
   {
      send_to_char_color( "&BYou must be at least third level to save.\n\r", ch );
      return;
   }
//    WAIT_STATE( ch, 2 );
   update_aris( ch );
   save_char_obj( ch );
// save_finger( ch );
   saving_char = NULL;
   send_to_char( "Saved...\n\r", ch );
   return;
}

void login_save( CHAR_DATA * ch )
{
   if( IS_NPC( ch ) )
      return;
   if( ch->level < 3 )
      return;
   update_aris( ch );
   save_char_obj( ch );
// save_finger( ch );
   saving_char = NULL;
   return;
}

bool circle_follow( CHAR_DATA * ch, CHAR_DATA * victim )
{
   CHAR_DATA *tmp;

   for( tmp = victim; tmp; tmp = tmp->master )
      if( tmp == ch )
         return TRUE;
   return FALSE;
}


void do_dismiss( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "Dismiss whom?\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( ( ( IS_AFFECTED( victim, AFF_CHARM ) || IS_AFFECTED( victim, AFF_JMT ) ) )
       && ( IS_NPC( victim ) ) && ( victim->master == ch ) )
   {
      stop_follower( victim );
      stop_hating( victim );
      stop_hunting( victim );
      stop_fearing( victim );
      act( AT_ACTION, "$n dismisses $N.", ch, NULL, victim, TO_NOTVICT );
      act( AT_ACTION, "You dismiss $N.", ch, NULL, victim, TO_CHAR );
   }
   else
   {
      send_to_char( "You cannot dismiss them.\n\r", ch );
   }

   return;
}

void do_follow( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "Follow whom?\n\r", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) && ch->master )
   {
      act( AT_PLAIN, "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
      return;
   }

   if( victim == ch )
   {
      if( !ch->master )
      {
         send_to_char( "You already follow yourself.\n\r", ch );
         return;
      }
      stop_follower( ch );
      return;
   }

   if( ( ch->level - victim->level < -10 || ch->level - victim->level > 10 )
       && !IS_HERO( ch ) && !( ch->level < 15 && !IS_NPC( victim )
                               && victim->pcdata->council && !str_cmp( victim->pcdata->council->name, "Newbie Council" ) ) )
   {
      send_to_char( "You are not of the right caliber to follow.\n\r", ch );
      return;
   }

   if( circle_follow( ch, victim ) )
   {
      send_to_char( "Following in loops is not allowed... sorry.\n\r", ch );
      return;
   }

   if( ch->master )
      stop_follower( ch );

   add_follower( ch, victim );
   return;
}



void add_follower( CHAR_DATA * ch, CHAR_DATA * master )
{
   if( ch->master )
   {
      bug( "Add_follower: non-null master.", 0 );
      return;
   }

   ch->master = master;
   ch->leader = NULL;

   if( IS_NPC( ch ) && xIS_SET( ch->act, ACT_PET ) && !IS_NPC( master ) )
      master->pcdata->pet = ch;

   if( can_see( master, ch ) )
      act( AT_ACTION, "$n now follows you.", ch, NULL, master, TO_VICT );

   act( AT_ACTION, "You now follow $N.", ch, NULL, master, TO_CHAR );

   return;
}



void stop_follower( CHAR_DATA * ch )
{
   if( !ch->master )
   {
      bug( "Stop_follower: null master.", 0 );
      return;
   }

   if( IS_NPC( ch ) && !IS_NPC( ch->master ) && ch->master->pcdata->pet == ch )
      ch->master->pcdata->pet = NULL;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
   {
      xREMOVE_BIT( ch->affected_by, AFF_CHARM );
      affect_strip( ch, gsn_charm_person );
      if( !IS_NPC( ch->master ) )
         ch->master->pcdata->charmies--;
   }
   if( IS_AFFECTED( ch, AFF_JMT ) )
   {
      xREMOVE_BIT( ch->affected_by, AFF_JMT );
      affect_strip( ch, gsn_jmt );
      if( !IS_NPC( ch->master ) )
         ch->master->pcdata->charmies--;
   }

   if( can_see( ch->master, ch ) )
      if( !( !IS_NPC( ch->master ) && IS_IMMORTAL( ch ) && !IS_IMMORTAL( ch->master ) ) )
         act( AT_ACTION, "$n stops following you.", ch, NULL, ch->master, TO_VICT );
   act( AT_ACTION, "You stop following $N.", ch, NULL, ch->master, TO_CHAR );

   ch->master = NULL;
   ch->leader = NULL;
   return;
}



void die_follower( CHAR_DATA * ch )
{
   CHAR_DATA *fch;

   if( ch->master )
      stop_follower( ch );

   ch->leader = NULL;

   for( fch = first_char; fch; fch = fch->next )
   {
      if( fch->master == ch )
         stop_follower( fch );
      if( fch->leader == ch )
         fch->leader = fch;
   }
   return;
}



void do_order( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char argbuf[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   CHAR_DATA *och;
   CHAR_DATA *och_next;
   bool found;
   bool fAll;

   strcpy( argbuf, argument );
   argument = one_argument( argument, arg );

   if( arg[0] == '\0' || argument[0] == '\0' )
   {
      send_to_char( "Order whom to do what?\n\r", ch );
      return;
   }

   if( ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) ) )
   {
      send_to_char( "You feel like taking, not giving, orders.\n\r", ch );
      return;
   }

   if( !str_cmp( arg, "all" ) )
   {
      fAll = TRUE;
      victim = NULL;
   }
   else
   {
      fAll = FALSE;
      if( ( victim = get_char_room( ch, arg ) ) == NULL )
      {
         send_to_char( "They aren't here.\n\r", ch );
         return;
      }

      if( victim == ch )
      {
         send_to_char( "Aye aye, right away!\n\r", ch );
         return;
      }

      if( ( !IS_AFFECTED( victim, AFF_CHARM ) || !IS_AFFECTED( victim, AFF_JMT ) ) || victim->master != ch )
      {
         send_to_char( "Do it yourself!\n\r", ch );
         return;
      }
   }

   found = FALSE;
   for( och = ch->in_room->first_person; och; och = och_next )
   {
      och_next = och->next_in_room;

      if( ( IS_AFFECTED( och, AFF_CHARM ) || IS_AFFECTED( och, AFF_JMT ) ) && och->master == ch
          && ( fAll || och == victim ) )
      {
         found = TRUE;
         act( AT_ACTION, "$n orders you to '$t'.", ch, argument, och, TO_VICT );
         interpret( och, argument );
      }
   }

   if( found )
   {
      sprintf( log_buf, "%s: order %s.", ch->name, argbuf );
      log_string_plus( log_buf, LOG_NORMAL, ch->level );
      send_to_char( "Ok.\n\r", ch );
//        WAIT_STATE( ch, 12 );
   }
   else
      send_to_char( "You have no followers here.\n\r", ch );
   return;
}


void do_group( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
//    char buf2[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];
   char clt[MAX_STRING_LENGTH];
   char clt2[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      CHAR_DATA *gch;
      CHAR_DATA *leader;

      leader = ch->leader ? ch->leader : ch;
      set_char_color( AT_DGREEN, ch );
      ch_printf( ch, "\n\r%s&g's group:\n\r", PERS( leader, ch ) );
      send_to_char( "------------------------------------------------------------------------------------\n\r", ch );
      for( gch = first_char; gch; gch = gch->next )
      {
         if( is_same_group( gch, ch ) )
         {
            set_char_color( AT_DGREEN, ch );
            if( IS_AFFECTED( gch, AFF_POSSESS ) )
               ch_printf( ch,
                          "[%2d %s] %-16s %4s/%4s hp %4s/%4s %s %4s/%4s mv %5s xp\n\r",
                          gch->level,
                          IS_NPC( gch ) ? "Mob" : class_table[gch->class]->who_name,
                          capitalize( GPERS( gch, ch ) ),
                          "????", "????", "????", "????", IS_VAMPIRE( gch ) ? "bp" : "mana", "????", "????", "?????" );
            else if( gch->level < 100 )
            {
               sprintf( buf, "%2d  Mt", gch->level );
            }
            if( gch->level == 100 )
            {
               sprintf( buf, "&G%3d &BHr&G", gch->sublevel );
            }
            if( gch->level == 200 )
            {
               sprintf( buf, "&G%3d &CAv&G", gch->sublevel );
            }
            if( gch->level == 300 )
            {
               sprintf( buf, "&G%3d &pNp&G", gch->sublevel );
            }
            if( gch->level == 400 )
            {
               sprintf( buf, "&G%3d &RAc&G", gch->sublevel );
            }

            switch ( gch->level )
            {
               default:
                  break;
               case MAX_LEVEL - 0:
                  sprintf( buf, "&WHt Adm&G" );
                  break;
               case MAX_LEVEL - 10:
                  sprintf( buf, "&WHt Sen&G" );
                  break;
               case MAX_LEVEL - 20:
                  sprintf( buf, "&WHost  &G" );
                  break;
               case MAX_LEVEL - 30:
                  sprintf( buf, "&WHi Tmp&G" );
                  break;
               case MAX_LEVEL - 40:
                  sprintf( buf, "&WAh Tmp&G" );
                  break;
               case MAX_LEVEL - 50:
                  sprintf( buf, "&WTempla&G" );
                  break;
               case MAX_LEVEL - 60:
                  sprintf( buf, "&WEl Wzd&G" );
                  break;
               case MAX_LEVEL - 70:
                  sprintf( buf, "&WAh Wzd&G" );
                  break;
               case MAX_LEVEL - 80:
                  sprintf( buf, "&WWizard&G" );
                  break;
               case MAX_LEVEL - 90:
                  sprintf( buf, "&WGd Bsp&G" );
                  break;
               case MAX_LEVEL - 100:
                  sprintf( buf, "&WAh Bsp&G" );
                  break;
               case MAX_LEVEL - 110:
                  sprintf( buf, "&WBishop&G" );
                  break;
               case MAX_LEVEL - 120:
                  sprintf( buf, "&WGd Lrd&G" );
                  break;
               case MAX_LEVEL - 130:
                  sprintf( buf, "&WLord  &G" );
                  break;
               case MAX_LEVEL - 140:
                  sprintf( buf, "&WAh Dke&G" );
                  break;
               case MAX_LEVEL - 150:
                  sprintf( buf, "&WDuke  &G" );
                  break;
               case MAX_LEVEL - 160:
                  sprintf( buf, "&WAh Drd&G" );
                  break;
               case MAX_LEVEL - 170:
                  sprintf( buf, "&WDruid &G" );
                  break;
               case MAX_LEVEL - 180:
                  sprintf( buf, "&WAh Agl&G" );
                  break;
               case MAX_LEVEL - 190:
                  sprintf( buf, "&WAngel &G" );
                  break;
               case MAX_LEVEL - 200:
                  sprintf( buf, "&WRetire&G" );
                  break;
            }
            switch ( gch->class )
            {
               default:
                  break;
               case 0:
                  sprintf( clt, "Mge" );
                  break;
               case 1:
                  sprintf( clt, "Clc" );
                  break;
               case 2:
                  sprintf( clt, "Thf" );
                  break;
               case 3:
                  sprintf( clt, "War" );
                  break;
               case 4:
                  sprintf( clt, "Vam" );
                  break;
               case 5:
                  sprintf( clt, "WWz" );
                  break;
               case 6:
                  sprintf( clt, "Jdi" );
                  break;
               case 7:
                  sprintf( clt, "Wer" );
                  break;
               case 8:
                  sprintf( clt, "Ran" );
                  break;
               case 9:
                  sprintf( clt, "Tae" );
                  break;
               case 10:
                  sprintf( clt, "Arc" );
                  break;
               case 11:
                  sprintf( clt, "Bar" );
                  break;
               case 12:
                  sprintf( clt, "Sor" );
                  break;
               case 13:
                  sprintf( clt, "Pal" );
                  break;
               case 14:
                  sprintf( clt, "Brd" );
                  break;
               case 15:
                  sprintf( clt, "Sam" );
                  break;
               case 16:
                  sprintf( clt, "Mnk" );
                  break;
            }
            switch ( gch->dualclass )
            {
               default:
                  break;
               case 0:
                  sprintf( clt2, "Mge" );
                  break;
               case 1:
                  sprintf( clt2, "Clc" );
                  break;
               case 2:
                  sprintf( clt2, "Thf" );
                  break;
               case 3:
                  sprintf( clt2, "War" );
                  break;
               case 4:
                  sprintf( clt2, "Vam" );
                  break;
               case 5:
                  sprintf( clt2, "WWz" );
                  break;
               case 6:
                  sprintf( clt2, "Jdi" );
                  break;
               case 7:
                  sprintf( clt2, "Wer" );
                  break;
               case 8:
                  sprintf( clt2, "Ran" );
                  break;
               case 9:
                  sprintf( clt2, "Tae" );
                  break;
               case 10:
                  sprintf( clt2, "Arc" );
                  break;
               case 11:
                  sprintf( clt2, "Bar" );
                  break;
               case 12:
                  sprintf( clt2, "Sor" );
                  break;
               case 13:
                  sprintf( clt2, "Pal" );
                  break;
               case 14:
                  sprintf( clt2, "Brd" );
                  break;
               case 15:
                  sprintf( clt2, "Sam" );
                  break;
               case 16:
                  sprintf( clt2, "Mnk" );
                  break;
            }

            set_char_color( AT_GREEN, ch );
            if( !IS_DUAL( gch ) )
            {
               ch_printf( ch, "[%5s %3.3s     ] ", buf, IS_NPC( gch ) ? "Mob" : clt );
            }
            else
            {
               ch_printf( ch, "[%5s %3.3s(%3.3s)] ", buf, IS_NPC( gch ) ? "Mob" : clt, IS_NPC( gch ) ? "   " : clt2 );
            }
            set_char_color( AT_GREEN, ch );
            ch_printf( ch, "%-12.12s ", capitalize( GPERS( gch, ch ) ) );
            if( gch->hit < gch->max_hit / 4 )
               set_char_color( AT_DANGER, ch );
            else if( gch->hit < gch->max_hit / 2.5 )
               set_char_color( AT_YELLOW, ch );
            else
               set_char_color( AT_GREY, ch );
            ch_printf( ch, "%d/%d hp  ", gch->hit, gch->max_hit );
            if( IS_VAMPIRE( gch ) )
               set_char_color( AT_BLOOD, ch );
            else
               set_char_color( AT_LBLUE, ch );
            ch_printf( ch, "%d/%d %s  ",
                       IS_VAMPIRE( gch ) ? gch->blood : gch->mana,
                       IS_VAMPIRE( gch ) ? gch->max_blood : gch->max_mana, IS_VAMPIRE( gch ) ? "bp" : "mn" );
            set_char_color( AT_DGREEN, ch );
            ch_printf( ch, "%d/%d mv ",   /*\n\r", */
                       gch->move, gch->max_move );
/*			if ( !IS_DUAL(gch) )
			{
		    set_char_color( AT_GREEN, ch );
		    switch (gch->position)
		    {
			case POS_DEAD:
				sprintf(buf2, "slowly decomposing");
			break;
			case POS_MORTAL:
				sprintf(buf2, "mortally wounded");
			break;
			case POS_INCAP:
				sprintf(buf2, "incapacitated");
			break;
			case POS_STUNNED:
				sprintf(buf2, "stunned");
			break;
			case POS_SLEEPING:
				sprintf(buf2, "sleeping");
			break;
			case POS_RESTING:
				sprintf(buf2, "resting");
			break;
			case POS_STANDING:
				sprintf(buf2, "standing");
			break;
			case POS_FIGHTING:
				sprintf(buf2, "fighting");
			break;
			case POS_EVASIVE:
				sprintf(buf2, "fighting (evasive)");   
		    break;
			case POS_DEFENSIVE:
                sprintf(buf2, "fighting (defensive)");
            break;
			case POS_AGGRESSIVE:
                sprintf(buf2, "fighting (aggressive)");
            break;
			case POS_BERSERK:
                sprintf(buf2, "fighting (berserk)");
            break;
			case POS_MOUNTED:
				sprintf(buf2, "mounted");
			break;
			case POS_SITTING:
				sprintf(buf2, "sitting");
			break;
			}*/
            if( gch->level < 100 )
            {
               sprintf( buf3, "%d", exp_level( gch, gch->level + 1 ) - gch->exp );
            }
            else
            {
               sprintf( buf3, "%d", IS_AGOD( gch ) ? get_exp_tnl( gch ) : exp_level( gch, gch->sublevel + 1 ) - gch->exp );
            }
            ch_printf( ch, "&G%4s tnl", buf3 );
/*		      ch_printf( ch, "\t\t\t\t    %4s tnl Position: %s",
				  buf3, buf2 );*/
            send_to_char( "\n\r", ch );
/*			}
			else
			{
		    set_char_color( AT_GREEN, ch );
		    switch (gch->position)
		    {
			case POS_DEAD:
				sprintf(buf2, "slowly decomposing");
			break;
			case POS_MORTAL:
				sprintf(buf2, "mortally wounded");
			break;
			case POS_INCAP:
				sprintf(buf2, "incapacitated");
			break;
			case POS_STUNNED:
				sprintf(buf2, "stunned");
			break;
			case POS_SLEEPING:
				sprintf(buf2, "sleeping");
			break;
			case POS_RESTING:
				sprintf(buf2, "resting");
			break;
			case POS_STANDING:
				sprintf(buf2, "standing");
			break;
			case POS_FIGHTING:
				sprintf(buf2, "fighting");
			break;
			case POS_EVASIVE:
				sprintf(buf2, "fighting (evasive)");   
		    break;
			case POS_DEFENSIVE:
                sprintf(buf2, "fighting (defensive)");
            break;
			case POS_AGGRESSIVE:
                sprintf(buf2, "fighting (aggressive)");
            break;
			case POS_BERSERK:
                sprintf(buf2, "fighting (berserk)");
            break;
			case POS_MOUNTED:
				sprintf(buf2, "mounted");
			break;
			case POS_SITTING:
				sprintf(buf2, "sitting");
			break;
			}
			if ( gch->level < 100 )
			{
				sprintf( buf3, "%d", exp_level( gch, gch->level+1) - gch->exp );
			}
			else
			{
				sprintf( buf3, "%d", IS_AGOD(gch) ? get_exp_tnl(gch) : exp_level( gch, gch->sublevel+1) - gch->exp );
			}
		      ch_printf( ch, "        &G[DualClas %3.3s] \t\t    %4s tnl Position: %s",
				  clt2,
				  buf3, buf2 );
		    send_to_char( "\n\r", ch);
			}*/
         }
      }
      return;
   }

   if( !strcmp( arg, "me" ) )
   {
      char chnl[MAX_STRING_LENGTH];

      if( IS_SET( ch->pcdata->flags, PCFLAG_GROUPWHO ) )
      {
         REMOVE_BIT( ch->pcdata->flags, PCFLAG_GROUPWHO );
         send_to_char( "You take yourself off of who group.\n\r", ch );
      }
      else
      {
         SET_BIT( ch->pcdata->flags, PCFLAG_GROUPWHO );
         if( ch->level < 100 )
         {
            if( ch->dualclass > -1 )
            {
               sprintf( chnl, "&R<&CGROUP INFO&R> &C%s, a level %d %s %s %s is looking for a group.&D",
                        ch->name, ch->level, get_race( ch ), get_class( ch ), get_dualclass( ch ) );
               talk_info( AT_PLAIN, chnl );
            }
            else
            {
               sprintf( chnl, "&R<&CGROUP INFO&R> &C%s, a level %d %s %s is looking for a group.&D",
                        ch->name, ch->level, get_race( ch ), get_class( ch ) );
               talk_info( AT_PLAIN, chnl );
            }
         }
         else
         {
            if( ch->dualclass > -1 )
            {
               sprintf( chnl, "&R<&CGROUP INFO&R> &C%s, a level %d(%d) %s %s %s is looking for a group.&D",
                        ch->name, ch->level, HAS_AWARD( ch ), get_race( ch ), get_class( ch ), get_dualclass( ch ) );
               talk_info( AT_PLAIN, chnl );
            }
            else
            {
               sprintf( chnl, "&R<&CGROUP INFO&R> &C%s, a level %d(%d) %s %s is looking for a group.&D",
                        ch->name, ch->level, HAS_AWARD( ch ), get_race( ch ), get_class( ch ) );
               talk_info( AT_PLAIN, chnl );
            }
         }
      }
      return;
   }

   if( !strcmp( arg, "disband" ) )
   {
      CHAR_DATA *gch;
      int count = 0;

      if( ch->leader || ch->master )
      {
         send_to_char( "You cannot disband a group if you're following someone.\n\r", ch );
         return;
      }

      for( gch = first_char; gch; gch = gch->next )
      {
         if( is_same_group( ch, gch ) && ( ch != gch ) )
         {
            gch->leader = NULL;
            gch->master = NULL;
            count++;
            send_to_char( "Your group is disbanded.\n\r", gch );
         }
      }

      if( count == 0 )
         send_to_char( "You have no group members to disband.\n\r", ch );
      else
         send_to_char( "You disband your group.\n\r", ch );

      return;
   }

   if( !strcmp( arg, "all" ) )
   {
      CHAR_DATA *rch;
      int count = 0;

      for( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
      {
         if( ch != rch && !IS_NPC( rch )
             && can_see( ch, rch ) && rch->master == ch
             && !ch->master && !ch->leader
             && abs( ch->level - rch->level ) < 9
             && !is_same_group( rch, ch )
             && IS_PKILL( ch ) == IS_PKILL( rch )
             && ( !IS_SET( ch->pcdata->flags, PCFLAG_IMMALT ) && !IS_SET( rch->pcdata->flags, PCFLAG_IMMALT ) ) )
         {
            rch->leader = ch;
            count++;
            if( IS_SET( rch->pcdata->flags, PCFLAG_GROUPWHO ) )
            {
               REMOVE_BIT( rch->pcdata->flags, PCFLAG_GROUPWHO );
            }
         }
      }

      if( count == 0 )
         send_to_char( "You have no eligible group members.\n\r", ch );
      else
      {
         act( AT_ACTION, "$n groups $s followers.", ch, NULL, NULL, TO_ROOM );
         send_to_char( "You group your followers.\n\r", ch );
         if( IS_SET( ch->pcdata->flags, PCFLAG_GROUPWHO ) )
         {
            REMOVE_BIT( ch->pcdata->flags, PCFLAG_GROUPWHO );
         }
      }
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( ch->master || ( ch->leader && ch->leader != ch ) )
   {
      send_to_char( "But you are following someone else!\n\r", ch );
      return;
   }

   if( victim->master != ch && ch != victim )
   {
      act( AT_PLAIN, "$N isn't following you.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if( victim == ch )
   {
      act( AT_PLAIN, "You can't group yourself.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if( ( IS_SET( victim->pcdata->flags, PCFLAG_IMMALT ) && !IS_SET( ch->pcdata->flags, PCFLAG_IMMALT ) )
       || ( !IS_SET( victim->pcdata->flags, PCFLAG_IMMALT ) && IS_SET( ch->pcdata->flags, PCFLAG_IMMALT ) ) )
   {
      act( AT_PLAIN, "You can't group that character.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if( is_same_group( victim, ch ) && ch != victim )
   {
      victim->leader = NULL;
      act( AT_ACTION, "$n removes $N from $s group.", ch, NULL, victim, TO_NOTVICT );
      act( AT_ACTION, "$n removes you from $s group.", ch, NULL, victim, TO_VICT );
      act( AT_ACTION, "You remove $N from your group.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if( ( !IS_IMMORTAL( ch ) && !IS_IMMORTAL( victim ) )
       && ( ch->level - victim->level < -20 || ch->level - victim->level > 20 || ( IS_PKILL( ch ) != IS_PKILL( victim ) ) ) )
   {
      act( AT_PLAIN, "$N cannot join $n's group.", ch, NULL, victim, TO_NOTVICT );
      act( AT_PLAIN, "You cannot join $n's group.", ch, NULL, victim, TO_VICT );
      act( AT_PLAIN, "$N cannot join your group.", ch, NULL, victim, TO_CHAR );
      return;
   }

   victim->leader = ch;
   act( AT_ACTION, "$N joins $n's group.", ch, NULL, victim, TO_NOTVICT );
   act( AT_ACTION, "You join $n's group.", ch, NULL, victim, TO_VICT );
   act( AT_ACTION, "$N joins your group.", ch, NULL, victim, TO_CHAR );
   if( IS_SET( ch->pcdata->flags, PCFLAG_GROUPWHO ) )
   {
      REMOVE_BIT( ch->pcdata->flags, PCFLAG_GROUPWHO );
   }
   if( IS_SET( victim->pcdata->flags, PCFLAG_GROUPWHO ) )
   {
      REMOVE_BIT( victim->pcdata->flags, PCFLAG_GROUPWHO );
   }
   return;
}

/*
void do_split( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount;
    int share;
    int extra;
    int type;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    
	
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Split <Amount> <Coin Type>\n\r", ch );
	send_to_char( "\n\rValid coin types: Katyr, Siam, Rona\n\r",ch);
	return;
    }
    
     * type 0 = gold
     * type 1 = silver
     * type 2 = copper
     *
	if ( !str_cmp("katyr",arg2)) 		type = 0;
	else if ( !str_cmp("siam",arg2))  type = 1;
	else if ( !str_cmp("sia",arg2))	type = 1;
	else if ( !str_cmp("rona",arg2))  type = 2;
	else if ( !str_cmp("ron",arg2)) 	type = 2;
	else {
		sprintf(buf,"%s is not a valid coin type.\n\r",arg2);
		send_to_char(buf,ch);
		return;
		}
    amount = atoi( arg1 );

    if ( amount < 0 )
    {
	send_to_char( "Your group wouldn't like that.\n\r", ch );
	return;
    }

    if ( amount == 0 )
    {
	send_to_char( "You hand out zero coins, but no one notices.\n\r", ch );
	return;
    }
	*
	 * Check for amount of Gold/Silver/Copper -Druid
	 *
	if (type == 0)	
    if ( ch->gold < amount )
    {
	send_to_char( "You don't have that much katyr.\n\r", ch );
	return;
    }
    if (type == 1)	
    if ( ch->silver < amount )
    {
	send_to_char( "You don't have that much siam.\n\r", ch );
	return;
    }
    if (type == 2)	
    if ( ch->copper < amount )
    {
	send_to_char( "You don't have that much rona.\n\r", ch );
	return;
    }
	
	if (type<0 || type >3)
	{
	send_to_char( "Invalid coin type, please try again\n\r",ch);
	bug("Invalid coin type: in do_split",0);
	return;
	}	
    members = 0;
    for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }
    
    if ( xIS_SET(ch->act, PLR_AUTOGOLD) && members < 2 )
	return;

    if ( members < 2 )
    {
	send_to_char( "Just keep it all.\n\r", ch );
	return;
    }

    share = amount / members;
    extra = amount % members;

    if ( share == 0 )
    {
	send_to_char( "Don't even bother, cheapskate.\n\r", ch );
	return;
    }
	if (type ==0)
	{
    ch->gold -= amount;
    ch->gold += share + extra;
	}
	if (type == 1)
	{
		ch->silver -= amount;
		ch->silver += share + extra;
	}
	if (type == 2)
	{
		ch->copper -= amount;
		ch->copper += share + extra;
	}
    set_char_color( AT_GOLD, ch );
    if (type == 0){
    ch_printf( ch,
	"You split %d katyr.  Your share is %d katyr.\n\r",
	amount, share + extra );

    sprintf( buf, "$n splits %d katyr.  Your share is %d katyr.",
	amount, share );
	}
	if (type == 1){
    ch_printf( ch,
	"You split %d siam.  Your share is %d siam.\n\r",
	amount, share + extra );

    sprintf( buf, "$n splits %d siam.  Your share is %d siam.",
	amount, share );
	}
	if (type == 2){
    ch_printf( ch,
	"You split %d rona.  Your share is %d rona.\n\r",
	amount, share + extra );

    sprintf( buf, "$n splits %d rona.  Your share is %d rona.",
	amount, share );
	}
    for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
    {
	if ( gch != ch && is_same_group( gch, ch ) )
	{
	    act( AT_GOLD, buf, ch, NULL, gch, TO_VICT );
	    if (type == 0) gch->gold += share;
	    if (type == 1) gch->silver += share;
	    if (type == 2) gch->copper += share;
	}
    }
    return;
}
*/
void do_split( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *gch;
   int members;
   int amount;
   int share;
   int extra;

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "Split how much?\n\r", ch );
      return;
   }

   amount = atoi( arg );

   if( amount < 0 )
   {
      send_to_char( "Your group wouldn't like that.\n\r", ch );
      return;
   }

   if( amount == 0 )
   {
      send_to_char( "You hand out zero coins, but no one notices.\n\r", ch );
      return;
   }

   if( ch->gold < amount )
   {
      send_to_char( "You don't have that much katyr.\n\r", ch );
      return;
   }

   members = 0;
   for( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
   {
      if( is_same_group( gch, ch ) )
         members++;
   }


   if( xIS_SET( ch->act, PLR_AUTOGOLD ) && members < 2 )
      return;

   if( members < 2 )
   {
      send_to_char( "Just keep it all.\n\r", ch );
      return;
   }

   share = amount / members;
   extra = amount % members;

   if( share == 0 )
   {
      send_to_char( "Don't even bother, cheapskate.\n\r", ch );
      return;
   }

   ch->gold -= amount;
   ch->gold += share + extra;

   set_char_color( AT_GOLD, ch );
   ch_printf( ch, "You split %d katyr.  Your share is %d katyr.\n\r", amount, share + extra );

   sprintf( buf, "$n splits %d katyr.  Your share is %d katyr.", amount, share );

   for( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
   {
      if( gch != ch && is_same_group( gch, ch ) )
      {
         act( AT_GOLD, buf, ch, NULL, gch, TO_VICT );
         gch->gold += share;
      }
   }
   return;
}



void do_gtell( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *gch;
#ifndef SCRAMBLE
   int speaking = -1, lang;

   for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
      if( ch->speaking & lang_array[lang] )
      {
         speaking = lang;
         break;
      }
#endif

   if( argument[0] == '\0' )
   {
      send_to_char( "Tell your group what?\n\r", ch );
      return;
   }

   if( xIS_SET( ch->act, PLR_NO_TELL ) )
   {
      send_to_char( "Your message didn't get through!\n\r", ch );
      return;
   }

   for( gch = first_char; gch; gch = gch->next )
   {
      if( is_same_group( gch, ch ) )
      {
         set_char_color( AT_GTELL, gch );
#ifndef SCRAMBLE
         if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
         {
            int speakswell = UMIN( knows_language( gch, ch->speaking, ch ),
                                   knows_language( ch, ch->speaking, gch ) );

            if( speakswell < 85 )
               ch_printf( gch, "&G%s tells the group '&w%s&G'.\n\r", ch->name,
                          translate( speakswell, argument, lang_names[speaking] ) );
            else
               ch_printf( gch, "&G%s tells the group '&w%s&G'.\n\r", ch->name, argument );
         }
         else
            ch_printf( gch, "&G%s tells the group '&w%s&G'.\n\r", ch->name, argument );
#else
         if( knows_language( gch, ch->speaking, gch ) || ( IS_NPC( ch ) && !ch->speaking ) )
            ch_printf( gch, "&G%s tells the group '&w%s&G'.\n\r", ch->name, argument );
         else
            ch_printf( gch, "&G%s tells the group '&w%s&G'.\n\r", ch->name, scramblechan( argument, ch->speaking ) );
#endif
      }
   }

   return;
}


bool is_same_group( CHAR_DATA * ach, CHAR_DATA * bch )
{
   if( ach->leader )
      ach = ach->leader;
   if( bch->leader )
      bch = bch->leader;
   return ach == bch;
}

void talk_auction( char *argument )
{
   DESCRIPTOR_DATA *d;
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *original;

   sprintf( buf, "&R<&zAUCTION&R>&P: %s", argument );

   for( d = first_descriptor; d; d = d->next )
   {
      original = d->original ? d->original : d->character;
      if( ( d->connected == CON_PLAYING ) && !xIS_SET( original->deaf, CHANNEL_AUCTION )
          && !xIS_SET( original->in_room->room_flags, ROOM_SILENCE ) && !NOT_AUTHED( original ) )
         act( AT_GOSSIP, buf, original, NULL, NULL, TO_CHAR );
   }
}

int knows_language( CHAR_DATA * ch, int language, CHAR_DATA * cch )
{
   sh_int sn;

   if( !IS_NPC( ch ) && IS_IMMORTAL( ch ) )
      return 100;
   if( IS_NPC( ch ) && !ch->speaks )
      return 100;
   if( IS_NPC( ch ) && IS_SET( ch->speaks, ( language & ~LANG_CLAN ) ) )
      return 100;
   if( IS_SET( language, LANG_COMMON ) )
      return 100;
   if( language & LANG_CLAN )
   {
      if( IS_NPC( ch ) || IS_NPC( cch ) )
         return 100;
      if( ch->pcdata->clan == cch->pcdata->clan && ch->pcdata->clan != NULL )
         return 100;
   }
   if( !IS_NPC( ch ) )
   {
      int lang;

      if( IS_SET( race_table[ch->race]->language, language ) )
         return 100;

      for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
         if( IS_SET( language, lang_array[lang] ) && IS_SET( ch->speaks, lang_array[lang] ) )
         {
            if( ( sn = skill_lookup( lang_names[lang] ) ) != -1 )
               return ch->pcdata->learned[sn];
         }
   }
   return 0;
}

bool can_learn_lang( CHAR_DATA * ch, int language )
{
   if( language & LANG_CLAN )
      return FALSE;
   if( IS_NPC( ch ) || IS_IMMORTAL( ch ) )
      return FALSE;
   if( race_table[ch->race]->language & language )
      return FALSE;
   if( ch->speaks & language )
   {
      int lang;

      for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
         if( language & lang_array[lang] )
         {
            int sn;

            if( !( VALID_LANGS & lang_array[lang] ) )
               return FALSE;
            if( ( sn = skill_lookup( lang_names[lang] ) ) < 0 )
            {
               bug( "Can_learn_lang: valid language without sn: %d", lang );
               continue;
            }
            if( ch->pcdata->learned[sn] >= 99 )
               return FALSE;
         }
   }
   if( VALID_LANGS & language )
      return TRUE;
   return FALSE;
}

int const lang_array[] = {
   LANG_COMMON, LANG_ELVEN, LANG_DWARVEN, LANG_PIXIE,
   LANG_OGRE, LANG_ORCISH, LANG_TROLLISH, LANG_RODENT,
   LANG_INSECTOID, LANG_MAMMAL, LANG_REPTILE,
   LANG_DRAGON, LANG_SPIRITUAL, LANG_MAGICAL,
   LANG_GOBLIN, LANG_GOD, LANG_ANCIENT, LANG_HALFLING,
   LANG_CLAN, LANG_GITH, LANG_UNKNOWN
};

char *const lang_names[] = {
   "common", "elvish", "dwarven", "pixie", "ogre",
   "orcish", "trollese", "rodent", "insectoid",
   "mammal", "reptile", "dragon", "spiritual",
   "magical", "goblin", "god", "ancient",
   "halfling", "clan", "gith", ""
};


int countlangs( int languages )
{
   int numlangs = 0;
   int looper;

   for( looper = 0; lang_array[looper] != LANG_UNKNOWN; looper++ )
   {
      if( lang_array[looper] == LANG_CLAN )
         continue;
      if( languages & lang_array[looper] )
         numlangs++;
   }
   return numlangs;
}

void do_speak( CHAR_DATA * ch, char *argument )
{
   int langs;
   char arg[MAX_INPUT_LENGTH];

   argument = one_argument( argument, arg );

   if( !str_cmp( arg, "all" ) && IS_IMMORTAL( ch ) )
   {
      set_char_color( AT_SAY, ch );
      ch->speaking = ~LANG_CLAN;
      send_to_char( "Now speaking all languages.\n\r", ch );
      return;
   }
   for( langs = 0; lang_array[langs] != LANG_UNKNOWN; langs++ )
      if( !str_prefix( arg, lang_names[langs] ) )
         if( knows_language( ch, lang_array[langs], ch ) )
         {
            if( lang_array[langs] == LANG_CLAN && ( IS_NPC( ch ) || !ch->pcdata->clan ) )
               continue;
            ch->speaking = lang_array[langs];
            set_char_color( AT_SAY, ch );
            ch_printf( ch, "You now speak %s.\n\r", lang_names[langs] );
            return;
         }
   set_char_color( AT_SAY, ch );
   send_to_char( "You do not know that language.\n\r", ch );
}

void do_languages( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   int lang;

   argument = one_argument( argument, arg );
   if( arg[0] != '\0' && !str_prefix( arg, "learn" ) && !IS_IMMORTAL( ch ) && !IS_NPC( ch ) )
   {
      CHAR_DATA *sch;
      char arg2[MAX_INPUT_LENGTH];
      int sn;
      int prct;
      int prac;

      argument = one_argument( argument, arg2 );
      if( arg2[0] == '\0' )
      {
         send_to_char( "Learn which language?\n\r", ch );
         return;
      }
      for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
      {
         if( lang_array[lang] == LANG_CLAN )
            continue;
         if( !str_prefix( arg2, lang_names[lang] ) )
            break;
      }
      if( lang_array[lang] == LANG_UNKNOWN )
      {
         send_to_char( "That is not a language.\n\r", ch );
         return;
      }
      if( !( VALID_LANGS & lang_array[lang] ) )
      {
         send_to_char( "You may not learn that language.\n\r", ch );
         return;
      }
      if( ( sn = skill_lookup( lang_names[lang] ) ) < 0 )
      {
         send_to_char( "That is not a language.\n\r", ch );
         return;
      }
      if( race_table[ch->race]->language & lang_array[lang] ||
          lang_array[lang] == LANG_COMMON || ch->pcdata->learned[sn] >= 99 )
      {
         act( AT_PLAIN, "You are already fluent in $t.", ch, lang_names[lang], NULL, TO_CHAR );
         return;
      }
      for( sch = ch->in_room->first_person; sch; sch = sch->next_in_room )
         if( IS_NPC( sch ) /*&& xIS_SET(sch->act, ACT_SCHOLAR) */
             && knows_language( sch, ch->speaking, ch )
             && knows_language( sch, lang_array[lang], sch )
             && ( !sch->speaking || knows_language( ch, sch->speaking, sch ) ) )
            break;
      if( !sch )
      {
         send_to_char( "There is no one who can teach that language here.\n\r", ch );
         return;
      }
      if( countlangs( ch->speaks ) >= ( ch->level / 10 ) && ch->pcdata->learned[sn] <= 0 )
      {
         act( AT_TELL, "$n tells you 'You may not learn a new language yet.'", sch, NULL, ch, TO_VICT );
         return;
      }
      prac = 2 - ( get_curr_cha( ch ) / 17 );
      if( ch->practice < prac )
      {
         act( AT_TELL, "$n tells you 'You do not have enough practices.'", sch, NULL, ch, TO_VICT );
         return;
      }
      ch->practice -= prac;
      prct = 5 + ( get_curr_int( ch ) / 6 ) + ( get_curr_wis( ch ) / 7 );
      ch->pcdata->learned[sn] += prct;
      ch->pcdata->learned[sn] = UMIN( ch->pcdata->learned[sn], 99 );
      SET_BIT( ch->speaks, lang_array[lang] );
      if( ch->pcdata->learned[sn] == prct )
         act( AT_PLAIN, "You begin lessons in $t.", ch, lang_names[lang], NULL, TO_CHAR );
      else if( ch->pcdata->learned[sn] < 60 )
         act( AT_PLAIN, "You continue lessons in $t.", ch, lang_names[lang], NULL, TO_CHAR );
      else if( ch->pcdata->learned[sn] < 60 + prct )
         act( AT_PLAIN, "You feel you can start communicating in $t.", ch, lang_names[lang], NULL, TO_CHAR );
      else if( ch->pcdata->learned[sn] < 99 )
         act( AT_PLAIN, "You become more fluent in $t.", ch, lang_names[lang], NULL, TO_CHAR );
      else
         act( AT_PLAIN, "You now speak perfect $t.", ch, lang_names[lang], NULL, TO_CHAR );
      return;
   }
   for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
      if( knows_language( ch, lang_array[lang], ch ) )
      {
         if( ch->speaking & lang_array[lang] || ( IS_NPC( ch ) && !ch->speaking ) )
            set_char_color( AT_SAY, ch );
         else
            set_char_color( AT_PLAIN, ch );
         send_to_char( lang_names[lang], ch );
         send_to_char( "\n\r", ch );
      }
   send_to_char( "\n\r", ch );
   return;
}

void do_traffic( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_TRAFFIC, "openly traffic" );
   return;
}

void do_wartalk( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_WARTALK, "war" );
   return;
}

void do_racetalk( CHAR_DATA * ch, char *argument )
{
   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   talk_channel( ch, argument, CHANNEL_RACETALK, race_table[ch->race]->race_name );
   return;
}

void do_beep( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   argument = one_argument( argument, arg );

   if( !*arg || !( victim = get_char_world( ch, arg ) ) )
   {
      send_to_char( "Beep who?\n\r", ch );
      return;
   }

   if( IS_NPC( victim ) )
   {
      send_to_char( "Beep who?\n\r", ch );
      return;
   }

   if( IS_SET( victim->pcdata->flags, PCFLAG_NOBEEP ) )
   {
      ch_printf( ch, "%s is not accepting beeps at this time.\n\r", victim->name );
      return;
   }

   ch_printf( victim, "%s is beeping you!\a\n\r", PERS( ch, victim ) );
   ch_printf( ch, "You beep %s.\n\r", PERS( victim, ch ) );
   return;
}

void do_rpemote( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *vic = NULL;

   if( argument[0] == '\0' )
   {
      send_to_char( "RPemote what?\n\r", ch );
      return;
   }

   for( vic = ch->in_room->first_person; vic; vic = vic->next )
   {
      pager_printf_color( vic, "&G**&g %s &G**&g\n\r", argument );
   }
   return;
}

/*
void do_xsocials( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;

    col = 0;

    for ( iSocial = 0; xsocial_table[iSocial] < MAX_XSOC; iSocial++ )
    {
	sprintf( buf, "%-12s", xsocial_table[iSocial]->name );
	send_to_char( buf, ch );
	if ( ++col % 6 == 0 )
	    send_to_char( "\n\r", ch );
    }
 
    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );

    return;
}
*/
#ifdef PROFANITY_CHECK
void init_profanity_checker(  )
{
}
#endif

void add_profane_word( char *word )
{
}

/*int is_profane (char *what)
{
  return(0); 
}*/

bool is_profane( char *what )
{

   if( !sysdata.profanityfilter )
      return FALSE;

   what = strlower( what );

   if( strstr( what, " fuck " ) )
      return TRUE;
   if( strstr( what, " shit " ) && !strstr( what, "shitake" ) )
      return TRUE;
   if( strstr( what, " bitch " ) )
      return TRUE;
   if( strstr( what, " bastard " ) )
      return TRUE;
   if( strstr( what, " faggot " ) )
      return TRUE;
   if( strstr( what, " pussy " ) )
      return TRUE;
   if( strstr( what, " cock " ) )
      return TRUE;
   if( strstr( what, " asshole " ) )
      return TRUE;
   if( strstr( what, " cunt " ) )
      return TRUE;
//    if (strstr( what, "piss" ))
//     return TRUE;
//    if (strstr( what, "crap" ) && !strstr( what, "scrap" ))
//     return TRUE;
   if( strstr( what, " ass " ) && !strstr( what, "assassin" ) )
      return TRUE;
   if( strstr( what, " whore " ) )
      return TRUE;
   if( strstr( what, " slut " ) )
      return TRUE;
   if( strstr( what, " twat " ) )
      return TRUE;
   if( strstr( what, " jackass " ) )
      return TRUE;
   if( strstr( what, " dick " ) )
      return TRUE;

   return FALSE;
}
