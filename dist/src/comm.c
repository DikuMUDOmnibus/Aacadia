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
/*				   System Communication module		                    */
/************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include "acadia.h"
#ifdef USE_IMC
#include "imc.h"
#include "icec.h"
#include "color.h"
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>
#include <netdb.h>
#define closesocket close

#ifdef sun
int gethostname( char *name, int namelen );
#endif

const char echo_off_str[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const char echo_on_str[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const char go_ahead_str[] = { IAC, GA, '\0' };

#define  IS                 '\x00'
#define  TERMINAL_TYPE      '\x18'
#define  SEND	            '\x01'
#define  SE                 '\xF0'

const unsigned char will_termtype_str[] = { IAC, WILL, TERMINAL_TYPE, '\0' };
const unsigned char wont_termtype_str[] = { IAC, WONT, TERMINAL_TYPE, '\0' };
const unsigned char do_termtype_str[] = { IAC, DO, TERMINAL_TYPE, '\0' };
const unsigned char dont_termtype_str[] = { IAC, DONT, TERMINAL_TYPE, '\0' };
const unsigned char term_call_back_str[] = { IAC, SB, TERMINAL_TYPE, IS };
const unsigned char req_termtype_str[] = { IAC, SB, TERMINAL_TYPE, SEND, IAC, SE, '\0' };

void auth_maxdesc args( ( int *md, fd_set * ins, fd_set * outs, fd_set * excs ) );
void auth_check args( ( fd_set * ins, fd_set * outs, fd_set * excs ) );
void set_auth args( ( DESCRIPTOR_DATA * d ) );
void kill_auth args( ( DESCRIPTOR_DATA * d ) );

void save_sysdata args( ( SYSTEM_DATA sys ) );

DESCRIPTOR_DATA *first_descriptor;
DESCRIPTOR_DATA *last_descriptor;
DESCRIPTOR_DATA *d_next;
int num_descriptors;
FILE *fpReserve;
bool mud_down;
bool service_shut_down;
bool wizlock;
time_t boot_time;
HOUR_MIN_SEC set_boot_time_struct;
HOUR_MIN_SEC *set_boot_time;
struct tm *new_boot_time;
struct tm new_boot_struct;
char str_boot_time[MAX_INPUT_LENGTH];
char lastplayercmd[MAX_INPUT_LENGTH * 2];
time_t current_time;
int control;
int control2;
//int        conclient;    
//int        conjava;   
int newdesc;
fd_set in_set;
fd_set out_set;
fd_set exc_set;
int maxdesc;
int numlock = 0;
char *alarm_section = "(unknown)";

void game_loop args( (  ) );
int init_socket args( ( int port ) );
void new_descriptor args( ( int new_desc ) );
bool read_from_descriptor args( ( DESCRIPTOR_DATA * d ) );
bool write_to_descriptor args( ( int desc, char *txt, int length ) );


bool check_parse_name args( ( char *name, bool newchar ) );
bool check_reconnect args( ( DESCRIPTOR_DATA * d, char *name, bool fConn ) );
bool check_playing args( ( DESCRIPTOR_DATA * d, char *name, bool kick ) );
int main args( ( int argc, char **argv ) );
void nanny args( ( DESCRIPTOR_DATA * d, char *argument ) );
bool flush_buffer args( ( DESCRIPTOR_DATA * d, bool fPrompt ) );
void read_from_buffer args( ( DESCRIPTOR_DATA * d ) );
void stop_idling args( ( CHAR_DATA * ch ) );
void free_desc args( ( DESCRIPTOR_DATA * d ) );
void display_prompt args( ( DESCRIPTOR_DATA * d ) );
int make_color_sequence args( ( const char *col, char *buf, DESCRIPTOR_DATA * d ) );
void set_pager_input args( ( DESCRIPTOR_DATA * d, char *argument ) );
bool pager_output args( ( DESCRIPTOR_DATA * d ) );

void mail_count args( ( CHAR_DATA * ch ) );

void imp_msg args( ( CHAR_DATA * ch ) );

void tax_player args( ( CHAR_DATA * ch ) );


int main( int argc, char **argv )
{
   struct timeval now_time;
   char buf[MAX_STRING_LENGTH];
   char hostn[128];
   bool fCopyOver = !TRUE;

#if defined(MALLOC_DEBUG)
   malloc_debug( 2 );
#endif

   DONT_UPPER = FALSE;
   num_descriptors = 0;
   first_descriptor = NULL;
   last_descriptor = NULL;
   sysdata.NO_NAME_RESOLVING = TRUE;
   sysdata.WAIT_FOR_AUTH = TRUE;

   gettimeofday( &now_time, NULL );
   current_time = ( time_t ) now_time.tv_sec;
   boot_time = time( 0 );
   strcpy( str_boot_time, ctime( &current_time ) );

   set_boot_time = &set_boot_time_struct;
   set_boot_time->manual = 0;

   new_boot_time = update_time( localtime( &current_time ) );
   new_boot_struct = *new_boot_time;
   new_boot_time = &new_boot_struct;
   new_boot_time->tm_mday += 1;
   if( new_boot_time->tm_hour > 12 )
      new_boot_time->tm_mday += 1;
   new_boot_time->tm_sec = 0;
   new_boot_time->tm_min = 0;
   new_boot_time->tm_hour = 6;

   new_boot_time = update_time( new_boot_time );
   new_boot_struct = *new_boot_time;
   new_boot_time = &new_boot_struct;
   new_boot_time_t = mktime( new_boot_time );
   reboot_check( mktime( new_boot_time ) );
   get_reboot_string(  );
   init_pfile_scan_time(  );

   if( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
   {
      perror( NULL_FILE );
      exit( 1 );
   }
   if( ( fpLOG = fopen( NULL_FILE, "r" ) ) == NULL )
   {
      perror( NULL_FILE );
      exit( 1 );
   }

   if( argc > 1 )
   {
      if( !is_number( argv[1] ) )
      {
         fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
         exit( 1 );
      }
      else if( atoi( argv[1] ) <= 1024 )
      {
         fprintf( stderr, "Port number must be above 1024.\n" );
         exit( 1 );
      }
      if( argv[2] && argv[2][0] )
      {
         fCopyOver = TRUE;
         control = atoi( argv[3] );
      }
      else
         fCopyOver = FALSE;
   }

   log_string( "Booting Database" );
   boot_db( fCopyOver );
   log_string( "Initializing socket" );
   if( !fCopyOver )
   {
      control = init_socket( PORTN );
   }

   if( gethostname( hostn, sizeof( hostn ) ) < 0 )
   {
      perror( "main: gethostname" );
      strcpy( hostn, "unresolved" );
   }
   sprintf( log_buf, "%s ready at address %s on port %d.", sysdata.mud_name, hostn, PORTN );
   log_string( log_buf );

   game_loop(  );

   sprintf( buf, "Shutting down Storm WebServer on port %d.", PORTN + 1 );
   log_string( buf );
   shutdown_web(  );

#ifdef USE_IMC
   imc_shutdown(  );
#endif

   closesocket( control );

   log_string( "Normal termination of game." );
   exit( 0 );
   return 0;
}


int init_socket( int port )
{
   char hostname[64];
   struct sockaddr_in sa;
   int x = 1;
   int fd;

   gethostname( hostname, sizeof( hostname ) );


   if( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
   {
      perror( "Init_socket: socket" );
      exit( 1 );
   }

   if( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, ( void * )&x, sizeof( x ) ) < 0 )
   {
      perror( "Init_socket: SO_REUSEADDR" );
      closesocket( fd );
      exit( 1 );
   }

#if defined(SO_DONTLINGER) && !defined(SYSV)
   {
      struct linger ld;

      ld.l_onoff = 1;
      ld.l_linger = 1000;

      if( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER, ( void * )&ld, sizeof( ld ) ) < 0 )
      {
         perror( "Init_socket: SO_DONTLINGER" );
         closesocket( fd );
         exit( 1 );
      }
   }
#endif

   memset( &sa, '\0', sizeof( sa ) );
   sa.sin_family = AF_INET;
   sa.sin_port = htons( port );

   if( bind( fd, ( struct sockaddr * )&sa, sizeof( sa ) ) == -1 )
   {
      perror( "Init_socket: bind" );
      closesocket( fd );
      exit( 1 );
   }

   if( listen( fd, 50 ) < 0 )
   {
      perror( "Init_socket: listen" );
      closesocket( fd );
      exit( 1 );
   }

   return fd;
}

void caught_alarm(  )
{
   char buf[MAX_STRING_LENGTH];

   sprintf( buf, "ALARM CLOCK!  In section %s", buf );
   bug( buf );
   strcpy( buf, "Alas, the hideous malevalent entity known only as 'Lag' rises once more!\n\r" );
   echo_to_all( AT_IMMORT, buf, ECHOTAR_ALL );
   if( newdesc )
   {
      FD_CLR( newdesc, &in_set );
      FD_CLR( newdesc, &out_set );
      FD_CLR( newdesc, &exc_set );
      log_string( "clearing newdesc" );
   }
}

bool check_bad_desc( int desc )
{
   if( FD_ISSET( desc, &exc_set ) )
   {
      FD_CLR( desc, &in_set );
      FD_CLR( desc, &out_set );
      log_string( "Bad FD caught and disposed." );
      return TRUE;
   }
   return FALSE;
}

bool chk_watch( sh_int player_level, char *player_name, char *player_site )
{
   WATCH_DATA *pw;
   if( !first_watch )
      return FALSE;

   for( pw = first_watch; pw; pw = pw->next )
   {
      if( pw->target_name )
      {
         if( !str_cmp( pw->target_name, player_name ) && player_level < pw->imm_level )
            return TRUE;
      }
      else if( pw->player_site )
      {
         if( !str_prefix( pw->player_site, player_site ) && player_level < pw->imm_level )
            return TRUE;
      }
   }
   return FALSE;
}


void accept_new( int ctrl )
{
   static struct timeval null_time;
   DESCRIPTOR_DATA *d;

#if defined(MALLOC_DEBUG)
   if( malloc_verify(  ) != 1 )
      abort(  );
#endif

   FD_ZERO( &in_set );
   FD_ZERO( &out_set );
   FD_ZERO( &exc_set );
   FD_SET( ctrl, &in_set );
   maxdesc = ctrl;
   newdesc = 0;
   for( d = first_descriptor; d; d = d->next )
   {
      maxdesc = UMAX( maxdesc, d->descriptor );
      FD_SET( d->descriptor, &in_set );
      FD_SET( d->descriptor, &out_set );
      FD_SET( d->descriptor, &exc_set );
      if( d == last_descriptor )
         break;
   }
   auth_maxdesc( &maxdesc, &in_set, &out_set, &exc_set );

#ifdef USE_IMC
   maxdesc = imc_fill_fdsets( maxdesc, &in_set, &out_set, &exc_set );
#endif

   if( select( maxdesc + 1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
   {
      perror( "accept_new: select: poll" );
      exit( 1 );
   }

   if( FD_ISSET( ctrl, &exc_set ) )
   {
      bug( "Exception raise on controlling descriptor %d", ctrl );
      FD_CLR( ctrl, &in_set );
      FD_CLR( ctrl, &out_set );
   }
   else if( FD_ISSET( ctrl, &in_set ) )
   {
      newdesc = ctrl;
      new_descriptor( newdesc );
   }
}

void game_loop(  )
{
   struct timeval last_time;
   char cmdline[MAX_INPUT_LENGTH];
   DESCRIPTOR_DATA *d;
   static int tt = 120;

   gettimeofday( &last_time, NULL );
   current_time = ( time_t ) last_time.tv_sec;

   while( !mud_down )
   {
      accept_new( control );
      handle_web(  );

      auth_check( &in_set, &out_set, &exc_set );

      for( d = first_descriptor; d; d = d_next )
      {
         if( d == d->next )
         {
            bug( "descriptor_loop: loop found & fixed" );
            d->next = NULL;
         }
         d_next = d->next;

         if( d->character && xIS_SET( d->character->act, PLR_PINGME ) )
         {
            if( --tt <= 0 )
            {
               tt = 120;
               write_to_buffer( d, do_termtype_str, 0 );
            }
         }

         if( d->character && !IS_IMMORTAL( d->character ) )
            d->idle++;
         if( FD_ISSET( d->descriptor, &exc_set ) )
         {
            FD_CLR( d->descriptor, &in_set );
            FD_CLR( d->descriptor, &out_set );
            if( d->character
                && ( d->connected == CON_PLAYING || d->connected == CON_MEETING || d->connected == CON_EDITING ) )
               save_char_obj( d->character );
            d->outtop = 0;
//    if ( !IS_IMMORTAL(d->character) )
            close_socket( d, TRUE );
            continue;
         }
         else
            if( ( !d->character && d->idle > 360 ) || ( d->connected != CON_PLAYING && d->idle > 1200 ) || d->idle > 28800 )
         {
            write_to_descriptor( d->descriptor, "\n\rIdle timeout... disconnecting.\n\r", 0 );
            d->outtop = 0;
//       if ( !IS_IMMORTAL(d->character) )
            close_socket( d, TRUE );
            continue;
         }
         else
         {
            d->fcommand = FALSE;

            if( FD_ISSET( d->descriptor, &in_set ) )
            {
               d->idle = 0;
               if( d->character )
                  d->character->timer = 0;
               if( !read_from_descriptor( d ) )
               {
                  FD_CLR( d->descriptor, &out_set );
                  if( d->character
                      && ( d->connected == CON_PLAYING || d->connected == CON_MEETING || d->connected == CON_EDITING ) )
                     save_char_obj( d->character );
                  d->outtop = 0;
//          if ( !IS_IMMORTAL(d->character) )
                  close_socket( d, FALSE );
                  continue;
               }
            }

/*		if ( d->character && IS_IMMORTAL(d->character) && d->idle == 800 )
		{
			
			xSET_BIT( d->character->act, PLR_IDLE );
			write_to_descriptor( d->descriptor, "You are now idling...\n\r", 0 );
			continue;
		}
*/
            if( d->character && d->character->wait > 0 )
            {
               --d->character->wait;
               continue;
            }

            read_from_buffer( d );
            if( d->incomm[0] != '\0' )
            {
               d->fcommand = TRUE;
               stop_idling( d->character );

               strcpy( cmdline, d->incomm );
               d->incomm[0] = '\0';

               if( d->character )
                  set_cur_char( d->character );

               if( d->pagepoint )
                  set_pager_input( d, cmdline );
               else
                  switch ( d->connected )
                  {
                     default:
                        nanny( d, cmdline );
                        break;
                     case CON_PLAYING:
                        d->character->cmd_recurse = 0;
                        substitute_alias( d, cmdline );
//          interpret( d->character, cmdline );
                        break;
                     case CON_EDITING:
                        edit_buffer( d->character, cmdline );
                        break;
                     case CON_MEETING:
                        meeting_interpret( d->character, cmdline );
                        break;
                  }
            }
         }
         if( d == last_descriptor )
            break;
      }

#ifdef USE_IMC
      imc_idle_select( &in_set, &out_set, &exc_set, current_time );
