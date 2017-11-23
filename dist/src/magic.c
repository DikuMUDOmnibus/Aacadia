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
/*					Spell Handling module		*/
/************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#ifdef sun
#include <strings.h>
#endif
#include <time.h>
#include "acadia.h"

void say_spell args( ( CHAR_DATA * ch, int sn ) );
ch_ret spell_affect args( ( int sn, int level, CHAR_DATA * ch, void *vo ) );
ch_ret spell_affectchar args( ( int sn, int level, CHAR_DATA * ch, void *vo ) );
int dispel_casting( AFFECT_DATA * paf, CHAR_DATA * ch, CHAR_DATA * victim, int affect, bool dispel );
bool can_charm( CHAR_DATA * ch );


bool is_immune( CHAR_DATA * ch, sh_int damtype )
{
   switch ( damtype )
   {
      case SD_FIRE:
         if( IS_SET( ch->immune, RIS_FIRE ) )
            return TRUE;
      case SD_COLD:
         if( IS_SET( ch->immune, RIS_COLD ) )
            return TRUE;
      case SD_ELECTRICITY:
         if( IS_SET( ch->immune, RIS_ELECTRICITY ) )
            return TRUE;
      case SD_ENERGY:
         if( IS_SET( ch->immune, RIS_ENERGY ) )
            return TRUE;
      case SD_ACID:
         if( IS_SET( ch->immune, RIS_ACID ) )
            return TRUE;
      case SD_POISON:
         if( IS_SET( ch->immune, RIS_POISON ) )
            return TRUE;
      case SD_DRAIN:
         if( IS_SET( ch->immune, RIS_DRAIN ) )
            return TRUE;
   }
   return FALSE;
}

int ch_slookup( CHAR_DATA * ch, const char *name )
{
   int sn;

   if( IS_NPC( ch ) )
      return skill_lookup( name );
   for( sn = 0; sn < top_sn; sn++ )
   {
      if( !skill_table[sn]->name )
         break;
      if( ch->pcdata->learned[sn] > 0
          && DUAL_SKILL( ch, sn )
          && LOWER( name[0] ) == LOWER( skill_table[sn]->name[0] ) && !str_prefix( name, skill_table[sn]->name ) )
         return sn;
   }

   return -1;
}

int herb_lookup( const char *name )
{
   int sn;

   for( sn = 0; sn < top_herb; sn++ )
   {
      if( !herb_table[sn] || !herb_table[sn]->name )
         return -1;
      if( LOWER( name[0] ) == LOWER( herb_table[sn]->name[0] ) && !str_prefix( name, herb_table[sn]->name ) )
         return sn;
   }
   return -1;
}

int personal_lookup( CHAR_DATA * ch, const char *name )
{
   int sn;

   if( !ch->pcdata )
      return -1;
   for( sn = 0; sn < MAX_PERSONAL; sn++ )
   {
      if( !ch->pcdata->special_skills[sn] || !ch->pcdata->special_skills[sn]->name )
         return -1;
      if( LOWER( name[0] ) == LOWER( ch->pcdata->special_skills[sn]->name[0] )
          && !str_prefix( name, ch->pcdata->special_skills[sn]->name ) )
         return sn;
   }
   return -1;
}

int skill_lookup( const char *name )
{
   int sn;

   if( ( sn = bsearch_skill( name, gsn_first_spell, gsn_first_skill - 1 ) ) == -1 )
      if( ( sn = bsearch_skill( name, gsn_first_skill, gsn_first_weapon - 1 ) ) == -1 )
         if( ( sn = bsearch_skill( name, gsn_first_weapon, gsn_first_song - 1 ) ) == -1 )
            if( ( sn = bsearch_skill( name, gsn_first_song, gsn_first_tongue - 1 ) ) == -1 )
               if( ( sn = bsearch_skill( name, gsn_first_tongue, gsn_top_sn - 1 ) ) == -1 && gsn_top_sn < top_sn )
               {
                  for( sn = gsn_top_sn; sn < top_sn; sn++ )
                  {
                     if( !skill_table[sn] || !skill_table[sn]->name )
                        return -1;
                     if( LOWER( name[0] ) == LOWER( skill_table[sn]->name[0] )
                         && !str_prefix( name, skill_table[sn]->name ) )
                        return sn;
                  }
                  return -1;
               }
   return sn;
}

SKILLTYPE *get_skilltype( int sn )
{
   if( sn >= TYPE_PERSONAL )
      return NULL;
   if( sn >= TYPE_HERB )
      return IS_VALID_HERB( sn - TYPE_HERB ) ? herb_table[sn - TYPE_HERB] : NULL;
   if( sn >= TYPE_HIT )
      return NULL;
   return IS_VALID_SN( sn ) ? skill_table[sn] : NULL;
}

int bsearch_skill_prefix( const char *name, int first, int top )
{
   int sn;

   for( ;; )
   {
      sn = ( first + top ) >> 1;
      if( !IS_VALID_SN( sn ) )
         return -1;
      if( LOWER( name[0] ) == LOWER( skill_table[sn]->name[0] ) && !str_prefix( name, skill_table[sn]->name ) )
         return sn;
      if( first >= top )
         return -1;
      if( strcmp( name, skill_table[sn]->name ) < 1 )
         top = sn - 1;
      else
         first = sn + 1;
   }
   return -1;
}

int bsearch_skill_exact( const char *name, int first, int top )
{
   int sn;

   for( ;; )
   {
      sn = ( first + top ) >> 1;
      if( !IS_VALID_SN( sn ) )
         return -1;
      if( !str_cmp( name, skill_table[sn]->name ) )
         return sn;
      if( first >= top )
         return -1;
      if( strcmp( name, skill_table[sn]->name ) < 1 )
         top = sn - 1;
      else
         first = sn + 1;
   }
   return -1;
}

int bsearch_skill( const char *name, int first, int top )
{
   int sn = bsearch_skill_exact( name, first, top );

   return ( sn == -1 ) ? bsearch_skill_prefix( name, first, top ) : sn;
}

int ch_bsearch_skill_prefix( CHAR_DATA * ch, const char *name, int first, int top )
{
   int sn;

   for( ;; )
   {
      sn = ( first + top ) >> 1;

      if( LOWER( name[0] ) == LOWER( skill_table[sn]->name[0] )
          && !str_prefix( name, skill_table[sn]->name )
          && ch->pcdata->learned[sn] > 0
          && ( ch->level >= skill_table[sn]->skill_level[ch->class] || DUAL_SKILL( ch, sn ) ) )
         return sn;
      if( first >= top )
         return -1;
      if( strcmp( name, skill_table[sn]->name ) < 1 )
         top = sn - 1;
      else
         first = sn + 1;
   }
   return -1;
}

int ch_bsearch_skill_exact( CHAR_DATA * ch, const char *name, int first, int top )
{
   int sn;

   for( ;; )
   {
      sn = ( first + top ) >> 1;

      if( !str_cmp( name, skill_table[sn]->name )
          && ch->pcdata->learned[sn] > 0
          && ( ch->level >= skill_table[sn]->skill_level[ch->class] || DUAL_SKILL( ch, sn ) ) )
         return sn;
      if( first >= top )
         return -1;
      if( strcmp( name, skill_table[sn]->name ) < 1 )
         top = sn - 1;
      else
         first = sn + 1;
   }
   return -1;
}

int ch_bsearch_skill( CHAR_DATA * ch, const char *name, int first, int top )
{
   int sn = ch_bsearch_skill_exact( ch, name, first, top );

   return ( sn == -1 ) ? ch_bsearch_skill_prefix( ch, name, first, top ) : sn;
}

int find_spell( CHAR_DATA * ch, const char *name, bool know )
{
   if( IS_NPC( ch ) || !know )
      return bsearch_skill( name, gsn_first_spell, gsn_first_skill - 1 );
   else
      return ch_bsearch_skill( ch, name, gsn_first_spell, gsn_first_skill - 1 );
}

int find_skill( CHAR_DATA * ch, const char *name, bool know )
{
   if( IS_NPC( ch ) || !know )
      return bsearch_skill( name, gsn_first_skill, gsn_first_weapon - 1 );
   else
      return ch_bsearch_skill( ch, name, gsn_first_skill, gsn_first_weapon - 1 );
}

int find_weapon( CHAR_DATA * ch, const char *name, bool know )
{
   if( IS_NPC( ch ) || !know )
      return bsearch_skill( name, gsn_first_weapon, gsn_first_song - 1 );
   else
      return ch_bsearch_skill( ch, name, gsn_first_weapon, gsn_first_song - 1 );
}

int find_song( CHAR_DATA * ch, const char *name, bool know )
{
   if( IS_NPC( ch ) || !know )
      return bsearch_skill( name, gsn_first_song, gsn_first_tongue - 1 );
   else
      return ch_bsearch_skill( ch, name, gsn_first_song, gsn_first_tongue - 1 );
}

int find_tongue( CHAR_DATA * ch, const char *name, bool know )
{
   if( IS_NPC( ch ) || !know )
      return bsearch_skill( name, gsn_first_tongue, gsn_top_sn - 1 );
   else
      return ch_bsearch_skill( ch, name, gsn_first_tongue, gsn_top_sn - 1 );
}

int slot_lookup( int slot )
{
   extern bool fBootDb;
   int sn;

   if( slot <= 0 )
      return -1;

   for( sn = 0; sn < top_sn; sn++ )
      if( slot == skill_table[sn]->slot )
         return sn;

   if( fBootDb )
   {
      bug( "Slot_lookup: bad slot %d.", slot );
      abort(  );
   }

   return -1;
}

int dispel_casting( AFFECT_DATA * paf, CHAR_DATA * ch, CHAR_DATA * victim, int affect, bool dispel )
{
   char buf[MAX_STRING_LENGTH];
   char *spell;
   SKILLTYPE *sktmp;
   bool is_mage = FALSE, has_detect = FALSE;
   EXT_BV ext_bv = meb( affect );

   if( IS_NPC( ch ) || ch->class == CLASS_MAGE )
      is_mage = TRUE;
   if( IS_AFFECTED( ch, AFF_DETECT_MAGIC ) )
      has_detect = TRUE;

   if( paf )
   {
      if( ( sktmp = get_skilltype( paf->type ) ) == NULL )
         return 0;
      spell = sktmp->name;
   }
   else
      spell = affect_bit_name( &ext_bv );

   set_char_color( AT_MAGIC, ch );
   set_char_color( AT_HITME, victim );

   if( !can_see( ch, victim ) )
      strcpy( buf, "Someone" );
   else
   {
      strcpy( buf, ( IS_NPC( victim ) ? victim->short_descr : victim->name ) );
      buf[0] = toupper( buf[0] );
   }

   if( dispel )
   {
      ch_printf( victim, "Your %s vanishes.\n\r", spell );
      if( is_mage && has_detect )
         ch_printf( ch, "%s's %s vanishes.\n\r", buf, spell );
      else
         return 0;
   }
   else
   {
      if( is_mage && has_detect )
         ch_printf( ch, "%s's %s wavers but holds.\n\r", buf, spell );
      else
         return 0;
   }
   return 1;
}

void successful_casting( SKILLTYPE * skill, CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * obj )
{
   sh_int chitroom = ( skill->type == SKILL_SPELL ? AT_MAGIC : AT_ACTION );
   sh_int chit = ( skill->type == SKILL_SPELL ? AT_MAGIC : AT_HIT );
   sh_int chitme = ( skill->type == SKILL_SPELL ? AT_MAGIC : AT_HITME );

   if( skill->target != TAR_CHAR_OFFENSIVE )
   {
      chit = chitroom;
      chitme = chitroom;
   }

   if( ch && ch != victim )
   {
      if( skill->hit_char && skill->hit_char[0] != '\0' )
      {
         if( str_cmp( skill->hit_char, SPELL_SILENT_MARKER ) )
            act( AT_COLORIZE, skill->hit_char, ch, obj, victim, TO_CHAR );
      }
      else if( skill->type == SKILL_SPELL )
         act( AT_COLORIZE, "Ok.", ch, NULL, NULL, TO_CHAR );
   }
   if( ch && skill->hit_room && skill->hit_room[0] != '\0' && str_cmp( skill->hit_room, SPELL_SILENT_MARKER ) )
      act( AT_COLORIZE, skill->hit_room, ch, obj, victim, TO_NOTVICT );
   if( ch && victim && skill->hit_vict && skill->hit_vict[0] != '\0' )
   {
      if( str_cmp( skill->hit_vict, SPELL_SILENT_MARKER ) )
      {
         if( ch != victim )
            act( AT_COLORIZE, skill->hit_vict, ch, obj, victim, TO_VICT );
         else
            act( AT_COLORIZE, skill->hit_vict, ch, obj, victim, TO_CHAR );
      }
   }
   else if( ch && ch == victim && skill->type == SKILL_SPELL )
      act( AT_COLORIZE, "Ok.", ch, NULL, NULL, TO_CHAR );
}

void failed_casting( SKILLTYPE * skill, CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * obj )
{
   sh_int chitroom = ( skill->type == SKILL_SPELL ? AT_MAGIC : AT_ACTION );
   sh_int chit = ( skill->type == SKILL_SPELL ? AT_MAGIC : AT_HIT );
   sh_int chitme = ( skill->type == SKILL_SPELL ? AT_MAGIC : AT_HITME );

   if( skill->target != TAR_CHAR_OFFENSIVE )
   {
      chit = chitroom;
      chitme = chitroom;
   }

   if( ch && ch != victim )
   {
      if( skill->miss_char && skill->miss_char[0] != '\0' )
      {
         if( str_cmp( skill->miss_char, SPELL_SILENT_MARKER ) )
            act( AT_COLORIZE, skill->miss_char, ch, obj, victim, TO_CHAR );
      }
      else if( skill->type == SKILL_SPELL )
         act( chitme, "You failed.", ch, NULL, NULL, TO_CHAR );
   }
   if( ch && skill->miss_room && skill->miss_room[0] != '\0' &&
       str_cmp( skill->miss_room, SPELL_SILENT_MARKER ) && str_cmp( skill->miss_room, "supress" ) )
      act( AT_COLORIZE, skill->miss_room, ch, obj, victim, TO_NOTVICT );
   if( ch && victim && skill->miss_vict && skill->miss_vict[0] != '\0' )
   {
      if( str_cmp( skill->miss_vict, SPELL_SILENT_MARKER ) )
      {
         if( ch != victim )
            act( AT_COLORIZE, skill->miss_vict, ch, obj, victim, TO_VICT );
         else
            act( AT_COLORIZE, skill->miss_vict, ch, obj, victim, TO_CHAR );
      }
   }
   else if( ch && ch == victim )
   {
      if( skill->miss_char && skill->miss_char[0] != '\0' )
      {
         if( str_cmp( skill->miss_char, SPELL_SILENT_MARKER ) )
            act( AT_COLORIZE, skill->miss_char, ch, obj, victim, TO_CHAR );
      }
      else if( skill->type == SKILL_SPELL )
         act( chitme, "You failed.", ch, NULL, NULL, TO_CHAR );
   }
}

void immune_casting( SKILLTYPE * skill, CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * obj )
{
   sh_int chitroom = ( skill->type == SKILL_SPELL ? AT_MAGIC : AT_ACTION );
   sh_int chit = ( skill->type == SKILL_SPELL ? AT_MAGIC : AT_HIT );
   sh_int chitme = ( skill->type == SKILL_SPELL ? AT_MAGIC : AT_HITME );

   if( skill->target != TAR_CHAR_OFFENSIVE )
   {
      chit = chitroom;
      chitme = chitroom;
   }

   if( ch && ch != victim )
   {
      if( skill->imm_char && skill->imm_char[0] != '\0' )
      {
         if( str_cmp( skill->imm_char, SPELL_SILENT_MARKER ) )
            act( AT_COLORIZE, skill->imm_char, ch, obj, victim, TO_CHAR );
      }
      else if( skill->miss_char && skill->miss_char[0] != '\0' )
      {
         if( str_cmp( skill->miss_char, SPELL_SILENT_MARKER ) )
            act( AT_COLORIZE, skill->hit_char, ch, obj, victim, TO_CHAR );
      }
      else if( skill->type == SKILL_SPELL || skill->type == SKILL_SKILL )
         act( chit, "That appears to have no effect.", ch, NULL, NULL, TO_CHAR );
   }
   if( ch && skill->imm_room && skill->imm_room[0] != '\0' )
   {
      if( str_cmp( skill->imm_room, SPELL_SILENT_MARKER ) )
         act( AT_COLORIZE, skill->imm_room, ch, obj, victim, TO_NOTVICT );
   }
   else if( ch && skill->miss_room && skill->miss_room[0] != '\0' )
   {
      if( str_cmp( skill->miss_room, SPELL_SILENT_MARKER ) )
         act( AT_COLORIZE, skill->miss_room, ch, obj, victim, TO_NOTVICT );
   }
   if( ch && victim && skill->imm_vict && skill->imm_vict[0] != '\0' )
   {
      if( str_cmp( skill->imm_vict, SPELL_SILENT_MARKER ) )
      {
         if( ch != victim )
            act( AT_COLORIZE, skill->imm_vict, ch, obj, victim, TO_VICT );
         else
            act( AT_COLORIZE, skill->imm_vict, ch, obj, victim, TO_CHAR );
      }
   }
   else if( ch && victim && skill->miss_vict && skill->miss_vict[0] != '\0' )
   {
      if( str_cmp( skill->miss_vict, SPELL_SILENT_MARKER ) )
      {
         if( ch != victim )
            act( AT_COLORIZE, skill->miss_vict, ch, obj, victim, TO_VICT );
         else
            act( AT_COLORIZE, skill->miss_vict, ch, obj, victim, TO_CHAR );
      }
   }
   else if( ch && ch == victim )
   {
      if( skill->imm_char && skill->imm_char[0] != '\0' )
      {
         if( str_cmp( skill->imm_char, SPELL_SILENT_MARKER ) )
            act( AT_COLORIZE, skill->imm_char, ch, obj, victim, TO_CHAR );
      }
      else if( skill->miss_char && skill->miss_char[0] != '\0' )
      {
         if( str_cmp( skill->hit_char, SPELL_SILENT_MARKER ) )
            act( AT_COLORIZE, skill->hit_char, ch, obj, victim, TO_CHAR );
      }
      else if( skill->type == SKILL_SPELL || skill->type == SKILL_SKILL )
         act( chit, "That appears to have no affect.", ch, NULL, NULL, TO_CHAR );
   }
}

void say_spell( CHAR_DATA * ch, int sn )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];
   CHAR_DATA *rch;
   char *pName;
   int iSyl;
   int length;
   SKILLTYPE *skill = get_skilltype( sn );

   struct syl_type
   {
      char *old;
      char *new;
   };

   static const struct syl_type syl_table[] = {
      {" ", " "},
      {"ar", "abra"},
      {"au", "kada"},
      {"bless", "fido"},
      {"blind", "nose"},
      {"bur", "mosa"},
      {"cu", "judi"},
      {"de", "oculo"},
      {"en", "unso"},
      {"light", "dies"},
      {"lo", "hi"},
      {"mor", "zak"},
      {"move", "sido"},
      {"ness", "lacri"},
      {"ning", "illa"},
      {"per", "duda"},
      {"polymorph", "iaddahs"},
      {"ra", "gru"},
      {"re", "candus"},
      {"son", "sabru"},
      {"tect", "infra"},
      {"tri", "cula"},
      {"ven", "nofo"},
      {"a", "a"}, {"b", "b"}, {"c", "q"}, {"d", "e"},
      {"e", "z"}, {"f", "y"}, {"g", "o"}, {"h", "p"},
      {"i", "u"}, {"j", "y"}, {"k", "t"}, {"l", "r"},
      {"m", "w"}, {"n", "i"}, {"o", "a"}, {"p", "s"},
      {"q", "d"}, {"r", "f"}, {"s", "g"}, {"t", "h"},
      {"u", "j"}, {"v", "z"}, {"w", "x"}, {"x", "n"},
      {"y", "l"}, {"z", "k"},
      {"", ""}
   };

   buf[0] = '\0';
   for( pName = skill->name; *pName != '\0'; pName += length )
   {
      for( iSyl = 0; ( length = strlen( syl_table[iSyl].old ) ) != 0; iSyl++ )
      {
         if( !str_prefix( syl_table[iSyl].old, pName ) )
         {
            strcat( buf, syl_table[iSyl].new );
            break;
         }
      }

      if( length == 0 )
         length = 1;
   }

   if( ch->class == 14 && skill->type == SKILL_SONG )
   {
      sprintf( buf, "$n plays the %s on $s flute.....", skill->name );
      sprintf( buf2, "$n plays the %s on $s flute.....", skill->name );
      sprintf( buf3, "You put your flute up to your lips and play the %s.....", skill->name );
      act( AT_MAGIC, buf3, ch, NULL, NULL, TO_CHAR );
   }
   else
   {
      sprintf( buf2, "$n utters the words, '%s'.", buf );
      sprintf( buf, "$n utters the words, '%s'.", skill->name );
   }

   for( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
   {
      if( rch != ch )
      {
         if( ch->class == rch->class || IS_IMMORTAL( rch ) )
            act( AT_MAGIC, buf, ch, NULL, rch, TO_VICT );
         else
            act( AT_MAGIC, buf2, ch, NULL, rch, TO_VICT );
      }
   }

   return;
}

int ris_save( CHAR_DATA * ch, int chance, int ris )
{
   sh_int modifier;

   modifier = 10;
   if( IS_SET( ch->immune, ris ) )
      modifier -= 10;
   if( IS_SET( ch->resistant, ris ) )
      modifier -= 2;
   if( IS_SET( ch->susceptible, ris ) )
   {
      if( IS_NPC( ch ) && IS_SET( ch->immune, ris ) )
         modifier += 0;
      else
         modifier += 2;
   }
   if( modifier <= 0 )
      return 1000;
   if( modifier == 10 )
      return chance;
   return ( chance * modifier ) / 10;
}

int rd_parse( CHAR_DATA * ch, int level, char *exp )
{
   int x, lop = 0, gop = 0, eop = 0;
   char operation;
   char *sexp[2];
   int total = 0, len = 0;

   if( !exp || !strlen( exp ) )
      return 0;

   if( ( *exp == '(' ) && !index( exp + 1, '(' ) && exp[strlen( exp ) - 1] == ')' )
   {
      exp[strlen( exp ) - 1] = '\0';
      exp++;
   }

   len = strlen( exp );
   if( len == 1 && isalpha( exp[0] ) )
   {
      switch ( exp[0] )
      {
         case 'L':
         case 'l':
            return level;
         case 'H':
         case 'h':
            return ch->hit;
         case 'M':
         case 'm':
            return ch->mana;
         case 'V':
         case 'v':
            return ch->move;
         case 'S':
         case 's':
            return get_curr_str( ch );
         case 'I':
         case 'i':
            return get_curr_int( ch );
         case 'W':
         case 'w':
            return get_curr_wis( ch );
         case 'X':
         case 'x':
            return get_curr_dex( ch );
         case 'C':
         case 'c':
            return get_curr_con( ch );
         case 'A':
         case 'a':
            return get_curr_cha( ch );
         case 'U':
         case 'u':
            return get_curr_lck( ch );
         case 'Y':
         case 'y':
            return get_age( ch );
      }
   }

   for( x = 0; x < len; ++x )
      if( !isdigit( exp[x] ) && !isspace( exp[x] ) )
         break;
   if( x == len )
      return atoi( exp );

   for( x = 0; x < strlen( exp ); ++x )
      switch ( exp[x] )
      {
         case '^':
            if( !total )
               eop = x;
            break;
         case '-':
         case '+':
            if( !total )
               lop = x;
            break;
         case '*':
         case '/':
         case '%':
         case 'd':
         case 'D':
         case '<':
         case '>':
         case '{':
         case '}':
         case '=':
            if( !total )
               gop = x;
            break;
         case '(':
            ++total;
            break;
         case ')':
            --total;
            break;
      }
   if( lop )
      x = lop;
   else if( gop )
      x = gop;
   else
      x = eop;
   operation = exp[x];
   exp[x] = '\0';
   sexp[0] = exp;
   sexp[1] = ( char * )( exp + x + 1 );

   total = rd_parse( ch, level, sexp[0] );
   switch ( operation )
   {
      case '-':
         total -= rd_parse( ch, level, sexp[1] );
         break;
      case '+':
         total += rd_parse( ch, level, sexp[1] );
         break;
      case '*':
         total *= rd_parse( ch, level, sexp[1] );
         break;
      case '/':
         total /= rd_parse( ch, level, sexp[1] );
         break;
      case '%':
         total %= rd_parse( ch, level, sexp[1] );
         break;
      case 'd':
      case 'D':
         total = dice( total, rd_parse( ch, level, sexp[1] ) );
         break;
      case '<':
         total = ( total < rd_parse( ch, level, sexp[1] ) );
         break;
      case '>':
         total = ( total > rd_parse( ch, level, sexp[1] ) );
         break;
      case '=':
         total = ( total == rd_parse( ch, level, sexp[1] ) );
         break;
      case '{':
         total = UMIN( total, rd_parse( ch, level, sexp[1] ) );
         break;
      case '}':
         total = UMAX( total, rd_parse( ch, level, sexp[1] ) );
         break;

      case '^':
      {
         int y = rd_parse( ch, level, sexp[1] ), z = total;

         for( x = 1; x < y; ++x, z *= total );
         total = z;
         break;
      }
   }
   return total;
}

int dice_parse( CHAR_DATA * ch, int level, char *exp )
{
   char buf[MAX_INPUT_LENGTH];

   strcpy( buf, exp );
   return rd_parse( ch, level, buf );
}

bool saves_poison_death( int level, CHAR_DATA * victim )
{
   int save;

   save = 50 + ( victim->level - level - victim->saving_poison_death ) * 5;
   save = URANGE( 5, save, 95 );
   return chance( victim, save );
}

bool saves_wands( int level, CHAR_DATA * victim )
{
   int save;

   if( IS_SET( victim->immune, RIS_MAGIC ) )
      return TRUE;

   save = 50 + ( victim->level - level - victim->saving_wand ) * 5;
   save = URANGE( 5, save, 95 );
   return chance( victim, save );
}

bool saves_para_petri( int level, CHAR_DATA * victim )
{
   int save;

   save = 50 + ( victim->level - level - victim->saving_para_petri ) * 5;
   save = URANGE( 5, save, 95 );
   return chance( victim, save );
}

bool saves_breath( int level, CHAR_DATA * victim )
{
   int save;

   save = 50 + ( victim->level - level - victim->saving_breath ) * 5;
   save = URANGE( 5, save, 95 );
   return chance( victim, save );
}

bool saves_spell_staff( int level, CHAR_DATA * victim )
{
   int save;

   if( IS_SET( victim->immune, RIS_MAGIC ) )
      return TRUE;

   if( IS_NPC( victim ) && level > 10 )
      level -= 5;
   save = 50 + ( victim->level - level - victim->saving_spell_staff ) * 5;
   save = URANGE( 5, save, 95 );
   return chance( victim, save );
}


bool process_spell_components( CHAR_DATA * ch, int sn )
{
   SKILLTYPE *skill = get_skilltype( sn );
   char *comp = skill->components;
   char *check;
   char arg[MAX_INPUT_LENGTH];
   bool consume, fail, found;
   int val, value;
   OBJ_DATA *obj;

   if( !comp || comp[0] == '\0' )
      return TRUE;

   while( comp[0] != '\0' )
   {
      comp = one_argument( comp, arg );
      consume = TRUE;
      fail = found = FALSE;
      val = -1;
      switch ( arg[1] )
      {
         default:
            check = arg + 1;
            break;
         case '!':
            check = arg + 2;
            fail = TRUE;
            break;
         case '+':
            check = arg + 2;
            consume = FALSE;
            break;
         case '@':
            check = arg + 2;
            val = 0;
            break;
         case '#':
            check = arg + 2;
            val = 1;
            break;
         case '$':
            check = arg + 2;
            val = 2;
            break;
         case '%':
            check = arg + 2;
            val = 3;
            break;
         case '^':
            check = arg + 2;
            val = 4;
            break;
         case '&':
            check = arg + 2;
            val = 5;
            break;
      }
      value = atoi( check );
      obj = NULL;
      switch ( UPPER( arg[0] ) )
      {
         case 'T':
            for( obj = ch->first_carrying; obj; obj = obj->next_content )
               if( obj->item_type == value )
               {
                  if( fail )
                  {
                     send_to_char( "Something disrupts the casting of this spell...\n\r", ch );
                     return FALSE;
                  }
                  found = TRUE;
                  break;
               }
            break;
         case 'V':
            for( obj = ch->first_carrying; obj; obj = obj->next_content )
               if( obj->pIndexData->vnum == value )
               {
                  if( fail )
                  {
                     send_to_char( "Something disrupts the casting of this spell...\n\r", ch );
                     return FALSE;
                  }
                  found = TRUE;
                  break;
               }
            break;
         case 'K':
            for( obj = ch->first_carrying; obj; obj = obj->next_content )
               if( nifty_is_name( check, obj->name ) )
               {
                  if( fail )
                  {
                     send_to_char( "Something disrupts the casting of this spell...\n\r", ch );
                     return FALSE;
                  }
                  found = TRUE;
                  break;
               }
            break;
         case 'G':
            if( ch->gold >= value )
            {
               if( fail )
               {
                  send_to_char( "Something disrupts the casting of this spell...\n\r", ch );
                  return FALSE;
               }
               else
               {
                  if( consume )
                  {
                     set_char_color( AT_GOLD, ch );
                     send_to_char( "You feel a little lighter...\n\r", ch );
                     ch->gold -= value;
                  }
                  continue;
               }
            }
            break;
         case 'H':
            if( ch->hit >= value )
            {
               if( fail )
               {
                  send_to_char( "Something disrupts the casting of this spell...\n\r", ch );
                  return FALSE;
               }
               else
               {
                  if( consume )
                  {
                     set_char_color( AT_BLOOD, ch );
                     send_to_char( "You feel a little weaker...\n\r", ch );
                     ch->hit -= value;
                     update_pos( ch );
                  }
                  continue;
               }
            }
            break;
      }
      if( fail )
         continue;
      if( !found )
      {
         send_to_char( "Something is missing...\n\r", ch );
         return FALSE;
      }
      if( obj )
      {
         if( val >= 0 && val < 6 )
         {
            separate_obj( obj );
            if( obj->value[val] <= 0 )
            {
               act( AT_MAGIC, "$p disappears in a puff of smoke!", ch, obj, NULL, TO_CHAR );
               act( AT_MAGIC, "$p disappears in a puff of smoke!", ch, obj, NULL, TO_ROOM );
               extract_obj( obj );
               return FALSE;
            }
            else if( --obj->value[val] == 0 )
            {
               act( AT_MAGIC, "$p glows briefly, then disappears in a puff of smoke!", ch, obj, NULL, TO_CHAR );
               act( AT_MAGIC, "$p glows briefly, then disappears in a puff of smoke!", ch, obj, NULL, TO_ROOM );
               extract_obj( obj );
            }
            else
               act( AT_MAGIC, "$p glows briefly and a whisp of smoke rises from it.", ch, obj, NULL, TO_CHAR );
         }
         else if( consume )
         {
            separate_obj( obj );
            act( AT_MAGIC, "$p glows brightly, then disappears in a puff of smoke!", ch, obj, NULL, TO_CHAR );
            act( AT_MAGIC, "$p glows brightly, then disappears in a puff of smoke!", ch, obj, NULL, TO_ROOM );
            extract_obj( obj );
         }
         else
         {
            int count = obj->count;

            obj->count = 1;
            act( AT_MAGIC, "$p glows briefly.", ch, obj, NULL, TO_CHAR );
            obj->count = count;
         }
      }
   }
   return TRUE;
}




int pAbort;

bool silence_locate_targets;

void *locate_targets( CHAR_DATA * ch, char *arg, int sn, CHAR_DATA ** victim, OBJ_DATA ** obj )
{
   SKILLTYPE *skill = get_skilltype( sn );
   void *vo = NULL;

   *victim = NULL;
   *obj = NULL;

   switch ( skill->target )
   {
      default:
         bug( "Do_cast: bad target for sn %d.", sn );
         return &pAbort;

      case TAR_IGNORE:
         break;

      case TAR_CHAR_OFFENSIVE:
      {
         if( arg[0] == '\0' )
         {
            if( ( *victim = who_fighting( ch ) ) == NULL )
            {
               if( !silence_locate_targets )
                  send_to_char( "Cast the spell on whom?\n\r", ch );
               return &pAbort;
            }
         }
         else
         {
            if( ( *victim = get_char_room( ch, arg ) ) == NULL )
            {
               if( !silence_locate_targets )
                  send_to_char( "They aren't here.\n\r", ch );
               return &pAbort;
            }
         }
      }
         if( sysdata.fight_lock == 1 )
         {
            send_to_char( "I am sorry, fight lock is on.\n\r", ch );
            return &pAbort;
         }


         if( !IS_NPC( ch ) && ch->pcdata->nuisance &&
             ch->pcdata->nuisance->flags > 5
             && number_percent(  ) < ( ( ( ch->pcdata->nuisance->flags - 5 ) * 8 ) + ch->pcdata->nuisance->power * 6 ) )
            *victim = ch;

         if( is_safe( ch, *victim ) )
            return &pAbort;

         if( ch == *victim )
         {
            if( SPELL_FLAG( get_skilltype( sn ), SF_NOSELF ) )
            {
               if( !silence_locate_targets )
                  send_to_char( "You can't cast this on yourself!\n\r", ch );
               return &pAbort;
            }
         }

         if( !IS_NPC( ch ) )
         {
            if( !IS_NPC( *victim ) )
            {
               if( get_timer( ch, TIMER_PKILLED ) > 0 )
               {
                  if( !silence_locate_targets )
                     send_to_char( "You have been killed in the last 5 minutes.\n\r", ch );
                  return &pAbort;
               }

               if( get_timer( *victim, TIMER_PKILLED ) > 0 )
               {
                  if( !silence_locate_targets )
                     send_to_char( "This player has been killed in the last 5 minutes.\n\r", ch );
                  return &pAbort;
               }
               if( xIS_SET( ch->act, PLR_NICE ) && ch != *victim )
               {
                  if( !silence_locate_targets )
                     send_to_char( "You are too nice to attack another player.\n\r", ch );
                  return &pAbort;
               }
               if( *victim != ch )
               {
                  if( !silence_locate_targets )
                     send_to_char( "You really shouldn't do this to another player...\n\r", ch );
                  else if( who_fighting( *victim ) != ch )
                  {
                     return &pAbort;
                  }
               }
            }

            if( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == *victim )
            {
               if( !silence_locate_targets )
                  send_to_char( "You can't do that on your own follower.\n\r", ch );
               return &pAbort;
            }
         }

         check_illegal_pk( ch, *victim );
         vo = ( void * )*victim;
         break;

      case TAR_CHAR_DEFENSIVE:
      {
         if( arg[0] == '\0' )
            *victim = ch;
         else
         {
            if( ( *victim = get_char_room( ch, arg ) ) == NULL )
            {
               if( !silence_locate_targets )
                  send_to_char( "They aren't here.\n\r", ch );
               return &pAbort;
            }
         }
      }


         if( !IS_NPC( ch ) && ch->fighting && ch->pcdata->nuisance &&
             ch->pcdata->nuisance->flags > 5
             && number_percent(  ) < ( ( ( ch->pcdata->nuisance->flags - 5 ) * 8 ) + 6 * ch->pcdata->nuisance->power ) )
            *victim = who_fighting( ch );

         if( ch == *victim && SPELL_FLAG( get_skilltype( sn ), SF_NOSELF ) )
         {
            if( !silence_locate_targets )
               send_to_char( "You can't cast this on yourself!\n\r", ch );
            return &pAbort;
         }

         vo = ( void * )*victim;
         break;

      case TAR_CHAR_SELF:
         if( arg[0] != '\0' && !nifty_is_name( arg, ch->name ) )
         {
            if( !silence_locate_targets )
               send_to_char( "You cannot cast this spell on another.\n\r", ch );
            return &pAbort;
         }

         vo = ( void * )ch;
         break;

      case TAR_OBJ_INV:
      {
         if( arg[0] == '\0' )
         {
            if( !silence_locate_targets )
               send_to_char( "What should the spell be cast upon?\n\r", ch );
            return &pAbort;
         }

         if( ( *obj = get_obj_carry( ch, arg ) ) == NULL )
         {
            if( !silence_locate_targets )
               send_to_char( "You are not carrying that.\n\r", ch );
            return &pAbort;
         }
      }

         vo = ( void * )*obj;
         break;
   }

   return vo;
}

char *target_name;
char *ranged_target_name = NULL;

void do_cast( CHAR_DATA * ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   static char staticbuf[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;
/*    OBJ_DATA *obj2; */
   void *vo = NULL;
   int mana = 0;
   int blood = 0;
   int sn;
   ch_ret retcode;
   bool dont_wait = FALSE;
   SKILLTYPE *skill = NULL;
   struct timeval time_used;
   bool cpk = FALSE;

   retcode = rNONE;

   switch ( ch->substate )
   {
      default:

         if( IS_NPC( ch ) && ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_POSSESS ) ) )
         {
            send_to_char( "You can't seem to do that right now...\n\r", ch );
            return;
         }

         if( sysdata.spell_lock == 1 )
         {
            send_to_char( "I'm sorry, global spell lock is on.\n\r", ch );
            return;
         }

         if( xIS_SET( ch->in_room->room_flags, ROOM_NO_MAGIC ) )
         {
            set_char_color( AT_MAGIC, ch );
            send_to_char( "You failed.\n\r", ch );
            return;
         }

         target_name = one_argument( argument, arg1 );
         one_argument( target_name, arg2 );

         if( arg1[0] == '\0' )
         {
            send_to_char( "Cast which what where?\n\r", ch );
            return;
         }

         if( get_trust( ch ) < LEVEL_BUILD )
         {
            if( ( sn = find_spell( ch, arg1, TRUE ) ) < 0 || ( !IS_NPC( ch ) && !DUAL_SKILL( ch, sn ) ) )
            {
               send_to_char( "&BYou can't do that.\n\r", ch );
               return;
            }
            if( ( skill = get_skilltype( sn ) ) == NULL )
            {
               send_to_char( "You can't do that right now...\n\r", ch );
               return;
            }
         }
         else
         {
            if( ( sn = skill_lookup( arg1 ) ) < 0 )
            {
               send_to_char( "We didn't create that yet...\n\r", ch );
               return;
            }
            if( sn >= MAX_SKILL )
            {
               send_to_char( "Hmm... that might hurt.\n\r", ch );
               return;
            }
            if( ( skill = get_skilltype( sn ) ) == NULL )
            {
               send_to_char( "Something is severely wrong with that one...\n\r", ch );
               return;
            }
            if( skill->type != SKILL_SPELL )
            {
               send_to_char( "That isn't a spell.\n\r", ch );
               return;
            }
            if( !skill->spell_fun )
            {
               send_to_char( "We didn't finish that one yet...\n\r", ch );
               return;
            }
         }

         if( ch->position < skill->minimum_position && !IS_NPC( ch ) )
         {
            switch ( ch->position )
            {
               default:
                  send_to_char( "You can't concentrate enough.\n\r", ch );
                  break;
               case POS_SITTING:
                  send_to_char( "You can't summon enough energy sitting down.\n\r", ch );
                  break;
               case POS_RESTING:
                  send_to_char( "You're too relaxed to cast that spell.\n\r", ch );
                  break;
               case POS_FIGHTING:
                  if( skill->minimum_position <= POS_EVASIVE )
                  {
                     send_to_char( "This fighting style is too demanding for that!\n\r", ch );
                  }
                  else
                  {
                     send_to_char( "No way!  You are still fighting!\n\r", ch );
                  }
                  break;
               case POS_DEFENSIVE:
                  if( skill->minimum_position <= POS_EVASIVE )
                  {
                     send_to_char( "This fighting style is too demanding for that!\n\r", ch );
                  }
                  else
                  {
                     send_to_char( "No way!  You are still fighting!\n\r", ch );
                  }
                  break;
               case POS_AGGRESSIVE:
                  if( skill->minimum_position <= POS_EVASIVE )
                  {
                     send_to_char( "This fighting style is too demanding for that!\n\r", ch );
                  }
                  else
                  {
                     send_to_char( "No way!  You are still fighting!\n\r", ch );
                  }
                  break;
               case POS_BERSERK:
                  if( skill->minimum_position <= POS_EVASIVE )
                  {
                     send_to_char( "This fighting style is too demanding for that!\n\r", ch );
                  }
                  else
                  {
                     send_to_char( "No way!  You are still fighting!\n\r", ch );
                  }
                  break;
               case POS_EVASIVE:
                  send_to_char( "No way!  You are still fighting!\n\r", ch );
                  break;
               case POS_SLEEPING:
                  send_to_char( "You dream about great feats of magic.\n\r", ch );
                  break;
            }
            return;
         }

         if( skill->spell_fun == spell_null )
         {
            send_to_char( "That's not a spell!\n\r", ch );
            return;
         }

         if( !skill->spell_fun )
         {
            send_to_char( "You cannot cast that... yet.\n\r", ch );
            return;
         }

         if( !IS_NPC( ch )
             && !IS_AGOD( ch )
             && skill->guild != CLASS_NONE && ( !ch->pcdata->clan || skill->guild != ch->pcdata->clan->class ) )
         {
            send_to_char( "That is only available to members of a certain guild.\n\r", ch );
            return;
         }

         if( !ch->in_room || ( skill->spell_sector && !IS_SET( skill->spell_sector, ( 1 << ch->in_room->sector_type ) ) ) )
         {
            send_to_char( "You can not cast that here.\n\r", ch );
            return;
         }

         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            if( IS_VAMPIRE( ch ) )
            {
               if( !IS_AGOD( ch ) || !IS_NPC( ch ) )
               {
                  if( skill->min_mana > 0 )
                  {
                     blood = UMAX( 5, skill->min_mana + ch->pcdata->deity->splcst );
                  }
                  else
                  {
                     blood = 0;
                  }
               }
               else
               {
                  blood = 0;
               }
            }
            else
            {
               if( !IS_AGOD( ch ) || !IS_NPC( ch ) )
               {
                  if( skill->min_mana > 0 )
                  {
                     mana = UMAX( 5, skill->min_mana + ch->pcdata->deity->splcst );
                  }
                  else
                  {
                     mana = 0;
                  }
               }
               else
               {
                  mana = 0;
               }
            }
         }
         else
         {
            if( IS_VAMPIRE( ch ) )
            {
               if( !IS_AGOD( ch ) || !IS_NPC( ch ) )
                  blood = skill->min_mana;
               else
                  blood = 0;
            }
            else
            {
               if( !IS_AGOD( ch ) || !IS_NPC( ch ) )
                  mana = skill->min_mana;
               else
                  mana = 0;
            }
         }
