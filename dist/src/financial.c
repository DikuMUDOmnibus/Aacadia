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
/*					   Banking module					                */
/************************************************************************/

#include "acadia.h"

void do_balance( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];

   argument = one_argument( argument, arg );

   if( IS_NPC( ch ) )
   {
      send_to_char( "Mobs don't have bank accounts.\n\r", ch );
      return;
   }

   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?", ch );
      return;
   }

   sprintf( buf, "Your account currently has %s katyr in it.\n\r", num_punct( ch->pcdata->balance ) );
   set_char_color( AT_YELLOW, ch );
   send_to_char( buf, ch );
   return;
}

void do_deposit( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   int amount;

   argument = one_argument( argument, arg );

   if( IS_NPC( ch ) )
   {
      send_to_char( "Mobs don't have bank accounts!\n\r", ch );
      return;
   }

   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?", ch );
      return;
   }

   if( !xIS_SET( ch->in_room->room_flags, ROOM_BANK ) )
   {
      send_to_char( "You are not in a bank.\n\r", ch );
      return;
   }

   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: 'deposit <all>' or 'deposit #'\n\r", ch );
      return;
   }

   if( !str_cmp( arg, "all" ) )
   {
      amount = ch->gold;
      ch->gold -= amount;
      ch->pcdata->balance += amount;
      sprintf( buf, "You deposit %s katyr.\n\rYour account now has %s katyr in it.", num_punct( amount ),
               num_punct( ch->pcdata->balance ) );
   }
   else if( is_number( arg ) )
   {
      amount = atoi( arg );

      if( ch->gold < amount )
      {
         send_to_char( "You don't have that much katyr!\n\r", ch );
         return;
      }
      if( amount < 0 )
      {
         send_to_char( "What kind of bank do you think I'm running here!\n\r", ch );
         return;
      }
      ch->gold -= amount;
      ch->pcdata->balance += amount;
      sprintf( buf, "You deposit %s katyr.\n\rYour account now has %s katyr in it.", num_punct( amount ),
               num_punct( ch->pcdata->balance ) );
   }
   else
   {
      send_to_char( "Syntax: 'deposit <all>' or 'deposit #'\n\r", ch );
      return;
   }
   act( AT_YELLOW, buf, ch, NULL, NULL, TO_CHAR );
   act( AT_YELLOW, "$n deposits some money.", ch, NULL, NULL, TO_ROOM );
   return;
}

void do_withdraw( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   int amount;

   argument = one_argument( argument, arg );

   if( IS_NPC( ch ) )
   {
      send_to_char( "Mobs don't have bank accounts!\n\r", ch );
      return;
   }

   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?", ch );
      return;
   }

   if( !xIS_SET( ch->in_room->room_flags, ROOM_BANK ) )
   {
      send_to_char( "You are not in a bank.\n\r", ch );
      return;
   }

   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: 'withdraw <all>' or 'withdraw #'\n\r", ch );
      return;
   }

   if( !str_cmp( arg, "all" ) )
   {
      amount = ch->pcdata->balance;
      ch->gold += amount;
      ch->pcdata->balance -= amount;
      sprintf( buf, "You withdraw %s katyr.\n\rYour account now has %s katyr in it.", num_punct( amount ),
               num_punct( ch->pcdata->balance ) );
   }
   else if( is_number( arg ) )
   {
      amount = atoi( arg );

      if( ch->pcdata->balance < amount )
      {
         send_to_char( "You don't have that much katyr in the bank!\n\r", ch );
         return;
      }

      if( amount < 0 )
      {
         send_to_char( "What kind of bank do you think I'm running here!\n\r", ch );
         return;
      }
      ch->gold += amount;
      ch->pcdata->balance -= amount;
      sprintf( buf, "You withdraw %s katyr.\n\rYour account now has %s katyr in it.", num_punct( amount ),
               num_punct( ch->pcdata->balance ) );
   }
   else
   {
      send_to_char( "Syntax: 'withdraw <all>' or 'withdraw #'\n\r", ch );
      return;
   }
   act( AT_YELLOW, buf, ch, NULL, NULL, TO_CHAR );
   act( AT_YELLOW, "$n withdraws some money.", ch, NULL, NULL, TO_ROOM );
   return;
}

