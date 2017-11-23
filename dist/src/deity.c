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
/*			Deity module				                    */
/************************************************************************/


#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "acadia.h"


DEITY_DATA *first_deity;
DEITY_DATA *last_deity;


void fread_deity args( ( DEITY_DATA * deity, FILE * fp ) );
bool load_deity_file args( ( char *deityfile ) );
void write_deity_list args( ( void ) );
int get_risflag args( ( char *flag ) );
int get_npc_race args( ( char *type ) );
int get_pc_race args( ( char *type ) );


DEITY_DATA *get_deity( char *name )
{
   DEITY_DATA *deity;
   for( deity = first_deity; deity; deity = deity->next )
      if( !str_cmp( name, deity->name ) )
         return deity;
   return NULL;
}

void write_deity_list(  )
{
   DEITY_DATA *tdeity;
   FILE *fpout;
   char filename[256];

   sprintf( filename, "%s%s", DEITY_DIR, DEITY_LIST );
   fclose( fpReserve );
   fpout = fopen( filename, "w" );
   if( !fpout )
      bug( "FATAL: cannot open deity.lst for writing!\n\r", 0 );
   else
   {
      for( tdeity = first_deity; tdeity; tdeity = tdeity->next )
         fprintf( fpout, "%s\n", tdeity->filename );
      fprintf( fpout, "$\n" );
      fclose( fpout );
   }
   fpReserve = fopen( NULL_FILE, "r" );
}


void save_deity( DEITY_DATA * deity )
{
   FILE *fp;
   char filename[256];
   char buf[MAX_STRING_LENGTH];

   if( !deity )
   {
      bug( "save_deity: null deity pointer!", 0 );
      return;
   }

   if( !deity->filename || deity->filename[0] == '\0' )
   {
      sprintf( buf, "save_deity: %s has no filename", deity->name );
      bug( buf, 0 );
      return;
   }

   sprintf( filename, "%s%s", DEITY_DIR, deity->filename );

   fclose( fpReserve );
   if( ( fp = fopen( filename, "w" ) ) == NULL )
   {
      bug( "save_deity: fopen", 0 );
      perror( filename );
   }
   else
   {
      fprintf( fp, "#DEITY\n" );
      fprintf( fp, "Filename		%s~\n", deity->filename );
      fprintf( fp, "Name		%s~\n", deity->name );
      fprintf( fp, "Description	%s~\n", deity->description );
      fprintf( fp, "Element		%d\n", deity->element );
      fprintf( fp, "Strpls		%d\n", deity->strpls );
      fprintf( fp, "Intpls		%d\n", deity->intpls );
      fprintf( fp, "Wispls		%d\n", deity->wispls );
      fprintf( fp, "Dexpls		%d\n", deity->dexpls );
      fprintf( fp, "Conpls		%d\n", deity->conpls );
      fprintf( fp, "Chapls		%d\n", deity->chapls );
      fprintf( fp, "Lckpls		%d\n", deity->lckpls );
      fprintf( fp, "Exppls		%d\n", deity->exppls );
      fprintf( fp, "Hitgain		%d\n", deity->hitgain );
      fprintf( fp, "Managain		%d\n", deity->managain );
      fprintf( fp, "Movegain		%d\n", deity->movegain );
      fprintf( fp, "Dampls		%d\n", deity->dampls );
      fprintf( fp, "Splcst		%d\n", deity->splcst );
      fprintf( fp, "Spldur		%d\n", deity->spldur );
      fprintf( fp, "End\n\n" );
      fprintf( fp, "#END\n" );
      fclose( fp );
   }
   fpReserve = fopen( NULL_FILE, "r" );
   return;
}


#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				      field = value;			\
				      fMatch = TRUE;			\
				      break;				\
				}

