/************************************************************************/
/*        Acadia code team (C) 2002-2005 Kara Enterprises               */
/************************************************************************/
/*                  Information on Release                              */ 
/************************************************************************/

Version 1.1 build 1

This version has a lot of neet changes. We include a temporary imm character 
to use to set up your character as an administrator level imm. Character name 
is tempimm, password tempimm.

When extracting this distribution, the startup file is meant to be outside 
of the dist folder everything else is in.

*IMPORTANT: This distribution only comes with 3 walkable areas. The limbo
and Dyrthianua's Training Grounds areas have items needed by the code in them
so please do not remove them unless you first remove the codes dependence.
There are some room vnum definitions that will all be the same vnum. This is
to prevent crashes due to the codes need to move your char there. When you 
have areas ready that you want to have the code point to please look at the
ROOM_VNUM_* defines in acadia.h and change them to what you want.

Before compiling the source you will need to change some settings in acadia.h
file. First, the PORTN value needs to be set to your desired port number that
you are planning on using. Second, the values for dbName, dbUser and dbPass
will need to be changed to your MySQL database name, database user and password. 
Bellow is what the code looks like that you need to change.

#define MYSQL_HOST          "localhost"
#define MYSQL_DB            "<dbName>"
#define MYSQL_USERID        "<dbUser>"
#define MYSQL_PASSWD        "<dbPass"

The next step is to create the tables. Below is what you need for the tables.
They are query inputs that will quickly create the tables for you.

CREATE TABLE `tbl_bugs` (
  `index` int(3) NOT NULL auto_increment,
  `vnum` int(11) default NULL,
  `name` text,
  `date` text,
  `what` longtext,
  PRIMARY KEY  (`index`),
  KEY `index` (`index`),
  FULLTEXT KEY `name` (`name`,`what`),
  KEY `index_2` (`index`)
) TYPE=MyISAM;

CREATE TABLE `tbl_changes` (
  `index` int(5) NOT NULL auto_increment,
  `name` text,
  `date` text,
  `what` text,
  PRIMARY KEY  (`index`),
  FULLTEXT KEY `what` (`what`)
) TYPE=MyISAM;

CREATE TABLE `tbl_ilog` (
  `index` int(3) NOT NULL auto_increment,
  `name` text NOT NULL,
  `ctime` text NOT NULL,
  `log` text,
  KEY `index` (`index`),
  FULLTEXT KEY `name` (`name`)
) TYPE=MyISAM;

CREATE TABLE `tbl_nch` (
  `index` int(3) NOT NULL auto_increment,
  `name` text,
  `ip` text,
  `date` text,
  PRIMARY KEY  (`index`),
  FULLTEXT KEY `name` (`name`,`ip`),
  FULLTEXT KEY `date` (`date`)
) TYPE=MyISAM COMMENT='new char records';

CREATE TABLE `tbl_rap` (
  `index` int(3) NOT NULL auto_increment,
  `name` text,
  `IP` text,
  `comment` text,
  PRIMARY KEY  (`index`),
  FULLTEXT KEY `name` (`name`,`IP`,`comment`)
) TYPE=MyISAM;

CREATE TABLE `tbl_register` (
  `index` int(3) NOT NULL auto_increment,
  `IP` text,
  `name` text,
  PRIMARY KEY  (`index`),
  FULLTEXT KEY `name` (`name`),
  FULLTEXT KEY `IP` (`IP`)
) TYPE=MyISAM;


Thank you for chosing the Acadia MUD Code.

Nicole Averety, Main coder
Owner of Kara's Storm
karastorm.org port 4000
http://www.karastorm.org
