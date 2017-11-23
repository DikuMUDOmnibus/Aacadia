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
/*						Tables module				                    */
/************************************************************************/
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "acadia.h"
#ifdef USE_IMC
#include "imc-mercbase.h"
#include "icec-mercbase.h"
#endif
#if defined(KEY)
#undef KEY
#endif
#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}
bool load_race_file( char *fname );
void write_race_file( int ra );


int top_sn;
int top_herb;
int MAX_PC_CLASS;
int MAX_PC_RACE;

SKILLTYPE *skill_table[MAX_SKILL];
struct class_type *class_table[MAX_CLASS];
RACE_TYPE *race_table[MAX_RACE];
char *title_table[MAX_CLASS][MAX_LEVEL + 1][2];
SKILLTYPE *herb_table[MAX_HERB];
SKILLTYPE *disease_table[MAX_DISEASE];

LANG_DATA *first_lang;
LANG_DATA *last_lang;


char *const skill_tname[] = { "unknown", "Spell", "Skill", "Weapon", "Song", "Tongue", "Herb", "Racial", "Disease" };


bool load_class_file( char *fname )
{
   char buf[MAX_STRING_LENGTH];
   char *word;
   bool fMatch;
   struct class_type *class;
   int cl = -1;
   int tlev = 0;
   FILE *fp;

   sprintf( buf, "%s%s", CLASS_DIR, fname );
   if( ( fp = fopen( buf, "r" ) ) == NULL )
   {
      perror( buf );
      return FALSE;
   }

   CREATE( class, struct class_type, 1 );

   class->attr_second = 0;
   class->attr_deficient = 0;
   xCLEAR_BITS( class->affected );
   class->resist = 0;
   class->suscept = 0;

   for( ;; )
   {
      word = feof( fp ) ? "End" : fread_word( fp );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
         case '*':
            fMatch = TRUE;
            fread_to_eol( fp );
            break;

         case 'A':
            KEY( "Affected", class->affected, fread_bitvector( fp ) );
            KEY( "AttrPrime", class->attr_prime, fread_number( fp ) );
            KEY( "AttrSecond", class->attr_second, fread_number( fp ) );
            KEY( "AttrDeficient", class->attr_deficient, fread_number( fp ) );
            break;

         case 'C':
            KEY( "CharC", class->fCreate, fread_number( fp ) );
            KEY( "Class", cl, fread_number( fp ) );
            KEY( "Cweight", class->can_carry_w, fread_number( fp ) );
            break;

         case 'E':
            if( !str_cmp( word, "End" ) )
            {
               fclose( fp );
               if( cl < 0 || cl >= MAX_CLASS )
               {
                  sprintf( buf, "Load_class_file: Class (%s) bad/not found (%d)",
                           class->who_name ? class->who_name : "name not found", cl );
                  bug( buf, 0 );
                  if( class->who_name )
                     STRFREE( class->who_name );
                  DISPOSE( class );
                  return FALSE;
               }
               class_table[cl] = class;
               return TRUE;
            }
            KEY( "ExpBase", class->exp_base, fread_number( fp ) );
            KEY( "Exptnl", class->exp_tnl, fread_number( fp ) );
            break;

         case 'G':
            KEY( "Guild", class->guild, fread_number( fp ) );
            break;

         case 'H':
            KEY( "Hpcost", class->hpcost, fread_number( fp ) );
            KEY( "HpMax", class->hp_max, fread_number( fp ) );
            KEY( "HpMin", class->hp_min, fread_number( fp ) );
            break;

         case 'M':
            KEY( "Mana", class->fMana, fread_number( fp ) );
            KEY( "Manacost", class->manacost, fread_number( fp ) );
            KEY( "Movecost", class->movecost, fread_number( fp ) );
            break;

         case 'N':
            KEY( "Name", class->who_name, fread_string( fp ) );
            break;

         case 'R':
            KEY( "Resist", class->resist, fread_number( fp ) );
            break;

         case 'S':
            if( !str_cmp( word, "Skill" ) )
            {
               int sn, lev, slev, adp;

               word = fread_word( fp );
               lev = fread_number( fp );
               slev = fread_number( fp );
               adp = fread_number( fp );
               sn = skill_lookup( word );
               if( cl < 0 || cl >= MAX_CLASS )
               {
                  sprintf( buf, "load_class_file: Skill %s -- class bad/not found (%d)", word, cl );
                  bug( buf, 0 );
               }
               else if( !IS_VALID_SN( sn ) )
               {
                  sprintf( buf, "load_class_file: Skill %s unknown", word );
                  bug( buf, 0 );
               }
               else
               {
                  skill_table[sn]->skill_level[cl] = lev;
                  skill_table[sn]->skill_slevel[cl] = slev;
                  skill_table[sn]->skill_adept[cl] = adp;
               }
               fMatch = TRUE;
               break;
            }
            KEY( "Skilladept", class->skill_adept, fread_number( fp ) );
            KEY( "Suscept", class->suscept, fread_number( fp ) );
            break;

         case 'T':
            if( !str_cmp( word, "Title" ) )
            {
               if( cl < 0 || cl >= MAX_CLASS )
               {
                  char *tmp;

                  sprintf( buf, "load_class_file: Title -- class bad/not found (%d)", cl );
                  bug( buf, 0 );
                  tmp = fread_string_nohash( fp );
                  DISPOSE( tmp );
                  tmp = fread_string_nohash( fp );
                  DISPOSE( tmp );
               }
               else if( tlev < MAX_LEVEL + 1 )
               {
                  title_table[cl][tlev][0] = fread_string_nohash( fp );
                  title_table[cl][tlev][1] = fread_string_nohash( fp );
                  ++tlev;
               }
               else
                  bug( "load_class_file: Too many titles" );
               fMatch = TRUE;
               break;
            }
            KEY( "Thac0", class->thac0_00, fread_number( fp ) );
            KEY( "Thac32", class->thac0_32, fread_number( fp ) );
            break;

         case 'W':
            KEY( "Weapon", class->weapon, fread_number( fp ) );
            break;
      }
      if( !fMatch )
      {
         sprintf( buf, "load_class_file: no match: %s", word );
         bug( buf, 0 );
      }
   }
   return FALSE;
}

void load_classes(  )
{
   FILE *fpList;
   char *filename;
   char classlist[256];
   char buf[MAX_STRING_LENGTH];
   int i;

   MAX_PC_CLASS = 0;

   for( i = 0; i < MAX_CLASS; i++ )
      class_table[i] = NULL;

   sprintf( classlist, "%s%s", CLASS_DIR, CLASS_LIST );
   if( ( fpList = fopen( classlist, "r" ) ) == NULL )
   {
      perror( classlist );
      exit( 1 );
   }

   for( ;; )
   {
      filename = feof( fpList ) ? "$" : fread_word( fpList );
      if( filename[0] == '$' )
         break;

      if( !load_class_file( filename ) )
      {
         sprintf( buf, "Cannot load class file: %s", filename );
         bug( buf, 0 );
      }
      else
         MAX_PC_CLASS++;
   }
   fclose( fpList );
   for( i = 0; i < MAX_CLASS; i++ )
   {
      if( class_table[i] == NULL )
      {
         CREATE( class_table[i], struct class_type, 1 );
         create_new_class( i, "" );
      }
   }
   return;
}


