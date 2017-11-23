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
/*						Web Server module			                    */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#include "acadia.h"

/* Moved the Struct to acadia.h */

/* FUNCTION DEFS */
int send_buf( int fd, char *buf, bool filter );
void handle_web_request( WEB_DESCRIPTOR * wdesc );
void handle_web_who_request( WEB_DESCRIPTOR * wdesc );
void handle_web_wwwwho_request( WEB_DESCRIPTOR * wdesc );
void handle_web_wizlist_request( WEB_DESCRIPTOR * wdesc );
char *color_filter( char *string );
char *text2html( const char *ip );
char *parse_quotes( char *arg );

/* The mark of the end of a HTTP/1.x request */
const char ENDREQUEST[5] = { 13, 10, 13, 10, 0 };  /* (CRLFCRLF) */

/* Some of these variables were scattered throughout the code with externs
   defined here, why keep all the web stuff together? -- LrdElder */

/* Locals */
int sockfd;
bool WEBSERVER_STATUS;
WEB_DESCRIPTOR *first_webdesc;
WEB_DESCRIPTOR *last_webdesc;
int top_web_desc;
void init_web( int port )
{
   struct sockaddr_in my_addr;
   char buf[MAX_STRING_LENGTH];
   sprintf( buf, "Web features starting on port: %d", port );
   log_string( buf );
   WEBSERVER_STATUS = TRUE;

   /*
    * Lets clear these out .. --GW 
    */
   first_webdesc = NULL;
   last_webdesc = NULL;
   top_web_desc = 0;
   if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
   {
      perror( "web-socket" );
      exit( 1 );
   }
   my_addr.sin_family = AF_INET;
   my_addr.sin_port = htons( port );
   my_addr.sin_addr.s_addr = htons( INADDR_ANY );
   bzero( &( my_addr.sin_zero ), 8 );
   if( ( bind( sockfd, ( struct sockaddr * )&my_addr, sizeof( struct sockaddr ) ) ) == -1 )

   {
      perror( "web-bind" );
      log_string( "WebServer Disabled." );
      WEBSERVER_STATUS = FALSE;
      return;
   }

   /*
    * Only listen for 5 connects at once, do we really need more? 
    */
   /*
    * Nah .. but thanks for asking! =P *smirk* --GW 
    */
   listen( sockfd, 5 );
}
struct timeval ZERO_TIME = { 0, 0 };
void handle_web( void )
{
   int max_fd;
   WEB_DESCRIPTOR *current, *next;
   fd_set readfds;
   if( WEBSERVER_STATUS == FALSE )
      return;
   FD_ZERO( &readfds );
   FD_SET( sockfd, &readfds );

   /*
    * it *will* be atleast sockfd 
    */
   max_fd = sockfd;

   /*
    * add in all the current web descriptors 
    */
   /*
    * Linked stuff again --GW 
    */
   for( current = first_webdesc; current; current = current->next )

   {
      FD_SET( current->fd, &readfds );
      if( max_fd < current->fd )
         max_fd = current->fd;
   }

   /*
    * Wait for ONE descriptor to have activity 
    */
   select( max_fd + 1, &readfds, NULL, NULL, &ZERO_TIME );
   if( FD_ISSET( sockfd, &readfds ) )
   {

      /*
       * NEW CONNECTION -- INIT & ADD TO LIST 
       */

      /*
       * Ok .. so we dont really need those memory functions
       * included in the original release, a function for 1 line
       * of code .. no. --GW 
       */
      CREATE( current, WEB_DESCRIPTOR, 1 );
      current->sin_size = sizeof( struct sockaddr_in );
      current->request[0] = '\0';
      if( ( current->fd = accept( sockfd, ( struct sockaddr * )&( current->their_addr ), &( current->sin_size ) ) ) == -1 )
      {
         perror( "web-accept" );
         exit( 1 );
      }

      /*
       * Ugh .. lets just use LINK here .. --GW 
       */
      LINK( current, first_webdesc, last_webdesc, next, prev );

      /*
       * END ADDING NEW DESC 
       */
   }

   /*
    * DATA IN! 
    */
   /*
    * Nother change for Linked List stuff --GW 
    */
   for( current = first_webdesc; current; current = current->next )

   {
      if( FD_ISSET( current->fd, &readfds ) )   /* We Got Data! */

      {
         char buf[1024];
         int numbytes;
         if( ( numbytes = read( current->fd, buf, sizeof( buf ) ) ) == -1 )
         {
            perror( "web-read" );
            exit( 1 );
         }
         buf[numbytes] = '\0';
         strcat( current->request, buf );
      }
   }  /* DONE WITH DATA IN */

   /*
    * DATA OUT 
    */
   /*
    * Again .... --GW 
    */
   for( current = first_webdesc; current; current = next )

   {
      next = current->next;
      if( strstr( current->request, "HTTP/1." ) /* 1.x request (vernum on FIRST LINE) */
          && strstr( current->request, ENDREQUEST ) )
         handle_web_request( current );

      else if( !strstr( current->request, "HTTP/1." ) && strchr( current->request, '\n' ) )  /* HTTP/0.9 (no ver number) */
         handle_web_request( current );

      else
      {
         continue;   /* Don't have full request yet! */
      }

      //close(current->fd);
      /*
       * Again, no function needed! 
       * also moved this up to here, were done with it, so
       * nuke away!  --GW 
       */
      UNLINK( current, first_webdesc, last_webdesc, next, prev );
      DISPOSE( current );
   }

   /*
    * Removed a whack of stuff here, we dont need it. --GW 
    */
}  /* END DATA-OUT */


