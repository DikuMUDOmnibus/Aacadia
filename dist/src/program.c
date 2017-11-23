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
/*					   MUD Prog module			                    */
/************************************************************************/
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "acadia.h"
void sportschan( char * );

static bool carryingvnum_visit( CHAR_DATA * ch, OBJ_DATA * obj, int vnum )
{
   if( obj->wear_loc == -1 && obj->pIndexData->vnum == vnum )
      return TRUE;
   if( obj->first_content )
   {
      if( carryingvnum_visit( ch, obj->first_content, vnum ) )
         return TRUE;
      if( obj->next_content )
         if( carryingvnum_visit( ch, obj->next_content, vnum ) )
            return TRUE;
   }
   else if( obj->next_content )
      if( carryingvnum_visit( ch, obj->next_content, vnum ) )
         return TRUE;
   return FALSE;
}

#define COMMANDOK    1
#define IFTRUE       2
#define IFFALSE      3
#define ORTRUE       4
#define ORFALSE      5
#define FOUNDELSE    6
#define FOUNDENDIF   7
#define IFIGNORED    8
#define ORIGNORED    9


int mprog_do_command( char *cmnd, CHAR_DATA * mob, CHAR_DATA * actor,
                      OBJ_DATA * obj, void *vo, CHAR_DATA * rndm, bool ignore, bool ignore_ors );

CHAR_DATA *supermob;
struct act_prog_data *room_act_list;
struct act_prog_data *obj_act_list;
struct act_prog_data *mob_act_list;

MPSLEEP_DATA *first_mpsleep = NULL;
MPSLEEP_DATA *last_mpsleep = NULL;
MPSLEEP_DATA *current_mpsleep = NULL;


char *mprog_next_command args( ( char *clist ) );
bool mprog_seval args( ( char *lhs, char *opr, char *rhs, CHAR_DATA * mob ) );
bool mprog_veval args( ( int lhs, char *opr, int rhs, CHAR_DATA * mob ) );
int mprog_do_ifcheck args( ( char *ifcheck, CHAR_DATA * mob,
                             CHAR_DATA * actor, OBJ_DATA * obj, void *vo, CHAR_DATA * rndm ) );
void mprog_translate args( ( char ch, char *t, CHAR_DATA * mob,
                             CHAR_DATA * actor, OBJ_DATA * obj, void *vo, CHAR_DATA * rndm ) );
void mprog_driver args( ( char *com_list, CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * obj, void *vo, bool single_step ) );

bool mprog_keyword_check args( ( const char *argu, const char *argl ) );


void oprog_wordlist_check( char *arg, CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * obj, void *vo, int type,
                           OBJ_DATA * iobj );
void set_supermob( OBJ_DATA * obj );
bool oprog_percent_check( CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * obj, void *vo, int type );
void rprog_percent_check( CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * obj, void *vo, int type );
void rprog_wordlist_check( char *arg, CHAR_DATA * mob, CHAR_DATA * actor,
                           OBJ_DATA * obj, void *vo, int type, ROOM_INDEX_DATA * room );


#ifdef DUNNO_STRSTR
char *strstr( s1, s2 )
     const char *s1;
     const char *s2;
{
   char *cp;
   int i, j = strlen( s1 ) - strlen( s2 ), k = strlen( s2 );
   if( j < 0 )
      return NULL;
   for( i = 0; i <= j && strncmp( s1++, s2, k ) != 0; i++ );
   return ( i > j ) ? NULL : ( s1 - 1 );
}
#endif

#define RID ROOM_INDEX_DATA


char *mprog_type_to_name args( ( int type ) );
ch_ret simple_damage( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt );

char *mprog_type_to_name( int type )
{
   switch ( type )
   {
      case IN_FILE_PROG:
         return "in_file_prog";
      case ACT_PROG:
         return "act_prog";
      case SPEECH_PROG:
         return "speech_prog";
      case RAND_PROG:
         return "rand_prog";
      case FIGHT_PROG:
         return "fight_prog";
      case HITPRCNT_PROG:
         return "hitprcnt_prog";
      case DEATH_PROG:
         return "death_prog";
      case ENTRY_PROG:
         return "entry_prog";
      case GREET_PROG:
         return "greet_prog";
      case ALL_GREET_PROG:
         return "all_greet_prog";
      case GIVE_PROG:
         return "give_prog";
      case BRIBE_PROG:
         return "bribe_prog";
      case HOUR_PROG:
         return "hour_prog";
      case TIME_PROG:
         return "time_prog";
      case WEAR_PROG:
         return "wear_prog";
      case REMOVE_PROG:
         return "remove_prog";
      case SAC_PROG:
         return "sac_prog";
      case LOOK_PROG:
         return "look_prog";
      case EXA_PROG:
         return "exa_prog";
      case ZAP_PROG:
         return "zap_prog";
      case GET_PROG:
         return "get_prog";
      case DROP_PROG:
         return "drop_prog";
      case REPAIR_PROG:
         return "repair_prog";
      case DAMAGE_PROG:
         return "damage_prog";
      case PULL_PROG:
         return "pull_prog";
      case PUSH_PROG:
         return "push_prog";
      case SCRIPT_PROG:
         return "script_prog";
      case SLEEP_PROG:
         return "sleep_prog";
      case REST_PROG:
         return "rest_prog";
      case LEAVE_PROG:
         return "leave_prog";
      case USE_PROG:
         return "use_prog";
      default:
         return "ERROR_PROG";
   }
}

void do_mpstat( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   MPROG_DATA *mprg;
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "MProg stat whom?\n\r", ch );
      return;
   }

   if( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if( !IS_NPC( victim ) )
   {
      send_to_char( "Only Mobiles can have MobPrograms!\n\r", ch );
      return;
   }
   if( get_trust( ch ) < LEVEL_BUILD && xIS_SET( victim->act, ACT_STATSHIELD ) )
   {
      set_pager_color( AT_IMMORT, ch );
      send_to_pager( "Their godly glow prevents you from getting a good look.\n\r", ch );
      return;
   }
   if( xIS_EMPTY( victim->pIndexData->progtypes ) )
   {
      send_to_char( "That Mobile has no Programs set.\n\r", ch );
      return;
   }

   ch_printf( ch, "Name: %s.  Vnum: %d.\n\r", victim->name, victim->pIndexData->vnum );

   ch_printf( ch, "Short description: %s.\n\rLong  description: %s",
              victim->short_descr, victim->long_descr[0] != '\0' ? victim->long_descr : "(none).\n\r" );

   ch_printf( ch, "Hp: %d/%d.  Mana: %d/%d.  Move: %d/%d. \n\r",
              victim->hit, victim->max_hit, victim->mana, victim->max_mana, victim->move, victim->max_move );

   ch_printf( ch, "Lv: %d.  Class: %d.  Align: %d.  AC: %d.  Exp: %d ",
              victim->level, victim->class, victim->alignment, GET_AC( victim ), victim->exp );
   ch_printf( ch, "Katyr: %d\n\r", victim->gold );

   for( mprg = victim->pIndexData->mudprogs; mprg; mprg = mprg->next )
      ch_printf( ch, ">%s %s\n\r%s\n\r", mprog_type_to_name( mprg->type ), mprg->arglist, mprg->comlist );
   return;
}

void do_opstat( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   MPROG_DATA *mprg;
   OBJ_DATA *obj;

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "OProg stat what?\n\r", ch );
      return;
   }

   if( ( obj = get_obj_world( ch, arg ) ) == NULL )
   {
      send_to_char( "You cannot find that.\n\r", ch );
      return;
   }

   if( xIS_EMPTY( obj->pIndexData->progtypes ) )
   {
      send_to_char( "That object has no programs set.\n\r", ch );
      return;
   }

   ch_printf( ch, "Name: %s.  Vnum: %d.\n\r", obj->name, obj->pIndexData->vnum );

   ch_printf( ch, "Short description: %s.\n\r", obj->short_descr );

   for( mprg = obj->pIndexData->mudprogs; mprg; mprg = mprg->next )
      ch_printf( ch, ">%s %s\n\r%s\n\r", mprog_type_to_name( mprg->type ), mprg->arglist, mprg->comlist );

   return;

}

void do_rpstat( CHAR_DATA * ch, char *argument )
{
   MPROG_DATA *mprg;

   if( xIS_EMPTY( ch->in_room->progtypes ) )
   {
      send_to_char( "This room has no programs set.\n\r", ch );
      return;
   }

   ch_printf( ch, "Name: %s.  Vnum: %d.\n\r", ch->in_room->name, ch->in_room->vnum );

   for( mprg = ch->in_room->mudprogs; mprg; mprg = mprg->next )
      ch_printf( ch, ">%s %s\n\r%s\n\r", mprog_type_to_name( mprg->type ), mprg->arglist, mprg->comlist );
   return;
}

void do_mpasupress( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int rnds;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   if( arg1[0] == '\0' )
   {
      send_to_char( "Mpasupress who?\n\r", ch );
      progbug( "Mpasupress:  invalid (nonexistent?) argument", ch );
      return;
   }
   if( arg2[0] == '\0' )
   {
      send_to_char( "Supress their attacks for how many rounds?\n\r", ch );
      progbug( "Mpasupress:  invalid (nonexistent?) argument", ch );
      return;
   }
   if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
      send_to_char( "No such victim in the room.\n\r", ch );
      progbug( "Mpasupress:  victim not present", ch );
      return;
   }
   rnds = atoi( arg2 );
   if( rnds < 0 || rnds > 32000 )
   {
      send_to_char( "Invalid number of rounds to supress attacks.\n\r", ch );
      progbug( "Mpsupress:  invalid (nonexistent?) argument", ch );
      return;
   }
   add_timer( victim, TIMER_ASUPRESSED, rnds, NULL, 0 );
   return;
}

void do_mpkill( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;


   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }
   if( !ch )
   {
      bug( "Nonexistent ch in do_mpkill!", 0 );
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      progbug( "MpKill - no argument", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      progbug( "MpKill - Victim not in room", ch );
      return;
   }

   if( victim == ch )
   {
      progbug( "MpKill - Bad victim to attack", ch );
      return;
   }

   if( ch->position == POS_FIGHTING
       || ch->position == POS_EVASIVE
       || ch->position == POS_DEFENSIVE || ch->position == POS_AGGRESSIVE || ch->position == POS_BERSERK )
   {
      progbug( "MpKill - Already fighting", ch );
      return;
   }

   multi_hit( ch, victim, TYPE_UNDEFINED );
   return;
}


void do_mpjunk( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      progbug( "Mpjunk - No argument", ch );
      return;
   }

   if( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
   {
      if( ( obj = get_obj_wear( ch, arg ) ) != NULL )
      {
         unequip_char( ch, obj );
         extract_obj( obj );
         return;
      }
      if( ( obj = get_obj_carry( ch, arg ) ) == NULL )
         return;
      extract_obj( obj );
   }
   else
      for( obj = ch->first_carrying; obj; obj = obj_next )
      {
         obj_next = obj->next_content;
         if( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
         {
            if( obj->wear_loc != WEAR_NONE )
               unequip_char( ch, obj );
            extract_obj( obj );
         }
      }

   return;

}

int get_color( char *argument )
{
   char color[MAX_INPUT_LENGTH];
   char *cptr;
   static char const *color_list = "_bla_red_dgr_bro_dbl_pur_cya_cha_dch_ora_gre_yel_blu_pin_lbl_whi";
//   static char const * blink_list= 
//         "*bla*red*dgr*bro*dbl*pur*cya*cha*dch*ora*gre*yel*blu*pin*lbl*whi"; 

   one_argument( argument, color );
   if( color[0] != '_' /* && color[0]!='*' */  )
      return 0;
   if( ( cptr = strstr( color_list, color ) ) )
      return ( cptr - color_list ) / 4;
//   if ( (cptr = strstr(blink_list, color)) ) 
//     return (cptr - blink_list) / 4 + AT_BLINK; 
   return 0;
}

void do_mpasound( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *was_in_room;
   EXIT_DATA *pexit;
   sh_int color;
   EXT_BV actflags;

   if( !ch )
   {
      bug( "Nonexistent ch in do_mpasound!", 0 );
      return;
   }
   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      progbug( "Mpasound - No argument", ch );
      return;
   }
   actflags = ch->act;
   xREMOVE_BIT( ch->act, ACT_SECRETIVE );
   if( ( color = get_color( argument ) ) )
      argument = one_argument( argument, arg1 );
   was_in_room = ch->in_room;
   for( pexit = was_in_room->first_exit; pexit; pexit = pexit->next )
   {
      if( pexit->to_room && pexit->to_room != was_in_room )
      {
         ch->in_room = pexit->to_room;
         MOBtrigger = FALSE;
         if( color )
            act( color, argument, ch, NULL, NULL, TO_ROOM );
         else
            act( AT_SAY, argument, ch, NULL, NULL, TO_ROOM );
      }
   }
   ch->act = actflags;
   ch->in_room = was_in_room;
   return;
}

void do_mpechoaround( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   EXT_BV actflags;
   sh_int color;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      progbug( "Mpechoaround - No argument", ch );
      return;
   }

   if( !( victim = get_char_room_p( ch, arg ) ) )
   {
      progbug( "Mpechoaround - victim does not exist", ch );
      return;
   }

   actflags = ch->act;
   xREMOVE_BIT( ch->act, ACT_SECRETIVE );

   DONT_UPPER = TRUE;
   if( ( color = get_color( argument ) ) )
   {
      argument = one_argument( argument, arg );
      act( color, argument, ch, NULL, victim, TO_NOTVICT );
   }
   else
      act( AT_ACTION, argument, ch, NULL, victim, TO_NOTVICT );

   DONT_UPPER = FALSE;
   ch->act = actflags;
}


void do_mpechoat( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   EXT_BV actflags;
   sh_int color;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      progbug( "Mpechoat - No argument", ch );
      return;
   }

   if( !( victim = get_char_room_p( ch, arg ) ) )
   {
      progbug( "Mpechoat - victim does not exist", ch );
      return;
   }

   actflags = ch->act;
   xREMOVE_BIT( ch->act, ACT_SECRETIVE );

   DONT_UPPER = TRUE;
   if( argument[0] == '\0' )
      act( AT_ACTION, " ", ch, NULL, victim, TO_VICT );
   else if( ( color = get_color( argument ) ) )
   {
      argument = one_argument( argument, arg );
      act( color, argument, ch, NULL, victim, TO_VICT );
   }
   else
      act( AT_ACTION, argument, ch, NULL, victim, TO_VICT );

   DONT_UPPER = FALSE;

   ch->act = actflags;
}


void do_mpecho( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   sh_int color;
   EXT_BV actflags;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   actflags = ch->act;
   xREMOVE_BIT( ch->act, ACT_SECRETIVE );

   DONT_UPPER = TRUE;
   if( argument[0] == '\0' )
      act( AT_ACTION, " ", ch, NULL, NULL, TO_ROOM );
   else if( ( color = get_color( argument ) ) )
   {
      argument = one_argument( argument, arg1 );
      act( color, argument, ch, NULL, NULL, TO_ROOM );
   }
   else
      act( AT_ACTION, argument, ch, NULL, NULL, TO_ROOM );
   DONT_UPPER = FALSE;
   ch->act = actflags;
}

void do_mpsoundaround( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char sound[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   EXT_BV actflags;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      progbug( "Mpsoundaround - No argument", ch );
      return;
   }

   if( !( victim = get_char_room( ch, arg ) ) )
   {
      progbug( "Mpsoundaround - victim does not exist", ch );
      return;
   }

   actflags = ch->act;
   xREMOVE_BIT( ch->act, ACT_SECRETIVE );

   sprintf( sound, "!!SOUND(%s)\n", argument );
   act( AT_ACTION, sound, ch, NULL, victim, TO_NOTVICT );

   ch->act = actflags;
}

void do_mpsoundat( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char sound[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   EXT_BV actflags;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   argument = one_argument( argument, arg );

   if( arg[0] == '\0' || argument[0] == '\0' )
   {
      progbug( "Mpsoundat - No argument", ch );
      return;
   }

   if( !( victim = get_char_room( ch, arg ) ) )
   {
      progbug( "Mpsoundat - victim does not exist", ch );
      return;
   }

   actflags = ch->act;
   xREMOVE_BIT( ch->act, ACT_SECRETIVE );

   sprintf( sound, "!!SOUND(%s)\n", argument );
   act( AT_ACTION, sound, ch, NULL, victim, TO_VICT );

   ch->act = actflags;
}

void do_mpsound( CHAR_DATA * ch, char *argument )
{
   char sound[MAX_INPUT_LENGTH];
   EXT_BV actflags;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   if( argument[0] == '\0' )
   {
      progbug( "Mpsound - called w/o argument", ch );
      return;
   }

   actflags = ch->act;
   xREMOVE_BIT( ch->act, ACT_SECRETIVE );

   sprintf( sound, "!!SOUND(%s)\n", argument );
   act( AT_ACTION, sound, ch, NULL, NULL, TO_ROOM );

   ch->act = actflags;
}

void do_mpmusicaround( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char music[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   EXT_BV actflags;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      progbug( "Mpmusicaround - No argument", ch );
      return;
   }
   if( !( victim = get_char_room( ch, arg ) ) )
   {
      progbug( "Mpmusicaround - victim does not exist", ch );
      return;
   }
   actflags = ch->act;
   xREMOVE_BIT( ch->act, ACT_SECRETIVE );
   sprintf( music, "!!MUSIC(%s)\n", argument );
   act( AT_ACTION, music, ch, NULL, victim, TO_NOTVICT );
   ch->act = actflags;
   return;
}
void do_mpmusic( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char music[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   EXT_BV actflags;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      progbug( "Mpmusic - No argument", ch );
      return;
   }
   if( !( victim = get_char_room( ch, arg ) ) )
   {
      progbug( "Mpmusic - victim does not exist", ch );
      return;
   }
   actflags = ch->act;
   xREMOVE_BIT( ch->act, ACT_SECRETIVE );
   sprintf( music, "!!MUSIC(%s)\n", argument );
   act( AT_ACTION, music, ch, NULL, victim, TO_ROOM );
   ch->act = actflags;
   return;
}
void do_mpmusicat( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char music[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   EXT_BV actflags;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      progbug( "Mpmusicat - No argument", ch );
      return;
   }
   if( !( victim = get_char_room( ch, arg ) ) )
   {
      progbug( "Mpmusicat - victim does not exist", ch );
      return;
   }
   actflags = ch->act;
   xREMOVE_BIT( ch->act, ACT_SECRETIVE );
   sprintf( music, "!!MUSIC(%s)\n", argument );
   act( AT_ACTION, music, ch, NULL, victim, TO_VICT );
   ch->act = actflags;
   return;
}

void do_mpmload( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   MOB_INDEX_DATA *pMobIndex;
   CHAR_DATA *victim;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' || !is_number( arg ) )
   {
      progbug( "Mpmload - Bad vnum as arg", ch );
      return;
   }

   if( ( pMobIndex = get_mob_index( atoi( arg ) ) ) == NULL )
   {
      progbug( "Mpmload - Bad mob vnum", ch );
      return;
   }

   victim = create_mobile( pMobIndex );
   char_to_room( victim, ch->in_room );
   return;
}

void do_mpoload( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   OBJ_INDEX_DATA *pObjIndex;
   OBJ_DATA *obj;
   int level;
   int timer = 0;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' || !is_number( arg1 ) )
   {
      progbug( "Mpoload - Bad syntax", ch );
      return;
   }

   if( arg2[0] == '\0' )
      level = get_trust( ch );
   else
   {
      if( !is_number( arg2 ) )
      {
         progbug( "Mpoload - Bad level syntax", ch );
         return;
      }
      level = atoi( arg2 );
      if( level < 0 || level > get_trust( ch ) )
      {
         progbug( "Mpoload - Bad level", ch );
         return;
      }

      timer = atoi( argument );
      if( timer < 0 )
      {
         progbug( "Mpoload - Bad timer", ch );
         return;
      }
   }

   if( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
   {
      progbug( "Mpoload - Bad vnum arg", ch );
      return;
   }

   obj = create_object( pObjIndex, level );
   obj->timer = timer;
   if( CAN_WEAR( obj, ITEM_TAKE ) )
      obj_to_char( obj, ch );
   else
      obj_to_room( obj, ch->in_room );

   return;
}

void do_mppardon( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      progbug( "Mppardon:  missing argument", ch );
      send_to_char( "Mppardon who for what?\n\r", ch );
      return;
   }
   if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
      progbug( "Mppardon: offender not present", ch );
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }
   if( IS_NPC( victim ) )
   {
      progbug( "Mppardon:  trying to pardon NPC", ch );
      send_to_char( "Not on NPC's.\n\r", ch );
      return;
   }
   if( !str_cmp( arg2, "attacker" ) )
   {
      if( xIS_SET( victim->act, PLR_ATTACKER ) )
      {
         xREMOVE_BIT( victim->act, PLR_ATTACKER );
         send_to_char( "Attacker flag removed.\n\r", ch );
         send_to_char( "Your crime of attack has been pardoned.\n\r", victim );
      }
      return;
   }
   if( !str_cmp( arg2, "killer" ) )
   {
      if( xIS_SET( victim->act, PLR_KILLER ) )
      {
         xREMOVE_BIT( victim->act, PLR_KILLER );
         send_to_char( "Killer flag removed.\n\r", ch );
         send_to_char( "Your crime of murder has been pardoned.\n\r", victim );
      }
      return;
   }
   if( !str_cmp( arg2, "litterbug" ) )
   {
      if( xIS_SET( victim->act, PLR_LITTERBUG ) )
      {
         xREMOVE_BIT( victim->act, PLR_LITTERBUG );
         send_to_char( "Litterbug flag removed.\n\r", ch );
         send_to_char( "Your crime of littering has been pardoned./n/r", victim );
      }
      return;
   }
   if( !str_cmp( arg2, "thief" ) )
   {
      if( xIS_SET( victim->act, PLR_THIEF ) )
      {
         xREMOVE_BIT( victim->act, PLR_THIEF );
         send_to_char( "Thief flag removed.\n\r", ch );
         send_to_char( "Your crime of theft has been pardoned.\n\r", victim );
      }
      return;
   }
   send_to_char( "Pardon who for what?\n\r", ch );
   progbug( "Mppardon: Invalid argument", ch );
   return;
}

