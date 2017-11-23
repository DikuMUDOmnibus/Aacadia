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
/*							Board 1 module			                    */
/************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "acadia.h"

#define L_SUP (MAX_LEVEL - 1)
#define CDR LEVEL_SUBCODER

NOTE_DATA *location;


GLOBAL_BOARD_DATA boards[MAX_BOARD] = {
   {"General", "General discussion", 0, 2, "all", DEF_INCLUDE, 21, NULL, FALSE}
   ,
   {"Ideas", "Ideas for the mud", 0, 2, "all", DEF_NORMAL, 60, NULL, FALSE}
   ,
   {"Quest", "Quest information", 0, 2, "all", DEF_NORMAL, 60, NULL, FALSE}
   ,
   {"Announce", "Announcements from Immortals", 0, IL, "all", DEF_NORMAL, 60, NULL, FALSE}
   ,
   {"Bugs", "Typos, bugs, errors", 0, 1, "imm", DEF_NORMAL, 60, NULL, FALSE}
   ,
   {"Creative", "Creative writings", 0, 1, "all", DEF_INCLUDE, 60, NULL, FALSE}
   ,
   {"Personal", "Personal messages", 0, 1, "all", DEF_EXCLUDE, 28, NULL, FALSE}
   ,
   {"Immortal", "Immortal Discussions", IL, IL, "all", DEF_NORMAL, 60, NULL, FALSE}
   ,
   {"Builder", "Builder Discussions", BL, BL, "all", DEF_NORMAL, 60, NULL, FALSE}
   ,
   {"HBuilder", "Head Builder Discussions", HBL, HBL, "all", DEF_NORMAL, 60, NULL, FALSE}
   ,
   {"Coder", "Coder Discussions", CDR, CDR, "all", DEF_NORMAL, 60, NULL, FALSE}
   ,
   {"Admin", "Admin Discussions", ADM, ADM, "all", DEF_NORMAL, 60, NULL, FALSE}
};

const char *szFinishPrompt = "&w(&YC&w)ontinue, (&YV&w)iew, (&YP&w)ost or (&YF&w)orget it?";

long last_note_stamp = 0;

int note_c_all;

#define BOARD_NOACCESS -1
#define BOARD_NOTFOUND -1

bool next_board( CHAR_DATA * ch );


void free_global_note( NOTE_DATA * note )
{
   if( note->sender )
      STRFREE( note->sender );
   if( note->to_list )
      STRFREE( note->to_list );
   if( note->subject )
      STRFREE( note->subject );
   if( note->date )
      STRFREE( note->date );
   if( note->text )
      STRFREE( note->text );

   note->next = note_free;
   note_free = note;
}

NOTE_DATA *new_note(  )
{
   NOTE_DATA *note;

   if( note_free )
   {
      note = note_free;
      note_free = note_free->next;
   }
   else
      CREATE( note, NOTE_DATA, 1 );

   note->next = NULL;
   note->sender = NULL;
   note->expire = 0;
   note->to_list = NULL;
   note->subject = NULL;
   note->date = NULL;
   note->date_stamp = 0;
   note->text = NULL;

   return note;
}

void append_note( FILE * fp, NOTE_DATA * note )
{
   fprintf( fp, "Sender  %s~\n", note->sender );
   fprintf( fp, "Date    %s~\n", note->date );
   fprintf( fp, "Stamp   %ld\n", note->date_stamp );
   fprintf( fp, "Expire  %ld\n", note->expire );
   fprintf( fp, "To      %s~\n", note->to_list );
   fprintf( fp, "Subject %s~\n", note->subject );
   fprintf( fp, "Text\n%s~\n\n", note->text );
}

void finish_note( GLOBAL_BOARD_DATA * board, NOTE_DATA * note )
{
   FILE *fp;
   NOTE_DATA *p;
   char filename[200];


   if( last_note_stamp >= current_time )
      note->date_stamp = ++last_note_stamp;
   else
   {
      note->date_stamp = current_time;
      last_note_stamp = current_time;
   }

   if( board->note_first )
   {
      for( p = board->note_first; p->next; p = p->next )
         ;

      p->next = note;
   }
   else
      board->note_first = note;


   sprintf( filename, "%s%s", NOTE_DIR, board->short_name );

   fp = fopen( filename, "a" );
   if( !fp )
   {
      bug( "Could not open one of the note files in append mode", 0 );
      board->changed = TRUE;
      return;
   }

   append_note( fp, note );
   fclose( fp );
}

int board_number( const GLOBAL_BOARD_DATA * board )
{
   int i;

   for( i = 0; i < MAX_BOARD; i++ )
      if( board == &boards[i] )
         return i;

   return -1;
}

int board_lookup( const char *name )
{
   int i;

   for( i = 0; i < MAX_BOARD; i++ )
      if( !str_cmp( boards[i].short_name, name ) )
         return i;

   return -1;
}

void unlink_note( GLOBAL_BOARD_DATA * board, NOTE_DATA * note )
{
   NOTE_DATA *p;

   if( board->note_first == note )
      board->note_first = note->next;
   else
   {
      for( p = board->note_first; p && p->next != note; p = p->next );
      if( !p )
         bug( "unlink_note: could not find note.", 0 );
      else
         p->next = note->next;
   }
}

NOTE_DATA *find_note( CHAR_DATA * ch, GLOBAL_BOARD_DATA * board, int num )
{
   int count = 0;
   NOTE_DATA *p;

   for( p = board->note_first; p; p = p->next )
      if( ++count == num )
         break;

   if( ( count == num ) && is_note_to( ch, p ) )
      return p;
   else
      return NULL;

}

void save_board( GLOBAL_BOARD_DATA * board )
{
   FILE *fp;
   char filename[200];
   char buf[200];
   NOTE_DATA *note;

   sprintf( filename, "%s%s", NOTE_DIR, board->short_name );

   fp = fopen( filename, "w" );
   if( !fp )
   {
      sprintf( buf, "Error writing to: %s", filename );
      bug( buf, 0 );
   }
   else
   {
      for( note = board->note_first; note; note = note->next )
         append_note( fp, note );

      fclose( fp );
   }
}

void show_note_to_char( CHAR_DATA * ch, NOTE_DATA * note, int num )
{
   char buf[4 * MAX_STRING_LENGTH];

   sprintf( buf,
            "[&W%4d&w] &W&Y%s&w: &g%s&w\n\r"
            "&W&YDate&w:  %s\n\r"
            "&W&YTo&w:    %s\n\r"
            "&g===========================================================================&w\n\r"
            "%s\n\r", num, note->sender, note->subject, note->date, note->to_list, note->text );

   send_to_char_color( buf, ch );
}

void save_notes(  )
{
   int i;

   for( i = 0; i < MAX_BOARD; i++ )
      if( boards[i].changed )
         save_board( &boards[i] );
}

void load_board( GLOBAL_BOARD_DATA * board )
{
   FILE *fp, *fp_archive;
   NOTE_DATA *last_note;
   char filename[200];

   sprintf( filename, "%s%s", NOTE_DIR, board->short_name );

   fp = fopen( filename, "r" );

   if( !fp )
      return;


   last_note = NULL;

   for( ;; )
   {
      NOTE_DATA *pnote;
      char letter;

      do
      {
         letter = getc( fp );
         if( feof( fp ) )
         {
            fclose( fp );
            return;
         }
      }
      while( isspace( letter ) );
      ungetc( letter, fp );

      CREATE( pnote, NOTE_DATA, sizeof( *pnote ) );

      if( str_cmp( fread_word( fp ), "sender" ) )
         break;
      pnote->sender = fread_string( fp );

      if( str_cmp( fread_word( fp ), "date" ) )
         break;
      pnote->date = fread_string( fp );

      if( str_cmp( fread_word( fp ), "stamp" ) )
         break;
      pnote->date_stamp = fread_number( fp );

      if( str_cmp( fread_word( fp ), "expire" ) )
         break;
      pnote->expire = fread_number( fp );

      if( str_cmp( fread_word( fp ), "to" ) )
         break;
      pnote->to_list = fread_string( fp );

      if( str_cmp( fread_word( fp ), "subject" ) )
         break;
      pnote->subject = fread_string( fp );

      if( str_cmp( fread_word( fp ), "text" ) )
         break;
      pnote->text = fread_string( fp );

      pnote->next = NULL;


      if( pnote->expire < current_time )
      {
         char archive_name[200];

         sprintf( archive_name, "%s%s.old", NOTE_DIR, board->short_name );
         fp_archive = fopen( archive_name, "a" );
         if( !fp_archive )
            bug( "Could not open archive boards for writing", 0 );
         else
         {
            append_note( fp_archive, pnote );
            fclose( fp_archive );
         }

         free_global_note( pnote );
         board->changed = TRUE;
         continue;

      }


      if( board->note_first == NULL )
         board->note_first = pnote;
      else
         last_note->next = pnote;

      last_note = pnote;
   }

   bug( "Load_notes: bad key word.", 0 );
   return;
}

void load_global_boards(  )
{
   int i;

   for( i = 0; i < MAX_BOARD; i++ )
      load_board( &boards[i] );
}

bool is_note_to( CHAR_DATA * ch, NOTE_DATA * note )
{
   if( !str_cmp( ch->name, note->sender ) )
      return TRUE;

   if( is_full_name( "all", note->to_list ) )
      return TRUE;

   if( IS_IMMORTAL( ch ) && ( is_full_name( "imm", note->to_list ) ||
                              is_full_name( "imms", note->to_list ) ||
                              is_full_name( "immortal", note->to_list ) ||
                              is_full_name( "god", note->to_list ) ||
                              is_full_name( "gods", note->to_list ) || is_full_name( "immortals", note->to_list ) ) )
      return TRUE;

   if( ( get_trust( ch ) == MAX_LEVEL ) && ( is_full_name( "imp", note->to_list ) ||
                                             is_full_name( "imps", note->to_list ) ||
                                             is_full_name( "implementor", note->to_list ) ||
                                             is_full_name( "implementors", note->to_list ) ) )
      return TRUE;

   if( is_full_name( ch->name, note->to_list ) )
      return TRUE;

   if( is_number( note->to_list ) && get_trust( ch ) >= atoi( note->to_list ) )
      return TRUE;

   return FALSE;
}

int unread_notes_check( CHAR_DATA * ch, GLOBAL_BOARD_DATA * board )
{
   NOTE_DATA *note;
   time_t last_read;
   int count = 0;

   if( board->read_level > get_trust( ch ) )
      return 0;

   last_read = ch->pcdata->last_note[board_number( board )];

   for( note = board->note_first; note; note = note->next )
      if( is_note_to( ch, note ) && ( ( long )last_read < ( long )note->date_stamp ) )
         count++;

   return count;
}

int unread_notes( CHAR_DATA * ch, GLOBAL_BOARD_DATA * board )
{
   NOTE_DATA *note;
   time_t last_read;
   int count = 0;

   if( board->read_level > get_trust( ch ) )
      return BOARD_NOACCESS;

   last_read = ch->pcdata->last_note[board_number( board )];

   for( note = board->note_first; note; note = note->next )
      if( is_note_to( ch, note ) && ( ( long )last_read < ( long )note->date_stamp ) )
         count++;

   return count;
}

int total_notes( CHAR_DATA * ch, GLOBAL_BOARD_DATA * board )
{
   NOTE_DATA *note;
   int count = 0;

   if( board->read_level > get_trust( ch ) )
      return BOARD_NOACCESS;

   for( note = board->note_first; note; note = note->next )
      if( is_note_to( ch, note ) )
         count++;

   return count;
}

void do_nwrite( CHAR_DATA * ch, char *argument )
{
   char *strtime;
   char buf[200];

   if( IS_NPC( ch ) )
      return;

   if( get_trust( ch ) < ch->pcdata->board->write_level )
   {
      send_to_char_color( "You cannot post notes on this board.\n\r", ch );
      return;
   }

   if( ch->hit < 1 )
   {
      send_to_char( "You cannot write notes while incapacitated.\n\r", ch );
      return;
   }

   if( ch->pcdata->in_progress )
   {
      free_global_note( ch->pcdata->in_progress );
      ch->pcdata->in_progress = NULL;
   }


   if( !ch->pcdata->in_progress )
   {
      ch->pcdata->in_progress = new_note(  );
      ch->pcdata->in_progress->sender = STRALLOC( ch->name );

      strtime = ctime( &current_time );
      strtime[strlen( strtime ) - 1] = '\0';

      ch->pcdata->in_progress->date = STRALLOC( strtime );
   }

   act( AT_GREEN, "$n starts writing a note.&w", ch, NULL, NULL, TO_ROOM );

   sprintf( buf, "You are now %s a new note on the &W%s&w board.\n\r"
            "If you are using tintin, type #verbose to turn off alias expansion!\n\r\n\r",
            ch->pcdata->in_progress->text ? "continuing" : "posting", ch->pcdata->board->short_name );
   send_to_char_color( buf, ch );

   sprintf( buf, "&YFrom&w:    %s\n\r\n\r", ch->name );
   send_to_char_color( buf, ch );

   switch ( ch->pcdata->board->force_type )
   {
      case DEF_NORMAL:
         sprintf( buf, "If you press Return, default recipient \"&W%s&w\" will be chosen.\n\r", ch->pcdata->board->names );
         break;
      case DEF_INCLUDE:
         sprintf( buf, "The recipient list MUST include \"&W%s&w\". If not, it will be added automatically.\n\r",
                  ch->pcdata->board->names );
         break;

      case DEF_EXCLUDE:
         sprintf( buf, "The recipient of this note must NOT include: \"&W%s&w\".", ch->pcdata->board->names );

         break;
   }

   send_to_char_color( buf, ch );
   send_to_char_color( "\n\r&YTo&w:      ", ch );

   ch->desc->connected = CON_NOTE_TO;

}


void do_nread( CHAR_DATA * ch, char *argument )
{
   NOTE_DATA *p;
   int count = 0, number;
   time_t *last_note = &ch->pcdata->last_note[board_number( ch->pcdata->board )];

   if( !str_cmp( argument, "again" ) )
   {

   }
   else if( is_number( argument ) )
   {
      number = atoi( argument );

      for( p = ch->pcdata->board->note_first; p; p = p->next )
         if( ++count == number )
            break;

      if( !p || !is_note_to( ch, p ) )
         send_to_char_color( "No such note.\n\r", ch );
      else
      {
         show_note_to_char( ch, p, count );
         *last_note = UMAX( *last_note, p->date_stamp );
      }
   }
   else
   {
      char buf[200];

      count = 1;
      for( p = ch->pcdata->board->note_first; p; p = p->next, count++ )
         if( ( p->date_stamp > *last_note ) && is_note_to( ch, p ) )
         {
            show_note_to_char( ch, p, count );
            *last_note = UMAX( *last_note, p->date_stamp );
            return;
         }

      send_to_char_color( "No new notes in this board.\n\r", ch );

      if( next_board( ch ) )
         sprintf( buf, "Changed to next board, %s.\n\r", ch->pcdata->board->short_name );
      else
         sprintf( buf, "There are no more boards.\n\r" );

      send_to_char_color( buf, ch );
   }
}

void do_nremove( CHAR_DATA * ch, char *argument )
{
   NOTE_DATA *p;

   if( !is_number( argument ) )
   {
      send_to_char_color( "Remove which note?\n\r", ch );
      return;
   }

   p = find_note( ch, ch->pcdata->board, atoi( argument ) );
   if( !p )
   {
      send_to_char_color( "No such note.\n\r", ch );
      return;
   }

   if( str_cmp( ch->name, p->sender ) && ( get_trust( ch ) < MAX_LEVEL ) )
   {
      send_to_char_color( "You are not authorized to remove this note.\n\r", ch );
      return;
   }

   unlink_note( ch->pcdata->board, p );
   free_global_note( p );
   send_to_char_color( "Note removed!\n\r", ch );

   save_board( ch->pcdata->board );
}

/*
void do_nlist (CHAR_DATA *ch, char *argument)
{
    int count= 0, show = 0, num = 0, has_shown = 0;
    time_t last_note;
    NOTE_DATA *p;
    char buf[MAX_STRING_LENGTH];


    if (is_number(argument))	 
    {
        show = atoi(argument);

        for (p = ch->pcdata->board->note_first; p; p = p->next)
            if (is_note_to(ch,p))
                count++;
    }
	
    send_to_char_color ( "&WNotes on this board:&w\n\r"
                   "&BNum> Author        Subject&w\n\r",ch);

    last_note = ch->pcdata->last_note[board_number (ch->pcdata->board)];

    for (p = ch->pcdata->board->note_first; p; p = p->next)
    {
        num++;
        if (is_note_to(ch,p))
        {
            has_shown++; 
            if (!show || ((count-show) < has_shown))
            {
                sprintf (buf, "&W%3d&w> &B%c&w &Y%-13s&w &Y%s&w \n\r",
                         num,
                         last_note < p->date_stamp ? '*' : ' ',
                         p->sender, p->subject);
                send_to_char_color (buf,ch);
            }
        }

    }
}*/