void write_class_file( int cl )
{
   FILE *fpout;
   char buf[MAX_STRING_LENGTH];
   char filename[MAX_INPUT_LENGTH];
   struct class_type *class = class_table[cl];
   int x, y, z;

   sprintf( filename, "%s%s.class", CLASSDIR, class->who_name );
   if( ( fpout = fopen( filename, "w" ) ) == NULL )
   {
      sprintf( buf, "Cannot open: %s for writing", filename );
      bug( buf, 0 );
      return;
   }
   fprintf( fpout, "Name        %s~\n", class->who_name );
   fprintf( fpout, "Class       %d\n", cl );
   fprintf( fpout, "AttrPrime   %d\n", class->attr_prime );
   fprintf( fpout, "AttrSecond   %d\n", class->attr_second );
   fprintf( fpout, "AttrDeficient   %d\n", class->attr_deficient );
   fprintf( fpout, "Weapon      %d\n", class->weapon );
   fprintf( fpout, "Guild       %d\n", class->guild );
   fprintf( fpout, "Skilladept  %d\n", class->skill_adept );
   fprintf( fpout, "Thac0       %d\n", class->thac0_00 );
   fprintf( fpout, "Thac32      %d\n", class->thac0_32 );
   fprintf( fpout, "Hpmin       %d\n", class->hp_min );
   fprintf( fpout, "Hpmax       %d\n", class->hp_max );
   fprintf( fpout, "Hpcost       %d\n", class->hpcost );
   fprintf( fpout, "Manacost       %d\n", class->manacost );
   fprintf( fpout, "Movecost       %d\n", class->movecost );
   fprintf( fpout, "Mana        %d\n", class->fMana );
   fprintf( fpout, "CharC       %d\n", class->fCreate );
   fprintf( fpout, "Expbase     %d\n", class->exp_base );
   fprintf( fpout, "Exptnl     %d\n", class->exp_tnl );
   fprintf( fpout, "Affected    %s\n", print_bitvector( &class->affected ) );
   fprintf( fpout, "Resist	 %d\n", class->resist );
   fprintf( fpout, "Suscept	 %d\n", class->suscept );
   fprintf( fpout, "Cweight     %d\n", class->can_carry_w );
   for( x = 0; x < top_sn; x++ )
   {
      if( !skill_table[x]->name || skill_table[x]->name[0] == '\0' )
         break;
      z = skill_table[x]->skill_slevel[cl];
      if( ( y = skill_table[x]->skill_level[cl] ) < LEVEL_IMMORTAL )
         fprintf( fpout, "Skill '%s' %d %d %d\n", skill_table[x]->name, y, z, skill_table[x]->skill_adept[cl] );
   }
   for( x = 0; x <= MAX_LEVEL; x++ )
      fprintf( fpout, "Title\n%s~\n%s~\n", title_table[cl][x][0], title_table[cl][x][1] );
   fprintf( fpout, "End\n" );
   fclose( fpout );
}

void load_races(  )
{
   FILE *fpList;
   char *filename;
   char racelist[256];
   char buf[MAX_STRING_LENGTH];
   int i;

   MAX_PC_RACE = 0;
   for( i = 0; i < MAX_RACE; i++ )
      race_table[i] = NULL;

   sprintf( racelist, "%s%s", RACEDIR, RACE_LIST );
   if( ( fpList = fopen( racelist, "r" ) ) == NULL )
   {
      perror( racelist );
      exit( 1 );
   }

   for( ;; )
   {
      filename = feof( fpList ) ? "$" : fread_word( fpList );
      if( filename[0] == '$' )
         break;

      if( !load_race_file( filename ) )
      {
         sprintf( buf, "Cannot load race file: %s", filename );
         bug( buf, 0 );
      }
      else
         MAX_PC_RACE++;
   }
   for( i = 0; i < MAX_RACE; i++ )
   {
      if( race_table[i] == NULL )
      {
         CREATE( race_table[i], struct race_type, 1 );
         sprintf( race_table[i]->race_name, "%s", "unused" );
      }
   }
   fclose( fpList );
   return;
}

void write_race_file( int ra )
{
   FILE *fpout;
   char buf[MAX_STRING_LENGTH];
   char filename[MAX_INPUT_LENGTH];
   struct race_type *race = race_table[ra];
   int i;
   int x, y;

   if( !race->race_name )
   {
      sprintf( buf, "Race %d has null name, not writing .race file.", ra );
      bug( buf, 0 );
      return;
   }

   sprintf( filename, "%s%s.race", RACEDIR, race->race_name );
   if( ( fpout = fopen( filename, "w+" ) ) == NULL )
   {
      sprintf( buf, "Cannot open: %s for writing", filename );
      bug( buf, 0 );
      return;
   }

   fprintf( fpout, "Name        %s~\n", race->race_name );
   fprintf( fpout, "Race        %d\n", ra );
   fprintf( fpout, "Evolve      %d\n", race->evolve );
   fprintf( fpout, "Classes     %d\n", race->class_restriction );
   fprintf( fpout, "Str_Plus    %d\n", race->str_plus );
   fprintf( fpout, "Dex_Plus    %d\n", race->dex_plus );
   fprintf( fpout, "Wis_Plus    %d\n", race->wis_plus );
   fprintf( fpout, "Int_Plus    %d\n", race->int_plus );
   fprintf( fpout, "Con_Plus    %d\n", race->con_plus );
   fprintf( fpout, "Cha_Plus    %d\n", race->cha_plus );
   fprintf( fpout, "Lck_Plus    %d\n", race->lck_plus );
   fprintf( fpout, "Hit         %d\n", race->hit );
   fprintf( fpout, "Mana        %d\n", race->mana );
   fprintf( fpout, "Affected    %s\n", print_bitvector( &race->affected ) );
   fprintf( fpout, "Resist      %d\n", race->resist );
   fprintf( fpout, "Suscept     %d\n", race->suscept );
   fprintf( fpout, "Language    %d\n", race->language );
   fprintf( fpout, "Align       %d\n", race->alignment );
   fprintf( fpout, "Min_Align  %d\n", race->minalign );
   fprintf( fpout, "Max_Align	%d\n", race->maxalign );
   fprintf( fpout, "AC_Plus    %d\n", race->ac_plus );
   fprintf( fpout, "Exp_Mult   %d\n", race->exp_multiplier );
   fprintf( fpout, "Exp_tnl    %d\n", race->exp_tnl );
   fprintf( fpout, "Attacks    %s\n", print_bitvector( &race->attacks ) );
   fprintf( fpout, "Defenses   %s\n", print_bitvector( &race->defenses ) );
   fprintf( fpout, "Height     %d\n", race->height );
   fprintf( fpout, "Weight     %d\n", race->weight );
   fprintf( fpout, "Hunger_Mod  %d\n", race->hunger_mod );
   fprintf( fpout, "Thirst_mod  %d\n", race->thirst_mod );
   fprintf( fpout, "Mana_Regen  %d\n", race->mana_regen );
   fprintf( fpout, "HP_Regen    %d\n", race->hp_regen );
   fprintf( fpout, "Race_Recall %d\n", race->race_recall );
   fprintf( fpout, "Cweight    %d\n", race->can_carry_w );
   for( i = 0; i <= MAX_WHERE_NAME; i++ )
      fprintf( fpout, "WhereName  %s~\n", race->where_name[i] );

   for( x = 0; x < top_sn; x++ )
   {
      if( !skill_table[x]->name || skill_table[x]->name[0] == '\0' )
         break;
      if( ( y = skill_table[x]->race_level[ra] ) < LEVEL_IMMORTAL )
         fprintf( fpout, "Skill '%s' %d %d\n", skill_table[x]->name, y, skill_table[x]->race_adept[ra] );
   }
   fprintf( fpout, "End\n" );
   fclose( fpout );
}