#endif

      update_handler(  );

      check_requests(  );

      for( d = first_descriptor; d; d = d_next )
      {
         d_next = d->next;

         if( ( d->fcommand || d->outtop > 0 ) && FD_ISSET( d->descriptor, &out_set ) )
         {
            if( d->pagepoint )
            {
               if( !pager_output( d ) )
               {
                  if( d->character
                      && ( d->connected == CON_PLAYING || d->connected == CON_MEETING || d->connected == CON_EDITING ) )
                     save_char_obj( d->character );
                  d->outtop = 0;
                  close_socket( d, FALSE );
               }
            }
            else if( !flush_buffer( d, TRUE ) )
            {
               if( d->character
                   && ( d->connected == CON_PLAYING || d->connected == CON_MEETING || d->connected == CON_EDITING ) )
                  save_char_obj( d->character );
               d->outtop = 0;
               close_socket( d, FALSE );
            }
         }
         if( d == last_descriptor )
            break;
      }

      {
         struct timeval now_time;
         long secDelta;
         long usecDelta;

         gettimeofday( &now_time, NULL );
         usecDelta = ( ( int )last_time.tv_usec ) - ( ( int )now_time.tv_usec ) + 1000000 / PULSE_PER_SECOND;
         secDelta = ( ( int )last_time.tv_sec ) - ( ( int )now_time.tv_sec );
         while( usecDelta < 0 )
         {
            usecDelta += 1000000;
            secDelta -= 1;
         }

         while( usecDelta >= 1000000 )
         {
            usecDelta -= 1000000;
            secDelta += 1;
         }

         if( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
         {
            struct timeval stall_time;

            stall_time.tv_usec = usecDelta;
            stall_time.tv_sec = secDelta;
            if( select( 0, NULL, NULL, NULL, &stall_time ) < 0 && errno != EINTR )
            {
               perror( "game_loop: select: stall" );
               exit( 1 );
            }
         }
      }

      gettimeofday( &last_time, NULL );
      current_time = ( time_t ) last_time.tv_sec;

   }
   if( sysdata.morph_opt )
      save_morphs(  );

   fflush( stderr );
   return;
}

void init_descriptor( DESCRIPTOR_DATA * dnew, int desc )
{
   dnew->next = NULL;
   dnew->descriptor = desc;
   dnew->connected = CON_GET_NAME;
   dnew->outsize = 2000;
   dnew->idle = 0;
   dnew->lines = 0;
   dnew->scrlen = 24;
   dnew->user = STRALLOC( "unknown" );
   dnew->newstate = 0;
   dnew->prevcolor = 0x07;
   dnew->client = STRALLOC( "(unknown)" );

   CREATE( dnew->outbuf, char, dnew->outsize );
}

void new_descriptor( int new_desc )
{
   char buf[MAX_STRING_LENGTH];
   DESCRIPTOR_DATA *dnew;
   struct sockaddr_in sock;
   struct hostent *from;
   int desc;
   int size;

   size = sizeof( sock );
   if( check_bad_desc( new_desc ) )
   {
      set_alarm( 0 );
      return;
   }
   set_alarm( 20 );
   alarm_section = "new_descriptor::accept";
   if( ( desc = accept( new_desc, ( struct sockaddr * )&sock, &size ) ) < 0 )
   {
      set_alarm( 0 );
      return;
   }
   if( check_bad_desc( new_desc ) )
   {
      set_alarm( 0 );
      return;
   }
#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

   set_alarm( 20 );
   alarm_section = "new_descriptor: after accept";

   if( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
   {
      perror( "New_descriptor: fcntl: FNDELAY" );
      set_alarm( 0 );
      return;
   }
   if( check_bad_desc( new_desc ) )
      return;

   CREATE( dnew, DESCRIPTOR_DATA, 1 );
   init_descriptor( dnew, desc );
   dnew->port = ntohs( sock.sin_port );
   strcpy( buf, inet_ntoa( sock.sin_addr ) );

   if( !str_cmp( buf, "216.194.99.14" ) || !str_cmp( buf, "204.209.44.14" ) )
      sprintf( log_buf, "Mudconnector.com active connection check: %s", buf );
   else if( !str_cmp( buf, "216.40.217.211" ) )
      sprintf( log_buf, "Kyndig.com active connection check: %s", buf );
   else
      sprintf( log_buf, "Sock.sinaddr:  %s, port %hd.", buf, dnew->port );
   log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
   dnew->host = STRALLOC( buf );
   from = gethostbyaddr( ( char * )&sock.sin_addr, sizeof( sock.sin_addr ), AF_INET );
   CREATE( dnew->outbuf, char, dnew->outsize );

   strcpy( buf, inet_ntoa( sock.sin_addr ) );
   sprintf( log_buf, "Sock.sinaddr:  %s, port %hd.", buf, dnew->port );
   log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
   if( sysdata.NO_NAME_RESOLVING )
      dnew->host = STRALLOC( buf );
   /*
    * else
    * *{
    * *  from = gethostbyaddr( (char *) &sock.sin_addr,
    * *           sizeof(sock.sin_addr), AF_INET );
    * *  dnew->host = STRALLOC( (char *)( from ? from->h_name : buf) );
    * *}
    */
   if( check_total_bans( dnew ) )
   {
      write_to_descriptor( desc, "Your site has been banned from this Mud.\n\r", 0 );
      free_desc( dnew );
      set_alarm( 0 );
      return;
   }

   if( !last_descriptor && first_descriptor )
   {
      DESCRIPTOR_DATA *d;

      bug( "New_descriptor: last_desc is NULL, but first_desc is not! ...fixing" );
      for( d = first_descriptor; d; d = d->next )
         if( !d->next )
            last_descriptor = d;
   }

   LINK( dnew, first_descriptor, last_descriptor, next, prev );
   {
      extern char *help_greeting;
      if( help_greeting[0] == '.' )
         write_to_buffer( dnew, help_greeting + 1, 0 );
      else
         write_to_buffer( dnew, help_greeting, 0 );
   }

   alarm_section = "new_descriptor: set_auth";
   set_auth( dnew );
   alarm_section = "new_descriptor: after set_auth";

   if( ++num_descriptors > sysdata.maxplayers )
      sysdata.maxplayers = num_descriptors;
   if( sysdata.maxplayers > sysdata.alltimemax )
   {
      if( sysdata.time_of_max )
         DISPOSE( sysdata.time_of_max );
      sprintf( buf, "%24.24s", ctime( &current_time ) );
      sysdata.time_of_max = str_dup( buf );
      sysdata.alltimemax = sysdata.maxplayers;
      sprintf( log_buf, "Broke all-time maximum player record: %d", sysdata.alltimemax );
      log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
      to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL, sysdata.log_level );
      save_sysdata( sysdata );
   }
   set_alarm( 0 );
   return;
}

void free_desc( DESCRIPTOR_DATA * d )
{
   kill_auth( d );
   closesocket( d->descriptor );
   STRFREE( d->host );
   DISPOSE( d->outbuf );
   STRFREE( d->user );
   if( d->pagebuf )
      DISPOSE( d->pagebuf );
   if( d->client )
      STRFREE( d->client );
   DISPOSE( d );
   return;
}

void close_socket( DESCRIPTOR_DATA * dclose, bool force )
{
   CHAR_DATA *ch;
   DESCRIPTOR_DATA *d;
   bool DoNotUnlink = FALSE;

   if( !force && dclose->outtop > 0 )
      flush_buffer( dclose, FALSE );

   if( dclose->snoop_by )
      write_to_buffer( dclose->snoop_by, "Your victim has left the game.\n\r", 0 );

   for( d = first_descriptor; d; d = d->next )
      if( d->snoop_by == dclose )
         d->snoop_by = NULL;

   if( dclose->original )
   {
      if( ( ch = dclose->character ) != NULL )
         do_return( ch, "" );
      else
      {
         bug( "Close_socket: dclose->original without character %s",
              ( dclose->original->name ? dclose->original->name : "unknown" ) );
         dclose->character = dclose->original;
         dclose->original = NULL;
      }
   }

   ch = dclose->character;

   if( !dclose->prev && dclose != first_descriptor )
   {
      DESCRIPTOR_DATA *dp, *dn;
      bug( "Close_socket: %s desc:%p != first_desc:%p and desc->prev = NULL!",
           ch ? ch->name : d->host, dclose, first_descriptor );
      dp = NULL;
      for( d = first_descriptor; d; d = dn )
      {
         dn = d->next;
         if( d == dclose )
         {
            bug( "Close_socket: %s desc:%p found, prev should be:%p, fixing.", ch ? ch->name : d->host, dclose, dp );
            dclose->prev = dp;
            break;
         }
         dp = d;
      }
      if( !dclose->prev )
      {
         bug( "Close_socket: %s desc:%p could not be found!.", ch ? ch->name : dclose->host, dclose );
         DoNotUnlink = TRUE;
      }
   }
   if( !dclose->next && dclose != last_descriptor )
   {
      DESCRIPTOR_DATA *dp, *dn;
      bug( "Close_socket: %s desc:%p != last_desc:%p and desc->next = NULL!",
           ch ? ch->name : d->host, dclose, last_descriptor );
      dn = NULL;
      for( d = last_descriptor; d; d = dp )
      {
         dp = d->prev;
         if( d == dclose )
         {
            bug( "Close_socket: %s desc:%p found, next should be:%p, fixing.", ch ? ch->name : d->host, dclose, dn );
            dclose->next = dn;
            break;
         }
         dn = d;
      }
      if( !dclose->next )
      {
         bug( "Close_socket: %s desc:%p could not be found!.", ch ? ch->name : dclose->host, dclose );
         DoNotUnlink = TRUE;
      }
   }

   if( dclose->character )
   {
      sprintf( log_buf, "Closing link to %s.", ch->pcdata->filename );
      log_string_plus( log_buf, LOG_COMM, UMAX( sysdata.log_level, ch->level ) );

      if( dclose->connected == CON_PLAYING
          || dclose->connected == CON_EDITING
          || dclose->connected == CON_MEETING
          || dclose->connected == CON_ROLL_STATS
          || ( dclose->connected >= CON_NOTE_TO && dclose->connected <= CON_NOTE_FINISH ) )
      {
         act( AT_ACTION, "$n has lost $s link.", ch, NULL, NULL, TO_CANSEE );
         ch->desc = NULL;
      }
      else
      {
         dclose->character->desc = NULL;
         free_char( dclose->character );
      }
   }


   if( !DoNotUnlink )
   {
      if( d_next == dclose )
         d_next = d_next->next;
      UNLINK( dclose, first_descriptor, last_descriptor, next, prev );
   }

   if( dclose->descriptor == maxdesc )
      --maxdesc;

   free_desc( dclose );
   --num_descriptors;
   return;
}


bool read_from_descriptor( DESCRIPTOR_DATA * d )
{
   int iStart, iErr;
//    unsigned char * p;

   if( d->incomm[0] != '\0' )
      return TRUE;
/*
	for (p = d->inbuf; *p; p++)
	if (*p == IAC)
	{
		if (memcmp (p, will_termtype_str, strlen(will_termtype_str)) == 0)
		{
			memmove (p, &p [strlen (will_termtype_str)], strlen (&p [strlen (will_termtype_str)]) + 1);
			p--;
			write_to_buffer(d, req_termtype_str, 0);
		}
		else if (memcmp (p, wont_termtype_str, strlen(wont_termtype_str)) == 0)
		{
		        memmove (p, &p [strlen (wont_termtype_str)], strlen (&p [strlen (wont_termtype_str)]) + 1);
		        p--;
		}
		else if (memcmp (p, term_call_back_str, strlen(term_call_back_str)) == 0)
		{
		        char tempbuf[120];
			sprintf( tempbuf, "%s", p+4);
			tempbuf[strlen(tempbuf)-2] = '\0';
			if (d->client)
				STRFREE(d->client);
			d->client = STRALLOC( tempbuf);
			memmove (p, &p[strlen (p)], strlen (&p [strlen (p)]) + 1);
			p--;
		}
	}
  */
   iStart = strlen( d->inbuf );
   if( iStart >= sizeof( d->inbuf ) - 10 && !IS_IMMORTAL( d->character ) )
   {
      sprintf( log_buf, "%s input overflow!", d->host );
      log_string( log_buf );
      write_to_descriptor( d->descriptor,
                           "\n\r*** PUT A LID ON IT!!! ***\n\rYou cannot enter the same command more than 20 consecutive times!\n\r",
                           0 );
      return FALSE;
   }

   for( ;; )
   {
      int nRead;

      nRead = recv( d->descriptor, d->inbuf + iStart, sizeof( d->inbuf ) - 10 - iStart, 0 );
      iErr = errno;
      if( nRead > 0 )
      {
         iStart += nRead;
         if( d->inbuf[iStart - 1] == '\n' || d->inbuf[iStart - 1] == '\r' )
            break;
      }
      else if( nRead == 0 )
      {
         log_string_plus( "EOF encountered on read.", LOG_COMM, sysdata.log_level );
         return FALSE;
      }
      else if( iErr == EWOULDBLOCK )
         break;
      else
      {
         perror( "Read_from_descriptor" );
         return FALSE;
      }
   }

   d->inbuf[iStart] = '\0';
   return TRUE;
}



