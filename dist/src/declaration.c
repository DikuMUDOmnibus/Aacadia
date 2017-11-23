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
/*						Tables II module        */
/************************************************************************/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include "acadia.h"

SKILLTYPE *skill_table[MAX_SKILL];
struct class_type *class_table[MAX_CLASS];
RACE_TYPE *race_table[MAX_RACE];
char *title_table[MAX_CLASS][MAX_LEVEL + 1][2];
SKILLTYPE *herb_table[MAX_HERB];
SKILLTYPE *disease_table[MAX_DISEASE];

LANG_DATA *first_lang;
LANG_DATA *last_lang;


SPELL_FUN *spell_function( char *name )
{
   if( !str_cmp( name, "spell_acadia" ) )
      return spell_acadia;
   if( !str_cmp( name, "spell_acid_blast" ) )
      return spell_acid_blast;
   if( !str_cmp( name, "spell_animate_dead" ) )
      return spell_animate_dead;
   if( !str_cmp( name, "spell_astral_walk" ) )
      return spell_astral_walk;
   if( !str_cmp( name, "spell_barkskin" ) )
      return spell_barkskin;
   if( !str_cmp( name, "spell_blindness" ) )
      return spell_blindness;
   if( !str_cmp( name, "spell_burning_hands" ) )
      return spell_burning_hands;
   if( !str_cmp( name, "spell_callmount" ) )
      return spell_callmount;
   if( !str_cmp( name, "spell_call_lightning" ) )
      return spell_call_lightning;
   if( !str_cmp( name, "spell_chainlightning" ) )
      return spell_chainlightning;
   if( !str_cmp( name, "spell_icechain" ) )
      return spell_icechain;
   if( !str_cmp( name, "spell_niaver" ) )
      return spell_niaver;
   if( !str_cmp( name, "spell_firechain" ) )
      return spell_firechain;
   if( !str_cmp( name, "spell_cause_critical" ) )
      return spell_cause_critical;
   if( !str_cmp( name, "spell_cause_light" ) )
      return spell_cause_light;
   if( !str_cmp( name, "spell_cause_serious" ) )
      return spell_cause_serious;
   if( !str_cmp( name, "spell_change_sex" ) )
      return spell_change_sex;
   if( !str_cmp( name, "spell_charm_person" ) )
      return spell_charm_person;
   if( !str_cmp( name, "spell_chill_touch" ) )
      return spell_chill_touch;
   if( !str_cmp( name, "spell_colour_spray" ) )
      return spell_colour_spray;
   if( !str_cmp( name, "spell_continual_light" ) )
      return spell_continual_light;
   if( !str_cmp( name, "spell_control_weather" ) )
      return spell_control_weather;
   if( !str_cmp( name, "spell_create_food" ) )
      return spell_create_food;
   if( !str_cmp( name, "spell_create_water" ) )
      return spell_create_water;
   if( !str_cmp( name, "spell_creeping_doom" ) )
      return spell_creeping_doom;
   if( !str_cmp( name, "spell_cure_blindness" ) )
      return spell_cure_blindness;
   if( !str_cmp( name, "spell_cure_poison" ) )
      return spell_cure_poison;
   if( !str_cmp( name, "spell_curse" ) )
      return spell_curse;
   if( !str_cmp( name, "spell_dangle" ) )
      return spell_dangle;
   if( !str_cmp( name, "spell_delirium" ) )
      return spell_delirium;
   if( !str_cmp( name, "spell_detect_poison" ) )
      return spell_detect_poison;
   if( !str_cmp( name, "spell_dispel_evil" ) )
      return spell_dispel_evil;
   if( !str_cmp( name, "spell_dispel_magic" ) )
      return spell_dispel_magic;
   if( !str_cmp( name, "spell_dream" ) )
      return spell_dream;
   if( !str_cmp( name, "spell_earthquake" ) )
      return spell_earthquake;
   if( !str_cmp( name, "spell_enchant_weapon" ) )
      return spell_enchant_weapon;
   if( !str_cmp( name, "spell_enchant_armor" ) )
      return spell_enchant_armor;
   if( !str_cmp( name, "spell_energy_drain" ) )
      return spell_energy_drain;
   if( !str_cmp( name, "spell_faerie_fire" ) )
      return spell_faerie_fire;
   if( !str_cmp( name, "spell_faerie_fog" ) )
      return spell_faerie_fog;
   if( !str_cmp( name, "spell_farsight" ) )
      return spell_farsight;
   if( !str_cmp( name, "spell_fireball" ) )
      return spell_fireball;
   if( !str_cmp( name, "spell_flamestrike" ) )
      return spell_flamestrike;
   if( !str_cmp( name, "spell_demistrike" ) )
      return spell_demistrike;
   if( !str_cmp( name, "spell_plasmastrike" ) )
      return spell_plasmastrike;
   if( !str_cmp( name, "spell_gate" ) )
      return spell_gate;
   if( !str_cmp( name, "spell_correlate" ) )
      return spell_correlate;
   if( !str_cmp( name, "spell_gpc" ) )
      return spell_gpc;
   if( !str_cmp( name, "spell_knock" ) )
      return spell_knock;
   if( !str_cmp( name, "spell_harm" ) )
      return spell_harm;
   if( !str_cmp( name, "spell_headache" ) )
      return spell_headache;
   if( !str_cmp( name, "spell_holyaura" ) )
      return spell_holyaura;
   if( !str_cmp( name, "spell_holysight" ) )
      return spell_holysight;
   if( !str_cmp( name, "spell_identify" ) )
      return spell_identify;
   if( !str_cmp( name, "spell_invincibility" ) )
      return spell_invincibility;
   if( !str_cmp( name, "spell_invis" ) )
      return spell_invis;
   if( !str_cmp( name, "spell_impinvis" ) )
      return spell_impinvis;
   if( !str_cmp( name, "spell_ironhand" ) )
      return spell_ironhand;
   if( !str_cmp( name, "spell_ironskin" ) )
      return spell_ironskin;
   if( !str_cmp( name, "spell_jedi_mind_trick" ) )
      return spell_jedi_mind_trick;
   if( !str_cmp( name, "spell_jir" ) )
      return spell_jir;
   if( !str_cmp( name, "spell_jur" ) )
      return spell_jur;
   if( !str_cmp( name, "spell_know_alignment" ) )
      return spell_know_alignment;
   if( !str_cmp( name, "spell_lightning_bolt" ) )
      return spell_lightning_bolt;
   if( !str_cmp( name, "spell_locate_object" ) )
      return spell_locate_object;
   if( !str_cmp( name, "spell_magic_missile" ) )
      return spell_magic_missile;
   if( !str_cmp( name, "spell_mindblast" ) )
      return spell_mindblast;
   if( !str_cmp( name, "spell_pass_door" ) )
      return spell_pass_door;
   if( !str_cmp( name, "spell_poison" ) )
      return spell_poison;
   if( !str_cmp( name, "spell_polymorph" ) )
      return spell_polymorph;
   if( !str_cmp( name, "spell_possess" ) )
      return spell_possess;
   if( !str_cmp( name, "spell_psidance" ) )
      return spell_psidance;
   if( !str_cmp( name, "spell_psiblast" ) )
      return spell_psiblast;
   if( !str_cmp( name, "spell_psibolt" ) )
      return spell_psibolt;
   if( !str_cmp( name, "spell_psiwave" ) )
      return spell_psiwave;
   if( !str_cmp( name, "spell_recharge" ) )
      return spell_recharge;
   if( !str_cmp( name, "spell_remove_curse" ) )
      return spell_remove_curse;
   if( !str_cmp( name, "spell_remove_invis" ) )
      return spell_remove_invis;
   if( !str_cmp( name, "spell_remove_trap" ) )
      return spell_remove_trap;
   if( !str_cmp( name, "spell_shocking_grasp" ) )
      return spell_shocking_grasp;
   if( !str_cmp( name, "spell_sleep" ) )
      return spell_sleep;
   if( !str_cmp( name, "spell_summon" ) )
      return spell_summon;
   if( !str_cmp( name, "spell_teleport" ) )
      return spell_teleport;
   if( !str_cmp( name, "spell_ventriloquate" ) )
      return spell_ventriloquate;
   if( !str_cmp( name, "spell_weaken" ) )
      return spell_weaken;
   if( !str_cmp( name, "spell_word_of_recall" ) )
      return spell_word_of_recall;
   if( !str_cmp( name, "spell_acid_breath" ) )
      return spell_acid_breath;
   if( !str_cmp( name, "spell_fire_breath" ) )
      return spell_fire_breath;
   if( !str_cmp( name, "spell_frost_breath" ) )
      return spell_frost_breath;
   if( !str_cmp( name, "spell_gas_breath" ) )
      return spell_gas_breath;
   if( !str_cmp( name, "spell_hurricane" ) )
      return spell_hurricane;
   if( !str_cmp( name, "spell_sandstorm" ) )
      return spell_sandstorm;
   if( !str_cmp( name, "spell_firestorm" ) )
      return spell_firestorm;
   if( !str_cmp( name, "spell_avalanche" ) )
      return spell_avalanche;
   if( !str_cmp( name, "spell_meteor" ) )
      return spell_meteor;
   if( !str_cmp( name, "spell_lightning_breath" ) )
      return spell_lightning_breath;
   if( !str_cmp( name, "spell_steel_skin" ) )
      return spell_steel_skin;
   if( !str_cmp( name, "spell_spiral_blast" ) )
      return spell_spiral_blast;
   if( !str_cmp( name, "spell_scorching_surge" ) )
      return spell_scorching_surge;
   if( !str_cmp( name, "spell_transport" ) )
      return spell_transport;
   if( !str_cmp( name, "spell_portal" ) )
      return spell_portal;
   if( !str_cmp( name, "spell_regeneration" ) )
      return spell_regeneration;
   if( !str_cmp( name, "spell_demonfire" ) )
      return spell_demonfire;
   if( !str_cmp( name, "spell_demi" ) )
      return spell_demi;
   if( !str_cmp( name, "spell_kyahu" ) )
      return spell_kyahu;
   if( !str_cmp( name, "spell_ethereal_fist" ) )
      return spell_ethereal_fist;
   if( !str_cmp( name, "spell_spectral_furor" ) )
      return spell_spectral_furor;
   if( !str_cmp( name, "spell_hand_of_chaos" ) )
      return spell_hand_of_chaos;
   if( !str_cmp( name, "spell_disruption" ) )
      return spell_disruption;
   if( !str_cmp( name, "spell_sonic_resonance" ) )
      return spell_sonic_resonance;
   if( !str_cmp( name, "spell_mind_wrack" ) )
      return spell_mind_wrack;
   if( !str_cmp( name, "spell_mind_wrench" ) )
      return spell_mind_wrench;
   if( !str_cmp( name, "spell_revive" ) )
      return spell_revive;
   if( !str_cmp( name, "spell_sulfurous_spray" ) )
      return spell_sulfurous_spray;
   if( !str_cmp( name, "spell_caustic_fount" ) )
      return spell_caustic_fount;
   if( !str_cmp( name, "spell_acetum_primus" ) )
      return spell_acetum_primus;
   if( !str_cmp( name, "spell_galvanic_whip" ) )
      return spell_galvanic_whip;
   if( !str_cmp( name, "spell_magnetic_thrust" ) )
      return spell_magnetic_thrust;
   if( !str_cmp( name, "spell_quantum_spike" ) )
      return spell_quantum_spike;
   if( !str_cmp( name, "spell_blackflame" ) )
      return spell_blackflame;
   if( !str_cmp( name, "spell_black_lightning" ) )
      return spell_black_lightning;
   if( !str_cmp( name, "spell_force_choke" ) )
      return spell_force_choke;
   if( !str_cmp( name, "spell_force_heal" ) )
      return spell_force_heal;
   if( !str_cmp( name, "spell_force_lightning" ) )
      return spell_force_lightning;
   if( !str_cmp( name, "spell_force_levitation" ) )
      return spell_force_levitation;
   if( !str_cmp( name, "spell_midas_touch" ) )
      return spell_midas_touch;
   if( !str_cmp( name, "spell_sith_fury" ) )
      return spell_sith_fury;
   if( !str_cmp( name, "spell_expurgation" ) )
      return spell_expurgation;
   if( !str_cmp( name, "spell_sacral_divinity" ) )
      return spell_sacral_divinity;
   if( !str_cmp( name, "spell_seluthiary" ) )
      return spell_seluthiary;
   if( !str_cmp( name, "spell_purity" ) )
      return spell_purity;
   if( !str_cmp( name, "spell_warp" ) )
      return spell_warp;
   if( !str_cmp( name, "spell_godspell" ) )
      return spell_godspell;
   if( !str_cmp( name, "spell_calltohades" ) )
      return spell_calltohades;
   if( !str_cmp( name, "spell_jer" ) )
      return spell_jer;
   if( !str_cmp( name, "spell_frenzy" ) )
      return spell_frenzy;
   if( !str_cmp( name, "spell_quickness" ) )
      return spell_quickness;
   if( !str_cmp( name, "spell_battleplan" ) )
      return spell_battleplan;
   if( !str_cmp( name, "spell_biofeedback" ) )
      return spell_biofeedback;
   if( !str_cmp( name, "spell_displacement" ) )
      return spell_displacement;
   if( !str_cmp( name, "spell_calcify_flesh" ) )
      return spell_calcify_flesh;
   if( !str_cmp( name, "spell_hadesarmor" ) )
      return spell_hadesarmor;
   if( !str_cmp( name, "spell_armor" ) )
      return spell_armor;
   if( !str_cmp( name, "spell_bless" ) )
      return spell_bless;
   if( !str_cmp( name, "spell_protect" ) )
      return spell_protect;
   if( !str_cmp( name, "spell_plague" ) )
      return spell_plague;
   if( !str_cmp( name, "spell_cure_plague" ) )
      return spell_cure_plague;
   if( !str_cmp( name, "spell_aquabreath" ) )
      return spell_aquabreath;
   if( !str_cmp( name, "spell_stoneskin" ) )
      return spell_stoneskin;
   if( !str_cmp( name, "spell_sanctuary" ) )
      return spell_sanctuary;
   if( !str_cmp( name, "spell_fireshield" ) )
      return spell_fireshield;
   if( !str_cmp( name, "spell_iceshield" ) )
      return spell_iceshield;
   if( !str_cmp( name, "spell_shockshield" ) )
      return spell_shockshield;
   if( !str_cmp( name, "spell_shield" ) )
      return spell_shield;
   if( !str_cmp( name, "spell_ablution" ) )
      return spell_ablution;
   if( !str_cmp( name, "spell_fly" ) )
      return spell_fly;
   if( !str_cmp( name, "spell_holy" ) )
      return spell_holy;
   if( !str_cmp( name, "spell_mistwalk" ) )
      return spell_mistwalk;
   if( !str_cmp( name, "spell_home" ) )
      return spell_home;
   if( !str_cmp( name, "spell_terrashift" ) )
      return spell_terrashift;

   if( !str_cmp( name, "spell_detect_evil" ) )
      return spell_detect_evil;
   if( !str_cmp( name, "spell_detect_invis" ) )
      return spell_detect_invis;
   if( !str_cmp( name, "spell_detect_magic" ) )
      return spell_detect_magic;
   if( !str_cmp( name, "spell_detect_hidden" ) )
      return spell_detect_hidden;
   if( !str_cmp( name, "spell_infravision" ) )
      return spell_infravision;
   if( !str_cmp( name, "spell_true_sight" ) )
      return spell_true_sight;
   if( !str_cmp( name, "spell_detect_traps" ) )
      return spell_detect_traps;
   if( !str_cmp( name, "spell_scry" ) )
      return spell_scry;

   if( !str_cmp( name, "spell_cure_light" ) )
      return spell_cure_light;
   if( !str_cmp( name, "spell_cure_serious" ) )
      return spell_cure_serious;
   if( !str_cmp( name, "spell_cure_critical" ) )
      return spell_cure_critical;

   if( !str_cmp( name, "spell_mana" ) )
      return spell_mana;

   if( !str_cmp( name, "spell_comfort" ) )
      return spell_comfort;

   if( !str_cmp( name, "spell_heal_i" ) )
      return spell_heal_i;
   if( !str_cmp( name, "spell_heal_ii" ) )
      return spell_heal_ii;
   if( !str_cmp( name, "spell_heal_iii" ) )
      return spell_heal_iii;
   if( !str_cmp( name, "spell_heal_iv" ) )
      return spell_heal_iv;
   if( !str_cmp( name, "spell_heal_v" ) )
      return spell_heal_v;
   // Songs
   if( !str_cmp( name, "spell_kn_minne" ) )
      return spell_kn_minne;
   if( !str_cmp( name, "spell_kn_minne2" ) )
      return spell_kn_minne2;
   if( !str_cmp( name, "spell_kn_minne3" ) )
      return spell_kn_minne3;
   if( !str_cmp( name, "spell_kn_minne4" ) )
      return spell_kn_minne4;
   if( !str_cmp( name, "spell_val_minuet" ) )
      return spell_val_minuet;
   if( !str_cmp( name, "spell_val_minuet2" ) )
      return spell_val_minuet2;
   if( !str_cmp( name, "spell_ice_threnody" ) )
      return spell_ice_threnody;
   if( !str_cmp( name, "spell_fire_threnody" ) )
      return spell_fire_threnody;
   if( !str_cmp( name, "spell_shock_threnody" ) )
      return spell_shock_threnody;
   if( !str_cmp( name, "spell_optic_etude" ) )
      return spell_optic_etude;

   if( !str_cmp( name, "reserved" ) )
      return NULL;
   if( !str_cmp( name, "spell_null" ) )
      return spell_null;
   return spell_notfound;
}