void fread_deity( DEITY_DATA * deity, FILE * fp )
{
   char buf[MAX_STRING_LENGTH];
   char *word;
   bool fMatch;

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
         case 'C':
            KEY( "Conpls", deity->conpls, fread_number( fp ) );
            KEY( "Chapls", deity->chapls, fread_number( fp ) );
            break;
         case 'D':
            KEY( "Dampls", deity->dampls, fread_number( fp ) );
            KEY( "Description", deity->description, fread_string( fp ) );
            KEY( "Dexpls", deity->dexpls, fread_number( fp ) );
            break;
         case 'E':
            if( !str_cmp( word, "End" ) )
            {
               if( !deity->name )
                  deity->name = STRALLOC( "" );
               if( !deity->description )
                  deity->description = STRALLOC( "" );
               return;
            }
            KEY( "Element", deity->element, fread_number( fp ) );
            KEY( "Exppls", deity->exppls, fread_number( fp ) );
            break;
         case 'F':
            KEY( "Filename", deity->filename, fread_string_nohash( fp ) );
            break;
         case 'H':
            KEY( "Hitgain", deity->hitgain, fread_number( fp ) );
            break;
         case 'I':
            KEY( "Intpls", deity->intpls, fread_number( fp ) );
            break;
         case 'L':
            KEY( "Lckpls", deity->lckpls, fread_number( fp ) );
            break;
         case 'M':
            KEY( "Managain", deity->managain, fread_number( fp ) );
            KEY( "Movegain", deity->movegain, fread_number( fp ) );
            break;
         case 'N':
            KEY( "Name", deity->name, fread_string( fp ) );
            break;
         case 'S':
            KEY( "Splcst", deity->splcst, fread_number( fp ) );
            KEY( "Spldur", deity->spldur, fread_number( fp ) );
            KEY( "Strpls", deity->strpls, fread_number( fp ) );
            break;
         case 'W':
            KEY( "Wispls", deity->wispls, fread_number( fp ) );
            break;
      }

      if( !fMatch )
      {
         sprintf( buf, "Fread_deity: no match: %s", word );
         bug( buf, 0 );
      }
   }
}


bool load_deity_file( char *deityfile )
{
   char filename[256];
   DEITY_DATA *deity;
   FILE *fp;
   bool found;

   found = FALSE;
   sprintf( filename, "%s%s", DEITY_DIR, deityfile );

   if( ( fp = fopen( filename, "r" ) ) != NULL )
   {
      for( ;; )
      {
         char letter;
         char *word;

         letter = fread_letter( fp );
         if( letter == '*' )
         {
            fread_to_eol( fp );
            continue;
         }

         if( letter != '#' )
         {
            bug( "Load_deity_file: # not found.", 0 );
            break;
         }

         word = fread_word( fp );
         if( !str_cmp( word, "DEITY" ) )
         {
            CREATE( deity, DEITY_DATA, 1 );
            fread_deity( deity, fp );
            LINK( deity, first_deity, last_deity, next, prev );
            found = TRUE;
            break;
         }
         else
         {
            char buf[MAX_STRING_LENGTH];
            sprintf( buf, "Load_deity_file: bad section: %s.", word );
            bug( buf, 0 );
            break;
         }
      }
      fclose( fp );
   }

   return found;
}


void load_deity(  )
{
   FILE *fpList;
   char *filename;
   char deitylist[256];
   char buf[MAX_STRING_LENGTH];

   first_deity = NULL;
   last_deity = NULL;

   log_string( "Loading deities..." );

   sprintf( deitylist, "%s%s", DEITY_DIR, DEITY_LIST );
   if( ( fpList = fopen( deitylist, "r" ) ) == NULL )
   {
      perror( deitylist );
      exit( 1 );
   }

   for( ;; )
   {
      filename = feof( fpList ) ? "$" : fread_word( fpList );
      log_string( filename );
      if( filename[0] == '$' )
         break;
      if( !load_deity_file( filename ) )
      {
         sprintf( buf, "Cannot load deity file: %s", filename );
         bug( buf, 0 );
      }
   }
   fclose( fpList );
   log_string( " Done deities " );
   return;
}