void do_nlist( CHAR_DATA * ch, char *argument )
{
   int count = 0, show = 0, num = 0, has_shown = 0;
   time_t last_note;
   NOTE_DATA *p;
   char buf[MAX_STRING_LENGTH];


   if( is_number( argument ) )
   {
      show = atoi( argument );

      for( p = ch->pcdata->board->note_first; p; p = p->next )
         if( is_note_to( ch, p ) )
            count++;
   }

   send_to_char_color( "&WNotes on this board:&w\n\r"
                       "( &rNum&w )&r      Author &R:&r Subject&w\n\r"
                       "&z---------------------------------------------------------\n\r", ch );

   last_note = ch->pcdata->last_note[board_number( ch->pcdata->board )];

   for( p = ch->pcdata->board->note_first; p; p = p->next )
   {
      num++;
      if( is_note_to( ch, p ) )
      {
         has_shown++;
         if( !show || ( ( count - show ) < has_shown ) )
         {
            sprintf( buf, "( &W%2d&Y%c&w ) &w%11s&w &R:&w &W%s&w \n\r",
                     num, last_note < p->date_stamp ? 'N' : ' ', p->sender, p->subject );
            send_to_char_color( buf, ch );
         }
      }

   }
}

void do_ncatchup( CHAR_DATA * ch, char *argument )
{
   NOTE_DATA *p;

   for( p = ch->pcdata->board->note_first; p && p->next; p = p->next );

   if( !p )
      send_to_char_color( "Alas, there are no notes in that board.\n\r", ch );
   else
   {
      ch->pcdata->last_note[board_number( ch->pcdata->board )] = p->date_stamp;
      send_to_char_color( "All mesages skipped.\n\r", ch );
   }
}

