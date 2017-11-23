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
/*                              Rent Module                              */
/*************************************************************************/
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include "acadia.h"
void char_leaving( CHAR_DATA * ch, int howleft, int cost )
{
   OBJ_DATA *obj, *obj_next;
   int x, y;
#ifdef AUTO_AUTH
   AUTH_LIST *old_auth;

   old_auth = NULL;

   if( NEW_AUTH( ch ) )
      remove_from_auth( ch->name );
   else
   {
      old_auth = get_auth_name( ch->name );
      if( old_auth != NULL )
         if( old_auth->state == AUTH_ONLINE )
            old_auth->state = AUTH_OFFLINE;
   }
#endif

   ch->pcdata->rent = 0;
   ch->pcdata->norares = FALSE;
   ch->pcdata->autorent = FALSE;

   if( howleft == 2 )
      ch->pcdata->autorent = TRUE;

   if( ch->position == POS_MOUNTED )
      do_dismount( ch, "" );

   if( howleft == 4 && cost > 0 )
   {
      for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
      {
         obj_next = obj->next_content;
         if( obj->wear_loc != WEAR_NONE )
            unequip_char( ch, obj );
      }

      for( obj = ch->first_carrying; obj; obj = obj_next )
      {
         obj_next = obj->next_content;
         separate_obj( obj );
         obj_from_char( obj );
         if( !obj_next )
            obj_next = ch->first_carrying;
#ifdef OVERLANDCODE
         obj = obj_to_room( obj, ch->in_room, ch );
#else
         obj = obj_to_room( obj, ch->in_room );
#endif
      }
   }

   quitting_char = ch;
   save_char_obj( ch );

   if( sysdata.save_pets && ch->pcdata->pet )
   {
      act( AT_BYE, "$N follows $S master into the Void.", ch, NULL, ch->pcdata->pet, TO_ROOM );
      extract_char( ch->pcdata->pet, TRUE );
   }

   if( ch->pcdata->clan )
      save_clan( ch->pcdata->clan );

   saving_char = NULL;

   extract_char( ch, TRUE );
   for( x = 0; x < MAX_WEAR; x++ )
      for( y = 0; y < MAX_LAYERS; y++ )
         save_equipment[x][y] = NULL;
   return;
}

/* 
void do_quit( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj;
   char arg1[MAX_INPUT_LENGTH];
   sh_int room_chance;
   int level = ch->level, rentcost = 0;

   if ( IS_NPC(ch) )
   {
	send_to_char( "NPCs cannot use the quit command.\n\r", ch );
	return;
   }

   for( obj = ch->first_carrying; obj; obj = obj->next_content )
      rent_calculate( ch, obj, &rentcost );

   argument = one_argument( argument, arg1 );

   if( !str_cmp( arg1, "auto" ) && sysdata.RENT && rentcost > 0 )
   {
      room_chance = number_range( 1,3 );

	if ( room_chance > 2 )
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

    if ( ch->position == POS_FIGHTING )
    {
	set_char_color( AT_RED, ch );
	send_to_char( "No way! You are fighting.\n\r", ch );
	return;
    }

    if ( ch->position  < POS_STUNNED  )
    {
	set_char_color( AT_BLOOD, ch );
	send_to_char( "You're not DEAD yet.\n\r", ch );
	return;
    }

    if ( get_timer(ch, TIMER_RECENTFIGHT) > 0 && !IS_IMMORTAL(ch) )
    {
	set_char_color( AT_RED, ch );
	send_to_char( "Your adrenaline is pumping too hard to quit now!\n\r", ch );
	return;
    }

    if ( auction->item != NULL && ((ch == auction->buyer) || (ch == auction->seller) ) )
    {
	send_to_char("Wait until you have bought/sold the item on auction.\n\r", ch);
	return;
    }

    send_to_char( "&WYou make a hasty break for the confines of reality...\n\r", ch );
    if( sysdata.RENT && rentcost > 0 )
	send_to_char( "As you leave, your equipment falls to the floor!\n\r", ch );
    act( AT_SAY, "A strange voice says, 'We await your return, $n...'", ch, NULL, NULL, TO_CHAR );
    act( AT_BYE, "$n has left the game.", ch, NULL, NULL, TO_ROOM );
    set_char_color( AT_GREY, ch );

    sprintf( log_buf, "%s has quit.", ch->name );
    log_string_plus( log_buf, LOG_COMM, level );
    if( sysdata.RENT )
      char_leaving( ch, 4, rentcost );
    else
	char_leaving( ch, 0, rentcost );
    return;
}
*/
CHAR_DATA *find_innkeeper( CHAR_DATA * ch )
{
   CHAR_DATA *innkeeper;

   for( innkeeper = ch->in_room->first_person; innkeeper; innkeeper = innkeeper->next_in_room )
      if( IS_NPC( innkeeper ) && xIS_SET( innkeeper->act, ACT_INNKEEPER ) )
         break;

   return innkeeper;
}

