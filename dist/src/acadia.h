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
/*			  MUD Header				                    */
/************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define PORTN  4000

#define EXE_FILE       "../../dist/src/acadia"

#define Ke_CODEBASE_NAME "Acadia"
#define Ke_VERSION_MAJOR "1"
#define Ke_VERSION_MINOR "1"
#define Ke_BUILD 1

#define NOCRYPT

#include <unistd.h>
#include <re_comp.h>
#include "/usr/include/mysql/mysql.h"
#include <sys/time.h>
#include <dirent.h>

typedef int ch_ret;
typedef int obj_ret;

#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_SPEC_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	ch_ret fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#endif


#if	!defined(FALSE)
#define FALSE	 0
#endif

#if	!defined(TRUE)
#define TRUE	 1
#endif

#if	!defined(BERR)
#define BERR	 255
#endif

#if	defined(_AIX)
#if	!defined(const)
#define const
#endif
typedef int sh_int;
typedef int bool;
#define unix
#else
typedef short int sh_int;
typedef unsigned char bool;
#endif

typedef struct affect_data AFFECT_DATA;
typedef struct area_data AREA_DATA;
typedef struct auction_data AUCTION_DATA;
typedef struct watch_data WATCH_DATA;
typedef struct ban_data BAN_DATA;
typedef struct extracted_char_data EXTRACT_CHAR_DATA;
typedef struct char_data CHAR_DATA;
typedef struct hunt_hate_fear HHF_DATA;
typedef struct fighting_data FIGHT_DATA;
typedef struct descriptor_data DESCRIPTOR_DATA;
typedef struct exit_data EXIT_DATA;
typedef struct extra_descr_data EXTRA_DESCR_DATA;
typedef struct help_data HELP_DATA;
typedef struct mob_index_data MOB_INDEX_DATA;
typedef struct char_morph CHAR_MORPH;
typedef struct morph_data MORPH_DATA;
typedef struct nuisance_data NUISANCE_DATA;
typedef struct note_data NOTE_DATA;
typedef struct comment_data COMMENT_DATA;
typedef struct board_data BOARD_DATA;
typedef struct obj_data OBJ_DATA;
typedef struct obj_index_data OBJ_INDEX_DATA;
typedef struct pc_data PC_DATA;
typedef struct plane_data PLANE_DATA;
typedef struct reset_data RESET_DATA;
typedef struct map_index_data MAP_INDEX_DATA;
typedef struct map_data MAP_DATA;
typedef struct room_index_data ROOM_INDEX_DATA;
typedef struct shop_data SHOP_DATA;
typedef struct race_type RACE_TYPE;
typedef struct repairshop_data REPAIR_DATA;
typedef struct reserve_data RESERVE_DATA;
typedef struct time_info_data TIME_INFO_DATA;
typedef struct hour_min_sec HOUR_MIN_SEC;
typedef struct weather_data WEATHER_DATA;
typedef struct neighbor_data NEIGHBOR_DATA;
typedef struct clan_data CLAN_DATA;
typedef struct council_data COUNCIL_DATA;
typedef struct tourney_data TOURNEY_DATA;
typedef struct mob_prog_data MPROG_DATA;
typedef struct mob_prog_act_list MPROG_ACT_LIST;
typedef struct mpsleep_data MPSLEEP_DATA;
typedef struct editor_data EDITOR_DATA;
typedef struct teleport_data TELEPORT_DATA;
typedef struct timer_data TIMER;
typedef struct godlist_data GOD_DATA;
typedef struct system_data SYSTEM_DATA;
typedef struct acadia_affect ACADIA_AFF;
typedef struct who_data WHO_DATA;
typedef struct skill_type SKILLTYPE;
typedef struct social_type SOCIALTYPE;
typedef struct xsocial_type XSOCIALTYPE;
typedef struct cmd_type CMDTYPE;
typedef struct killed_data KILLED_DATA;
typedef struct deity_data DEITY_DATA;
typedef struct wizent WIZENT;
typedef struct ignore_data IGNORE_DATA;
typedef struct project_data PROJECT_DATA;
typedef struct extended_bitvector EXT_BV;
typedef struct lcnv_data LCNV_DATA;
typedef struct lang_data LANG_DATA;
typedef struct news_data NEWS_DATA;
typedef struct last_data LAST_DATA;
typedef struct web_descriptor WEB_DESCRIPTOR;


typedef void DO_FUN args( ( CHAR_DATA * ch, char *argument ) );
typedef bool SPEC_FUN args( ( CHAR_DATA * ch ) );
typedef ch_ret SPELL_FUN args( ( int sn, int level, CHAR_DATA * ch, void *vo ) );

#define MYSQL_HOST          "localhost"
#define MYSQL_DB            "<dbName>"
#define MYSQL_USERID        "<dbUser>"
#define MYSQL_PASSWD        "<dbPass>"

MYSQL mysql;

void init_mysql( const char *mysqldb );
void kill_mysql( void );
void mysql_keep_alive(  );

#define DUR_CONV	23.333333333333333333333333
#define HIDDEN_TILDE	'*'

#define BV00		(1 <<  0)
#define BV01		(1 <<  1)
#define BV02		(1 <<  2)
#define BV03		(1 <<  3)
#define BV04		(1 <<  4)
#define BV05		(1 <<  5)
#define BV06		(1 <<  6)
#define BV07		(1 <<  7)
#define BV08		(1 <<  8)
#define BV09		(1 <<  9)
#define BV10		(1 << 10)
#define BV11		(1 << 11)
#define BV12		(1 << 12)
#define BV13		(1 << 13)
#define BV14		(1 << 14)
#define BV15		(1 << 15)
#define BV16		(1 << 16)
#define BV17		(1 << 17)
#define BV18		(1 << 18)
#define BV19		(1 << 19)
#define BV20		(1 << 20)
#define BV21		(1 << 21)
#define BV22		(1 << 22)
#define BV23		(1 << 23)
#define BV24		(1 << 24)
#define BV25		(1 << 25)
#define BV26		(1 << 26)
#define BV27		(1 << 27)
#define BV28		(1 << 28)
#define BV29		(1 << 29)
#define BV30		(1 << 30)
#define BV31		(1 << 31)

#define MAX_KEY_HASH		 2048
#define MAX_STRING_LENGTH	 6666
#define MAX_INPUT_LENGTH	 1024
#define MAX_INBUF_SIZE		 1024

#define HASHSTR

#define	MAX_LAYERS		  1
#define MAX_NEST	       100

#define MAX_KILLTRACK		25

#define MAX_EXP_WORTH	       500000
#define MIN_EXP_WORTH		   20

#define MAX_REXITS		   20
#define MAX_SKILL		  500

#define SPELL_SILENT_MARKER   "silent"

#define MAX_CLASS           	   21
#define MAX_NPC_CLASS		   27
#define MAX_RACE                   42
#define MAX_NPC_RACE		   112

#define MAX_XSOC		   85

#define MAX_VNUMS               2000000000

#define MAX_GOLD               99999999

#define sdur 250


extern bool double_exp;
extern bool half_exp;
extern int global_exp;
extern int global2_exp;
extern sh_int display;

extern int MAX_PC_RACE;
extern int MAX_PC_CLASS;

#define MAX_LEVEL		   1000
#define MAX_SUB 		   999
#define MAX_CLAN		   50
#define MAX_DEITY		   50
#define MAX_CPD			    4
#define	MAX_HERB		   20
#define MAX_ALIAS			200
#define	MAX_DISEASE		   20
#define MAX_PERSONAL		    5
#define MAX_WHERE_NAME             32

#define LEVEL_ADMIN 			MAX_LEVEL
#define LEVEL_ADMINADVISOR		(MAX_LEVEL - 10)
#define LEVEL_SUBADMIN			(MAX_LEVEL - 20)
#define LEVEL_SUPERCODER		(MAX_LEVEL - 30)
#define LEVEL_CODER 			(MAX_LEVEL - 40)
#define LEVEL_SUBCODER			(MAX_LEVEL - 50)
#define LEVEL_HEADBUILD 		(MAX_LEVEL - 60)
#define LEVEL_ADVBUILD 			(MAX_LEVEL - 70)
#define LEVEL_BUILD				(MAX_LEVEL - 80)
#define LEVEL_HELPS				(MAX_LEVEL - 90)
#define LEVEL_GOD				(MAX_LEVEL - 100)
#define LEVEL_GODI				(MAX_LEVEL - 110)
#define LEVEL_GODII				(MAX_LEVEL - 120)
#define LEVEL_GODIII			(MAX_LEVEL - 130)
#define LEVEL_GODIV				(MAX_LEVEL - 140)
#define LEVEL_GODS				(MAX_LEVEL - 150)
#define LEVEL_DEITY 			(MAX_LEVEL - 160)
#define LEVEL_VISITOR			(MAX_LEVEL - 170)
#define LEVEL_ARCHANGEL			(MAX_LEVEL - 180)
#define LEVEL_ANGEL				(MAX_LEVEL - 190)
#define LEVEL_RETIRED			(MAX_LEVEL - 200)
#define LEVEL_IMMORTAL			(MAX_LEVEL - 200)

#define LEVEL_MAXGAIN			(MAX_LEVEL - 400)
#define LEVEL_ACOLYTE			(MAX_LEVEL - 600)
#define LEVEL_NEOPHYTE			(MAX_LEVEL - 700)
#define LEVEL_AVATAR			(MAX_LEVEL - 800)
#define LEVEL_HERO				(MAX_LEVEL - 900)

#define SUB_HERO			999
#define SUB_AVATAR			999
#define SUB_NEOPHYTE		999
#define SUB_ACOLYTE			999

#define IL					LEVEL_IMMORTAL
#define ADM					LEVEL_ADMINADVISOR
#define BL					LEVEL_BUILD
#define HBL					LEVEL_HEADBUILD
#define LEVEL_LOG		    LEVEL_HEADBUILD
#define LEVEL_HIGOD		    LEVEL_HEADBUILD

#include "color.h"
#include "finger.h"
#include "declaration.h"
#include "pfiles.h"
#include "imm_host.h"
#include "rent.h"

bool DONT_UPPER;

#define	SECONDS_PER_TICK			 18

#define PULSE_PER_SECOND			  2
#define PULSE_VIOLENCE				 (3 * PULSE_PER_SECOND)
#define PULSE_MOBILE				 (4 * PULSE_PER_SECOND)
#define PULSE_TICK		  (SECONDS_PER_TICK * PULSE_PER_SECOND)
#define PULSE_AREA				(60 * PULSE_PER_SECOND)
#define PULSE_AUCTION				 (9 * PULSE_PER_SECOND)
#define PULSE_ARENA                  (30 * PULSE_PER_SECOND)


int area_version;
#define HAS_SPELL_INDEX     -1
#define AREA_VERSION_WRITE 3
int top_mob_serial;

typedef enum
{
   LOG_NORMAL, LOG_ALWAYS, LOG_NEVER, LOG_BUILD, LOG_HIGH, LOG_COMM,
   LOG_WARN, LOG_ALL
}
log_types;

typedef enum
{
   rNONE, rCHAR_DIED, rVICT_DIED, rBOTH_DIED, rCHAR_QUIT, rVICT_QUIT,
   rBOTH_QUIT, rSPELL_FAILED, rOBJ_SCRAPPED, rOBJ_EATEN, rOBJ_EXPIRED,
   rOBJ_TIMER, rOBJ_SACCED, rOBJ_QUAFFED, rOBJ_USED, rOBJ_EXTRACTED,
   rOBJ_DRUNK, rCHAR_IMMUNE, rVICT_IMMUNE,
   rCHAR_AND_OBJ_EXTRACTED = 128,
   rERROR = 255
}
ret_types;

#define ECHOTAR_ALL	0
#define ECHOTAR_PC	1
#define ECHOTAR_IMM	2

#define WT_MORTAL	0
#define WT_DEADLY	1
#define WT_IMM		2
#define WT_GROUPED	3
#define WT_GROUPWHO	4
#define WT_ANGEL	5
#define WT_RETIRED	6
#define WT_BOUNTIED	7
#define WT_HERO		8
#define WT_AVATAR   9
#define WT_NEOPHYTE	10
#define WT_ACOLYTE 	11

#ifndef INTBITS
#define INTBITS	32
#endif
#define XBM		31
#define RSV		5
#define XBI		4
#define MAX_BITS	XBI * INTBITS

struct extended_bitvector
{
   int bits[XBI];
};



#define ONLY_PKILL  	1
#define ONLY_PEACEFULL  2

struct char_morph
{
   MORPH_DATA *morph;
   EXT_BV affected_by;
   EXT_BV no_affected_by;
   int no_immune;
   int no_resistant;
   int no_suscept;
   int immune;
   int resistant;
   int suscept;
   int timer;
   sh_int ac;
   sh_int blood;
   sh_int cha;
   sh_int con;
   int damroll;
   sh_int dex;
   sh_int dodge;
   sh_int hit;
   int hitroll;
   sh_int inte;
   sh_int lck;
   sh_int mana;
   sh_int move;
   sh_int parry;
   sh_int saving_breath;
   sh_int saving_para_petri;
   sh_int saving_poison_death;
   sh_int saving_spell_staff;
   sh_int saving_wand;
   sh_int str;
   sh_int tumble;
   sh_int wis;
};

struct morph_data
{
   MORPH_DATA *next;
   MORPH_DATA *prev;
   char *blood;
   char *damroll;
   char *deity;
   char *description;
   char *help;
   char *hit;
   char *hitroll;
   char *key_words;
   char *long_desc;
   char *mana;
   char *morph_other;
   char *morph_self;
   char *morph_start;
   char *morph_end;
   char *move;
   char *name;
   char *short_desc;
   char *no_skills;
   char *skills;
   char *unmorph_other;
   char *unmorph_self;
   char *isusing;
   char *mamfin;
   char *mamfout;
   EXT_BV affected_by;
   int class;
   int defpos;
   EXT_BV no_affected_by;
   int no_immune;
   int no_resistant;
   int no_suscept;
   int immune;
   int resistant;
   int susceptnum;
   int suscept;
   int obj[3];
   int race;
   int timer;
   int used;
   int vnum;
   int height;
   int weight;
   sh_int ac;
   sh_int bloodused;
   sh_int cha;
   sh_int con;
   sh_int dayfrom;
   sh_int dayto;
   sh_int dex;
   sh_int dodge;
   sh_int favourused;
   sh_int gloryused;
   sh_int hpused;
   sh_int inte;
   sh_int lck;
   sh_int level;
   sh_int manaused;
   sh_int moveused;
   sh_int parry;
   sh_int pkill;
   sh_int saving_breath;
   sh_int saving_para_petri;
   sh_int saving_poison_death;
   sh_int saving_spell_staff;
   sh_int saving_wand;
   sh_int sex;
   sh_int str;
   sh_int timefrom;
   sh_int timeto;
   sh_int tumble;
   sh_int wis;
   bool no_cast;
   bool objuse[3];
};


struct lcnv_data
{
   LCNV_DATA *next;
   LCNV_DATA *prev;
   char *old;
   int olen;
   char *new;
   int nlen;
};

struct lang_data
{
   LANG_DATA *next;
   LANG_DATA *prev;
   char *name;
   LCNV_DATA *first_precnv;
   LCNV_DATA *last_precnv;
   char *alphabet;
   LCNV_DATA *first_cnv;
   LCNV_DATA *last_cnv;
};

struct who_data
{
   WHO_DATA *prev;
   WHO_DATA *next;
   char *text;
   int type;
};

struct watch_data
{
   WATCH_DATA *next;
   WATCH_DATA *prev;
   sh_int imm_level;
   char *imm_name;
   char *target_name;
   char *player_site;
};


#define MAX_NUISANCE_STAGE 10
struct nuisance_data
{
   long int time;
   long int max_time;
   int flags;
   int power;
};

#define BAN_SITE        1
#define BAN_CLASS       2
#define BAN_RACE        3
#define BAN_WARN        -1

#include "board.h"
#define is_full_name is_name


struct ban_data
{
   BAN_DATA *next;
   BAN_DATA *prev;
   char *name;
   char *user;
   char *note;
   char *ban_by;
   char *ban_time;
   int flag;
   int unban_date;
   sh_int duration;
   sh_int level;
   bool warn;
   bool prefix;
   bool suffix;
};


struct reserve_data
{
   RESERVE_DATA *next;
   RESERVE_DATA *prev;
   char *name;
};

typedef enum
{
   SUN_DARK, SUN_RISE, SUN_LIGHT, SUN_SET
}
sun_positions;

typedef enum
{
   SKY_CLOUDLESS, SKY_CLOUDY, SKY_RAINING, SKY_LIGHTNING
}
sky_conditions;

void look_sky args( ( CHAR_DATA * ch ) );

struct time_info_data
{
   int hour;
   int day;
   int month;
   int year;
   int sunlight;
};

struct hour_min_sec
{
   int hour;
   int min;
   int sec;
   int manual;
};

#define MAX_CLIMATE 5

struct weather_data
{
   int temp;
   int precip;
   int wind;
   int temp_vector;
   int precip_vector;
   int wind_vector;
   int climate_temp;
   int climate_precip;
   int climate_wind;
   NEIGHBOR_DATA *first_neighbor;
   NEIGHBOR_DATA *last_neighbor;
   char *echo;
   int echo_color;
};

struct neighbor_data
{
   NEIGHBOR_DATA *next;
   NEIGHBOR_DATA *prev;
   char *name;
   AREA_DATA *address;
};

struct wizent
{
   WIZENT *next;
   WIZENT *last;
   char *name;
   sh_int level;
};

struct project_data
{
   PROJECT_DATA *next;
   PROJECT_DATA *prev;
   NOTE_DATA *first_log;
   NOTE_DATA *last_log;
   char *name;
   char *owner;
   char *coder;
   char *status;
   char *date;
   char *description;
   bool taken;
};

typedef enum
{
   CON_PLAYING, CON_GET_NAME, CON_GET_OLD_PASSWORD,
   CON_CONFIRM_NEW_NAME, CON_GET_NEW_PASSWORD, CON_CONFIRM_NEW_PASSWORD,
   CON_GET_NEW_SEX, CON_GET_NEW_CLASS, CON_READ_MOTD,
   CON_GET_NEW_RACE, CON_GET_EMULATION, CON_EDITING,
   CON_GET_WANT_RIPANSI, CON_TITLE, CON_PRESS_ENTER,
   CON_WAIT_1, CON_WAIT_2, CON_WAIT_3,
   CON_ACCEPTED, CON_GET_PKILL, CON_READ_IMOTD,
   CON_ROLL_STATS, CON_MEETING,
   CON_NOTE_TO, CON_NOTE_SUBJECT, CON_NOTE_EXPIRE,
   CON_NOTE_TEXT, CON_NOTE_FINISH, CON_COPYOVER_RECOVER, CON_GET_EXP
}
connection_types;

typedef enum
{
   SUB_NONE, SUB_PAUSE, SUB_PERSONAL_DESC, SUB_BAN_DESC, SUB_OBJ_SHORT,
   SUB_OBJ_LONG, SUB_OBJ_EXTRA, SUB_MOB_LONG, SUB_MOB_DESC, SUB_ROOM_DESC,
   SUB_ROOM_EXTRA, SUB_ROOM_EXIT_DESC, SUB_WRITING_NOTE, SUB_MPROG_EDIT,
   SUB_HELP_EDIT, SUB_WRITING_MAP, SUB_PERSONAL_BIO, SUB_REPEATCMD,
   SUB_RESTRICTED, SUB_DEITYDESC, SUB_MORPH_DESC, SUB_MORPH_HELP, SUB_MORPH_USING,
   SUB_PROJ_DESC,
   SUB_TIMER_DO_ABORT = 128, SUB_TIMER_CANT_ABORT, SUB_NOTE_TEXT, SUB_NS
}
char_substates;

struct descriptor_data
{
   DESCRIPTOR_DATA *next;
   DESCRIPTOR_DATA *prev;
   DESCRIPTOR_DATA *snoop_by;
   CHAR_DATA *character;
   CHAR_DATA *original;
   char *host;
   int port;
   int descriptor;
   sh_int connected;
   sh_int idle;
   sh_int lines;
   sh_int scrlen;
   bool fcommand;
   char inbuf[MAX_INBUF_SIZE];
   char incomm[MAX_INPUT_LENGTH];
   char inlast[MAX_INPUT_LENGTH];
   int repeat;
   char *outbuf;
   unsigned long outsize;
   int outtop;
   char *pagebuf;
   unsigned long pagesize;
   int pagetop;
   char *pagepoint;
   char pagecmd;
   char pagecolor;
   char *user;
   int newstate;
   unsigned char prevcolor;
   char *client;
};

struct str_app_type
{
   sh_int tohit;
   sh_int todam;
   sh_int carry;
   sh_int wield;
};

struct int_app_type
{
   sh_int learn;
};

struct wis_app_type
{
   sh_int practice;
};

struct dex_app_type
{
   sh_int defensive;
   sh_int carryn;
};

struct con_app_type
{
   sh_int hitp;
   sh_int shock;
};

struct cha_app_type
{
   sh_int charm;
};

struct lck_app_type
{
   sh_int luck;
};

typedef enum
{
   RACE_HUMAN, RACE_ELF, RACE_DWARF, RACE_HALFLING, RACE_PIXIE, RACE_VAMPIRE,
   RACE_HALF_OGRE, RACE_HALF_ORC, RACE_HALF_TROLL, RACE_HALF_ELF, RACE_GITH,
   RACE_DROW, RACE_KZINTI, RACE_LIZARDMAN, RACE_RA, RACE_DENIZEN
}
race_types;

#define	RACE_DRAGON	    31

#define CLASS_NONE	   -1
#define CLASS_MAGE	    0
#define CLASS_CLERIC	    1
#define CLASS_THIEF	    2
#define CLASS_WARRIOR	    3
#define CLASS_VAMPIRE	    4
#define CLASS_WARWIZARD   5
#define CLASS_JEDI  	    6
#define CLASS_WEREWOLF	    7
#define CLASS_RANGER        8
#define CLASS_TAEDIEN       9
#define CLASS_ARCHER	   10
#define CLASS_BARBARIAN    11
#define CLASS_SORCERER     12
#define CLASS_PALADIN	   13
#define CLASS_BARD	   14
#define CLASS_SAMURAI      15
#define CLASS_MONK	   16
#define CLASS_NEPHANDI	   17
#define CLASS_SAVAGE	   18

#define LANG_COMMON      BV00
#define LANG_ELVEN       BV01
#define LANG_DWARVEN     BV02
#define LANG_PIXIE       BV03
#define LANG_OGRE        BV04
#define LANG_ORCISH      BV05
#define LANG_TROLLISH    BV06
#define LANG_RODENT      BV07
#define LANG_INSECTOID   BV08
#define LANG_MAMMAL      BV09
#define LANG_REPTILE     BV10
#define LANG_DRAGON      BV11
#define LANG_SPIRITUAL   BV12
#define LANG_MAGICAL     BV13
#define LANG_GOBLIN      BV14
#define LANG_GOD         BV15
#define LANG_ANCIENT     BV16
#define LANG_HALFLING    BV17
#define LANG_CLAN	 BV18
#define LANG_GITH	 BV19
#define LANG_UNKNOWN        0
#define VALID_LANGS    ( LANG_COMMON | LANG_ELVEN | LANG_DWARVEN | LANG_PIXIE | LANG_OGRE | LANG_ORCISH | LANG_TROLLISH | LANG_GOBLIN | LANG_HALFLING | LANG_GITH )

typedef enum
{ TO_ROOM, TO_NOTVICT, TO_VICT, TO_CHAR, TO_CANSEE, TO_NOBSPAM }
to_types;

#define AT_COLORIZE	   -1
#define AT_BLACK	    0
#define AT_BLOOD	    1
#define AT_DGREEN           2
#define AT_ORANGE	    3
#define AT_DBLUE	    4
#define AT_PURPLE	    5
#define AT_CYAN	  	    6
#define AT_GREY		    7
#define AT_DGREY	    8
#define AT_RED		    9
#define AT_GREEN	   10
#define AT_YELLOW	   11
#define AT_BLUE		   12
#define AT_PINK		   13
#define AT_LBLUE	   14
#define AT_WHITE	   15
#define AT_BLINK	   16
typedef enum
{ AT_COLORBASE = 1024,
   AT_PLAIN = AT_COLORBASE,
   AT_ACTION,
   AT_SAY,
   AT_GOSSIP,
   AT_YELL,
   AT_TELL,
   AT_WHISPER,
   AT_HIT,
   AT_HITME,
   AT_IMMORT,
   AT_HURT,
   AT_FALLING,
   AT_DANGER,
   AT_MAGIC,
   AT_CONSIDER,
   AT_REPORT,
   AT_POISON,
   AT_SOCIAL,
   AT_DYING,
   AT_DEAD,
   AT_SKILL,
   AT_CARNAGE,
   AT_DAMAGE,
   AT_FLEE,
   AT_RMNAME,
   AT_RMDESC,
   AT_OBJECT,
   AT_PERSON,
   AT_LIST,
   AT_BYE,
   AT_GOLD,
   AT_GTELL,
   AT_NOTE,
   AT_HUNGRY,
   AT_THIRSTY,
   AT_FIRE,
   AT_SOBER,
   AT_WEAROFF,
   AT_EXITS,
   AT_SCORE,
   AT_RESET,
   AT_LOG,
   AT_DIEMSG,
   AT_WARTALK,
   AT_RACETALK,
   AT_IGNORE,
   AT_DIVIDER,
   AT_MORPH,
   AT_TOPCOLOR
}
at_color_types;
#define AT_MAXCOLOR	(AT_TOPCOLOR-AT_COLORBASE)

#if 0
#define AT_PLAIN	   AT_GREY
#define AT_ACTION	   AT_GREY
#define AT_SAY		   AT_LBLUE
#define AT_GOSSIP	   AT_LBLUE
#define AT_YELL	           AT_WHITE
#define AT_TELL		   AT_WHITE
#define AT_WHISPER	   AT_WHITE
#define AT_HIT		   AT_WHITE
#define AT_HITME	   AT_YELLOW
#define AT_IMMORT	   AT_YELLOW
#define AT_HURT		   AT_RED
#define AT_FALLING	   AT_WHITE
#define AT_DANGER	   AT_RED
#define AT_MAGIC	   AT_BLUE
#define AT_CONSIDER	   AT_GREY
#define AT_REPORT	   AT_GREY
#define AT_POISON	   AT_GREEN
#define AT_SOCIAL	   AT_CYAN
#define AT_DYING	   AT_YELLOW
#define AT_DEAD		   AT_RED
#define AT_SKILL	   AT_GREEN
#define AT_CARNAGE	   AT_BLOOD
#define AT_DAMAGE	   AT_WHITE
#define AT_FLEE		   AT_YELLOW
#define AT_RMNAME	   AT_WHITE
#define AT_RMDESC	   AT_ORANGE
#define AT_OBJECT	   AT_GREEN
#define AT_PERSON	   AT_PINK
#define AT_LIST		   AT_BLUE
#define AT_BYE		   AT_GREEN
#define AT_GOLD		   AT_YELLOW
#define AT_GTELL	   AT_BLUE
#define AT_NOTE		   AT_GREEN
#define AT_HUNGRY	   AT_ORANGE
#define AT_THIRSTY	   AT_BLUE
#define	AT_FIRE		   AT_RED
#define AT_SOBER	   AT_WHITE
#define AT_WEAROFF	   AT_YELLOW
#define AT_EXITS	   AT_WHITE
#define AT_SCORE	   AT_LBLUE
#define AT_RESET	   AT_DGREEN
#define AT_LOG		   AT_PURPLE
#define AT_DIEMSG	   AT_WHITE
#define AT_WARTALK         AT_RED
#define AT_RACETALK	   AT_DGREEN
#define AT_IGNORE	   AT_GREEN
#define AT_DIVIDER	   AT_PLAIN
#define AT_MORPH           AT_GREY
#endif

#define INIT_WEAPON_CONDITION    12
#define MAX_ITEM_IMPACT		 30

struct help_data
{
   HELP_DATA *next;
   HELP_DATA *prev;
   sh_int level;
   char *keyword;
   char *text;
};

#define MAX_TRADE	 5

struct shop_data
{
   SHOP_DATA *next;
   SHOP_DATA *prev;
   int keeper;
   sh_int buy_type[MAX_TRADE];
   sh_int profit_buy;
   sh_int profit_sell;
   sh_int open_hour;
   sh_int close_hour;
};

#define MAX_FIX		3
#define SHOP_FIX	1
#define SHOP_RECHARGE	2

struct repairshop_data
{
   REPAIR_DATA *next;
   REPAIR_DATA *prev;
   int keeper;
   sh_int fix_type[MAX_FIX];
   sh_int profit_fix;
   sh_int shop_type;
   sh_int open_hour;
   sh_int close_hour;
};


#define MAX_IFS 20
#define IN_IF 0
#define IN_ELSE 1
#define DO_IF 2
#define DO_ELSE 3

#define MAX_PROG_NEST 20

struct act_prog_data
{
   struct act_prog_data *next;
   void *vo;
};

struct mob_prog_act_list
{
   MPROG_ACT_LIST *next;
   char *buf;
   CHAR_DATA *ch;
   OBJ_DATA *obj;
   void *vo;
};

struct mob_prog_data
{
   MPROG_DATA *next;
   sh_int type;
   bool triggered;
   int resetdelay;
   char *arglist;
   char *comlist;
};

typedef enum
{ MP_MOB, MP_ROOM, MP_OBJ }
mp_types;

struct mpsleep_data
{
   MPSLEEP_DATA *next;
   MPSLEEP_DATA *prev;

   int timer;
   mp_types type;
   ROOM_INDEX_DATA *room;

   int ignorelevel;
   int iflevel;
   bool ifstate[MAX_IFS][DO_ELSE];

   char *com_list;
   CHAR_DATA *mob;
   CHAR_DATA *actor;
   OBJ_DATA *obj;
   void *vo;
   bool single_step;
};


bool MOBtrigger;

struct class_type
{
   char *who_name;
   EXT_BV affected;
   sh_int attr_prime;
   sh_int attr_second;
   sh_int attr_deficient;
   int resist;
   int suscept;
   int weapon;
   int guild;
   sh_int hpcost;
   sh_int manacost;
   sh_int movecost;
   sh_int skill_adept;
   sh_int thac0_00;
   sh_int thac0_32;
   sh_int hp_min;
   sh_int hp_max;
   bool fMana;
   bool fCreate;
   sh_int exp_base;
   sh_int exp_tnl;
   sh_int can_carry_w;
   sh_int exp_multiplier;
};

struct race_type
{
   char race_name[16];
   EXT_BV affected;
   sh_int str_plus;
   sh_int dex_plus;
   sh_int wis_plus;
   sh_int int_plus;
   sh_int con_plus;
   sh_int cha_plus;
   sh_int lck_plus;
   sh_int hit;
   sh_int mana;
   int evolve;
   int resist;
   int suscept;
   int class_restriction;
   int language;
   sh_int ac_plus;
   sh_int alignment;
   EXT_BV attacks;
   EXT_BV defenses;
   sh_int minalign;
   sh_int maxalign;
   sh_int exp_multiplier;
   sh_int height;
   sh_int weight;
   sh_int hunger_mod;
   sh_int thirst_mod;
   sh_int saving_poison_death;
   sh_int saving_wand;
   sh_int saving_para_petri;
   sh_int saving_breath;
   sh_int saving_spell_staff;
   char *where_name[MAX_WHERE_NAME];
   int mana_regen;
   int hp_regen;
   sh_int race_recall;
   sh_int can_carry_w;
   sh_int exp_tnl;
};

typedef enum
{
   CLAN_PLAIN, CLAN_VAMPIRE, CLAN_WARRIOR, CLAN_MAGE, CLAN_CELTIC,
   CLAN_THIEF, CLAN_CLERIC, CLAN_UNDEAD, CLAN_CHAOTIC, CLAN_NEUTRAL, CLAN_LAWFUL,
   CLAN_NOKILL, CLAN_ORDER, CLAN_GUILD
}
clan_types;

typedef enum
{
   GROUP_CLAN, GROUP_COUNCIL, GROUP_GUILD
}
group_types;


struct clan_data
{
   CLAN_DATA *next;
   CLAN_DATA *prev;
   char *filename;
   char *name;
   char *longname;
   char *motto;
   char *description;
   char *deity;
   char *leader;
   char *number1;
   char *number2;
   char *badge;
   int memrank;
   int leadrank;
   int onerank;
   int tworank;
   int pkyorn;
   int pkills[7];
   int pdeaths[7];
   int mkills;
   int mdeaths;
   int illegal_pk;
   int score;
   sh_int clan_type;
   sh_int favour;
   sh_int strikes;
   sh_int members;
   sh_int mem_limit;
   sh_int alignment;
   int board;
   int clanobj1;
   int clanobj2;
   int clanobj3;
   int clanobj4;
   int clanobj5;
   int recall;
   int storeroom;
   int guard1;
   int guard2;
   int class;
};

struct council_data
{
   COUNCIL_DATA *next;
   COUNCIL_DATA *prev;
   char *filename;
   char *name;
   char *description;
   char *head;
   char *head2;
   char *powers;
   sh_int members;
   int board;
   int meeting;
};

struct deity_data
{
   DEITY_DATA *next;
   DEITY_DATA *prev;
   char *filename;
   char *name;
   char *description;
   int element;
   sh_int strpls;
   sh_int intpls;
   sh_int wispls;
   sh_int dexpls;
   sh_int conpls;
   sh_int chapls;
   sh_int lckpls;
   sh_int exppls;
   sh_int hitgain;
   sh_int managain;
   sh_int movegain;
   sh_int dampls;
   sh_int splcst;
   sh_int spldur;
};

struct tourney_data
{
   int open;
   int low_level;
   int hi_level;
};

struct note_data
{
   NOTE_DATA *next;
   NOTE_DATA *prev;
   char *sender;
   char *date;
   char *to_list;
   char *subject;
   int voting;
   char *yesvotes;
   char *novotes;
   char *abstentions;
   char *text;
   time_t expire;
   time_t date_stamp;
};

struct board_data
{
   BOARD_DATA *next;
   BOARD_DATA *prev;
   NOTE_DATA *first_note;
   NOTE_DATA *last_note;
   char *note_file;
   char *read_group;
   char *post_group;
   char *extra_readers;
   char *extra_removers;
   char *otakemessg;
   char *opostmessg;
   char *oremovemessg;
   char *ocopymessg;
   char *olistmessg;
   char *postmessg;
   char *oreadmessg;
   int board_obj;
   sh_int num_posts;
   sh_int min_read_level;
   sh_int min_post_level;
   sh_int min_remove_level;
   sh_int max_posts;
   int type;
};



struct at_color_type
{
   char *name;
   sh_int def_color;
};



struct affect_data
{
   AFFECT_DATA *next;
   AFFECT_DATA *prev;
   sh_int type;
   sh_int duration;
   sh_int location;
   int modifier;
   EXT_BV bitvector;
};


struct acadia_affect
{
   ACADIA_AFF *next;
   char *duration;
   sh_int location;
   char *modifier;
   int bitvector;
};

struct news_data
{
   NEWS_DATA *next;
   NEWS_DATA *prev;
   int day;
   int month;
   int year;
   char *news_data;
   time_t time_stamp;
};

#define MOB_VNUM_CITYGUARD	   3060
#define MOB_VNUM_VAMPIRE	   80
#define MOB_VNUM_ANIMATED_CORPSE   5
#define MOB_VNUM_POLY_WOLF	   10
#define MOB_VNUM_POLY_MIST	   11
#define MOB_VNUM_POLY_BAT	   12
#define MOB_VNUM_POLY_HAWK	   13
#define MOB_VNUM_POLY_CAT	   14
#define MOB_VNUM_POLY_DOVE	   15
#define MOB_VNUM_POLY_FISH	   16
#define MOB_VNUM_DEITY		   17
#define MOB_VNUM_CALLMOUNT		4

#define ACT_IS_NPC		  0
#define ACT_SENTINEL		  1
#define ACT_SCAVENGER		  2
#define ACT_BLOCKER			3
#define ACT_UNDERTAKER		 4
#define ACT_AGGRESSIVE		  5
#define ACT_STAY_AREA		  6
#define ACT_WIMPY		  7
#define ACT_PET			  8
#define ACT_TRAIN		  9
#define ACT_PRACTICE		 10
#define ACT_IMMORTAL		 11
#define ACT_DEADLY		 12
#define ACT_SHOP		 13
#define ACT_INNKEEPER		 14
#define ACT_GUARDIAN		 15
#define ACT_RUNNING		 16
#define ACT_NOWANDER		 17
#define ACT_MOUNTABLE		 18
#define ACT_MOUNTED		 19
#define ACT_DRAGOON          20
#define ACT_SECRETIVE		 21
#define ACT_NOPURGE			22
#define ACT_MOBINVIS		 23
#define ACT_NOASSIST		 24
#define ACT_AUTONOMOUS		 25
#define ACT_PACIFIST             26
#define ACT_NOATTACK		 27
#define ACT_ANNOYING		 28
#define ACT_STATSHIELD		 29
#define ACT_PROTOTYPE		 30
#define ACT_REGISTRAR		31

typedef enum
{
   // Spell and Skill affects
   AFF_BLIND, AFF_INVISIBLE, AFF_DETECT_EVIL, AFF_DETECT_INVIS,
   AFF_DETECT_MAGIC, AFF_DETECT_HIDDEN, AFF_HOLD, AFF_SANCTUARY,
   AFF_FAERIE_FIRE, AFF_INFRARED, AFF_CURSE, AFF_MOVEHIDE, AFF_POISON,
   AFF_PROTECT, AFF_PARALYSIS, AFF_SNEAK, AFF_HIDE, AFF_SLEEP, AFF_CHARM,
   AFF_FLYING, AFF_PASS_DOOR, AFF_FLOATING, AFF_TRUESIGHT, AFF_DETECTTRAPS,
   AFF_SCRYING, AFF_FIRESHIELD, AFF_SHOCKSHIELD, AFF_IMPINVISIBLE, AFF_ICESHIELD,
   AFF_ALERTNESS, AFF_BERSERK, AFF_AQUA_BREATH, AFF_INVINCIBILITY, AFF_IRONSKIN,
   AFF_GODSPELL, AFF_HOLYAURA, AFF_POSSESS, AFF_JMT, AFF_REGEN, AFF_BARKSKIN,
   AFF_DEMONFIRE, AFF_CONTRACEPTION, AFF_DELIRIUM, AFF_STEEL_SKIN, AFF_JER, AFF_HADESARMOR,
   AFF_ARMOR, AFF_JIR, AFF_PURITY, AFF_JUR, AFF_HEADACHE, AFF_BLACKFLAME, AFF_DANGLE,
   AFF_GODBLESS, AFF_FRENZY, AFF_HOLYSIGHT, AFF_QUICKNESS, AFF_BATTLEPLAN, AFF_BIOFEEDBACK,
   AFF_DISPLACEMENT, AFF_CALCIFY, AFF_PLAGUE, AFF_STONESKIN, AFF_SHIELD, AFF_IRONHAND,
   AFF_SHAPESHIFT, AFF_KARAAURA, AFF_DEMI,
   // Bard affects
   AFF_MINNE, AFF_MINNE2, AFF_MINNE3, AFF_MINNE4, AFF_MINUET, AFF_MINUET2, AFF_MINUET3, AFF_MINUET4,
   AFF_ETUDE,
   // Wraooer affects
   AFF_RECURRINGSPELL, AFF_CONTAGIOUS, AFF_ACIDMIST, AFF_VENOMSHIELD, AFF_HAUS1, MAX_AFFECTED_BY
}
affected_by_types;

#define RIS_FIRE		  BV00
#define RIS_COLD		  BV01
#define RIS_ELECTRICITY		  BV02
#define RIS_ENERGY		  BV03
#define RIS_BLUNT		  BV04
#define RIS_PIERCE		  BV05
#define RIS_SLASH		  BV06
#define RIS_ACID		  BV07
#define RIS_POISON		  BV08
#define RIS_DRAIN		  BV09
#define RIS_SLEEP		  BV10
#define RIS_CHARM		  BV11
#define RIS_HOLD		  BV12
#define RIS_NONMAGIC		  BV13
#define RIS_PLUS1		  BV14
#define RIS_PLUS2		  BV15
#define RIS_PLUS3		  BV16
#define RIS_PLUS4		  BV17
#define RIS_PLUS5		  BV18
#define RIS_PLUS6		  BV19
#define RIS_MAGIC		  BV20
#define RIS_PARALYSIS		  BV21
#define RIS_HACK		  BV22
#define RIS_LASH		  BV23
#define RIS_PSIONIC		  BV24

typedef enum
{
   ATCK_BITE, ATCK_CLAWS, ATCK_TAIL, ATCK_STING, ATCK_PUNCH, ATCK_KICK,
   ATCK_TRIP, ATCK_BASH, ATCK_STUN, ATCK_GOUGE, ATCK_BACKSTAB, ATCK_FEED,
   ATCK_DRAIN, ATCK_FIREBREATH, ATCK_FROSTBREATH, ATCK_ACIDBREATH,
   ATCK_LIGHTNBREATH, ATCK_GASBREATH, ATCK_POISON, ATCK_NASTYPOISON, ATCK_GAZE,
   ATCK_BLINDNESS, ATCK_CAUSESERIOUS, ATCK_EARTHQUAKE, ATCK_CAUSECRITICAL,
   ATCK_CURSE, ATCK_FLAMESTRIKE, ATCK_HARM, ATCK_FIREBALL, ATCK_COLORSPRAY,
   ATCK_WEAKEN, ATCK_SPIRALBLAST, MAX_ATTACK_TYPE
}
attack_types;

typedef enum
{
   DFND_PARRY, DFND_DODGE, DFND_HEAL, DFND_CURELIGHT, DFND_CURESERIOUS,
   DFND_CURECRITICAL, DFND_DISPELMAGIC, DFND_DISPELEVIL, DFND_SANCTUARY,
   DFND_FIRESHIELD, DFND_SHOCKSHIELD, DFND_SHIELD, DFND_BLESS, DFND_STONESKIN,
   DFND_TELEPORT, DFND_MONSUM1, DFND_MONSUM2, DFND_MONSUM3, DFND_MONSUM4,
   DFND_DISARM, DFND_ICESHIELD, DFND_GRIP, DFND_TRUESIGHT, DFND_ACIDMIST,
   DFND_VENOMSHIELD, MAX_DEFENSE_TYPE
}
defense_types;

#define PART_HEAD		  BV00
#define PART_ARMS		  BV01
#define PART_LEGS		  BV02
#define PART_HEART		  BV03
#define PART_BRAINS		  BV04
#define PART_GUTS		  BV05
#define PART_HANDS		  BV06
#define PART_FEET		  BV07
#define PART_FINGERS		  BV08
#define PART_EAR		  BV09
#define PART_EYE		  BV10
#define PART_LONG_TONGUE	  BV11
#define PART_EYESTALKS		  BV12
#define PART_TENTACLES		  BV13
#define PART_FINS		  BV14
#define PART_WINGS		  BV15
#define PART_TAIL		  BV16
#define PART_SCALES		  BV17
#define PART_CLAWS		  BV18
#define PART_FANGS		  BV19
#define PART_HORNS		  BV20
#define PART_TUSKS		  BV21
#define PART_TAILATTACK		  BV22
#define PART_SHARPSCALES	  BV23
#define PART_BEAK		  BV24

#define PART_HAUNCH		  BV25
#define PART_HOOVES		  BV26
#define PART_PAWS		  BV27
#define PART_FORELEGS		  BV28
#define PART_FEATHERS		  BV29

#define SV_DEATH		  BV00
#define SV_KILL			  BV01
#define SV_PASSCHG		  BV02
#define SV_DROP			  BV03
#define SV_PUT			  BV04
#define SV_GIVE			  BV05
#define SV_AUTO			  BV06
#define SV_ZAPDROP		  BV07
#define SV_AUCTION		  BV08
#define SV_GET			  BV09
#define SV_RECEIVE		  BV10
#define SV_IDLE			  BV11
#define SV_BACKUP		  BV12
#define SV_QUITBACKUP	  BV13
#define SV_FILL			  BV14
#define SV_EMPTY		  BV15
#define SV_LEVEL		  BV16

#define PIPE_TAMPED		  BV01
#define PIPE_LIT		  BV02
#define PIPE_HOT		  BV03
#define PIPE_DIRTY		  BV04
#define PIPE_FILTHY		  BV05
#define PIPE_GOINGOUT		  BV06
#define PIPE_BURNT		  BV07
#define PIPE_FULLOFASH		  BV08

#define STRING_NONE               0
#define STRING_IMM                BV01


#define OLD_SF_SAVE_HALF_DAMAGE	  BV18
#define OLD_SF_SAVE_NEGATES	  BV19

#define SF_WATER		  BV00
#define SF_EARTH		  BV01
#define SF_AIR			  BV02
#define SF_ASTRAL		  BV03
#define SF_AREA			  BV04
#define SF_DISTANT		  BV05
#define SF_REVERSE		  BV06
#define SF_NOSELF		  BV07
#define SF_UNUSED2		  BV08
#define SF_ACCUMULATIVE		  BV09
#define SF_RECASTABLE		  BV10
#define SF_NOSCRIBE		  BV11
#define SF_NOBREW		  BV12
#define SF_GROUPSPELL		  BV13
#define SF_OBJECT		  BV14
#define SF_CHARACTER		  BV15
#define SF_SECRETSKILL		  BV16
#define SF_PKSENSITIVE		  BV17
#define SF_STOPONFAIL		  BV18
#define SF_NOFIGHT		  BV19
#define SF_NODISPEL               BV20
#define SF_RANDOMTARGET		  BV21
typedef enum
{ SS_NONE, SS_POISON_DEATH, SS_ROD_WANDS, SS_PARA_PETRI,
   SS_BREATH, SS_SPELL_STAFF
}
save_types;

#define ALL_BITS		INT_MAX
#define SDAM_MASK		ALL_BITS & ~(BV00 | BV01 | BV02)
#define SACT_MASK		ALL_BITS & ~(BV03 | BV04 | BV05)
#define SCLA_MASK		ALL_BITS & ~(BV06 | BV07 | BV08)
#define SPOW_MASK		ALL_BITS & ~(BV09 | BV10)
#define SSAV_MASK		ALL_BITS & ~(BV11 | BV12 | BV13)

typedef enum
{ SD_NONE, SD_FIRE, SD_COLD, SD_ELECTRICITY, SD_ENERGY, SD_ACID,
   SD_POISON, SD_DRAIN
}
spell_dam_types;

typedef enum
{ SA_NONE, SA_CREATE, SA_DESTROY, SA_RESIST, SA_SUSCEPT,
   SA_DIVINATE, SA_OBSCURE, SA_CHANGE
}
spell_act_types;

typedef enum
{ SP_NONE, SP_MINOR, SP_GREATER, SP_MAJOR }
spell_power_types;

typedef enum
{ SC_NONE, SC_LUNAR, SC_SOLAR, SC_TRAVEL, SC_SUMMON,
   SC_LIFE, SC_DEATH, SC_ILLUSION
}
spell_class_types;

typedef enum
{ SE_NONE, SE_NEGATE, SE_EIGHTHDAM, SE_QUARTERDAM, SE_HALFDAM,
   SE_3QTRDAM, SE_REFLECT, SE_ABSORB
}
spell_save_effects;

typedef enum
{ SEX_MALE, SEX_FEMALE }
sex_types;

typedef enum
{
   TRAP_TYPE_POISON_GAS = 1, TRAP_TYPE_POISON_DART, TRAP_TYPE_POISON_NEEDLE,
   TRAP_TYPE_POISON_DAGGER, TRAP_TYPE_POISON_ARROW, TRAP_TYPE_BLINDNESS_GAS,
   TRAP_TYPE_SLEEPING_GAS, TRAP_TYPE_FLAME, TRAP_TYPE_EXPLOSION,
   TRAP_TYPE_ACID_SPRAY, TRAP_TYPE_ELECTRIC_SHOCK, TRAP_TYPE_BLADE,
   TRAP_TYPE_SEX_CHANGE
}
trap_types;

#define MAX_TRAPTYPE		   TRAP_TYPE_SEX_CHANGE

#define TRAP_ROOM      		   BV00
#define TRAP_OBJ	      	   BV01
#define TRAP_ENTER_ROOM		   BV02
#define TRAP_LEAVE_ROOM		   BV03
#define TRAP_OPEN		   BV04
#define TRAP_CLOSE		   BV05
#define TRAP_GET		   BV06
#define TRAP_PUT		   BV07
#define TRAP_PICK		   BV08
#define TRAP_UNLOCK		   BV09
#define TRAP_N			   BV10
#define TRAP_S			   BV11
#define TRAP_E	      		   BV12
#define TRAP_W	      		   BV13
#define TRAP_U	      		   BV14
#define TRAP_D	      		   BV15
#define TRAP_EXAMINE		   BV16
#define TRAP_NE			   BV17
#define TRAP_NW			   BV18
#define TRAP_SE			   BV19
#define TRAP_SW			   BV20

#define SIT_ON     BV00
#define SIT_IN     BV01
#define SIT_AT     BV02

#define STAND_ON   BV03
#define STAND_IN   BV04
#define STAND_AT   BV05

#define SLEEP_ON   BV06
#define SLEEP_IN   BV07
#define SLEEP_AT   BV08

#define REST_ON     BV09
#define REST_IN     BV10
#define REST_AT     BV11

#define OBJ_VNUM_GOLD_ONE			2
#define OBJ_VNUM_GOLD_SOME			3
#define OBJ_VNUM_SILVER_ONE			76
#define OBJ_VNUM_SILVER_SOME		77
#define OBJ_VNUM_COPPER_ONE			78
#define OBJ_VNUM_COPPER_SOME		79

#define OBJ_VNUM_CLIGHT 		9143
#define OBJ_VNUM_CORPSE_NPC			10
#define OBJ_VNUM_CORPSE_PC			11
#define OBJ_VNUM_SEVERED_HEAD	    12
#define OBJ_VNUM_TORN_HEART			13
#define OBJ_VNUM_SLICED_ARM			14
#define OBJ_VNUM_SLICED_LEG			15
#define OBJ_VNUM_SPILLED_GUTS	    16
#define OBJ_VNUM_BLOOD				17
#define OBJ_VNUM_BLOODSTAIN			18
#define OBJ_VNUM_SCRAPS				19

#define OBJ_VNUM_MUSHROOM			20
#define OBJ_VNUM_LIGHT_BALL			21
#define OBJ_VNUM_SPRING				22

#define OBJ_VNUM_SKIN				23
#define OBJ_VNUM_SLICE				24
#define OBJ_VNUM_SHOPPING_BAG	    25

#define OBJ_VNUM_BLOODLET			26

#define OBJ_VNUM_FIRE				30
#define OBJ_VNUM_TRAP				31
#define OBJ_VNUM_PORTAL				32

#define OBJ_VNUM_BLACK_POWDER	    9100
#define OBJ_VNUM_SCROLL_SCRIBING    34
#define OBJ_VNUM_FLASK_BREWING      35
#define OBJ_VNUM_NOTE				36
#define OBJ_VNUM_DEITY				64

#define OBJ_VNUM_SHARPEN			9101

#define OBJ_VNUM_SCHOOL_MACE		9111
#define OBJ_VNUM_SCHOOL_LIGHTSABER	9110
#define OBJ_VNUM_SCHOOL_DAGGER		9112
#define OBJ_VNUM_SCHOOL_GHOST		9102
#define OBJ_VNUM_SCHOOL_SWORD		9113
#define OBJ_VNUM_SCHOOL_SWORD_TWO	9144
#define OBJ_VNUM_SCHOOL_VEST		9114
#define OBJ_VNUM_SCHOOL_SHIELD		9115
#define OBJ_VNUM_SCHOOL_BANNER		9116
#define OBJ_VNUM_SCHOOL_STAFF		9117
#define OBJ_VNUM_DIAMOND_RING		9103
#define OBJ_VNUM_WEDDING_RING		9103
#define OBJ_VNUM_LIGHTSABER_MAKE  1021
#define OBJ_VNUM_ARROW_MAKE			9118
#define OBJ_VNUM_QUIVER_MAKE		9119
#define OBJ_VNUM_BOW_MAKE			9120
#define OBJ_VNUM_FLUTE_MAKE			98
#define OBJ_VNUM_POTION_MAKE		9145
#define OBJ_VNUM_SCHOOL_BOW			9121
#define OBJ_VNUM_SCHOOL_ARROW       9122
#define OBJ_VNUM_SCHOOL_QUIVER		9123

#define OBJ_VNUM_ANGEL_HALO			9108
#define OBJ_VNUM_ANGEL_WINGS		9109
#define OBJ_VNUM_ANGEL_ARMOR		9104
#define OBJ_VNUM_ANGEL_BOOTS		9105
#define OBJ_VNUM_ANGEL_PANTS		9106
#define OBJ_VNUM_ANGEL_AURA 		9107

#define IMMEQ1	9124
#define IMMEQ2	9125
#define IMMEQ3	9126
#define IMMEQ4	9127
#define IMMEQ5	9128
#define IMMEQ6	9129
#define IMMEQ7	9130
#define IMMEQ8	9131
#define IMMEQ9	9132
#define IMMEQ10	9133
#define IMMEQ11	9134
#define IMMEQ12	9135
#define IMMEQ13	9136
#define IMMEQ14	9137
#define IMMEQ15	9138
#define IMMEQ16	9140
#define IMMEQ17	9141
#define IMMEQ18	9142


typedef enum
{
   ITEM_NONE, ITEM_LIGHT, ITEM_SCROLL, ITEM_WAND, ITEM_STAFF, ITEM_WEAPON,
   ITEM_FIREWEAPON, ITEM_MISSILE, ITEM_TREASURE, ITEM_ARMOR, ITEM_POTION,
   ITEM_WORN, ITEM_FURNITURE, ITEM_TRASH, ITEM_OLDTRAP, ITEM_CONTAINER,
   ITEM_NOTE, ITEM_DRINK_CON, ITEM_KEY, ITEM_FOOD, ITEM_GOLD, ITEM_PEN,
   ITEM_BOAT, ITEM_CORPSE_NPC, ITEM_CORPSE_PC, ITEM_FOUNTAIN, ITEM_PILL,
   ITEM_BLOOD, ITEM_BLOODSTAIN, ITEM_SCRAPS, ITEM_PIPE, ITEM_HERB_CON,
   ITEM_HERB, ITEM_INCENSE, ITEM_FIRE, ITEM_BOOK, ITEM_SWITCH, ITEM_LEVER,
   ITEM_PULLCHAIN, ITEM_BUTTON, ITEM_DIAL, ITEM_RUNE, ITEM_RUNEPOUCH,
   ITEM_MATCH, ITEM_TRAP, ITEM_MAP, ITEM_PORTAL, ITEM_PAPER,
   ITEM_TINDER, ITEM_LOCKPICK, ITEM_SPIKE, ITEM_DISEASE, ITEM_OIL, ITEM_FUEL,
   ITEM_PIECE, ITEM_GHOST, ITEM_MISSILE_WEAPON, ITEM_PROJECTILE, ITEM_QUIVER,
   ITEM_SHOVEL, ITEM_SALVE, ITEM_COOK, ITEM_KEYRING, ITEM_ODOR, ITEM_WINDOW,
   ITEM_TOOL, ITEM_SILVERC, ITEM_COPPER, ITEM_FLUTE, ITEM_ORE, ITEM_KEYITEM,
   ITEM_CHANCE
}
item_types;

#define MAX_ITEM_TYPE		     ITEM_CHANCE

typedef enum
{
   ITEM_GLOW, ITEM_HUM, ITEM_DARK, ITEM_LOYAL, ITEM_EVIL, ITEM_INVIS, ITEM_MAGIC,
   ITEM_NODROP, ITEM_BLESS, ITEM_ANTI_GOOD, ITEM_ANTI_EVIL, ITEM_ANTI_NEUTRAL,
   ITEM_NOREMOVE, ITEM_INVENTORY, ITEM_ANTI_MAGE, ITEM_ANTI_THIEF, ITEM_ANTI_WARRIOR,
   ITEM_ANTI_CLERIC, ITEM_ORGANIC, ITEM_METAL, ITEM_DONATION, ITEM_CLANOBJECT,
   ITEM_CLANCORPSE, ITEM_ANTI_VAMPIRE, ITEM_ANTI_DRUID, ITEM_HIDDEN, ITEM_POISONED,
   ITEM_COVERING, ITEM_DEATHROT, ITEM_BURIED, ITEM_PROTOTYPE, ITEM_NOLOCATE,
   ITEM_GROUNDROT, ITEM_LOOTABLE, ITEM_ANTI_MALE, ITEM_ANTI_FEMALE, ITEM_ANTI_RANGER,
   ITEM_NOSCRAP, ITEM_ANTI_BARBARIAN, ITEM_ANTI_SORCERER, ITEM_ANTI_PALADIN, ITEM_NOIMP,
   ITEM_LODGED, ITEM_SHARP, ITEM_ANTI_ARCHER, ITEM_ANTI_JEDI, ITEM_ANTI_WEREWOLF,
   ITEM_JEDI_ONLY, ITEM_ARCHER_ONLY, ITEM_ANTI_TAEDIEN, ITEM_NODUALW, ITEM_OWNED,
   ITEM_NOPURGE, ITEM_TWOHAND, ITEM_SAM_ONLY, ITEM_RARE, ITEM_DNIGHT, ITEM_SILVER, ITEM_QOP,
   MAX_ITEM_FLAG
}
item_extra_flags;

#define ITEM_RETURNING		BV00
#define ITEM_BACKSTABBER  	BV01
#define ITEM_BANE		BV02
#define ITEM_MAGIC_LOYAL	BV03
#define ITEM_HASTE		BV04
#define ITEM_DRAIN		BV05
#define ITEM_LIGHTNING_BLADE  	BV06
#define ITEM_PKDISARMED		BV07

#define TRIG_UP			BV00
#define TRIG_UNLOCK		BV01
#define TRIG_LOCK		BV02
#define TRIG_D_NORTH		BV03
#define TRIG_D_SOUTH		BV04
#define TRIG_D_EAST		BV05
#define TRIG_D_WEST		BV06
#define TRIG_D_UP		BV07
#define TRIG_D_DOWN		BV08
#define TRIG_DOOR		BV09
#define TRIG_CONTAINER		BV10
#define TRIG_OPEN		BV11
#define TRIG_CLOSE		BV12
#define TRIG_PASSAGE		BV13
#define TRIG_OLOAD		BV14
#define TRIG_MLOAD		BV15
#define TRIG_TELEPORT		BV16
#define TRIG_TELEPORTALL	BV17
#define TRIG_TELEPORTPLUS	BV18
#define TRIG_DEATH		BV19
#define TRIG_CAST		BV20
#define TRIG_FAKEBLADE		BV21
#define TRIG_RAND4		BV22
#define TRIG_RAND6		BV23
#define TRIG_TRAPDOOR		BV24
#define TRIG_ANOTHEROOM		BV25
#define TRIG_USEDIAL		BV26
#define TRIG_ABSOLUTEVNUM	BV27
#define TRIG_SHOWROOMDESC	BV28
#define TRIG_AUTORETURN		BV29

#define TELE_SHOWDESC		BV00
#define TELE_TRANSALL		BV01
#define TELE_TRANSALLPLUS	BV02


#define ITEM_TAKE		BV00
#define ITEM_WEAR_FINGER	BV01
#define ITEM_WEAR_NECK		BV02
#define ITEM_WEAR_BODY		BV03
#define ITEM_WEAR_HEAD		BV04
#define ITEM_WEAR_LEGS		BV05
#define ITEM_WEAR_FEET		BV06
#define ITEM_WEAR_HANDS		BV07
#define ITEM_WEAR_ARMS		BV08
#define ITEM_WEAR_SHIELD	BV09
#define ITEM_WEAR_ABOUT		BV10
#define ITEM_WEAR_WAIST		BV11
#define ITEM_WEAR_WRIST		BV12
#define ITEM_WIELD		BV13
#define ITEM_HOLD		BV14
#define ITEM_DUAL_WIELD		BV15
#define ITEM_WEAR_EARS		BV16
#define ITEM_WEAR_EYES		BV17
#define ITEM_MISSILE_WIELD	BV18
#define ITEM_WEAR_BACK		BV19
#define ITEM_WEAR_FACE		BV20
#define ITEM_WEAR_ANKLE		BV21
#define ITEM_WEAR_FLOAT		BV22
#define ITEM_WEAR_CLAN		BV23
#define ITEM_LODGE_RIB		BV24
#define ITEM_LODGE_ARM		BV25
#define ITEM_LODGE_LEG		BV26
#define ITEM_WEAR_MAX		27

typedef enum
{
   APPLY_NONE, APPLY_STR, APPLY_DEX, APPLY_INT, APPLY_WIS, APPLY_CON,
   APPLY_SEX, APPLY_CLASS, APPLY_LEVEL, APPLY_AGE, APPLY_HEIGHT, APPLY_WEIGHT,
   APPLY_MANA, APPLY_HIT, APPLY_MOVE, APPLY_GOLD, APPLY_EXP, APPLY_AC,
   APPLY_HITROLL, APPLY_DAMROLL, APPLY_SAVING_POISON, APPLY_SAVING_ROD,
   APPLY_SAVING_PARA, APPLY_SAVING_BREATH, APPLY_SAVING_SPELL, APPLY_CHA,
   APPLY_AFFECT, APPLY_RESISTANT, APPLY_IMMUNE, APPLY_SUSCEPTIBLE,
   APPLY_WEAPONSPELL, APPLY_LCK, APPLY_BACKSTAB, APPLY_PICK, APPLY_TRACK,
   APPLY_STEAL, APPLY_SNEAK, APPLY_HIDE, APPLY_PALM, APPLY_DETRAP, APPLY_DODGE,
   APPLY_PEEK, APPLY_SCAN, APPLY_GOUGE, APPLY_SEARCH, APPLY_MOUNT, APPLY_DISARM,
   APPLY_KICK, APPLY_PARRY, APPLY_BASH, APPLY_STUN, APPLY_PUNCH, APPLY_CLIMB,
   APPLY_GRIP, APPLY_SCRIBE, APPLY_BREW, APPLY_WEARSPELL, APPLY_REMOVESPELL,
   APPLY_EMOTION, APPLY_MENTALSTATE, APPLY_STRIPSN, APPLY_REMOVE, APPLY_DIG,
   APPLY_FULL, APPLY_THIRST, APPLY_DRUNK, APPLY_BLOOD, APPLY_COOK,
   APPLY_RECURRINGSPELL, APPLY_CONTAGIOUS, APPLY_EXT_AFFECT, APPLY_ODOR,
   APPLY_ROOMFLAG, APPLY_SECTORTYPE, APPLY_ROOMLIGHT, APPLY_TELEVNUM,
   APPLY_TELEDELAY, APPLY_MOVEHIDE, APPLY_EWHITROLL, APPLY_EWDAMROLL,
   APPLY_EAAC, APPLY_SILVER, APPLY_COPPER, MAX_APPLY_TYPE
}
apply_types;

#define REVERSE_APPLY		   1000

#define CONT_CLOSEABLE		   BV00
#define CONT_PICKPROOF		   BV01
#define CONT_CLOSED		   BV02
#define CONT_LOCKED		   BV03
#define CONT_EATKEY		   BV04

#define ROOM_VNUM_LIMBO		      2
#define ROOM_VNUM_POLY		      3
#define ROOM_VNUM_CHAT		   1200
#define ROOM_VNUM_TEMPLE	  100000
#define ROOM_VNUM_ALTAR		  1010
#define ROOM_VNUM_SCHOOL_ADV	  9025
#define ROOM_VNUM_SCHOOL		    100
#define ROOM_VNUM_HALLOFFALLEN    21195
#define ROOM_VNUM_MORGUE        100195
#define BANK_VNUM_1				100206
#define ROOM_VNUM_HELL			30197
#define ROOM_VNUM_HELL2			30198
#define ROOM_VNUM_HELL3			30199
#define ROOM_VNUM_DONATION		100199
#define ROOM_VNUM_DONATIONTWO	8004
#define ROOM_VNUM_TERRA			100200
#define ROOM_VNUM_WHITE			31651
#define ROOM_VNUM_LORUR			50000
#define ROOM_VNUM_OPHELIA		50051
#define ROOM_VNUM_UTOPIA		50101
#define ROOM_VNUM_D_RECALL		100000
#define ROOM_VNUM_MEETING		1300
#define ROOM_VNUM_SANCTUARY		8000
#define ROOM_VNUM_ANGEL			1400
#define ROOM_VNUM_DRAGOON		502

#define BVSECT_INSIDE 			BV00
#define BVSECT_CITY 			BV01
#define BVSECT_FIELD 			BV02
#define BVSECT_FOREST 			BV03
#define BVSECT_HILLS 			BV04
#define BVSECT_MOUNTAIN 		BV05
#define BVSECT_WATER_SWIM 		BV06
#define BVSECT_WATER_NOSWIM 		BV07
#define BVSECT_UNDERWATER 		BV08
#define BVSECT_AIR 			BV09
#define BVSECT_DESERT 			BV10
#define BVSECT_DUNNO 			BV11
#define BVSECT_OCEANFLOOR 		BV12
#define BVSECT_UNDERGROUND 		BV13
#define BVSECT_LAVA			BV14
#define BVSECT_SWAMP			BV15
#define MAX_SECFLAG 			15

typedef enum
{
   ROOM_DARK, ROOM_DEATH, ROOM_NO_MOB, ROOM_INDOORS, ROOM_LAWFUL, ROOM_NEUTRAL, ROOM_CHAOTIC,
   ROOM_NO_MAGIC, ROOM_TUNNEL, ROOM_PRIVATE, ROOM_SAFE, ROOM_SOLITARY, ROOM_PET_SHOP,
   ROOM_NO_RECALL, ROOM_DONATION, ROOM_NODROPALL, ROOM_SILENCE, ROOM_LOGSPEECH, ROOM_NODROP,
   ROOM_BLOODROOM, ROOM_NO_SUMMON, ROOM_NO_ASTRAL, ROOM_TELEPORT, ROOM_CLANSTOREROOM,
   ROOM_NOFLOOR, ROOM_NOSUPPLICATE, ROOM_ARENA, ROOM_NOMISSILE, ROOM_PROTOTYPE, ROOM_DND,
   ROOM_NO_TELEPORT, ROOM_NO_PORTAL, ROOM_BANK, ROOM_NO_HOME, ROOM_NO_SHIFT, ROOM_NO_SLEEP,
   ROOM_NOIMP, ROOM_HADND, ROOM_SAFETY, ROOM_FORGE, ROOM_NOSANC,
   ROOM_NORECALLSET, ROOM_NO_TAP, BFS_MARK, ROOM_NULL, ROOM_NULL2
}
room_flags;

typedef enum
{
   DIR_NORTH, DIR_EAST, DIR_SOUTH, DIR_WEST, DIR_UP, DIR_DOWN,
   DIR_NORTHEAST, DIR_NORTHWEST, DIR_SOUTHEAST, DIR_SOUTHWEST, DIR_SOMEWHERE
}
dir_types;

#define PT_WATER	100
#define PT_AIR		200
#define PT_EARTH	300
#define PT_FIRE		400

typedef enum
{
   PULL_UNDEFINED, PULL_VORTEX, PULL_VACUUM, PULL_SLIP, PULL_ICE, PULL_MYSTERIOUS,
   PULL_CURRENT = PT_WATER, PULL_WAVE, PULL_WHIRLPOOL, PULL_GEYSER,
   PULL_WIND = PT_AIR, PULL_STORM, PULL_COLDWIND, PULL_BREEZE,
   PULL_LANDSLIDE = PT_EARTH, PULL_SINKHOLE, PULL_QUICKSAND, PULL_EARTHQUAKE,
   PULL_LAVA = PT_FIRE, PULL_HOTAIR
}
dir_pulltypes;


#define MAX_DIR			DIR_SOUTHWEST
#define DIR_PORTAL		DIR_SOMEWHERE


#define EX_ISDOOR		  BV00
#define EX_CLOSED		  BV01
#define EX_LOCKED		  BV02
#define EX_SECRET		  BV03
#define EX_SWIM			  BV04
#define EX_PICKPROOF		  BV05
#define EX_FLY			  BV06
#define EX_CLIMB		  BV07
#define EX_DIG			  BV08
#define EX_EATKEY		  BV09
#define EX_NOPASSDOOR		  BV10
#define EX_HIDDEN		  BV11
#define EX_PASSAGE		  BV12
#define EX_PORTAL 		  BV13
#define EX_RES1			  BV14
#define EX_RES2			  BV15
#define EX_xCLIMB		  BV16
#define EX_xENTER		  BV17
#define EX_xLEAVE		  BV18
#define EX_xAUTO		  BV19
#define EX_NOFLEE	  	  BV20
#define EX_xSEARCHABLE		  BV21
#define EX_BASHED                 BV22
#define EX_BASHPROOF              BV23
#define EX_NOMOB		  BV24
#define EX_WINDOW		  BV25
#define EX_xLOOK		  BV26
#define EX_ISBOLT		  BV27
#define EX_BOLTED		  BV28
#define MAX_EXFLAG		  28

typedef enum
{
   SECT_INSIDE, SECT_CITY, SECT_FIELD, SECT_FOREST, SECT_HILLS, SECT_MOUNTAIN,
   SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_UNDERWATER, SECT_AIR, SECT_DESERT,
   SECT_DUNNO, SECT_OCEANFLOOR, SECT_UNDERGROUND, SECT_RUINS, SECT_LAVA, SECT_SWAMP,
   SECT_MAX
}
sector_types;

typedef enum
{
   WEAR_NONE = -1, WEAR_LIGHT = 0, WEAR_FINGER_L, WEAR_FINGER_R, WEAR_NECK_1,
   WEAR_NECK_2, WEAR_BODY, WEAR_HEAD, WEAR_LEGS, WEAR_FEET, WEAR_HANDS,
   WEAR_ARMS, WEAR_SHIELD, WEAR_ABOUT, WEAR_WAIST, WEAR_WRIST_L, WEAR_WRIST_R,
   WEAR_WIELD, WEAR_HOLD, WEAR_DUAL_WIELD, WEAR_EARS, WEAR_EYES,
   WEAR_MISSILE_WIELD, WEAR_BACK, WEAR_FACE, WEAR_ANKLE_L, WEAR_ANKLE_R, WEAR_FLOAT, WEAR_CLAN,
   WEAR_LODGE_RIB, WEAR_LODGE_ARM, WEAR_LODGE_LEG, MAX_WEAR
}
wear_locations;

typedef enum
{ BOARD_NOTE, BOARD_MAIL }
board_types;

#define FLAG_WRAUTH		      1
#define FLAG_AUTH		      2

typedef enum
{
   COND_DRUNK, COND_FULL, COND_THIRST, COND_BLOODTHIRST, MAX_CONDS
}
conditions;

typedef enum
{
   POS_DEAD, POS_MORTAL, POS_INCAP, POS_STUNNED, POS_SLEEPING, POS_BERSERK,
   POS_RESTING, POS_AGGRESSIVE, POS_SITTING, POS_FIGHTING, POS_DEFENSIVE,
   POS_EVASIVE, POS_STANDING, POS_MOUNTED, POS_SHOVE, POS_DRAG
}
positions;

typedef enum
{
   STYLE_BERSERK, STYLE_AGGRESSIVE, STYLE_FIGHTING, STYLE_DEFENSIVE,
   STYLE_EVASIVE,
}
styles;

typedef enum
{
   PLR_IS_NPC, PLR_BOUGHT_PET, PLR_SHOVEDRAG, PLR_AUTOEXIT, PLR_AUTOLOOT,
   PLR_AUTOSAC, PLR_BLANK, PLR_OUTCAST, PLR_BRIEF, PLR_COMBINE, PLR_PROMPT,
   PLR_TELNET_GA, PLR_HOLYLIGHT, PLR_WIZINVIS, PLR_ROOMVNUM, PLR_SILENCE,
   PLR_NO_EMOTE, PLR_ATTACKER, PLR_NO_TELL, PLR_LOG, PLR_DENY, PLR_FREEZE,
   PLR_THIEF, PLR_KILLER, PLR_LITTERBUG, PLR_ANSI, PLR_RIP, PLR_NICE, PLR_FLEE,
   PLR_AUTOGOLD, PLR_AUTOMAP, PLR_AFK, PLR_INVISPROMPT, PLR_IAW, PLR_SCRESHRT,
   PLR_SCREAFF, PLR_A_TITLE, PLR_COMPASS, PLR_QUESTING, PLR_COND,
   PLR_PINGME, PLR_AFFSA, PLR_MIDSCORE, PLR_DEAF, PLR_IDLE, TIED_UP, GAGGED,
   BLINDFOLDED, EXTRA_EXP, EXTRA_DONE, EXTRA_PREGNANT, PLR_BOUNTIED, PLR_BOOSTED, PLR_TRACK
}
player_flags;

#define PCFLAG_R1                  BV00
#define PCFLAG_DEADLY              BV01
#define PCFLAG_UNAUTHED		   BV02
#define PCFLAG_NORECALL            BV03
#define PCFLAG_NOINTRO             BV04
#define PCFLAG_GAG		   BV05
#define PCFLAG_RETIRED             BV06
#define PCFLAG_GUEST               BV07
#define PCFLAG_NOSUMMON		   BV08
#define PCFLAG_PAGERON		   BV09
#define PCFLAG_NOTITLE             BV10
#define PCFLAG_GROUPWHO		   BV11
#define PCFLAG_DIAGNOSE		   BV12
#define PCFLAG_HIGHGAG		   BV13
#define PCFLAG_WATCH		   BV14
#define PCFLAG_HELPSTART	   BV15
#define PCFLAG_DND      	   BV16
#define PCFLAG_IDLE				BV17
#define PCFLAG_AUTOFLAGS		BV18
#define PCFLAG_SECTORD			BV19
#define PCFLAG_ANAME			BV20
#define PCFLAG_NOBEEP		   BV21
#define PCFLAG_PRIVACY		   BV22
#define PCFLAG_POSI 		   BV23
#define PCFLAG_EXP	 		   BV24
#define PCFLAG_BUILDING			BV25
#define PCFLAG_NOBATTLESPAM		BV26
#define PCFLAG_IMMALT			BV27
#define PCFLAG_NOPRETITLE		BV28

typedef enum
{
   TIMER_NONE, TIMER_RECENTFIGHT, TIMER_SHOVEDRAG, TIMER_DO_FUN,
   TIMER_APPLIED, TIMER_PKILLED, TIMER_ASUPRESSED, TIMER_NUISANCE
}
timer_types;

struct timer_data
{
   TIMER *prev;
   TIMER *next;
   DO_FUN *do_fun;
   int value;
   sh_int type;
   int count;
};

typedef enum
{
   CHANNEL_AUCTION, CHANNEL_CHAT, CHANNEL_QUEST, CHANNEL_IMMTALK, CHANNEL_MUSIC,
   CHANNEL_ASK, CHANNEL_SHOUT, CHANNEL_YELL, CHANNEL_MONITOR, CHANNEL_LOG,
   CHANNEL_HIGHGOD, CHANNEL_CLAN, CHANNEL_BUILD, CHANNEL_HIGH, CHANNEL_AVTALK,
   CHANNEL_PRAY, CHANNEL_COUNCIL, CHANNEL_GUILD, CHANNEL_COMM, CHANNEL_TELLS,
   CHANNEL_ORDER, CHANNEL_NEWBIE, CHANNEL_WARTALK, CHANNEL_RACETALK,
   CHANNEL_WARN, CHANNEL_WHISPER, CHANNEL_AUTH, CHANNEL_TRAFFIC, CHANNEL_ADMTALK,
   CHANNEL_INFO, CHANNEL_IC, CHANNEL_FLAME, CHANNEL_HTALK, CHANNEL_NEOTALK, CHANNEL_ACOTALK,
   CHANNEL_HBUILD, CHANNEL_CODER, CHANNEL_NS, CHANNEL_GRATZ, CHANNEL_MARRY
}
channel_bits;


#define AREA_DELETED		   BV00
#define AREA_LOADED                BV01

#define AFLAG_NOPKILL               BV00
#define AFLAG_FREEKILL		    BV01
#define AFLAG_NOTELEPORT	    BV02
#define AFLAG_SPELLLIMIT	    BV03
#define AFLAG_SLEEPDAM		    BV04

struct mob_index_data
{
   MOB_INDEX_DATA *next;
   MOB_INDEX_DATA *next_sort;
   SPEC_FUN *spec_fun;
   SHOP_DATA *pShop;
   REPAIR_DATA *rShop;
   MPROG_DATA *mudprogs;
   EXT_BV progtypes;
   char *player_name;
   char *short_descr;
   char *long_descr;
   char *description;
   int vnum;
   int exptier;
   sh_int count;
   sh_int killed;
   sh_int sex;
   sh_int level;
   EXT_BV act;
   EXT_BV affected_by;
   sh_int alignment;
   sh_int mobthac0;
   sh_int ac;
   sh_int hitnodice;
   sh_int hitsizedice;
   sh_int hitplus;
   sh_int damnodice;
   sh_int damsizedice;
   sh_int damplus;
   sh_int numattacks;
   int gold;
   int silver;
   int copper;
   int exp;
   int xflags;
   int immune;
   int resistant;
   int susceptible;
   EXT_BV attacks;
   EXT_BV defenses;
   int speaks;
   int speaking;
   sh_int position;
   sh_int defposition;
   sh_int height;
   sh_int weight;
   sh_int race;
   sh_int class;
   int hitroll;
   int damroll;
   sh_int perm_str;
   sh_int perm_int;
   sh_int perm_wis;
   sh_int perm_dex;
   sh_int perm_con;
   sh_int perm_cha;
   sh_int perm_lck;
   sh_int saving_poison_death;
   sh_int saving_wand;
   sh_int saving_para_petri;
   sh_int saving_breath;
   sh_int saving_spell_staff;
};


struct hunt_hate_fear
{
   char *name;
   CHAR_DATA *who;
};

struct fighting_data
{
   CHAR_DATA *who;
   int xp;
   sh_int align;
   sh_int duration;
   sh_int timeskilled;
};

struct editor_data
{
   sh_int numlines;
   sh_int on_line;
   sh_int size;
   char line[49][81];
};

struct extracted_char_data
{
   EXTRACT_CHAR_DATA *next;
   CHAR_DATA *ch;
   ROOM_INDEX_DATA *room;
   ch_ret retcode;
   bool extract;
};

struct char_data
{
   CHAR_DATA *next;
   CHAR_DATA *prev;
   CHAR_DATA *next_in_room;
   CHAR_DATA *prev_in_room;
   CHAR_DATA *master;
   CHAR_DATA *leader;
   FIGHT_DATA *fighting;
   CHAR_DATA *reply;
   CHAR_DATA *retell;
   CHAR_DATA *switched;
   CHAR_DATA *mount;
   HHF_DATA *hunting;
   HHF_DATA *fearing;
   HHF_DATA *hating;
   SPEC_FUN *spec_fun;
   MPROG_ACT_LIST *mpact;
   CHAR_DATA *challenged;
   CHAR_DATA *betted_on;
   int bet_amt;
   int mpactnum;
   sh_int mpscriptpos;
   MOB_INDEX_DATA *pIndexData;
   DESCRIPTOR_DATA *desc;
   AFFECT_DATA *first_affect;
   AFFECT_DATA *last_affect;
   NOTE_DATA *pnote;
   NOTE_DATA *comments;
   OBJ_DATA *first_carrying;
   OBJ_DATA *last_carrying;
   OBJ_DATA *on;
   ROOM_INDEX_DATA *in_room;
   ROOM_INDEX_DATA *was_in_room;
   PC_DATA *pcdata;
   DO_FUN *last_cmd;
   DO_FUN *prev_cmd;
   void *dest_buf;
   char *alloc_ptr;
   void *spare_ptr;
   int tempnum;
   EDITOR_DATA *editor;
   TIMER *first_timer;
   TIMER *last_timer;
   CHAR_MORPH *morph;
   char *name;
   char *short_descr;
   char *long_descr;
   char *description;
   sh_int num_fighting;
   sh_int substate;
   sh_int sex;
   sh_int class;
   sh_int dualclass;
   sh_int race;
   sh_int level;
   sh_int sublevel;
   sh_int wlevel;
   sh_int trust;
   sh_int tmplevel;
   sh_int tmptrust;
   int played;
   time_t logon;
   time_t save_time;
   sh_int timer;
   sh_int wait;
   int subtype;
   int hit;
   int max_hit;
   int mana;
   int max_mana;
   int blood;
   int max_blood;
   int move;
   int max_move;
   sh_int practice;
   sh_int numattacks;
   int gold;
   int silver;
   int copper;
   int exp;
   int exptier;
   EXT_BV act;
   EXT_BV affected_by;
   EXT_BV no_affected_by;
   int carry_weight;
   int carry_number;
   int xflags;
   int no_immune;
   int no_resistant;
   int no_susceptible;
   int immune;
   int resistant;
   int susceptible;
   EXT_BV attacks;
   EXT_BV defenses;
   int speaks;
   int speaking;
   sh_int saving_poison_death;
   sh_int saving_wand;
   sh_int saving_para_petri;
   sh_int saving_breath;
   sh_int saving_spell_staff;
   sh_int alignment;
   sh_int barenumdie;
   sh_int baresizedie;
   sh_int mobthac0;
   int hitroll;
   int damroll;
   sh_int hitplus;
   sh_int damplus;
   sh_int position;
   sh_int defposition;
   sh_int style;
   sh_int height;
   sh_int weight;
   int armor;
   sh_int wimpy;
   EXT_BV deaf;
   sh_int perm_str;
   sh_int perm_int;
   sh_int perm_wis;
   sh_int perm_dex;
   sh_int perm_con;
   sh_int perm_cha;
   sh_int perm_lck;
   sh_int mod_str;
   sh_int mod_int;
   sh_int mod_wis;
   sh_int mod_dex;
   sh_int mod_con;
   sh_int mod_cha;
   sh_int mod_lck;
   sh_int mental_state;
   sh_int emotional_state;
   int pagelen;
   sh_int inter_page;
   sh_int inter_type;
   char *inter_editing;
   int inter_editing_vnum;
   sh_int inter_substate;
   int retran;
   int regoto;
   sh_int mobinvis;
   int mob_serial;
   sh_int cmd_recurse;
   int premeeting_vnum;
   bool voted;
   char *site;
   char *created;

};


struct killed_data
{
   int vnum;
   char count;
};

struct ignore_data
{
   IGNORE_DATA *next;
   IGNORE_DATA *prev;
   char *name;
};

#define MAX_IGN		6


struct pc_data
{
   CHAR_DATA *pet;
   CHAR_DATA *monitor;
   bool monitoring;
   CLAN_DATA *clan;
   COUNCIL_DATA *council;
   AREA_DATA *area;
   DEITY_DATA *deity;
   char *homepage;
   char *clan_name;
   int clan_c;
   char *council_name;
   char *deity_name;
   char *pwd;
   char *bamfin;
   char *bamfout;
   char *filename;
   int rank;
   char *sname;
   char *pretit;
   int pretiti;
   char *title;
   char *movein;
   char *moveout;
   char *bestowments;
   int flags;
   int wasat;
   int pkills;
   int pdeaths;
   int mkills;
   int mdeaths;
   int illegal_pk;
   int damhigh;
   long int outcast_time;
   NUISANCE_DATA *nuisance;

   long int restore_time;
   sh_int restore_number;
   long int silence_time;
   int r_range_lo;
   int r_range_hi;
   int m_range_lo;
   int m_range_hi;
   int o_range_lo;
   int o_range_hi;
   sh_int wizinvis;
   sh_int min_snoop;
   sh_int condition[MAX_CONDS];
   sh_int learned[MAX_SKILL];
   KILLED_DATA killed[MAX_KILLTRACK];
   sh_int quest_number;
   sh_int favor;
   sh_int charmies;
   int auth_state;
   time_t silence_release_date;
   time_t release_date;
   char *helled_by;
   char *bio;
   char *authed_by;
   SKILLTYPE *special_skills[MAX_PERSONAL];
   char *prompt;
   char *subprompt;
   sh_int pagerlen;
   bool openedtourney;
   IGNORE_DATA *first_ignored;
   IGNORE_DATA *last_ignored;
   char **tell_history;
   sh_int lt_index;

   long imc_deaf;
   long imc_allow;
   long imc_deny;
   char *rreply;
   char *rreply_name;
   char *ice_listen;

   int balance;
   int balance2;
   int balance3;

   int bounty;
   int bountytype;

   char *spouse;
   int recall;

   sh_int colorize[AT_MAXCOLOR];

   long int last_read_news;

   char *email;
   int icq;
   char *aol;
   char *yahoo;

   int wizshow;

   GLOBAL_BOARD_DATA *board;
   time_t last_note[MAX_BOARD];
   NOTE_DATA *in_progress;

   int quest_curr;
   int quest_accum;
   CHAR_DATA *questgiver;
   int questpoints;
   int questinroom;
   int nextquest;
   int countdown;
   int questobj;
   int questmob;
   char *alias[MAX_ALIAS];
   char *alias_sub[MAX_ALIAS];
   int genes[10];
   sh_int stage[3];
   char *conception;
   char *parents;
   char *cparents;
   int rent;
   bool norares;
   bool autorent;
   char *tracking;

   int techpnt;
};



#define LIQ_WATER        0
#define LIQ_MAX		18

struct liq_type
{
   char *liq_name;
   char *liq_color;
   sh_int liq_affect[3];
};

/*
typedef enum
{
   DAM_HIT, DAM_SLICE, DAM_STAB, DAM_SLASH, DAM_WHIP, DAM_CLAW,
   DAM_BLAST, DAM_POUND, DAM_CRUSH, DAM_GREP, DAM_BITE, DAM_PIERCE,
   DAM_SUCTION, DAM_BOLT, DAM_ARROW, DAM_DART, DAM_STONE, DAM_PEA
} damage_types;
*/
typedef enum
{
   DAM_HIT, DAM_SLASH, DAM_STAB, DAM_HACK, DAM_CRUSH, DAM_LASH,
   DAM_PIERCE, DAM_THRUST, DAM_FLAMING_ARROW, DAM_PIERCING_ARROW,
   DAM_BARBED_ARROW, DAM_SHADOW_ARROW, DAM_CAMO_ARROW, DAM_STANDARD_ARROW,
   DAM_MAX_TYPE
}
damage_types;