void do_global_note( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];

   switch ( ch->substate )
   {
      default:
         break;
      case SUB_NS:
         if( arg[0] != '\0' && !strcmp( arg, "write" ) )
         {
            break;
         }
         location = ch->dest_buf;
         if( !location )
         {
            bug( "note: sub_ns: NULL ch->dest_buf", 0 );
            location = ch->pcdata->in_progress;
         }
         STRFREE( location->text );
         location->text = copy_buffer( ch );
         stop_editing( ch );
         ch->substate = ch->tempnum;
         finish_note( ch->pcdata->board, ch->pcdata->in_progress );
         send_to_char_color( "Note posted.\n\r", ch );
         ch->desc->connected = CON_PLAYING;
         ch->pcdata->in_progress = NULL;
         if( note_c_all == 1 )
         {
            sprintf( buf, "&R<&BNOTE INFO&R> &w%s has sent a note to all.&D", ch->name );
            talk_info( AT_PLAIN, buf );
         }
         act( AT_GREEN, "$n finishes $s note.", ch, NULL, NULL, TO_ROOM );
         return;
   }

   if( IS_NPC( ch ) )
      return;

   argument = one_argument( argument, arg );

   if( ( !arg[0] ) || ( !str_cmp( arg, "read" ) ) )
      do_nread( ch, argument );

   else if( !str_cmp( arg, "list" ) )
      do_nlist( ch, argument );

   else if( !str_cmp( arg, "write" ) )
   {
      do_channels( ch, "-all" );
      do_nwrite( ch, argument );
   }

   else if( !str_cmp( arg, "remove" ) )
      do_nremove( ch, argument );

   else if( !str_cmp( arg, "purge" ) )
      send_to_char_color( "Obsolete.\n\r", ch );

   else if( !str_cmp( arg, "archive" ) )
      send_to_char_color( "Obsolete.\n\r", ch );

   else if( !str_cmp( arg, "catchup" ) )
      do_ncatchup( ch, argument );
   else
      do_help( ch, "note" );
}

