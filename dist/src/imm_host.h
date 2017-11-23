/************************************************************************/
/*        Acadia code team (C) 2002-2005 Kara Enterprises                   */
/************************************************************************/
/*		Immortal Host Header			                */
/************************************************************************/
/*****************************************************
 * **     _________       __			    **
 * **     \_   ___ \_____|__| _____  ________  ___     **
 * **	/    \	\/_  __ \ |/	 \/  ___/_ \/	\   **
 * **	\     \___|  | \/ |  | |  \___ \  / ) |  \  **
 * **	 \______  /__| |__|__|_|  /____ \__/__|  /  **
 * **	   ____\/____ _        \/ ___ \/      \/    **
 * **	   \______   \ |_____  __| _/___	    **
 * **	    |	 |  _/ |\__  \/ __ | __ \	    **
 * **	    |	 |   \ |_/ __ \  / | ___/_	    **
 * **	    |_____  /__/____  /_  /___	/	    **
 * **		 \/Antipode\/  \/    \/ 	    **
 * ******************************************************
 * **	   Crimson Blade Codebase (CbC) 	    **
 * **     (c) 2000-2002 John Bellone (Noplex)	    **
 * **	     Coders: Noplex, Krowe		    **
 * **	  http://www.crimsonblade.org		    **
 * ******************************************************
 * ** Based on SMAUG 1.4a, by; Thoric, Altrag, Blodkai **
 * **  Narn, Haus, Scryn, Rennard, Swordbearer, Gorog  **
 * **    Grishnakh, Nivek, Tricops, and Fireblade	    **
 * ******************************************************
 * ** Merc 2.1 by; Michael Chastain, Michael Quan, and **
 * **		    Mitchell Tse		    **
 * ******************************************************
 * **   DikuMUD by; Sebastian Hammer, Michael Seifert, **
 * **     Hans Staerfeldt, Tom Madsen and Katja Nyobe  **
 * *****************************************************/

/**********************************************
 * **	   Advanced Immortal Host	     **
 * ***********************************************
 * 	 By Noplex, Samson and Senir
 * 	 **********************************************/

#define MAX_DOMAIN     10

typedef struct immortal_host_log IMMORTAL_HOST_LOG;
struct immortal_host_log
{
   IMMORTAL_HOST_LOG *next;
   IMMORTAL_HOST_LOG *prev;
   char *host;
   char *date;
};

typedef struct immortal_host IMMORTAL_HOST;
struct immortal_host
{
   IMMORTAL_HOST *next;
   IMMORTAL_HOST *prev;
   char *name;
   char *domain[MAX_DOMAIN];
   IMMORTAL_HOST_LOG *first_log;
   IMMORTAL_HOST_LOG *last_log;
};

IMMORTAL_HOST *first_imm_host;
IMMORTAL_HOST *last_imm_host;

void load_imm_host( void );
void save_imm_host( void );
IMMORTAL_HOST *fread_imm_host( FILE * fp );
IMMORTAL_HOST_LOG *fread_imm_host_log( FILE * fp );
bool check_immortal_domain( CHAR_DATA * ch, char *host );

DECLARE_DO_FUN( do_immhost );