bool load_race_file( char *fname )
{
   char buf[MAX_STRING_LENGTH];
   char *word;
   char *race_name = NULL;
   bool fMatch;
   struct race_type *race;
   int ra = -1;
   FILE *fp;
   int i, wear = 0;

   sprintf( buf, "%s%s", RACEDIR, fname );
   if( ( fp = fopen( buf, "r" ) ) == NULL )
   {
      perror( buf );
      return FALSE;
   }

   CREATE( race, struct race_type, 1 );
   for( i = 0; i < MAX_WHERE_NAME; i++ )
      race->where_name[i] = where_name[i];

   for( ;; )
   {
      word = feof( fp ) ? "End" : fread_word( fp );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
         case '*':
            fMatch = TRUE;
            fread_to_eol( fp );
            break;

         case 'A':
            KEY( "Align", race->alignment, fread_number( fp ) );
            KEY( "AC_Plus", race->ac_plus, fread_number( fp ) );
            KEY( "Affected", race->affected, fread_bitvector( fp ) );
            KEY( "Attacks", race->attacks, fread_bitvector( fp ) );
            break;

         case 'C':
            KEY( "Con_Plus", race->con_plus, fread_number( fp ) );
            KEY( "Cha_Plus", race->cha_plus, fread_number( fp ) );
            KEY( "Classes", race->class_restriction, fread_number( fp ) );
            KEY( "Cweight", race->can_carry_w, fread_number( fp ) );
            break;


         case 'D':
            KEY( "Dex_Plus", race->dex_plus, fread_number( fp ) );
            KEY( "Defenses", race->defenses, fread_bitvector( fp ) );
            break;

         case 'E':
            if( !str_cmp( word, "End" ) )
            {
               fclose( fp );
               if( ra < 0 || ra >= MAX_RACE )
               {
                  sprintf( buf, "Load_race_file: Race (%s) bad/not found (%d)",
                           race->race_name ? race->race_name : "name not found", ra );
                  bug( buf, 0 );
                  DISPOSE( race );
                  return FALSE;
               }
               race_table[ra] = race;
               return TRUE;
            }

            KEY( "Exp_Mult", race->exp_multiplier, fread_number( fp ) );
            KEY( "Exp_tnl", race->exp_tnl, fread_number( fp ) );
            KEY( "Evolve", race->evolve, fread_number( fp ) );

            break;


         case 'I':
            KEY( "Int_Plus", race->int_plus, fread_number( fp ) );
            break;

         case 'H':
            KEY( "Height", race->height, fread_number( fp ) );
            KEY( "Hit", race->hit, fread_number( fp ) );
            KEY( "HP_Regen", race->hp_regen, fread_number( fp ) );
            KEY( "Hunger_Mod", race->hunger_mod, fread_number( fp ) );
            break;

         case 'L':
            KEY( "Language", race->language, fread_number( fp ) );
            KEY( "Lck_Plus", race->lck_plus, fread_number( fp ) );
            break;


         case 'M':
            KEY( "Mana", race->mana, fread_number( fp ) );
            KEY( "Mana_Regen", race->mana_regen, fread_number( fp ) );
            KEY( "Min_Align", race->minalign, fread_number( fp ) );
            race->minalign = -1000;
            KEY( "Max_Align", race->maxalign, fread_number( fp ) );
            race->maxalign = -1000;
            break;

         case 'N':
            KEY( "Name", race_name, fread_string( fp ) );
            break;

         case 'R':
            KEY( "Race", ra, fread_number( fp ) );
            KEY( "Race_Recall", race->race_recall, fread_number( fp ) );
            KEY( "Resist", race->resist, fread_number( fp ) );
            break;

         case 'S':
            KEY( "Str_Plus", race->str_plus, fread_number( fp ) );
            KEY( "Suscept", race->suscept, fread_number( fp ) );
            if( !str_cmp( word, "Skill" ) )
            {
               int sn, lev, adp;

               word = fread_word( fp );
               lev = fread_number( fp );
               adp = fread_number( fp );
               sn = skill_lookup( word );
               if( ra < 0 || ra >= MAX_RACE )
               {
                  sprintf( buf, "load_race_file: Skill %s -- race bad/not found (%d)", word, ra );
                  bug( buf, 0 );
               }
               else if( !IS_VALID_SN( sn ) )
               {
                  sprintf( buf, "load_race_file: Skill %s unknown", word );
                  bug( buf, 0 );
               }
               else
               {
                  skill_table[sn]->race_level[ra] = lev;
                  skill_table[sn]->race_adept[ra] = adp;
               }
               fMatch = TRUE;
               break;
            }
            break;

         case 'T':
            KEY( "Thirst_Mod", race->thirst_mod, fread_number( fp ) );
            break;

         case 'W':
            KEY( "Weight", race->weight, fread_number( fp ) );
            KEY( "Wis_Plus", race->wis_plus, fread_number( fp ) );
            if( !str_cmp( word, "WhereName" ) )
            {
               if( ra < 0 || ra >= MAX_RACE )
               {
                  char *tmp;

                  sprintf( buf, "load_race_file: Title -- race bad/not found (%d)", ra );
                  bug( buf, 0 );
                  tmp = fread_string_nohash( fp );
                  DISPOSE( tmp );
                  tmp = fread_string_nohash( fp );
                  DISPOSE( tmp );
               }
               else if( wear < MAX_WHERE_NAME + 1 )
               {
                  race->where_name[wear] = fread_string_nohash( fp );
                  ++wear;
               }
               else
                  bug( "load_race_file: Too many where_names" );
               fMatch = TRUE;
               break;
            }
            break;
      }

      if( race_name != NULL )
         sprintf( race->race_name, "%-.16s", race_name );

      if( !fMatch )
      {
         sprintf( buf, "load_race_file: no match: %s", word );
         bug( buf, 0 );
      }
   }
   return FALSE;
}

int skill_comp( SKILLTYPE ** sk1, SKILLTYPE ** sk2 )
{
   SKILLTYPE *skill1 = ( *sk1 );
   SKILLTYPE *skill2 = ( *sk2 );

   if( !skill1 && skill2 )
      return 1;
   if( skill1 && !skill2 )
      return -1;
   if( !skill1 && !skill2 )
      return 0;
   if( skill1->type < skill2->type )
      return -1;
   if( skill1->type > skill2->type )
      return 1;
   return strcmp( skill1->name, skill2->name );
}

void sort_skill_table(  )
{
   log_string( "Sorting skill table..." );
   qsort( &skill_table[1], top_sn - 1, sizeof( SKILLTYPE * ), ( int ( * )( const void *, const void * ) )skill_comp );
}