void rent_calculate( CHAR_DATA * ch, OBJ_DATA * obj, int *rent )
{
   OBJ_DATA *tobj;
   if( obj->pIndexData->rent >= MIN_RENT )
      *rent += obj->pIndexData->rent * obj->count;
   for( tobj = obj->first_content; tobj; tobj = tobj->next_content )
      rent_calculate( ch, tobj, rent );
}

void rent_display( CHAR_DATA * ch, OBJ_DATA * obj, int *rent )
{
   OBJ_DATA *tobj;
   if( obj->pIndexData->rent >= MIN_RENT )
   {
      *rent += obj->pIndexData->rent * obj->count;
      ch_printf( ch, "%s:\t%d coins per day.\n\r", obj->short_descr, obj->pIndexData->rent );
   }
   for( tobj = obj->first_content; tobj; tobj = tobj->next_content )
      rent_display( ch, tobj, rent );
}

void rent_check( CHAR_DATA * ch, OBJ_DATA * obj )
{
   OBJ_DATA *tobj;
   if( obj->pIndexData->rent >= MIN_RENT )
   {
      obj_from_char( obj );
      extract_obj( obj );
   }
   for( tobj = obj->first_content; tobj; tobj = tobj->next_content )
      rent_check( ch, tobj );
}

void rent_leaving( CHAR_DATA * ch, OBJ_DATA * obj, int *rent )
{
   OBJ_DATA *tobj;
   if( obj->pIndexData->rent >= MIN_RENT )
   {
      *rent += obj->pIndexData->rent * obj->count;
      ch_printf( ch, "%s:\t%d coins per day.\n\r", obj->short_descr, obj->pIndexData->rent );
   }
   if( obj->pIndexData->rent == -1 )
   {
      if( obj->wear_loc != WEAR_NONE )
         unequip_char( ch, obj );
      separate_obj( obj );
      obj_from_char( obj );
      ch_printf( ch, "%s dissapears in a cloud of smoke!\n\r", obj->short_descr );
      extract_obj( obj );
   }
   for( tobj = obj->first_content; tobj; tobj = tobj->next_content )
      rent_leaving( ch, tobj, rent );
}

void scan_rent( CHAR_DATA * ch )
{
   OBJ_DATA *tobj;
   char buf[MAX_STRING_LENGTH];
   int rentcost = 0;
   int cost = 0;
   struct stat fst;

   sprintf( buf, "%s%c/%s", PLAYER_DIR, tolower( ch->name[0] ), capitalize( ch->name ) );
   if( stat( buf, &fst ) != -1 )
   {
      for( tobj = ch->first_carrying; tobj; tobj = tobj->next_content )
         rent_calculate( ch, tobj, &rentcost );
      cost = ( int )( ( rentcost * ( double )( time( 0 ) - fst.st_mtime ) ) / 86400 );

      if( IS_IMMORTAL( ch ) || !sysdata.RENT )
         cost = 0;

      if( ch->pcdata->autorent == TRUE )
         cost *= 5;

      if( ch->gold < cost || ch->pcdata->norares == TRUE )
      {
         for( tobj = ch->first_carrying; tobj; tobj = tobj->next_content )
            rent_check( ch, tobj );

         ch->gold = 0;
         set_char_color( AT_BLUE, ch );
         ch_printf( ch, "You ran up charges of %d in rent, but could not afford it!\n\r", cost + ch->pcdata->rent );
         send_to_char( "Your rare items have been sold to cover the debt.\n\r", ch );
         if( ch->pcdata->autorent == TRUE )
            send_to_char( "Note: You autorented to leave the game - your cost was multiplied by 5.\n\r", ch );
         sprintf( log_buf, "%s ran up %d in rent costs, but ran out of money. Rare items recirculated.", ch->name,
                  cost + ch->pcdata->rent );
         log_string_plus( log_buf, LOG_COMM, LEVEL_IMMORTAL );
         ch->pcdata->rent = 0;
         ch->pcdata->norares = FALSE;
         ch->pcdata->autorent = FALSE;
      }
      else
      {
         ch->gold -= cost;

         if( !IS_IMMORTAL( ch ) )
         {
            if( sysdata.RENT )
            {
               set_char_color( AT_BLUE, ch );
               ch_printf( ch, "You ran up charges of %d in rent.\n\r", cost + ch->pcdata->rent );
               if( ch->pcdata->autorent == TRUE )
                  send_to_char( "Note: You autorented to leave the game - your cost was multiplied by 5.\n\r", ch );
               sprintf( log_buf, "%s ran up %d in rent costs.", ch->name, cost + ch->pcdata->rent );
               log_string( log_buf );
               ch->pcdata->rent = 0;
               ch->pcdata->norares = FALSE;
               ch->pcdata->autorent = FALSE;
            }
         }
         else
         {
            sprintf( log_buf, "%s returns from beyond the void.", ch->name );
            log_string_plus( log_buf, LOG_COMM, ch->level );
         }
      }
   }
   return;
}