void do_global_boards( CHAR_DATA * ch, char *argument )
{
   int i, count, number;
   char buf[200];

   if( IS_NPC( ch ) )
      return;

   if( !argument[0] )
   {
      int unread;
      int total;

      count = 1;
      sprintf( buf, "&R%s Note System&w\n\r&z-=[&BNum        Name   Unread Total  Description&z]=-&w\n\r",
               sysdata.mud_name );
      send_to_char_color( buf, ch );
      for( i = 0; i < MAX_BOARD; i++ )
      {
         unread = unread_notes( ch, &boards[i] );
         total = total_notes( ch, &boards[i] );
         if( unread != BOARD_NOACCESS )
         {
            sprintf( buf, "&W   <&Y%2d&W>&G%12s&z  [%s%4d&z] [&R%4d&z] &r%s&w\n\r",
                     count, boards[i].short_name, unread ? "&G" : "&g", unread, total, boards[i].long_name );
            send_to_char_color( buf, ch );
            count++;
         }

      }

      if( ch->pcdata->board == NULL )
         ch->pcdata->board = &boards[DEFAULT_BOARD];
      sprintf( buf, "\n\rYou current board is &W%s&w.\n\r", ch->pcdata->board->short_name );
      send_to_char_color( buf, ch );

      if( ch->pcdata->board->read_level > get_trust( ch ) )
         send_to_char_color( "You cannot read nor write notes on this board.\n\r", ch );
      else if( ch->pcdata->board->write_level > get_trust( ch ) )
         send_to_char_color( "You can only read notes from this board.\n\r", ch );
      else
         send_to_char_color( "You can both read and write on this board.\n\r", ch );

      return;
   }

   if( ch->pcdata->in_progress )
   {
      send_to_char_color( "Please finish your interrupted note first.\n\r", ch );
      return;
   }

   if( is_number( argument ) )
   {
      count = 0;
      number = atoi( argument );
      for( i = 0; i < MAX_BOARD; i++ )
         if( unread_notes( ch, &boards[i] ) != BOARD_NOACCESS )
            if( ++count == number )
               break;

      if( count == number )
      {
         ch->pcdata->board = &boards[i];
         sprintf( buf, "Current board changed to &W%s&w. %s.\n\r", boards[i].short_name,
                  ( get_trust( ch ) < boards[i].write_level )
                  ? "You can only read here" : "You can both read and write here" );
         send_to_char_color( buf, ch );
      }
      else
         send_to_char_color( "No such board.\n\r", ch );

      return;
   }


   for( i = 0; i < MAX_BOARD; i++ )
      if( !str_cmp( boards[i].short_name, argument ) )
         break;

   if( i == MAX_BOARD )
   {
      send_to_char_color( "No such board.\n\r", ch );
      return;
   }

   if( unread_notes( ch, &boards[i] ) == BOARD_NOACCESS )
   {
      send_to_char_color( "No such board.\n\r", ch );
      return;
   }

   ch->pcdata->board = &boards[i];
   sprintf( buf, "Current board changed to &W%s&w. %s.\n\r", boards[i].short_name,
            ( get_trust( ch ) < boards[i].write_level ) ? "You can only read here" : "You can both read and write here" );
   send_to_char_color( buf, ch );
}