void remap_slot_numbers(  )
{
   SKILLTYPE *skill;
   ACADIA_AFF *aff;
   char tmp[32];
   int sn;

   log_string( "Remapping slots to sns" );

   for( sn = 0; sn <= top_sn; sn++ )
   {
      if( ( skill = skill_table[sn] ) != NULL )
      {
         for( aff = skill->affects; aff; aff = aff->next )
            if( aff->location == APPLY_WEAPONSPELL
                || aff->location == APPLY_WEARSPELL
                || aff->location == APPLY_REMOVESPELL
                || aff->location == APPLY_STRIPSN || aff->location == APPLY_RECURRINGSPELL )
            {
               sprintf( tmp, "%d", slot_lookup( atoi( aff->modifier ) ) );
               DISPOSE( aff->modifier );
               aff->modifier = str_dup( tmp );
            }
      }
   }
}

void fwrite_skill( FILE * fpout, SKILLTYPE * skill )
{
   ACADIA_AFF *aff;
   int modifier;

   fprintf( fpout, "Name         %s~\n", skill->name );
   fprintf( fpout, "Type         %s\n", skill_tname[skill->type] );
   fprintf( fpout, "Info         %d\n", skill->info );
   fprintf( fpout, "Flags        %d\n", skill->flags );
   if( skill->target )
      fprintf( fpout, "Target       %d\n", skill->target );
   if( skill->minimum_position )
      fprintf( fpout, "Minpos       %d\n", skill->minimum_position + 100 );
   if( skill->spell_sector )
      fprintf( fpout, "Ssector      %d\n", skill->spell_sector );
   if( skill->saves )
      fprintf( fpout, "Saves        %d\n", skill->saves );
   if( skill->slot )
      fprintf( fpout, "Slot         %d\n", skill->slot );
   if( skill->min_mana )
      fprintf( fpout, "Mana         %d\n", skill->min_mana );
   if( skill->beats )
      fprintf( fpout, "Rounds       %d\n", skill->beats );
   if( skill->range )
      fprintf( fpout, "Range        %d\n", skill->range );
   if( skill->guild != -1 )
      fprintf( fpout, "Guild        %d\n", skill->guild );
   if( skill->skill_fun )
      fprintf( fpout, "Code         %s\n", skill_name( skill->skill_fun ) );
   else if( skill->spell_fun )
      fprintf( fpout, "Code         %s\n", spell_name( skill->spell_fun ) );
   fprintf( fpout, "Dammsg       %s~\n", skill->noun_damage );
   if( skill->msg_off && skill->msg_off[0] != '\0' )
      fprintf( fpout, "Wearoff      %s~\n", skill->msg_off );

   if( skill->hit_char && skill->hit_char[0] != '\0' )
      fprintf( fpout, "Hitchar      %s~\n", skill->hit_char );
   if( skill->hit_vict && skill->hit_vict[0] != '\0' )
      fprintf( fpout, "Hitvict      %s~\n", skill->hit_vict );
   if( skill->hit_room && skill->hit_room[0] != '\0' )
      fprintf( fpout, "Hitroom      %s~\n", skill->hit_room );
   if( skill->hit_dest && skill->hit_dest[0] != '\0' )
      fprintf( fpout, "Hitdest      %s~\n", skill->hit_dest );

   if( skill->miss_char && skill->miss_char[0] != '\0' )
      fprintf( fpout, "Misschar     %s~\n", skill->miss_char );
   if( skill->miss_vict && skill->miss_vict[0] != '\0' )
      fprintf( fpout, "Missvict     %s~\n", skill->miss_vict );
   if( skill->miss_room && skill->miss_room[0] != '\0' )
      fprintf( fpout, "Missroom     %s~\n", skill->miss_room );

   if( skill->die_char && skill->die_char[0] != '\0' )
      fprintf( fpout, "Diechar      %s~\n", skill->die_char );
   if( skill->die_vict && skill->die_vict[0] != '\0' )
      fprintf( fpout, "Dievict      %s~\n", skill->die_vict );
   if( skill->die_room && skill->die_room[0] != '\0' )
      fprintf( fpout, "Dieroom      %s~\n", skill->die_room );

   if( skill->imm_char && skill->imm_char[0] != '\0' )
      fprintf( fpout, "Immchar      %s~\n", skill->imm_char );
   if( skill->imm_vict && skill->imm_vict[0] != '\0' )
      fprintf( fpout, "Immvict      %s~\n", skill->imm_vict );
   if( skill->imm_room && skill->imm_room[0] != '\0' )
      fprintf( fpout, "Immroom      %s~\n", skill->imm_room );

   if( skill->dice && skill->dice[0] != '\0' )
      fprintf( fpout, "Dice         %s~\n", skill->dice );
   if( skill->value )
      fprintf( fpout, "Value        %d\n", skill->value );
   if( skill->difficulty )
      fprintf( fpout, "Difficulty   %d\n", skill->difficulty );
   if( skill->participants )
      fprintf( fpout, "Participants %d\n", skill->participants );
   if( skill->components && skill->components[0] != '\0' )
      fprintf( fpout, "Components   %s~\n", skill->components );
   if( skill->teachers && skill->teachers[0] != '\0' )
      fprintf( fpout, "Teachers     %s~\n", skill->teachers );
   for( aff = skill->affects; aff; aff = aff->next )
   {
      fprintf( fpout, "Affect       '%s' %d ", aff->duration, aff->location );
      modifier = atoi( aff->modifier );
      if( ( aff->location == APPLY_WEAPONSPELL
            || aff->location == APPLY_WEARSPELL
            || aff->location == APPLY_REMOVESPELL
            || aff->location == APPLY_STRIPSN || aff->location == APPLY_RECURRINGSPELL ) && IS_VALID_SN( modifier ) )
         fprintf( fpout, "'%d' ", skill_table[modifier]->slot );
      else
         fprintf( fpout, "'%s' ", aff->modifier );
      fprintf( fpout, "%d\n", aff->bitvector );
   }

   if( skill->type != SKILL_HERB )
   {
      int y;
      int min = 1000;
      for( y = 0; y < MAX_CLASS; y++ )
         if( skill->skill_level[y] < min )
            min = skill->skill_level[y];

      fprintf( fpout, "Minlevel     %d\n", min );

   }
   fprintf( fpout, "End\n\n" );
}

void save_skill_table(  )
{
   int x;
   FILE *fpout;

   if( ( fpout = fopen( SKILL_FILE, "w" ) ) == NULL )
   {
      perror( SKILL_FILE );
      bug( "Cannot open skills.dat for writting", 0 );
      return;
   }

   for( x = 0; x < top_sn; x++ )
   {
      if( !skill_table[x]->name || skill_table[x]->name[0] == '\0' )
         break;
      fprintf( fpout, "#SKILL\n" );
      fwrite_skill( fpout, skill_table[x] );
   }
   fprintf( fpout, "#END\n" );
   fclose( fpout );
}

void save_herb_table(  )
{
   int x;
   FILE *fpout;

   if( ( fpout = fopen( HERB_FILE, "w" ) ) == NULL )
   {
      bug( "Cannot open herbs.dat for writting", 0 );
      perror( HERB_FILE );
      return;
   }

   for( x = 0; x < top_herb; x++ )
   {
      if( !herb_table[x]->name || herb_table[x]->name[0] == '\0' )
         break;
      fprintf( fpout, "#HERB\n" );
      fwrite_skill( fpout, herb_table[x] );
   }
   fprintf( fpout, "#END\n" );
   fclose( fpout );
}

