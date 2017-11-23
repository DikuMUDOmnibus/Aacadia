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
/*				   Player Information module		*/
/************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "acadia.h"

char *tiny_affect_loc_name( int location );

void do_gold( CHAR_DATA * ch, char *argument )
{
   set_char_color( AT_GOLD, ch );
   ch_printf( ch, "You have %s katyr.\n\r", num_punct( ch->gold ) );
   return;
}


void do_worth( CHAR_DATA * ch, char *argument )
{

   if( IS_NPC( ch ) )
      return;

   ch_printf( ch, "&cYou have &Y%s&c katyr on hand and &Y%s&c katyr in bank.\n\r", num_punct( ch->gold ),
              num_punct( ch->pcdata->balance ) );
   if( ch->level < 100 )
   {
      ch_printf( ch, "&cYou have &W%d&c experience and need &W%d&c more experience to get the next level.\n\r", ch->exp,
                 exp_level( ch, ( ch->level + 1 ) ) - ch->exp );
   }
   else
   {
      ch_printf( ch, "&cYou have &W%d&c experience and need &W%d&c more experience to get the next level.\n\r", ch->exp,
                 IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch, ( ch->sublevel + 1 ) ) - ch->exp );
   }
   ch_printf( ch, "&cYou have killed &R%d&c mobiles and have been killed by &R%d&c mobiles.\n\r&D", ch->pcdata->mkills,
              ch->pcdata->mdeaths );
   if( ch->pcdata->wasat != 0 )
      ch_printf( ch, "&cYou need to get &P%d&c levels to try to tier again.\n\r", ch->pcdata->wasat );
   return;
}

void do_pscore( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];
   char bam[MAX_INPUT_LENGTH];
   char bam2[MAX_INPUT_LENGTH];
   char cla[MAX_INPUT_LENGTH];
   AFFECT_DATA *paf;
   CHAR_DATA *victim;

   if( argument[0] == '\0' )
   {
      send_to_char( "Pscore who?\n\rSyntax: pscore <who>\n\r", ch );
      return;
   }
   strcpy( buf, "0." );
   strcat( buf, argument );
   victim = get_char_world( ch, buf );

   if( !victim )
   {
      sprintf( buf2, "%s is not logged on now.\n\r", capitalize( argument ) );
      send_to_char( buf2, ch );
      return;
   }

   if( IS_AGOD( victim ) && !IS_AGOD( ch ) )
   {
      sprintf( buf2, "You are not allowed to see %s's score.\n\r", victim->name );
      send_to_char( buf2, ch );
      return;
   }

   pager_printf_color( ch, "\n\r&g%s%s\n\r", IS_SNAME( victim ) ? victim->pcdata->sname : victim->name,
                       victim->pcdata->title );
   if( victim->pcdata->spouse )
      pager_printf_color( ch, "&g%s is married to %s.\n\r", victim->name, victim->pcdata->spouse );
   if( get_trust( victim ) != victim->level )
      pager_printf( ch, "%s is trusted at level %d.\n\r", victim->name, get_trust( victim ) );
   if( victim->pcdata->wasat != 0 )
      pager_printf( ch, "%s needs to get %d levels to try to tier again.\n\r", victim->name, victim->pcdata->wasat );

   send_to_pager_color( "&g&W----------------------------------------------------------------------------\n\r", ch );

   pager_printf_color( ch, "&gStrength      : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou are &Y%d&g years old (&Y%d hours&g).\n\r",
                       victim->perm_str, get_curr_str( victim ), get_age( victim ), ( get_age( victim ) - 17 ) * 2 );
   pager_printf_color( ch, "&gIntelligence  : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou have &R%d&g/&r%d&g hits.\n\r",
                       victim->perm_int, get_curr_int( victim ), victim->hit, victim->max_hit );
   if( IS_VAMPIRE( victim ) )
      pager_printf_color( ch, "&gWisdom        : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou have &R%d&g/&r%d&g blood.\n\r",
                          victim->perm_wis, get_curr_wis( victim ), victim->blood, victim->max_blood );
   else
      pager_printf_color( ch, "&gWisdom        : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou have &R%d&g/&r%d&g mana.\n\r",
                          victim->perm_wis, get_curr_wis( victim ), victim->mana, victim->max_mana );
   pager_printf_color( ch, "&gDexterity     : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou have &R%d&g/&r%d&g moves.\n\r",
                       victim->perm_dex, get_curr_dex( victim ), victim->move, victim->max_move );
   pager_printf_color( ch, "&gConstitution  : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou are carrying &W%d&g/&w%d&g items.\n\r",
                       victim->perm_con, get_curr_con( victim ), victim->carry_number, can_carry_n( victim ) );
   pager_printf_color( ch, "&gCharisma      : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou are carrying &W%d&g/&w%d&g lbs.\n\r",
                       victim->perm_cha, get_curr_cha( victim ), victim->carry_weight, can_carry_w( victim ) );

   switch ( victim->position )
   {
      case POS_DEAD:
         sprintf( buf, "slowly decomposing" );
         break;
      case POS_MORTAL:
         sprintf( buf, "mortally wounded" );
         break;
      case POS_INCAP:
         sprintf( buf, "incapacitated" );
         break;
      case POS_STUNNED:
         sprintf( buf, "stunned" );
         break;
      case POS_SLEEPING:
         sprintf( buf, "sleeping" );
         break;
      case POS_RESTING:
         sprintf( buf, "resting" );
         break;
      case POS_STANDING:
         sprintf( buf, "standing" );
         break;
      case POS_FIGHTING:
         sprintf( buf, "fighting" );
         break;
      case POS_EVASIVE:
         sprintf( buf, "fighting (evasive)" );
         break;
      case POS_DEFENSIVE:
         sprintf( buf, "fighting (defensive)" );
         break;
      case POS_AGGRESSIVE:
         sprintf( buf, "fighting (aggressive)" );
         break;
      case POS_BERSERK:
         sprintf( buf, "fighting (berserk)" );
         break;
      case POS_MOUNTED:
         sprintf( buf, "mounted" );
         break;
      case POS_SITTING:
         sprintf( buf, "sitting" );
         break;
   }
   pager_printf_color( ch, "&gLuck          : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou are currently &R%s&g.\n\r",
                       victim->perm_lck, get_curr_lck( victim ), buf );

   if( victim->level < 100 )
   {
      if( victim->dualclass > -1 )
      {
         pager_printf_color( ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g Race: &Y%s&g.\n\r",
                             victim->exp, victim->level, get_race( victim ) );
         pager_printf_color( ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s &gDual Class: &Y%s&g.\n\r",
                             exp_level( victim, ( victim->level + 1 ) ) - victim->exp, get_class( victim ),
                             get_dualclass( victim ) );
      }
      else
      {
         pager_printf_color( ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g Race: &Y%s&g.\n\r",
                             victim->exp, victim->level, get_race( victim ) );
         pager_printf_color( ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s&g.\n\r",
                             exp_level( victim, ( victim->level + 1 ) ) - victim->exp, get_class( victim ) );
      }
   }
   else
   {
      if( victim->dualclass > -1 )
      {
         pager_printf_color( ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g(&R%d&g) Race: &Y%s&g.\n\r",
                             victim->exp, victim->level, HAS_AWARDV( victim ), get_race( victim ) );
         pager_printf_color( ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s &gDual Class: &Y%s&g.\n\r",
                             IS_AGOD( victim ) ? get_exp_tnl( victim ) : exp_level( victim,
                                                                                    ( victim->sublevel + 1 ) ) - victim->exp,
                             get_class( victim ), get_dualclass( victim ) );
      }
      else
      {
         pager_printf_color( ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g(&R%d&g) Race: &Y%s&g.\n\r",
                             victim->exp, victim->level, HAS_AWARDV( victim ), get_race( victim ) );
         pager_printf_color( ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s&g.\n\r",
                             IS_AGOD( victim ) ? get_exp_tnl( victim ) : exp_level( victim,
                                                                                    ( victim->sublevel + 1 ) ) - victim->exp,
                             get_class( victim ) );
      }
   }

   if( GET_AC( victim ) >= 400 )
      sprintf( buf, "worse than naked" );
   else if( GET_AC( victim ) >= 300 )
      sprintf( buf, "naked" );
   else if( GET_AC( victim ) >= 101 )
      sprintf( buf, "the rags of a prisoner" );
   else if( GET_AC( victim ) >= 80 )
      sprintf( buf, "clothes of a beggar" );
   else if( GET_AC( victim ) >= 55 )
      sprintf( buf, "squire's hides" );
   else if( GET_AC( victim ) >= 40 )
      sprintf( buf, "leather plating" );
   else if( GET_AC( victim ) >= 20 )
      sprintf( buf, "copper plating" );
   else if( GET_AC( victim ) >= 10 )
      sprintf( buf, "gold plating" );
   else if( GET_AC( victim ) >= 0 )
      sprintf( buf, "shod iron plating" );
   else if( GET_AC( victim ) >= -10 )
      sprintf( buf, "iron plating" );
   else if( GET_AC( victim ) >= -20 )
      sprintf( buf, "steel plating" );
   else if( GET_AC( victim ) >= -40 )
      sprintf( buf, "graphite carbon plating" );
   else if( GET_AC( victim ) >= -60 )
      sprintf( buf, "kevlar plating" );
   else if( GET_AC( victim ) >= -80 )
      sprintf( buf, "titanium plating" );
   else if( GET_AC( victim ) >= -100 )
      sprintf( buf, "mystic plating" );
   else if( GET_AC( victim ) >= -200 )
      sprintf( buf, "xenon plating" );
   else if( GET_AC( victim ) >= -300 )
      sprintf( buf, "creon plating" );
   else if( GET_AC( victim ) >= -400 )
      sprintf( buf, "mythril plating" );
   else if( GET_AC( victim ) >= -550 )
      sprintf( buf, "cirra plating" );
   else if( GET_AC( victim ) >= -700 )
      sprintf( buf, "aflua plating" );
   else if( GET_AC( victim ) >= -1000 )
      sprintf( buf, "elemental plating" );
   else if( GET_AC( victim ) >= -1500 )
      sprintf( buf, "diaoite plating" );
   else if( GET_AC( victim ) >= -2000 )
      sprintf( buf, "jasminsite plating" );
   else if( GET_AC( victim ) >= -2500 )
      sprintf( buf, "shrouds of immortality" );
   else
      sprintf( buf, "Ironhand Armored" );
   if( victim->level > 24 )
      pager_printf_color( ch, "&gArmor Class   : &W%d &g\t\t\tYou are of &G%s&g.\n\r", GET_AC( victim ), buf );
   else
      pager_printf_color( ch, "&gArmor Class   : &W%s&g.\n\r", buf );
   if( victim->level >= 15 )
      pager_printf_color( ch, "&gHitroll       : &W%d &g\t\t\tDamroll       : &W%d\n\r",
                          GET_HITROLL( victim ), GET_DAMROLL( victim ) );

   if( IS_IMMORTAL( victim ) )
   {
      if( victim->alignment > 900 )
         sprintf( buf, "Master of Angels" );
      else if( victim->alignment > 700 )
         sprintf( buf, "saintly" );
      else if( victim->alignment > 350 )
         sprintf( buf, "devout" );
      else if( victim->alignment > 100 )
         sprintf( buf, "good" );
      else if( victim->alignment > -100 )
         sprintf( buf, "neutral" );
      else if( victim->alignment > -350 )
         sprintf( buf, "evil" );
      else if( victim->alignment > -700 )
         sprintf( buf, "diabolical" );
      else if( victim->alignment > -900 )
         sprintf( buf, "satanic" );
      else if( victim->alignment < -900 )
         sprintf( buf, "Satan's Master" );
   }
   else if( !IS_IMMORTAL( victim ) )
   {
      if( victim->alignment > 900 )
         sprintf( buf, "angelic" );
      else if( victim->alignment > 700 )
         sprintf( buf, "saintly" );
      else if( victim->alignment > 350 )
         sprintf( buf, "devout" );
      else if( victim->alignment > 100 )
         sprintf( buf, "good" );
      else if( victim->alignment > -100 )
         sprintf( buf, "neutral" );
      else if( victim->alignment > -350 )
         sprintf( buf, "evil" );
      else if( victim->alignment > -700 )
         sprintf( buf, "diabolical" );
      else if( victim->alignment > -900 )
         sprintf( buf, "satanic" );
      else if( victim->alignment < -900 )
         sprintf( buf, "evil incarnate" );
   }
   if( victim->level < 10 )
      pager_printf_color( ch, "&gAlignment     : &W%s&g.\n\r", buf );
   else
      pager_printf_color( ch, "&gAlignment     : &W%d &g\t\t\tYou are &w%s&g.\n\r", victim->alignment, buf );
   pager_printf_color( ch, "&gKilled        : &W%d &g\t\t\tDied          : &W%d\n\r",
                       victim->pcdata->mkills, victim->pcdata->mdeaths );
   if( IS_PKILL( victim ) )
      pager_printf_color( ch, "&gPlayer Kills  : &W%d &g\t\t\tPlayer Deaths : &W%d\n\r",
                          victim->pcdata->pkills, victim->pcdata->pdeaths );

   switch ( victim->style )
   {
      case STYLE_EVASIVE:
         sprintf( buf, "evasive" );
         break;
      case STYLE_DEFENSIVE:
         sprintf( buf, "defensive" );
         break;
      case STYLE_AGGRESSIVE:
         sprintf( buf, "aggressive" );
         break;
      case STYLE_BERSERK:
         sprintf( buf, "berserk" );
         break;
      default:
         sprintf( buf, "standard" );
         break;
   }
   pager_printf_color( ch, "&gYou have wimpy set to &Y%d &ghits.\t\tYou are using &Y%s &gstyle.\n\r", victim->wimpy, buf );
   pager_printf_color( ch, "&gYou have &Y%4d &gpractices.\t\tYou have &W%d&g/&w%d &gglory.\n\r",
                       victim->practice, victim->pcdata->quest_curr, victim->pcdata->quest_accum );
   pager_printf_color( ch, "&gYou have &W%10.10s &gkatyr on hand.\t&gYou have &W%s&g katyr in the bank.\n\r",
                       num_punct( victim->gold ), num_punct( victim->pcdata->balance ) );

   if( victim->pcdata->bounty )
   {
      if( victim->pcdata->bountytype == 1 )
      {
         sprintf( buf3, "katyr" );
      }
      else if( victim->pcdata->bountytype == 2 )
      {
         sprintf( buf3, "experience" );
      }
      else if( victim->pcdata->bountytype == 3 )
      {
         sprintf( buf3, "practices" );
      }
      pager_printf_color( ch, "&gYou have been bountied for &W%s&g %s.\n\r", num_punct( victim->pcdata->bounty ), buf3 );
   }

   if( victim->level > 4 )
      pager_printf_color( ch, "&gYour Highest Damage Level is: &P%d&D\n\r", victim->pcdata->damhigh );

   if( victim->pcdata->deity )
   {
      pager_printf_color( ch, "&gYou are worshiping &W%s&g.\n\r", capitalize( victim->pcdata->deity->name ) );
   }
   if( victim->mount )
   {
      pager_printf_color( ch, "&gYou are mounted on: &W%s&g\n\r", victim->mount->short_descr );
   }
   if( victim->pcdata->clan && victim->pcdata->clan->clan_type != CLAN_ORDER && victim->pcdata->clan->clan_type != CLAN_GUILD
       && victim->pcdata->clan->pkyorn == 1 )
   {
      pager_printf_color( ch, "&gYou are in the clan &W%18s&g(&W%3s&g)\tYour rank is &W%-2d&g. \n\r",
                          victim->pcdata->clan->longname, victim->pcdata->clan->name, victim->pcdata->rank );
      pager_printf_color( ch,
                          "&gClan AvPkills: &W%-5d \t\t\t&gClan NonAvpkills: &W%-5d\n\r&gClan AvPdeaths: &W%-5d \t\t\t&gClan NonAvpdeaths: &W%-5d\n\r",
                          victim->pcdata->clan->pkills[5],
                          ( victim->pcdata->clan->pkills[0] + victim->pcdata->clan->pkills[1] +
                            victim->pcdata->clan->pkills[2] + victim->pcdata->clan->pkills[3] +
                            victim->pcdata->clan->pkills[4] ), victim->pcdata->clan->pdeaths[5],
                          ( victim->pcdata->clan->pdeaths[0] + victim->pcdata->clan->pdeaths[1] +
                            victim->pcdata->clan->pdeaths[2] + victim->pcdata->clan->pdeaths[3] +
                            victim->pcdata->clan->pdeaths[4] ) );
   }
   if( victim->pcdata->clan && victim->pcdata->clan->clan_type != CLAN_ORDER && victim->pcdata->clan->clan_type != CLAN_GUILD
       && victim->pcdata->clan->pkyorn == 0 )
   {
      pager_printf_color( ch, "&gYou are in the clan &W%18s&g(&W%3s&g)\tYour rank is &W%-2d&g. \n\r",
                          victim->pcdata->clan->longname, victim->pcdata->clan->name, victim->pcdata->rank );
   }
   if( victim->pcdata->clan && victim->pcdata->clan->clan_type == CLAN_ORDER )
   {
      pager_printf_color( ch, "&gYou are in the order &W%18s&g(&W%3s&g)&g. \n\r",
                          victim->pcdata->clan->longname, victim->pcdata->clan->name );
   }
   if( victim->pcdata->clan && victim->pcdata->clan->clan_type == CLAN_GUILD )
   {
      pager_printf_color( ch, "&gYou are in the guild &W%18s&g(&W%3s&g)&g. \n\r",
                          victim->pcdata->clan->longname, victim->pcdata->clan->name );
   }

   send_to_pager_color( "&c&W----------------------------------------------------------------------------\n\r", ch );
   pager_printf_color( ch,
                       "&gDeaf: &W%-3s&g Pager: &W%-3s&g &GAUTOS&g: katyr: &W%-3s&g loot: &W%-3s&g sac: &W%-3s&g title: &W%-3s&g\n\r",
                       xIS_SET( victim->act, PLR_DEAF ) ? "Yes" : "No", IS_SET( victim->pcdata->flags,
                                                                                PCFLAG_PAGERON ) ? "Yes" : "No",
                       xIS_SET( victim->act, PLR_AUTOGOLD ) ? "Yes" : "No", xIS_SET( victim->act,
                                                                                     PLR_AUTOLOOT ) ? "Yes" : "No",
                       xIS_SET( victim->act, PLR_AUTOSAC ) ? "Yes" : "No", xIS_SET( victim->act,
                                                                                    PLR_A_TITLE ) ? "Yes" : "No" );
   pager_printf_color( ch, "&gYou have your pager set to &Y%d &glines.\n\r", victim->pcdata->pagerlen );

   if( victim->created == '\0' || victim->created == "(null)" )
   {
      time_t cl;

      time( &cl );

      victim->created = ctime( &cl );
   }
   pager_printf_color( ch, "&gYou were created on &w%s\n\r", victim->created );

   if( !IS_AGOD( victim ) )
   {
      if( !IS_NPC( victim ) && victim->pcdata->condition[COND_DRUNK] > 10 )
         send_to_pager( "You are drunk.\n\r", ch );
      if( !IS_NPC( victim ) && victim->pcdata->condition[COND_THIRST] == 0 )
         send_to_pager( "You are in danger of dehydrating.\n\r", ch );
      if( !IS_NPC( victim ) && victim->pcdata->condition[COND_FULL] == 0 )
         send_to_pager( "You are starving to death.\n\r", ch );
   }

   if( victim->pcdata->bestowments && victim->pcdata->bestowments[0] != '\0' )
      pager_printf_color( ch, "&gYou are bestowed with the command(s): &Y%s\n\r", victim->pcdata->bestowments );

   if( victim->morph && victim->morph->morph )
   {
      send_to_pager_color( "&W----------------------------------------------------------------------------&g\n\r", ch );
      if( IS_IMMORTAL( victim ) )
         pager_printf( ch, "Morphed as (%d) %s with a timer of %d.\n\r",
                       victim->morph->morph->vnum, victim->morph->morph->short_desc, victim->morph->timer );
      else
         pager_printf( ch, "You are morphed into a %s.\n\r", victim->morph->morph->short_desc );
   }

   if( xIS_SET( victim->act, EXTRA_PREGNANT ) )
   {
      send_to_pager_color( "&W----------------------------------------------------------------------------&g\n\r", ch );
      pager_printf_color( ch, "&gYou are pregnant with &W%s&g child.\n\r", victim->pcdata->cparents );
   }
   if( IS_ANGEL( victim ) )
   {
      send_to_pager_color( "&W----------------------------------------------------------------------------&g\n\r", ch );
      pager_printf( ch, "&gANGEL DATA:  Number of Restores Given: &Y%d\n\r", victim->pcdata->restore_number );
   }
   if( IS_IMM( victim ) )
   {
      switch ( victim->level )
      {
         case MAX_LEVEL - 0:
            sprintf( cla, "Host Admin" );
            break;
         case MAX_LEVEL - 10:
            sprintf( cla, "Host Senior" );
            break;
         case MAX_LEVEL - 20:
            sprintf( cla, "Host" );
            break;
         case MAX_LEVEL - 30:
            sprintf( cla, "High Templar" );
            break;
         case MAX_LEVEL - 40:
            sprintf( cla, "Arch Templar" );
            break;
         case MAX_LEVEL - 50:
            sprintf( cla, "Templar" );
            break;
         case MAX_LEVEL - 60:
            sprintf( cla, "Elder Wizard" );
            break;
         case MAX_LEVEL - 70:
            sprintf( cla, "Arch Wizard" );
            break;
         case MAX_LEVEL - 80:
            sprintf( cla, "Wizard" );
            break;
         case MAX_LEVEL - 90:
            sprintf( cla, "Grand Bishop" );
            break;
         case MAX_LEVEL - 100:
            sprintf( cla, "Arch Bishop" );
            break;
         case MAX_LEVEL - 110:
            sprintf( cla, "Bishop" );
            break;
         case MAX_LEVEL - 120:
            sprintf( cla, "Grand Lord" );
            break;
         case MAX_LEVEL - 130:
            sprintf( cla, "Lord" );
            break;
         case MAX_LEVEL - 140:
            sprintf( cla, "Arch Duke" );
            break;
         case MAX_LEVEL - 150:
            sprintf( cla, "Duke" );
            break;
         case MAX_LEVEL - 160:
            sprintf( cla, "Arch Druid" );
            break;
         case MAX_LEVEL - 170:
            sprintf( cla, "Druid" );
            break;
         case MAX_LEVEL - 200:
            sprintf( cla, "Retired" );
            break;
      }

      send_to_pager_color( "&W----------------------------------------------------------------------------&g\n\r", ch );
      pager_printf( ch, "&YIMMORTAL DATA&g:  Wizinvis [&z%s&g]  Wizlevel (&z%d&g) Immortal Rank: &z%s\n\r",
                    xIS_SET( victim->act, PLR_WIZINVIS ) ? "X" : " ", victim->pcdata->wizinvis, cla );
   }

   sprintf( bam, "%s appears in a swirling mist.", victim->name );
   sprintf( bam2, "%s disappears in a swirling mist.", victim->name );
   if( IS_ANGEL( victim ) )
   {
      pager_printf( ch, "&gBamfin:  &Y%s\n\r", ( victim->pcdata->bamfin[0] != '\0' ) ? victim->pcdata->bamfin : bam );
      pager_printf( ch, "&gBamfout: &Y%s\n\r", ( victim->pcdata->bamfout[0] != '\0' ) ? victim->pcdata->bamfout : bam2 );
   }
   if( IS_IMM( victim ) )
   {
      pager_printf( ch, "&gBamfin:  &Y%s\n\r", ( victim->pcdata->bamfin[0] != '\0' ) ? victim->pcdata->bamfin : bam );
      pager_printf( ch, "&gBamfout: &Y%s\n\r", ( victim->pcdata->bamfout[0] != '\0' ) ? victim->pcdata->bamfout : bam2 );

      if( victim->pcdata->area )
      {
         pager_printf( ch,
                       "&gVnums:   Room (&Y%-5.5d - %-5.5d&g)   Object (&Y%-5.5d - %-5.5d&g)   Mob (&Y%-5.5d - %-5.5d&g)\n\r",
                       victim->pcdata->area->low_r_vnum, victim->pcdata->area->hi_r_vnum, victim->pcdata->area->low_o_vnum,
                       victim->pcdata->area->hi_o_vnum, victim->pcdata->area->low_m_vnum, victim->pcdata->area->hi_m_vnum );
         pager_printf( ch, "Area &Y%s&g Loaded [&Y%s&g]\n\r", victim->pcdata->area->filename,
                       ( IS_SET( victim->pcdata->area->status, AREA_LOADED ) ) ? "yes" : "no" );
      }
   }
   if( xIS_SET( victim->act, PLR_SCREAFF ) )
   {
      int i;
      SKILLTYPE *sktmp;

      i = 0;
      send_to_pager_color( "&W----------------------------------------------------------------------------&g\n\r", ch );
      send_to_pager_color( "AFFECT DATA:                            ", ch );
      for( paf = victim->first_affect; paf; paf = paf->next )
      {
         if( ( sktmp = get_skilltype( paf->type ) ) == NULL )
            continue;
         if( victim->level < 20 )
         {
            pager_printf_color( ch, "&g[&W%-34.34s&g]    ", sktmp->name );
            if( i == 0 )
               i = 2;
            if( ( ++i % 3 ) == 0 )
               send_to_pager( "\n\r", ch );
         }
         if( victim->level >= 20 )
         {
            if( paf->modifier == 0 )
               pager_printf_color( ch, "&g[&W%-24.24s;%5d &grds]    ", sktmp->name, paf->duration );
            else if( paf->modifier > 999 )
               pager_printf_color( ch, "&g[&W%-15.15s; %7.7s;%5d &grds]    ",
                                   sktmp->name, tiny_affect_loc_name( paf->location ), paf->duration );
            else
               pager_printf_color( ch, "&g[&W%-11.11s;%+-3.3d %7.7s;%5d &grds]    ",
                                   sktmp->name, paf->modifier, tiny_affect_loc_name( paf->location ), paf->duration );
            if( i == 0 )
               i = 1;
            if( ( ++i % 2 ) == 0 )
               send_to_pager( "\n\r", ch );
         }
      }
   }
   send_to_pager( "\n\r", ch );
   return;
}

void do_score( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];
   char bam[MAX_INPUT_LENGTH];
   char bam2[MAX_INPUT_LENGTH];
   char cla[MAX_INPUT_LENGTH];
   AFFECT_DATA *paf;

   if( IS_NPC( ch ) )
   {
      do_score( ch, argument );
      return;
   }
   set_pager_color( AT_SCORE, ch );

   if( xIS_SET( ch->act, PLR_SCRESHRT ) && xIS_SET( ch->act, PLR_MIDSCORE ) )
   {
      send_to_char( "Please chose either shortscore, midscore, or none with config +/-shortscore or config +/-midscore.\n\r",
                    ch );
      return;
   }

   if( xIS_SET( ch->act, PLR_SCRESHRT ) && !xIS_SET( ch->act, PLR_MIDSCORE ) )
   {
      pager_printf_color( ch, "\n\r&g%s%s\n\r", IS_SNAME( ch ) ? ch->pcdata->sname : ch->name, ch->pcdata->title );
      if( ch->pcdata->spouse )
         pager_printf_color( ch, "&gYou are married to %s.\n\r", ch->pcdata->spouse );
      if( get_trust( ch ) != ch->level )
         pager_printf( ch, "You are trusted at level %d.\n\r", get_trust( ch ) );
      if( ch->pcdata->wasat != 0 )
         pager_printf( ch, "You need to get %d levels to try to tier again.\n\r", ch->pcdata->wasat );

      send_to_pager_color( "&c&W----------------------------------------------------------------------------\n\r", ch );
      pager_printf_color( ch,
                          "&gStr: &W%2.2d&G(&w%2.2d&G) &gDex: &W%2.2d&G(&w%2.2d&G) &gCon: &W%2.2d&G(&w%2.2d&G) &gInt: &W%2.2d&G(&w%2.2d&G) &gWis: &W%2.2d&G(&w%2.2d&G)\n\r",
                          ch->perm_str, get_curr_str( ch ), ch->perm_dex, get_curr_dex( ch ), ch->perm_con,
                          get_curr_con( ch ), ch->perm_int, get_curr_int( ch ), ch->perm_wis, get_curr_wis( ch ) );
      if( ch->level < 100 )
      {
         if( IS_VAMPIRE( ch ) )
            pager_printf_color( ch, "&gHMV: &R%d&g/&r%d&g &R%d&g/&r%d&g &R%d&g/&r%d&g\tLevel: &R%d&g TNL: &B%d&g\n\r",
                                ch->hit, ch->max_hit, ch->blood, ch->max_blood, ch->move, ch->max_move, ch->level,
                                exp_level( ch, ( ch->level + 1 ) ) - ch->exp );
         else
            pager_printf_color( ch, "&gHMV: &R%d&g/&r%d&g &R%d&g/&r%d&g &R%d&g/&r%d&g\tLevel: &R%d&g TNL: &B%d&g\n\r",
                                ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move, ch->level,
                                exp_level( ch, ( ch->level + 1 ) ) - ch->exp );
      }
      else
      {
         if( IS_VAMPIRE( ch ) )
            pager_printf_color( ch,
                                "&gHMV: &R%d&g/&r%d&g &R%d&g/&r%d&g &R%d&g/&r%d&g\tLevel: &R%d&g(&R%d&g) TNL: &B%d&g\n\r",
                                ch->hit, ch->max_hit, ch->blood, ch->max_blood, ch->move, ch->max_move, ch->level,
                                HAS_AWARD( ch ), IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch,
                                                                                                ( ch->sublevel + 1 ) ) -
                                ch->exp );
         else
            pager_printf_color( ch,
                                "&gHMV: &R%d&g/&r%d&g &R%d&g/&r%d&g &R%d&g/&r%d&g\tLevel: &R%d&g(&R%d&g) TNL: &B%d&g\n\r",
                                ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move, ch->level,
                                HAS_AWARD( ch ), IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch,
                                                                                                ( ch->sublevel + 1 ) ) -
                                ch->exp );
      }

      if( GET_AC( ch ) >= 400 )
         sprintf( buf, "worse than naked" );
      else if( GET_AC( ch ) >= 300 )
         sprintf( buf, "naked" );
      else if( GET_AC( ch ) >= 101 )
         sprintf( buf, "the rags of a prisoner" );
      else if( GET_AC( ch ) >= 80 )
         sprintf( buf, "clothes of a beggar" );
      else if( GET_AC( ch ) >= 55 )
         sprintf( buf, "squire's hides" );
      else if( GET_AC( ch ) >= 40 )
         sprintf( buf, "leather plating" );
      else if( GET_AC( ch ) >= 20 )
         sprintf( buf, "copper plating" );
      else if( GET_AC( ch ) >= 10 )
         sprintf( buf, "gold plating" );
      else if( GET_AC( ch ) >= 0 )
         sprintf( buf, "shod iron plating" );
      else if( GET_AC( ch ) >= -10 )
         sprintf( buf, "iron plating" );
      else if( GET_AC( ch ) >= -20 )
         sprintf( buf, "steel plating" );
      else if( GET_AC( ch ) >= -40 )
         sprintf( buf, "graphite carbon plating" );
      else if( GET_AC( ch ) >= -60 )
         sprintf( buf, "kevlar plating" );
      else if( GET_AC( ch ) >= -80 )
         sprintf( buf, "titanium plating" );
      else if( GET_AC( ch ) >= -100 )
         sprintf( buf, "mystic plating" );
      else if( GET_AC( ch ) >= -200 )
         sprintf( buf, "xenon plating" );
      else if( GET_AC( ch ) >= -300 )
         sprintf( buf, "creon plating" );
      else if( GET_AC( ch ) >= -400 )
         sprintf( buf, "mythril plating" );
      else if( GET_AC( ch ) >= -550 )
         sprintf( buf, "cirra plating" );
      else if( GET_AC( ch ) >= -700 )
         sprintf( buf, "aflua plating" );
      else if( GET_AC( ch ) >= -1000 )
         sprintf( buf, "elemental plating" );
      else if( GET_AC( ch ) >= -1500 )
         sprintf( buf, "diaoite plating" );
      else if( GET_AC( ch ) >= -2000 )
         sprintf( buf, "jasminsite plating" );
      else if( GET_AC( ch ) >= -2500 )
         sprintf( buf, "shrouds of immortality" );
      else
         sprintf( buf, "Ironhand Armored" );
      if( ch->level > 24 )
         pager_printf_color( ch, "&gArmor Class   : &W%d &g\t\t\tYou are of &G%s&g.\n\r", GET_AC( ch ), buf );
      else
         pager_printf_color( ch, "&gArmor Class   : &W%s&g.\n\r", buf );

      if( ch->level >= 15 )
         pager_printf_color( ch, "&gHitroll       : &W%d &g\t\t\tDamroll       : &W%d\n\r",
                             GET_HITROLL( ch ), GET_DAMROLL( ch ) );

      switch ( ch->style )
      {
         case STYLE_EVASIVE:
            sprintf( buf, "evasive" );
            break;
         case STYLE_DEFENSIVE:
            sprintf( buf, "defensive" );
            break;
         case STYLE_AGGRESSIVE:
            sprintf( buf, "aggressive" );
            break;
         case STYLE_BERSERK:
            sprintf( buf, "berserk" );
            break;
         default:
            sprintf( buf, "standard" );
            break;
      }
      pager_printf_color( ch, "&gYou have wimpy set to &Y%d &ghits.\t\tYou are using &Y%s &gstyle.\n\r", ch->wimpy, buf );
   }
   if( !xIS_SET( ch->act, PLR_SCRESHRT ) && xIS_SET( ch->act, PLR_MIDSCORE ) )
   {
      pager_printf_color( ch, "\n\r&g%s%s\n\r", IS_SNAME( ch ) ? ch->pcdata->sname : ch->name, ch->pcdata->title );
      if( ch->pcdata->spouse )
         pager_printf_color( ch, "&gYou are married to %s.\n\r", ch->pcdata->spouse );
      if( get_trust( ch ) != ch->level )
         pager_printf( ch, "You are trusted at level %d.\n\r", get_trust( ch ) );
      if( ch->pcdata->wasat != 0 )
         pager_printf( ch, "You need to get %d levels to try to tier again.\n\r", ch->pcdata->wasat );

      send_to_pager_color( "&g&W----------------------------------------------------------------------------\n\r", ch );
      pager_printf_color( ch,
                          "&gStr: &W%2.2d&G(&w%2.2d&G) &gDex: &W%2.2d&G(&w%2.2d&G) &gCon: &W%2.2d&G(&w%2.2d&G) &gInt: &W%2.2d&G(&w%2.2d&G) &gWis: &W%2.2d&G(&w%2.2d&G)\n\r",
                          ch->perm_str, get_curr_str( ch ), ch->perm_dex, get_curr_dex( ch ), ch->perm_con,
                          get_curr_con( ch ), ch->perm_int, get_curr_int( ch ), ch->perm_wis, get_curr_wis( ch ) );
      if( IS_VAMPIRE( ch ) )
         pager_printf_color( ch, "&gHits: &R%d&g/&r%d&g &gBlood: &R%d&g/&r%d&g &gMoves: &R%d&g/&r%d\n\r",
                             ch->hit, ch->max_hit, ch->blood, ch->max_blood, ch->move, ch->max_move );
      else
         pager_printf_color( ch, "&gHits: &R%d&g/&r%d&g &gMana: &R%d&g/&r%d&g &gMoves: &R%d&g/&r%d\n\r",
                             ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move );

      if( ch->level < 100 )
      {
         if( ch->dualclass > -1 )
         {
            pager_printf_color( ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g Race: &Y%s&g.\n\r",
                                ch->exp, ch->level, get_race( ch ) );
            pager_printf_color( ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s &gDual Class: &Y%s&g.\n\r",
                                exp_level( ch, ( ch->level + 1 ) ) - ch->exp, get_class( ch ), get_dualclass( ch ) );
         }
         else
         {
            pager_printf_color( ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g Race: &Y%s&g.\n\r",
                                ch->exp, ch->level, get_race( ch ) );
            pager_printf_color( ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s&g.\n\r",
                                exp_level( ch, ( ch->level + 1 ) ) - ch->exp, get_class( ch ) );
         }
      }
      else
      {
         if( ch->dualclass > -1 )
         {
            pager_printf_color( ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g(&R%d&g) Race: &Y%s&g.\n\r",
                                ch->exp, ch->level, HAS_AWARD( ch ), get_race( ch ) );
            pager_printf_color( ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s &gDual Class: &Y%s&g.\n\r",
                                IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch, ( ch->sublevel + 1 ) ) - ch->exp,
                                get_class( ch ), get_dualclass( ch ) );
         }
         else
         {
            pager_printf_color( ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g(&R%d&g) Race: &Y%s&g.\n\r",
                                ch->exp, ch->level, HAS_AWARD( ch ), get_race( ch ) );
            pager_printf_color( ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s&g.\n\r",
                                IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch, ( ch->sublevel + 1 ) ) - ch->exp,
                                get_class( ch ) );
         }
      }

      if( GET_AC( ch ) >= 400 )
         sprintf( buf, "worse than naked" );
      else if( GET_AC( ch ) >= 300 )
         sprintf( buf, "naked" );
      else if( GET_AC( ch ) >= 101 )
         sprintf( buf, "the rags of a prisoner" );
      else if( GET_AC( ch ) >= 80 )
         sprintf( buf, "clothes of a beggar" );
      else if( GET_AC( ch ) >= 55 )
         sprintf( buf, "squire's hides" );
      else if( GET_AC( ch ) >= 40 )
         sprintf( buf, "leather plating" );
      else if( GET_AC( ch ) >= 20 )
         sprintf( buf, "copper plating" );
      else if( GET_AC( ch ) >= 10 )
         sprintf( buf, "gold plating" );
      else if( GET_AC( ch ) >= 0 )
         sprintf( buf, "shod iron plating" );
      else if( GET_AC( ch ) >= -10 )
         sprintf( buf, "iron plating" );
      else if( GET_AC( ch ) >= -20 )
         sprintf( buf, "steel plating" );
      else if( GET_AC( ch ) >= -40 )
         sprintf( buf, "graphite carbon plating" );
      else if( GET_AC( ch ) >= -60 )
         sprintf( buf, "kevlar plating" );
      else if( GET_AC( ch ) >= -80 )
         sprintf( buf, "titanium plating" );
      else if( GET_AC( ch ) >= -100 )
         sprintf( buf, "mystic plating" );
      else if( GET_AC( ch ) >= -200 )
         sprintf( buf, "xenon plating" );
      else if( GET_AC( ch ) >= -300 )
         sprintf( buf, "creon plating" );
      else if( GET_AC( ch ) >= -400 )
         sprintf( buf, "mythril plating" );
      else if( GET_AC( ch ) >= -550 )
         sprintf( buf, "cirra plating" );
      else if( GET_AC( ch ) >= -700 )
         sprintf( buf, "aflua plating" );
      else if( GET_AC( ch ) >= -1000 )
         sprintf( buf, "elemental plating" );
      else if( GET_AC( ch ) >= -1500 )
         sprintf( buf, "diaoite plating" );
      else if( GET_AC( ch ) >= -2000 )
         sprintf( buf, "jasminsite plating" );
      else if( GET_AC( ch ) >= -2500 )
         sprintf( buf, "shrouds of immortality" );
      else
         sprintf( buf, "Ironhand Armored" );
      if( ch->level > 24 )
         pager_printf_color( ch, "&gArmor Class   : &W%d &g\t\t\tYou are of &G%s&g.\n\r", GET_AC( ch ), buf );
      else
         pager_printf_color( ch, "&gArmor Class   : &W%s&g.\n\r", buf );
      if( ch->level >= 15 )
         pager_printf_color( ch, "&gHitroll       : &W%d &g\t\t\tDamroll       : &W%d\n\r",
                             GET_HITROLL( ch ), GET_DAMROLL( ch ) );

      if( IS_IMMORTAL( ch ) )
      {
         if( ch->alignment > 900 )
            sprintf( buf, "Master of Angels" );
         else if( ch->alignment > 700 )
            sprintf( buf, "saintly" );
         else if( ch->alignment > 350 )
            sprintf( buf, "devout" );
         else if( ch->alignment > 100 )
            sprintf( buf, "good" );
         else if( ch->alignment > -100 )
            sprintf( buf, "neutral" );
         else if( ch->alignment > -350 )
            sprintf( buf, "evil" );
         else if( ch->alignment > -700 )
            sprintf( buf, "diabolical" );
         else if( ch->alignment > -900 )
            sprintf( buf, "satanic" );
         else if( ch->alignment < -900 )
            sprintf( buf, "Satan's Master" );
      }
      else if( !IS_IMMORTAL( ch ) )
      {
         if( ch->alignment > 900 )
            sprintf( buf, "angelic" );
         else if( ch->alignment > 700 )
            sprintf( buf, "saintly" );
         else if( ch->alignment > 350 )
            sprintf( buf, "devout" );
         else if( ch->alignment > 100 )
            sprintf( buf, "good" );
         else if( ch->alignment > -100 )
            sprintf( buf, "neutral" );
         else if( ch->alignment > -350 )
            sprintf( buf, "evil" );
         else if( ch->alignment > -700 )
            sprintf( buf, "diabolical" );
         else if( ch->alignment > -900 )
            sprintf( buf, "satanic" );
         else if( ch->alignment < -900 )
            sprintf( buf, "evil incarnate" );
      }
      if( ch->level < 10 )
         pager_printf_color( ch, "&gAlignment     : &W%s&g.\n\r", buf );
      else
         pager_printf_color( ch, "&gAlignment     : &W%d &g\t\t\tYou are &w%s&g.\n\r", ch->alignment, buf );
      pager_printf_color( ch, "&gKilled        : &W%d &g\t\t\tDied          : &W%d\n\r",
                          ch->pcdata->mkills, ch->pcdata->mdeaths );
      if( IS_PKILL( ch ) )
         pager_printf_color( ch, "&gPlayer Kills  : &W%d &g\t\t\tPlayer Deaths : &W%d\n\r",
                             ch->pcdata->pkills, ch->pcdata->pdeaths );

      switch ( ch->style )
      {
         case STYLE_EVASIVE:
            sprintf( buf, "evasive" );
            break;
         case STYLE_DEFENSIVE:
            sprintf( buf, "defensive" );
            break;
         case STYLE_AGGRESSIVE:
            sprintf( buf, "aggressive" );
            break;
         case STYLE_BERSERK:
            sprintf( buf, "berserk" );
            break;
         default:
            sprintf( buf, "standard" );
            break;
      }
      pager_printf_color( ch, "&gYou have wimpy set to &Y%d &ghits.\t\tYou are using &Y%s &gstyle.\n\r", ch->wimpy, buf );
      if( ch->level > 4 )
         pager_printf_color( ch, "&gYour Highest Damage Level is: &P%d&D\n\r", ch->pcdata->damhigh );
      if( ch->mount )
      {
         pager_printf_color( ch, "&gYou are mounted on: &W%s&g\n\r", ch->mount->short_descr );
      }
      send_to_pager_color( "&g&W----------------------------------------------------------------------------\n\r", ch );
      pager_printf_color( ch,
                          "&gDeaf: &W%-3s&g Pager: &W%-3s&g &GAUTOS&g: katyr: &W%-3s&g loot: &W%-3s&g sac: &W%-3s&g title: &W%-3s&g\n\r",
                          xIS_SET( ch->act, PLR_DEAF ) ? "Yes" : "No", IS_SET( ch->pcdata->flags,
                                                                               PCFLAG_PAGERON ) ? "Yes" : "No",
                          xIS_SET( ch->act, PLR_AUTOGOLD ) ? "Yes" : "No", xIS_SET( ch->act, PLR_AUTOLOOT ) ? "Yes" : "No",
                          xIS_SET( ch->act, PLR_AUTOSAC ) ? "Yes" : "No", xIS_SET( ch->act, PLR_A_TITLE ) ? "Yes" : "No" );
      if( xIS_SET( ch->act, PLR_SCREAFF ) )
      {
         int i;
         SKILLTYPE *sktmp;

         i = 0;
         send_to_pager_color( "&W----------------------------------------------------------------------------&g\n\r", ch );
         send_to_pager_color( "AFFECT DATA:                            ", ch );
         for( paf = ch->first_affect; paf; paf = paf->next )
         {
            if( ( sktmp = get_skilltype( paf->type ) ) == NULL )
               continue;
            if( ch->level < 20 )
            {
               pager_printf_color( ch, "&g[&W%-34.34s&g]    ", sktmp->name );
               if( i == 0 )
                  i = 2;
               if( ( ++i % 3 ) == 0 )
                  send_to_pager( "\n\r", ch );
            }
            if( ch->level >= 20 )
            {
               if( paf->modifier == 0 )
                  pager_printf_color( ch, "&g[&W%-24.24s;%5d &grds]    ", sktmp->name, paf->duration );
               else if( paf->modifier > 999 )
                  pager_printf_color( ch, "&g[&W%-15.15s; %7.7s;%5d &grds]    ",
                                      sktmp->name, tiny_affect_loc_name( paf->location ), paf->duration );
               else
                  pager_printf_color( ch, "&g[&W%-11.11s;%+-3.3d %7.7s;%5d &grds]    ",
                                      sktmp->name, paf->modifier, tiny_affect_loc_name( paf->location ), paf->duration );
               if( i == 0 )
                  i = 1;
               if( ( ++i % 2 ) == 0 )
                  send_to_pager( "\n\r", ch );
            }
         }
      }
   }
   if( !xIS_SET( ch->act, PLR_SCRESHRT ) && !xIS_SET( ch->act, PLR_MIDSCORE ) )
   {
      pager_printf_color( ch, "\n\r&g%s%s\n\r", IS_SNAME( ch ) ? ch->pcdata->sname : ch->name, ch->pcdata->title );
      if( ch->pcdata->spouse )
         pager_printf_color( ch, "&gYou are married to %s.\n\r", ch->pcdata->spouse );
      if( get_trust( ch ) != ch->level )
         pager_printf( ch, "You are trusted at level %d.\n\r", get_trust( ch ) );
      if( ch->pcdata->wasat != 0 )
         pager_printf( ch, "You need to get %d levels to try to tier again.\n\r", ch->pcdata->wasat );

      send_to_pager_color( "&g&W----------------------------------------------------------------------------\n\r", ch );

      pager_printf_color( ch, "&gStrength      : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou are &Y%d&g years old (&Y%d hours&g).\n\r",
                          ch->perm_str, get_curr_str( ch ), get_age( ch ), ( get_age( ch ) - 17 ) * 2 );
      pager_printf_color( ch, "&gIntelligence  : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou have &R%d&g/&r%d&g hits.\n\r",
                          ch->perm_int, get_curr_int( ch ), ch->hit, ch->max_hit );
      if( IS_VAMPIRE( ch ) )
         pager_printf_color( ch, "&gWisdom        : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou have &R%d&g/&r%d&g blood.\n\r",
                             ch->perm_wis, get_curr_wis( ch ), ch->blood, ch->max_blood );
      else
         pager_printf_color( ch, "&gWisdom        : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou have &R%d&g/&r%d&g mana.\n\r",
                             ch->perm_wis, get_curr_wis( ch ), ch->mana, ch->max_mana );
      pager_printf_color( ch, "&gDexterity     : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou have &R%d&g/&r%d&g moves.\n\r",
                          ch->perm_dex, get_curr_dex( ch ), ch->move, ch->max_move );
      pager_printf_color( ch, "&gConstitution  : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou are carrying &W%d&g/&w%d&g items.\n\r",
                          ch->perm_con, get_curr_con( ch ), ch->carry_number, can_carry_n( ch ) );
      pager_printf_color( ch, "&gCharisma      : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou are carrying &W%d&g/&w%d&g lbs.\n\r",
                          ch->perm_cha, get_curr_cha( ch ), ch->carry_weight, can_carry_w( ch ) );

      switch ( ch->position )
      {
         case POS_DEAD:
            sprintf( buf, "slowly decomposing" );
            break;
         case POS_MORTAL:
            sprintf( buf, "mortally wounded" );
            break;
         case POS_INCAP:
            sprintf( buf, "incapacitated" );
            break;
         case POS_STUNNED:
            sprintf( buf, "stunned" );
            break;
         case POS_SLEEPING:
            sprintf( buf, "sleeping" );
            break;
         case POS_RESTING:
            sprintf( buf, "resting" );
            break;
         case POS_STANDING:
            sprintf( buf, "standing" );
            break;
         case POS_FIGHTING:
            sprintf( buf, "fighting" );
            break;
         case POS_EVASIVE:
            sprintf( buf, "fighting (evasive)" );
            break;
         case POS_DEFENSIVE:
            sprintf( buf, "fighting (defensive)" );
            break;
         case POS_AGGRESSIVE:
            sprintf( buf, "fighting (aggressive)" );
            break;
         case POS_BERSERK:
            sprintf( buf, "fighting (berserk)" );
            break;
         case POS_MOUNTED:
            sprintf( buf, "mounted" );
            break;
         case POS_SITTING:
            sprintf( buf, "sitting" );
            break;
      }
      pager_printf_color( ch, "&gLuck          : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou are currently &R%s&g.\n\r",
                          ch->perm_lck, get_curr_lck( ch ), buf );

      if( ch->level < 100 )
      {
         if( ch->dualclass > -1 )
         {
            pager_printf_color( ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g Race: &Y%s&g.\n\r",
                                ch->exp, ch->level, get_race( ch ) );
            pager_printf_color( ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s &gDual Class: &Y%s&g.\n\r",
                                exp_level( ch, ( ch->level + 1 ) ) - ch->exp, get_class( ch ), get_dualclass( ch ) );
         }
         else
         {
            pager_printf_color( ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g Race: &Y%s&g.\n\r",
                                ch->exp, ch->level, get_race( ch ) );
            pager_printf_color( ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s&g.\n\r",
                                exp_level( ch, ( ch->level + 1 ) ) - ch->exp, get_class( ch ) );
         }
      }
      else
      {
         if( ch->dualclass > -1 )
         {
            pager_printf_color( ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g(&R%d&g) Race: &Y%s&g.\n\r",
                                ch->exp, ch->level, HAS_AWARD( ch ), get_race( ch ) );
            pager_printf_color( ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s &gDual Class: &Y%s&g.\n\r",
                                IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch, ( ch->sublevel + 1 ) ) - ch->exp,
                                get_class( ch ), get_dualclass( ch ) );
         }
         else
         {
            pager_printf_color( ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g(&R%d&g) Race: &Y%s&g.\n\r",
                                ch->exp, ch->level, HAS_AWARD( ch ), get_race( ch ) );
            pager_printf_color( ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s&g.\n\r",
                                IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch, ( ch->sublevel + 1 ) ) - ch->exp,
                                get_class( ch ) );
         }
      }

      if( GET_AC( ch ) >= 400 )
         sprintf( buf, "worse than naked" );
      else if( GET_AC( ch ) >= 300 )
         sprintf( buf, "naked" );
      else if( GET_AC( ch ) >= 101 )
         sprintf( buf, "the rags of a prisoner" );
      else if( GET_AC( ch ) >= 80 )
         sprintf( buf, "clothes of a beggar" );
      else if( GET_AC( ch ) >= 55 )
         sprintf( buf, "squire's hides" );
      else if( GET_AC( ch ) >= 40 )
         sprintf( buf, "leather plating" );
      else if( GET_AC( ch ) >= 20 )
         sprintf( buf, "copper plating" );
      else if( GET_AC( ch ) >= 10 )
         sprintf( buf, "gold plating" );
      else if( GET_AC( ch ) >= 0 )
         sprintf( buf, "shod iron plating" );
      else if( GET_AC( ch ) >= -10 )
         sprintf( buf, "iron plating" );
      else if( GET_AC( ch ) >= -20 )
         sprintf( buf, "steel plating" );
      else if( GET_AC( ch ) >= -40 )
         sprintf( buf, "graphite carbon plating" );
      else if( GET_AC( ch ) >= -60 )
         sprintf( buf, "kevlar plating" );
      else if( GET_AC( ch ) >= -80 )
         sprintf( buf, "titanium plating" );
      else if( GET_AC( ch ) >= -100 )
         sprintf( buf, "mystic plating" );
      else if( GET_AC( ch ) >= -200 )
         sprintf( buf, "xenon plating" );
      else if( GET_AC( ch ) >= -300 )
         sprintf( buf, "creon plating" );
      else if( GET_AC( ch ) >= -400 )
         sprintf( buf, "mythril plating" );
      else if( GET_AC( ch ) >= -550 )
         sprintf( buf, "cirra plating" );
      else if( GET_AC( ch ) >= -700 )
         sprintf( buf, "aflua plating" );
      else if( GET_AC( ch ) >= -1000 )
         sprintf( buf, "elemental plating" );
      else if( GET_AC( ch ) >= -1500 )
         sprintf( buf, "diaoite plating" );
      else if( GET_AC( ch ) >= -2000 )
         sprintf( buf, "jasminsite plating" );
      else if( GET_AC( ch ) >= -2500 )
         sprintf( buf, "shrouds of immortality" );
      else
         sprintf( buf, "Ironhand Armored" );
      if( ch->level > 24 )
         pager_printf_color( ch, "&gArmor Class   : &W%d &g\t\t\tYou are of &G%s&g.\n\r", GET_AC( ch ), buf );
      else
         pager_printf_color( ch, "&gArmor Class   : &W%s&g.\n\r", buf );
      if( ch->level >= 15 )
         pager_printf_color( ch, "&gHitroll       : &W%d &g\t\t\tDamroll       : &W%d\n\r",
                             GET_HITROLL( ch ), GET_DAMROLL( ch ) );

      if( IS_IMMORTAL( ch ) )
      {
         if( ch->alignment > 900 )
            sprintf( buf, "Master of Angels" );
         else if( ch->alignment > 700 )
            sprintf( buf, "saintly" );
         else if( ch->alignment > 350 )
            sprintf( buf, "devout" );
         else if( ch->alignment > 100 )
            sprintf( buf, "good" );
         else if( ch->alignment > -100 )
            sprintf( buf, "neutral" );
         else if( ch->alignment > -350 )
            sprintf( buf, "evil" );
         else if( ch->alignment > -700 )
            sprintf( buf, "diabolical" );
         else if( ch->alignment > -900 )
            sprintf( buf, "satanic" );
         else if( ch->alignment < -900 )
            sprintf( buf, "Satan's Master" );
      }
      else if( !IS_IMMORTAL( ch ) )
      {
         if( ch->alignment > 900 )
            sprintf( buf, "angelic" );
         else if( ch->alignment > 700 )
            sprintf( buf, "saintly" );
         else if( ch->alignment > 350 )
            sprintf( buf, "devout" );
         else if( ch->alignment > 100 )
            sprintf( buf, "good" );
         else if( ch->alignment > -100 )
            sprintf( buf, "neutral" );
         else if( ch->alignment > -350 )
            sprintf( buf, "evil" );
         else if( ch->alignment > -700 )
            sprintf( buf, "diabolical" );
         else if( ch->alignment > -900 )
            sprintf( buf, "satanic" );
         else if( ch->alignment < -900 )
            sprintf( buf, "evil incarnate" );
      }
      if( ch->level < 10 )
         pager_printf_color( ch, "&gAlignment     : &W%s&g.\n\r", buf );
      else
         pager_printf_color( ch, "&gAlignment     : &W%d &g\t\t\tYou are &w%s&g.\n\r", ch->alignment, buf );
      pager_printf_color( ch, "&gKilled        : &W%d &g\t\t\tDied          : &W%d\n\r",
                          ch->pcdata->mkills, ch->pcdata->mdeaths );
      if( IS_PKILL( ch ) )
         pager_printf_color( ch, "&gPlayer Kills  : &W%d &g\t\t\tPlayer Deaths : &W%d\n\r",
                             ch->pcdata->pkills, ch->pcdata->pdeaths );

      switch ( ch->style )
      {
         case STYLE_EVASIVE:
            sprintf( buf, "evasive" );
            break;
         case STYLE_DEFENSIVE:
            sprintf( buf, "defensive" );
            break;
         case STYLE_AGGRESSIVE:
            sprintf( buf, "aggressive" );
            break;
         case STYLE_BERSERK:
            sprintf( buf, "berserk" );
            break;
         default:
            sprintf( buf, "standard" );
            break;
      }
      pager_printf_color( ch, "&gYou have wimpy set to &Y%d &ghits.\t\tYou are using &Y%s &gstyle.\n\r", ch->wimpy, buf );
      pager_printf_color( ch, "&gYou have &Y%4d &gpractices.\t\tYou have &W%d&g/&w%d &gglory.\n\r",
                          ch->practice, ch->pcdata->quest_curr, ch->pcdata->quest_accum );
      pager_printf_color( ch, "&gYou have &W%10.10s &gkatyr on hand.\t&gYou have &W%s&g katyr in the bank.\n\r",
                          num_punct( ch->gold ), num_punct( ch->pcdata->balance ) );

      if( ch->pcdata->bounty )
      {
         if( ch->pcdata->bountytype == 1 )
         {
            sprintf( buf3, "katyr" );
         }
         else if( ch->pcdata->bountytype == 2 )
         {
            sprintf( buf3, "experience" );
         }
         else if( ch->pcdata->bountytype == 3 )
         {
            sprintf( buf3, "practices" );
         }
         pager_printf_color( ch, "&gYou have been bountied for &W%s&g %s.\n\r", num_punct( ch->pcdata->bounty ), buf3 );
      }

      if( ch->level > 4 )
         pager_printf_color( ch, "&gYour Highest Damage Level is: &P%d&D\n\r", ch->pcdata->damhigh );

      if( ch->pcdata->deity )
      {
         pager_printf_color( ch, "&gYou are worshiping &W%s&g.\n\r", capitalize( ch->pcdata->deity->name ) );
      }
      if( ch->mount )
      {
         pager_printf_color( ch, "&gYou are mounted on: &W%s&g\n\r", ch->mount->short_descr );
      }
      if( ch->pcdata->clan && ch->pcdata->clan->clan_type != CLAN_ORDER && ch->pcdata->clan->clan_type != CLAN_GUILD
          && ch->pcdata->clan->pkyorn == 1 )
      {
         pager_printf_color( ch, "&gYou are in the clan &W%18s&g(&W%3s&g)\tYour rank is &W%-2d&g. \n\r",
                             ch->pcdata->clan->longname, ch->pcdata->clan->name, ch->pcdata->rank );
         pager_printf_color( ch,
                             "&gClan AvPkills: &W%-5d \t\t\t&gClan NonAvpkills: &W%-5d\n\r&gClan AvPdeaths: &W%-5d \t\t\t&gClan NonAvpdeaths: &W%-5d\n\r",
                             ch->pcdata->clan->pkills[5],
                             ( ch->pcdata->clan->pkills[0] + ch->pcdata->clan->pkills[1] + ch->pcdata->clan->pkills[2] +
                               ch->pcdata->clan->pkills[3] + ch->pcdata->clan->pkills[4] ), ch->pcdata->clan->pdeaths[5],
                             ( ch->pcdata->clan->pdeaths[0] + ch->pcdata->clan->pdeaths[1] + ch->pcdata->clan->pdeaths[2] +
                               ch->pcdata->clan->pdeaths[3] + ch->pcdata->clan->pdeaths[4] ) );
      }
      if( ch->pcdata->clan && ch->pcdata->clan->clan_type != CLAN_ORDER && ch->pcdata->clan->clan_type != CLAN_GUILD
          && ch->pcdata->clan->pkyorn == 0 )
      {
         pager_printf_color( ch, "&gYou are in the clan &W%18s&g(&W%3s&g)\tYour rank is &W%-2d&g. \n\r",
                             ch->pcdata->clan->longname, ch->pcdata->clan->name, ch->pcdata->rank );
      }
      if( ch->pcdata->clan && ch->pcdata->clan->clan_type == CLAN_ORDER )
      {
         pager_printf_color( ch, "&gYou are in the order &W%18s&g(&W%3s&g)&g. \n\r",
                             ch->pcdata->clan->longname, ch->pcdata->clan->name );
      }
      if( ch->pcdata->clan && ch->pcdata->clan->clan_type == CLAN_GUILD )
      {
         pager_printf_color( ch, "&gYou are in the guild &W%18s&g(&W%3s&g)&g. \n\r",
                             ch->pcdata->clan->longname, ch->pcdata->clan->name );
      }

      send_to_pager_color( "&c&W----------------------------------------------------------------------------\n\r", ch );
      pager_printf_color( ch,
                          "&gDeaf: &W%-3s&g Pager: &W%-3s&g &GAUTOS&g: katyr: &W%-3s&g loot: &W%-3s&g sac: &W%-3s&g title: &W%-3s&g\n\r",
                          xIS_SET( ch->act, PLR_DEAF ) ? "Yes" : "No", IS_SET( ch->pcdata->flags,
                                                                               PCFLAG_PAGERON ) ? "Yes" : "No",
                          xIS_SET( ch->act, PLR_AUTOGOLD ) ? "Yes" : "No", xIS_SET( ch->act, PLR_AUTOLOOT ) ? "Yes" : "No",
                          xIS_SET( ch->act, PLR_AUTOSAC ) ? "Yes" : "No", xIS_SET( ch->act, PLR_A_TITLE ) ? "Yes" : "No" );
      pager_printf_color( ch, "&gYou have your pager set to &Y%d &glines.\n\r", ch->pcdata->pagerlen );

      if( ch->created == '\0' || ch->created == "(null)" )
      {
         time_t cl;

         time( &cl );

         ch->created = ctime( &cl );
      }
      pager_printf_color( ch, "&gYou were created on &w%s\r", ch->created );

      if( !IS_AGOD( ch ) )
      {
         if( !IS_NPC( ch ) && ch->pcdata->condition[COND_DRUNK] > 10 )
            send_to_pager( "&gYou are drunk.\n\r", ch );
         if( !IS_NPC( ch ) && ch->pcdata->condition[COND_THIRST] == 0 )
            send_to_pager( "&gYou are in danger of dehydrating.\n\r", ch );
         if( !IS_NPC( ch ) && ch->pcdata->condition[COND_FULL] == 0 )
            send_to_pager( "&gYou are starving to death.\n\r", ch );
      }

      if( ch->pcdata->bestowments && ch->pcdata->bestowments[0] != '\0' )
         pager_printf_color( ch, "&gYou are bestowed with the command(s): &Y%s\n\r", ch->pcdata->bestowments );

      if( ch->morph && ch->morph->morph )
      {
         send_to_pager_color( "&W----------------------------------------------------------------------------&g\n\r", ch );
         if( IS_IMMORTAL( ch ) )
            pager_printf( ch, "Morphed as (%d) %s with a timer of %d.\n\r",
                          ch->morph->morph->vnum, ch->morph->morph->short_desc, ch->morph->timer );
         else
            pager_printf( ch, "You are morphed into a %s.\n\r", ch->morph->morph->short_desc );
      }

      if( xIS_SET( ch->act, EXTRA_PREGNANT ) )
      {
         send_to_pager_color( "&W----------------------------------------------------------------------------&g\n\r", ch );
         pager_printf_color( ch, "&gYou are pregnant with &W%s&g child.\n\r", ch->pcdata->cparents );
      }
      if( IS_ANGEL( ch ) )
      {
         send_to_pager_color( "&W----------------------------------------------------------------------------&g\n\r", ch );
         pager_printf( ch, "&gANGEL DATA:  Number of Restores Given: &Y%d\n\r", ch->pcdata->restore_number );
      }
      if( IS_IMM( ch ) )
      {
         switch ( ch->level )
         {
            case MAX_LEVEL - 0:
               sprintf( cla, "Host Admin" );
               break;
            case MAX_LEVEL - 10:
               sprintf( cla, "Host Senior" );
               break;
            case MAX_LEVEL - 20:
               sprintf( cla, "Host" );
               break;
            case MAX_LEVEL - 30:
               sprintf( cla, "High Templar" );
               break;
            case MAX_LEVEL - 40:
               sprintf( cla, "Arch Templar" );
               break;
            case MAX_LEVEL - 50:
               sprintf( cla, "Templar" );
               break;
            case MAX_LEVEL - 60:
               sprintf( cla, "Elder Wizard" );
               break;
            case MAX_LEVEL - 70:
               sprintf( cla, "Arch Wizard" );
               break;
            case MAX_LEVEL - 80:
               sprintf( cla, "Wizard" );
               break;
            case MAX_LEVEL - 90:
               sprintf( cla, "Grand Bishop" );
               break;
            case MAX_LEVEL - 100:
               sprintf( cla, "Arch Bishop" );
               break;
            case MAX_LEVEL - 110:
               sprintf( cla, "Bishop" );
               break;
            case MAX_LEVEL - 120:
               sprintf( cla, "Grand Lord" );
               break;
            case MAX_LEVEL - 130:
               sprintf( cla, "Lord" );
               break;
            case MAX_LEVEL - 140:
               sprintf( cla, "Arch Duke" );
               break;
            case MAX_LEVEL - 150:
               sprintf( cla, "Duke" );
               break;
            case MAX_LEVEL - 160:
               sprintf( cla, "Arch Druid" );
               break;
            case MAX_LEVEL - 170:
               sprintf( cla, "Druid" );
               break;
            case MAX_LEVEL - 200:
               sprintf( cla, "Retired" );
               break;
         }

         send_to_pager_color( "&W----------------------------------------------------------------------------&g\n\r", ch );
         pager_printf( ch, "&YIMMORTAL DATA&g:  Wizinvis [&z%s&g]  Wizlevel (&z%d&g) Immortal Rank: &z%s\n\r",
                       xIS_SET( ch->act, PLR_WIZINVIS ) ? "X" : " ", ch->pcdata->wizinvis, cla );
      }

      sprintf( bam, "%s appears in a swirling mist.", ch->name );
      sprintf( bam2, "%s disappears in a swirling mist.", ch->name );
      if( IS_ANGEL( ch ) )
      {
         pager_printf( ch, "&gBamfin:  &Y%s\n\r", ( ch->pcdata->bamfin[0] != '\0' ) ? ch->pcdata->bamfin : bam );
         pager_printf( ch, "&gBamfout: &Y%s\n\r", ( ch->pcdata->bamfout[0] != '\0' ) ? ch->pcdata->bamfout : bam2 );
      }
      if( IS_IMM( ch ) )
      {
         pager_printf( ch, "&gBamfin:  &Y%s\n\r", ( ch->pcdata->bamfin[0] != '\0' ) ? ch->pcdata->bamfin : bam );
         pager_printf( ch, "&gBamfout: &Y%s\n\r", ( ch->pcdata->bamfout[0] != '\0' ) ? ch->pcdata->bamfout : bam2 );

         if( ch->pcdata->area )
         {
            pager_printf( ch,
                          "&gVnums:   Room (&Y%-5.5d - %-5.5d&g)   Object (&Y%-5.5d - %-5.5d&g)   Mob (&Y%-5.5d - %-5.5d&g)\n\r",
                          ch->pcdata->area->low_r_vnum, ch->pcdata->area->hi_r_vnum, ch->pcdata->area->low_o_vnum,
                          ch->pcdata->area->hi_o_vnum, ch->pcdata->area->low_m_vnum, ch->pcdata->area->hi_m_vnum );
            pager_printf( ch, "Area &Y%s&g Loaded [&Y%s&g]\n\r", ch->pcdata->area->filename,
                          ( IS_SET( ch->pcdata->area->status, AREA_LOADED ) ) ? "yes" : "no" );
         }
      }
      if( xIS_SET( ch->act, PLR_SCREAFF ) )
      {
         int i;
         SKILLTYPE *sktmp;

         i = 0;
         send_to_pager_color( "&W----------------------------------------------------------------------------&g\n\r", ch );
         send_to_pager_color( "AFFECT DATA:                            ", ch );
         for( paf = ch->first_affect; paf; paf = paf->next )
         {
            if( ( sktmp = get_skilltype( paf->type ) ) == NULL )
               continue;
            if( ch->level < 20 )
            {
               pager_printf_color( ch, "&g[&W%-34.34s&g]    ", sktmp->name );
               if( i == 0 )
                  i = 2;
               if( ( ++i % 3 ) == 0 )
                  send_to_pager( "\n\r", ch );
            }
            if( ch->level >= 20 )
            {
               if( paf->modifier == 0 )
                  pager_printf_color( ch, "&g[&W%-24.24s;%5d &grds]    ", sktmp->name, paf->duration );
               else if( paf->modifier > 999 )
                  pager_printf_color( ch, "&g[&W%-15.15s; %7.7s;%5d &grds]    ",
                                      sktmp->name, tiny_affect_loc_name( paf->location ), paf->duration );
               else
                  pager_printf_color( ch, "&g[&W%-11.11s;%+-3.3d %7.7s;%5d &grds]    ",
                                      sktmp->name, paf->modifier, tiny_affect_loc_name( paf->location ), paf->duration );
               if( i == 0 )
                  i = 1;
               if( ( ++i % 2 ) == 0 )
                  send_to_pager( "\n\r", ch );
            }
         }
      }
   }
   send_to_pager( "\n\r", ch );
   return;
}

void do_scoresheet( CHAR_DATA * ch, char *argument )
{
/*    char buf[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];
    char bam[MAX_INPUT_LENGTH];
    char bam2[MAX_INPUT_LENGTH];
    char cla[MAX_INPUT_LENGTH];
    AFFECT_DATA    *paf;*/

   if( IS_NPC( ch ) )
   {
      do_score( ch, argument );
      return;
   }
   set_pager_color( AT_SCORE, ch );
   pager_printf_color( ch, "&B+---------{ &W%-12s &B}-----------------------------------------------------+&D\n\r",
                       ch->name );
   pager_printf_color( ch,
                       "&B| &cSTR &z%2.2d&g(&w%2.2d&g) &cINT &z%2.2d&g(&w%2.2d&g) &cWIS &z%2.2d&g(&w%2.2d&g) &cDEX &z%2.2d&g(&w%2.2d&g) &cCON &z%2.2d&g(&w%2.2d&g) &cCHA &z%2.2d&g(&w%2.2d&g) &cLCK &z%2.2d&g(&w%2.2d&g) &B|&D\n\r",
                       ch->perm_str, get_curr_str( ch ), ch->perm_int, get_curr_int( ch ), ch->perm_wis, get_curr_wis( ch ),
                       ch->perm_dex, get_curr_dex( ch ), ch->perm_con, get_curr_con( ch ), ch->perm_cha, get_curr_cha( ch ),
                       ch->perm_lck, get_curr_lck( ch ) );
   if( IS_VAMPIRE( ch ) )
   {
      pager_printf_color( ch, "&B| &gHit Points &R%8d&g/&r%-8d&g Blood &R%8d&g/&r%-8d&g Moves &R%8d&g/&r%-8d &B|&D\n\r",
                          ch->hit, ch->max_hit, ch->blood, ch->max_blood, ch->move, ch->max_move );
   }
   else
   {
      pager_printf_color( ch, "&B| &gHit Points &R%8d&g/&r%-8d&g Mana &R%8d&g/&r%-8d&g Moves &R%8d&g/&r%-8d  &B|&D\n\r",
                          ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move );
   }
   /*
    * pager_printf_color(ch, "\n\r&g%s%s\n\r", IS_SNAME(ch) ? ch->pcdata->sname : ch->name, ch->pcdata->title);
    * if ( ch->pcdata->spouse )
    * pager_printf_color(ch, "&gYou are married to %s.\n\r", ch->pcdata->spouse );
    * if ( get_trust( ch ) != ch->level )
    * pager_printf( ch, "You are trusted at level %d.\n\r", get_trust( ch ) );
    * if ( ch->pcdata->wasat != 0 )
    * pager_printf( ch, "You need to get %d levels to try to tier again.\n\r", ch->pcdata->wasat );
    * 
    * send_to_pager_color("&g&W----------------------------------------------------------------------------\n\r", ch);
    * 
    * pager_printf_color(ch, "&gStrength      : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou are &Y%d&g years old (&Y%d hours&g).\n\r", 
    * ch->perm_str, get_curr_str(ch), get_age(ch), (get_age(ch) - 17) * 2);
    * pager_printf_color(ch, "&gIntelligence  : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou have &R%d&g/&r%d&g hits.\n\r", 
    * ch->perm_int, get_curr_int(ch), ch->hit, ch->max_hit);
    * if (IS_VAMPIRE(ch))
    * pager_printf_color(ch, "&gWisdom        : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou have &R%d&g/&r%d&g blood.\n\r", 
    * ch->perm_wis, get_curr_wis(ch), ch->blood, ch->max_blood);
    * else
    * pager_printf_color(ch, "&gWisdom        : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou have &R%d&g/&r%d&g mana.\n\r", 
    * ch->perm_wis, get_curr_wis(ch), ch->mana, ch->max_mana);
    * pager_printf_color(ch, "&gDexterity     : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou have &R%d&g/&r%d&g moves.\n\r", 
    * ch->perm_dex, get_curr_dex(ch), ch->move, ch->max_move);
    * pager_printf_color(ch, "&gConstitution  : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou are carrying &W%d&g/&w%d&g items.\n\r", 
    * ch->perm_con, get_curr_con(ch), ch->carry_number, can_carry_n(ch));
    * pager_printf_color(ch, "&gCharisma      : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou are carrying &W%d&g/&w%d&g lbs.\n\r", 
    * ch->perm_cha, get_curr_cha(ch), ch->carry_weight, can_carry_w(ch));
    * 
    * switch (ch->position)
    * {
    * case POS_DEAD:
    * sprintf(buf, "slowly decomposing");
    * break;
    * case POS_MORTAL:
    * sprintf(buf, "mortally wounded");
    * break;
    * case POS_INCAP:
    * sprintf(buf, "incapacitated");
    * break;
    * case POS_STUNNED:
    * sprintf(buf, "stunned");
    * break;
    * case POS_SLEEPING:
    * sprintf(buf, "sleeping");
    * break;
    * case POS_RESTING:
    * sprintf(buf, "resting");
    * break;
    * case POS_STANDING:
    * sprintf(buf, "standing");
    * break;
    * case POS_FIGHTING:
    * sprintf(buf, "fighting");
    * break;
    * case POS_EVASIVE:
    * sprintf(buf, "fighting (evasive)");   
    * break;
    * case POS_DEFENSIVE:
    * sprintf(buf, "fighting (defensive)");
    * break;
    * case POS_AGGRESSIVE:
    * sprintf(buf, "fighting (aggressive)");
    * break;
    * case POS_BERSERK:
    * sprintf(buf, "fighting (berserk)");
    * break;
    * case POS_MOUNTED:
    * sprintf(buf, "mounted");
    * break;
    * case POS_SITTING:
    * sprintf(buf, "sitting");
    * break;
    * }
    * pager_printf_color(ch, "&gLuck          : &W%2.2d&G(&w%2.2d&G)\t\t\t&gYou are currently &R%s&g.\n\r", 
    * ch->perm_lck, get_curr_lck(ch), buf);
    * 
    * if ( ch->level < 100 )
    * {
    * if ( ch->dualclass > -1 )
    * {
    * pager_printf_color(ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g Race: &Y%s&g.\n\r", 
    * ch->exp, ch->level, get_race(ch) );
    * pager_printf_color(ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s &gDual Class: &Y%s&g.\n\r", 
    * exp_level(ch, (ch->level + 1)) - ch->exp, get_class(ch), get_dualclass(ch) );
    * }
    * else
    * {
    * pager_printf_color(ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g Race: &Y%s&g.\n\r", 
    * ch->exp, ch->level, get_race(ch) );
    * pager_printf_color(ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s&g.\n\r", 
    * exp_level(ch, (ch->level + 1)) - ch->exp, get_class(ch) );
    * }
    * }
    * else
    * {
    * if ( ch->dualclass > -1 )
    * {
    * pager_printf_color(ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g(&R%d&g) Race: &Y%s&g.\n\r", 
    * ch->exp, ch->level, HAS_AWARD(ch), get_race(ch) );
    * pager_printf_color(ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s &gDual Class: &Y%s&g.\n\r", 
    * IS_AGOD(ch) ? get_exp_tnl(ch) : exp_level(ch, (ch->sublevel + 1)) - ch->exp, get_class(ch), get_dualclass(ch) );
    * }
    * else
    * {
    * pager_printf_color(ch, "&gExperience    : &B%d\t\t\t&gLevel: &R%d&g(&R%d&g) Race: &Y%s&g.\n\r", 
    * ch->exp, ch->level, HAS_AWARD(ch), get_race(ch) );
    * pager_printf_color(ch, "&gTo Next Level : &B%d\t\t\t&gClass: &Y%s&g.\n\r", 
    * IS_AGOD(ch) ? get_exp_tnl(ch) : exp_level(ch, (ch->sublevel + 1)) - ch->exp, get_class(ch) );
    * }
    * }
    * 
    * if (GET_AC(ch) >= 400)
    * sprintf(buf, "worse than naked");
    * else if (GET_AC(ch) >= 300)
    * sprintf(buf, "naked");
    * else if (GET_AC(ch) >= 101)
    * sprintf(buf, "the rags of a prisoner");
    * else if (GET_AC(ch) >= 80)
    * sprintf(buf, "clothes of a beggar");
    * else if (GET_AC(ch) >= 55)
    * sprintf(buf, "squire's hides");
    * else if (GET_AC(ch) >= 40)
    * sprintf(buf, "leather plating");
    * else if (GET_AC(ch) >= 20)
    * sprintf(buf, "copper plating");
    * else if (GET_AC(ch) >= 10)
    * sprintf(buf, "gold plating");
    * else if (GET_AC(ch) >= 0)
    * sprintf(buf, "shod iron plating");
    * else if (GET_AC(ch) >= -10)
    * sprintf(buf, "iron plating");
    * else if (GET_AC(ch) >= -20)
    * sprintf(buf, "steel plating");
    * else if (GET_AC(ch) >= -40)
    * sprintf(buf, "graphite carbon plating");
    * else if (GET_AC(ch) >= -60)
    * sprintf(buf, "kevlar plating");
    * else if (GET_AC(ch) >= -80)
    * sprintf(buf, "titanium plating");
    * else if (GET_AC(ch) >= -100)
    * sprintf(buf, "mystic plating");
    * else if (GET_AC(ch) >= -200)
    * sprintf(buf, "xenon plating");
    * else if (GET_AC(ch) >= -300)
    * sprintf(buf, "creon plating");
    * else if (GET_AC(ch) >= -400)
    * sprintf(buf, "mythril plating");
    * else if (GET_AC(ch) >= -550)
    * sprintf(buf, "cirra plating");
    * else if (GET_AC(ch) >= -700)
    * sprintf(buf, "aflua plating");
    * else if (GET_AC(ch) >= -1000)
    * sprintf(buf, "elemental plating");
    * else if (GET_AC(ch) >= -1500)
    * sprintf(buf, "diaoite plating");
    * else if (GET_AC(ch) >= -2000)
    * sprintf(buf, "jasminsite plating");
    * else if (GET_AC(ch) >= -2500)
    * sprintf(buf, "shrouds of immortality");
    * else
    * sprintf(buf, "Ironhand Armored");
    * if (ch->level > 24)
    * pager_printf_color(ch, "&gArmor Class   : &W%d &g\t\t\tYou are of &G%s&g.\n\r", 
    * GET_AC(ch), buf);
    * else
    * pager_printf_color(ch, "&gArmor Class   : &W%s&g.\n\r", 
    * buf);
    * if (ch->level >= 15)
    * pager_printf_color(ch, "&gHitroll       : &W%d &g\t\t\tDamroll       : &W%d\n\r", 
    * GET_HITROLL(ch), GET_DAMROLL(ch));
    * 
    * if (IS_IMMORTAL(ch))
    * {
    * if (ch->alignment > 900)
    * sprintf(buf, "Master of Angels");
    * else if (ch->alignment > 700)
    * sprintf(buf, "saintly");
    * else if (ch->alignment > 350)
    * sprintf(buf, "devout");
    * else if (ch->alignment > 100)
    * sprintf(buf, "good");
    * else if (ch->alignment > -100)
    * sprintf(buf, "neutral");
    * else if (ch->alignment > -350)
    * sprintf(buf, "evil");
    * else if (ch->alignment > -700)
    * sprintf(buf, "diabolical");
    * else if (ch->alignment > -900)
    * sprintf(buf, "satanic");
    * else if (ch->alignment < -900)
    * sprintf(buf, "Satan's Master");
    * }
    * else if (!IS_IMMORTAL(ch))
    * {
    * if (ch->alignment > 900)
    * sprintf(buf, "angelic");
    * else if (ch->alignment > 700)
    * sprintf(buf, "saintly");
    * else if (ch->alignment > 350)
    * sprintf(buf, "devout");
    * else if (ch->alignment > 100)
    * sprintf(buf, "good");
    * else if (ch->alignment > -100)
    * sprintf(buf, "neutral");
    * else if (ch->alignment > -350)
    * sprintf(buf, "evil");
    * else if (ch->alignment > -700)
    * sprintf(buf, "diabolical");
    * else if (ch->alignment > -900)
    * sprintf(buf, "satanic");
    * else if (ch->alignment < -900)
    * sprintf(buf, "evil incarnate");
    * }
    * if (ch->level < 10)
    * pager_printf_color(ch, "&gAlignment     : &W%s&g.\n\r", 
    * buf);
    * else
    * pager_printf_color(ch, "&gAlignment     : &W%d &g\t\t\tYou are &w%s&g.\n\r", 
    * ch->alignment, buf);
    * pager_printf_color(ch, "&gKilled        : &W%d &g\t\t\tDied          : &W%d\n\r", 
    * ch->pcdata->mkills, ch->pcdata->mdeaths);
    * if ( IS_PKILL(ch))
    * pager_printf_color(ch, "&gPlayer Kills  : &W%d &g\t\t\tPlayer Deaths : &W%d\n\r", 
    * ch->pcdata->pkills, ch->pcdata->pdeaths);
    * 
    * switch (ch->style) {
    * case STYLE_EVASIVE:
    * sprintf(buf, "evasive");
    * break;
    * case STYLE_DEFENSIVE:
    * sprintf(buf, "defensive");
    * break;
    * case STYLE_AGGRESSIVE:
    * sprintf(buf, "aggressive");
    * break;
    * case STYLE_BERSERK:
    * sprintf(buf, "berserk");
    * break;
    * default:
    * sprintf(buf, "standard");
    * break;
    * }
    * pager_printf_color(ch, "&gYou have wimpy set to &Y%d &ghits.\t\tYou are using &Y%s &gstyle.\n\r", 
    * ch->wimpy, buf);
    * pager_printf_color(ch, "&gYou have &Y%4d &gpractices.\t\tYou have &W%d&g/&w%d &gglory.\n\r", 
    * ch->practice, ch->pcdata->quest_curr, ch->pcdata->quest_accum);
    * pager_printf_color(ch, "&gYou have &W%10.10s &gkatyr on hand.\t&gYou have &W%s&g katyr in the bank.\n\r",
    * num_punct(ch->gold), num_punct(ch->pcdata->balance) );
    * pager_printf_color(ch, "&gYou have &W%10.10s &gsiam on hand.\t&gYou have &W%s&g siam in the bank.\n\r",
    * num_punct(ch->silver), num_punct(ch->pcdata->balance2) );
    * pager_printf_color(ch, "&gYou have &W%10.10s &grona on hand.\t&gYou have &W%s&g rona in the bank.\n\r",
    * num_punct(ch->copper), num_punct(ch->pcdata->balance3) );
    * 
    * if ( ch->pcdata->bounty )
    * {
    * if ( ch->pcdata->bountytype == 1 )
    * {
    * sprintf( buf3, "katyr" );
    * }
    * else if ( ch->pcdata->bountytype == 2 )
    * {
    * sprintf( buf3, "experience" );
    * }
    * else if ( ch->pcdata->bountytype == 3 )
    * {
    * sprintf( buf3, "practices" );
    * }
    * pager_printf_color(ch, "&gYou have been bountied for &W%s&g %s.\n\r", num_punct(ch->pcdata->bounty), buf3 );
    * }
    * 
    * if ( ch->level > 4 )
    * pager_printf_color(ch, "&gYour Highest Damage Level is: &P%d&D\n\r", ch->pcdata->damhigh );
    * 
    * if (ch->pcdata->deity)
    * {
    * pager_printf_color(ch, "&gYou are worshiping &W%s&g.\n\r", capitalize( ch->pcdata->deity->name ) );
    * }
    * if ( ch->mount )
    * {
    * pager_printf_color(ch, "&gYou are mounted on: &W%s&g\n\r",
    * ch->mount->short_descr );
    * }
    * if (ch->pcdata->clan && ch->pcdata->clan->clan_type != CLAN_ORDER  && ch->pcdata->clan->clan_type != CLAN_GUILD && ch->pcdata->clan->pkyorn == 1)
    * {
    * pager_printf_color(ch, "&gYou are in the clan &W%18s&g(&W%3s&g)\tYour rank is &W%-2d&g. \n\r",
    * ch->pcdata->clan->longname, ch->pcdata->clan->name, ch->pcdata->rank );
    * pager_printf_color(ch, "&gClan AvPkills: &W%-5d \t\t\t&gClan NonAvpkills: &W%-5d\n\r&gClan AvPdeaths: &W%-5d \t\t\t&gClan NonAvpdeaths: &W%-5d\n\r",
    * ch->pcdata->clan->pkills[5],
    * (ch->pcdata->clan->pkills[0]+ch->pcdata->clan->pkills[1]+
    * ch->pcdata->clan->pkills[2]+ch->pcdata->clan->pkills[3]+
    * ch->pcdata->clan->pkills[4]),
    * ch->pcdata->clan->pdeaths[5],
    * ( ch->pcdata->clan->pdeaths[0] + ch->pcdata->clan->pdeaths[1] +
    * ch->pcdata->clan->pdeaths[2] + ch->pcdata->clan->pdeaths[3] +
    * ch->pcdata->clan->pdeaths[4] ) );
    * }
    * if (ch->pcdata->clan && ch->pcdata->clan->clan_type != CLAN_ORDER  && ch->pcdata->clan->clan_type != CLAN_GUILD && ch->pcdata->clan->pkyorn == 0)
    * {
    * pager_printf_color(ch, "&gYou are in the clan &W%18s&g(&W%3s&g)\tYour rank is &W%-2d&g. \n\r",
    * ch->pcdata->clan->longname, ch->pcdata->clan->name, ch->pcdata->rank );
    * }
    * if (ch->pcdata->clan && ch->pcdata->clan->clan_type == CLAN_ORDER )
    * {
    * pager_printf_color(ch, "&gYou are in the order &W%18s&g(&W%3s&g)&g. \n\r",
    * ch->pcdata->clan->longname, ch->pcdata->clan->name );
    * }
    * if (ch->pcdata->clan && ch->pcdata->clan->clan_type == CLAN_GUILD )
    * {
    * pager_printf_color(ch, "&gYou are in the guild &W%18s&g(&W%3s&g)&g. \n\r",
    * ch->pcdata->clan->longname, ch->pcdata->clan->name );
    * }
    * 
    * send_to_pager_color("&c&W----------------------------------------------------------------------------\n\r", ch);
    * pager_printf_color(ch, "&gDeaf: &W%-3s&g Pager: &W%-3s&g &GAUTOS&g: katyr: &W%-3s&g loot: &W%-3s&g sac: &W%-3s&g title: &W%-3s&g\n\r",
    * xIS_SET(ch->act, PLR_DEAF) ? "Yes" : "No", IS_SET(ch->pcdata->flags, PCFLAG_PAGERON) ? "Yes" : "No", xIS_SET(ch->act, PLR_AUTOGOLD) ? "Yes" : "No", 
    * xIS_SET(ch->act, PLR_AUTOLOOT) ? "Yes" : "No", xIS_SET(ch->act, PLR_AUTOSAC) ? "Yes" : "No", xIS_SET(ch->act, PLR_A_TITLE) ? "Yes" : "No" );
    * pager_printf_color(ch, "&gYou have your pager set to &Y%d &glines.\n\r", 
    * ch->pcdata->pagerlen );
    * 
    * if ( ch->created == '\0' || ch->created == "(null)" )
    * {
    * time_t cl;
    * 
    * time( &cl );
    * 
    * ch->created = ctime( &cl );
    * }
    * pager_printf_color(ch, "&gYou were created on &w%s\r", ch->created );
    * 
    * if ( !IS_AGOD(ch) )
    * {
    * if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
    * send_to_pager("&gYou are drunk.\n\r", ch);
    * if (!IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] == 0)
    * send_to_pager("&gYou are in danger of dehydrating.\n\r", ch);
    * if (!IS_NPC(ch) && ch->pcdata->condition[COND_FULL] == 0)
    * send_to_pager("&gYou are starving to death.\n\r", ch);
    * }
    * 
    * if ( ch->pcdata->bestowments && ch->pcdata->bestowments[0] != '\0' )
    * pager_printf_color(ch, "&gYou are bestowed with the command(s): &Y%s\n\r", 
    * ch->pcdata->bestowments );
    * 
    * if ( ch->morph && ch->morph->morph )
    * {
    * send_to_pager_color("&W----------------------------------------------------------------------------&g\n\r", ch);
    * if ( IS_IMMORTAL( ch ) )
    * pager_printf (ch, "Morphed as (%d) %s with a timer of %d.\n\r",
    * ch->morph->morph->vnum, ch->morph->morph->short_desc, 
    * ch->morph->timer
    * );
    * else
    * pager_printf (ch, "You are morphed into a %s.\n\r",
    * ch->morph->morph->short_desc );
    * } 
    * 
    * if ( xIS_SET(ch->act, EXTRA_PREGNANT ) )
    * {
    * send_to_pager_color( "&W----------------------------------------------------------------------------&g\n\r", ch);
    * pager_printf_color(ch, "&gYou are pregnant with &W%s&g child.\n\r",
    * ch->pcdata->cparents );
    * }
    * if (IS_ANGEL(ch))
    * {
    * send_to_pager_color("&W----------------------------------------------------------------------------&g\n\r", ch);
    * pager_printf(ch, "&gANGEL DATA:  Number of Restores Given: &Y%d\n\r",
    * ch->pcdata->restore_number );
    * }
    * if (IS_IMM(ch))
    * {
    * switch( ch->level )
    * {
    * case MAX_LEVEL - 0:    sprintf( cla, "Host Admin");  break;
    * case MAX_LEVEL - 10:   sprintf( cla, "Host Senior"); break;
    * case MAX_LEVEL - 20:   sprintf( cla, "Host");        break;
    * case MAX_LEVEL - 30:   sprintf( cla, "High Templar");   break;
    * case MAX_LEVEL - 40:   sprintf( cla, "Arch Templar");   break;
    * case MAX_LEVEL - 50:   sprintf( cla, "Templar");     break;
    * case MAX_LEVEL - 60:   sprintf( cla, "Elder Wizard");   break;
    * case MAX_LEVEL - 70:   sprintf( cla, "Arch Wizard"); break;
    * case MAX_LEVEL - 80:   sprintf( cla, "Wizard");      break;
    * case MAX_LEVEL - 90:   sprintf( cla, "Grand Bishop");   break;
    * case MAX_LEVEL - 100:  sprintf( cla, "Arch Bishop"); break;
    * case MAX_LEVEL - 110:  sprintf( cla, "Bishop");      break;
    * case MAX_LEVEL - 120:  sprintf( cla, "Grand Lord");  break;
    * case MAX_LEVEL - 130:  sprintf( cla, "Lord");        break;
    * case MAX_LEVEL - 140:  sprintf( cla, "Arch Duke");      break;
    * case MAX_LEVEL - 150:  sprintf( cla, "Duke");        break;
    * case MAX_LEVEL - 160:  sprintf( cla, "Arch Druid");  break;
    * case MAX_LEVEL - 170:  sprintf( cla, "Druid");       break;
    * case MAX_LEVEL - 200:  sprintf( cla, "Retired");     break;
    * }
    * 
    * send_to_pager_color("&W----------------------------------------------------------------------------&g\n\r", ch);
    * pager_printf(ch, "&YIMMORTAL DATA&g:  Wizinvis [&z%s&g]  Wizlevel (&z%d&g) Immortal Rank: &z%s\n\r",
    * xIS_SET(ch->act, PLR_WIZINVIS) ? "X" : " ", ch->pcdata->wizinvis, cla );
    * }
    * 
    * sprintf( bam, "%s appears in a swirling mist.", ch->name );
    * sprintf( bam2, "%s disappears in a swirling mist.", ch->name );
    * if (IS_ANGEL(ch))
    * {
    * pager_printf(ch, "&gBamfin:  &Y%s\n\r", (ch->pcdata->bamfin[0] != '\0')
    * ? ch->pcdata->bamfin : bam);
    * pager_printf(ch, "&gBamfout: &Y%s\n\r", (ch->pcdata->bamfout[0] != '\0')
    * ? ch->pcdata->bamfout : bam2);
    * }
    * if (IS_IMM(ch))
    * {
    * pager_printf(ch, "&gBamfin:  &Y%s\n\r", (ch->pcdata->bamfin[0] != '\0')
    * ? ch->pcdata->bamfin : bam);
    * pager_printf(ch, "&gBamfout: &Y%s\n\r", (ch->pcdata->bamfout[0] != '\0')
    * ? ch->pcdata->bamfout : bam2);
    * 
    * if (ch->pcdata->area)
    * {
    * pager_printf(ch, "&gVnums:   Room (&Y%-5.5d - %-5.5d&g)   Object (&Y%-5.5d - %-5.5d&g)   Mob (&Y%-5.5d - %-5.5d&g)\n\r",
    * ch->pcdata->area->low_r_vnum, ch->pcdata->area->hi_r_vnum,
    * ch->pcdata->area->low_o_vnum, ch->pcdata->area->hi_o_vnum,
    * ch->pcdata->area->low_m_vnum, ch->pcdata->area->hi_m_vnum);
    * pager_printf(ch, "Area &Y%s&g Loaded [&Y%s&g]\n\r", ch->pcdata->area->filename, (IS_SET (ch->pcdata->area->status, AREA_LOADED)) ? "yes" : "no");
    * }
    * }
    * if ( xIS_SET(ch->act, PLR_SCREAFF ) )
    * {
    * int i;
    * SKILLTYPE *sktmp;
    * 
    * i = 0;
    * send_to_pager_color( "&W----------------------------------------------------------------------------&g\n\r", ch);
    * send_to_pager_color("AFFECT DATA:                            ", ch);
    * for (paf = ch->first_affect; paf; paf = paf->next)
    * {
    * if ( (sktmp=get_skilltype(paf->type)) == NULL )
    * continue;
    * if (ch->level < 20)
    * {
    * pager_printf_color(ch, "&g[&W%-34.34s&g]    ", sktmp->name);
    * if (i == 0)
    * i = 2;
    * if ((++i % 3) == 0)
    * send_to_pager("\n\r", ch);
    * }
    * if (ch->level >= 20)
    * {
    * if (paf->modifier == 0)
    * pager_printf_color(ch, "&g[&W%-24.24s;%5d &grds]    ",
    * sktmp->name,
    * paf->duration);
    * else
    * if (paf->modifier > 999)
    * pager_printf_color(ch, "&g[&W%-15.15s; %7.7s;%5d &grds]    ",
    * sktmp->name,
    * tiny_affect_loc_name(paf->location),
    * paf->duration);
    * else
    * pager_printf_color(ch, "&g[&W%-11.11s;%+-3.3d %7.7s;%5d &grds]    ",
    * sktmp->name,
    * paf->modifier,
    * tiny_affect_loc_name(paf->location),
    * paf->duration);
    * if (i == 0)
    * i = 1;
    * if ((++i % 2) == 0)
    * send_to_pager("\n\r", ch);
    * }
    * }
    * }
    */
   send_to_pager( "\n\r", ch );
   return;
}

