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
/*				     News Information header		                    */
/************************************************************************/

#define NEWS_VERSION		"6.00c"

#define NEWS_FILE		NEWS_DIR "news.dat"
#define HTML_NEWS_FILE		HTML_NEWS_DIR "news.html"
#define HTML_NEWS_IMAGES	"../images/"

#define MAX_NEWS_LENGTH		(MAX_STRING_LENGTH * 3)
#define MAX_HTML_LENGTH		(MAX_STRING_LENGTH * 6)

#define AT_BLADE		"&G&w"
#define HTML_BLADE		HTML_BLACK
#define AT_BASE			"&G&W"
#define HTML_BASE		HTML_BLACK
#define AT_HILT			"&G&c"
#define HTML_HILT		HTML_DCYAN
#define AT_BALL			"&G&W"
#define HTML_BALL		HTML_DBLUE
#define AT_TITLE		"&G&W"
#define HTML_TITLE		HTML_BLACK

#define AT_DATE			"&G&W"
#define HTML_DATE		HTML_BLUE
#define AT_SEPARATOR		"&G&c"
#define HTML_SEPARATOR		HTML_DCYAN
#define AT_NEWS			"&G&w"
#define HTML_NEWS		HTML_BLACK

typedef enum
{
   TYPE_ALL, TYPE_NORMAL, TYPE_LIST_FIRST, TYPE_LIST_LAST,
   TYPE_IMM_LIST,
   MAX_SHOW_TYPE
}
news_show_types;

#define HTML_RED	"#CC0000"
#define HTML_DRED	"#880000"

#define HTML_BLUE	"#0000CC"
#define HTML_DBLUE	"#000088"

#define HTML_GREEN	"#33FF33"
#define HTML_DGREEN	"#339933"

#define HTML_PURPLE	"#CC00FF"
#define HTML_DPURPLE	"#9900CC"

#define HTML_CYAN	"#00FFFF"
#define HTML_DCYAN	"#009999"

#define HTML_WHITE	"#FFFFFF"
#define HTML_GREY	"#CCCCCC"
#define HTML_DGREY	"#888888"
#define HTML_BLACK	"#000000"

#define HTML_COLOR_OPEN1	"<font color=\""
#define HTML_COLOR_OPEN2	"\">"
#define HTML_COLOR_CLOSE	"</font>"