void do_offer( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *obj;
   CHAR_DATA *innkeeper;
   CHAR_DATA *victim;
   char buf[MAX_STRING_LENGTH];
   int rentcost;

   if( !( innkeeper = find_innkeeper( ch ) ) )
   {
      send_to_char( "You can only offer at an inn.\n\r", ch );
      return;
   }

   victim = innkeeper;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Get Real! Mobs can't offer!\n\r", ch );
      return;
   }

   rentcost = 0;

   if( sysdata.RENT )
   {
      act( AT_SOCIAL, "$n takes a look at your items.....", victim, NULL, ch, TO_VICT );
      set_char_color( AT_GREEN, ch );
      for( obj = ch->first_carrying; obj; obj = obj->next_content )
         rent_display( ch, obj, &rentcost );

      sprintf( buf, "$n says 'Your rent will cost you %d coins per day.'", rentcost );
      act( AT_SAY, buf, victim, NULL, ch, TO_VICT );

      if( IS_IMMORTAL( ch ) )
      {
         sprintf( buf, "$n says 'But for you, oh mighty %s, I shall waive my fees!", ch->name );
         act( AT_SAY, buf, victim, NULL, ch, TO_VICT );
      }
   }
   else
      send_to_char( "Rent is disabled. No cost applies.\n\r", ch );
   return;
}

void do_rent( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *obj;
   CHAR_DATA *innkeeper;
   CHAR_DATA *victim;
   char buf[MAX_STRING_LENGTH];
   char room_buf[MAX_STRING_LENGTH];
   int level = get_trust( ch );
   int rentcost;

   if( !( innkeeper = find_innkeeper( ch ) ) )
   {
      send_to_char( "You can only rent at an inn.\n\r", ch );
      return;
   }

   victim = innkeeper;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Get Real! Mobs can't rent!\n\r", ch );
      return;
   }

   if( auction->item != NULL && ( ( ch == auction->buyer ) || ( ch == auction->seller ) ) )
   {
      send_to_char( "Wait until you have bought/sold the item on auction.\n\r", ch );
      return;
   }

   rentcost = 0;
   if( sysdata.RENT )
   {
      act( AT_SOCIAL, "$n takes a look at your items.....", victim, NULL, ch, TO_VICT );
      set_char_color( AT_GREEN, ch );
      for( obj = ch->first_carrying; obj; obj = obj->next_content )
         rent_leaving( ch, obj, &rentcost );
      sprintf( buf, "$n says 'Your rent will cost you %d coins per day.'", rentcost );
      act( AT_SAY, buf, victim, NULL, ch, TO_VICT );

      if( IS_IMMORTAL( ch ) )
      {
         sprintf( buf, "$n says 'But for you, oh mighty %s, I shall waive my fees!", ch->name );
         act( AT_SAY, buf, victim, NULL, ch, TO_VICT );
         rentcost = 0;
      }

      if( ch->gold < rentcost )
      {
         act( AT_SAY, "$n says 'You cannot afford this much!!'", victim, NULL, ch, TO_VICT );
         return;
      }
   }

   act( AT_WHITE, "$n takes your equipment into storage, and shows you to your room.", victim, NULL, ch, TO_VICT );
   act( AT_SAY, "A strange voice says, 'We await your return, $n...'", ch, NULL, NULL, TO_CHAR );
   act( AT_BYE, "$n shows $N to $S room, and stores $S equipment.", victim, NULL, ch, TO_NOTVICT );
   set_char_color( AT_GREY, ch );

   sprintf( log_buf, "%s has rented, at a cost of %d per day.", ch->name, rentcost );
   sprintf( room_buf, "%s rented in: %s, %s", ch->name, ch->in_room->name, ch->in_room->area->name );
   log_string_plus( log_buf, LOG_COMM, level );
   log_string_plus( room_buf, LOG_COMM, level );

   char_leaving( ch, 0, rentcost );

   return;
}