void do_convert( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   int amount = 0, x = 0, y = 0;

   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   if( IS_NPC( ch ) )
   {
      send_to_char( "Mobs don't have need to convert!\n\r", ch );
      return;
   }

   if( NOT_AUTHED( ch ) )
   {
      send_to_char( "Huh?", ch );
      return;
   }

   if( !xIS_SET( ch->in_room->room_flags, ROOM_BANK ) )
   {
      send_to_char( "You are not in a bank.\n\r", ch );
      return;
   }

   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: convert # <katyr/siam/rona> <katyr/siam/rona>\n\r", ch );
      return;
   }
   if( is_number( arg ) )
   {
      if( arg2[0] == '\0' || arg3[0] == '\0' )
      {
         send_to_char( "Syntax: convert # <katyr/siam/rona> <katyr/siam/rona>\n\r", ch );
         return;
      }
      else if( !str_cmp( arg2, "katyr" ) && !str_cmp( arg3, "katyr" ) )
      {
         send_to_char( "You don't need to convert katyr to katyr.\n\r", ch );
         return;
      }
      else if( !str_cmp( arg2, "katyr" ) && !str_cmp( arg3, "siam" ) )
      {
         amount = atoi( arg );

         x = amount * 1000;
         ch->gold -= amount;
         if( x + ch->silver > MAX_GOLD )
         {
            x = ( MAX_GOLD - ch->silver );
            ch->silver = MAX_GOLD;
         }
         else
         {
            ch->silver += x;
         }
         sprintf( buf, "You convert %s katyr to %s siam.", num_punct( amount ), num_punct( x ) );
      }
      else if( !str_cmp( arg2, "katyr" ) && !str_cmp( arg3, "rona" ) )
      {
         amount = atoi( arg );

         y = amount * 10000;
         ch->gold -= amount;
         if( y + ch->copper > MAX_GOLD )
         {
            y = ( MAX_GOLD - ch->copper );
            ch->copper = MAX_GOLD;
         }
         else
         {
            ch->copper += y;
         }
         sprintf( buf, "You convert %s katyr to %s rona.", num_punct( amount ), num_punct( y ) );
      }
      else if( !str_cmp( arg2, "siam" ) && !str_cmp( arg3, "siam" ) )
      {
         send_to_char( "You don't need to convert siam to siam.\n\r", ch );
         return;
      }
      else if( !str_cmp( arg2, "siam" ) && !str_cmp( arg3, "rona" ) )
      {
         x = atoi( arg );

         y = x * 1000;
         ch->silver -= x;
         if( y + ch->copper > MAX_GOLD )
         {
            y = ( MAX_GOLD - ch->copper );
            ch->copper = MAX_GOLD;
         }
         else
         {
            ch->copper += y;
         }
         sprintf( buf, "You convert %s siam to %s rona.", num_punct( x ), num_punct( y ) );
      }
      else if( !str_cmp( arg2, "siam" ) && !str_cmp( arg3, "katyr" ) )
      {
         x = atoi( arg );

         if( x < 1000 )
         {
            send_to_char( "You need to specify a larger amount of siam.\n\r", ch );
            return;
         }

         amount = x / 1000;

         ch->silver -= x;
         ch->gold += amount;
         sprintf( buf, "You convert %s siam to %s katyr.", num_punct( x ), num_punct( amount ) );
      }
      else if( !str_cmp( arg2, "rona" ) && !str_cmp( arg3, "rona" ) )
      {
         send_to_char( "You don't need to convert rona to rona.\n\r", ch );
         return;
      }
      else if( !str_cmp( arg2, "rona" ) && !str_cmp( arg3, "siam" ) )
      {
         y = atoi( arg );

         if( y < 1000 )
         {
            send_to_char( "You need to specify a larger amount of rona.\n\r", ch );
            return;
         }

         x = y / 1000;

         ch->copper -= y;
         ch->silver += x;
         sprintf( buf, "You convert %s rona to %s siam.", num_punct( y ), num_punct( x ) );
      }
      else if( !str_cmp( arg2, "rona" ) && !str_cmp( arg3, "katyr" ) )
      {
         y = atoi( arg );

         if( y < 10000 )
         {
            send_to_char( "You need to specify a larger amount of rona.\n\r", ch );
            return;
         }

         amount = y / 10000;

         ch->copper -= y;
         ch->gold += amount;
         sprintf( buf, "You convert %s rona to %s katyr.", num_punct( y ), num_punct( amount ) );
      }
   }
   act( AT_YELLOW, buf, ch, NULL, NULL, TO_CHAR );
   act( AT_YELLOW, "$n converts some money.", ch, NULL, NULL, TO_ROOM );
   return;
}
