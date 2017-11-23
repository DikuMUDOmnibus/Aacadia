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
/*						 Boards Header				                    */
/************************************************************************/

#define NOTE_DIR "../system/"

#define DEF_NORMAL  0
#define DEF_INCLUDE 1
#define DEF_EXCLUDE 2

#define MAX_BOARD 	  12

#define DEFAULT_BOARD 0

#define BA_ALL 6

#define MAX_LINE_LENGTH 80

#define MAX_NOTE_TEXT (4*MAX_STRING_LENGTH - 1000)

#define BOARD_NOTFOUND -1

struct global_board_data
{
   char *short_name;
   char *long_name;

   int read_level;
   int write_level;

   char *names;
   int force_type;

   int purge_days;


   NOTE_DATA *note_first;
   bool changed;

};

typedef struct global_board_data GLOBAL_BOARD_DATA;



extern GLOBAL_BOARD_DATA boards[MAX_BOARD];


void finish_note( GLOBAL_BOARD_DATA * board, NOTE_DATA * note );
void free_note( NOTE_DATA * note );
void load_global_boards( void );
int board_lookup( const char *name );
bool is_note_to( CHAR_DATA * ch, NOTE_DATA * note );
void personal_message( const char *sender, const char *to, const char *subject, const int expire_days, const char *text );
void make_note( const char *board_name, const char *sender, const char *to, const char *subject, const int expire_days,
                const char *text );
void save_notes(  );

void handle_con_note_to( DESCRIPTOR_DATA * d, char *argument );
void handle_con_note_subject( DESCRIPTOR_DATA * d, char *argument );
void handle_con_note_expire( DESCRIPTOR_DATA * d, char *argument );
void handle_con_note_text( DESCRIPTOR_DATA * d, char *argument );
void handle_con_note_finish( DESCRIPTOR_DATA * d, char *argument );



DECLARE_DO_FUN( do_global_note );
DECLARE_DO_FUN( do_global_boards );