void do_mppurge( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      CHAR_DATA *vnext;

      for( victim = ch->in_room->first_person; victim; victim = vnext )
      {
         vnext = victim->next_in_room;
         if( IS_NPC( victim ) && victim != ch )
            extract_char( victim, TRUE );
      }
      while( ch->in_room->first_content )
         extract_obj( ch->in_room->first_content );

      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      if( ( obj = get_obj_here( ch, arg ) ) != NULL )
         extract_obj( obj );
      else
         progbug( "Mppurge - Bad argument", ch );
      return;
   }

   if( !IS_NPC( victim ) )
   {
      progbug( "Mppurge - Trying to purge a PC", ch );
      return;
   }

   if( victim == ch )
   {
      progbug( "Mppurge - Trying to purge oneself", ch );
      return;
   }

   if( IS_NPC( victim ) && victim->pIndexData->vnum == 3 )
   {
      progbug( "Mppurge: trying to purge supermob", ch );
      return;
   }

   extract_char( victim, TRUE );
   return;
}

void do_mpinvis( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   sh_int level;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   argument = one_argument( argument, arg );
   if( arg && arg[0] != '\0' )
   {
      if( !is_number( arg ) )
      {
         progbug( "Mpinvis - Non numeric argument ", ch );
         return;
      }
      level = atoi( arg );
      if( level < 2 || level > 51 )
      {
         progbug( "MPinvis - Invalid level ", ch );
         return;
      }

      ch->mobinvis = level;
      ch_printf( ch, "Mobinvis level set to %d.\n\r", level );
      return;
   }

   if( ch->mobinvis < 2 )
      ch->mobinvis = ch->level;

   if( xIS_SET( ch->act, ACT_MOBINVIS ) )
   {
      xREMOVE_BIT( ch->act, ACT_MOBINVIS );
      act( AT_IMMORT, "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "You slowly fade back into existence.\n\r", ch );
   }
   else
   {
      xSET_BIT( ch->act, ACT_MOBINVIS );
      act( AT_IMMORT, "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "You slowly vanish into thin air.\n\r", ch );
   }
   return;
}

void do_mpgoto( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *location;
   CHAR_DATA *fch;
   CHAR_DATA *fch_next;
   ROOM_INDEX_DATA *in_room;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      progbug( "Mpgoto - No argument", ch );
      return;
   }

   if( ( location = find_location( ch, arg ) ) == NULL )
   {
      progbug( "Mpgoto - No such location", ch );
      return;
   }

   in_room = ch->in_room;
   if( ch->fighting )
      stop_fighting( ch, TRUE );
   char_from_room( ch );
   if( ch->on )
   {
      ch->on = NULL;
      ch->position = POS_STANDING;
   }
   if( ch->position != POS_STANDING )
   {
      ch->position = POS_STANDING;
   }
   char_to_room( ch, location );
   for( fch = in_room->first_person; fch; fch = fch_next )
   {
      fch_next = fch->next_in_room;
      if( fch->mount && fch->mount == ch )
      {
         char_from_room( fch );
         char_to_room( fch, location );
      }
   }
   return;
}

void do_mpat( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *location;
   ROOM_INDEX_DATA *original;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   argument = one_argument( argument, arg );

   if( arg[0] == '\0' || argument[0] == '\0' )
   {
      progbug( "Mpat - Bad argument", ch );
      return;
   }

   if( ( location = find_location( ch, arg ) ) == NULL )
   {
      progbug( "Mpat - No such location", ch );
      return;
   }

   original = ch->in_room;
   char_from_room( ch );
   char_to_room( ch, location );
   interpret( ch, argument );

   if( !char_died( ch ) )
   {
      char_from_room( ch );
      char_to_room( ch, original );
   }

   return;
}

void do_mpadvance( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int level;
   int iLevel;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      progbug( "Mpadvance - Bad syntax", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      progbug( "Mpadvance - Victim not there", ch );
      return;
   }

   if( IS_NPC( victim ) )
   {
      progbug( "Mpadvance - Victim is NPC", ch );
      return;
   }

   if( victim->level >= LEVEL_AVATAR )
      return;

   level = victim->level + 1;

   if( victim->level > ch->level )
   {
      act( AT_TELL, "$n tells you, 'Sorry... you must seek someone more powerful than I.'", ch, NULL, victim, TO_VICT );
      return;
   }

   if( victim->level >= LEVEL_AVATAR )
   {
      set_char_color( AT_IMMORT, victim );
      act( AT_IMMORT, "$n makes some arcane gestures with $s hands, then points $s fingers at you!",
           ch, NULL, victim, TO_VICT );
      act( AT_IMMORT, "$n makes some arcane gestures with $s hands, then points $s fingers at $N!",
           ch, NULL, victim, TO_NOTVICT );
      set_char_color( AT_WHITE, victim );
      send_to_char( "You suddenly feel very strange...\n\r\n\r", victim );
      set_char_color( AT_LBLUE, victim );
   }

   switch ( level )
   {
      default:
         send_to_char( "You feel more powerful!\n\r", victim );
         break;
      case LEVEL_IMMORTAL:
         do_help( victim, "M_GODLVL1_" );
         set_char_color( AT_WHITE, victim );
         send_to_char( "You awake... all your possessions are gone.\n\r", victim );

         while( victim->first_carrying )
            extract_obj( victim->first_carrying );
         break;
      case LEVEL_BUILD:
         do_help( victim, "M_GODLVL5_" );
         break;
      case LEVEL_ADVBUILD:
         do_help( victim, "M_GODLVL9_" );
         break;
      case LEVEL_HEADBUILD:
         do_help( victim, "M_GODLVL11_" );
         break;
      case LEVEL_ADMINADVISOR:
         do_help( victim, "M_GODLVL15_" );
   }

   for( iLevel = victim->level; iLevel < level; )
   {
      if( level < LEVEL_IMMORTAL )
         send_to_char( "You raise a level!!  ", victim );
      victim->level += 1;
      iLevel++;
      advance_level( victim, iLevel == level );
   }
   victim->exp = 1000 * UMAX( 1, victim->level );
   victim->trust = 0;
   return;
}

void do_mptransfer( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   ROOM_INDEX_DATA *location;
   CHAR_DATA *victim;
   CHAR_DATA *nextinroom;
   CHAR_DATA *immortal;
   DESCRIPTOR_DATA *d;


   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' )
   {
      progbug( "Mptransfer - Bad syntax", ch );
      return;
   }

   if( !str_cmp( arg1, "all" ) )
   {
      for( victim = ch->in_room->first_person; victim; victim = nextinroom )
      {
         nextinroom = victim->next_in_room;
         if( IS_IMMORTAL( ch ) && ch->master != NULL )
            continue;
         if( victim != ch && !NOT_AUTHED( victim ) && can_see( ch, victim ) )
         {
            sprintf( buf, "%s %s", victim->name, arg2 );
            do_mptransfer( ch, buf );
         }
      }
      return;
   }
   if( !str_cmp( arg1, "area" ) )
   {
      for( d = first_descriptor; d; d = d->next )
      {
         if( !d->character || ( d->connected != CON_PLAYING &&
                                d->connected != CON_EDITING ) || !can_see( ch, d->character )
             || ch->in_room->area != d->character->in_room->area || NOT_AUTHED( d->character ) )
            continue;
         sprintf( buf, "%s %s", d->character->name, arg2 );
         do_mptransfer( ch, buf );
      }
      return;
   }

   if( arg2[0] == '\0' )
   {
      location = ch->in_room;
   }
   else
   {
      if( ( location = find_location( ch, arg2 ) ) == NULL )
      {
         progbug( "Mptransfer - No such location", ch );
         return;
      }

      if( room_is_private( location ) )
      {
         progbug( "Mptransfer - Private room", ch );
         return;
      }
   }

   if( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      progbug( "Mptransfer - No such person", ch );
      return;
   }

   if( !victim->in_room )
   {
      progbug( "Mptransfer - Victim in Limbo", ch );
      return;
   }

   if( NOT_AUTHED( victim ) && location->area != victim->in_room->area )
   {
      sprintf( buf, "Mptransfer - unauthed char (%s)", victim->name );
      progbug( buf, ch );
      return;
   }


   if( !in_hard_range( victim, location->area ) && !xIS_SET( location->room_flags, ROOM_PROTOTYPE ) )
      return;

   if( victim->fighting )
      stop_fighting( victim, TRUE );


   for( immortal = victim->in_room->first_person; immortal; immortal = nextinroom )
   {
      nextinroom = immortal->next_in_room;
      if( IS_NPC( immortal ) || get_trust( immortal ) < LEVEL_IMMORTAL || immortal->master != victim )
         continue;
      if( immortal->fighting )
         stop_fighting( immortal, TRUE );
      char_from_room( immortal );
      char_to_room( immortal, location );
   }

   char_from_room( victim );
   if( victim->on )
   {
      victim->on = NULL;
      victim->position = POS_STANDING;
   }
   if( victim->position != POS_STANDING )
   {
      victim->position = POS_STANDING;
   }
   char_to_room( victim, location );
   do_look( victim, "auto" );
   return;
}

void do_mpforce( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   argument = one_argument( argument, arg );

   if( arg[0] == '\0' || argument[0] == '\0' )
   {
      progbug( "Mpforce - Bad syntax", ch );
      return;
   }

   if( !str_cmp( arg, "all" ) )
   {
      CHAR_DATA *vch;

      for( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
         if( get_trust( vch ) < get_trust( ch ) )
            interpret( vch, argument );
   }
   else
   {
      CHAR_DATA *victim;

      if( ( victim = get_char_room_p( ch, arg ) ) == NULL )
      {
         progbug( "Mpforce - No such victim", ch );
         return;
      }

      if( victim == ch )
      {
         progbug( "Mpforce - Forcing oneself", ch );
         return;
      }

      if( !IS_NPC( victim ) && ( !victim->desc ) && IS_IMMORTAL( victim ) )
      {
         progbug( "Mpforce - Attempting to force link dead immortal", ch );
         return;
      }


      interpret( victim, argument );
   }

   return;
}

void do_mpnuisance( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   char arg1[MAX_STRING_LENGTH];
   struct tm *now_time;

   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg1 );

   if( arg1[0] == '\0' )
   {
      progbug( "Mpnuisance - called w/o enough argument(s)", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
      send_to_char( "Victim must be in room.\n\r", ch );
      progbug( "Mpnuisance: victim not in room", ch );
      return;
   }
   if( IS_NPC( victim ) )
   {
      progbug( "Mpnuisance: victim is a mob", ch );
      return;
   }
   if( IS_IMMORTAL( victim ) )
   {
      progbug( "Mpnuisance: not allowed on immortals", ch );
      return;
   }
   if( victim->pcdata->nuisance )
   {
      progbug( "Mpnuisance: victim is already nuisanced", ch );
      return;
   }
   CREATE( victim->pcdata->nuisance, NUISANCE_DATA, 1 );
   victim->pcdata->nuisance->time = current_time;
   victim->pcdata->nuisance->flags = 1;
   victim->pcdata->nuisance->power = 2;
   now_time = localtime( &current_time );
   now_time->tm_mday += 1;
   victim->pcdata->nuisance->max_time = mktime( now_time );
   add_timer( victim, TIMER_NUISANCE, ( 28800 * 2 ), NULL, 0 );
   return;
}

void do_mpunnuisance( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   TIMER *timer, *timer_next;
   char arg1[MAX_STRING_LENGTH];

   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg1 );

   if( arg1[0] == '\0' )
   {
      progbug( "Mpunnuisance - called w/o enough argument(s)", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
      send_to_char( "Victim must be in room.\n\r", ch );
      progbug( "Mpunnuisance: victim not in room", ch );
      return;
   }

   if( IS_NPC( victim ) )
   {
      progbug( "Mpunnuisance: victim was a mob", ch );
      return;
   }

   if( IS_IMMORTAL( victim ) )
   {
      progbug( "Mpunnuisance: victim was an immortal", ch );
      return;
   }

   if( !ch->pcdata->nuisance )
   {
      progbug( "Mpunnuisance: victim is not nuisanced", ch );
      return;
   }
   for( timer = victim->first_timer; timer; timer = timer_next )
   {
      timer_next = timer->next;
      if( timer->type == TIMER_NUISANCE )
         extract_timer( victim, timer );
   }
   DISPOSE( victim->pcdata->nuisance );
   return;
}

void do_mpbodybag( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   char arg[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];
   char buf4[MAX_STRING_LENGTH];


   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      progbug( "Mpbodybag - called w/o enough argument(s)", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "Victim must be in room.\n\r", ch );
      progbug( "Mpbodybag: victim not in room", ch );
      return;
   }
   if( IS_NPC( victim ) )
   {
      progbug( "Mpbodybag: bodybagging a npc corpse", ch );
      return;
   }
   sprintf( buf3, " " );
   sprintf( buf2, "the corpse of %s", arg );
   for( obj = first_object; obj; obj = obj->next )
   {
      if( obj->in_room && !str_cmp( buf2, obj->short_descr ) && ( obj->pIndexData->vnum == 11 ) )
      {
         obj_from_room( obj );
         obj = obj_to_char( obj, ch );
         obj->timer = -1;
      }
   }
   sprintf( buf4, "Mpbodybag: Grabbed %s", buf2 );
   progbug( buf4, ch );
   return;
}

void do_mpmorph( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   MORPH_DATA *morph;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];

   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      progbug( "Mpmorph - called w/o enough argument(s)", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
      send_to_char( "Victim must be in room.\n\r", ch );
      progbug( "Mpmorph: victim not in room", ch );
      return;
   }


   if( !is_number( arg2 ) )
      morph = get_morph( arg2 );
   else
      morph = get_morph_vnum( atoi( arg2 ) );
   if( !morph )
   {
      progbug( "Mpmorph - unknown morph", ch );
      return;
   }
   if( victim->morph )
   {
      progbug( "Mpmorph - victim already morphed", ch );
      return;
   }
   do_morph_char( victim, morph );
   return;
}

void do_mpunmorph( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   char arg[MAX_STRING_LENGTH];

   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      progbug( "Mpmorph - called w/o an argument", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "Victim must be in room.\n\r", ch );
      progbug( "Mpunmorph: victim not in room", ch );
      return;
   }
   if( !victim->morph )
   {
      progbug( "Mpunmorph: victim not morphed", ch );
      return;
   }
   do_unmorph_char( victim );
   return;
}

void do_mpechozone( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   sh_int color;
   EXT_BV actflags;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   actflags = ch->act;
   xREMOVE_BIT( ch->act, ACT_SECRETIVE );
   if( ( color = get_color( argument ) ) )
      argument = one_argument( argument, arg1 );
   DONT_UPPER = TRUE;
   for( vch = first_char; vch; vch = vch_next )
   {
      vch_next = vch->next;
      if( vch->in_room->area == ch->in_room->area && !IS_NPC( vch ) && IS_AWAKE( vch ) )
      {
         if( argument[0] == '\0' )
            act( AT_ACTION, " ", vch, NULL, NULL, TO_CHAR );
         else if( color )
            act( color, argument, vch, NULL, NULL, TO_CHAR );
         else
            act( AT_ACTION, argument, vch, NULL, NULL, TO_CHAR );
      }
   }
   DONT_UPPER = FALSE;
   ch->act = actflags;
}