/* Generic Utility Function */
int send_buf( int fd, char *buf, bool filter )
{
   if( filter )

   {
      send( fd, "<CODE>", 6, 0 );
      buf = color_filter( buf );
      buf = text2html( buf );
      send( fd, "</CODE>", 7, 0 );
   }
   return send( fd, buf, strlen( buf ), 0 );
}

void handle_web_request( WEB_DESCRIPTOR * wdesc )
{

   /*
    * process request 
    */
   /*
    * are we using HTTP/1.x? If so, write out header stuff.. 
    */
   if( !strstr( wdesc->request, "GET" ) )

   {
      send_buf( wdesc->fd, "HTTP/1.0 501 Not Implemented", FALSE );
      return;
   }
   else if( strstr( wdesc->request, "HTTP/1." ) )

   {
      send_buf( wdesc->fd, "HTTP/1.0 200 OK\n", FALSE );
      send_buf( wdesc->fd, "Content-type: text/html\n\n", FALSE );
   }

   /*
    * Handle the actual request 
    */
   if( strstr( wdesc->request, "/wholist" ) )

   {
      log_string( "Web Hit: WHOLIST" );
      handle_web_who_request( wdesc );
   }

   else if( strstr( wdesc->request, "/wwwlist" ) )

   {
      log_string( "Web Hit: WWW-WHOLIST" );
      handle_web_who_request( wdesc );
   }

   else if( strstr( wdesc->request, "/wizlist" ) )

   {
      log_string( "Web Hit: WIZ-LIST" );
      handle_web_wizlist_request( wdesc );
   }

   else

   {
      log_string( "Web Hit: INVALID URL" );
      send_buf( wdesc->fd, "Sorry, Wolfpaw Integrated Webserver 1.0 only supports /wholist, /wwwlist and /wizlist", FALSE );
   }
}
void shutdown_web( void )
{
   WEB_DESCRIPTOR *current, *next;

   /*
    * Close All Current Connections 
    */
   /*
    * lets change this around ... blah blah --GW 
    */
   for( current = first_webdesc; current; current = next )

   {
      next = current->next;
      close( current->fd );

      /*
       * Again, no function needed! --GW 
       */
      UNLINK( current, first_webdesc, last_webdesc, next, prev );
      DISPOSE( current );
   }

   /*
    * Stop Listening 
    */
   close( sockfd );
}

