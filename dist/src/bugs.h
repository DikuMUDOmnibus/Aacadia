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
/*						Bug Report Header			                    */
/************************************************************************/

typedef struct bugs_data BUGS_DATA;

void load_bugs args( ( void ) );
void save_bugs args( ( void ) );
void delete_bug args( ( int num ) );

struct bugs_data
{

   char *bug;
   char *who;
   char *date;
   int vnum;
   time_t mudtime;
};

extern struct bugs_data *bugs_table;