void do_mp_practice( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int sn, max, tmp, adept;
   char *skill_name;

   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   if( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
   {
      send_to_char( "Mppractice: bad syntax", ch );
      progbug( "Mppractice - Bad syntax", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
      send_to_char( "Mppractice: Student not in room? Invis?", ch );
      progbug( "Mppractice: Invalid student not in room", ch );
      return;
   }

   if( ( sn = skill_lookup( arg2 ) ) < 0 )
   {
      send_to_char( "Mppractice: Invalid spell/skill name", ch );
      progbug( "Mppractice: Invalid spell/skill name", ch );
      return;
   }


   if( IS_NPC( victim ) )
   {
      send_to_char( "Mppractice: Can't train a mob", ch );
      progbug( "Mppractice: Can't train a mob", ch );
      return;
   }

   skill_name = skill_table[sn]->name;

   max = atoi( arg3 );
   if( ( max < 0 ) || ( max > 100 ) )
   {
      sprintf( log_buf, "mp_practice: Invalid maxpercent: %d", max );
      send_to_char( log_buf, ch );
      progbug( log_buf, ch );
      return;
   }

   if( victim->level < find_skill_level( ch, sn ) )
   {
      sprintf( buf, "$n attempts to tutor you in %s, but it's beyond your comprehension.", skill_name );
      act( AT_TELL, buf, ch, NULL, victim, TO_VICT );
      return;
   }

   adept = GET_ADEPT( victim, sn );

   if( ( victim->pcdata->learned[sn] >= adept ) || ( victim->pcdata->learned[sn] >= max ) )
   {
      sprintf( buf, "$n shows some knowledge of %s, but yours is clearly superior.", skill_name );
      act( AT_TELL, buf, ch, NULL, victim, TO_VICT );
      return;
   }


   tmp = UMIN( victim->pcdata->learned[sn] + int_app[get_curr_int( victim )].learn, max );
   act( AT_ACTION, "$N demonstrates $t to you.  You feel more learned in this subject.", victim, skill_table[sn]->name, ch,
        TO_CHAR );

   victim->pcdata->learned[sn] = max;


   if( victim->pcdata->learned[sn] >= adept )
   {
      victim->pcdata->learned[sn] = adept;
      act( AT_TELL, "$n tells you, 'You have learned all I know on this subject...'", ch, NULL, victim, TO_VICT );
   }
   return;

}

void do_mpscatter( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_STRING_LENGTH];
   char arg2[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   ROOM_INDEX_DATA *pRoomIndex;
   int low_vnum, high_vnum, rvnum;

   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   if( arg1[0] == '\0' )
   {
      send_to_char( "Mpscatter whom?\n\r", ch );
      progbug( "Mpscatter: invalid (nonexistent?) argument", ch );
      return;
   }
   if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
      send_to_char( "Victim must be in room.\n\r", ch );
      progbug( "Mpscatter: victim not in room", ch );
      return;
   }
   if( IS_IMMORTAL( victim ) && get_trust( victim ) >= get_trust( ch ) )
   {
      send_to_char( "You haven't the power to succeed against this victim.\n\r", ch );
      progbug( "Mpscatter: victim level too high", ch );
      return;
   }
   if( arg2[0] == '\0' )
   {
      send_to_char( "You must specify a low vnum.\n\r", ch );
      progbug( "Mpscatter:  missing low vnum", ch );
      return;
   }
   if( argument[0] == '\0' )
   {
      send_to_char( "You must specify a high vnum.\n\r", ch );
      progbug( "Mpscatter:  missing high vnum", ch );
      return;
   }
   low_vnum = atoi( arg2 );
   high_vnum = atoi( argument );
   if( low_vnum < 1 || high_vnum < low_vnum || low_vnum > high_vnum || low_vnum == high_vnum || high_vnum > MAX_VNUMS )
   {
      send_to_char( "Invalid range.\n\r", ch );
      progbug( "Mpscatter:  invalid range", ch );
      return;
   }
   while( 1 )
   {
      rvnum = number_range( low_vnum, high_vnum );
      pRoomIndex = get_room_index( rvnum );
      if( pRoomIndex )
         break;
   }
   if( victim->fighting )
      stop_fighting( victim, TRUE );
   char_from_room( victim );
   char_to_room( victim, pRoomIndex );
   victim->position = POS_RESTING;
   do_look( victim, "auto" );
   return;
}

void do_mp_slay( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   if( arg1[0] == '\0' )
   {
      send_to_char( "mpslay whom?\n\r", ch );
      progbug( "Mpslay: invalid (nonexistent?) argument", ch );
      return;
   }

   if( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "Victim must be in world.\n\r", ch );
      progbug( "Mpslay: victim not in world", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "You try to slay yourself.  You fail.\n\r", ch );
      progbug( "Mpslay: trying to slay self", ch );
      return;
   }

   if( IS_NPC( victim ) && victim->pIndexData->vnum == 3 )
   {
      send_to_char( "You cannot slay supermob!\n\r", ch );
      progbug( "Mpslay: trying to slay supermob", ch );
      return;
   }

   if( victim->level < LEVEL_IMMORTAL )
   {
      act( AT_IMMORT, "You slay $M in cold blood!", ch, NULL, victim, TO_CHAR );
      act( AT_IMMORT, "$n slays you in cold blood!", ch, NULL, victim, TO_VICT );
      act( AT_IMMORT, "$n slays $N in cold blood!", ch, NULL, victim, TO_NOTVICT );
      set_cur_char( victim );
      raw_kill( ch, victim, TRUE );
      stop_fighting( ch, FALSE );
      stop_hating( ch );
      stop_fearing( ch );
      stop_hunting( ch );
   }
   else
   {
      act( AT_IMMORT, "You attempt to slay $M and fail!", ch, NULL, victim, TO_CHAR );
      act( AT_IMMORT, "$n attempts to slay you.  What a kneebiter!", ch, NULL, victim, TO_VICT );
      act( AT_IMMORT, "$n attempts to slay $N.  Needless to say $e fails.", ch, NULL, victim, TO_NOTVICT );
   }
   return;
}

void do_mp_damage( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   CHAR_DATA *nextinroom;
   int dam;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   if( arg1[0] == '\0' )
   {
      send_to_char( "mpdamage whom?\n\r", ch );
      progbug( "Mpdamage: invalid argument1", ch );
      return;
   }
   if( !str_cmp( arg1, "all" ) )
   {
      for( victim = ch->in_room->first_person; victim; victim = nextinroom )
      {
         nextinroom = victim->next_in_room;
         if( victim != ch && can_see( ch, victim ) )
         {
            sprintf( buf, "'%s' %s", victim->name, arg2 );
            do_mp_damage( ch, buf );
         }
      }
      return;
   }
   if( arg2[0] == '\0' )
   {
      send_to_char( "mpdamage inflict how many hps?\n\r", ch );
      progbug( "Mpdamage: invalid argument2", ch );
      return;
   }
   if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
      send_to_char( "Victim must be in room.\n\r", ch );
      progbug( "Mpdamage: victim not in room", ch );
      return;
   }
   if( victim == ch )
   {
      send_to_char( "You can't mpdamage yourself.\n\r", ch );
      progbug( "Mpdamage: trying to damage self", ch );
      return;
   }
   dam = atoi( arg2 );
   if( ( dam < 0 ) || ( dam > 32000 ) )
   {
      send_to_char( "Mpdamage how much?\n\r", ch );
      progbug( "Mpdamage: invalid (nonexistent?) argument", ch );
      return;
   }
   if( simple_damage( ch, victim, dam, TYPE_UNDEFINED ) == rVICT_DIED )
   {
      stop_fighting( ch, FALSE );
      stop_hating( ch );
      stop_fearing( ch );
      stop_hunting( ch );
   }
   return;
}

void do_mp_log( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   struct tm *t = localtime( &current_time );

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      progbug( "Mp_log:  non-existent entry", ch );
      return;
   }
   sprintf( buf, "&p%-2.2d/%-2.2d | %-2.2d:%-2.2d  &P%s:  &p%s",
            t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, ch->short_descr, argument );
   append_to_file( MOBLOG_FILE, buf );
   return;
}

void do_mp_restore( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int hp;

   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' )
   {
      send_to_char( "mprestore whom?\n\r", ch );
      progbug( "Mprestore: invalid argument1", ch );
      return;
   }

   if( arg2[0] == '\0' )
   {
      send_to_char( "mprestore how many hps?\n\r", ch );
      progbug( "Mprestore: invalid argument2", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
      send_to_char( "Victim must be in room.\n\r", ch );
      progbug( "Mprestore: victim not in room", ch );
      return;
   }

   hp = atoi( arg2 );

   if( ( hp < 0 ) || ( hp > 32000 ) )
   {
      send_to_char( "Mprestore how much?\n\r", ch );
      progbug( "Mprestore: invalid (nonexistent?) argument", ch );
      return;
   }
   hp += victim->hit;
   victim->hit = ( hp > 32000 || hp < 0 || hp > victim->max_hit ) ? victim->max_hit : hp;
}

void do_mpfavor( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int favor;
   char *tmp;
   bool plus = FALSE, minus = FALSE;

   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' )
   {
      send_to_char( "mpfavor whom?\n\r", ch );
      progbug( "Mpfavor: invalid argument1", ch );
      return;
   }

   if( arg2[0] == '\0' )
   {
      send_to_char( "mpfavor how much favor?\n\r", ch );
      progbug( "Mpfavor: invalid argument2", ch );
      return;
   }

   tmp = arg2;
   if( tmp[0] == '+' )
   {
      plus = TRUE;
      tmp++;
      if( tmp[0] == '\0' )
      {
         send_to_char( "mpfavor how much favor?\n\r", ch );
         progbug( "Mpfavor: invalid argument2", ch );
         return;
      }
   }
   else if( tmp[0] == '-' )
   {
      minus = TRUE;
      tmp++;
      if( tmp[0] == '\0' )
      {
         send_to_char( "mpfavor how much favor?\n\r", ch );
         progbug( "Mpfavor: invalid argument2", ch );
         return;
      }
   }
   if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
      send_to_char( "Victim must be in room.\n\r", ch );
      progbug( "Mpfavor: victim not in room", ch );
      return;
   }

   favor = atoi( tmp );
   if( plus )
      victim->pcdata->favor = URANGE( -2500, victim->pcdata->favor + favor, 2500 );
   else if( minus )
      victim->pcdata->favor = URANGE( -2500, victim->pcdata->favor - favor, 2500 );
   else
      victim->pcdata->favor = URANGE( -2500, favor, 2500 );
}

void do_mp_open_passage( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *targetRoom, *fromRoom;
   int targetRoomVnum, fromRoomVnum, exit_num;
   EXIT_DATA *pexit;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   if( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
   {
      progbug( "MpOpenPassage - Bad syntax", ch );
      return;
   }

   if( !is_number( arg1 ) )
   {
      progbug( "MpOpenPassage - Bad syntax", ch );
      return;
   }

   fromRoomVnum = atoi( arg1 );
   if( ( fromRoom = get_room_index( fromRoomVnum ) ) == NULL )
   {
      progbug( "MpOpenPassage - Bad syntax", ch );
      return;
   }

   if( !is_number( arg2 ) )
   {
      progbug( "MpOpenPassage - Bad syntax", ch );
      return;
   }

   targetRoomVnum = atoi( arg2 );
   if( ( targetRoom = get_room_index( targetRoomVnum ) ) == NULL )
   {
      progbug( "MpOpenPassage - Bad syntax", ch );
      return;
   }

   if( !is_number( arg3 ) )
   {
      progbug( "MpOpenPassage - Bad syntax", ch );
      return;
   }

   exit_num = atoi( arg3 );
   if( ( exit_num < 0 ) || ( exit_num > MAX_DIR ) )
   {
      progbug( "MpOpenPassage - Bad syntax", ch );
      return;
   }

   if( ( pexit = get_exit( fromRoom, exit_num ) ) != NULL )
   {
      if( !IS_SET( pexit->exit_info, EX_PASSAGE ) )
         return;
      progbug( "MpOpenPassage - Exit exists", ch );
      return;
   }

   pexit = make_exit( fromRoom, targetRoom, exit_num );
   pexit->keyword = STRALLOC( "" );
   pexit->description = STRALLOC( "" );
   pexit->key = -1;
   pexit->exit_info = EX_PASSAGE;

   return;
}


void do_mp_fill_in( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   EXIT_DATA *pexit;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( ( pexit = find_door( ch, arg, TRUE ) ) == NULL )
   {
      progbug( "MpFillIn - Exit does not exist", ch );
      return;
   }
   SET_BIT( pexit->exit_info, EX_CLOSED );
   return;
}

void do_mp_close_passage( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *fromRoom;
   int fromRoomVnum, exit_num;
   EXIT_DATA *pexit;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   if( arg1[0] == '\0' || arg2[0] == '\0' || arg2[0] == '\0' )
   {
      progbug( "MpClosePassage - Bad syntax", ch );
      return;
   }

   if( !is_number( arg1 ) )
   {
      progbug( "MpClosePassage - Bad syntax", ch );
      return;
   }

   fromRoomVnum = atoi( arg1 );
   if( ( fromRoom = get_room_index( fromRoomVnum ) ) == NULL )
   {
      progbug( "MpClosePassage - Bad syntax", ch );
      return;
   }

   if( !is_number( arg2 ) )
   {
      progbug( "MpClosePassage - Bad syntax", ch );
      return;
   }

   exit_num = atoi( arg2 );
   if( ( exit_num < 0 ) || ( exit_num > MAX_DIR ) )
   {
      progbug( "MpClosePassage - Bad syntax", ch );
      return;
   }

   if( ( pexit = get_exit( fromRoom, exit_num ) ) == NULL )
   {
      return;
   }

   if( !IS_SET( pexit->exit_info, EX_PASSAGE ) )
   {
      progbug( "MpClosePassage - Exit not a passage", ch );
      return;
   }

   extract_exit( fromRoom, pexit );

   return;
}

void do_mpnothing( CHAR_DATA * ch, char *argument )
{
   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   return;
}

void do_mpdream( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_STRING_LENGTH];
   CHAR_DATA *vict;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   argument = one_argument( argument, arg1 );

   if( ( vict = get_char_world( ch, arg1 ) ) == NULL )
   {
      progbug( "Mpdream: No such character", ch );
      return;
   }

   if( vict->position <= POS_SLEEPING )
   {
      send_to_char( argument, vict );
      send_to_char( "\n\r", vict );
   }
   return;
}

void do_mpapply( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;

   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      progbug( "Mpapply - bad syntax", ch );
      return;
   }

   if( ( victim = get_char_room( ch, argument ) ) == NULL )
   {
      progbug( "Mpapply - no such player in room.", ch );
      return;
   }

   if( !victim->desc )
   {
      send_to_char( "Not on linkdeads.\n\r", ch );
      return;
   }

   if( !NOT_AUTHED( victim ) )
      return;

   if( victim->pcdata->auth_state >= 1 )
      return;

   sprintf( log_buf, "%s@%s new %s %s %s applying...",
            victim->name, victim->desc->host,
            race_table[victim->race]->race_name,
            class_table[victim->class]->who_name, IS_PKILL( victim ) ? "(Deadly)" : "(Peaceful)" );
   to_channel( log_buf, CHANNEL_AUTH, "Auth", LEVEL_IMMORTAL, sysdata.log_level );
   victim->pcdata->auth_state = 1;
   return;
}

void do_mpapplyb( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;


   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      progbug( "Mpapplyb - bad syntax", ch );
      return;
   }

   if( ( victim = get_char_room( ch, argument ) ) == NULL )
   {
      progbug( "Mpapplyb - no such player in room.", ch );
      return;
   }

   if( !victim->desc )
   {
      send_to_char( "Not on linkdeads.\n\r", ch );
      return;
   }

   if( !NOT_AUTHED( victim ) )
      return;

   if( get_timer( victim, TIMER_APPLIED ) >= 1 )
      return;

   switch ( victim->pcdata->auth_state )
   {
      case 0:
      case 1:
      default:
         send_to_char( "\n\rYou attempt to gain the gods' attention.\n\r", victim );
         if( IS_SET( victim->pcdata->flags, PCFLAG_EXP ) )
         {
            sprintf( log_buf, "%s @ %s new %s %s applying...",
                     victim->name, victim->desc->host,
                     race_table[victim->race]->race_name, class_table[victim->class]->who_name );
         }
         log_string( log_buf );
         to_channel( log_buf, CHANNEL_AUTH, "Auth", LEVEL_IMMORTAL, victim->level );
         add_timer( victim, TIMER_APPLIED, 10, NULL, 0 );
         victim->pcdata->auth_state = 1;
         break;

      case 2:
         send_to_char
            ( "Your name has been deemed unsuitable by the gods.  Please choose a more medieval name with the 'name' command.\n\r",
              victim );
         add_timer( victim, TIMER_APPLIED, 10, NULL, 0 );
         break;

      case 3:
         ch_printf( victim, "The gods permit you to enter the %s.\n\r", sysdata.mud_name );
         REMOVE_BIT( victim->pcdata->flags, PCFLAG_UNAUTHED );
         if( victim->fighting )
            stop_fighting( victim, TRUE );
         char_from_room( victim );
         char_to_room( victim, get_room_index( ROOM_VNUM_SCHOOL ) );
         act( AT_WHITE, "$n enters this world from within a column of blinding light!", victim, NULL, NULL, TO_ROOM );
         do_look( victim, "auto" );
         break;
   }

   return;
}

void do_mp_deposit( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   int gold, money, tmpvalue;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      progbug( "Mpdeposit - bad syntax", ch );
      return;
   }
   gold = atoi( arg );
   money = ch->gold;
   if( gold <= money && ch->in_room )
   {
      tmpvalue = money - gold;
      conv_currency( ch, tmpvalue );
      boost_economy( ch->in_room->area, gold );
   }
}

void do_mp_withdraw( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   int gold, money, tmpvalue;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }


   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      progbug( "Mpwithdraw - bad syntax", ch );
      return;
   }
   gold = atoi( arg );
   if( ch->gold < 1000000000 && gold < 1000000000 && ch->in_room && economy_has( ch->in_room->area, gold ) )
   {
      money = get_value( ch->gold, ch->silver, ch->copper );
      tmpvalue = money + gold;
      conv_currency( ch, tmpvalue );
      lower_economy( ch->in_room->area, gold );
   }
}

void do_mpdelay( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int delay;

   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );
   if( !*arg )
   {
      send_to_char( "Delay for how many rounds?n\r", ch );
      progbug( "Mpdelay: no duration specified", ch );
      return;
   }
   if( !( victim = get_char_room( ch, arg ) ) )
   {
      send_to_char( "They aren't here.\n\r", ch );
      progbug( "Mpdelay: target not in room", ch );
      return;
   }
   if( IS_IMMORTAL( victim ) )
   {
      send_to_char( "Not against immortals.\n\r", ch );
      progbug( "Mpdelay: target is immortal", ch );
      return;
   }
   argument = one_argument( argument, arg );
   if( !*arg || !is_number( arg ) )
   {
      send_to_char( "Delay them for how many rounds?\n\r", ch );
      progbug( "Mpdelay: invalid (nonexistant?) argument", ch );
      return;
   }
   delay = atoi( arg );
   if( delay < 1 || delay > 30 )
   {
      send_to_char( "Argument out of range.\n\r", ch );
      progbug( "Mpdelay:  argument out of range (1 to 30)", ch );
      return;
   }
   WAIT_STATE( victim, delay * PULSE_VIOLENCE );
   send_to_char( "Mpdelay applied.\n\r", ch );
   return;
}

void do_mppeace( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *rch;
   CHAR_DATA *victim;

   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );
   if( !*arg )
   {
      send_to_char( "Who do you want to mppeace?\n\r", ch );
      progbug( "Mppeace: invalid (nonexistent?) argument", ch );
      return;
   }
   if( !str_cmp( arg, "all" ) )
   {
      for( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
      {
         if( rch->fighting )
         {
            stop_fighting( rch, TRUE );
            do_sit( rch, "" );
         }
         stop_hating( rch );
         stop_hunting( rch );
         stop_fearing( rch );
      }
      send_to_char( "Ok.\n\r", ch );
      return;
   }
   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They must be in the room.n\r", ch );
      progbug( "Mppeace: target not in room", ch );
      return;
   }
   if( victim->fighting )
      stop_fighting( victim, TRUE );
   stop_hating( ch );
   stop_hunting( ch );
   stop_fearing( ch );
   stop_hating( victim );
   stop_hunting( victim );
   stop_fearing( victim );
   send_to_char( "Ok.\n\r", ch );
   return;
}