char *tiny_affect_loc_name( int location )
{
   switch ( location )
   {
      case APPLY_NONE:
         return "NIL";
      case APPLY_STR:
         return " STR  ";
      case APPLY_DEX:
         return " DEX  ";
      case APPLY_INT:
         return " INT  ";
      case APPLY_WIS:
         return " WIS  ";
      case APPLY_CON:
         return " CON  ";
      case APPLY_CHA:
         return " CHA  ";
      case APPLY_LCK:
         return " LCK  ";
      case APPLY_SEX:
         return " SEX  ";
      case APPLY_CLASS:
         return " CLASS";
      case APPLY_LEVEL:
         return " LVL  ";
      case APPLY_AGE:
         return " AGE  ";
      case APPLY_MANA:
         return " MANA ";
      case APPLY_HIT:
         return " HV   ";
      case APPLY_MOVE:
         return " MOVE ";
      case APPLY_GOLD:
         return " GOLD ";
      case APPLY_SILVER:
         return " SILVER";
      case APPLY_COPPER:
         return " COPPER";
      case APPLY_EXP:
         return " EXP  ";
      case APPLY_AC:
         return " AC   ";
      case APPLY_HITROLL:
         return " HITRL";
      case APPLY_DAMROLL:
         return " DAMRL";
      case APPLY_EAAC:
         return " EAAC ";
      case APPLY_EWHITROLL:
         return " EWHTR";
      case APPLY_EWDAMROLL:
         return " EWDMR";
      case APPLY_SAVING_POISON:
         return "SV POI";
      case APPLY_SAVING_ROD:
         return "SV ROD";
      case APPLY_SAVING_PARA:
         return "SV PARA";
      case APPLY_SAVING_BREATH:
         return "SV BRTH";
      case APPLY_SAVING_SPELL:
         return "SV SPLL";
      case APPLY_HEIGHT:
         return "HEIGHT";
      case APPLY_WEIGHT:
         return "WEIGHT";
      case APPLY_AFFECT:
         return "AFF BY";
      case APPLY_RESISTANT:
         return "RESIST";
      case APPLY_IMMUNE:
         return "IMMUNE";
      case APPLY_SUSCEPTIBLE:
         return "SUSCEPT";
      case APPLY_WEAPONSPELL:
         return " WEAPON";
      case APPLY_BACKSTAB:
         return "BACKSTB";
      case APPLY_PICK:
         return " PICK  ";
      case APPLY_TRACK:
         return " TRACK ";
      case APPLY_STEAL:
         return " STEAL ";
      case APPLY_SNEAK:
         return " SNEAK ";
      case APPLY_HIDE:
         return " HIDE  ";
      case APPLY_MOVEHIDE:
         return " MOVE_HIDDEN  ";
      case APPLY_PALM:
         return " PALM  ";
      case APPLY_DETRAP:
         return " DETRAP";
      case APPLY_DODGE:
         return " DODGE ";
      case APPLY_PEEK:
         return " PEEK  ";
      case APPLY_SCAN:
         return " SCAN  ";
      case APPLY_GOUGE:
         return " GOUGE ";
      case APPLY_SEARCH:
         return " SEARCH";
      case APPLY_MOUNT:
         return " MOUNT ";
      case APPLY_DISARM:
         return " DISARM";
      case APPLY_KICK:
         return " KICK  ";
      case APPLY_PARRY:
         return " PARRY ";
      case APPLY_BASH:
         return " BASH  ";
      case APPLY_STUN:
         return " STUN  ";
      case APPLY_PUNCH:
         return " PUNCH ";
      case APPLY_CLIMB:
         return " CLIMB ";
      case APPLY_GRIP:
         return " GRIP  ";
      case APPLY_SCRIBE:
         return " SCRIBE";
      case APPLY_BREW:
         return " BREW  ";
      case APPLY_WEARSPELL:
         return " WEAR  ";
      case APPLY_REMOVESPELL:
         return " REMOVE";
      case APPLY_EMOTION:
         return "EMOTION";
      case APPLY_MENTALSTATE:
         return " MENTAL";
      case APPLY_STRIPSN:
         return " DISPEL";
      case APPLY_REMOVE:
         return " REMOVE";
      case APPLY_DIG:
         return " DIG   ";
      case APPLY_FULL:
         return " HUNGER";
      case APPLY_THIRST:
         return " THIRST";
      case APPLY_DRUNK:
         return " DRUNK ";
      case APPLY_BLOOD:
         return " BLOOD ";
      case APPLY_COOK:
         return " COOK  ";
      case APPLY_RECURRINGSPELL:
         return " RECURR";
      case APPLY_CONTAGIOUS:
         return "CONTGUS";
      case APPLY_ODOR:
         return " ODOR  ";
      case APPLY_ROOMFLAG:
         return " RMFLG ";
      case APPLY_SECTORTYPE:
         return " SECTOR";
      case APPLY_ROOMLIGHT:
         return " LIGHT ";
      case APPLY_TELEVNUM:
         return " TELEVN";
      case APPLY_TELEDELAY:
         return " TELEDY";
   };

   bug( "Affect_location_name: unknown location %d.", location );
   return "Unknown";
}