void read_from_buffer( DESCRIPTOR_DATA * d )
{
   int i, j, k;

   if( d->incomm[0] != '\0' )
      return;

   for( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r' && i < MAX_INBUF_SIZE * 2; i++ )
   {
      if( d->inbuf[i] == '\0' )
         return;
   }

   for( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
   {
      if( k >= 494 )
      {
         write_to_descriptor( d->descriptor, "Line too long.\n\r", 0 );

         d->inbuf[i] = '\n';
         d->inbuf[i + 1] = '\0';
         break;
      }

      if( d->inbuf[i] == '\b' && k > 0 )
         --k;
      else if( isascii( d->inbuf[i] ) && isprint( d->inbuf[i] ) )
         d->incomm[k++] = d->inbuf[i];
   }

   if( k == 0 )
      d->incomm[k++] = ' ';
   d->incomm[k] = '\0';

   if( k > 1 || d->incomm[0] == '!' )
   {
      if( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
      {
         d->repeat = 0;
      }
      else
      {
         if( ++d->repeat >= 20 && !IS_IMMORTAL( d->character ) )
         {
            write_to_descriptor( d->descriptor,
                                 "\n\r*** PUT A LID ON IT!!! ***\n\rYou cannot enter the same command more than 20 consecutive times!\n\r",
                                 0 );
            strcpy( d->incomm, "quit" );
         }
      }
   }

   if( d->incomm[0] == '!' )
      strcpy( d->incomm, d->inlast );
   else
      strcpy( d->inlast, d->incomm );

   while( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
      i++;
   for( j = 0; ( d->inbuf[j] = d->inbuf[i + j] ) != '\0'; j++ )
      ;
   return;
}



bool flush_buffer( DESCRIPTOR_DATA * d, bool fPrompt )
{
   char buf[MAX_INPUT_LENGTH];
   extern bool mud_down;

   if( !mud_down && d->outtop > 4096 )
   {
      memcpy( buf, d->outbuf, 512 );
      d->outtop -= 512;
      memmove( d->outbuf, d->outbuf + 512, d->outtop );
      if( d->snoop_by )
      {
         char snoopbuf[MAX_INPUT_LENGTH];

         buf[512] = '\0';
         if( d->character && d->character->name )
         {
            if( d->original && d->original->name )
               sprintf( snoopbuf, "%s (%s)", d->character->name, d->original->name );
            else
               sprintf( snoopbuf, "%s", d->character->name );
            write_to_buffer( d->snoop_by, snoopbuf, 0 );
         }
         write_to_buffer( d->snoop_by, "% ", 2 );
         write_to_buffer( d->snoop_by, buf, 0 );
      }
      if( !write_to_descriptor( d->descriptor, buf, 512 ) )
      {
         d->outtop = 0;
         return FALSE;
      }
      return TRUE;
   }


   if( fPrompt && !mud_down && d->connected == CON_PLAYING )
   {
      CHAR_DATA *ch;

      ch = d->original ? d->original : d->character;
      if( xIS_SET( ch->act, PLR_BLANK ) )
         write_to_buffer( d, "\n\r", 2 );


      if( xIS_SET( ch->act, PLR_PROMPT ) )
         display_prompt( d );
      if( xIS_SET( ch->act, PLR_TELNET_GA ) )
         write_to_buffer( d, go_ahead_str, 0 );
   }

   if( d->outtop == 0 )
      return TRUE;

   if( d->snoop_by )
   {
      if( d->character && d->character->name )
      {
         if( d->original && d->original->name )
            sprintf( buf, "%s (%s)", d->character->name, d->original->name );
         else
            sprintf( buf, "%s", d->character->name );
         write_to_buffer( d->snoop_by, buf, 0 );
      }
      write_to_buffer( d->snoop_by, "% ", 2 );
      write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
   }

   if( !write_to_descriptor( d->descriptor, d->outbuf, d->outtop ) )
   {
      d->outtop = 0;
      return FALSE;
   }
   else
   {
      d->outtop = 0;
      return TRUE;
   }
}



void write_to_buffer( DESCRIPTOR_DATA * d, const char *txt, int length )
{
   if( !d )
   {
      bug( "Write_to_buffer: NULL descriptor" );
      return;
   }

   if( !d->outbuf )
      return;

   if( length <= 0 )
      length = strlen( txt );

   if( d->outtop == 0 && !d->fcommand )
   {
      d->outbuf[0] = '\n';
      d->outbuf[1] = '\r';
      d->outtop = 2;
   }

   while( d->outtop + length >= d->outsize )
   {
      if( d->outsize > 32000 )
      {
         d->outtop = 0;
         close_socket( d, TRUE );
         bug( "Buffer overflow. Closing (%s).", d->character ? d->character->name : "???" );
         return;
      }
      d->outsize *= 2;
      RECREATE( d->outbuf, char, d->outsize );
   }

   strncpy( d->outbuf + d->outtop, txt, length );
   d->outtop += length;
   d->outbuf[d->outtop] = '\0';
   return;
}


bool write_to_descriptor( int desc, char *txt, int length )
{
   int iStart;
   int nWrite;
   int nBlock;

   if( length <= 0 )
      length = strlen( txt );

   for( iStart = 0; iStart < length; iStart += nWrite )
   {
      nBlock = UMIN( length - iStart, 4096 );
      if( ( nWrite = send( desc, txt + iStart, nBlock, 0 ) ) < 0 )
      {
         perror( "Write_to_descriptor" );
         return FALSE;
      }
   }

   return TRUE;
}



void show_title( DESCRIPTOR_DATA * d )
{
   CHAR_DATA *ch;

   ch = d->character;

   if( ch->pcdata->recall != 100000 )
   {
      ch->pcdata->recall = 100000;
   }
   write_to_buffer( d, "Press enter...\n\r", 0 );

   d->connected = CON_PRESS_ENTER;
}

void nanny( DESCRIPTOR_DATA * d, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];
   char buf4[MAX_STRING_LENGTH];
   char arg[MAX_STRING_LENGTH];
   CHAR_DATA *ch;
   char *pwdnew;
   char *p;
   int iClass;
   int iRace;
   bool fOld, chk;
   int i;
   int nct = 0;
   int nc = 0;
   int n;
   int count;
   int xe = 50;
   time_t cl;
   bool back = FALSE;

   if( d->connected != CON_NOTE_TEXT )
   {
      while( isspace( *argument ) )
         argument++;
   }

   ch = d->character;

   switch ( d->connected )
   {

      default:
         bug( "Nanny: bad d->connected %d.", d->connected );
         close_socket( d, TRUE );
         return;

      case CON_GET_NAME:
         if( argument[0] == '\0' )
         {
            close_socket( d, FALSE );
            return;
         }

         argument[0] = UPPER( argument[0] );

         if( !check_parse_name( argument, ( d->newstate != 0 ) ) )
         {
            write_to_buffer( d, "Illegal name, try another.\n\rName: ", 0 );
            return;
         }

         if( !str_cmp( argument, "New" ) )
         {
            if( d->newstate == 0 )
            {
               if( sysdata.DENY_NEW_PLAYERS == TRUE )
               {
                  sprintf( buf, "The mud is currently preparing for a reboot.\n\r" );
                  write_to_buffer( d, buf, 0 );
                  sprintf( buf, "New players are not accepted during this time.\n\r" );
                  write_to_buffer( d, buf, 0 );
                  sprintf( buf, "Please try again in a few minutes.\n\r" );
                  write_to_buffer( d, buf, 0 );
                  close_socket( d, FALSE );
               }
               sprintf( buf, "\n\rChoosing a name is one of the most important parts of this game...\n\r"
                        "Make sure to pick a name appropriate to the character you are going\n\r"
                        "to role play, and be sure that it suits a medieval theme.\n\r"
                        "If the name you select is not acceptable, you will be asked to choose\n\r"
                        "another one.\n\r\n\rPlease choose a name for your character: " );
               write_to_buffer( d, buf, 0 );
               d->newstate++;
               d->connected = CON_GET_NAME;
               return;
            }
            else
            {
               write_to_buffer( d, "Illegal name, try another.\n\rName: ", 0 );
               return;
            }
         }


         if( check_playing( d, argument, FALSE ) == BERR )
         {
            write_to_buffer( d, "Name: ", 0 );
            return;
         }

         fOld = load_char_obj( d, argument, TRUE );
         if( !d->character )
         {
            sprintf( log_buf, "Bad player file %s@%s.", argument, d->host );
            log_string( log_buf );
            write_to_buffer( d, "Your playerfile is corrupt...Please notify mud@karaenterprises.com.\n\r", 0 );
            close_socket( d, FALSE );
            return;
         }
         ch = d->character;
         if( check_bans( ch, BAN_SITE ) )
         {
            write_to_buffer( d, "Your site has been banned from this Mud.\n\r", 0 );
            close_socket( d, FALSE );
            return;
         }

         if( fOld )
         {
            if( check_bans( ch, BAN_CLASS ) )
            {
               write_to_buffer( d, "Your class has been banned from this Mud.\n\r", 0 );
               close_socket( d, FALSE );
               return;
            }
            if( check_bans( ch, BAN_RACE ) )
            {
               write_to_buffer( d, "Your race has been banned from this Mud.\n\r", 0 );
               close_socket( d, FALSE );
               return;
            }
         }

         if( xIS_SET( ch->act, PLR_DENY ) )
         {
            sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
            log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
            if( d->newstate != 0 )
            {
               write_to_buffer( d, "That name is already taken.  Please choose another: ", 0 );
               d->connected = CON_GET_NAME;
               d->character->desc = NULL;
               free_char( d->character );
               d->character = NULL;
               return;
            }
            write_to_buffer( d, "You are denied access.\n\r", 0 );
            close_socket( d, FALSE );
            return;
         }


         if( IS_IMMORTAL( ch ) && sysdata.check_imm_host && !check_immortal_domain( ch, d->host ) )
         {
            close_socket( d, FALSE );
            return;
         }

         write_to_buffer( d, do_termtype_str, 0 );
         chk = check_reconnect( d, argument, FALSE );
         if( chk == BERR )
            return;

         if( chk )
         {
            fOld = TRUE;
         }
         else
         {
            if( wizlock && !IS_IMMORTAL( ch ) )
            {
               write_to_buffer( d, "The game is wizlocked.  Only immortals can connect now.\n\r", 0 );
               write_to_buffer( d, "Please try back later.\n\r", 0 );
               close_socket( d, FALSE );
               return;
            }
            if( ch->level <= numlock && !IS_AGOD( ch ) && numlock != 0 )
            {
               write_to_buffer( d, "The game is locked to your level character.\n\r", 0 );
               write_to_buffer( d, "Please try back later.\n\r", 0 );
               close_socket( d, FALSE );
               return;
            }
         }

         if( fOld )
         {
            if( d->newstate != 0 )
            {
               write_to_buffer( d, "That name is already taken.  Please choose another: ", 0 );
               d->connected = CON_GET_NAME;
               d->character->desc = NULL;
               free_char( d->character );
               d->character = NULL;
               return;
            }
            write_to_buffer( d, "Password: ", 0 );
            write_to_buffer( d, echo_off_str, 0 );
            d->connected = CON_GET_OLD_PASSWORD;
            return;
         }
         else
         {
/*
	    if (d->newstate == 0)
	    {
	      write_to_buffer( d, "\n\rNo such player exists.\n\rPlease check your spelling, or type new to start a new player.\n\r\n\rName: ", 0 );
	      d->connected = CON_GET_NAME;
	      d->character->desc = NULL;
	      free_char( d->character ); 
	      d->character = NULL;
	      return;
	    }
*/
            if( sysdata.DENY_NEW_PLAYERS == TRUE )
            {
               sprintf( buf, "The mud is currently preparing for a reboot.\n\r" );
               write_to_buffer( d, buf, 0 );
               sprintf( buf, "New players are not accepted during this time.\n\r" );
               write_to_buffer( d, buf, 0 );
               sprintf( buf, "Please try again in a few minutes.\n\r" );
               write_to_buffer( d, buf, 0 );
               close_socket( d, FALSE );
               return;
            }
            sprintf( buf, "\n\rChoosing a name is one of the most important parts of this game...\n\r"
                     "Make sure to pick a name appropriate to the character you are going\n\r"
                     "to role play, and be sure that it suits a medieval theme.\n\r"
                     "If the name you select is not acceptable, you will be asked to choose\n\r"
                     "another one.\n\r\n\rDid I get that right, %s (Y/N)? ", argument );
            write_to_buffer( d, buf, 0 );
            d->newstate++;
            d->connected = CON_CONFIRM_NEW_NAME;
            return;
         }
         break;

      case CON_GET_OLD_PASSWORD:
         write_to_buffer( d, "\n\r", 2 );

         if( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
         {
            write_to_buffer( d, "Wrong password.\n\r", 0 );
            d->character->desc = NULL;
            close_socket( d, FALSE );
            return;
         }

         write_to_buffer( d, echo_on_str, 0 );

         if( check_playing( d, ch->pcdata->filename, TRUE ) )
            return;

         chk = check_reconnect( d, ch->pcdata->filename, TRUE );
         if( chk == BERR )
         {
            if( d->character && d->character->desc )
               d->character->desc = NULL;
            close_socket( d, FALSE );
            return;
         }
         if( chk == TRUE )
            return;

         sprintf( buf, ch->pcdata->filename );
         d->character->desc = NULL;
         free_char( d->character );
         d->character = NULL;
         fOld = load_char_obj( d, buf, FALSE );
         ch = d->character;
         if( ch->position == POS_FIGHTING
             || ch->position == POS_EVASIVE
             || ch->position == POS_DEFENSIVE || ch->position == POS_AGGRESSIVE || ch->position == POS_BERSERK )
            ch->position = POS_STANDING;

         if( ch->pcdata->learned[gsn_karaaura] != 100 )
         {
            ch->pcdata->learned[gsn_karaaura] = 100;
         }
         sprintf( log_buf, "%s@%s(%s) has connected.", ch->pcdata->filename, d->host, d->user );
         if( IS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) )
         {
            REMOVE_BIT( ch->pcdata->flags, PCFLAG_DEADLY );
         }
         if( !xIS_SET( ch->act, PLR_WIZINVIS ) )
         {
            if( ch->level < LEVEL_BUILD )
            {
               log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
            }
            else
            {
               log_string_plus( log_buf, LOG_COMM, ch->level );
            }
         }
         show_title( d );
         break;

      case CON_CONFIRM_NEW_NAME:
         switch ( *argument )
         {
            case 'y':
            case 'Y':
               sprintf( buf, "\n\rMake sure to use a password that won't be easily guessed by someone else."
                        "\n\rPick a good password for %s: %s", ch->name, echo_off_str );
               write_to_buffer( d, buf, 0 );
               d->connected = CON_GET_NEW_PASSWORD;
               break;

            case 'n':
            case 'N':
               back = TRUE;
               if( back )
               {
                  write_to_buffer( d, "Ok, what IS it, then? ", 0 );
                  d->character->desc = NULL;
                  free_char( d->character );
                  d->character = NULL;
                  d->connected = CON_GET_NAME;
                  d->newstate = 0;
                  return;
               }
               break;

            default:
               write_to_buffer( d, "Please type Yes or No. ", 0 );
               break;
         }
         break;

      case CON_GET_NEW_PASSWORD:
         write_to_buffer( d, "\n\r", 2 );

         if( strlen( argument ) < 5 )
         {
            write_to_buffer( d, "Password must be at least five characters long.\n\rPassword: ", 0 );
            return;
         }

         pwdnew = crypt( argument, ch->name );
         for( p = pwdnew; *p != '\0'; p++ )
         {
            if( *p == '~' )
            {
               write_to_buffer( d, "New password not acceptable, try again.\n\rPassword: ", 0 );
               return;
            }
         }

         DISPOSE( ch->pcdata->pwd );
         ch->pcdata->pwd = str_dup( pwdnew );
         write_to_buffer( d, "\n\rPlease retype the password to confirm: ", 0 );
         d->connected = CON_CONFIRM_NEW_PASSWORD;
         break;

      case CON_CONFIRM_NEW_PASSWORD:
         write_to_buffer( d, "\n\r", 2 );

         if( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
         {
            write_to_buffer( d, "Passwords don't match.\n\rRetype password: ", 0 );
            d->connected = CON_GET_NEW_PASSWORD;
            return;
         }

         write_to_buffer( d, echo_on_str, 0 );
         write_to_buffer( d, "\n\rWhat is your sex (M/F)? ", 0 );
         d->connected = CON_GET_NEW_SEX;
         break;

      case CON_GET_NEW_SEX:
         switch ( argument[0] )
         {
            case 'm':
            case 'M':
               ch->sex = SEX_MALE;
               break;
            case 'f':
            case 'F':
               ch->sex = SEX_FEMALE;
               break;
            default:
               write_to_buffer( d, "That's not a sex.\n\rWhat IS your sex? ", 0 );
               return;
         }
/*
	write_to_buffer( d, "\n\rSelect a class, or type help [class] to learn more about that class.\n\r", 0 );
	write_to_buffer( d, "---------------------------------------------------\n\r", 0 );
	buf[0] = '\0';

	for ( iClass = 0; iClass < (MAX_PC_CLASS-4); iClass++ )
	{
	    if ( class_table[iClass]->who_name &&
	    	 class_table[iClass]->who_name[0] != '\0' )
	    {
			if ( str_cmp( class_table[iClass]->who_name, "Jedika" ) )
			{
			strcat( buf, class_table[iClass]->who_name );
			strcat( buf, "    \t- TNL " );
			sprintf( buf2, "%d", class_table[iClass]->exp_tnl );
			strcat( buf, buf2 );
			strcat( buf, "\n\r" );
			}
	    }
	}
	strcat( buf, "\n\rPlease choose a class: " );
	write_to_buffer( d, buf, 0 );*/
         write_to_buffer( d, "\n\rSelect a class, or type help [class] to learn more about that class.\n\r[", 0 );
         buf[0] = '\0';
         for( iClass = 0; iClass < MAX_PC_CLASS; iClass++ )
         {
            if( class_table[iClass]->who_name && class_table[iClass]->who_name[0] != '\0' )
            {
               if( class_table[iClass]->fCreate )
               {
                  if( iClass > 0 )
                  {
                     if( strlen( buf ) + strlen( class_table[iClass]->who_name ) > 77 )
                     {
                        strcat( buf, "\n\r" );
                        write_to_buffer( d, buf, 0 );
                        buf[0] = '\0';
                     }
                     else
                     {
                        strcat( buf, " " );
                     }
                  }
                  strcat( buf, class_table[iClass]->who_name );
               }
            }
         }
         strcat( buf, "]\n\r: " );
         write_to_buffer( d, buf, 0 );
         d->connected = CON_GET_NEW_CLASS;
         break;

      case CON_GET_NEW_CLASS:
         argument = one_argument( argument, arg );

         if( !str_cmp( arg, "help" ) )
         {

            for( iClass = 0; iClass < MAX_PC_CLASS; iClass++ )
            {
               if( class_table[iClass]->who_name && class_table[iClass]->who_name[0] != '\0' )
               {
                  if( class_table[iClass]->fCreate )
                  {
                     if( toupper( argument[0] ) == toupper( class_table[iClass]->who_name[0] )
                         && !str_prefix( argument, class_table[iClass]->who_name ) )
                     {
                        do_help( ch, argument );
                        write_to_buffer( d, "Please choose a class: ", 0 );
                        return;
                     }
                  }
               }
            }
            write_to_buffer( d, "No such help topic.  Please choose a class: ", 0 );
            return;
         }

         for( iClass = 0; iClass < MAX_PC_CLASS; iClass++ )
         {
            if( class_table[iClass]->who_name && class_table[iClass]->who_name[0] != '\0' )
            {
               if( class_table[iClass]->fCreate )
               {
                  if( toupper( arg[0] ) == toupper( class_table[iClass]->who_name[0] )
                      && !str_prefix( arg, class_table[iClass]->who_name ) )
                  {
                     ch->class = iClass;
                     break;
                  }
               }
            }
         }

         if( iClass == MAX_PC_CLASS
             || !class_table[iClass]->fCreate
             || !class_table[iClass]->who_name
             || class_table[iClass]->who_name[0] == '\0' || !str_cmp( class_table[iClass]->who_name, "unused" ) )
         {
            write_to_buffer( d, "That's not a class.\n\rWhat IS your class? ", 0 );
            return;
         }


         if( check_bans( ch, BAN_CLASS ) )
         {
            write_to_buffer( d, "That class is not currently avaiable.\n\rWhat IS your class? ", 0 );
            return;
         }

         write_to_buffer( d, "\n\rYou may choose from the following races, or type help [race] to learn more:\n\r", 0 );
         write_to_buffer( d, "---------------------------------------------------\n\r", 0 );
         buf[0] = '\0';
         for( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
         {
            if( race_table[iRace]->race_name && race_table[iRace]->race_name[0] != '\0'
                && !IS_SET( race_table[iRace]->class_restriction, 1 << ch->class )
                && str_cmp( race_table[iRace]->race_name, "unused" ) )
            {  /*
                * if ( iRace > 0 )
                * {
                * if ( strlen(buf)+strlen(race_table[iRace]->race_name) > 77 )
                * {
                * strcat( buf, "\n\r" );
                * write_to_buffer( d, buf, 0 );
                * buf[0] = '\0';
                * }
                * else
                * strcat( buf, " " );
                * }
                * strcat( buf, race_table[iRace]->race_name );
                * } */
               sprintf( buf4, "%10s", race_table[iRace]->race_name );
//    strcat( buf, race_table[iRace]->race_name );
               strcat( buf, buf4 );
               strcat( buf, " - TNL " );
               sprintf( buf2, "%d", race_table[iRace]->exp_tnl );
               strcat( buf, buf2 );
               strcat( buf, "\n\r" );
            }
         }
         strcat( buf, "\n\n\rPlease choose a race: " );
         write_to_buffer( d, buf, 0 );
         d->connected = CON_GET_NEW_RACE;
         break;

      case CON_GET_NEW_RACE:
         argument = one_argument( argument, arg );
         if( !str_cmp( arg, "help" ) )
         {
            for( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
            {
               if( toupper( argument[0] ) == toupper( race_table[iRace]->race_name[0] )
                   && !str_prefix( argument, race_table[iRace]->race_name ) )
               {
                  do_help( ch, argument );
                  write_to_buffer( d, "Please choose a race: ", 0 );
                  return;
               }
            }
            write_to_buffer( d, "No help on that topic.  Please choose a race: ", 0 );
            return;
         }


         for( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
         {
            if( toupper( arg[0] ) == toupper( race_table[iRace]->race_name[0] )
                && !str_prefix( arg, race_table[iRace]->race_name ) )
            {
               ch->race = iRace;
               break;
            }
         }

         if( iRace == MAX_PC_RACE
             || !race_table[iRace]->race_name || race_table[iRace]->race_name[0] == '\0'
             || IS_SET( race_table[iRace]->class_restriction, 1 << ch->class )
             || !str_cmp( race_table[iRace]->race_name, "unused" ) )
         {
            write_to_buffer( d, "That's not a race.\n\rWhat IS your race? ", 0 );
            return;
         }
         if( check_bans( ch, BAN_RACE ) )
         {
            write_to_buffer( d, "That race is not currently available.\n\rWhat is your race? ", 0 );
            return;
         }


         write_to_buffer( d, "\n\rAre you an experienced player, (Y/N)? ", 0 );



         d->connected = CON_GET_EXP;
         break;

      case CON_GET_WANT_RIPANSI:
         switch ( argument[0] )
         {
            case 'r':
            case 'R':
               xSET_BIT( ch->act, PLR_RIP );
               xSET_BIT( ch->act, PLR_ANSI );
               break;
            case 'a':
            case 'A':
               xSET_BIT( ch->act, PLR_ANSI );
               break;
            case 'n':
            case 'N':
               break;
            default:
               write_to_buffer( d, "Invalid selection.\n\rANSI or NONE? ", 0 );
               return;
         }
         sprintf( log_buf, "%s@%s new %s %s.", ch->name, d->host,
                  race_table[ch->race]->race_name, class_table[ch->class]->who_name );
         log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
         to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL, sysdata.log_level );
         show_title( d );
         ch->level = 0;
         ch->sublevel = 0;
         ch->wlevel = 0;
         SET_BIT( ch->pcdata->flags, PCFLAG_HELPSTART );
         ch->position = POS_STANDING;
         d->connected = CON_PRESS_ENTER;
         set_pager_color( AT_PLAIN, ch );
         return;
         break;

      case CON_PRESS_ENTER:
         if( chk_watch( get_trust( ch ), ch->name, d->host ) )
            SET_BIT( ch->pcdata->flags, PCFLAG_WATCH );
         else
            REMOVE_BIT( ch->pcdata->flags, PCFLAG_WATCH );

         if( ch->position == POS_MOUNTED )
            ch->position = POS_STANDING;

         set_pager_color( AT_PLAIN, ch );
         if( xIS_SET( ch->act, PLR_RIP ) )
            send_rip_screen( ch );
         if( xIS_SET( ch->act, PLR_ANSI ) )
            send_to_pager( "\033[2J", ch );
         else
            send_to_pager( "\014", ch );
         if( IS_AGOD( ch ) )
            do_help( ch, "imotd" );
         if( ch->level < 601 && ch->level > 100 )
            do_help( ch, "amotd" );
         if( ch->level <= 100 )
            do_help( ch, "motd" );
         send_to_pager( "\n\rPress enter...", ch );
         d->connected = CON_READ_MOTD;
         break;

      case CON_READ_MOTD:
      {
         char motdbuf[MAX_STRING_LENGTH];

         sprintf( motdbuf, "\n\rWelcome to %s...\n\r"
                  "....running on %s version %s.%s build %d.\n\r", sysdata.mud_name,
                  Ke_CODEBASE_NAME, Ke_VERSION_MAJOR, Ke_VERSION_MINOR, Ke_BUILD );
         write_to_buffer( d, motdbuf, 0 );
      }
         send_to_char( "&W~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~&D\n\r", ch );
         add_char( ch );
         d->connected = CON_PLAYING;

         if( ch->level == 0 )
         {
            OBJ_DATA *obj;
            int iLang;

            ch->pcdata->clan_name = STRALLOC( "" );
            ch->pcdata->clan = NULL;
            name_stamp_stats( ch );
            ch->affected_by = race_table[ch->race]->affected;

            ch->armor += race_table[ch->race]->ac_plus;
            ch->alignment += race_table[ch->race]->alignment;
            ch->attacks = race_table[ch->race]->attacks;
            ch->defenses = race_table[ch->race]->defenses;
            ch->saving_poison_death = race_table[ch->race]->saving_poison_death;
            ch->saving_wand = race_table[ch->race]->saving_wand;
            ch->saving_para_petri = race_table[ch->race]->saving_para_petri;
            ch->saving_breath = race_table[ch->race]->saving_breath;
            ch->saving_spell_staff = race_table[ch->race]->saving_spell_staff;

            ch->height = number_range( race_table[ch->race]->height * .9, race_table[ch->race]->height * 1.1 );
            ch->weight = number_range( race_table[ch->race]->weight * .9, race_table[ch->race]->weight * 1.1 );




            if( ch->class == CLASS_PALADIN )
               ch->alignment = 1000;
            if( ch->class == CLASS_SORCERER )
               ch->alignment = -1000;





            if( ( iLang = skill_lookup( "common" ) ) < 0 )
               bug( "Nanny: cannot find common language." );
            else
               ch->pcdata->learned[iLang] = 100;

            ch->level = 1;
            ch->exp = 0;
            ch->exp += ( 1 * class_table[ch->class]->exp_tnl );
            ch->max_hit += race_table[ch->race]->hit;
            ch->max_mana += race_table[ch->race]->mana;
            ch->max_blood += race_table[ch->race]->mana;
            ch->hit = UMAX( 1, ch->max_hit );
            ch->mana = UMAX( 1, ch->max_mana );
            ch->blood = UMAX( 1, ch->max_blood );
            ch->move = ch->max_move;
            ch->pcdata->pretiti = 0;
            time( &cl );
            ch->created = ctime( &cl );
            ch->practice = 45;
            ch->gold = 300;
            sprintf( buf, "the %s", title_table[ch->class][ch->level][ch->sex == SEX_FEMALE ? 1 : 0] );
            set_title( ch, buf );
            newchar( ch );
//    update_newpl( ch );

            ch->pcdata->learned[gsn_karaaura] = 100;

            xSET_BIT( ch->act, PLR_AUTOGOLD );
            xSET_BIT( ch->act, PLR_AUTOLOOT );
            xSET_BIT( ch->act, PLR_A_TITLE );
            SET_BIT( ch->pcdata->flags, PCFLAG_POSI );
            ch->mod_str = 3;
            ch->mod_int = 3;
            ch->mod_wis = 3;
            ch->mod_dex = 3;
            ch->mod_con = 3;
            ch->mod_cha = 3;
            ch->mod_lck = 3;

            {
               obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_VEST ), 1 );
               if( IS_ARM_TYPE( obj ) )
               {
                  basestat( obj );
               }
               obj_to_char( obj, ch );
               obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_GHOST ), 1 );
               if( IS_ARM_TYPE( obj ) )
               {
                  basestat( obj );
               }
               obj_to_char( obj, ch );
               if( ch->class != CLASS_SAMURAI )
               {
                  obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SHIELD ), 1 );
                  if( IS_ARM_TYPE( obj ) )
                  {
                     basestat( obj );
                  }
                  obj_to_char( obj, ch );
               }
               obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_BANNER ), 1 );
               if( IS_ARM_TYPE( obj ) )
               {
                  basestat( obj );
               }
               obj_to_char( obj, ch );

               if( ( ch->class == CLASS_MAGE ) || ( ch->class == CLASS_THIEF ) || ( ch->class == CLASS_BARD )
                   || ( ch->class == CLASS_TAEDIEN ) || ( ch->class == CLASS_VAMPIRE ) )
               {
                  obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_DAGGER ), 1 );
                  if( IS_ARM_TYPE( obj ) )
                  {
                     basestat( obj );
                  }
                  obj_to_char( obj, ch );
                  ch->pcdata->learned[gsn_daggers] = 100;
               }


               else if( ch->class == CLASS_CLERIC )
               {
                  obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_MACE ), 1 );
                  if( IS_ARM_TYPE( obj ) )
                  {
                     basestat( obj );
                  }
                  obj_to_char( obj, ch );
                  ch->pcdata->learned[gsn_maces_hammers] = 100;
               }

               else if( ch->class == CLASS_ARCHER )
               {
                  for( count = 0; count < xe; count++ )
                  {
                     obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_ARROW ), 1 );
                     if( IS_ARM_TYPE( obj ) )
                     {
                        basestat( obj );
                     }
                     obj_to_char( obj, ch );
                  }
                  obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_BOW ), 1 );
                  if( IS_ARM_TYPE( obj ) )
                  {
                     basestat( obj );
                  }
                  obj_to_char( obj, ch );
                  obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_QUIVER ), 1 );
                  if( IS_ARM_TYPE( obj ) )
                  {
                     basestat( obj );
                  }
                  obj_to_char( obj, ch );
                  ch->pcdata->learned[gsn_archery] = 100;
               }

               else if( ch->class == CLASS_JEDI )
               {
                  obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_LIGHTSABER ), 1 );
                  if( IS_ARM_TYPE( obj ) )
                  {
                     basestat( obj );
                  }
                  obj_to_char( obj, ch );
                  ch->pcdata->learned[gsn_energy] = 100;
               }
               else if( ch->class == CLASS_RANGER )
               {
                  obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_STAFF ), 1 );
                  if( IS_ARM_TYPE( obj ) )
                  {
                     basestat( obj );
                  }
                  obj_to_char( obj, ch );
                  obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
                  if( IS_ARM_TYPE( obj ) )
                  {
                     basestat( obj );
                  }
                  obj_to_char( obj, ch );
                  ch->pcdata->learned[gsn_swords] = 100;
               }
               else if( ch->class == CLASS_BARBARIAN )
               {
                  obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
                  if( IS_ARM_TYPE( obj ) )
                  {
                     basestat( obj );
                  }
                  obj_to_char( obj, ch );
                  ch->pcdata->learned[gsn_swords] = 100;
               }
               else if( ch->class == CLASS_SAMURAI )
               {
                  obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD_TWO ), 1 );
                  if( IS_ARM_TYPE( obj ) )
                  {
                     basestat( obj );
                  }
                  obj_to_char( obj, ch );
                  ch->pcdata->learned[gsn_katana] = 100;
               }


               else if( ch->class == CLASS_SORCERER )
               {
                  obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_DAGGER ), 1 );
                  if( IS_ARM_TYPE( obj ) )
                  {
                     basestat( obj );
                  }
                  obj_to_char( obj, ch );
                  ch->pcdata->learned[gsn_daggers] = 100;
               }


               else if( ch->class == CLASS_PALADIN )
               {
                  obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
                  if( IS_ARM_TYPE( obj ) )
                  {
                     basestat( obj );
                  }
                  obj_to_char( obj, ch );
                  ch->pcdata->learned[gsn_swords] = 100;
               }



               else if( ( ch->class == CLASS_WARRIOR ) ||
                        ( ch->class == CLASS_WARWIZARD ) || ( ch->class == CLASS_WEREWOLF ) )
               {
                  obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SWORD ), 1 );
                  if( IS_ARM_TYPE( obj ) )
                  {
                     basestat( obj );
                  }
                  obj_to_char( obj, ch );
                  ch->pcdata->learned[gsn_swords] = 100;
               }
            }
            if( !sysdata.WAIT_FOR_AUTH )
            {
               if( IS_SET( ch->pcdata->flags, PCFLAG_EXP ) )
               {
                  char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL_ADV ) );
                  ch->pcdata->recall = 100000;
               }
               else
               {
                  char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
                  ch->pcdata->recall = 9025;
               }
               ch_printf_color( ch, "&pYou feel a tap as Nicole releases a wave of power through you.&D\n\r" );
               ch->level += 1;
               advance_level( ch, TRUE );
               ch->exp = exp_level( ch, ch->level );
               ch->trust = 0;
               ch->pcdata->authed_by = "Admin";
            }
            else
            {
               if( IS_SET( ch->pcdata->flags, PCFLAG_EXP ) )
               {
                  char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL_ADV ) );
                  ch->pcdata->recall = 21000;
                  ch->pcdata->auth_state = 0;
                  SET_BIT( ch->pcdata->flags, PCFLAG_UNAUTHED );
               }
               else
               {
                  char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
                  ch->pcdata->recall = 9025;
                  ch->pcdata->auth_state = 1;
                  SET_BIT( ch->pcdata->flags, PCFLAG_UNAUTHED );
                  sprintf( log_buf, "(NEWBIE) %s @ %s new %s %s applying...",
                           ch->name, ch->desc->host, race_table[ch->race]->race_name, class_table[ch->class]->who_name );
                  to_channel( log_buf, CHANNEL_AUTH, "Auth", LEVEL_IMMORTAL, sysdata.log_level );
               }
            }
            ch->pcdata->prompt = STRALLOC( "" );
            if( ch->class == CLASS_ARCHER )
            {
               do_put( ch, "all.arrow quiver" );
            }
            do_wear( ch, "all" );
         }
         else if( !IS_IMMORTAL( ch ) && ch->pcdata->release_date > 0 && ch->pcdata->release_date > current_time )
         {
            if( ch->in_room->vnum == 6 || ch->in_room->vnum == 8 || ch->in_room->vnum == 1206 )
               char_to_room( ch, ch->in_room );
            else
               char_to_room( ch, get_room_index( 8 ) );
         }
         else if( ch->in_room && ( IS_IMMORTAL( ch ) || !xIS_SET( ch->in_room->room_flags, ROOM_PROTOTYPE ) ) )
         {
            char_to_room( ch, ch->in_room );
         }
         else if( IS_IMMORTAL( ch ) )
         {
            char_to_room( ch, get_room_index( ROOM_VNUM_CHAT ) );
         }
         else
         {
            char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
         }


         if( get_timer( ch, TIMER_SHOVEDRAG ) > 0 )
            remove_timer( ch, TIMER_SHOVEDRAG );

         if( get_timer( ch, TIMER_PKILLED ) > 0 )
            remove_timer( ch, TIMER_PKILLED );

         if( !IS_IMMORTAL( ch ) )
         {
            sprintf( buf, "&R<&WLOG INFO&R>&D %s has entered %s...", ch->name, sysdata.mud_name );
            talk_info( AT_BLUE, buf );
         }

         act( AT_ACTION, "$n has entered the game.", ch, NULL, NULL, TO_CANSEE );
         if( xIS_SET( ch->act, PLR_PINGME ) )
         {
            send_to_char( "Automatically disabling pingme...\n\r", ch );
            xREMOVE_BIT( ch->act, PLR_PINGME );
         }

         if( ch->pcdata->pet )
         {
            act( AT_ACTION, "$n returns to $s master from the Void.", ch->pcdata->pet, NULL, ch, TO_NOTVICT );
            act( AT_ACTION, "$N returns with you to the realms.", ch, NULL, ch->pcdata->pet, TO_CHAR );
         }
         i = num_changes(  );
         if( i > 0 )
         {
            sprintf( buf, "&R+++ &WThere were &R%d &d&Wchanges made to the mud today type &GCHANGES&W to view &R+++&D\n\r",
                     i );
            send_to_char( buf, ch );
         }
         i = num_bugs(  );
         if( i > 0 )
            if( IS_IMMORTAL( ch ) )
               ch_printf( ch, "&R+++ &WThere were &R%d &d&Wbugs found today. &R+++\n\r", i );

         for( n = 0; n < MAX_BOARD; n++ )
         {
            nc = unread_notes_check( ch, &boards[n] );
            nct = ( nct + nc );
         }

         if( nct == 0 )
         {
            send_to_char( "&gYou have no unread notes.&D\n\r", ch );
         }
         else
         {
            sprintf( buf3, "&gYou have &Y%d&g unread notes. Type &Yboards&g to see which board they are on.&D\n\r", nct );
            send_to_char( buf3, ch );
         }

         if( ch->pcdata->board == NULL )
            ch->pcdata->board = &boards[DEFAULT_BOARD];

         if( ch->level > 799 )
         {
            immlog_onoff( ch, "on" );
         }