void do_mppkset( CHAR_DATA * ch, char *argument )
{
   CHAR_DATA *victim;
   char arg[MAX_STRING_LENGTH];

   if( !IS_NPC( ch ) || ch->desc || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_JMT ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if( argument[0] == '\0' || arg[0] == '\0' )
   {
      progbug( "Mppkset - bad syntax", ch );
      return;
   }

   if( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      progbug( "Mppkset - no such player in room.", ch );
      return;
   }

   if( !str_cmp( argument, "yes" ) || !str_cmp( argument, "y" ) )
   {
      if( !IS_SET( victim->pcdata->flags, PCFLAG_DEADLY ) )
         SET_BIT( victim->pcdata->flags, PCFLAG_DEADLY );
   }
   else if( !str_cmp( argument, "no" ) || !str_cmp( argument, "n" ) )
   {
      if( IS_SET( victim->pcdata->flags, PCFLAG_DEADLY ) )
         REMOVE_BIT( victim->pcdata->flags, PCFLAG_DEADLY );
   }
   else
   {
      progbug( "Mppkset - bad syntax", ch );
      return;
   }
   return;
}

ch_ret simple_damage( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt )
{
   sh_int dameq;
   bool npcvict;
   OBJ_DATA *damobj;
   ch_ret retcode;


   retcode = rNONE;

   if( !ch )
   {
      bug( "Damage: null ch!", 0 );
      return rERROR;
   }
   if( !victim )
   {
      progbug( "Damage: null victim!", ch );
      return rVICT_DIED;
   }

   if( victim->position == POS_DEAD )
   {
      return rVICT_DIED;
   }

   npcvict = IS_NPC( victim );

   if( dam )
   {
      if( IS_FIRE( dt ) )
         dam = ris_damage( victim, dam, RIS_FIRE );
      else if( IS_COLD( dt ) )
         dam = ris_damage( victim, dam, RIS_COLD );
      else if( IS_ACID( dt ) )
         dam = ris_damage( victim, dam, RIS_ACID );
      else if( IS_ELECTRICITY( dt ) )
         dam = ris_damage( victim, dam, RIS_ELECTRICITY );
      else if( IS_ENERGY( dt ) )
         dam = ris_damage( victim, dam, RIS_ENERGY );
      else if( dt == gsn_poison )
         dam = ris_damage( victim, dam, RIS_POISON );
      else if( dt == ( TYPE_HIT + 7 ) || dt == ( TYPE_HIT + 8 ) )
         dam = ris_damage( victim, dam, RIS_BLUNT );
      else if( dt == ( TYPE_HIT + 2 ) || dt == ( TYPE_HIT + 11 ) )
         dam = ris_damage( victim, dam, RIS_PIERCE );
      else if( dt == ( TYPE_HIT + 1 ) || dt == ( TYPE_HIT + 3 ) )
         dam = ris_damage( victim, dam, RIS_SLASH );
      if( dam < 0 )
         dam = 0;
   }

   if( victim != ch )
   {
      if( IS_AFFECTED( victim, AFF_DEMONFIRE ) )
         dam *= 2;

      if( IS_AFFECTED( victim, AFF_SANCTUARY ) )
         dam /= 2;

      if( IS_AFFECTED( victim, AFF_PROTECT ) && IS_EVIL( ch ) )
         dam -= ( int )( dam / 4 );

      if( dam < 0 )
         dam = 0;

   }


   if( dam > 10 )
   {
      dameq = number_range( WEAR_LIGHT, WEAR_EYES );
      damobj = get_eq_char( victim, dameq );
      if( damobj )
      {
         if( dam > get_obj_resistance( damobj ) )
         {
            set_cur_obj( damobj );
            damage_obj( damobj );
         }
      }
   }

   victim->hit -= dam;
   if( !IS_NPC( victim ) && victim->level >= LEVEL_IMMORTAL && victim->hit < 1 )
      victim->hit = 1;

   if( !npcvict && get_trust( victim ) >= LEVEL_IMMORTAL && get_trust( ch ) >= LEVEL_IMMORTAL && victim->hit < 1 )
      victim->hit = 1;
   update_pos( victim );

   switch ( victim->position )
   {
      case POS_MORTAL:
         act( AT_DYING, "$n is mortally wounded, and will die soon, if not aided.", victim, NULL, NULL, TO_ROOM );
         act( AT_DANGER, "}RYou are mortally wounded, and will die soon, if not aided.&D", victim, NULL, NULL, TO_CHAR );
         break;

      case POS_INCAP:
         act( AT_DYING, "$n is incapacitated and will slowly die, if not aided.", victim, NULL, NULL, TO_ROOM );
         act( AT_DANGER, "}RYou are incapacitated and will slowly die, if not aided.&D", victim, NULL, NULL, TO_CHAR );
         break;

      case POS_STUNNED:
         if( !IS_AFFECTED( victim, AFF_PARALYSIS ) )
         {
            act( AT_ACTION, "$n is stunned, but will probably recover.", victim, NULL, NULL, TO_ROOM );
            act( AT_HURT, "You are stunned, but will probably recover.", victim, NULL, NULL, TO_CHAR );
         }
         break;

      case POS_DEAD:
         act( AT_DEAD, "$n is DEAD!!", victim, 0, 0, TO_ROOM );
         act( AT_DEAD, "You have been KILLED!!\n\r", victim, 0, 0, TO_CHAR );
         break;

      default:
         if( dam > victim->max_hit / 4 )
            act( AT_HURT, "That really did HURT!", victim, 0, 0, TO_CHAR );
         if( victim->hit < victim->max_hit / 4 )
            act( AT_DANGER, "}RYou wish that your wounds would stop BLEEDING so much!&D", victim, 0, 0, TO_CHAR );
         break;
   }

   if( victim->position == POS_DEAD )
   {
      if( !npcvict )
      {
         sprintf( log_buf, "%s (%d) killed by %s at %d",
                  victim->name, victim->level, ( IS_NPC( ch ) ? ch->short_descr : ch->name ), victim->in_room->vnum );
         log_string( log_buf );
         to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL, ch->level );

         if( victim->level < 100 )
         {
            if( victim->exp > exp_level( victim, victim->level ) )
            {
               gain_exp( victim, ( exp_level( victim, victim->level ) - victim->exp ) / 2 );
            }
         }
         else
         {
            if( victim->exp > exp_level( victim, victim->sublevel ) )
            {
               gain_exp( victim, ( exp_level( victim, victim->sublevel ) - victim->exp ) / 2 );
            }
         }
         if( !IS_NPC( victim ) && IS_BOUNTIED( victim ) )
         {
            xREMOVE_BIT( victim->act, PLR_BOUNTIED );
         }
      }
      set_cur_char( victim );
      raw_kill( ch, victim, TRUE );
      victim = NULL;

      return rVICT_DIED;
   }

   if( victim == ch )
      return rNONE;

   if( !npcvict && !victim->desc )
   {
      if( number_range( 0, victim->wait ) == 0 )
      {
         do_recall( victim, "" );
         return rNONE;
      }
   }

   if( npcvict && dam > 0 )
   {
      if( ( xIS_SET( victim->act, ACT_WIMPY ) && number_bits( 1 ) == 0
            && victim->hit < victim->max_hit / 2 )
          || ( ( IS_AFFECTED( victim, AFF_CHARM ) || IS_AFFECTED( victim, AFF_JMT ) ) && victim->master
               && victim->master->in_room != victim->in_room ) )
      {
         start_fearing( victim, ch );
         stop_hunting( victim );
         do_flee( victim, "" );
      }
   }

   if( !npcvict && victim->hit > 0 && victim->hit <= victim->wimpy && victim->wait == 0 )
      do_flee( victim, "" );
   else if( !npcvict && xIS_SET( victim->act, PLR_FLEE ) )
      do_flee( victim, "" );

   tail_chain(  );
   return rNONE;
}

void init_supermob(  )
{
   RID *office;

   supermob = create_mobile( get_mob_index( 3 ) );
   office = get_room_index( 3 );
   char_to_room( supermob, office );

#ifdef NOTDEFD
   CREATE( supermob, CHAR_DATA, 1 );
   clear_char( supermob );

   xSET_BIT( supermob->act, ACT_IS_NPC );
   supermob->name = STRALLOC( "supermob" );
   supermob->short_descr = STRALLOC( "supermob" );
   supermob->long_descr = STRALLOC( "supermob is here" );

   CREATE( supermob_index, MOB_INDEX_DATA, 1 )
#endif
}


#undef RID


char *mprog_next_command( char *clist )
{

   char *pointer = clist;

   while( *pointer != '\n' && *pointer != '\0' )
      pointer++;
   if( *pointer == '\n' )
      *pointer++ = '\0';
   if( *pointer == '\r' )
      *pointer++ = '\0';

   return ( pointer );

}

bool mprog_seval( char *lhs, char *opr, char *rhs, CHAR_DATA * mob )
{

   if( !str_cmp( opr, "==" ) )
      return ( bool ) ( !str_cmp( lhs, rhs ) );
   if( !str_cmp( opr, "!=" ) )
      return ( bool ) ( str_cmp( lhs, rhs ) );
   if( !str_cmp( opr, "/" ) )
      return ( bool ) ( !str_infix( rhs, lhs ) );
   if( !str_cmp( opr, "!/" ) )
      return ( bool ) ( str_infix( rhs, lhs ) );

   sprintf( log_buf, "Improper MOBprog operator '%s'", opr );
   progbug( log_buf, mob );
   return 0;

}

bool mprog_veval( int lhs, char *opr, int rhs, CHAR_DATA * mob )
{

   if( !str_cmp( opr, "==" ) )
      return ( lhs == rhs );
   if( !str_cmp( opr, "!=" ) )
      return ( lhs != rhs );
   if( !str_cmp( opr, ">" ) )
      return ( lhs > rhs );
   if( !str_cmp( opr, "<" ) )
      return ( lhs < rhs );
   if( !str_cmp( opr, "<=" ) )
      return ( lhs <= rhs );
   if( !str_cmp( opr, ">=" ) )
      return ( lhs >= rhs );
   if( !str_cmp( opr, "&" ) )
      return ( lhs & rhs );
   if( !str_cmp( opr, "|" ) )
      return ( lhs | rhs );

   sprintf( log_buf, "Improper MOBprog operator '%s'", opr );
   progbug( log_buf, mob );

   return 0;

}