char *get_class( CHAR_DATA * ch )
{
   if( IS_NPC( ch ) && ch->class < MAX_NPC_CLASS && ch->class >= 0 )
      return ( npc_class[ch->class] );
   else if( !IS_NPC( ch ) && ch->class < MAX_PC_CLASS && ch->class >= 0 )
      return class_table[ch->class]->who_name;
   return ( "Unknown" );
}

char *get_dualclass( CHAR_DATA * ch )
{
   if( IS_NPC( ch ) && ch->dualclass < MAX_NPC_CLASS && ch->dualclass >= 0 )
      return ( npc_class[ch->dualclass] );
   else if( !IS_NPC( ch ) && ch->dualclass < MAX_PC_CLASS && ch->dualclass >= 0 )
      return class_table[ch->dualclass]->who_name;
   return ( "Unknown" );
}


char *get_race( CHAR_DATA * ch )
{
   if( ch->race < MAX_PC_RACE && ch->race >= 0 )
      return ( race_table[ch->race]->race_name );
   if( ch->race < MAX_NPC_RACE && ch->race >= 0 )
      return ( npc_race[ch->race] );
   return ( "Unknown" );
}

void do_level( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   int x, lowlvl, hilvl;

   if( ch->level == 1 )
      lowlvl = 1;
   else
      lowlvl = UMAX( 2, ch->level - 5 );
   hilvl = URANGE( ch->level, ch->level + 5, MAX_LEVEL );
   set_char_color( AT_SCORE, ch );
   ch_printf( ch, "\n\rExperience required, levels %d to %d:\n\r______________________________________________\n\r\n\r",
              lowlvl, hilvl );
   sprintf( buf, " exp  (Current: %12s)", num_punct( ch->exp ) );
   sprintf( buf2, " exp  (Needed:  %12s)", num_punct( exp_level( ch, ch->level + 1 ) - ch->exp ) );
   for( x = lowlvl; x <= hilvl; x++ )
      ch_printf( ch, " (%2d) %12s%s\n\r", x, num_punct( exp_level( ch, x ) ),
                 ( x == ch->level ) ? buf : ( x == ch->level + 1 ) ? buf2 : " exp" );
   send_to_char( "______________________________________________\n\r", ch );
}

