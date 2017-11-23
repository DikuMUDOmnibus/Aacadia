/************************************************************************/
/*        Acadia code team (C) 2002-2005 Kara Enterprises                   */
/************************************************************************/
/*				  Database Manipulation module		                    */
/************************************************************************/
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "acadia.h"
void do_ilog( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   char strins[MAX_STRING_LENGTH];
   MYSQL_RES *result;
   MYSQL_ROW row;
   MYSQL *con = &mysql;
   int state;
   int tnum = 0, num = 0;

   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg2 );
   if( arg[0] == '\0' )
   {
      send_to_char( "Usage: 'ilog show (all/[who])' or 'ilog delete <ID>'\n\r", ch );
      return;
   }
   if( !str_cmp( arg, "show" ) )
   {
      if( arg2[0] == '\0' )
      {
         state = mysql_query( con, "SELECT `index`,`name`,`ctime`, `log` FROM `tbl_ilog`" );
         if( state != 0 )
         {
            perror( mysql_error( con ) );
            return;
         }
         result = mysql_store_result( con );
         tnum = mysql_num_rows( result );
         if( tnum > 10 )
         {
            num = tnum - 10;
         }
         else
         {
            num = 0;
         }
         sprintf( buf, "SELECT `index`,`name`,`ctime`, `log` FROM `tbl_ilog` LIMIT %d, 10", num );
         mysql_free_result( result );

         state = mysql_query( con, buf );
         if( state != 0 )
         {
            perror( mysql_error( con ) );
            return;
         }
         result = mysql_store_result( con );

         ch_printf( ch, "&YNumber of People in the database: &W%d\n\r"
                    "&GID   Name         Time                          On or Off?\n\r"
                    "------------------------------------------------------------\n\r", tnum );
         while( ( row = mysql_fetch_row( result ) ) != NULL )
         {
            ch_printf( ch, "&g%-3s  &P%-12s  &C%23s  &R%s&D\n\r", row[0], row[1], row[2], row[3] );
         }
         mysql_free_result( result );
         return;
      }
      else if( !str_cmp( arg2, "all" ) )
      {
         state = mysql_query( con, "SELECT `index`,`name`,`ctime`, `log` FROM `tbl_ilog`" );
         if( state != 0 )
         {
            perror( mysql_error( con ) );
            return;
         }
         result = mysql_store_result( con );
         ch_printf( ch, "&YNumber of People in the database: &W%d\n\r"
                    "&GID   Name         Time                          On or Off?\n\r"
                    "------------------------------------------------------------\n\r", mysql_num_rows( result ) );
         while( ( row = mysql_fetch_row( result ) ) != NULL )
         {
            ch_printf( ch, "&g%-3s  &P%-12s  &C%23s  &R%s&D\n\r", row[0], row[1], row[2], row[3] );
         }
         mysql_free_result( result );
         return;
      }
      else
      {
         sprintf( strins, "SELECT `index`,`name`,`ctime`, `log` FROM `tbl_ilog` WHERE 1 AND `name` LIKE '%s'", arg2 );
         state = mysql_query( con, strins );
         if( state != 0 )
         {
            perror( mysql_error( con ) );
            return;
         }
         result = mysql_store_result( con );
         ch_printf( ch, "&YNumber of People in the database: &W%d\n\r"
                    "&GID   Name         Time                          On or Off?\n\r"
                    "------------------------------------------------------------\n\r", mysql_num_rows( result ) );
         while( ( row = mysql_fetch_row( result ) ) != NULL )
         {
            ch_printf( ch, "&g%-3s  &P%-12s  &C%23s  &R%s&D\n\r", row[0], row[1], row[2], row[3] );
         }
         mysql_free_result( result );
         return;
      }
   }
   else if( !str_cmp( arg, "delete" ) )
   {
      if( arg2[0] == '\0' || !is_number( arg2 ) )
      {
         send_to_char( "Usage: 'ilog show (all/[who])' or 'ilog delete <ID>'\n\r", ch );
         return;
      }
      sprintf( strins, "DELETE FROM `tbl_ilog` WHERE `index` = '%d'", atoi( arg2 ) );
      state = mysql_query( con, strins );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      send_to_char( "Deleted. Type 'ilog show' to view.\n\r", ch );
      return;
   }
   else
   {
      send_to_char( "Usage: 'ilog show (all/[who])' or 'ilog delete <ID>'\n\r", ch );
      return;
   }
}