/*
	if ( strlen( arg2 ) < 3 )
	{
		send_to_char( "You must use a three letter target name.\n\r", ch );
		return;
	}
*/
         vo = locate_targets( ch, arg2, sn, &victim, &obj );
         if( vo == &pAbort )
            return;

         if( !IS_IMMORTAL( ch ) )
         {
            if( IS_VAMPIRE( ch ) )
            {
               if( !IS_NPC( ch ) && ch->blood < blood )
               {
                  send_to_char( "You don't have enough blood power.\n\r", ch );
                  return;
               }
            }
            else
            {
               if( !IS_NPC( ch ) && ch->mana < mana )
               {
                  send_to_char( "You don't have enough mana.\n\r", ch );
                  return;
               }
            }
         }

         if( skill->participants <= 1 )
            break;

         add_timer( ch, TIMER_DO_FUN, UMAX( skill->beats / 10, 3 ), do_cast, 1 );
         act( AT_MAGIC, "You begin to chant...", ch, NULL, NULL, TO_CHAR );
         act( AT_MAGIC, "$n begins to chant...", ch, NULL, NULL, TO_ROOM );
         sprintf( staticbuf, "%s %s", arg2, target_name );
         ch->alloc_ptr = str_dup( staticbuf );
         ch->tempnum = sn;
         return;
      case SUB_TIMER_DO_ABORT:
         DISPOSE( ch->alloc_ptr );
         if( IS_VALID_SN( ( sn = ch->tempnum ) ) )
         {
            if( ( skill = get_skilltype( sn ) ) == NULL )
            {
               send_to_char( "Something went wrong...\n\r", ch );
               bug( "do_cast: SUB_TIMER_DO_ABORT: bad sn %d", sn );
               return;
            }

            if( !IS_IMMORTAL( ch ) || !IS_NPC( ch ) )
            {
               if( !IS_NPC( ch ) && ch->pcdata->deity )
               {
                  if( IS_VAMPIRE( ch ) )
                  {
                     blood = skill->min_mana + ch->pcdata->deity->splcst;
                  }
                  else
                  {
                     mana = skill->min_mana + ch->pcdata->deity->splcst;
                  }
               }
               else
               {
                  if( IS_VAMPIRE( ch ) )
                  {
                     blood = skill->min_mana;
                  }
                  else
                  {
                     mana = skill->min_mana;
                  }
               }

               if( IS_VAMPIRE( ch ) )
                  ch->blood -= blood / 3;
               else
                  ch->mana -= mana / 3;
            }
         }
         set_char_color( AT_MAGIC, ch );
         send_to_char( "You stop chanting...\n\r", ch );
         return;
      case 1:
         sn = ch->tempnum;
         if( ( skill = get_skilltype( sn ) ) == NULL )
         {
            send_to_char( "Something went wrong...\n\r", ch );
            bug( "do_cast: substate 1: bad sn %d", sn );
            return;
         }
         if( !ch->alloc_ptr || !IS_VALID_SN( sn ) || skill->type != SKILL_SPELL )
         {
            send_to_char( "Something cancels out the spell!\n\r", ch );
            bug( "do_cast: ch->alloc_ptr NULL or bad sn (%d)", sn );
            return;
         }
         if( !IS_IMMORTAL( ch ) || !IS_NPC( ch ) )
         {
            if( ch->pcdata->deity )
            {
               if( IS_VAMPIRE( ch ) )
               {
                  blood = skill->min_mana + ch->pcdata->deity->splcst;
               }
               else
               {
                  mana = skill->min_mana + ch->pcdata->deity->splcst;
               }
            }
            else
            {
               if( IS_VAMPIRE( ch ) )
               {
                  blood = skill->min_mana;
               }
               else
               {
                  mana = skill->min_mana;
               }
            }
         }
         strcpy( staticbuf, ch->alloc_ptr );
         target_name = one_argument( staticbuf, arg2 );
         DISPOSE( ch->alloc_ptr );
         ch->substate = SUB_NONE;
         if( skill->participants > 1 )
         {
            int cnt = 1;
            CHAR_DATA *tmp;
            TIMER *t;

            for( tmp = ch->in_room->first_person; tmp; tmp = tmp->next_in_room )
               if( tmp != ch
                   && ( t = get_timerptr( tmp, TIMER_DO_FUN ) ) != NULL
                   && t->count >= 1 && t->do_fun == do_cast
                   && tmp->tempnum == sn && tmp->alloc_ptr && !str_cmp( tmp->alloc_ptr, staticbuf ) )
                  ++cnt;
            if( cnt >= skill->participants )
            {
               for( tmp = ch->in_room->first_person; tmp; tmp = tmp->next_in_room )
                  if( tmp != ch
                      && ( t = get_timerptr( tmp, TIMER_DO_FUN ) ) != NULL
                      && t->count >= 1 && t->do_fun == do_cast
                      && tmp->tempnum == sn && tmp->alloc_ptr && !str_cmp( tmp->alloc_ptr, staticbuf ) )
                  {
                     extract_timer( tmp, t );
                     act( AT_MAGIC, "Channeling your energy into $n, you help cast the spell!", ch, NULL, tmp, TO_VICT );
                     act( AT_MAGIC, "$N channels $S energy into you!", ch, NULL, tmp, TO_CHAR );
                     act( AT_MAGIC, "$N channels $S energy into $n!", ch, NULL, tmp, TO_NOTVICT );
                     learn_from_success( tmp, sn );
                     if( !IS_IMMORTAL( ch ) )
                     {
                        if( IS_VAMPIRE( ch ) )
                           tmp->blood -= blood;
                        else
                           tmp->mana -= mana;
                     }
                     tmp->substate = SUB_NONE;
                     tmp->tempnum = -1;
                     DISPOSE( tmp->alloc_ptr );
                  }
               dont_wait = TRUE;
               send_to_char( "You concentrate all the energy into a burst of mystical words!\n\r", ch );
               vo = locate_targets( ch, arg2, sn, &victim, &obj );
               if( vo == &pAbort )
                  return;
            }
            else
            {
               set_char_color( AT_MAGIC, ch );
               send_to_char( "There was not enough power for the spell to succeed...\n\r", ch );
               if( !IS_IMMORTAL( ch ) )
               {
                  if( IS_VAMPIRE( ch ) )
                     ch->blood -= blood / 2;
                  else
                     ch->mana -= mana / 2;
               }
               learn_from_failure( ch, sn );
               return;
            }
         }
   }

   if( ch->class == 14 && skill->type == SKILL_SONG )
   {
      send_to_char( "You can't cast that.\n\r", ch );
      return;
   }

   if( !IS_NPC( ch ) && !LEARNED( ch, gsn_lore ) )
   {
      send_to_char( "You don't know enough lore.\n\r", ch );
      learn_from_failure( ch, sn );
      return;
   }
   if( !IS_NPC( ch ) && !LEARNED( ch, gsn_magic_lore ) )
   {
      send_to_char( "You don't know enough magic lore.\n\r", ch );
      learn_from_failure( ch, sn );
      return;
   }

   if( str_cmp( skill->name, "ventriloquate" ) )
      say_spell( ch, sn );

   if( !dont_wait )
      WAIT_STATE( ch, skill->beats );

   if( !process_spell_components( ch, sn ) )
   {
      if( !IS_IMMORTAL( ch ) )
      {
         if( IS_VAMPIRE( ch ) )
            ch->blood -= blood / 2;
         else
            ch->mana -= mana / 2;
      }
      learn_from_failure( ch, sn );
      return;
   }

   if( !IS_NPC( ch ) && !IS_AGOD( ch ) && ( number_percent(  ) + skill->difficulty * 5 ) > ch->pcdata->learned[sn] )
   {
      switch ( number_bits( 2 ) )
      {
         case 0:
            if( ch->fighting )
               send_to_char( "This round of battle is too hectic to concentrate properly.\n\r", ch );
            else
               send_to_char( "A tickle in your nose prevents you from keeping your concentration.\n\r", ch );
            break;
         case 1:
            if( number_bits( 2 ) == 0 )
            {
               switch ( number_bits( 2 ) )
               {
                  case 0:
                     send_to_char( "A tickle in your nose prevents you from keeping your concentration.\n\r", ch );
                     break;
                  case 1:
                     send_to_char( "An itch on your leg keeps you from properly casting your spell.\n\r", ch );
                     break;
                  case 2:
                     send_to_char( "Something in your throat prevents you from uttering the proper phrase.\n\r", ch );
                     break;
                  case 3:
                     send_to_char( "A twitch in your eye disrupts your concentration for a moment.\n\r", ch );
                     break;
               }
            }
            else
               send_to_char( "Something distracts you, and you lose your concentration.\n\r", ch );
            break;
         case 2:
            if( ch->fighting )
               send_to_char( "There wasn't enough time this round to complete the casting.\n\r", ch );
            else
               send_to_char( "You get a mental block mid-way through the casting.\n\r", ch );
            break;
         case 3:
            send_to_char( "You get a mental block mid-way through the casting.\n\r", ch );
            break;
      }
      if( !IS_IMMORTAL( ch ) )
      {
         if( IS_VAMPIRE( ch ) )
            ch->blood -= blood / 2;
         else
            ch->mana -= mana / 2;
      }
      learn_from_failure( ch, sn );
      return;
   }
   else
   {
      if( !IS_IMMORTAL( ch ) )
      {
         if( IS_VAMPIRE( ch ) )
            ch->blood -= blood;
         else
            ch->mana -= mana;
      }
      if( ( ( skill->target == TAR_CHAR_DEFENSIVE
              || skill->target == TAR_CHAR_SELF ) && victim && IS_SET( victim->immune, RIS_MAGIC ) ) )
      {
         immune_casting( skill, ch, victim, NULL );
         retcode = rSPELL_FAILED;
      }
      else
      {
         start_timer( &time_used );
         retcode = ( *skill->spell_fun ) ( sn, ch->level, ch, vo );
         end_timer( &time_used );
         update_userec( &time_used, &skill->userec );
      }
   }

   if( ch->in_room && IS_SET( ch->in_room->area->flags, AFLAG_SPELLLIMIT ) )
      ch->in_room->area->curr_spell_count++;

   if( retcode == rCHAR_DIED || retcode == rERROR || char_died( ch ) )
      return;

   if( retcode != rSPELL_FAILED )
      learn_from_success( ch, sn );
   else
      learn_from_failure( ch, sn );


   if( skill->target == TAR_CHAR_OFFENSIVE && victim && !char_died( victim ) && victim != ch )
   {
      CHAR_DATA *vch, *vch_next;

      if( sysdata.fight_lock == 1 )
      {
         send_to_char( "I'm sorry, fight lock is on.\n\r", ch );
         return;
      }

      for( vch = ch->in_room->first_person; vch; vch = vch_next )
      {
         vch_next = vch->next_in_room;

         if( vch == victim )
         {
            cpk = check_illegal_pk( ch, vch );
            if( cpk )
            {
               send_to_char( "You cannot kill a non-pk char.\n\rNow time to pay the piper.\n\r", ch );
               ch->hit = -10;
               ch->position = POS_MORTAL;
               update_pos( ch );
               return;
            }
            else
            {
               if( vch->master != ch && !vch->fighting )
                  retcode = multi_hit( vch, ch, TYPE_UNDEFINED );
               break;
            }
         }
      }
   }

   return;
}

ch_ret obj_cast_spell( int sn, int level, CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * obj )
{
   void *vo;
   ch_ret retcode = rNONE;
   int levdiff = ch->level - level;
   SKILLTYPE *skill = get_skilltype( sn );
   struct timeval time_used;

   if( sn == -1 )
      return retcode;
   if( !skill || !skill->spell_fun )
   {
      bug( "Obj_cast_spell: bad sn %d.", sn );
      return rERROR;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_NO_MAGIC ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "Nothing seems to happen...\n\r", ch );
      return rNONE;
   }

   if( ( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) ) &&
       skill->target == TAR_CHAR_OFFENSIVE )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "Nothing seems to happen...\n\r", ch );
      return rNONE;
   }

   if( ( skill->target == TAR_CHAR_OFFENSIVE
         || number_bits( 7 ) == 1 ) && skill->type != SKILL_HERB && !chance( ch, 95 + levdiff ) )
   {
      switch ( number_bits( 2 ) )
      {
         case 0:
            failed_casting( skill, ch, victim, NULL );
            break;
         case 1:
            act( AT_MAGIC, "The $t spell backfires!", ch, skill->name, victim, TO_CHAR );
            if( victim )
               act( AT_MAGIC, "$n's $t spell backfires!", ch, skill->name, victim, TO_VICT );
            act( AT_MAGIC, "$n's $t spell backfires!", ch, skill->name, victim, TO_NOTVICT );
            return damage( ch, ch, number_range( 1, level ), TYPE_UNDEFINED );
         case 2:
            failed_casting( skill, ch, victim, NULL );
            break;
         case 3:
            act( AT_MAGIC, "The $t spell backfires!", ch, skill->name, victim, TO_CHAR );
            if( victim )
               act( AT_MAGIC, "$n's $t spell backfires!", ch, skill->name, victim, TO_VICT );
            act( AT_MAGIC, "$n's $t spell backfires!", ch, skill->name, victim, TO_NOTVICT );
            return damage( ch, ch, number_range( 1, level ), TYPE_UNDEFINED );
      }
      return rNONE;
   }

   target_name = "";
   switch ( skill->target )
   {
      default:
         bug( "Obj_cast_spell: bad target for sn %d.", sn );
         return rERROR;

      case TAR_IGNORE:
         vo = NULL;
         if( victim )
            target_name = victim->name;
         else if( obj )
            target_name = obj->name;
         break;

      case TAR_CHAR_OFFENSIVE:
         if( victim != ch )
         {
            if( !victim )
               victim = who_fighting( ch );
            if( !victim || ( !IS_NPC( victim ) && !IN_ARENA( victim ) ) )
            {
               send_to_char( "You can't do that.\n\r", ch );
               return rNONE;
            }
         }
         if( ch != victim && is_safe( ch, victim ) )
            return rNONE;
         vo = ( void * )victim;
         break;

      case TAR_CHAR_DEFENSIVE:
         if( victim == NULL )
            victim = ch;
         vo = ( void * )victim;
         if( skill->type != SKILL_HERB && IS_SET( victim->immune, RIS_MAGIC ) )
         {
            immune_casting( skill, ch, victim, NULL );
            return rNONE;
         }
         break;

      case TAR_CHAR_SELF:
         vo = ( void * )ch;
         if( skill->type != SKILL_HERB && IS_SET( ch->immune, RIS_MAGIC ) )
         {
            immune_casting( skill, ch, victim, NULL );
            return rNONE;
         }
         break;

      case TAR_OBJ_INV:
         if( obj == NULL )
         {
            send_to_char( "You can't do that.\n\r", ch );
            return rNONE;
         }
         vo = ( void * )obj;
         break;
   }

   start_timer( &time_used );
   retcode = ( *skill->spell_fun ) ( sn, level, ch, vo );
   end_timer( &time_used );
   update_userec( &time_used, &skill->userec );

   if( retcode == rSPELL_FAILED )
      retcode = rNONE;

   if( retcode == rCHAR_DIED || retcode == rERROR )
      return retcode;

   if( char_died( ch ) )
      return rCHAR_DIED;

   if( skill->target == TAR_CHAR_OFFENSIVE && victim != ch && !char_died( victim ) )
   {
      CHAR_DATA *vch;
      CHAR_DATA *vch_next;

      for( vch = ch->in_room->first_person; vch; vch = vch_next )
      {
         vch_next = vch->next_in_room;
         if( victim == vch && !vch->fighting && vch->master != ch )
         {
            retcode = multi_hit( vch, ch, TYPE_UNDEFINED );
            break;
         }
      }
   }

   return retcode;
}



ch_ret spell_acid_blast( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   dam = dice( level, 6 );
   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}




ch_ret spell_blindness( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   int tmp;
   SKILLTYPE *skill = get_skilltype( sn );

   if( SPELL_FLAG( skill, SF_PKSENSITIVE ) && !IS_NPC( ch ) && !IS_NPC( victim ) )
      tmp = level / 2;
   else
      tmp = level;

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }
   if( IS_AFFECTED( victim, AFF_BLIND ) || saves_spell_staff( tmp, victim ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   af.location = APPLY_HITROLL;
   af.modifier = -4;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_BLIND );
   affect_to_char( victim, &af );
   set_char_color( AT_MAGIC, victim );
   send_to_char( "You are blinded!\n\r", victim );
   if( ch != victim )
   {
      act( AT_MAGIC, "You weave a spell of blindness around $N.", ch, NULL, victim, TO_CHAR );
      act( AT_MAGIC, "$n weaves a spell of blindness about $N.", ch, NULL, victim, TO_NOTVICT );
   }
   return rNONE;
}

ch_ret spell_force_choke( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );
   int dam;

   if( !LEARNED( ch, gsn_force_lore ) )
   {
      return rSPELL_FAILED;
   }

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   level = UMAX( 0, level );
   dam = dice( 10, level );

   if( !saves_spell_staff( level, victim ) )
   {
      af.type = gsn_stun;
      af.location = APPLY_AC;
      af.modifier = 80;
      af.duration = 15;
      af.bitvector = meb( AFF_PARALYSIS );
      affect_to_char( victim, &af );
      update_pos( victim );
   }
   else
   {
      dam /= 2;
   }

   act( AT_MAGIC, "$n uses the force to choke  $N.", ch, NULL, victim, TO_ROOM );
   act( AT_MAGIC, "You use the force to choke $N.", ch, NULL, victim, TO_CHAR );

   return damage( ch, victim, dam, sn );
}

ch_ret spell_burning_hands( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   int dam;
   bool ch_died;

   ch_died = FALSE;

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return rNONE;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "Your hands heat up but nothing happens.\n\r", ch );
      return rNONE;
   }

   act( AT_FIRE, "Your hands flare with fire!", ch, NULL, NULL, TO_CHAR );
   act( AT_FIRE, "$n's hands flare with fire!", ch, NULL, NULL, TO_NOTVICT );

   for( vch = ch->in_room->first_person; vch; vch = vch_next )
   {
      vch_next = vch->next_in_room;
      if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
         continue;

      if( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
      {
         level = UMAX( 0, level );
         dam = dice( 10, level );
         if( saves_breath( level, vch ) )
            dam /= 2;
         if( vch == ch )
            dam = 0;
         if( damage( ch, vch, dam, sn ) == rCHAR_DIED || char_died( ch ) )
            ch_died = TRUE;
      }
   }
   if( ch_died )
      return rCHAR_DIED;
   else
      return rNONE;
}



ch_ret spell_call_lightning( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   int dam;
   bool ch_died;
   ch_ret retcode = rNONE;

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return rNONE;
   }

   if( !IS_OUTSIDE( ch ) )
   {
      send_to_char( "You must be out of doors.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( ch->in_room->area->weather->precip <= 0 )
   {
      send_to_char( "You need bad weather.\n\r", ch );
      return rSPELL_FAILED;
   }

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.5;

   set_char_color( AT_MAGIC, ch );
   send_to_char( "God's lightning strikes your foes!\n\r", ch );
   act( AT_MAGIC, "$n calls God's lightning to strike $s foes!", ch, NULL, NULL, TO_ROOM );

   ch_died = FALSE;
   for( vch = first_char; vch; vch = vch_next )
   {
      vch_next = vch->next;
      if( !vch->in_room )
         continue;
      if( vch->in_room == ch->in_room )
      {
         if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
            continue;

         if( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) ) )
            retcode = damage( ch, vch, saves_spell_staff( level, vch ) ? dam / 2 : dam, sn );
         if( retcode == rCHAR_DIED || char_died( ch ) )
            ch_died = TRUE;
         continue;
      }

      if( !ch_died && vch->in_room->area == ch->in_room->area && IS_OUTSIDE( vch ) && IS_AWAKE( vch ) )
      {
         if( number_bits( 3 ) == 0 )
            send_to_char_color( "&BLightning flashes in the sky.\n\r", vch );
      }
   }

   if( ch_died )
      return rCHAR_DIED;
   else
      return rNONE;
}



ch_ret spell_cause_light( int sn, int level, CHAR_DATA * ch, void *vo )
{
   return damage( ch, ( CHAR_DATA * ) vo, dice( 1, 8 ) + level / 3, sn );
}



ch_ret spell_cause_critical( int sn, int level, CHAR_DATA * ch, void *vo )
{
   return damage( ch, ( CHAR_DATA * ) vo, dice( 3, 8 ) + level - 6, sn );
}



ch_ret spell_cause_serious( int sn, int level, CHAR_DATA * ch, void *vo )
{
   return damage( ch, ( CHAR_DATA * ) vo, dice( 2, 8 ) + level / 2, sn );
}


