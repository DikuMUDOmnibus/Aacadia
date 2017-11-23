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
/*					MUD Constants module			                    */
/************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include "acadia.h"


#define AM 95
#define AC 95
#define AT 85
#define AW 85
#define AV 95
#define AD 95
#define AR 90
#define AA 95

char *const npc_race[MAX_NPC_RACE] = {
   "human", "elf", "dwarf", "halfling", "pixie", "vampire", "half-ogre",
   "half-orc", "half-troll", "half-elf", "gith", "drow", "sea-elf",
   "lizardman", "gnome", "denizen", "ra", "uri", "demon", "troll",
   "ant", "ape", "baboon", "bat", "bear", "bee",
   "beetle", "boar", "bugbear", "cat", "dog", "dragon", "ferret", "fly",
   "gargoyle", "gelatin", "ghoul", "gnoll", "gnome", "goblin", "golem",
   "gorgon", "harpy", "hobgoblin", "kobold", "lizardman", "locust",
   "lycanthrope", "minotaur", "mold", "mule", "neanderthal", "ooze", "orc",
   "rat", "rustmonster", "shadow", "shapeshifter", "shrew", "shrieker",
   "skeleton", "slime", "snake", "spider", "stirge", "thoul", "troglodyte",
   "undead", "wight", "wolf", "worm", "zombie", "bovine", "canine", "feline",
   "porcine", "mammal", "rodent", "bird", "reptile", "amphibian", "fish",
   "crustacean", "insect", "spirit", "magical", "horse", "animal", "humanoid",
   "monster", "god", "brewergnome", "feral", "fleshcrawler", "hag", "brinehag",
   "hamadryad", "harpy", "mistwalker", "narleth", "slarecian", "spider-eye-goblin",
   "slarecian-muse", "shadowman", "charfiend", "ent", "vile-bush", "vine", "wabbit",
   "wyrm", "abeit"
};


char *const npc_class[MAX_NPC_CLASS] = {
   "mage", "cleric", "thief", "warrior", "vampire", "warwizard", "jedika",
   "werewolf", "ranger", "teadien", "barbarian", "paladin", "sorcerer", "sorcereres", "nephandi",
   "savage", "pirate", "prince", "princess", "elementalist", "pc19",
   "baker", "butcher", "blacksmith", "mayor", "king", "queen"
};



#if 0
const struct at_color_type at_color_table[AT_MAXCOLOR] = { {"plain", AT_GREY},
{"action", AT_GREY},
{"say", AT_LBLUE},
{"gossip", AT_LBLUE},
{"yell", AT_WHITE},
{"tell", AT_WHITE},
{"whisper", AT_WHITE},
{"hit", AT_WHITE},
{"hitme", AT_YELLOW},
{"immortal", AT_YELLOW},
{"hurt", AT_RED},
{"falling", AT_WHITE},
{"danger", AT_RED},
{"magic", AT_BLUE},
{"consider", AT_GREY},
{"report", AT_GREY},
{"poison", AT_GREEN},
{"social", AT_CYAN},
{"dying", AT_YELLOW},
{"dead", AT_RED},
{"skill", AT_GREEN},
{"carnage", AT_BLOOD},
{"damage", AT_WHITE},
{"flee", AT_YELLOW},
{"roomname", AT_WHITE},
{"roomdesc", AT_YELLOW},
{"object", AT_GREEN},
{"person", AT_PINK},
{"list", AT_BLUE},
{"bye", AT_GREEN},
{"gold", AT_YELLOW},
{"gtell", AT_BLUE},
{"note", AT_GREEN},
{"hungry", AT_ORANGE},
{"thirsty", AT_BLUE},
{"fire", AT_RED},
{"sober", AT_WHITE},
{"wearoff", AT_YELLOW},
{"exits", AT_WHITE},
{"score", AT_LBLUE},
{"reset", AT_DGREEN},
{"log", AT_PURPLE},
{"diemsg", AT_WHITE},
{"wartalk", AT_RED},
{"racetalk", AT_DGREEN},
{"ignore", AT_GREEN},
{"divider", AT_PLAIN},
{"morph", AT_GREY},
};
#endif


const struct str_app_type str_app[45] = {
   {-5, -4, 0, 0},
   {-5, -4, 3, 1},
   {-3, -2, 3, 2},
   {-3, -1, 10, 3},
   {-2, -1, 25, 4},
   {-2, -1, 55, 5},
   {-1, 0, 80, 6},
   {-1, 0, 90, 7},
   {0, 0, 100, 8},
   {0, 0, 140, 9},
   {0, 0, 175, 10},
   {0, 0, 200, 11},
   {0, 0, 250, 12},
   {0, 0, 300, 13},
   {0, 1, 340, 14},
   {1, 1, 370, 15},
   {1, 2, 400, 16},
   {2, 3, 450, 22},
   {2, 4, 500, 25},
   {3, 5, 550, 30},
   {3, 6, 600, 35},
   {4, 7, 650, 40},
   {5, 7, 700, 45},
   {6, 8, 750, 50},
   {8, 10, 800, 55},
   {10, 12, 850, 60},
   {10, 12, 900, 60},
   {10, 12, 940, 60},
   {10, 12, 970, 60},
   {10, 12, 999, 60},
   {10, 12, 999, 60},
   {10, 12, 999, 60},
   {10, 12, 999, 60},
   {10, 12, 999, 60},
   {10, 12, 999, 60},
   {10, 12, 999, 60},
   {10, 12, 999, 60},
   {10, 12, 999, 60},
   {10, 12, 999, 60},
   {10, 12, 999, 60},
   {10, 12, 999, 60},
   {10, 12, 999, 60},
   {10, 12, 999, 60},
   {10, 12, 999, 60},
   {10, 12, 999, 60}
};



const struct int_app_type int_app[45] = {
   {3},
   {5},
   {7},
   {8},
   {9},
   {10},
   {11},
   {12},
   {13},
   {15},
   {17},
   {19},
   {22},
   {25},
   {25},
   {25},
   {25},
   {27},
   {27},
   {27},
   {27},
   {30},
   {30},
   {30},
   {35},
   {35},
   {35},
   {35},
   {40},
   {40},
   {40},
   {45},
   {45},
   {45},
   {45},
   {50},
   {50},
   {50},
   {50},
   {55},
   {55},
   {55},
   {60},
   {60},
   {65}
};



const struct wis_app_type wis_app[45] = {
   {0},
   {0},
   {1},
   {1},
   {1},
   {1},
   {1},
   {1},
   {2},
   {2},
   {2},
   {2},
   {3},
   {3},
   {3},
   {3},
   {3},
   {3},
   {3},
   {3},
   {3},
   {3},
   {4},
   {4},
   {4},
   {4},
   {4},
   {4},
   {4},
   {4},
   {5},
   {5},
   {5},
   {5},
   {5},
   {5},
   {5},
   {5},
   {6},
   {6},
   {6},
   {6},
   {6},
   {6},
   {6}
};



const struct dex_app_type dex_app[45] = {
   {60, 65},
   {50, 70},
   {50, 75},
   {40, 80},
   {30, 85},
   {20, 90},
   {10, 95},
   {0, 100},
   {0, 105},
   {0, 110},
   {0, 115},
   {0, 120},
   {0, 125},
   {0, 130},
   {0, 135},
   {-10, 140},
   {-15, 145},
   {-20, 150},
   {-30, 155},
   {-40, 160},
   {-50, 165},
   {-60, 170},
   {-75, 175},
   {-90, 180},
   {-105, 185},
   {-120, 190},
   {-120, 195},
   {-120, 200},
   {-120, 205},
   {-120, 210},
   {-120, 215},
   {-120, 215},
   {-120, 215},
   {-120, 215},
   {-120, 215},
   {-120, 215},
   {-120, 215},
   {-120, 215},
   {-120, 215},
   {-120, 215},
   {-120, 215},
   {-120, 215},
   {-120, 215},
   {-120, 215},
   {-120, 220}
};



const struct con_app_type con_app[45] = {
   {-4, 20},
   {-3, 25},
   {-2, 30},
   {-2, 35},
   {-1, 40},
   {-1, 45},
   {-1, 50},
   {0, 55},
   {0, 60},
   {0, 65},
   {0, 70},
   {0, 75},
   {0, 80},
   {0, 85},
   {0, 88},
   {1, 90},
   {2, 95},
   {2, 97},
   {3, 99},
   {3, 99},
   {4, 99},
   {4, 99},
   {5, 99},
   {6, 99},
   {7, 99},
   {7, 99},
   {8, 99},
   {8, 99},
   {9, 99},
   {9, 99},
   {10, 99},
   {10, 99},
   {10, 99},
   {10, 99},
   {10, 99},
   {10, 99},
   {10, 99},
   {10, 99},
   {10, 99},
   {10, 99},
   {10, 99},
   {10, 99},
   {10, 99},
   {10, 99},
   {10, 99}
};


const struct cha_app_type cha_app[45] = {
   {-60},
   {-50},
   {-50},
   {-40},
   {-30},
   {-20},
   {-10},
   {-5},
   {-1},
   {0},
   {0},
   {0},
   {0},
   {0},
   {1},
   {5},
   {10},
   {20},
   {30},
   {40},
   {50},
   {60},
   {70},
   {80},
   {90},
   {99},
   {99},
   {99},
   {99},
   {99},
   {99},
   {99},
   {99},
   {99},
   {99},
   {99},
   {99},
   {99},
   {99},
   {99},
   {99},
   {99},
   {99},
   {99}
};

const struct lck_app_type lck_app[45] = {
   {60},
   {50},
   {50},
   {40},
   {30},
   {20},
   {10},
   {0},
   {0},
   {0},
   {0},
   {0},
   {0},
   {0},
   {0},
   {-10},
   {-15},
   {-20},
   {-30},
   {-40},
   {-50},
   {-60},
   {-75},
   {-90},
   {-105},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120},
   {-120}
};


const struct liq_type liq_table[LIQ_MAX] = {
   {"water", "clear", {0, 1, 10}},
   {"beer", "amber", {3, 2, 5}},
   {"wine", "rose", {5, 2, 5}},
   {"ale", "brown", {2, 2, 5}},
   {"dark ale", "dark", {1, 2, 5}},

   {"whisky", "golden", {6, 1, 4}},
   {"lemonade", "pink", {0, 1, 8}},
   {"firebreather", "boiling", {10, 0, 0}},
   {"local specialty", "everclear", {3, 3, 3}},
   {"slime mold juice", "green", {0, 4, -8}},

   {"milk", "white", {0, 3, 6}},
   {"tea", "tan", {0, 1, 6}},
   {"coffee", "black", {0, 1, 6}},
   {"blood", "red", {0, 2, -1}},
   {"salt water", "clear", {0, 1, -2}},

   {"cola", "cherry", {0, 1, 5}},
   {"mead", "honey color", {4, 2, 5}},
   {"grog", "thick brown", {3, 2, 5}}
};
/*
char *	const	attack_table	[18] =
{
    "hit",
    "slice",  "stab",    "slash", "whip",  "claw",
    "blast",  "pound",   "crush", "grep",  "bite",
    "pierce", "suction", "bolt",  "arrow", "dart",
    "stone",  "pea"
};
*/
char *attack_table[DAM_MAX_TYPE] = {
   "hit", "slash", "stab", "hack", "crush", "lash", "pierce",
   "thrust", "flaming arrow", "piercing arrow", "shadow arrow",
   "barbed arrow", "camouflaged arrow", "standard arrow"
};