#define isoperator(c) ((c)=='='||(c)=='<'||(c)=='>'||(c)=='!'||(c)=='&'||(c)=='|')
#define MAX_IF_ARGS 6
int mprog_do_ifcheck( char *ifcheck, CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * obj, void *vo, CHAR_DATA * rndm )
{
   char buf[MAX_STRING_LENGTH];
   char opr[MAX_INPUT_LENGTH];
   char *chck, *cvar;
   char *argv[MAX_IF_ARGS];
   char *rval = "";
   char *q, *p = buf;
   int argc = 0;
   CHAR_DATA *chkchar = NULL;
   OBJ_DATA *chkobj = NULL;
   int lhsvl, rhsvl = 0;

   if( !*ifcheck )
   {
      progbug( "Null ifcheck", mob );
      return BERR;
   }

   strcpy( buf, ifcheck );
   opr[0] = '\0';
   while( isspace( *p ) )
      ++p;
   argv[argc++] = p;
   while( isalnum( *p ) )
      ++p;
   while( isspace( *p ) )
      *p++ = '\0';
   if( *p != '(' )
   {
      progbug( "Ifcheck Syntax error (missing left bracket)", mob );
      return BERR;
   }

   *p++ = '\0';
   while( isspace( *p ) )
      *p++ = '\0';
   for( ;; )
   {
      argv[argc++] = p;
      while( *p == '$' || isalnum( *p ) )
         ++p;
      while( isspace( *p ) )
         *p++ = '\0';
      switch ( *p )
      {
         case ',':
            *p++ = '\0';
            while( isspace( *p ) )
               *p++ = '\0';
            if( argc >= MAX_IF_ARGS )
            {
               while( *p && *p != ')' )
                  ++p;
               if( *p )
                  *p++ = '\0';
               while( isspace( *p ) )
                  *p++ = '\0';
               goto doneargs;
            }
            break;
         case ')':
            *p++ = '\0';
            while( isspace( *p ) )
               *p++ = '\0';
            goto doneargs;
            break;
         default:
            progbug( "Ifcheck Syntax warning (missing right bracket)", mob );
            goto doneargs;
            break;
      }
   }
 doneargs:
   q = p;
   while( isoperator( *p ) )
      ++p;
   strncpy( opr, q, p - q );
   opr[p - q] = '\0';
   while( isspace( *p ) )
      *p++ = '\0';
   rval = p;
   while( *p )
      ++p;
   *p = '\0';

   chck = argv[0] ? argv[0] : "";
   cvar = argv[1] ? argv[1] : "";

   if( cvar[0] == '$' )
   {
      switch ( cvar[1] )
      {
         case 'i':
            chkchar = mob;
            break;
         case 'n':
            chkchar = actor;
            break;
         case 't':
            chkchar = ( CHAR_DATA * ) vo;
            break;
         case 'r':
            chkchar = rndm;
            break;
         case 'o':
            chkobj = obj;
            break;
         case 'p':
            chkobj = ( OBJ_DATA * ) vo;
            break;
         default:
            sprintf( rval, "Bad argument '%c' to '%s'", cvar[0], chck );
            progbug( rval, mob );
            return BERR;
      }
      if( !chkchar && !chkobj )
         return BERR;
   }
   if( !str_cmp( chck, "rand" ) )
   {
      return ( number_percent(  ) <= atoi( cvar ) );
   }
   if( !str_cmp( chck, "economy" ) )
   {
      int idx = atoi( cvar );
      ROOM_INDEX_DATA *room;

      if( !idx )
      {
         if( !mob->in_room )
         {
            progbug( "'economy' ifcheck: mob in NULL room with no room vnum " "argument", mob );
            return BERR;
         }
         room = mob->in_room;
      }
      else
         room = get_room_index( idx );
      if( !room )
      {
         progbug( "Bad room vnum passed to 'economy'", mob );
         return BERR;
      }
      return mprog_veval( ( ( room->area->high_economy > 0 ) ? 1000000000 : 0 )
                          + room->area->low_economy, opr, atoi( rval ), mob );
   }
   if( !str_cmp( chck, "mobinarea" ) )
   {
      int vnum = atoi( cvar );
      int lhsvl;
      int world_count;
      int found_count;
      CHAR_DATA *tmob;
      MOB_INDEX_DATA *m_index;

      if( vnum < 1 || vnum > MAX_VNUMS )
      {
         progbug( "Bad vnum to 'mobinarea'", mob );
         return BERR;
      }

      m_index = get_mob_index( vnum );

      if( !m_index )
         world_count = 0;
      else
         world_count = m_index->count;

      lhsvl = 0;
      found_count = 0;

      for( tmob = first_char; tmob && found_count != world_count; tmob = tmob->next )
      {
         if( IS_NPC( tmob ) && tmob->pIndexData->vnum == vnum )
         {
            found_count++;

            if( tmob->in_room->area == mob->in_room->area )
               lhsvl++;
         }
      }
      rhsvl = atoi( rval );

      if( rhsvl < 0 )
         rhsvl = 0;
      if( !*opr )
         strcpy( opr, "==" );

      return mprog_veval( lhsvl, opr, rhsvl, mob );
   }

   if( !str_cmp( chck, "mobinroom" ) )
   {
      int vnum = atoi( cvar );
      int lhsvl;
      CHAR_DATA *oMob;

      if( vnum < 1 || vnum > MAX_VNUMS )
      {
         progbug( "Bad vnum to 'mobinroom'", mob );
         return BERR;
      }
      lhsvl = 0;
      for( oMob = mob->in_room->first_person; oMob; oMob = oMob->next_in_room )
         if( IS_NPC( oMob ) && oMob->pIndexData->vnum == vnum )
            lhsvl++;
      rhsvl = atoi( rval );
      if( rhsvl < 0 )
         rhsvl = 0;
      if( !*opr )
         strcpy( opr, "==" );
      return mprog_veval( lhsvl, opr, rhsvl, mob );
   }

   if( !str_cmp( chck, "mobinworld" ) )
   {
      int vnum = atoi( cvar );
      int lhsvl;
      MOB_INDEX_DATA *m_index;

      if( vnum < 1 || vnum > MAX_VNUMS )
      {
         progbug( "Bad vnum to 'mobinworld'", mob );
         return BERR;
      }

      m_index = get_mob_index( vnum );

      if( !m_index )
         lhsvl = 0;
      else
         lhsvl = m_index->count;

      rhsvl = atoi( rval );

      if( rhsvl < 0 )
         rhsvl = 0;
      if( !*opr )
         strcpy( opr, "==" );

      return mprog_veval( lhsvl, opr, rhsvl, mob );
   }
   if( !str_cmp( chck, "timeskilled" ) )
   {
      MOB_INDEX_DATA *pMob;

      if( chkchar )
         pMob = chkchar->pIndexData;
      else if( !( pMob = get_mob_index( atoi( cvar ) ) ) )
      {
         progbug( "TimesKilled ifcheck: bad vnum", mob );
         return BERR;
      }
      return mprog_veval( pMob->killed, opr, atoi( rval ), mob );
   }
   if( !str_cmp( chck, "ovnumhere" ) )
   {
      OBJ_DATA *pObj;
      int vnum = atoi( cvar );

      if( vnum < 1 || vnum > MAX_VNUMS )
      {
         progbug( "OvnumHere: bad vnum", mob );
         return BERR;
      }
      lhsvl = 0;

      for( pObj = mob->first_carrying; pObj; pObj = pObj->next_content )
         if( pObj->pIndexData->vnum == vnum )
            lhsvl += pObj->count;
      for( pObj = mob->in_room->first_content; pObj; pObj = pObj->next_content )
         if( pObj->pIndexData->vnum == vnum )
            lhsvl += pObj->count;
      rhsvl = is_number( rval ) ? atoi( rval ) : -1;
      if( rhsvl < 0 )
         rhsvl = 0;
      if( !*opr )
         strcpy( opr, "==" );
      return mprog_veval( lhsvl, opr, rhsvl, mob );
   }
   if( !str_cmp( chck, "otypehere" ) )
   {
      OBJ_DATA *pObj;
      int type;

      if( is_number( cvar ) )
         type = atoi( cvar );
      else
         type = get_otype( cvar );
      if( type < 0 || type > MAX_ITEM_TYPE )
      {
         progbug( "OtypeHere: bad type", mob );
         return BERR;
      }
      lhsvl = 0;

      for( pObj = mob->first_carrying; pObj; pObj = pObj->next_content )
         if( pObj->item_type == type )
            lhsvl += pObj->count;
      for( pObj = mob->in_room->first_content; pObj; pObj = pObj->next_content )
         if( pObj->item_type == type )
            lhsvl += pObj->count;
      rhsvl = is_number( rval ) ? atoi( rval ) : -1;
      if( rhsvl < 0 )
         rhsvl = 0;
      if( !*opr )
         strcpy( opr, "==" );
      return mprog_veval( lhsvl, opr, rhsvl, mob );
   }
   if( !str_cmp( chck, "ovnumroom" ) )
   {
      OBJ_DATA *pObj;
      int vnum = atoi( cvar );

      if( vnum < 1 || vnum > MAX_VNUMS )
      {
         progbug( "OvnumRoom: bad vnum", mob );
         return BERR;
      }
      lhsvl = 0;
      for( pObj = mob->in_room->first_content; pObj; pObj = pObj->next_content )
         if( pObj->pIndexData->vnum == vnum )
            lhsvl += pObj->count;
      rhsvl = is_number( rval ) ? atoi( rval ) : -1;
      if( rhsvl < 0 )
         rhsvl = 0;
      if( !*opr )
         strcpy( opr, "==" );
      return mprog_veval( lhsvl, opr, rhsvl, mob );
   }
   if( !str_cmp( chck, "otyperoom" ) )
   {
      OBJ_DATA *pObj;
      int type;

      if( is_number( cvar ) )
         type = atoi( cvar );
      else
         type = get_otype( cvar );
      if( type < 0 || type > MAX_ITEM_TYPE )
      {
         progbug( "OtypeRoom: bad type", mob );
         return BERR;
      }
      lhsvl = 0;

      for( pObj = mob->in_room->first_content; pObj; pObj = pObj->next_content )
         if( pObj->item_type == type )
            lhsvl += pObj->count;
      rhsvl = is_number( rval ) ? atoi( rval ) : -1;
      if( rhsvl < 0 )
         rhsvl = 0;
      if( !*opr )
         strcpy( opr, "==" );
      return mprog_veval( lhsvl, opr, rhsvl, mob );
   }
   if( !str_cmp( chck, "ovnumcarry" ) )
   {
      OBJ_DATA *pObj;
      int vnum = atoi( cvar );

      if( vnum < 1 || vnum > MAX_VNUMS )
      {
         progbug( "OvnumCarry: bad vnum", mob );
         return BERR;
      }
      lhsvl = 0;

      for( pObj = mob->first_carrying; pObj; pObj = pObj->next_content )
         if( pObj->pIndexData->vnum == vnum )
            lhsvl += pObj->count;
      rhsvl = is_number( rval ) ? atoi( rval ) : -1;

      if( rhsvl < 0 )
         rhsvl = 0;
      if( !*opr )
         strcpy( opr, "==" );
      return mprog_veval( lhsvl, opr, rhsvl, mob );
   }
   if( !str_cmp( chck, "otypecarry" ) )
   {
      OBJ_DATA *pObj;
      int type;

      if( is_number( cvar ) )
         type = atoi( cvar );
      else
         type = get_otype( cvar );
      if( type < 0 || type > MAX_ITEM_TYPE )
      {
         progbug( "OtypeCarry: bad type", mob );
         return BERR;
      }
      lhsvl = 0;
      for( pObj = mob->first_carrying; pObj; pObj = pObj->next_content )
         if( pObj->item_type == type )
            lhsvl += pObj->count;
      rhsvl = is_number( rval ) ? atoi( rval ) : -1;
      if( rhsvl < 0 )
         rhsvl = 0;
      if( !*opr )
         strcpy( opr, "==" );
      return mprog_veval( lhsvl, opr, rhsvl, mob );
   }
   if( !str_cmp( chck, "ovnumwear" ) )
   {
      OBJ_DATA *pObj;
      int vnum = atoi( cvar );

      if( vnum < 1 || vnum > MAX_VNUMS )
      {
         progbug( "OvnumWear: bad vnum", mob );
         return BERR;
      }
      lhsvl = 0;
      for( pObj = mob->first_carrying; pObj; pObj = pObj->next_content )
         if( pObj->wear_loc != WEAR_NONE && pObj->pIndexData->vnum == vnum )
            lhsvl += pObj->count;
      rhsvl = is_number( rval ) ? atoi( rval ) : -1;
      if( rhsvl < 0 )
         rhsvl = 0;
      if( !*opr )
         strcpy( opr, "==" );
      return mprog_veval( lhsvl, opr, rhsvl, mob );
   }
   if( !str_cmp( chck, "otypewear" ) )
   {
      OBJ_DATA *pObj;
      int type;

      if( is_number( cvar ) )
         type = atoi( cvar );
      else
         type = get_otype( cvar );
      if( type < 0 || type > MAX_ITEM_TYPE )
      {
         progbug( "OtypeWear: bad type", mob );
         return BERR;
      }
      lhsvl = 0;
      for( pObj = mob->first_carrying; pObj; pObj = pObj->next_content )
         if( pObj->wear_loc != WEAR_NONE && pObj->item_type == type )
            lhsvl += pObj->count;
      rhsvl = is_number( rval ) ? atoi( rval ) : -1;
      if( rhsvl < 0 )
         rhsvl = 0;
      if( !*opr )
         strcpy( opr, "==" );
      return mprog_veval( lhsvl, opr, rhsvl, mob );
   }
   if( !str_cmp( chck, "ovnuminv" ) )
   {
      OBJ_DATA *pObj;
      int vnum = atoi( cvar );

      if( vnum < 1 || vnum > MAX_VNUMS )
      {
         progbug( "OvnumInv: bad vnum", mob );
         return BERR;
      }
      lhsvl = 0;

      for( pObj = mob->first_carrying; pObj; pObj = pObj->next_content )
         if( pObj->wear_loc == WEAR_NONE && pObj->pIndexData->vnum == vnum )
            lhsvl += pObj->count;
      rhsvl = is_number( rval ) ? atoi( rval ) : -1;
      if( rhsvl < 0 )
         rhsvl = 0;
      if( !*opr )
         strcpy( opr, "==" );
      return mprog_veval( lhsvl, opr, rhsvl, mob );
   }
   if( !str_cmp( chck, "otypeinv" ) )
   {
      OBJ_DATA *pObj;
      int type;

      if( is_number( cvar ) )
         type = atoi( cvar );
      else
         type = get_otype( cvar );
      if( type < 0 || type > MAX_ITEM_TYPE )
      {
         progbug( "OtypeInv: bad type", mob );
         return BERR;
      }
      lhsvl = 0;
      for( pObj = mob->first_carrying; pObj; pObj = pObj->next_content )
         if( pObj->wear_loc == WEAR_NONE && pObj->item_type == type )
            lhsvl += pObj->count;
      rhsvl = is_number( rval ) ? atoi( rval ) : -1;
      if( rhsvl < 0 )
         rhsvl = 0;
      if( !*opr )
         strcpy( opr, "==" );
      return mprog_veval( lhsvl, opr, rhsvl, mob );
   }
   if( chkchar )
   {
      if( !str_cmp( chck, "ispacifist" ) )
      {
         return ( IS_NPC( chkchar ) && xIS_SET( chkchar->act, ACT_PACIFIST ) );
      }
      if( !str_cmp( chck, "ismobinvis" ) )
      {
         return ( IS_NPC( chkchar ) && xIS_SET( chkchar->act, ACT_MOBINVIS ) );
      }
      if( !str_cmp( chck, "mobinvislevel" ) )
      {
         return ( IS_NPC( chkchar ) ? mprog_veval( chkchar->mobinvis, opr, atoi( rval ), mob ) : FALSE );
      }
      if( !str_cmp( chck, "ispc" ) )
      {
         return IS_NPC( chkchar ) ? FALSE : TRUE;
      }
      if( !str_cmp( chck, "isnpc" ) )
      {
         return IS_NPC( chkchar ) ? TRUE : FALSE;
      }
      if( !str_cmp( chck, "cansee" ) )
      {
         return can_see( mob, chkchar );
      }
      if( !str_cmp( chck, "ispassage" ) )
      {
         if( find_door( chkchar, rval, TRUE ) == NULL )
            return FALSE;
         else
            return TRUE;
      }
      if( !str_cmp( chck, "isopen" ) )
      {
         EXIT_DATA *pexit;

         if( ( pexit = find_door( chkchar, rval, TRUE ) ) == NULL )
            return FALSE;
         if( !IS_SET( pexit->exit_info, EX_CLOSED ) )
            return TRUE;
         return FALSE;
      }
      if( !str_cmp( chck, "islocked" ) )
      {
         EXIT_DATA *pexit;

         if( ( pexit = find_door( chkchar, rval, TRUE ) ) == NULL )
            return FALSE;
         if( IS_SET( pexit->exit_info, EX_LOCKED ) )
            return TRUE;
         return FALSE;
      }
      if( !str_cmp( chck, "ispkill" ) )
      {
         return IS_PKILL( chkchar ) ? TRUE : FALSE;
      }
      if( !str_cmp( chck, "isdevoted" ) )
      {
         return IS_DEVOTED( chkchar ) ? TRUE : FALSE;
      }
      if( !str_cmp( chck, "canpkill" ) )
      {
         return CAN_PKILL( chkchar ) ? TRUE : FALSE;
      }
      if( !str_cmp( chck, "ismounted" ) )
      {
         return ( chkchar->position == POS_MOUNTED );
      }
      if( !str_cmp( chck, "ismorphed" ) )
      {
         return ( chkchar->morph != NULL ) ? TRUE : FALSE;
      }
      if( !str_cmp( chck, "isnuisance" ) )
      {
         return ( !IS_NPC( chkchar ) ? chkchar->pcdata->nuisance ? TRUE : FALSE : FALSE );
      }
      if( !str_cmp( chck, "isgood" ) )
      {
         return IS_GOOD( chkchar ) ? TRUE : FALSE;
      }
      if( !str_cmp( chck, "isneutral" ) )
      {
         return IS_NEUTRAL( chkchar ) ? TRUE : FALSE;
      }
      if( !str_cmp( chck, "isevil" ) )
      {
         return IS_EVIL( chkchar ) ? TRUE : FALSE;
      }
      if( !str_cmp( chck, "isfight" ) )
      {
         return who_fighting( chkchar ) ? TRUE : FALSE;
      }
      if( !str_cmp( chck, "isimmort" ) )
      {
         return ( get_trust( chkchar ) >= LEVEL_IMMORTAL );
      }
      if( !str_cmp( chck, "ischarmed" ) )
      {
         return IS_AFFECTED( chkchar, AFF_CHARM ) ? TRUE : FALSE;
      }
      if( !str_cmp( chck, "isflying" ) )
      {
         return IS_AFFECTED( chkchar, AFF_FLYING ) ? TRUE : FALSE;
      }
      if( !str_cmp( chck, "isthief" ) )
      {
         return ( !IS_NPC( chkchar ) && xIS_SET( chkchar->act, PLR_THIEF ) );
      }
      if( !str_cmp( chck, "isattacker" ) )
      {
         return ( !IS_NPC( chkchar ) && xIS_SET( chkchar->act, PLR_ATTACKER ) );
      }
      if( !str_cmp( chck, "iskiller" ) )
      {
         return ( !IS_NPC( chkchar ) && xIS_SET( chkchar->act, PLR_KILLER ) );
      }
      if( !str_cmp( chck, "isfollow" ) )
      {
         return ( chkchar->master != NULL && chkchar->master->in_room == chkchar->in_room );
      }
      if( !str_cmp( chck, "isaffected" ) )
      {
         int value = get_aflag( rval );

         if( value < 0 || value > MAX_BITS )
         {
            progbug( "Unknown affect being checked", mob );
            return BERR;
         }
         return IS_AFFECTED( chkchar, value ) ? TRUE : FALSE;
      }
      if( !str_cmp( chck, "numfighting" ) )
      {
         return mprog_veval( chkchar->num_fighting - 1, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "hitprcnt" ) )
      {
         return mprog_veval( chkchar->hit / chkchar->max_hit, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "inroom" ) )
      {
         return mprog_veval( chkchar->in_room->vnum, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "wasinroom" ) )
      {
         if( !chkchar->was_in_room )
            return FALSE;
         return mprog_veval( chkchar->was_in_room->vnum, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "norecall" ) )
      {
         return xIS_SET( chkchar->in_room->room_flags, ROOM_NO_RECALL ) ? TRUE : FALSE;
      }
      if( !str_cmp( chck, "sex" ) )
      {
         return mprog_veval( chkchar->sex, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "position" ) )
      {
         return mprog_veval( chkchar->position, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "doingquest" ) )
      {
         return IS_NPC( chkchar ) ? FALSE : mprog_veval( chkchar->pcdata->quest_number, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "ishelled" ) )
      {
         return IS_NPC( chkchar ) ? FALSE : mprog_veval( chkchar->pcdata->release_date, opr, atoi( rval ), mob );
      }

      if( !str_cmp( chck, "level" ) )
      {
         return mprog_veval( get_trust( chkchar ), opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "goldamt" ) )
      {
         return mprog_veval( chkchar->gold, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "class" ) )
      {
         if( IS_NPC( chkchar ) )
            return mprog_seval( npc_class[chkchar->class], opr, rval, mob );
         return mprog_seval( ( char * )class_table[chkchar->class]->who_name, opr, rval, mob );
      }
      if( !str_cmp( chck, "weight" ) )
      {
         return mprog_veval( chkchar->carry_weight, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "hostdesc" ) )
      {
         if( IS_NPC( chkchar ) || !chkchar->desc->host )
            return FALSE;
         return mprog_seval( chkchar->desc->host, opr, rval, mob );
      }
      if( !str_cmp( chck, "multi" ) )
      {
         CHAR_DATA *ch;
         int lhsvl = 0;

         for( ch = first_char; ch; ch = ch->next )
            if( !IS_NPC( chkchar ) && !IS_NPC( ch )
                && ch->desc && chkchar->desc && QUICKMATCH( ch->desc->host, chkchar->desc->host ) )
               lhsvl++;
         rhsvl = atoi( rval );
         if( rhsvl < 0 )
            rhsvl = 0;
         if( !*opr )
            strcpy( opr, "==" );
         return mprog_veval( lhsvl, opr, rhsvl, mob );
      }
      if( !str_cmp( chck, "race" ) )
      {
         if( IS_NPC( chkchar ) )
            return mprog_seval( npc_race[chkchar->race], opr, rval, mob );
         return mprog_seval( ( char * )race_table[chkchar->race]->race_name, opr, rval, mob );
      }
      if( !str_cmp( chck, "morph" ) )
      {
         if( chkchar->morph == NULL )
            return FALSE;
         if( chkchar->morph->morph == NULL )
            return FALSE;
         return mprog_veval( chkchar->morph->morph->vnum, opr, rhsvl, mob );
      }
      if( !str_cmp( chck, "nuisance" ) )
      {
         if( IS_NPC( chkchar ) || !chkchar->pcdata->nuisance )
            return FALSE;
         return mprog_veval( chkchar->pcdata->nuisance->flags, opr, rhsvl, mob );
      }
      if( !str_cmp( chck, "clan" ) )
      {
         if( IS_NPC( chkchar ) || !chkchar->pcdata->clan )
            return FALSE;
         return mprog_seval( chkchar->pcdata->clan->name, opr, rval, mob );
      }
      if( !str_cmp( chck, "isleader" ) )
      {
         CLAN_DATA *temp;
         if( IS_NPC( chkchar ) )
            return FALSE;
         if( ( temp = get_clan( rval ) ) == NULL )
            return FALSE;
         if( mprog_seval( chkchar->name, opr, temp->leader, mob )
             || mprog_seval( chkchar->name, opr, temp->number1, mob )
             || mprog_seval( chkchar->name, opr, temp->number2, mob ) )
            return TRUE;
         else
            return FALSE;
      }

      if( !str_cmp( chck, "wearing" ) )
      {
         OBJ_DATA *obj;
         int i = 0;
         for( obj = chkchar->first_carrying; obj; obj = obj->next_content )
         {
            i++;
            if( chkchar == obj->carried_by && obj->wear_loc > -1 && !str_cmp( rval, item_w_flags[obj->wear_loc] ) )
               return TRUE;
         }
         return FALSE;
      }
      if( !str_cmp( chck, "wearingvnum" ) )
      {
         OBJ_DATA *obj;

         if( !is_number( rval ) )
            return FALSE;
         for( obj = chkchar->first_carrying; obj; obj = obj->next_content )
         {
            if( chkchar == obj->carried_by && obj->wear_loc > -1 && obj->pIndexData->vnum == atoi( rval ) )
               return TRUE;
         }
         return FALSE;
      }

      if( !str_cmp( chck, "carryingvnum" ) )
      {
         int vnum;

         if( !is_number( rval ) )
            return FALSE;
         vnum = atoi( rval );
         if( !chkchar->first_carrying )
            return FALSE;
         return ( carryingvnum_visit( chkchar, chkchar->first_carrying, vnum ) );
      }

      if( !str_cmp( chck, "isclanleader" ) )
      {
         CLAN_DATA *temp;
         if( IS_NPC( chkchar ) )
            return FALSE;
         if( ( temp = get_clan( rval ) ) == NULL )
            return FALSE;
         if( mprog_seval( chkchar->name, opr, temp->leader, mob ) )
            return TRUE;
         else
            return FALSE;
      }
      if( !str_cmp( chck, "isclan1" ) )
      {
         CLAN_DATA *temp;
         if( IS_NPC( chkchar ) )
            return FALSE;
         if( ( temp = get_clan( rval ) ) == NULL )
            return FALSE;
         if( mprog_seval( chkchar->name, opr, temp->number1, mob ) )
            return TRUE;
         else
            return FALSE;
      }
      if( !str_cmp( chck, "isclan2" ) )
      {
         CLAN_DATA *temp;
         if( IS_NPC( chkchar ) )
            return FALSE;
         if( ( temp = get_clan( rval ) ) == NULL )
            return FALSE;
         if( mprog_seval( chkchar->name, opr, temp->number2, mob ) )
            return TRUE;
         else
            return FALSE;
      }
      if( !str_cmp( chck, "council" ) )
      {
         if( IS_NPC( chkchar ) || !chkchar->pcdata->council )
            return FALSE;
         return mprog_seval( chkchar->pcdata->council->name, opr, rval, mob );
      }
      if( !str_cmp( chck, "deity" ) )
      {
         if( IS_NPC( chkchar ) || !chkchar->pcdata->deity )
            return FALSE;
         return mprog_seval( chkchar->pcdata->deity->name, opr, rval, mob );
      }
      if( !str_cmp( chck, "guild" ) )
      {
         if( IS_NPC( chkchar ) || !IS_GUILDED( chkchar ) )
            return FALSE;
         return mprog_seval( chkchar->pcdata->clan->name, opr, rval, mob );
      }
      if( !str_cmp( chck, "clantype" ) )
      {
         if( IS_NPC( chkchar ) || !chkchar->pcdata->clan )
            return FALSE;
         return mprog_veval( chkchar->pcdata->clan->clan_type, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "waitstate" ) )
      {
         if( IS_NPC( chkchar ) || !chkchar->wait )
            return FALSE;
         return mprog_veval( chkchar->wait, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "asupressed" ) )
      {
         return mprog_veval( get_timer( chkchar, TIMER_ASUPRESSED ), opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "favor" ) )
      {
         if( IS_NPC( chkchar ) || !chkchar->pcdata->favor )
            return FALSE;
         return mprog_veval( chkchar->pcdata->favor, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "hps" ) )
      {
         return mprog_veval( chkchar->hit, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "mana" ) )
      {
         return mprog_veval( chkchar->mana, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "str" ) )
      {
         return mprog_veval( get_curr_str( chkchar ), opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "wis" ) )
      {
         return mprog_veval( get_curr_wis( chkchar ), opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "int" ) )
      {
         return mprog_veval( get_curr_int( chkchar ), opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "dex" ) )
      {
         return mprog_veval( get_curr_dex( chkchar ), opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "con" ) )
      {
         return mprog_veval( get_curr_con( chkchar ), opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "cha" ) )
      {
         return mprog_veval( get_curr_cha( chkchar ), opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "lck" ) )
      {
         return mprog_veval( get_curr_lck( chkchar ), opr, atoi( rval ), mob );
      }
   }
   if( chkobj )
   {
      if( !str_cmp( chck, "objtype" ) )
      {
         return mprog_veval( chkobj->item_type, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "leverpos" ) )
      {
         int isup = FALSE, wantsup = FALSE;
         if( chkobj->item_type != ITEM_SWITCH || chkobj->item_type != ITEM_LEVER || chkobj->item_type != ITEM_PULLCHAIN )
            return FALSE;

         if( IS_SET( obj->value[0], TRIG_UP ) )
            isup = TRUE;
         if( !str_cmp( rval, "up" ) )
            wantsup = TRUE;
         return mprog_veval( wantsup, opr, isup, mob );
      }
      if( !str_cmp( chck, "objval0" ) )
      {
         return mprog_veval( chkobj->value[0], opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "objval1" ) )
      {
         return mprog_veval( chkobj->value[1], opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "objval2" ) )
      {
         return mprog_veval( chkobj->value[2], opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "objval3" ) )
      {
         return mprog_veval( chkobj->value[3], opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "objval4" ) )
      {
         return mprog_veval( chkobj->value[4], opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "objval5" ) )
      {
         return mprog_veval( chkobj->value[5], opr, atoi( rval ), mob );
      }
   }
   if( !str_cmp( chck, "number" ) )
   {
      if( chkchar )
      {
         if( !IS_NPC( chkchar ) )
            return FALSE;
         lhsvl = ( chkchar == mob ) ? chkchar->gold : chkchar->pIndexData->vnum;
         return mprog_veval( lhsvl, opr, atoi( rval ), mob );
      }
      return mprog_veval( chkobj->pIndexData->vnum, opr, atoi( rval ), mob );
   }
   if( !str_cmp( chck, "time" ) )
   {
      return mprog_veval( time_info.hour, opr, atoi( rval ), mob );
   }
   if( !str_cmp( chck, "name" ) )
   {
      if( chkchar )
         return mprog_seval( chkchar->name, opr, rval, mob );
      return mprog_seval( chkobj->name, opr, rval, mob );
   }
/*
    if ( !str_cmp(chck, "rank") ) 
    {
	if ( chkchar && !IS_NPC( chkchar ) )
	    return mprog_seval(chkchar->pcdata->rank, opr, rval, mob);
	return FALSE;
    }
*/
   if( !str_cmp( chck, "mortinworld" ) )
   {
      DESCRIPTOR_DATA *d;
      for( d = first_descriptor; d; d = d->next )
         if( d->connected == CON_PLAYING
             && d->character && get_trust( d->character ) < LEVEL_IMMORTAL && nifty_is_name( d->character->name, cvar ) )
            return TRUE;
      return FALSE;
   }

   if( !str_cmp( chck, "mortinroom" ) )
   {
      CHAR_DATA *ch;
      for( ch = mob->in_room->first_person; ch; ch = ch->next_in_room )
         if( ( !IS_NPC( ch ) ) && get_trust( ch ) < LEVEL_IMMORTAL && nifty_is_name( ch->name, cvar ) )
            return TRUE;
      return FALSE;
   }

   if( !str_cmp( chck, "mortinarea" ) )
   {
      CHAR_DATA *ch;
      for( ch = first_char; ch; ch = ch->next )
         if( ( !IS_NPC( ch ) )
             && ch->in_room->area == mob->in_room->area
             && get_trust( ch ) < LEVEL_IMMORTAL && nifty_is_name( ch->name, cvar ) )
            return TRUE;
      return FALSE;
   }


   if( !str_cmp( chck, "mortcount" ) )
   {
      CHAR_DATA *tch;
      ROOM_INDEX_DATA *room;
      int count = 0;
      int rvnum = atoi( cvar );

      room = get_room_index( rvnum ? rvnum : mob->in_room->vnum );

      for( tch = room ? room->first_person : NULL; tch; tch = tch->next_in_room )
         if( ( !IS_NPC( tch ) ) && get_trust( tch ) < LEVEL_IMMORTAL )
            count++;
      return mprog_veval( count, opr, atoi( rval ), mob );
   }


   if( !str_cmp( chck, "mobcount" ) )
   {
      CHAR_DATA *tch;
      ROOM_INDEX_DATA *room;
      int count = -1;
      int rvnum = atoi( cvar );

      room = get_room_index( rvnum ? rvnum : mob->in_room->vnum );

      for( tch = room ? room->first_person : NULL; tch; tch = tch->next_in_room )
         if( ( IS_NPC( tch ) ) )
            count++;
      return mprog_veval( count, opr, atoi( rval ), mob );
   }


   if( !str_cmp( chck, "charcount" ) )
   {
      CHAR_DATA *tch;
      ROOM_INDEX_DATA *room;
      int count = -1;
      int rvnum = atoi( cvar );

      room = get_room_index( rvnum ? rvnum : mob->in_room->vnum );

      for( tch = room ? room->first_person : NULL; tch; tch = tch->next_in_room )

         if( ( ( !IS_NPC( tch ) ) && get_trust( tch ) < LEVEL_IMMORTAL ) || IS_NPC( tch ) )
            count++;
      return mprog_veval( count, opr, atoi( rval ), mob );
   }



   progbug( "Unknown ifcheck", mob );
   return BERR;
}