void save_xsocials(  )
{
   FILE *fpout;
   XSOCIALTYPE *social;
   int x;

   if( ( fpout = fopen( XSOCIAL_FILE, "w" ) ) == NULL )
   {
      bug( "Cannot open xsocials.dat for writting", 0 );
      perror( XSOCIAL_FILE );
      return;
   }

   for( x = 0; x < 27; x++ )
   {
      for( social = xsocial_index[x]; social; social = social->next )
      {
         if( !social->name || social->name[0] == '\0' )
         {
            bug( "Save_socials: blank social in hash bucket %d", x );
            continue;
         }
         fprintf( fpout, "#XSOCIAL\n" );
         fprintf( fpout, "Name        %s~\n", social->name );
         if( social->char_no_arg )
            fprintf( fpout, "CharNoArg   %s~\n", social->char_no_arg );
         else
            bug( "Save_socials: NULL char_no_arg in hash bucket %d", x );
         if( social->others_no_arg )
            fprintf( fpout, "OthersNoArg %s~\n", social->others_no_arg );
         if( social->char_found )
            fprintf( fpout, "CharFound   %s~\n", social->char_found );
         if( social->others_found )
            fprintf( fpout, "OthersFound %s~\n", social->others_found );
         if( social->vict_found )
            fprintf( fpout, "VictFound   %s~\n", social->vict_found );
         if( social->char_auto )
            fprintf( fpout, "CharAuto    %s~\n", social->char_auto );
         if( social->others_auto )
            fprintf( fpout, "OthersAuto  %s~\n", social->others_auto );
         fprintf( fpout, "Chance  %d\n", social->chance );
         fprintf( fpout, "Pos  %d\n", social->position );
         fprintf( fpout, "Gender  %d\n", social->gender );
         fprintf( fpout, "Self  %d\n", social->self );
         fprintf( fpout, "Stage  %d\n", social->stage );
         fprintf( fpout, "Other  %d\n", social->other );
         fprintf( fpout, "Extra  %d\n", social->extra );
         fprintf( fpout, "End\n\n" );
      }
   }
   fprintf( fpout, "#END\n" );
   fclose( fpout );
}

void save_socials(  )
{
   FILE *fpout;
   SOCIALTYPE *social;
   int x;

   if( ( fpout = fopen( SOCIAL_FILE, "w" ) ) == NULL )
   {
      bug( "Cannot open socials.dat for writting", 0 );
      perror( SOCIAL_FILE );
      return;
   }

   for( x = 0; x < 27; x++ )
   {
      for( social = social_index[x]; social; social = social->next )
      {
         if( !social->name || social->name[0] == '\0' )
         {
            bug( "Save_socials: blank social in hash bucket %d", x );
            continue;
         }
         fprintf( fpout, "#SOCIAL\n" );
         fprintf( fpout, "Name        %s~\n", social->name );
         if( social->char_no_arg )
            fprintf( fpout, "CharNoArg   %s~\n", social->char_no_arg );
         else
            bug( "Save_socials: NULL char_no_arg in hash bucket %d", x );
         if( social->others_no_arg )
            fprintf( fpout, "OthersNoArg %s~\n", social->others_no_arg );
         if( social->char_found )
            fprintf( fpout, "CharFound   %s~\n", social->char_found );
         if( social->others_found )
            fprintf( fpout, "OthersFound %s~\n", social->others_found );
         if( social->vict_found )
            fprintf( fpout, "VictFound   %s~\n", social->vict_found );
         if( social->char_auto )
            fprintf( fpout, "CharAuto    %s~\n", social->char_auto );
         if( social->others_auto )
            fprintf( fpout, "OthersAuto  %s~\n", social->others_auto );
         fprintf( fpout, "End\n\n" );
      }
   }
   fprintf( fpout, "#END\n" );
   fclose( fpout );
}

int get_skill( char *skilltype )
{
   if( !str_cmp( skilltype, "Racial" ) )
      return SKILL_RACIAL;
   if( !str_cmp( skilltype, "Spell" ) )
      return SKILL_SPELL;
   if( !str_cmp( skilltype, "Skill" ) )
      return SKILL_SKILL;
   if( !str_cmp( skilltype, "Song" ) )
      return SKILL_SONG;
   if( !str_cmp( skilltype, "Weapon" ) )
      return SKILL_WEAPON;
   if( !str_cmp( skilltype, "Tongue" ) )
      return SKILL_TONGUE;
   if( !str_cmp( skilltype, "Herb" ) )
      return SKILL_HERB;
   return SKILL_UNKNOWN;
}

void save_commands(  )
{
   FILE *fpout;
   CMDTYPE *command;
   int x;

   if( ( fpout = fopen( COMMAND_FILE, "w" ) ) == NULL )
   {
      bug( "Cannot open commands.dat for writing", 0 );
      perror( COMMAND_FILE );
      return;
   }

   for( x = 0; x < 126; x++ )
   {
      for( command = command_hash[x]; command; command = command->next )
      {
         if( !command->name || command->name[0] == '\0' )
         {
            bug( "Save_commands: blank command in hash bucket %d", x );
            continue;
         }
         fprintf( fpout, "#COMMAND\n" );
         fprintf( fpout, "Name        %s~\n", command->name );
         fprintf( fpout, "Code        %s\n", skill_name( command->do_fun ) );
         if( command->position < 100 )
            fprintf( fpout, "Position    %d\n", command->position + 100 );
         else
            fprintf( fpout, "Position    %d\n", command->position );
         fprintf( fpout, "Level       %d\n", command->level );
         fprintf( fpout, "CShow       %d\n", command->cshow );
         fprintf( fpout, "Log         %d\n", command->log );
         if( command->flags )
            fprintf( fpout, "Flags       %d\n", command->flags );
         fprintf( fpout, "End\n\n" );
      }
   }
   fprintf( fpout, "#END\n" );
   fclose( fpout );
}