void rent_adjust_pfile( char *argument )
{
   CHAR_DATA *temp, *ch;
   ROOM_INDEX_DATA *temproom, *original;
   OBJ_DATA *tobj;
   char fname[1024];
   char name[256];
   struct stat fst;
   bool loaded;
   DESCRIPTOR_DATA *d;
   int old_room_vnum;
   int cost = 0;
   int rentcost = 0;
   int x, y;

   one_argument( argument, name );

   for( temp = first_char; temp; temp = temp->next )
   {
      if( IS_NPC( temp ) )
         continue;
      if( !str_cmp( name, temp->name ) )
         break;
   }
   if( temp != NULL )
   {
      sprintf( log_buf, "Skipping rent adjustments for %s, player is online.", temp->name );
      log_string( log_buf );
      if( IS_IMMORTAL( temp ) )
      {
         sprintf( log_buf, "Immortal: Removing rent items from %s.", temp->name );
         log_string( log_buf );
         for( tobj = temp->first_carrying; tobj; tobj = tobj->next_content )
            rent_check( temp, tobj );
      }
      return;
   }

   temproom = get_room_index( ROOM_VNUM_RENTUPDATE );

   if( temproom == NULL )
   {
      bug( "Error in rent adjustment, temporary loading room is missing!", 0 );
      return;
   }

   name[0] = UPPER( name[0] );
   sprintf( fname, "%s%c/%s", PLAYER_DIR, tolower( name[0] ), capitalize( name ) );

   if( stat( fname, &fst ) != -1 )
   {
      CREATE( d, DESCRIPTOR_DATA, 1 );
      d->next = NULL;
      d->prev = NULL;
      d->connected = CON_GET_NAME;
      d->outsize = 2000;
      CREATE( d->outbuf, char, d->outsize );

      loaded = load_char_obj( d, name, FALSE );
      add_char( d->character );
      old_room_vnum = d->character->in_room->vnum;
      original = d->character->in_room;
      char_to_room( d->character, temproom );
      ch = d->character;
      d->character->desc = NULL;
      d->character->retran = old_room_vnum;
      d->character = NULL;
      DISPOSE( d->outbuf );
      DISPOSE( d );

      for( tobj = ch->first_carrying; tobj; tobj = tobj->next_content )
         rent_calculate( ch, tobj, &rentcost );

      cost = ( int )( ( rentcost * ( double )( time( 0 ) - fst.st_mtime ) ) / 86400 );

      if( IS_IMMORTAL( ch ) || !sysdata.RENT )
         cost = 0;

      if( ch->pcdata->autorent == TRUE )
         cost *= 5;

      if( ch->gold < cost )
      {
         for( tobj = ch->first_carrying; tobj; tobj = tobj->next_content )
            rent_check( ch, tobj );

         ch->gold = 0;
         ch->pcdata->rent += cost;
         ch->pcdata->norares = TRUE;
         sprintf( log_buf, "%s ran up %d in rent costs, but ran out of money. Rare items recirculated.", ch->name, cost );
         log_string( log_buf );
         sprintf( log_buf, "%s accrued %d in charges before running out.", ch->name, cost + ch->pcdata->rent );
         log_string( log_buf );
         if( ch->pcdata->autorent == TRUE )
         {
            sprintf( log_buf, "%s autorented to leave the game - costs were quintupled.", ch->name );
            log_string( log_buf );
         }
      }
      else
      {
         if( !IS_IMMORTAL( ch ) )
         {
            ch->gold -= cost;

            ch->pcdata->rent += cost;
            sprintf( log_buf, "%s paid rent charges of %d for the day.", ch->name, cost );
            log_string( log_buf );

            if( ch->pcdata->autorent == TRUE )
            {
               sprintf( log_buf, "%s autorented to leave the game - costs were quintupled.", ch->name );
               log_string( log_buf );
            }
         }
         else if( IS_IMMORTAL( ch ) )
         {
            sprintf( log_buf, "Immortal: Removing rent items from %s.", ch->name );
            log_string( log_buf );
            for( tobj = ch->first_carrying; tobj; tobj = tobj->next_content )
               rent_check( ch, tobj );
         }
      }

      char_from_room( ch );
      char_to_room( ch, original );

      quitting_char = ch;
      save_char_obj( ch );

      if( sysdata.save_pets && ch->pcdata->pet )
      {
         act( AT_BYE, "$N follows $S master into the Void.", ch, NULL, ch->pcdata->pet, TO_ROOM );
         extract_char( ch->pcdata->pet, TRUE );
      }

      if( ch->pcdata->clan )
         save_clan( ch->pcdata->clan );

      saving_char = NULL;

      extract_char( ch, TRUE );
      for( x = 0; x < MAX_WEAR; x++ )
         for( y = 0; y < MAX_LAYERS; y++ )
            save_equipment[x][y] = NULL;


      sprintf( log_buf, "Rent totals for %s updated sucessfully.", name );
      log_string( log_buf );

      return;
   }
   return;
}