void immlog_onoff( CHAR_DATA * ch, char *onoff )
{
   char strins[MAX_STRING_LENGTH];
   MYSQL *con = &mysql;
   int state;

   sprintf( strins, "INSERT INTO `tbl_ilog` (`name`, `ctime`, `log`) VALUES ('%s', '%s', '%s');",
            ch->name, ( char * )friendly_ctime( &current_time ), capitalize( onoff ) );
   state = mysql_query( con, strins );
   if( state != 0 )
   {
      perror( mysql_error( con ) );
      return;
   }
   return;
}

void newchar( CHAR_DATA * ch )
{
   char strins[MAX_STRING_LENGTH];
   MYSQL *con = &mysql;
   int state;

   sprintf( strins, "INSERT INTO `tbl_nch` (`name`, `ip`, `date`) VALUES ('%s', '%s', '%s');",
            ch->name, ch->desc->host, ( char * )friendly_ctime( &current_time ) );
   state = mysql_query( con, strins );
   if( state != 0 )
   {
      perror( mysql_error( con ) );
      return;
   }
   return;
}

void do_nch( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   char strins[MAX_STRING_LENGTH];
   MYSQL_RES *result;
   MYSQL_ROW row;
   MYSQL *con = &mysql;
   int state;
   int tnum = 0, num = 0;

   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg2 );
   if( arg[0] == '\0' )
   {
      send_to_char( "Usage: 'nch show (all/[who])' or 'nch show2 <ip>' or 'nch delete <ID>'\n\r", ch );
      return;
   }
   if( !str_cmp( arg, "show" ) )
   {
      if( arg2[0] == '\0' )
      {
         state = mysql_query( con, "SELECT `index`,`name`, `ip`, `date` FROM `tbl_nch`" );
         if( state != 0 )
         {
            perror( mysql_error( con ) );
            return;
         }
         result = mysql_store_result( con );
         tnum = mysql_num_rows( result );
         if( tnum > 10 )
         {
            num = tnum - 10;
         }
         else
         {
            num = 0;
         }
         sprintf( buf, "SELECT `index`,`name`,`ip`, `date` FROM `tbl_nch` LIMIT %d, 10", num );
         mysql_free_result( result );
         state = mysql_query( con, buf );
         if( state != 0 )
         {
            perror( mysql_error( con ) );
            return;
         }
         result = mysql_store_result( con );

         ch_printf( ch, "&YNumber of People in the database: &W%d\n\r"
                    "&GID   Name            IP               Date\n\r"
                    "----------------------------------------------\n\r", tnum );
         while( ( row = mysql_fetch_row( result ) ) != NULL )
         {
            ch_printf( ch, "&g%-3s  &P%-15s  &C%-15s &R%s&D\n\r", row[0], row[1], row[2], row[3] );
         }
         mysql_free_result( result );
         return;
      }
      else if( !str_cmp( arg2, "all" ) )
      {
         state = mysql_query( con, "SELECT `index`,`name`,`ip`, `date` FROM `tbl_nch`" );
         if( state != 0 )
         {
            perror( mysql_error( con ) );
            return;
         }
         result = mysql_store_result( con );
         ch_printf( ch, "&YNumber of People in the database: &W%d\n\r"
                    "&GID   Name            IP               Date\n\r"
                    "----------------------------------------------\n\r", mysql_num_rows( result ) );
         while( ( row = mysql_fetch_row( result ) ) != NULL )
         {
            ch_printf( ch, "&g%-3s  &P%-15s  &C%-15s &R%s&D\n\r", row[0], row[1], row[2], row[3] );
         }
         mysql_free_result( result );
         return;
      }
      else
      {
         sprintf( strins, "SELECT `index`,`name`,`ip`, `date` FROM `tbl_nch` WHERE 1 AND `name` LIKE '%s'", arg2 );
         state = mysql_query( con, strins );
         if( state != 0 )
         {
            perror( mysql_error( con ) );
            return;
         }
         result = mysql_store_result( con );
         ch_printf( ch, "&YNumber of People in the database: &W%d\n\r"
                    "&GID   Name            IP               Date\n\r"
                    "----------------------------------------------\n\r", mysql_num_rows( result ) );
         while( ( row = mysql_fetch_row( result ) ) != NULL )
         {
            ch_printf( ch, "&g%-3s  &P%-15s  &C%-15s &R%s&D\n\r", row[0], row[1], row[2], row[3] );
         }
         mysql_free_result( result );
         return;
      }
   }
   else if( !str_cmp( arg, "show2" ) )
   {
      sprintf( strins, "SELECT `index`,`name`,`ip`, `date` FROM `tbl_nch` WHERE 1 AND `ip` LIKE '%s'", arg2 );
      state = mysql_query( con, strins );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      result = mysql_store_result( con );
      ch_printf( ch, "&YNumber of People in the database: &W%d\n\r"
                 "&GID   Name            IP               Date\n\r"
                 "----------------------------------------------\n\r", mysql_num_rows( result ) );
      while( ( row = mysql_fetch_row( result ) ) != NULL )
      {
         ch_printf( ch, "&g%-3s  &P%-15s  &C%-15s &R%s&D\n\r", row[0], row[1], row[2], row[3] );
      }
      mysql_free_result( result );
      return;
   }
   else if( !str_cmp( arg, "delete" ) )
   {
      if( arg2[0] == '\0' || !is_number( arg2 ) )
      {
         send_to_char( "Usage: 'nch show (all/[who])' or 'nch show2 <ip>' or 'nch delete <ID>'\n\r", ch );
         return;
      }
      sprintf( strins, "DELETE FROM `tbl_nch` WHERE `index` = '%d'", atoi( arg2 ) );
      state = mysql_query( con, strins );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      send_to_char( "Deleted. Type 'nch show' to view.\n\r", ch );
      return;
   }
   else
   {
      send_to_char( "Usage: 'nch show (all/[who])' or 'nch show2 <ip>' or 'nch delete <ID>'\n\r", ch );
      return;
   }
}