SKILLTYPE *fread_skill( FILE * fp )
{
   char buf[MAX_STRING_LENGTH];
   char *word;
   bool fMatch;
   bool got_info = FALSE;
   SKILLTYPE *skill;
   int x;

   CREATE( skill, SKILLTYPE, 1 );
   skill->slot = 0;
   skill->min_mana = 0;
   for( x = 0; x < MAX_CLASS; x++ )
   {
      skill->skill_level[x] = LEVEL_IMMORTAL;
      skill->skill_slevel[x] = LEVEL_IMMORTAL;
      skill->skill_adept[x] = 95;
   }
   for( x = 0; x < MAX_RACE; x++ )
   {
      skill->race_level[x] = LEVEL_IMMORTAL;
      skill->race_adept[x] = 95;
   }
   skill->guild = -1;
   skill->target = 0;
   skill->skill_fun = NULL;
   skill->spell_fun = NULL;
   skill->spell_sector = 0;


   for( ;; )
   {
      word = feof( fp ) ? "End" : fread_word( fp );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
         case '*':
            fMatch = TRUE;
            fread_to_eol( fp );
            break;

         case 'A':
            if( !str_cmp( word, "Affect" ) )
            {
               ACADIA_AFF *aff;

               CREATE( aff, ACADIA_AFF, 1 );
               aff->duration = str_dup( fread_word( fp ) );
               aff->location = fread_number( fp );
               aff->modifier = str_dup( fread_word( fp ) );
               aff->bitvector = fread_number( fp );

               if( !got_info )
               {
                  for( x = 0; x < 32; x++ )
                  {
                     if( IS_SET( aff->bitvector, 1 << x ) )
                     {
                        aff->bitvector = x;
                        break;
                     }
                  }
                  if( x == 32 )
                     aff->bitvector = -1;
               }
               aff->next = skill->affects;
               skill->affects = aff;
               fMatch = TRUE;
               break;
            }
            break;

         case 'C':
            if( !str_cmp( word, "Class" ) )
            {
               int class = fread_number( fp );

               skill->skill_level[class] = fread_number( fp );
               skill->skill_slevel[class] = fread_number( fp );
               skill->skill_adept[class] = fread_number( fp );
               fMatch = TRUE;
               break;
            }
            if( !str_cmp( word, "Code" ) )
            {
               SPELL_FUN *spellfun;
               DO_FUN *dofun;
               char *w = fread_word( fp );

               fMatch = TRUE;
               if( ( spellfun = spell_function( w ) ) != spell_notfound )
               {
                  skill->spell_fun = spellfun;
                  skill->skill_fun = NULL;
               }
               else if( ( dofun = skill_function( w ) ) != skill_notfound )
               {
                  skill->skill_fun = dofun;
                  skill->spell_fun = NULL;
               }
               else
               {
                  bug( "fread_skill: unknown skill/spell %s", w );
                  skill->spell_fun = spell_null;
               }
               break;
            }
            KEY( "Code", skill->spell_fun, spell_function( fread_word( fp ) ) );
            KEY( "Components", skill->components, fread_string_nohash( fp ) );
            break;

         case 'D':
            KEY( "Dammsg", skill->noun_damage, fread_string_nohash( fp ) );
            KEY( "Dice", skill->dice, fread_string_nohash( fp ) );
            KEY( "Diechar", skill->die_char, fread_string_nohash( fp ) );
            KEY( "Dieroom", skill->die_room, fread_string_nohash( fp ) );
            KEY( "Dievict", skill->die_vict, fread_string_nohash( fp ) );
            KEY( "Difficulty", skill->difficulty, fread_number( fp ) );
            break;

         case 'E':
            if( !str_cmp( word, "End" ) )
            {
               if( skill->saves != 0 && SPELL_SAVE( skill ) == SE_NONE )
               {
                  bug( "fread_skill(%s):  Has saving throw (%d) with no saving effect.", skill->name, skill->saves );
                  SET_SSAV( skill, SE_NEGATE );
               }
               return skill;
            }
            break;

         case 'F':
            if( !str_cmp( word, "Flags" ) )
            {
               skill->flags = fread_number( fp );
               if( !got_info )
               {
                  skill->info = skill->flags & ( BV11 - 1 );
                  if( IS_SET( skill->flags, OLD_SF_SAVE_NEGATES ) )
                  {
                     if( IS_SET( skill->flags, OLD_SF_SAVE_HALF_DAMAGE ) )
                     {
                        SET_SSAV( skill, SE_QUARTERDAM );
                        REMOVE_BIT( skill->flags, OLD_SF_SAVE_HALF_DAMAGE );
                     }
                     else
                        SET_SSAV( skill, SE_NEGATE );
                     REMOVE_BIT( skill->flags, OLD_SF_SAVE_NEGATES );
                  }
                  else if( IS_SET( skill->flags, OLD_SF_SAVE_HALF_DAMAGE ) )
                  {
                     SET_SSAV( skill, SE_HALFDAM );
                     REMOVE_BIT( skill->flags, OLD_SF_SAVE_HALF_DAMAGE );
                  }
                  skill->flags >>= 11;
               }
               fMatch = TRUE;
               break;
            }
            break;

         case 'G':
            KEY( "Guild", skill->guild, fread_number( fp ) );
            break;

         case 'H':
            KEY( "Hitchar", skill->hit_char, fread_string_nohash( fp ) );
            KEY( "Hitdest", skill->hit_dest, fread_string_nohash( fp ) );
            KEY( "Hitroom", skill->hit_room, fread_string_nohash( fp ) );
            KEY( "Hitvict", skill->hit_vict, fread_string_nohash( fp ) );
            break;

         case 'I':
            KEY( "Immchar", skill->imm_char, fread_string_nohash( fp ) );
            KEY( "Immroom", skill->imm_room, fread_string_nohash( fp ) );
            KEY( "Immvict", skill->imm_vict, fread_string_nohash( fp ) );
            if( !str_cmp( word, "Info" ) )
            {
               skill->info = fread_number( fp );
               got_info = TRUE;
               fMatch = TRUE;
               break;
            }
            break;

         case 'M':
            KEY( "Mana", skill->min_mana, fread_number( fp ) );
            if( !str_cmp( word, "Minlevel" ) )
            {
               fread_to_eol( fp );
               fMatch = TRUE;
               break;
            }
            if( !str_cmp( word, "Minpos" ) )
            {
               fMatch = TRUE;
               skill->minimum_position = fread_number( fp );
               if( skill->minimum_position < 100 )
               {
                  switch ( skill->minimum_position )
                  {
                     default:
                     case 0:
                     case 1:
                     case 2:
                     case 3:
                     case 4:
                        break;
                     case 5:
                        skill->minimum_position = 6;
                        break;
                     case 6:
                        skill->minimum_position = 8;
                        break;
                     case 7:
                        skill->minimum_position = 9;
                        break;
                     case 8:
                        skill->minimum_position = 12;
                        break;
                     case 9:
                        skill->minimum_position = 13;
                        break;
                     case 10:
                        skill->minimum_position = 14;
                        break;
                     case 11:
                        skill->minimum_position = 15;
                        break;
                  }
               }
               else
                  skill->minimum_position -= 100;
               break;
            }

            KEY( "Misschar", skill->miss_char, fread_string_nohash( fp ) );
            KEY( "Missroom", skill->miss_room, fread_string_nohash( fp ) );
            KEY( "Missvict", skill->miss_vict, fread_string_nohash( fp ) );
            break;

         case 'N':
            KEY( "Name", skill->name, fread_string_nohash( fp ) );
            break;

         case 'P':
            KEY( "Participants", skill->participants, fread_number( fp ) );
            break;

         case 'R':
            KEY( "Range", skill->range, fread_number( fp ) );
            KEY( "Rounds", skill->beats, fread_number( fp ) );
            if( !str_cmp( word, "Race" ) )
            {
               int race = fread_number( fp );

               skill->race_level[race] = fread_number( fp );
               skill->race_adept[race] = fread_number( fp );
               fMatch = TRUE;
               break;
            }
            break;

         case 'S':
            KEY( "Saves", skill->saves, fread_number( fp ) );
            KEY( "Slot", skill->slot, fread_number( fp ) );
            KEY( "Ssector", skill->spell_sector, fread_number( fp ) );
            break;

         case 'T':
            KEY( "Target", skill->target, fread_number( fp ) );
            KEY( "Teachers", skill->teachers, fread_string_nohash( fp ) );
            KEY( "Type", skill->type, get_skill( fread_word( fp ) ) );
            break;

         case 'V':
            KEY( "Value", skill->value, fread_number( fp ) );
            break;

         case 'W':
            KEY( "Wearoff", skill->msg_off, fread_string_nohash( fp ) );
            break;
      }

      if( !fMatch )
      {
         sprintf( buf, "Fread_skill: no match: %s", word );
         bug( buf, 0 );
      }
   }
}