void handle_web_who_request( WEB_DESCRIPTOR * wdesc )
{
   FILE *fp;
   char buf[MAX_STRING_LENGTH];
   char *buf2;
   int c;
   int num = 0;

   /*
    * Well .. why have 2 copies of your who? Smaug already supports
    * webwho, however wont give anyone the cgi for it .. so here we go!
    * --GW 
    */

/* Send the Basic Html Config, hard coded background .. sue me. --GW */
   sprintf( buf, "<HTML><HEAD><TITLE>%s -- Who Listing</TITLE></HEAD>\n\r", sysdata.mud_name );
   send_buf( wdesc->fd, buf, FALSE );
   sprintf( buf,
            "<BODY BGCOLOR=*#000000* TEXT=*#F8FFFA* LINK=*#9E9295* VLINK=*#B5B5B5*><CENTER><B>%s Who Listing</B><P>\n\r",
            sysdata.mud_name );
   buf2 = STRALLOC( buf );
   buf2 = parse_quotes( buf2 );
   send_buf( wdesc->fd, buf2, FALSE );
   do_who( NULL, "" );

/*
 * Modified version of Show File, used in here --GW
 */
   if( ( fp = fopen( WHO_FILE, "r" ) ) != NULL )

   {
      while( !feof( fp ) )

      {
         while( ( buf[num] = fgetc( fp ) ) != EOF
                && buf[num] != '\n' && buf[num] != '\r' && num < ( MAX_STRING_LENGTH - 2 ) )
            num++;
         c = fgetc( fp );
         if( ( c != '\n' && c != '\r' ) || c == buf[num] )
            ungetc( c, fp );
         buf[num++] = '\n';
         buf[num++] = '\r';
         buf[num] = '\0';
         if( strlen( buf ) > 32000 )

         {
            bug( "Strlen Greater then 32000: show_file", 0 );
            buf[32000] = '\0';
         }
         num = 0;
         send_buf( wdesc->fd, buf, TRUE );
         send_buf( wdesc->fd, "<BR>", FALSE );  /*Equiv to /n/r --GW */
      }
      fclose( fp );
   }
   return;
}

void handle_web_wwwwho_request( WEB_DESCRIPTOR * wdesc )
{
   FILE *fp;
   char buf[MAX_STRING_LENGTH];
   char *buf2;
   int c;
   int num = 0;

   /*
    * Well .. why have 2 copies of your who? Smaug already supports
    * webwho, however wont give anyone the cgi for it .. so here we go!
    * --GW 
    */
/*sprintf(buf, "<TML><HEAD><TITLE>%s -- WWW-Who Listing</TITLE></HEAD>\n\r",sysdata.mud_name);
send_buf(wdesc->fd, buf , FALSE);
sprintf(buf, "<BODY BGCOLOR=*#000000* TEXT=*#F8FFFA* LINK=*#9E9295* VLINK=*#B5B5B5*><font face=*Arial*><CENTER><B>%s WWW-Who Listing</B></CENTER><P>\n\r", sysdata.mud_name);
buf2 = STRALLOC( buf );
buf2 = parse_quotes(buf2);
send_buf(wdesc->fd,buf2,FALSE);*/
   do_who( NULL, "www" );

/*
 * Modified version of Show File, used in here --GW
 */
   if( ( fp = fopen( WEBWHO_FILE, "r" ) ) != NULL )

   {
      while( !feof( fp ) )

      {
         while( ( buf[num] = fgetc( fp ) ) != EOF
                && buf[num] != '\n' && buf[num] != '\r' && num < ( MAX_STRING_LENGTH - 2 ) )
            num++;
         c = fgetc( fp );
         if( ( c != '\n' && c != '\r' ) || c == buf[num] )
            ungetc( c, fp );
         buf[num++] = '\n';
         buf[num++] = '\r';
         buf[num] = '\0';
         if( strlen( buf ) > 32000 )

         {
            bug( "Strlen Greater then 32000: show_file", 0 );
            buf[32000] = '\0';
         }
         num = 0;
         buf2 = STRALLOC( "<font face=*Arial*>" );
         buf2 = parse_quotes( buf2 );
         send_buf( wdesc->fd, buf2, FALSE );
         send_buf( wdesc->fd, buf, TRUE );
         send_buf( wdesc->fd, "</font>", FALSE );
         send_buf( wdesc->fd, "<BR>", FALSE );  /* Equiv to /n/r --GW */
      }
      fclose( fp );
   }
   return;
}