void do_remains( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *obj;
   bool found = FALSE;

   if( IS_NPC( ch ) )
      return;
   set_char_color( AT_MAGIC, ch );
   if( !ch->pcdata->deity )
   {
      send_to_pager( "You have no deity from which to seek such assistance...\n\r", ch );
      return;
   }
   if( ch->pcdata->favor < ch->level * 2 )
   {
      send_to_pager( "Your favor is insufficient for such assistance...\n\r", ch );
      return;
   }
   pager_printf( ch, "%s appears in a vision, revealing that your remains... ", ch->pcdata->deity->name );
   sprintf( buf, "the corpse of %s", ch->name );
   for( obj = first_object; obj; obj = obj->next )
   {
      if( obj->in_room && !str_cmp( buf, obj->short_descr ) && ( obj->pIndexData->vnum == 11 ) )
      {
         found = TRUE;
         pager_printf( ch, "\n\r  - at %s will endure for %d ticks", obj->in_room->name, obj->timer );
      }
   }
   if( !found )
      send_to_pager( " no longer exist.\n\r", ch );
   else
   {
      send_to_pager( "\n\r", ch );
      ch->pcdata->favor -= ch->level * 2;
   }
   return;
}

void do_affected( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   AFFECT_DATA *paf;
   SKILLTYPE *skill;

   if( IS_NPC( ch ) )
      return;

   set_char_color( AT_SCORE, ch );

   argument = one_argument( argument, arg );
   if( !str_cmp( arg, "by" ) )
   {
      send_to_char_color( "\n\r&BImbued with:\n\r", ch );
      ch_printf_color( ch, "&C%s\n\r", !xIS_EMPTY( ch->affected_by ) ? affect_bit_name( &ch->affected_by ) : "nothing" );
      if( ch->level >= 20 )
      {
         send_to_char( "\n\r", ch );
         if( ch->resistant > 0 )
         {
            send_to_char_color( "&BResistances:  ", ch );
            ch_printf_color( ch, "&C%s\n\r", flag_string( ch->resistant, ris_flags ) );
         }
         if( ch->immune > 0 )
         {
            send_to_char_color( "&BImmunities:   ", ch );
            ch_printf_color( ch, "&C%s\n\r", flag_string( ch->immune, ris_flags ) );
         }
         if( ch->susceptible > 0 )
         {
            send_to_char_color( "&BSuscepts:     ", ch );
            ch_printf_color( ch, "&C%s\n\r", flag_string( ch->susceptible, ris_flags ) );
         }
      }
      return;
   }

   if( !ch->first_affect )
   {
      send_to_char_color( "\n\r&CNo cantrip or skill affects you.\n\r", ch );
   }
   else
   {
      send_to_char( "\n\r", ch );
      for( paf = ch->first_affect; paf; paf = paf->next )
         if( ( skill = get_skilltype( paf->type ) ) != NULL )
         {
            set_char_color( AT_BLUE, ch );
            send_to_char( "Affected:  ", ch );
            set_char_color( AT_SCORE, ch );
            if( ch->level >= 20 || IS_PKILL( ch ) )
            {
               if( paf->duration < 25 )
                  set_char_color( AT_WHITE, ch );
               if( paf->duration < 6 )
                  set_char_color( AT_RED, ch );
               ch_printf( ch, "(%5d)   ", paf->duration );
            }
            ch_printf( ch, "%-18s\n\r", skill->name );
         }
   }
   return;
}