void personal_message( const char *sender, const char *to, const char *subject, const int expire_days, const char *text )
{
   make_note( "Personal", sender, to, subject, expire_days, text );
}

void make_note( const char *board_name, const char *sender, const char *to, const char *subject, const int expire_days,
                const char *text )
{
   int board_index = board_lookup( board_name );
   GLOBAL_BOARD_DATA *board;
   NOTE_DATA *note;
   char *strtime;

   if( board_index == BOARD_NOTFOUND )
   {
      bug( "make_note: board not found", 0 );
      return;
   }

   if( strlen( text ) > MAX_NOTE_TEXT )
   {
      bug( "make_note: text too long (%d bytes)", strlen( text ) );
      return;
   }


   board = &boards[board_index];

   note = new_note(  );

   note->sender = STRALLOC( ( char * )sender );
   note->to_list = STRALLOC( ( char * )to );
   note->subject = STRALLOC( ( char * )subject );
   note->expire = current_time + expire_days * 60 * 60 * 24;
   note->text = STRALLOC( ( char * )text );

   strtime = ctime( &current_time );
   strtime[strlen( strtime ) - 1] = '\0';

   note->date = STRALLOC( strtime );

   finish_note( board, note );

}

bool next_board( CHAR_DATA * ch )
{
   int i = board_number( ch->pcdata->board ) + 1;

   while( ( i < MAX_BOARD ) && ( unread_notes( ch, &boards[i] ) == BOARD_NOACCESS ) )
      i++;

   if( i == MAX_BOARD )
   {
      ch->pcdata->board = &boards[0];
      return TRUE;
   }
   else
   {
      ch->pcdata->board = &boards[i];
      return TRUE;
   }
}