DO_FUN *skill_function( char *name )
{
   switch ( name[3] )
   {
         /*
          * //T1
          */
      case 'a':
         if( !str_cmp( name, "do_assassinate" ) )
            return do_assassinate;
         if( !str_cmp( name, "do_aassign" ) )
            return do_aassign;
         if( !str_cmp( name, "do_abjucate" ) )
            return do_abjucate;
         if( !str_cmp( name, "do_advance" ) )
            return do_advance;
         if( !str_cmp( name, "do_advhero" ) )
            return do_advhero;
         if( !str_cmp( name, "do_addchange" ) )
            return do_addchange;
         if( !str_cmp( name, "do_addlag" ) )
            return do_addlag;
         if( !str_cmp( name, "do_addsanc" ) )
            return do_addsanc;
         if( !str_cmp( name, "do_addsith" ) )
            return do_addsith;
         if( !str_cmp( name, "do_admtalk" ) )
            return do_admtalk;
         if( !str_cmp( name, "do_aexit" ) )
            return do_aexit;
         if( !str_cmp( name, "do_affected" ) )
            return do_affected;
         if( !str_cmp( name, "do_affstrip" ) )
            return do_affstrip;
         if( !str_cmp( name, "do_afk" ) )
            return do_afk;
         if( !str_cmp( name, "do_aid" ) )
            return do_aid;
         if( !str_cmp( name, "do_alertness" ) )
            return do_alertness;
         if( !str_cmp( name, "do_alia" ) )
            return do_alia;
         if( !str_cmp( name, "do_alias" ) )
            return do_alias;
         if( !str_cmp( name, "do_allow" ) )
            return do_allow;
         if( !str_cmp( name, "do_ansi" ) )
            return do_ansi;
         if( !str_cmp( name, "do_answer" ) )
            return do_answer;
         if( !str_cmp( name, "do_aol" ) )
            return do_aol;
         if( !str_cmp( name, "do_apoth" ) )
            return do_apoth;
         if( !str_cmp( name, "do_apply" ) )
            return do_apply;
         if( !str_cmp( name, "do_appraise" ) )
            return do_appraise;
         if( !str_cmp( name, "do_areas" ) )
            return do_areas;
         if( !str_cmp( name, "do_aset" ) )
            return do_aset;
         if( !str_cmp( name, "do_ask" ) )
            return do_ask;
         if( !str_cmp( name, "do_astat" ) )
            return do_astat;
         if( !str_cmp( name, "do_at" ) )
            return do_at;
         if( !str_cmp( name, "do_atobj" ) )
            return do_atobj;
         if( !str_cmp( name, "do_auction" ) )
            return do_auction;
         if( !str_cmp( name, "do_authorize" ) )
            return do_authorize;
         if( !str_cmp( name, "do_autos" ) )
            return do_autos;
/*	if ( !str_cmp( name, "do_autoloot" ))		return do_autoloot;
	if ( !str_cmp( name, "do_autosac" ))		return do_autosac;
	if ( !str_cmp( name, "do_autocoin" )) 		return do_autocoin; */
         if( !str_cmp( name, "do_avtalk" ) )
            return do_avtalk;
         if( !str_cmp( name, "do_acotalk" ) )
            return do_acotalk;
         if( !str_cmp( name, "do_accept" ) )
            return do_accept;
         if( !str_cmp( name, "do_ahall" ) )
            return do_ahall;
         if( !str_cmp( name, "do_arena" ) )
            return do_arena;
         if( !str_cmp( name, "do_ascend" ) )
            return do_ascend;
         if( !str_cmp( name, "do_award" ) )
            return do_award;
         if( !str_cmp( name, "do_awho" ) )
            return do_awho;
         if( !str_cmp( name, "do_aclist" ) )
            return do_aclist;
         if( !str_cmp( name, "do_avlist" ) )
            return do_avlist;
         break;
      case 'b':
         if( !str_cmp( name, "do_backstab" ) )
            return do_backstab;
         if( !str_cmp( name, "do_backup" ) )
            return do_backup;
         if( !str_cmp( name, "do_balance" ) )
            return do_balance;
         if( !str_cmp( name, "do_balzhur" ) )
            return do_balzhur;
         if( !str_cmp( name, "do_bamfin" ) )
            return do_bamfin;
         if( !str_cmp( name, "do_bamfout" ) )
            return do_bamfout;
         if( !str_cmp( name, "do_ban" ) )
            return do_ban;
         if( !str_cmp( name, "do_bash" ) )
            return do_bash;
         if( !str_cmp( name, "do_bashdoor" ) )
            return do_bashdoor;
         if( !str_cmp( name, "do_beep" ) )
            return do_beep;
         if( !str_cmp( name, "do_berserk" ) )
            return do_berserk;
         if( !str_cmp( name, "do_bestow" ) )
            return do_bestow;
         if( !str_cmp( name, "do_bestowarea" ) )
            return do_bestowarea;
         if( !str_cmp( name, "do_bio" ) )
            return do_bio;
         if( !str_cmp( name, "do_bite" ) )
            return do_bite;
         if( !str_cmp( name, "do_bloodlet" ) )
            return do_bloodlet;
         if( !str_cmp( name, "do_bet" ) )
            return do_bet;
         if( !str_cmp( name, "do_boards" ) )
            return do_boards;
         if( !str_cmp( name, "do_bodybag" ) )
            return do_bodybag;
         if( !str_cmp( name, "do_bolt" ) )
            return do_bolt;
         if( !str_cmp( name, "do_book" ) )
            return do_book;
         if( !str_cmp( name, "do_bounty" ) )
            return do_bounty;
         if( !str_cmp( name, "do_bountylist" ) )
            return do_bountylist;
         if( !str_cmp( name, "do_bowcraft" ) )
            return do_bowcraft;
         if( !str_cmp( name, "do_brandish" ) )
            return do_brandish;
         if( !str_cmp( name, "do_broach" ) )
            return do_broach;
         if( !str_cmp( name, "do_bset" ) )
            return do_bset;
         if( !str_cmp( name, "do_bstat" ) )
            return do_bstat;
         if( !str_cmp( name, "do_bug" ) )
            return do_bug;
         if( !str_cmp( name, "do_bugs" ) )
            return do_bugs;
         if( !str_cmp( name, "do_bsave" ) )
            return do_bsave;
         if( !str_cmp( name, "do_building" ) )
            return do_building;
         if( !str_cmp( name, "do_bury" ) )
            return do_bury;
         if( !str_cmp( name, "do_buy" ) )
            return do_buy;
         break;
      case 'c':
         if( !str_cmp( name, "do_cast" ) )
            return do_cast;
         if( !str_cmp( name, "do_cedit" ) )
            return do_cedit;
         if( !str_cmp( name, "do_coe" ) )
            return do_coe;
         if( !str_cmp( name, "do_channels" ) )
            return do_channels;
         if( !str_cmp( name, "do_challenge" ) )
            return do_challenge;
         if( !str_cmp( name, "do_chaos" ) )
            return do_chaos;
         if( !str_cmp( name, "do_chat" ) )
            return do_chat;
         if( !str_cmp( name, "do_changes" ) )
            return do_changes;
         if( !str_cmp( name, "do_chareg" ) )
            return do_chareg;
         if( !str_cmp( name, "do_chsave" ) )
            return do_chsave;
         if( !str_cmp( name, "do_check_vnums" ) )
            return do_check_vnums;
         if( !str_cmp( name, "do_circle" ) )
            return do_circle;
         if( !str_cmp( name, "do_classes" ) )
            return do_classes;
         if( !str_cmp( name, "do_clans" ) )
            return do_clans;
         if( !str_cmp( name, "do_clantalk" ) )
            return do_clantalk;
         if( !str_cmp( name, "do_climate" ) )
            return do_climate;
         if( !str_cmp( name, "do_climb" ) )
            return do_climb;
         if( !str_cmp( name, "do_close" ) )
            return do_close;
         if( !str_cmp( name, "do_cmdtable" ) )
            return do_cmdtable;
         if( !str_cmp( name, "do_cmeeting" ) )
            return do_cmeeting;
         if( !str_cmp( name, "do_codetalk" ) )
            return do_codetalk;
         if( !str_cmp( name, "do_colorize" ) )
            return do_colorize;
         if( !str_cmp( name, "do_commands" ) )
            return do_commands;
         if( !str_cmp( name, "do_comment" ) )
            return do_comment;
         if( !str_cmp( name, "do_compare" ) )
            return do_compare;
         if( !str_cmp( name, "do_config" ) )
            return do_config;
         if( !str_cmp( name, "do_convert" ) )
            return do_convert;
         if( !str_cmp( name, "do_connect" ) )
            return do_connect;
         if( !str_cmp( name, "do_consider" ) )
            return do_consider;
         if( !str_cmp( name, "do_cook" ) )
            return do_cook;
         if( !str_cmp( name, "do_copyover" ) )
            return do_copyover;
         if( !str_cmp( name, "do_copyright" ) )
            return do_copyright;
         if( !str_cmp( name, "do_corpse" ) )
            return do_corpse;
         if( !str_cmp( name, "do_count" ) )
            return do_count;
         if( !str_cmp( name, "do_council_induct" ) )
            return do_council_induct;
         if( !str_cmp( name, "do_council_outcast" ) )
            return do_council_outcast;
         if( !str_cmp( name, "do_councils" ) )
            return do_councils;
         if( !str_cmp( name, "do_counciltalk" ) )
            return do_counciltalk;
         if( !str_cmp( name, "do_craft" ) )
            return do_craft;
         if( !str_cmp( name, "do_crank" ) )
            return do_crank;
         if( !str_cmp( name, "do_credits" ) )
            return do_credits;
         if( !str_cmp( name, "do_cribsheet" ) )
            return do_cribsheet;
         if( !str_cmp( name, "do_cset" ) )
            return do_cset;
         break;
      case 'd':
         if( !str_cmp( name, "do_deaf" ) )
            return do_deaf;
         if( !str_cmp( name, "do_decline" ) )
            return do_decline;
         if( !str_cmp( name, "do_deities" ) )
            return do_deities;
         if( !str_cmp( name, "do_delay" ) )
            return do_delay;
         if( !str_cmp( name, "do_delete" ) )
            return do_delete;
         if( !str_cmp( name, "do_demote" ) )
            return do_demote;
         if( !str_cmp( name, "do_deny" ) )
            return do_deny;
         if( !str_cmp( name, "do_deposit" ) )
            return do_deposit;
         if( !str_cmp( name, "do_description" ) )
            return do_description;
         if( !str_cmp( name, "do_destro" ) )
            return do_destro;
         if( !str_cmp( name, "do_destroy" ) )
            return do_destroy;
         if( !str_cmp( name, "do_detrap" ) )
            return do_detrap;
         if( !str_cmp( name, "do_devote" ) )
            return do_devote;
         if( !str_cmp( name, "do_dice" ) )
            return do_dice;
         if( !str_cmp( name, "do_dig" ) )
            return do_dig;
         if( !str_cmp( name, "do_disarm" ) )
            return do_disarm;
         if( !str_cmp( name, "do_disconnect" ) )
            return do_disconnect;
         if( !str_cmp( name, "do_dislodge" ) )
            return do_dislodge;
         if( !str_cmp( name, "do_dismiss" ) )
            return do_dismiss;
         if( !str_cmp( name, "do_dismount" ) )
            return do_dismount;
         if( !str_cmp( name, "do_divorce" ) )
            return do_divorce;
         if( !str_cmp( name, "do_dmesg" ) )
            return do_dmesg;
         if( !str_cmp( name, "do_dnd" ) )
            return do_dnd;
         if( !str_cmp( name, "do_donate" ) )
            return do_donate;
         if( !str_cmp( name, "do_doublexp" ) )
            return do_doublexp;
         if( !str_cmp( name, "do_down" ) )
            return do_down;
         if( !str_cmp( name, "do_drag" ) )
            return do_drag;
         if( !str_cmp( name, "do_draw" ) )
            return do_draw;
         if( !str_cmp( name, "do_drink" ) )
            return do_drink;
         if( !str_cmp( name, "do_drop" ) )
            return do_drop;
         if( !str_cmp( name, "do_dual" ) )
            return do_dual;
         if( !str_cmp( name, "do_dump" ) )
            return do_dump;
         if( !str_cmp( name, "do_diagnose" ) )
            return do_diagnose;
         break;
      case 'e':
         if( !str_cmp( name, "do_east" ) )
            return do_east;
         if( !str_cmp( name, "do_eat" ) )
            return do_eat;
         if( !str_cmp( name, "do_ech" ) )
            return do_ech;
         if( !str_cmp( name, "do_echo" ) )
            return do_echo;
         if( !str_cmp( name, "do_email" ) )
            return do_email;
         if( !str_cmp( name, "do_emote" ) )
            return do_emote;
         if( !str_cmp( name, "do_empty" ) )
            return do_empty;
         if( !str_cmp( name, "do_enter" ) )
            return do_enter;
         if( !str_cmp( name, "do_equipment" ) )
            return do_equipment;
         if( !str_cmp( name, "do_eqreg" ) )
            return do_eqreg;
         if( !str_cmp( name, "do_examine" ) )
            return do_examine;
         if( !str_cmp( name, "do_exits" ) )
            return do_exits;
         if( !str_cmp( name, "do_expset" ) )
            return do_expset;
         break;
      case 'f':
         if( !str_cmp( name, "do_feed" ) )
            return do_feed;
         if( !str_cmp( name, "do_ferralclaw" ) )
            return do_ferralclaw;
         if( !str_cmp( name, "do_fill" ) )
            return do_fill;
         if( !str_cmp( name, "do_findnote" ) )
            return do_findnote;
         if( !str_cmp( name, "do_finger" ) )
            return do_finger;
         if( !str_cmp( name, "do_fxp" ) )
            return do_fxp;
         if( !str_cmp( name, "do_fhelp" ) )
            return do_fhelp;
         if( !str_cmp( name, "do_fire" ) )
            return do_fire;
         if( !str_cmp( name, "do_fixchar" ) )
            return do_fixchar;
         if( !str_cmp( name, "do_fixmobs" ) )
            return do_fixmobs;
         if( !str_cmp( name, "do_fixed" ) )
            return do_fixed;
         if( !str_cmp( name, "do_flame" ) )
            return do_flame;
         if( !str_cmp( name, "do_flee" ) )
            return do_flee;
         if( !str_cmp( name, "do_fletch" ) )
            return do_fletch;
         if( !str_cmp( name, "do_flock" ) )
            return do_flock;
         if( !str_cmp( name, "do_flocko" ) )
            return do_flocko;
         if( !str_cmp( name, "do_flutecraft" ) )
            return do_flutecraft;
         if( !str_cmp( name, "do_foldarea" ) )
            return do_foldarea;
         if( !str_cmp( name, "do_follow" ) )
            return do_follow;
         if( !str_cmp( name, "do_for" ) )
            return do_for;
         if( !str_cmp( name, "do_forge" ) )
            return do_forge;
         if( !str_cmp( name, "do_forget" ) )
            return do_forget;
         if( !str_cmp( name, "do_force" ) )
            return do_force;
         if( !str_cmp( name, "do_force_throw" ) )
            return do_force_throw;
         if( !str_cmp( name, "do_force_push" ) )
            return do_force_push;
         if( !str_cmp( name, "do_forceclose" ) )
            return do_forceclose;
         if( !str_cmp( name, "do_form_password" ) )
            return do_form_password;
         if( !str_cmp( name, "do_fquit" ) )
            return do_fquit;
         if( !str_cmp( name, "do_free_vnums" ) )
            return do_free_vnums;
         if( !str_cmp( name, "do_freeze" ) )
            return do_freeze;
         if( !str_cmp( name, "do_fshow" ) )
            return do_fshow;
         break;
      case 'g':
         if( !str_cmp( name, "do_garb" ) )
            return do_garb;
         if( !str_cmp( name, "do_gaso" ) )
            return do_gaso;
         if( !str_cmp( name, "do_get" ) )
            return do_get;
         if( !str_cmp( name, "do_gfighting" ) )
            return do_gfighting;
         if( !str_cmp( name, "do_give" ) )
            return do_give;
         if( !str_cmp( name, "do_glance" ) )
            return do_glance;
         if( !str_cmp( name, "do_global_boards" ) )
            return do_global_boards;
         if( !str_cmp( name, "do_global_note" ) )
            return do_global_note;
         if( !str_cmp( name, "do_gloryset" ) )
            return do_gloryset;
         if( !str_cmp( name, "do_glorytrade" ) )
            return do_glorytrade;
         if( !str_cmp( name, "do_gold" ) )
            return do_gold;
         if( !str_cmp( name, "do_goto" ) )
            return do_goto;
         if( !str_cmp( name, "do_gouge" ) )
            return do_gouge;
         if( !str_cmp( name, "do_group" ) )
            return do_group;
         if( !str_cmp( name, "do_gratz" ) )
            return do_gratz;
         if( !str_cmp( name, "do_grub" ) )
            return do_grub;
         if( !str_cmp( name, "do_gtell" ) )
            return do_gtell;
         if( !str_cmp( name, "do_guilds" ) )
            return do_guilds;
         if( !str_cmp( name, "do_guildtalk" ) )
            return do_guildtalk;
         if( !str_cmp( name, "do_gwhere" ) )
            return do_gwhere;
         if( !str_cmp( name, "do_gplague" ) )
            return do_gplague;
         break;
      case 'h':
         if( !str_cmp( name, "do_hadnd" ) )
            return do_hadnd;
         if( !str_cmp( name, "do_halfexp" ) )
            return do_halfexp;
         if( !str_cmp( name, "do_hbuild" ) )
            return do_hbuild;
         if( !str_cmp( name, "do_hedit" ) )
            return do_hedit;
         if( !str_cmp( name, "do_hell" ) )
            return do_hell;
         if( !str_cmp( name, "do_hell2" ) )
            return do_hell2;
         if( !str_cmp( name, "do_hell3" ) )
            return do_hell3;
         if( !str_cmp( name, "do_helllist" ) )
            return do_helllist;
         if( !str_cmp( name, "do_help" ) )
            return do_help;
         if( !str_cmp( name, "do_heighten" ) )
            return do_heighten;
         if( !str_cmp( name, "do_hide" ) )
            return do_hide;
         if( !str_cmp( name, "do_highapoth" ) )
            return do_highapoth;
         if( !str_cmp( name, "do_highdam" ) )
            return do_highdam;
         if( !str_cmp( name, "do_hitall" ) )
            return do_hitall;
         if( !str_cmp( name, "do_htalk" ) )
            return do_htalk;
         if( !str_cmp( name, "do_hl" ) )
            return do_hl;
         if( !str_cmp( name, "do_hlist" ) )
            return do_hlist;
         if( !str_cmp( name, "do_helplist" ) )
            return do_helplist;
         if( !str_cmp( name, "do_holylight" ) )
            return do_holylight;
         if( !str_cmp( name, "do_homebound" ) )
            return do_homebound;
         if( !str_cmp( name, "do_homepage" ) )
            return do_homepage;
         if( !str_cmp( name, "do_hset" ) )
            return do_hset;
         if( !str_cmp( name, "do_hstat" ) )
            return do_hstat;
         break;
      case 'i':
         if( !str_cmp( name, "do_iaw" ) )
            return do_iaw;
         if( !str_cmp( name, "do_ic" ) )
            return do_ic;
         if( !str_cmp( name, "do_icq" ) )
            return do_icq;
         if( !str_cmp( name, "do_idle" ) )
            return do_idle;
         if( !str_cmp( name, "do_ide" ) )
            return do_ide;
         if( !str_cmp( name, "do_idea" ) )
            return do_idea;
         if( !str_cmp( name, "do_ignore" ) )
            return do_ignore;
         if( !str_cmp( name, "do_ilog" ) )
            return do_ilog;
         if( !str_cmp( name, "do_immeq" ) )
            return do_immeq;
         if( !str_cmp( name, "do_immhost" ) )
            return do_immhost;
         if( !str_cmp( name, "do_immlogtime" ) )
            return do_immlogtime;
         if( !str_cmp( name, "do_imm_morph" ) )
            return do_imm_morph;
         if( !str_cmp( name, "do_imm_unmorph" ) )
            return do_imm_unmorph;
         if( !str_cmp( name, "do_immortalize" ) )
            return do_immortalize;
         if( !str_cmp( name, "do_immtalk" ) )
            return do_immtalk;
         if( !str_cmp( name, "do_immwhere" ) )
            return do_immwhere;
         if( !str_cmp( name, "do_imm_morph" ) )
            return do_imm_morph;
         if( !str_cmp( name, "do_imm_unmorph" ) )
            return do_imm_unmorph;
         if( !str_cmp( name, "do_impmsg" ) )
            return do_impmsg;
         if( !str_cmp( name, "do_induct" ) )
            return do_induct;
         if( !str_cmp( name, "do_info" ) )
            return do_info;
         if( !str_cmp( name, "do_installarea" ) )
            return do_installarea;
         if( !str_cmp( name, "do_instaroom" ) )
            return do_instaroom;
         if( !str_cmp( name, "do_instaworld" ) )
            return do_instaworld;
         if( !str_cmp( name, "do_instazone" ) )
            return do_instazone;
         if( !str_cmp( name, "do_inventory" ) )
            return do_inventory;
         if( !str_cmp( name, "do_invis" ) )
            return do_invis;
         if( !str_cmp( name, "do_ipcompare" ) )
            return do_ipcompare;
         break;
      case 'j':
         if( !str_cmp( name, "do_junk" ) )
            return do_junk;
         break;
      case 'k':
         if( !str_cmp( name, "do_kar" ) )
            return do_kar;
         if( !str_cmp( name, "do_kaura" ) )
            return do_kaura;
         if( !str_cmp( name, "do_keyitems" ) )
            return do_keyitems;
         if( !str_cmp( name, "do_khistory" ) )
            return do_khistory;
         if( !str_cmp( name, "do_kiamstat" ) )
            return do_kiamstat;
         if( !str_cmp( name, "do_kick" ) )
            return do_kick;
         if( !str_cmp( name, "do_kill" ) )
            return do_kill;
         break;
      case 'l':
         if( !str_cmp( name, "do_languages" ) )
            return do_languages;
         if( !str_cmp( name, "do_land" ) )
            return do_land;
         if( !str_cmp( name, "do_last" ) )
            return do_last;
         if( !str_cmp( name, "do_laws" ) )
            return do_laws;
         if( !str_cmp( name, "do_lcmd" ) )
            return do_lcmd;
         if( !str_cmp( name, "do_leave" ) )
            return do_leave;
         if( !str_cmp( name, "do_level" ) )
            return do_level;
         if( !str_cmp( name, "do_light" ) )
            return do_light;
         if( !str_cmp( name, "do_list" ) )
            return do_list;
         if( !str_cmp( name, "do_litterbug" ) )
            return do_litterbug;
         if( !str_cmp( name, "do_loadarea" ) )
            return do_loadarea;
         if( !str_cmp( name, "do_loadup" ) )
            return do_loadup;
         if( !str_cmp( name, "do_lock" ) )
            return do_lock;
         if( !str_cmp( name, "do_log" ) )
            return do_log;
         if( !str_cmp( name, "do_look" ) )
            return do_look;
         if( !str_cmp( name, "do_lookmap" ) )
            return do_lookmap;
         if( !str_cmp( name, "do_low_purge" ) )
            return do_low_purge;
         break;
      case 'm':
         if( !str_cmp( name, "do_mailroom" ) )
            return do_mailroom;
         if( !str_cmp( name, "do_make" ) )
            return do_make;
         if( !str_cmp( name, "do_makepotion" ) )
            return do_makepotion;
         if( !str_cmp( name, "do_makeboard" ) )
            return do_makeboard;
         if( !str_cmp( name, "do_makeclan" ) )
            return do_makeclan;
         if( !str_cmp( name, "do_makectime" ) )
            return do_makectime;
         if( !str_cmp( name, "do_makecouncil" ) )
            return do_makecouncil;
         if( !str_cmp( name, "do_makedeity" ) )
            return do_makedeity;
         if( !str_cmp( name, "do_makerepair" ) )
            return do_makerepair;
         if( !str_cmp( name, "do_makeshop" ) )
            return do_makeshop;
         if( !str_cmp( name, "do_makewizlist" ) )
            return do_makewizlist;
         if( !str_cmp( name, "do_mapout" ) )
            return do_mapout;
         if( !str_cmp( name, "do_marry" ) )
            return do_marry;
         if( !str_cmp( name, "do_massign" ) )
            return do_massign;
         if( !str_cmp( name, "do_mcreate" ) )
            return do_mcreate;
         if( !str_cmp( name, "do_mdelete" ) )
            return do_mdelete;
         if( !str_cmp( name, "do_meeting" ) )
            return do_meeting;
         if( !str_cmp( name, "do_memory" ) )
            return do_memory;
         if( !str_cmp( name, "do_mfind" ) )
            return do_mfind;
         if( !str_cmp( name, "do_midscore" ) )
            return do_midscore;
         if( !str_cmp( name, "do_minvoke" ) )
            return do_minvoke;
         if( !str_cmp( name, "do_mlist" ) )
            return do_mlist;
         if( !str_cmp( name, "do_monitor" ) )
            return do_monitor;
         //if ( !str_cmp( name, "do_morph" ) )      return do_morph;
         //if ( !str_cmp( name, "do_morph_char" ) )      return do_morph_char;
         if( !str_cmp( name, "do_morphcreate" ) )
            return do_morphcreate;
         if( !str_cmp( name, "do_morphdestroy" ) )
            return do_morphdestroy;
         if( !str_cmp( name, "do_morphset" ) )
            return do_morphset;
         if( !str_cmp( name, "do_morphstat" ) )
            return do_morphstat;
         if( !str_cmp( name, "do_mortality" ) )
            return do_mortality;
         if( !str_cmp( name, "do_mortalize" ) )
            return do_mortalize;
         if( !str_cmp( name, "do_motd" ) )
            return do_motd;
         if( !str_cmp( name, "do_mount" ) )
            return do_mount;
         if( !str_cmp( name, "do_move_hidden" ) )
            return do_move_hidden;
         if( !str_cmp( name, "do_movein" ) )
            return do_movein;
         if( !str_cmp( name, "do_moveout" ) )
            return do_moveout;
         if( !str_cmp( name, "do_mp_close_passage" ) )
            return do_mp_close_passage;
         if( !str_cmp( name, "do_mp_damage" ) )
            return do_mp_damage;
         if( !str_cmp( name, "do_mp_deposit" ) )
            return do_mp_deposit;
         if( !str_cmp( name, "do_mp_fill_in" ) )
            return do_mp_fill_in;
         if( !str_cmp( name, "do_mp_log" ) )
            return do_mp_log;
         if( !str_cmp( name, "do_mp_open_passage" ) )
            return do_mp_open_passage;
         if( !str_cmp( name, "do_mp_practice" ) )
            return do_mp_practice;
         if( !str_cmp( name, "do_mp_restore" ) )
            return do_mp_restore;
         if( !str_cmp( name, "do_mp_slay" ) )
            return do_mp_slay;
         if( !str_cmp( name, "do_mp_withdraw" ) )
            return do_mp_withdraw;
         if( !str_cmp( name, "do_mpadvance" ) )
            return do_mpadvance;
         if( !str_cmp( name, "do_mpapply" ) )
            return do_mpapply;
         if( !str_cmp( name, "do_mpapplyb" ) )
            return do_mpapplyb;
         if( !str_cmp( name, "do_mpasound" ) )
            return do_mpasound;
         if( !str_cmp( name, "do_mpasupress" ) )
            return do_mpasupress;
         if( !str_cmp( name, "do_mpat" ) )
            return do_mpat;
         if( !str_cmp( name, "do_mpbodybag" ) )
            return do_mpbodybag;
         if( !str_cmp( name, "do_mpcopy" ) )
            return do_mpcopy;
         if( !str_cmp( name, "do_mpdelay" ) )
            return do_mpdelay;
         if( !str_cmp( name, "do_mpdream" ) )
            return do_mpdream;
         if( !str_cmp( name, "do_mpecho" ) )
            return do_mpecho;
         if( !str_cmp( name, "do_mpechoaround" ) )
            return do_mpechoaround;
         if( !str_cmp( name, "do_mpechoat" ) )
            return do_mpechoat;
         if( !str_cmp( name, "do_mpechozone" ) )
            return do_mpechozone;
         if( !str_cmp( name, "do_mpedit" ) )
            return do_mpedit;
         if( !str_cmp( name, "do_mpfavor" ) )
            return do_mpfavor;
         if( !str_cmp( name, "do_mpforce" ) )
            return do_mpforce;
         if( !str_cmp( name, "do_mpgoto" ) )
            return do_mpgoto;
         if( !str_cmp( name, "do_mpinvis" ) )
            return do_mpinvis;
         if( !str_cmp( name, "do_mpjunk" ) )
            return do_mpjunk;
         if( !str_cmp( name, "do_mpkill" ) )
            return do_mpkill;
         if( !str_cmp( name, "do_mpmload" ) )
            return do_mpmload;
         if( !str_cmp( name, "do_mpmorph" ) )
            return do_mpmorph;
         if( !str_cmp( name, "do_mpmset" ) )
            return do_mpmset;
         if( !str_cmp( name, "do_mpmusic" ) )
            return do_mpmusic;
         if( !str_cmp( name, "do_mpmusicaround" ) )
            return do_mpmusicaround;
         if( !str_cmp( name, "do_mpmusicat" ) )
            return do_mpmusicat;
         if( !str_cmp( name, "do_mpnothing" ) )
            return do_mpnothing;
         if( !str_cmp( name, "do_mpnuisance" ) )
            return do_nuisance;
         if( !str_cmp( name, "do_mpoload" ) )
            return do_mpoload;
         if( !str_cmp( name, "do_mposet" ) )
            return do_mposet;
         if( !str_cmp( name, "do_mppardon" ) )
            return do_mppardon;
         if( !str_cmp( name, "do_mppeace" ) )
            return do_mppeace;
         if( !str_cmp( name, "do_mppkset" ) )
            return do_mppkset;
         if( !str_cmp( name, "do_mppquit" ) )
            return do_mppquit;
         if( !str_cmp( name, "do_mppurge" ) )
            return do_mppurge;
         if( !str_cmp( name, "do_mpscatter" ) )
            return do_mpscatter;
         if( !str_cmp( name, "do_mpsound" ) )
            return do_mpsound;
         if( !str_cmp( name, "do_mpsoundaround" ) )
            return do_mpsoundaround;
         if( !str_cmp( name, "do_mpsoundat" ) )
            return do_mpsoundat;
         if( !str_cmp( name, "do_mpstat" ) )
            return do_mpstat;
         if( !str_cmp( name, "do_mptransfer" ) )
            return do_mptransfer;
         if( !str_cmp( name, "do_mpunmorph" ) )
            return do_mpunmorph;
         if( !str_cmp( name, "do_mpunnuisance" ) )
            return do_mpunnuisance;
         if( !str_cmp( name, "do_mrange" ) )
            return do_mrange;
         if( !str_cmp( name, "do_mpfind" ) )
            return do_mpfind;
         if( !str_cmp( name, "do_mset" ) )
            return do_mset;
         if( !str_cmp( name, "do_mstat" ) )
            return do_mstat;
         if( !str_cmp( name, "do_mudschool" ) )
            return do_mudschool;
         if( !str_cmp( name, "do_mudstat" ) )
            return do_mudstat;
         if( !str_cmp( name, "do_murde" ) )
            return do_murde;
         if( !str_cmp( name, "do_murder" ) )
            return do_murder;
         if( !str_cmp( name, "do_muse" ) )
            return do_muse;
         if( !str_cmp( name, "do_music" ) )
            return do_music;
         if( !str_cmp( name, "do_mwhere" ) )
            return do_mwhere;
         break;
      case 'n':
         if( !str_cmp( name, "do_name" ) )
            return do_name;
         if( !str_cmp( name, "do_nch" ) )
            return do_nch;
         if( !str_cmp( name, "do_neotalk" ) )
            return do_neotalk;
         if( !str_cmp( name, "do_newaff" ) )
            return do_newaff;
         if( !str_cmp( name, "do_newbiechat" ) )
            return do_newbiechat;
         if( !str_cmp( name, "do_newbieset" ) )
            return do_newbieset;
         if( !str_cmp( name, "do_newpass" ) )
            return do_newpass;
         if( !str_cmp( name, "do_newpl" ) )
            return do_newpl;
         if( !str_cmp( name, "do_newpl2" ) )
            return do_newpl2;
         if( !str_cmp( name, "do_news" ) )
            return do_news;
         if( !str_cmp( name, "do_newzones" ) )
            return do_newzones;
         if( !str_cmp( name, "do_nohelps" ) )
            return do_nohelps;
         if( !str_cmp( name, "do_noemote" ) )
            return do_noemote;
         if( !str_cmp( name, "do_noresolve" ) )
            return do_noresolve;
         if( !str_cmp( name, "do_north" ) )
            return do_north;
         if( !str_cmp( name, "do_northeast" ) )
            return do_northeast;
         if( !str_cmp( name, "do_northwest" ) )
            return do_northwest;
         if( !str_cmp( name, "do_notell" ) )
            return do_notell;
         if( !str_cmp( name, "do_notitle" ) )
            return do_notitle;
         if( !str_cmp( name, "do_noteroom" ) )
            return do_noteroom;
         if( !str_cmp( name, "do_nrange" ) )
            return do_nrange;
         if( !str_cmp( name, "do_nuisance" ) )
            return do_nuisance;
         if( !str_cmp( name, "do_numlock" ) )
            return do_numlock;
         if( !str_cmp( name, "do_nelist" ) )
            return do_nelist;
         if( !str_cmp( name, "do_nstalk" ) )
            return do_nstalk;
         break;
      case 'o':
         if( !str_cmp( name, "do_oassign" ) )
            return do_oassign;
         if( !str_cmp( name, "do_ocreate" ) )
            return do_ocreate;
         if( !str_cmp( name, "do_odelete" ) )
            return do_odelete;
         if( !str_cmp( name, "do_ofind" ) )
            return do_ofind;
         if( !str_cmp( name, "do_ofha" ) )
            return do_ofha;
         if( !str_cmp( name, "do_ogrub" ) )
            return do_ogrub;
         if( !str_cmp( name, "do_oinvoke" ) )
            return do_oinvoke;
         if( !str_cmp( name, "do_offer" ) )
            return do_offer;
         if( !str_cmp( name, "do_olist" ) )
            return do_olist;
         if( !str_cmp( name, "do_oldchanges" ) )
            return do_oldchanges;
         if( !str_cmp( name, "do_onedown" ) )
            return do_onedown;
         if( !str_cmp( name, "do_oneup" ) )
            return do_oneup;
         if( !str_cmp( name, "do_opcopy" ) )
            return do_opcopy;
         if( !str_cmp( name, "do_opedit" ) )
            return do_opedit;
         if( !str_cmp( name, "do_open" ) )
            return do_open;
         if( !str_cmp( name, "do_opfind" ) )
            return do_opfind;
         if( !str_cmp( name, "do_opstat" ) )
            return do_opstat;
         if( !str_cmp( name, "do_orange" ) )
            return do_orange;
         if( !str_cmp( name, "do_order" ) )
            return do_order;
         if( !str_cmp( name, "do_orders" ) )
            return do_orders;
         if( !str_cmp( name, "do_ordertalk" ) )
            return do_ordertalk;
         if( !str_cmp( name, "do_osan" ) )
            return do_osan;
         if( !str_cmp( name, "do_oset" ) )
            return do_oset;
         if( !str_cmp( name, "do_ostat" ) )
            return do_ostat;
         if( !str_cmp( name, "do_outcast" ) )
            return do_outcast;
         if( !str_cmp( name, "do_owhere" ) )
            return do_owhere;
         break;
      case 'p':
         if( !str_cmp( name, "do_pager" ) )
            return do_pager;
         if( !str_cmp( name, "do_pardon" ) )
            return do_pardon;
         if( !str_cmp( name, "do_password" ) )
            return do_password;
         if( !str_cmp( name, "do_pcc" ) )
            return do_pcc;
         if( !str_cmp( name, "do_pcrename" ) )
            return do_pcrename;
         if( !str_cmp( name, "do_peace" ) )
            return do_peace;
         if( !str_cmp( name, "do_pfiles" ) )
            return do_pfiles;
         if( !str_cmp( name, "do_pick" ) )
            return do_pick;
         if( !str_cmp( name, "do_plock" ) )
            return do_plock;
         if( !str_cmp( name, "do_plocko" ) )
            return do_plocko;
         if( !str_cmp( name, "do_poison_weapon" ) )
            return do_poison_weapon;
         if( !str_cmp( name, "do_pplague" ) )
            return do_pplague;
         if( !str_cmp( name, "do_pqui" ) )
            return do_pqui;
         if( !str_cmp( name, "do_pquit" ) )
            return do_pquit;
         if( !str_cmp( name, "do_practice" ) )
            return do_practice;
         if( !str_cmp( name, "do_pracset" ) )
            return do_pracset;
         if( !str_cmp( name, "do_pray" ) )
            return do_pray;
         if( !str_cmp( name, "do_prepreboot" ) )
            return do_prepreboot;
         if( !str_cmp( name, "do_pretitle" ) )
            return do_pretitle;
         if( !str_cmp( name, "do_privacy" ) )
            return do_privacy;
         if( !str_cmp( name, "do_profane" ) )
            return do_profane;
         if( !str_cmp( name, "do_project" ) )
            return do_project;
         if( !str_cmp( name, "do_promote" ) )
            return do_promote;
         if( !str_cmp( name, "do_prompt" ) )
            return do_prompt;
         if( !str_cmp( name, "do_pscore" ) )
            return do_pscore;
         if( !str_cmp( name, "do_pull" ) )
            return do_pull;
         if( !str_cmp( name, "do_punch" ) )
            return do_punch;
         if( !str_cmp( name, "do_purge" ) )
            return do_purge;
         if( !str_cmp( name, "do_push" ) )
            return do_push;
         if( !str_cmp( name, "do_put" ) )
            return do_put;
         break;
      case 'q':
         if( !str_cmp( name, "do_qboost" ) )
            return do_qboost;
         if( !str_cmp( name, "do_qpset" ) )
            return do_qpset;
         if( !str_cmp( name, "do_qpstat" ) )
            return do_qpstat;
         if( !str_cmp( name, "do_quaff" ) )
            return do_quaff;
         if( !str_cmp( name, "do_quest" ) )
            return do_quest;
         if( !str_cmp( name, "do_qui" ) )
            return do_qui;
         if( !str_cmp( name, "do_quit" ) )
            return do_quit;
         if( !str_cmp( name, "do_quiver" ) )
            return do_quiver;
         if( !str_cmp( name, "do_quota" ) )
            return do_quota;
         if( !str_cmp( name, "do_qwest" ) )
            return do_qwest;
         break;
      case 'r':
         if( !str_cmp( name, "do_races" ) )
            return do_races;
         if( !str_cmp( name, "do_racial" ) )
            return do_racial;
         if( !str_cmp( name, "do_racetalk" ) )
            return do_racetalk;
         if( !str_cmp( name, "do_rapsheet" ) )
            return do_rapsheet;
         if( !str_cmp( name, "do_rap" ) )
            return do_rap;
         if( !str_cmp( name, "do_rapon" ) )
            return do_rapon;
         if( !str_cmp( name, "do_rassign" ) )
            return do_rassign;
         if( !str_cmp( name, "do_rat" ) )
            return do_rat;
         if( !str_cmp( name, "do_rdelete" ) )
            return do_rdelete;
         if( !str_cmp( name, "do_reboo" ) )
            return do_reboo;
         if( !str_cmp( name, "do_reboot" ) )
            return do_reboot;
         if( !str_cmp( name, "do_rebuild" ) )
            return do_rebuild;
         if( !str_cmp( name, "do_recall" ) )
            return do_recall;
         if( !str_cmp( name, "do_recho" ) )
            return do_recho;
         if( !str_cmp( name, "do_recite" ) )
            return do_recite;
         if( !str_cmp( name, "do_redit" ) )
            return do_redit;
         if( !str_cmp( name, "do_register" ) )
            return do_register;
         if( !str_cmp( name, "do_reglist" ) )
            return do_reglist;
         if( !str_cmp( name, "do_regoto" ) )
            return do_regoto;
         if( !str_cmp( name, "do_reimb" ) )
            return do_reimb;
         if( !str_cmp( name, "do_remove" ) )
            return do_remove;
         if( !str_cmp( name, "do_remains" ) )
            return do_remains;
         if( !str_cmp( name, "do_rent" ) )
            return do_rent;
         if( !str_cmp( name, "do_repair" ) )
            return do_repair;
         if( !str_cmp( name, "do_repairset" ) )
            return do_repairset;
         if( !str_cmp( name, "do_repairshops" ) )
            return do_repairshops;
         if( !str_cmp( name, "do_repairstat" ) )
            return do_repairstat;
         if( !str_cmp( name, "do_repeat" ) )
            return do_repeat;
         if( !str_cmp( name, "do_reply" ) )
            return do_reply;
         if( !str_cmp( name, "do_report" ) )
            return do_report;
         if( !str_cmp( name, "do_rescue" ) )
            return do_rescue;
         if( !str_cmp( name, "do_reserve" ) )
            return do_reserve;
         if( !str_cmp( name, "do_reset" ) )
            return do_reset;
         if( !str_cmp( name, "do_rest" ) )
            return do_rest;
         if( !str_cmp( name, "do_rrestore" ) )
            return do_rrestore;
         if( !str_cmp( name, "do_restore" ) )
            return do_restore;
         if( !str_cmp( name, "do_restoretime" ) )
            return do_restoretime;
         if( !str_cmp( name, "do_restrict" ) )
            return do_restrict;
         if( !str_cmp( name, "do_retell" ) )
            return do_retell;
         if( !str_cmp( name, "do_retire" ) )
            return do_retire;
         if( !str_cmp( name, "do_retran" ) )
            return do_retran;
         if( !str_cmp( name, "do_return" ) )
            return do_return;
         if( !str_cmp( name, "do_rework" ) )
            return do_rework;
         if( !str_cmp( name, "do_rgrub" ) )
            return do_rgrub;
         if( !str_cmp( name, "do_rip" ) )
            return do_rip;
         if( !str_cmp( name, "do_rlist" ) )
            return do_rlist;
         if( !str_cmp( name, "do_rolldice" ) )
            return do_rolldice;
         if( !str_cmp( name, "do_rolldie" ) )
            return do_rolldie;
         if( !str_cmp( name, "do_rpedit" ) )
            return do_rpedit;
         if( !str_cmp( name, "do_rpemote" ) )
            return do_rpemote;
         if( !str_cmp( name, "do_rpfind" ) )
            return do_rpfind;
         if( !str_cmp( name, "do_rpstat" ) )
            return do_rpstat;
         if( !str_cmp( name, "do_rreset" ) )
            return do_rreset;
         if( !str_cmp( name, "do_rset" ) )
            return do_rset;
         if( !str_cmp( name, "do_rstat" ) )
            return do_rstat;
         break;
      case 's':
         if( !str_cmp( name, "do_sacrifice" ) )
            return do_sacrifice;
         if( !str_cmp( name, "do_safety" ) )
            return do_safety;
         if( !str_cmp( name, "do_sanctuary" ) )
            return do_sanctuary;
         if( !str_cmp( name, "do_save" ) )
            return do_save;
         if( !str_cmp( name, "do_saveall" ) )
            return do_saveall;
         if( !str_cmp( name, "do_savearea" ) )
            return do_savearea;
         if( !str_cmp( name, "do_say" ) )
            return do_say;
         if( !str_cmp( name, "do_say_to_char" ) )
            return do_say_to_char;
         if( !str_cmp( name, "do_scan" ) )
            return do_scan;
         if( !str_cmp( name, "do_scatter" ) )
            return do_scatter;
         if( !str_cmp( name, "do_score" ) )
            return do_score;
         if( !str_cmp( name, "do_scoresheet" ) )
            return do_scoresheet;
         if( !str_cmp( name, "do_scribe" ) )
            return do_scribe;
         if( !str_cmp( name, "do_search" ) )
            return do_search;
         if( !str_cmp( name, "do_sedit" ) )
            return do_sedit;
         if( !str_cmp( name, "do_seedate" ) )
            return do_seedate;
         if( !str_cmp( name, "do_sell" ) )
            return do_sell;
         if( !str_cmp( name, "do_set" ) )
            return do_set;
         if( !str_cmp( name, "do_set_boot_time" ) )
            return do_set_boot_time;
         if( !str_cmp( name, "do_setclan" ) )
            return do_setclan;
         if( !str_cmp( name, "do_setclass" ) )
            return do_setclass;
         if( !str_cmp( name, "do_setcouncil" ) )
            return do_setcouncil;
         if( !str_cmp( name, "do_setdeity" ) )
            return do_setdeity;
         if( !str_cmp( name, "do_setrace" ) )
            return do_setrace;
         if( !str_cmp( name, "do_setweather" ) )
            return do_setweather;
         if( !str_cmp( name, "do_sharpen" ) )
            return do_sharpen;
         if( !str_cmp( name, "do_shapeshift" ) )
            return do_shapeshift;
         if( !str_cmp( name, "do_shops" ) )
            return do_shops;
         if( !str_cmp( name, "do_shopset" ) )
            return do_shopset;
         if( !str_cmp( name, "do_shopstat" ) )
            return do_shopstat;
         if( !str_cmp( name, "do_shortscore" ) )
            return do_shortscore;
         if( !str_cmp( name, "do_shout" ) )
            return do_shout;
         if( !str_cmp( name, "do_shove" ) )
            return do_shove;
         if( !str_cmp( name, "do_showban" ) )
            return do_showban;
         if( !str_cmp( name, "do_showclan" ) )
            return do_showclan;
         if( !str_cmp( name, "do_showclass" ) )
            return do_showclass;
         if( !str_cmp( name, "do_showcouncil" ) )
            return do_showcouncil;
         if( !str_cmp( name, "do_showdeity" ) )
            return do_showdeity;
         if( !str_cmp( name, "do_showrace" ) )
            return do_showrace;
         if( !str_cmp( name, "do_showweather" ) )
            return do_showweather;
         if( !str_cmp( name, "do_shutdow" ) )
            return do_shutdow;
         if( !str_cmp( name, "do_shutdown" ) )
            return do_shutdown;
         if( !str_cmp( name, "do_silence" ) )
            return do_silence;
         if( !str_cmp( name, "do_sic" ) )
            return do_sic;
         if( !str_cmp( name, "do_sit" ) )
            return do_sit;
         if( !str_cmp( name, "do_skin" ) )
            return do_skin;
         if( !str_cmp( name, "do_sla" ) )
            return do_sla;
         if( !str_cmp( name, "do_slay" ) )
            return do_slay;
         if( !str_cmp( name, "do_slearn" ) )
            return do_slearn;
         if( !str_cmp( name, "do_sleep" ) )
            return do_sleep;
         if( !str_cmp( name, "do_slice" ) )
            return do_slice;
         if( !str_cmp( name, "do_slist" ) )
            return do_slist;
         if( !str_cmp( name, "do_sslist" ) )
            return do_sslist;
         if( !str_cmp( name, "do_slock" ) )
            return do_slock;
         if( !str_cmp( name, "do_slocko" ) )
            return do_slocko;
         if( !str_cmp( name, "do_slookup" ) )
            return do_slookup;
         if( !str_cmp( name, "do_smeeting" ) )
            return do_smeeting;
         if( !str_cmp( name, "do_smoke" ) )
            return do_smoke;
         if( !str_cmp( name, "do_snoop" ) )
            return do_snoop;
         if( !str_cmp( name, "do_socials" ) )
            return do_socials;
         if( !str_cmp( name, "do_south" ) )
            return do_south;
         if( !str_cmp( name, "do_southeast" ) )
            return do_southeast;
         if( !str_cmp( name, "do_southwest" ) )
            return do_southwest;
         if( !str_cmp( name, "do_speak" ) )
            return do_speak;
         if( !str_cmp( name, "do_spousetalk" ) )
            return do_spousetalk;
         if( !str_cmp( name, "do_spells" ) )
            return do_spells;
         if( !str_cmp( name, "do_split" ) )
            return do_split;
         if( !str_cmp( name, "do_ssang" ) )
            return do_ssang;
         if( !str_cmp( name, "do_sset" ) )
            return do_sset;
         if( !str_cmp( name, "do_stand" ) )
            return do_stand;
         if( !str_cmp( name, "do_stat" ) )
            return do_stat;
         if( !str_cmp( name, "do_statboost" ) )
            return do_statboost;
         if( !str_cmp( name, "do_statreport" ) )
            return do_statreport;
         if( !str_cmp( name, "do_statshield" ) )
            return do_statshield;
         if( !str_cmp( name, "do_steal" ) )
            return do_steal;
         if( !str_cmp( name, "do_strew" ) )
            return do_strew;
         if( !str_cmp( name, "do_strip" ) )
            return do_strip;
         if( !str_cmp( name, "do_stun" ) )
            return do_stun;
         if( !str_cmp( name, "do_style" ) )
            return do_style;
         if( !str_cmp( name, "do_switch" ) )
            return do_switch;
         if( !str_cmp( name, "do_showlayers" ) )
            return do_showlayers;
         break;
      case 't':
         if( !str_cmp( name, "do_tachi_kashi" ) )
            return do_tachi_kashi;
         if( !str_cmp( name, "do_tachi_jinpu" ) )
            return do_tachi_jinpu;
         if( !str_cmp( name, "do_tachi_haboku" ) )
            return do_tachi_haboku;
         if( !str_cmp( name, "do_tachi_gekko" ) )
            return do_tachi_gekko;
         if( !str_cmp( name, "do_tachi_koki" ) )
            return do_tachi_koki;
         if( !str_cmp( name, "do_tachi_enpi" ) )
            return do_tachi_enpi;
         if( !str_cmp( name, "do_tachi_kagero" ) )
            return do_tachi_kagero;
         if( !str_cmp( name, "do_tamp" ) )
            return do_tamp;
         if( !str_cmp( name, "do_tail" ) )
            return do_tail;
         if( !str_cmp( name, "do_tap" ) )
            return do_tap;
         if( !str_cmp( name, "do_tier" ) )
            return do_tier;
         if( !str_cmp( name, "do_tell" ) )
            return do_tell;
         if( !str_cmp( name, "do_think" ) )
            return do_think;
         if( !str_cmp( name, "do_throw" ) )
            return do_throw;
         if( !str_cmp( name, "do_time" ) )
            return do_time;
         if( !str_cmp( name, "do_timecmd" ) )
            return do_timecmd;
         if( !str_cmp( name, "do_title" ) )
            return do_title;
         if( !str_cmp( name, "do_tfind" ) )
            return do_tfind;
         if( !str_cmp( name, "do_tlock" ) )
            return do_tlock;
         if( !str_cmp( name, "do_tlocko" ) )
            return do_tlocko;
         if( !str_cmp( name, "do_track" ) )
            return do_track;
         if( !str_cmp( name, "do_ttrack" ) )
            return do_ttrack;
         if( !str_cmp( name, "do_train" ) )
            return do_train;
         if( !str_cmp( name, "do_traffic" ) )
            return do_traffic;
         if( !str_cmp( name, "do_transfer" ) )
            return do_transfer;
         if( !str_cmp( name, "do_trust" ) )
            return do_trust;
         if( !str_cmp( name, "do_tune" ) )
            return do_tune;
         if( !str_cmp( name, "do_typo" ) )
            return do_typo;
         break;
      case 'u':
         if( !str_cmp( name, "do_unalias" ) )
            return do_unalias;
         if( !str_cmp( name, "do_unbolt" ) )
            return do_unbolt;
         if( !str_cmp( name, "do_undeny" ) )
            return do_undeny;
         if( !str_cmp( name, "do_unfoldarea" ) )
            return do_unfoldarea;
         if( !str_cmp( name, "do_unhell" ) )
            return do_unhell;
         if( !str_cmp( name, "do_unhell2" ) )
            return do_unhell2;
         if( !str_cmp( name, "do_unhell3" ) )
            return do_unhell3;
         if( !str_cmp( name, "do_unlock" ) )
            return do_unlock;
         //if ( !str_cmp( name, "do_unmorph" ) )      return do_unmorph;
         //if ( !str_cmp( name, "do_unmorph_char" ) )      return do_unmorph_char;
         if( !str_cmp( name, "do_unnuisance" ) )
            return do_unnuisance;
         if( !str_cmp( name, "do_unrestore" ) )
            return do_unrestore;
         if( !str_cmp( name, "do_unsilence" ) )
            return do_unsilence;
         if( !str_cmp( name, "do_unshift" ) )
            return do_unshift;
         if( !str_cmp( name, "do_up" ) )
            return do_up;
         if( !str_cmp( name, "do_urna" ) )
            return do_urna;
         if( !str_cmp( name, "do_users" ) )
            return do_users;
         break;
      case 'v':
         if( !str_cmp( name, "do_value" ) )
            return do_value;
         if( !str_cmp( name, "do_vassign" ) )
            return do_vassign;
         if( !str_cmp( name, "do_version" ) )
            return do_version;
         if( !str_cmp( name, "do_victories" ) )
            return do_victories;
         if( !str_cmp( name, "do_visible" ) )
            return do_visible;
         if( !str_cmp( name, "do_vnums" ) )
            return do_vnums;
         if( !str_cmp( name, "do_vsearch" ) )
            return do_vsearch;
         break;
      case 'w':
         if( !str_cmp( name, "do_wake" ) )
            return do_wake;
         if( !str_cmp( name, "do_wartalk" ) )
            return do_wartalk;
         if( !str_cmp( name, "do_warn" ) )
            return do_warn;
         if( !str_cmp( name, "do_watch" ) )
            return do_watch;
         if( !str_cmp( name, "do_wear" ) )
            return do_wear;
         if( !str_cmp( name, "do_weather" ) )
            return do_weather;
         if( !str_cmp( name, "do_west" ) )
            return do_west;
         if( !str_cmp( name, "do_where" ) )
            return do_where;
         if( !str_cmp( name, "do_whisper" ) )
            return do_whisper;
         if( !str_cmp( name, "do_who" ) )
            return do_who;
         if( !str_cmp( name, "do_whois" ) )
            return do_whois;
         if( !str_cmp( name, "do_wimpy" ) )
            return do_wimpy;
         if( !str_cmp( name, "do_withdraw" ) )
            return do_withdraw;
         if( !str_cmp( name, "do_wizhelp" ) )
            return do_wizhelp;
         if( !str_cmp( name, "do_wizinfo" ) )
            return do_wizinfo;
         if( !str_cmp( name, "do_wizlist" ) )
            return do_wizlist;
         if( !str_cmp( name, "do_wizlock" ) )
            return do_wizlock;
         if( !str_cmp( name, "do_worth" ) )
            return do_worth;
         break;
      case 'x':
         if( !str_cmp( name, "do_xsedit" ) )
            return do_xsedit;
         if( !str_cmp( name, "do_xsocials" ) )
            return do_xsocials;
         break;
      case 'y':
         if( !str_cmp( name, "do_yahoo" ) )
            return do_yahoo;
         if( !str_cmp( name, "do_yell" ) )
            return do_yell;
         break;
      case 'z':
         if( !str_cmp( name, "do_zap" ) )
            return do_zap;
         if( !str_cmp( name, "do_zones" ) )
            return do_zones;
         /*
          * //T2
          */
   }
   return skill_notfound;
}