char *attack_table_plural[DAM_MAX_TYPE] = {
   "hits", "slashes", "stabs", "hacks", "crushes", "lashes", "pierces",
   "thrusts", "flaming arrow", "piercing arrow", "shadow arrow",
   "barbed arrow", "camouflaged arrow", "standard arrow"
};

char *weapon_skills[WEP_MAX] = {
   "Barehand", "Sword", "Dagger", "Whip", "Talon",
   "Mace", "Archery", "Blowgun", "Sling", "Axe", "Spear",
   "Staff", "Energy Weapons", "Katana"
};

char *projectiles[PROJ_MAX] = {
   "Bolt", "Arrow", "Dart", "Stone"
};


char *s_blade_messages[28] = {
   "misses", "with grazing", "with scratching", "with nicking", "with cutting", "with hitting",
   "with tearing", "with ripping", "with gashing", "with lacerating", "with hacking",
   "with mauling", "with rending", "with decimating", "with _mangling_", "with _devastating_",
   "with _cleaving_", "with _butchering_", "with DISEMBOWELING", "with DISFIGURING", "with GUTTING",
   "with EVISCERATING", "with * SLAUGHTERING *", "with *** ANNIHILATING ***",
   "with **** SMITING ****", "with >-* EVISCERATING *-<", "with >>-* LIQUIFYING *-<<",
   "with >>*<<*>> IMMORAL <<*>>*<<"
};

char *p_blade_messages[28] = {
   "misses", "with grazing", "with scratching", "with nicking", "with cutting", "with hitting",
   "with tearing", "with ripping", "with gashing", "with lacerating", "with hacking",
   "with mauling", "with rending", "with decimating", "with _mangling_", "with _devastating_",
   "with _cleaving_", "with _butchering_", "with DISEMBOWELING", "with DISFIGURING", "with GUTTING",
   "with EVISCERATING", "with * SLAUGHTERING *", "with *** ANNIHILATING ***",
   "with **** SMITING ****", "with >-* EVISCERATING *-<", "with >>-* LIQUIFYING *-<<",
   "with >>*<<*>> IMMORAL <<*>>*<<"
};

char *s_blunt_messages[28] = {
   "misses", "with grazing", "with scuffing", "with pelting", "with bruising", "with striking",
   "with thrashing", "with battering", "with flogging", "with pummelling", "with smashing", "with mauling",
   "with bludgeoning", "with decimating", "with _shattering_", "with _devastating_", "with _maiming_",
   "with _crippling_", "with MUTILATING", "with DISFIGURING", "with MASSACRING", "with PULVERIZING",
   "with * OBLITERATING *", "with *** ANNIHILATING ***", "with **** SMITING ****", "with >-* EVISCERATING *-<",
   "with >>-* LIQUIFYING *-<<", "with >>*<<*>> IMMORAL <<*>>*<<"
};

char *p_blunt_messages[28] = {
   "misses", "with grazing", "with scuffing", "with pelting", "with bruising", "with striking",
   "with thrashing", "with battering", "with flogging", "with pummelling", "with smashing", "with mauling",
   "with bludgeoning", "with decimating", "with _shattering_", "with _devastating_", "with _maiming_",
   "with _crippling_", "with MUTILATING", "with DISFIGURING", "with MASSACRING", "with PULVERIZING",
   "with * OBLITERATING *", "with *** ANNIHILATING ***", "with **** SMITING ****", "with >-* EVISCERATING *-<",
   "with >>-* LIQUIFYING *-<<", "with >>*<<*>> IMMORAL <<*>>*<<"
};

char *s_generic_messages[28] = {
   "misses", "with brushing", "with scratching", "with grazing", "with nicking", "with jolting",
   "with wounding", "with injuring", "with hitting", "with jarring", "with thrashing", "with mauling",
   "with decimating", "with _traumatizing_", "with _devastating_", "with _maiming_", "with disemboweling",
   "with _demolishing_", "with MUTILATING", "with MASSACRING", "with PULVERIZING", "with DESTROYING",
   "with * OBLITERATING *", "with *** ANNIHILATING ***", "with **** SMITING ****",
   "with >-* EVISCERATING *-<", "with >>-* LIQUIFYING *-<<", "with >>*<<*>> IMMORAL <<*>>*<<"
};