void handle_con_note_to( DESCRIPTOR_DATA * d, char *argument )
{
   char buf[MAX_INPUT_LENGTH];
   CHAR_DATA *ch = d->character;

   if( !ch->pcdata->in_progress )
   {
      d->connected = CON_PLAYING;
      bug( "nanny: In CON_NOTE_TO, but no note in progress", 0 );
      return;
   }

   strcpy( buf, argument );
   smash_tilde( buf );

   switch ( ch->pcdata->board->force_type )
   {
      case DEF_NORMAL:
         if( !buf[0] )
         {
            ch->pcdata->in_progress->to_list = STRALLOC( ch->pcdata->board->names );
            sprintf( buf, "Assumed default recipient: &W%s&w\n\r", ch->pcdata->board->names );
            send_to_char_color( buf, ch );
         }
         else
            ch->pcdata->in_progress->to_list = STRALLOC( buf );

         break;

      case DEF_INCLUDE:
         if( !is_full_name( ch->pcdata->board->names, buf ) )
         {
            strcat( buf, " " );
            strcat( buf, ch->pcdata->board->names );
            ch->pcdata->in_progress->to_list = STRALLOC( buf );

            sprintf( buf, "\n\rYou did not specify %s as recipient, so it was automatically added.\n\r"
                     "&YNew To&w :  %s&w\n\r", ch->pcdata->board->names, ch->pcdata->in_progress->to_list );
            send_to_char_color( buf, ch );
         }
         else
            ch->pcdata->in_progress->to_list = STRALLOC( buf );
         break;

      case DEF_EXCLUDE:
         if( !buf[0] )
         {
            send_to_char_color( "You must specify a recipient.\n\r" "&YTo&w:      ", ch );
            return;
         }

         if( is_full_name( ch->pcdata->board->names, buf ) )
         {
            sprintf( buf, "You are not allowed to send notes to %s on this board. Try again.\n\r"
                     "&YTo&w:      ", ch->pcdata->board->names );
            send_to_char_color( buf, ch );
            return;
         }
         else
            ch->pcdata->in_progress->to_list = STRALLOC( buf );
         break;

   }

   if( is_full_name( "all", ch->pcdata->in_progress->to_list ) )
   {
      note_c_all = 1;
   }
   else
   {
      note_c_all = 0;
   }

   send_to_char_color( "&Y\n\rSubject&w: ", ch );
   d->connected = CON_NOTE_SUBJECT;
}