void do_inventory( CHAR_DATA * ch, char *argument )
{
   set_char_color( AT_RED, ch );
   ch_printf( ch, "You are carrying %d items and can carry %d more:\n\r"
              "You are burdened with %d pounds and can carry %d more:\n\r",
              ch->carry_number, can_carry_n( ch ) - ch->carry_number,
              ch->carry_weight, can_carry_w( ch ) - ch->carry_weight );
   show_list_to_char_inv( ch->first_carrying, ch, TRUE, TRUE, 0 );
   return;
}

void do_keyitems( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *obj;
   bool found;

   set_char_color( AT_RED, ch );
   ch_printf( ch, "You currently have the following key items.\n\r" );
   found = FALSE;
   set_char_color( AT_OBJECT, ch );
   for( obj = ch->first_carrying; obj; obj = obj->next_content )
   {
      if( obj->item_type == ITEM_KEYITEM )
      {
         send_to_char( "     ", ch );
         send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
         send_to_char( "\n\r", ch );
         found = TRUE;
      }
   }
   if( !found )
   {
      send_to_char( "     &zNothing&D\n\r", ch );
   }
   return;
}

void do_equipment( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *obj;
   int iWear;
   bool found;
   bool empty;
   int cond;
   char buf[MSL];
   int counter;

   set_char_color( AT_RED, ch );
   send_to_char( "&gType 'garb' to see what wear locations there are.\n\r&RYou are using:\n\r", ch );
   found = FALSE;
   set_char_color( AT_OBJECT, ch );
   for( iWear = 0; iWear < MAX_WEAR; iWear++ )
   {
      counter = 0;
      empty = TRUE;
      for( obj = ch->first_carrying; obj; obj = obj->next_content )
      {
         if( obj->wear_loc == iWear )
         {
            set_char_color( AT_OBJECT, ch );
            send_to_char( where_name[iWear], ch );
            if( can_see_obj( ch, obj ) )
            {
               if( xIS_SET( ch->act, PLR_COND ) )
               {
                  switch ( obj->item_type )
                  {
                     case ITEM_ARMOR:
                        if( obj->value[1] > 0 )
                        {
                           cond = ( int )( ( 100 * obj->value[0] / obj->value[1] ) );
                        }
                        else
                        {
                           cond = 100;
                        }
                        break;
                     case ITEM_WEAPON:
                        cond = ( int )( ( 100 * obj->value[0] / 12 ) );
                        break;
                     case ITEM_QUIVER:
                        cond = ( int )( ( 100 * obj->value[3] / 12 ) );
                        break;
                     case ITEM_MISSILE_WEAPON:
                        cond = ( int )( ( 100 * obj->value[0] / 12 ) );
                        break;
                     case ITEM_CONTAINER:
                        cond = ( int )( ( 100 * obj->value[3] / 12 ) );
                        break;
                     case ITEM_LIGHT:
                        cond = ( int )( ( 100 * obj->value[0] / 12 ) );
                        break;
                     case ITEM_GHOST:
                        cond = ( int )( ( 100 * obj->value[0] / 12 ) );
                        break;
                     default:
                        cond = -1;
                        break;
                  }
                  if( cond > -1 )
                  {
                     send_to_char( "&C<&R", ch );
                     sprintf( buf, "%3d%%", cond );
                     send_to_char( buf, ch );
                     send_to_char( "&C>&g ", ch );
                  }
                  else
                  {
                     send_to_char( "&C<&RNone&C>&g ", ch );
                  }
               }
               send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
               send_to_char( "\n\r", ch );
            }
            else
            {
               send_to_char( " &c<&zSomething&c>&D\n\r", ch );
            }
            counter++;
            found = TRUE;
            empty = FALSE;
         }
      }
      if( sysdata.dsq == TRUE && empty == TRUE && counter == 0 )
      {
         set_char_color( AT_OBJECT, ch );
         send_to_char( where_name[iWear], ch );
         send_to_char( " &c<&zNothing&c>&D\n\r", ch );
         counter++;
      }
   }
   if( sysdata.dsq == FALSE && !found )
   {
      send_to_char( "Nothing.\n\r", ch );
   }
   return;
}