char *p_generic_messages[28] = {
   "misses", "with brushing", "with scratching", "with grazing", "with nicking", "with jolting",
   "with wounding", "with injuring", "with hitting", "with jarring", "with thrashing", "with mauling",
   "with decimating", "with _traumatizing_", "with _devastating_", "with _maiming_", "with disemboweling",
   "with _demolishing_", "with MUTILATING", "with MASSACRING", "with PULVERIZING", "with DESTROYING",
   "with * OBLITERATING *", "with *** ANNIHILATING ***", "with **** SMITING ****",
   "with >-* EVISCERATING *-<", "with >>-* LIQUIFYING *-<<", "with >>*<<*>> IMMORAL <<*>>*<<"
};

char *ft_end_msg[6] = {
   "completely", "intensity", "passion", "might", "power", "brutality"
};

/*
char ** const s_message_table[18] =
{
	s_generic_messages,	
	s_blade_messages,	
	s_blade_messages,	
	s_blade_messages,	
	s_blunt_messages,	
	s_blade_messages,	
	s_generic_messages,	
	s_blunt_messages,	
	s_blunt_messages,	
	s_generic_messages,	
	s_blade_messages,	
	s_blade_messages,	
	s_blunt_messages,	
	s_generic_messages,	
	s_generic_messages,	
	s_generic_messages,	
	s_generic_messages,	
	s_generic_messages	
};

char ** const p_message_table[18] =
{
	p_generic_messages,	
	p_blade_messages,	
	p_blade_messages,	
	p_blade_messages,	
	p_blunt_messages,	
	p_blade_messages,	
	p_generic_messages,	
	p_blunt_messages,	
	p_blunt_messages,	
	p_generic_messages,	
	p_blade_messages,	
	p_blade_messages,	
	p_blunt_messages,	
	p_generic_messages,	
	p_generic_messages,	
	p_generic_messages,	
	p_generic_messages,	
	p_generic_messages	
};
*/
char **const s_message_table[DAM_MAX_TYPE] = {
   s_generic_messages,  /* hit */
   s_blade_messages, /* slash */
   s_blade_messages, /* stab */
   s_blade_messages, /* hack */
   s_blunt_messages, /* crush */
   s_blunt_messages, /* lash */
   s_blade_messages, /* pierce */
   s_blade_messages, /* thrust */
   s_generic_messages,  /* hit */
   s_generic_messages,  /* hit */
   s_generic_messages,  /* hit */
   s_generic_messages,  /* hit */
   s_generic_messages,  /* hit */
   s_generic_messages   /* hit */
};

char **const p_message_table[DAM_MAX_TYPE] = {
   p_generic_messages,  /* hit */
   p_blade_messages, /* slash */
   p_blade_messages, /* stab */
   p_blade_messages, /* hack */
   p_blunt_messages, /* crush */
   p_blunt_messages, /* lash */
   p_blade_messages, /* pierce */
   p_blade_messages, /* thrust */
   p_generic_messages,  /* hit */
   p_generic_messages,  /* hit */
   p_generic_messages,  /* hit */
   p_generic_messages,  /* hit */
   p_generic_messages,  /* hit */
   p_generic_messages   /* hit */
};

char *const temp_settings[MAX_CLIMATE] = {
   "cold",
   "cool",
   "normal",
   "warm",
   "hot",
};

char *const precip_settings[MAX_CLIMATE] = {
   "arid",
   "dry",
   "normal",
   "damp",
   "wet",
};

char *const wind_settings[MAX_CLIMATE] = {
   "still",
   "calm",
   "normal",
   "breezy",
   "windy",
};

char *const preciptemp_msg[6][6] = {
   {
    "Frigid temperatures settle over the land",
    "It is bitterly cold",
    "The weather is crisp and dry",
    "A comfortable warmth sets in",
    "A dry heat warms the land",
    "Seething heat bakes the land"},
   {
    "A few flurries drift from the high clouds",
    "Frozen drops of rain fall from the sky",
    "An occasional raindrop falls to the ground",
    "Mild drops of rain seep from the clouds",
    "It is very warm, and the sky is overcast",
    "High humidity intensifies the searing heat"},
   {
    "A brief snow squall dusts the earth",
    "A light flurry dusts the ground",
    "Light snow drifts down from the heavens",
    "A light drizzle mars an otherwise perfect day",
    "A few drops of rain fall to the warm ground",
    "A light rain falls through the sweltering sky"},
   {
    "Snowfall covers the frigid earth",
    "Light snow falls to the ground",
    "A brief shower moistens the crisp air",
    "A pleasant rain falls from the heavens",
    "The warm air is heavy with rain",
    "A refreshing shower eases the oppressive heat"},
   {
    "Sleet falls in sheets through the frosty air",
    "Snow falls quickly, piling upon the cold earth",
    "Rain pelts the ground on this crisp day",
    "Rain drums the ground rythmically",
    "A warm rain drums the ground loudly",
    "Tropical rain showers pelt the searing ground"},
   {
    "A downpour of frozen rain covers the land in ice",
    "A blizzard blankets everything in pristine white",
    "Torrents of rain fall from a cool sky",
    "A drenching downpour obscures the temperate day",
    "Warm rain pours from the sky",
    "A torrent of rain soaks the heated earth"}
};

char *const windtemp_msg[6][6] = {
   {
    "The frigid air is completely still",
    "A cold temperature hangs over the area",
    "The crisp air is eerily calm",
    "The warm air is still",
    "No wind makes the day uncomfortably warm",
    "The stagnant heat is sweltering"},
   {
    "A light breeze makes the frigid air seem colder",
    "A stirring of the air intensifies the cold",
    "A touch of wind makes the day cool",
    "It is a temperate day, with a slight breeze",
    "It is very warm, the air stirs slightly",
    "A faint breeze stirs the feverish air"},
   {
    "A breeze gives the frigid air bite",
    "A breeze swirls the cold air",
    "A lively breeze cools the area",
    "It is a temperate day, with a pleasant breeze",
    "Very warm breezes buffet the area",
    "A breeze circulates the sweltering air"},
   {
    "Stiff gusts add cold to the frigid air",
    "The cold air is agitated by gusts of wind",
    "Wind blows in from the north, cooling the area",
    "Gusty winds mix the temperate air",
    "Brief gusts of wind punctuate the warm day",
    "Wind attempts to cut the sweltering heat"},
   {
    "The frigid air whirls in gusts of wind",
    "A strong, cold wind blows in from the north",
    "Strong wind makes the cool air nip",
    "It is a pleasant day, with gusty winds",
    "Warm, gusty winds move through the area",
    "Blustering winds punctuate the searing heat"},
   {
    "A frigid gale sets bones shivering",
    "Howling gusts of wind cut the cold air",
    "An angry wind whips the air into a frenzy",
    "Fierce winds tear through the tepid air",
    "Gale-like winds whip up the warm air",
    "Monsoon winds tear the feverish air"}
};

char *const precip_msg[3] = {
   "there is not a cloud in the sky",
   "pristine white clouds are in the sky",
   "thick, grey clouds mask the sun"
};