void handle_con_note_subject( DESCRIPTOR_DATA * d, char *argument )
{
   char buf[MAX_INPUT_LENGTH];
   CHAR_DATA *ch = d->character;

   if( !ch->pcdata->in_progress )
   {
      d->connected = CON_PLAYING;
      bug( "nanny: In CON_NOTE_SUBJECT, but no note in progress", 0 );
      return;
   }

   strcpy( buf, argument );
   smash_tilde( buf );

   if( !buf[0] )
   {
      send_to_char_color( "Please find a meaningful subject!\n\r", ch );
      send_to_char_color( "&YSubject&w: ", ch );
   }
   else if( strlen( buf ) > 60 )
   {
      send_to_char_color( "No, no. This is just the Subject. You're note writing the note yet. Twit.\n\r", ch );
   }
   else
   {
      ch->pcdata->in_progress->subject = STRALLOC( buf );
      if( IS_IMMORTAL( ch ) )
      {
         sprintf( buf, "\n\rHow many days do you want this note to expire in?\n\r"
                  "Press Enter for default value for this board, &W%d&w days.\n\r"
                  "&YExpire&w:  ", ch->pcdata->board->purge_days );
         send_to_char_color( buf, ch );
         d->connected = CON_NOTE_EXPIRE;
      }
      else
      {
         ch->pcdata->in_progress->expire = current_time + ch->pcdata->board->purge_days * 24L * 3600L;
         sprintf( buf, "This note will expire %s\r", ctime( &ch->pcdata->in_progress->expire ) );
         send_to_char_color( buf, ch );
         send_to_char_color( "\n\r", ch );
         if( !ch->pcdata->in_progress->text )
         {
            ch->pcdata->in_progress->text = STRALLOC( "" );
         }
         if( ch->substate == SUB_REPEATCMD )
            ch->tempnum = SUB_REPEATCMD;
         else
            ch->tempnum = SUB_NONE;
         ch->substate = SUB_NS;
         ch->dest_buf = ch->pcdata->in_progress;
         location = ch->pcdata->in_progress;
         start_editing( ch, location->text );
      }
   }
}

void handle_con_note_expire( DESCRIPTOR_DATA * d, char *argument )
{
   CHAR_DATA *ch = d->character;
   char buf[MAX_STRING_LENGTH];
   time_t expire;
   int days;

   if( !ch->pcdata->in_progress )
   {
      d->connected = CON_PLAYING;
      bug( "nanny: In CON_NOTE_EXPIRE, but no note in progress", 0 );
      return;
   }

   strcpy( buf, argument );
   if( !buf[0] )
      days = ch->pcdata->board->purge_days;
   else if( !is_number( buf ) )
   {
      send_to_char_color( "Write the number of days!\n\r", ch );
      send_to_char_color( "&YExpire&w:  ", ch );
      return;
   }
   else
   {
      days = atoi( buf );
      if( days <= 0 )
      {
         send_to_char_color( "This is a positive MUD. Use positive numbers only! :)\n\r", ch );
         send_to_char_color( "&YExpire&w:  ", ch );
         return;
      }
   }

   expire = current_time + ( days * 24L * 3600L );

   ch->pcdata->in_progress->expire = expire;


   send_to_char_color( "\n\r", ch );

   if( !ch->pcdata->in_progress->text )
   {
      ch->pcdata->in_progress->text = STRALLOC( "" );
   }
   if( ch->substate == SUB_REPEATCMD )
      ch->tempnum = SUB_REPEATCMD;
   else
      ch->tempnum = SUB_NONE;
   ch->substate = SUB_NS;
   ch->dest_buf = ch->pcdata->in_progress;
   location = ch->pcdata->in_progress;
   start_editing( ch, location->text );
}