// do_news( ch, "last 3" );
         if( sysdata.dmud == 1 )
         {
            do_mudstat( ch, "" );
         }
         login_save( ch );
         tax_player( ch );
         imp_msg( ch );
         mail_count( ch );
         scan_rent( ch );

         if( ch->class == 6 )
         {
            do_rework( ch, "warrior" );
         }

         if( !ch->was_in_room && ch->in_room == get_room_index( ROOM_VNUM_TEMPLE ) )
            ch->was_in_room = get_room_index( ROOM_VNUM_TEMPLE );
         else if( ch->was_in_room == get_room_index( ROOM_VNUM_TEMPLE ) )
            ch->was_in_room = get_room_index( ROOM_VNUM_TEMPLE );
         else if( !ch->was_in_room )
            ch->was_in_room = ch->in_room;
         break;

      case CON_ROLL_STATS:
         switch ( argument[0] )
         {
            case 'y':
            case 'Y':
               write_to_buffer( d, "\n\rAre you an experienced player, (Y/N)? ", 0 );
               d->connected = CON_GET_EXP;
               break;

            case 'n':
            case 'N':

               name_stamp_stats( ch );

               sprintf( buf, "\n\rStr: %d  Int: %d  Wis: %d  Dex: %d  Con: %d  Cha: %d  Lck: %d\n\rKeep? (Y/N)",
                        ch->perm_str, ch->perm_int, ch->perm_wis, ch->perm_dex, ch->perm_con, ch->perm_cha, ch->perm_lck );
               write_to_buffer( d, buf, 0 );
               return;
            default:
               write_to_buffer( d, "Yes or No? ", 0 );
               return;
         }
         break;
      case CON_GET_EXP:
         switch ( argument[0] )
         {
            case 'y':
            case 'Y':
               write_to_buffer( d, "\n\rWould you like ANSI, or no color support, (A/N)? ", 0 );
               SET_BIT( ch->pcdata->flags, PCFLAG_EXP );
               d->connected = CON_GET_WANT_RIPANSI;
               break;

            case 'n':
            case 'N':
               write_to_buffer( d, "\n\rWould you like ANSI, or no color support, (A/N)? ", 0 );
               d->connected = CON_GET_WANT_RIPANSI;
               return;
            default:
               write_to_buffer( d, "Yes or No? ", 0 );
               return;
         }
         break;
      case CON_NOTE_TO:
         handle_con_note_to( d, argument );
         break;

      case CON_NOTE_SUBJECT:
         handle_con_note_subject( d, argument );
         break;

      case CON_NOTE_EXPIRE:
         handle_con_note_expire( d, argument );
         break;

      case CON_NOTE_TEXT:
         handle_con_note_text( d, argument );
         break;

      case CON_NOTE_FINISH:
         handle_con_note_finish( d, argument );
         break;


   }

   return;
}