int rent_scan_pfiles( char *dirname, char *filename, bool updating )
{
   FILE *fpChar;
   char fname[MAX_STRING_LENGTH];
   int adjust = 0;

   sprintf( fname, "%s/%s", dirname, filename );

   if( ( fpChar = fopen( fname, "r" ) ) == NULL )
   {
      perror( fname );
      return 0;
   }

   for( ;; )
   {
      int vnum, temp = 0, counter = 1;
      char letter;
      char *word;
      char *tempstring;
      OBJ_INDEX_DATA *pObjIndex;

      letter = fread_letter( fpChar );

      if( ( letter != '#' ) && ( !feof( fpChar ) ) )
         continue;

      word = feof( fpChar ) ? "End" : fread_word( fpChar );

      if( !str_cmp( word, "End" ) )
         break;

      if( !str_cmp( word, "OBJECT" ) )
      {
         word = feof( fpChar ) ? "End" : fread_word( fpChar );

         if( !str_cmp( word, "End" ) )
            break;

         if( !str_cmp( word, "Nest" ) )
         {
            temp = fread_number( fpChar );
            word = fread_word( fpChar );
         }

         if( !str_cmp( word, "Count" ) )
         {
            counter = fread_number( fpChar );
            word = fread_word( fpChar );
         }

         if( !str_cmp( word, "Name" ) )
         {
            tempstring = fread_string( fpChar );
            word = fread_word( fpChar );
         }

         if( !str_cmp( word, "ShortDescr" ) )
         {
            tempstring = fread_string( fpChar );
            word = fread_word( fpChar );
         }

         if( !str_cmp( word, "Description" ) )
         {
            tempstring = fread_string( fpChar );
            word = fread_word( fpChar );
         }

         if( !str_cmp( word, "Ovnum" ) )
         {
            vnum = fread_number( fpChar );
            if( ( get_obj_index( vnum ) ) == NULL )
            {
               bug( "rent_scan_pfiles: %s has bad obj vnum.", filename );
               adjust = 1;
            }
            else
            {
               pObjIndex = get_obj_index( vnum );
               if( pObjIndex->rent >= MIN_RENT )
               {
                  if( !updating )
                  {
                     pObjIndex->count += counter;
                     sprintf( log_buf, "%s: Counted %d of Vnum %d", filename, counter, vnum );
                     log_string( log_buf );
                  }
                  else
                     adjust = 1;
               }
            }
         }
      }
   }
   FCLOSE( fpChar );
   return ( adjust );
}