void handle_con_note_text( DESCRIPTOR_DATA * d, char *argument )
{
   CHAR_DATA *ch = d->character;
   char buf[MAX_STRING_LENGTH];
   char letter[4 * MAX_STRING_LENGTH];

   if( !ch->pcdata->in_progress )
   {
      d->connected = CON_PLAYING;
      bug( "nanny: In CON_NOTE_TEXT, but no note in progress", 0 );
      return;
   }


   strcpy( buf, argument );
   if( ( !str_cmp( buf, "~" ) ) || ( !str_cmp( buf, "END" ) ) )
   {
      send_to_char_color( "\n\r\n\r", ch );
      send_to_char_color( szFinishPrompt, ch );
      send_to_char_color( "\n\r", ch );
      d->connected = CON_NOTE_FINISH;
      return;
   }

   smash_tilde( buf );


   if( strlen( buf ) > MAX_LINE_LENGTH )
   {
      send_to_char_color( "Too long line rejected. Do NOT go over 80 characters!\n\r", ch );
      return;
   }

   if( ch->pcdata->in_progress->text )
   {
      strcpy( letter, ch->pcdata->in_progress->text );
      STRFREE( ch->pcdata->in_progress->text );
   }
   else
      strcpy( letter, "" );


   if( ( strlen( letter ) + strlen( buf ) ) > MAX_NOTE_TEXT )
   {
      send_to_char_color( "Note too long!\n\r", ch );
      free_global_note( ch->pcdata->in_progress );
      ch->pcdata->in_progress = NULL;
      d->connected = CON_PLAYING;
      return;
   }


   strcat( letter, buf );
   strcat( letter, "\r\n" );

   ch->pcdata->in_progress->text = STRALLOC( letter );
}

void handle_con_note_finish( DESCRIPTOR_DATA * d, char *argument )
{
   CHAR_DATA *ch = d->character;
   char buf[MAX_STRING_LENGTH];
   bool bx = FALSE;

   if( !ch->pcdata->in_progress )
   {
      d->connected = CON_PLAYING;
      bug( "nanny: In CON_NOTE_FINISH, but no note in progress", 0 );
      return;
   }

   switch ( tolower( argument[0] ) )
   {
      case 'c':
         send_to_char_color( "Continuing note...\n\r", ch );
         d->connected = CON_NOTE_TEXT;
         break;

      case 'v':
         if( ch->pcdata->in_progress->text )
         {
            send_to_char_color( "&gText of your note so far:&w\n\r", ch );
            send_to_char_color( ch->pcdata->in_progress->text, ch );
         }
         else
            send_to_char_color( "You haven't written a thing!\n\r\n\r", ch );
         send_to_char_color( szFinishPrompt, ch );
         send_to_char_color( "\n\r", ch );
         break;

      case 'p':
         if( board_number( ch->pcdata->board ) >= BA_ALL )
         {
            bx = TRUE;
         }
         finish_note( ch->pcdata->board, ch->pcdata->in_progress );
         send_to_char_color( "Note posted.\n\r", ch );
         d->connected = CON_PLAYING;
         ch->pcdata->in_progress = NULL;
         if( note_c_all == 1 && bx )
         {
            sprintf( buf, "&R<&BNOTE INFO&R> &w%s has sent a note to all.&D", ch->name );
            talk_info( AT_PLAIN, buf );
         }
         act( AT_GREEN, "$n finishes $s note.", ch, NULL, NULL, TO_ROOM );
         do_channels( ch, "+all" );
         break;

      case 'f':
         send_to_char_color( "Note cancelled!\n\r", ch );
         free_global_note( ch->pcdata->in_progress );
         ch->pcdata->in_progress = NULL;
         d->connected = CON_PLAYING;
         do_channels( ch, "+all" );
         break;

      default:
         send_to_char_color( "Huh? Valid answers are:\n\r\n\r", ch );
         send_to_char_color( szFinishPrompt, ch );
         send_to_char_color( "\n\r", ch );

   }
}