bool is_reserved_name( char *name )
{
   RESERVE_DATA *res;

   for( res = first_reserved; res; res = res->next )
      if( ( *res->name == '*' && !str_infix( res->name + 1, name ) ) || !str_cmp( res->name, name ) )
         return TRUE;
   return FALSE;
}


bool check_parse_name( char *name, bool newchar )
{

   if( is_reserved_name( name ) )
      return FALSE;

   if( strlen( name ) < 3 )
      return FALSE;

   if( strlen( name ) > 12 )
      return FALSE;

   {
      char *pc;
      bool fIll;

      fIll = TRUE;
      for( pc = name; *pc != '\0'; pc++ )
      {
         if( !isalpha( *pc ) )
            return FALSE;
         if( LOWER( *pc ) != 'i' && LOWER( *pc ) != 'l' )
            fIll = FALSE;
      }

      if( fIll )
         return FALSE;
   }


   return TRUE;
}



bool check_reconnect( DESCRIPTOR_DATA * d, char *name, bool fConn )
{
   CHAR_DATA *ch;

   for( ch = first_char; ch; ch = ch->next )
   {
      if( !IS_NPC( ch ) && ( !fConn || !ch->desc ) && ch->pcdata->filename && !str_cmp( name, ch->pcdata->filename ) )
      {
         if( fConn && ch->switched )
         {
            write_to_buffer( d, "Already playing.\n\rName: ", 0 );
            d->connected = CON_GET_NAME;
            if( d->character )
            {
               d->character->desc = NULL;
               free_char( d->character );
               d->character = NULL;
            }
            return BERR;
         }
         if( fConn == FALSE )
         {
            DISPOSE( d->character->pcdata->pwd );
            d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
         }
         else
         {
            d->character->desc = NULL;
            free_char( d->character );
            d->character = ch;
            ch->desc = d;
            ch->timer = 0;
            send_to_char( "Reconnecting.\n\r", ch );
            do_look( ch, "auto" );
            act( AT_ACTION, "$n has reconnected.", ch, NULL, NULL, TO_CANSEE );
            sprintf( log_buf, "%s@%s(%s) reconnected.", ch->pcdata->filename, d->host, d->user );
            log_string_plus( log_buf, LOG_COMM, UMAX( sysdata.log_level, ch->level ) );
            d->connected = CON_PLAYING;
            if( ch->pcdata->in_progress )
               send_to_char( "You have a note in progress. Type \"note write\" to continue it.\n\r", ch );

         }
         return TRUE;
      }
   }

   return FALSE;
}



bool check_playing( DESCRIPTOR_DATA * d, char *name, bool kick )
{
   CHAR_DATA *ch;

   DESCRIPTOR_DATA *dold;
   int cstate;

   for( dold = first_descriptor; dold; dold = dold->next )
   {
      if( dold != d
          && ( dold->character || dold->original )
          && !str_cmp( name, dold->original ? dold->original->pcdata->filename : dold->character->pcdata->filename ) )
      {
         cstate = dold->connected;
         ch = dold->original ? dold->original : dold->character;
         if( !ch->name
             || ( cstate != CON_PLAYING && cstate != CON_EDITING && cstate != CON_MEETING && cstate != CON_ROLL_STATS ) )
         {
            write_to_buffer( d, "Already connected - try again.\n\r", 0 );
            sprintf( log_buf, "%s already connected.", ch->pcdata->filename );
            log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
            return BERR;
         }
         if( !kick )
            return TRUE;
         write_to_buffer( d, "Already playing... Kicking off old connection.\n\r", 0 );
         write_to_buffer( dold, "Kicking off old connection... bye!\n\r", 0 );
         close_socket( dold, FALSE );
         d->character->desc = NULL;
         free_char( d->character );
         d->character = ch;
         ch->desc = d;
         ch->timer = 0;
         if( ch->switched )
            do_return( ch->switched, "" );
         ch->switched = NULL;
         send_to_char( "Reconnecting.\n\r", ch );
         do_look( ch, "auto" );
         act( AT_ACTION, "$n has reconnected, kicking off old link.", ch, NULL, NULL, TO_CANSEE );
         sprintf( log_buf, "%s@%s reconnected, kicking off old link.", ch->pcdata->filename, d->host );
         log_string_plus( log_buf, LOG_COMM, UMAX( sysdata.log_level, ch->level ) );
         d->connected = cstate;
         return TRUE;
      }
   }

   return FALSE;
}