void do_garb( CHAR_DATA * ch, char *argument )
{
   OBJ_DATA *obj;
   int iWear;
   bool found;
   bool empty;
   int cond;
   char buf[MSL];
   int counter;

   set_char_color( AT_RED, ch );
   send_to_char( "You are using:\n\r", ch );
   found = FALSE;
   set_char_color( AT_OBJECT, ch );
   for( iWear = 0; iWear < MAX_WEAR - 3; iWear++ )
   {
      counter = 0;
      empty = TRUE;
      for( obj = ch->first_carrying; obj; obj = obj->next_content )
      {
         if( obj->wear_loc == iWear )
         {
            set_char_color( AT_OBJECT, ch );
            send_to_char( where_name[iWear], ch );
            if( can_see_obj( ch, obj ) )
            {
               if( xIS_SET( ch->act, PLR_COND ) )
               {
                  switch ( obj->item_type )
                  {
                     case ITEM_ARMOR:
                        if( obj->value[1] > 0 )
                        {
                           cond = ( int )( ( 100 * obj->value[0] / obj->value[1] ) );
                        }
                        else
                        {
                           cond = 100;
                        }
                        break;
                     case ITEM_WEAPON:
                        cond = ( int )( ( 100 * obj->value[0] / 12 ) );
                        break;
                     case ITEM_QUIVER:
                        cond = ( int )( ( 100 * obj->value[3] / 12 ) );
                        break;
                     case ITEM_MISSILE_WEAPON:
                        cond = ( int )( ( 100 * obj->value[0] / 12 ) );
                        break;
                     case ITEM_CONTAINER:
                        cond = ( int )( ( 100 * obj->value[3] / 12 ) );
                        break;
                     case ITEM_LIGHT:
                        cond = ( int )( ( 100 * obj->value[0] / 12 ) );
                        break;
                     case ITEM_GHOST:
                        cond = ( int )( ( 100 * obj->value[0] / 12 ) );
                        break;
                     default:
                        cond = -1;
                        break;
                  }
                  if( cond > -1 )
                  {
                     send_to_char( "&C<&R", ch );
                     sprintf( buf, "%3d%%", cond );
                     send_to_char( buf, ch );
                     send_to_char( "&C>&g ", ch );
                  }
                  else
                  {
                     send_to_char( "&C<&RNone&C>&g ", ch );
                  }
               }
               send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
               send_to_char( "\n\r", ch );
            }
            else
            {
               send_to_char( " &c<&zSomething&c>&D\n\r", ch );
            }
            counter++;
            found = TRUE;
            empty = FALSE;
         }
      }
      if( empty == TRUE && counter == 0 )
      {
         set_char_color( AT_OBJECT, ch );
         send_to_char( where_name[iWear], ch );
         send_to_char( " &c<&zNothing&c>&D\n\r", ch );
         counter++;
      }
   }
   return;
}