char *spell_name( SPELL_FUN * spell )
{
   if( spell == spell_acadia )
      return "spell_acadia";
   if( spell == spell_acid_blast )
      return "spell_acid_blast";
   if( spell == spell_animate_dead )
      return "spell_animate_dead";
   if( spell == spell_astral_walk )
      return "spell_astral_walk";
   if( spell == spell_barkskin )
      return "spell_barkskin";
   if( spell == spell_blindness )
      return "spell_blindness";
   if( spell == spell_burning_hands )
      return "spell_burning_hands";
   if( spell == spell_callmount )
      return "spell_callmount";
   if( spell == spell_call_lightning )
      return "spell_call_lightning";
   if( spell == spell_chainlightning )
      return "spell_chainlightning";
   if( spell == spell_icechain )
      return "spell_icechain";
   if( spell == spell_firechain )
      return "spell_firechain";
   if( spell == spell_cause_critical )
      return "spell_cause_critical";
   if( spell == spell_cause_light )
      return "spell_cause_light";
   if( spell == spell_cause_serious )
      return "spell_cause_serious";
   if( spell == spell_change_sex )
      return "spell_change_sex";
   if( spell == spell_charm_person )
      return "spell_charm_person";
   if( spell == spell_chill_touch )
      return "spell_chill_touch";
   if( spell == spell_colour_spray )
      return "spell_colour_spray";
   if( spell == spell_control_weather )
      return "spell_control_weather";
   if( spell == spell_continual_light )
      return "spell_continual_light";
   if( spell == spell_create_food )
      return "spell_create_food";
   if( spell == spell_create_water )
      return "spell_create_water";
   if( spell == spell_creeping_doom )
      return "spell_creeping_doom";
   if( spell == spell_cure_blindness )
      return "spell_cure_blindness";
   if( spell == spell_cure_poison )
      return "spell_cure_poison";
   if( spell == spell_curse )
      return "spell_curse";
   if( spell == spell_dangle )
      return "spell_dangle";
   if( spell == spell_delirium )
      return "spell_delirium";
   if( spell == spell_detect_poison )
      return "spell_detect_poison";
   if( spell == spell_dispel_evil )
      return "spell_dispel_evil";
   if( spell == spell_dispel_magic )
      return "spell_dispel_magic";
   if( spell == spell_dream )
      return "spell_dream";
   if( spell == spell_earthquake )
      return "spell_earthquake";
   if( spell == spell_enchant_weapon )
      return "spell_enchant_weapon";
   if( spell == spell_enchant_armor )
      return "spell_enchant_armor";
   if( spell == spell_energy_drain )
      return "spell_energy_drain";
   if( spell == spell_faerie_fire )
      return "spell_faerie_fire";
   if( spell == spell_faerie_fog )
      return "spell_faerie_fog";
   if( spell == spell_farsight )
      return "spell_farsight";
   if( spell == spell_fireball )
      return "spell_fireball";
   if( spell == spell_flamestrike )
      return "spell_flamestrike";
   if( spell == spell_demistrike )
      return "spell_demistrike";
   if( spell == spell_plasmastrike )
      return "spell_plasmastrike";
   if( spell == spell_gate )
      return "spell_gate";
   if( spell == spell_correlate )
      return "spell_correlate";
   if( spell == spell_gpc )
      return "spell_gpc";
   if( spell == spell_knock )
      return "spell_knock";
   if( spell == spell_harm )
      return "spell_harm";
   if( spell == spell_niaver )
      return "spell_niaver";
   if( spell == spell_headache )
      return "spell_headache";
   if( spell == spell_holyaura )
      return "spell_holyaura";
   if( spell == spell_holysight )
      return "spell_holysight";
   if( spell == spell_identify )
      return "spell_identify";
   if( spell == spell_invincibility )
      return "spell_invincibility";
   if( spell == spell_invis )
      return "spell_invis";
   if( spell == spell_impinvis )
      return "spell_impinvis";
   if( spell == spell_ironhand )
      return "spell_ironhand";
   if( spell == spell_ironskin )
      return "spell_ironskin";
   if( spell == spell_jedi_mind_trick )
      return "spell_jedi_mind_trick";
   if( spell == spell_jir )
      return "spell_jir";
   if( spell == spell_jur )
      return "spell_jur";
   if( spell == spell_know_alignment )
      return "spell_know_alignment";
   if( spell == spell_lightning_bolt )
      return "spell_lightning_bolt";
   if( spell == spell_locate_object )
      return "spell_locate_object";
   if( spell == spell_magic_missile )
      return "spell_magic_missile";
   if( spell == spell_mindblast )
      return "spell_mindblast";
   if( spell == spell_pass_door )
      return "spell_pass_door";
   if( spell == spell_poison )
      return "spell_poison";
   if( spell == spell_polymorph )
      return "spell_polymorph";
   if( spell == spell_possess )
      return "spell_possess";
   if( spell == spell_psidance )
      return "spell_psidance";
   if( spell == spell_psiblast )
      return "spell_psiblast";
   if( spell == spell_psibolt )
      return "spell_psibolt";
   if( spell == spell_psiwave )
      return "spell_psiwave";
   if( spell == spell_recharge )
      return "spell_recharge";
   if( spell == spell_remove_curse )
      return "spell_remove_curse";
   if( spell == spell_remove_invis )
      return "spell_remove_invis";
   if( spell == spell_remove_trap )
      return "spell_remove_trap";
   if( spell == spell_shocking_grasp )
      return "spell_shocking_grasp";
   if( spell == spell_sith_fury )
      return "spell_sith_fury";
   if( spell == spell_sleep )
      return "spell_sleep";
   if( spell == spell_steel_skin )
      return "spell_steel_skin";
   if( spell == spell_summon )
      return "spell_summon";
   if( spell == spell_teleport )
      return "spell_teleport";
   if( spell == spell_ventriloquate )
      return "spell_ventriloquate";
   if( spell == spell_weaken )
      return "spell_weaken";
   if( spell == spell_word_of_recall )
      return "spell_word_of_recall";
   if( spell == spell_acid_breath )
      return "spell_acid_breath";
   if( spell == spell_fire_breath )
      return "spell_fire_breath";
   if( spell == spell_frost_breath )
      return "spell_frost_breath";
   if( spell == spell_gas_breath )
      return "spell_gas_breath";
   if( spell == spell_hurricane )
      return "spell_hurricane";
   if( spell == spell_sandstorm )
      return "spell_sandstorm";
   if( spell == spell_firestorm )
      return "spell_firestorm";
   if( spell == spell_avalanche )
      return "spell_avalanche";
   if( spell == spell_meteor )
      return "spell_meteor";
   if( spell == spell_lightning_breath )
      return "spell_lightning_breath";
   if( spell == spell_spiral_blast )
      return "spell_spiral_blast";
   if( spell == spell_scorching_surge )
      return "spell_scorching_surge";
   if( spell == spell_transport )
      return "spell_transport";
   if( spell == spell_portal )
      return "spell_portal";
   if( spell == spell_regeneration )
      return "spell_regeneration";
   if( spell == spell_demonfire )
      return "spell_demonfire";
   if( spell == spell_demi )
      return "spell_demi";
   if( spell == spell_kyahu )
      return "spell_kyahu";
   if( spell == spell_ethereal_fist )
      return "spell_ethereal_fist";
   if( spell == spell_spectral_furor )
      return "spell_spectral_furor";
   if( spell == spell_hand_of_chaos )
      return "spell_hand_of_chaos";
   if( spell == spell_disruption )
      return "spell_disruption";
   if( spell == spell_sonic_resonance )
      return "spell_sonic_resonance";
   if( spell == spell_mind_wrack )
      return "spell_mind_wrack";
   if( spell == spell_mind_wrench )
      return "spell_mind_wrench";
   if( spell == spell_revive )
      return "spell_revive";
   if( spell == spell_sulfurous_spray )
      return "spell_sulfurous_spray";
   if( spell == spell_caustic_fount )
      return "spell_caustic_fount";
   if( spell == spell_acetum_primus )
      return "spell_acetum_primus";
   if( spell == spell_galvanic_whip )
      return "spell_galvanic_whip";
   if( spell == spell_magnetic_thrust )
      return "spell_magnetic_thrust";
   if( spell == spell_quantum_spike )
      return "spell_quantum_spike";
   if( spell == spell_blackflame )
      return "spell_blackflame";
   if( spell == spell_black_lightning )
      return "spell_black_lightning";
   if( spell == spell_force_choke )
      return "spell_force_choke";
   if( spell == spell_force_heal )
      return "spell_force_heal";
   if( spell == spell_force_lightning )
      return "spell_force_lightning";
   if( spell == spell_force_levitation )
      return "spell_force_levitation";
   if( spell == spell_midas_touch )
      return "spell_midas_touch";
   if( spell == spell_expurgation )
      return "spell_expurgation";
   if( spell == spell_sacral_divinity )
      return "spell_sacral_divinity";
   if( spell == spell_seluthiary )
      return "spell_seluthiary";
   if( spell == spell_purity )
      return "spell_purity";
   if( spell == spell_warp )
      return "spell_warp";
   if( spell == spell_godspell )
      return "spell_godspell";
   if( spell == spell_calltohades )
      return "spell_calltohades";
   if( spell == spell_armor )
      return "spell_armor";
   if( spell == spell_hadesarmor )
      return "spell_hadesarmor";
   if( spell == spell_calcify_flesh )
      return "spell_calcify_flesh";
   if( spell == spell_displacement )
      return "spell_displacement";
   if( spell == spell_biofeedback )
      return "spell_biofeedback";
   if( spell == spell_battleplan )
      return "spell_battleplan";
   if( spell == spell_quickness )
      return "spell_quickness";
   if( spell == spell_frenzy )
      return "spell_frenzy";
   if( spell == spell_jer )
      return "spell_jer";
   if( spell == spell_bless )
      return "spell_bless";
   if( spell == spell_protect )
      return "spell_protect";
   if( spell == spell_plague )
      return "spell_plague";
   if( spell == spell_cure_plague )
      return "spell_cure_plague";
   if( spell == spell_aquabreath )
      return "spell_aquabreath";
   if( spell == spell_stoneskin )
      return "spell_stoneskin";
   if( spell == spell_sanctuary )
      return "spell_sanctuary";
   if( spell == spell_fireshield )
      return "spell_fireshield";
   if( spell == spell_iceshield )
      return "spell_iceshield";
   if( spell == spell_shockshield )
      return "spell_shockshield";
   if( spell == spell_shield )
      return "spell_shield";
   if( spell == spell_ablution )
      return "spell_ablution";
   if( spell == spell_fly )
      return "spell_fly";
   if( spell == spell_holy )
      return "spell_holy";
   if( spell == spell_mistwalk )
      return "spell_mistwalk";
   if( spell == spell_home )
      return "spell_home";
   if( spell == spell_terrashift )
      return "spell_terrashift";

   if( spell == spell_detect_evil )
      return "spell_detect_evil";
   if( spell == spell_detect_invis )
      return "spell_detect_invis";
   if( spell == spell_detect_magic )
      return "spell_detect_magic";
   if( spell == spell_detect_hidden )
      return "spell_detect_hidden";
   if( spell == spell_infravision )
      return "spell_infravision";
   if( spell == spell_true_sight )
      return "spell_true_sight";
   if( spell == spell_detect_traps )
      return "spell_detect_traps";
   if( spell == spell_scry )
      return "spell_scry";

   if( spell == spell_cure_light )
      return "spell_cure_light";
   if( spell == spell_cure_serious )
      return "spell_cure_serious";
   if( spell == spell_cure_critical )
      return "spell_cure_critical";

   if( spell == spell_mana )
      return "spell_mana";

   if( spell == spell_comfort )
      return "spell_comfort";

   if( spell == spell_heal_i )
      return "spell_heal_i";
   if( spell == spell_heal_ii )
      return "spell_heal_ii";
   if( spell == spell_heal_iii )
      return "spell_heal_iii";
   if( spell == spell_heal_iv )
      return "spell_heal_iv";
   if( spell == spell_heal_v )
      return "spell_heal_v";
   // Songs
   if( spell == spell_kn_minne )
      return "spell_kn_minne";
   if( spell == spell_kn_minne2 )
      return "spell_kn_minne2";
   if( spell == spell_kn_minne3 )
      return "spell_kn_minne3";
   if( spell == spell_kn_minne4 )
      return "spell_kn_minne4";
   if( spell == spell_val_minuet )
      return "spell_val_minuet";
   if( spell == spell_val_minuet2 )
      return "spell_val_minuet2";
   if( spell == spell_ice_threnody )
      return "spell_ice_threnody";
   if( spell == spell_fire_threnody )
      return "spell_fire_threnody";
   if( spell == spell_shock_threnody )
      return "spell_shock_threnody";
   if( spell == spell_optic_etude )
      return "spell_optic_etude";

   if( spell == spell_null )
      return "spell_null";
   return "reserved";
}