void corpse_scan( char *dirname, char *filename )
{
   FILE *fpChar;
   char fname[MAX_STRING_LENGTH];

   sprintf( fname, "%s/%s", dirname, filename );

   if( ( fpChar = fopen( fname, "r" ) ) == NULL )
   {
      perror( fname );
      return;
   }

   for( ;; )
   {
      int vnum, counter = 1, nest = 0;
      char letter;
      char *word;
      OBJ_INDEX_DATA *pObjIndex;

      letter = fread_letter( fpChar );

      if( ( letter != '#' ) && ( !feof( fpChar ) ) )
         continue;

      word = feof( fpChar ) ? "End" : fread_word( fpChar );

      if( !str_cmp( word, "End" ) )
         break;

      if( !str_cmp( word, "OBJECT" ) )
      {
         word = feof( fpChar ) ? "End" : fread_word( fpChar );

         if( !str_cmp( word, "End" ) )
            break;

         if( !str_cmp( word, "Nest" ) )
         {
            nest = fread_number( fpChar );
            word = fread_word( fpChar );
         }

         if( !str_cmp( word, "Count" ) )
         {
            counter = fread_number( fpChar );
            word = fread_word( fpChar );
         }

         if( !str_cmp( word, "Ovnum" ) )
         {
            vnum = fread_number( fpChar );
            if( ( get_obj_index( vnum ) ) == NULL )
            {
               bug( "corpse_scan: %s's corpse has bad obj vnum.", filename );
            }
            else
            {
               pObjIndex = get_obj_index( vnum );
               if( pObjIndex->rent >= MIN_RENT )
               {
                  pObjIndex->count += counter;
                  sprintf( log_buf, "%s: Counted %d of Vnum %d", filename, counter, vnum );
                  log_string( log_buf );
               }
            }
         }
      }
   }
   FCLOSE( fpChar );
   return;
}

void load_equipment_totals( void )
{
   DIR *dp;
   struct dirent *dentry;
   char directory_name[100];
   int adjust = 0;
   sh_int alpha_loop;

#ifdef PFILECODE
   check_pfiles( 255 );
#endif

   log_string( "Updating rare item counts....." );

   log_string( "Checking player files...." );

   for( alpha_loop = 0; alpha_loop <= 25; alpha_loop++ )
   {
      sprintf( directory_name, "%s%c", PLAYER_DIR, 'a' + alpha_loop );
      dp = opendir( directory_name );
      dentry = readdir( dp );
      while( dentry )
      {
         if( !str_cmp( dentry->d_name, "CVS" ) )
         {
            dentry = readdir( dp );
            continue;
         }
         if( dentry->d_name[0] != '.' )
         {
            adjust = rent_scan_pfiles( directory_name, dentry->d_name, FALSE );
            adjust = 0;
         }
         dentry = readdir( dp );
      }
      closedir( dp );
   }

   log_string( "Checking corpses...." );

   sprintf( directory_name, "%s", CORPSE_DIR );
   dp = opendir( directory_name );
   dentry = readdir( dp );
   while( dentry )
   {
      if( !str_cmp( dentry->d_name, "CVS" ) )
      {
         dentry = readdir( dp );
         continue;
      }
      if( dentry->d_name[0] != '.' )
         corpse_scan( directory_name, dentry->d_name );
      dentry = readdir( dp );
   }
   closedir( dp );

   return;
}

void rent_update( void )
{
   DIR *dp;
   struct dirent *dentry;
   char directory_name[100];
   int adjust = 0;
   sh_int alpha_loop;

   if( !sysdata.RENT )
      return;

   log_string( "Checking daily rent for players...." );

   for( alpha_loop = 0; alpha_loop <= 25; alpha_loop++ )
   {
      sprintf( directory_name, "%s%c", PLAYER_DIR, 'a' + alpha_loop );
      dp = opendir( directory_name );
      dentry = readdir( dp );
      while( dentry )
      {
         if( !str_cmp( dentry->d_name, "CVS" ) )
         {
            dentry = readdir( dp );
            continue;
         }
         if( dentry->d_name[0] != '.' )
         {
            adjust = rent_scan_pfiles( directory_name, dentry->d_name, TRUE );
            if( adjust == 1 )
            {
               rent_adjust_pfile( dentry->d_name );
               adjust = 0;
            }
         }
         dentry = readdir( dp );
      }
      closedir( dp );
   }
   log_string( "Daily rent updates completed." );
   return;
}