#undef isoperator
#undef MAX_IF_ARGS

void mprog_translate( char ch, char *t, CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * obj, void *vo, CHAR_DATA * rndm )
{
   static char *he_she[] = { "he", "she" };
   static char *him_her[] = { "him", "her" };
   static char *his_her[] = { "his", "her" };
   CHAR_DATA *vict = ( CHAR_DATA * ) vo;
   OBJ_DATA *v_obj = ( OBJ_DATA * ) vo;

   if( v_obj && v_obj->serial )
      vict = NULL;
   else
      v_obj = NULL;

   *t = '\0';
   switch ( ch )
   {
      case 'i':
         if( mob && !char_died( mob ) )
         {
            if( mob->name )
               one_argument( mob->name, t );
         }
         else
            strcpy( t, "someone" );
         break;

      case 'I':
         if( mob && !char_died( mob ) )
         {
            if( mob->short_descr )
            {
               strcpy( t, mob->short_descr );
            }
            else
            {
               strcpy( t, "someone" );
            }
         }
         else
            strcpy( t, "someone" );
         break;

      case 'n':
         if( actor && !char_died( actor ) )
         {
//    if ( can_see( mob,actor ) )
            one_argument( actor->name, t );
            if( !IS_NPC( actor ) )
               *t = UPPER( *t );
         }
//  else
//       strcpy( t, "someone" );
         break;

      case 'N':
         if( actor && !char_died( actor ) )
         {
            if( can_see( mob, actor ) )
               if( IS_NPC( actor ) )
                  strcpy( t, actor->short_descr );
               else
               {
                  strcpy( t, actor->name );
                  strcat( t, actor->pcdata->title );
               }
            else
               strcpy( t, "someone" );
         }
         else
            strcpy( t, "someone" );
         break;

      case 't':
         if( vict && !char_died( vict ) )
         {
            if( can_see( mob, vict ) )
               one_argument( vict->name, t );
            if( !IS_NPC( vict ) )
               *t = UPPER( *t );
         }
         else
            strcpy( t, "someone" );

         break;

      case 'T':
         if( vict && !char_died( vict ) )
         {
            if( can_see( mob, vict ) )
               if( IS_NPC( vict ) )
                  strcpy( t, vict->short_descr );
               else
               {
                  strcpy( t, vict->name );
                  strcat( t, " " );
                  strcat( t, vict->pcdata->title );
               }
            else
               strcpy( t, "someone" );
         }
         else
            strcpy( t, "someone" );
         break;

      case 'r':
         if( rndm && !char_died( rndm ) )
         {
            if( can_see( mob, rndm ) )
            {
               one_argument( rndm->name, t );
            }
            if( !IS_NPC( rndm ) )
            {
               *t = UPPER( *t );
            }
         }
         else
            strcpy( t, "someone" );
         break;

      case 'R':
         if( rndm && !char_died( rndm ) )
         {
            if( can_see( mob, rndm ) )
               if( IS_NPC( rndm ) )
                  strcpy( t, rndm->short_descr );
               else
               {
                  strcpy( t, rndm->name );
                  strcat( t, " " );
                  strcat( t, rndm->pcdata->title );
               }
            else
               strcpy( t, "someone" );
         }
         else
            strcpy( t, "someone" );
         break;

      case 'e':
         if( actor && !char_died( actor ) )
         {
            can_see( mob, actor ) ? strcpy( t, he_she[actor->sex] ) : strcpy( t, "someone" );
         }
         else
            strcpy( t, "it" );
         break;

      case 'm':
         if( actor && !char_died( actor ) )
         {
            can_see( mob, actor ) ? strcpy( t, him_her[actor->sex] ) : strcpy( t, "someone" );
         }
         else
            strcpy( t, "it" );
         break;

      case 's':
         if( actor && !char_died( actor ) )
         {
            can_see( mob, actor ) ? strcpy( t, his_her[actor->sex] ) : strcpy( t, "someone's" );
         }
         else
            strcpy( t, "its'" );
         break;

      case 'E':
         if( vict && !char_died( vict ) )
         {
            can_see( mob, vict ) ? strcpy( t, he_she[vict->sex] ) : strcpy( t, "someone" );
         }
         else
            strcpy( t, "it" );
         break;

      case 'M':
         if( vict && !char_died( vict ) )
         {
            can_see( mob, vict ) ? strcpy( t, him_her[vict->sex] ) : strcpy( t, "someone" );
         }
         else
            strcpy( t, "it" );
         break;

      case 'S':
         if( vict && !char_died( vict ) )
         {
            can_see( mob, vict ) ? strcpy( t, his_her[vict->sex] ) : strcpy( t, "someone's" );
         }
         else
            strcpy( t, "its'" );
         break;

      case 'j':
         if( mob && !char_died( mob ) )
         {
            strcpy( t, he_she[mob->sex] );
         }
         else
         {
            strcpy( t, "it" );
         }
         break;

      case 'k':
         if( mob && !char_died( mob ) )
         {
            strcpy( t, him_her[mob->sex] );
         }
         else
         {
            strcpy( t, "it" );
         }
         break;

      case 'l':
         if( mob && !char_died( mob ) )
         {
            strcpy( t, his_her[mob->sex] );
         }
         else
         {
            strcpy( t, "it" );
         }
         break;

      case 'J':
         if( rndm && !char_died( rndm ) )
         {
            can_see( mob, rndm ) ? strcpy( t, he_she[rndm->sex] ) : strcpy( t, "someone" );
         }
         else
            strcpy( t, "it" );
         break;

      case 'K':
         if( rndm && !char_died( rndm ) )
         {
            can_see( mob, rndm ) ? strcpy( t, him_her[rndm->sex] ) : strcpy( t, "someone's" );
         }
         else
            strcpy( t, "its'" );
         break;

      case 'L':
         if( rndm && !char_died( rndm ) )
         {
            can_see( mob, rndm ) ? strcpy( t, his_her[rndm->sex] ) : strcpy( t, "someone" );
         }
         else
            strcpy( t, "its" );
         break;

      case 'o':
         if( obj && !obj_extracted( obj ) )
         {
            can_see_obj( mob, obj ) ? one_argument( obj->name, t ) : strcpy( t, "something" );
         }
         else
            strcpy( t, "something" );
         break;

      case 'O':
         if( obj && !obj_extracted( obj ) )
         {
            can_see_obj( mob, obj ) ? strcpy( t, obj->short_descr ) : strcpy( t, "something" );
         }
         else
            strcpy( t, "something" );
         break;

      case 'p':
         if( v_obj && !obj_extracted( v_obj ) )
         {
            can_see_obj( mob, v_obj ) ? one_argument( v_obj->name, t ) : strcpy( t, "something" );
         }
         else
            strcpy( t, "something" );
         break;

      case 'P':
         if( v_obj && !obj_extracted( v_obj ) )
         {
            can_see_obj( mob, v_obj ) ? strcpy( t, v_obj->short_descr ) : strcpy( t, "something" );
         }
         else
            strcpy( t, "something" );
         break;

      case 'a':
         if( obj && !obj_extracted( obj ) )
         {
            strcpy( t, aoran( obj->name ) );
         }
         else
            strcpy( t, "a" );
         break;

      case 'A':
         if( v_obj && !obj_extracted( v_obj ) )
         {
            strcpy( t, aoran( v_obj->name ) );
         }
         else
            strcpy( t, "a" );
         break;

      case '$':
         strcpy( t, "$" );
         break;

      default:
         progbug( "Bad $var", mob );
         break;
   }

   return;

}

void mprog_driver( char *com_list, CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * obj, void *vo, bool single_step )
{
   char tmpcmndlst[MAX_STRING_LENGTH];
   char *command_list;
   char *cmnd;
   CHAR_DATA *rndm = NULL;
   CHAR_DATA *vch = NULL;
   int count = 0;
   int count2 = 0;
   int ignorelevel = 0;
   int iflevel, result;
   bool ifstate[MAX_IFS][DO_ELSE + 1];
   static int prog_nest;
   MPSLEEP_DATA *mpsleep = NULL;
   char arg[MAX_INPUT_LENGTH];

   if( IS_AFFECTED( mob, AFF_CHARM ) )
      return;

   if( mob == actor )
   {
      progbug( "triggering oneself.", mob );
      return;
   }

   if( ++prog_nest > MAX_PROG_NEST )
   {
      progbug( "max_prog_nest exceeded.", mob );
      --prog_nest;
      return;
   }

   for( iflevel = 0; iflevel < MAX_IFS; iflevel++ )
   {
      for( count = 0; count < DO_ELSE; count++ )
      {
         ifstate[iflevel][count] = FALSE;
      }
   }

   iflevel = 0;

   count = 0;
   for( vch = mob->in_room->first_person; vch; vch = vch->next_in_room )
      if( !IS_NPC( vch )   /*
                            * &&  can_see( mob, vch ) */  )
      {
         if( number_range( 0, count ) == 0 )
            rndm = vch;
         count++;
      }

   strcpy( tmpcmndlst, com_list );
   command_list = tmpcmndlst;

   if( current_mpsleep )
   {
      ignorelevel = current_mpsleep->ignorelevel;
      iflevel = current_mpsleep->iflevel;

      if( single_step )
         mob->mpscriptpos = 0;

      for( count = 0; count < MAX_IFS; count++ )
      {
         for( count2 = 0; count2 < DO_ELSE; count2++ )
            ifstate[count][count2] = current_mpsleep->ifstate[count][count2];
      }

      current_mpsleep = NULL;
   }

   if( single_step )
   {
      if( mob->mpscriptpos > strlen( tmpcmndlst ) )
         mob->mpscriptpos = 0;
      else
         command_list += mob->mpscriptpos;
      if( *command_list == '\0' )
      {
         command_list = tmpcmndlst;
         mob->mpscriptpos = 0;
      }
   }


   while( TRUE )
   {
      cmnd = command_list;
      command_list = mprog_next_command( command_list );

      if( cmnd[0] == '\0' )
      {
         if( ifstate[iflevel][IN_IF] || ifstate[iflevel][IN_ELSE] )
         {
            progbug( "Missing endif", mob );
         }
         --prog_nest;
         return;
      }
      if( !str_prefix( "mpsleep", cmnd ) )
      {
         CREATE( mpsleep, MPSLEEP_DATA, 1 );

         mpsleep->ignorelevel = ignorelevel;
         mpsleep->iflevel = iflevel;
         for( count = 0; count < MAX_IFS; count++ )
         {
            for( count2 = 0; count2 < DO_ELSE; count2++ )
            {
               mpsleep->ifstate[count][count2] = ifstate[count][count2];
            }
         }

         mpsleep->com_list = STRALLOC( command_list );
         mpsleep->mob = mob;
         mpsleep->actor = actor;
         mpsleep->obj = obj;
         mpsleep->vo = vo;
         mpsleep->single_step = single_step;

         cmnd = one_argument( cmnd, arg );
         cmnd = one_argument( cmnd, arg );
         if( arg[0] == '\0' )
            mpsleep->timer = 4;
         else
            mpsleep->timer = atoi( arg );

         if( mpsleep->timer < 1 )
         {
            progbug( "mpsleep - bad arg, using default", mob );
            mpsleep->timer = 4;
         }

         if( mpsleep->mob->pIndexData->vnum == 3 )
         {
            if( !str_prefix( "Room", mpsleep->mob->description ) )
            {
               mpsleep->type = MP_ROOM;
               mpsleep->room = mpsleep->mob->in_room;
            }
            else if( !str_prefix( "Object", mpsleep->mob->description ) )
               mpsleep->type = MP_OBJ;
         }
         else
            mpsleep->type = MP_MOB;

         LINK( mpsleep, first_mpsleep, last_mpsleep, next, prev );

         --prog_nest;
         return;
      }

      result = mprog_do_command( cmnd, mob, actor, obj, vo, rndm,
                                 ( ifstate[iflevel][IN_IF] && !ifstate[iflevel][DO_IF] )
                                 || ( ifstate[iflevel][IN_ELSE] && !ifstate[iflevel][DO_ELSE] ), ( ignorelevel > 0 ) );

      if( single_step )
      {
         mob->mpscriptpos = command_list - tmpcmndlst;
         --prog_nest;
         return;
      }

      switch ( result )
      {
         case COMMANDOK:
#ifdef DEBUG
            log_string( "COMMANDOK" );
#endif
            continue;
            break;

         case IFTRUE:
#ifdef DEBUG
            log_string( "IFTRUE" );
#endif
            iflevel++;
            if( iflevel == MAX_IFS )
            {
               progbug( "Maximum nested ifs exceeded", mob );
               --prog_nest;
               return;
            }

            ifstate[iflevel][IN_IF] = TRUE;
            ifstate[iflevel][DO_IF] = TRUE;
            break;

         case IFFALSE:
#ifdef DEBUG
            log_string( "IFFALSE" );
#endif
            iflevel++;
            if( iflevel == MAX_IFS )
            {
               progbug( "Maximum nested ifs exceeded", mob );
               --prog_nest;
               return;
            }
            ifstate[iflevel][IN_IF] = TRUE;
            ifstate[iflevel][DO_IF] = FALSE;
            break;

         case ORTRUE:
#ifdef DEBUG
            log_string( "ORTRUE" );
#endif
            if( !ifstate[iflevel][IN_IF] )
            {
               progbug( "Unmatched or", mob );
               --prog_nest;
               return;
            }
            ifstate[iflevel][DO_IF] = TRUE;
            break;

         case ORFALSE:
#ifdef DEBUG
            log_string( "ORFALSE" );
#endif
            if( !ifstate[iflevel][IN_IF] )
            {
               progbug( "Unmatched or", mob );
               --prog_nest;
               return;
            }
            continue;
            break;

         case FOUNDELSE:
#ifdef DEBUG
            log_string( "FOUNDELSE" );
#endif
            if( ignorelevel > 0 )
               continue;

            if( ifstate[iflevel][IN_ELSE] )
            {
               progbug( "Found else in an else section", mob );
               --prog_nest;
               return;
            }
            if( !ifstate[iflevel][IN_IF] )
            {
               progbug( "Unmatched else", mob );
               --prog_nest;
               return;
            }

            ifstate[iflevel][IN_ELSE] = TRUE;
            ifstate[iflevel][DO_ELSE] = !ifstate[iflevel][DO_IF];
            ifstate[iflevel][IN_IF] = FALSE;
            ifstate[iflevel][DO_IF] = FALSE;

            break;

         case FOUNDENDIF:
#ifdef DEBUG
            log_string( "FOUNDENDIF" );
#endif
            if( !( ifstate[iflevel][IN_IF] || ifstate[iflevel][IN_ELSE] ) )
            {
               progbug( "Unmatched endif", mob );
               --prog_nest;
               return;
            }

            if( ignorelevel > 0 )
            {
               ignorelevel--;
               continue;
            }

            ifstate[iflevel][IN_IF] = FALSE;
            ifstate[iflevel][DO_IF] = FALSE;
            ifstate[iflevel][IN_ELSE] = FALSE;
            ifstate[iflevel][DO_ELSE] = FALSE;

            iflevel--;
            break;

         case IFIGNORED:
#ifdef DEBUG
            log_string( "IFIGNORED" );
#endif
            if( !( ifstate[iflevel][IN_IF] || ifstate[iflevel][IN_ELSE] ) )
            {
               progbug( "Parse error, ignoring if while not in if or else", mob );
               --prog_nest;
               return;
            }
            ignorelevel++;
            break;

         case ORIGNORED:
#ifdef DEBUG
            log_string( "ORIGNORED" );
#endif
            if( !( ifstate[iflevel][IN_IF] || ifstate[iflevel][IN_ELSE] ) )
            {
               progbug( "Unmatched or", mob );
               --prog_nest;
               return;
            }
            if( ignorelevel == 0 )
            {
               progbug( "Parse error, mistakenly ignoring or", mob );
               --prog_nest;
               return;
            }

            break;

         case BERR:
#ifdef DEBUG
            log_string( "BERR" );
#endif
            --prog_nest;
            return;
            break;
      }
   }
   --prog_nest;
   return;
}

int mprog_do_command( char *cmnd, CHAR_DATA * mob, CHAR_DATA * actor,
                      OBJ_DATA * obj, void *vo, CHAR_DATA * rndm, bool ignore, bool ignore_ors )
{
   char firstword[MAX_INPUT_LENGTH];
   char *ifcheck;
   char buf[MAX_INPUT_LENGTH];
   char tmp[MAX_INPUT_LENGTH];
   char *point, *str, *i;
   int validif, vnum;

   ifcheck = one_argument( cmnd, firstword );

   if( !str_cmp( firstword, "if" ) )
   {
      if( ignore )
         return IFIGNORED;
      else
         validif = mprog_do_ifcheck( ifcheck, mob, actor, obj, vo, rndm );

      if( validif == 1 )
         return IFTRUE;

      if( validif == 0 )
         return IFFALSE;

      return BERR;
   }

   if( !str_cmp( firstword, "or" ) )
   {
      if( ignore_ors )
         return ORIGNORED;
      else
         validif = mprog_do_ifcheck( ifcheck, mob, actor, obj, vo, rndm );

      if( validif == 1 )
         return ORTRUE;

      if( validif == 0 )
         return ORFALSE;

      return BERR;
   }

   if( !str_cmp( firstword, "else" ) )
   {
      return FOUNDELSE;
   }

   if( !str_cmp( firstword, "endif" ) )
   {
      return FOUNDENDIF;
   }


   if( ignore )
      return COMMANDOK;

   if( !str_cmp( firstword, "break" ) )
      return BERR;

   vnum = mob->pIndexData->vnum;
   point = buf;
   str = cmnd;

   while( *str != '\0' )
   {
      if( *str != '$' )
      {
         *point++ = *str++;
         continue;
      }
      str++;
      mprog_translate( *str, tmp, mob, actor, obj, vo, rndm );
      i = tmp;
      ++str;
      while( ( *point = *i ) != '\0' )
         ++point, ++i;
   }
   *point = '\0';

   interpret( mob, buf );

   if( char_died( mob ) )
   {
      bug( "Mob died while executing program, vnum %d.", vnum );
      return BERR;
   }

   return COMMANDOK;
}

void mpsleep_update(  )
{
   MPSLEEP_DATA *mpsleep;
   MPSLEEP_DATA *tmpMpsleep;
   bool delete_it;

   mpsleep = first_mpsleep;
   while( mpsleep )
   {
      delete_it = FALSE;

      if( mpsleep->mob )
         delete_it = char_died( mpsleep->mob );

      if( mpsleep->actor && !delete_it )
         delete_it = char_died( mpsleep->actor );

      if( mpsleep->obj && !delete_it )
         delete_it = obj_extracted( mpsleep->obj );

      if( delete_it )
      {
         log_string( "mpsleep_update - Deleting expired prog." );

         tmpMpsleep = mpsleep;
         mpsleep = mpsleep->next;
         STRFREE( tmpMpsleep->com_list );
         UNLINK( tmpMpsleep, first_mpsleep, last_mpsleep, next, prev );
         DISPOSE( tmpMpsleep );

         continue;
      }

      mpsleep = mpsleep->next;
   }

   mpsleep = first_mpsleep;
   while( mpsleep )
   {
      if( --mpsleep->timer <= 0 )
      {
         current_mpsleep = mpsleep;

         if( mpsleep->type == MP_ROOM )
            rset_supermob( mpsleep->room );
         else if( mpsleep->type == MP_OBJ )
            set_supermob( mpsleep->obj );

         mprog_driver( mpsleep->com_list, mpsleep->mob, mpsleep->actor, mpsleep->obj, mpsleep->vo, mpsleep->single_step );

         release_supermob(  );

         tmpMpsleep = mpsleep;
         mpsleep = mpsleep->next;
         STRFREE( tmpMpsleep->com_list );
         UNLINK( tmpMpsleep, first_mpsleep, last_mpsleep, next, prev );
         DISPOSE( tmpMpsleep );

         continue;
      }

      mpsleep = mpsleep->next;
   }
}