void do_rap( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   char editstr[MAX_STRING_LENGTH];
   char strins[MAX_STRING_LENGTH];
   CHAR_DATA *vch;
   MYSQL_RES *result;
   MYSQL_ROW row;
   MYSQL *con = &mysql;
   int state;

   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg2 );
   if( arg[0] == '\0' )
   {
      send_to_char
         ( "Usage: 'rap show [who]' or 'rap show2 <IP>' or 'rap add <who> [comment]'\n\r"
           "or 'rap edit <name> <column> <what>' or 'rap delete <ID>'\n\r"
           "PLEASE NOTE: You can edit the IP or comment fields only.\n\r", ch );
      return;
   }
   if( !str_cmp( arg, "show" ) )
   {
      if( arg2[0] == '\0' )
      {
         state = mysql_query( con, "SELECT `index`,`name`,`IP`, `comment` FROM `tbl_rap`" );
         if( state != 0 )
         {
            perror( mysql_error( con ) );
            return;
         }
         result = mysql_store_result( con );
         ch_printf( ch,
                    "&YNumber of People in the database: &W%d\n\r&GID   Name         IP            Comment\n\r"
                    "--------------------------------------------------\n\r", mysql_num_rows( result ) );
         while( ( row = mysql_fetch_row( result ) ) != NULL )
         {
            ch_printf( ch, "&g%-3s  &P%-12s  &C%s  &R%s&D\n\r", row[0], row[1], row[2], row[3] );
         }
         mysql_free_result( result );
         return;
      }
      else
      {
         sprintf( strins, "SELECT `index`,`name`,`IP`, `comment` FROM `tbl_rap` WHERE 1 AND `name` LIKE '%s'", arg2 );
         state = mysql_query( con, strins );
         if( state != 0 )
         {
            perror( mysql_error( con ) );
            return;
         }
         result = mysql_store_result( con );
         ch_printf( ch,
                    "&YNumber of People in the database: &W%d\n\r&GID   Name         IP            Comment\n\r"
                    "--------------------------------------------------\n\r", mysql_num_rows( result ) );
         while( ( row = mysql_fetch_row( result ) ) != NULL )
         {
            ch_printf( ch, "&g%-3s  &P%-12s  &C%s  &R%s&D\n\r", row[0], row[1], row[2], row[3] );
         }
         mysql_free_result( result );
         return;
      }
   }
   else if( !str_cmp( arg, "show2" ) )
   {
      sprintf( strins, "SELECT `index`,`name`,`IP`, `comment` FROM `tbl_rap` WHERE 1 AND `IP` LIKE '%s'", arg2 );
      state = mysql_query( con, strins );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      result = mysql_store_result( con );
      ch_printf( ch,
                 "&YNumber of People in the database: &W%d\n\r&GID   Name         IP            Comment\n\r"
                 "--------------------------------------------------\n\r", mysql_num_rows( result ) );
      while( ( row = mysql_fetch_row( result ) ) != NULL )
      {
         ch_printf( ch, "&g%-3s  &P%-12s  &C%s  &R%s&D\n\r", row[0], row[1], row[2], row[3] );
      }
      mysql_free_result( result );
      return;
   }
   else if( !str_cmp( arg, "edit" ) )
   {
      argument = one_argument( argument, arg3 );
      if( arg2[0] == '\0' || arg3[0] == '\0' )
      {
         send_to_char
            ( "Usage: 'rap show [who]' or 'rap show2 <IP>' or 'rap add <who> [comment]'\n\r"
              "or 'rap edit <name> <column> <what>' or 'rap delete <ID>'\n\r"
              "PLEASE NOTE: You can edit the IP or comment fields only.\n\r", ch );
         return;
      }

      strcat( editstr, argument );

      sprintf( strins, "UPDATE tbl_rap SET %s='%s' WHERE 1 AND `name` LIKE '%s'", arg3, editstr, arg2 );
      state = mysql_query( con, strins );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      send_to_char( "Updated. Type 'rap show' to view.\n\r", ch );
      return;
   }
   else if( !str_cmp( arg, "add" ) )
   {
      if( arg2[0] == '\0' )
      {
         send_to_char
            ( "Usage: 'rap show [who]' or 'rap show2 <IP>' or 'rap add <who> [comment]'\n\r"
              "or 'rap edit <name> <column> <what>' or 'rap delete <ID>'\n\r"
              "PLEASE NOTE: You can edit the IP or comment fields only.\n\r", ch );
         return;
      }

      if( ( vch = get_char_world( ch, arg2 ) ) == NULL )
      {
         send_to_char( "That player isn't online now.\n\r", ch );
         return;
      }
      else
      {
         strcpy( arg3, argument );

         sprintf( strins, "INSERT INTO `tbl_rap` (`name`, `IP`, `comment`) VALUES ('%s', '%s', '%s');",
                  vch->name, vch->desc->host, arg3 );
         state = mysql_query( con, strins );
         if( state != 0 )
         {
            perror( mysql_error( con ) );
            return;
         }
         send_to_char( "Recorded. Type 'rap show' to view.\n\r", ch );
         return;
      }
   }
   else if( !str_cmp( arg, "delete" ) )
   {
      if( arg2[0] == '\0' || !is_number( arg2 ) )
      {
         send_to_char
            ( "Usage: 'rap show [who]' or 'rap show2 <IP>' or 'rap add <who> [comment]'\n\r"
              "or 'rap edit <name> <column> <what>' or 'rap delete <ID>'\n\r"
              "PLEASE NOTE: You can edit the IP or comment fields only.\n\r", ch );
         return;
      }
      sprintf( strins, "DELETE FROM `tbl_rap` WHERE `index` = '%d'", atoi( arg2 ) );
      state = mysql_query( con, strins );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      send_to_char( "Deleted. Type 'rap show' to view.\n\r", ch );
      return;
   }
   else
   {
      send_to_char
         ( "Usage: 'rap show [who]' or 'rap show2 <IP>' or 'rap add <who> [comment]'\n\r"
           "or 'rap edit <name> <column> <what>' or 'rap delete <ID>'\n\r"
           "PLEASE NOTE: You can edit the IP or comment fields only.\n\r", ch );
      return;
   }
}