#define WEBWIZLIST_FILE SYSTEM_DIR "WIZLIST"
void handle_web_wizlist_request( WEB_DESCRIPTOR * wdesc )
{
   FILE *fp;
   char buf[MAX_STRING_LENGTH];
   char *buf2;
   int c;
   int num = 0;

   /*
    * Well .. why have 2 copies of your who? Smaug already supports
    * webwho, however wont give anyone the cgi for it .. so here we go!
    * --GW 
    */
   sprintf( buf, "<HTML><HEAD><TITLE>%s -- Wizlist</TITLE></HEAD>\n\r", sysdata.mud_name );
   send_buf( wdesc->fd, buf, FALSE );
   sprintf( buf,
            "<BODY BGCOLOR=*#000000* TEXT=*#F8FFFA* LINK=*#9E9295* VLINK=*#B5B5B5*><font face=*Arial*><CENTER><B>%s -- Wizlist</B></CENTER><P>\n\r",
            sysdata.mud_name );
   buf2 = STRALLOC( buf );
   buf2 = parse_quotes( buf2 );
   send_buf( wdesc->fd, buf2, FALSE );
   do_who( NULL, "" );

/*
 * Modified version of Show File, used in here --GW
 */
   if( ( fp = fopen( WEBWIZLIST_FILE, "r" ) ) != NULL )

   {
      while( !feof( fp ) )

      {
         while( ( buf[num] = fgetc( fp ) ) != EOF
                && buf[num] != '\n' && buf[num] != '\r' && num < ( MAX_STRING_LENGTH - 2 ) )
            num++;
         c = fgetc( fp );
         if( ( c != '\n' && c != '\r' ) || c == buf[num] )
            ungetc( c, fp );
         buf[num++] = '\n';
         buf[num++] = '\r';
         buf[num] = '\0';
         if( strlen( buf ) > 32000 )

         {
            bug( "Strlen Greater then 32000: show_file", 0 );
            buf[32000] = '\0';
         }
         num = 0;

         /*
          * Lets Center the Who list --GW 
          */
         send_buf( wdesc->fd, "<CENTER>", FALSE );
         send_buf( wdesc->fd, buf, FALSE );
         send_buf( wdesc->fd, "<BR>", FALSE );  /* Equiv to /n/r --GW */
         send_buf( wdesc->fd, "</CENTER>", FALSE );
         send_buf( wdesc->fd, "</font>", FALSE );
      }
      fclose( fp );
   }
   return;
}


/* The Mem functions that did reside here, arnt needed anymore,
    so ill save the real estate. =P --GW */

/* Rip out the Smaug Color Sequences --GW */
/* Hey -- I didnt sayit was perfect.. a hack at most*/
char *color_filter( char *string )
{
   int c;
   char temp[MAX_STRING_LENGTH];
   char *temp2;
   temp2 = string;
   for( c = 0; temp2[c] != '\0'; c++ )

   {

/* First Scan for color .. then others --GW */
      if( temp2[c] == '&' || temp2[c] == '^' )

      {
         temp2[c] = '%';
         temp2[c + 1] = 's';
         sprintf( temp, temp2, "" );
         temp2 = STRALLOC( temp );
         c = -1;  /* found 1 .. start again */
      }
   }
   return temp2;
}


/*
 * Many thanks to Altrag who contributed this function! --GW
 */
char *text2html( const char *ip )
{
   static struct
   {
      const char *text;
      const char *html;
      int tlen, hlen;
   }
   convert_table[] =
   {
      {
      "<", "&lt;"}
      ,
      {
      ">", "&gt;"}
      ,
      {
      "&", "&amp;"}
      ,
      {
      "\"", "&quot;"}
      ,
      {
      " ", "&nbsp;"}
      ,
      {
      NULL, NULL}
   };
   static char buf[MAX_STRING_LENGTH * 2];   /* Safety here .. --GW */
   char *bp = buf;
   int i;
   if( !convert_table[0].tlen )

   {
      for( i = 0; convert_table[i].text; ++i )

      {
         convert_table[i].tlen = strlen( convert_table[i].text );
         convert_table[i].hlen = strlen( convert_table[i].html );
      }
   }
   while( *ip )

   {
      for( i = 0; convert_table[i].text; ++i )
         if( !strncmp( ip, convert_table[i].text, convert_table[i].tlen ) )
            break;
      if( convert_table[i].text )

      {
         strcpy( bp, convert_table[i].html );
         bp += convert_table[i].hlen;
         ip += convert_table[i].tlen;
      }

      else
         *bp++ = *ip++;
   }
   *bp = '\0';
   return buf;
}
char *parse_quotes( char *arg )
{
   int str;
   for( str = 0; arg[str] != '\0'; str++ )

   {
      if( arg[str] == '*' )
         arg[str] = '"';
   }
   return arg;
}