void load_skill_table(  )
{
   FILE *fp;

   if( ( fp = fopen( SKILL_FILE, "r" ) ) != NULL )
   {
      top_sn = 0;
      for( ;; )
      {
         char letter;
         char *word;

         letter = fread_letter( fp );
         if( letter == '*' )
         {
            fread_to_eol( fp );
            continue;
         }

         if( letter != '#' )
         {
            bug( "Load_skill_table: # not found.", 0 );
            break;
         }

         word = fread_word( fp );
         if( !str_cmp( word, "SKILL" ) )
         {
            if( top_sn >= MAX_SKILL )
            {
               bug( "load_skill_table: more skills than MAX_SKILL %d", MAX_SKILL );
               fclose( fp );
               return;
            }
            skill_table[top_sn++] = fread_skill( fp );
            continue;
         }
         else if( !str_cmp( word, "END" ) )
            break;
         else
         {
            bug( "Load_skill_table: bad section.", 0 );
            continue;
         }
      }
      fclose( fp );
   }
   else
   {
      perror( SKILL_FILE );
      bug( "Cannot open skills.dat", 0 );
      exit( 0 );
   }
}


void load_herb_table(  )
{
   FILE *fp;

   if( ( fp = fopen( HERB_FILE, "r" ) ) != NULL )
   {
      top_herb = 0;
      for( ;; )
      {
         char letter;
         char *word;

         letter = fread_letter( fp );
         if( letter == '*' )
         {
            fread_to_eol( fp );
            continue;
         }

         if( letter != '#' )
         {
            bug( "Load_herb_table: # not found.", 0 );
            break;
         }

         word = fread_word( fp );
         if( !str_cmp( word, "HERB" ) )
         {
            if( top_herb >= MAX_HERB )
            {
               bug( "load_herb_table: more herbs than MAX_HERB %d", MAX_HERB );
               fclose( fp );
               return;
            }
            herb_table[top_herb++] = fread_skill( fp );
            if( herb_table[top_herb - 1]->slot == 0 )
               herb_table[top_herb - 1]->slot = top_herb - 1;
            continue;
         }
         else if( !str_cmp( word, "END" ) )
            break;
         else
         {
            bug( "Load_herb_table: bad section.", 0 );
            continue;
         }
      }
      fclose( fp );
   }
   else
   {
      bug( "Cannot open herbs.dat", 0 );
      exit( 0 );
   }
}

void fread_xsocial( FILE * fp )
{
   char buf[MAX_STRING_LENGTH];
   char *word;
   bool fMatch;
   XSOCIALTYPE *social;

   CREATE( social, XSOCIALTYPE, 1 );

   for( ;; )
   {
      word = feof( fp ) ? "End" : fread_word( fp );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
         case '*':
            fMatch = TRUE;
            fread_to_eol( fp );
            break;

         case 'C':
            KEY( "Chance", social->chance, fread_number( fp ) );
            KEY( "CharNoArg", social->char_no_arg, fread_string_nohash( fp ) );
            KEY( "CharFound", social->char_found, fread_string_nohash( fp ) );
            KEY( "CharAuto", social->char_auto, fread_string_nohash( fp ) );
            break;

         case 'E':
            if( !str_cmp( word, "End" ) )
            {
               if( !social->name )
               {
                  bug( "Fread_social: Name not found", 0 );
                  free_xsocial( social );
                  return;
               }
               if( !social->char_no_arg )
               {
                  bug( "Fread_social: CharNoArg not found", 0 );
                  free_xsocial( social );
                  return;
               }
               add_xsocial( social );
               return;
            }
            KEY( "Extra", social->extra, fread_number( fp ) );
            break;

         case 'G':
            KEY( "Gender", social->gender, fread_number( fp ) );
            break;

         case 'N':
            KEY( "Name", social->name, fread_string_nohash( fp ) );
            break;

         case 'O':
            KEY( "Other", social->other, fread_number( fp ) );
            KEY( "OthersNoArg", social->others_no_arg, fread_string_nohash( fp ) );
            KEY( "OthersFound", social->others_found, fread_string_nohash( fp ) );
            KEY( "OthersAuto", social->others_auto, fread_string_nohash( fp ) );
            break;

         case 'P':
            KEY( "Pos", social->position, fread_number( fp ) );
            break;

         case 'S':
            KEY( "Self", social->self, fread_number( fp ) );
            KEY( "Stage", social->stage, fread_number( fp ) );
            break;

         case 'V':
            KEY( "VictFound", social->vict_found, fread_string_nohash( fp ) );
            break;
      }

      if( !fMatch )
      {
         sprintf( buf, "Fread_social: no match: %s", word );
         bug( buf, 0 );
      }
   }
}

void load_xsocials(  )
{
   FILE *fp;

   if( ( fp = fopen( XSOCIAL_FILE, "r" ) ) != NULL )
   {
      top_sn = 0;
      for( ;; )
      {
         char letter;
         char *word;

         letter = fread_letter( fp );
         if( letter == '*' )
         {
            fread_to_eol( fp );
            continue;
         }

         if( letter != '#' )
         {
            bug( "Load_xsocials: # not found.", 0 );
            break;
         }

         word = fread_word( fp );
         if( !str_cmp( word, "XSOCIAL" ) )
         {
            fread_xsocial( fp );
            continue;
         }
         else if( !str_cmp( word, "END" ) )
            break;
         else
         {
            bug( "Load_xsocials: bad section.", 0 );
            continue;
         }
      }
      fclose( fp );
   }
   else
   {
      bug( "Cannot open xsocials.dat", 0 );
      exit( 0 );
   }
}

void fread_social( FILE * fp )
{
   char buf[MAX_STRING_LENGTH];
   char *word;
   bool fMatch;
   SOCIALTYPE *social;

   CREATE( social, SOCIALTYPE, 1 );

   for( ;; )
   {
      word = feof( fp ) ? "End" : fread_word( fp );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
         case '*':
            fMatch = TRUE;
            fread_to_eol( fp );
            break;

         case 'C':
            KEY( "CharNoArg", social->char_no_arg, fread_string_nohash( fp ) );
            KEY( "CharFound", social->char_found, fread_string_nohash( fp ) );
            KEY( "CharAuto", social->char_auto, fread_string_nohash( fp ) );
            break;

         case 'E':
            if( !str_cmp( word, "End" ) )
            {
               if( !social->name )
               {
                  bug( "Fread_social: Name not found", 0 );
                  free_social( social );
                  return;
               }
               if( !social->char_no_arg )
               {
                  bug( "Fread_social: CharNoArg not found", 0 );
                  free_social( social );
                  return;
               }
               add_social( social );
               return;
            }
            break;

         case 'N':
            KEY( "Name", social->name, fread_string_nohash( fp ) );
            break;

         case 'O':
            KEY( "OthersNoArg", social->others_no_arg, fread_string_nohash( fp ) );
            KEY( "OthersFound", social->others_found, fread_string_nohash( fp ) );
            KEY( "OthersAuto", social->others_auto, fread_string_nohash( fp ) );
            break;

         case 'V':
            KEY( "VictFound", social->vict_found, fread_string_nohash( fp ) );
            break;
      }

      if( !fMatch )
      {
         sprintf( buf, "Fread_social: no match: %s", word );
         bug( buf, 0 );
      }
   }
}