bool mprog_keyword_check( const char *argu, const char *argl )
{
   char word[MAX_INPUT_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   int i;
   char *arg, *arglist;
   char *start, *end;

   strcpy( arg1, strlower( argu ) );
   arg = arg1;
   strcpy( arg2, strlower( argl ) );
   arglist = arg2;

   for( i = 0; i < strlen( arglist ); i++ )
      arglist[i] = LOWER( arglist[i] );
   for( i = 0; i < strlen( arg ); i++ )
      arg[i] = LOWER( arg[i] );
   if( ( arglist[0] == 'p' ) && ( arglist[1] == ' ' ) )
   {
      arglist += 2;
      while( ( start = strstr( arg, arglist ) ) )
         if( ( start == arg || *( start - 1 ) == ' ' )
             && ( *( end = start + strlen( arglist ) ) == ' ' || *end == '\n' || *end == '\r' || *end == '\0' ) )
            return TRUE;
         else
            arg = start + 1;
   }
   else
   {
      arglist = one_argument( arglist, word );
      for( ; word[0] != '\0'; arglist = one_argument( arglist, word ) )
         while( ( start = strstr( arg, word ) ) )
            if( ( start == arg || *( start - 1 ) == ' ' )
                && ( *( end = start + strlen( word ) ) == ' ' || *end == '\n' || *end == '\r' || *end == '\0' ) )
               return TRUE;
            else
               arg = start + 1;
   }
   return FALSE;
}


void mprog_wordlist_check( char *arg, CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * obj, void *vo, int type )
{

   char temp1[MAX_STRING_LENGTH];
   char temp2[MAX_INPUT_LENGTH];
   char word[MAX_INPUT_LENGTH];
   MPROG_DATA *mprg;
   char *list;
   char *start;
   char *dupl;
   char *end;
   int i;

   for( mprg = mob->pIndexData->mudprogs; mprg; mprg = mprg->next )
      if( mprg->type == type )
      {
         strcpy( temp1, mprg->arglist );
         list = temp1;
         for( i = 0; i < strlen( list ); i++ )
            list[i] = LOWER( list[i] );
         strcpy( temp2, arg );
         dupl = temp2;
         for( i = 0; i < strlen( dupl ); i++ )
            dupl[i] = LOWER( dupl[i] );
         if( ( list[0] == 'p' ) && ( list[1] == ' ' ) )
         {
            list += 2;
            while( ( start = strstr( dupl, list ) ) )
               if( ( start == dupl || *( start - 1 ) == ' ' )
                   && ( *( end = start + strlen( list ) ) == ' ' || *end == '\n' || *end == '\r' || *end == '\0' ) )
               {
                  mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
                  break;
               }
               else
                  dupl = start + 1;
         }
         else
         {
            list = one_argument( list, word );
            for( ; word[0] != '\0'; list = one_argument( list, word ) )
               while( ( start = strstr( dupl, word ) ) )
                  if( ( start == dupl || *( start - 1 ) == ' ' )
                      && ( *( end = start + strlen( word ) ) == ' ' || *end == '\n' || *end == '\r' || *end == '\0' ) )
                  {
                     mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
                     break;
                  }
                  else
                     dupl = start + 1;
         }
      }

   return;

}

void mprog_percent_check( CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * obj, void *vo, int type )
{
   MPROG_DATA *mprg;

   for( mprg = mob->pIndexData->mudprogs; mprg; mprg = mprg->next )
      if( ( mprg->type == type ) && ( number_percent(  ) <= atoi( mprg->arglist ) ) )
      {
         mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
         if( type != GREET_PROG && type != ALL_GREET_PROG )
            break;
      }

   return;

}

void mprog_time_check( CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * obj, void *vo, int type )
{
   MPROG_DATA *mprg;
   bool trigger_time;

   for( mprg = mob->pIndexData->mudprogs; mprg; mprg = mprg->next )
   {
      trigger_time = ( time_info.hour == atoi( mprg->arglist ) );

      if( !trigger_time )
      {
         if( mprg->triggered )
            mprg->triggered = FALSE;
         continue;
      }

      if( ( mprg->type == type ) && ( ( !mprg->triggered ) || ( mprg->type == HOUR_PROG ) ) )
      {
         mprg->triggered = TRUE;
         mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
      }
   }
   return;
}


void mob_act_add( CHAR_DATA * mob )
{
   struct act_prog_data *runner;

   for( runner = mob_act_list; runner; runner = runner->next )
      if( runner->vo == mob )
         return;
   CREATE( runner, struct act_prog_data, 1 );
   runner->vo = mob;
   runner->next = mob_act_list;
   mob_act_list = runner;
}

void mprog_act_trigger( char *buf, CHAR_DATA * mob, CHAR_DATA * ch, OBJ_DATA * obj, void *vo )
{
   MPROG_ACT_LIST *tmp_act;
   MPROG_DATA *mprg;
   bool found = FALSE;

   if( IS_NPC( mob ) && HAS_PROG( mob->pIndexData, ACT_PROG ) )
   {
      if( IS_NPC( ch ) && ch->pIndexData == mob->pIndexData )
         return;

      for( mprg = mob->pIndexData->mudprogs; mprg; mprg = mprg->next )
         if( mprg->type == ACT_PROG && mprog_keyword_check( buf, mprg->arglist ) )
         {
            found = TRUE;
            break;
         }
      if( !found )
         return;

      CREATE( tmp_act, MPROG_ACT_LIST, 1 );
      if( mob->mpactnum > 0 )
         tmp_act->next = mob->mpact;
      else
         tmp_act->next = NULL;

      mob->mpact = tmp_act;
      mob->mpact->buf = str_dup( buf );
      mob->mpact->ch = ch;
      mob->mpact->obj = obj;
      mob->mpact->vo = vo;
      mob->mpactnum++;
      mob_act_add( mob );
   }
   return;
}

void mprog_bribe_trigger( CHAR_DATA * mob, CHAR_DATA * ch, int amount )
{

   char buf[MAX_STRING_LENGTH];
   MPROG_DATA *mprg;
   OBJ_DATA *obj;

   if( IS_NPC( mob ) && can_see( mob, ch ) && HAS_PROG( mob->pIndexData, BRIBE_PROG ) )
   {
      if( IS_NPC( ch ) && ch->pIndexData == mob->pIndexData )
         return;

      obj = create_object( get_obj_index( OBJ_VNUM_GOLD_SOME ), 0 );
      sprintf( buf, obj->short_descr, amount );
      STRFREE( obj->short_descr );
      obj->short_descr = STRALLOC( buf );
      obj->value[0] = amount;
      obj = obj_to_char( obj, mob );
      mob->gold -= amount;

      for( mprg = mob->pIndexData->mudprogs; mprg; mprg = mprg->next )
         if( ( mprg->type == BRIBE_PROG ) && ( amount >= atoi( mprg->arglist ) ) )
         {
            mprog_driver( mprg->comlist, mob, ch, obj, NULL, FALSE );
            break;
         }
   }

   return;

}

void mprog_death_trigger( CHAR_DATA * killer, CHAR_DATA * mob )
{
   if( IS_NPC( mob ) && killer != mob && HAS_PROG( mob->pIndexData, DEATH_PROG ) )
   {
      mob->position = POS_STANDING;
      mprog_percent_check( mob, killer, NULL, NULL, DEATH_PROG );
      mob->position = POS_DEAD;
   }
   death_cry( mob );
   return;
}

void mprog_entry_trigger( CHAR_DATA * mob )
{
   if( IS_NPC( mob ) && HAS_PROG( mob->pIndexData, ENTRY_PROG ) )
      mprog_percent_check( mob, NULL, NULL, NULL, ENTRY_PROG );

   return;
}

void mprog_fight_trigger( CHAR_DATA * mob, CHAR_DATA * ch )
{
   if( IS_NPC( mob ) && HAS_PROG( mob->pIndexData, FIGHT_PROG ) )
      mprog_percent_check( mob, ch, NULL, NULL, FIGHT_PROG );

   return;
}

void mprog_give_trigger( CHAR_DATA * mob, CHAR_DATA * ch, OBJ_DATA * obj )
{
   char buf[MAX_INPUT_LENGTH];
   MPROG_DATA *mprg;

   if( IS_NPC( mob ) && can_see( mob, ch ) && HAS_PROG( mob->pIndexData, GIVE_PROG ) )
   {
      if( IS_NPC( ch ) && ch->pIndexData == mob->pIndexData )
         return;

      for( mprg = mob->pIndexData->mudprogs; mprg; mprg = mprg->next )
      {
         one_argument( mprg->arglist, buf );

         if( mprg->type == GIVE_PROG && ( !str_cmp( obj->name, mprg->arglist ) || !str_cmp( "all", buf ) ) )
         {
            mprog_driver( mprg->comlist, mob, ch, obj, NULL, FALSE );
            break;
         }
      }
   }
   return;
}

void mprog_greet_trigger( CHAR_DATA * ch )
{
   CHAR_DATA *vmob, *vmob_next;

#ifdef DEBUG
   char buf[MAX_STRING_LENGTH];
   sprintf( buf, "mprog_greet_trigger -> %s", ch->name );
   log_string( buf );
#endif

   for( vmob = ch->in_room->first_person; vmob; vmob = vmob_next )
   {
      vmob_next = vmob->next_in_room;
      if( !IS_NPC( vmob ) || !can_see( vmob, ch ) || vmob->fighting || !IS_AWAKE( vmob ) )
         continue;

      if( IS_NPC( ch ) && ch->pIndexData == vmob->pIndexData )
         continue;

      if( HAS_PROG( vmob->pIndexData, GREET_PROG ) )
         mprog_percent_check( vmob, ch, NULL, NULL, GREET_PROG );
      else if( HAS_PROG( vmob->pIndexData, ALL_GREET_PROG ) )
         mprog_percent_check( vmob, ch, NULL, NULL, ALL_GREET_PROG );
   }
   return;

}

void mprog_hitprcnt_trigger( CHAR_DATA * mob, CHAR_DATA * ch )
{
   MPROG_DATA *mprg;

   if( IS_NPC( mob ) && HAS_PROG( mob->pIndexData, HITPRCNT_PROG ) )
   {
      for( mprg = mob->pIndexData->mudprogs; mprg; mprg = mprg->next )
         if( mprg->type == HITPRCNT_PROG && ( 100 * mob->hit / mob->max_hit ) < atoi( mprg->arglist ) )
         {
            mprog_driver( mprg->comlist, mob, ch, NULL, NULL, FALSE );
            break;
         }
   }
   return;
}

void mprog_random_trigger( CHAR_DATA * mob )
{
   if( HAS_PROG( mob->pIndexData, RAND_PROG ) )
      mprog_percent_check( mob, NULL, NULL, NULL, RAND_PROG );
}

void mprog_time_trigger( CHAR_DATA * mob )
{
   if( HAS_PROG( mob->pIndexData, TIME_PROG ) )
      mprog_time_check( mob, NULL, NULL, NULL, TIME_PROG );
}

void mprog_hour_trigger( CHAR_DATA * mob )
{
   if( HAS_PROG( mob->pIndexData, HOUR_PROG ) )
      mprog_time_check( mob, NULL, NULL, NULL, HOUR_PROG );
}

void mprog_speech_trigger( char *txt, CHAR_DATA * actor )
{
   CHAR_DATA *vmob;

   for( vmob = actor->in_room->first_person; vmob; vmob = vmob->next_in_room )
   {
      if( IS_NPC( vmob ) && HAS_PROG( vmob->pIndexData, SPEECH_PROG ) )
      {
         if( IS_NPC( actor ) && actor->pIndexData == vmob->pIndexData )
            continue;
         mprog_wordlist_check( txt, vmob, actor, NULL, NULL, SPEECH_PROG );
      }
   }
}

void mprog_script_trigger( CHAR_DATA * mob )
{
   MPROG_DATA *mprg;

   if( HAS_PROG( mob->pIndexData, SCRIPT_PROG ) )
      for( mprg = mob->pIndexData->mudprogs; mprg; mprg = mprg->next )
         if( mprg->type == SCRIPT_PROG
             && ( mprg->arglist[0] == '\0' || mob->mpscriptpos != 0 || atoi( mprg->arglist ) == time_info.hour ) )
            mprog_driver( mprg->comlist, mob, NULL, NULL, NULL, TRUE );
}

void oprog_script_trigger( OBJ_DATA * obj )
{
   MPROG_DATA *mprg;

   if( HAS_PROG( obj->pIndexData, SCRIPT_PROG ) )
      for( mprg = obj->pIndexData->mudprogs; mprg; mprg = mprg->next )
         if( mprg->type == SCRIPT_PROG )
         {
            if( mprg->arglist[0] == '\0' || obj->mpscriptpos != 0 || atoi( mprg->arglist ) == time_info.hour )
            {
               set_supermob( obj );
               mprog_driver( mprg->comlist, supermob, NULL, NULL, NULL, TRUE );
               obj->mpscriptpos = supermob->mpscriptpos;
               release_supermob(  );
            }
         }
   return;
}

void rprog_script_trigger( ROOM_INDEX_DATA * room )
{
   MPROG_DATA *mprg;

   if( HAS_PROG( room, SCRIPT_PROG ) )
      for( mprg = room->mudprogs; mprg; mprg = mprg->next )
         if( mprg->type == SCRIPT_PROG )
         {
            if( mprg->arglist[0] == '\0' || room->mpscriptpos != 0 || atoi( mprg->arglist ) == time_info.hour )
            {
               rset_supermob( room );
               mprog_driver( mprg->comlist, supermob, NULL, NULL, NULL, TRUE );
               room->mpscriptpos = supermob->mpscriptpos;
               release_supermob(  );
            }
         }
   return;
}

void set_supermob( OBJ_DATA * obj )
{
   ROOM_INDEX_DATA *room;
   OBJ_DATA *in_obj;
   CHAR_DATA *mob;
   char buf[200];

   if( !supermob )
      supermob = create_mobile( get_mob_index( 3 ) );

   mob = supermob;

   if( !obj )
      return;

   for( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj )
      ;

   if( in_obj->carried_by )
   {
      room = in_obj->carried_by->in_room;
   }
   else
   {
      room = obj->in_room;
   }

   if( !room )
      return;

   if( supermob->short_descr )
      STRFREE( supermob->short_descr );

   supermob->short_descr = QUICKLINK( obj->short_descr );
   supermob->mpscriptpos = obj->mpscriptpos;

   sprintf( buf, "Object #%d", obj->pIndexData->vnum );
   STRFREE( supermob->description );
   supermob->description = STRALLOC( buf );

   if( room != NULL )
   {
      char_from_room( supermob );
      char_to_room( supermob, room );
   }
}

void release_supermob(  )
{
   char_from_room( supermob );
   char_to_room( supermob, get_room_index( 3 ) );
}


bool oprog_percent_check( CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * obj, void *vo, int type )
{
   MPROG_DATA *mprg;
   bool executed = FALSE;

   for( mprg = obj->pIndexData->mudprogs; mprg; mprg = mprg->next )
      if( mprg->type == type && ( number_percent(  ) <= atoi( mprg->arglist ) ) )
      {
         executed = TRUE;
         mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
         if( type != GREET_PROG )
            break;
      }

   return executed;
}

void oprog_greet_trigger( CHAR_DATA * ch )
{
   OBJ_DATA *vobj;

   for( vobj = ch->in_room->first_content; vobj; vobj = vobj->next_content )
      if( HAS_PROG( vobj->pIndexData, GREET_PROG ) )
      {
         set_supermob( vobj );
         oprog_percent_check( supermob, ch, vobj, NULL, GREET_PROG );
         release_supermob(  );
      }
}

void oprog_speech_trigger( char *txt, CHAR_DATA * ch )
{
   OBJ_DATA *vobj;

   for( vobj = ch->in_room->first_content; vobj; vobj = vobj->next_content )
      if( HAS_PROG( vobj->pIndexData, SPEECH_PROG ) )
         oprog_wordlist_check( txt, supermob, ch, vobj, NULL, SPEECH_PROG, vobj );

   return;
}

void oprog_random_trigger( OBJ_DATA * obj )
{
   if( HAS_PROG( obj->pIndexData, RAND_PROG ) )
   {
      set_supermob( obj );
      oprog_percent_check( supermob, NULL, obj, NULL, RAND_PROG );
      release_supermob(  );
   }
}

void oprog_wear_trigger( CHAR_DATA * ch, OBJ_DATA * obj )
{
   if( HAS_PROG( obj->pIndexData, WEAR_PROG ) )
   {
      set_supermob( obj );
      oprog_percent_check( supermob, ch, obj, NULL, WEAR_PROG );
      release_supermob(  );
   }
}

bool oprog_use_trigger( CHAR_DATA * ch, OBJ_DATA * obj, CHAR_DATA * vict, OBJ_DATA * targ, void *vo )
{
   bool executed = FALSE;

   if( HAS_PROG( obj->pIndexData, USE_PROG ) )
   {
      set_supermob( obj );
      if( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND || obj->item_type == ITEM_SCROLL )
      {
         if( vict )
            executed = oprog_percent_check( supermob, ch, obj, vict, USE_PROG );
         else
            executed = oprog_percent_check( supermob, ch, obj, targ, USE_PROG );
      }
      else
         executed = oprog_percent_check( supermob, ch, obj, NULL, USE_PROG );
      release_supermob(  );
   }
   return executed;
}

void oprog_remove_trigger( CHAR_DATA * ch, OBJ_DATA * obj )
{
   if( HAS_PROG( obj->pIndexData, REMOVE_PROG ) )
   {
      set_supermob( obj );
      oprog_percent_check( supermob, ch, obj, NULL, REMOVE_PROG );
      release_supermob(  );
   }
}

void oprog_sac_trigger( CHAR_DATA * ch, OBJ_DATA * obj )
{
   if( HAS_PROG( obj->pIndexData, SAC_PROG ) )
   {
      set_supermob( obj );
      oprog_percent_check( supermob, ch, obj, NULL, SAC_PROG );
      release_supermob(  );
   }
}

void oprog_get_trigger( CHAR_DATA * ch, OBJ_DATA * obj )
{
   if( HAS_PROG( obj->pIndexData, GET_PROG ) )
   {
      set_supermob( obj );
      oprog_percent_check( supermob, ch, obj, NULL, GET_PROG );
      release_supermob(  );
   }
}

void oprog_damage_trigger( CHAR_DATA * ch, OBJ_DATA * obj )
{
   if( HAS_PROG( obj->pIndexData, DAMAGE_PROG ) )
   {
      set_supermob( obj );
      oprog_percent_check( supermob, ch, obj, NULL, DAMAGE_PROG );
      release_supermob(  );
   }
}

void oprog_repair_trigger( CHAR_DATA * ch, OBJ_DATA * obj )
{
   if( HAS_PROG( obj->pIndexData, REPAIR_PROG ) )
   {
      set_supermob( obj );
      oprog_percent_check( supermob, ch, obj, NULL, REPAIR_PROG );
      release_supermob(  );
   }
}

void oprog_drop_trigger( CHAR_DATA * ch, OBJ_DATA * obj )
{
   if( HAS_PROG( obj->pIndexData, DROP_PROG ) )
   {
      set_supermob( obj );
      oprog_percent_check( supermob, ch, obj, NULL, DROP_PROG );
      release_supermob(  );
   }
}

void oprog_examine_trigger( CHAR_DATA * ch, OBJ_DATA * obj )
{
   if( HAS_PROG( obj->pIndexData, EXA_PROG ) )
   {
      set_supermob( obj );
      oprog_percent_check( supermob, ch, obj, NULL, EXA_PROG );
      release_supermob(  );
   }
}

void oprog_zap_trigger( CHAR_DATA * ch, OBJ_DATA * obj )
{
   if( HAS_PROG( obj->pIndexData, ZAP_PROG ) )
   {
      set_supermob( obj );
      oprog_percent_check( supermob, ch, obj, NULL, ZAP_PROG );
      release_supermob(  );
   }
}

void oprog_pull_trigger( CHAR_DATA * ch, OBJ_DATA * obj )
{
   if( HAS_PROG( obj->pIndexData, PULL_PROG ) )
   {
      set_supermob( obj );
      oprog_percent_check( supermob, ch, obj, NULL, PULL_PROG );
      release_supermob(  );
   }
}

void oprog_push_trigger( CHAR_DATA * ch, OBJ_DATA * obj )
{
   if( HAS_PROG( obj->pIndexData, PUSH_PROG ) )
   {
      set_supermob( obj );
      oprog_percent_check( supermob, ch, obj, NULL, PUSH_PROG );
      release_supermob(  );
   }
}

void obj_act_add( OBJ_DATA * obj );
void oprog_act_trigger( char *buf, OBJ_DATA * mobj, CHAR_DATA * ch, OBJ_DATA * obj, void *vo )
{
   if( HAS_PROG( mobj->pIndexData, ACT_PROG ) )
   {
      MPROG_ACT_LIST *tmp_act;

      CREATE( tmp_act, MPROG_ACT_LIST, 1 );
      if( mobj->mpactnum > 0 )
         tmp_act->next = mobj->mpact;
      else
         tmp_act->next = NULL;

      mobj->mpact = tmp_act;
      mobj->mpact->buf = str_dup( buf );
      mobj->mpact->ch = ch;
      mobj->mpact->obj = obj;
      mobj->mpact->vo = vo;
      mobj->mpactnum++;
      obj_act_add( mobj );
   }
}

void oprog_wordlist_check( char *arg, CHAR_DATA * mob, CHAR_DATA * actor,
                           OBJ_DATA * obj, void *vo, int type, OBJ_DATA * iobj )
{
   char temp1[MAX_STRING_LENGTH];
   char temp2[MAX_INPUT_LENGTH];
   char word[MAX_INPUT_LENGTH];
   MPROG_DATA *mprg;
   char *list;
   char *start;
   char *dupl;
   char *end;
   int i;

   for( mprg = iobj->pIndexData->mudprogs; mprg; mprg = mprg->next )
      if( mprg->type == type )
      {
         strcpy( temp1, mprg->arglist );
         list = temp1;
         for( i = 0; i < strlen( list ); i++ )
            list[i] = LOWER( list[i] );
         strcpy( temp2, arg );
         dupl = temp2;
         for( i = 0; i < strlen( dupl ); i++ )
            dupl[i] = LOWER( dupl[i] );
         if( ( list[0] == 'p' ) && ( list[1] == ' ' ) )
         {
            list += 2;
            while( ( start = strstr( dupl, list ) ) )
               if( ( start == dupl || *( start - 1 ) == ' ' )
                   && ( *( end = start + strlen( list ) ) == ' ' || *end == '\n' || *end == '\r' || *end == '\0' ) )
               {
                  set_supermob( iobj );
                  mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
                  release_supermob(  );
                  break;
               }
               else
                  dupl = start + 1;
         }
         else
         {
            list = one_argument( list, word );
            for( ; word[0] != '\0'; list = one_argument( list, word ) )
               while( ( start = strstr( dupl, word ) ) )
                  if( ( start == dupl || *( start - 1 ) == ' ' )
                      && ( *( end = start + strlen( word ) ) == ' ' || *end == '\n' || *end == '\r' || *end == '\0' ) )
                  {
                     set_supermob( iobj );
                     mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
                     release_supermob(  );
                     break;
                  }
                  else
                     dupl = start + 1;
         }
      }

   return;
}

void rset_supermob( ROOM_INDEX_DATA * room )
{
   char buf[200];

   if( room )
   {
      STRFREE( supermob->short_descr );
      supermob->short_descr = QUICKLINK( room->name );
      STRFREE( supermob->name );
      supermob->name = QUICKLINK( room->name );
      supermob->mpscriptpos = room->mpscriptpos;

      sprintf( buf, "Room #%d", room->vnum );
      STRFREE( supermob->description );
      supermob->description = STRALLOC( buf );

      char_from_room( supermob );
      char_to_room( supermob, room );
   }
}


void rprog_percent_check( CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * obj, void *vo, int type )
{
   MPROG_DATA *mprg;

   if( !mob->in_room )
      return;

   for( mprg = mob->in_room->mudprogs; mprg; mprg = mprg->next )
      if( mprg->type == type && number_percent(  ) <= atoi( mprg->arglist ) )
      {
         mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
         if( type != ENTER_PROG )
            break;
      }
}

void room_act_add( ROOM_INDEX_DATA * room );
void rprog_act_trigger( char *buf, ROOM_INDEX_DATA * room, CHAR_DATA * ch, OBJ_DATA * obj, void *vo )
{
   if( HAS_PROG( room, ACT_PROG ) )
   {
      MPROG_ACT_LIST *tmp_act;

      CREATE( tmp_act, MPROG_ACT_LIST, 1 );
      if( room->mpactnum > 0 )
         tmp_act->next = room->mpact;
      else
         tmp_act->next = NULL;

      room->mpact = tmp_act;
      room->mpact->buf = str_dup( buf );
      room->mpact->ch = ch;
      room->mpact->obj = obj;
      room->mpact->vo = vo;
      room->mpactnum++;
      room_act_add( room );
   }
}


void rprog_leave_trigger( CHAR_DATA * ch )
{
   if( HAS_PROG( ch->in_room, LEAVE_PROG ) )
   {
      rset_supermob( ch->in_room );
      rprog_percent_check( supermob, ch, NULL, NULL, LEAVE_PROG );
      release_supermob(  );
   }
}

void rprog_enter_trigger( CHAR_DATA * ch )
{
   if( HAS_PROG( ch->in_room, ENTER_PROG ) )
   {
      rset_supermob( ch->in_room );
      rprog_percent_check( supermob, ch, NULL, NULL, ENTER_PROG );
      release_supermob(  );
   }
}

void rprog_sleep_trigger( CHAR_DATA * ch )
{
   if( HAS_PROG( ch->in_room, SLEEP_PROG ) )
   {
      rset_supermob( ch->in_room );
      rprog_percent_check( supermob, ch, NULL, NULL, SLEEP_PROG );
      release_supermob(  );
   }
}

void rprog_rest_trigger( CHAR_DATA * ch )
{
   if( HAS_PROG( ch->in_room, REST_PROG ) )
   {
      rset_supermob( ch->in_room );
      rprog_percent_check( supermob, ch, NULL, NULL, REST_PROG );
      release_supermob(  );
   }
}

void rprog_rfight_trigger( CHAR_DATA * ch )
{
   if( HAS_PROG( ch->in_room, RFIGHT_PROG ) )
   {
      rset_supermob( ch->in_room );
      rprog_percent_check( supermob, ch, NULL, NULL, RFIGHT_PROG );
      release_supermob(  );
   }
}

void rprog_death_trigger( CHAR_DATA * killer, CHAR_DATA * ch )
{
   if( HAS_PROG( ch->in_room, RDEATH_PROG ) )
   {
      rset_supermob( ch->in_room );
      rprog_percent_check( supermob, ch, NULL, NULL, RDEATH_PROG );
      release_supermob(  );
   }
}

void rprog_speech_trigger( char *txt, CHAR_DATA * ch )
{
   if( HAS_PROG( ch->in_room, SPEECH_PROG ) )
   {
      rprog_wordlist_check( txt, supermob, ch, NULL, NULL, SPEECH_PROG, ch->in_room );
   }
}

void rprog_random_trigger( CHAR_DATA * ch )
{
   if( HAS_PROG( ch->in_room, RAND_PROG ) )
   {
      rset_supermob( ch->in_room );
      rprog_percent_check( supermob, ch, NULL, NULL, RAND_PROG );
      release_supermob(  );
   }
}

void rprog_wordlist_check( char *arg, CHAR_DATA * mob, CHAR_DATA * actor,
                           OBJ_DATA * obj, void *vo, int type, ROOM_INDEX_DATA * room )
{

   char temp1[MAX_STRING_LENGTH];
   char temp2[MAX_INPUT_LENGTH];
   char word[MAX_INPUT_LENGTH];
   MPROG_DATA *mprg;
   char *list;
   char *start;
   char *dupl;
   char *end;
   int i;

   if( actor && !char_died( actor ) && actor->in_room )
      room = actor->in_room;

   for( mprg = room->mudprogs; mprg; mprg = mprg->next )
      if( mprg->type == type )
      {
         strcpy( temp1, mprg->arglist );
         list = temp1;
         for( i = 0; i < strlen( list ); i++ )
            list[i] = LOWER( list[i] );
         strcpy( temp2, arg );
         dupl = temp2;
         for( i = 0; i < strlen( dupl ); i++ )
            dupl[i] = LOWER( dupl[i] );
         if( ( list[0] == 'p' ) && ( list[1] == ' ' ) )
         {
            list += 2;
            while( ( start = strstr( dupl, list ) ) )
               if( ( start == dupl || *( start - 1 ) == ' ' )
                   && ( *( end = start + strlen( list ) ) == ' ' || *end == '\n' || *end == '\r' || *end == '\0' ) )
               {
                  rset_supermob( room );
                  mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
                  release_supermob(  );
                  break;
               }
               else
                  dupl = start + 1;
         }
         else
         {
            list = one_argument( list, word );
            for( ; word[0] != '\0'; list = one_argument( list, word ) )
               while( ( start = strstr( dupl, word ) ) )
                  if( ( start == dupl || *( start - 1 ) == ' ' )
                      && ( *( end = start + strlen( word ) ) == ' ' || *end == '\n' || *end == '\r' || *end == '\0' ) )
                  {
                     rset_supermob( room );
                     mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
                     release_supermob(  );
                     break;
                  }
                  else
                     dupl = start + 1;
         }
      }
   return;
}

void rprog_time_check( CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * obj, void *vo, int type )
{
   ROOM_INDEX_DATA *room = ( ROOM_INDEX_DATA * ) vo;
   MPROG_DATA *mprg;
   bool trigger_time;

   for( mprg = room->mudprogs; mprg; mprg = mprg->next )
   {
      trigger_time = ( time_info.hour == atoi( mprg->arglist ) );

      if( !trigger_time )
      {
         if( mprg->triggered )
            mprg->triggered = FALSE;
         continue;
      }

      if( mprg->type == type && ( ( !mprg->triggered ) || ( mprg->type == HOUR_PROG ) ) )
      {
         mprg->triggered = TRUE;
         mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
      }
   }
   return;
}

void rprog_time_trigger( CHAR_DATA * ch )
{
   if( HAS_PROG( ch->in_room, TIME_PROG ) )
   {
      rset_supermob( ch->in_room );
      rprog_time_check( supermob, NULL, NULL, ch->in_room, TIME_PROG );
      release_supermob(  );
   }
}

void rprog_hour_trigger( CHAR_DATA * ch )
{
   if( HAS_PROG( ch->in_room, HOUR_PROG ) )
   {
      rset_supermob( ch->in_room );
      rprog_time_check( supermob, NULL, NULL, ch->in_room, HOUR_PROG );
      release_supermob(  );
   }
}

void progbug( char *str, CHAR_DATA * mob )
{
   char buf[MAX_STRING_LENGTH];
   int vnum = mob->pIndexData ? mob->pIndexData->vnum : 0;

   if( vnum == 3 )
   {
      sprintf( buf, "%s, %s.", str, mob->description == NULL ? "(unknown)" : mob->description );
   }
   else
   {
      sprintf( buf, "%s, Mob #%d.", str, vnum );
   }

   bug( buf, 0 );
   return;
}


void room_act_add( ROOM_INDEX_DATA * room )
{
   struct act_prog_data *runner;

   for( runner = room_act_list; runner; runner = runner->next )
      if( runner->vo == room )
         return;
   CREATE( runner, struct act_prog_data, 1 );
   runner->vo = room;
   runner->next = room_act_list;
   room_act_list = runner;
}


void room_act_update( void )
{
   struct act_prog_data *runner;
   MPROG_ACT_LIST *mpact;

   while( ( runner = room_act_list ) != NULL )
   {
      ROOM_INDEX_DATA *room = runner->vo;

      while( ( mpact = room->mpact ) != NULL )
      {
         if( mpact->ch->in_room == room )
            rprog_wordlist_check( mpact->buf, supermob, mpact->ch, mpact->obj, mpact->vo, ACT_PROG, room );
         room->mpact = mpact->next;
         DISPOSE( mpact->buf );
         DISPOSE( mpact );
      }
      room->mpact = NULL;
      room->mpactnum = 0;
      room_act_list = runner->next;
      DISPOSE( runner );
   }
   return;
}

void obj_act_add( OBJ_DATA * obj )
{
   struct act_prog_data *runner;

   for( runner = obj_act_list; runner; runner = runner->next )
      if( runner->vo == obj )
         return;
   CREATE( runner, struct act_prog_data, 1 );
   runner->vo = obj;
   runner->next = obj_act_list;
   obj_act_list = runner;
}
void obj_act_update( void )
{
   struct act_prog_data *runner;
   MPROG_ACT_LIST *mpact;

   while( ( runner = obj_act_list ) != NULL )
   {
      OBJ_DATA *obj = runner->vo;

      while( ( mpact = obj->mpact ) != NULL )
      {
         oprog_wordlist_check( mpact->buf, supermob, mpact->ch, mpact->obj, mpact->vo, ACT_PROG, obj );
         obj->mpact = mpact->next;
         DISPOSE( mpact->buf );
         DISPOSE( mpact );
      }
      obj->mpact = NULL;
      obj->mpactnum = 0;
      obj_act_list = runner->next;
      DISPOSE( runner );
   }
   return;
}

void mprog_bribe_trigger_silver( CHAR_DATA * mob, CHAR_DATA * ch, int amount )
{

   char buf[MAX_STRING_LENGTH];
   MPROG_DATA *mprg;
   OBJ_DATA *obj;

   if( IS_NPC( mob ) && can_see( mob, ch ) && HAS_PROG( mob->pIndexData, BRIBE_SILVER_PROG ) )
   {
      if( IS_NPC( ch ) && ch->pIndexData == mob->pIndexData )
         return;

      obj = create_object( get_obj_index( OBJ_VNUM_SILVER_SOME ), 0 );
      sprintf( buf, obj->short_descr, amount );
      STRFREE( obj->short_descr );
      obj->short_descr = STRALLOC( buf );
      obj->value[0] = amount;
      obj = obj_to_char( obj, mob );
      mob->silver -= amount;

      for( mprg = mob->pIndexData->mudprogs; mprg; mprg = mprg->next )
         if( ( mprg->type == BRIBE_SILVER_PROG ) && ( amount >= atoi( mprg->arglist ) ) )
         {
            mprog_driver( mprg->comlist, mob, ch, obj, NULL, FALSE );
            break;
         }
   }

   return;

}

void mprog_bribe_trigger_copper( CHAR_DATA * mob, CHAR_DATA * ch, int amount )
{

   char buf[MAX_STRING_LENGTH];
   MPROG_DATA *mprg;
   OBJ_DATA *obj;

   if( IS_NPC( mob ) && can_see( mob, ch ) && HAS_PROG( mob->pIndexData, BRIBE_SILVER_PROG ) )
   {
      if( IS_NPC( ch ) && ch->pIndexData == mob->pIndexData )
         return;

      obj = create_object( get_obj_index( OBJ_VNUM_COPPER_SOME ), 0 );
      sprintf( buf, obj->short_descr, amount );
      STRFREE( obj->short_descr );
      obj->short_descr = STRALLOC( buf );
      obj->value[0] = amount;
      obj = obj_to_char( obj, mob );
      mob->copper -= amount;

      for( mprg = mob->pIndexData->mudprogs; mprg; mprg = mprg->next )
         if( ( mprg->type == BRIBE_SILVER_PROG ) && ( amount >= atoi( mprg->arglist ) ) )
         {
            mprog_driver( mprg->comlist, mob, ch, obj, NULL, FALSE );
            break;
         }
   }

   return;

}
void do_mppquit( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char qbuf[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   sh_int room_chance;
   int level = ch->level, rentcost = 0;

   set_char_color( AT_IMMORT, ch );

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( !IS_NPC( ch ) )
   {
      bug( "mppquit: Room %d. trying to force a player to pquit someone.", ch->in_room->vnum );
      return;
   }

   if( arg1[0] == '\0' )
   {
      bug( "mppquit: Room %d. mppquit with no argument.\n\r", ch->in_room->vnum );
      return;
   }

   if( !( victim = get_char_world( supermob, arg1 ) ) )
   {
      bug( "mppquit: Room %d. mppquit trying to quit invalid char.\n\r", ch->in_room->vnum );
      return;
   }

   if( IS_NPC( victim ) )
   {
      bug( "mppquit: Room %d. mppquit trying to quit NPC.\n\r", ch->in_room->vnum );
      return;
   }


   for( obj = victim->first_carrying; obj; obj = obj->next_content )
      rent_calculate( victim, obj, &rentcost );


   if( !str_cmp( arg2, "auto" ) && sysdata.RENT && rentcost > 0 )
   {
      room_chance = number_range( 1, 3 );

      if( room_chance > 2 )
      {
         sprintf( log_buf, "%s has failed autorent, setting autorent flag.", victim->name );
         log_string_plus( log_buf, LOG_COMM, level );
         char_leaving( victim, 2, rentcost );
      }
      else
      {
         sprintf( log_buf, "%s has autorented safely.", victim->name );
         log_string_plus( log_buf, LOG_COMM, level );
         char_leaving( victim, 3, rentcost );
      }
      return;
   }

   set_char_color( AT_IMMORT, victim );
   send_to_char( "The MUD administrators had you quit due to linkdead state or other reason.\n\r", victim );
   if( victim->fighting )
      stop_fighting( victim, TRUE );

   if( auction->item != NULL && ( ( victim == auction->buyer ) || ( victim == auction->seller ) ) )
   {
      do_auction( supermob, "stop" );
      return;

   }

   if( victim->challenged )
   {
      sprintf( qbuf, "%s has quit! Challenge is void. WHAT A WUSS!", victim->name );
      victim->challenged = NULL;
      sportschan( qbuf );
   }

   if( IS_PKILL( victim ) && victim->wimpy > ( int )victim->max_hit / 2.25 )
   {
      send_to_char( "Your wimpy has been adjusted to the maximum level for deadlies.\n\r", victim );
      do_wimpy( victim, "max" );
   }
   if( victim->position == POS_MOUNTED )
      do_dismount( victim, "" );
   set_char_color( AT_WHITE, victim );
   if( sysdata.RENT && rentcost > 0 )
      send_to_char( "As you leave, your equipment falls to the floor!\n\r", victim );
   if( is_affected( victim, gsn_karaaura ) )
   {
      affect_strip( victim, gsn_karaaura );
      send_to_char( "Kara's Aura leaves your soul feeling empty.\n\r", victim );
   }
   send_to_char( "The Storm swallows you as you fade from the realm....\n\n\n\r", victim );
   act( AT_BYE, "$n has left the game.", victim, NULL, NULL, TO_CANSEE );
   set_char_color( AT_GREY, victim );
   if( !IS_IMMORTAL( victim ) )
   {
      sprintf( buf, "&R<&WLOG INFO&R>&D %s has left %s...", victim->name, sysdata.mud_name );
      talk_info( AT_BLUE, buf );
   }

   sprintf( log_buf, "%s has quit (Room %d).", victim->name, ( victim->in_room ? victim->in_room->vnum : -1 ) );
   log_string_plus( log_buf, LOG_COMM, level );

   if( sysdata.RENT )
      char_leaving( victim, 4, rentcost );
   else
      char_leaving( victim, 0, rentcost );

   return;
}