typedef enum
{
   WEP_BAREHAND, WEP_SWORD, WEP_DAGGER, WEP_WHIP, WEP_TALON, WEP_MACE,
   WEP_ARCHERY, WEP_BLOWGUN, WEP_SLING, WEP_AXE, WEP_SPEAR, WEP_STAFF,
   WEP_ENERGY, WEP_KATANA,
   WEP_MAX
}
weapon_types;

typedef enum
{
   PROJ_BOLT, PROJ_ARROW, PROJ_DART, PROJ_STONE, PROJ_MAX
}
projectile_types;


struct extra_descr_data
{
   EXTRA_DESCR_DATA *next;
   EXTRA_DESCR_DATA *prev;
   char *keyword;
   char *description;
};



struct obj_index_data
{
   OBJ_INDEX_DATA *next;
   OBJ_INDEX_DATA *next_sort;
   EXTRA_DESCR_DATA *first_extradesc;
   EXTRA_DESCR_DATA *last_extradesc;
   AFFECT_DATA *first_affect;
   AFFECT_DATA *last_affect;
   MPROG_DATA *mudprogs;
   EXT_BV progtypes;
   char *name;
   char *short_descr;
   char *description;
   char *action_desc;
   int vnum;
   sh_int level;
   sh_int item_type;
   EXT_BV extra_flags;
   int magic_flags;
   int wear_flags;
   sh_int count;
   sh_int weight;
   int cost;
   int gold_cost;
   int silver_cost;
   int copper_cost;
   int value[8];
   int serial;
   sh_int layers;
   int rent;
};