void stop_idling( CHAR_DATA * ch )
{
   ROOM_INDEX_DATA *was_in_room;


   if( !ch || !ch->desc || ch->desc->connected != CON_PLAYING || !IS_IDLE( ch ) )
      return;


   ch->timer = 0;
   was_in_room = ch->was_in_room;
   char_from_room( ch );
   char_to_room( ch, was_in_room );
   ch->was_in_room = ch->in_room;
   REMOVE_BIT( ch->pcdata->flags, PCFLAG_IDLE );
   act( AT_ACTION, "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
   return;
}



void send_to_char( const char *txt, CHAR_DATA * ch )
{
   if( !ch )
   {
      bug( "Send_to_char: NULL *ch" );
      return;
   }

   send_to_char_color( txt, ch );
   return;
}

void send_to_char_color( const char *txt, CHAR_DATA * ch )
{
   DESCRIPTOR_DATA *d;
   char *colstr;
   const char *prevstr = txt;
   char colbuf[20];
   int ln;

   if( !ch )
   {
      bug( "Send_to_char_color: NULL *ch" );
      return;
   }
   if( !txt || !ch->desc )
      return;
   d = ch->desc;
   while( ( colstr = strpbrk( prevstr, "&^}" ) ) != NULL )
   {
      if( colstr > prevstr )
         write_to_buffer( d, prevstr, ( colstr - prevstr ) );
      ln = make_color_sequence( colstr, colbuf, d );
      if( ln < 0 )
      {
         prevstr = colstr + 1;
         break;
      }
      else if( ln > 0 )
         write_to_buffer( d, colbuf, ln );
      prevstr = colstr + 2;
   }
   if( *prevstr )
      write_to_buffer( d, prevstr, 0 );
   return;
}

void write_to_pager( DESCRIPTOR_DATA * d, const char *txt, int length )
{
   int pageroffset;

   if( length <= 0 )
      length = strlen( txt );
   if( length == 0 )
      return;
   if( !d->pagebuf )
   {
      d->pagesize = MAX_STRING_LENGTH;
      CREATE( d->pagebuf, char, d->pagesize );
   }
   if( !d->pagepoint )
   {
      d->pagepoint = d->pagebuf;
      d->pagetop = 0;
      d->pagecmd = '\0';
   }
   if( d->pagetop == 0 && !d->fcommand )
   {
      d->pagebuf[0] = '\n';
      d->pagebuf[1] = '\r';
      d->pagetop = 2;
   }
   pageroffset = d->pagepoint - d->pagebuf;
   while( d->pagetop + length >= d->pagesize )
   {
      if( d->pagesize > 32000 )
      {
         bug( "Pager overflow.  Ignoring.\n\r" );
         d->pagetop = 0;
         d->pagepoint = NULL;
         DISPOSE( d->pagebuf );
         d->pagesize = MAX_STRING_LENGTH;
         return;
      }
      d->pagesize *= 2;
      RECREATE( d->pagebuf, char, d->pagesize );
   }
   d->pagepoint = d->pagebuf + pageroffset;
   strncpy( d->pagebuf + d->pagetop, txt, length );
   d->pagetop += length;
   d->pagebuf[d->pagetop] = '\0';
   return;
}

void send_to_pager( const char *txt, CHAR_DATA * ch )
{
   if( !ch )
   {
      bug( "Send_to_pager: NULL *ch" );
      return;
   }
   send_to_pager_color( txt, ch );
   return;
}

void send_to_pager_color( const char *txt, CHAR_DATA * ch )
{
   DESCRIPTOR_DATA *d;
   char *colstr;
   const char *prevstr = txt;
   char colbuf[20];
   int ln;

   if( !ch )
   {
      bug( "Send_to_pager_color: NULL *ch" );
      return;
   }
   if( !txt || !ch->desc )
      return;
   d = ch->desc;
   ch = d->original ? d->original : d->character;
   if( IS_NPC( ch ) || !IS_SET( ch->pcdata->flags, PCFLAG_PAGERON ) )
   {
      send_to_char_color( txt, d->character );
      return;
   }
   while( ( colstr = strpbrk( prevstr, "&^}" ) ) != NULL )
   {
      if( colstr > prevstr )
         write_to_pager( d, prevstr, ( colstr - prevstr ) );
      ln = make_color_sequence( colstr, colbuf, d );
      if( ln < 0 )
      {
         prevstr = colstr + 1;
         break;
      }
      else if( ln > 0 )
         write_to_pager( d, colbuf, ln );
      prevstr = colstr + 2;
   }
   if( *prevstr )
      write_to_pager( d, prevstr, 0 );
   return;
}

sh_int figure_color( sh_int AType, CHAR_DATA * ch )
{
   int at = AType;
   if( at >= AT_COLORBASE && at < AT_TOPCOLOR )
   {
      at -= AT_COLORBASE;
      if( IS_NPC( ch ) || ch->pcdata->colorize[at] == -1 )
         at = at_color_table[at].def_color;
      else
         at = ch->pcdata->colorize[at];
   }
   if( at < 0 || at > AT_WHITE + AT_BLINK )
   {
      bug( "Figure_color: color %d invalid.", at );
      at = AT_GREY;
   }
   return at;
}

void set_char_color( sh_int AType, CHAR_DATA * ch )
{
   if( !ch || !ch->desc || IS_NPC( ch ) )
      return;

   AType = figure_color( AType, ch );

   write_to_buffer( ch->desc, color_str( AType, ch ), 0 );
   ch->desc->pagecolor = ch->pcdata->colorize[AType];
}

void set_pager_color( sh_int AType, CHAR_DATA * ch )
{
   if( !ch || !ch->desc || IS_NPC( ch ) )
      return;

   AType = figure_color( AType, ch );

   write_to_pager( ch->desc, color_str( AType, ch ), 0 );
   ch->desc->pagecolor = ch->pcdata->colorize[AType];
}


void ch_printf( CHAR_DATA * ch, char *fmt, ... )
{
   char buf[MAX_STRING_LENGTH * 2];
   va_list args;

   va_start( args, fmt );
   vsprintf( buf, fmt, args );
   va_end( args );

   send_to_char( buf, ch );
}

void pager_printf( CHAR_DATA * ch, char *fmt, ... )
{
   char buf[MAX_STRING_LENGTH * 2];
   va_list args;

   va_start( args, fmt );
   vsprintf( buf, fmt, args );
   va_end( args );

   send_to_pager( buf, ch );
}


char *myobj( OBJ_DATA * obj )
{
   if( !str_prefix( "a ", obj->short_descr ) )
      return obj->short_descr + 2;
   if( !str_prefix( "an ", obj->short_descr ) )
      return obj->short_descr + 3;
   if( !str_prefix( "the ", obj->short_descr ) )
      return obj->short_descr + 4;
   if( !str_prefix( "some ", obj->short_descr ) )
      return obj->short_descr + 5;
   return obj->short_descr;
}

void log_printf( char *fmt, ... )
{
   char buf[MAX_STRING_LENGTH * 2];
   va_list args;

   va_start( args, fmt );
   vsprintf( buf, fmt, args );
   va_end( args );

   log_string( buf );
}

char *obj_short( OBJ_DATA * obj )
{
   static char buf[MAX_STRING_LENGTH];

   if( obj->count > 1 )
   {
      sprintf( buf, "%s (%d)", obj->short_descr, obj->count );
      return buf;
   }
   return obj->short_descr;
}

void ch_printf_color( CHAR_DATA * ch, char *fmt, ... )
{
   char buf[MAX_STRING_LENGTH * 2];
   va_list args;

   va_start( args, fmt );
   vsprintf( buf, fmt, args );
   va_end( args );

   send_to_char_color( buf, ch );
}

void pager_printf_color( CHAR_DATA * ch, char *fmt, ... )
{
   char buf[MAX_STRING_LENGTH * 2];
   va_list args;

   va_start( args, fmt );
   vsprintf( buf, fmt, args );
   va_end( args );

   send_to_pager_color( buf, ch );
}

#define MORPHNAME(ch)   ((ch->morph&&ch->morph->morph)? \
                         ch->morph->morph->short_desc: \
                         IS_NPC(ch) ? ch->short_descr : ch->name)
#define NAME(ch)        (IS_NPC(ch) ? ch->short_descr : ch->name)

char *act_string( const char *format, CHAR_DATA * to, CHAR_DATA * ch, const void *arg1, const void *arg2, int flags )
{
   static char *const he_she[] = { "he", "she" };
   static char *const him_her[] = { "him", "her" };
   static char *const his_her[] = { "his", "her" };
   static char buf[MAX_STRING_LENGTH * 2];
   char fname[MAX_INPUT_LENGTH];
   char *point = buf;
   const char *str = format;
   const char *i;
   CHAR_DATA *vch = ( CHAR_DATA * ) arg2;
   OBJ_DATA *obj1 = ( OBJ_DATA * ) arg1;
   OBJ_DATA *obj2 = ( OBJ_DATA * ) arg2;

   if( str[0] == '$' )
      DONT_UPPER = FALSE;

   while( *str != '\0' )
   {
      if( *str != '$' )
      {
         *point++ = *str++;
         continue;
      }
      ++str;
      if( !arg2 && *str >= 'A' && *str <= 'Z' )
      {
         bug( "Act: missing arg2 for code %c:", *str );
         bug( format );
         i = " <@@@> ";
      }
      else
      {
         switch ( *str )
         {
            default:
               bug( "Act: bad code %c.", *str );
               i = " <@@@> ";
               break;
            case 't':
               i = ( char * )arg1;
               break;
            case 'T':
               i = ( char * )arg2;
               break;
            case 'n':
               i = ( to ? IS_AFFECTED( ch, AFF_SHAPESHIFT ) ? ch->morph->morph->short_desc : PERS( ch, to ) : NAME( ch ) );
               break;
            case 'N':
               i = ( to ? IS_AFFECTED( vch, AFF_SHAPESHIFT ) ? vch->morph->morph->
                     short_desc : PERS( vch, to ) : NAME( vch ) );
               break;

            case 'e':
               if( ch->sex > 1 || ch->sex < 0 )
               {
                  bug( "act_string: player %s has sex set at %d! (%d)", ch->name, ch->sex, HAS_VNUM( ch ) );
                  i = "he";
               }
               else
                  i = he_she[URANGE( 0, ch->sex, 1 )];
               break;
            case 'E':
               if( vch->sex > 1 || vch->sex < 0 )
               {
                  bug( "act_string: player %s has sex set at %d! (%d)", vch->name, vch->sex, HAS_VNUM( ch ) );
                  i = "he";
               }
               else
                  i = he_she[URANGE( 0, vch->sex, 1 )];
               break;
            case 'm':
               if( ch->sex > 1 || ch->sex < 0 )
               {
                  bug( "act_string: player %s has sex set at %d! (%d)", ch->name, ch->sex, HAS_VNUM( ch ) );
                  i = "he";
               }
               else
                  i = him_her[URANGE( 0, ch->sex, 1 )];
               break;
            case 'M':
               if( vch->sex > 1 || vch->sex < 0 )
               {
                  bug( "act_string: player %s has sex set at %d! (%d)", vch->name, vch->sex, HAS_VNUM( ch ) );
                  i = "he";
               }
               else
                  i = him_her[URANGE( 0, vch->sex, 1 )];
               break;
            case 's':
               if( ch->sex > 1 || ch->sex < 0 )
               {
                  bug( "act_string: player %s has sex set at %d! (%d)", ch->name, ch->sex, HAS_VNUM( ch ) );
                  i = "his";
               }
               else
                  i = his_her[URANGE( 0, ch->sex, 1 )];
               break;
            case 'S':
               if( vch->sex > 1 || vch->sex < 0 )
               {
                  bug( "act_string: player %s has sex set at %d! (%d)", vch->name, vch->sex, HAS_VNUM( ch ) );
                  i = "his";
               }
               else
                  i = his_her[URANGE( 0, vch->sex, 1 )];
               break;
            case 'q':
               i = ( to == ch ) ? "" : "s";
               break;
            case 'Q':
               i = ( to == ch ) ? "your" : his_her[URANGE( 0, ch->sex, 1 )];
               break;
            case 'p':
               i = ( !to || can_see_obj( to, obj1 ) ? obj_short( obj1 ) : "something" );
               break;
            case 'P':
               i = ( !to || can_see_obj( to, obj2 ) ? obj_short( obj2 ) : "something" );
               break;
            case 'd':
               if( !arg2 || ( ( char * )arg2 )[0] == '\0' )
                  i = "door";
               else
               {
                  one_argument( ( char * )arg2, fname );
                  i = fname;
               }
               break;
         }
      }
      ++str;
      while( ( *point = *i ) != '\0' )
         ++point, ++i;

   }
   strcpy( point, "\n\r" );
   if( !DONT_UPPER )
      buf[0] = UPPER( buf[0] );
   return buf;
}

#undef NAME

void act( sh_int AType, const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, int type )
{
   char *txt;
   CHAR_DATA *to;
   CHAR_DATA *vch = ( CHAR_DATA * ) arg2;

   if( !format || format[0] == '\0' )
      return;

   if( !ch )
   {
      bug( "Act: null ch. (%s)", format );
      return;
   }

   if( !ch->in_room )
      to = NULL;
   else if( type == TO_CHAR )
      to = ch;
   else
      to = ch->in_room->first_person;

   if( IS_NPC( ch ) && xIS_SET( ch->act, ACT_SECRETIVE ) && type != TO_CHAR )
      return;

   if( type == TO_VICT )
   {
      if( !vch )
      {
         bug( "Act: null vch with TO_VICT." );
         bug( "%s (%s)", ch->name, format );
         return;
      }
      if( !vch->in_room )
      {
         bug( "Act: vch in NULL room!" );
         bug( "%s -> %s (%s)", ch->name, vch->name, format );
         return;
      }
      to = vch;
   }

   if( MOBtrigger && type != TO_CHAR && type != TO_VICT && to )
   {
      OBJ_DATA *to_obj;

      txt = act_string( format, NULL, ch, arg1, arg2, STRING_IMM );
      if( HAS_PROG( to->in_room, ACT_PROG ) )
         rprog_act_trigger( txt, to->in_room, ch, ( OBJ_DATA * ) arg1, ( void * )arg2 );
      for( to_obj = to->in_room->first_content; to_obj; to_obj = to_obj->next_content )
         if( HAS_PROG( to_obj->pIndexData, ACT_PROG ) )
            oprog_act_trigger( txt, to_obj, ch, ( OBJ_DATA * ) arg1, ( void * )arg2 );
   }

   for( ; to; to = ( type == TO_CHAR || type == TO_VICT ) ? NULL : to->next_in_room )
   {
      if( ( !to->desc && ( IS_NPC( to ) && !HAS_PROG( to->pIndexData, ACT_PROG ) ) ) || !IS_AWAKE( to ) )
         continue;

      if( type == TO_CHAR && to != ch )
         continue;
      if( type == TO_VICT && ( to != vch || to == ch ) )
         continue;
      if( type == TO_ROOM && to == ch )
         continue;
      if( type == TO_NOBSPAM && !IS_NOBSPAM( to ) && ( to == ch || to == vch ) )
         continue;
      if( type == TO_NOTVICT && ( to == ch || to == vch ) )
         continue;
      if( type == TO_CANSEE && ( to == ch ||
                                 ( !IS_IMMORTAL( to ) && !IS_NPC( ch ) && ( xIS_SET( ch->act, PLR_WIZINVIS )
                                                                            && ( get_trust( to ) <
                                                                                 ( ch->pcdata ? ch->pcdata->
                                                                                   wizinvis : 0 ) ) ) ) ) )
         continue;

      if( IS_IMMORTAL( to ) )
         txt = act_string( format, to, ch, arg1, arg2, STRING_IMM );
      else
         txt = act_string( format, to, ch, arg1, arg2, STRING_NONE );

      if( to->desc )
      {
         if( AType == AT_COLORIZE )
         {
            if( txt[0] == '&' )
               send_to_char_color( txt, to );
            else
            {
               set_char_color( AT_MAGIC, to );
               send_to_char_color( txt, to );
            }
         }
         else
         {
            set_char_color( AType, to );
            send_to_char_color( txt, to );
         }
      }
      if( MOBtrigger )
      {
         mprog_act_trigger( txt, to, ch, ( OBJ_DATA * ) arg1, ( void * )arg2 );
      }
   }
   MOBtrigger = TRUE;
   return;
}

void imp_msg( CHAR_DATA * ch )
{
   if( sysdata.impmsg && sysdata.impmsg[0] != '\0' )
   {
      ch_printf( ch, "\n\r&R%s\n\r&D", sysdata.impmsg );
   }
   return;
}

void do_name( CHAR_DATA * ch, char *argument )
{
   char fname[1024];
   struct stat fst;
   CHAR_DATA *tmp;

   if( !NOT_AUTHED( ch ) || ch->pcdata->auth_state != 2 )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument[0] = UPPER( argument[0] );

   if( !check_parse_name( argument, TRUE ) )
   {
      send_to_char( "Illegal name, try another.\n\r", ch );
      return;
   }

   if( !str_cmp( ch->name, argument ) )
   {
      send_to_char( "That's already your name!\n\r", ch );
      return;
   }

   for( tmp = first_char; tmp; tmp = tmp->next )
   {
      if( !str_cmp( argument, tmp->name ) )
         break;
   }

   if( tmp )
   {
      send_to_char( "That name is already taken.  Please choose another.\n\r", ch );
      return;
   }

   sprintf( fname, "%s%c/%s", PLAYER_DIR, tolower( argument[0] ), capitalize( argument ) );
   if( stat( fname, &fst ) != -1 )
   {
      send_to_char( "That name is already taken.  Please choose another.\n\r", ch );
      return;
   }

   STRFREE( ch->name );
   ch->name = STRALLOC( argument );
   STRFREE( ch->pcdata->filename );
   ch->pcdata->filename = STRALLOC( argument );
   send_to_char( "Your name has been changed.  Please apply again.\n\r", ch );
   ch->pcdata->auth_state = 0;
   return;
}

char *default_prompt( CHAR_DATA * ch )
{
   static char buf[60];

   strcpy( buf, "&p%A&w<&R%h hp " );
   if( IS_VAMPIRE( ch ) )
      strcat( buf, "&r%b bp" );
   else
      strcat( buf, "&C%m m" );
   strcat( buf, " &G%v mv" );
   if( !IS_NPC( ch ) )
      strcat( buf, " &W%X Tnl %l%L &B(&R%t&B)&w> %c" );
   else
      strcat( buf, "&w>" );
   if( !IS_NPC( ch ) && IS_IMMORTAL( ch ) )
      strcat( buf, "%i%R" );
   strcat( buf, "&D%k" );
   return buf;
}