void set_title( CHAR_DATA * ch, char *title )
{
   char buf[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) )
   {
      bug( "Set_title: NPC.", 0 );
      return;
   }

   if( isalpha( title[0] ) || isdigit( title[0] ) )
   {
      buf[0] = ' ';
      strcpy( buf + 1, title );
   }
   else
      strcpy( buf, title );

   STRFREE( ch->pcdata->title );
   ch->pcdata->title = STRALLOC( buf );
   return;
}

void set_movein( CHAR_DATA * ch, char *title )
{
   char buf[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) )
   {
      bug( "Set_movein: NPC.", 0 );
      return;
   }

   sprintf( buf, "%s %s", IS_SNAME( ch ) ? ch->pcdata->sname : ch->name, title );

   STRFREE( ch->pcdata->movein );
   ch->pcdata->movein = STRALLOC( buf );
   return;
}

void set_moveout( CHAR_DATA * ch, char *title )
{
   char buf[MAX_STRING_LENGTH];

   if( IS_NPC( ch ) )
   {
      bug( "Set_moveout: NPC.", 0 );
      return;
   }

   sprintf( buf, "%s %s", IS_SNAME( ch ) ? ch->pcdata->sname : ch->name, title );

   STRFREE( ch->pcdata->moveout );
   ch->pcdata->moveout = STRALLOC( buf );
   return;
}