struct obj_data
{
   OBJ_DATA *next;
   OBJ_DATA *prev;
   OBJ_DATA *next_content;
   OBJ_DATA *prev_content;
   OBJ_DATA *first_content;
   OBJ_DATA *last_content;
   OBJ_DATA *in_obj;
   CHAR_DATA *carried_by;
   EXTRA_DESCR_DATA *first_extradesc;
   EXTRA_DESCR_DATA *last_extradesc;
   AFFECT_DATA *first_affect;
   AFFECT_DATA *last_affect;
   OBJ_INDEX_DATA *pIndexData;
   ROOM_INDEX_DATA *in_room;
   char *name;
   char *owner;
   char *short_descr;
   char *description;
   char *action_desc;
   sh_int item_type;
   sh_int mpscriptpos;
   EXT_BV extra_flags;
   int magic_flags;
   int wear_flags;
   MPROG_ACT_LIST *mpact;
   int mpactnum;
   sh_int wear_loc;
   sh_int weight;
   int cost;
   int gold_cost;
   int silver_cost;
   int copper_cost;
   sh_int level;
   sh_int timer;
   int value[8];
   sh_int count;
   int serial;
};


struct exit_data
{
   EXIT_DATA *prev;
   EXIT_DATA *next;
   EXIT_DATA *rexit;
   ROOM_INDEX_DATA *to_room;
   char *keyword;
   char *description;
   int vnum;
   int rvnum;
   int exit_info;
   int key;
   sh_int vdir;
   sh_int distance;
   sh_int pull;
   sh_int pulltype;
};



