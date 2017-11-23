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
/*                      Finger Header                                   */
/************************************************************************/

typedef struct wizinfo_data WIZINFO_DATA;
extern WIZINFO_DATA *first_wizinfo;
extern WIZINFO_DATA *last_wizinfo;

struct wizinfo_data
{
   WIZINFO_DATA *next;
   WIZINFO_DATA *prev;
   char *name;
   char *email;
   char *aol;
   char *yahoo;
   int icq;
   sh_int level;
};

DECLARE_DO_FUN( do_icq );
DECLARE_DO_FUN( do_email );
DECLARE_DO_FUN( do_aol );
DECLARE_DO_FUN( do_yahoo );
DECLARE_DO_FUN( do_finger );
DECLARE_DO_FUN( do_wizinfo );
DECLARE_DO_FUN( do_privacy );
