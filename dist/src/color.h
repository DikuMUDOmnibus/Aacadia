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
/*						 Color Header		*/
/************************************************************************/

#ifndef MSL
#define MSL MAX_STRING_LENGTH
#endif

#ifndef MIL
#define MIL MAX_INPUT_LENGTH
#endif

void set_char_color( sh_int AType, CHAR_DATA * ch );
void set_pager_color( sh_int AType, CHAR_DATA * ch );
char *color_str( sh_int AType, CHAR_DATA * ch );

#define ANSI_BLACK    	"\033[0;30m"
#define ANSI_DRED    	"\033[0;31m"
#define ANSI_DGREEN     "\033[0;32m"
#define ANSI_ORANGE    	"\033[0;33m"
#define ANSI_DBLUE    	"\033[0;34m"
#define ANSI_PURPLE    	"\033[0;35m"
#define ANSI_CYAN	"\033[0;36m"
#define ANSI_GREY	"\033[0;37m"
#define ANSI_DGREY	"\033[1;30m"
#define ANSI_RED	"\033[1;31m"
#define ANSI_GREEN	"\033[1;32m"
#define ANSI_YELLOW   	"\033[1;33m"
#define ANSI_BLUE	"\033[1;34m"
#define ANSI_PINK	"\033[1;35m"
#define ANSI_LBLUE   	"\033[1;36m"
#define ANSI_WHITE   	"\033[1;37m"
#define ANSI_RESET	"\033[0m"

#define BLINK_BLACK		"\033[0;5;30m"
#define BLINK_DRED		"\033[0;5;31m"
#define BLINK_DGREEN		"\033[0;5;32m"
#define BLINK_ORANGE		"\033[0;5;33m"
#define BLINK_DBLUE		"\033[0;5;34m"
#define BLINK_PURPLE		"\033[0;5;35m"
#define BLINK_CYAN		"\033[0;5;36m"
#define BLINK_GREY		"\033[0;5;37m"
#define BLINK_DGREY		"\033[1;5;30m"
#define BLINK_RED		"\033[1;5;31m"
#define BLINK_GREEN		"\033[1;5;32m"
#define BLINK_YELLOW		"\033[1;5;33m"
#define BLINK_BLUE		"\033[1;5;34m"
#define BLINK_PINK		"\033[1;5;35m"
#define BLINK_LBLUE		"\033[1;5;36m"
#define BLINK_WHITE		"\033[1;5;37m"

#define BACK_BLACK 	"\033[40m"
#define BACK_DRED  	"\033[41m"
#define BACK_DGREEN	"\033[42m"
#define BACK_ORANGE     "\033[43m"
#define BACK_DBLUE      "\033[44m"
#define BACK_PURPLE     "\033[45m"
#define BACK_CYAN       "\033[46m"
#define BACK_GREY       "\033[47m"

#define ANSI_UNDERLINE  "\033[4m"
#define ANSI_ITALIC	"\033[6m"
#define ANSI_REVERSE    "\033[7m"