struct reset_data
{
   RESET_DATA *next;
   RESET_DATA *prev;
   char command;
   int extra;
   int arg1;
   int arg2;
   int arg3;
   int reset_serial;
};

#define	BIT_RESET_DOOR			0
#define BIT_RESET_OBJECT		1
#define BIT_RESET_MOBILE		2
#define BIT_RESET_ROOM			3
#define BIT_RESET_TYPE_MASK		0xFF
#define BIT_RESET_DOOR_THRESHOLD	8
#define BIT_RESET_DOOR_MASK		0xFF00
#define BIT_RESET_SET			BV30
#define BIT_RESET_TOGGLE		BV31
#define BIT_RESET_FREEBITS	  0x3FFF0000



struct area_data
{
   AREA_DATA *next;
   AREA_DATA *prev;
   AREA_DATA *next_sort;
   AREA_DATA *prev_sort;
   AREA_DATA *next_sort_name;
   AREA_DATA *prev_sort_name;
   RESET_DATA *first_reset;
   RESET_DATA *last_reset;
   char *name;
   char *filename;
   int flags;
   sh_int status;
   sh_int age;
   sh_int nplayer;
   sh_int reset_frequency;
   int low_r_vnum;
   int hi_r_vnum;
   int low_o_vnum;
   int hi_o_vnum;
   int low_m_vnum;
   int hi_m_vnum;
   int low_soft_range;
   int hi_soft_range;
   int low_hard_range;
   int hi_hard_range;
   int spelllimit;
   int curr_spell_count;
   char *author;
   char *resetmsg;
   int plane;
   RESET_DATA *last_mob_reset;
   RESET_DATA *last_obj_reset;
   sh_int max_players;
   int mkills;
   int mdeaths;
   int pkills;
   int pdeaths;
   int gold_looted;
   int silver_looted;
   int copper_looted;
   int illegal_pk;
   int high_economy;
   int low_economy;
   WEATHER_DATA *weather;
};



struct godlist_data
{
   GOD_DATA *next;
   GOD_DATA *prev;
   int level;
   int low_r_vnum;
   int hi_r_vnum;
   int low_o_vnum;
   int hi_o_vnum;
   int low_m_vnum;
   int hi_m_vnum;
};


struct system_data
{
   char *lastcommand;
   char *impmsg;
   int max_hp;
   int max_mana;
   int max_move;
   int max_prac;
   int max_glory;
   sh_int mset_auto;
   sh_int imp;
   sh_int dsq;
   sh_int kaura;
   int bsql;
   sh_int dmud;
   sh_int debug;
   sh_int manualb;
   bool warmboot;
   sh_int cver;
   sh_int fight_lock;
   sh_int trans_lock;
   sh_int pkill_lock;
   sh_int spell_lock;
   int maxplayers;
   int alltimemax;
   int global_gold_looted;
   int global_silver_looted;
   int global_copper_looted;
   int upill_val;
   int upotion_val;
   int brewed_used;
   int scribed_used;
   char *salltime;
   char *time_of_max;
   char *mud_name;
   bool NO_NAME_RESOLVING;
   bool DENY_NEW_PLAYERS;
   bool WAIT_FOR_AUTH;
   sh_int read_all_mail;
   sh_int read_mail_free;
   sh_int write_mail_free;
   sh_int take_others_mail;
   int imc_mail_vnum;
   sh_int imc_mail_level;
   sh_int muse_level;
   sh_int think_level;
   sh_int build_level;
   sh_int log_level;
   sh_int level_modify_proto;
   sh_int level_override_private;
   sh_int level_mset_player;
   sh_int bash_plr_vs_plr;
   sh_int bash_nontank;
   sh_int gouge_plr_vs_plr;
   sh_int gouge_nontank;
   sh_int stun_plr_vs_plr;
   sh_int stun_regular;
   sh_int dodge_mod;
   sh_int parry_mod;
   sh_int tumble_mod;
   sh_int dam_plr_vs_plr;
   sh_int dam_plr_vs_mob;
   sh_int dam_mob_vs_plr;
   sh_int dam_mob_vs_mob;
   sh_int level_getobjnotake;
   sh_int level_forcepc;
   sh_int bestow_dif;
   sh_int max_sn;
   char *guild_overseer;
   char *guild_advisor;
   int save_flags;
   sh_int save_frequency;
   sh_int check_imm_host;
   sh_int morph_opt;
   sh_int save_pets;
   sh_int ban_site_level;
   sh_int ban_class_level;
   sh_int ban_race_level;
   sh_int ident_retries;
   sh_int immune_to_idle;
   sh_int pk_loot;
   sh_int newbie_purge;
   sh_int regular_purge;
   bool CLEANPFILES;
   bool RENT;
   int exp1;
   int exp2;
   int exp3;
   int exp4;
   int exp5;
   int exp6;
   int exp7;
   int exp8;
   int exp9;
   bool profanityfilter;
   char *rname;
   bool showhitgains;
};


struct room_index_data
{
   ROOM_INDEX_DATA *next;
   ROOM_INDEX_DATA *next_sort;
   CHAR_DATA *first_person;
   CHAR_DATA *last_person;
   OBJ_DATA *first_content;
   OBJ_DATA *last_content;
   EXTRA_DESCR_DATA *first_extradesc;
   EXTRA_DESCR_DATA *last_extradesc;
   AREA_DATA *area;
   EXIT_DATA *first_exit;
   EXIT_DATA *last_exit;
   AFFECT_DATA *first_affect;
   AFFECT_DATA *last_affect;
   MAP_DATA *map;
   PLANE_DATA *plane;
   MPROG_ACT_LIST *mpact;
   int mpactnum;
   MPROG_DATA *mudprogs;
   sh_int mpscriptpos;
   char *name;
   char *description;
   int vnum;
   EXT_BV room_flags;
   EXT_BV progtypes;
   sh_int light;
   sh_int sector_type;
   int tele_vnum;
   sh_int tele_delay;
   sh_int tunnel;
};

struct teleport_data
{
   TELEPORT_DATA *next;
   TELEPORT_DATA *prev;
   ROOM_INDEX_DATA *room;
   sh_int timer;
};


#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000
#define TYPE_HERB		     2000
#define TYPE_PERSONAL		     3000
#define TYPE_RACIAL		     4000
#define TYPE_DISEASE		     5000

typedef enum
{
   TAR_IGNORE, TAR_CHAR_OFFENSIVE, TAR_CHAR_DEFENSIVE, TAR_CHAR_SELF,
   TAR_OBJ_INV,
}
target_types;

