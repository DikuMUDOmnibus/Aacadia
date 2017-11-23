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
/*		Boards 2 module				                    */
/************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "acadia.h"
#ifdef USE_IMC
#include "imc-mercbase.h"
#endif


#define VOTE_NONE 0
#define VOTE_OPEN 1
#define VOTE_CLOSED 2

BOARD_DATA *first_board;
BOARD_DATA *last_board;

bool is_note_to args( ( CHAR_DATA * ch, NOTE_DATA * pnote ) );
void note_attach args( ( CHAR_DATA * ch ) );
void note_remove args( ( CHAR_DATA * ch, BOARD_DATA * board, NOTE_DATA * pnote ) );



bool can_remove( CHAR_DATA * ch, BOARD_DATA * board )
{
   if( get_trust( ch ) >= board->min_remove_level )
      return TRUE;

   if( board->extra_removers[0] != '\0' )
   {
      if( is_name( ch->name, board->extra_removers ) )
         return TRUE;
   }
   return FALSE;
}

bool can_read( CHAR_DATA * ch, BOARD_DATA * board )
{
   if( get_trust( ch ) >= board->min_read_level )
      return TRUE;

   if( board->read_group[0] != '\0' )
   {
      if( ch->pcdata->clan && !str_cmp( ch->pcdata->clan->name, board->read_group ) )
         return TRUE;
      if( ch->pcdata->council && !str_cmp( ch->pcdata->council->name, board->read_group ) )
         return TRUE;
   }
   if( board->extra_readers[0] != '\0' )
   {
      if( is_name( ch->name, board->extra_readers ) )
         return TRUE;
   }
   return FALSE;
}

bool can_post( CHAR_DATA * ch, BOARD_DATA * board )
{
   if( get_trust( ch ) >= board->min_post_level )
      return TRUE;

   if( board->post_group[0] != '\0' )
   {
      if( ch->pcdata->clan && !str_cmp( ch->pcdata->clan->name, board->post_group ) )
         return TRUE;
      if( ch->pcdata->council && !str_cmp( ch->pcdata->council->name, board->post_group ) )
         return TRUE;
   }
   return FALSE;
}


void write_boards_txt(  )
{
   BOARD_DATA *tboard;
   FILE *fpout;
   char filename[256];

   sprintf( filename, "%s%s", BOARD_DIR, BOARD_FILE );
   fpout = fopen( filename, "w" );
   if( !fpout )
   {
      bug( "FATAL: cannot open board.txt for writing!\n\r", 0 );
      return;
   }
   for( tboard = first_board; tboard; tboard = tboard->next )
   {
      fprintf( fpout, "Filename          %s~\n", tboard->note_file );
      fprintf( fpout, "Vnum              %d\n", tboard->board_obj );
      fprintf( fpout, "Min_read_level    %d\n", tboard->min_read_level );
      fprintf( fpout, "Min_post_level    %d\n", tboard->min_post_level );
      fprintf( fpout, "Min_remove_level  %d\n", tboard->min_remove_level );
      fprintf( fpout, "Max_posts         %d\n", tboard->max_posts );
      fprintf( fpout, "Type 	           %d\n", tboard->type );
      fprintf( fpout, "Read_group        %s~\n", tboard->read_group );
      fprintf( fpout, "Post_group        %s~\n", tboard->post_group );
      fprintf( fpout, "Extra_readers     %s~\n", tboard->extra_readers );
      fprintf( fpout, "Extra_removers    %s~\n", tboard->extra_removers );
      if( tboard->ocopymessg )
         fprintf( fpout, "OCopymessg	   %s~\n", tboard->ocopymessg );
      if( tboard->olistmessg )
         fprintf( fpout, "OListmessg	   %s~\n", tboard->olistmessg );
      if( tboard->opostmessg )
         fprintf( fpout, "OPostmessg	   %s~\n", tboard->opostmessg );
      if( tboard->oreadmessg )
         fprintf( fpout, "OReadmessg	   %s~\n", tboard->oreadmessg );
      if( tboard->oremovemessg )
         fprintf( fpout, "ORemovemessg      %s~\n", tboard->oremovemessg );
      if( tboard->otakemessg )
         fprintf( fpout, "OTakemessg	   %s~\n", tboard->otakemessg );
      if( tboard->postmessg )
         fprintf( fpout, "Postmessg	   %s~\n", tboard->postmessg );
      fprintf( fpout, "End\n" );
   }
   fclose( fpout );
}

BOARD_DATA *get_board( OBJ_DATA * obj )
{
   BOARD_DATA *board;

   for( board = first_board; board; board = board->next )
      if( board->board_obj == obj->pIndexData->vnum )
         return board;
   return NULL;
}

BOARD_DATA *find_board( CHAR_DATA * ch )
{
   OBJ_DATA *obj;
   BOARD_DATA *board;

   for( obj = ch->in_room->first_content; obj; obj = obj->next_content )
   {
      if( ( board = get_board( obj ) ) != NULL )
         return board;
   }

   return NULL;
}

void note_attach( CHAR_DATA * ch )
{
   NOTE_DATA *pnote;

   if( ch->pnote )
      return;

   CREATE( pnote, NOTE_DATA, 1 );
   pnote->next = NULL;
   pnote->prev = NULL;
   pnote->sender = QUICKLINK( ch->name );
   pnote->date = STRALLOC( "" );
   pnote->to_list = STRALLOC( "" );
   pnote->subject = STRALLOC( "" );
   pnote->text = STRALLOC( "" );
   ch->pnote = pnote;
   return;
}

void write_board( BOARD_DATA * board )
{
   FILE *fp;
   char filename[256];
   NOTE_DATA *pnote;

   fclose( fpReserve );
   sprintf( filename, "%s%s", BOARD_DIR, board->note_file );
   if( ( fp = fopen( filename, "w" ) ) == NULL )
   {
      perror( filename );
   }
   else
   {
      for( pnote = board->first_note; pnote; pnote = pnote->next )
      {
         fprintf( fp,
                  "Sender  %s~\nDate    %s~\nTo      %s~\nSubject %s~\nVoting %d\nYesvotes %s~\nNovotes %s~\nAbstentions %s~\nText\n%s~\n\n",
                  pnote->sender, pnote->date, pnote->to_list, pnote->subject, pnote->voting, pnote->yesvotes, pnote->novotes,
                  pnote->abstentions, pnote->text );
      }
      fclose( fp );
   }
   fpReserve = fopen( NULL_FILE, "r" );
   return;
}


void free_note( NOTE_DATA * pnote )
{
   STRFREE( pnote->text );
   STRFREE( pnote->subject );
   STRFREE( pnote->to_list );
   STRFREE( pnote->date );
   STRFREE( pnote->sender );
   if( pnote->yesvotes )
      DISPOSE( pnote->yesvotes );
   if( pnote->novotes )
      DISPOSE( pnote->novotes );
   if( pnote->abstentions )
      DISPOSE( pnote->abstentions );
   DISPOSE( pnote );
}

void note_remove( CHAR_DATA * ch, BOARD_DATA * board, NOTE_DATA * pnote )
{

   if( !board )
   {
      bug( "note remove: null board", 0 );
      return;
   }

   if( !pnote )
   {
      bug( "note remove: null pnote", 0 );
      return;
   }

   UNLINK( pnote, board->first_note, board->last_note, next, prev );

   --board->num_posts;
   free_note( pnote );
   write_board( board );
}


OBJ_DATA *find_quill( CHAR_DATA * ch )
{
   OBJ_DATA *quill;

   for( quill = ch->last_carrying; quill; quill = quill->prev_content )
      if( quill->item_type == ITEM_PEN && can_see_obj( ch, quill ) )
         return quill;
   return NULL;
}

void do_noteroom( CHAR_DATA * ch, char *argument )
{
   BOARD_DATA *board;
   char arg[MAX_STRING_LENGTH];
   char arg_passed[MAX_STRING_LENGTH];

   strcpy( arg_passed, argument );

   switch ( ch->substate )
   {
      case SUB_WRITING_NOTE:
         break;

      default:

         argument = one_argument( argument, arg );
         smash_tilde( argument );
         if( !str_cmp( arg, "write" ) || !str_cmp( arg, "to" ) || !str_cmp( arg, "subject" ) || !str_cmp( arg, "show" ) )
         {
            return;
         }

         board = find_board( ch );
         if( !board )
         {
            send_to_char( "There is no bulletin board here to look at.\n\r", ch );
            return;
         }

         if( board->type != BOARD_NOTE )
         {
            send_to_char( "You can only use note commands on a note board.\n\r", ch );
            return;
         }
         else
         {
            return;
         }
   }
}

void do_mailroom( CHAR_DATA * ch, char *argument )
{
   BOARD_DATA *board;
   char arg[MAX_STRING_LENGTH];
   char arg_passed[MAX_STRING_LENGTH];

   strcpy( arg_passed, argument );

   switch ( ch->substate )
   {
      case SUB_WRITING_NOTE:
         break;

      default:

         argument = one_argument( argument, arg );
         smash_tilde( argument );
         if( !str_cmp( arg, "write" ) || !str_cmp( arg, "to" ) || !str_cmp( arg, "subject" ) || !str_cmp( arg, "show" ) )
         {
            return;
         }

         board = find_board( ch );
         if( !board )
         {
            send_to_char( "There is no mail facility here.\n\r", ch );
            return;
         }

         if( board->type != BOARD_MAIL )
         {
            send_to_char( "You can only use mail commands in a post office.\n\r", ch );
            return;
         }
         else
         {
            return;
         }
   }
}

BOARD_DATA *read_board( char *boardfile, FILE * fp )
{
   BOARD_DATA *board;
   char *word;
   char buf[MAX_STRING_LENGTH];
   bool fMatch;
   char letter;

   do
   {
      letter = getc( fp );
      if( feof( fp ) )
      {
         fclose( fp );
         return NULL;
      }
   }
   while( isspace( letter ) );
   ungetc( letter, fp );

   CREATE( board, BOARD_DATA, 1 );
   board->otakemessg = NULL;
   board->opostmessg = NULL;
   board->oremovemessg = NULL;
   board->olistmessg = NULL;
   board->ocopymessg = NULL;
   board->oreadmessg = NULL;
   board->postmessg = NULL;

#ifdef KEY
#undef KEY
#endif
#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}


   for( ;; )
   {
      word = feof( fp ) ? "End" : fread_word( fp );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
         case '*':
            fMatch = TRUE;
            fread_to_eol( fp );
            break;
         case 'E':
            KEY( "Extra_readers", board->extra_readers, fread_string_nohash( fp ) );
            KEY( "Extra_removers", board->extra_removers, fread_string_nohash( fp ) );
            if( !str_cmp( word, "End" ) )
            {
               board->num_posts = 0;
               board->first_note = NULL;
               board->last_note = NULL;
               board->next = NULL;
               board->prev = NULL;
               if( !board->read_group )
                  board->read_group = str_dup( "" );
               if( !board->post_group )
                  board->post_group = str_dup( "" );
               if( !board->extra_readers )
                  board->extra_readers = str_dup( "" );
               if( !board->extra_removers )
                  board->extra_removers = str_dup( "" );
               return board;
            }
         case 'F':
            KEY( "Filename", board->note_file, fread_string_nohash( fp ) );
         case 'M':
            KEY( "Min_read_level", board->min_read_level, fread_number( fp ) );
            KEY( "Min_post_level", board->min_post_level, fread_number( fp ) );
            KEY( "Min_remove_level", board->min_remove_level, fread_number( fp ) );
            KEY( "Max_posts", board->max_posts, fread_number( fp ) );
         case 'O':
            KEY( "OTakemessg", board->otakemessg, fread_string_nohash( fp ) );
            KEY( "OCopymessg", board->ocopymessg, fread_string_nohash( fp ) );
            KEY( "OReadmessg", board->oreadmessg, fread_string_nohash( fp ) );
            KEY( "ORemovemessg", board->oremovemessg, fread_string_nohash( fp ) );
            KEY( "OListmessg", board->olistmessg, fread_string_nohash( fp ) );
            KEY( "OPostmessg", board->opostmessg, fread_string_nohash( fp ) );
         case 'P':
            KEY( "Post_group", board->post_group, fread_string_nohash( fp ) );
            KEY( "Postmessg", board->postmessg, fread_string_nohash( fp ) );
         case 'R':
            KEY( "Read_group", board->read_group, fread_string_nohash( fp ) );
         case 'T':
            KEY( "Type", board->type, fread_number( fp ) );
         case 'V':
            KEY( "Vnum", board->board_obj, fread_number( fp ) );
      }
      if( !fMatch )
      {
         sprintf( buf, "read_board: no match: %s", word );
         bug( buf, 0 );
      }
   }

   return board;
}

NOTE_DATA *read_note( char *notefile, FILE * fp )
{
   NOTE_DATA *pnote;
   char *word;

   for( ;; )
   {
      char letter;

      do
      {
         letter = getc( fp );
         if( feof( fp ) )
         {
            fclose( fp );
            return NULL;
         }
      }
      while( isspace( letter ) );
      ungetc( letter, fp );

      CREATE( pnote, NOTE_DATA, 1 );

      if( str_cmp( fread_word( fp ), "sender" ) )
         break;
      pnote->sender = fread_string( fp );

      if( str_cmp( fread_word( fp ), "date" ) )
         break;
      pnote->date = fread_string( fp );

      if( str_cmp( fread_word( fp ), "to" ) )
         break;
      pnote->to_list = fread_string( fp );

      if( str_cmp( fread_word( fp ), "subject" ) )
         break;
      pnote->subject = fread_string( fp );

      word = fread_word( fp );
      if( !str_cmp( word, "voting" ) )
      {
         pnote->voting = fread_number( fp );

         if( str_cmp( fread_word( fp ), "yesvotes" ) )
            break;
         pnote->yesvotes = fread_string_nohash( fp );

         if( str_cmp( fread_word( fp ), "novotes" ) )
            break;
         pnote->novotes = fread_string_nohash( fp );

         if( str_cmp( fread_word( fp ), "abstentions" ) )
            break;
         pnote->abstentions = fread_string_nohash( fp );

         word = fread_word( fp );
      }

      if( str_cmp( word, "text" ) )
         break;
      pnote->text = fread_string( fp );

      if( !pnote->yesvotes )
         pnote->yesvotes = str_dup( "" );
      if( !pnote->novotes )
         pnote->novotes = str_dup( "" );
      if( !pnote->abstentions )
         pnote->abstentions = str_dup( "" );
      pnote->next = NULL;
      pnote->prev = NULL;
      return pnote;
   }
   bug( "read_note: bad key word." );
   exit( 1 );
   return NULL;
}

void load_boards( void )
{
   FILE *board_fp;
   FILE *note_fp;
   BOARD_DATA *board;
   NOTE_DATA *pnote;
   char boardfile[256];
   char notefile[256];

   first_board = NULL;
   last_board = NULL;

   sprintf( boardfile, "%s%s", BOARD_DIR, BOARD_FILE );
   if( ( board_fp = fopen( boardfile, "r" ) ) == NULL )
      return;

   while( ( board = read_board( boardfile, board_fp ) ) != NULL )
   {
      LINK( board, first_board, last_board, next, prev );
      sprintf( notefile, "%s%s", BOARD_DIR, board->note_file );
      log_string( notefile );
      if( ( note_fp = fopen( notefile, "r" ) ) != NULL )
      {
         while( ( pnote = read_note( notefile, note_fp ) ) != NULL )
         {
            LINK( pnote, board->first_note, board->last_note, next, prev );
            board->num_posts++;
         }
      }
   }
   return;
}


void do_makeboard( CHAR_DATA * ch, char *argument )
{
   BOARD_DATA *board;

   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Usage: makeboard <filename>\n\r", ch );
      return;
   }

   smash_tilde( argument );

   CREATE( board, BOARD_DATA, 1 );

   LINK( board, first_board, last_board, next, prev );
   board->note_file = str_dup( strlower( argument ) );
   board->read_group = str_dup( "" );
   board->post_group = str_dup( "" );
   board->extra_readers = str_dup( "" );
   board->extra_removers = str_dup( "" );
}

void do_bset( CHAR_DATA * ch, char *argument )
{
   BOARD_DATA *board;
   bool found;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int value;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   set_char_color( AT_NOTE, ch );
   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Usage: bset <board filename> <field> value\n\r", ch );
      send_to_char( "\n\rField being one of:\n\r", ch );
      send_to_char( "  ovnum read post remove maxpost filename type\n\r", ch );
      send_to_char( "  read_group post_group extra_readers extra_removers\n\r", ch );
      send_to_char( "The following will affect how an action is sent:\n\r", ch );
      send_to_char( "  oremove otake olist oread ocopy opost postmessg\n\r", ch );
      return;
   }

   value = atoi( argument );
   found = FALSE;
   for( board = first_board; board; board = board->next )
      if( !str_cmp( arg1, board->note_file ) )
      {
         found = TRUE;
         break;
      }
   if( !found )
   {
      send_to_char( "Board not found.\n\r", ch );
      return;
   }

   if( !str_cmp( arg2, "ovnum" ) )
   {
      if( !get_obj_index( value ) )
      {
         send_to_char( "No such object.\n\r", ch );
         return;
      }
      board->board_obj = value;
      write_boards_txt(  );
      send_to_char( "Done.  (board's object vnum set)\n\r", ch );
      return;
   }

   if( !str_cmp( arg2, "read" ) )
   {
      if( value < 0 || value > MAX_LEVEL )
      {
         send_to_char( "Value outside valid character level range.\n\r", ch );
         return;
      }
      board->min_read_level = value;
      write_boards_txt(  );
      send_to_char( "Done.  (minimum reading level set)\n\r", ch );
      return;
   }

   if( !str_cmp( arg2, "read_group" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "No reading group specified.\n\r", ch );
         return;
      }
      DISPOSE( board->read_group );
      if( !str_cmp( argument, "none" ) )
         board->read_group = str_dup( "" );
      else
         board->read_group = str_dup( argument );
      write_boards_txt(  );
      send_to_char( "Done.  (reading group set)\n\r", ch );
      return;
   }

   if( !str_cmp( arg2, "post_group" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "No posting group specified.\n\r", ch );
         return;
      }
      DISPOSE( board->post_group );
      if( !str_cmp( argument, "none" ) )
         board->post_group = str_dup( "" );
      else
         board->post_group = str_dup( argument );
      write_boards_txt(  );
      send_to_char( "Done.  (posting group set)\n\r", ch );
      return;
   }
   if( !str_cmp( arg2, "postmessg" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "No message specified.\n\r", ch );
         return;
      }
      if( board->postmessg )
         DISPOSE( board->postmessg );
      if( !str_cmp( argument, "none" ) )
         board->postmessg = NULL;
      else
      {
         sprintf( buf, "%s", argument );
         board->postmessg = str_dup( buf );
      }
      write_boards_txt(  );
      send_to_char( "Done.\n\r", ch );
      return;
   }
   if( !str_cmp( arg2, "opost" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "No message specified.\n\r", ch );
         return;
      }
      if( board->opostmessg )
         DISPOSE( board->opostmessg );
      if( !str_cmp( argument, "none" ) )
         board->opostmessg = NULL;
      else
      {
         sprintf( buf, "%s", argument );
         board->opostmessg = str_dup( buf );
      }
      write_boards_txt(  );
      send_to_char( "Done.\n\r", ch );
      return;
   }
   if( !str_cmp( arg2, "oremove" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "No message specified.\n\r", ch );
         return;
      }
      if( board->oremovemessg )
         DISPOSE( board->oremovemessg );
      if( !str_cmp( argument, "none" ) )
         board->oremovemessg = NULL;
      else
      {
         sprintf( buf, "%s", argument );
         board->oremovemessg = str_dup( buf );
      }
      write_boards_txt(  );
      send_to_char( "Done.\n\r", ch );
      return;
   }
   if( !str_cmp( arg2, "otake" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "No message specified.\n\r", ch );
         return;
      }
      if( board->otakemessg )
         DISPOSE( board->otakemessg );
      if( !str_cmp( argument, "none" ) )
         board->otakemessg = NULL;
      else
      {
         sprintf( buf, "%s", argument );
         board->otakemessg = str_dup( buf );
      }
      write_boards_txt(  );
      send_to_char( "Done.\n\r", ch );
      return;
   }
   if( !str_cmp( arg2, "ocopy" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "No message specified.\n\r", ch );
         return;
      }
      if( board->ocopymessg )
         DISPOSE( board->ocopymessg );
      if( !str_cmp( argument, "none" ) )
         board->ocopymessg = NULL;
      else
      {
         sprintf( buf, "%s", argument );
         board->ocopymessg = str_dup( buf );
      }
      write_boards_txt(  );
      send_to_char( "Done.\n\r", ch );
      return;
   }
   if( !str_cmp( arg2, "oread" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "No message sepcified.\n\r", ch );
         return;
      }
      if( board->oreadmessg )
         DISPOSE( board->oreadmessg );
      if( !str_cmp( argument, "none" ) )
         board->oreadmessg = NULL;
      else
      {
         sprintf( buf, "%s", argument );
         board->oreadmessg = str_dup( buf );
      }
      write_boards_txt(  );
      send_to_char( "Done.\n\r", ch );
      return;
   }
   if( !str_cmp( arg2, "olist" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "No message specified.\n\r", ch );
         return;
      }
      if( board->olistmessg )
         DISPOSE( board->olistmessg );
      if( !str_cmp( argument, "none" ) )
         board->olistmessg = NULL;
      else
      {
         sprintf( buf, "%s", argument );
         board->olistmessg = str_dup( buf );
      }
      write_boards_txt(  );
      send_to_char( "Done.\n\r", ch );
      return;
   }
   if( !str_cmp( arg2, "extra_removers" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "No names specified.\n\r", ch );
         return;
      }
      if( !str_cmp( argument, "none" ) )
         buf[0] = '\0';
      else
         sprintf( buf, "%s %s", board->extra_removers, argument );
      DISPOSE( board->extra_removers );
      board->extra_removers = str_dup( buf );
      write_boards_txt(  );
      send_to_char( "Done.  (extra removers set)\n\r", ch );
      return;
   }

   if( !str_cmp( arg2, "extra_readers" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "No names specified.\n\r", ch );
         return;
      }
      if( !str_cmp( argument, "none" ) )
         buf[0] = '\0';
      else
         sprintf( buf, "%s %s", board->extra_readers, argument );
      DISPOSE( board->extra_readers );
      board->extra_readers = str_dup( buf );
      write_boards_txt(  );
      send_to_char( "Done.  (extra readers set)\n\r", ch );
      return;
   }

   if( !str_cmp( arg2, "filename" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "No filename specified.\n\r", ch );
         return;
      }
      DISPOSE( board->note_file );
      board->note_file = str_dup( argument );
      write_boards_txt(  );
      send_to_char( "Done.  (board's filename set)\n\r", ch );
      return;
   }

   if( !str_cmp( arg2, "post" ) )
   {
      if( value < 0 || value > MAX_LEVEL )
      {
         send_to_char( "Value outside valid character level range.\n\r", ch );
         return;
      }
      board->min_post_level = value;
      write_boards_txt(  );
      send_to_char( "Done.  (minimum posting level set)\n\r", ch );
      return;
   }

   if( !str_cmp( arg2, "remove" ) )
   {
      if( value < 0 || value > MAX_LEVEL )
      {
         send_to_char( "Value outside valid character level range.\n\r", ch );
         return;
      }
      board->min_remove_level = value;
      write_boards_txt(  );
      send_to_char( "Done.  (minimum remove level set)\n\r", ch );
      return;
   }

   if( !str_cmp( arg2, "maxpost" ) )
   {
      if( value < 1 || value > 999 )
      {
         send_to_char( "Value out of range.\n\r", ch );
         return;
      }
      board->max_posts = value;
      write_boards_txt(  );
      send_to_char( "Done.  (maximum number of posts set)\n\r", ch );
      return;
   }
   if( !str_cmp( arg2, "type" ) )
   {
      if( value < 0 || value > 1 )
      {
         send_to_char( "Value out of range.\n\r", ch );
         return;
      }
      board->type = value;
      write_boards_txt(  );
      send_to_char( "Done.  (board's type set)\n\r", ch );
      return;
   }

   do_bset( ch, "" );
   return;
}


void do_bstat( CHAR_DATA * ch, char *argument )
{
   BOARD_DATA *board;
   bool found;
   char arg[MAX_INPUT_LENGTH];

   argument = one_argument( argument, arg );
   found = FALSE;
   for( board = first_board; board; board = board->next )
      if( !str_cmp( arg, board->note_file ) )
      {
         found = TRUE;
         break;
      }

   if( !found )
   {
      if( argument && argument[0] != '\0' )
      {
         send_to_char_color( "&GBoard not found.  Usage: bstat <board filename>\n\r", ch );
         return;
      }
      else
      {
         board = find_board( ch );
         if( !board )
         {
            send_to_char_color( "&GNo board present.  Usage: bstat <board filename>\n\r", ch );
            return;
         }
      }
   }

   ch_printf_color( ch,
                    "\n\r&GFilename: &W%-15.15s &GOVnum: &W%-5d  &GRead: &W%-2d  &GPost: &W%-2d  &GRemove: &W%-2d\n\r&GMaxpost:  &W%-3d              &GType: &W%d\n\r&GPosts:    %d\n\r",
                    board->note_file, board->board_obj, board->min_read_level, board->min_post_level,
                    board->min_remove_level, board->max_posts, board->type, board->num_posts );

   ch_printf_color( ch,
                    "&GRead_group:     &W%s\n\r&GPost_group:     &W%s\n\r&GExtra_readers:  &W%s\n\r&GExtra_removers: &W%s\n\r",
                    board->read_group, board->post_group, board->extra_readers, board->extra_removers );
   ch_printf_color( ch, "&GPost Message:    %s\n\r", board->postmessg ? board->postmessg : "Default Message" );
   ch_printf_color( ch, "&GOPost Message:   %s\n\r", board->opostmessg ? board->opostmessg : "Default Message" );
   ch_printf_color( ch, "&GORead Message:   %s\n\r", board->oreadmessg ? board->oreadmessg : "Default Message" );
   ch_printf_color( ch, "&GORemove Message: %s\n\r", board->oremovemessg ? board->oremovemessg : "Default Message" );
   ch_printf_color( ch, "&GOTake Message:   %s\n\r", board->otakemessg ? board->otakemessg : "Default Message" );
   ch_printf_color( ch, "&GOList Message:   %s\n\r", board->olistmessg ? board->olistmessg : "Default Message" );
   ch_printf_color( ch, "&GOCopy Message:   %s\n\r", board->ocopymessg ? board->ocopymessg : "Default Message" );
   return;
}


void do_boards( CHAR_DATA * ch, char *argument )
{
   BOARD_DATA *board;

   if( !first_board )
   {
      send_to_char_color( "T&Ghere are no boards yet.\n\r", ch );
      return;
   }
   for( board = first_board; board; board = board->next )
      pager_printf_color( ch, "&G%-15.15s #: %5d Read: %2d Post: %2d Rmv: %2d Max: %3d Posts: &g%3d &GType: %d\n\r",
                          board->note_file, board->board_obj,
                          board->min_read_level, board->min_post_level,
                          board->min_remove_level, board->max_posts, board->num_posts, board->type );
}

void mail_count( CHAR_DATA * ch )
{
   BOARD_DATA *board;
   NOTE_DATA *note;
   int cnt_to = 0, cnt_from = 0;

   for( board = first_board; board; board = board->next )
      if( board->type == BOARD_MAIL && can_read( ch, board ) )
         for( note = board->first_note; note; note = note->next )
         {
            if( is_name( ch->name, note->to_list ) )
               ++cnt_to;
            else if( !str_cmp( ch->name, note->sender ) )
               ++cnt_from;
         }
   if( cnt_to )
      ch_printf( ch, "You have %d mail message%swaiting for you.\n\r", cnt_to, ( cnt_to > 1 ) ? "s " : " " );

   if( cnt_from )
      ch_printf( ch, "You have %d mail message%swritten by you.\n\r", cnt_from, ( cnt_from > 1 ) ? "s " : " " );
   return;
}