char *const wind_msg[6] = {
   "there is not a breath of wind in the air",
   "a slight breeze stirs the air",
   "a breeze wafts through the area",
   "brief gusts of wind punctuate the air",
   "angry gusts of wind blow",
   "howling winds whip the air into a frenzy"
};
/*
const	struct	xsocial_type	xsocial_table [MAX_XSOC] =
{

    {
	"x-earlobe",
	"On whom do you wish to do this?",
	NULL,
	"You gently tug on $M earlobe with your teeth.",
	"$n gently tugs on $N's earlobe with $s teeth.",
	"$n gently tugs on your earlobe with $s teeth.",
	"Not on yourself!",
	NULL,
	0, 0, 0, 1, 1, FALSE
    },

{
"x-kneel",
"Drop to your knees before who?",
NULL,
"You drop to your knees before $M, begging for mercy.",
"$n drops to $s knees before $N, begging for $M mercy.",
"$n drops to $s knees before you, begging for mercy.",
"Nope.",
NULL,
0, 0, 0, 7, 7, FALSE
},
    {
	"x-french",
	"On whom do you wish to do this?",
	NULL,
	"You give $M a deep throbbing kiss, rolling your tongue around $S.",
	"$n gives $N a deep throbbing kiss.",
	"$n gives you a deep throbbing kiss, rolling $s tongue around yours.",
	"Not on yourself!",
	NULL,
	0, 0, 0, 1, 1, FALSE
    },

    {
	"x-kissneck",
	"On whom do you wish to do this?",
	NULL,
	"You slowly and softly kiss and nuzzle $M neck.",
	"$n slowly and softly kisses and nuzzles $N's neck.",
	"$n slowly and softly kisses and nuzzles your neck.",
	"Not on yourself!",
	NULL,
	0, 0, 0, 1, 1, FALSE
    },

{
"x-wcream",
"On whom do you wish to do this?",
NULL,
"You spray whipped cream all over $M body, licking it up with your tongue.",
"$n coats $N's body in whipped cream and then licks it up with their tongue.",
"$n covers your body in whipped cream, and you moan as it gets licked up with their tongue.",
"No.",
NULL,
0, 0, 0, 5, 5, FALSE
},
{
"x-ice",
"On who?",
NULL,
"You run a cold piece of ice down $M back, following up with your tongue.",
"$n runs a piece of ice over $N's back, following up with their tongue.",
"$n runs a piece of ice over your burning hot flesh, following up with their tongue.",
"No.",
NULL,
0, 0, 0, 6, 8, FALSE
},

{
"x-whip",
"Who's been bad?",
NULL,
"You smirk slightly as you crack a whip over $N's tender skin, making $M whimper.",
"$n grins slightly as he whips $N's tender flesh, making $M whimper.",
"$n grins wickedly as they whip your tender flesh, making you whimper slightly in fear.",
"No.",
NULL,
0, 0, 0, 9, 2, FALSE
},
    {
	"x-moan",
	"On whom do you wish to do this?",
	NULL,
	"You start moaning 'Oh $N...oh yes...don't stop...mmMMmm...'",
	"$n starts moaning 'Oh $N...oh yes...don't stop...mmMMmm...'",
	"$n starts moaning 'Oh $N...oh yes...don't stop...mmMMmm...'",
	"Not on yourself!",
	NULL,
	0, 2, 0, 1, 1, FALSE
    },

    {
	"x-nuttella",
	"On whom do you wish to do this?",
	NULL,
	"You cover $N's naked flesh with a popular choc spread, before licking it off.",
	"$n smears a chocolate spread over $N's body, licking $M clean with relish",
	"$n smears a popular choc spread on your naked flesh, licking it off you",
	"Not on yourself!",
	NULL,
	0, 0, 0, 5, 20, FALSE
    },

    {
	"x-stroke",
	"On whom do you wish to do this?",
	NULL,
	"You lightly run your fingers along the insides of $S thighs.",
	"$n lightly runs $s fingers along the insides of $N's thighs.",
	"$n lightly runs $s fingers along the insides of your thighs.",
	"Not on yourself!",
	NULL,
	0, 0, 0, 5, 10, FALSE
    },

    {
	"x-tender",
	"On whom do you wish to do this?",
	NULL,
	"You run your fingers through $S hair and kiss $M tenderly on the lips.",
	"$n runs $s fingers through $N's hair and kisses $M tenderly on the lips.",
	"$n runs $s fingers through your hair and kisses you tenderly on the lips.",
	"Not on yourself!",
	NULL,
	0, 0, 0, 1, 1, FALSE
    },

    {
	"x-tie",
	"On whom do you wish to do this?",
	NULL,
	"You tie $N to the bed posts!",
	"$n ties $N's body, fixing $M helplessly on the bed.  Oh!! Kinky!",
	"$n ties you to the bed post, leaving you completely at $s mercy",
	"Not on yourself!",
	NULL,
	0, 0, 0, 15, 10, FALSE
    },

	{
	"x-gag",
	"Gag whom?",
	NULL,
	"You tear off a small strip of $N's clothing, and stuff it into their mouth, tying it back behind their head to secure it.",
	"$n reaches down and tears off a small strip of $N's clothing, stuffing it into $S mouth and tying it behind $S head, effectively gagging $S.",
	"$n tears off a strip of your clothing, and as you look up at $s, $s shoves it into your mouth and ties it behind your head, gagging you.",
	"No.",
	NULL,
	0, 0, 0, 5, 2, FALSE
	},

	{
	"x-blindfold",
	"Blindfold who?",
	NULL,
	"You take out a black silk blindfold, and place it over $N's eyes, tying it securely.",
	"$n takes out a black silk blindfold, and places it over $N's eyes, tying it securely behind $S.",
	"$n takes out a black silk blindfold, and grins as $s places it over your eyes, tying it behind your head.",
	"No.",
	NULL,
	0, 0, 0, 7, 2, FALSE
	},

	{
    "x-knife",
	"On whom do you wish to do this?",
    NULL,
    "You lightly run a cool knife over $N's skin, letting it slide across their body and make weak cuts in their flesh.",
    "$n lightly traces a knife over $N's skin, smiling as it makes a few shallow cuts, drawing a bit of blood.",
    "$n lightly runs a knife over your skin, and you shiver a bit as it makes a few shallow cuts, drawing a bit of your blood.",
    "No.",
    NULL,
    0, 0, 0, 15, 3, FALSE
	}, 

    {
	"x-withdraw",
	"On whom do you wish to do this?",
	NULL,
	"You gently pull yourself free of $M.",
	"$n gently pulls $mself free of $N.",
	"$n gently pulls $mself free of you.",
	"Not on yourself!",
	NULL,
	0, 1, 0, 0, 0, FALSE
    },

	{
	"xl-candle",
	"On who?",
	NULL,
	"You produce a thick white candle, and you step behind $N with it, grinning a bit as you run it up and down her pussy lips.  After a moment you push it in and twist it, starting to slowly pump it in and out.",
	"$n produces a thick white candle, and walks behind $N, holding it in her hands.  She grins a bit as she pushes it to the girl's wet pussy lips, and pushes it in, twisting it as she begins to pump it in and out.",
	"$n produces a thick white candle, and steps behind you as you whimper a bit.  After a moment, you feel it running up and down your cunt lips, and you let out a moan as it slips inside you, twisting as it slides in and out.",
	"No.",
	NULL,
	3, 0, 0, 17, 39, FALSE
	},
{
"xl-afinger",
"Use this on who?",
NULL,
"You take your finger to $N's mouth and have her lick it before taking it to her puckered ass and slowly slipping it in.",
"$N sucks lightly on $n's finger a moment before she takes it between $N's ass cheeks and pushes it in to the knuckle, twisting.",
"$n brings her finger to your lips, and you suck on it a bit before she pulls it away.  A moment later, she presses it to your tight little ass and pushes it in, twisting as you whimper softly.",
"No.",
NULL,
3, 0, 0, 20, 25, FALSE
},

	{
    "xl-finger",
	"On whom do you wish to do this?",
    NULL,
    "You spread $N's thighs and slip your finger between them, tracing it over her lips a few times before pushing it past and into her hot and wet pink folds.",
    "$n spreads $N's thighs and plunges her finger into her twat, pumping it out a few times as sweet, sticky juice spills over it.",
    "$n spreads your legs and begins to trace her finger over your moistened lips, and you let out a soft moan as she slips it past them and into your hot snatch, plunging it in and out as it collects your juices.",
    "No.",
    NULL,
    3, 0, 250, 19, 30, FALSE
	},

	{
	"xl-gag",
	"Gag who?",
	NULL,
	"You pick up your soaked panties, and wad them up into a ball, stuffing them into $N's mouth to gag her.",
	"$n picks up her drenched panties, wadding them into a ball and stuffing them into $N's mouth, gagging her.",
	"$n picks up her panties, soaked through with her cunt juice, and wads them into a ball, stuffing them into your mouth to gag you.",
	"No.",
	NULL,
	3, 0, 0, 7, 9, FALSE
	},

	{
    "xl-stroke",
	"On whom do you wish to do this?",
    NULL,
    "You slip your finger between $N's legs and gently stroke her throbbing clit, making her squirm and moan.",
    "$n slips her finger between $N's damp thighs and begins to softly stroke her clit, making the poor girl squirm and moan in pleasure.",
    "$n slips her finger between your thighs and begins to softly stroke your throbbing clit, making you shut your eyes and moan in pleasure.",
    "No.",
    NULL,
    3, 0, 250, 10, 17, FALSE
	},

	{
    "xl-undress",
	"On whom do you wish to do this?",
    NULL,
    "You reach back and unhook your bra, letting it slide to the floor with your panties.",
    "$n reaches back and unhooks her bra, letting it fall to the floor along with her panties.",
    "$n reaches back and unhooks her bra, letting it fall to the floor along with her panties.",
    "Pick a valid target.",
    NULL,
    3, 0, 250, 15, 15, FALSE
	}, 

	{
    "xl-lick",
	"On whom do you wish to do this?",
    NULL,
    "You bury your face between $N's thighs, letting your tongue slide over her swollen pussy lips and gently tease her clit.",
    "$n pushes $N to the floor and buries her face between her thighs, letting her tongue slide over $N's pussy lips and caress her clit.",
    "$n pushes you to the floor, burying her face between your damp thighs as she lets her tongue caress your swollen pussy lips, and you squirm as it lightly runs over your clit.",
    "No.",
    NULL,
    3, 0, 250, 25, 37, FALSE 
	},

	{ 
    "xl-69",
	"On whom do you wish to do this?",
    NULL, 
    "You lie down on your back and motion for $N to straddle your face, moaning as she licks at you while you hungrily devour her wet snatch.", 
    "$N lowers her pussy over $n's face and places her face between her thighs, both of them gasping as their tongues dart in and out of each other.",
    "$n pulls you on top of her, placing your hot, wet pussy right over her face, and you do the same, both of you panting and moaning as you devour each other.", 
    "No.", 
    NULL, 
    3, 1, 250, 40, 40, FALSE 
	},

	{ 
    "xl-breasts",
	"On whom do you wish to do this?",
    NULL, 
    "You lean forward and place your hands over $N's supple breasts, kneading them softly as she moans.",
    "$n places her hands over $N's soft breasts and begins to gently knead them, smiling as she begins to moan.", 
    "$n places her hands over your soft breasts and begins to knead them, a smile playing over her face as you start moaning.", 
    "No.",
    NULL,
    3, 0, 250, 10, 18, FALSE 
	}, 
	
	{
	"xf-oral",
	"On whom do you wish to do this?",
	NULL,
	"You take $N's hot member in your hands, licking his cock from base to tip.",
	"$n takes $N's penis in $s hands, licking $N's cock from base to tip.",
    "You let out a small moan as $n takes your cock into her hands, licking it from your balls to the tip.",
	"On yourself? I'm impressed!",
	NULL,
	2, 0, 0, 5, 15, FALSE
    },

	{
	"xf-spank",
	"Who's been naughty?",
	NULL,
	"You take $N over your knee and spank him hard, making him squirm.",
	"$n bends $N over her knees and spanks him hard on the ass, making him squirm.",
	"You whimper softly as $n takes you over her knee, spanking you hard on the ass, making it sting.",
	"Nah.",
	NULL,
	2, 0, 0, 25, 4, FALSE
	},

	{
	"xf-urine",
	"On whom do you wish to do this?",
	NULL,
	"You maliciously lean $N's head back and stand over him with his mouth open before letting a hard stream of piss shoot down his throat.",
	"$n shoots a golden stream of urine down $N's throat, making him choke and swallow.",
	"You eyes widen as your mistress shoots a stream of piss o.down your throat, making you swallow it all.",
	"Nope.",
	NULL,
	2, 0, 0, 37, -10, FALSE
	},
    
	{
	"xf-beg",
	"Beg who, you stupid slut?",
	NULL,
	"You drop to your kneees before $N, begging for him to abuse you and fuck you hard.",
	"$n drops to her knees and begs for $N to take her and use her for his pleasure.",
	"$n drops to her knees before you, begging for you to abuse her worthless slut body.",
	"It doesn't work that way, you stupid whore.",
	NULL,
	2, 0, 0, -10, 12, FALSE
	},

	{
	"xf-blowjob",
	"On whom do you wish to do this?",
	NULL,
	"You take $N's hot member in your mouth, sucking $S shaft.",
	"$n takes $N's throbbing penis in $s mouth, sucking $N's cock.",
	"You gasp as $n takes your penis in $s mouth, licking your head.",
	"On yourself? I'm impressed!",
	NULL,
	2, 1, 250, 10, 25, FALSE
    },

	{
       "xf-tip",
       "On whom do you wish to do this?",
       NULL,
       "You gently run the tip of your tongue along the underside of $N's hot cock.",
       "$n runs the tip of $s tongue along the bottom of $N's pulsing cock.",
       "You let out a low moan as $n runs the tip of her tongue along the underside of your cock.",
       "I think not.",
       NULL,
         2, 0, 0, 5, 59, FALSE
	},

	{
	"xf-handjob",
	"On who?",
	NULL,
	"You take his hot cock in your hand and wrap your fingers around it, slowly pumping up and down.",
	"$n takes $N's hot cock in her hands, wrapping her fingers around it as she slowly pumps up and down.",
	"$n takes your hot cock into her hands, and you moan softly as she wraps her fingers around it, slowly pumping up and down.",
	"No.",
	NULL,
	2, 0, 0, 9, 14, FALSE
	},

	{
	"xf-sitface",
	"Who gets to taste your sweet juice?",
	NULL,
	"You push $N onto his back and crawl up his body, sitting down on his face as he licks at your pussy.",
	"$n pushes $N onto his back and straddles his face, moaning as he licks her hot pussy.",
	"$n pushes you onto your back and straddles your face, moaning and squirming as you eagerly lap at her hot, wet cunt.",
	"No.",
	NULL,
	2, 1, 250, 15, 0, FALSE
	},

	{
	"xf-69",
	"Who?",
	NULL,
	"You crawl on top of $N and takes his hot cock between your lips, moaning as he laps at your cunt.",
	"$n pushes $N onto his back and crawls on top of him, sucking his cock while he wriggles his tongue about in her cunt.",
	"$n pushes you onto your back, crawling on top of you as she sucks on your cock, moaning while you lick her pussy.",
	"No.",
	NULL,
	2, 1, 250, 20, 20, TRUE
	},

    {
	"xf-breasts",
	"On whom do you wish to do this?",
	NULL,
	"You take $S hands and press them to your breasts.",
	"$n takes $N's hands and presses them to $m breasts.",
	"$n takes your hands and presses them to $m breasts.",
	"Not on yourself!",
	NULL,
	2, 0, 0, 5, 10, FALSE
    },

{
"xf-cum",
"Wrong...",
NULL,
"You feel your muscles tighten for a moment, and you let out a loud moan and arch your back as you release, cumming hard as your juices run down your thighs.",
"$n arches her back and lets out a loud moan, gasping and writhing in pleasure as she cums, her juices running down her thighs.",
"$n arches her back and lets out a loud moan, gasping and writhing in pleasure as her juices gush down her thighs.",
"No.",
NULL,
2, 1, 0, 500, 25, FALSE
},
{ 
"xf-cface",
"Wrong...",
NULL,
"You bite down hard on your lip, letting out a loud moan as you climax, gushing your sticky, wet cum all over $N's face, glazing it over.",
"$n bites down on her lip and lets out a loud cry of pleasure, gushing her juices all over $N's face as he sucks and slurps at her dripping wet pussy.",
"$n bites down on her lip and lets out a loud moan as you slurp at her dripping wet cunt, and she shudders before gushing her sweet juices all over your face, glazing it completely over.",
"No.",
NULL,
2, 1, 0, 500, 30, FALSE
},
{
"xf-chands",
"No...",
NULL,
"You keep sliding your hands back and forth over $N's cock as it grows hotter and hotter, not stopping until he lets out a low groan and fills your hand with his hot, sticky cum.",
"$n slides her hands back and forth over $N's cock as he moans, eventually gritting his teeth and shutting his eyes as he fills her hand full of his hot sticky load of cum, most of it oozing out from between her fingertips.", 
"$n slides her hands over your cock as it grows hotter and hotter, and you let out a low moan as you grit your teeth and shoot a hot, sticky load of cum into her closed hands, filling it up as most of your gooey sperm drips out from between her fingertips.",
"No.",
NULL,
2, 1, 0, 30, 500, FALSE
},

	{
"xf-cbreasts",
"No..",
NULL,
"You take $N's twitching cock between your hands, and aim it straight at your heaving tits, jerking him off until you feel stream after stream of hot, white cum splatter against them and trickle over your stiff nipples.",
"$n takes $N's stiff cock in her hands and aims it at her soft tits, giving him a few quick jerks to finish him off as he shoots out a jet of hot, sticky cum all over her ample mounds and hard nipples.",
"$n takes your cock into her hands and aims it straight at her tits, giving you a few quick jerks to finish you off, and you moan as you shoot your sticky white seed all over her soft breasts, your cum oozing down and trickling over her nipples.",
"No.",
NULL,
1, 1, 0, 37, 500, FALSE
},

	{
      "xf-rub",
      "Who gets to feel the magic touch?",
	  NULL,
      "You gently run your fingers over $N's hardening cock.",
      "$n gently runs her fingers over $N's hardening cock.",
      "$n gently runs her fingers over your hardening cock.",
      "You don't have a penis.  At least, I hope not.",
      NULL,
      2, 0, 0, 15, 20, FALSE
	},
    {
	"xf-contract",
	"On whom do you wish to do this?",
	NULL,
	"You contract your vaginal muscles, driving $M wild.",
	"$n contracts $m vaginal muscles, driving $N wild.",
	"$n contracts $m vaginal muscles, driving you wild.",
	"Not on yourself!",
	NULL,
	2, 2, 0, 10, 15, TRUE
    },

{
"xf-afinger",
"Use this on who?",
NULL,
"You lick your finger and press it to $N's ass, pushing lightly until it pops in and he lets out a small moan.",
"$n licks her finger and takes it to $N's ass, popping it in and gently pumping it back and forth, making him moan.",
"$n licks her finger and takes it to your ass, pushing it in and pumping it in and out, making your dick rock-hard as you moan.",
"No.",
NULL,
2, 1, 0, 20, 23, FALSE
},

    {
	"xf-finger",
	"On whom do you wish to do this?",
	NULL,
	"You put your hands between your legs and begin to masterbate for $N.",
	"$n puts $m hands between $m legs and begins to masterbate for $N.",
	"$n puts $m hands between $m legs and begins to masterbate for your viewing pleasure.  What a turn on!.",
	"Not on yourself!",
	NULL,
	2, 0, 0, 20, 10, FALSE
    },
    {
	"xf-floor",
	"On whom do you wish to do this?",
	NULL,
	"You lie on your back, and pull $M between your parted legs.",
	"$n lies on $m back, and pulls $N between $m parted legs.",
	"$n lies on $m back, and pulls you between $m parted legs.",
	"Not on yourself!",
	NULL,
	2, 1, 250, 25, 20, TRUE
    },

    {
	"xf-grind",
	"On whom do you wish to do this?",
	NULL,
	"You grind your hips up to meet $S thrusts.",
	"$n grinds $m hips up to meet $N's thrusts.",
	"$n grinds $m hips up to meet your thrusts.",
	"Not on yourself!",
	NULL,
	2, 2, 0, 15, 10, TRUE
    },

    {
	"xf-mount",
	"On whom do you wish to do this?",
	NULL,
	"You push $M onto $S back, and slowly lower yourself onto $S erection.",
	"$n pushes $N onto $S back, and slowly lowers $mself onto $S erection.",
	"$n pushes you onto your back, and slowly lowers $mself onto your erection.",
	"Not on yourself!",
	NULL,
	2, 1, 250, 25, 20, TRUE
    },

    {
	"xf-nails",
	"On whom do you wish to do this?",
	NULL,
	"You scratch your nails down $S back.",
	"$n scratches $m nails down $N's back.",
	"$n scratches $m nails down your back.",
	"Not on yourself!",
	NULL,
	2, 2, 0, 5, 1, TRUE
    },

    {
	"xf-pull",
	"On whom do you wish to do this?",
	NULL,
	"You wrap your arms and legs around $M and pull $M into you.",
	"$n wraps $m arms and legs around $N and pulls $M into $m.",
	"$n wraps $m arms and legs around you and pulls you into $m.",
	"Not on yourself!",
	NULL,
	2, 1, 250, 15, 10, TRUE
    },

    {
	"xf-squeeze",
	"On whom do you wish to do this?",
	NULL,
	"Your wrap your legs around $M and squeeze tightly.",
	"$n wraps $m legs around $N and squeezes tightly.",
	"$n wraps $m legs around you and squeezes tightly.",
	"Not on yourself!",
	NULL,
	2, 2, 0, 15, 10, TRUE
    },

    {
	"xf-titfuck",
	"Whom do you wish to treat to this?",
	NULL,
	"You take $N's cock and place it between your breasts, as $N gently thrusts.",
	"$n takes $N's penis, places it between $s breasts, and wanks $M off.",
	"$n takes your cock, places it between her breasts, and wanks you gently with them.",
	"Not on yourself!",
	NULL,
	2, 1, 0, 10, 20, FALSE
    },

{
"xf-fondle",
"On whom do you wish to perform this?",
NULL,
"You reach down and gently fondle $N's warm penis.",
"$n takes $N's cock into her hands, fondling it gently.",
"$n takes your warm cock into her hands, fondling you gently.",
"No.",
NULL,
2, 0, 0, 5, 13, FALSE
},

{
"xf-bob",
"On whom do you wish to perform this?",
NULL,
"You wrap your lips tight around $N's cock and bob your head back and forth, forcing him into your mouth.",
"$n wraps her lips over $N's cock and bobs her head back and forth, making him fuck her mouth.",
"$n wraps her lips tight around your cock and bobs her head back and forth, pumping your length in and out of her mouth.",
"No.",
NULL,
2, 1, 0, 20, 25, TRUE
},

    {
	"xf-undress",
	"On whom do you wish to do this?",
	NULL,
	"You push $N onto the floor, a gleam in your eyes, and tear $S clothes off.",
	"$n pushes $N to the floor, tears $S clothes from $S body.",
	"$n pushes you to the floor, grins, and rips your clothes from your body.",
	"Not on yourself!",
	NULL,
	2, 0, 0, 1, 1, FALSE
    },

    {
	"xm-anal",
	"On whom do you wish to do this?",
	NULL,
	"$N screams in extacy as you come from behind and penetrate $M tight ass from behind.",
	"$n comes from behind $N and penetrates $M tight ass from behind.",
	"You scream as $n comes from behind and penetrates your tight ass from behind!",
	"Not on yourself!",
	NULL,
	1, 1, 250, 25, 25, FALSE
    },

    {
	"xm-spank",
	"Who's the naughty whore?",
	NULL,
	"You grab $N by the hair and throw her over your knees, slapping her hard on the ass.",
	"$n visciously throws $N over his knee, spanking her relentlessly on her tight ass.",
	"You cry out softly as $n visciously takes you over his knee, spanking you mercilessly.",
	"Nah.",
	NULL,
	1, 0, 0, 33, -14, FALSE
	},

	{
	"xm-69",
	"Whom do you wish to perform this on?",
	NULL,
	"You pull $N on top of you and spread her legs, burying your face in her pussy as she sucks on your hard cock.",
	"$n pulls $N on top of him and buries his face between her legs, eating her out as she goes down on his cock.",
	"$n pulls you on top of him and spreads your legs, burying his face between your legs and sucking on your clit as you takes his cock into your mouth.",
	"No.",
	NULL,
	1, 1, 0, 24, 24, TRUE
	},

	{
	"xm-urine",
	"Who gets to taste it?",
	NULL,
	"You force open $N's slutty mouth and shoot a hot stream of piss into it, making her swallow.",
	"$n forces open $Ns mouth and shoots a hot stream of golden piss into it, making her swallow.",
	"$n forces open your slutty mouth and shoots a hot, golden stream of his piss into it, making you swallow.",
	"No.",
	NULL,
	1, 0, 0, 40, -19, FALSE
	},

	{
	"xm-beg",
	"Beg who, you worthless bitchboy?",
	NULL,
	"You drop to your knees, tears in your eyes, and beg for $N to take you as hers.",
	"$n drops to his knees before $N, begging for her to take him as hers.",
	"$n drops to his knees before you with tears in his eyes, sobbing as he begs for you to take him as your own personal toy.",
	"You worthless slut, you can't even do this right.",
	NULL,
	1, 0, 0, -15, 30, FALSE
	},

	{
	"xm-facial",
	"Who gets to have their slutty face done over?",
	NULL,
	"You groan as you shoot your load across her delicate features, your hot cum coating her face and dripping from her chin.",
	"$n grits his teeth as he shoots his sticky cum over $N's face, moaning as she licks her lips and lets it drip down.",
	"$n's eyes widen as he shoots his hot, sticky cum over your face, letting it drip from your chin and trickle over your lips.",
	"Nope.",
	NULL,
	1, 1, 0, 300, 20, TRUE
	},
	
	{
	"xm-cum",
	"So close, yet, so far away?",
	NULL,
	"You let out a low moan as you release your load into $N, filling her up with your hot cum.",
	"$n cries out as he shoots his hot, sticky cum all inside of $N, his creamy load shooting home.",
	"$n grits his teeth and shoots his sticky cum inside of yor whorish body, filling you up until it drips out.",
	"Nopers.",
	NULL,
	1, 2, 0, 300, 20, TRUE
	},

{
"xm-chair",
"Cum in whose hair?",
NULL,
"You pull your hard cock out of $N's hole, and force her to her knees before you as you finish yourself off with your hand, shooting your gooey wad straight into her hair and using a handfull of it to wipe off the sticky head.",
"$n pulls his cock out of $N's hole, and forces her to her knees, jerking himself off for a moment before shooting his hot, sticky sperm into her hair, and he takes a handfull of it to wipe off the head of his still-stiff meat.",
"$n pulls himself out of your hole, and forces you to your knees before him, jerking off into your hair as you look up.  A hot, gooey splatter of cum smacks against your scalp, and he takes a handfull of your hair to wipe off the still-sticky head of his cock.",
"No.",
NULL,
1, 1, 0, 500, 47, FALSE
},

{
"xm-canal",
"Cum in whose ass?",
NULL,
"You pump your cock in and out of $N's ass a few more times before grunting and shooting your sticky load of cum right up it, feeling it fill and trickle over your still-hard cock and down her thighs.",
"$n pumps into $N's tight ass a few more times before he grunts, holding his cock there as he shoots her full of cum, and she lets out a tiny gasp as a little bit oozes out of her tight ass and trickles down her thighs.",
"$n shoves his cock into your tight little ass a few more times, and you hear a grunt from behind you as he starts to cum, shooting a sticky white jet of his cream straight up your ass, and you feel a bit of it trickle out and run down your thighs.",
"No.",
NULL,
1, 1, 0, 500, 47, FALSE
},


{
"xm-cbreasts",
"So close, yet, so far away?",
NULL,
"You let out a low moan as you release your load into $N, filling her up with your hot cum.",
"$n cries out as he shoots his hot, sticky cum all inside of $N, his creamy load shooting home.",
"$n grits his teeth and shoots his sticky cum inside of yor whorish body, filling you up until it drips out.",
"Nopers.",
NULL,
1, 1, 0, 300, 20, TRUE
},
	{
	"xm-chand",
	"So close, yet, so far away?",
	NULL,
	"You let out a low moan as you release your load into $N, filling her up with your hot cum.",
	"$n cries out as he shoots his hot, sticky cum all inside of $N, his creamy load shooting home.",
	"$n grits his teeth and shoots his sticky cum inside of yor whorish body, filling you up until it drips out.",
	"Nopers.",
	NULL,
	1, 1, 0, 300, 20, TRUE
	},

	{
	"xm-stomach",
	"Cum on whose stomach?",
	NULL,
	"You pull out of $N and stand up, pushing her to her back.  After a few quick jerks on your stiff cock, you let out a moan and start to cum, spraying your hot, gooey cum all over her smooth body.",
	"$n pulls out of $N and stands up, pushing her to her back.  After a few quick jerks on his stiff cock, he closes his eyes and lets out a moan, a stream of gooey whip cum spraying from the swollen head of his cock and all over $N's smooth skin.",
	"$n pulls out of you and pushes you onto your back.  As you look up, he gives his hard cock a few quick jerks, and he moans as he starts to cum, a spray of hot, gooey white cum splattering all over your body, coating you from head to toe.",
	"No.",
	NULL,
	1, 1, 0, 500, 50, TRUE
	},

	{
	"xm-breasts",
	"On whom do you wish to do this?",
	NULL,
	"You gently run the tip of your tongue across $M naked breasts.",
	"$n gently runs the tip of $s tongue across $N's naked breasts.",
	"$n gently runs the tip of $s tongue across your naked breasts.",
	"Not on yourself!",
	NULL,
	1, 0, 0, 5, 10, FALSE
    },

    {
	"xm-cup",
	"On whom do you wish to do this?",
	NULL,
	"You cup $N's breasts in your hands, and caress $S nipples.",
	"$n cups $N's breasts in $s hands and caress' $S nipples.",
	"$n cups your breasts in $s palms and caress' your nipples.",
	"ON YOURSELF??",
	NULL,
	1, 0, 0, 1, 5, FALSE
    },

    {
	"xm-doggy",
	"On whom do you wish to do this?",
	NULL,
	"You roll $M onto all fours and penetrate $M hot flesh from behind.",
	"$n rolls $N onto all fours and penetrates $M hot flesh from behind.",
	"$n rolls you onto all fours and penetrates your hot flesh from behind.",
	"Not on yourself!",
	NULL,
	1, 1, 250, 25, 15, FALSE
    },

    {
	"xm-finger",
	"On whom do you wish to do this?",
	NULL,
	"You slide your fingers between $M legs, gently stroking $M clitoris.",
	"$n slides $s fingers between $N's legs, gently stroking $M clitoris.",
	"$n slides $s fingers between your legs, gently stroking your clitoris.",
	"Not on yourself!",
	NULL,
	1, 0, 0, 0, 10, FALSE
    },

    {
	"xm-fist",
	"On whom do you wish to do this? I hope you asked!",
	NULL,
	"You make a fist and thrust it up $N's spread wide vagina as she screams with pleasure.",
	"$N gasps as $n pushes $s hand up between $S legs into $S vagina.",
	"$n spreads your legs wide, and thrusts $s hand up your vagina making you cry out in pleasure.",
	"C'est non possible.. I hope...",
	NULL,
        1, 1, 0, 5, 15, FALSE
    },

    {
	"xm-floor",
	"On whom do you wish to do this?",
	NULL,
	"You lower $M to the floor, and slide your body between $M parted legs.",
	"$n lowers $N to the floor, and slides $s body between $M parted legs.",
	"$n lowers you to the floor, and slides $s body between your parted legs.",
	"Not on yourself!",
	NULL,
	1, 1, 250, 15, 10, TRUE
    },

    {
	"xm-jerkoff",
	"On whom do you wish to do this?",
	NULL,
	"You grab your penis and begin to jerkoff for $N.",
	"$n grabs your penis and begins to jerkoff for $N.",
	"$n grab his penis and begin to jerkoff for your viewing pleasure.  What a turn on!.",
	"Not on yourself!",
	NULL,
	1, 1, 0, 25, 5, FALSE
    },

    {
	"xm-nipple",
	"On whom do you wish to do this?",
	NULL,
	"You gently twist $M nipple between your thumb and forefinger.",
	"$n gently twists $N's nipple between $s thumb and forefinger.",
	"$n gently twists your nipple between $s thumb and forefinger.",
	"Not on yourself!",
	NULL,
	1, 0, 0, 5, 10, FALSE
    },

    {
	"xm-oral",
	"On whom do you wish to do this?",
	NULL,
	"$N squirms in delight as you bend down and run your tongue along and into $S vagina.",
	"$N squirms ecstatically as $n licks and kisses $S loveslit.",
	"$n wriggles $s tongue about between your legs, making you squirm in ecstacy.",
	"Biologically impossible I think you'll find!",
	NULL,
	1, 0, 0, 10, 25, FALSE
    },

    {
	"xm-press",
	"On whom do you wish to do this?",
	NULL,
	"You press $M against the wall, pulling $M legs around your hips.",
	"$n presses $N against the wall, pulling $M legs around $s hips.",
	"$n presses you against the wall, pulling your legs around $s hips.",
	"Not on yourself!",
	NULL,
	1, 1, 250, 25, 20, TRUE
    },

    {
	"xm-pull",
	"On whom do you wish to do this?",
	NULL,
	"You grab $M around the hips and pull $M firmly onto your erection.",
	"$n grabs $N around the hips and pull $M firmly onto $s erection.",
	"$n grabs you around the hips and pulls you firmly onto $s erection.",
	"Not on yourself!",
	NULL,
	1, 1, 250, 10, 10, TRUE
    },

    {
	"xm-spoon",
	"On whom do you wish to do this?",
	NULL,
	"You roll $M onto $S side and penetrate $M hot flesh from behind.",
	"$n rolls $N onto $S side and penetrates $M hot flesh from behind.",
	"$n rolls you onto your side and penetrates your hot flesh from behind.",
	"Not on yourself!",
	NULL,
	1, 1, 250, 20, 20, TRUE
    },

    {
	"xm-suck",
	"On whom do you wish to do this?",
	NULL,
	"You suck slowly on $M nipple, feeling it harden between your lips.",
	"$n suck slowly on $N's nipple.",
	"$n sucks slowly on your nipple, and you feel it harden between $s lips.",
	"Not on yourself!",
	NULL,
	1, 0, 0, 5, 10, FALSE
    },

    {
	"xm-thrust",
	"On whom do you wish to do this?",
	NULL,
	"You thrust deeply between $M warm, slippery thighs.",
	"$n thrusts deeply between $N's warm, slippery thighs.",
	"$n thrusts deeply between your warm, slippery thighs.",
	"Not on yourself!",
	NULL,
	1, 2, 0, 15, 10, TRUE
    },
{
"xm-hump",
"On whom do you wish to do this?",
NULL,
"You push $M onto all fours and mount her, madly shoving your hard cock in and out of her tiny hole.",
"$n mounts $N like she was a bitch in heat, madly pumping in and out of her hole.",
"$n pushes you onto all fours like you were a bitch and heat and mounts you as he rapidly shoves his cock in and out of your slutty hole.",
"No!.",
NULL,
1, 2, 0, 35, 65, TRUE
},

{
"xm-mfuck",
"Fuck who?",
NULL,
"You have $N wrap her lips tight around your cock, and you thrust gently, pumping your cock in and out of her warm, wet mouth.",
"$N wraps her lips tight aroudn $n's cock, and he thrusts gently, pumping in and out from between her lips.",
"You wrap your lips tight around $n's cock and he thrusts gently, pumping in and out from between your ruby red lips.",
"No.",
NULL,
1, 1, 0, 12, 20, TRUE
},

{
"xm-feed",
"Feed who?",
NULL,
"You gently stick your fingers in $N's hot cunt and bring them to her lips, making her suck them clean.",
"$n sticks his fingers in $N's twat and then brings them to her lips, making her lick them clean.",
"$n sticks his fingers in your dripping wet pussy and then brings them to your lips, where you suck them clean.",
"No.",
NULL,
1, 1, 0, 5, 10, FALSE
},

{
"xm-gag",
"Who gets to taste it?",
NULL,
"You ram your cock deep into $M mouth, making her gag and try to swallow.",
"$n rams his cock deep into $N's throat, making her gag and try to swallow.",
"$n shoves his cock deep into your throat, making you gag and try to swallow.",
"No.",
NULL,
1, 1, 0, 5, 13, FALSE
},
    {
	"xm-tug",
	"On whom do you wish to do this?",
	NULL,
	"You gently tug $M nipple between your teeth.",
	"$n gently tugs $N's nipple between $s teeth.",
	"$n gently tugs your nipple between $s teeth.",
	"Not on yourself!",
	NULL,
	1, 0, 0, 5, 10, FALSE
    },

{
"xm-nibble",
"On whom do you wish to do this?",
NULL,
"You push $M onto her back and bury your face in her cunt, nibbling gently on her clit.",
"$n pushes $N onto her back and buries his face between her legs, nibbling gently on her clit.",
"$n pushes you onto your back and buries his face in your womanhood, nibbling gently on your clit.",
"Nope.",
NULL,
1, 1, 0, 10, 45, FALSE
},
    {
	"xm-undress",
	"Who do you wish to undress?",
	NULL,
	"You gently tug at $N's garments with your teeth, until $E stands naked before you.",
	"$n gently tugs at $N's clothes with his teeth, until $E stands naked infront of $m.",
	"$n gently tugs at your clothing with his teeth, you stand naked before $m.",
	"Not on yourself!",
	NULL,
	1, 0, 0, 1, 1, FALSE
    },

    {
	"",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, FALSE
    }
};
*/

#define SLOT(n)	n
#define LI LEVEL_IMMORTAL

#undef AM
#undef AC
#undef AT
#undef AW
#undef AV
#undef AD
#undef AR
#undef AA

#undef LI