typedef enum
{
   SKILL_UNKNOWN, SKILL_SPELL, SKILL_SKILL, SKILL_WEAPON, SKILL_SONG, SKILL_TONGUE,
   SKILL_HERB, SKILL_RACIAL, SKILL_DISEASE
}
skill_types;



struct timerset
{
   int num_uses;
   struct timeval total_time;
   struct timeval min_time;
   struct timeval max_time;
};



struct skill_type
{
   char *name;
   sh_int skill_level[MAX_CLASS];
   sh_int skill_slevel[MAX_CLASS];
   sh_int skill_adept[MAX_CLASS];
   sh_int race_level[MAX_RACE];
   sh_int race_adept[MAX_RACE];
   SPELL_FUN *spell_fun;
   DO_FUN *skill_fun;
   sh_int target;
   sh_int minimum_position;
   sh_int slot;
   sh_int min_mana;
   sh_int beats;
   char *noun_damage;
   char *msg_off;
   sh_int guild;
   sh_int min_level;
   sh_int type;
   sh_int range;
   int info;
   int flags;
   char *hit_char;
   char *hit_vict;
   char *hit_room;
   char *hit_dest;
   char *miss_char;
   char *miss_vict;
   char *miss_room;
   char *die_char;
   char *die_vict;
   char *die_room;
   char *imm_char;
   char *imm_vict;
   char *imm_room;
   char *dice;
   int value;
   int spell_sector;
   char saves;
   char difficulty;
   ACADIA_AFF *affects;
   char *components;
   char *teachers;
   char participants;
   struct timerset userec;
};


#define AUCTION_MEM 3

struct auction_data
{
   OBJ_DATA *item;
   CHAR_DATA *seller;
   CHAR_DATA *buyer;
   int bet;
   sh_int going;
   sh_int pulse;
   int starting;
   OBJ_INDEX_DATA *history[AUCTION_MEM];
   sh_int hist_timer;
};

struct web_descriptor
{
   int fd;
   char request[2048];
   struct sockaddr_in *their_addr;
   int sin_size;
   WEB_DESCRIPTOR *next;
   WEB_DESCRIPTOR *prev;
   bool valid;
};

extern int port;
extern sh_int gsn_style_evasive;
extern sh_int gsn_style_defensive;
extern sh_int gsn_style_standard;
extern sh_int gsn_style_aggressive;
extern sh_int gsn_style_berserk;
extern sh_int gsn_shapeshift;
extern sh_int gsn_sharpen;
extern sh_int gsn_detrap;
extern sh_int gsn_backstab;
extern sh_int gsn_circle;
extern sh_int gsn_cook;
extern sh_int gsn_dodge;
extern sh_int gsn_alertness;
extern sh_int gsn_hide;
extern sh_int gsn_movehide;
extern sh_int gsn_peek;
extern sh_int gsn_pick_lock;
extern sh_int gsn_longsight;
extern sh_int gsn_godspell;
extern sh_int gsn_sneak;
extern sh_int gsn_fast_healing;
extern sh_int gsn_fast_healing2;
extern sh_int gsn_fast_healing3;
extern sh_int gsn_fast_healing4;
extern sh_int gsn_fast_healing5;
extern sh_int gsn_meditation;
extern sh_int gsn_meditation2;
extern sh_int gsn_meditation3;
extern sh_int gsn_meditation4;
extern sh_int gsn_meditation5;
extern sh_int gsn_steal;
extern sh_int gsn_heighten;
extern sh_int gsn_force_throw;
extern sh_int gsn_force_push;
extern sh_int gsn_gouge;
extern sh_int gsn_search;
extern sh_int gsn_dig;
extern sh_int gsn_mount;
extern sh_int gsn_bashdoor;
extern sh_int gsn_berserk;
extern sh_int gsn_hitall;
extern sh_int gsn_kyahu;

extern sh_int gsn_tachi_kashi;
extern sh_int gsn_tachi_jinpu;
extern sh_int gsn_tachi_gekko;
extern sh_int gsn_tachi_enpi;
extern sh_int gsn_tachi_koki;
extern sh_int gsn_tachi_haboku;
extern sh_int gsn_tachi_kagero;

extern sh_int gsn_fclaw;
extern sh_int gsn_disarm;
extern sh_int gsn_enhanced_damage;
extern sh_int gsn_throw;
extern sh_int gsn_kick;
extern sh_int gsn_firekick;
extern sh_int gsn_icekick;
extern sh_int gsn_lightningkick;
extern sh_int gsn_guard;
extern sh_int gsn_parry;
extern sh_int gsn_rescue;
extern sh_int gsn_second_attack;
extern sh_int gsn_third_attack;
extern sh_int gsn_fourth_attack;
extern sh_int gsn_fifth_attack;
extern sh_int gsn_sixth_attack;
extern sh_int gsn_dual_wield;

extern sh_int gsn_flaming_arrow;
extern sh_int gsn_barbed_arrow;
extern sh_int gsn_shadow_arrow;
extern sh_int gsn_piercing_arrow;
extern sh_int gsn_camouflaged_arrow;
extern sh_int gsn_dodge_craft;

extern sh_int gsn_feed;
extern sh_int gsn_bloodlet;
extern sh_int gsn_broach;
extern sh_int gsn_mistwalk;

extern sh_int gsn_aid;

extern sh_int gsn_heal_ii;
extern sh_int gsn_heal_iii;
extern sh_int gsn_heal_iv;
extern sh_int gsn_heal_v;

extern sh_int gsn_de;
extern sh_int gsn_di;
extern sh_int gsn_dh;
extern sh_int gsn_dm;
extern sh_int gsn_dt;
extern sh_int gsn_if;
extern sh_int gsn_sc;
extern sh_int gsn_ts;

extern sh_int gsn_first_spell;
extern sh_int gsn_first_skill;
extern sh_int gsn_first_weapon;
extern sh_int gsn_first_song;
extern sh_int gsn_first_tongue;
extern sh_int gsn_top_sn;

extern sh_int gsn_blindness;
extern sh_int gsn_charm_person;
extern sh_int gsn_jmt;
extern sh_int gsn_aqua_breath;
extern sh_int gsn_curse;
extern sh_int gsn_psidance;
extern sh_int gsn_holyaura;
extern sh_int gsn_karaaura;
extern sh_int gsn_purity;
extern sh_int gsn_change_sex;
extern sh_int gsn_invis;
extern sh_int gsn_impinvis;
extern sh_int gsn_mass_invis;
extern sh_int gsn_plague;
extern sh_int gsn_poison;
extern sh_int gsn_blackflame;
extern sh_int gsn_dangle;
extern sh_int gsn_delirium;
extern sh_int gsn_sleep;
extern sh_int gsn_possess;
extern sh_int gsn_demonfire;
extern sh_int gsn_demi;
extern sh_int gsn_fireball;
extern sh_int gsn_chill_touch;
extern sh_int gsn_lightning_bolt;

extern sh_int gsn_makepotion;
extern sh_int gsn_hadesarmor;
extern sh_int gsn_armor;
extern sh_int gsn_protect;
extern sh_int gsn_bless;
extern sh_int gsn_frenzy;
extern sh_int gsn_sanctuary;
extern sh_int gsn_calcify;
extern sh_int gsn_displacement;
extern sh_int gsn_passdoor;
extern sh_int gsn_flying;
extern sh_int gsn_fireshield;
extern sh_int gsn_iceshield;
extern sh_int gsn_shockshield;
extern sh_int gsn_biofeedback;
extern sh_int gsn_battleplan;
extern sh_int gsn_quickness;

extern sh_int gsn_punch;
extern sh_int gsn_bash;
extern sh_int gsn_stun;
extern sh_int gsn_bite;
extern sh_int gsn_claw;
extern sh_int gsn_sting;
extern sh_int gsn_tail;

extern sh_int gsn_assassinate;
extern sh_int gsn_poison_weapon;
extern sh_int gsn_scribe;
extern sh_int gsn_brew;
extern sh_int gsn_climb;
extern sh_int gsn_track;

extern sh_int gsn_clight;
extern sh_int gsn_cureplague;
extern sh_int gsn_remcurse;
extern sh_int gsn_niaver;

extern sh_int gsn_lore;
extern sh_int gsn_elem_lore;
extern sh_int gsn_magic_lore;
extern sh_int gsn_weapon_lore;
extern sh_int gsn_armor_lore;
extern sh_int gsn_force_lore;
extern sh_int gsn_forest_lore;
extern sh_int gsn_map_lore;

extern sh_int gsn_swords;
extern sh_int gsn_katana;
extern sh_int gsn_daggers;
extern sh_int gsn_whips;
extern sh_int gsn_maces_hammers;
extern sh_int gsn_blowguns;
extern sh_int gsn_slings;
extern sh_int gsn_axes;
extern sh_int gsn_spears;
extern sh_int gsn_staves;
extern sh_int gsn_energy;
extern sh_int gsn_archery;

extern sh_int gsn_shieldwork;

extern sh_int gsn_grip;
extern sh_int gsn_slice;
extern sh_int gsn_sith;

extern sh_int gsn_tumble;

extern sh_int gsn_common;
extern sh_int gsn_elven;
extern sh_int gsn_dwarven;
extern sh_int gsn_pixie;
extern sh_int gsn_ogre;
extern sh_int gsn_orcish;
extern sh_int gsn_trollish;
extern sh_int gsn_goblin;
extern sh_int gsn_halfling;

extern char *const cmd_flags[];

#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))


#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)	((var) ^= (bit))
#define CH(d)                  ((d)->original ? (d)->original : (d)->character)

bool ext_is_empty args( ( EXT_BV * bits ) );
void ext_clear_bits args( ( EXT_BV * bits ) );
int ext_has_bits args( ( EXT_BV * var, EXT_BV * bits ) );
bool ext_same_bits args( ( EXT_BV * var, EXT_BV * bits ) );
void ext_set_bits args( ( EXT_BV * var, EXT_BV * bits ) );
void ext_remove_bits args( ( EXT_BV * var, EXT_BV * bits ) );
void ext_toggle_bits args( ( EXT_BV * var, EXT_BV * bits ) );

int get_value args( ( int gval, int sval, int cval ) );
void conv_currency args( ( CHAR_DATA * ch, int tmpvalue ) );



#define xIS_SET(var, bit)	((var).bits[(bit) >> RSV] & 1 << ((bit) & XBM))
#define xSET_BIT(var, bit)	((var).bits[(bit) >> RSV] |= 1 << ((bit) & XBM))
#define xSET_BITS(var, bit)	(ext_set_bits(&(var), &(bit)))
#define xREMOVE_BIT(var, bit)	((var).bits[(bit) >> RSV] &= ~(1 << ((bit) & XBM)))
#define xREMOVE_BITS(var, bit)	(ext_remove_bits(&(var), &(bit)))
#define xTOGGLE_BIT(var, bit)	((var).bits[(bit) >> RSV] ^= 1 << ((bit) & XBM))
#define xTOGGLE_BITS(var, bit)	(ext_toggle_bits(&(var), &(bit)))
#define xCLEAR_BITS(var)	(ext_clear_bits(&(var)))
#define xIS_EMPTY(var)		(ext_is_empty(&(var)))
#define xHAS_BITS(var, bit)	(ext_has_bits(&(var), &(bit)))
#define xSAME_BITS(var, bit)	(ext_same_bits(&(var), &(bit)))


/*
#define CREATE(result, type, number)				\
do								\
{								\
    if (!((result) = (type *) calloc ((number), sizeof(type))))	\
    {								\
	perror("malloc failure");				\
	fprintf(stderr, "Malloc failure @ %s:%d\n", __FILE__, __LINE__ ); \
	abort();						\
    }								\
} while(0)

#define RECREATE(result,type,number)				\
do								\
{								\
    if (!((result) = (type *) realloc ((result), sizeof(type) * (number))))\
    {								\
	perror("realloc failure");				\
	fprintf(stderr, "Realloc failure @ %s:%d\n", __FILE__, __LINE__ ); \
	abort();						\
    }								\
} while(0)


#define DISPOSE(point) 						\
do								\
{								\
  if (!(point))							\
  {								\
	bug( "Freeing null pointer %s:%d", __FILE__, __LINE__ ); \
	fprintf( stderr, "DISPOSEing NULL in %s, line %d\n", __FILE__, __LINE__ ); \
  }								\
  else free(point);						\
  point = NULL;							\
} while(0)

#ifdef HASHSTR
#define STRALLOC(point)		str_alloc((point))
#define QUICKLINK(point)	quick_link((point))
#define QUICKMATCH(p1, p2)	(int) (p1) == (int) (p2)
#define STRFREE(point)						\
do								\
{								\
  if (!(point))							\
  {								\
	bug( "Freeing null pointer %s:%d", __FILE__, __LINE__ ); \
	fprintf( stderr, "STRFREEing NULL in %s, line %d\n", __FILE__, __LINE__ ); \
  }								\
  else if (str_free((point))==-1) 				\
    fprintf( stderr, "STRFREEing bad pointer in %s, line %d\n", __FILE__, __LINE__ ); \
  point = NULL;							\
} while(0)
#else
#define STRALLOC(point)		str_dup((point))
#define QUICKLINK(point)	str_dup((point))
#define QUICKMATCH(p1, p2)	strcmp((p1), (p2)) == 0
#define STRFREE(point)						\
do								\
{								\
  if (!(point))							\
  {								\
	bug( "Freeing null pointer %s:%d", __FILE__, __LINE__ ); \
	fprintf( stderr, "STRFREEing NULL in %s, line %d\n", __FILE__, __LINE__ ); \
  }								\
  else free((point));						\
  point = NULL;							\
} while(0)
#endif


#define LINK(link, first, last, next, prev)			\
do								\
{								\
    if ( !(first) )						\
      (first)			= (link);			\
    else							\
      (last)->next		= (link);			\
    (link)->next		= NULL;				\
    (link)->prev		= (last);			\
    (last)			= (link);			\
} while(0)

#define INSERT(link, insert, first, next, prev)			\
do								\
{								\
    (link)->prev		= (insert)->prev;		\
    if ( !(insert)->prev )					\
      (first)			= (link);			\
    else							\
      (insert)->prev->next	= (link);			\
    (insert)->prev		= (link);			\
    (link)->next		= (insert);			\
} while(0)

#define UNLINK(link, first, last, next, prev)			\
do								\
{								\
    if ( !(link)->prev )					\
      (first)			= (link)->next;			\
    else							\
      (link)->prev->next	= (link)->next;			\
    if ( !(link)->next )					\
      (last)			= (link)->prev;			\
    else							\
      (link)->next->prev	= (link)->prev;			\
} while(0)
*/


#define CREATE(result, type, number)			                		\
do									            	                	\
{											                            \
    if (!((result) = (type *) MudCalloc ((number), sizeof(type), 1)))	        \
    {											                        \
	perror("malloc failure");						                    \
	fprintf(stderr, "Malloc failure @ %s:%d\n", __FILE__, __LINE__ );   \
	abort();									                        \
    }											                        \
} while(0)
/*
#define CREATE(result, type, number)			                	\
if (!((result) = (type *) MudCalloc ((number), sizeof(type))))	        \
{											                        \
  perror("malloc failure");						                    \
  fprintf(stderr, "Malloc failure @ %s:%d\n", __FILE__, __LINE__ ); \
  abort();									                        \
}
*/
#define RECREATE(result,type,number)					\
do											\
{											\
    if (!((result) = (type *) MudRealloc ((result), sizeof(type) * (number), 3)))\
    {											\
	perror("realloc failure");						\
	fprintf(stderr, "Realloc failure @ %s:%d\n", __FILE__, __LINE__ ); \
	abort();									\
    }											\
} while(0)


#define DISPOSE(point) 								\
do											\
{											\
  if (!(point))									\
  {											\
	bug( "Freeing null pointer %s:%d", __FILE__, __LINE__ ); 	\
	fprintf( stderr, "DISPOSEing NULL in %s, line %d\n", __FILE__, __LINE__ ); \
  }											\
  else										\
  {											\
     MudFree((point), 5);								\
     (point) = NULL;								\
  }											\
} while(0)

#ifdef HASHSTR
#define STRALLOC(point)		str_alloc((point))
#define QUICKLINK(point)	quick_link((point))
#define QUICKMATCH(p1, p2)	(int) (p1) == (int) (p2)
#define STRFREE(point)								\
do											\
{											\
  if (!(point))									\
  {											\
	bug( "Freeing null pointer %s:%d", __FILE__, __LINE__ ); 	\
	fprintf( stderr, "STRFREEing NULL in %s, line %d\n", __FILE__, __LINE__ ); \
  }											\
  else										\
  {											\
    if (str_free((point))==-1) 						\
       fprintf( stderr, "STRFREEing bad pointer in %s, line %d\n", __FILE__, __LINE__ ); \
    (point) = NULL;									\
  }											\
} while(0)
#else
#define STRALLOC(point)		str_dup((point))
#define QUICKLINK(point)	str_dup((point))
#define QUICKMATCH(p1, p2)	strcmp((p1), (p2)) == 0
#define STRFREE(point)								\
do											\
{											\
  if (!(point))									\
  {											\
	bug( "Freeing null pointer %s:%d", __FILE__, __LINE__ ); 	\
	fprintf( stderr, "STRFREEing NULL in %s, line %d\n", __FILE__, __LINE__ ); \
  }											\
  else										\
  {											\
     MudFree((point), 6);								\
     (point) = NULL;								\
  }											\
} while(0)
#endif

#define LINK(link, first, last, next, prev)                     	\
do                                                              	\
{                                                               	\
   if ( !(first) )								\
   {                                           				\
      (first) = (link);				                       	\
      (last) = (link);							    	\
   }											\
   else                                                      	\
      (last)->next = (link);			                       	\
   (link)->next = NULL;			                         	\
   if (first == link)								\
      (link)->prev = NULL;							\
   else										\
      (link)->prev = (last);			                       	\
   (last) = (link);				                       	\
} while(0)

#define INSERT(link, insert, first, next, prev)                 \
do                                                              \
{                                                               \
   (link)->prev = (insert)->prev;			                \
   if ( !(insert)->prev )                                       \
      (first) = (link);                                         \
   else                                                         \
      (insert)->prev->next = (link);                            \
   (insert)->prev = (link);                                     \
   (link)->next = (insert);                                     \
} while(0)

#define UNLINK(link, first, last, next, prev)                   	\
do                                                              	\
{                                                               	\
	if ( !(link)->prev )							\
	{			                                    	\
         (first) = (link)->next;			                 	\
	   if ((first))							 	\
	      (first)->prev = NULL;						\
	} 										\
	else										\
	{                                                 		\
         (link)->prev->next = (link)->next;                 	\
	}										\
	if ( !(link)->next ) 							\
	{				                                    \
         (last) = (link)->prev;                 			\
	   if ((last))								\
	      (last)->next = NULL;						\
	} 										\
	else										\
	{                                                    		\
         (link)->next->prev = (link)->prev;                 	\
	}										\
} while(0)

#define CHECK_LINKS(first, last, next, prev, type)		\
do {								\
  type *ptr, *pptr = NULL;					\
  if ( !(first) && !(last) )					\
    break;							\
  if ( !(first) )						\
  {								\
    bug( "CHECK_LINKS: last with NULL first!  %s.",		\
        __STRING(first) );					\
    for ( ptr = (last); ptr->prev; ptr = ptr->prev );		\
    (first) = ptr;						\
  }								\
  else if ( !(last) )						\
  {								\
    bug( "CHECK_LINKS: first with NULL last!  %s.",		\
        __STRING(first) );					\
    for ( ptr = (first); ptr->next; ptr = ptr->next );		\
    (last) = ptr;						\
  }								\
  if ( (first) )						\
  {								\
    for ( ptr = (first); ptr; ptr = ptr->next )			\
    {								\
      if ( ptr->prev != pptr )					\
      {								\
        bug( "CHECK_LINKS(%s): %p:->prev != %p.  Fixing.",	\
            __STRING(first), ptr, pptr );			\
        ptr->prev = pptr;					\
      }								\
      if ( ptr->prev && ptr->prev->next != ptr )		\
      {								\
        bug( "CHECK_LINKS(%s): %p:->prev->next != %p.  Fixing.",\
            __STRING(first), ptr, ptr );			\
        ptr->prev->next = ptr;					\
      }								\
      pptr = ptr;						\
    }								\
    pptr = NULL;						\
  }								\
  if ( (last) )							\
  {								\
    for ( ptr = (last); ptr; ptr = ptr->prev )			\
    {								\
      if ( ptr->next != pptr )					\
      {								\
        bug( "CHECK_LINKS (%s): %p:->next != %p.  Fixing.",	\
            __STRING(first), ptr, pptr );			\
        ptr->next = pptr;					\
      }								\
      if ( ptr->next && ptr->next->prev != ptr )		\
      {								\
        bug( "CHECK_LINKS(%s): %p:->next->prev != %p.  Fixing.",\
            __STRING(first), ptr, ptr );			\
        ptr->next->prev = ptr;					\
      }								\
      pptr = ptr;						\
    }								\
  }								\
} while(0)

#define ASSIGN_GSN(gsn, skill)					\
do								\
{								\
    if ( ((gsn) = skill_lookup((skill))) == -1 )		\
	fprintf( stderr, "ASSIGN_GSN: Skill %s not found.\n",	\
		(skill) );					\
} while(0)

#define CHECK_SUBRESTRICTED(ch)					\
do								\
{								\
    if ( (ch)->substate == SUB_RESTRICTED )			\
    {								\
	send_to_char( "You cannot use this command from within another command.\n\r", ch );	\
	return;							\
    }								\
} while(0)


#define IS_NPC(ch)		(xIS_SET((ch)->act, ACT_IS_NPC))
#define IS_NPURGE(ch)		(xIS_SET((ch)->act, ACT_NOPURGE))
#define IS_SHOP(ch)		(xIS_SET((ch)->act, ACT_SHOP))
#define IS_IMMORTAL(ch)		(get_trust((ch)) >= LEVEL_IMMORTAL)
#define IS_IMM(ch)		(get_trust((ch)) >= LEVEL_VISITOR)
#define IS_HBUILD(ch)		(get_trust((ch)) >= LEVEL_HEADBUILD)
#define IS_BUILD(ch)		(get_trust((ch)) >= LEVEL_BUILD)
#define IS_ANGEL(ch)	(get_trust((ch)) == LEVEL_ANGEL || get_trust((ch)) == LEVEL_ARCHANGEL)
#define IS_AGOD(ch)		(get_trust((ch)) >= LEVEL_IMMORTAL)
#define IS_ADMIN(ch)		(get_trust((ch)) >= LEVEL_SUBADMIN)
#define IS_MAXED(ch)		(get_trust((ch)) == (LEVEL_IMMORTAL - 1))
#define IS_MORTAL(ch)		(get_trust((ch)) < LEVEL_HERO)
#define IS_HERO(ch)		(get_trust((ch)) >= LEVEL_HERO)
#define IS_AVA(ch)		(get_trust((ch)) >= LEVEL_AVATAR)
#define IS_NEO(ch)		(get_trust((ch)) >= LEVEL_NEOPHYTE)
#define IS_ACO(ch)		(get_trust((ch)) >= LEVEL_ACOLYTE)
#define IS_HERO2(ch)		(get_trust((ch)) >= LEVEL_HERO)
#define IS_AVA2(ch)		(get_trust((ch)) >= LEVEL_AVATAR)
#define IS_NEO2(ch)		(get_trust((ch)) >= LEVEL_NEOPHYTE)
#define IS_ACO2(ch)		(get_trust((ch)) >= LEVEL_ACOLYTE)
#define WAS_IMMORTAL(ch)	(!IS_NPC((ch)) && (ch)->tmplevel > 603)
#define IS_AFFECTED(ch, sn)	(xIS_SET((ch)->affected_by, (sn)))
#define HAS_BODYPART(ch, part)	((ch)->xflags == 0 || IS_SET((ch)->xflags, (part)))

#define IS_NOBSPAM(ch) (ch->pcdata && IS_SET(ch->pcdata->flags,PCFLAG_NOBATTLESPAM))