void do_title( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) )
      return;

   set_char_color( AT_SCORE, ch );
   if( ch->level < 2 )
   {
      send_to_char( "Sorry... you must be at least level 2 to set your title...\n\r", ch );
      return;
   }
   if( IS_SET( ch->pcdata->flags, PCFLAG_NOTITLE ) )
   {
      set_char_color( AT_IMMORT, ch );
      send_to_char( "The Gods prohibit you from changing your title.\n\r", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      send_to_char( "Change your title to what?\n\r", ch );
      return;
   }

   if( strlen( argument ) > 80 )
      argument[80] = '\0';

   if( xIS_SET( ch->act, PLR_A_TITLE ) )
   {
      xREMOVE_BIT( ch->act, PLR_A_TITLE );
   }

   smash_tilde( argument );
   set_title( ch, argument );
   send_to_char( "Ok.\n\r", ch );
}

void do_description( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) )
   {
      send_to_char( "Monsters are too dumb to do that!\n\r", ch );
      return;
   }

   if( !ch->desc )
   {
      bug( "do_description: no descriptor", 0 );
      return;
   }

   switch ( ch->substate )
   {
      default:
         bug( "do_description: illegal substate", 0 );
         return;

      case SUB_RESTRICTED:
         send_to_char( "You cannot use this command from within another command.\n\r", ch );
         return;

      case SUB_NONE:
         ch->substate = SUB_PERSONAL_DESC;
         ch->dest_buf = ch;
         start_editing( ch, ch->description );
         return;

      case SUB_PERSONAL_DESC:
         STRFREE( ch->description );
         ch->description = copy_buffer( ch );
         stop_editing( ch );
         return;
   }
}

void do_bio( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) )
   {
      send_to_char( "Mobs cannot set a bio.\n\r", ch );
      return;
   }
   if( ch->level < 5 )
   {
      set_char_color( AT_SCORE, ch );
      send_to_char( "You must be at least level five to write your bio...\n\r", ch );
      return;
   }
   if( !ch->desc )
   {
      bug( "do_bio: no descriptor", 0 );
      return;
   }

   switch ( ch->substate )
   {
      default:
         bug( "do_bio: illegal substate", 0 );
         return;

      case SUB_RESTRICTED:
         send_to_char( "You cannot use this command from within another command.\n\r", ch );
         return;

      case SUB_NONE:
         ch->substate = SUB_PERSONAL_BIO;
         ch->dest_buf = ch;
         start_editing( ch, ch->pcdata->bio );
         return;

      case SUB_PERSONAL_BIO:
         STRFREE( ch->pcdata->bio );
         ch->pcdata->bio = copy_buffer( ch );
         stop_editing( ch );
         return;
   }
}

void do_statreport( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_INPUT_LENGTH];

   if( IS_NPC( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   if( ch->level < 100 )
   {
      if( IS_VAMPIRE( ch ) )
      {
         ch_printf( ch, "You report: %d/%d hp %d/%d blood %d/%d mv %d tnl.\n\r",
                    ch->hit, ch->max_hit, ch->blood, ch->max_blood, ch->move, ch->max_move,
                    exp_level( ch, ch->level + 1 ) - ch->exp );
         sprintf( buf, "$n reports: %d/%d hp %d/%d blood %d/%d mv %d tnl.",
                  ch->hit, ch->max_hit, ch->blood, ch->max_blood, ch->move, ch->max_move,
                  exp_level( ch, ch->level + 1 ) - ch->exp );
         act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );
      }
      else
      {
         ch_printf( ch, "You report: %d/%d hp %d/%d mana %d/%d mv %d tnl.\n\r",
                    ch->hit, ch->max_hit, ch->mana, ch->max_mana,
                    ch->move, ch->max_move, exp_level( ch, ch->level + 1 ) - ch->exp );
         sprintf( buf, "$n reports: %d/%d hp %d/%d mana %d/%d mv %d tnl.",
                  ch->hit, ch->max_hit, ch->mana, ch->max_mana,
                  ch->move, ch->max_move, exp_level( ch, ch->level + 1 ) - ch->exp );
         act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );
      }
   }
   else
   {
      if( IS_VAMPIRE( ch ) )
      {
         ch_printf( ch, "You report: %d/%d hp %d/%d blood %d/%d mv %d tnl.\n\r",
                    ch->hit, ch->max_hit, ch->blood, ch->max_blood, ch->move, ch->max_move,
                    IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch, ch->sublevel + 1 ) - ch->exp );
         sprintf( buf, "$n reports: %d/%d hp %d/%d blood %d/%d mv %d tnl.",
                  ch->hit, ch->max_hit, ch->blood, ch->max_blood, ch->move, ch->max_move,
                  IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch, ch->sublevel + 1 ) - ch->exp );
         act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );
      }
      else
      {
         ch_printf( ch, "You report: %d/%d hp %d/%d mana %d/%d mv %d tnl.\n\r",
                    ch->hit, ch->max_hit, ch->mana, ch->max_mana,
                    ch->move, ch->max_move, IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch,
                                                                                           ch->sublevel + 1 ) - ch->exp );
         sprintf( buf, "$n reports: %d/%d hp %d/%d mana %d/%d mv %d tnl.", ch->hit, ch->max_hit, ch->mana, ch->max_mana,
                  ch->move, ch->max_move, IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch, ch->sublevel + 1 ) - ch->exp );
         act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );
      }
   }

   ch_printf( ch, "Your base stats:    %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.\n\r",
              ch->perm_str, ch->perm_wis, ch->perm_int, ch->perm_dex, ch->perm_con, ch->perm_cha, ch->perm_lck );
   sprintf( buf, "$n's base stats:    %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.",
            ch->perm_str, ch->perm_wis, ch->perm_int, ch->perm_dex, ch->perm_con, ch->perm_cha, ch->perm_lck );
   act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );

   ch_printf( ch, "Your current stats: %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.\n\r",
              get_curr_str( ch ), get_curr_wis( ch ), get_curr_int( ch ),
              get_curr_dex( ch ), get_curr_con( ch ), get_curr_cha( ch ), get_curr_lck( ch ) );
   sprintf( buf, "$n's current stats: %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.",
            get_curr_str( ch ), get_curr_wis( ch ), get_curr_int( ch ),
            get_curr_dex( ch ), get_curr_con( ch ), get_curr_cha( ch ), get_curr_lck( ch ) );
   act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );
   return;
}

void do_stat( CHAR_DATA * ch, char *argument )
{
   if( IS_NPC( ch ) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   if( ch->level < 100 )
   {
      if( IS_VAMPIRE( ch ) )
      {
         ch_printf( ch, "You report: %d/%d hp %d/%d blood %d/%d mv %d tnl.\n\r",
                    ch->hit, ch->max_hit, ch->blood, ch->max_blood, ch->move, ch->max_move,
                    exp_level( ch, ch->level + 1 ) - ch->exp );
      }
      else
      {
         ch_printf( ch, "You report: %d/%d hp %d/%d mana %d/%d mv %d tnl.\n\r",
                    ch->hit, ch->max_hit, ch->mana, ch->max_mana,
                    ch->move, ch->max_move, exp_level( ch, ch->level + 1 ) - ch->exp );
      }
   }
   else
   {
      if( IS_VAMPIRE( ch ) )
      {
         ch_printf( ch, "You report: %d/%d hp %d/%d blood %d/%d mv %d tnl.\n\r",
                    ch->hit, ch->max_hit, ch->blood, ch->max_blood, ch->move, ch->max_move,
                    IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch, ch->sublevel + 1 ) - ch->exp );
      }
      else
      {
         ch_printf( ch, "You report: %d/%d hp %d/%d mana %d/%d mv %d tnl.\n\r",
                    ch->hit, ch->max_hit, ch->mana, ch->max_mana,
                    ch->move, ch->max_move, IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch,
                                                                                           ch->sublevel + 1 ) - ch->exp );
      }
   }

   ch_printf( ch, "Your base stats:    %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.\n\r",
              ch->perm_str, ch->perm_wis, ch->perm_int, ch->perm_dex, ch->perm_con, ch->perm_cha, ch->perm_lck );

   ch_printf( ch, "Your current stats: %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.\n\r",
              get_curr_str( ch ), get_curr_wis( ch ), get_curr_int( ch ),
              get_curr_dex( ch ), get_curr_con( ch ), get_curr_cha( ch ), get_curr_lck( ch ) );
   return;
}


void do_report( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_INPUT_LENGTH];

   if( IS_NPC( ch ) && ch->fighting )
      return;

   if( IS_AFFECTED( ch, AFF_POSSESS ) )
   {
      send_to_char( "You can't do that in your current state of mind!\n\r", ch );
      return;
   }

   if( ch->level < 100 )
   {
      if( IS_VAMPIRE( ch ) )
      {
         ch_printf( ch, "You report: %d/%d hp %d/%d blood %d/%d mv %d tnl.\n\r",
                    ch->hit, ch->max_hit, ch->blood, ch->max_blood, ch->move, ch->max_move,
                    exp_level( ch, ch->level + 1 ) - ch->exp );
         sprintf( buf, "$n reports: %d/%d hp %d/%d blood %d/%d mv %d tnl.",
                  ch->hit, ch->max_hit, ch->blood, ch->max_blood, ch->move, ch->max_move,
                  exp_level( ch, ch->level + 1 ) - ch->exp );
         act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );
      }
      else
      {
         ch_printf( ch, "You report: %d/%d hp %d/%d mana %d/%d mv %d tnl.\n\r",
                    ch->hit, ch->max_hit, ch->mana, ch->max_mana,
                    ch->move, ch->max_move, exp_level( ch, ch->level + 1 ) - ch->exp );
         sprintf( buf, "$n reports: %d/%d hp %d/%d mana %d/%d mv %d tnl.",
                  ch->hit, ch->max_hit, ch->mana, ch->max_mana,
                  ch->move, ch->max_move, exp_level( ch, ch->level + 1 ) - ch->exp );
         act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );
      }
   }
   else
   {
      if( IS_VAMPIRE( ch ) )
      {
         ch_printf( ch, "You report: %d/%d hp %d/%d blood %d/%d mv %d tnl.\n\r",
                    ch->hit, ch->max_hit, ch->blood, ch->max_blood, ch->move, ch->max_move,
                    IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch, ch->sublevel + 1 ) - ch->exp );
         sprintf( buf, "$n reports: %d/%d hp %d/%d blood %d/%d mv %d tnl.",
                  ch->hit, ch->max_hit, ch->blood, ch->max_blood, ch->move, ch->max_move,
                  IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch, ch->sublevel + 1 ) - ch->exp );
         act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );
      }
      else
      {
         ch_printf( ch, "You report: %d/%d hp %d/%d mana %d/%d mv %d tnl.\n\r",
                    ch->hit, ch->max_hit, ch->mana, ch->max_mana,
                    ch->move, ch->max_move, IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch,
                                                                                           ch->sublevel + 1 ) - ch->exp );
         sprintf( buf, "$n reports: %d/%d hp %d/%d mana %d/%d mv %d tnl.", ch->hit, ch->max_hit, ch->mana, ch->max_mana,
                  ch->move, ch->max_move, IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch, ch->sublevel + 1 ) - ch->exp );
         act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );
      }
   }
   return;
}


void do_prompt( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];

   set_char_color( AT_GREY, ch );

   if( IS_NPC( ch ) )
   {
      send_to_char( "NPC's can't change their prompt..\n\r", ch );
      return;
   }
   smash_tilde( argument );
   one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      if( xIS_SET( ch->act, PLR_PROMPT ) )
      {
         xREMOVE_BIT( ch->act, PLR_PROMPT );
         send_to_char( "Your prompt is now off.\n\r", ch );
      }
      else
      {
         xSET_BIT( ch->act, PLR_PROMPT );
         send_to_char( "Your prompt is now on.\n\r", ch );
      }
   }
   else if( !str_cmp( arg, "display" ) )
   {
      send_to_char( "Your current prompt string:\n\r", ch );
      set_char_color( AT_WHITE, ch );
      ch_printf( ch, "%s\n\r", !str_cmp( ch->pcdata->prompt, "" ) ? "(default prompt)" : ch->pcdata->prompt );
      set_char_color( AT_GREY, ch );
      send_to_char( "Type 'help prompt' for information on changing your prompt.\n\r", ch );
      return;
   }
   else
   {
      send_to_char( "Replacing old prompt of:\n\r", ch );
      set_char_color( AT_WHITE, ch );
      ch_printf( ch, "%s\n\r", !str_cmp( ch->pcdata->prompt, "" ) ? "(default prompt)" : ch->pcdata->prompt );
      if( ch->pcdata->prompt )
         STRFREE( ch->pcdata->prompt );
      if( strlen( argument ) > 128 )
         argument[128] = '\0';

      if( !str_cmp( arg, "default" ) )
         ch->pcdata->prompt = STRALLOC( "" );
      else
         ch->pcdata->prompt = STRALLOC( argument );
      return;
   }
}

void tax_player( CHAR_DATA * ch )
{
}