void do_setdeity( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   DEITY_DATA *deity;
   int value;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   switch ( ch->substate )
   {
      default:
         break;

      case SUB_RESTRICTED:
         send_to_char( "You cannot do this while in another command.\n\r", ch );
         return;

      case SUB_DEITYDESC:
         deity = ch->dest_buf;
         STRFREE( deity->description );
         deity->description = copy_buffer( ch );
         stop_editing( ch );
         save_deity( deity );
         ch->substate = ch->tempnum;
         return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' )
   {
      send_to_char( "Usage: setdeity <deity> <field> <toggle>\n\r", ch );
      send_to_char( "\n\rField being one of:\n\r", ch );
      send_to_char( "filename name description exppls dampls splcst spldur element\n\r", ch );
      send_to_char( "strpls intpls wispls dexpls conpls chapls lckpls hitgain movegain managain\n\r", ch );
      return;
   }

   deity = get_deity( arg1 );
   if( !deity )
   {
      send_to_char( "No such deity.\n\r", ch );
      return;
   }

   if( !str_cmp( arg2, "name" ) )
   {
      STRFREE( deity->name );
      deity->name = STRALLOC( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      return;
   }

   if( !str_cmp( arg2, "filename" ) )
   {
      DISPOSE( deity->filename );
      deity->filename = str_dup( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      write_deity_list(  );
      return;
   }

   if( !str_cmp( arg2, "description" ) )
   {
      if( ch->substate == SUB_REPEATCMD )
         ch->tempnum = SUB_REPEATCMD;
      else
         ch->tempnum = SUB_NONE;
      ch->substate = SUB_DEITYDESC;
      ch->dest_buf = deity;
      start_editing( ch, deity->description );
      return;
   }

   if( !str_cmp( arg2, "element" ) )
   {
      bool fMatch = FALSE;

      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         if( !str_cmp( arg3, "none" ) )
         {
            fMatch = TRUE;
            deity->element = 0;
         }
         else
         {
            value = get_risflag( arg3 );
            if( value < 0 || value > 31 )
               ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
            else
            {
               TOGGLE_BIT( deity->element, 1 << value );
               fMatch = TRUE;
            }
         }
      }

      if( fMatch )
         ch_printf( ch, "Done.\n\r" );
      save_deity( deity );
      return;
   }

   if( !str_cmp( arg2, "exppls" ) )
   {
      deity->exppls = atoi( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      return;
   }
   if( !str_cmp( arg2, "hitgain" ) )
   {
      deity->hitgain = atoi( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      return;
   }
   if( !str_cmp( arg2, "managain" ) )
   {
      deity->managain = atoi( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      return;
   }
   if( !str_cmp( arg2, "movegain" ) )
   {
      deity->movegain = atoi( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      return;
   }
   if( !str_cmp( arg2, "dampls" ) )
   {
      deity->dampls = atoi( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      return;
   }
   if( !str_cmp( arg2, "splcst" ) )
   {
      deity->splcst = atoi( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      return;
   }
   if( !str_cmp( arg2, "spldur" ) )
   {
      deity->spldur = atoi( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      return;
   }
   if( !str_cmp( arg2, "strpls" ) )
   {
      deity->strpls = atoi( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      return;
   }
   if( !str_cmp( arg2, "intpls" ) )
   {
      deity->intpls = atoi( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      return;
   }
   if( !str_cmp( arg2, "wispls" ) )
   {
      deity->wispls = atoi( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      return;
   }
   if( !str_cmp( arg2, "dexpls" ) )
   {
      deity->dexpls = atoi( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      return;
   }
   if( !str_cmp( arg2, "conpls" ) )
   {
      deity->conpls = atoi( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      return;
   }
   if( !str_cmp( arg2, "chapls" ) )
   {
      deity->chapls = atoi( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      return;
   }
   if( !str_cmp( arg2, "lckpls" ) )
   {
      deity->lckpls = atoi( argument );
      send_to_char( "Done.\n\r", ch );
      save_deity( deity );
      return;
   }

   do_setdeity( ch, "" );
   return;
}


void do_showdeity( CHAR_DATA * ch, char *argument )
{
   DEITY_DATA *deity;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      send_to_char( "Usage: showdeity <deity>\n\r", ch );
      return;
   }

   deity = get_deity( argument );
   if( !deity )
   {
      send_to_char( "No such deity.\n\r", ch );
      return;
   }
   ch_printf( ch, "Deity: %s\n\rFilename: %s\n\rDescription:\n\r%s\n\r", deity->name, deity->filename, deity->description );
   ch_printf( ch, "Exppls: %d%\t\tDampls: %d%\t\tSplcst: %d\t\tSpldur: %d\n\r", deity->exppls, deity->dampls, deity->splcst,
              deity->spldur );
   ch_printf( ch, "Hitgain: %d\t\tManagain: %d\t\tMovegain: %d\n\r", deity->hitgain, deity->managain, deity->movegain );
   ch_printf( ch, "Strpls: %d\t\tIntpls: %d\t\tWispls: %d\t\tDexpls: %d\n\r", deity->strpls, deity->intpls, deity->wispls,
              deity->dexpls );
   ch_printf( ch, "Conpls: %d\t\tChapls: %d\t\tLckpls: %d\n\r", deity->conpls, deity->chapls, deity->lckpls );
   ch_printf( ch, "Element: %s\n\r", flag_string( deity->element, ris_flags ) );

   return;
}

void do_makedeity( CHAR_DATA * ch, char *argument )
{
   char filename[256];
   DEITY_DATA *deity;
   bool found;

   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Usage: makedeity <deity name>\n\r", ch );
      return;
   }

   found = FALSE;
   sprintf( filename, "%s%s", DEITY_DIR, strlower( argument ) );
   CREATE( deity, DEITY_DATA, 1 );
   LINK( deity, first_deity, last_deity, next, prev );
   deity->name = STRALLOC( argument );
   deity->description = STRALLOC( "" );
   deity->filename = str_dup( filename );
   write_deity_list(  );
   save_deity( deity );

}

void do_devote( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   DEITY_DATA *deity;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   if( ch->level < 10 )
   {
      send_to_char( "You are not yet prepared for such devotion.\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax to devote: devote <deity>\n\r", ch );
      send_to_char( "    or\n\r", ch );
      send_to_char( "Syntax to undevote: devote none\n\r", ch );
      return;
   }

   if( !str_cmp( arg, "none" ) )
   {
      AFFECT_DATA af;
      if( !ch->pcdata->deity )
      {
         send_to_char( "You have already chosen to be an athiest.\n\r", ch );
         return;
      }
      if( ch->practice < 200 )
      {
         send_to_char( "You don't have have enough practices to become athiest again.\n\r", ch );
         send_to_char( "You need 200 practices to become an athiest.\n\r", ch );
         return;
      }
      send_to_char( "A terrible curse afflicts you as you forsake a deity!\n\r", ch );

      REMOVE_BIT( ch->resistant, ch->pcdata->deity->element );

      affect_strip( ch, gsn_blindness );
      af.type = gsn_blindness;
      af.location = APPLY_HITROLL;
      af.modifier = -4;
      af.duration = 50 * DUR_CONV;
      af.bitvector = meb( AFF_BLIND );
      affect_to_char( ch, &af );

      send_to_char( "You cease to worship any deity.\n\r", ch );
/*
		ch->perm_str = 13;
		ch->perm_dex = 13;
		ch->perm_wis = 13;
		ch->perm_int = 13;
		ch->perm_con = 13;
		ch->perm_cha = 13;
		ch->perm_lck = 13;

		switch ( class_table[ch->class]->attr_prime )
		{
		    case APPLY_STR: ch->perm_str = 16; break;
		    case APPLY_INT: ch->perm_int = 16; break;
		    case APPLY_WIS: ch->perm_wis = 16; break;
		    case APPLY_DEX: ch->perm_dex = 16; break;
		    case APPLY_CON: ch->perm_con = 16; break;
		    case APPLY_CHA: ch->perm_cha = 16; break;
		    case APPLY_LCK: ch->perm_lck = 16; break;
		}
     
		ch->perm_str	+= race_table[ch->race]->str_plus;
		ch->perm_int	+= race_table[ch->race]->int_plus;
		ch->perm_wis	+= race_table[ch->race]->wis_plus;
		ch->perm_dex	+= race_table[ch->race]->dex_plus;
		ch->perm_con	+= race_table[ch->race]->con_plus;
		ch->perm_cha	+= race_table[ch->race]->cha_plus; 
		ch->perm_lck	+= race_table[ch->race]->lck_plus;
*/
      ch->perm_str -= ch->pcdata->deity->strpls;
      ch->perm_int -= ch->pcdata->deity->intpls;
      ch->perm_wis -= ch->pcdata->deity->wispls;
      ch->perm_dex -= ch->pcdata->deity->dexpls;
      ch->perm_con -= ch->pcdata->deity->conpls;
      ch->perm_cha -= ch->pcdata->deity->chapls;
      ch->perm_lck -= ch->pcdata->deity->lckpls;

      ch->pcdata->deity = NULL;
      STRFREE( ch->pcdata->deity_name );
      ch->pcdata->deity_name = STRALLOC( "" );

      ch->practice -= 200;

      save_char_obj( ch );
      return;
   }

   deity = get_deity( arg );
   if( !deity )
   {
      send_to_char( "No such deity holds weight on this world.\n\r", ch );
      return;
   }

   if( ch->pcdata->deity )
   {
      send_to_char( "You are already devoted to a deity.\n\r", ch );
      return;
   }

   STRFREE( ch->pcdata->deity_name );
   ch->pcdata->deity_name = QUICKLINK( deity->name );
   ch->pcdata->deity = deity;
   SET_BIT( ch->resistant, ch->pcdata->deity->element );
   act( AT_MAGIC, "Body and soul, you devote yourself to $t!", ch, ch->pcdata->deity_name, NULL, TO_CHAR );
/*
    ch->perm_str = 13;
    ch->perm_dex = 13;
    ch->perm_wis = 13;
    ch->perm_int = 13;
    ch->perm_con = 13;
    ch->perm_cha = 13;
    ch->perm_lck = 13;

    switch ( class_table[ch->class]->attr_prime )
    {
	    case APPLY_STR: ch->perm_str = 16; break;
	    case APPLY_INT: ch->perm_int = 16; break;
	    case APPLY_WIS: ch->perm_wis = 16; break;
	    case APPLY_DEX: ch->perm_dex = 16; break;
	    case APPLY_CON: ch->perm_con = 16; break;
	    case APPLY_CHA: ch->perm_cha = 16; break;
	    case APPLY_LCK: ch->perm_lck = 16; break;
    }
     
    ch->perm_str	+= race_table[ch->race]->str_plus;
    ch->perm_int	+= race_table[ch->race]->int_plus;
    ch->perm_wis	+= race_table[ch->race]->wis_plus;
    ch->perm_dex	+= race_table[ch->race]->dex_plus;
    ch->perm_con	+= race_table[ch->race]->con_plus;
    ch->perm_cha	+= race_table[ch->race]->cha_plus; 
    ch->perm_lck	+= race_table[ch->race]->lck_plus;
*/
   ch->perm_str += ch->pcdata->deity->strpls;
   ch->perm_int += ch->pcdata->deity->intpls;
   ch->perm_wis += ch->pcdata->deity->wispls;
   ch->perm_dex += ch->pcdata->deity->dexpls;
   ch->perm_con += ch->pcdata->deity->conpls;
   ch->perm_cha += ch->pcdata->deity->chapls;
   ch->perm_lck += ch->pcdata->deity->lckpls;

// ch->practice += 100;

   save_char_obj( ch );
   return;
}


void do_deities( CHAR_DATA * ch, char *argument )
{
   DEITY_DATA *deity;
   int count = 0;

   if( argument[0] == '\0' )
   {
      send_to_pager_color( "&gFor detailed information on a deity, try 'deities <deity>' or 'help deities'\n\r", ch );
      send_to_pager_color( "Deity\n\r", ch );
      for( deity = first_deity; deity; deity = deity->next )
      {
         pager_printf_color( ch, "&G%-14s\n\r", deity->name );
         count++;
      }
      if( !count )
      {
         send_to_pager_color( "&gThere are no deities on this world.\n\r", ch );
         return;
      }
      else
      {
         pager_printf_color( ch, "&RThere are &Y%d&R deities on this realm.&D\n\r", count );
         return;
      }
   }

   deity = get_deity( argument );
   if( !deity )
   {
      send_to_pager_color( "&gThat deity does not exist.\n\r", ch );
      return;
   }

   pager_printf_color( ch, "&gDeity:        &G%s\n\r", deity->name );
   pager_printf_color( ch, "&gDescription:\n\r&G%s", deity->description );


   return;
}