#define IS_MARRIED(ch)		(!IS_NPC((ch)) && (ch)->pcdata->spouse)

#define IS_MALE(ch)		( !IS_NPC(ch) && (ch)->sex == SEX_MALE )

#define CAN_CAST(ch)		((ch)->class != 2 && (ch)->class != 3)

#define IS_VAMPIRE(ch)		( !IS_NPC(ch)			     \
				  && ( (ch)->class==CLASS_VAMPIRE    \
				  || (ch)->dualclass==CLASS_VAMPIRE  \
				  || (ch)->class==CLASS_WEREWOLF     \
				  || (ch)->dualclass==CLASS_WEREWOLF ) )

#define IS_DUAL(ch)	((ch)->dualclass > -1)

#define IS_BLIND(ch) ( IS_NPC(ch) && xIS_SET( (ch)->affected_by, AFF_BLIND ) )

#define IS_READY(ch) ( !IS_NPC(ch) && get_trust(ch) >= 1 )

#define HAS_AWARD(ch)	( IS_AGOD(ch) ? (ch)->wlevel : (ch)->sublevel )
#define HAS_AWARDV(victim)	( IS_AGOD(victim) ? (victim)->wlevel : (victim)->sublevel )

#define IS_GOOD(ch)		((ch)->alignment >= 350)
#define IS_EVIL(ch)		((ch)->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		((ch)->position > POS_SLEEPING)
#define GET_AC(ch)		((ch)->armor				    \
				    + ( IS_AWAKE(ch)			    \
				    ? dex_app[get_curr_dex(ch)].defensive   \
				    : 0 )				    \
				    + VAMP_AC(ch))
#define GET_HITROLL(ch)		((ch)->hitroll				    \
				    +str_app[get_curr_str(ch)].tohit)

#define GET_DAMROLL(ch)		((ch)->damroll                              \
				    +(ch)->damplus			    \
				    +str_app[get_curr_str(ch)].todam)

#define IS_OUTSIDE(ch)		(ch->in_room->sector_type == SECT_CITY || 	           \
							ch->in_room->sector_type == SECT_FIELD ||               \
                            ch->in_room->sector_type == SECT_FOREST ||               \
                            ch->in_room->sector_type == SECT_HILLS ||               \
                            ch->in_room->sector_type == SECT_MOUNTAIN ||               \
                            ch->in_room->sector_type == SECT_WATER_SWIM ||               \
                            ch->in_room->sector_type == SECT_AIR ||               \
                            ch->in_room->sector_type == SECT_DESERT ||               \
                            ch->in_room->sector_type == SECT_SWAMP)


#define WATER_SECT(ch)  (  ch->in_room->sector_type == SECT_WATER_SWIM || 	           \
						ch->in_room->sector_type == SECT_UNDERWATER ||               \
                                  ch->in_room->sector_type == SECT_OCEANFLOOR ||               \
                                  ch->in_room->sector_type == SECT_WATER_NOSWIM )

#define LAVA_SECT(ch)  ( ch->in_room->sector_type == SECT_LAVA )

#define IS_DRUNK(ch, drunk)     (number_percent() < \
			        ( (ch)->pcdata->condition[COND_DRUNK] \
				* 2 / (drunk) ) )

#define IS_CLANNED(ch)		(!IS_NPC((ch))				    \
				&& (ch)->pcdata->clan			    \
				&& (ch)->pcdata->clan->clan_type != CLAN_ORDER  \
				&& (ch)->pcdata->clan->clan_type != CLAN_GUILD)

#define IS_ORDERED(ch)		(!IS_NPC((ch))				    \
				&& (ch)->pcdata->clan			    \
				&& (ch)->pcdata->clan->clan_type == CLAN_ORDER)

#define IS_GUILDED(ch)		(!IS_NPC((ch))				    \
				&& (ch)->pcdata->clan			    \
				&& (ch)->pcdata->clan->clan_type == CLAN_GUILD)

#define IS_DEADLYCLAN(ch)	(!IS_NPC((ch))				    \
				&& (ch)->pcdata->clan			    \
				&& (ch)->pcdata->clan->clan_type != CLAN_NOKILL) \
				&& (ch)->pcdata->clan->clan_type != CLAN_ORDER)  \
				&& (ch)->pcdata->clan->clan_type != CLAN_GUILD)

#define IS_DEVOTED(ch)		(!IS_NPC((ch)) && (ch)->pcdata->deity)

#define IS_IDLE(ch)		(ch->pcdata && IS_SET( ch->pcdata->flags, PCFLAG_IDLE ))
#define IS_PKILL(ch)            (ch->pcdata && IS_SET( ch->pcdata->flags, PCFLAG_DEADLY ))

#define CAN_PKILL(ch)           (IS_PKILL(ch) && ch->level >= 5 && get_age( ch ) >= 18 )


#define WAIT_STATE(ch, npulse) ((ch)->wait=(!IS_NPC(ch)&&ch->pcdata->nuisance&&\
				(ch->pcdata->nuisance->flags>4))?UMAX((ch)->wait,\
                              (npulse+((ch)->pcdata->nuisance->flags-4)+ \
                              ch->pcdata->nuisance->power)): \
                              UMAX((ch)->wait, (IS_IMMORTAL(ch) ? 0 :(npulse))))



#define EXIT(ch, door)		( get_exit( (ch)->in_room, door ) )

#define CAN_GO(ch, door)	(EXIT((ch),(door))			 \
				&& (EXIT((ch),(door))->to_room != NULL)  \
                          	&& !IS_SET(EXIT((ch), (door))->exit_info, EX_CLOSED))

#define IS_FLOATING(ch)		( IS_AFFECTED((ch), AFF_FLYING) || IS_AFFECTED((ch), AFF_FLOATING) )

#define IS_VALID_SN(sn)		( (sn) >=0 && (sn) < MAX_SKILL		     \
				&& skill_table[(sn)]			     \
				&& skill_table[(sn)]->name )

#define IS_VALID_HERB(sn)	( (sn) >=0 && (sn) < MAX_HERB		     \
				&& herb_table[(sn)]			     \
				&& herb_table[(sn)]->name )

#define IS_VALID_DISEASE(sn)	( (sn) >=0 && (sn) < MAX_DISEASE	     \
				&& disease_table[(sn)]			     \
				&& disease_table[(sn)]->name )

#define IS_PACIFIST(ch)		(IS_NPC(ch) && xIS_SET(ch->act, ACT_PACIFIST))

#define IS_BOUNTIED(ch)		(!IS_NPC(ch) && xIS_SET(ch->act, PLR_BOUNTIED))

#define IS_TRACKING(ch)		(!IS_NPC(ch) && xIS_SET(ch->act, PLR_TRACK))

#define SPELL_FLAG(skill, flag)	( IS_SET((skill)->flags, (flag)) )
#define SPELL_DAMAGE(skill)	( ((skill)->info      ) & 7 )
#define SPELL_ACTION(skill)	( ((skill)->info >>  3) & 7 )
#define SPELL_CLASS(skill)	( ((skill)->info >>  6) & 7 )
#define SPELL_POWER(skill)	( ((skill)->info >>  9) & 3 )
#define SPELL_SAVE(skill)	( ((skill)->info >> 11) & 7 )
#define SET_SDAM(skill, val)	( (skill)->info =  ((skill)->info & SDAM_MASK) + ((val) & 7) )
#define SET_SACT(skill, val)	( (skill)->info =  ((skill)->info & SACT_MASK) + (((val) & 7) << 3) )
#define SET_SCLA(skill, val)	( (skill)->info =  ((skill)->info & SCLA_MASK) + (((val) & 7) << 6) )
#define SET_SPOW(skill, val)	( (skill)->info =  ((skill)->info & SPOW_MASK) + (((val) & 3) << 9) )
#define SET_SSAV(skill, val)	( (skill)->info =  ((skill)->info & SSAV_MASK) + (((val) & 7) << 11) )

#define IS_RETIRED(ch) (ch->pcdata && IS_SET(ch->pcdata->flags,PCFLAG_RETIRED))
#define IS_GUEST(ch) (ch->pcdata && IS_SET(ch->pcdata->flags,PCFLAG_GUEST))


#define IS_FIRE(dt)		( IS_VALID_SN(dt) &&			     \
				SPELL_DAMAGE(skill_table[(dt)]) == SD_FIRE )
#define IS_COLD(dt)		( IS_VALID_SN(dt) &&			     \
				SPELL_DAMAGE(skill_table[(dt)]) == SD_COLD )
#define IS_ACID(dt)		( IS_VALID_SN(dt) &&			     \
				SPELL_DAMAGE(skill_table[(dt)]) == SD_ACID )
#define IS_ELECTRICITY(dt)	( IS_VALID_SN(dt) &&			     \
				SPELL_DAMAGE(skill_table[(dt)]) == SD_ELECTRICITY )
#define IS_ENERGY(dt)		( IS_VALID_SN(dt) &&			     \
				SPELL_DAMAGE(skill_table[(dt)]) == SD_ENERGY )

#define IS_DRAIN(dt)		( IS_VALID_SN(dt) &&			     \
				SPELL_DAMAGE(skill_table[(dt)]) == SD_DRAIN )

#define IS_POISON(dt)		( IS_VALID_SN(dt) &&			     \
				SPELL_DAMAGE(skill_table[(dt)]) == SD_POISON )


#define NOT_AUTHED(ch)		(!IS_NPC(ch) && ch->pcdata->auth_state <= 3  \
			      && IS_SET(ch->pcdata->flags, PCFLAG_UNAUTHED) )

#define IS_WAITING_FOR_AUTH(ch) (!IS_NPC(ch) && ch->desc		     \
			      && ch->pcdata->auth_state == 1		     \
			      && IS_SET(ch->pcdata->flags, PCFLAG_UNAUTHED) )

#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(xIS_SET((obj)->extra_flags, (stat)))
#define HAS_STAT(obj)	( !xIS_EMPTY( (obj)->extra_flags ) )

#define HAS_PROG(what, prog)	(xIS_SET((what)->progtypes, (prog)))

#define PERS(ch, looker)	( can_see( (looker), (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
				: (IS_SNAME(ch) ? (ch)->pcdata->sname : (ch)->name )) \
				: (IS_IMMORTAL(ch) ? "An Immortal" : "someone") )

#define GPERS(ch, looker)	( can_see( (looker), (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr : (ch)->name ) \
				: (IS_IMMORTAL(ch) ? "An Immortal" : "someone") )

#define MORPHPERS(ch, looker)   ( can_see( (looker), (ch) ) ?           \
                                (ch)->morph->morph->short_desc       \
                                : "someone" )

#define IS_COE(ch)			( !IS_NPC(ch) && (ch)->pcdata->clan_c == 1 )

#define IS_SNAME(ch)        ( !IS_NPC(ch) && (ch)->pcdata->pretiti == 1 )

#define HAS_VNUM(ch)		( IS_NPC(ch) ? (ch)->pIndexData->vnum : ch->in_room->vnum )

#define GET_BETTED_ON(ch)    ((ch)->betted_on)
#define GET_BET_AMT(ch) ((ch)->bet_amt)
#define IN_ARENA(ch)            (xIS_SET((ch)->in_room->room_flags, ROOM_ARENA))

#define IS_ARM_TYPE(obj) ( obj->item_type == ITEM_ARMOR || obj->item_type == ITEM_FLUTE )

#define log_string(txt)		( log_string_plus( (txt), LOG_NORMAL, LEVEL_LOG ) )
#define dam_message(ch, victim, dam, dt, totalHitsA)	(new_dam_message((ch), (victim), (dam), (dt), (totalHitsA), NULL) )

#define	CMD_FLAG_POSSESS	BV00
#define CMD_FLAG_POLYMORPHED	BV01
#define CMD_WATCH		BV02

struct cmd_type
{
   CMDTYPE *next;
   char *name;
   DO_FUN *do_fun;
   int flags;
   sh_int position;
   sh_int level;
   sh_int log;
   struct timerset userec;
   int lag_count;
   sh_int cshow;
};

struct social_type
{
   SOCIALTYPE *next;
   char *name;
   char *char_no_arg;
   char *others_no_arg;
   char *char_found;
   char *others_found;
   char *vict_found;
   char *char_auto;
   char *others_auto;
};

struct xsocial_type
{
   XSOCIALTYPE *next;
   char *name;
   char *char_no_arg;
   char *others_no_arg;
   char *char_found;
   char *others_found;
   char *vict_found;
   char *char_auto;
   char *others_auto;
   sh_int gender;
   sh_int stage;
   sh_int position;
   sh_int self;
   sh_int other;
   sh_int extra;
   sh_int chance;
};

struct last_data
{
   LAST_DATA *next;
   LAST_DATA *prev;
   char *name;
   char *authed_by;
   sh_int class;
   sh_int dualclass;
   int level;
   char *pwd;
   sh_int race;
   sh_int sex;
   sh_int sublevel;
   char *site;
   sh_int trust;
};

extern int pAbort;

extern bool silence_locate_targets;

extern time_t last_restore_all_time;
extern time_t boot_time;
extern HOUR_MIN_SEC *set_boot_time;
extern struct tm *new_boot_time;
extern time_t new_boot_time_t;

extern const struct str_app_type str_app[45];
extern const struct int_app_type int_app[45];
extern const struct wis_app_type wis_app[45];
extern const struct dex_app_type dex_app[45];
extern const struct con_app_type con_app[45];
extern const struct cha_app_type cha_app[45];
extern const struct lck_app_type lck_app[45];

//extern const struct   xsocial_type   xsocial_table  [MAX_XSOC];

extern const struct race_type _race_table[MAX_RACE];
extern struct race_type *race_table[MAX_RACE];
extern struct at_color_type at_color_table[AT_MAXCOLOR];
extern const struct liq_type liq_table[LIQ_MAX];
/*
extern	char *	const			attack_table	[18];

extern  char ** const			s_message_table [18];
extern	char ** const			p_message_table	[18];
*/

extern char *attack_table[DAM_MAX_TYPE];
extern char *attack_table_plural[DAM_MAX_TYPE];
extern char **const s_message_table[DAM_MAX_TYPE];
extern char **const p_message_table[DAM_MAX_TYPE];
extern char *ft_end_msg[6];
extern char *weapon_skills[WEP_MAX];
extern char *projectiles[PROJ_MAX];

extern char *const skill_tname[];
extern sh_int const movement_loss[SECT_MAX];
extern char *const dir_name[];
extern char *const where_name[MAX_WHERE_NAME];
extern const sh_int rev_dir[];
extern const int trap_door[];
extern char *const r_flags[];
extern char *const w_flags[];
extern char *const sec_flags[];
extern char *const item_w_flags[];
extern char *const o_flags[];
extern char *const a_flags[];
extern char *const o_types[];
extern char *const a_types[];
extern char *const act_flags[];
extern char *const plr_flags[];
extern char *const pc_flags[];
extern char *const trap_flags[];
extern char *const ris_flags[];
extern char *const trig_flags[];
extern char *const part_flags[];
extern char *const npc_race[];
extern char *const npc_class[];
extern char *const defense_flags[];
extern char *const attack_flags[];
extern char *const area_flags[];
extern char *const ex_pmisc[];
extern char *const ex_pwater[];
extern char *const ex_pair[];
extern char *const ex_pearth[];
extern char *const ex_pfire[];

extern int const lang_array[];
extern char *const lang_names[];

extern char *const temp_settings[];
extern char *const precip_settings[];
extern char *const wind_settings[];
extern char *const preciptemp_msg[6][6];
extern char *const windtemp_msg[6][6];
extern char *const precip_msg[];
extern char *const wind_msg[];

extern MPSLEEP_DATA *first_mpwait;
extern MPSLEEP_DATA *last_mpwait;
extern MPSLEEP_DATA *current_mpwait;

extern char *bigregex;
extern char *preg;


extern char *target_name;
extern char *ranged_target_name;
extern int numobjsloaded;
extern int nummobsloaded;
extern int physicalobjects;
extern int last_pkroom;
extern int num_descriptors;
extern struct system_data sysdata;
extern int top_sn;
extern int top_vroom;
extern int top_herb;
extern int share_value;

extern CMDTYPE *command_hash[126];

extern struct class_type *class_table[MAX_CLASS];
extern char *title_table[MAX_CLASS][MAX_LEVEL + 1][2];

extern SKILLTYPE *skill_table[MAX_SKILL];
extern SOCIALTYPE *social_index[27];
extern XSOCIALTYPE *xsocial_index[27];
extern CHAR_DATA *cur_char;
extern ROOM_INDEX_DATA *cur_room;
extern bool cur_char_died;
extern ch_ret global_retcode;
extern SKILLTYPE *herb_table[MAX_HERB];
extern SKILLTYPE *disease_table[MAX_DISEASE];

extern int cur_obj;
extern int cur_obj_serial;
extern bool cur_obj_extracted;
extern obj_ret global_objcode;

extern HELP_DATA *first_help;
extern HELP_DATA *last_help;
extern SHOP_DATA *first_shop;
extern SHOP_DATA *last_shop;
extern REPAIR_DATA *first_repair;
extern REPAIR_DATA *last_repair;

extern WATCH_DATA *first_watch;
extern WATCH_DATA *last_watch;
extern BAN_DATA *first_ban;
extern BAN_DATA *last_ban;
extern BAN_DATA *first_ban_class;
extern BAN_DATA *last_ban_class;
extern BAN_DATA *first_ban_race;
extern BAN_DATA *last_ban_race;
extern RESERVE_DATA *first_reserved;
extern RESERVE_DATA *last_reserved;
extern CHAR_DATA *first_char;
extern CHAR_DATA *last_char;
extern MORPH_DATA *morph_start;
extern MORPH_DATA *morph_end;
extern DESCRIPTOR_DATA *first_descriptor;
extern DESCRIPTOR_DATA *last_descriptor;
extern BOARD_DATA *first_board;
extern BOARD_DATA *last_board;
extern PLANE_DATA *first_plane;
extern PLANE_DATA *last_plane;
extern PROJECT_DATA *first_project;
extern PROJECT_DATA *last_project;
extern OBJ_DATA *first_object;
extern OBJ_DATA *last_object;
extern CLAN_DATA *first_clan;
extern CLAN_DATA *last_clan;
extern COUNCIL_DATA *first_council;
extern COUNCIL_DATA *last_council;
extern DEITY_DATA *first_deity;
extern DEITY_DATA *last_deity;
extern AREA_DATA *first_area;
extern AREA_DATA *last_area;
extern AREA_DATA *first_build;
extern AREA_DATA *last_build;
extern AREA_DATA *first_asort;
extern AREA_DATA *last_asort;
extern AREA_DATA *first_bsort;
extern AREA_DATA *last_bsort;
extern AREA_DATA *first_area_name;
extern AREA_DATA *last_area_name;

extern NEWS_DATA *first_news;
extern NEWS_DATA *last_news;

extern LANG_DATA *first_lang;
extern LANG_DATA *last_lang;

extern TELEPORT_DATA *first_teleport;
extern TELEPORT_DATA *last_teleport;
extern OBJ_DATA *extracted_obj_queue;
extern EXTRACT_CHAR_DATA *extracted_char_queue;
extern OBJ_DATA *save_equipment[MAX_WEAR][MAX_LAYERS];
extern CHAR_DATA *quitting_char;
extern CHAR_DATA *loading_char;
extern CHAR_DATA *saving_char;
extern OBJ_DATA *all_obj;

extern char bug_buf[];
extern time_t current_time;
extern bool fLogAll;
extern FILE *fpReserve;
extern FILE *fpLOG;
extern char log_buf[];
extern TIME_INFO_DATA time_info;
extern WEATHER_DATA weather_info;
extern IMMORTAL_HOST *immortal_host_start;
extern IMMORTAL_HOST *immortal_host_end;
extern int weath_unit;
extern int rand_factor;
extern int climate_factor;
extern int neigh_factor;
extern int max_vector;

extern AUCTION_DATA *auction;
extern struct act_prog_data *mob_act_list;



#if	defined(_AIX)
char *crypt args( ( const char *key, const char *salt ) );
#endif

#if	defined(apollo)
int atoi args( ( const char *string ) );
void *calloc args( ( unsigned nelem, size_t size ) );
char *crypt args( ( const char *key, const char *salt ) );
#endif

#if	defined(hpux)
char *crypt args( ( const char *key, const char *salt ) );
#endif

#if	defined(interactive)
#endif

#if	defined(linux)
char *crypt args( ( const char *key, const char *salt ) );
#endif

#if	defined(MIPS_OS)
char *crypt args( ( const char *key, const char *salt ) );
#endif

#if	defined(NeXT)
char *crypt args( ( const char *key, const char *salt ) );
#endif

#if	defined(sequent)
char *crypt args( ( const char *key, const char *salt ) );
int fclose args( ( FILE * stream ) );
int fprintf args( ( FILE * stream, const char *format, ... ) );
int fread args( ( void *ptr, int size, int n, FILE * stream ) );
int fseek args( ( FILE * stream, long offset, int ptrname ) );
void perror args( ( const char *s ) );
int ungetc args( ( int c, FILE * stream ) );
#endif

#if	defined(sun)
char *crypt args( ( const char *key, const char *salt ) );
int fclose args( ( FILE * stream ) );
int fprintf args( ( FILE * stream, const char *format, ... ) );
#if 	defined(SYSV)
size_t fread args( ( void *ptr, size_t size, size_t n, FILE * stream ) );
#else
int fread args( ( void *ptr, int size, int n, FILE * stream ) );
#endif
int fseek args( ( FILE * stream, long offset, int ptrname ) );
void perror args( ( const char *s ) );
int ungetc args( ( int c, FILE * stream ) );
#endif

#if	defined(ultrix)
char *crypt args( ( const char *key, const char *salt ) );
#endif

#if	defined(NOCRYPT)
#define crypt(s1, s2)	(s1)
#endif


#define ERROR_DIR	"../error/"
#define PLAYER_DIR	"../player/"
#define BACKUP_DIR	"../player/"
#define GOD_DIR		"../system/gods/"
#define BOARD_DIR	"../system/"
#define CLAN_DIR	"../system/"
#define STORAGE_DIR     "../system/storage/"
#define COUNCIL_DIR  	"../system/"
#define DEITY_DIR	"../system/"
#define BUILD_DIR       "./building/"
#define SYSTEM_DIR	"../system/"
#define PROG_DIR	"mudprogs/"
#define CORPSE_DIR	"../corpses/"
#define NEWS_DIR        "../system/"
#define HTML_NEWS_DIR   "../system/"
#define NULL_FILE	"/dev/null"

#define LOG_DIR		"../log/"
#define	CLASS_DIR	"../system/"
#define WATCH_DIR	"../log/watch/"
#define REP_DIR		"../.rep/"
#define HTML_DIR	"../../public_html/"
#define REGISTER_DIR    "../system/register/"

#define NEWP_LIST  PLAYER_DIR "newp.lst"
#define NEWP_LIST2  PLAYER_DIR "newp2.lst"

#define REP_FILE	REP_DIR ".rep.log"
#define ERROR_FILE  ERROR_DIR "error.log"
#define BUGS_FILE   SYSTEM_DIR "bugs.dat"
#define CHANGES_FILE   SYSTEM_DIR "changes.dat"
#define AREA_LIST	"area.lst"
#define WATCH_LIST      "watch.lst"
#define BAN_LIST        "ban.lst"
#define RESERVED_LIST	"reserved.lst"
#define CLAN_LIST	"clan.lst"
#define COUNCIL_LIST	"council.lst"
#define GUILD_LIST      "guild.lst"
#define GOD_LIST	"gods.lst"
#define DEITY_LIST	"deity.lst"
#define	CLASS_LIST	"class.lst"
#define	RACE_LIST	"race.lst"

#define IMM_LOGTIME     LOG_DIR "immlogtime.log"
#define MORPH_FILE      "morph.dat"
#define BOARD_FILE	"boards.txt"
#define SHUTDOWN_FILE	"shutdown.txt"
#define IMM_HOST_FILE   SYSTEM_DIR "immortal.host"