void do_chareg( CHAR_DATA * ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   char editstr[MAX_STRING_LENGTH];
   char strins[MAX_STRING_LENGTH];
   CHAR_DATA *vch;
   MYSQL_RES *result;
   MYSQL_ROW row;
   MYSQL *con = &mysql;
   int state;

   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg2 );
   if( arg[0] == '\0' )
   {
      send_to_char
         ( "Usage: 'chareg show [IP]' or 'chareg add <IP> <alt names>' or 'chareg addalt <IP> <what to add>' "
           "or 'chareg delete <index number>'\n\r", ch );
      send_to_char( "Also: 'chareg check <person>' to check if a character is registered.\n\r", ch );
      return;
   }
   if( !str_cmp( arg, "check" ) )
   {
      bool found = FALSE;
      if( arg2[0] == '\0' )
      {
         send_to_char( "You must say someone to check!\n\r", ch );
         return;
      }
      state = mysql_query( con, "SELECT `name` FROM `tbl_register`" );

      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      result = mysql_store_result( con );
      while( ( row = mysql_fetch_row( result ) ) != NULL )
      {
         if( is_name( capitalize( arg2 ), row[0] ) )
            found = TRUE;
      }
      ch_printf( ch, "%s is %sregistered.\n\r", capitalize( arg2 ), found == TRUE ? "" : "not " );
//    ch_printf( ch, "%s is %sregistered.\n\r", capitalize(arg2), !is_registered( arg2 ) ? "not" : "");
      mysql_free_result( result );
      return;
   }
   if( !str_cmp( arg, "show" ) || !str_cmp( arg, "list" ) )
   {
      if( arg2[0] == '\0' )
      {
         state = mysql_query( con, "SELECT `index`,`IP`,`name` FROM `tbl_register`" );
         if( state != 0 )
         {
            perror( mysql_error( con ) );
            return;
         }
         result = mysql_store_result( con );
         ch_printf( ch,
                    "&YNumber of People in the database: &W%d\n\r&GID   IP               Names\n\r"
                    "--------------------------------------------------\n\r", mysql_num_rows( result ) );
         while( ( row = mysql_fetch_row( result ) ) != NULL )
         {
            ch_printf( ch, "&g%-3s  &C%-15s  &R%s&D\n\r", row[0], row[1], row[2] );
         }
         mysql_free_result( result );
         return;
      }
      else
      {
         sprintf( strins, "SELECT `index`,`IP`, `name` FROM `tbl_register` WHERE 1 AND `IP` LIKE '%s'", arg2 );
         state = mysql_query( con, strins );
         if( state != 0 )
         {
            perror( mysql_error( con ) );
            return;
         }
         result = mysql_store_result( con );
         ch_printf( ch,
                    "&YNumber of People in the database: &W%d\n\r&GID   IP               Names\n\r"
                    "--------------------------------------------------\n\r", mysql_num_rows( result ) );
         while( ( row = mysql_fetch_row( result ) ) != NULL )
         {
            ch_printf( ch, "&g%-3s  &C%-15s  &R%s&D\n\r", row[0], row[1], row[2] );
         }
         mysql_free_result( result );
         return;
      }
   }
   else if( !str_cmp( arg, "add" ) )
   {
      if( arg2[0] == '\0' )
      {
         send_to_char
            ( "Usage: 'chareg show [IP]' or 'chareg add <IP> <alt names>' or 'chareg addalt <IP> <what to add>' "
              "or 'chareg delete <index number>'\n\r", ch );
         return;
      }

      vch = get_char_world( ch, arg2 );

      strcpy( arg3, argument );

      sprintf( strins, "INSERT INTO `tbl_register` (`IP`, `name`) VALUES ('%s', '%s');", arg2, arg3 );
      state = mysql_query( con, strins );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      send_to_char( "Recorded. Type 'chareg show' to view.\n\r", ch );
      return;
   }
   else if( !str_cmp( arg, "addalt" ) )
   {
      if( arg2[0] == '\0' )
      {
         send_to_char
            ( "Usage: 'chareg show [IP]' or 'chareg add <IP> <alt names>' or 'chareg addalt <IP> <what to add>' "
              "or 'chareg delete <index number>'\n\r", ch );
         return;
      }

      sprintf( strins, "SELECT `index`,`IP`, `name` FROM `tbl_register` WHERE 1 AND `IP` LIKE '%s'", arg2 );
      state = mysql_query( con, strins );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      result = mysql_store_result( con );
      while( ( row = mysql_fetch_row( result ) ) != NULL )
      {
         sprintf( editstr, "%s ", row[2] );
      }
      mysql_free_result( result );
      strcat( editstr, argument );

      sprintf( strins, "UPDATE tbl_register SET name='%s' WHERE 1 AND `IP` LIKE '%s'", editstr, arg2 );
      state = mysql_query( con, strins );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      send_to_char( "Updated. Type 'chareg show' to view.\n\r", ch );
      return;
   }
   else if( !str_cmp( arg, "delete" ) )
   {
      if( arg2[0] == '\0' || !is_number( arg2 ) )
      {
         send_to_char
            ( "Usage: 'chareg show [IP]' or 'chareg add <IP> <alt names>' or 'chareg addalt <IP> <what to add>' "
              "or 'chareg delete <index number>'\n\r", ch );
         return;
      }
      sprintf( strins, "DELETE FROM `tbl_register` WHERE `index` = '%d'", atoi( arg2 ) );
      state = mysql_query( con, strins );
      if( state != 0 )
      {
         perror( mysql_error( con ) );
         return;
      }
      send_to_char( "Deleted. Type 'chareg show' to view.\n\r", ch );
      return;
   }
   else
   {
      send_to_char
         ( "Usage: 'chareg show [IP]' or 'chareg add <IP> <alt names>' or 'chareg addalt <IP> <what to add>' "
           "or 'chareg delete <index number>'\n\r", ch );
      return;
   }
}


bool is_registered( char *ch )
{
   MYSQL_RES *result = NULL;
   MYSQL_ROW row;
   MYSQL *con = &mysql;
   int state;
   bool found = FALSE;

   state = mysql_query( con, "SELECT `name` FROM `tbl_register`" );

   if( state != 0 )
   {
      perror( mysql_error( con ) );
      return FALSE;
   }

   while( ( row = mysql_fetch_row( result ) ) != NULL )
   {
      if( is_name( capitalize( ch ), row[0] ) )
         found = TRUE;
   }
   mysql_free_result( result );
   return found;
}
