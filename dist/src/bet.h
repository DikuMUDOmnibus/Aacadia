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
/*					Auction Handling Header	        */
/************************************************************************/

#include <ctype.h>

int advatoi( char *s )
{
   int tmpnumber = 0;
   int number = 0;

   for( ;; )
   {
      while( isdigit( s[0] ) )
      {
         tmpnumber = tmpnumber + atoi( s );
         s++;
      }
      switch ( UPPER( s[0] ) )
      {
         case 'S':
            tmpnumber *= 100;
            ++s;
            break;
         case 'G':
            tmpnumber *= 10000;
            ++s;
            break;
         case '\0':
            number += tmpnumber;
            return number;
         default:
            return 0;
      }
      number += tmpnumber;
      tmpnumber = 0;
   }
}

int parsebet( const int currentbet, char *s )
{
   if( s[0] != '\0' )
   {
      if( isdigit( s[0] ) )
         return ( advatoi( s ) );
      if( s[0] == '+' )
      {
         if( s[1] == '\0' )
            return ( currentbet * 125 ) / 100;
         return ( currentbet * ( 100 + atoi( s + 1 ) ) ) / 100;
      }
      if( s[0] == '*' || s[0] == 'x' )
      {
         if( s[1] == '\0' )
            return ( currentbet * 2 );
         return ( currentbet * atoi( s + 1 ) );
      }
   }
   return 0;
}