#define RIPSCREEN_FILE	SYSTEM_DIR "mudrip.rip"
#define RIPTITLE_FILE	SYSTEM_DIR "mudtitle.rip"
#define ANSITITLE_FILE	SYSTEM_DIR "mudtitle.ans"
#define ASCTITLE_FILE	SYSTEM_DIR "mudtitle.asc"
#define BOOTLOG_FILE	SYSTEM_DIR "boot.txt"
#define BUG_FILE	SYSTEM_DIR "bugs.txt"
#define PBUG_FILE	SYSTEM_DIR "pbugs.txt"
#define IDEA_FILE	SYSTEM_DIR "ideas.txt"
#define TYPO_FILE	SYSTEM_DIR "typos.txt"
#define FIXED_FILE	SYSTEM_DIR "fixed.txt"
#define EQREG_FILE	SYSTEM_DIR "eqreg.txt"
#define LOG_FILE	SYSTEM_DIR "log.txt"

#define immLOG_FILE	REP_DIR "imm.log"
#define hbLOG_FILE       REP_DIR "hb.log"
#define cdLOG_FILE       REP_DIR "cd.log"
#define admLOG_FILE       REP_DIR "adm.log"
#define tellLOG_FILE       REP_DIR "tell.log"
#define heroLOG_FILE       REP_DIR "hero.log"
#define avaLOG_FILE       REP_DIR "ava.log"
#define neoLOG_FILE       REP_DIR "neo.log"
#define acoLOG_FILE       REP_DIR "aco.log"
#define chatLOG_FILE       REP_DIR "chat.log"
#define sayLOG_FILE       REP_DIR "say.log"
#define genLOG_FILE       REP_DIR "gen.log"

#define MOBLOG_FILE	SYSTEM_DIR "moblog.txt"
#define PLEVEL_FILE	SYSTEM_DIR "plevel.txt"
#define WIZLIST_FILE	SYSTEM_DIR "WIZLIST"
#define WHO_FILE	SYSTEM_DIR "WHO"
#define WEBWHO_FILE	SYSTEM_DIR "WEBWHO"
#define REQUEST_PIPE	SYSTEM_DIR "REQUESTS"
#define SKILL_FILE	SYSTEM_DIR "skills.dat"
#define HERB_FILE	SYSTEM_DIR "herbs.dat"
#define TONGUE_FILE	SYSTEM_DIR "tongues.dat"
#define SOCIAL_FILE	SYSTEM_DIR "socials.dat"
#define XSOCIAL_FILE	SYSTEM_DIR "xsocials.dat"
#define COMMAND_FILE	SYSTEM_DIR "commands.dat"
#define USAGE_FILE	SYSTEM_DIR "usage.txt"
#define ECONOMY_FILE	SYSTEM_DIR "economy.txt"
#define PROJECTS_FILE	SYSTEM_DIR "projects.txt"
#define COLOR_FILE	SYSTEM_DIR "colors.dat"
#define TEMP_FILE SYSTEM_DIR "charsave.tmp"
#define CLASSDIR	"../system/"
#define RACEDIR 	"../system/"
#define BANK_FILE SYSTEM_DIR "bank.txt"
#define MEETING_FILE	SYSTEM_DIR "meeting.txt"

#define COPYOVER_FILE  SYSTEM_DIR "copyover.dat"

#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define SF	SPEC_FUN
#define BD	BOARD_DATA
#define CL	CLAN_DATA
#define EDD	EXTRA_DESCR_DATA
#define RD	RESET_DATA
#define ED	EXIT_DATA
#define	ST	SOCIALTYPE
#define	XST	XSOCIALTYPE
#define	CO	COUNCIL_DATA
#define DE	DEITY_DATA
#define SK	SKILLTYPE

void *MudMalloc args( ( size_t size, int index ) );
void *MudCalloc args( ( size_t num, size_t size, int index ) );
void *MudRealloc args( ( void *memblock, size_t size, int index ) );
void MudFree args( ( void *memblock, int index ) );

bool DUAL_SKILL args( ( CHAR_DATA * ch, int sn ) );
int dual_adept args( ( CHAR_DATA * ch, int sn ) );
int find_skill_level args( ( CHAR_DATA * ch, int sn ) );
int find_skill_slevel args( ( CHAR_DATA * ch, int sn ) );
int find_skill_level_target args( ( int clas, int sn ) );
int find_skill_slevel_target args( ( int clas, int sn ) );
int hp_max args( ( CHAR_DATA * ch ) );
int hp_min args( ( CHAR_DATA * ch ) );
bool use_mana args( ( CHAR_DATA * ch ) );

void talk_info args( ( sh_int AT_COLOR, char *argument ) );
void char_quit args( ( CHAR_DATA * ch, bool broadcast, char *argument ) );

bool remove_obj args( ( CHAR_DATA * ch, int iWear, bool fReplace ) );
void wear_obj args( ( CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace, sh_int wear_bit, bool fAll ) );

void init_web( int port );
void handle_web( void );
void shutdown_web( void );

bool circle_follow args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void add_follower args( ( CHAR_DATA * ch, CHAR_DATA * master ) );
void stop_follower args( ( CHAR_DATA * ch ) );
void die_follower args( ( CHAR_DATA * ch ) );
bool is_same_group args( ( CHAR_DATA * ach, CHAR_DATA * bch ) );
void send_rip_screen args( ( CHAR_DATA * ch ) );
void send_rip_title args( ( CHAR_DATA * ch ) );
void send_ansi_title args( ( CHAR_DATA * ch ) );
void send_ascii_title args( ( CHAR_DATA * ch ) );
void to_channel args( ( const char *argument, int channel, const char *verb, sh_int level, sh_int ulvl ) );
void talk_auction args( ( char *argument ) );
int knows_language args( ( CHAR_DATA * ch, int language, CHAR_DATA * cch ) );
bool can_learn_lang args( ( CHAR_DATA * ch, int language ) );
int countlangs args( ( int languages ) );
char *translate args( ( int percent, const char *in, const char *name ) );
char *obj_short args( ( OBJ_DATA * obj ) );
void init_profanity_checker args( ( void ) );

void stage_update args( ( CHAR_DATA * ch, CHAR_DATA * victim, int stage, char *argument ) );
void make_preg args( ( CHAR_DATA * mother, CHAR_DATA * father ) );

void immlog_onoff args( ( CHAR_DATA * ch, char *onff ) );
void newchar args( ( CHAR_DATA * ch ) );

char *friendly_ctime args( ( time_t * time ) );

int num_bugs args( ( void ) );
int num_changes args( ( void ) );
void init_descriptor args( ( DESCRIPTOR_DATA * dnew, int desc ) );
void copyover args( ( char *argument ) );
void copyover_recover args( ( void ) );
int get_door args( ( char *arg ) );
char *num_punct args( ( int foo ) );
char *format_obj_to_char args( ( OBJ_DATA * obj, CHAR_DATA * ch, bool fShort ) );
void show_list_to_char args( ( OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing ) );
void show_list_to_char_inv args( ( OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing, int itemtype ) );
bool is_ignoring args( ( CHAR_DATA * ch, CHAR_DATA * ign_ch ) );
void show_race_line args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );

void clear_vrooms args( ( void ) );
ED *find_door args( ( CHAR_DATA * ch, char *arg, bool quiet ) );
ED *get_exit args( ( ROOM_INDEX_DATA * room, sh_int dir ) );
ED *get_exit_to args( ( ROOM_INDEX_DATA * room, sh_int dir, int vnum ) );
ED *get_exit_num args( ( ROOM_INDEX_DATA * room, sh_int count ) );
ch_ret move_char args( ( CHAR_DATA * ch, EXIT_DATA * pexit, int fall ) );
void teleport args( ( CHAR_DATA * ch, int room, int flags ) );
sh_int encumbrance args( ( CHAR_DATA * ch, sh_int move ) );
bool will_fall args( ( CHAR_DATA * ch, int fall ) );
ch_ret pullcheck args( ( CHAR_DATA * ch, int pulse ) );
char *rev_exit args( ( sh_int vdir ) );

void rent_calculate args( ( CHAR_DATA * ch, OBJ_DATA * obj, int *rent ) );
void char_leaving args( ( CHAR_DATA * ch, int howleft, int cost ) );