int getcolor( char clr )
{
   static const char colors[16] = "xrgObpcwzRGYBPCW";
   int r;

   for( r = 0; r < 16; r++ )
      if( clr == colors[r] )
         return r;
   return -1;
}

void display_prompt( DESCRIPTOR_DATA * d )
{
   CHAR_DATA *ch = d->character;
   CHAR_DATA *och = ( d->original ? d->original : d->character );
   CHAR_DATA *victim;
   CHAR_DATA *targ;
   bool ansi = ( !IS_NPC( och ) && xIS_SET( och->act, PLR_ANSI ) );
   const char *prompt;
   char buf[MAX_STRING_LENGTH];
   char *pbuf = buf;
   int stat, percent;

   if( !ch )
   {
      bug( "display_prompt: NULL ch" );
      return;
   }
   if( !IS_NPC( ch ) && ch->substate != SUB_NONE && ch->pcdata->subprompt && ch->pcdata->subprompt[0] != '\0' )
      prompt = ch->pcdata->subprompt;
   else if( IS_NPC( ch ) || ( ( !ch->pcdata->prompt || !*ch->pcdata->prompt ) ) )
      prompt = default_prompt( ch );
   else
      prompt = ch->pcdata->prompt;
   if( ansi )
   {
      strcpy( pbuf, "\033[m" );
      d->prevcolor = 0x07;
      pbuf += 3;
   }
   for( ; *prompt; prompt++ )
   {
      if( *prompt != '%' )
      {
         *( pbuf++ ) = *prompt;
         continue;
      }
      ++prompt;
      if( !*prompt )
         break;
      if( *prompt == *( prompt - 1 ) )
      {
         *( pbuf++ ) = *prompt;
         continue;
      }
      switch ( *( prompt - 1 ) )
      {
         default:
            bug( "Display_prompt: bad command char '%c'.", *( prompt - 1 ) );
            break;
         case '%':
            *pbuf = '\0';
            stat = 0x80000000;
            switch ( *prompt )
            {
               case '%':
                  *pbuf++ = '%';
                  *pbuf = '\0';
                  break;
               case 'a':
                  if( ch->level >= 10 )
                     stat = ch->alignment;
                  else if( IS_GOOD( ch ) )
                     strcpy( pbuf, "good" );
                  else if( IS_EVIL( ch ) )
                     strcpy( pbuf, "evil" );
                  else
                     strcpy( pbuf, "neutral" );
                  break;
               case 'A':
                  sprintf( pbuf, "%s%s%s%s%s%s%s%s%s",
                           xIS_SET( ch->act, PLR_AFK ) ? "(AFK) " : "",
                           xIS_SET( ch->act, PLR_IAW ) ? "(IAW) " : "",
                           xIS_SET( ch->act, PLR_IDLE ) ? "(Idling) " : "",
                           xIS_SET( ch->act, PLR_DEAF ) ? "(Deaf) " : "",
                           ( IS_AFFECTED( ch, AFF_INVISIBLE )
                             || IS_AFFECTED( ch, AFF_IMPINVISIBLE ) ) ? "!" : "",
                           IS_AFFECTED( ch, AFF_SANCTUARY ) ? "$" : "",
                           IS_AFFECTED( ch, AFF_DEMONFIRE ) ? "D" : "",
                           ( IS_AFFECTED( ch, AFF_MOVEHIDE )
                             || IS_AFFECTED( ch, AFF_HIDE ) ) ? "@" : "", IS_AFFECTED( ch, AFF_SNEAK ) ? "#" : "" );
                  break;
               case 'C':
                  if( !IS_IMMORTAL( ch ) )
                     break;
                  if( !ch->fighting || ( victim = ch->fighting->who ) == NULL )
                     strcpy( pbuf, "N/A" );
                  else if( !victim->fighting || ( victim = victim->fighting->who ) == NULL )
                     strcpy( pbuf, "N/A" );
                  else
                  {
                     if( victim->max_hit > 0 )
                        percent = ( 100 * victim->hit ) / victim->max_hit;
                     else
                        percent = -1;
                     if( percent >= 100 )
                        strcpy( pbuf, "perfect health" );
                     else if( percent >= 90 )
                        strcpy( pbuf, "slightly scratched" );
                     else if( percent >= 80 )
                        strcpy( pbuf, "few bruises" );
                     else if( percent >= 70 )
                        strcpy( pbuf, "some cuts" );
                     else if( percent >= 60 )
                        strcpy( pbuf, "several wounds" );
                     else if( percent >= 50 )
                        strcpy( pbuf, "nasty wounds" );
                     else if( percent >= 40 )
                        strcpy( pbuf, "bleeding freely" );
                     else if( percent >= 30 )
                        strcpy( pbuf, "covered in blood" );
                     else if( percent >= 20 )
                        strcpy( pbuf, "leaking guts" );
                     else if( percent >= 10 )
                        strcpy( pbuf, "almost dead" );
                     else
                        strcpy( pbuf, "DYING" );
                  }
                  break;
               case 'c':
                  if( !ch->fighting || ( victim = ch->fighting->who ) == NULL )
                     strcpy( pbuf, "" );
                  else
                  {
                     if( victim->max_hit > 0 )
                        percent = ( 100 * victim->hit ) / victim->max_hit;
                     else
                        percent = -1;
                     if( percent >= 100 )
                        strcpy( pbuf, "<Mob Cond: &Rperfect health&w>" );
                     else if( percent >= 90 )
                        strcpy( pbuf, "<Mob Cond: &Rslightly scratched&w>" );
                     else if( percent >= 80 )
                        strcpy( pbuf, "<Mob Cond: &Rfew bruises&w>" );
                     else if( percent >= 70 )
                        strcpy( pbuf, "<Mob Cond: &Rsome cuts&w>" );
                     else if( percent >= 60 )
                        strcpy( pbuf, "<Mob Cond: &Rseveral wounds&w>" );
                     else if( percent >= 50 )
                        strcpy( pbuf, "<Mob Cond: &Rnasty wounds&w>" );
                     else if( percent >= 40 )
                        strcpy( pbuf, "<Mob Cond: &Rbleeding freely&w>" );
                     else if( percent >= 30 )
                        strcpy( pbuf, "<Mob Cond: &Rcovered in blood&w>" );
                     else if( percent >= 20 )
                        strcpy( pbuf, "<Mob Cond: &Rleaking guts&w>" );
                     else if( percent >= 10 )
                        strcpy( pbuf, "<Mob Cond: &Ralmost dead&w>" );
                     else
                        strcpy( pbuf, "<Mob Cond: &RDYING&w>" );
                  }
                  break;
               case 'h':
                  stat = ch->hit;
                  break;
               case 'H':
                  stat = ch->max_hit;
                  break;
               case 'l':
                  if( ch->class == 0 )
                     strcpy( pbuf, "&YMg" );
                  else if( ch->class == 1 )
                     strcpy( pbuf, "&YCl" );
                  else if( ch->class == 2 )
                     strcpy( pbuf, "&YTh" );
                  else if( ch->class == 3 )
                     strcpy( pbuf, "&YWa" );
                  else if( ch->class == 4 )
                     strcpy( pbuf, "&YVm" );
                  else if( ch->class == 5 )
                     strcpy( pbuf, "&YWz" );
                  else if( ch->class == 6 )
                     strcpy( pbuf, "&YJe" );
                  else if( ch->class == 7 )
                     strcpy( pbuf, "&YWw" );
                  else if( ch->class == 8 )
                     strcpy( pbuf, "&YRg" );
                  else if( ch->class == 9 )
                     strcpy( pbuf, "&YTa" );
                  else if( ch->class == 10 )
                     strcpy( pbuf, "&YAr" );
                  else if( ch->class == 11 )
                     strcpy( pbuf, "&YBb" );
                  else if( ch->class == 12 )
                     strcpy( pbuf, "&YSr" );
                  else if( ch->class == 13 )
                     strcpy( pbuf, "&YPl" );
                  else if( ch->class == 14 )
                     strcpy( pbuf, "&YBr" );
                  else if( ch->class == 15 )
                     strcpy( pbuf, "&YSm" );
                  else if( ch->class == 16 )
                     strcpy( pbuf, "&YMk" );
                  break;
               case 'L':
                  if( ch->dualclass == 0 )
                     strcpy( pbuf, "&BMg" );
                  else if( ch->dualclass == 1 )
                     strcpy( pbuf, "&BCl" );
                  else if( ch->dualclass == 2 )
                     strcpy( pbuf, "&BTh" );
                  else if( ch->dualclass == 3 )
                     strcpy( pbuf, "&BWa" );
                  else if( ch->dualclass == 4 )
                     strcpy( pbuf, "&BVm" );
                  else if( ch->dualclass == 5 )
                     strcpy( pbuf, "&BWz" );
                  else if( ch->dualclass == 6 )
                     strcpy( pbuf, "&BJe" );
                  else if( ch->dualclass == 7 )
                     strcpy( pbuf, "&BWw" );
                  else if( ch->dualclass == 8 )
                     strcpy( pbuf, "&BRg" );
                  else if( ch->dualclass == 9 )
                     strcpy( pbuf, "&BTa" );
                  else if( ch->dualclass == 10 )
                     strcpy( pbuf, "&BAr" );
                  else if( ch->dualclass == 11 )
                     strcpy( pbuf, "&BBb" );
                  else if( ch->dualclass == 12 )
                     strcpy( pbuf, "&BSr" );
                  else if( ch->dualclass == 13 )
                     strcpy( pbuf, "&BPl" );
                  else if( ch->dualclass == 14 )
                     strcpy( pbuf, "&BBr" );
                  else if( ch->dualclass == 15 )
                     strcpy( pbuf, "&BSm" );
                  else if( ch->class == 16 )
                     strcpy( pbuf, "&BMk" );
                  break;
               case 'm':
                  if( IS_VAMPIRE( ch ) )
                     stat = 0;
                  else
                     stat = ch->mana;
                  break;
               case 'M':
                  if( IS_VAMPIRE( ch ) )
                     stat = 0;
                  else
                     stat = ch->max_mana;
                  break;
               case 'N':
                  if( !IS_IMMORTAL( ch ) )
                     break;
                  if( !ch->fighting || ( victim = ch->fighting->who ) == NULL )
                     strcpy( pbuf, "N/A" );
                  else if( !victim->fighting || ( victim = victim->fighting->who ) == NULL )
                     strcpy( pbuf, "N/A" );
                  else
                  {
                     if( ch == victim )
                        strcpy( pbuf, "You" );
                     else if( IS_NPC( victim ) )
                        strcpy( pbuf, victim->short_descr );
                     else
                        strcpy( pbuf, victim->name );
                     pbuf[0] = UPPER( pbuf[0] );
                  }
                  break;
               case 'n':
                  if( !IS_IMMORTAL( ch ) )
                     break;
                  if( !ch->fighting || ( victim = ch->fighting->who ) == NULL )
                     strcpy( pbuf, "N/A" );
                  else
                  {
                     if( ch == victim )
                        strcpy( pbuf, "You" );
                     else if( IS_NPC( victim ) )
                        strcpy( pbuf, victim->short_descr );
                     else
                        strcpy( pbuf, victim->name );
                     pbuf[0] = UPPER( pbuf[0] );
                  }
                  break;
               case 'k':
                  strcpy( pbuf, "\n\r" );
                  break;
               case 't':
                  if( ( targ = ch->pcdata->monitor ) == NULL )
                  {
                     strcpy( pbuf, "-" );
                  }
                  else
                  {
                     stat = targ->hit;
                  }
                  break;

               case 'T':
                  if( time_info.hour < 5 )
                     strcpy( pbuf, "night" );
                  else if( time_info.hour < 6 )
                     strcpy( pbuf, "dawn" );
                  else if( time_info.hour < 19 )
                     strcpy( pbuf, "day" );
                  else if( time_info.hour < 21 )
                     strcpy( pbuf, "dusk" );
                  else
                     strcpy( pbuf, "night" );
                  break;
               case 'b':
                  if( IS_VAMPIRE( ch ) )
                     stat = ch->blood;
                  else
                     stat = 0;
                  break;
               case 'B':
                  if( IS_VAMPIRE( ch ) )
                     stat = ch->max_blood;
                  else
                     stat = 0;
                  break;
               case 'u':
                  stat = num_descriptors;
                  break;
               case 'U':
                  stat = sysdata.maxplayers;
                  break;
               case 'v':
                  stat = ch->move;
                  break;
               case 'V':
                  stat = ch->max_move;
                  break;
               case 'g':
                  stat = ch->gold;
                  break;
               case 'r':
                  if( IS_IMMORTAL( och ) )
                     stat = ch->in_room->vnum;
                  break;
               case 'F':
                  if( IS_IMMORTAL( och ) )
                     sprintf( pbuf, "%s", ext_flag_string( &ch->in_room->room_flags, r_flags ) );
                  break;
               case 'R':
                  if( xIS_SET( och->act, PLR_ROOMVNUM ) )
                     sprintf( pbuf, "<#%d> ", ch->in_room->vnum );
                  break;
               case 'x':
                  stat = ch->exp;
                  break;
               case 'X':
                  if( ch->level < 100 )
                     stat = exp_level( ch, ch->level + 1 ) - ch->exp;
                  else
                     stat = IS_AGOD( ch ) ? get_exp_tnl( ch ) : exp_level( ch, ch->sublevel + 1 ) - ch->exp;
                  break;
               case 'o':
                  if( auction->item )
                     strcpy( pbuf, auction->item->name );
                  break;
               case 'S':
                  if( ch->style == STYLE_BERSERK )
                     strcpy( pbuf, "B" );
                  else if( ch->style == STYLE_AGGRESSIVE )
                     strcpy( pbuf, "A" );
                  else if( ch->style == STYLE_DEFENSIVE )
                     strcpy( pbuf, "D" );
                  else if( ch->style == STYLE_EVASIVE )
                     strcpy( pbuf, "E" );
                  else
                     strcpy( pbuf, "S" );
                  break;
               case 'i':
                  if( ( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_WIZINVIS ) ) ||
                      ( IS_NPC( ch ) && xIS_SET( ch->act, ACT_MOBINVIS ) ) )
                     sprintf( pbuf, "(Invis %d) ", ( IS_NPC( ch ) ? ch->mobinvis : ch->pcdata->wizinvis ) );
                  else if( IS_AFFECTED( ch, AFF_INVISIBLE ) )
                     sprintf( pbuf, "(Invis) " );
                  break;
               case 'I':
                  stat = ( IS_NPC( ch ) ? ( xIS_SET( ch->act, ACT_MOBINVIS ) ? ch->mobinvis : 0 )
                           : ( xIS_SET( ch->act, PLR_WIZINVIS ) ? ch->pcdata->wizinvis : 0 ) );
                  break;
            }
            if( stat != 0x80000000 )
               sprintf( pbuf, "%d", stat );
            pbuf += strlen( pbuf );
            break;
      }
   }
   *pbuf = '\0';
   send_to_char( buf, ch );
   return;
}