void load_socials(  )
{
   FILE *fp;

   if( ( fp = fopen( SOCIAL_FILE, "r" ) ) != NULL )
   {
      top_sn = 0;
      for( ;; )
      {
         char letter;
         char *word;

         letter = fread_letter( fp );
         if( letter == '*' )
         {
            fread_to_eol( fp );
            continue;
         }

         if( letter != '#' )
         {
            bug( "Load_socials: # not found.", 0 );
            break;
         }

         word = fread_word( fp );
         if( !str_cmp( word, "SOCIAL" ) )
         {
            fread_social( fp );
            continue;
         }
         else if( !str_cmp( word, "END" ) )
            break;
         else
         {
            bug( "Load_socials: bad section.", 0 );
            continue;
         }
      }
      fclose( fp );
   }
   else
   {
      bug( "Cannot open socials.dat", 0 );
      exit( 0 );
   }
}

void fread_command( FILE * fp )
{
   char buf[MAX_STRING_LENGTH];
   char *word;
   bool fMatch;
   CMDTYPE *command;

   CREATE( command, CMDTYPE, 1 );
   command->lag_count = 0;
   command->flags = 0;
   command->cshow = 1;

   for( ;; )
   {
      word = feof( fp ) ? "End" : fread_word( fp );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
         case '*':
            fMatch = TRUE;
            fread_to_eol( fp );
            break;

         case 'C':
            KEY( "Code", command->do_fun, skill_function( fread_word( fp ) ) );
            KEY( "CShow", command->cshow, fread_number( fp ) );
            break;

         case 'E':
            if( !str_cmp( word, "End" ) )
            {
               if( !command->name )
               {
                  bug( "Fread_command: Name not found", 0 );
                  free_command( command );
                  return;
               }
               if( !command->do_fun )
               {
                  bug( "Fread_command: Function not found", 0 );
                  free_command( command );
                  return;
               }
               add_command( command );
               return;
            }
            break;

         case 'F':
            KEY( "Flags", command->flags, fread_number( fp ) );
            break;

         case 'L':
            KEY( "Level", command->level, fread_number( fp ) );
            KEY( "Log", command->log, fread_number( fp ) );
            break;

         case 'N':
            KEY( "Name", command->name, fread_string_nohash( fp ) );
            break;

         case 'P':
            if( !str_cmp( word, "Position" ) )
            {
               fMatch = TRUE;
               command->position = fread_number( fp );
               if( command->position < 100 )
               {
                  switch ( command->position )
                  {
                     default:
                     case 0:
                     case 1:
                     case 2:
                     case 3:
                     case 4:
                        break;
                     case 5:
                        command->position = 6;
                        break;
                     case 6:
                        command->position = 8;
                        break;
                     case 7:
                        command->position = 9;
                        break;
                     case 8:
                        command->position = 12;
                        break;
                     case 9:
                        command->position = 13;
                        break;
                     case 10:
                        command->position = 14;
                        break;
                     case 11:
                        command->position = 15;
                        break;
                  }
               }
               else
                  command->position -= 100;
               break;
            }
            break;
      }

      if( !fMatch )
      {
         sprintf( buf, "Fread_command: no match: %s", word );
         bug( buf, 0 );
      }
   }
}

void load_commands(  )
{
   FILE *fp;

   if( ( fp = fopen( COMMAND_FILE, "r" ) ) != NULL )
   {
      top_sn = 0;
      for( ;; )
      {
         char letter;
         char *word;

         letter = fread_letter( fp );
         if( letter == '*' )
         {
            fread_to_eol( fp );
            continue;
         }

         if( letter != '#' )
         {
            bug( "Load_commands: # not found.", 0 );
            break;
         }

         word = fread_word( fp );
         if( !str_cmp( word, "COMMAND" ) )
         {
            fread_command( fp );
            continue;
         }
         else if( !str_cmp( word, "END" ) )
            break;
         else
         {
            bug( "Load_commands: bad section.", 0 );
            continue;
         }
      }
      fclose( fp );
   }
   else
   {
      bug( "Cannot open commands.dat", 0 );
      exit( 0 );
   }

}

void save_classes(  )
{
   int x;

   for( x = 0; x < MAX_CLASS; x++ )
      write_class_file( x );
}

void fread_cnv( FILE * fp, LCNV_DATA ** first_cnv, LCNV_DATA ** last_cnv )
{
   LCNV_DATA *cnv;
   char letter;

   for( ;; )
   {
      letter = fread_letter( fp );
      if( letter == '~' || letter == EOF )
         break;
      ungetc( letter, fp );
      CREATE( cnv, LCNV_DATA, 1 );

      cnv->old = str_dup( fread_word( fp ) );
      cnv->olen = strlen( cnv->old );
      cnv->new = str_dup( fread_word( fp ) );
      cnv->nlen = strlen( cnv->new );
      fread_to_eol( fp );
      LINK( cnv, *first_cnv, *last_cnv, next, prev );
   }
}

void load_tongues(  )
{
   FILE *fp;
   LANG_DATA *lng;
   char *word;
   char letter;

   if( !( fp = fopen( TONGUE_FILE, "r" ) ) )
   {
      perror( "Load_tongues" );
      return;
   }
   for( ;; )
   {
      letter = fread_letter( fp );
      if( letter == EOF )
         return;
      else if( letter == '*' )
      {
         fread_to_eol( fp );
         continue;
      }
      else if( letter != '#' )
      {
         bug( "Letter '%c' not #.", letter );
         exit( 0 );
      }
      word = fread_word( fp );
      if( !str_cmp( word, "end" ) )
         return;
      fread_to_eol( fp );
      CREATE( lng, LANG_DATA, 1 );
      lng->name = STRALLOC( word );
      fread_cnv( fp, &lng->first_precnv, &lng->last_precnv );
      lng->alphabet = fread_string( fp );
      fread_cnv( fp, &lng->first_cnv, &lng->last_cnv );
      fread_to_eol( fp );
      LINK( lng, first_lang, last_lang, next, prev );
   }
   return;
}

void fwrite_langs( void )
{
   FILE *fp;
   LANG_DATA *lng;
   LCNV_DATA *cnv;

   if( !( fp = fopen( TONGUE_FILE, "w" ) ) )
   {
      perror( "fwrite_langs" );
      return;
   }
   for( lng = first_lang; lng; lng = lng->next )
   {
      fprintf( fp, "#%s\n", lng->name );
      for( cnv = lng->first_precnv; cnv; cnv = cnv->next )
         fprintf( fp, "'%s' '%s'\n", cnv->old, cnv->new );
      fprintf( fp, "~\n%s~\n", lng->alphabet );
      for( cnv = lng->first_cnv; cnv; cnv = cnv->next )
         fprintf( fp, "'%s' '%s'\n", cnv->old, cnv->new );
      fprintf( fp, "\n" );
   }
   fprintf( fp, "#end\n\n" );
   fclose( fp );
   return;
}