char *skill_name( DO_FUN * skill )
{
   static char buf[64];

   if( skill == NULL )
      return "reserved";
   /*
    * //T3
    */
   if( skill == do_assassinate )
      return "do_assassinate";
   if( skill == do_aassign )
      return "do_aassign";
   if( skill == do_abjucate )
      return "do_abjucate";
   if( skill == do_addchange )
      return "do_addchange";
   if( skill == do_addlag )
      return "do_addlag";
   if( skill == do_addsanc )
      return "do_addsanc";
   if( skill == do_addsith )
      return "do_addsith";
   if( skill == do_advance )
      return "do_advance";
   if( skill == do_advhero )
      return "do_advhero";
   if( skill == do_admtalk )
      return "do_admtalk";
   if( skill == do_aexit )
      return "do_aexit";
   if( skill == do_affected )
      return "do_affected";
   if( skill == do_affstrip )
      return "do_affstrip";
   if( skill == do_afk )
      return "do_afk";
   if( skill == do_aid )
      return "do_aid";
   if( skill == do_alertness )
      return "do_alertness";
   if( skill == do_alia )
      return "do_alia";
   if( skill == do_alias )
      return "do_alias";
   if( skill == do_allow )
      return "do_allow";
   if( skill == do_ansi )
      return "do_ansi";
   if( skill == do_answer )
      return "do_answer";
   if( skill == do_aol )
      return "do_aol";
   if( skill == do_apoth )
      return "do_apoth";
   if( skill == do_apply )
      return "do_apply";
   if( skill == do_appraise )
      return "do_appraise";
   if( skill == do_areas )
      return "do_areas";
   if( skill == do_aset )
      return "do_aset";
   if( skill == do_ask )
      return "do_ask";
   if( skill == do_astat )
      return "do_astat";
   if( skill == do_at )
      return "do_at";
   if( skill == do_atobj )
      return "do_atobj";
   if( skill == do_auction )
      return "do_auction";
   if( skill == do_authorize )
      return "do_authorize";
   if( skill == do_autos )
      return "do_autos";
/*    if ( skill == do_autoloot )		return "do_autoloot";
    if ( skill == do_autosac )		return "do_autosac";
    if ( skill == do_autocoin )		return "do_autocoin"; */
   if( skill == do_avtalk )
      return "do_avtalk";
   if( skill == do_acotalk )
      return "do_acotalk";
   if( skill == do_accept )
      return "do_accept";
   if( skill == do_ahall )
      return "do_ahall";
   if( skill == do_arena )
      return "do_arena";
   if( skill == do_ascend )
      return "do_ascend";
   if( skill == do_award )
      return "do_award";
   if( skill == do_awho )
      return "do_awho";
   if( skill == do_aclist )
      return "do_aclist";
   if( skill == do_avlist )
      return "do_avlist";
   if( skill == do_bet )
      return "do_bet";
   if( skill == do_challenge )
      return "do_challenge";
   if( skill == do_chaos )
      return "do_chaos";
   if( skill == do_decline )
      return "do_decline";
   if( skill == do_backup )
      return "do_backup";
   if( skill == do_backstab )
      return "do_backstab";
   if( skill == do_balance )
      return "do_balance";
   if( skill == do_balzhur )
      return "do_balzhur";
   if( skill == do_bamfin )
      return "do_bamfin";
   if( skill == do_bamfout )
      return "do_bamfout";
   if( skill == do_ban )
      return "do_ban";
   if( skill == do_bash )
      return "do_bash";
   if( skill == do_bashdoor )
      return "do_bashdoor";
   if( skill == do_beep )
      return "do_beep";
   if( skill == do_berserk )
      return "do_berserk";
   if( skill == do_bestow )
      return "do_bestow";
   if( skill == do_bestowarea )
      return "do_bestowarea";
   if( skill == do_bio )
      return "do_bio";
   if( skill == do_bite )
      return "do_bite";
   if( skill == do_bloodlet )
      return "do_bloodlet";
   if( skill == do_boards )
      return "do_boards";
   if( skill == do_bodybag )
      return "do_bodybag";
   if( skill == do_bolt )
      return "do_bolt";
   if( skill == do_book )
      return "do_book";
   if( skill == do_bounty )
      return "do_bounty";
   if( skill == do_bountylist )
      return "do_bountylist";
   if( skill == do_bowcraft )
      return "do_bowcraft";
   if( skill == do_brandish )
      return "do_brandish";
   if( skill == do_broach )
      return "do_broach";
   if( skill == do_bset )
      return "do_bset";
   if( skill == do_bstat )
      return "do_bstat";
   if( skill == do_bug )
      return "do_bug";
   if( skill == do_bugs )
      return "do_bugs";
   if( skill == do_bsave )
      return "do_bsave";
   if( skill == do_building )
      return "do_building";
   if( skill == do_bury )
      return "do_bury";
   if( skill == do_buy )
      return "do_buy";
   if( skill == do_cast )
      return "do_cast";
   if( skill == do_cedit )
      return "do_cedit";
   if( skill == do_coe )
      return "do_coe";
   if( skill == do_channels )
      return "do_channels";
   if( skill == do_chat )
      return "do_chat";
   if( skill == do_changes )
      return "do_changes";
   if( skill == do_chareg )
      return "do_chareg";
   if( skill == do_chsave )
      return "do_chsave";
   if( skill == do_check_vnums )
      return "do_check_vnums";
   if( skill == do_circle )
      return "do_circle";
   if( skill == do_classes )
      return "do_classes";
   if( skill == do_clans )
      return "do_clans";
   if( skill == do_clantalk )
      return "do_clantalk";
   if( skill == do_climate )
      return "do_climate";
   if( skill == do_climb )
      return "do_climb";
   if( skill == do_close )
      return "do_close";
   if( skill == do_cmdtable )
      return "do_cmdtable";
   if( skill == do_cmeeting )
      return "do_cmeeting";
   if( skill == do_codetalk )
      return "do_codetalk";
   if( skill == do_colorize )
      return "do_colorize";
   if( skill == do_commands )
      return "do_commands";
   if( skill == do_comment )
      return "do_comment";
   if( skill == do_compare )
      return "do_compare";
   if( skill == do_config )
      return "do_config";
   if( skill == do_convert )
      return "do_convert";
   if( skill == do_connect )
      return "do_connect";
   if( skill == do_consider )
      return "do_consider";
   if( skill == do_cook )
      return "do_cook";
   if( skill == do_copyover )
      return "do_copyover";
   if( skill == do_copyright )
      return "do_copyright";
   if( skill == do_corpse )
      return "do_corpse";
   if( skill == do_count )
      return "do_count";
   if( skill == do_council_induct )
      return "do_council_induct";
   if( skill == do_council_outcast )
      return "do_council_outcast";
   if( skill == do_councils )
      return "do_councils";
   if( skill == do_counciltalk )
      return "do_counciltalk";
   if( skill == do_craft )
      return "do_craft";
   if( skill == do_crank )
      return "do_crank";
   if( skill == do_credits )
      return "do_credits";
   if( skill == do_cribsheet )
      return "do_cribsheet";
   if( skill == do_cset )
      return "do_cset";
   if( skill == do_deaf )
      return "do_deaf";
   if( skill == do_deities )
      return "do_deities";
   if( skill == do_delay )
      return "do_delay";
   if( skill == do_delete )
      return "do_delete";
   if( skill == do_demote )
      return "do_demote";
   if( skill == do_deny )
      return "do_deny";
   if( skill == do_deposit )
      return "do_deposit";
   if( skill == do_description )
      return "do_description";
   if( skill == do_destro )
      return "do_destro";
   if( skill == do_destroy )
      return "do_destroy";
   if( skill == do_detrap )
      return "do_detrap";
   if( skill == do_devote )
      return "do_devote";
   if( skill == do_dice )
      return "do_dice";
   if( skill == do_dig )
      return "do_dig";
   if( skill == do_disarm )
      return "do_disarm";
   if( skill == do_disconnect )
      return "do_disconnect";
   if( skill == do_dislodge )
      return "do_dislodge";
   if( skill == do_dismiss )
      return "do_dismiss";
   if( skill == do_dismount )
      return "do_dismount";
   if( skill == do_divorce )
      return "do_divorce";
   if( skill == do_dmesg )
      return "do_dmesg";
   if( skill == do_dnd )
      return "do_dnd";
   if( skill == do_donate )
      return "do_donate";
   if( skill == do_doublexp )
      return "do_doublexp";
   if( skill == do_down )
      return "do_down";
   if( skill == do_drag )
      return "do_drag";
   if( skill == do_draw )
      return "do_draw";
   if( skill == do_drink )
      return "do_drink";
   if( skill == do_drop )
      return "do_drop";
   if( skill == do_dual )
      return "do_dual";
   if( skill == do_dump )
      return "do_dump";
   if( skill == do_diagnose )
      return "do_diagnose";
   if( skill == do_east )
      return "do_east";
   if( skill == do_eat )
      return "do_eat";
   if( skill == do_ech )
      return "do_ech";
   if( skill == do_echo )
      return "do_echo";
   if( skill == do_email )
      return "do_email";
   if( skill == do_emote )
      return "do_emote";
   if( skill == do_empty )
      return "do_empty";
   if( skill == do_enter )
      return "do_enter";
   if( skill == do_equipment )
      return "do_equipment";
   if( skill == do_eqreg )
      return "do_eqreg";
   if( skill == do_examine )
      return "do_examine";
   if( skill == do_exits )
      return "do_exits";
   if( skill == do_expset )
      return "do_expset";
   if( skill == do_feed )
      return "do_feed";
   if( skill == do_ferralclaw )
      return "do_ferralclaw";
   if( skill == do_fill )
      return "do_fill";
   if( skill == do_findnote )
      return "do_findnote";
   if( skill == do_finger )
      return "do_finger";
   if( skill == do_fxp )
      return "do_fxp";
   if( skill == do_fhelp )
      return "do_fhelp";
   if( skill == do_fire )
      return "do_fire";
   if( skill == do_fixchar )
      return "do_fixchar";
   if( skill == do_fixmobs )
      return "do_fixmobs";
   if( skill == do_fixed )
      return "do_fixed";
   if( skill == do_flame )
      return "do_flame";
   if( skill == do_flee )
      return "do_flee";
   if( skill == do_fletch )
      return "do_fletch";
   if( skill == do_flock )
      return "do_flock";
   if( skill == do_flocko )
      return "do_flocko";
   if( skill == do_flutecraft )
      return "do_flutecraft";
   if( skill == do_foldarea )
      return "do_foldarea";
   if( skill == do_follow )
      return "do_follow";
   if( skill == do_for )
      return "do_for";
   if( skill == do_forge )
      return "do_forge";
   if( skill == do_forget )
      return "do_forget";
   if( skill == do_force )
      return "do_force";
   if( skill == do_forceclose )
      return "do_forceclose";
   if( skill == do_force_throw )
      return "do_force_throw";
   if( skill == do_force_push )
      return "do_force_push";
   if( skill == do_form_password )
      return "do_form_password";
   if( skill == do_fquit )
      return "do_fquit";
   if( skill == do_free_vnums )
      return "do_free_vnums";
   if( skill == do_freeze )
      return "do_freeze";
   if( skill == do_fshow )
      return "do_fshow";
   if( skill == do_garb )
      return "do_garb";
   if( skill == do_gaso )
      return "do_gaso";
   if( skill == do_get )
      return "do_get";
   if( skill == do_gfighting )
      return "do_gfighting";
   if( skill == do_give )
      return "do_give";
   if( skill == do_glance )
      return "do_glance";
   if( skill == do_global_boards )
      return "do_global_boards";
   if( skill == do_global_note )
      return "do_global_note";
   if( skill == do_gloryset )
      return "do_gloryset";
   if( skill == do_glorytrade )
      return "do_glorytrade";
   if( skill == do_gold )
      return "do_gold";
   if( skill == do_goto )
      return "do_goto";
   if( skill == do_gouge )
      return "do_gouge";
   if( skill == do_group )
      return "do_group";
   if( skill == do_gratz )
      return "do_gratz";
   if( skill == do_grub )
      return "do_grub";
   if( skill == do_gtell )
      return "do_gtell";
   if( skill == do_guilds )
      return "do_guilds";
   if( skill == do_guildtalk )
      return "do_guildtalk";
   if( skill == do_gwhere )
      return "do_gwhere";
   if( skill == do_hadnd )
      return "do_hadnd";
   if( skill == do_halfexp )
      return "do_halfexp";
   if( skill == do_hbuild )
      return "do_hbuild";
   if( skill == do_hedit )
      return "do_hedit";
   if( skill == do_hell )
      return "do_hell";
   if( skill == do_hell2 )
      return "do_hell2";
   if( skill == do_hell3 )
      return "do_hell3";
   if( skill == do_helllist )
      return "do_helllist";
   if( skill == do_help )
      return "do_help";
   if( skill == do_heighten )
      return "do_heighten";
   if( skill == do_hide )
      return "do_hide";
   if( skill == do_highapoth )
      return "do_highapoth";
   if( skill == do_highdam )
      return "do_highdam";
   if( skill == do_hitall )
      return "do_hitall";
   if( skill == do_htalk )
      return "do_htalk";
   if( skill == do_hl )
      return "do_hl";
   if( skill == do_hlist )
      return "do_hlist";
   if( skill == do_helplist )
      return "do_helplist";
   if( skill == do_holylight )
      return "do_holylight";
   if( skill == do_homebound )
      return "do_homebound";
   if( skill == do_homepage )
      return "do_homepage";
   if( skill == do_hset )
      return "do_hset";
   if( skill == do_hstat )
      return "do_hstat";
   if( skill == do_iaw )
      return "do_iaw";
   if( skill == do_ic )
      return "do_ic";
   if( skill == do_icq )
      return "do_icq";
   if( skill == do_idle )
      return "do_idle";
   if( skill == do_ide )
      return "do_ide";
   if( skill == do_idea )
      return "do_idea";
   if( skill == do_ignore )
      return "do_ignore";
   if( skill == do_immortalize )
      return "do_immortalize";
   if( skill == do_ilog )
      return "do_ilog";
   if( skill == do_immeq )
      return "do_immeq";
   if( skill == do_immhost )
      return "do_immhost";
   if( skill == do_immlogtime )
      return "do_immlogtime";
   if( skill == do_immtalk )
      return "do_immtalk";
   if( skill == do_immwhere )
      return "do_immwhere";
   if( skill == do_imm_morph )
      return "do_imm_morph";
   if( skill == do_imm_unmorph )
      return "do_imm_unmorph";
   if( skill == do_impmsg )
      return "do_impmsg";
   if( skill == do_induct )
      return "do_induct";
   if( skill == do_info )
      return "do_info";
   if( skill == do_installarea )
      return "do_installarea";
   if( skill == do_instaroom )
      return "do_instaroom";
   if( skill == do_instaworld )
      return "do_instaworld";
   if( skill == do_instazone )
      return "do_instazone";
   if( skill == do_inventory )
      return "do_inventory";
   if( skill == do_invis )
      return "do_invis";
   if( skill == do_ipcompare )
      return "do_ipcompare";
   if( skill == do_junk )
      return "do_junk";
   if( skill == do_kar )
      return "do_kar";
   if( skill == do_kaura )
      return "do_kaura";
   if( skill == do_keyitems )
      return "do_keyitems";
   if( skill == do_khistory )
      return "do_khistory";
   if( skill == do_kiamstat )
      return "do_kiamstat";
   if( skill == do_kick )
      return "do_kick";
   if( skill == do_kill )
      return "do_kill";
   if( skill == do_languages )
      return "do_languages";
   if( skill == do_land )
      return "do_land";
   if( skill == do_last )
      return "do_last";
   if( skill == do_laws )
      return "do_laws";
   if( skill == do_lcmd )
      return "do_lcmd";
   if( skill == do_leave )
      return "do_leave";
   if( skill == do_level )
      return "do_level";
   if( skill == do_light )
      return "do_light";
   if( skill == do_list )
      return "do_list";
   if( skill == do_litterbug )
      return "do_litterbug";
   if( skill == do_loadarea )
      return "do_loadarea";
   if( skill == do_loadup )
      return "do_loadup";
   if( skill == do_lock )
      return "do_lock";
   if( skill == do_log )
      return "do_log";
   if( skill == do_look )
      return "do_look";
   if( skill == do_lookmap )
      return "do_lookmap";
   if( skill == do_low_purge )
      return "do_low_purge";
   if( skill == do_mailroom )
      return "do_mailroom";
   if( skill == do_make )
      return "do_make";
   if( skill == do_makepotion )
      return "do_makepotion";
   if( skill == do_makeboard )
      return "do_makeboard";
   if( skill == do_makeclan )
      return "do_makeclan";
   if( skill == do_makectime )
      return "do_makectime";
   if( skill == do_makecouncil )
      return "do_makecouncil";
   if( skill == do_makedeity )
      return "do_makedeity";
   if( skill == do_makerepair )
      return "do_makerepair";
   if( skill == do_makeshop )
      return "do_makeshop";
   if( skill == do_makewizlist )
      return "do_makewizlist";
   if( skill == do_mapout )
      return "do_mapout";
   if( skill == do_marry )
      return "do_marry";
   if( skill == do_massign )
      return "do_massign";
   if( skill == do_mcreate )
      return "do_mcreate";
   if( skill == do_mdelete )
      return "do_mdelete";
   if( skill == do_meeting )
      return "do_meeting";
   if( skill == do_memory )
      return "do_memory";
   if( skill == do_mfind )
      return "do_mfind";
   if( skill == do_midscore )
      return "do_midscore";
   if( skill == do_minvoke )
      return "do_minvoke";
   if( skill == do_mlist )
      return "do_mlist";
   if( skill == do_monitor )
      return "do_monitor";
   //if ( skill == do_morph)  return "do_morph";
   //if ( skill == do_morph_char)   return "do_morph_char";
   if( skill == do_morphcreate )
      return "do_morphcreate";
   if( skill == do_morphdestroy )
      return "do_morphdestroy";
   if( skill == do_morphset )
      return "do_morphset";
   if( skill == do_morphstat )
      return "do_morphstat";
   if( skill == do_mortality )
      return "do_mortality";
   if( skill == do_mortalize )
      return "do_mortalize";
   if( skill == do_motd )
      return "do_motd";
   if( skill == do_mount )
      return "do_mount";
   if( skill == do_move_hidden )
      return "do_move_hidden";
   if( skill == do_movein )
      return "do_movein";
   if( skill == do_moveout )
      return "do_moveout";
   if( skill == do_mp_close_passage )
      return "do_mp_close_passage";
   if( skill == do_mp_damage )
      return "do_mp_damage";
   if( skill == do_mp_deposit )
      return "do_mp_deposit";
   if( skill == do_mp_fill_in )
      return "do_mp_fill_in";
   if( skill == do_mp_log )
      return "do_mp_log";
   if( skill == do_mp_open_passage )
      return "do_mp_open_passage";
   if( skill == do_mp_practice )
      return "do_mp_practice";
   if( skill == do_mp_restore )
      return "do_mp_restore";
   if( skill == do_mp_slay )
      return "do_mp_slay";
   if( skill == do_mp_withdraw )
      return "do_mp_withdraw";
   if( skill == do_mpadvance )
      return "do_mpadvance";
   if( skill == do_mpapply )
      return "do_mpapply";
   if( skill == do_mpapplyb )
      return "do_mpapplyb";
   if( skill == do_mpasound )
      return "do_mpasound";
   if( skill == do_mpasupress )
      return "do_mpasupress";
   if( skill == do_mpat )
      return "do_mpat";
   if( skill == do_mpbodybag )
      return "do_mpbodybag";
   if( skill == do_mpcopy )
      return "do_mpcopy";
   if( skill == do_mpdelay )
      return "do_mpdelay";
   if( skill == do_mpdream )
      return "do_mpdream";
   if( skill == do_mpecho )
      return "do_mpecho";
   if( skill == do_mpechoaround )
      return "do_mpechoaround";
   if( skill == do_mpechoat )
      return "do_mpechoat";
   if( skill == do_mpechozone )
      return "do_mpechozone";
   if( skill == do_mpedit )
      return "do_mpedit";
   if( skill == do_mpfavor )
      return "do_mpfavor";
   if( skill == do_mpforce )
      return "do_mpforce";
   if( skill == do_mpgoto )
      return "do_mpgoto";
   if( skill == do_mpinvis )
      return "do_mpinvis";
   if( skill == do_mpjunk )
      return "do_mpjunk";
   if( skill == do_mpkill )
      return "do_mpkill";
   if( skill == do_mpmload )
      return "do_mpmload";
   if( skill == do_mpmorph )
      return "do_mpmorph";
   if( skill == do_mpmset )
      return "do_mpmset";
   if( skill == do_mpmusic )
      return "do_mpmusic";
   if( skill == do_mpmusicaround )
      return "do_mpmusicaround";
   if( skill == do_mpmusicat )
      return "do_mpmusicat";
   if( skill == do_mpnothing )
      return "do_mpnothing";
   if( skill == do_mpnuisance )
      return "do_mpnuisance";
   if( skill == do_mpoload )
      return "do_mpoload";
   if( skill == do_mposet )
      return "do_mposet";
   if( skill == do_mppardon )
      return "do_mppardon";
   if( skill == do_mppeace )
      return "do_mppeace";
   if( skill == do_mppkset )
      return "do_mppkset";
   if( skill == do_mppquit )
      return "do_mpqquit";
   if( skill == do_mppurge )
      return "do_mppurge";
   if( skill == do_mpscatter )
      return "do_mpscatter";
   if( skill == do_mpsound )
      return "do_mpsound";
   if( skill == do_mpsoundaround )
      return "do_mpsoundaround";
   if( skill == do_mpsoundat )
      return "do_mpsoundat";
   if( skill == do_mpstat )
      return "do_mpstat";
   if( skill == do_mptransfer )
      return "do_mptransfer";
   if( skill == do_mpunmorph )
      return "do_mpunmorph";
   if( skill == do_mpunnuisance )
      return "do_mpunnuisance";
   if( skill == do_mpfind )
      return "do_mpfind";
   if( skill == do_mrange )
      return "do_mrange";
   if( skill == do_mset )
      return "do_mset";
   if( skill == do_mstat )
      return "do_mstat";
   if( skill == do_mudschool )
      return "do_mudschool";
   if( skill == do_mudstat )
      return "do_mudstat";
   if( skill == do_murde )
      return "do_murde";
   if( skill == do_murder )
      return "do_murder";
   if( skill == do_muse )
      return "do_muse";
   if( skill == do_music )
      return "do_music";
   if( skill == do_mwhere )
      return "do_mwhere";
   if( skill == do_name )
      return "do_name";
   if( skill == do_nch )
      return "do_nch";
   if( skill == do_neotalk )
      return "do_neotalk";
   if( skill == do_newaff )
      return "do_newaff";
   if( skill == do_newbiechat )
      return "do_newbiechat";
   if( skill == do_newbieset )
      return "do_newbieset";
   if( skill == do_newpass )
      return "do_newpass";
   if( skill == do_newpl )
      return "do_newpl";
   if( skill == do_newpl2 )
      return "do_newpl2";
   if( skill == do_news )
      return "do_news";
   if( skill == do_newzones )
      return "do_newzones";
   if( skill == do_nohelps )
      return "do_nohelps";
   if( skill == do_noemote )
      return "do_noemote";
   if( skill == do_noresolve )
      return "do_noresolve";
   if( skill == do_north )
      return "do_north";
   if( skill == do_northeast )
      return "do_northeast";
   if( skill == do_nelist )
      return "do_nelist";
   if( skill == do_nstalk )
      return "do_nstalk";
   if( skill == do_northwest )
      return "do_northwest";
   if( skill == do_notell )
      return "do_notell";
   if( skill == do_notitle )
      return "do_notitle";
   if( skill == do_noteroom )
      return "do_noteroom";
   if( skill == do_nrange )
      return "do_nrange";
   if( skill == do_nuisance )
      return "do_nuisance";
   if( skill == do_numlock )
      return "do_numlock";
   if( skill == do_oassign )
      return "do_oassign";
   if( skill == do_ocreate )
      return "do_ocreate";
   if( skill == do_odelete )
      return "do_odelete";
   if( skill == do_ofind )
      return "do_ofind";
   if( skill == do_ofha )
      return "do_ofha";
   if( skill == do_ogrub )
      return "do_ogrub";
   if( skill == do_oinvoke )
      return "do_oinvoke";
   if( skill == do_offer )
      return "do_offer";
   if( skill == do_olist )
      return "do_olist";
   if( skill == do_oldchanges )
      return "do_oldchanges";
   if( skill == do_onedown )
      return "do_onedown";
   if( skill == do_oneup )
      return "do_oneup";
   if( skill == do_opcopy )
      return "do_opcopy";
   if( skill == do_opedit )
      return "do_opedit";
   if( skill == do_open )
      return "do_open";
   if( skill == do_opfind )
      return "do_opfind";
   if( skill == do_opstat )
      return "do_opstat";
   if( skill == do_orange )
      return "do_orange";
   if( skill == do_order )
      return "do_order";
   if( skill == do_orders )
      return "do_orders";
   if( skill == do_ordertalk )
      return "do_ordertalk";
   if( skill == do_osan )
      return "do_osan";
   if( skill == do_oset )
      return "do_oset";
   if( skill == do_ostat )
      return "do_ostat";
   if( skill == do_outcast )
      return "do_outcast";
   if( skill == do_owhere )
      return "do_owhere";
   if( skill == do_pager )
      return "do_pager";
   if( skill == do_pardon )
      return "do_pardon";
   if( skill == do_password )
      return "do_password";
   if( skill == do_pcc )
      return "do_pcc";
   if( skill == do_pcrename )
      return "do_pcrename";
   if( skill == do_peace )
      return "do_peace";
   if( skill == do_pfiles )
      return "do_pfiles";
   if( skill == do_pick )
      return "do_pick";
   if( skill == do_plock )
      return "do_plock";
   if( skill == do_plocko )
      return "do_plocko";
   if( skill == do_poison_weapon )
      return "do_poison_weapon";
   if( skill == do_gplague )
      return "do_gplague";
   if( skill == do_pplague )
      return "do_pplague";
   if( skill == do_pqui )
      return "do_pqui";
   if( skill == do_pquit )
      return "do_pquit";
   if( skill == do_practice )
      return "do_practice";
   if( skill == do_pracset )
      return "do_pracset";
   if( skill == do_pray )
      return "do_pray";
   if( skill == do_prepreboot )
      return "do_prepreboot";
   if( skill == do_pretitle )
      return "do_pretitle";
   if( skill == do_privacy )
      return "do_privacy";
   if( skill == do_profane )
      return "do_profane";
   if( skill == do_promote )
      return "do_promote";
   if( skill == do_prompt )
      return "do_prompt";
   if( skill == do_project )
      return "do_project";
   if( skill == do_pscore )
      return "do_pscore";
   if( skill == do_pull )
      return "do_pull";
   if( skill == do_punch )
      return "do_punch";
   if( skill == do_purge )
      return "do_purge";
   if( skill == do_push )
      return "do_push";
   if( skill == do_put )
      return "do_put";
   if( skill == do_qboost )
      return "do_qboost";
   if( skill == do_qpset )
      return "do_qpset";
   if( skill == do_qpstat )
      return "do_qpstat";
   if( skill == do_quaff )
      return "do_quaff";
   if( skill == do_quest )
      return "do_quest";
   if( skill == do_qui )
      return "do_qui";
   if( skill == do_quit )
      return "do_quit";
   if( skill == do_quiver )
      return "do_quiver";
   if( skill == do_quota )
      return "do_quota";
   if( skill == do_qwest )
      return "do_qwest";
   if( skill == do_races )
      return "do_races";
   if( skill == do_racial )
      return "do_racial";
   if( skill == do_racetalk )
      return "do_racetalk";
   if( skill == do_rapsheet )
      return "do_rapsheet";
   if( skill == do_rap )
      return "do_rap";
   if( skill == do_rapon )
      return "do_rapon";
   if( skill == do_rassign )
      return "do_rassign";
   if( skill == do_rat )
      return "do_rat";
   if( skill == do_rdelete )
      return "do_rdelete";
   if( skill == do_reboo )
      return "do_reboo";
   if( skill == do_reboot )
      return "do_reboot";
   if( skill == do_rebuild )
      return "do_rebuild";
   if( skill == do_recall )
      return "do_recall";
   if( skill == do_recho )
      return "do_recho";
   if( skill == do_recite )
      return "do_recite";
   if( skill == do_redit )
      return "do_redit";
   if( skill == do_register )
      return "do_register";
   if( skill == do_reglist )
      return "do_reglist";
   if( skill == do_regoto )
      return "do_regoto";
   if( skill == do_reimb )
      return "do_reimb";
   if( skill == do_remove )
      return "do_remove";
   if( skill == do_remains )
      return "do_remains";
   if( skill == do_rent )
      return "do_rent";
   if( skill == do_repair )
      return "do_repair";
   if( skill == do_repairset )
      return "do_repairset";
   if( skill == do_repairshops )
      return "do_repairshops";
   if( skill == do_repairstat )
      return "do_repairstat";
   if( skill == do_repeat )
      return "do_repeat";
   if( skill == do_reply )
      return "do_reply";
   if( skill == do_report )
      return "do_report";
   if( skill == do_rescue )
      return "do_rescue";
   if( skill == do_reserve )
      return "do_reserve";
   if( skill == do_reset )
      return "do_reset";
   if( skill == do_rest )
      return "do_rest";
   if( skill == do_rrestore )
      return "do_rrestore";
   if( skill == do_restore )
      return "do_restore";
   if( skill == do_restoretime )
      return "do_restoretime";
   if( skill == do_restrict )
      return "do_restrict";
   if( skill == do_retell )
      return "do_retell";
   if( skill == do_retire )
      return "do_retire";
   if( skill == do_retran )
      return "do_retran";
   if( skill == do_return )
      return "do_return";
   if( skill == do_rework )
      return "do_rework";
   if( skill == do_rgrub )
      return "do_rgrub";
   if( skill == do_rip )
      return "do_rip";
   if( skill == do_rlist )
      return "do_rlist";
   if( skill == do_rolldice )
      return "do_rolldice";
   if( skill == do_rolldie )
      return "do_rolldie";
   if( skill == do_rpedit )
      return "do_rpedit";
   if( skill == do_rpemote )
      return "do_rpemote";
   if( skill == do_rpfind )
      return "do_rpfind";
   if( skill == do_rpstat )
      return "do_rpstat";
   if( skill == do_rreset )
      return "do_rreset";
   if( skill == do_rset )
      return "do_rset";
   if( skill == do_rstat )
      return "do_rstat";
   if( skill == do_sacrifice )
      return "do_sacrifice";
   if( skill == do_safety )
      return "do_safety";
   if( skill == do_sanctuary )
      return "do_sanctuary";
   if( skill == do_save )
      return "do_save";
   if( skill == do_saveall )
      return "do_saveall";
   if( skill == do_savearea )
      return "do_savearea";
   if( skill == do_say )
      return "do_say";
   if( skill == do_say_to_char )
      return "do_say_to_char";
   if( skill == do_scan )
      return "do_scan";
   if( skill == do_scatter )
      return "do_scatter";
   if( skill == do_score )
      return "do_score";
   if( skill == do_scoresheet )
      return "do_scoresheet";
   if( skill == do_scribe )
      return "do_scribe";
   if( skill == do_search )
      return "do_search";
   if( skill == do_sedit )
      return "do_sedit";
   if( skill == do_seedate )
      return "do_seedate";
   if( skill == do_sell )
      return "do_sell";
   if( skill == do_set )
      return "do_set";
   if( skill == do_set_boot_time )
      return "do_set_boot_time";
   if( skill == do_setclan )
      return "do_setclan";
   if( skill == do_setclass )
      return "do_setclass";
   if( skill == do_setcouncil )
      return "do_setcouncil";
   if( skill == do_setdeity )
      return "do_setdeity";
   if( skill == do_setrace )
      return "do_setrace";
   if( skill == do_setweather )
      return "do_setweather";
   if( skill == do_sharpen )
      return "do_sharpen";
   if( skill == do_shapeshift )
      return "do_shapeshift";
   if( skill == do_shops )
      return "do_shops";
   if( skill == do_shopset )
      return "do_shopset";
   if( skill == do_shopstat )
      return "do_shopstat";
   if( skill == do_shortscore )
      return "do_shortscore";
   if( skill == do_shout )
      return "do_shout";
   if( skill == do_shove )
      return "do_shove";
   if( skill == do_showban )
      return "do_showban";
   if( skill == do_showclan )
      return "do_showclan";
   if( skill == do_showclass )
      return "do_showclass";
   if( skill == do_showcouncil )
      return "do_showcouncil";
   if( skill == do_showdeity )
      return "do_showdeity";
   if( skill == do_showrace )
      return "do_showrace";
   if( skill == do_showweather )
      return "do_showweather";
   if( skill == do_shutdow )
      return "do_shutdow";
   if( skill == do_shutdown )
      return "do_shutdown";
   if( skill == do_silence )
      return "do_silence";
   if( skill == do_sic )
      return "do_sic";
   if( skill == do_sit )
      return "do_sit";
   if( skill == do_skin )
      return "do_skin";
   if( skill == do_sla )
      return "do_sla";
   if( skill == do_slay )
      return "do_slay";
   if( skill == do_slearn )
      return "do_slearn";
   if( skill == do_sleep )
      return "do_sleep";
   if( skill == do_slice )
      return "do_slice";
   if( skill == do_slist )
      return "do_slist";
   if( skill == do_sslist )
      return "do_sslist";
   if( skill == do_slock )
      return "do_slock";
   if( skill == do_slocko )
      return "do_slocko";
   if( skill == do_slookup )
      return "do_slookup";
   if( skill == do_smeeting )
      return "do_smeeting";
   if( skill == do_smoke )
      return "do_smoke";
   if( skill == do_snoop )
      return "do_snoop";
   if( skill == do_socials )
      return "do_socials";
   if( skill == do_south )
      return "do_south";
   if( skill == do_southeast )
      return "do_southeast";
   if( skill == do_southwest )
      return "do_southwest";
   if( skill == do_speak )
      return "do_speak";
   if( skill == do_spousetalk )
      return "do_spousetalk";
   if( skill == do_spells )
      return "do_spells";
   if( skill == do_split )
      return "do_split";
   if( skill == do_ssang )
      return "do_ssang";
   if( skill == do_sset )
      return "do_sset";
   if( skill == do_stand )
      return "do_stand";
   if( skill == do_stat )
      return "do_stat";
   if( skill == do_statboost )
      return "do_statboost";
   if( skill == do_statreport )
      return "do_statreport";
   if( skill == do_statshield )
      return "do_statshield";
   if( skill == do_steal )
      return "do_steal";
   if( skill == do_strew )
      return "do_strew";
   if( skill == do_strip )
      return "do_strip";
   if( skill == do_stun )
      return "do_stun";
   if( skill == do_style )
      return "do_style";
   if( skill == do_switch )
      return "do_switch";
   if( skill == do_showlayers )
      return "do_showlayers";
   if( skill == do_tachi_kashi )
      return "do_tachi_kashi";
   if( skill == do_tachi_jinpu )
      return "do_tachi_jinpu";
   if( skill == do_tachi_haboku )
      return "do_tachi_haboku";
   if( skill == do_tachi_gekko )
      return "do_tachi_gekko";
   if( skill == do_tachi_koki )
      return "do_tachi_koki";
   if( skill == do_tachi_enpi )
      return "do_tachi_enpi";
   if( skill == do_tachi_kagero )
      return "do_tachi_kagero";
   if( skill == do_tamp )
      return "do_tamp";
   if( skill == do_tail )
      return "do_tail";
   if( skill == do_tap )
      return "do_tap";
   if( skill == do_tier )
      return "do_tier";
   if( skill == do_tell )
      return "do_tell";
   if( skill == do_think )
      return "do_think";
   if( skill == do_time )
      return "do_time";
   if( skill == do_timecmd )
      return "do_timecmd";
   if( skill == do_title )
      return "do_title";
   if( skill == do_tfind )
      return "do_tfind";
   if( skill == do_tlock )
      return "do_tlock";
   if( skill == do_tlocko )
      return "do_tlocko";
   if( skill == do_track )
      return "do_track";
   if( skill == do_throw )
      return "do_throw";
   if( skill == do_ttrack )
      return "do_ttrack";
   if( skill == do_train )
      return "do_train";
   if( skill == do_traffic )
      return "do_traffic";
   if( skill == do_transfer )
      return "do_transfer";
   if( skill == do_trust )
      return "do_trust";
   if( skill == do_tune )
      return "do_tune";
   if( skill == do_typo )
      return "do_typo";
   if( skill == do_unalias )
      return "do_unalias";
   if( skill == do_unbolt )
      return "do_unbolt";
   if( skill == do_undeny )
      return "do_undeny";
   if( skill == do_unfoldarea )
      return "do_unfoldarea";
   if( skill == do_unhell )
      return "do_unhell";
   if( skill == do_unhell2 )
      return "do_unhell2";
   if( skill == do_unhell3 )
      return "do_unhell3";
   if( skill == do_unlock )
      return "do_unlock";
   //if ( skill == do_unmorph)   return "do_unmorph";
   //if ( skill == do_unmorph_char)    return "do_unmorph_char";
   if( skill == do_unnuisance )
      return "do_unnuisance";
   if( skill == do_unrestore )
      return "do_unrestore";
   if( skill == do_unsilence )
      return "do_unsilence";
   if( skill == do_unshift )
      return "do_unshift";
   if( skill == do_up )
      return "do_up";
   if( skill == do_urna )
      return "do_urna";
   if( skill == do_users )
      return "do_users";
   if( skill == do_value )
      return "do_value";
   if( skill == do_vassign )
      return "do_vassign";
   if( skill == do_version )
      return "do_version";
   if( skill == do_victories )
      return "do_victories";
   if( skill == do_visible )
      return "do_visible";
   if( skill == do_vnums )
      return "do_vnums";
   if( skill == do_vsearch )
      return "do_vsearch";
   if( skill == do_wake )
      return "do_wake";
   if( skill == do_wartalk )
      return "do_wartalk";
   if( skill == do_warn )
      return "do_warn";
   if( skill == do_watch )
      return "do_watch";
   if( skill == do_wear )
      return "do_wear";
   if( skill == do_weather )
      return "do_weather";
   if( skill == do_west )
      return "do_west";
   if( skill == do_where )
      return "do_where";
   if( skill == do_whisper )
      return "do_whisper";
   if( skill == do_who )
      return "do_who";
   if( skill == do_whois )
      return "do_whois";
   if( skill == do_wimpy )
      return "do_wimpy";
   if( skill == do_withdraw )
      return "do_withdraw";
   if( skill == do_wizhelp )
      return "do_wizhelp";
   if( skill == do_wizinfo )
      return "do_wizinfo";
   if( skill == do_wizlist )
      return "do_wizlist";
   if( skill == do_wizlock )
      return "do_wizlock";
   if( skill == do_worth )
      return "do_worth";
   if( skill == do_xsedit )
      return "do_xsedit";
   if( skill == do_xsocials )
      return "do_xsocials";
   if( skill == do_yahoo )
      return "do_yahoo";
   if( skill == do_yell )
      return "do_yell";
   if( skill == do_zap )
      return "do_zap";
   if( skill == do_zones )
      return "do_zones";
   /*
    * //T4
    */
   sprintf( buf, "(%p)", skill );
   return buf;
}