int make_color_sequence( const char *col, char *code, DESCRIPTOR_DATA * d )
{
   CHAR_DATA *ch = ( d->original ? d->original : d->character );
   const char *ctype = col;
   int ln;
   bool ansi = FALSE;

   if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_ANSI ) )
   {
      ansi = TRUE;
   }

   col++;

   if( !*col )
      ln = -1;
   else if( *ctype != '&' && *ctype != '^' && *ctype != '}' )
   {
      bug( "colorcode: command '%c' not '&', '^' or '}'", *ctype );
      ln = -1;
   }
   else if( *col == *ctype )
   {
      code[0] = *col;
      code[1] = '\0';
      ln = 1;
   }
   else if( !ansi )
      ln = 0;
   else
   {
      if( *ctype == '&' )
      {
         switch ( *col )
         {
            default:
               code[0] = *ctype;
               code[1] = *col;
               code[2] = '\0';
               return 2;
            case 'i':
            case 'I':
               strcpy( code, ANSI_ITALIC );
               break;
            case 'v':
            case 'V':
               strcpy( code, ANSI_REVERSE );
               break;
            case 'u':
            case 'U':
               strcpy( code, ANSI_UNDERLINE );
               break;
            case 'd':
               strcpy( code, ANSI_RESET );
               break;
            case 'D':
               strcpy( code, ANSI_RESET );
               strcat( code, color_str( ch->desc->pagecolor, ch ) );
               break;
            case 'x':
               strcpy( code, ANSI_BLACK );
               break;
            case 'O':
               strcpy( code, ANSI_ORANGE );
               break;
            case 'c':
               strcpy( code, ANSI_CYAN );
               break;
            case 'z':
               strcpy( code, ANSI_DGREY );
               break;
            case 'g':
               strcpy( code, ANSI_DGREEN );
               break;
            case 'G':
               strcpy( code, ANSI_GREEN );
               break;
            case 'P':
               strcpy( code, ANSI_PINK );
               break;
            case 'r':
               strcpy( code, ANSI_DRED );
               break;
            case 'b':
               strcpy( code, ANSI_DBLUE );
               break;
            case 'w':
               strcpy( code, ANSI_GREY );
               break;
            case 'Y':
               strcpy( code, ANSI_YELLOW );
               break;
            case 'C':
               strcpy( code, ANSI_LBLUE );
               break;

            case 'p':
               strcpy( code, ANSI_PURPLE );
               break;
            case 'R':
               strcpy( code, ANSI_RED );
               break;
            case 'B':
               strcpy( code, ANSI_BLUE );
               break;
            case 'W':
               strcpy( code, ANSI_WHITE );
               break;
         }
      }
      if( *ctype == '}' )
      {
         switch ( *col )
         {
            default:
               code[0] = *ctype;
               code[1] = *col;
               code[2] = '\0';
               return 2;
            case 'x':
               strcpy( code, ANSI_BLACK );
               break;
            case 'O':
               strcpy( code, ANSI_ORANGE );
               break;
            case 'c':
               strcpy( code, ANSI_CYAN );
               break;
            case 'z':
               strcpy( code, ANSI_DGREY );
               break;
            case 'g':
               strcpy( code, ANSI_DGREEN );
               break;
            case 'G':
               strcpy( code, ANSI_GREEN );
               break;
            case 'P':
               strcpy( code, ANSI_PINK );
               break;
            case 'r':
               strcpy( code, ANSI_DRED );
               break;
            case 'b':
               strcpy( code, ANSI_DBLUE );
               break;
            case 'w':
               strcpy( code, ANSI_GREY );
               break;
            case 'Y':
               strcpy( code, ANSI_YELLOW );
               break;
            case 'C':
               strcpy( code, ANSI_LBLUE );
               break;
            case 'p':
               strcpy( code, ANSI_PURPLE );
               break;
            case 'R':
               strcpy( code, ANSI_RED );
               break;
            case 'B':
               strcpy( code, ANSI_BLUE );
               break;
            case 'W':
               strcpy( code, ANSI_WHITE );
               break;
         }
      }

      if( *ctype == '^' )
      {
         switch ( *col )
         {
            default:
               code[0] = *ctype;
               code[1] = *col;
               code[2] = '\0';
               return 2;
            case 'x':
               strcpy( code, BACK_BLACK );
               break;
            case 'r':
               strcpy( code, BACK_DRED );
               break;
            case 'g':
               strcpy( code, BACK_DGREEN );
               break;
            case 'O':
               strcpy( code, BACK_ORANGE );
               break;
            case 'b':
               strcpy( code, BACK_DBLUE );
               break;
            case 'p':
               strcpy( code, BACK_PURPLE );
               break;
            case 'c':
               strcpy( code, BACK_CYAN );
               break;
            case 'w':
               strcpy( code, BACK_GREY );
               break;
         }
      }
      ln = strlen( code );
   }
   if( ln <= 0 )
      *code = '\0';
   return ln;
}

void set_pager_input( DESCRIPTOR_DATA * d, char *argument )
{
   while( isspace( *argument ) )
      argument++;
   d->pagecmd = *argument;
   return;
}

bool pager_output( DESCRIPTOR_DATA * d )
{
   /*
    * register
    */ char *last;
   CHAR_DATA *ch;
   int pclines;
   /*
    * register
    */ int lines;
   bool ret;

   if( !d || !d->pagepoint || d->pagecmd == -1 )
      return TRUE;
   ch = d->original ? d->original : d->character;
   pclines = UMAX( ch->pcdata->pagerlen, 5 ) - 1;
   switch ( LOWER( d->pagecmd ) )
   {
      default:
         lines = 0;
         break;
      case 'b':
         lines = -1 - ( pclines * 2 );
         break;
      case 'r':
         lines = -1 - pclines;
         break;
      case 'n':
         lines = 0;
         pclines = 0x7FFFFFFF;
         break;
      case 'q':
         d->pagetop = 0;
         d->pagepoint = NULL;
         flush_buffer( d, TRUE );
         DISPOSE( d->pagebuf );
         d->pagesize = MAX_STRING_LENGTH;
         return TRUE;
   }
   while( lines < 0 && d->pagepoint >= d->pagebuf )
      if( *( --d->pagepoint ) == '\n' )
         ++lines;
   if( *d->pagepoint == '\n' && *( ++d->pagepoint ) == '\r' )
      ++d->pagepoint;
   if( d->pagepoint < d->pagebuf )
      d->pagepoint = d->pagebuf;
   for( lines = 0, last = d->pagepoint; lines < pclines; ++last )
      if( !*last )
         break;
      else if( *last == '\n' )
         ++lines;
   if( *last == '\r' )
      ++last;
   if( last != d->pagepoint )
   {
      if( !write_to_descriptor( d->descriptor, d->pagepoint, ( last - d->pagepoint ) ) )
         return FALSE;
      d->pagepoint = last;
   }
   while( isspace( *last ) )
      ++last;
   if( !*last )
   {
      d->pagetop = 0;
      d->pagepoint = NULL;
      flush_buffer( d, TRUE );
      DISPOSE( d->pagebuf );
      d->pagesize = MAX_STRING_LENGTH;
      return TRUE;
   }
   d->pagecmd = -1;
   if( xIS_SET( ch->act, PLR_ANSI ) )
      if( write_to_descriptor( d->descriptor, ANSI_LBLUE, 0 ) == FALSE )
         return FALSE;
   if( ( ret = write_to_descriptor( d->descriptor,
                                    "(C)ontinue, (N)on-stop, (R)efresh, (B)ack, (Q)uit: [C] ", 0 ) ) == FALSE )
      return FALSE;
   if( xIS_SET( ch->act, PLR_ANSI ) )
   {
      char buf[32];

      sprintf( buf, "%s", color_str( d->pagecolor, ch ) );
      ret = write_to_descriptor( d->descriptor, buf, 0 );
   }
   return ret;
}


void do_copyover( CHAR_DATA * ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   FILE *fp;
   fp = fopen( COPYOVER_FILE, "w" );

   if( !fp )
   {
      send_to_char( "Copyover file not writeable, aborted.\n\r", ch );
      log_printf( "Could not write to copyover file: %s", COPYOVER_FILE );
      perror( "do_copyover:fopen" );
      return;
   }

   sprintf( buf, ANSI_RED "\n\r<" ANSI_BLUE "INFO" ANSI_RED ">" ANSI_LBLUE " Warmboot of the MUD initiated.\n\r" ANSI_GREY );

   copyover( buf );
}

void copyover( char *argument )
{
   FILE *fp;
   DESCRIPTOR_DATA *d, *d_next;
   char buf[100], buf2[100];
   int position;
   fp = fopen( COPYOVER_FILE, "w" );

   mysql_keep_alive(  );
   for( d = first_descriptor; d; d = d_next )
   {
      CHAR_DATA *och = CH( d );
      d_next = d->next;
      if( !d->character || d->connected != CON_PLAYING )
      {
         write_to_descriptor( d->descriptor, "\n\rSorry, we are rebooting." " Come back in a few minutes.\n\r", 0 );
         close_socket( d, FALSE );
      }
      else
      {
         if( ( d->connected == CON_PLAYING ) && !xIS_SET( och->deaf, CHANNEL_INFO )
             && !xIS_SET( och->in_room->room_flags, ROOM_SILENCE ) && !NOT_AUTHED( och ) )
         {
            position = och->position;
            och->position = POS_STANDING;
            write_to_descriptor( d->descriptor, argument, 0 );
            och->position = position;
         }
         fprintf( fp, "%d %s %s\n", d->descriptor, och->name, d->host );
         if( och->level == 1 )
         {
            write_to_descriptor( d->descriptor, "Since you are level one,"
                                 "and level one characters do not save, you gain a free level!\n\r", 0 );
            advance_level( och, FALSE );
            och->level++;
         }
         save_char_obj( och );
//      save_finger( och );
      }
   }
   fprintf( fp, "-1\n" );
   fclose( fp );

   fclose( fpReserve );
   fclose( fpLOG );

   sprintf( buf, "%d", PORTN );
   sprintf( buf2, "%d", control );

   execl( EXE_FILE, "../../dist/src/acadia", buf, "copyover", buf2, "", "", "", ( char * )NULL );

   perror( "do_copyover: execl" );

   if( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
   {
      perror( NULL_FILE );
      exit( 1 );
   }
   if( ( fpLOG = fopen( NULL_FILE, "r" ) ) == NULL )
   {
      perror( NULL_FILE );
      exit( 1 );
   }

}
void save_sysdata args( ( SYSTEM_DATA sys ) );

void copyover_recover(  )
{
   DESCRIPTOR_DATA *d;
   FILE *fp;
   char name[100];
   char host[MAX_STRING_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int desc;
   bool fOld;
   int position;

   log_string( "Warmboot recovery initiated" );
   fp = fopen( COPYOVER_FILE, "r" );

   if( !fp )
   {
      perror( "copyover_recover:fopen" );
      log_string( "Warmboot file not found. Exitting.\n\r" );
      exit( 1 );
   }
   init_web( PORTN + 1 );
   sprintf( buf, "\n\r&R<&BINFO&R> &CWarmboot of MUD complete.&w\n\r" );
   unlink( COPYOVER_FILE );
   for( ;; )
   {
      fscanf( fp, "%d %s %s\n", &desc, name, host );
      if( desc == -1 )
         break;

//     if (!write_to_descriptor (desc, "\n\rRestoring from copyover...\n\r", 0))
      if( !desc )
      {
         close( desc );
         continue;
      }

      CREATE( d, DESCRIPTOR_DATA, 1 );
      init_descriptor( d, desc );

      d->host = STRALLOC( host );

      LINK( d, first_descriptor, last_descriptor, next, prev );
      d->connected = CON_COPYOVER_RECOVER;

      fOld = load_char_obj( d, name, FALSE );


      if( !fOld )
      {
         write_to_descriptor( desc, "\n\rSomehow, your character was lost in the warmboot. Please contact an Immortal.\n\r",
                              0 );
         close_socket( d, FALSE );
      }
      else
      {
//          write_to_descriptor (desc, "\n\rWarmboot recovery complete.\n\r",0);

         if( !d->character->in_room )
            d->character->in_room = get_room_index( ROOM_VNUM_TEMPLE );
         LINK( d->character, first_char, last_char, next, prev );

         num_descriptors++;
         sysdata.maxplayers++;
         char_to_room( d->character, d->character->in_room );
         if( !xIS_SET( d->character->deaf, CHANNEL_INFO )
             && !xIS_SET( d->character->in_room->room_flags, ROOM_SILENCE ) && !NOT_AUTHED( d->character ) )
         {
            position = d->character->position;
            d->character->position = POS_STANDING;
            act( AT_PLAIN, buf, d->character, NULL, NULL, TO_CHAR );
            d->character->position = position;
         }

         do_look( d->character, "auto noprog" );
         act( AT_ACTION, "$n materializes!", d->character, NULL, NULL, TO_ROOM );
         d->connected = CON_PLAYING;
         d->character->pcdata->board = &boards[DEFAULT_BOARD];
         if( d->character->pcdata->learned[gsn_karaaura] != 100 )
         {
            d->character->pcdata->learned[gsn_karaaura] = 100;
         }
         write_to_buffer( d, do_termtype_str, 0 );
      }
   }
   if( sysdata.kaura == TRUE )
   {
      sysdata.kaura = FALSE;
      save_sysdata( sysdata );
   }
   if( sysdata.fight_lock == TRUE )
   {
      sysdata.fight_lock = FALSE;
      save_sysdata( sysdata );
   }
}

char *color_str( sh_int AType, CHAR_DATA * ch )
{
   if( !ch )
   {
      bug( "color_str: NULL ch!", 0 );
      return ( "" );
   }

   if( IS_NPC( ch ) || !xIS_SET( ch->act, PLR_ANSI ) )
      return ( "" );

   switch ( AType )
   {
      case 0:
         return ( ANSI_BLACK );
         break;
      case 1:
         return ( ANSI_DRED );
         break;
      case 2:
         return ( ANSI_DGREEN );
         break;
      case 3:
         return ( ANSI_ORANGE );
         break;
      case 4:
         return ( ANSI_DBLUE );
         break;
      case 5:
         return ( ANSI_PURPLE );
         break;
      case 6:
         return ( ANSI_CYAN );
         break;
      case 7:
         return ( ANSI_GREY );
         break;
      case 8:
         return ( ANSI_DGREY );
         break;
      case 9:
         return ( ANSI_RED );
         break;
      case 10:
         return ( ANSI_GREEN );
         break;
      case 11:
         return ( ANSI_YELLOW );
         break;
      case 12:
         return ( ANSI_BLUE );
         break;
      case 13:
         return ( ANSI_PINK );
         break;
      case 14:
         return ( ANSI_LBLUE );
         break;
      case 15:
         return ( ANSI_WHITE );
         break;

      case 16:
         return ( BLINK_BLACK );
         break;
      case 17:
         return ( BLINK_DRED );
         break;
      case 18:
         return ( BLINK_DGREEN );
         break;
      case 19:
         return ( BLINK_ORANGE );
         break;
      case 20:
         return ( BLINK_DBLUE );
         break;
      case 21:
         return ( BLINK_PURPLE );
         break;
      case 22:
         return ( BLINK_CYAN );
         break;
      case 23:
         return ( BLINK_GREY );
         break;
      case 24:
         return ( BLINK_DGREY );
         break;
      case 25:
         return ( BLINK_RED );
         break;
      case 26:
         return ( BLINK_GREEN );
         break;
      case 27:
         return ( BLINK_YELLOW );
         break;
      case 28:
         return ( BLINK_BLUE );
         break;
      case 29:
         return ( BLINK_PINK );
         break;
      case 30:
         return ( BLINK_LBLUE );
         break;
      case 31:
         return ( BLINK_WHITE );
         break;

      default:
         return ( ANSI_RESET );
         break;
   }
}