ch_ret spell_change_sex( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }
   if( is_affected( victim, sn ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   if( af.duration < 1 )
      af.duration = 100;
   af.location = APPLY_SEX;
   do
   {
      af.modifier = number_range( 0, 1 ) - victim->sex;
   }
   while( af.modifier == 0 );
   xCLEAR_BITS( af.bitvector );
   affect_to_char( victim, &af );
   successful_casting( skill, ch, victim, NULL );
   return rNONE;
}

bool can_charm( CHAR_DATA * ch )
{
   if( IS_NPC( ch ) || IS_IMMORTAL( ch ) )
      return TRUE;
   if( ( ( get_curr_cha( ch ) / 3 ) + 1 ) > ch->pcdata->charmies )
      return TRUE;
   return FALSE;
}

ch_ret spell_charm_person( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   int chance;
   char buf[MAX_STRING_LENGTH];
   SKILLTYPE *skill = get_skilltype( sn );

   if( victim == ch )
   {
      send_to_char( "You like yourself even better!\n\r", ch );
      return rSPELL_FAILED;
   }

   if( IS_SET( victim->immune, RIS_MAGIC ) || IS_SET( victim->immune, RIS_CHARM ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( !IS_NPC( victim ) && !IS_NPC( ch ) )
   {
      send_to_char( "I don't think so...\n\r", ch );
      send_to_char( "You feel charmed...\n\r", victim );
      return rSPELL_FAILED;
   }

   chance = ris_save( victim, level, RIS_CHARM );

   if( IS_AFFECTED( victim, AFF_CHARM )
       || chance == 1000
       || IS_AFFECTED( ch, AFF_JMT )
       || IS_AFFECTED( ch, AFF_CHARM )
       || level < victim->level || circle_follow( victim, ch ) || !can_charm( ch ) || saves_spell_staff( chance, victim ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( victim->master )
      stop_follower( victim );
   add_follower( victim, ch );
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = ( sdur / 3 ) + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur / 3;
   }
   af.location = 0;
   af.modifier = 0;
   af.bitvector = meb( AFF_CHARM );
   affect_to_char( victim, &af );
   successful_casting( skill, ch, victim, NULL );

   sprintf( buf, "%s has charmed %s.", ch->name, victim->name );
   log_string_plus( buf, LOG_NORMAL, ch->level );
   if( !IS_NPC( ch ) )
      ch->pcdata->charmies++;
   if( IS_NPC( victim ) )
   {
      start_hating( victim, ch );
//      start_hunting( victim, ch );
   }
   return rNONE;
}

ch_ret spell_jedi_mind_trick( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   int chance;
   char buf[MAX_STRING_LENGTH];
   SKILLTYPE *skill = get_skilltype( sn );

   if( !LEARNED( ch, gsn_force_lore ) )
   {
      return rSPELL_FAILED;
   }

   if( victim == ch )
   {
      send_to_char( "You like yourself even better!\n\r", ch );
      return rSPELL_FAILED;
   }

   if( IS_SET( victim->immune, RIS_MAGIC ) || IS_SET( victim->immune, RIS_CHARM ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( !IS_NPC( victim ) && !IS_NPC( ch ) )
   {
      send_to_char( "I don't think so...\n\r", ch );
      send_to_char( "You feel different...\n\r", victim );
      return rSPELL_FAILED;
   }

   chance = ris_save( victim, level, RIS_CHARM );

   if( IS_AFFECTED( victim, AFF_JMT )
       || chance == 1000
       || IS_AFFECTED( ch, AFF_JMT )
       || IS_AFFECTED( ch, AFF_CHARM )
       || level < victim->level || circle_follow( victim, ch ) || !can_charm( ch ) || saves_spell_staff( chance, victim ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( victim->master )
      stop_follower( victim );
   add_follower( victim, ch );
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = ( sdur / 3 ) + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur / 3;
   }
   af.location = 0;
   af.modifier = 0;
   af.bitvector = meb( AFF_JMT );
   affect_to_char( victim, &af );
   successful_casting( skill, ch, victim, NULL );

   sprintf( buf, "%s has jedi mind tricked %s.", ch->name, victim->name );
   log_string_plus( buf, LOG_NORMAL, ch->level );
   if( !IS_NPC( ch ) )
      ch->pcdata->charmies++;
   if( IS_NPC( victim ) )
   {
      start_hating( victim, ch );
//      start_hunting( victim, ch );
   }
   return rNONE;
}

ch_ret spell_chill_touch( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.3;

   if( !saves_spell_staff( level, victim ) )
   {
      af.type = sn;
      af.duration = 14;
      af.location = APPLY_STR;
      af.modifier = -1;
      xCLEAR_BITS( af.bitvector );
      affect_join( victim, &af );
   }
   else
   {
      dam /= 2;
   }

   return damage( ch, victim, dam, sn );
}



ch_ret spell_colour_spray( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.4;

   if( saves_spell_staff( level, victim ) )
      dam /= 2;

   return damage( ch, victim, dam, sn );
}

ch_ret spell_continual_light( int sn, int level, CHAR_DATA * ch, void *vo )
{
   SKILLTYPE *skill = get_skilltype( sn );
   OBJ_INDEX_DATA *oi;
   OBJ_DATA *obj;
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;

   set_char_color( AT_IMMORT, ch );

   if( ( oi = get_obj_index( OBJ_VNUM_CLIGHT ) ) == NULL || ( obj = create_object( oi, 1 ) ) == NULL )
   {
      failed_casting( skill, ch, NULL, NULL );
      return rNONE;
   }

   if( CAN_WEAR( obj, ITEM_TAKE ) )
   {
      obj_to_char( obj, victim );
   }
   else
   {
      obj_to_room( obj, victim->in_room );
   }
   act( AT_MAGIC, "Shards of light appear on $N!", ch, NULL, victim, TO_ROOM );
   ch_printf_color( ch, "&YYou call forth shards of light to aid %s&Y.&D\n\r", PERS( victim, ch ) );
   return rNONE;
}


ch_ret spell_control_weather( int sn, int level, CHAR_DATA * ch, void *vo )
{
   SKILLTYPE *skill = get_skilltype( sn );
   WEATHER_DATA *weath;
   int change;
   weath = ch->in_room->area->weather;

   change = number_range( -rand_factor, rand_factor ) + ( ch->level * 3 ) / ( 2 * max_vector );

   if( !str_cmp( target_name, "warmer" ) )
      weath->temp_vector += change;
   else if( !str_cmp( target_name, "colder" ) )
      weath->temp_vector -= change;
   else if( !str_cmp( target_name, "wetter" ) )
      weath->precip_vector += change;
   else if( !str_cmp( target_name, "drier" ) )
      weath->precip_vector -= change;
   else if( !str_cmp( target_name, "windier" ) )
      weath->wind_vector += change;
   else if( !str_cmp( target_name, "calmer" ) )
      weath->wind_vector -= change;
   else
   {
      send_to_char( "Do you want it to get warmer, colder, wetter, " "drier, windier, or calmer?\n\r", ch );
      return rSPELL_FAILED;
   }

   weath->temp_vector = URANGE( -max_vector, weath->temp_vector, max_vector );
   weath->precip_vector = URANGE( -max_vector, weath->precip_vector, max_vector );
   weath->wind_vector = URANGE( -max_vector, weath->wind_vector, max_vector );

   successful_casting( skill, ch, NULL, NULL );
   return rNONE;
}


ch_ret spell_create_food( int sn, int level, CHAR_DATA * ch, void *vo )
{
   OBJ_DATA *mushroom;

   mushroom = create_object( get_obj_index( OBJ_VNUM_MUSHROOM ), 0 );
   mushroom->value[0] = 5 + level;
   act( AT_MAGIC, "$p suddenly appears.", ch, mushroom, NULL, TO_ROOM );
   act( AT_MAGIC, "$p suddenly appears.", ch, mushroom, NULL, TO_CHAR );
   mushroom = obj_to_room( mushroom, ch->in_room );
   return rNONE;
}


ch_ret spell_create_water( int sn, int level, CHAR_DATA * ch, void *vo )
{
   OBJ_DATA *obj = ( OBJ_DATA * ) vo;
   WEATHER_DATA *weath;
   int water;

   if( obj->item_type != ITEM_DRINK_CON )
   {
      send_to_char( "It is unable to hold water.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( obj->value[2] != LIQ_WATER && obj->value[1] != 0 )
   {
      send_to_char( "It contains some other liquid.\n\r", ch );
      return rSPELL_FAILED;
   }

   weath = ch->in_room->area->weather;

   water = UMIN( level * ( weath >= 0 ? 4 : 2 ), obj->value[0] - obj->value[1] );

   if( water > 0 )
   {
      separate_obj( obj );
      obj->value[2] = LIQ_WATER;
      obj->value[1] += water;
      if( !is_name( "water", obj->name ) )
      {
         char buf[MAX_STRING_LENGTH];

         sprintf( buf, "%s water", obj->name );
         STRFREE( obj->name );
         obj->name = STRALLOC( buf );
      }
      act( AT_MAGIC, "$p is filled.", ch, obj, NULL, TO_CHAR );
   }

   return rNONE;
}

ch_ret spell_cure_blindness( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );

   set_char_color( AT_MAGIC, ch );
   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( !is_affected( victim, gsn_blindness ) )
   {
      if( ch != victim )
         send_to_char( "You work your cure, but it has no apparent effect.\n\r", ch );
      else
         send_to_char( "You don't seem to be blind.\n\r", ch );
      return rSPELL_FAILED;
   }
   affect_strip( victim, gsn_blindness );
   set_char_color( AT_MAGIC, victim );
   send_to_char( "Your vision returns!\n\r", victim );
   if( ch != victim )
      send_to_char( "You work your cure, restoring vision.\n\r", ch );
   return rNONE;
}

ch_ret spell_sacral_divinity( int sn, int level, CHAR_DATA * ch, void *vo )
{
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( ch->alignment < 350 )
   {
      act( AT_MAGIC, "Your prayer goes unanswered.", ch, NULL, NULL, TO_CHAR );
      return rSPELL_FAILED;
   }
   if( IS_SET( ch->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }
   if( IS_AFFECTED( ch, AFF_SANCTUARY ) )
      return rSPELL_FAILED;
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_SANCTUARY );
   affect_to_char( ch, &af );
   act( AT_MAGIC, "A shroud of glittering light slowly wraps itself about $n.", ch, NULL, NULL, TO_ROOM );
   act( AT_MAGIC, "A shroud of glittering light slowly wraps itself around you.", ch, NULL, NULL, TO_CHAR );
   return rNONE;
}

ch_ret spell_aquabreath( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }
   if( IS_AFFECTED( victim, AFF_AQUA_BREATH ) )
      return rSPELL_FAILED;
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_AQUA_BREATH );
   affect_to_char( victim, &af );
   act( AT_MAGIC, "$n's aquabreath takes effect on $N.", ch, NULL, victim, TO_ROOM );
   act( AT_MAGIC, "Your aquabreath takes effect on $N.", ch, NULL, victim, TO_CHAR );
   act( AT_MAGIC, "Your lungs take on the ability to breathe water...", ch, NULL, victim, TO_VICT );
   return rNONE;
}

ch_ret spell_demonfire( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }
   if( IS_AFFECTED( victim, AFF_DEMONFIRE ) )
      return rSPELL_FAILED;

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_DEMONFIRE );
   affect_to_char( victim, &af );
   act( AT_MAGIC, "A dark flame bursts about $N.", ch, NULL, victim, TO_ROOM );
   act( AT_MAGIC, "Your demonfire takes hold of $N.", ch, NULL, victim, TO_CHAR );
   act( AT_MAGIC, "$n throws dark flame upon you.", ch, NULL, victim, TO_VICT );
   return rNONE;
}

ch_ret spell_demi( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }
   if( IS_AFFECTED( victim, AFF_DEMI ) )
      return rSPELL_FAILED;

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_DEMI );
   affect_to_char( victim, &af );
   act( AT_MAGIC, "A dark field forms about $N.", ch, NULL, victim, TO_ROOM );
   act( AT_MAGIC, "Your demi takes hold of $N.", ch, NULL, victim, TO_CHAR );
   act( AT_MAGIC, "$n forms a dark field about you.", ch, NULL, victim, TO_VICT );
   return rNONE;
}

ch_ret spell_invincibility( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }
   if( IS_AFFECTED( victim, AFF_INVINCIBILITY ) )
      return rSPELL_FAILED;

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AC;
   af.modifier = -100;
   af.bitvector = meb( AFF_INVINCIBILITY );
   affect_to_char( victim, &af );
   act( AT_MAGIC, "A light shines about $N.", ch, NULL, victim, TO_ROOM );
   act( AT_MAGIC, "Your invincibility takes hold of $N.", ch, NULL, victim, TO_CHAR );
   act( AT_MAGIC, "$n makes you feel invincible.", ch, NULL, victim, TO_VICT );
   return rNONE;
}

ch_ret spell_ironskin( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }
   if( IS_AFFECTED( victim, AFF_IRONSKIN ) )
      return rSPELL_FAILED;

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AC;
   af.modifier = -55;
   af.bitvector = meb( AFF_IRONSKIN );
   affect_to_char( victim, &af );
   act( AT_MAGIC, "$N's skin becomes hard as iron.", ch, NULL, victim, TO_ROOM );
   act( AT_MAGIC, "$N's skin becomes hard as iron.", ch, NULL, victim, TO_CHAR );
   act( AT_MAGIC, "$n turns your skin into iron.", ch, NULL, victim, TO_VICT );
   return rNONE;
}

ch_ret spell_barkskin( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }
   if( IS_AFFECTED( victim, AFF_BARKSKIN ) )
      return rSPELL_FAILED;

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AC;
   af.modifier = -45;
   af.bitvector = meb( AFF_BARKSKIN );
   affect_to_char( victim, &af );
   act( AT_MAGIC, "$N's skin becomes like bark.", ch, NULL, victim, TO_ROOM );
   act( AT_MAGIC, "Your barkskin takes hold on $N.", ch, NULL, victim, TO_CHAR );
   act( AT_MAGIC, "Your makes your skin turn to bark.", ch, NULL, victim, TO_VICT );
   return rNONE;
}

ch_ret spell_force_levitation( int sn, int level, CHAR_DATA * ch, void *vo )
{
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( !LEARNED( ch, gsn_force_lore ) )
   {
      return rSPELL_FAILED;
   }

   if( IS_SET( ch->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( ch, AFF_FLYING ) )
   {
      return rSPELL_FAILED;
   }
   af.type = sn;
   af.duration = level * 3;
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_FLYING );
   affect_to_char( ch, &af );
   act( AT_MAGIC, "$n starts to lift into the air.", ch, NULL, NULL, TO_ROOM );
   act( AT_MAGIC, "You lift up into the air.", ch, NULL, NULL, TO_CHAR );
   return rNONE;
}

ch_ret spell_dangle( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_FLYING ) )
   {
      immune_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }

   af.type = gsn_dangle;
   af.duration = level * 2;
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_FLYING );
   affect_to_char( victim, &af );

   af.type = gsn_dangle;
   af.duration = level * 2;
   af.location = APPLY_AC;
   af.modifier = 40;
   af.bitvector = meb( AFF_DANGLE );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "$N starts to dangle in the air.", ch, NULL, victim, TO_ROOM );
   act( AT_MAGIC, "You lift up $N into the air by one foot.", ch, NULL, victim, TO_CHAR );
   return rNONE;
}

ch_ret spell_expurgation( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }
   if( !is_affected( victim, gsn_poison ) )
      return rSPELL_FAILED;
   affect_strip( victim, gsn_poison );
   act( AT_MAGIC, "You speak an ancient prayer, begging your god for purification.", ch, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n speaks an ancient prayer begging $s god for purification.", ch, NULL, NULL, TO_ROOM );
   return rNONE;
}


ch_ret spell_cure_poison( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( is_affected( victim, gsn_poison ) )
   {
      affect_strip( victim, gsn_poison );
      set_char_color( AT_MAGIC, victim );
      send_to_char( "A warm feeling runs through your body.\n\r", victim );
      if( ch != victim )
      {
         act( AT_MAGIC, "A flush of health washes over $N.", ch, NULL, victim, TO_NOTVICT );
         act( AT_MAGIC, "You lift the poison from $N's body.", ch, NULL, victim, TO_CHAR );
      }
      return rNONE;
   }
   else
   {
      set_char_color( AT_MAGIC, ch );
      if( ch != victim )
         send_to_char( "You work your cure, but it has no apparent effect.\n\r", ch );
      else
         send_to_char( "You don't seem to be poisoned.\n\r", ch );
      return rSPELL_FAILED;
   }
}

ch_ret spell_cure_plague( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( is_affected( victim, gsn_plague ) )
   {
      affect_strip( victim, gsn_plague );
      set_char_color( AT_MAGIC, victim );
      send_to_char( "A warm feeling runs through your body.\n\r", victim );
      if( ch != victim )
      {
         act( AT_MAGIC, "A flush of health washes over $N.", ch, NULL, victim, TO_NOTVICT );
         act( AT_MAGIC, "You lift the poison from $N's body.", ch, NULL, victim, TO_CHAR );
      }
      return rNONE;
   }
   else
   {
      set_char_color( AT_MAGIC, ch );
      if( ch != victim )
         send_to_char( "You work your cure, but it has no apparent effect.\n\r", ch );
      else
         send_to_char( "You don't seem to be poisoned.\n\r", ch );
      return rSPELL_FAILED;
   }
}

ch_ret spell_curse( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_CURSE ) || saves_spell_staff( level, victim ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_HITROLL;
   af.modifier = -1;
   af.bitvector = meb( AFF_CURSE );
   affect_to_char( victim, &af );

   af.location = APPLY_SAVING_SPELL;
   af.modifier = 1;
   affect_to_char( victim, &af );

   set_char_color( AT_MAGIC, victim );
   send_to_char( "You feel unclean.\n\r", victim );
   if( ch != victim )
   {
      act( AT_MAGIC, "You utter a curse upon $N.", ch, NULL, victim, TO_CHAR );
      act( AT_MAGIC, "$n utters a curse upon $N.", ch, NULL, victim, TO_NOTVICT );
   }
   return rNONE;
}

ch_ret spell_headache( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) || IS_SET( victim->immune, RIS_PSIONIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_HEADACHE ) || saves_spell_staff( level, victim ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AC;
   af.modifier = 4 * level;
   af.bitvector = meb( AFF_HEADACHE );
   affect_to_char( victim, &af );

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_HITROLL;
   af.modifier = -100;
   af.bitvector = meb( AFF_HEADACHE );
   affect_to_char( victim, &af );

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_DAMROLL;
   af.modifier = -100;
   af.bitvector = meb( AFF_HEADACHE );
   affect_to_char( victim, &af );

   WAIT_STATE( victim, PULSE_VIOLENCE * 2 );

   set_char_color( AT_MAGIC, victim );
   send_to_char( "You feel intesne pain in your head.\n\r", victim );
   if( ch != victim )
   {
      act( AT_MAGIC, "You cause $N to writhe with pain.", ch, NULL, victim, TO_CHAR );
      act( AT_MAGIC, "$n concentrates deeply....", ch, NULL, victim, TO_NOTVICT );
   }
   return rNONE;
}

ch_ret spell_holyaura( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_HOLYAURA ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }
   af.type = gsn_holyaura;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_HOLYAURA );
   af.location = APPLY_HITROLL;
   af.modifier = 2000;
   affect_to_char( victim, &af );

   af.type = gsn_holyaura;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_HOLYAURA );
   af.location = APPLY_DAMROLL;
   af.modifier = 2000;
   affect_to_char( victim, &af );

   af.type = gsn_holyaura;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_HOLYAURA );
   af.location = APPLY_HIT;
   af.modifier = 10000;
   affect_to_char( victim, &af );

   af.type = gsn_holyaura;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_HOLYAURA );
   af.location = APPLY_MANA;
   af.modifier = 10000;
   affect_to_char( victim, &af );

   af.type = gsn_holyaura;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_HOLYAURA );
   af.location = APPLY_MOVE;
   af.modifier = 10000;
   affect_to_char( victim, &af );

   act( AT_IMMORT, "$N's aura flares with power!", ch, NULL, victim, TO_ROOM );
   act( AT_IMMORT, "$N's aura flares with power!", ch, NULL, victim, TO_CHAR );
   return rNONE;
}

ch_ret spell_purity( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( ch->practice < 100 )
   {
      send_to_char( "You don't have enough practices.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      send_to_char( "You feel drained.\n\r", ch );
      ch->practice -= 25;
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_PURITY ) )
   {
      send_to_char( "You feel drained.\n\r", ch );
      ch->practice -= 50;
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }
   af.type = gsn_purity;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_PURITY );
   af.location = APPLY_SAVING_SPELL;
   af.modifier = -20;
   affect_to_char( victim, &af );

   af.type = gsn_purity;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_PURITY );
   af.location = APPLY_AC;
   af.modifier = -100;
   affect_to_char( victim, &af );

   af.type = gsn_purity;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_PURITY );
   af.location = APPLY_HIT;
   af.modifier = 100;
   affect_to_char( victim, &af );

   af.type = gsn_purity;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_PURITY );
   af.location = APPLY_MANA;
   af.modifier = 100;
   affect_to_char( victim, &af );

   af.type = gsn_purity;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_PURITY );
   af.location = APPLY_MOVE;
   af.modifier = -200;
   affect_to_char( victim, &af );

   ch->practice -= 100;

   act( AT_IMMORT, "$N is cleansed of impurities.", ch, NULL, victim, TO_ROOM );
   act( AT_IMMORT, "$N is cleansed of impurities.", ch, NULL, victim, TO_CHAR );
   return rNONE;
}

ch_ret spell_detect_poison( int sn, int level, CHAR_DATA * ch, void *vo )
{
   OBJ_DATA *obj = ( OBJ_DATA * ) vo;

   set_char_color( AT_MAGIC, ch );
   if( obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD || obj->item_type == ITEM_COOK )
   {
      if( obj->item_type == ITEM_COOK && obj->value[2] == 0 )
         send_to_char( "It looks undercooked.\n\r", ch );
      else if( obj->value[3] != 0 )
         send_to_char( "You smell poisonous fumes.\n\r", ch );
      else
         send_to_char( "It looks very delicious.\n\r", ch );
   }
   else
   {
      send_to_char( "It doesn't look poisoned.\n\r", ch );
   }

   return rNONE;
}


ch_ret spell_dispel_evil( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;
   SKILLTYPE *skill = get_skilltype( sn );

   if( !IS_NPC( ch ) && IS_EVIL( ch ) )
      victim = ch;

   if( IS_GOOD( victim ) )
   {
      act( AT_MAGIC, "Thoric protects $N.", ch, NULL, victim, TO_ROOM );
      return rSPELL_FAILED;
   }

   if( IS_NEUTRAL( victim ) )
   {
      act( AT_MAGIC, "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
      return rSPELL_FAILED;
   }

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   dam = dice( level, 4 );
   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}


ch_ret spell_dispel_magic( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int cnt = 0, affect_num, affected_by = 0, times = 0;
   int chance;
   SKILLTYPE *skill = get_skilltype( sn );
   AFFECT_DATA *paf;
   bool found = FALSE, twice = FALSE, three = FALSE;
   bool is_mage = FALSE;

   set_char_color( AT_MAGIC, ch );

   chance = ( get_curr_int( ch ) - get_curr_int( victim ) );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_NPC( ch ) || ch->class == CLASS_MAGE )
      is_mage = TRUE;

   if( is_mage )
      chance += 5;
   else
      chance -= 15;


   if( ch == victim )
   {
      if( ch->first_affect )
      {
         send_to_char( "You pass your hands around your body...\n\r", ch );
         while( ch->first_affect )
            affect_remove( ch, ch->first_affect );
         if( !IS_NPC( ch ) )
            update_aris( victim );
         return rNONE;
      }
      else
      {
         send_to_char( "You pass your hands around your body...\n\r", ch );
         return rNONE;
      }
   }
   if( !is_mage && !IS_AFFECTED( ch, AFF_DETECT_MAGIC ) )
   {
      send_to_char( "You don't sense a magical aura to dispel.\n\r", ch );
      return rERROR;
   }

   if( number_percent(  ) > ( 75 - chance ) )
   {
      twice = TRUE;
      if( number_percent(  ) > ( 75 - chance ) )
         three = TRUE;
   }

 start_loop:

   if( IS_NPC( victim ) && !xIS_EMPTY( victim->affected_by ) )
   {
      for( ;; )
      {
         affected_by = number_range( 0, MAX_AFFECTED_BY - 1 );
         if( xIS_SET( victim->affected_by, affected_by ) )
         {
            found = TRUE;
            break;
         }
         if( cnt++ > 30 )
         {
            found = FALSE;
            break;
         }
      }
      if( found )
      {
         for( paf = victim->first_affect; paf; paf = paf->next )
            if( xIS_SET( paf->bitvector, affected_by ) )
               break;
         if( paf )
         {

            if( level < victim->level || saves_spell_staff( level, victim ) )
            {
               if( !dispel_casting( paf, ch, victim, FALSE, FALSE ) )
                  failed_casting( skill, ch, victim, NULL );
               return rSPELL_FAILED;
            }
            if( SPELL_FLAG( get_skilltype( paf->type ), SF_NODISPEL ) )
            {
               if( !dispel_casting( paf, ch, victim, FALSE, FALSE ) )
                  failed_casting( skill, ch, victim, NULL );
               return rSPELL_FAILED;
            }
            if( !dispel_casting( paf, ch, victim, FALSE, TRUE ) && times == 0 )
               successful_casting( skill, ch, victim, NULL );
            affect_remove( victim, paf );
            if( ( twice && times < 1 ) || ( three && times < 2 ) )
            {
               times++;
               goto start_loop;
            }
            return rNONE;
         }
         else
         {
            if( level < victim->level || saves_spell_staff( level, victim ) )
            {
               if( !dispel_casting( NULL, ch, victim, affected_by, FALSE ) )
                  failed_casting( skill, ch, victim, NULL );
               return rSPELL_FAILED;
            }
            if( !dispel_casting( NULL, ch, victim, affected_by, TRUE ) && times == 0 )
               successful_casting( skill, ch, victim, NULL );
            xREMOVE_BIT( victim->affected_by, affected_by );
            if( ( twice && times < 1 ) || ( three && times < 2 ) )
            {
               times++;
               goto start_loop;
            }
            return rNONE;
         }
      }
   }


   if( !victim->first_affect )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   cnt = 0;


   for( paf = victim->first_affect; paf; paf = paf->next )
      cnt++;

   paf = victim->first_affect;

   for( affect_num = number_range( 0, ( cnt - 1 ) ); affect_num > 0; affect_num-- )
      paf = paf->next;

   if( level < victim->level || saves_spell_staff( level, victim ) )
   {
      if( !dispel_casting( paf, ch, victim, FALSE, FALSE ) )
         failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( !paf || SPELL_FLAG( get_skilltype( paf->type ), SF_NODISPEL ) )
   {
      if( !dispel_casting( paf, ch, victim, FALSE, FALSE ) )
         failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }
   if( !dispel_casting( paf, ch, victim, FALSE, TRUE ) && times == 0 )
      successful_casting( skill, ch, victim, NULL );
   affect_remove( victim, paf );
   if( ( twice && times < 1 ) || ( three && times < 2 ) )
   {
      times++;
      goto start_loop;
   }

   if( !IS_NPC( victim ) )
      update_aris( victim );
   return rNONE;
}



ch_ret spell_polymorph( int sn, int level, CHAR_DATA * ch, void *vo )
{
   MORPH_DATA *morph;
   SKILLTYPE *skill = get_skilltype( sn );

   morph = find_morph( ch, target_name, TRUE );
   if( !morph )
   {
      send_to_char( "You can't morph into anything like that!\n\r", ch );
      return rSPELL_FAILED;
   }
   if( !do_morph_char( ch, morph ) )
   {
      failed_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }
   return rNONE;
}

ch_ret spell_earthquake( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   int dam;
   bool ch_died;
   ch_ret retcode;
   SKILLTYPE *skill = get_skilltype( sn );

   ch_died = FALSE;
   retcode = rNONE;

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return rNONE;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
   {
      failed_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }

   act( AT_MAGIC, "The earth trembles beneath your feet!", ch, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n makes the earth tremble and shiver.", ch, NULL, NULL, TO_ROOM );

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.5;

   for( vch = first_char; vch; vch = vch_next )
   {
      vch_next = vch->next;
      if( !vch->in_room )
         continue;
      if( vch->in_room == ch->in_room )
      {
         if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
            continue;

         if( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
             && !IS_AFFECTED( vch, AFF_FLYING ) && !IS_AFFECTED( vch, AFF_FLOATING ) )
            retcode = damage( ch, vch, dam, sn );
         if( retcode == rCHAR_DIED || char_died( ch ) )
         {
            ch_died = TRUE;
            continue;
         }
         if( char_died( vch ) )
            continue;
      }

      if( !ch_died && vch->in_room->area == ch->in_room->area )
      {
         if( number_bits( 3 ) == 0 )
            send_to_char_color( "&BThe earth trembles and shivers.\n\r", vch );
      }
   }

   if( ch_died )
      return rCHAR_DIED;
   else
      return rNONE;
}


ch_ret spell_energy_drain( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;
   int chance;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   chance = ris_save( victim, victim->level, RIS_DRAIN );
   if( chance == 1000 || saves_spell_staff( chance, victim ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   ch->alignment = UMAX( -1000, ch->alignment - 200 );
   if( victim->level <= 2 )
      dam = ch->hit + 1;
   else
   {
      gain_exp( victim, 0 - number_range( level / 2, 3 * level / 2 ) );
      victim->mana /= 2;
      victim->move /= 2;
      dam = dice( 1, level );
      ch->hit += dam;
   }

   if( ch->hit > ch->max_hit )
      ch->hit = ch->max_hit;
   return damage( ch, victim, dam, sn );
}



ch_ret spell_force_heal( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );
   int heal;

   if( !LEARNED( ch, gsn_force_lore ) )
   {
      return rSPELL_FAILED;
   }

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   level = UMAX( 0, level );
   heal = dice( 10, level ) / 3.7;

   victim->hit = URANGE( 0, victim->hit + heal, victim->max_hit );
   update_pos( victim );
   act( AT_MAGIC, "You summon up the force to help $N.", ch, NULL, victim, TO_CHAR );
   act( AT_MAGIC, "$n summons up the force to help $N", ch, NULL, victim, TO_ROOM );

   return rNONE;
}

ch_ret spell_blackflame( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   int chance;
   int dam;
   bool first = TRUE;

   chance = number_range( 1, 100 );
   if( chance <= 40 )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "Your magic fails to take hold.\n\r", ch );
      return rSPELL_FAILED;
   }
   if( IS_AFFECTED( victim, AFF_BLACKFLAME ) )
      first = FALSE;
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_BLACKFLAME );
   affect_join( victim, &af );
   set_char_color( AT_GREEN, victim );
   send_to_char( "You feel hot.\n\r", victim );
   if( ch != victim )
   {
      act( AT_GREEN, "$N starts to sweat as your black flame spreads over $S body.", ch, NULL, victim, TO_CHAR );
      act( AT_GREEN, "$N starts to sweat as $n's black flame spreads over $S body.", ch, NULL, victim, TO_NOTVICT );
   }

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.7;

   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_fireball( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.7;

   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_firechain( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   int dam;
   bool ch_died;
   ch_ret retcode = rNONE;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.8;

   set_char_color( AT_MAGIC, ch );
   send_to_char( "God's flame strikes your foes!\n\r", ch );
   act( AT_MAGIC, "$n calls God's flame to strike $s foes!", ch, NULL, NULL, TO_ROOM );

   ch_died = FALSE;
   for( vch = first_char; vch; vch = vch_next )
   {
      vch_next = vch->next;
      if( !vch->in_room )
         continue;
      if( vch->in_room == ch->in_room )
      {
         if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
            continue;

         if( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) ) )
            retcode = damage( ch, vch, saves_spell_staff( level, vch ) ? dam / 2 : dam, sn );
         if( retcode == rCHAR_DIED || char_died( ch ) )
            ch_died = TRUE;
         continue;
      }

      if( !ch_died && vch->in_room->area == ch->in_room->area && IS_OUTSIDE( vch ) && IS_AWAKE( vch ) )
      {
//     if ( number_bits( 3 ) == 0 )
         send_to_char_color( "&BThe sky burns red.\n\r", vch );
      }
   }

   if( ch_died )
      return rCHAR_DIED;
   else
      return rNONE;
}

ch_ret spell_icechain( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   int dam;
   bool ch_died;
   ch_ret retcode = rNONE;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.8;

   set_char_color( AT_MAGIC, ch );
   send_to_char( "God's frost strikes your foes!\n\r", ch );
   act( AT_MAGIC, "$n calls God's frost to strike $s foes!", ch, NULL, NULL, TO_ROOM );

   ch_died = FALSE;
   for( vch = first_char; vch; vch = vch_next )
   {
      vch_next = vch->next;
      if( !vch->in_room )
         continue;
      if( vch->in_room == ch->in_room )
      {
         if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
            continue;

         if( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) ) )
            retcode = damage( ch, vch, saves_spell_staff( level, vch ) ? dam / 2 : dam, sn );
         if( retcode == rCHAR_DIED || char_died( ch ) )
            ch_died = TRUE;
         continue;
      }

      if( !ch_died && vch->in_room->area == ch->in_room->area && IS_OUTSIDE( vch ) && IS_AWAKE( vch ) )
      {
//     if ( number_bits( 3 ) == 0 )
         send_to_char_color( "&BThe air chills strangely.\n\r", vch );
      }
   }

   if( ch_died )
      return rCHAR_DIED;
   else
      return rNONE;
}

ch_ret spell_chainlightning( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   int dam;
   bool ch_died;
   ch_ret retcode = rNONE;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.8;

   set_char_color( AT_MAGIC, ch );
   send_to_char( "God's lightning strikes your foes!\n\r", ch );
   act( AT_MAGIC, "$n calls God's lightning to strike $s foes!", ch, NULL, NULL, TO_ROOM );

   ch_died = FALSE;
   for( vch = first_char; vch; vch = vch_next )
   {
      vch_next = vch->next;
      if( !vch->in_room )
         continue;
      if( vch->in_room == ch->in_room )
      {
         if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
            continue;

         if( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) ) )
            retcode = damage( ch, vch, saves_spell_staff( level, vch ) ? dam / 2 : dam, sn );
         if( retcode == rCHAR_DIED || char_died( ch ) )
            ch_died = TRUE;
         continue;
      }

      if( !ch_died && vch->in_room->area == ch->in_room->area && IS_OUTSIDE( vch ) && IS_AWAKE( vch ) )
      {
//     if ( number_bits( 3 ) == 0 )
         send_to_char_color( "&BLightning flashes in the sky.\n\r", vch );
      }
   }

   if( ch_died )
      return rCHAR_DIED;
   else
      return rNONE;
}