obj_ret damage_obj args( ( OBJ_DATA * obj ) );
sh_int get_obj_resistance args( ( OBJ_DATA * obj ) );
void save_clan_storeroom args( ( CHAR_DATA * ch ) );
void save_donation args( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
void obj_fall args( ( OBJ_DATA * obj, bool through ) );

void immune_casting args( ( SKILLTYPE * skill, CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * obj ) );
void failed_casting args( ( SKILLTYPE * skill, CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * obj ) );

bool create_new_race args( ( int index, char *argument ) );
bool create_new_class args( ( int index, char *argument ) );
RID *find_location args( ( CHAR_DATA * ch, char *arg ) );
void echo_to_all args( ( sh_int AT_COLOR, char *argument, sh_int tar ) );
void get_reboot_string args( ( void ) );
struct tm *update_time args( ( struct tm * old_time ) );
void free_social args( ( SOCIALTYPE * social ) );
void add_social args( ( SOCIALTYPE * social ) );
void free_xsocial args( ( XSOCIALTYPE * social ) );
void add_xsocial args( ( XSOCIALTYPE * social ) );
void free_command args( ( CMDTYPE * command ) );
void unlink_command args( ( CMDTYPE * command ) );
void add_command args( ( CMDTYPE * command ) );

void load_boards args( ( void ) );
BD *get_board args( ( OBJ_DATA * obj ) );
void free_note args( ( NOTE_DATA * pnote ) );

NOTE_DATA *note_free;
void free_global_note args( ( NOTE_DATA * note ) );

int get_cmdflag args( ( char *flag ) );
char *flag_string args( ( int bitvector, char *const flagarray[] ) );
char *ext_flag_string args( ( EXT_BV * bitvector, char *const flagarray[] ) );
int get_mpflag args( ( char *flag ) );
int get_dir args( ( char *txt ) );
char *strip_cr args( ( char *str ) );

CL *get_clan args( ( char *name ) );
void load_clans args( ( void ) );
void save_clan args( ( CLAN_DATA * clan ) );

CO *get_council args( ( char *name ) );
void load_councils args( ( void ) );
void save_council args( ( COUNCIL_DATA * council ) );

DE *get_deity args( ( char *name ) );
void load_deity args( ( void ) );
void save_deity args( ( DEITY_DATA * deity ) );

void close_socket args( ( DESCRIPTOR_DATA * dclose, bool force ) );
void write_to_buffer args( ( DESCRIPTOR_DATA * d, const char *txt, int length ) );
void write_to_pager args( ( DESCRIPTOR_DATA * d, const char *txt, int length ) );
void send_to_char args( ( const char *txt, CHAR_DATA * ch ) );
void send_to_char_color args( ( const char *txt, CHAR_DATA * ch ) );
void send_to_pager args( ( const char *txt, CHAR_DATA * ch ) );
void send_to_pager_color args( ( const char *txt, CHAR_DATA * ch ) );
void set_char_color args( ( sh_int AType, CHAR_DATA * ch ) );
void set_pager_color args( ( sh_int AType, CHAR_DATA * ch ) );
void ch_printf args( ( CHAR_DATA * ch, char *fmt, ... ) );
void ch_printf_color args( ( CHAR_DATA * ch, char *fmt, ... ) );
void log_printf args( ( char *fmt, ... ) );
void pager_printf args( ( CHAR_DATA * ch, char *fmt, ... ) );
void pager_printf_color args( ( CHAR_DATA * ch, char *fmt, ... ) );
void act args( ( sh_int AType, const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, int type ) );
char *myobj args( ( OBJ_DATA * obj ) );
char *obj_short args( ( OBJ_DATA * obj ) );

RD *make_reset args( ( char letter, int extra, int arg1, int arg2, int arg3 ) );
RD *add_reset args( ( AREA_DATA * tarea, char letter, int extra, int arg1, int arg2, int arg3 ) );
RD *place_reset args( ( AREA_DATA * tarea, char letter, int extra, int arg1, int arg2, int arg3 ) );
void reset_area args( ( AREA_DATA * pArea ) );

void check_spirit args( ( CHAR_DATA * ch, CHAR_DATA * victim, ROOM_INDEX_DATA * loc ) );

void login_save args( ( CHAR_DATA * ch ) );
char *get_wizrank_name args( ( int level ) );
void show_file args( ( CHAR_DATA * ch, char *filename ) );
char *str_dup args( ( char const *str ) );
void boot_db args( ( bool fCopyOver ) );
void area_update args( ( void ) );
void quest_update args( ( void ) );
void add_char args( ( CHAR_DATA * ch ) );
CD *create_mobile args( ( MOB_INDEX_DATA * pMobIndex ) );
OD *create_object args( ( OBJ_INDEX_DATA * pObjIndex, int level ) );
void clear_char args( ( CHAR_DATA * ch ) );
void free_char args( ( CHAR_DATA * ch ) );
char *get_extra_descr args( ( const char *name, EXTRA_DESCR_DATA * ed ) );
MID *get_mob_index args( ( int vnum ) );
OID *get_obj_index args( ( int vnum ) );
RID *get_room_index args( ( int vnum ) );
char fread_letter args( ( FILE * fp ) );
int fread_number args( ( FILE * fp ) );
EXT_BV fread_bitvector args( ( FILE * fp ) );
void fwrite_bitvector args( ( EXT_BV * bits, FILE * fp ) );
char *print_bitvector args( ( EXT_BV * bits ) );
char *fread_string args( ( FILE * fp ) );
char *fread_string_nohash args( ( FILE * fp ) );
void fread_to_eol args( ( FILE * fp ) );
char *fread_word args( ( FILE * fp ) );
char *fread_line args( ( FILE * fp ) );
int number_fuzzy args( ( int number ) );
int number_range args( ( int from, int to ) );
int number_percent args( ( void ) );
int number_door args( ( void ) );
int number_bits args( ( int width ) );
int number_mm args( ( void ) );
int dice args( ( int number, int size ) );
int interpolate args( ( int level, int value_00, int value_32 ) );
void smash_tilde args( ( char *str ) );
void hide_tilde args( ( char *str ) );
char *show_tilde args( ( char *str ) );
bool str_cmp args( ( const char *astr, const char *bstr ) );
bool str_prefix args( ( const char *astr, const char *bstr ) );
bool str_infix args( ( const char *astr, const char *bstr ) );
bool str_suffix args( ( const char *astr, const char *bstr ) );
char *capitalize args( ( const char *str ) );
char *strlower args( ( const char *str ) );
char *strupper args( ( const char *str ) );
char *aoran args( ( const char *str ) );
void append_file args( ( CHAR_DATA * ch, char *file, char *str ) );
void append_to_file args( ( char *file, char *str ) );
void bug args( ( const char *str, ... ) );
void log_string_plus args( ( const char *str, sh_int log_type, sh_int level ) );
RID *make_room args( ( int vnum ) );
OID *make_object args( ( int vnum, int cvnum, char *name ) );
MID *make_mobile args( ( int vnum, int cvnum, char *name ) );
ED *make_exit args( ( ROOM_INDEX_DATA * pRoomIndex, ROOM_INDEX_DATA * to_room, sh_int door ) );
void add_help args( ( HELP_DATA * pHelp ) );
void fix_area_exits args( ( AREA_DATA * tarea ) );
void load_area_file args( ( AREA_DATA * tarea, char *filename ) );
void randomize_exits args( ( ROOM_INDEX_DATA * room, sh_int maxdir ) );
void make_wizlist args( ( void ) );
void tail_chain args( ( void ) );
bool delete_room args( ( ROOM_INDEX_DATA * room ) );
bool delete_obj args( ( OBJ_INDEX_DATA * obj ) );
bool delete_mob args( ( MOB_INDEX_DATA * mob ) );
void sort_area args( ( AREA_DATA * pArea, bool proto ) );
void sort_area_by_name args( ( AREA_DATA * pArea ) );
void write_projects args( ( void ) );
void evolve_char args( ( CHAR_DATA * ch ) );

void start_editing args( ( CHAR_DATA * ch, char *data ) );
void stop_editing args( ( CHAR_DATA * ch ) );
void edit_buffer args( ( CHAR_DATA * ch, char *argument ) );
char *copy_buffer args( ( CHAR_DATA * ch ) );
bool can_rmodify args( ( CHAR_DATA * ch, ROOM_INDEX_DATA * room ) );
bool can_omodify args( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
bool can_mmodify args( ( CHAR_DATA * ch, CHAR_DATA * mob ) );
bool can_medit args( ( CHAR_DATA * ch, MOB_INDEX_DATA * mob ) );
void free_reset args( ( AREA_DATA * are, RESET_DATA * res ) );
void free_area args( ( AREA_DATA * are ) );
void assign_area args( ( CHAR_DATA * ch ) );
EDD *SetRExtra args( ( ROOM_INDEX_DATA * room, char *keywords ) );
bool DelRExtra args( ( ROOM_INDEX_DATA * room, char *keywords ) );
EDD *SetOExtra args( ( OBJ_DATA * obj, char *keywords ) );
bool DelOExtra args( ( OBJ_DATA * obj, char *keywords ) );
EDD *SetOExtraProto args( ( OBJ_INDEX_DATA * obj, char *keywords ) );
bool DelOExtraProto args( ( OBJ_INDEX_DATA * obj, char *keywords ) );
void fold_area args( ( AREA_DATA * tarea, char *filename, bool install ) );
int get_otype args( ( char *type ) );
int get_atype args( ( char *type ) );
int get_aflag args( ( char *flag ) );
int get_oflag args( ( char *flag ) );
int get_wflag args( ( char *flag ) );
void init_area_weather args( ( void ) );
void save_weatherdata args( ( void ) );
sh_int get_exp_tnl args( ( CHAR_DATA * ch ) );

int max_fight args( ( CHAR_DATA * ch ) );
void violence_update args( ( void ) );
ch_ret multi_hit args( ( CHAR_DATA * ch, CHAR_DATA * victim, int dt ) );
ch_ret projectile_hit args( ( CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * wield, OBJ_DATA * projectile, sh_int dist ) );
sh_int ris_damage args( ( CHAR_DATA * ch, sh_int dam, int ris ) );
ch_ret damage args( ( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt ) );
void update_pos args( ( CHAR_DATA * victim ) );
void set_fighting args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void stop_fighting args( ( CHAR_DATA * ch, bool Nicoleoth ) );
void free_fight args( ( CHAR_DATA * ch ) );
CD *who_fighting args( ( CHAR_DATA * ch ) );
void check_killer args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void check_attacker args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void death_cry args( ( CHAR_DATA * ch ) );
void stop_hunting args( ( CHAR_DATA * ch ) );
void stop_hating args( ( CHAR_DATA * ch ) );
void stop_fearing args( ( CHAR_DATA * ch ) );
void start_hunting args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void start_hating args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void start_fearing args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool is_hunting args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool is_hating args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool is_fearing args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool is_safe args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool legal_loot args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
sh_int VAMP_AC args( ( CHAR_DATA * ch ) );
bool check_illegal_pk args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void raw_kill args( ( CHAR_DATA * ch, CHAR_DATA * victim, bool spirit ) );
bool in_arena args( ( CHAR_DATA * ch ) );
bool can_astral args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool can_portal args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool can_tele args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );

void make_corpse args( ( CHAR_DATA * ch, CHAR_DATA * killer ) );
void make_blood args( ( CHAR_DATA * ch ) );
void make_bloodstain args( ( CHAR_DATA * ch ) );
void make_scraps args( ( OBJ_DATA * obj ) );
void make_fire args( ( ROOM_INDEX_DATA * in_room, sh_int timer ) );
OD *make_trap args( ( int v0, int v1, int v2, int v3 ) );
OD *create_money args( ( int amount, int type ) );

void actiondesc args( ( CHAR_DATA * ch, OBJ_DATA * obj, void *vo ) );
EXT_BV meb args( ( int bit ) );
EXT_BV multimeb args( ( int bit, ... ) );



char *mprog_type_to_name args( ( int type ) );

#ifdef DUNNO_STRSTR
char *strstr args( ( const char *s1, const char *s2 ) );
#endif

void mprog_wordlist_check args( ( char *arg, CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * object, void *vo, int type ) );
void mprog_percent_check args( ( CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * object, void *vo, int type ) );
void mprog_act_trigger args( ( char *buf, CHAR_DATA * mob, CHAR_DATA * ch, OBJ_DATA * obj, void *vo ) );
void mprog_bribe_trigger args( ( CHAR_DATA * mob, CHAR_DATA * ch, int amount ) );
void mprog_bribe_trigger_silver args( ( CHAR_DATA * mob, CHAR_DATA * ch, int amount ) );
void mprog_bribe_trigger_copper args( ( CHAR_DATA * mob, CHAR_DATA * ch, int amount ) );
void mprog_entry_trigger args( ( CHAR_DATA * mob ) );
void mprog_give_trigger args( ( CHAR_DATA * mob, CHAR_DATA * ch, OBJ_DATA * obj ) );
void mprog_greet_trigger args( ( CHAR_DATA * mob ) );
void mprog_fight_trigger args( ( CHAR_DATA * mob, CHAR_DATA * ch ) );
void mprog_hitprcnt_trigger args( ( CHAR_DATA * mob, CHAR_DATA * ch ) );
void mprog_death_trigger args( ( CHAR_DATA * killer, CHAR_DATA * mob ) );
void mprog_random_trigger args( ( CHAR_DATA * mob ) );
void mprog_speech_trigger args( ( char *txt, CHAR_DATA * mob ) );
void mprog_script_trigger args( ( CHAR_DATA * mob ) );
void mprog_hour_trigger args( ( CHAR_DATA * mob ) );
void mprog_time_trigger args( ( CHAR_DATA * mob ) );
void progbug args( ( char *str, CHAR_DATA * mob ) );
void rset_supermob args( ( ROOM_INDEX_DATA * room ) );
void release_supermob args( (  ) );
void mpsleep_update args( (  ) );

PLANE_DATA *plane_lookup args( ( const char *name ) );
void load_planes args( ( void ) );
void save_planes args( ( void ) );
void check_planes args( ( PLANE_DATA * p ) );

void set_title args( ( CHAR_DATA * ch, char *title ) );
void set_movein args( ( CHAR_DATA * ch, char *title ) );
void set_moveout args( ( CHAR_DATA * ch, char *title ) );

void fwrite_morph_data args( ( CHAR_DATA * ch, FILE * fp ) );
void fread_morph_data args( ( CHAR_DATA * ch, FILE * fp ) );
void clear_char_morph args( ( CHAR_MORPH * morph ) );
CHAR_MORPH *make_char_morph args( ( MORPH_DATA * morph ) );
void free_char_morph args( ( CHAR_MORPH * morph ) );
CHAR_MORPH *make_char_morph args( ( MORPH_DATA * morph ) );
char *race_string args( ( int bitvector ) );
char *class_string args( ( int bitvector ) );
void setup_morph_vnum args( ( void ) );
void unmorph_all args( ( MORPH_DATA * morph ) );
MORPH_DATA *get_morph args( ( char *arg ) );
MORPH_DATA *get_morph_vnum args( ( int arg ) );
int do_morph_char args( ( CHAR_DATA * ch, MORPH_DATA * morph ) );
MORPH_DATA *find_morph args( ( CHAR_DATA * ch, char *target, bool is_cast ) );
void do_unmorph_char args( ( CHAR_DATA * ch ) );
void send_morph_message args( ( CHAR_DATA * ch, MORPH_DATA * morph, bool is_morph ) );
bool can_morph args( ( CHAR_DATA * ch, MORPH_DATA * morph, bool is_cast ) );
void do_morph args( ( CHAR_DATA * ch, MORPH_DATA * morph ) );
void do_unmorph args( ( CHAR_DATA * ch ) );
void save_morphs args( ( void ) );
void fwrite_morph args( ( FILE * fp, MORPH_DATA * morph ) );
void load_morphs args( ( void ) );
MORPH_DATA *fread_morph args( ( FILE * fp ) );
void free_morph args( ( MORPH_DATA * morph ) );
void morph_defaults args( ( MORPH_DATA * morph ) );
void sort_morphs args( ( void ) );


bool can_use_skill args( ( CHAR_DATA * ch, int percent, int gsn ) );
bool check_skill args( ( CHAR_DATA * ch, char *command, char *argument ) );
void learn_from_success args( ( CHAR_DATA * ch, int sn ) );
void learn_from_failure args( ( CHAR_DATA * ch, int sn ) );
bool check_parry args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool check_guard args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool check_dodge args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool check_tumble args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool check_grip args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void disarm args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void trip args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool mob_fire args( ( CHAR_DATA * ch, char *name ) );
CD *scan_for_victim args( ( CHAR_DATA * ch, EXIT_DATA * pexit, char *name ) );

int add_ban args( ( CHAR_DATA * ch, char *arg1, char *arg2, int time, int type ) );
void show_bans args( ( CHAR_DATA * ch, int type ) );
void save_banlist args( ( void ) );
void load_banlist args( ( void ) );
bool check_total_bans args( ( DESCRIPTOR_DATA * d ) );
bool check_bans args( ( CHAR_DATA * ch, int type ) );


HELP_DATA *get_help args( ( CHAR_DATA * ch, char *argument ) );
AREA_DATA *get_area_obj args( ( OBJ_INDEX_DATA * obj ) );
int get_exp args( ( CHAR_DATA * ch ) );
int get_exp_worth args( ( CHAR_DATA * ch ) );
int exp_level args( ( CHAR_DATA * ch, sh_int level ) );
sh_int get_trust args( ( CHAR_DATA * ch ) );
sh_int get_age args( ( CHAR_DATA * ch ) );
sh_int get_curr_str args( ( CHAR_DATA * ch ) );
sh_int get_curr_int args( ( CHAR_DATA * ch ) );
sh_int get_curr_wis args( ( CHAR_DATA * ch ) );
sh_int get_curr_dex args( ( CHAR_DATA * ch ) );
sh_int get_curr_con args( ( CHAR_DATA * ch ) );
sh_int get_curr_cha args( ( CHAR_DATA * ch ) );
sh_int get_curr_lck args( ( CHAR_DATA * ch ) );
bool can_take_proto args( ( CHAR_DATA * ch ) );
int can_carry_n args( ( CHAR_DATA * ch ) );
int can_carry_w args( ( CHAR_DATA * ch ) );
bool is_name args( ( const char *str, char *namelist ) );
int string_without_ansi_length args( ( const char *txt ) );
bool is_name_prefix args( ( const char *str, char *namelist ) );
bool nifty_is_name args( ( char *str, char *namelist ) );
bool nifty_is_name_prefix args( ( char *str, char *namelist ) );
void affect_modify args( ( CHAR_DATA * ch, AFFECT_DATA * paf, bool fAdd ) );
void affect_to_char args( ( CHAR_DATA * ch, AFFECT_DATA * paf ) );
void affect_remove args( ( CHAR_DATA * ch, AFFECT_DATA * paf ) );
void affect_strip args( ( CHAR_DATA * ch, int sn ) );
bool is_affected args( ( CHAR_DATA * ch, int sn ) );
void affect_join args( ( CHAR_DATA * ch, AFFECT_DATA * paf ) );
void char_from_room args( ( CHAR_DATA * ch ) );
void char_to_room args( ( CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex ) );
OD *obj_to_char args( ( OBJ_DATA * obj, CHAR_DATA * ch ) );
void obj_from_char args( ( OBJ_DATA * obj ) );
int apply_ac args( ( OBJ_DATA * obj, int iWear ) );
OD *get_eq_char args( ( CHAR_DATA * ch, int iWear ) );
void equip_char args( ( CHAR_DATA * ch, OBJ_DATA * obj, int iWear ) );
void unequip_char args( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
int count_obj_list args( ( OBJ_INDEX_DATA * obj, OBJ_DATA * list ) );
void obj_from_room args( ( OBJ_DATA * obj ) );
OD *obj_to_room args( ( OBJ_DATA * obj, ROOM_INDEX_DATA * pRoomIndex ) );
OD *obj_to_room_cr args( ( OBJ_DATA * obj, ROOM_INDEX_DATA * pRoomIndex, CHAR_DATA * ch ) );
OD *obj_to_obj args( ( OBJ_DATA * obj, OBJ_DATA * obj_to ) );
void obj_from_obj args( ( OBJ_DATA * obj ) );
void extract_obj args( ( OBJ_DATA * obj ) );
void extract_exit args( ( ROOM_INDEX_DATA * room, EXIT_DATA * pexit ) );
void extract_room args( ( ROOM_INDEX_DATA * room ) );
void clean_room args( ( ROOM_INDEX_DATA * room ) );
void clean_obj args( ( OBJ_INDEX_DATA * obj ) );
void clean_mob args( ( MOB_INDEX_DATA * mob ) );
void clean_resets args( ( AREA_DATA * tarea ) );
void extract_char args( ( CHAR_DATA * ch, bool fPull ) );
CD *get_char_room args( ( CHAR_DATA * ch, char *argument ) );
CD *get_char_area args( ( CHAR_DATA * ch, char *argument ) );
CD *get_char_room_p args( ( CHAR_DATA * ch, char *argument ) );
CD *get_char_world args( ( CHAR_DATA * ch, char *argument ) );
OD *get_obj_type args( ( OBJ_INDEX_DATA * pObjIndexData ) );
OD *get_obj_list args( ( CHAR_DATA * ch, char *argument, OBJ_DATA * list ) );
OD *get_obj_list_rev args( ( CHAR_DATA * ch, char *argument, OBJ_DATA * list ) );
OD *get_obj_carry args( ( CHAR_DATA * ch, char *argument ) );
OD *get_obj_wear args( ( CHAR_DATA * ch, char *argument ) );
OD *get_obj_vnum args( ( CHAR_DATA * ch, int vnum ) );
OD *get_obj_here args( ( CHAR_DATA * ch, char *argument ) );
OD *get_obj_here2 args( ( CHAR_DATA * ch, char *argument ) );
OD *get_obj_world args( ( CHAR_DATA * ch, char *argument ) );
int get_obj_number args( ( OBJ_DATA * obj ) );
int get_obj_weight args( ( OBJ_DATA * obj ) );
int get_real_obj_weight args( ( OBJ_DATA * obj ) );
bool rare_obj args( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
bool room_is_dark args( ( ROOM_INDEX_DATA * pRoomIndex ) );
bool room_is_private args( ( ROOM_INDEX_DATA * pRoomIndex ) );
CD *room_is_dnd args( ( CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex ) );
CD *room_is_HAdnd args( ( CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex ) );
bool can_see args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool can_see_obj args( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
bool can_drop_obj args( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
char *item_type_name args( ( OBJ_DATA * obj ) );
char *affect_loc_name args( ( int location ) );
char *affect_bit_name args( ( EXT_BV * vector ) );
char *extra_bit_name args( ( EXT_BV * extra_flags ) );
char *magic_bit_name args( ( int magic_flags ) );
char *pull_type_name args( ( int pulltype ) );
ch_ret check_for_trap args( ( CHAR_DATA * ch, OBJ_DATA * obj, int flag ) );
ch_ret check_room_for_traps args( ( CHAR_DATA * ch, int flag ) );
bool is_trapped args( ( OBJ_DATA * obj ) );
OD *get_trap args( ( OBJ_DATA * obj ) );
ch_ret spring_trap args( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
void name_stamp_stats args( ( CHAR_DATA * ch ) );
void fix_char args( ( CHAR_DATA * ch ) );

bool can_gpc args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );

void showaffect args( ( CHAR_DATA * ch, AFFECT_DATA * paf ) );
void showaffect_aff args( ( CHAR_DATA * ch, AFFECT_DATA * paf ) );

void *locate_targets args( ( CHAR_DATA * ch, char *arg, int sn, CHAR_DATA ** victim, OBJ_DATA ** obj ) );


void set_cur_obj args( ( OBJ_DATA * obj ) );
bool obj_extracted args( ( OBJ_DATA * obj ) );
void queue_extracted_obj args( ( OBJ_DATA * obj ) );
void clean_obj_queue args( ( void ) );
void set_cur_char args( ( CHAR_DATA * ch ) );
bool char_died args( ( CHAR_DATA * ch ) );
void queue_extracted_char args( ( CHAR_DATA * ch, bool extract ) );
void clean_char_queue args( ( void ) );
void add_timer args( ( CHAR_DATA * ch, sh_int type, sh_int count, DO_FUN * fun, int value ) );
TIMER *get_timerptr args( ( CHAR_DATA * ch, sh_int type ) );
sh_int get_timer args( ( CHAR_DATA * ch, sh_int type ) );
void extract_timer args( ( CHAR_DATA * ch, TIMER * timer ) );
void remove_timer args( ( CHAR_DATA * ch, sh_int type ) );
bool in_soft_range args( ( CHAR_DATA * ch, AREA_DATA * tarea ) );
bool in_hard_range args( ( CHAR_DATA * ch, AREA_DATA * tarea ) );
bool chance args( ( CHAR_DATA * ch, sh_int percent ) );
bool chance_attrib args( ( CHAR_DATA * ch, sh_int percent, sh_int attrib ) );
OD *clone_object args( ( OBJ_DATA * obj ) );
void split_obj args( ( OBJ_DATA * obj, int num ) );
void separate_obj args( ( OBJ_DATA * obj ) );
bool empty_obj args( ( OBJ_DATA * obj, OBJ_DATA * destobj, ROOM_INDEX_DATA * destroom ) );
OD *find_obj args( ( CHAR_DATA * ch, char *argument, bool carryonly ) );
bool ms_find_obj args( ( CHAR_DATA * ch ) );
void worsen_mental_state args( ( CHAR_DATA * ch, int mod ) );
void better_mental_state args( ( CHAR_DATA * ch, int mod ) );
void boost_economy args( ( AREA_DATA * tarea, int gold ) );
void lower_economy args( ( AREA_DATA * tarea, int gold ) );
void economize_mobgold args( ( CHAR_DATA * mob ) );
bool economy_has args( ( AREA_DATA * tarea, int gold ) );
void add_kill args( ( CHAR_DATA * ch, CHAR_DATA * mob ) );
int times_killed args( ( CHAR_DATA * ch, CHAR_DATA * mob ) );
void update_aris args( ( CHAR_DATA * ch ) );
AREA_DATA *get_area args( ( char *name ) );
OD *get_objtype args( ( CHAR_DATA * ch, sh_int type ) );

bool check_pos args( ( CHAR_DATA * ch, sh_int position ) );
void interpret args( ( CHAR_DATA * ch, char *argument ) );
void substitute_alias args( ( DESCRIPTOR_DATA * d, char *argument ) );
bool is_number args( ( char *arg ) );
int number_argument args( ( char *argument, char *arg ) );
char *one_argument args( ( char *argument, char *arg_first ) );
char *one_interpret args( ( char *argument, char *arg_first ) );
char *one_argument2 args( ( char *argument, char *arg_first ) );
ST *find_social args( ( char *command ) );
XST *find_xsocial args( ( char *command ) );
CMDTYPE *find_command args( ( char *command ) );
void hash_commands args( (  ) );
void start_timer args( ( struct timeval * stime ) );
time_t end_timer args( ( struct timeval * stime ) );
void send_timer args( ( struct timerset * vtime, CHAR_DATA * ch ) );
void update_userec args( ( struct timeval * time_used, struct timerset * userec ) );

bool process_spell_components args( ( CHAR_DATA * ch, int sn ) );
int ch_slookup args( ( CHAR_DATA * ch, const char *name ) );
int find_spell args( ( CHAR_DATA * ch, const char *name, bool know ) );
int find_song args( ( CHAR_DATA * ch, const char *name, bool know ) );
int find_skill args( ( CHAR_DATA * ch, const char *name, bool know ) );
int find_weapon args( ( CHAR_DATA * ch, const char *name, bool know ) );
int find_tongue args( ( CHAR_DATA * ch, const char *name, bool know ) );
int skill_lookup args( ( const char *name ) );
int herb_lookup args( ( const char *name ) );
int personal_lookup args( ( CHAR_DATA * ch, const char *name ) );
int slot_lookup args( ( int slot ) );
int bsearch_skill args( ( const char *name, int first, int top ) );
int bsearch_skill_exact args( ( const char *name, int first, int top ) );
int bsearch_skill_prefix args( ( const char *name, int first, int top ) );
bool saves_poison_death args( ( int level, CHAR_DATA * victim ) );
bool saves_wand args( ( int level, CHAR_DATA * victim ) );
bool saves_para_petri args( ( int level, CHAR_DATA * victim ) );
bool saves_breath args( ( int level, CHAR_DATA * victim ) );
bool saves_spell_staff args( ( int level, CHAR_DATA * victim ) );
ch_ret obj_cast_spell args( ( int sn, int level, CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * obj ) );
int dice_parse args( ( CHAR_DATA * ch, int level, char *exp ) );
SK *get_skilltype args( ( int sn ) );
sh_int get_chain_type args( ( ch_ret retcode ) );
ch_ret chain_spells args( ( int sn, int level, CHAR_DATA * ch, void *vo, sh_int chain ) );

void basestat args( ( OBJ_DATA * obj ) );

void init_request_pipe args( ( void ) );
void check_requests args( ( void ) );

#define OS_CARRY	0
#define OS_CORPSE	1
#define OS_REGISTER     2
void update_newpl args( ( CHAR_DATA * ch ) );
void save_char_obj args( ( CHAR_DATA * ch ) );
bool load_char_obj args( ( DESCRIPTOR_DATA * d, char *name, bool preload ) );
void set_alarm args( ( long seconds ) );
void requip_char args( ( CHAR_DATA * ch ) );
void fwrite_obj args( ( CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, int iNest, sh_int os_type ) );
void fread_obj args( ( CHAR_DATA * ch, FILE * fp, sh_int os_type ) );
void de_equip_char args( ( CHAR_DATA * ch ) );
void re_equip_char args( ( CHAR_DATA * ch ) );
void read_char_mobile args( ( char *argument ) );
void write_char_mobile args( ( CHAR_DATA * ch, char *argument ) );
CHAR_DATA *fread_mobile args( ( FILE * fp ) );
void fwrite_mobile args( ( FILE * fp, CHAR_DATA * mob ) );

SF *spec_lookup args( ( const char *name ) );
char *lookup_spec args( ( SPEC_FUN * special ) );

int get_skill args( ( char *skilltype ) );
char *spell_name args( ( SPELL_FUN * spell ) );
char *skill_name args( ( DO_FUN * skill ) );
void load_skill_table args( ( void ) );
void save_skill_table args( ( void ) );
void sort_skill_table args( ( void ) );
void remap_slot_numbers args( ( void ) );
void load_socials args( ( void ) );
void save_socials args( ( void ) );
void load_xsocials args( ( void ) );
void save_xsocials args( ( void ) );
void load_commands args( ( void ) );
void save_commands args( ( void ) );
SPELL_FUN *spell_function args( ( char *name ) );
DO_FUN *skill_function args( ( char *name ) );
void write_class_file args( ( int cl ) );
void save_classes args( ( void ) );
void load_classes args( ( void ) );
void load_herb_table args( ( void ) );
void save_herb_table args( ( void ) );
void load_races args( ( void ) );
void load_tongues args( ( void ) );

void helphtml args( ( void ) );

void found_prey args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void hunt_victim args( ( CHAR_DATA * ch ) );

void advance_level args( ( CHAR_DATA * ch, bool broadquit ) );
void advance_slevel args( ( CHAR_DATA * ch, bool broadcast, int gain ) );
void advance_level_down args( ( CHAR_DATA * ch, bool broadquit ) );
void advance_slevel_down args( ( CHAR_DATA * ch, bool broadcast, int gain ) );
void gain_exp args( ( CHAR_DATA * ch, int gain ) );
void gain_condition args( ( CHAR_DATA * ch, int iCond, int value ) );
void check_alignment args( ( CHAR_DATA * ch ) );
void update_handler args( ( void ) );
void reboot_check args( ( time_t reset ) );
#if 0
void reboot_check args( ( char *arg ) );
#endif
void auction_update args( ( void ) );
void remove_portal args( ( OBJ_DATA * portal ) );
void weather_update args( ( void ) );
void bank_update args( ( void ) );

char *str_alloc args( ( char *str ) );
char *quick_link args( ( char *str ) );
int str_free args( ( char *str ) );
void show_hash args( ( int count ) );
char *hash_stats args( ( void ) );
char *check_hash args( ( char *str ) );
void hash_dump args( ( int hash ) );
void show_high_hash args( ( int top ) );

int unread_notes_check args( ( CHAR_DATA * ch, GLOBAL_BOARD_DATA * board ) );

char *get_class args( ( CHAR_DATA * ch ) );
char *get_dualclass args( ( CHAR_DATA * ch ) );
char *get_race args( ( CHAR_DATA * ch ) );

void save_finger args( ( CHAR_DATA * ch ) );
void fwrite_finger args( ( CHAR_DATA * ch, FILE * fp ) );
void read_finger args( ( CHAR_DATA * ch, char *argument ) );
void fread_finger args( ( CHAR_DATA * ch, FILE * fp ) );

void meeting_interpret args( ( CHAR_DATA * ch, char *argument ) );

/* zutil.c */
bool is_registered args( ( char *ch ) );
#undef	SK
#undef	CO
#undef	ST
#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF
#undef	BD
#undef	CL
#undef	EDD
#undef	RD
#undef	ED


#define SH_INT 1
#define INT 2
#define CHAR 3
#define STRING 4
#define SPECIAL 5


#define NO_PAGE    0
#define MOB_PAGE_A 1
#define MOB_PAGE_B 2
#define MOB_PAGE_C 3
#define MOB_PAGE_D 4
#define MOB_PAGE_E 5
#define MOB_PAGE_F 17
#define MOB_HELP_PAGE 14
#define ROOM_PAGE_A 6
#define ROOM_PAGE_B 7
#define ROOM_PAGE_C 8
#define ROOM_HELP_PAGE 15
#define OBJ_PAGE_A 9
#define OBJ_PAGE_B 10
#define OBJ_PAGE_C 11
#define OBJ_PAGE_D 12
#define OBJ_PAGE_E 13
#define OBJ_HELP_PAGE 16
#define CONTROL_PAGE_A 18
#define CONTROL_HELP_PAGE 19

#define NO_TYPE   0
#define MOB_TYPE  1
#define OBJ_TYPE  2
#define ROOM_TYPE 3
#define CONTROL_TYPE 4

#define SUB_NORTH DIR_NORTH
#define SUB_EAST  DIR_EAST
#define SUB_SOUTH DIR_SOUTH
#define SUB_WEST  DIR_WEST
#define SUB_UP    DIR_UP
#define SUB_DOWN  DIR_DOWN
#define SUB_NE    DIR_NORTHEAST
#define SUB_NW    DIR_NORTHWEST
#define SUB_SE    DIR_SOUTHEAST
#define SUB_SW    DIR_SOUTHWEST


#define RIS_000		BV00
#define RIS_R00		BV01
#define RIS_0I0		BV02
#define RIS_RI0		BV03
#define RIS_00S		BV04
#define RIS_R0S		BV05
#define RIS_0IS		BV06
#define RIS_RIS		BV07

#define GA_AFFECTED	BV09
#define GA_RESISTANT	BV10
#define GA_IMMUNE	BV11
#define GA_SUSCEPTIBLE	BV12
#define GA_RIS          BV30



struct map_data
{
   int vnum;
   int x;
   int y;
   char entry;
};


struct map_index_data
{
   MAP_INDEX_DATA *next;
   int vnum;
   int map_of_vnums[49][81];
};


MAP_INDEX_DATA *get_map_index( int vnum );
void init_maps(  );

extern CHAR_DATA *supermob;

void oprog_speech_trigger( char *txt, CHAR_DATA * ch );
void oprog_random_trigger( OBJ_DATA * obj );
void oprog_wear_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
bool oprog_use_trigger( CHAR_DATA * ch, OBJ_DATA * obj, CHAR_DATA * vict, OBJ_DATA * targ, void *vo );
void oprog_remove_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_sac_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_damage_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_repair_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_drop_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_zap_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
char *oprog_type_to_name( int type );

void oprog_greet_trigger( CHAR_DATA * ch );
void oprog_speech_trigger( char *txt, CHAR_DATA * ch );
void oprog_random_trigger( OBJ_DATA * obj );
void oprog_random_trigger( OBJ_DATA * obj );
void oprog_remove_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_sac_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_get_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_damage_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_repair_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_drop_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_examine_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_zap_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_pull_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_push_trigger( CHAR_DATA * ch, OBJ_DATA * obj );


#define ERROR_PROG        -1
#define IN_FILE_PROG      -2

typedef enum
{
   ACT_PROG, SPEECH_PROG, RAND_PROG, FIGHT_PROG, DEATH_PROG, HITPRCNT_PROG,
   ENTRY_PROG, GREET_PROG, ALL_GREET_PROG, GIVE_PROG, BRIBE_PROG, HOUR_PROG,
   TIME_PROG, WEAR_PROG, REMOVE_PROG, SAC_PROG, LOOK_PROG, EXA_PROG, ZAP_PROG,
   GET_PROG, DROP_PROG, DAMAGE_PROG, REPAIR_PROG, RANDIW_PROG, SPEECHIW_PROG,
   PULL_PROG, PUSH_PROG, SLEEP_PROG, REST_PROG, LEAVE_PROG, SCRIPT_PROG,
   USE_PROG, BRIBE_COPPER_PROG, BRIBE_SILVER_PROG
}
prog_types;

#define RDEATH_PROG DEATH_PROG
#define ENTER_PROG  ENTRY_PROG
#define RFIGHT_PROG FIGHT_PROG
#define RGREET_PROG GREET_PROG
#define OGREET_PROG GREET_PROG

void rprog_leave_trigger( CHAR_DATA * ch );
void rprog_enter_trigger( CHAR_DATA * ch );
void rprog_sleep_trigger( CHAR_DATA * ch );
void rprog_rest_trigger( CHAR_DATA * ch );
void rprog_rfight_trigger( CHAR_DATA * ch );
void rprog_death_trigger( CHAR_DATA * killer, CHAR_DATA * ch );
void rprog_speech_trigger( char *txt, CHAR_DATA * ch );
void rprog_random_trigger( CHAR_DATA * ch );
void rprog_time_trigger( CHAR_DATA * ch );
void rprog_hour_trigger( CHAR_DATA * ch );
char *rprog_type_to_name( int type );

#define OPROG_ACT_TRIGGER
#ifdef OPROG_ACT_TRIGGER
void oprog_act_trigger( char *buf, OBJ_DATA * mobj, CHAR_DATA * ch, OBJ_DATA * obj, void *vo );
#endif
#define RPROG_ACT_TRIGGER
#ifdef RPROG_ACT_TRIGGER
void rprog_act_trigger( char *buf, ROOM_INDEX_DATA * room, CHAR_DATA * ch, OBJ_DATA * obj, void *vo );
#endif


#define GET_ADEPT(ch,sn)    (  skill_table[(sn)]->skill_adept[(ch)->class])
#define LEARNED(ch,sn)	    (IS_NPC(ch) ? 80 : URANGE(0, ch->pcdata->learned[sn], 101))

#define CHAR_SIZE sizeof(char)

typedef char *LONG_VECTOR;

#define LV_CREATE(vector, bit_length)					\
do									\
{									\
	int i;								\
	CREATE(vector, char, 1 + bit_length/CHAR_SIZE);			\
									\
	for(i = 0; i <= bit_length/CHAR_SIZE; i++)			\
		*(vector + i) = 0;					\
}while(0)

#define LV_IS_SET(vector, index)					\
	(*(vector + index/CHAR_SIZE) & (1 << index%CHAR_SIZE))

#define LV_SET_BIT(vector, index)					\
	(*(vector + index/CHAR_SIZE) |= (1 << index%CHAR_SIZE))

#define LV_REMOVE_BIT(vector, index)					\
	(*(vector + index/CHAR_SIZE) &= ~(1 << index%CHAR_SIZE))

#define LV_TOGGLE_BIT(vector, index)					\
	(*(vector + index/CHAR_SIZE) ^= (1 << index%CHAR_SIZE))

int RENAME( const char *oldpath, const char *newpath );