ch_ret spell_flamestrike( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.5;

   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_demistrike( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.5;

   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_plasmastrike( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.7;

   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}



ch_ret spell_faerie_fire( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_FAERIE_FIRE ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AC;
   af.modifier = 2 * level;
   af.bitvector = meb( AFF_FAERIE_FIRE );
   affect_to_char( victim, &af );
   act( AT_PINK, "You are surrounded by a pink outline.", victim, NULL, NULL, TO_CHAR );
   act( AT_PINK, "$n is surrounded by a pink outline.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}



ch_ret spell_faerie_fog( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *ich;

   act( AT_MAGIC, "$n conjures a cloud of purple smoke.", ch, NULL, NULL, TO_ROOM );
   act( AT_MAGIC, "You conjure a cloud of purple smoke.", ch, NULL, NULL, TO_CHAR );

   for( ich = ch->in_room->first_person; ich; ich = ich->next_in_room )
   {
      if( !IS_NPC( ich ) && xIS_SET( ich->act, PLR_WIZINVIS ) )
         continue;

      if( IS_NPC( ich ) && xIS_SET( ich->act, ACT_MOBINVIS ) )
         continue;

      if( ich == ch || saves_spell_staff( level, ich ) )
         continue;

      affect_strip( ich, gsn_invis );
      affect_strip( ich, gsn_mass_invis );
      affect_strip( ich, gsn_sneak );
      xREMOVE_BIT( ich->affected_by, AFF_HIDE );
      xREMOVE_BIT( ich->affected_by, AFF_INVISIBLE );
      xREMOVE_BIT( ich->affected_by, AFF_SNEAK );
      act( AT_MAGIC, "$n is revealed!", ich, NULL, NULL, TO_ROOM );
      act( AT_MAGIC, "You are revealed!", ich, NULL, NULL, TO_CHAR );
   }
   return rNONE;
}


ch_ret spell_gate( int sn, int level, CHAR_DATA * ch, void *vo )
{
   MOB_INDEX_DATA *temp;
   if( ( temp = get_mob_index( MOB_VNUM_VAMPIRE ) ) == NULL )
   {
      bug( "Spell_gate: Vampire vnum %d doesn't exist.", MOB_VNUM_VAMPIRE );
      return rSPELL_FAILED;
   }
   char_to_room( create_mobile( temp ), ch->in_room );
   return rNONE;
}


ch_ret spell_harm( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.5;

   if( saves_spell_staff( level, victim ) )
      dam /= 4;
   return damage( ch, victim, dam, sn );
}


ch_ret spell_identify( int sn, int level, CHAR_DATA * ch, void *vo )
{
   ROOM_INDEX_DATA *room;
   OBJ_DATA *obj;
   CHAR_DATA *victim;
   AFFECT_DATA *paf;
   SKILLTYPE *sktmp;
   SKILLTYPE *skill = get_skilltype( sn );
   char *name;
   char p[MAX_STRING_LENGTH];

   if( target_name[0] == '\0' )
   {
      send_to_char( "What should the spell be cast upon?\n\r", ch );
      return rSPELL_FAILED;
   }

   if( ( obj = get_obj_carry( ch, target_name ) ) != NULL )
   {
      set_char_color( AT_LBLUE, ch );
      ch_printf( ch, "\n\r&CObject '%s&C' is %s", obj->short_descr, aoran( item_type_name( obj ) ) );
      if( obj->item_type != ITEM_LIGHT && obj->wear_flags - 1 > 0 )
         ch_printf( ch, ", with wear location:  %s\n\r", flag_string( obj->wear_flags - 1, w_flags ) );
      else
         send_to_char( ".\n\r", ch );
      ch_printf( ch,
                 "Special properties:  %s\n\rIts weight is %d lbs., and level is %d.",
                 extra_bit_name( &obj->extra_flags ), obj->weight, obj->level );
      if( IS_AGOD( ch ) )
         ch_printf( ch, " VNUM %d\n\r", obj->pIndexData->vnum );
      else
         send_to_char( "\n\r", ch );
      ch_printf( ch, "Its value is: " );
      ch_printf( ch, "%d Katyr\n\r", obj->cost );
      if( obj->owner && obj->owner != '\0' )
      {
         ch_printf( ch, "This item is owned by %s.&D\n\r", obj->owner );
      }
      set_char_color( AT_MAGIC, ch );

      if( LEARNED( ch, gsn_lore ) )
      {
         switch ( obj->item_type )
         {
            case ITEM_CONTAINER:
               ch_printf( ch, "%s can carry %d items.\n\r", capitalize( obj->short_descr ), obj->value[0] );
               break;

            case ITEM_QUIVER:
               ch_printf( ch, "%s can carry %d arrows.\n\r", capitalize( obj->short_descr ), obj->value[0] );
               break;

            case ITEM_PILL:
            case ITEM_SCROLL:
            case ITEM_POTION:
               if( LEARNED( ch, gsn_magic_lore ) )
               {
                  ch_printf( ch, "Level %d spells of:", obj->value[0] );

                  if( obj->value[1] >= 0 && ( sktmp = get_skilltype( obj->value[1] ) ) != NULL )
                  {
                     send_to_char( " '", ch );
                     send_to_char( sktmp->name, ch );
                     send_to_char( "'", ch );
                  }

                  if( obj->value[2] >= 0 && ( sktmp = get_skilltype( obj->value[2] ) ) != NULL )
                  {
                     send_to_char( " '", ch );
                     send_to_char( sktmp->name, ch );
                     send_to_char( "'", ch );
                  }

                  if( obj->value[3] >= 0 && ( sktmp = get_skilltype( obj->value[3] ) ) != NULL )
                  {
                     send_to_char( " '", ch );
                     send_to_char( sktmp->name, ch );
                     send_to_char( "'", ch );
                  }

                  send_to_char( ".\n\r", ch );
               }
               break;

            case ITEM_SALVE:
               if( LEARNED( ch, gsn_magic_lore ) )
               {
                  ch_printf( ch, "Has %d(%d) applications of level %d", obj->value[1], obj->value[2], obj->value[0] );
                  if( obj->value[4] >= 0 && ( sktmp = get_skilltype( obj->value[4] ) ) != NULL )
                  {
                     send_to_char( " '", ch );
                     send_to_char( sktmp->name, ch );
                     send_to_char( "'", ch );
                  }
                  if( obj->value[5] >= 0 && ( sktmp = get_skilltype( obj->value[5] ) ) != NULL )
                  {
                     send_to_char( " '", ch );
                     send_to_char( sktmp->name, ch );
                     send_to_char( "'", ch );
                  }
                  send_to_char( ".\n\r", ch );
               }
               break;

            case ITEM_WAND:
            case ITEM_STAFF:
               if( LEARNED( ch, gsn_magic_lore ) )
               {
                  ch_printf( ch, "Has %d(%d) charges of level %d", obj->value[1], obj->value[2], obj->value[0] );

                  if( obj->value[3] >= 0 && ( sktmp = get_skilltype( obj->value[3] ) ) != NULL )
                  {
                     send_to_char( " '", ch );
                     send_to_char( sktmp->name, ch );
                     send_to_char( "'", ch );
                  }

                  send_to_char( ".\n\r", ch );
               }
               break;

            case ITEM_WEAPON:
               if( LEARNED( ch, gsn_weapon_lore ) )
               {
                  ch_printf( ch, "Damage is %d to %d (average %d)%s\n\r",
                             obj->value[1], obj->value[2],
                             ( obj->value[1] + obj->value[2] ) / 2,
                             IS_OBJ_STAT( obj, ITEM_POISONED ) ? ", and is poisonous." : "." );
                  ch_printf( ch, "Skill needed: %s\n\r", weapon_skills[obj->value[4]] );
                  ch_printf( ch, "Damage type:  %s\n\r", attack_table[obj->value[3]] );
               }
               break;

            case ITEM_MISSILE_WEAPON:
               if( LEARNED( ch, gsn_weapon_lore ) )
               {
                  ch_printf( ch, "Bonus damage added to projectiles is %d to %d (average %d).\n\r",
                             obj->value[1], obj->value[2], ( obj->value[1] + obj->value[2] ) / 2 );
                  ch_printf( ch, "Skill needed:      %s\n\r", weapon_skills[obj->value[4]] );
                  ch_printf( ch, "Projectiles fired: %s\n\r", projectiles[obj->value[5]] );
               }
               break;

            case ITEM_PROJECTILE:
               if( LEARNED( ch, gsn_weapon_lore ) )
               {
                  ch_printf( ch, "Damage is %d to %d (average %d)%s\n\r",
                             obj->value[1], obj->value[2],
                             ( obj->value[1] + obj->value[2] ) / 2,
                             IS_OBJ_STAT( obj, ITEM_POISONED ) ? ", and is poisonous." : "." );
                  ch_printf( ch, "Damage type: %s\n\r", attack_table[obj->value[3]] );
                  ch_printf( ch, "Projectile type: %s\n\r", projectiles[obj->value[4]] );
               }
               break;

            case ITEM_ARMOR:
               if( LEARNED( ch, gsn_armor_lore ) )
               {
                  ch_printf( ch, "Base AC is %d.\n\r", obj->value[0] );
               }
               break;

            case ITEM_FLUTE:
               ch_printf( ch, "Improves innate defense by AC %d.\n\r", obj->value[0] );
               break;

            case ITEM_KEY:
               if( ( room = get_room_index( obj->value[0] ) ) == NULL )
               {
                  sprintf( p, "Unknown" );
               }
               else
               {
                  sprintf( p, "%s", room->name );
               }
               ch_printf( ch, "This key opens something in the room, %s.\n\r", p );
               break;
         }

         for( paf = obj->pIndexData->first_affect; paf; paf = paf->next )
            showaffect( ch, paf );

         for( paf = obj->first_affect; paf; paf = paf->next )
            showaffect( ch, paf );
      }
      return rNONE;
   }
   else if( ( victim = get_char_room( ch, target_name ) ) != NULL )
   {

      if( IS_SET( victim->immune, RIS_MAGIC ) )
      {
         immune_casting( skill, ch, victim, NULL );
         return rSPELL_FAILED;
      }


      if( victim->morph && victim->morph->morph )
         name = capitalize( victim->morph->morph->short_desc );
      else if( IS_NPC( victim ) )
         name = capitalize( victim->short_descr );
      else
         name = victim->name;

      ch_printf( ch, "%s appears to be between level %d and %d.\n\r",
                 name, victim->level - ( victim->level % 5 ), victim->level - ( victim->level % 5 ) + 5 );

      if( IS_NPC( victim ) && victim->morph )
         ch_printf( ch, "%s appears to truly be %s.\n\r",
                    name, ( ch->level > victim->level + 10 ) ? victim->name : "someone else" );

      ch_printf( ch, "%s looks like %s, and follows the ways of the %s.\n\r",
                 name, aoran( get_race( victim ) ), get_class( victim ) );

      if( ( chance( ch, 50 ) && ch->level >= victim->level + 10 ) || IS_IMMORTAL( ch ) )
      {
         ch_printf( ch, "%s appears to be affected by: ", name );

         if( !victim->first_affect )
         {
            send_to_char( "nothing.\n\r", ch );
            return rNONE;
         }

         for( paf = victim->first_affect; paf; paf = paf->next )
         {
            if( victim->first_affect != victim->last_affect )
            {
               if( paf != victim->last_affect && ( sktmp = get_skilltype( paf->type ) ) != NULL )
                  ch_printf( ch, "%s, ", sktmp->name );

               if( paf == victim->last_affect && ( sktmp = get_skilltype( paf->type ) ) != NULL )
               {
                  ch_printf( ch, "and %s.\n\r", sktmp->name );
                  return rNONE;
               }
            }
            else
            {
               if( ( sktmp = get_skilltype( paf->type ) ) != NULL )
                  ch_printf( ch, "%s.\n\r", sktmp->name );
               else
                  send_to_char( "\n\r", ch );
               return rNONE;
            }
         }
      }
   }

   else
   {
      ch_printf( ch, "You can't find %s!\n\r", target_name );
      return rSPELL_FAILED;
   }
   return rNONE;
}



ch_ret spell_impinvis( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim;
   SKILLTYPE *skill = get_skilltype( sn );
   AFFECT_DATA af;

   if( target_name[0] == '\0' )
      victim = ch;
   else
      victim = get_char_room( ch, target_name );

/* if ( victim == NULL )
 * { 
 * send_to_char( "&BYou failed.\n\r", ch );
 * return rSPELL_FAILED;
 * }&D
 */
   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_IMPINVISIBLE ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   act( AT_MAGIC, "$n fades out of existence.", victim, NULL, NULL, TO_ROOM );
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_NONE;
   af.modifier = 0;
   af.bitvector = meb( AFF_IMPINVISIBLE );
   affect_to_char( victim, &af );
   act( AT_MAGIC, "You fade out of existence.", victim, NULL, NULL, TO_CHAR );
   return rNONE;
}

ch_ret spell_invis( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim;
   SKILLTYPE *skill = get_skilltype( sn );
   AFFECT_DATA af;

   if( target_name[0] == '\0' )
      victim = ch;
   else
      victim = get_char_room( ch, target_name );

   if( victim == NULL )
   {
      send_to_char( "&BYou failed.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_INVISIBLE ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   act( AT_MAGIC, "$n fades out of existence.", victim, NULL, NULL, TO_ROOM );
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_NONE;
   af.modifier = 0;
   af.bitvector = meb( AFF_INVISIBLE );
   affect_to_char( victim, &af );
   act( AT_MAGIC, "You fade out of existence.", victim, NULL, NULL, TO_CHAR );
   return rNONE;
}



ch_ret spell_know_alignment( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   char *msg;
   int ap;
   SKILLTYPE *skill = get_skilltype( sn );

   if( !victim )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   ap = victim->alignment;

   if( ap > 700 )
      msg = "$N has an aura as white as the driven snow.";
   else if( ap > 350 )
      msg = "$N is of excellent moral character.";
   else if( ap > 100 )
      msg = "$N is often kind and thoughtful.";
   else if( ap > -100 )
      msg = "$N doesn't have a firm moral commitment.";
   else if( ap > -350 )
      msg = "$N lies to $S friends.";
   else if( ap > -700 )
      msg = "$N would just as soon kill you as look at you.";
   else
      msg = "I'd rather just not say anything at all about $N.";

   act( AT_MAGIC, msg, ch, NULL, victim, TO_CHAR );
   return rNONE;
}


ch_ret spell_lightning_bolt( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.5;

   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}



ch_ret spell_locate_object( int sn, int level, CHAR_DATA * ch, void *vo )
{
   char buf[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   OBJ_DATA *in_obj;
   int cnt, found = 0;

   if( !LEARNED( ch, gsn_lore ) )
   {
      send_to_char( "You don't know enough lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   for( obj = first_object; obj; obj = obj->next )
   {
      if( !can_see_obj( ch, obj ) || !nifty_is_name( target_name, obj->name ) )
         continue;
      if( ( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) || IS_OBJ_STAT( obj, ITEM_NOLOCATE ) ) && !IS_IMMORTAL( ch ) )
         continue;

      found++;

      for( cnt = 0, in_obj = obj; in_obj->in_obj && cnt < 100; in_obj = in_obj->in_obj, ++cnt )
         ;
      if( cnt >= MAX_NEST )
      {
         sprintf( buf, "spell_locate_obj: object [%d] %s is nested more than %d times!",
                  obj->pIndexData->vnum, obj->short_descr, MAX_NEST );
         bug( buf, 0 );
         continue;
      }

      if( in_obj->carried_by )
      {
         if( IS_IMMORTAL( in_obj->carried_by )
             && !IS_NPC( in_obj->carried_by )
             && ( get_trust( ch ) < in_obj->carried_by->pcdata->wizinvis )
             && xIS_SET( in_obj->carried_by->act, PLR_WIZINVIS ) )
         {
            found--;
            continue;
         }

         sprintf( buf, "%s carried by %s.\n\r", obj_short( obj ), PERS( in_obj->carried_by, ch ) );
      }
      else
      {
         sprintf( buf, "%s in %s.\n\r", obj_short( obj ), in_obj->in_room == NULL ? "somewhere" : in_obj->in_room->name );
      }

      buf[0] = UPPER( buf[0] );
      set_char_color( AT_MAGIC, ch );

      send_to_char( buf, ch );
   }

   if( !found )
   {
      send_to_char( "Nothing like that exists.\n\r", ch );
      return rSPELL_FAILED;
   }
   return rNONE;
}



ch_ret spell_magic_missile( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level );

   return damage( ch, victim, dam, sn );
}



ch_ret spell_pass_door( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_PASS_DOOR ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_NONE;
   af.modifier = 0;
   af.bitvector = meb( AFF_PASS_DOOR );
   affect_to_char( victim, &af );
   act( AT_MAGIC, "$n turns translucent.", victim, NULL, NULL, TO_ROOM );
   act( AT_MAGIC, "You turn translucent.", victim, NULL, NULL, TO_CHAR );
   return rNONE;
}



ch_ret spell_poison( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   int chance;
   bool first = TRUE;

   chance = ris_save( victim, level, RIS_POISON );
   if( chance == 1000 || saves_poison_death( chance, victim ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "Your magic fails to take hold.\n\r", ch );
      return rSPELL_FAILED;
   }
   if( IS_AFFECTED( victim, AFF_POISON ) )
      first = FALSE;
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = ( sdur / 3 ) + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur / 3;
   }
   af.location = APPLY_STR;
   af.modifier = -2;
   af.bitvector = meb( AFF_POISON );
   affect_join( victim, &af );
   set_char_color( AT_GREEN, victim );
   send_to_char( "You feel very sick.\n\r", victim );
   if( ch != victim )
   {
      act( AT_GREEN, "$N shivers as your poison spreads through $S body.", ch, NULL, victim, TO_CHAR );
      act( AT_GREEN, "$N shivers as $n's poison spreads through $S body.", ch, NULL, victim, TO_NOTVICT );
   }
   return rNONE;
}


ch_ret spell_plague( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   int chance;
   bool first = TRUE;

   chance = ris_save( victim, level, RIS_POISON );
   if( chance == 1000 || saves_poison_death( chance, victim ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "Your magic fails to take hold.\n\r", ch );
      return rSPELL_FAILED;
   }
   if( IS_AFFECTED( victim, AFF_PLAGUE ) )
      first = FALSE;
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = ( sdur / 3 ) + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur / 3;
   }
   af.location = APPLY_STR;
   af.modifier = -5;
   af.bitvector = meb( AFF_PLAGUE );
   affect_to_char( victim, &af );
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = ( sdur / 3 ) + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur / 3;
   }
   af.location = APPLY_CON;
   af.modifier = -5;
   af.bitvector = meb( AFF_PLAGUE );
   affect_to_char( victim, &af );
   set_char_color( AT_GREEN, victim );
   send_to_char( "You feel very sick.\n\r", victim );
   if( ch != victim )
   {
      act( AT_GREEN, "$N shivers as your plague rips $S body.", ch, NULL, victim, TO_CHAR );
      act( AT_GREEN, "$N shivers as $n's plague rips $S body.", ch, NULL, victim, TO_NOTVICT );
   }
   return rNONE;
}


ch_ret spell_remove_curse( int sn, int level, CHAR_DATA * ch, void *vo )
{
   OBJ_DATA *obj;
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( is_affected( victim, gsn_curse ) )
   {
      affect_strip( victim, gsn_curse );
      set_char_color( AT_MAGIC, victim );
      send_to_char( "The weight of your curse is lifted.\n\r", victim );
      if( ch != victim )
      {
         act( AT_MAGIC, "You dispel the curses afflicting $N.", ch, NULL, victim, TO_CHAR );
         act( AT_MAGIC, "$n's dispels the curses afflicting $N.", ch, NULL, victim, TO_NOTVICT );
      }
   }
   else if( victim->first_carrying )
   {
      for( obj = victim->first_carrying; obj; obj = obj->next_content )
         if( !obj->in_obj && ( IS_OBJ_STAT( obj, ITEM_NOREMOVE ) || IS_OBJ_STAT( obj, ITEM_NODROP ) ) )
         {
            if( IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
               xREMOVE_BIT( obj->extra_flags, ITEM_NOREMOVE );
            if( IS_OBJ_STAT( obj, ITEM_NODROP ) )
               xREMOVE_BIT( obj->extra_flags, ITEM_NODROP );
            set_char_color( AT_MAGIC, victim );
            send_to_char( "You feel a burden released.\n\r", victim );
            if( ch != victim )
            {
               act( AT_MAGIC, "You dispel the curses afflicting $N.", ch, NULL, victim, TO_CHAR );
               act( AT_MAGIC, "$n's dispels the curses afflicting $N.", ch, NULL, victim, TO_NOTVICT );
            }
            return rNONE;
         }
   }
   return rNONE;
}

ch_ret spell_remove_trap( int sn, int level, CHAR_DATA * ch, void *vo )
{
   OBJ_DATA *obj;
   OBJ_DATA *trap;
   bool found;
   int retcode;
   SKILLTYPE *skill = get_skilltype( sn );

   if( !target_name || target_name[0] == '\0' )
   {
      send_to_char( "Remove trap on what?\n\r", ch );
      return rSPELL_FAILED;
   }

   found = FALSE;

   if( !ch->in_room->first_content )
   {
      send_to_char( "You can't find that here.\n\r", ch );
      return rNONE;
   }

   for( obj = ch->in_room->first_content; obj; obj = obj->next_content )
      if( can_see_obj( ch, obj ) && nifty_is_name( target_name, obj->name ) )
      {
         found = TRUE;
         break;
      }

   if( !found )
   {
      send_to_char( "You can't find that here.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( ( trap = get_trap( obj ) ) == NULL )
   {
      failed_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }


   if( chance( ch, 70 + get_curr_wis( ch ) ) )
   {
      send_to_char( "Ooops!\n\r", ch );
      retcode = spring_trap( ch, trap );
      if( retcode == rNONE )
         retcode = rSPELL_FAILED;
      return retcode;
   }

   extract_obj( trap );

   successful_casting( skill, ch, NULL, NULL );
   return rNONE;
}


ch_ret spell_calltohades( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam, x;

   x = ch->max_hit / 5;
   ch->max_hit -= x;
   dam = x / 2;

   act( AT_MAGIC, "You call upon Hades to fell $N.", ch, NULL, victim, TO_CHAR );
   act( AT_MAGIC, "$n calls upon Hades to fell $N.", ch, NULL, victim, TO_NOTVICT );
   act( AT_MAGIC, "$n calls upon Hades to fell you.", ch, NULL, victim, TO_VICT );
   return damage( ch, victim, dam, sn );
}

ch_ret spell_shocking_grasp( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.2;

   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_psibolt( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.7;

   return damage( ch, victim, dam, sn );
}

ch_ret spell_psiblast( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.4;

   return damage( ch, victim, dam, sn );
}

ch_ret spell_psiwave( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.2;

   return damage( ch, victim, dam, sn );
}



ch_ret spell_sleep( int sn, int level, CHAR_DATA * ch, void *vo )
{
   AFFECT_DATA af;
   int retcode;
   int chance;
   int tmp;
   CHAR_DATA *victim;
   SKILLTYPE *skill = get_skilltype( sn );

   if( ( victim = get_char_room( ch, target_name ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( !IS_NPC( victim ) && victim->fighting )
   {
      send_to_char( "You cannot sleep a fighting player.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( is_safe( ch, victim ) )
      return rSPELL_FAILED;

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( SPELL_FLAG( skill, SF_PKSENSITIVE ) && !IS_NPC( ch ) && !IS_NPC( victim ) )
      tmp = level / 2;
   else
      tmp = level;

   if( IS_AFFECTED( victim, AFF_SLEEP )
       || ( chance = ris_save( victim, tmp, RIS_SLEEP ) ) == 1000
       || level < victim->level
       || ( victim != ch
            && ( xIS_SET( victim->in_room->room_flags, ROOM_SAFE ) || xIS_SET( victim->in_room->room_flags, ROOM_SAFETY ) ) )
       || saves_spell_staff( chance, victim ) )
   {
      failed_casting( skill, ch, victim, NULL );
      if( ch == victim )
         return rSPELL_FAILED;
      if( !victim->fighting )
      {
         retcode = multi_hit( victim, ch, TYPE_UNDEFINED );
         if( retcode == rNONE )
            retcode = rSPELL_FAILED;
         return retcode;
      }
   }
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = ( sdur / 3 ) + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur / 3;
   }
   af.location = APPLY_NONE;
   af.modifier = 0;
   af.bitvector = meb( AFF_SLEEP );
   affect_join( victim, &af );

   if( !IS_NPC( victim ) )
   {
      sprintf( log_buf, "%s has cast sleep on %s.", ch->name, victim->name );
      log_string_plus( log_buf, LOG_NORMAL, ch->level );
      to_channel( log_buf, CHANNEL_MONITOR, "Monitor", UMAX( LEVEL_IMMORTAL, ch->level ), ch->level );
   }

   if( IS_AWAKE( victim ) )
   {
      act( AT_MAGIC, "You feel very sleepy ..... zzzzzz.", victim, NULL, NULL, TO_CHAR );
      act( AT_MAGIC, "$n goes to sleep.", victim, NULL, NULL, TO_ROOM );
      victim->position = POS_SLEEPING;
   }
   if( IS_NPC( victim ) )
      start_hating( victim, ch );

   return rNONE;
}



ch_ret spell_gpc( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   char buf[MAX_STRING_LENGTH];
   SKILLTYPE *skill = get_skilltype( sn );
   char name[MAX_INPUT_LENGTH];
   char *pd;

   if( ( victim = get_char_world( ch, target_name ) ) == NULL || IS_NPC( ch ) || !can_gpc( ch, victim ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( ( obj = get_obj_here( ch, "pcc" ) ) == NULL )
   {
      pd = obj->name;
      pd = one_argument( pd, name );
      pd = one_argument( pd, name );
      pd = one_argument( pd, name );

      if( str_cmp( name, victim->name ) )
      {
         failed_casting( skill, ch, victim, NULL );
         return rSPELL_FAILED;
      }
   }

   if( sysdata.trans_lock == 1 )
   {
      send_to_char( "I'm sorry, global trans lock is on.\n\r", ch );
      return rNONE;
   }

   if( !IS_NPC( ch ) )
   {
      act( AT_MAGIC, "You are pulled from the plane of existance...", ch, NULL, NULL, TO_CHAR );
      act( AT_MAGIC, "$n is pulled from the plane of existance!", ch, NULL, NULL, TO_ROOM );

      sprintf( buf, "%s gpc %s to room %d.", ch->name, victim->name, ch->in_room->vnum );
      log_string_plus( buf, LOG_NORMAL, ch->level );
      to_channel( buf, CHANNEL_MONITOR, "Monitor", UMAX( LEVEL_IMMORTAL, ch->level ), ch->level );
   }

   act( AT_MAGIC, "$n disappears suddenly.", victim, NULL, NULL, TO_ROOM );
   char_from_room( victim );
   char_to_room( victim, ch->in_room );
   victim->position = POS_STANDING;
   act( AT_MAGIC, "$n arrives suddenly.", victim, NULL, NULL, TO_ROOM );
   act( AT_MAGIC, "$N has GPCed you!", victim, NULL, ch, TO_CHAR );
   do_look( victim, "auto" );
   return rNONE;
}

ch_ret spell_summon( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim;
   char buf[MAX_STRING_LENGTH];
   SKILLTYPE *skill = get_skilltype( sn );

   if( ( victim = get_char_world( ch, target_name ) ) == NULL
       || victim == ch
       || !victim->in_room
       || victim->in_room->area->plane != ch->in_room->area->plane
       || xIS_SET( victim->in_room->room_flags, ROOM_NO_SUMMON )
       || victim->level >= level + 3
       || victim->fighting
       || ( IS_NPC( victim ) && xIS_SET( victim->act, ACT_PROTOTYPE ) )
       || ( IS_NPC( victim ) && saves_spell_staff( level, victim ) )
       || !in_hard_range( victim, ch->in_room->area )
       || ( !IS_NPC( ch ) && !IS_NPC( victim ) && IS_SET( victim->pcdata->flags, PCFLAG_NOSUMMON ) ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( sysdata.trans_lock == 1 )
   {
      send_to_char( "I'm sorry, global trans lock is on.\n\r", ch );
      return rNONE;
   }

   if( ch->in_room->area != victim->in_room->area )
   {
      if( ( ( IS_NPC( ch ) != IS_NPC( victim ) ) && chance( ch, 30 ) )
          || ( ( IS_NPC( ch ) == IS_NPC( victim ) ) && chance( ch, 60 ) ) )
      {
         failed_casting( skill, ch, victim, NULL );
         set_char_color( AT_MAGIC, victim );
         send_to_char( "You feel a strange pulling sensation...\n\r", victim );
         return rSPELL_FAILED;
      }
   }

   if( !IS_NPC( ch ) )
   {
      act( AT_MAGIC, "You feel a wave of nausea overcome you...", ch, NULL, NULL, TO_CHAR );
      act( AT_MAGIC, "$n collapses, stunned!", ch, NULL, NULL, TO_ROOM );
      ch->position = POS_STUNNED;

      sprintf( buf, "%s summoned %s to room %d.", ch->name, victim->name, ch->in_room->vnum );
      log_string_plus( buf, LOG_NORMAL, ch->level );
      to_channel( buf, CHANNEL_MONITOR, "Monitor", UMAX( LEVEL_IMMORTAL, ch->level ), ch->level );
   }

   act( AT_MAGIC, "$n disappears suddenly.", victim, NULL, NULL, TO_ROOM );
   char_from_room( victim );
   char_to_room( victim, ch->in_room );
   act( AT_MAGIC, "$n arrives suddenly.", victim, NULL, NULL, TO_ROOM );
   act( AT_MAGIC, "$N has summoned you!", victim, NULL, ch, TO_CHAR );
   do_look( victim, "auto" );
   return rNONE;
}

ch_ret spell_astral_walk( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim;
   struct skill_type *skill = get_skilltype( sn );

   if( ( victim = get_char_world( ch, target_name ) ) == NULL
       || !can_astral( ch, victim ) || !in_hard_range( ch, victim->in_room->area ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( sysdata.trans_lock == 1 )
   {
      send_to_char( "I'm sorry, global trans lock is on.\n\r", ch );
      return rNONE;
   }

   if( skill->hit_char && skill->hit_char[0] != '\0' )
      act( AT_COLORIZE, skill->hit_char, ch, NULL, victim, TO_CHAR );
   if( skill->hit_vict && skill->hit_vict[0] != '\0' )
      act( AT_COLORIZE, skill->hit_vict, ch, NULL, victim, TO_VICT );

   if( skill->hit_room && skill->hit_room[0] != '\0' )
      act( AT_COLORIZE, skill->hit_room, ch, NULL, victim, TO_NOTVICT );
   else
      act( AT_MAGIC, "$n disappears in a flash of light!", ch, NULL, NULL, TO_ROOM );
   char_from_room( ch );
   char_to_room( ch, victim->in_room );
   if( ch->on )
   {
      ch->on = NULL;
      ch->position = POS_STANDING;
   }
   if( ch->position != POS_STANDING )
   {
      ch->position = POS_STANDING;
   }
   if( skill->hit_dest && skill->hit_dest[0] != '\0' )
      act( AT_COLORIZE, skill->hit_dest, ch, NULL, victim, TO_NOTVICT );
   else
      act( AT_MAGIC, "$n appears in a flash of light!", ch, NULL, NULL, TO_ROOM );
   do_look( ch, "auto" );
   return rNONE;
}

ch_ret spell_teleport( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim;
   SKILLTYPE *skill = get_skilltype( sn );

   if( ( victim = get_char_world( ch, target_name ) ) == NULL || !can_tele( ch, victim ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( sysdata.trans_lock == 1 )
   {
      send_to_char( "I'm sorry, global trans lock is on.\n\r", ch );
      return rNONE;
   }

   if( victim->in_room == ch->in_room )
   {
      send_to_char( "They are right beside you!", ch );
      return rSPELL_FAILED;
   }


   act( AT_MAGIC, "$n disappears in a blinding flash of light!", ch, NULL, NULL, TO_ROOM );
   char_from_room( ch );
   char_to_room( ch, victim->in_room );
   if( ch->on )
   {
      ch->on = NULL;
      ch->position = POS_STANDING;
   }
   if( ch->position != POS_STANDING )
   {
      ch->position = POS_STANDING;
   }
   act( AT_COLORIZE, "$n appears in a blinding flash of light!", ch, NULL, NULL, TO_ROOM );
   do_look( ch, "auto" );
   return rNONE;
}

ch_ret spell_mistwalk( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim;
   SKILLTYPE *skill = get_skilltype( sn );

   if( ( victim = get_char_world( ch, target_name ) ) == NULL || !can_tele( ch, victim ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( sysdata.trans_lock == 1 )
   {
      send_to_char( "I'm sorry, global trans lock is on.\n\r", ch );
      return rNONE;
   }

   if( victim->in_room == ch->in_room )
   {
      send_to_char( "They are right beside you!", ch );
      return rSPELL_FAILED;
   }


   act( AT_MAGIC, "$n dissolves into a light mist!", ch, NULL, NULL, TO_ROOM );
   char_from_room( ch );
   char_to_room( ch, victim->in_room );
   if( ch->on )
   {
      ch->on = NULL;
      ch->position = POS_STANDING;
   }
   if( ch->position != POS_STANDING )
   {
      ch->position = POS_STANDING;
   }
   act( AT_COLORIZE, "A light mist forms and $n appears!", ch, NULL, NULL, TO_ROOM );
   do_look( ch, "auto" );
   return rNONE;
}

ch_ret spell_home( int sn, int level, CHAR_DATA * ch, void *vo )
{
   char buf[MAX_STRING_LENGTH];
   ROOM_INDEX_DATA *location;
   SKILLTYPE *skill = get_skilltype( sn );

   if( sysdata.trans_lock == 1 )
   {
      send_to_char( "I'm sorry, global trans lock is on.\n\r", ch );
      return rNONE;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_NO_HOME )
       || ( ch->in_room->area->plane == 1 || ch->in_room->area->plane == 6 || ch->in_room->area->plane == 7 ) )
   {
      send_to_char( "For some strange reason... nothing happens.\n\r", ch );

      failed_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }

   if( IS_NPC( ch ) )
   {
      send_to_char( "NPC's cannot shift.\n\r", ch );
      return rNONE;
   }

   location = get_room_index( ROOM_VNUM_TERRA );

   act( AT_MAGIC, "$n blinks out of existance!", ch, NULL, NULL, TO_ROOM );
   act( AT_MAGIC, "Your existance blinks in and out!", ch, NULL, NULL, TO_CHAR );
   char_from_room( ch );
   char_to_room( ch, location );
   sprintf( buf, "&R<&BSHIFT INFO&R> %s has returned home.&D", ch->name );
   talk_info( AT_BLUE, buf );
   act( AT_COLORIZE, "$n blinks into existance!", ch, NULL, NULL, TO_ROOM );
   do_look( ch, "auto" );
   return rNONE;
}

ch_ret spell_terrashift( int sn, int level, CHAR_DATA * ch, void *vo )
{
   char buf[MAX_STRING_LENGTH];
   ROOM_INDEX_DATA *location;
   SKILLTYPE *skill = get_skilltype( sn );

   if( sysdata.trans_lock == 1 )
   {
      send_to_char( "I'm sorry, global trans lock is on.\n\r", ch );
      return rNONE;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_NO_SHIFT ) )
   {
      send_to_char( "For some strange reason... nothing happens.\n\r", ch );


      failed_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }

   if( IS_NPC( ch ) )
   {
      send_to_char( "NPC's cannot shift.\n\r", ch );
      return rNONE;
   }

   if( target_name[0] == '\0' )
   {
      send_to_char( "Syntax: cast terrashift <plane>.\n\r", ch );
      return rNONE;
   }
   if( !str_cmp( target_name, "terra" ) )
   {
      location = get_room_index( ROOM_VNUM_TERRA );

      act( AT_MAGIC, "$n blinks out of existance!", ch, NULL, NULL, TO_ROOM );
      act( AT_MAGIC, "Your existance blinks in and out!", ch, NULL, NULL, TO_CHAR );
      char_from_room( ch );
      char_to_room( ch, location );
      sprintf( buf, "&R<&BSHIFT INFO&R> %s has shifted to the plane Terra.&D", ch->name );
      talk_info( AT_BLUE, buf );
      act( AT_COLORIZE, "$n blinks into existance!", ch, NULL, NULL, TO_ROOM );
      do_look( ch, "auto" );
      return rNONE;
   }
   if( !str_cmp( target_name, "white" ) )
   {
      location = get_room_index( ROOM_VNUM_WHITE );

      act( AT_MAGIC, "$n blinks out of existance!", ch, NULL, NULL, TO_ROOM );
      act( AT_MAGIC, "Your existance blinks in and out!", ch, NULL, NULL, TO_CHAR );
      char_from_room( ch );
      char_to_room( ch, location );
      sprintf( buf, "&R<&BSHIFT INFO&R> %s has shifted to the plane White.&D", ch->name );
      talk_info( AT_BLUE, buf );
      act( AT_COLORIZE, "$n blinks into existance!", ch, NULL, NULL, TO_ROOM );
      do_look( ch, "auto" );
      return rNONE;
   }
   if( !str_cmp( target_name, "lorur" ) )
   {
      if( ch->level < 200 )
      {
         send_to_char( "You cannot access this plane.", ch );
         failed_casting( skill, ch, NULL, NULL );
         return rSPELL_FAILED;
      }
      else
      {
         location = get_room_index( ROOM_VNUM_LORUR );

         act( AT_MAGIC, "$n blinks out of existance!", ch, NULL, NULL, TO_ROOM );
         act( AT_MAGIC, "Your existance blinks in and out!", ch, NULL, NULL, TO_CHAR );
         char_from_room( ch );
         char_to_room( ch, location );
         sprintf( buf, "&R<&BSHIFT INFO&R> %s has shifted to the plane Lorur.&D", ch->name );
         talk_info( AT_BLUE, buf );
         act( AT_COLORIZE, "$n blinks into existance!", ch, NULL, NULL, TO_ROOM );
         do_look( ch, "auto" );
         return rNONE;
      }
   }
   if( !str_cmp( target_name, "ophelia" ) )
   {
      if( ch->level < 300 )
      {
         send_to_char( "You cannot access this plane.", ch );
         failed_casting( skill, ch, NULL, NULL );
         return rSPELL_FAILED;
      }
      else
      {
         location = get_room_index( ROOM_VNUM_OPHELIA );

         act( AT_MAGIC, "$n blinks out of existance!", ch, NULL, NULL, TO_ROOM );
         act( AT_MAGIC, "Your existance blinks in and out!", ch, NULL, NULL, TO_CHAR );
         char_from_room( ch );
         char_to_room( ch, location );
         sprintf( buf, "&R<&BSHIFT INFO&R> %s has shifted to the plane Ophelia.&D", ch->name );
         talk_info( AT_BLUE, buf );
         act( AT_COLORIZE, "$n blinks into existance!", ch, NULL, NULL, TO_ROOM );
         do_look( ch, "auto" );
         return rNONE;
      }
   }
   if( !str_cmp( target_name, "utopia" ) )
   {
      if( ch->level < 400 )
      {
         send_to_char( "You cannot access this plane.", ch );
         failed_casting( skill, ch, NULL, NULL );
         return rSPELL_FAILED;
      }
      else
      {
         location = get_room_index( ROOM_VNUM_UTOPIA );

         act( AT_MAGIC, "$n blinks out of existance!", ch, NULL, NULL, TO_ROOM );
         act( AT_MAGIC, "Your existance blinks in and out!", ch, NULL, NULL, TO_CHAR );
         char_from_room( ch );
         char_to_room( ch, location );
         sprintf( buf, "&R<&BSHIFT INFO&R> %s has shifted to the plane Utopia.&D", ch->name );
         talk_info( AT_BLUE, buf );
         act( AT_COLORIZE, "$n blinks into existance!", ch, NULL, NULL, TO_ROOM );
         do_look( ch, "auto" );
         return rNONE;
      }
   }
   return rNONE;
}


ch_ret spell_warp( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim;
   SKILLTYPE *skill = get_skilltype( sn );

   if( ( victim = get_char_world( ch, target_name ) ) == NULL || !can_tele( ch, victim ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( sysdata.trans_lock == 1 )
   {
      send_to_char( "I'm sorry, global trans lock is on.\n\r", ch );
      return rNONE;
   }

   if( victim->in_room == ch->in_room )
   {
      send_to_char( "They are right beside you!", ch );
      return rSPELL_FAILED;
   }


   act( AT_MAGIC, "$n starts to fade out as $e warps out!", ch, NULL, NULL, TO_ROOM );
   char_from_room( ch );
   char_to_room( ch, victim->in_room );
   if( ch->on )
   {
      ch->on = NULL;
      ch->position = POS_STANDING;
   }
   if( ch->position != POS_STANDING )
   {
      ch->position = POS_STANDING;
   }
   act( AT_COLORIZE, "$n starts to fade in as $e warps in!", ch, NULL, NULL, TO_ROOM );
   do_look( ch, "auto" );
   return rNONE;
}

ch_ret spell_ventriloquate( int sn, int level, CHAR_DATA * ch, void *vo )
{
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char speaker[MAX_INPUT_LENGTH];
   CHAR_DATA *vch;

   target_name = one_argument( target_name, speaker );

   sprintf( buf1, "%s says '%s'.\n\r", speaker, target_name );
   sprintf( buf2, "Someone makes %s say '%s'.\n\r", speaker, target_name );
   buf1[0] = UPPER( buf1[0] );

   for( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
   {
      if( !is_name( speaker, vch->name ) )
      {
         set_char_color( AT_SAY, vch );
         send_to_char( saves_spell_staff( level, vch ) ? buf2 : buf1, vch );
      }
   }

   return rNONE;
}



ch_ret spell_weaken( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   set_char_color( AT_MAGIC, ch );
   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }
   if( is_affected( victim, sn ) || saves_wands( level, victim ) )
   {
      send_to_char( "Your magic fails to take hold.\n\r", ch );
      return rSPELL_FAILED;
   }
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_STR;
   af.modifier = -2;
   xCLEAR_BITS( af.bitvector );
   affect_to_char( victim, &af );
   set_char_color( AT_MAGIC, victim );
   send_to_char( "Your muscles seem to atrophy!\n\r", victim );
   if( ch != victim )
   {
      if( ( ( ( !IS_NPC( victim ) && class_table[victim->class]->attr_prime == APPLY_STR )
              || IS_NPC( victim ) ) && get_curr_str( victim ) < 25 ) || get_curr_str( victim ) < 20 )
      {
         act( AT_MAGIC, "$N labors weakly as your spell atrophies $S muscles.", ch, NULL, victim, TO_CHAR );
         act( AT_MAGIC, "$N labors weakly as $n's spell atrophies $S muscles.", ch, NULL, victim, TO_NOTVICT );
      }
      else
      {
         act( AT_MAGIC, "You induce a mild atrophy in $N's muscles.", ch, NULL, victim, TO_CHAR );
         act( AT_MAGIC, "$n induces a mild atrophy in $N's muscles.", ch, NULL, victim, TO_NOTVICT );
      }
   }
   return rNONE;
}

ch_ret spell_delirium( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );
   int chance;

   set_char_color( AT_MAGIC, ch );
   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }
   if( is_affected( victim, gsn_delirium ) || saves_wands( level, victim ) )
   {
      send_to_char( "Your magic fails to take hold.\n\r", ch );
      return rSPELL_FAILED;
   }
   chance = number_range( 0, 100 );
   if( chance <= 10 )
   {
      af.type = gsn_delirium;
      af.duration = 150;
      af.location = APPLY_STR;
      af.modifier = -2;
      xCLEAR_BITS( af.bitvector );
      affect_to_char( victim, &af );
      set_char_color( AT_MAGIC, victim );
      send_to_char( "Your muscles seem to atrophy!\n\r", victim );
      if( ch != victim )
      {
         if( ( ( ( !IS_NPC( victim ) && class_table[victim->class]->attr_prime == APPLY_STR )
                 || IS_NPC( victim ) ) && get_curr_str( victim ) < 25 ) || get_curr_str( victim ) < 20 )
         {
            act( AT_MAGIC, "$N labors weakly as your spell atrophies $S muscles.", ch, NULL, victim, TO_CHAR );
            act( AT_MAGIC, "$N labors weakly as $n's spell atrophies $S muscles.", ch, NULL, victim, TO_NOTVICT );
         }
         else
         {
            act( AT_MAGIC, "You induce a mild atrophy in $N's muscles.", ch, NULL, victim, TO_CHAR );
            act( AT_MAGIC, "$n induces a mild atrophy in $N's muscles.", ch, NULL, victim, TO_NOTVICT );
         }
      }
   }
   if( chance > 10 && chance <= 20 )
   {
      af.type = gsn_delirium;
      af.duration = 150;
      af.location = APPLY_AC;
      af.modifier = -20;
      xCLEAR_BITS( af.bitvector );
      affect_to_char( victim, &af );

      af.type = gsn_delirium;
      af.duration = 150;
      af.location = APPLY_HITROLL;
      af.modifier = 20;
      xCLEAR_BITS( af.bitvector );
      affect_to_char( victim, &af );

      af.type = gsn_delirium;
      af.duration = 150;
      af.location = APPLY_DAMROLL;
      af.modifier = 20;
      xCLEAR_BITS( af.bitvector );
      affect_to_char( victim, &af );

      act( AT_MAGIC, "You induce a feeling of confidence in $N.", ch, NULL, victim, TO_CHAR );
      act( AT_MAGIC, "$n induces a feeling of confidence in $N.", ch, NULL, victim, TO_NOTVICT );
      act( AT_MAGIC, "$n induces a feeling of confidence in you.", ch, NULL, victim, TO_VICT );
   }
   if( chance > 20 && chance <= 60 )
   {
      af.type = gsn_delirium;
      af.duration = 150;
      af.location = APPLY_AC;
      af.modifier = 20;
      xCLEAR_BITS( af.bitvector );
      affect_to_char( victim, &af );

      act( AT_MAGIC, "You induce a hex on $N.", ch, NULL, victim, TO_CHAR );
      act( AT_MAGIC, "$n induces a hex on $N.", ch, NULL, victim, TO_NOTVICT );
      act( AT_MAGIC, "$n induces a hex on you.", ch, NULL, victim, TO_VICT );
   }
   if( chance > 60 && chance <= 100 )
   {
      af.type = gsn_delirium;
      af.duration = 150;
      af.location = APPLY_AFFECT;
      af.modifier = 0;
      af.bitvector = meb( AFF_POISON );
      affect_to_char( victim, &af );

      act( AT_MAGIC, "You induce a feeling of sickness in $N.", ch, NULL, victim, TO_CHAR );
      act( AT_MAGIC, "$n induces a feeling of sickness in $N.", ch, NULL, victim, TO_NOTVICT );
      act( AT_MAGIC, "$n induces a feeling of sickness in you.", ch, NULL, victim, TO_VICT );
   }
   return rNONE;
}

ch_ret spell_word_of_recall( int sn, int level, CHAR_DATA * ch, void *vo )
{
   if( sysdata.trans_lock == 1 )
   {
      send_to_char( "I'm sorry, global trans lock is on.\n\r", ch );
      return rNONE;
   }

   do_recall( ( CHAR_DATA * ) vo, "" );
   return rNONE;
}


ch_ret spell_acid_breath( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   OBJ_DATA *obj_lose;
   OBJ_DATA *obj_next;
   int dam;
   int hpch;

   if( chance( ch, 2 * level ) && !saves_breath( level, victim ) )
   {
      for( obj_lose = victim->first_carrying; obj_lose; obj_lose = obj_next )
      {
         int iWear;

         obj_next = obj_lose->next_content;

         if( number_bits( 2 ) != 0 )
            continue;

         switch ( obj_lose->item_type )
         {
            case ITEM_ARMOR:
               if( obj_lose->value[0] > 0 )
               {
                  separate_obj( obj_lose );
                  act( AT_DAMAGE, "$p is pitted and etched!", victim, obj_lose, NULL, TO_CHAR );
                  if( ( iWear = obj_lose->wear_loc ) != WEAR_NONE )
                     victim->armor -= apply_ac( obj_lose, iWear );
                  obj_lose->value[0] -= 1;
                  obj_lose->cost = 0;
                  if( iWear != WEAR_NONE )
                     victim->armor += apply_ac( obj_lose, iWear );
               }
               break;

            case ITEM_CONTAINER:
               separate_obj( obj_lose );
               act( AT_DAMAGE, "$p fumes and dissolves!", victim, obj_lose, NULL, TO_CHAR );
               act( AT_OBJECT, "The contents of $p held by $N spill onto the ground.", victim, obj_lose, victim, TO_ROOM );
               act( AT_OBJECT, "The contents of $p spill out onto the ground!", victim, obj_lose, NULL, TO_CHAR );
               empty_obj( obj_lose, NULL, victim->in_room );
               extract_obj( obj_lose );
               break;
         }
      }
   }

   hpch = UMAX( 10, ch->hit );
   if( !IS_NPC( ch ) )
   {
      dam = ( ( hpch / 3 ) + ( level / 4 ) );
   }
   else
   {
      dam = level * 1.5;
   }
   if( saves_breath( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}



ch_ret spell_fire_breath( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   OBJ_DATA *obj_lose;
   OBJ_DATA *obj_next;
   int dam;
   int hpch;

   if( chance( ch, 2 * level ) && !saves_breath( level, victim ) )
   {
      for( obj_lose = victim->first_carrying; obj_lose; obj_lose = obj_next )
      {
         char *msg;

         obj_next = obj_lose->next_content;
         if( number_bits( 2 ) != 0 )
            continue;

         switch ( obj_lose->item_type )
         {
            default:
               continue;
            case ITEM_CONTAINER:
               msg = "$p ignites and burns!";
               break;
            case ITEM_POTION:
               msg = "$p bubbles and boils!";
               break;
            case ITEM_SCROLL:
               msg = "$p crackles and burns!";
               break;
            case ITEM_STAFF:
               msg = "$p smokes and chars!";
               break;
            case ITEM_WAND:
               msg = "$p sparks and sputters!";
               break;
            case ITEM_COOK:
            case ITEM_FOOD:
               msg = "$p blackens and crisps!";
               break;
            case ITEM_PILL:
               msg = "$p melts and drips!";
               break;
         }

         separate_obj( obj_lose );
         act( AT_DAMAGE, msg, victim, obj_lose, NULL, TO_CHAR );
         if( obj_lose->item_type == ITEM_CONTAINER )
         {
            act( AT_OBJECT, "The contents of $p held by $N spill onto the ground.", victim, obj_lose, victim, TO_ROOM );
            act( AT_OBJECT, "The contents of $p spill out onto the ground!", victim, obj_lose, NULL, TO_CHAR );
            empty_obj( obj_lose, NULL, victim->in_room );
         }
         extract_obj( obj_lose );
      }
   }

   hpch = UMAX( 10, ch->hit );
   if( !IS_NPC( ch ) )
   {
      dam = ( ( hpch / 3 ) + ( level / 4 ) );
   }
   else
   {
      dam = level * 1.5;
   }
   if( saves_breath( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}



ch_ret spell_frost_breath( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   OBJ_DATA *obj_lose;
   OBJ_DATA *obj_next;
   int dam;
   int hpch;

   if( chance( ch, 2 * level ) && !saves_breath( level, victim ) )
   {
      for( obj_lose = victim->first_carrying; obj_lose; obj_lose = obj_next )
      {
         char *msg;

         obj_next = obj_lose->next_content;
         if( number_bits( 2 ) != 0 )
            continue;

         switch ( obj_lose->item_type )
         {
            default:
               continue;
            case ITEM_CONTAINER:
            case ITEM_DRINK_CON:
            case ITEM_POTION:
               msg = "$p freezes and shatters!";
               break;
         }

         separate_obj( obj_lose );
         act( AT_DAMAGE, msg, victim, obj_lose, NULL, TO_CHAR );
         if( obj_lose->item_type == ITEM_CONTAINER )
         {
            act( AT_OBJECT, "The contents of $p held by $N spill onto the ground.", victim, obj_lose, victim, TO_ROOM );
            act( AT_OBJECT, "The contents of $p spill out onto the ground!", victim, obj_lose, NULL, TO_CHAR );
            empty_obj( obj_lose, NULL, victim->in_room );
         }
         extract_obj( obj_lose );
      }
   }

   hpch = UMAX( 10, ch->hit );
   if( !IS_NPC( ch ) )
   {
      dam = ( ( hpch / 3 ) + ( level / 4 ) );
   }
   else
   {
      dam = level * 1.5;
   }
   if( saves_breath( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_mindblast( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   AFFECT_DATA af;
   int dam;
   int hpch;
   bool ch_died;

   ch_died = FALSE;

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return rNONE;
   }
   if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "You fail to mindblast.\n\r", ch );
      return rNONE;
   }

   for( vch = ch->in_room->first_person; vch; vch = vch_next )
   {
      vch_next = vch->next_in_room;
      if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
         continue;

      if( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
      {
         hpch = UMAX( 10, ch->hit );
         dam = ( hpch + ( level / 5 ) );
         if( saves_breath( level, vch ) )
            dam /= 2;
         if( vch == ch )
            dam = 0;
         if( number_percent(  ) > 70 )
         {
            af.type = sn;
            af.location = APPLY_AC;
            af.modifier = 100;
            af.duration = 3;
            af.bitvector = meb( AFF_PARALYSIS );
            affect_to_char( vch, &af );
            update_pos( vch );
            act( AT_FIRE, "$n suffers a migraine!", vch, NULL, NULL, TO_ROOM );
         }
         if( damage( ch, vch, dam, sn ) == rCHAR_DIED || char_died( ch ) )
            ch_died = TRUE;
      }
   }
   if( ch_died )
      return rCHAR_DIED;
   else
      return rNONE;
}

ch_ret spell_hurricane( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   AFFECT_DATA af;
   int dam;
   int hpch;
   bool ch_died;

   ch_died = FALSE;

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return rNONE;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "You fail to make a hurricane.\n\r", ch );
      return rNONE;
   }

   for( vch = ch->in_room->first_person; vch; vch = vch_next )
   {
      vch_next = vch->next_in_room;
      if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
         continue;

      if( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
      {
         hpch = UMAX( 10, ch->hit );
         dam = ( hpch + ( level / 4 ) );
         if( saves_breath( level, vch ) )
            dam /= 2;
         if( vch == ch )
            dam = 0;
         if( number_percent(  ) > 70 )
         {
            af.type = sn;
            af.location = APPLY_AC;
            af.modifier = 100;
            af.duration = 3;
            af.bitvector = meb( AFF_PARALYSIS );
            affect_to_char( vch, &af );
            update_pos( vch );
            act( AT_FIRE, "$n nearly drowns!", vch, NULL, NULL, TO_ROOM );
         }
         if( damage( ch, vch, dam, sn ) == rCHAR_DIED || char_died( ch ) )
            ch_died = TRUE;
      }
   }
   if( ch_died )
      return rCHAR_DIED;
   else
      return rNONE;
}

ch_ret spell_holy( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   AFFECT_DATA af;
   int dam;
   int hpch;
   bool ch_died;

   ch_died = FALSE;

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return rNONE;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "You fail to call holy.\n\r", ch );
      return rNONE;
   }

   for( vch = ch->in_room->first_person; vch; vch = vch_next )
   {
      vch_next = vch->next_in_room;
      if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
         continue;

      if( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
      {
         hpch = UMAX( 10, ch->hit );
         dam = ( hpch + ( level / 4 ) );
         if( saves_breath( level, vch ) )
            dam /= 2;
         if( vch == ch )
            dam = 0;
         if( number_percent(  ) > 70 )
         {
            af.type = sn;
            af.location = APPLY_AC;
            af.modifier = 100;
            af.duration = 3;
            af.bitvector = meb( AFF_PARALYSIS );
            affect_to_char( vch, &af );
            update_pos( vch );
            act( AT_FIRE, "$n feels really really remorseful!", vch, NULL, NULL, TO_ROOM );
         }
         if( damage( ch, vch, dam, sn ) == rCHAR_DIED || char_died( ch ) )
            ch_died = TRUE;
      }
   }
   if( ch_died )
      return rCHAR_DIED;
   else
      return rNONE;
}

ch_ret spell_sandstorm( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   AFFECT_DATA af;
   int dam;
   int hpch;
   bool ch_died;

   ch_died = FALSE;

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return rNONE;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "You fail to make a sandstorm.\n\r", ch );
      return rNONE;
   }

   for( vch = ch->in_room->first_person; vch; vch = vch_next )
   {
      vch_next = vch->next_in_room;
      if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
         continue;

      if( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
      {
         hpch = UMAX( 10, ch->hit );
         dam = ( hpch + ( level / 5 ) );
         if( saves_breath( level, vch ) )
            dam /= 2;
         if( vch == ch )
            dam = 0;
         if( number_percent(  ) > 70 )
         {
            af.type = sn;
            af.location = APPLY_AC;
            af.modifier = 100;
            af.duration = 3;
            af.bitvector = meb( AFF_PARALYSIS );
            affect_to_char( vch, &af );
            update_pos( vch );
            act( AT_FIRE, "$n gags and chokes!", vch, NULL, NULL, TO_ROOM );
         }
         if( damage( ch, vch, dam, sn ) == rCHAR_DIED || char_died( ch ) )
            ch_died = TRUE;
      }
   }
   if( ch_died )
      return rCHAR_DIED;
   else
      return rNONE;
}

ch_ret spell_firestorm( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   AFFECT_DATA af;
   int dam;
   int hpch;
   bool ch_died;

   ch_died = FALSE;

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return rNONE;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "You fail to make a firestorm.\n\r", ch );
      return rNONE;
   }

   for( vch = ch->in_room->first_person; vch; vch = vch_next )
   {
      vch_next = vch->next_in_room;
      if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
         continue;

      if( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
      {
         hpch = UMAX( 10, ch->hit );
         dam = ( hpch + ( level / 3 ) );
         if( saves_breath( level, vch ) )
            dam /= 2;
         if( vch == ch )
            dam = 0;
         if( number_percent(  ) > 70 )
         {
            af.type = sn;
            af.location = APPLY_AC;
            af.modifier = 100;
            af.duration = 3;
            af.bitvector = meb( AFF_PARALYSIS );
            affect_to_char( vch, &af );
            update_pos( vch );
            act( AT_FIRE, "$n catches fire!", vch, NULL, NULL, TO_ROOM );
         }
         if( damage( ch, vch, dam, sn ) == rCHAR_DIED || char_died( ch ) )
            ch_died = TRUE;
      }
   }
   if( ch_died )
      return rCHAR_DIED;
   else
      return rNONE;
}

ch_ret spell_meteor( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   AFFECT_DATA af;
   int dam;
   int hpch;
   bool ch_died;

   ch_died = FALSE;
   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return rNONE;
   }


   if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "You fail to call a meteor.\n\r", ch );
      return rNONE;
   }

   for( vch = ch->in_room->first_person; vch; vch = vch_next )
   {
      vch_next = vch->next_in_room;
      if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
         continue;

      if( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
      {
         hpch = UMAX( 10, ch->hit );
         dam = ( hpch + ( level / 3 ) );
         if( saves_breath( level, vch ) )
            dam /= 2;
         if( vch == ch )
            dam = 0;
         if( number_percent(  ) > 70 )
         {
            af.type = sn;
            af.location = APPLY_AC;
            af.modifier = 100;
            af.duration = 3;
            af.bitvector = meb( AFF_PARALYSIS );
            affect_to_char( vch, &af );
            update_pos( vch );
            act( AT_FIRE, "$n is trapped under the meteor!", vch, NULL, NULL, TO_ROOM );
         }
         if( damage( ch, vch, dam, sn ) == rCHAR_DIED || char_died( ch ) )
            ch_died = TRUE;
      }
   }
   if( ch_died )
      return rCHAR_DIED;
   else
      return rNONE;
}

ch_ret spell_avalanche( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   AFFECT_DATA af;
   int dam;
   int hpch;
   bool ch_died;

   ch_died = FALSE;

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return rNONE;
   }

   if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "You fail to make a avalanche.\n\r", ch );
      return rNONE;
   }

   for( vch = ch->in_room->first_person; vch; vch = vch_next )
   {
      vch_next = vch->next_in_room;
      if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
         continue;

      if( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
      {
         hpch = UMAX( 10, ch->hit );
         dam = ( hpch + ( level / 6 ) );
         if( saves_breath( level, vch ) )
            dam /= 2;
         if( vch == ch )
            dam = 0;
         if( number_percent(  ) > 70 )
         {
            af.type = sn;
            af.location = APPLY_AC;
            af.modifier = 100;
            af.duration = 3;
            af.bitvector = meb( AFF_PARALYSIS );
            affect_to_char( vch, &af );
            update_pos( vch );
            act( AT_FIRE, "$n freezes under the ice!", vch, NULL, NULL, TO_ROOM );
         }
         if( damage( ch, vch, dam, sn ) == rCHAR_DIED || char_died( ch ) )
            ch_died = TRUE;
      }
   }
   if( ch_died )
      return rCHAR_DIED;
   else
      return rNONE;
}


ch_ret spell_gas_breath( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   int dam;
   int hpch;
   bool ch_died;

   ch_died = FALSE;

   if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "You fail to breathe.\n\r", ch );
      return rNONE;
   }

   for( vch = ch->in_room->first_person; vch; vch = vch_next )
   {
      vch_next = vch->next_in_room;
      if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
         continue;

      if( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
      {
         hpch = UMAX( 10, ch->hit );
         if( !IS_NPC( ch ) )
         {
            dam = ( ( hpch / 3 ) + ( level / 4 ) );
         }
         else
         {
            dam = level * 1.5;
         }
         if( saves_breath( level, vch ) )
            dam /= 2;
         if( damage( ch, vch, dam, sn ) == rCHAR_DIED || char_died( ch ) )
            ch_died = TRUE;
      }
   }
   if( ch_died )
      return rCHAR_DIED;
   else
      return rNONE;
}



ch_ret spell_lightning_breath( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;
   int hpch;

   hpch = UMAX( 10, ch->hit );
   if( !IS_NPC( ch ) )
   {
      dam = ( ( hpch / 3 ) + ( level / 4 ) );
   }
   else
   {
      dam = level * 1.5;
   }
   if( saves_breath( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_null( int sn, int level, CHAR_DATA * ch, void *vo )
{
   send_to_char( "That's not a spell!\n\r", ch );
   return rNONE;
}

ch_ret spell_notfound( int sn, int level, CHAR_DATA * ch, void *vo )
{
   send_to_char( "That's not a spell!\n\r", ch );
   return rNONE;
}


ch_ret spell_psidance( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   OBJ_DATA *obj;
   OBJ_DATA *wield;
   int dam = 0;
   int chance;
   SKILLTYPE *skill = get_skilltype( sn );
   int iWear;

   wield = NULL;

   for( iWear = 0; iWear < MAX_WEAR; iWear++ )
   {
      for( obj = ch->first_carrying; obj; obj = obj->next_content )
      {
         if( obj->wear_loc == WEAR_WIELD )
         {
            wield = obj;
         }
      }
   }
   if( !wield )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   chance = number_range( 0, 100 );
   separate_obj( wield );
   if( !IS_NPC( ch ) && chance > LEARNED( ch, gsn_psidance ) )
   {
      act( AT_ACTION, "$p flies through the air and then falls to the ground.", ch, wield, NULL, TO_CHAR );
      act( AT_ACTION, "$p flies through the air and then falls to the ground.", ch, wield, NULL, TO_ROOM );
      obj_from_char( wield );
      obj_to_room( wield, ch->in_room );
   }
   else
   {
      act( AT_MAGIC, "$p flies through the air and returns to your hand.", ch, wield, NULL, TO_CHAR );
      act( AT_MAGIC, "$p flies through the air and returns to $n's hand", ch, wield, NULL, TO_ROOM );
   }
   dam = ( wield->value[1] + wield->value[2] ) / 2;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_transport( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim;
   char arg3[MAX_STRING_LENGTH];
   OBJ_DATA *obj;
   SKILLTYPE *skill = get_skilltype( sn );

   target_name = one_argument( target_name, arg3 );

   if( ( victim = get_char_world( ch, target_name ) ) == NULL
       || victim == ch
       || xIS_SET( victim->in_room->room_flags, ROOM_PRIVATE )
       || xIS_SET( victim->in_room->room_flags, ROOM_SOLITARY )
       || xIS_SET( victim->in_room->room_flags, ROOM_NO_ASTRAL )
       || xIS_SET( victim->in_room->room_flags, ROOM_DEATH )
       || xIS_SET( victim->in_room->room_flags, ROOM_PROTOTYPE )
       || xIS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
       || victim->level >= level + 15
       || ( IS_NPC( victim ) && xIS_SET( victim->act, ACT_PROTOTYPE ) )
       || ( IS_NPC( victim ) && saves_spell_staff( level, victim ) ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }


   if( victim->in_room == ch->in_room )
   {
      send_to_char( "They are right beside you!", ch );
      return rSPELL_FAILED;
   }

   if( ( obj = get_obj_carry( ch, arg3 ) ) == NULL
       || ( victim->carry_weight + get_obj_weight( obj ) ) > can_carry_w( victim )
       || ( IS_NPC( victim ) && xIS_SET( victim->act, ACT_PROTOTYPE ) ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   separate_obj( obj );

   if( IS_OBJ_STAT( obj, ITEM_NODROP ) )
   {
      send_to_char( "You can't seem to let go of it.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) && get_trust( victim ) < LEVEL_IMMORTAL )
   {
      send_to_char( "That item is not for mortal hands to touch!\n\r", ch );
      return rSPELL_FAILED;
   }

   act( AT_MAGIC, "$p slowly dematerializes...", ch, obj, NULL, TO_CHAR );
   act( AT_MAGIC, "$p slowly dematerializes from $n's hands..", ch, obj, NULL, TO_ROOM );
   obj_from_char( obj );
   obj_to_char( obj, victim );
   act( AT_MAGIC, "$p from $n appears in your hands!", ch, obj, victim, TO_VICT );
   act( AT_MAGIC, "$p appears in $n's hands!", victim, obj, NULL, TO_ROOM );
   save_char_obj( ch );
   save_char_obj( victim );
   return rNONE;
}

ch_ret spell_portal( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim;
   ROOM_INDEX_DATA *targetRoom, *fromRoom;
   int targetRoomVnum;
   OBJ_DATA *portalObj;
   EXIT_DATA *pexit;
   char buf[MAX_STRING_LENGTH];
   SKILLTYPE *skill = get_skilltype( sn );

   if( ( victim = get_char_world( ch, target_name ) ) == NULL || !can_portal( ch, victim ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( sysdata.trans_lock == 1 )
   {
      send_to_char( "I'm sorry, global trans lock is on.\n\r", ch );
      return rNONE;
   }

   if( victim->in_room == ch->in_room )
   {
      send_to_char( "They are right beside you!", ch );
      return rSPELL_FAILED;
   }


   targetRoomVnum = victim->in_room->vnum;
   fromRoom = ch->in_room;
   targetRoom = victim->in_room;

   for( pexit = fromRoom->first_exit; pexit; pexit = pexit->next )
   {
      if( IS_SET( pexit->exit_info, EX_PORTAL ) )
      {
         send_to_char( "There is already a portal in this room.\n\r", ch );
         return rSPELL_FAILED;
      }

      if( pexit->vdir == DIR_PORTAL )
      {
         send_to_char( "You may not create a portal in this room.\n\r", ch );
         return rSPELL_FAILED;
      }
   }

   for( pexit = targetRoom->first_exit; pexit; pexit = pexit->next )
      if( pexit->vdir == DIR_PORTAL )
      {
         failed_casting( skill, ch, victim, NULL );
         return rSPELL_FAILED;
      }

   pexit = make_exit( fromRoom, targetRoom, DIR_PORTAL );
   pexit->keyword = STRALLOC( "portal" );
   pexit->description = STRALLOC( "You gaze into the shimmering portal...\n\r" );
   pexit->key = -1;
   pexit->exit_info = EX_PORTAL | EX_xENTER | EX_HIDDEN | EX_xLOOK;
   pexit->vnum = targetRoomVnum;

   portalObj = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
   portalObj->timer = 3;
   sprintf( buf, "a portal created by %s", ch->name );
   STRFREE( portalObj->short_descr );
   portalObj->short_descr = STRALLOC( buf );

   if( !skill->hit_char || skill->hit_char[0] == '\0' )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "You utter an incantation, and a portal forms in front of you!\n\r", ch );
   }
   else
      act( AT_COLORIZE, skill->hit_char, ch, NULL, victim, TO_CHAR );
   if( !skill->hit_room || skill->hit_room[0] == '\0' )
      act( AT_COLORIZE, "$n utters an incantation, and a portal forms in front of you!", ch, NULL, NULL, TO_ROOM );
   else
      act( AT_COLORIZE, skill->hit_room, ch, NULL, victim, TO_ROOM );
   if( !skill->hit_vict || skill->hit_vict[0] == '\0' )
      act( AT_MAGIC, "A shimmering portal forms in front of you!", victim, NULL, NULL, TO_ROOM );
   else
      act( AT_COLORIZE, skill->hit_vict, victim, NULL, victim, TO_ROOM );
   portalObj = obj_to_room( portalObj, ch->in_room );

   pexit = make_exit( targetRoom, fromRoom, DIR_PORTAL );
   pexit->keyword = STRALLOC( "portal" );
   pexit->description = STRALLOC( "You gaze into the shimmering portal...\n\r" );
   pexit->key = -1;
   pexit->exit_info = EX_PORTAL | EX_xENTER | EX_HIDDEN;
   pexit->vnum = targetRoomVnum;

   portalObj = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
   portalObj->timer = 3;
   STRFREE( portalObj->short_descr );
   portalObj->short_descr = STRALLOC( buf );
   portalObj = obj_to_room( portalObj, targetRoom );
   return rNONE;
}

ch_ret spell_farsight( int sn, int level, CHAR_DATA * ch, void *vo )
{
   ROOM_INDEX_DATA *location;
   ROOM_INDEX_DATA *original;
   CHAR_DATA *victim;
   SKILLTYPE *skill = get_skilltype( sn );

   if( ( victim = get_char_world( ch, target_name ) ) == NULL
       || victim == ch
       || !victim->in_room
       || xIS_SET( victim->in_room->room_flags, ROOM_PRIVATE )
       || xIS_SET( victim->in_room->room_flags, ROOM_SOLITARY )
       || xIS_SET( victim->in_room->room_flags, ROOM_NO_ASTRAL )
       || xIS_SET( victim->in_room->room_flags, ROOM_DEATH )
       || xIS_SET( victim->in_room->room_flags, ROOM_PROTOTYPE )
       || xIS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
       || victim->level >= level + 15
       || ( IS_NPC( victim ) && xIS_SET( victim->act, ACT_PROTOTYPE ) )
       || ( IS_NPC( victim ) && saves_spell_staff( level, victim ) )
       || ( !IS_NPC( victim ) && CAN_PKILL( victim ) && !CAN_PKILL( ch ) ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   location = victim->in_room;
   if( !location )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }
   successful_casting( skill, ch, victim, NULL );
   original = ch->in_room;
   char_from_room( ch );
   char_to_room( ch, location );
   do_look( ch, "auto" );
   char_from_room( ch );
   char_to_room( ch, original );
   return rNONE;
}

ch_ret spell_recharge( int sn, int level, CHAR_DATA * ch, void *vo )
{
   OBJ_DATA *obj = ( OBJ_DATA * ) vo;

   if( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND )
   {
      separate_obj( obj );
      if( obj->value[2] == obj->value[1] || obj->value[1] > ( obj->pIndexData->value[1] * 4 ) )
      {
         act( AT_FIRE, "$p bursts into flames, injuring you!", ch, obj, NULL, TO_CHAR );
         act( AT_FIRE, "$p bursts into flames, charring $n!", ch, obj, NULL, TO_ROOM );
         extract_obj( obj );
         if( damage( ch, ch, obj->level * 2, TYPE_UNDEFINED ) == rCHAR_DIED || char_died( ch ) )
            return rCHAR_DIED;
         else
            return rSPELL_FAILED;
      }

      if( chance( ch, 2 ) )
      {
         act( AT_YELLOW, "$p glows with a blinding magical luminescence.", ch, obj, NULL, TO_CHAR );
         obj->value[1] *= 2;
         obj->value[2] = obj->value[1];
         return rNONE;
      }
      else if( chance( ch, 5 ) )
      {
         act( AT_YELLOW, "$p glows brightly for a few seconds...", ch, obj, NULL, TO_CHAR );
         obj->value[2] = obj->value[1];
         return rNONE;
      }
      else if( chance( ch, 10 ) )
      {
         act( AT_WHITE, "$p disintegrates into a void.", ch, obj, NULL, TO_CHAR );
         act( AT_WHITE, "$n's attempt at recharging fails, and $p disintegrates.", ch, obj, NULL, TO_ROOM );
         extract_obj( obj );
         return rSPELL_FAILED;
      }
      else if( chance( ch, 50 - ( ch->level / 2 ) ) )
      {
         send_to_char( "Nothing happens.\n\r", ch );
         return rSPELL_FAILED;
      }
      else
      {
         act( AT_MAGIC, "$p feels warm to the touch.", ch, obj, NULL, TO_CHAR );
         --obj->value[1];
         obj->value[2] = obj->value[1];
         return rNONE;
      }
   }
   else
   {
      send_to_char( "You can't recharge that!\n\r", ch );
      return rSPELL_FAILED;
   }
}

ch_ret spell_remove_invis( int sn, int level, CHAR_DATA * ch, void *vo )
{
   OBJ_DATA *obj;
   SKILLTYPE *skill = get_skilltype( sn );

   if( target_name[0] == '\0' )
   {
      send_to_char( "What should the spell be cast upon?\n\r", ch );
      return rSPELL_FAILED;
   }

   obj = get_obj_carry( ch, target_name );

   if( obj )
   {
      if( !IS_OBJ_STAT( obj, ITEM_INVIS ) )
      {
         send_to_char( "Its not invisible!\n\r", ch );
         return rSPELL_FAILED;
      }

      xREMOVE_BIT( obj->extra_flags, ITEM_INVIS );
      act( AT_MAGIC, "$p becomes visible again.", ch, obj, NULL, TO_CHAR );

      send_to_char( "Ok.\n\r", ch );
      return rNONE;
   }
   else
   {
      CHAR_DATA *victim;

      victim = get_char_room( ch, target_name );

      if( victim )
      {
         if( !can_see( ch, victim ) )
         {
            ch_printf( ch, "You don't see %s!\n\r", target_name );
            return rSPELL_FAILED;
         }

         if( !IS_AFFECTED( victim, AFF_INVISIBLE ) )
         {
            send_to_char( "They are not invisible!\n\r", ch );
            return rSPELL_FAILED;
         }

         if( is_safe( ch, victim ) )
         {
            failed_casting( skill, ch, victim, NULL );
            return rSPELL_FAILED;
         }

         if( IS_SET( victim->immune, RIS_MAGIC ) )
         {
            immune_casting( skill, ch, victim, NULL );
            return rSPELL_FAILED;
         }
         if( !IS_NPC( victim ) )
         {
            if( chance( ch, 50 ) && ch->level + 10 < victim->level )
            {
               failed_casting( skill, ch, victim, NULL );
               return rSPELL_FAILED;
            }
            else
               check_illegal_pk( ch, victim );
         }
         else
         {
            if( chance( ch, 50 ) && ch->level + 15 < victim->level )
            {
               failed_casting( skill, ch, victim, NULL );
               return rSPELL_FAILED;
            }
         }

         affect_strip( victim, gsn_invis );
         affect_strip( victim, gsn_mass_invis );
         xREMOVE_BIT( victim->affected_by, AFF_INVISIBLE );
         successful_casting( skill, ch, victim, NULL );
         return rNONE;
      }

      ch_printf( ch, "You can't find %s!\n\r", target_name );
      return rSPELL_FAILED;
   }
}

ch_ret spell_animate_dead( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *mob;
   OBJ_DATA *corpse;
   OBJ_DATA *corpse_next;
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;
   bool found;
   MOB_INDEX_DATA *pMobIndex;
   AFFECT_DATA af;
   char buf[MAX_STRING_LENGTH];
   SKILLTYPE *skill = get_skilltype( sn );

   found = FALSE;

   for( corpse = ch->in_room->first_content; corpse; corpse = corpse_next )
   {
      corpse_next = corpse->next_content;

      if( corpse->item_type == ITEM_CORPSE_NPC && ( ( corpse->gold_cost != -5 ) ) )
      {
         found = TRUE;
         break;
      }
   }

   if( !found )
   {
      send_to_char( "You cannot find a suitable corpse here.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( get_mob_index( MOB_VNUM_ANIMATED_CORPSE ) == NULL )
   {
      bug( "Vnum 5 not found for spell_animate_dead!", 0 );
      return rNONE;
   }


   if( ( pMobIndex = get_mob_index( ( sh_int ) abs( corpse->gold_cost ) ) ) == NULL )
   {
      bug( "Can not find mob for cost of corpse, spell_animate_dead", 0 );
      return rSPELL_FAILED;
   }

   if( pMobIndex == get_mob_index( MOB_VNUM_DEITY ) )
   {
      send_to_char( "You can't animate the corpse of a Deity's Avatar.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( !IS_NPC( ch ) )
   {
      if( IS_VAMPIRE( ch ) )
      {
         if( !IS_IMMORTAL( ch ) && ch->blood - ( pMobIndex->level / 3 ) < 0 )
         {
            send_to_char( "You do not have enough blood power to reanimate this" " corpse.\n\r", ch );
            return rSPELL_FAILED;
         }
         ch->blood -= pMobIndex->level / 3;
      }
      else if( ch->mana - ( pMobIndex->level * 4 ) < 0 )
      {
         send_to_char( "You do not have enough mana to reanimate this " "corpse.\n\r", ch );
         return rSPELL_FAILED;
      }
      else
         ch->mana -= ( pMobIndex->level * 4 );
   }



   if( IS_IMMORTAL( ch ) || ( chance( ch, 75 ) && pMobIndex->level - ch->level < 10 ) )
   {
      mob = create_mobile( get_mob_index( MOB_VNUM_ANIMATED_CORPSE ) );
      char_to_room( mob, ch->in_room );
      mob->level = UMIN( ch->level / 2, pMobIndex->level );
      mob->race = pMobIndex->race;
      if( !pMobIndex->hitnodice )
         mob->max_hit = pMobIndex->level * 8 + number_range( pMobIndex->level * pMobIndex->level / 4,
                                                             pMobIndex->level * pMobIndex->level );
      else
         mob->max_hit = dice( pMobIndex->hitnodice, pMobIndex->hitsizedice ) + pMobIndex->hitplus;
      mob->max_hit = UMAX( URANGE( mob->max_hit / 4,
                                   ( mob->max_hit * corpse->value[3] ) / 100, ch->level * dice( 20, 10 ) ), 1 );


      mob->hit = mob->max_hit;
      mob->damroll = ch->level / 8;
      mob->hitroll = ch->level / 6;
      mob->alignment = ch->alignment;

      act( AT_MAGIC, "$n makes $T rise from the grave!", ch, NULL, pMobIndex->short_descr, TO_ROOM );
      act( AT_MAGIC, "You make $T rise from the grave!", ch, NULL, pMobIndex->short_descr, TO_CHAR );

      sprintf( buf, "animated corpse %s", pMobIndex->player_name );
      STRFREE( mob->name );
      mob->name = STRALLOC( buf );

      sprintf( buf, "The animated corpse of %s", pMobIndex->short_descr );
      STRFREE( mob->short_descr );
      mob->short_descr = STRALLOC( buf );

      sprintf( buf, "An animated corpse of %s struggles with the horror of its undeath.\n\r", pMobIndex->short_descr );
      STRFREE( mob->long_descr );
      mob->long_descr = STRALLOC( buf );
      add_follower( mob, ch );
      af.type = sn;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.location = 0;
      af.modifier = 0;
      af.bitvector = meb( AFF_CHARM );
      affect_to_char( mob, &af );

      if( corpse->first_content )
         for( obj = corpse->first_content; obj; obj = obj_next )
         {
            obj_next = obj->next_content;
            obj_from_obj( obj );
            obj_to_room( obj, corpse->in_room );
         }

      separate_obj( corpse );
      extract_obj( corpse );
      return rNONE;
   }
   else
   {
      failed_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }
}

ch_ret spell_possess( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim;
   char buf[MAX_STRING_LENGTH];
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );
   int try;

   if( !ch->desc || ch->desc->original )
   {
      send_to_char( "You are not in your original state.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( ( victim = get_char_room( ch, target_name ) ) == NULL )
   {
      send_to_char( "They aren't here!\n\r", ch );
      return rSPELL_FAILED;
   }

   if( victim == ch )
   {
      send_to_char( "You can't possess yourself!\n\r", ch );
      return rSPELL_FAILED;
   }

   if( !IS_NPC( victim ) )
   {
      send_to_char( "You can't possess another player!\n\r", ch );
      return rSPELL_FAILED;
   }

   if( victim->desc )
   {
      ch_printf( ch, "%s is already possessed.\n\r", victim->short_descr );
      return rSPELL_FAILED;
   }

   if( IS_SET( victim->immune, RIS_MAGIC ) || IS_SET( victim->immune, RIS_CHARM ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   try = ris_save( victim, level, RIS_CHARM );

   if( IS_AFFECTED( victim, AFF_POSSESS )
       || IS_AFFECTED( ch, AFF_CHARM )
       || IS_AFFECTED( ch, AFF_JMT )
       || level < victim->level || victim->desc || saves_spell_staff( try, victim ) || !chance( ch, 25 ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   af.duration = sdur - 20;
   af.location = 0;
   af.modifier = 0;
   af.bitvector = meb( AFF_POSSESS );
   affect_to_char( victim, &af );

   sprintf( buf, "You have possessed %s!\n\r", victim->short_descr );

   ch->desc->character = victim;
   ch->desc->original = ch;
   victim->desc = ch->desc;
   ch->desc = NULL;
   ch->switched = victim;
   send_to_char( buf, victim );

   return rNONE;
}

ch_ret spell_knock( int sn, int level, CHAR_DATA * ch, void *vo )
{
   EXIT_DATA *pexit;
   SKILLTYPE *skill = get_skilltype( sn );

   set_char_color( AT_MAGIC, ch );
   if( !( pexit = find_door( ch, target_name, FALSE ) )
       || !IS_SET( pexit->exit_info, EX_CLOSED )
       || !IS_SET( pexit->exit_info, EX_LOCKED ) || IS_SET( pexit->exit_info, EX_PICKPROOF ) )
   {
      failed_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }
   REMOVE_BIT( pexit->exit_info, EX_LOCKED );
   send_to_char( "*Click*\n\r", ch );
   if( pexit->rexit && pexit->rexit->to_room == ch->in_room )
      REMOVE_BIT( pexit->rexit->exit_info, EX_LOCKED );
   check_room_for_traps( ch, TRAP_UNLOCK | trap_door[pexit->vdir] );
   return rNONE;
}

ch_ret spell_dream( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim;
   char arg[MAX_INPUT_LENGTH];

   target_name = one_argument( target_name, arg );
   set_char_color( AT_MAGIC, ch );
   if( !( victim = get_char_world( ch, arg ) ) || victim->in_room->area != ch->in_room->area )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return rSPELL_FAILED;
   }
   if( victim->position != POS_SLEEPING )
   {
      send_to_char( "They aren't asleep.\n\r", ch );
      return rSPELL_FAILED;
   }
   if( !target_name )
   {
      send_to_char( "What do you want them to dream about?\n\r", ch );
      return rSPELL_FAILED;
   }

   set_char_color( AT_TELL, victim );
   ch_printf( victim, "You have dreams about %s telling you '%s'.\n\r", PERS( ch, victim ), target_name );
   successful_casting( get_skilltype( sn ), ch, victim, NULL );
   return rNONE;
}

ch_ret spell_spiral_blast( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   int dam;
   int hpch;
   bool ch_died;

   ch_died = FALSE;

   if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
   {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "You fail to breathe.\n\r", ch );
      return rNONE;
   }

   for( vch = ch->in_room->first_person; vch; vch = vch_next )
   {
      vch_next = vch->next_in_room;
      if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
         continue;

      if( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
      {
         act( AT_MAGIC, "Swirling colours radiate from $n" ", encompassing $N.", ch, ch, vch, TO_ROOM );
         act( AT_MAGIC, "Swirling colours radiate from you," " encompassing $N", ch, ch, vch, TO_CHAR );

         hpch = UMAX( 10, ch->hit );
         dam = number_range( hpch / 14 + 1, hpch / 7 );
         if( saves_breath( level, vch ) )
            dam /= 2;
         if( damage( ch, vch, dam, sn ) == rCHAR_DIED || char_died( ch ) )
            ch_died = TRUE;
      }
   }

   if( ch_died )
      return rCHAR_DIED;
   else
      return rNONE;
}

ch_ret spell_scorching_surge( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level );

   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   act( AT_MAGIC, "A fiery current lashes through $n's body!", ch, NULL, NULL, TO_ROOM );
   act( AT_MAGIC, "A fiery current lashes through your body!", ch, NULL, NULL, TO_CHAR );
   return damage( ch, victim, ( dam * 1.4 ), sn );
}


ch_ret spell_helical_flow( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim;
   SKILLTYPE *skill = get_skilltype( sn );

   if( ( victim = get_char_world( ch, target_name ) ) == NULL
       || victim == ch
       || !victim->in_room
       || victim->in_room->area->plane != ch->in_room->area->plane
       || xIS_SET( victim->in_room->room_flags, ROOM_SOLITARY )
       || xIS_SET( victim->in_room->room_flags, ROOM_NO_ASTRAL )
       || xIS_SET( victim->in_room->room_flags, ROOM_PROTOTYPE )
       || xIS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
       || ( IS_NPC( victim ) && xIS_SET( victim->act, ACT_PROTOTYPE ) )
       || ( IS_NPC( victim ) && saves_spell_staff( level, victim ) )
       || !in_hard_range( ch, victim->in_room->area )
       || ( IS_SET( victim->in_room->area->flags, AFLAG_NOPKILL ) && IS_PKILL( ch ) ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( sysdata.trans_lock == 1 )
   {
      send_to_char( "I'm sorry, global trans lock is on.\n\r", ch );
      return rNONE;
   }

   act( AT_MAGIC, "$n coils into an ascending column of colour," " vanishing into thin air.", ch, NULL, NULL, TO_ROOM );
   char_from_room( ch );
   char_to_room( ch, victim->in_room );
   if( ch->on )
   {
      ch->on = NULL;
      ch->position = POS_STANDING;
   }
   if( ch->position != POS_STANDING )
   {
      ch->position = POS_STANDING;
   }
   act( AT_MAGIC, "A coil of colours descends from above, " "revealing $n as it dissipates.", ch, NULL, NULL, TO_ROOM );
   do_look( ch, "auto" );
   return rNONE;
}

bool check_save( int sn, int level, CHAR_DATA * ch, CHAR_DATA * victim )
{
   SKILLTYPE *skill = get_skilltype( sn );
   bool saved = FALSE;

   if( SPELL_FLAG( skill, SF_PKSENSITIVE ) && !IS_NPC( ch ) && !IS_NPC( victim ) )
      level /= 2;

   if( skill->saves )
      switch ( skill->saves )
      {
         case SS_POISON_DEATH:
            saved = saves_poison_death( level, victim );
            break;
         case SS_ROD_WANDS:
            saved = saves_wands( level, victim );
            break;
         case SS_PARA_PETRI:
            saved = saves_para_petri( level, victim );
            break;
         case SS_BREATH:
            saved = saves_breath( level, victim );
            break;
         case SS_SPELL_STAFF:
            saved = saves_spell_staff( level, victim );
            break;
      }
   return saved;
}

ch_ret spell_attack( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );
   bool saved = check_save( sn, level, ch, victim );
   int dam;
   ch_ret retcode = rNONE;

   if( saved && SPELL_SAVE( skill ) == SE_NEGATE )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }
   if( skill->dice )
      dam = UMAX( 0, dice_parse( ch, level, skill->dice ) );
   else
      dam = dice( 1, level / 2 );
   if( saved )
   {
      switch ( SPELL_SAVE( skill ) )
      {
         case SE_3QTRDAM:
            dam = ( dam * 3 ) / 4;
            break;
         case SE_HALFDAM:
            dam >>= 1;
            break;
         case SE_QUARTERDAM:
            dam >>= 2;
            break;
         case SE_EIGHTHDAM:
            dam >>= 3;
            break;

         case SE_ABSORB:
            act( AT_MAGIC, "$N absorbs your $t!", ch, skill->noun_damage, victim, TO_CHAR );
            act( AT_MAGIC, "You absorb $N's $t!", victim, skill->noun_damage, ch, TO_CHAR );
            act( AT_MAGIC, "$N absorbs $n's $t!", ch, skill->noun_damage, victim, TO_NOTVICT );
            victim->hit = URANGE( 0, victim->hit + dam, victim->max_hit );
            update_pos( victim );
            if( ( dam > 0 && ch->fighting && ch->fighting->who == victim )
                || ( dam > 0 && victim->fighting && victim->fighting->who == ch ) )
            {
               int xp = ch->fighting ? ch->fighting->xp : victim->fighting->xp;
               int xp_gain = ( int )( xp * dam * 2 ) / victim->max_hit;

               gain_exp( ch, 0 - xp_gain );
            }
            if( skill->affects )
               retcode = spell_affectchar( sn, level, ch, victim );
            return retcode;

         case SE_REFLECT:
            return spell_attack( sn, level, victim, ch );
      }
   }
   retcode = damage( ch, victim, dam, sn );
   if( retcode == rNONE && skill->affects
       && !char_died( ch ) && !char_died( victim )
       && ( !is_affected( victim, sn ) || SPELL_FLAG( skill, SF_ACCUMULATIVE ) || SPELL_FLAG( skill, SF_RECASTABLE ) ) )
      retcode = spell_affectchar( sn, level, ch, victim );
   return retcode;
}

ch_ret spell_area_attack( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch, *vch_next;
   SKILLTYPE *skill = get_skilltype( sn );
   bool saved;
   bool affects;
   int dam;
   bool ch_died = FALSE;
   ch_ret retcode = rNONE;

   if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFETY ) )
   {
      failed_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }

   affects = ( skill->affects ? TRUE : FALSE );
   if( skill->hit_char && skill->hit_char[0] != '\0' )
      act( AT_COLORIZE, skill->hit_char, ch, NULL, NULL, TO_CHAR );
   if( skill->hit_room && skill->hit_room[0] != '\0' )
      act( AT_COLORIZE, skill->hit_room, ch, NULL, NULL, TO_ROOM );

   for( vch = ch->in_room->first_person; vch; vch = vch_next )
   {
      vch_next = vch->next_in_room;

      if( !IS_NPC( vch ) && xIS_SET( vch->act, PLR_WIZINVIS ) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
         continue;

      if( vch == ch )
         continue;

      if( is_safe( ch, vch ) )
         continue;

      if( !IS_NPC( ch ) && !IS_NPC( vch ) && !IN_ARENA( ch ) && ( !IS_PKILL( ch ) || !IS_PKILL( vch ) ) )
         continue;

      saved = check_save( sn, level, ch, vch );
      if( saved && SPELL_SAVE( skill ) == SE_NEGATE )
      {
         failed_casting( skill, ch, vch, NULL );
         continue;
      }
      else if( skill->dice )
         dam = dice_parse( ch, level, skill->dice );
      else
         dam = dice( 1, level / 2 );
      if( saved )
      {
         switch ( SPELL_SAVE( skill ) )
         {
            case SE_3QTRDAM:
               dam = ( dam * 3 ) / 4;
               break;
            case SE_HALFDAM:
               dam >>= 1;
               break;
            case SE_QUARTERDAM:
               dam >>= 2;
               break;
            case SE_EIGHTHDAM:
               dam >>= 3;
               break;

            case SE_ABSORB:
               act( AT_MAGIC, "$N absorbs your $t!", ch, skill->noun_damage, vch, TO_CHAR );
               act( AT_MAGIC, "You absorb $N's $t!", vch, skill->noun_damage, ch, TO_CHAR );
               act( AT_MAGIC, "$N absorbs $n's $t!", ch, skill->noun_damage, vch, TO_NOTVICT );
               vch->hit = URANGE( 0, vch->hit + dam, vch->max_hit );
               update_pos( vch );
               if( ( dam > 0 && ch->fighting && ch->fighting->who == vch )
                   || ( dam > 0 && vch->fighting && vch->fighting->who == ch ) )
               {
                  int xp = ch->fighting ? ch->fighting->xp : vch->fighting->xp;
                  int xp_gain = ( int )( xp * dam * 2 ) / vch->max_hit;

                  gain_exp( ch, 0 - xp_gain );
               }
               continue;

            case SE_REFLECT:
               retcode = spell_attack( sn, level, vch, ch );
               if( char_died( ch ) )
               {
                  ch_died = TRUE;
                  break;
               }
               continue;
         }
      }
      retcode = damage( ch, vch, dam, sn );
      if( retcode == rNONE && affects && !char_died( ch ) && !char_died( vch )
          && ( !is_affected( vch, sn ) || SPELL_FLAG( skill, SF_ACCUMULATIVE ) || SPELL_FLAG( skill, SF_RECASTABLE ) ) )
         retcode = spell_affectchar( sn, level, ch, vch );
      if( retcode == rCHAR_DIED || char_died( ch ) )
      {
         ch_died = TRUE;
         break;
      }
   }
   return retcode;
}


ch_ret spell_affectchar( int sn, int level, CHAR_DATA * ch, void *vo )
{
   AFFECT_DATA af;
   ACADIA_AFF *saf;
   SKILLTYPE *skill = get_skilltype( sn );
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int chance;
   ch_ret retcode = rNONE;

   if( SPELL_FLAG( skill, SF_RECASTABLE ) )
      affect_strip( victim, sn );
   for( saf = skill->affects; saf; saf = saf->next )
   {
      if( saf->location >= REVERSE_APPLY )
         victim = ch;
      else
         victim = ( CHAR_DATA * ) vo;
      af.bitvector = meb( saf->bitvector );
      if( saf->bitvector >= 0 && xIS_SET( victim->affected_by, saf->bitvector ) && !SPELL_FLAG( skill, SF_ACCUMULATIVE ) )
         continue;
      switch ( saf->bitvector )
      {
         default:
            af.type = sn;
            break;
         case AFF_POISON:
            af.type = gsn_poison;
            chance = ris_save( victim, level, RIS_POISON );
            if( chance == 1000 )
            {
               retcode = rVICT_IMMUNE;
               if( SPELL_FLAG( skill, SF_STOPONFAIL ) )
                  return retcode;
               continue;
            }
            if( saves_poison_death( chance, victim ) )
            {
               if( SPELL_FLAG( skill, SF_STOPONFAIL ) )
                  return retcode;
               continue;
            }
            break;
         case AFF_BLIND:
            af.type = gsn_blindness;
            break;
         case AFF_CURSE:
            af.type = gsn_curse;
            break;
         case AFF_INVISIBLE:
            af.type = gsn_invis;
            break;
         case AFF_IMPINVISIBLE:
            af.type = gsn_impinvis;
            break;
         case AFF_SLEEP:
            af.type = gsn_sleep;
            chance = ris_save( victim, level, RIS_SLEEP );
            if( chance == 1000 )
            {
               retcode = rVICT_IMMUNE;
               if( SPELL_FLAG( skill, SF_STOPONFAIL ) )
                  return retcode;
               continue;
            }
            break;
         case AFF_CHARM:
            af.type = gsn_charm_person;
            chance = ris_save( victim, level, RIS_CHARM );
            if( chance == 1000 )
            {
               retcode = rVICT_IMMUNE;
               if( SPELL_FLAG( skill, SF_STOPONFAIL ) )
                  return retcode;
               continue;
            }
            break;
         case AFF_JMT:
            af.type = gsn_jmt;
            chance = ris_save( victim, level, RIS_CHARM );
            if( chance == 1000 )
            {
               retcode = rVICT_IMMUNE;
               if( SPELL_FLAG( skill, SF_STOPONFAIL ) )
                  return retcode;
               continue;
            }
            break;
         case AFF_POSSESS:
            af.type = gsn_possess;
            break;
      }
      af.duration = dice_parse( ch, level, saf->duration );
      af.modifier = dice_parse( ch, level, saf->modifier );
      af.location = saf->location % REVERSE_APPLY;


      if( af.duration == 0 )
      {
         int xp_gain;

         switch ( af.location )
         {
            case APPLY_HIT:
               victim->hit = URANGE( 0, victim->hit + af.modifier, victim->max_hit );
               update_pos( victim );
               if( ( af.modifier > 0 && ch->fighting && ch->fighting->who == victim )
                   || ( af.modifier > 0 && victim->fighting && victim->fighting->who == ch ) )
               {
                  int xp = ch->fighting ? ch->fighting->xp : victim->fighting->xp;

                  xp_gain = ( int )( xp * af.modifier * 2 ) / victim->max_hit;
                  gain_exp( ch, 0 - xp_gain );
               }
               if( IS_NPC( victim ) && victim->hit <= 0 )
                  damage( ch, victim, 5, TYPE_UNDEFINED );
               break;
            case APPLY_MANA:
               victim->mana = URANGE( 0, victim->mana + af.modifier, victim->max_mana );
               update_pos( victim );
               break;
            case APPLY_MOVE:
               victim->move = URANGE( 0, victim->move + af.modifier, victim->max_move );
               update_pos( victim );
               break;
            default:
               affect_modify( victim, &af, TRUE );
               break;
         }
      }
      else if( SPELL_FLAG( skill, SF_ACCUMULATIVE ) )
         affect_join( victim, &af );
      else
         affect_to_char( victim, &af );
   }
   update_pos( victim );
   return retcode;
}


ch_ret spell_affect( int sn, int level, CHAR_DATA * ch, void *vo )
{
   ACADIA_AFF *saf;
   SKILLTYPE *skill = get_skilltype( sn );
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   bool groupsp;
   bool areasp;
   bool hitchar = FALSE, hitroom = FALSE, hitvict = FALSE;
   ch_ret retcode;

   if( !skill->affects )
   {
      bug( "spell_affect has no affects sn %d (%s)", sn, ch->name );
      return rNONE;
   }
   if( SPELL_FLAG( skill, SF_GROUPSPELL ) )
      groupsp = TRUE;
   else
      groupsp = FALSE;

   if( SPELL_FLAG( skill, SF_AREA ) )
      areasp = TRUE;
   else
      areasp = FALSE;
   if( !groupsp && !areasp )
   {
      if( !victim )
      {
         failed_casting( skill, ch, victim, NULL );
         return rSPELL_FAILED;
      }

      if( ( skill->type != SKILL_HERB
            && IS_SET( victim->immune, RIS_MAGIC ) ) || is_immune( victim, SPELL_DAMAGE( skill ) ) )
      {
         immune_casting( skill, ch, victim, NULL );
         return rSPELL_FAILED;
      }

      if( is_affected( victim, sn ) && !SPELL_FLAG( skill, SF_ACCUMULATIVE ) && !SPELL_FLAG( skill, SF_RECASTABLE ) )
      {
         failed_casting( skill, ch, victim, NULL );
         return rSPELL_FAILED;
      }

      if( ( saf = skill->affects ) && !saf->next
          && saf->location == APPLY_STRIPSN && !is_affected( victim, dice_parse( ch, level, saf->modifier ) ) )
      {
         failed_casting( skill, ch, victim, NULL );
         return rSPELL_FAILED;
      }

      if( check_save( sn, level, ch, victim ) )
      {
         failed_casting( skill, ch, victim, NULL );
         return rSPELL_FAILED;
      }
   }
   else
   {
      if( skill->hit_char && skill->hit_char[0] != '\0' )
      {
         if( strstr( skill->hit_char, "$N" ) )
            hitchar = TRUE;
         else
            act( AT_COLORIZE, skill->hit_char, ch, NULL, NULL, TO_CHAR );
      }
      if( skill->hit_room && skill->hit_room[0] != '\0' )
      {
         if( strstr( skill->hit_room, "$N" ) )
            hitroom = TRUE;
         else
            act( AT_COLORIZE, skill->hit_room, ch, NULL, NULL, TO_ROOM );
      }
      if( skill->hit_vict && skill->hit_vict[0] != '\0' )
         hitvict = TRUE;
      if( victim )
         victim = victim->in_room->first_person;
      else
         victim = ch->in_room->first_person;
   }
   if( !victim )
   {
      bug( "spell_affect: could not find victim: sn %d", sn );
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   for( ; victim; victim = victim->next_in_room )
   {
      if( groupsp || areasp )
      {
         if( ( groupsp && !is_same_group( victim, ch ) )
             || IS_SET( victim->immune, RIS_MAGIC )
             || is_immune( victim, SPELL_DAMAGE( skill ) )
             || check_save( sn, level, ch, victim ) || ( !SPELL_FLAG( skill, SF_RECASTABLE ) && is_affected( victim, sn ) ) )
            continue;

         if( hitvict && ch != victim )
         {
            act( AT_COLORIZE, skill->hit_vict, ch, NULL, victim, TO_VICT );
            if( hitroom )
            {
               act( AT_COLORIZE, skill->hit_room, ch, NULL, victim, TO_NOTVICT );
               act( AT_COLORIZE, skill->hit_room, ch, NULL, victim, TO_CHAR );
            }
         }
         else if( hitroom )
            act( AT_COLORIZE, skill->hit_room, ch, NULL, victim, TO_ROOM );
         if( ch == victim )
         {
            if( hitvict )
               act( AT_COLORIZE, skill->hit_vict, ch, NULL, ch, TO_CHAR );
            else if( hitchar )
               act( AT_COLORIZE, skill->hit_char, ch, NULL, ch, TO_CHAR );
         }
         else if( hitchar )
            act( AT_COLORIZE, skill->hit_char, ch, NULL, victim, TO_CHAR );
      }
      retcode = spell_affectchar( sn, level, ch, victim );
      if( !groupsp && !areasp )
      {
         if( retcode == rVICT_IMMUNE )
            immune_casting( skill, ch, victim, NULL );
         else
            successful_casting( skill, ch, victim, NULL );
         break;
      }
   }
   return rNONE;
}

ch_ret spell_obj_inv( int sn, int level, CHAR_DATA * ch, void *vo )
{
   OBJ_DATA *obj = ( OBJ_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );

   if( !obj )
   {
      failed_casting( skill, ch, NULL, NULL );
      return rNONE;
   }

   switch ( SPELL_ACTION( skill ) )
   {
      default:
      case SA_NONE:
         return rNONE;

      case SA_CREATE:
         if( SPELL_FLAG( skill, SF_WATER ) )
         {
            int water;
            WEATHER_DATA *weath = ch->in_room->area->weather;

            if( obj->item_type != ITEM_DRINK_CON )
            {
               send_to_char( "It is unable to hold water.\n\r", ch );
               return rSPELL_FAILED;
            }

            if( obj->value[2] != LIQ_WATER && obj->value[1] != 0 )
            {
               send_to_char( "It contains some other liquid.\n\r", ch );
               return rSPELL_FAILED;
            }

            water = UMIN( ( skill->dice ? dice_parse( ch, level, skill->dice ) : level )
                          * ( weath->precip >= 0 ? 2 : 1 ), obj->value[0] - obj->value[1] );

            if( water > 0 )
            {
               separate_obj( obj );
               obj->value[2] = LIQ_WATER;
               obj->value[1] += water;
               if( !is_name( "water", obj->name ) )
               {
                  char buf[MAX_STRING_LENGTH];

                  sprintf( buf, "%s water", obj->name );
                  STRFREE( obj->name );
                  obj->name = STRALLOC( buf );
               }
            }
            successful_casting( skill, ch, NULL, obj );
            return rNONE;
         }
         if( SPELL_DAMAGE( skill ) == SD_FIRE )
         {
         }
         if( SPELL_DAMAGE( skill ) == SD_POISON || SPELL_CLASS( skill ) == SC_DEATH )
         {
            switch ( obj->item_type )
            {
               default:
                  failed_casting( skill, ch, NULL, obj );
                  break;
               case ITEM_COOK:
               case ITEM_FOOD:
               case ITEM_DRINK_CON:
                  separate_obj( obj );
                  obj->value[3] = 1;
                  successful_casting( skill, ch, NULL, obj );
                  break;
            }
            return rNONE;
         }
         if( SPELL_CLASS( skill ) == SC_LIFE
             && ( obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_COOK ) )
         {
            switch ( obj->item_type )
            {
               default:
                  failed_casting( skill, ch, NULL, obj );
                  break;
               case ITEM_COOK:
               case ITEM_FOOD:
               case ITEM_DRINK_CON:
                  separate_obj( obj );
                  obj->value[3] = 0;
                  successful_casting( skill, ch, NULL, obj );
                  break;
            }
            return rNONE;
         }

         if( SPELL_CLASS( skill ) != SC_NONE )
         {
            failed_casting( skill, ch, NULL, obj );
            return rNONE;
         }
         switch ( SPELL_POWER( skill ) )
         {
               OBJ_DATA *clone;

            default:
            case SP_NONE:
               if( ch->level - obj->level < 10 || obj->cost > ch->level * get_curr_int( ch ) * get_curr_wis( ch ) )
               {
                  failed_casting( skill, ch, NULL, obj );
                  return rNONE;
               }
               break;
            case SP_MINOR:
               if( ch->level - obj->level < 20 || obj->cost > ch->level * get_curr_int( ch ) / 5 )
               {
                  failed_casting( skill, ch, NULL, obj );
                  return rNONE;
               }
               break;
            case SP_GREATER:
               if( ch->level - obj->level < 5 || obj->cost > ch->level * 10 * get_curr_int( ch ) * get_curr_wis( ch ) )
               {
                  failed_casting( skill, ch, NULL, obj );
                  return rNONE;
               }
               break;
            case SP_MAJOR:
               if( ch->level - obj->level < 0 || obj->cost > ch->level * 50 * get_curr_int( ch ) * get_curr_wis( ch ) )
               {
                  failed_casting( skill, ch, NULL, obj );
                  return rNONE;
               }
               break;
               clone = clone_object( obj );
               clone->timer = skill->dice ? dice_parse( ch, level, skill->dice ) : 0;
               obj_to_char( clone, ch );
               successful_casting( skill, ch, NULL, obj );
         }
         return rNONE;

      case SA_DESTROY:
      case SA_RESIST:
      case SA_SUSCEPT:
      case SA_DIVINATE:
         if( SPELL_DAMAGE( skill ) == SD_POISON )
         {
            if( obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD || obj->item_type == ITEM_COOK )
            {
               if( obj->item_type == ITEM_COOK && obj->value[2] == 0 )
                  send_to_char( "It looks undercooked.\n\r", ch );
               else if( obj->value[3] != 0 )
                  send_to_char( "You smell poisonous fumes.\n\r", ch );
               else
                  send_to_char( "It looks very delicious.\n\r", ch );
            }
            else
               send_to_char( "It doesn't look poisoned.\n\r", ch );
            return rNONE;
         }
         return rNONE;
      case SA_OBSCURE:
         if( IS_OBJ_STAT( obj, ITEM_INVIS ) || chance( ch, skill->dice ? dice_parse( ch, level, skill->dice ) : 20 ) )
         {
            failed_casting( skill, ch, NULL, NULL );
            return rSPELL_FAILED;
         }
         successful_casting( skill, ch, NULL, obj );
         xSET_BIT( obj->extra_flags, ITEM_INVIS );
         return rNONE;

      case SA_CHANGE:
         return rNONE;
   }
   return rNONE;
}

ch_ret spell_create_obj( int sn, int level, CHAR_DATA * ch, void *vo )
{
   SKILLTYPE *skill = get_skilltype( sn );
   int lvl;
   int vnum = skill->value;
   OBJ_DATA *obj;
   OBJ_INDEX_DATA *oi;

   switch ( SPELL_POWER( skill ) )
   {
      default:
      case SP_NONE:
         lvl = 10;
         break;
      case SP_MINOR:
         lvl = 0;
         break;
      case SP_GREATER:
         lvl = level / 2;
         break;
      case SP_MAJOR:
         lvl = level;
         break;
   }

   if( vnum == 0 )
   {
      if( !str_cmp( target_name, "sword" ) )
         vnum = OBJ_VNUM_SCHOOL_SWORD;
      if( !str_cmp( target_name, "shield" ) )
         vnum = OBJ_VNUM_SCHOOL_SHIELD;
   }

   if( ( oi = get_obj_index( vnum ) ) == NULL || ( obj = create_object( oi, lvl ) ) == NULL )
   {
      failed_casting( skill, ch, NULL, NULL );
      return rNONE;
   }
   obj->timer = skill->dice ? dice_parse( ch, level, skill->dice ) : 0;
   successful_casting( skill, ch, NULL, obj );
   if( CAN_WEAR( obj, ITEM_TAKE ) )
      obj_to_char( obj, ch );
   else
      obj_to_room( obj, ch->in_room );
   return rNONE;
}

ch_ret spell_callmount( int sn, int level, CHAR_DATA * ch, void *vo )
{
   SKILLTYPE *skill = get_skilltype( sn );
   MOB_INDEX_DATA *pMobIndex;
   CHAR_DATA *victim;

   victim = NULL;

   set_char_color( AT_IMMORT, ch );

   if( ( pMobIndex = get_mob_index( MOB_VNUM_CALLMOUNT ) ) == NULL )
   {
      send_to_char( "No mobile has that vnum.\n\r", ch );
      failed_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }
   victim = create_mobile( pMobIndex );
   victim->level = ch->level - 5;
   if( victim->level < 100 )
   {
      victim->max_hit = victim->level * 15;
   }
   else
   {
      victim->max_hit = victim->level * 150;
   }
   victim->hit = victim->max_hit;
   victim->timer = 25;
   char_to_room( victim, ch->in_room );
   act( AT_MAGIC, "$n calls $N forth to $m!", ch, NULL, victim, TO_ROOM );
   ch_printf_color( ch, "&YYou call forth %s to aid you.\n\rYou have 25 ticks to use your mount.\n\r", victim->short_descr );
   return rNONE;
}

ch_ret spell_create_mob( int sn, int level, CHAR_DATA * ch, void *vo )
{
   SKILLTYPE *skill = get_skilltype( sn );
   int lvl;
   int vnum = skill->value;
   CHAR_DATA *mob;
   MOB_INDEX_DATA *mi;
   AFFECT_DATA af;

   switch ( SPELL_POWER( skill ) )
   {
      default:
      case SP_NONE:
         lvl = 20;
         break;
      case SP_MINOR:
         lvl = 5;
         break;
      case SP_GREATER:
         lvl = level / 2;
         break;
      case SP_MAJOR:
         lvl = level;
         break;
   }

   if( vnum == 0 )
   {
      if( !str_cmp( target_name, "cityguard" ) )
         vnum = MOB_VNUM_CITYGUARD;
      if( !str_cmp( target_name, "vampire" ) )
         vnum = MOB_VNUM_VAMPIRE;
   }

   if( ( mi = get_mob_index( vnum ) ) == NULL || ( mob = create_mobile( mi ) ) == NULL )
   {
      failed_casting( skill, ch, NULL, NULL );
      return rNONE;
   }
   mob->level = UMIN( lvl, skill->dice ? dice_parse( ch, level, skill->dice ) : mob->level );
   mob->armor = interpolate( mob->level, 100, -100 );

   mob->max_hit = mob->level * 8 + number_range( mob->level * mob->level / 4, mob->level * mob->level );
   mob->hit = mob->max_hit;
   mob->gold = 0;
   successful_casting( skill, ch, mob, NULL );
   char_to_room( mob, ch->in_room );
   add_follower( mob, ch );
   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = 0;
   af.modifier = 0;
   af.bitvector = meb( AFF_CHARM );
   affect_to_char( mob, &af );
   return rNONE;
}

ch_ret ranged_attack( CHAR_DATA *, char *, OBJ_DATA *, OBJ_DATA *, sh_int, sh_int );

ch_ret spell_acadia( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim;
   struct skill_type *skill = get_skilltype( sn );


   if( !skill )
   {
      bug( "spell_acadia: Called with a null skill for sn %d (%s)", sn, ch->name );
      return rERROR;
   }

   switch ( skill->target )
   {
      case TAR_IGNORE:

         if( SPELL_FLAG( skill, SF_AREA )
             && ( ( SPELL_ACTION( skill ) == SA_DESTROY
                    && SPELL_CLASS( skill ) == SC_LIFE )
                  || ( SPELL_ACTION( skill ) == SA_CREATE && SPELL_CLASS( skill ) == SC_DEATH ) ) )
            return spell_area_attack( sn, level, ch, vo );

         if( SPELL_ACTION( skill ) == SA_CREATE )
         {
            if( SPELL_FLAG( skill, SF_OBJECT ) )
               return spell_create_obj( sn, level, ch, vo );
            if( SPELL_CLASS( skill ) == SC_LIFE )
               return spell_create_mob( sn, level, ch, vo );
         }

         if( SPELL_FLAG( skill, SF_DISTANT )
             && ( victim = get_char_world( ch, target_name ) )
             && !xIS_SET( victim->in_room->room_flags, ROOM_NO_ASTRAL ) && SPELL_FLAG( skill, SF_CHARACTER ) )
            return spell_affect( sn, level, ch, get_char_world( ch, target_name ) );

         if( SPELL_FLAG( skill, SF_CHARACTER ) )
            return spell_affect( sn, level, ch, get_char_room( ch, target_name ) );

         if( skill->range > 0 && ( ( SPELL_ACTION( skill ) == SA_DESTROY
                                     && SPELL_CLASS( skill ) == SC_LIFE )
                                   || ( SPELL_ACTION( skill ) == SA_CREATE && SPELL_CLASS( skill ) == SC_DEATH ) ) )
            return ranged_attack( ch, ranged_target_name, NULL, NULL, sn, skill->range );
         return spell_affect( sn, level, ch, vo );

      case TAR_CHAR_OFFENSIVE:

         if( ( SPELL_ACTION( skill ) == SA_DESTROY
               && SPELL_CLASS( skill ) == SC_LIFE )
             || ( SPELL_ACTION( skill ) == SA_CREATE && SPELL_CLASS( skill ) == SC_DEATH ) )
            return spell_attack( sn, level, ch, vo );

         return spell_affect( sn, level, ch, vo );

      case TAR_CHAR_DEFENSIVE:
      case TAR_CHAR_SELF:
         if( SPELL_FLAG( skill, SF_NOFIGHT ) &&
             ( ch->position == POS_FIGHTING
               || ch->position == POS_EVASIVE
               || ch->position == POS_DEFENSIVE || ch->position == POS_AGGRESSIVE || ch->position == POS_BERSERK ) )
         {
            send_to_char( "You can't concentrate enough for that!\n\r", ch );
            return rNONE;
         }

         if( vo && SPELL_ACTION( skill ) == SA_DESTROY )
         {
            CHAR_DATA *victim = ( CHAR_DATA * ) vo;

            if( SPELL_DAMAGE( skill ) == SD_POISON )
            {
               if( is_affected( victim, gsn_poison ) )
               {
                  affect_strip( victim, gsn_poison );
                  successful_casting( skill, ch, victim, NULL );
                  return rNONE;
               }
               failed_casting( skill, ch, victim, NULL );
               return rSPELL_FAILED;
            }
            if( SPELL_CLASS( skill ) == SC_ILLUSION )
            {
               if( is_affected( victim, gsn_blindness ) )
               {
                  affect_strip( victim, gsn_blindness );
                  successful_casting( skill, ch, victim, NULL );
                  return rNONE;
               }
               failed_casting( skill, ch, victim, NULL );
               return rSPELL_FAILED;
            }
         }
         return spell_affect( sn, level, ch, vo );

      case TAR_OBJ_INV:
         return spell_obj_inv( sn, level, ch, vo );
   }
   return rNONE;
}

ch_ret spell_ethereal_fist( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.9;

   if( saves_spell_staff( level, victim ) )
      dam /= 4;

   return damage( ch, victim, dam, sn );
}

ch_ret spell_kyahu( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 2.2;

   if( saves_spell_staff( level, victim ) )
      dam /= 4;

   return damage( ch, victim, dam, sn );
}

ch_ret spell_spectral_furor( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.2;

   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_hand_of_chaos( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.3;

   if( saves_spell_staff( level, victim ) )
      dam /= 4;
   return damage( ch, victim, dam, sn );
}


ch_ret spell_disruption( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.1;

   if( saves_spell_staff( level, victim ) )
      dam = 0;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_sonic_resonance( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level );

   if( saves_spell_staff( level, victim ) )
      dam = dam * 3 / 4;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_mind_wrack( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) / .2;

   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_mind_wrench( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level );

   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}


ch_ret spell_revive( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level );

   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_sulfurous_spray( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) / .2;

   if( saves_spell_staff( level, victim ) )
      dam /= 4;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_caustic_fount( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level );

   if( saves_spell_staff( level, victim ) )
      dam = dam * 1 / 2;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_acetum_primus( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.5;

   if( saves_spell_staff( level, victim ) )
      dam = 3 * dam / 4;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_galvanic_whip( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level );

   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_magnetic_thrust( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.1;

   if( saves_spell_staff( level, victim ) )
      dam /= 3;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_quantum_spike( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.2;

   if( saves_spell_staff( level, victim ) )
      dam /= 2;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_black_lightning( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   dam = dice( 10, level );

   if( saves_spell_staff( level, victim ) )
      dam /= 4;
   return damage( ch, victim, dam, sn );
}

ch_ret spell_force_lightning( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   if( !LEARNED( ch, gsn_force_lore ) )
   {
      return rSPELL_FAILED;
   }

   level = UMAX( 0, level );
   dam = dice( 10, level ) * 1.15;

   return damage( ch, victim, dam, sn );
}

ch_ret spell_midas_touch( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = NULL;
   int gval;
   OBJ_DATA *obj = ( OBJ_DATA * ) vo;

   if( IS_OBJ_STAT( obj, ITEM_NODROP ) )
   {
      send_to_char( "You can't seem to let go of it.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) && get_trust( ch ) < LEVEL_IMMORTAL )
   {
      send_to_char( "That item is not for mortal hands to touch!\n\r", ch );
      return rSPELL_FAILED;
   }

   if( !CAN_WEAR( obj, ITEM_TAKE ) || ( obj->item_type == ITEM_CORPSE_NPC ) || ( obj->item_type == ITEM_CORPSE_PC ) )
   {
      send_to_char( "You cannot seem to turn this item to katyr!\n\r", ch );
      return rNONE;
   }

   separate_obj( obj );

   gval = obj->cost / 10;
   gval = UMAX( 0, gval );

   if( obj->item_type == ITEM_WEAPON )
   {
      switch ( number_bits( 2 ) )
      {
         case 0:
            victim = get_char_world( ch, "shmalnoth" );
            break;
         case 1:
            victim = get_char_world( ch, "midas001" );
            break;
         case 2:
            victim = get_char_world( ch, "midas003" );
            break;
         case 3:
            victim = get_char_world( ch, "shmalnak" );
            break;
      }
   }
   else if( obj->item_type == ITEM_ARMOR )
   {
      switch ( number_bits( 3 ) )
      {
         case 0:
            victim = get_char_world( ch, "shmalnoth" );
            break;
         case 1:
            victim = get_char_world( ch, "shmalnoth" );
            break;
         case 2:
            victim = get_char_world( ch, "midas001" );
            break;
         case 3:
            victim = get_char_world( ch, "midas001" );
            break;
         case 4:
            victim = get_char_world( ch, "midas003" );
            break;
         case 5:
            victim = get_char_world( ch, "midas005" );
            break;
         case 6:
            victim = get_char_world( ch, "crafter" );
            break;
         case 7:
            victim = get_char_world( ch, "crafter" );
            break;
      }
   }
   else if( obj->item_type == ITEM_SCROLL )
   {
      switch ( number_bits( 2 ) )
      {
         case 0:
            victim = get_char_world( ch, "tatorious" );
            break;
         case 1:
            victim = get_char_world( ch, "midas002" );
            break;
         case 2:
            victim = get_char_world( ch, "midas002" );
            break;
         case 3:
            victim = get_char_world( ch, "midas007" );
            break;
      }
   }
   else if( obj->item_type == ITEM_STAFF )
   {
      switch ( number_bits( 2 ) )
      {
         case 0:
            victim = get_char_world( ch, "tatorious" );
            break;
         case 1:
            victim = get_char_world( ch, "midas007" );
            break;
         case 2:
            victim = get_char_world( ch, "midas007" );
            break;
         case 3:
            victim = get_char_world( ch, "midas008" );
            break;
      }
   }
   else if( obj->item_type == ITEM_WAND )
   {
      switch ( number_bits( 2 ) )
      {
         case 0:
            victim = get_char_world( ch, "tatorious" );
            break;
         case 1:
            victim = get_char_world( ch, "midas006" );
            break;
         case 2:
            victim = get_char_world( ch, "midas007" );
            break;
         case 3:
            victim = get_char_world( ch, "midas008" );
            break;
      }
   }
   else
   {
      victim = get_char_world( ch, "terror" );
   }

   if( victim == NULL )
   {
      ch->gold += gval;

      if( obj_extracted( obj ) )
         return rNONE;
      if( cur_obj == obj->serial )
         global_objcode = rOBJ_SACCED;
      extract_obj( obj );
      send_to_char( "Ok\n\r", ch );
      return rNONE;
   }


   if( ( victim->carry_weight + get_obj_weight( obj ) ) > can_carry_w( victim )
       || ( IS_NPC( victim ) && xIS_SET( victim->act, ACT_PROTOTYPE ) ) )
   {
      ch->gold += gval;

      if( obj_extracted( obj ) )
         return rNONE;
      if( cur_obj == obj->serial )
         global_objcode = rOBJ_SACCED;
      extract_obj( obj );
      send_to_char( "Ok\n\r", ch );
      return rNONE;
   }


   ch->gold += gval;
   obj_from_char( obj );
   obj_to_char( obj, victim );

   send_to_char( "You transmogrify the item to katyr!\n\r", ch );
   return rNONE;
}

ch_ret spell_steel_skin( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;

   if( !LEARNED( ch, gsn_force_lore ) )
   {
      return rSPELL_FAILED;
   }

   if( is_affected( victim, sn ) )
   {
      if( victim == ch )
         send_to_char( "Your skin is already as hard as possible.\n\r", ch );
      else
         act( AT_MAGIC, "$N's skin is already hard as steel.", ch, NULL, victim, TO_CHAR );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = 0;
   af.modifier = 0;
   af.bitvector = meb( AFF_STEEL_SKIN );
   affect_to_char( victim, &af );
   victim->subtype = level * 6;
   send_to_char( "Your skin becomes hard as steel.\n\r", victim );
   if( ch != victim )
      act( AT_MAGIC, "$N's skin becomes hard as steel.", ch, NULL, victim, TO_CHAR );
   return rNONE;
}

ch_ret spell_sith_fury( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   level = UMIN( 51, level );
   dam = dice( 10, level ) * 10;

   act( AT_MAGIC, "$N calls on $S Sith Lord's fury against you.", victim, NULL, ch, TO_CHAR );
   act( AT_MAGIC, "You call on your Sith Lord's fury on $N.", ch, NULL, victim, TO_CHAR );
   act( AT_MAGIC, "$n's Sith Lord's fury is let out on $N.", ch, NULL, victim, TO_NOTVICT );

   return damage( ch, victim, dam, sn );
}

ch_ret spell_seluthiary( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   level = UMAX( 0, level );
   level = UMIN( 51, level );
   dam = dice( 10, level ) * 15;

   act( AT_MAGIC, "Strong black energy hits you.", victim, NULL, ch, TO_CHAR );
   act( AT_MAGIC, "You send strong black energy at $N.", ch, NULL, victim, TO_CHAR );
   act( AT_MAGIC, "Strong black energy hits $N.", ch, NULL, victim, TO_NOTVICT );

   return damage( ch, victim, dam, sn );
}

ch_ret spell_creeping_doom( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   int dam;

   if( sysdata.fight_lock == 1 )
   {
      send_to_char( "I am sorry, fight lock is on.\n\r", ch );
      return rNONE;
   }
   dam = number_range( level, 1000 );

   act( AT_MAGIC, "A swarm of insects hits you.", victim, NULL, ch, TO_CHAR );
   act( AT_MAGIC, "You send a swarm of insects at $N.", ch, NULL, victim, TO_CHAR );
   act( AT_MAGIC, "A swarm of insects hits $N.", ch, NULL, victim, TO_NOTVICT );

   return damage( ch, victim, dam, sn );
}

ch_ret spell_regeneration( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;

   if( is_affected( victim, sn ) )
   {
      if( victim == ch )
         send_to_char( "Your are already regenerating as much as possible.\n\r", ch );
      else
         act( AT_MAGIC, "$N is already regenerating as much as possible.", ch, NULL, victim, TO_CHAR );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_REGEN );
   affect_to_char( victim, &af );
   send_to_char( "You begin to regenerate quicker.\n\r", victim );
   if( ch != victim )
      act( AT_MAGIC, "$N begins to regenerate quicker.", ch, NULL, victim, TO_CHAR );
   return rNONE;
}

ch_ret spell_enchant_armor( int sn, int level, CHAR_DATA * ch, void *vo )
{
   OBJ_DATA *obj = ( OBJ_DATA * ) vo;
   AFFECT_DATA *paf;
   int aenchant;
   int oldaffect, oldlevel, exchance;
   int levdif;

   if( !LEARNED( ch, gsn_armor_lore ) )
   {
      send_to_char( "You don't know enough armor lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( obj->item_type != ITEM_ARMOR && !IS_IMMORTAL( ch ) )
   {
      act( AT_MAGIC, "Winding and twisting about $p your magic fails to take hold.", ch, obj, NULL, TO_CHAR );
      act( AT_MAGIC, "As energy twists and winds about $p, $n's magic fails to take hold.", ch, obj, NULL, TO_NOTVICT );
      return rSPELL_FAILED;
   }

   aenchant = ( number_percent(  ) / 5 ) + ( ( get_curr_int( ch ) / 6 ) + ( get_curr_lck( ch ) / 10 ) );

   separate_obj( obj );
   exchance = 0;
   for( paf = obj->first_affect; paf; paf = paf->next )
   {
      if( paf->location == APPLY_EAAC )
      {
         exchance += paf->modifier;
      }
   }
   if( number_percent(  ) > 30 )
   {
      aenchant += exchance;
      if( obj->level > ch->level )
         aenchant += ( obj->level - ch->level );
   }
   else
   {
      aenchant -= exchance;
      if( obj->level > ch->level )
         aenchant -= ( obj->level - ch->level );
   }
   if( IS_IMMORTAL( ch ) )
      aenchant = 23;

   if( aenchant >= 25 )
   {
      act( AT_FIRE, "In a fiery display $p violently explodes.", ch, obj, NULL, TO_CHAR );
      act( AT_FIRE, "In a shower of shards $p violently explodes.", ch, obj, NULL, TO_NOTVICT );
      extract_obj( obj );
   }
   else if( aenchant >= 10 )
   {
      oldaffect = -1;
      if( aenchant >= 23 )
      {
         oldaffect--;
      }
      if( !obj->first_affect )
      {
         if( !IS_OBJ_STAT( obj, ITEM_MAGIC ) )
         {
            xSET_BIT( obj->extra_flags, ITEM_MAGIC );
         }
         CREATE( paf, AFFECT_DATA, 1 );
         paf->type = -1;
         paf->duration = -1;
         paf->location = APPLY_EAAC;
         paf->modifier = oldaffect;
         xCLEAR_BITS( paf->bitvector );
         LINK( paf, obj->first_affect, obj->last_affect, next, prev );
      }
      else
      {
         for( paf = obj->first_affect; paf; paf = paf->next )
         {
            if( paf->location == APPLY_EAAC )
            {
               oldaffect = paf->modifier;
               UNLINK( paf, obj->first_affect, obj->last_affect, next, prev );
            }
         }
         if( aenchant >= 23 )
         {
            oldaffect -= 2;
         }
         else
         {
            oldaffect--;
         }
         if( !IS_OBJ_STAT( obj, ITEM_MAGIC ) )
         {
            xSET_BIT( obj->extra_flags, ITEM_MAGIC );
         }
         CREATE( paf, AFFECT_DATA, 1 );
         paf->type = -1;
         paf->duration = -1;
         paf->location = APPLY_EAAC;
         paf->modifier = oldaffect;
         xCLEAR_BITS( paf->bitvector );
         LINK( paf, obj->first_affect, obj->last_affect, next, prev );
      }
      if( !IS_IMMORTAL( ch ) )
      {
         oldlevel = obj->level;
         oldlevel++;
         if( obj->level < ch->level )
         {
            levdif = ch->level - obj->level;
            if( levdif > 500 )
               oldlevel++;
            if( levdif > 300 )
               oldlevel++;
            if( levdif > 150 )
               oldlevel++;
            if( levdif > 50 )
               oldlevel++;
         }
         obj->level = oldlevel;
      }

      if( !IS_IMMORTAL( ch ) )
      {
         if( IS_GOOD( ch ) )
         {
            xSET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
         }
         else if( IS_EVIL( ch ) )
         {
            xSET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
         }
         else
         {
            xSET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
            xSET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
         }
      }
      if( aenchant >= 23 )
      {
         xSET_BIT( obj->extra_flags, ITEM_GLOW );
         act( AT_BYE, "$p glows brilliant green.", ch, obj, NULL, TO_CHAR );
         act( AT_BYE, "$p glows brilliant green", ch, obj, NULL, TO_NOTVICT );
      }
      else
      {
         act( AT_MAGIC, "As the magic takes hold $p glows a lovely blue.", ch, obj, NULL, TO_CHAR );
         act( AT_MAGIC, "$n's magic take hold and makes $p glow a lovely blue.", ch, obj, NULL, TO_NOTVICT );
      }
   }
   else if( aenchant >= 6 )
   {
      act( AT_PLAIN, "Nothing Happens.", ch, obj, NULL, TO_CHAR );
      act( AT_PLAIN, "$n's magic fails to take hold on $p.", ch, obj, NULL, TO_NOTVICT );
   }
   else
   {

      for( paf = obj->first_affect; paf; paf = paf->next )
      {
         if( paf->location == APPLY_EAAC )
         {
            UNLINK( paf, obj->first_affect, obj->last_affect, next, prev );
         }
      }

      if( IS_OBJ_STAT( obj, ITEM_DNIGHT ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_DNIGHT );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_NEUTRAL );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_CLERIC ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_CLERIC );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_MAGE ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_MAGE );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_WARRIOR ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_WARRIOR );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_ARCHER ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_ARCHER );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_JEDI ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_JEDI );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_WEREWOLF ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_WEREWOLF );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_DRUID ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_DRUID );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_VAMPIRE ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_VAMPIRE );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_TAEDIEN ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_TAEDIEN );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_THIEF ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_THIEF );
      if( IS_OBJ_STAT( obj, ITEM_NOLOCATE ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_NOLOCATE );
      if( IS_OBJ_STAT( obj, ITEM_NOSCRAP ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_NOSCRAP );
      if( IS_OBJ_STAT( obj, ITEM_INVIS ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_INVIS );
      if( IS_OBJ_STAT( obj, ITEM_GLOW ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_GLOW );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_SORCERER ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_SORCERER );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_BARBARIAN ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_BARBARIAN );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_MALE ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_MALE );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_FEMALE ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_FEMALE );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_PALADIN ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_PALADIN );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_RANGER ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_RANGER );
      if( IS_OBJ_STAT( obj, ITEM_NODROP ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_NODROP );
      if( IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_NOREMOVE );
      if( IS_OBJ_STAT( obj, ITEM_BLESS ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_BLESS );
      if( IS_OBJ_STAT( obj, ITEM_MAGIC ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_MAGIC );

      if( !IS_IMMORTAL( ch ) )
      {
         oldlevel = obj->level;
         oldlevel++;
         if( obj->level < ch->level )
         {
            levdif = ch->level - obj->level;
            if( levdif > 500 )
               oldlevel++;
            if( levdif > 300 )
               oldlevel++;
            if( levdif > 150 )
               oldlevel++;
            if( levdif > 50 )
               oldlevel++;
         }
         obj->level = oldlevel;
      }

      act( AT_MAGIC, "With a dull light $p fades slightly.", ch, obj, NULL, TO_CHAR );
      act( AT_MAGIC, "Suddenly $p glows brightly and then fades.", ch, obj, NULL, TO_NOTVICT );
   }
   return rNONE;
}

ch_ret spell_enchant_weapon( int sn, int level, CHAR_DATA * ch, void *vo )
{
   OBJ_DATA *obj = ( OBJ_DATA * ) vo;
   AFFECT_DATA *paf;
   int aenchant;
   int oldaffect, oldlevel, exchance;
   int levdif;

   if( !LEARNED( ch, gsn_weapon_lore ) )
   {
      send_to_char( "You don't know enough weapon lore.\n\r", ch );
      return rSPELL_FAILED;
   }

   if( obj->item_type != ITEM_WEAPON && !IS_IMMORTAL( ch ) )
   {
      act( AT_MAGIC, "Winding and twisting about $p your magic fails to take hold.", ch, obj, NULL, TO_CHAR );
      act( AT_MAGIC, "As energy twists and winds about $p, $n's magic fails to take hold.", ch, obj, NULL, TO_NOTVICT );
      return rSPELL_FAILED;
   }

   aenchant = ( number_percent(  ) / 5 ) + ( ( get_curr_int( ch ) / 6 ) + ( get_curr_lck( ch ) / 10 ) );

   separate_obj( obj );
   exchance = 0;
   for( paf = obj->first_affect; paf; paf = paf->next )
   {
      if( paf->location == APPLY_EWHITROLL )
      {
         oldaffect += paf->modifier;
      }
      if( paf->location == APPLY_EWDAMROLL )
      {
         oldaffect += paf->modifier;
      }
   }
   if( number_percent(  ) > 30 )
   {
      aenchant += exchance;
      if( obj->level > ch->level )
         aenchant += ( obj->level - ch->level );
   }
   else
   {
      aenchant -= exchance;
      if( obj->level > ch->level )
         aenchant -= ( obj->level - ch->level );
   }
   if( IS_IMMORTAL( ch ) )
      aenchant = 23;

   if( aenchant >= 25 )
   {
      act( AT_FIRE, "In a fiery display $p violently explodes.", ch, obj, NULL, TO_CHAR );
      act( AT_FIRE, "In a shower of shards $p violently explodes.", ch, obj, NULL, TO_NOTVICT );
      extract_obj( obj );
   }
   else if( aenchant >= 10 )
   {
      oldaffect = 1;
      if( aenchant >= 23 )
      {
         oldaffect++;
      }
      if( !obj->first_affect )
      {
         if( !IS_OBJ_STAT( obj, ITEM_MAGIC ) )
         {
            xSET_BIT( obj->extra_flags, ITEM_MAGIC );
         }
         CREATE( paf, AFFECT_DATA, 1 );
         paf->type = -1;
         paf->duration = -1;
         paf->location = APPLY_EWHITROLL;
         paf->modifier = oldaffect;
         xCLEAR_BITS( paf->bitvector );
         LINK( paf, obj->first_affect, obj->last_affect, next, prev );

         CREATE( paf, AFFECT_DATA, 1 );
         paf->type = -1;
         paf->duration = -1;
         paf->location = APPLY_EWDAMROLL;
         paf->modifier = oldaffect;
         xCLEAR_BITS( paf->bitvector );
         LINK( paf, obj->first_affect, obj->last_affect, next, prev );
      }
      else
      {
         for( paf = obj->first_affect; paf; paf = paf->next )
         {
            if( paf->location == APPLY_EWHITROLL )
            {
               oldaffect = paf->modifier;
               UNLINK( paf, obj->first_affect, obj->last_affect, next, prev );
            }
            if( paf->location == APPLY_EWDAMROLL )
            {
               oldaffect = paf->modifier;
               UNLINK( paf, obj->first_affect, obj->last_affect, next, prev );
            }
         }
         if( aenchant >= 23 )
         {
            oldaffect += 2;
         }
         else
         {
            oldaffect++;
         }
         if( !IS_OBJ_STAT( obj, ITEM_MAGIC ) )
         {
            xSET_BIT( obj->extra_flags, ITEM_MAGIC );
         }
         CREATE( paf, AFFECT_DATA, 1 );
         paf->type = -1;
         paf->duration = -1;
         paf->location = APPLY_EWHITROLL;
         paf->modifier = oldaffect;
         xCLEAR_BITS( paf->bitvector );
         LINK( paf, obj->first_affect, obj->last_affect, next, prev );

         CREATE( paf, AFFECT_DATA, 1 );
         paf->type = -1;
         paf->duration = -1;
         paf->location = APPLY_EWDAMROLL;
         paf->modifier = oldaffect;
         xCLEAR_BITS( paf->bitvector );
         LINK( paf, obj->first_affect, obj->last_affect, next, prev );
      }
      if( !IS_IMMORTAL( ch ) )
      {
         oldlevel = obj->level;
         oldlevel++;
         if( obj->level < ch->level )
         {
            levdif = ch->level - obj->level;
            if( levdif > 500 )
               oldlevel++;
            if( levdif > 300 )
               oldlevel++;
            if( levdif > 150 )
               oldlevel++;
            if( levdif > 50 )
               oldlevel++;
         }
         obj->level = oldlevel;
      }

      if( !IS_IMMORTAL( ch ) )
      {
         if( IS_GOOD( ch ) )
         {
            xSET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
         }
         else if( IS_EVIL( ch ) )
         {
            xSET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
         }
         else
         {
            xSET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
            xSET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
         }
      }
      if( aenchant >= 23 )
      {
         xSET_BIT( obj->extra_flags, ITEM_GLOW );
         act( AT_BYE, "$p glows brilliant green.", ch, obj, NULL, TO_CHAR );
         act( AT_BYE, "$p glows brilliant green", ch, obj, NULL, TO_NOTVICT );
      }
      else
      {
         act( AT_MAGIC, "As the magic takes hold $p glows a lovely blue.", ch, obj, NULL, TO_CHAR );
         act( AT_MAGIC, "$n's magic take hold and makes $p glow a lovely blue.", ch, obj, NULL, TO_NOTVICT );
      }
   }
   else if( aenchant >= 6 )
   {
      act( AT_PLAIN, "Nothing Happens.", ch, obj, NULL, TO_CHAR );
      act( AT_PLAIN, "$n's magic fails to take hold on $p.", ch, obj, NULL, TO_NOTVICT );
   }
   else
   {

      for( paf = obj->first_affect; paf; paf = paf->next )
      {
         if( paf->location == APPLY_EWHITROLL )
         {
            UNLINK( paf, obj->first_affect, obj->last_affect, next, prev );
         }
         if( paf->location == APPLY_EWDAMROLL )
         {
            UNLINK( paf, obj->first_affect, obj->last_affect, next, prev );
         }
      }

      if( IS_OBJ_STAT( obj, ITEM_DNIGHT ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_DNIGHT );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_NEUTRAL );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_CLERIC ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_CLERIC );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_MAGE ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_MAGE );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_ARCHER ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_ARCHER );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_JEDI ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_JEDI );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_WEREWOLF ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_WEREWOLF );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_WARRIOR ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_WARRIOR );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_TAEDIEN ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_TAEDIEN );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_DRUID ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_DRUID );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_VAMPIRE ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_VAMPIRE );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_THIEF ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_THIEF );
      if( IS_OBJ_STAT( obj, ITEM_NOLOCATE ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_NOLOCATE );
      if( IS_OBJ_STAT( obj, ITEM_NOSCRAP ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_NOSCRAP );
      if( IS_OBJ_STAT( obj, ITEM_INVIS ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_INVIS );
      if( IS_OBJ_STAT( obj, ITEM_GLOW ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_GLOW );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_SORCERER ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_SORCERER );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_BARBARIAN ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_BARBARIAN );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_MALE ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_MALE );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_FEMALE ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_FEMALE );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_PALADIN ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_PALADIN );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_RANGER ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_RANGER );
      if( IS_OBJ_STAT( obj, ITEM_NODROP ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_NODROP );
      if( IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_NOREMOVE );
      if( IS_OBJ_STAT( obj, ITEM_BLESS ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_BLESS );
      if( IS_OBJ_STAT( obj, ITEM_MAGIC ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_MAGIC );

      if( !IS_IMMORTAL( ch ) )
      {
         oldlevel = obj->level;
         oldlevel++;
         if( obj->level < ch->level )
         {
            levdif = ch->level - obj->level;
            if( levdif > 500 )
               oldlevel++;
            if( levdif > 300 )
               oldlevel++;
            if( levdif > 150 )
               oldlevel++;
            if( levdif > 50 )
               oldlevel++;
         }
         obj->level = oldlevel;
      }

      act( AT_MAGIC, "With a dull light $p fades slightly.", ch, obj, NULL, TO_CHAR );
      act( AT_MAGIC, "Suddenly $p glows brightly and then fades.", ch, obj, NULL, TO_NOTVICT );
   }
   return rNONE;
}

ch_ret spell_godspell( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   AFFECT_DATA af;

   level = 800;

   for( vch = first_char; vch; vch = vch_next )
   {
      vch_next = vch->next;

      if( !IS_NPC( vch ) && !IS_IMMORTAL( vch ) )
      {
         af.type = gsn_godspell;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.location = APPLY_NONE;
         af.modifier = 0;
         af.bitvector = meb( AFF_GODSPELL );
         affect_to_char( vch, &af );

         af.type = gsn_godspell;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.location = APPLY_AC;
         af.modifier = -200;
         af.bitvector = meb( AFF_GODSPELL );
         affect_to_char( vch, &af );

         af.type = gsn_godspell;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.location = APPLY_HITROLL;
         af.modifier = 200;
         af.bitvector = meb( AFF_GODSPELL );
         affect_to_char( vch, &af );

         af.type = gsn_godspell;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.location = APPLY_DAMROLL;
         af.modifier = 300;
         af.bitvector = meb( AFF_GODSPELL );
         affect_to_char( vch, &af );

         af.type = gsn_godspell;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.location = APPLY_AFFECT;
         af.modifier = 0;
         af.bitvector = meb( AFF_SANCTUARY );
         affect_to_char( vch, &af );

         af.type = gsn_godspell;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.location = APPLY_AFFECT;
         af.modifier = 0;
         af.bitvector = meb( AFF_PROTECT );
         affect_to_char( vch, &af );

         af.type = gsn_godspell;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.location = APPLY_AFFECT;
         af.modifier = 0;
         af.bitvector = meb( AFF_FLYING );
         affect_to_char( vch, &af );

         af.type = gsn_godspell;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.location = APPLY_AFFECT;
         af.modifier = 0;
         af.bitvector = meb( AFF_PASS_DOOR );
         affect_to_char( vch, &af );

         af.type = gsn_godspell;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.location = APPLY_AFFECT;
         af.modifier = 0;
         af.bitvector = meb( AFF_DETECT_INVIS );
         affect_to_char( vch, &af );

         af.type = gsn_godspell;
         if( !IS_NPC( ch ) && ch->pcdata->deity )
         {
            af.duration = sdur + ch->pcdata->deity->spldur;
         }
         else
         {
            af.duration = sdur;
         }
         af.location = APPLY_AFFECT;
         af.modifier = 0;
         af.bitvector = meb( AFF_DETECT_HIDDEN );
         affect_to_char( vch, &af );

         act( AT_MAGIC, "$N glows with the light of the gods.", ch, NULL, vch, TO_NOTVICT );
         act( AT_MAGIC, "You glow with the light of the gods.", ch, NULL, vch, TO_VICT );
      }
   }
   send_to_char( "Done.\n\r", ch );
   return rNONE;
}

ch_ret spell_calcify_flesh( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_CALCIFY ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_CALCIFY );
   af.location = APPLY_AC;
   af.modifier = -140;
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Your skin stiffens.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's skin stiffens.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_displacement( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_DISPLACEMENT ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_DISPLACEMENT );
   af.location = APPLY_AC;
   af.modifier = -100;
   affect_to_char( victim, &af );

   act( AT_MAGIC, "You shift phases.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n shifts phases.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_biofeedback( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_BIOFEEDBACK ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_BIOFEEDBACK );
   af.location = APPLY_CON;
   af.modifier = 4;
   affect_to_char( victim, &af );

   act( AT_MAGIC, "You feel your pulse slow.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's pulse slows.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_ironhand( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_IRONHAND ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_IRONHAND );
   af.location = APPLY_STR;
   af.modifier = 3;
   affect_to_char( victim, &af );

   act( AT_MAGIC, "You hands become hard as iron.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's hands become harder.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_battleplan( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_BATTLEPLAN ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_BATTLEPLAN );
   af.location = APPLY_INT;
   af.modifier = 2;
   affect_to_char( victim, &af );

   act( AT_MAGIC, "You are aware of how to kill better.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n is aware of how to kill better.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_quickness( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_QUICKNESS ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_QUICKNESS );
   af.location = APPLY_DEX;
   af.modifier = 3;
   affect_to_char( victim, &af );

   act( AT_MAGIC, "You feel quicker on your feet.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n feels quicker on $s feet.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_hadesarmor( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_HADESARMOR ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_HADESARMOR );
   af.location = APPLY_AC;
   af.modifier = -160;
   affect_to_char( victim, &af );

   act( AT_MAGIC, "You are shielded by Hades' cloak.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n is shielded by Hades' cloak.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_stoneskin( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_STONESKIN ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_STONESKIN );
   af.location = APPLY_AC;
   af.modifier = -60;
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Your skin hardens to a malleable stone.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's skin hardens to a malleable stone.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_armor( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_ARMOR ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_ARMOR );
   af.location = APPLY_AC;
   af.modifier = -100;
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Your armor glows.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's armor glows.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_shield( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_SHIELD ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_SHIELD );
   af.location = APPLY_AC;
   af.modifier = -20;
   affect_to_char( victim, &af );

   act( AT_MAGIC, "A force shield of shimmering blue surrounds you.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "A force shield of shimmering blue surrounds $n.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_detect_evil( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_DETECT_EVIL ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = gsn_de;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_DETECT_EVIL );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Traces of red outline all evil in plain sight.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "A tint of red appears in $n's eyes, mirroring his own vision.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_detect_invis( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_DETECT_INVIS ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = gsn_di;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_DETECT_INVIS );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Your eyes fixate as they gain the ability to see the unseen.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's eyes fixate as they gain the ability to see the unseen.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_detect_magic( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_DETECT_MAGIC ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = gsn_dm;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_DETECT_MAGIC );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Traces of blue outline the magical objects in your field of vision.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "A tint of blue in $n's eyes mirrors $s own perception.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_detect_hidden( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_DETECT_HIDDEN ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = gsn_dh;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_DETECT_HIDDEN );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Your senses are heightened to those of an animal.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's senses are heightened to those of an animal.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_infravision( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_INFRARED ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = gsn_if;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_INFRARED );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Heat appears red through your eyes.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's eyes dart about as they grow accustomed to infravision.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_true_sight( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_TRUESIGHT ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = gsn_ts;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_TRUESIGHT );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Your vision is elevated to the highest plane.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's eyes begin to glow a soft white.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_detect_traps( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_DETECTTRAPS ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = gsn_dt;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_DETECTTRAPS );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "You suddenly grow aware of the dangers about you.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n peers about the room, intent on finding all manner of danger.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_scry( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_SCRYING ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = gsn_sc;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_SCRYING );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "You receive a revelatory vision...", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's eyes glaze over as $e endures a vision...", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_sanctuary( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_SANCTUARY ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_SANCTUARY );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "A luminous aura spreads slowly over your body.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "A luminous aura spreads slowly over $n's body.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_fireshield( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_FIRESHIELD ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_FIRESHIELD );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Mystical flames rise to enshroud you.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "Mystical flames rise to enshroud $n.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_iceshield( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_ICESHIELD ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_ICESHIELD );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "A glistening hail of ice encompasses you.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "A glistening hail of ice encompasses $n.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_shockshield( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_SHOCKSHIELD ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_SHOCKSHIELD );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Torrents of cascading energy form around you.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "Torrents of cascading energy form around $n.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_fly( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_FLYING ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_FLYING );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "You rise into the currents of air...", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n rises into the currents of air...", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_protect( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_PROTECT ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_PROTECT );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "You feel protected.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n feels protected.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_bless( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_GODBLESS ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_GODBLESS );
   af.location = APPLY_SAVING_SPELL;
   af.modifier = -25;
   affect_to_char( victim, &af );

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.bitvector = meb( AFF_GODBLESS );
   af.location = APPLY_HITROLL;
   af.modifier = 25;
   affect_to_char( victim, &af );

   act( AT_MAGIC, "You are blessed by the gods.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n is blessed by the gods.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_frenzy( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_FRENZY ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = ( sdur / 2 ) + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur / 2;
   }
   af.bitvector = meb( AFF_FRENZY );
   af.location = APPLY_AC;
   af.modifier = 120;
   affect_to_char( victim, &af );

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = ( sdur / 2 ) + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur / 2;
   }
   af.bitvector = meb( AFF_FRENZY );
   af.location = APPLY_DAMROLL;
   af.modifier = 14;
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Your blood boils as you frenzy!", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's blood boils as $s frenzies!", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}


ch_ret spell_holysight( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_HOLYSIGHT ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_HOLYSIGHT );
   affect_to_char( victim, &af );

   af.type = gsn_de;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_DETECT_EVIL );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Traces of red outline all evil in plain sight.", victim, NULL, NULL, TO_CHAR );

   af.type = gsn_di;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_DETECT_INVIS );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Your eyes fixate as they gain the ability to see the unseen.", victim, NULL, NULL, TO_CHAR );

   af.type = gsn_dm;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_DETECT_MAGIC );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Traces of blue outline the magical objects in your field of vision.", victim, NULL, NULL, TO_CHAR );

   af.type = gsn_dh;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_DETECT_HIDDEN );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Your senses are heightened to those of an animal.", victim, NULL, NULL, TO_CHAR );

   af.type = gsn_if;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_INFRARED );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Heat appears red through your eyes.", victim, NULL, NULL, TO_CHAR );

   af.type = gsn_ts;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_TRUESIGHT );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "Your vision is elevated to the highest plane.", victim, NULL, NULL, TO_CHAR );

   af.type = gsn_dt;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_DETECTTRAPS );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "You suddenly grow aware of the dangers about you.", victim, NULL, NULL, TO_CHAR );

   af.type = gsn_sc;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_SCRYING );
   affect_to_char( victim, &af );

   act( AT_MAGIC, "You receive a revelatory vision...", victim, NULL, NULL, TO_CHAR );

   act( AT_IMMORT, "$N eyes glow.", ch, NULL, victim, TO_ROOM );
   act( AT_IMMORT, "$N eyes glow.", ch, NULL, victim, TO_CHAR );
   return rNONE;
}

ch_ret spell_jer( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_JER ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_JER );
   affect_to_char( victim, &af );

   if( !IS_AFFECTED( victim, AFF_HADESARMOR ) )
   {
      af.type = gsn_hadesarmor;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.bitvector = meb( AFF_HADESARMOR );
      af.location = APPLY_AC;
      af.modifier = -160;
      affect_to_char( victim, &af );

      act( AT_MAGIC, "You are shielded by Hades' cloak.", victim, NULL, NULL, TO_CHAR );
   }

   if( !IS_AFFECTED( victim, AFF_ARMOR ) )
   {
      af.type = gsn_armor;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.bitvector = meb( AFF_ARMOR );
      af.location = APPLY_AC;
      af.modifier = -100;
      affect_to_char( victim, &af );

      act( AT_MAGIC, "Your armor glows.", victim, NULL, NULL, TO_CHAR );
   }

   if( !IS_AFFECTED( victim, AFF_PROTECT ) )
   {
      af.type = gsn_protect;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.location = APPLY_AFFECT;
      af.modifier = 0;
      af.bitvector = meb( AFF_PROTECT );
      affect_to_char( victim, &af );

      act( AT_MAGIC, "You feel protected.", victim, NULL, NULL, TO_CHAR );
   }

   if( !IS_AFFECTED( victim, AFF_GODBLESS ) )
   {
      af.type = gsn_bless;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.bitvector = meb( AFF_GODBLESS );
      af.location = APPLY_SAVING_SPELL;
      af.modifier = -25;
      affect_to_char( victim, &af );

      af.type = gsn_bless;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.bitvector = meb( AFF_GODBLESS );
      af.location = APPLY_HITROLL;
      af.modifier = 25;
      affect_to_char( victim, &af );

      act( AT_MAGIC, "You are blessed by the gods.", victim, NULL, NULL, TO_CHAR );
   }

   if( !IS_AFFECTED( victim, AFF_FRENZY ) )
   {
      af.type = gsn_frenzy;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = ( sdur / 2 ) + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur / 2;
      }
      af.bitvector = meb( AFF_FRENZY );
      af.location = APPLY_AC;
      af.modifier = 120;
      affect_to_char( victim, &af );

      af.type = gsn_frenzy;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = ( sdur / 2 ) + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur / 2;
      }
      af.bitvector = meb( AFF_FRENZY );
      af.location = APPLY_DAMROLL;
      af.modifier = 14;
      affect_to_char( victim, &af );

      act( AT_MAGIC, "Your blood boils as you frenzy!", victim, NULL, NULL, TO_CHAR );
   }

   if( !IS_AFFECTED( victim, AFF_SANCTUARY ) )
   {
      af.type = gsn_sanctuary;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.location = APPLY_AFFECT;
      af.modifier = 0;
      af.bitvector = meb( AFF_SANCTUARY );
      affect_to_char( victim, &af );

      act( AT_MAGIC, "A luminous aura spreads slowly over your body.", victim, NULL, NULL, TO_CHAR );
   }

   act( AT_IMMORT, "$n shines with power.", victim, NULL, NULL, TO_ROOM );
   act( AT_IMMORT, "You shine with power.", victim, NULL, NULL, TO_CHAR );
   return rNONE;
}

ch_ret spell_jir( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_JIR ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_JIR );
   affect_to_char( victim, &af );

   if( !IS_AFFECTED( victim, AFF_PASS_DOOR ) )
   {
      af.type = gsn_passdoor;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.location = APPLY_AFFECT;
      af.modifier = 0;
      af.bitvector = meb( AFF_PASS_DOOR );
      affect_to_char( victim, &af );

      act( AT_MAGIC, "You turn translucent.", victim, NULL, NULL, TO_CHAR );
   }

   if( !IS_AFFECTED( victim, AFF_FLYING ) )
   {
      af.type = gsn_flying;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.location = APPLY_AFFECT;
      af.modifier = 0;
      af.bitvector = meb( AFF_FLYING );
      affect_to_char( victim, &af );

      act( AT_MAGIC, "You rise into the currents of air...", victim, NULL, NULL, TO_CHAR );
   }

   if( !IS_AFFECTED( victim, AFF_FIRESHIELD ) )
   {
      af.type = gsn_fireshield;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.location = APPLY_AFFECT;
      af.modifier = 0;
      af.bitvector = meb( AFF_FIRESHIELD );
      affect_to_char( victim, &af );

      act( AT_MAGIC, "Mystical flames rise to enshroud you.", victim, NULL, NULL, TO_CHAR );
   }

   if( !IS_AFFECTED( victim, AFF_SHOCKSHIELD ) )
   {
      af.type = gsn_shockshield;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.location = APPLY_AFFECT;
      af.modifier = 0;
      af.bitvector = meb( AFF_SHOCKSHIELD );
      affect_to_char( victim, &af );

      act( AT_MAGIC, "Torrents of cascading energy form around you.", victim, NULL, NULL, TO_CHAR );
   }

   if( !IS_AFFECTED( victim, AFF_ICESHIELD ) )
   {
      af.type = gsn_iceshield;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.location = APPLY_AFFECT;
      af.modifier = 0;
      af.bitvector = meb( AFF_ICESHIELD );
      affect_to_char( victim, &af );

      act( AT_MAGIC, "A glistening hail of ice encompasses you.", victim, NULL, NULL, TO_CHAR );
   }
   act( AT_IMMORT, "$n glows.", victim, NULL, NULL, TO_ROOM );
   act( AT_IMMORT, "You glow.", victim, NULL, NULL, TO_CHAR );
   return rNONE;
}

ch_ret spell_jur( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   AFFECT_DATA af;
   SKILLTYPE *skill = get_skilltype( sn );

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( IS_AFFECTED( victim, AFF_JUR ) )
   {
      failed_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   af.type = sn;
   if( !IS_NPC( ch ) && ch->pcdata->deity )
   {
      af.duration = sdur + ch->pcdata->deity->spldur;
   }
   else
   {
      af.duration = sdur;
   }
   af.location = APPLY_AFFECT;
   af.modifier = 0;
   af.bitvector = meb( AFF_JUR );
   affect_to_char( victim, &af );

   if( !IS_AFFECTED( victim, AFF_CALCIFY ) )
   {
      af.type = gsn_calcify;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.bitvector = meb( AFF_CALCIFY );
      af.location = APPLY_AC;
      af.modifier = -140;
      affect_to_char( victim, &af );

      act( AT_MAGIC, "Your skin stiffens.", victim, NULL, NULL, TO_CHAR );
   }

   if( !IS_AFFECTED( victim, AFF_DISPLACEMENT ) )
   {
      af.type = gsn_displacement;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.bitvector = meb( AFF_DISPLACEMENT );
      af.location = APPLY_AC;
      af.modifier = -100;
      affect_to_char( victim, &af );

      act( AT_MAGIC, "You shift phases.", victim, NULL, NULL, TO_CHAR );
   }

   if( !IS_AFFECTED( victim, AFF_BIOFEEDBACK ) )
   {
      af.type = gsn_biofeedback;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.bitvector = meb( AFF_BIOFEEDBACK );
      af.location = APPLY_CON;
      af.modifier = 4;
      affect_to_char( victim, &af );

      act( AT_MAGIC, "You feel your pulse slow.", victim, NULL, NULL, TO_CHAR );
   }

   if( !IS_AFFECTED( victim, AFF_BATTLEPLAN ) )
   {
      af.type = gsn_battleplan;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.bitvector = meb( AFF_BATTLEPLAN );
      af.location = APPLY_INT;
      af.modifier = 2;
      affect_to_char( victim, &af );

      act( AT_MAGIC, "You are aware of how to kill better.", victim, NULL, NULL, TO_CHAR );
   }

   if( !IS_AFFECTED( victim, AFF_QUICKNESS ) )
   {
      af.type = gsn_quickness;
      if( !IS_NPC( ch ) && ch->pcdata->deity )
      {
         af.duration = sdur + ch->pcdata->deity->spldur;
      }
      else
      {
         af.duration = sdur;
      }
      af.bitvector = meb( AFF_QUICKNESS );
      af.location = APPLY_DEX;
      af.modifier = 3;
      affect_to_char( victim, &af );

      act( AT_MAGIC, "You feel quicker on your feet.", victim, NULL, NULL, TO_CHAR );
   }

   act( AT_IMMORT, "$n begin to glow softly and luminesce!", victim, NULL, NULL, TO_ROOM );
   act( AT_IMMORT, "You begin to glow softly and luminesce!", victim, NULL, NULL, TO_CHAR );
   return rNONE;
}

ch_ret spell_cure_light( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );
   int x = 0;

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   x = dice( 1, 8 ) + ( 3 * 1.5 );
   if( x + victim->hit > victim->max_hit )
   {
      victim->hit = victim->max_hit;
   }
   else
   {
      victim->hit += x;
   }
   act( AT_MAGIC, "Your light wounds mend and your pain ebbs slightly.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's light wounds mend.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_cure_serious( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );
   int x = 0;

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   x = dice( 2, 8 ) + ( 3 * 1.5 );
   if( x + victim->hit > victim->max_hit )
   {
      victim->hit = victim->max_hit;
   }
   else
   {
      victim->hit += x;
   }
   act( AT_MAGIC, "Your serious wounds mend and your pain ebbs slightly.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's serious wounds mend.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_cure_critical( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );
   int x = 0;

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   x = dice( 3, 8 ) + ( 3 * 1.5 );
   if( x + victim->hit > victim->max_hit )
   {
      victim->hit = victim->max_hit;
   }
   else
   {
      victim->hit += x;
   }
   act( AT_MAGIC, "Your critical wounds mend and your pain ebbs slightly.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's critical wounds mend.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_comfort( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );
   int x = 1000;

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( x + victim->hit > victim->max_hit )
   {
      victim->hit = victim->max_hit;
   }
   else
   {
      victim->hit += x;
   }
   act( AT_MAGIC, "You feel a sudden jolt as a high intensity charge sears through your flesh...", victim, NULL, NULL,
        TO_CHAR );
   act( AT_MAGIC, "$n's feels a sudden jolt as a high intensity charge sears through $m.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_heal_i( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );
   int x = 100;

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( x + victim->hit > victim->max_hit )
   {
      victim->hit = victim->max_hit;
   }
   else
   {
      victim->hit += x;
   }
   act( AT_MAGIC, "A warm feeling fills your body.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's wounds heal.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_heal_ii( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );
   int x = 200;

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( x + victim->hit > victim->max_hit )
   {
      victim->hit = victim->max_hit;
   }
   else
   {
      victim->hit += x;
   }
   act( AT_MAGIC, "A warm feeling fills your body.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's wounds heal.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_heal_iii( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );
   int x = 300;

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( x + victim->hit > victim->max_hit )
   {
      victim->hit = victim->max_hit;
   }
   else
   {
      victim->hit += x;
   }
   act( AT_MAGIC, "A warm feeling fills your body.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's wounds heal.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_heal_iv( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );
   int x = 400;

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( x + victim->hit > victim->max_hit )
   {
      victim->hit = victim->max_hit;
   }
   else
   {
      victim->hit += x;
   }
   act( AT_MAGIC, "A warm feeling fills your body.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's wounds heal.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_heal_v( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );
   int x = 500;
   int z = 500;

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( x + victim->hit > victim->max_hit )
   {
      victim->hit = victim->max_hit;
   }
   else
   {
      victim->hit += x;
   }
   if( IS_VAMPIRE( victim ) )
   {
      if( z + victim->blood > victim->max_blood )
      {
         victim->blood = victim->max_blood;
      }
      else
      {
         victim->blood += z;
      }
   }
   else
   {
      if( z + victim->mana > victim->max_mana )
      {
         victim->mana = victim->max_mana;
      }
      else
      {
         victim->mana += z;
      }
   }
   act( AT_MAGIC, "A warm feeling fills your body.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's wounds heal.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}

ch_ret spell_niaver( int sn, int level, CHAR_DATA * ch, void *vo )
{
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *obj, *outer_obj;
   bool found = FALSE;
   SKILLTYPE *skill = get_skilltype( sn );

   if( number_range( 1, 10 ) < 2 )
   {
      immune_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }
   if( target_name[0] == '\0' )
   {
      immune_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }
   if( !IS_AGOD( ch ) && ch->practice < 50 )
   {
      immune_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }

   strcpy( buf, "the corpse of " );
   strcat( buf, target_name );
   for( obj = first_object; obj; obj = obj->next )
   {
      if( !nifty_is_name( buf, obj->short_descr ) )
         continue;

      if( obj->item_type == ITEM_CORPSE_NPC )
         continue;

      found = TRUE;

      outer_obj = obj;
      while( outer_obj->in_obj )
         outer_obj = outer_obj->in_obj;

      separate_obj( outer_obj );
      obj_from_room( outer_obj );
      obj_to_room_cr( outer_obj, ch->in_room, ch );

      act( AT_PLAIN, "Your corpse arrives at you with a flash!", ch, NULL, NULL, TO_CHAR );
      act( AT_PLAIN, "Sparks fly as $T appears here.", ch, NULL, buf, TO_ROOM );
   }

   if( !found )
   {
      immune_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }
   if( !IS_AGOD( ch ) )
   {
      ch->practice -= 50;
   }
   return rNONE;
}

ch_ret spell_ablution( int sn, int level, CHAR_DATA * ch, void *vo )
{
   AFFECT_DATA *paf;
   AFFECT_DATA *paf_next;
   OBJ_DATA *obj, *obj_next;

   if( ch->position != 12 )
   {
      send_to_char( "You must be standing to do this.\n\r", ch );
      return rNONE;
   }

   for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
   {
      obj_next = obj->next_content;
      if( obj->wear_loc != WEAR_NONE && can_see_obj( ch, obj ) )
         remove_obj( ch, obj->wear_loc, TRUE );
   }

   send_to_char( "Your gear has been removed.\n\r", ch );
   act( AT_PLAIN, "$n gear has been removed.", ch, NULL, NULL, TO_ROOM );

   for( paf = ch->first_affect; paf; paf = paf_next )
   {
      paf_next = paf->next;
      affect_remove( ch, paf );
   }

   send_to_char( "Your have been cleansed.\n\r", ch );
   act( AT_PLAIN, "$n has been cleansed.", ch, NULL, NULL, TO_ROOM );

   update_aris( ch );
   return rNONE;
}

ch_ret spell_mana( int sn, int level, CHAR_DATA * ch, void *vo )
{
   CHAR_DATA *victim = ( CHAR_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );
   int x = 100;

   if( IS_SET( victim->immune, RIS_MAGIC ) )
   {
      immune_casting( skill, ch, victim, NULL );
      return rSPELL_FAILED;
   }

   if( x + victim->mana > victim->max_mana )
   {
      victim->mana = victim->max_mana;
   }
   else
   {
      victim->mana += x;
   }
   act( AT_MAGIC, "A jolt runs through your body.", victim, NULL, NULL, TO_CHAR );
   act( AT_MAGIC, "$n's mana is bolstered.", victim, NULL, NULL, TO_ROOM );
   return rNONE;
}


ch_ret spell_correlate( int sn, int level, CHAR_DATA * ch, void *vo )
{
   OBJ_DATA *obj = ( OBJ_DATA * ) vo;
   SKILLTYPE *skill = get_skilltype( sn );
   int chance = 0;

   if( IS_OBJ_STAT( obj, ITEM_DNIGHT ) )
   {
      failed_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }
   if( ch->practice < 50 )
   {
      failed_casting( skill, ch, NULL, NULL );
      return rSPELL_FAILED;
   }

   chance = number_range( 0, 100 );
   separate_obj( obj );
   if( chance < 8 )
   {
      act( AT_MAGIC, "$p shivers violently and explodes into many pieces....", ch, obj, NULL, TO_CHAR );
      act( AT_MAGIC, "$p shivers violently and explodes into many pieces from $n's correlate spell.", ch, obj, NULL,
           TO_ROOM );
      extract_obj( obj );
   }
   else if( chance > 7 && chance < 20 )
   {
      act( AT_MAGIC, "$p goes dark as midnight.", ch, obj, NULL, TO_CHAR );
      act( AT_MAGIC, "$p goes dark as midnight from $n's correlate spell.", ch, obj, NULL, TO_ROOM );
      xSET_BIT( obj->extra_flags, ITEM_DNIGHT );
   }
   else if( chance > 19 )
   {
      if( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
      if( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL ) )
         xREMOVE_BIT( obj->extra_flags, ITEM_ANTI_NEUTRAL );
      act( AT_MAGIC, "$p correlates evenly.", ch, obj, NULL, TO_CHAR );
      act( AT_MAGIC, "$p correlates evenly from $n's correlate spell.", ch, obj, NULL, TO_ROOM );
   }
   ch->practice -= 50;
   return rNONE;
}
