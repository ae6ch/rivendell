// revertschema.cpp
//
// Revert Rivendell DB schema
//
//   (C) Copyright 2018-2024 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "rddbmgr.h"

bool MainObject::RevertSchema(int cur_schema,int set_schema,QString *err_msg)
{
  QString sql;
  RDSqlQuery *q; 
  RDSqlQuery *q1; 
  QString tablename;

  //
  // Maintainer's Note:
  //
  // When adding a schema reversion here, be sure also to implement the
  // corresponding update in updateschema.cpp!
  //

  // NEW SCHEMA REVERSIONS GO HERE...

  //
  // Revert 375
  //
  if((cur_schema == 375) && (set_schema < cur_schema))
  {
    //
    // So we don't get truncation errors when modifying the column width
    //
    sql=QString("update `ELR_LINES` ")+
      "set `EXT_EVENT_ID`=substr(`EXT_EVENT_ID`,1,8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `ELR_LINES` ")+
      "modify column `EXT_EVENT_ID` varchar(8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 374
  //
  if((cur_schema == 374) && (set_schema < cur_schema))
  {
    DropColumn("RDAIRPLAY","MESSAGE_WIDGET_URL");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 373
  //
  if((cur_schema == 373) && (set_schema < cur_schema))
  {
    sql=QString("alter table `AUDIO_CARDS` ")+
      "modify column `NAME` varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 372
  //
  if((cur_schema == 372) && (set_schema < cur_schema))
  {
    DropIndex("ISCI_XREFERENCE","FILENAME_IDX");
    DropColumn("REPLICATORS", "PROGRAM_CODE");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 371
  //
  if((cur_schema == 371) && (set_schema < cur_schema))
  {
    DropColumn("DROPBOXES", "UPDATE_METADATA");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 370
  //
  if((cur_schema==370)&&(set_schema<cur_schema)) {
    sql=QString("alter table `SYSTEM` ")+
      "add column `MAX_POST_LENGTH` int unsigned default 10000000 "+
      "after `FIX_DUP_CART_TITLES`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 369
  //
  if((cur_schema==369)&&(set_schema<cur_schema)) {
    DropColumn("FEEDS","CDN_PURGE_PLUGIN_PATH");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 368
  //
  if((cur_schema==368)&&(set_schema<cur_schema)) {
    DropColumn("FEEDS","SHA1_HASH");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 367
  //
  if((cur_schema==367)&&(set_schema<cur_schema)) {
    DropColumn("RDLIBRARY","IS_SINGLETON");
    DropColumn("RDLOGEDIT","IS_SINGLETON");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 366
  //
  if((cur_schema==366)&&(set_schema<cur_schema)) {
    sql=QString("alter table `RDAIRPLAY` ")+
      "modify column `PAUSE_ENABLED` enum('N','Y')";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 365
  //
  if((cur_schema==365)&&(set_schema<cur_schema)) {
    DropColumn("FEED_IMAGES","DATA_SMALL_THUMB");
    DropColumn("FEED_IMAGES","DATA_MID_THUMB");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 364
  //
  if((cur_schema==364)&&(set_schema<cur_schema)) {
    sql=QString("update `IMPORT_TEMPLATES` set ")+
      "`TRANS_TYPE_OFFSET`=NULL,"+
      "`TRANS_TYPE_LENGTH`=NULL,"+
      "`TIME_TYPE_OFFSET`=NULL,"+
      "`TIME_TYPE_LENGTH`=NULL "+
      "where `NAME`='Rivendell Standard Import'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 363
  //
  if((cur_schema==363)&&(set_schema<cur_schema)) {
    DropColumn("IMPORTER_LINES","GRACE_TIME");
    DropColumn("IMPORTER_LINES","TIME_TYPE");
    DropColumn("IMPORTER_LINES","TRANS_TYPE");

    DropColumn("IMPORT_TEMPLATES","TIME_TYPE_LENGTH");
    DropColumn("IMPORT_TEMPLATES","TIME_TYPE_OFFSET");
    DropColumn("IMPORT_TEMPLATES","TRANS_TYPE_LENGTH");
    DropColumn("IMPORT_TEMPLATES","TRANS_TYPE_OFFSET");

    DropColumn("SERVICES","MUS_TIME_TYPE_LENGTH");
    DropColumn("SERVICES","MUS_TIME_TYPE_OFFSET");
    DropColumn("SERVICES","MUS_TRANS_TYPE_LENGTH");
    DropColumn("SERVICES","MUS_TRANS_TYPE_OFFSET");
    DropColumn("SERVICES","BYPASS_MODE");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 362
  //
  if((cur_schema==362)&&(set_schema<cur_schema)) {
    sql=QString("select ")+
      "`ID`,"+          // 00
      "`PASSWORD`,"+    // 01
      "`PASSWORD_2` "+  // 02
      "from `MATRICES`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      if(!q->value(1).isNull()) {
	sql=QString("update `MATRICES` set ")+
	  "`PASSWORD`='"+
	  RDEscapeString(QByteArray::fromBase64(q->value(1).toString().toUtf8()))+"' "+
	  QString::asprintf("where `ID`=%d",q->value(0).toInt());
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      if(!q->value(2).isNull()) {
	sql=QString("update `MATRICES` set ")+
	  "`PASSWORD_2`='"+
	  RDEscapeString(QByteArray::fromBase64(q->value(2).toString().toUtf8()))+"' "+
	  QString::asprintf("where `ID`=%d",q->value(0).toInt());
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
    }
    delete q;
    sql=QString("alter table `MATRICES` ")+
      "modify column `PASSWORD` varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `MATRICES` ")+
      "modify column `PASSWORD_2` varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 361
  //
  if((cur_schema==361)&&(set_schema<cur_schema)) {
    sql=QString("select ")+
      "`ID`,"+        // 00
      "`PASSWORD` "+  // 01
      "from `SWITCHER_NODES`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      if(!q->value(1).isNull()) {
	sql=QString("update `SWITCHER_NODES` set ")+
	  "`PASSWORD`='"+
	  RDEscapeString(QByteArray::fromBase64(q->value(1).toString().toUtf8()))+"' "+
	  QString::asprintf("where `ID`=%d",q->value(0).toInt());
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
    }
    delete q;
    sql=QString("alter table `REPLICATORS` ")+
      "modify column `URL_PASSWORD` varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 360
  //
  if((cur_schema==360)&&(set_schema<cur_schema)) {
    sql=QString("select ")+
      "`NAME`,"+          // 00
      "`URL_PASSWORD` "+  // 01
      "from `REPLICATORS`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      if(!q->value(1).isNull()) {
	sql=QString("update `REPLICATORS` set ")+
	  "`URL_PASSWORD`='"+
	  RDEscapeString(QByteArray::fromBase64(q->value(1).toString().toUtf8()))+"' "+
	  "where `NAME`='"+RDEscapeString(q->value(0).toString())+"'";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
    }
    delete q;
    sql=QString("alter table `REPLICATORS` ")+
      "modify column `URL_PASSWORD` varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 359
  //
  if((cur_schema==359)&&(set_schema<cur_schema)) {
    sql=QString("select ")+
      "`LOGIN_NAME`,"+  // 00
      "`PASSWORD` "+    // 01
      "from `USERS`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("update `USERS` set ");
      if(q->value(1).isNull()) {
	sql+="`PASSWORD`='' ";
      }
      else {
	sql+="`PASSWORD`='"+
	  RDEscapeString(QByteArray::fromBase64(q->value(1).toString().toUtf8()))+"' ";
      }
      sql+="where `LOGIN_NAME`='"+RDEscapeString(q->value(0).toString())+"'";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;
    sql=QString("alter table `USERS` ")+
      "modify column `PASSWORD` varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 358
  //
  if((cur_schema==358)&&(set_schema<cur_schema)) {
    sql=QString("select ")+
      "`ID`,"+           // 00
      "`URL_PASSWORD` "  // 01
      "from `RECORDINGS`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("update `RECORDINGS` set ")+
	"`URL_PASSWORD`='"+
	RDEscapeString(QByteArray::fromBase64(q->value(1).toString().toUtf8()))+"' "+
	QString::asprintf("where `ID`=%u",q->value(0).toUInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;
    sql=QString("alter table `RECORDINGS` ")+
      "modify column `URL_PASSWORD` varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 357
  //
  if((cur_schema==357)&&(set_schema<cur_schema)) {
    sql=QString("alter table `STATIONS` ")+
      "add column `EDITOR_PATH` varchar(191) after `STARTUP_CART`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 356
  //
  if((cur_schema==356)&&(set_schema<cur_schema)) {
    // Nothing to do here!

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 355
  //
  if((cur_schema==355)&&(set_schema<cur_schema)) {
    DropColumn("RDAIRPLAY","LOGO_PATH");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 354
  //
  if((cur_schema==354)&&(set_schema<cur_schema)) {
    sql=QString("select ")+
      "`NAME`,"+                        // 00
      "`INCLUDE_MUS_IMPORT_MARKERS`,"+  // 01
      "`INCLUDE_TFC_IMPORT_MARKERS` "+  // 02
      "from `SERVICES`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("update `SERVICES` set ")+
	"`INCLUDE_IMPORT_MARKERS`='"+
	RDYesNo((q->value(1).toString()=="Y")||(q->value(2).toString()=="Y"))+
	"' where "+
	"`NAME`='"+RDEscapeString(q->value(0).toString())+"'";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;
    DropColumn("SERVICES","INCLUDE_MUS_IMPORT_MARKERS");
    DropColumn("SERVICES","INCLUDE_TFC_IMPORT_MARKERS");

    sql=QString("select ")+
      "`NAME`,"+                        // 00
      "`INCLUDE_MUS_IMPORT_MARKERS`,"+  // 01
      "`INCLUDE_TFC_IMPORT_MARKERS` "+  // 02
      "from `LOGS`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("update `LOGS` set ")+
	"`INCLUDE_IMPORT_MARKERS`='"+
	RDYesNo((q->value(1).toString()=="Y")||(q->value(2).toString()=="Y"))+
	"' where "+
	"`NAME`='"+RDEscapeString(q->value(0).toString())+"'";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;
    DropColumn("LOGS","INCLUDE_MUS_IMPORT_MARKERS");
    DropColumn("LOGS","INCLUDE_TFC_IMPORT_MARKERS");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 353
  //
  if((cur_schema==353)&&(set_schema<cur_schema)) {
    DropColumn("SYSTEM","REALM_NAME");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 352
  //
  if((cur_schema==352)&&(set_schema<cur_schema)) {
    DropColumn("SYSTEM","SHOW_TWELVE_HOUR_TIME");
    sql=QString("alter table `SYSTEM` ")+
      "add column `TIME_FORMAT` varchar(32) not null default 'hh:mm:ss' "+
      "after `SHORT_DATE_FORMAT`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 351
  //
  if((cur_schema==351)&&(set_schema<cur_schema)) {
    DropColumn("SYSTEM","TIME_FORMAT");
    DropColumn("SYSTEM","SHORT_DATE_FORMAT");
    DropColumn("SYSTEM","LONG_DATE_FORMAT");
    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 350
  //
  if((cur_schema==350)&&(set_schema<cur_schema)) {
    DropColumn("AUDIO_INPUTS","LABEL");
    DropColumn("AUDIO_OUTPUTS","LABEL");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 349
  //
  if((cur_schema==349)&&(set_schema<cur_schema)) {
    sql=QString("alter table `RDAIRPLAY` ")+
      "modify column EXIT_PASSWORD varchar(41)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 348
  //
  if((cur_schema==348)&&(set_schema<cur_schema)) {
    //
    // Remove "out-of-bounds" data for SoundPanel buttons so they don't
    // cause segfaults in "naive" v3.x code.
    //
    sql=QString("delete from `PANELS` where ")+
      "`ROW_NO`>=5 || `COLUMN_NO`>=5";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("delete from `EXTENDED_PANELS` where ")+
      "`ROW_NO`>=7 || `COLUMN_NO`>=9";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  /************************************************************************
   * Boundary between v3.x and v4.x
   ************************************************************************/

  //
  // Revert 347
  //
  if((cur_schema==347)&&(set_schema<cur_schema)) {
    DropColumn("STACK_LINES","TITLE");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 346
  //
  if((cur_schema==346)&&(set_schema<cur_schema)) {
    DropColumn("USERS","ADMIN_RSS_PRIV");

    sql=QString("alter table `USERS` add column ")+
      "`ADMIN_USERS_PRIV` enum('N','Y') not null default 'N' "+
      "after `ADMIN_CONFIG_PRIV`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 345
  //
  if((cur_schema==345)&&(set_schema<cur_schema)) {
    DropColumn("DROPBOXES","SEND_EMAIL");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 344
  //
  if((cur_schema==344)&&(set_schema<cur_schema)) {
    DropColumn("GROUPS","NOTIFY_EMAIL_ADDRESS");
    DropColumn("SYSTEM","ORIGIN_EMAIL_ADDRESS");


    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 343
  //
  if((cur_schema==343)&&(set_schema<cur_schema)) {
    DropTable("ENCODER_PRESETS");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 342
  //
  if((cur_schema==342)&&(set_schema<cur_schema)) {
    DropIndex("GPIO_EVENTS","EVENT_DATETIME_IDX");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 341
  //
  if((cur_schema==341)&&(set_schema<cur_schema)) {
    DropColumn("IMPORTER_LINES","FILE_LINE");
    DropColumn("IMPORTER_LINES","LINK_START_TIME");
    DropColumn("IMPORTER_LINES","LINK_LENGTH");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 340
  //
  if((cur_schema==340)&&(set_schema<cur_schema)) {
    sql=QString("alter table `IMPORTER_LINES` ")+
      "add column `INSERT_BREAK` enum('N','Y') default 'N' after `LENGTH`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `IMPORTER_LINES` ")+
      "add column `INSERT_TRACK` enum('N','Y') default 'N' after `INSERT_BREAK`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `IMPORTER_LINES` ")+
      "add column `INSERT_FIRST` int unsigned default 0 after `INSERT_TRACK`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `IMPORTER_LINES` ")+
      "add column `TRACK_STRING` varchar(191) after `INSERT_FIRST`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `IMPORTER_LINES` ")+
      "add column `LINK_START_TIME` time after `EXT_CART_NAME`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `IMPORTER_LINES` ")+
      "add column `LINK_LENGTH` int after `LINK_START_TIME`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `IMPORTER_LINES` ")+
      "modify column `START_HOUR` int not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `IMPORTER_LINES` ")+
      "modify column `START_SECS` int not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    DropColumn("IMPORTER_LINES","TYPE");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 339
  //
  if((cur_schema==339)&&(set_schema<cur_schema)) {
    DropColumn("SERVICES","SUB_EVENT_INHERITANCE");
    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 338
  //
  if((cur_schema==338)&&(set_schema<cur_schema)) {
    sql=QString("alter table `FEEDS` modify column ")+
      "`CHANNEL_LANGUAGE` varchar(5) default 'en-us'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 337
  //
  if((cur_schema==337)&&(set_schema<cur_schema)) {
    DropIndex("PODCASTS","ITEM_IMAGE_ID_IDX");
    DropIndex("FEEDS","DEFAULT_ITEM_IMAGE_ID_IDX");
    DropIndex("FEEDS","CHANNEL_IMAGE_ID_IDX");
    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 336
  //
  if((cur_schema==336)&&(set_schema<cur_schema)) {
    sql=QString("alter table `FEED_IMAGES` modify column ")+
      "`DATA` mediumblob not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 335
  //
  if((cur_schema==335)&&(set_schema<cur_schema)) {
    DropIndex("PODCASTS","SHA1_HASH_IDX");
    DropColumn("PODCASTS","SHA1_HASH");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 334
  //
  if((cur_schema==334)&&(set_schema<cur_schema)) {
    sql=QString("select ")+
      "`KEY_NAME`,"+        // 00
      "`PURGE_PASSWORD` "+  // 01
      "from `FEEDS`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      QString password(QByteArray::fromBase64(q->value(1).toString().toUtf8()));
      sql=QString("update `FEEDS` set ")+
	"`PURGE_PASSWORD`='"+RDEscapeString(password)+"' where "+
	"`KEY_NAME`='"+RDEscapeString(q->value(0).toString())+"'";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;
    sql=QString("alter table `FEEDS` modify column ")+
      "`PURGE_PASSWORD` varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

 
  //
  // Revert 333
  //
  if((cur_schema==333)&&(set_schema<cur_schema)) {
    DropColumn("STATIONS","SSH_IDENTITY_FILE");
    DropColumn("RECORDINGS","URL_USE_ID_FILE");
    DropColumn("FEEDS","PURGE_USE_ID_FILE");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 332
  //
  if((cur_schema==332)&&(set_schema<cur_schema)) {
    DropColumn("FEEDS","CHANNEL_AUTHOR_IS_DEFAULT");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 331
  //
  if((cur_schema==331)&&(set_schema<cur_schema)) {
    sql=QString("create table if not exists `ENCODER_CHANNELS` (")+
      "`ID` int not null auto_increment primary key,"+
      "`ENCODER_ID` int not null,"+
      "`CHANNELS` int not null,"+
      "index `ENCODER_ID_IDX` (`ENCODER_ID`))"+
      " charset utf8mb4 collate "+db_config->mysqlCollation()+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists `ENCODER_BITRATES` (")+
      "`ID` int not null auto_increment primary key,"+
      "`ENCODER_ID` int not null,"+
      "`BITRATES` int not null,"+
      "index `ENCODER_ID_IDX` (`ENCODER_ID`))"+
      " charset utf8mb4 collate "+db_config->mysqlCollation()+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists `ENCODER_SAMPLERATES` (")+
      "`ID` int not null auto_increment primary key,"+
      "`ENCODER_ID` int not null,"+
      "`SAMPLERATES` int not null,"+
      "index `ENCODER_ID_IDX` (`ENCODER_ID`))"+
      " charset utf8mb4 collate "+db_config->mysqlCollation()+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists `ENCODERS` (")+
      "`ID` int not null auto_increment primary key,"+
      "`NAME` char(32) not null,"+
      "`STATION_NAME` char(64),"+
      "`COMMAND_LINE` char(255),"+
      "`DEFAULT_EXTENSION` char(16),"+
      "index `NAME_IDX` (`NAME`,`STATION_NAME`))"+
      " charset utf8mb4 collate "+db_config->mysqlCollation()+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    // Ensure that dynamic format IDs start after 100
    sql=QString("insert into `ENCODERS` set `ID`=100,`NAME`='dummy'");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("delete from `ENCODERS` where `ID`=100");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 330
  //
  if((cur_schema==330)&&(set_schema<cur_schema)) {
    DropColumn("PODCASTS","ORIGIN_STATION");
    DropColumn("PODCASTS","ORIGIN_LOGIN_NAME");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 329
  //
  if((cur_schema==329)&&(set_schema<cur_schema)) {
    sql=QString("alter table `PODCASTS` ")+
      "add column `SHELF_LIFE` int after `AUDIO_TIME`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select ")+
      "`ID`,"+                   // 00
      "`ORIGIN_DATETIME`,"+      // 01
      "`EXPIRATION_DATETIME` "+  // 02
      "from `PODCASTS`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      if(q->value(2).isNull()) {
	sql=QString("update `PODCASTS` set ")+
	  "`SHELF_LIFE`=0 where "+
	  QString::asprintf("`ID`=%u",q->value(0).toUInt());
      }
      else {
	sql=QString("update `PODCASTS` set ")+
	  QString::asprintf("`SHELF_LIFE`=%lld where ",
			    q->value(1).toDateTime().
			    daysTo(q->value(2).toDateTime()))+
	  QString::asprintf("`ID`=%u",q->value(0).toUInt());
      }
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    DropColumn("PODCASTS","EXPIRATION_DATETIME");
    DropColumn("SYSTEM","RSS_PROCESSOR_STATION");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 328
  //
  if((cur_schema==328)&&(set_schema<cur_schema)) {
    sql=QString("create table if not exists `CAST_DOWNLOADS` (")+
      "`ID` int unsigned not null auto_increment primary key,"+
      "`FEED_KEY_NAME` varchar(8) not null,"+
      "`CAST_ID` int unsigned not null,"+
      "`ACCESS_DATE` date not null,"+
      "`ACCESS_COUNT` int unsigned not null default 0,"+
      "unique index `KEY_NAME_CAST_ID_DATE_IDX` (`FEED_KEY_NAME`,`CAST_ID`,`ACCESS_DATE`))"+
      " charset utf8mb4 collate "+db_config->mysqlCollation()+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `FEEDS` add column ")+
      "`AUDIENCE_METRICS` enum('N','Y') default 'N' after `IS_SUPERFEED`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("update `FEEDS` set `AUDIENCE_METRICS`='Y'");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `FEEDS` add column ")+
      "`KEEP_METADATA` enum('N','Y') default 'Y' after `ENABLE_AUTOPOST`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `FEEDS` add column ")+
      "`REDIRECT_PATH` varchar(191) after `NORMALIZE_LEVEL`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `FEEDS` add column ")+
      "`MEDIA_LINK_MODE` int default 0 after `REDIRECT_PATH`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }


    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 327
  //
  if((cur_schema==327)&&(set_schema<cur_schema)) {
    DropColumn("FEEDS","CHANNEL_SUB_CATEGORY");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 326
  //
  if((cur_schema==326)&&(set_schema<cur_schema)) {
    DropColumn("PODCASTS","ITEM_EXPLICIT");
    DropColumn("FEEDS","CHANNEL_EXPLICIT");
    DropColumn("FEEDS","CHANNEL_OWNER_EMAIL");
    DropColumn("FEEDS","CHANNEL_OWNER_NAME");
    DropColumn("FEEDS","CHANNEL_AUTHOR");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 325
  //
  if((cur_schema==325)&&(set_schema<cur_schema)) {
    sql=QString("create table `RSS_SCHEMAS` (")+
      "`ID` int unsigned primary key,"+
      "`NAME` varchar(64) unique not null,"+
      "`HEADER_XML` text,"+
      "`CHANNEL_XML` text,"+
      "`ITEM_XML` text,"+
      "index `NAME_IDX` (`NAME`)) "+
      " charset utf8mb4 collate "+db_config->mysqlCollation()+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 324
  //
  if((cur_schema==324)&&(set_schema<cur_schema)) {
    DropColumn("PODCASTS","ITEM_IMAGE_ID");
    DropColumn("FEEDS","DEFAULT_ITEM_IMAGE_ID");
    DropColumn("FEEDS","CHANNEL_IMAGE_ID");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 323
  //
  if((cur_schema==323)&&(set_schema<cur_schema)) {
    DropTable("FEED_IMAGES");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 322
  //
  if((cur_schema==322)&&(set_schema<cur_schema)) {
    sql=QString("select `ID`,`RSS_SCHEMA` from `FEEDS` where `RSS_SCHEMA`!=0");
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("select ")+
	"`HEADER_XML`,"+   // 00
	"`CHANNEL_XML`,"+  // 01
	"`ITEM_XML` "+     // 02
	"from `RSS_SCHEMAS` where "+
	QString::asprintf("ID=%u",q->value(1).toUInt());
      q1=new RDSqlQuery(sql);
      if(q1->first()) {
	QString("update `FEEDS` set ")+
	  "`HEADER_XML`='"+RDEscapeString(q1->value(0).toString())+"',"+
	  "`CHANNEL_XML`='"+RDEscapeString(q1->value(1).toString())+"',"+
	  "`ITEM_XML`='"+RDEscapeString(q1->value(2).toString())+"' "+
	  "where "+
	  QString::asprintf("`ID`=%u",q->value(0).toUInt());
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }
    delete q;
    DropColumn("FEEDS","RSS_SCHEMA");
    DropTable("RSS_SCHEMAS");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 321
  //
  if((cur_schema==321)&&(set_schema<cur_schema)) {
    DropColumn("FEEDS","CHANNEL_EDITOR");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 320
  //
  if((cur_schema==320)&&(set_schema<cur_schema)) {
    DropColumn("USERS","EMAIL_ADDRESS");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 319
  //
  if((cur_schema==319)&&(set_schema<cur_schema)) {
    DropColumn("FEEDS","AUDIENCE_METRICS");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 318
  //
  if((cur_schema==318)&&(set_schema<cur_schema)) {
    DropTable("SUPERFEED_MAPS");
    DropIndex("FEEDS","IS_SUPERFEED_IDX");
    DropColumn("FEEDS","IS_SUPERFEED");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 317
  //
  if((cur_schema==317)&&(set_schema<cur_schema)) {
    DropIndex("STACK_SCHED_CODES","STACK_LINES_ID_IDX");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 316
  //
  if((cur_schema==316)&&(set_schema<cur_schema)) {
    sql=QString("alter table `EVENTS` add column `PROPERTIES` varchar(64) ")+
      "after `NAME`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 315
  //
  if((cur_schema==315)&&(set_schema<cur_schema)) {
    sql=QString("alter table `EVENTS` add column `POST_POINT` enum('N','Y') ")+
      "default 'N' after `GRACE_TIME`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `LOG_LINES` add column `POST_POINT` enum('N','Y') ")+
      "default 'N' after `TIME_TYPE`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 314
  //
  if((cur_schema==314)&&(set_schema<cur_schema)) {
    DropColumn("STATIONS","BROWSER_PATH");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 313
  //
  if((cur_schema==313)&&(set_schema<cur_schema)) {
    DropColumn("CUTS","RELEASE_MBID");
    DropColumn("CUTS","RECORDING_MBID");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 312
  //
  if((cur_schema==312)&&(set_schema<cur_schema)) {
    DropColumn("RDLIBRARY","MB_SERVER");
    DropColumn("RDLIBRARY","CD_SERVER_TYPE");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 311
  //
  if((cur_schema==311)&&(set_schema<cur_schema)) {
    DropColumn("RDLOGEDIT","WAVEFORM_CAPTION");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 310
  //
  if((cur_schema==310)&&(set_schema<cur_schema)) {
    DropColumn("CART","MINIMUM_TALK_LENGTH");
    DropColumn("CART","MAXIMUM_TALK_LENGTH");

    DropTable("NEXUS_FIELDS",err_msg);
    DropTable("NEXUS_QUEUE",err_msg);
    DropTable("NEXUS_SERVER",err_msg);
    DropTable("NEXUS_STATIONS",err_msg);

    sql=QString("delete from `IMPORT_TEMPLATES` where ")+
      "`NAME`='MusicMaster Nexus'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 309
  //
  if((cur_schema==309)&&(set_schema<cur_schema)) {
    sql=QString("alter table `DROPBOXES` drop column `LOG_TO_SYSLOG`");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 308
  //
  if((cur_schema==308)&&(set_schema<cur_schema)) {
    sql=QString("alter table `STACK_LINES` add column ")+
      "`SCHED_CODES` varchar(191) not null after `ARTIST`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    QString codes="";
    sql=QString("select ")+
      "`ID` "  // 00
      "from `STACK_LINES`";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("select ")+
	"`SCHED_CODE` "      // 00
	"from `STACK_SCHED_CODES` where "+
	QString::asprintf("`STACK_LINES_ID`=%u",q->value(0).toUInt());
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	codes+=q1->value(0).toString();
	while((codes.length()%10)!=0) {
	  codes+=" ";
	}
      }
      delete q1;
      codes+=".";
      sql=QString("update `STACK_LINES` set ")+
	"`SCHED_CODES`='"+RDEscapeString(codes)+"' where "+
	QString::asprintf("`ID`=%u",q->value(0).toUInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
      codes="";
    }
    delete q;
    DropTable("STACK_SCHED_CODES",err_msg);

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 307
  //
  if((cur_schema==307)&&(set_schema<cur_schema)) {
    DropColumn("EVENTS","ARTIST_SEP");
    sql="alter table `EVENTS` modify column `TITLE_SEP` int(10) unsigned";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 306
  //
  if((cur_schema==306)&&(set_schema<cur_schema)) {
    sql=QString("alter table `RDLIBRARY` drop index `STATION_IDX`");
    RDSqlQuery::apply(sql);

    sql=QString("alter table `RDLIBRARY` add column ")+
      "`INSTANCE` int unsigned not null after `STATION`";
    RDSqlQuery::apply(sql);

    sql=QString("create index `STATION_IDX` on `RDLIBRARY` (`STATION`,`INSTANCE`)"); 
    RDSqlQuery::apply(sql);

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 305
  //
  if((cur_schema==305)&&(set_schema<cur_schema)) {
    sql=QString("alter table `LOG_MACHINES` add column ")+
      "`UDP_ADDR` varchar(191) after `NEXT_CART`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `LOG_MACHINES` add column ")+
      "`UDP_PORT` int unsigned after `UDP_ADDR`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `LOG_MACHINES` add column ")+
      "`UDP_STRING` varchar(191) after `UDP_PORT`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `LOG_MACHINES` add column ")+
      "`LOG_RML` varchar(191) after `UDP_STRING`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists `NOWNEXT_PLUGINS` (")+
      "`ID` int auto_increment not null primary key,"+
      "`STATION_NAME` varchar(64) not null,"+
      "`LOG_MACHINE` int unsigned not null default 0,"+
      "`PLUGIN_PATH` varchar(191),"+
      "`PLUGIN_ARG` varchar(101),"+
      "index `STATION_NAME_IDX` (`STATION_NAME`))"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 304
  //
  if((cur_schema==304)&&(set_schema<cur_schema)) {
    DropColumn("PYPAD_INSTANCES","ERROR_TEXT");
    DropColumn("PYPAD_INSTANCES","ERROR_CODE");
    DropColumn("PYPAD_INSTANCES","IS_RUNNING");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 303
  //
  if((cur_schema==303)&&(set_schema<cur_schema)) {
    DropTable("PYPAD_INSTANCES",err_msg);

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 302
  //
  if((cur_schema==302)&&(set_schema<cur_schema)) {
    DropColumn("STATIONS","REPORT_EDITOR_PATH");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 301
  //
  if((cur_schema==301)&&(set_schema<cur_schema)) {
    DropColumn("LOGS","INCLUDE_IMPORT_MARKERS");
    DropColumn("SERVICES","INCLUDE_IMPORT_MARKERS");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 300
  //
  if((cur_schema==300)&&(set_schema<cur_schema)) {
    sql=QString("alter table `SERVICES` add column `TFC_WIN_PATH` varchar(191) ")+
      "after `TFC_PREIMPORT_CMD`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` add column `TFC_WIN_PREIMPORT_CMD` text ")+
      "after `TFC_WIN_PATH`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `SERVICES` add column `MUS_WIN_PATH` varchar(191) ")+
      "after `MUS_PREIMPORT_CMD`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` add column `MUS_WIN_PREIMPORT_CMD` text ")+
      "after `MUS_WIN_PATH`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 299
  //
  if((cur_schema==299)&&(set_schema<cur_schema)) {

    QString schedcode;

    q=new RDSqlQuery("alter table `CART` add column `SCHED_CODES` varchar( 255 ) NULL default '.' after `MACROS`",false);
    delete q;

    q=new RDSqlQuery("select `CART_NUMBER`,`SCHED_CODE` from `CART_SCHED_CODES` order by `CART_NUMBER`",false);
    while(q->next()) {
      schedcode=QString::asprintf("%-11s",
				  q->value(1).toString().toUtf8().constData());
      q1=new RDSqlQuery(QString::asprintf("update `CART` set `SCHED_CODES`=CONCAT('%s',`SCHED_CODES`) where `NUMBER`=%d",schedcode.toUtf8().constData(),
					  q->value(0).toUInt()),false);
      delete q1;
    }
    delete q;

    q=new RDSqlQuery("drop table `CART_SCHED_CODES`",false);
    delete q;

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 298
  //
  if((cur_schema==298)&&(set_schema<cur_schema)) {
    sql=QString("alter table `RDLOGEDIT` ")+
      "modify column `INPUT_CARD` int(11) default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDLOGEDIT` ")+
      "modify column `OUTPUT_CARD` int(11) default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Maintainer's Note:
  //
  // All tables created above this point should use a character set
  // of 'utf8mb4' and the collation returned by 'RDConfig::mysqlCollation()'.
  //

  //
  // Revert 297
  //
  if((cur_schema==297)&&(set_schema<cur_schema)) {
    ModifyCharset("latin1","latin1_swedish_ci");

    sql=QString("alter table `AUDIO_CARDS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `AUDIO_CARDS` ")+
      "modify column `NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `AUDIO_INPUTS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `AUDIO_OUTPUTS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `AUDIO_PERMS` ")+
      "modify column `GROUP_NAME` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `AUDIO_PERMS` ")+
      "modify column `SERVICE_NAME` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `AUTOFILLS` ")+
      "modify column `SERVICE` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `AUX_METADATA` ")+
      "modify column `VAR_NAME` char(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `AUX_METADATA` ")+
      "modify column `CAPTION` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `CART` ")+
      "modify column `GROUP_NAME` char(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CART` ")+
      "modify column `TITLE` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CART` ")+
      "modify column `ARTIST` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CART` ")+
      "modify column `ALBUM` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CART` ")+
      "modify column `CONDUCTOR` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CART` ")+
      "modify column `LABEL` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CART` ")+
      "modify column `CLIENT` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CART` ")+
      "modify column `AGENCY` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CART` ")+
      "modify column `PUBLISHER` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CART` ")+
      "modify column `COMPOSER` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CART` ")+
      "modify column `USER_DEFINED` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CART` ")+
      "modify column `SONG_ID` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CART` ")+
      "modify column `OWNER` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CART` ")+
      "modify column `PENDING_STATION` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `CARTSLOTS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CARTSLOTS` ")+
      "modify column `SERVICE_NAME` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `CAST_DOWNLOADS` ")+
      "modify column `FEED_KEY_NAME` char(8) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `CLIPBOARD` ")+
      "modify column `CUT_NAME` char(12) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CLIPBOARD` ")+
      "modify column `DESCRIPTION` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CLIPBOARD` ")+
      "modify column `OUTCUE` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CLIPBOARD` ")+
      "modify column `ORIGIN_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `CLOCKS` ")+
      "modify column `NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CLOCKS` ")+
      "modify column `SHORT_NAME` char(8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CLOCKS` ")+
      "modify column `COLOR` char(7)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CLOCKS` ")+
      "modify column `REMARKS` text";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `CLOCK_LINES` ")+
      "modify column `CLOCK_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CLOCK_LINES` ")+
      "modify column `EVENT_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `CLOCK_PERMS` ")+
      "modify column `CLOCK_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CLOCK_PERMS` ")+
      "modify column `SERVICE_NAME` char(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `CUTS` ")+
      "modify column `CUT_NAME` char(12) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CUTS` ")+
      "modify column `DESCRIPTION` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CUTS` ")+
      "modify column `OUTCUE` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CUTS` ")+
      "modify column `ISRC` char(12)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CUTS` ")+
      "modify column `ISCI` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CUTS` ")+
      "modify column `SHA1_HASH` char(40)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CUTS` ")+
      "modify column `ORIGIN_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CUTS` ")+
      "modify column `ORIGIN_LOGIN_NAME` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `CUTS` ")+
      "modify column `SOURCE_HOSTNAME` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `CUT_EVENTS` ")+
      "modify column `CUT_NAME` char(12) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `DECKS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `DECKS` ")+
      "modify column `SWITCH_STATION` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `DECK_EVENTS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `DROPBOXES` ")+
      "modify column `STATION_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `DROPBOXES` ")+
      "modify column `GROUP_NAME` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `DROPBOXES` ")+
      "modify column `PATH` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `DROPBOXES` ")+
      "modify column `METADATA_PATTERN` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `DROPBOXES` ")+
      "modify column `LOG_PATH` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `DROPBOXES` ")+
      "modify column `SET_USER_DEFINED` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `DROPBOX_PATHS` ")+
      "modify column `FILE_PATH` char(255) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `DROPBOX_SCHED_CODES` ")+
      "modify column `SCHED_CODE` char(11) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `ELR_LINES` ")+
      "modify column `SERVICE_NAME` char(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `LOG_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `TITLE` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `ARTIST` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `PUBLISHER` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `COMPOSER` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `USER_DEFINED` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `SONG_ID` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `ALBUM` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `LABEL` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `CONDUCTOR` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `DESCRIPTION` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `OUTCUE` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `ISRC` char(12)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `ISCI` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `STATION_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `EXT_CART_NAME` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `EXT_DATA` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `EXT_EVENT_ID` char(8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ELR_LINES` ")+
      "modify column `EXT_ANNC_TYPE` char(8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `ENCODERS` ")+
      "modify column `NAME` char(32) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ENCODERS` ")+
      "modify column `STATION_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ENCODERS` ")+
      "modify column `COMMAND_LINE` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ENCODERS` ")+
      "modify column `DEFAULT_EXTENSION` char(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `EVENTS` ")+
      "modify column `NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `EVENTS` ")+
      "modify column `PROPERTIES` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `EVENTS` ")+
      "modify column `DISPLAY_TEXT` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `EVENTS` ")+
      "modify column `NOTE_TEXT` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `EVENTS` ")+
      "modify column `COLOR` char(7)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `EVENTS` ")+
      "modify column `SCHED_GROUP` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `EVENTS` ")+
      "modify column `HAVE_CODE` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `EVENTS` ")+
      "modify column `HAVE_CODE2` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `EVENTS` ")+
      "modify column `NESTED_EVENT` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `EVENTS` ")+
      "modify column `REMARKS` text";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `EVENT_LINES` ")+
      "modify column `EVENT_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `EVENT_LINES` ")+
      "modify column `MARKER_COMMENT` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `EVENT_PERMS` ")+
      "modify column `EVENT_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `EVENT_PERMS` ")+
      "modify column `SERVICE_NAME` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `EXTENDED_PANELS` ")+
      "modify column `OWNER` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `EXTENDED_PANELS` ")+
      "modify column `LABEL` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `EXTENDED_PANELS` ")+
      "modify column `DEFAULT_COLOR` char(7)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `EXTENDED_PANEL_NAMES` ")+
      "modify column `OWNER` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `EXTENDED_PANEL_NAMES` ")+
      "modify column `NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `FEEDS` ")+
      "modify column `KEY_NAME` char(8) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `FEEDS` ")+
      "modify column `CHANNEL_TITLE` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `FEEDS` ")+
      "modify column `CHANNEL_DESCRIPTION` text";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `FEEDS` ")+
      "modify column `CHANNEL_CATEGORY` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `FEEDS` ")+
      "modify column `CHANNEL_LINK` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `FEEDS` ")+
      "modify column `CHANNEL_COPYRIGHT` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `FEEDS` ")+
      "modify column `CHANNEL_WEBMASTER` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `FEEDS` ")+
      "modify column `CHANNEL_LANGUAGE` char(5) default 'en-us'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `FEEDS` ")+
      "modify column `BASE_URL` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `FEEDS` ")+
      "modify column `BASE_PREAMBLE` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `FEEDS` ")+
      "modify column `PURGE_URL` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `FEEDS` ")+
      "modify column `PURGE_USERNAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `FEEDS` ")+
      "modify column `PURGE_PASSWORD` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `FEEDS` ")+
      "modify column `UPLOAD_EXTENSION` char(16) default 'mp3'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `FEEDS` ")+
      "modify column `REDIRECT_PATH` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `FEED_PERMS` ")+
      "modify column `USER_NAME` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `FEED_PERMS` ")+
      "modify column `KEY_NAME` char(8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `GPIO_EVENTS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `GPIS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `GPOS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `GROUPS` ")+
      "modify column `NAME` char(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `GROUPS` ")+
      "modify column `DESCRIPTION` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `GROUPS` ")+
      "modify column `DEFAULT_TITLE` char(255) default 'Imported from %f.%e'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `GROUPS` ")+
      "modify column `COLOR` char(7)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `HOSTVARS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `HOSTVARS` ")+
      "modify column `NAME` char(32) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `HOSTVARS` ")+
      "modify column `VARVALUE` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `HOSTVARS` ")+
      "modify column `REMARK` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `IMPORTER_LINES` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `IMPORTER_LINES` ")+
      "modify column `TITLE` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `IMPORTER_LINES` ")+
      "modify column `TRACK_STRING` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `IMPORTER_LINES` ")+
      "modify column `EXT_DATA` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `IMPORTER_LINES` ")+
      "modify column `EXT_EVENT_ID` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `IMPORTER_LINES` ")+
      "modify column `EXT_ANNC_TYPE` char(8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `IMPORTER_LINES` ")+
      "modify column `EXT_CART_NAME` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `IMPORT_TEMPLATES` ")+
      "modify column `NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `INPUTS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `INPUTS` ")+
      "modify column `NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `INPUTS` ")+
      "modify column `FEED_NAME` char(8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `INPUTS` ")+
      "modify column `NODE_HOSTNAME` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `ISCI_XREFERENCE` ")+
      "modify column `ISCI` char(32) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ISCI_XREFERENCE` ")+
      "modify column `FILENAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ISCI_XREFERENCE` ")+
      "modify column `TYPE` char(1) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ISCI_XREFERENCE` ")+
      "modify column `ADVERTISER_NAME` char(30)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ISCI_XREFERENCE` ")+
      "modify column `PRODUCT_NAME` char(35)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ISCI_XREFERENCE` ")+
      "modify column `CREATIVE_TITLE` char(30)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `ISCI_XREFERENCE` ")+
      "modify column `REGION_NAME` char(80)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `JACK_CLIENTS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `JACK_CLIENTS` ")+
      "modify column `DESCRIPTION` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `JACK_CLIENTS` ")+
      "modify column `COMMAND_LINE` text";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `LIVEWIRE_GPIO_SLOTS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LIVEWIRE_GPIO_SLOTS` ")+
      "modify column `IP_ADDRESS` char(15)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `LOGS` ")+
      "modify column `NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOGS` ")+
      "modify column `SERVICE` char(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOGS` ")+
      "modify column `DESCRIPTION` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOGS` ")+
      "modify column `ORIGIN_USER` char(255) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOGS` ")+
      "modify column `LOCK_USER_NAME` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOGS` ")+
      "modify column `LOCK_STATION_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOGS` ")+
      "modify column `LOCK_IPV4_ADDRESS` char(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOGS` ")+
      "modify column `LOCK_GUID` char(82)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `LOG_LINES` ")+
      "modify column `LOG_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOG_LINES` ")+
      "modify column `COMMENT` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOG_LINES` ")+
      "modify column `LABEL` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOG_LINES` ")+
      "modify column `ORIGIN_USER` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOG_LINES` ")+
      "modify column `LINK_EVENT_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOG_LINES` ")+
      "modify column `EXT_CART_NAME` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOG_LINES` ")+
      "modify column `EXT_DATA` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOG_LINES` ")+
      "modify column `EXT_EVENT_ID` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOG_LINES` ")+
      "modify column `EXT_ANNC_TYPE` char(8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `LOG_MACHINES` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOG_MACHINES` ")+
      "modify column `LOG_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOG_MACHINES` ")+
      "modify column `CURRENT_LOG` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOG_MACHINES` ")+
      "modify column `UDP_ADDR` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOG_MACHINES` ")+
      "modify column `UDP_STRING` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `LOG_MACHINES` ")+
      "modify column `LOG_RML` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `LOG_MODES` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `MATRICES` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `MATRICES` ")+
      "modify column `NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `MATRICES` ")+
      "modify column `IP_ADDRESS` char(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `MATRICES` ")+
      "modify column `IP_ADDRESS_2` char(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `MATRICES` ")+
      "modify column `USERNAME` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `MATRICES` ")+
      "modify column `USERNAME_2` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `MATRICES` ")+
      "modify column `PASSWORD` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `MATRICES` ")+
      "modify column `PASSWORD_2` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `MATRICES` ")+
      "modify column `GPIO_DEVICE` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `NOWNEXT_PLUGINS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `NOWNEXT_PLUGINS` ")+
      "modify column `PLUGIN_PATH` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `NOWNEXT_PLUGINS` ")+
      "modify column `PLUGIN_ARG` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `OUTPUTS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `OUTPUTS` ")+
      "modify column `NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `OUTPUTS` ")+
      "modify column `NODE_HOSTNAME` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `PANELS` ")+
      "modify column `OWNER` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `PANELS` ")+
      "modify column `LABEL` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `PANELS` ")+
      "modify column `DEFAULT_COLOR` char(7)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `PANEL_NAMES` ")+
      "modify column `OWNER` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `PANEL_NAMES` ")+
      "modify column `NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `PODCASTS` ")+
      "modify column `ITEM_TITLE` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `PODCASTS` ")+
      "modify column `ITEM_DESCRIPTION` text";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `PODCASTS` ")+
      "modify column `ITEM_CATEGORY` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `PODCASTS` ")+
      "modify column `ITEM_LINK` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `PODCASTS` ")+
      "modify column `ITEM_COMMENTS` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `PODCASTS` ")+
      "modify column `ITEM_AUTHOR` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `PODCASTS` ")+
      "modify column `ITEM_SOURCE_TEXT` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `PODCASTS` ")+
      "modify column `ITEM_SOURCE_URL` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `PODCASTS` ")+
      "modify column `AUDIO_FILENAME` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDAIRPLAY` ")+
      "modify column `STATION` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDAIRPLAY` ")+
      "modify column `BUTTON_LABEL_TEMPLATE` char(32) default '%t'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDAIRPLAY` ")+
      "modify column `DEFAULT_SERVICE` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDAIRPLAY` ")+
      "modify column `TITLE_TEMPLATE` char(64) default '%t'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDAIRPLAY` ")+
      "modify column `ARTIST_TEMPLATE` char(64) default '%a'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDAIRPLAY` ")+
      "modify column `OUTCUE_TEMPLATE` char(64) default '%o'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDAIRPLAY` ")+
      "modify column `DESCRIPTION_TEMPLATE` char(64) default '%i'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDAIRPLAY` ")+
      "modify column `EXIT_PASSWORD` char(41)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDAIRPLAY` ")+
      "modify column `SKIN_PATH` char(255) default '/usr/share/pixmaps/rivendell/rdairplay_skin.png'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDAIRPLAY_CHANNELS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDAIRPLAY_CHANNELS` ")+
      "modify column `START_RML` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDAIRPLAY_CHANNELS` ")+
      "modify column `STOP_RML` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDCATCH` ")+
      "modify column `STATION` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDCATCH` ")+
      "modify column `ERROR_RML` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDHOTKEYS` ")+
      "modify column `STATION_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDHOTKEYS` ")+
      "modify column `MODULE_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDHOTKEYS` ")+
      "modify column `KEY_VALUE` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDHOTKEYS` ")+
      "modify column `KEY_LABEL` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDLIBRARY` ")+
      "modify column `STATION` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDLIBRARY` ")+
      "modify column `RIPPER_DEVICE` char(64) default '/dev/cdrom'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDLIBRARY` ")+
      "modify column `CDDB_SERVER` char(64) default 'freedb.freedb.org'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDLOGEDIT` ")+
      "modify column `STATION` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDPANEL` ")+
      "modify column `STATION` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDPANEL` ")+
      "modify column `BUTTON_LABEL_TEMPLATE` char(32) default '%t'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDPANEL` ")+
      "modify column `DEFAULT_SERVICE` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDPANEL` ")+
      "modify column `SKIN_PATH` char(255) default '/usr/share/pixmaps/rivendell/rdpanel_skin.png'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDPANEL_CHANNELS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDPANEL_CHANNELS` ")+
      "modify column `START_RML` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RDPANEL_CHANNELS` ")+
      "modify column `STOP_RML` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RECORDINGS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RECORDINGS` ")+
      "modify column `CUT_NAME` char(12) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RECORDINGS` ")+
      "modify column `DESCRIPTION` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RECORDINGS` ")+
      "modify column `URL` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RECORDINGS` ")+
      "modify column `URL_USERNAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `RECORDINGS` ")+
      "modify column `URL_PASSWORD` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `REPLICATORS` ")+
      "modify column `NAME` char(32) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPLICATORS` ")+
      "modify column `DESCRIPTION` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPLICATORS` ")+
      "modify column `STATION_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPLICATORS` ")+
      "modify column `URL` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPLICATORS` ")+
      "modify column `URL_USERNAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPLICATORS` ")+
      "modify column `URL_PASSWORD` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `REPLICATOR_MAP` ")+
      "modify column `REPLICATOR_NAME` char(32) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPLICATOR_MAP` ")+
      "modify column `GROUP_NAME` char(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `REPL_CART_STATE` ")+
      "modify column `REPLICATOR_NAME` char(32) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPL_CART_STATE` ")+
      "modify column `POSTED_FILENAME` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `REPL_CUT_STATE` ")+
      "modify column `REPLICATOR_NAME` char(32) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPL_CUT_STATE` ")+
      "modify column `CUT_NAME` char(12) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `REPORTS` ")+
      "modify column `NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPORTS` ")+
      "modify column `DESCRIPTION` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPORTS` ")+
      "modify column `EXPORT_PATH` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPORTS` ")+
      "modify column `WIN_EXPORT_PATH` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPORTS` ")+
      "modify column `STATION_ID` char(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPORTS` ")+
      "modify column `SERVICE_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPORTS` ")+
      "modify column `STATION_FORMAT` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `REPORT_GROUPS` ")+
      "modify column `REPORT_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPORT_GROUPS` ")+
      "modify column `GROUP_NAME` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `REPORT_SERVICES` ")+
      "modify column `REPORT_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPORT_SERVICES` ")+
      "modify column `SERVICE_NAME` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `REPORT_SERVICES` ")+
      "modify column `REPORT_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPORT_SERVICES` ")+
      "modify column `SERVICE_NAME` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `REPORT_STATIONS` ")+
      "modify column `REPORT_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `REPORT_STATIONS` ")+
      "modify column `STATION_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `SERVICES` ")+
      "modify column `NAME` char(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `DESCRIPTION` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `NAME_TEMPLATE` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `DESCRIPTION_TEMPLATE` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `PROGRAM_CODE` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `TRACK_GROUP` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `AUTOSPOT_GROUP` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `TFC_PATH` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `TFC_WIN_PATH` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `TFC_IMPORT_TEMPLATE` char(64) default 'Rivendell Standard Import'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `TFC_LABEL_CART` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `TFC_TRACK_CART` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `TFC_BREAK_STRING` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `TFC_TRACK_STRING` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `SERVICES` ")+
      "modify column `MUS_PATH` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `MUS_WIN_PATH` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `MUS_IMPORT_TEMPLATE` char(64) default 'Rivendell Standard Import'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `MUS_LABEL_CART` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `MUS_TRACK_CART` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `MUS_BREAK_STRING` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICES` ")+
      "modify column `MUS_TRACK_STRING` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `SERVICE_CLOCKS` ")+
      "modify column `SERVICE_NAME` char(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICE_CLOCKS` ")+
      "modify column `CLOCK_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `SERVICE_PERMS` ")+
      "modify column `STATION_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SERVICE_PERMS` ")+
      "modify column `SERVICE_NAME` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `STACK_LINES` ")+
      "modify column `SERVICE_NAME` char(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `STACK_LINES` ")+
      "modify column `ARTIST` char(255) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `STACK_LINES` ")+
      "modify column `SCHED_CODES` char(255) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `STATIONS` ")+
      "modify column `NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `STATIONS` ")+
      "modify column `SHORT_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `STATIONS` ")+
      "modify column `DESCRIPTION` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `STATIONS` ")+
      "modify column `USER_NAME` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `STATIONS` ")+
      "modify column `DEFAULT_NAME` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `STATIONS` ")+
      "modify column `IPV4_ADDRESS` char(15) default '127.0.0.2'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `STATIONS` ")+
      "modify column `HTTP_STATION` char(64) default 'localhost'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `STATIONS` ")+
      "modify column `CAE_STATION` char(64) default 'localhost'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `STATIONS` ")+
      "modify column `EDITOR_PATH` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `STATIONS` ")+
      "modify column `JACK_SERVER_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `STATIONS` ")+
      "modify column `JACK_COMMAND_LINE` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `STATIONS` ")+
      "modify column `HPI_VERSION` char(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `STATIONS` ")+
      "modify column `JACK_VERSION` char(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `STATIONS` ")+
      "modify column `ALSA_VERSION` char(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `SWITCHER_NODES` ")+
      "modify column `STATION_NAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SWITCHER_NODES` ")+
      "modify column `HOSTNAME` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SWITCHER_NODES` ")+
      "modify column `PASSWORD` char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SWITCHER_NODES` ")+
      "modify column `DESCRIPTION` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `SYSTEM` ")+
      "modify column `ISCI_XREFERENCE_PATH` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SYSTEM` ")+
      "modify column `TEMP_CART_GROUP` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `SYSTEM` ")+
      "modify column `NOTIFICATION_ADDRESS` char(15) default '"+
      RD_NOTIFICATION_ADDRESS+"'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `TRIGGERS` ")+
      "modify column `CUT_NAME` char(12)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `TTYS` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `TTYS` ")+
      "modify column `PORT` char(20)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `USERS` ")+
      "modify column `LOGIN_NAME` char(255) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `USERS` ")+
      "modify column `FULL_NAME` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `USERS` ")+
      "modify column `PHONE_NUMBER` char(20)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `USERS` ")+
      "modify column `DESCRIPTION` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `USERS` ")+
      "modify column `PASSWORD` char(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `USERS` ")+
      "modify column `PAM_SERVICE` char(32) default 'rivendell'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `USER_PERMS` ")+
      "modify column `USER_NAME` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `USER_PERMS` ")+
      "modify column `GROUP_NAME` char(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `USER_SERVICE_PERMS` ")+
      "modify column `USER_NAME` char(255) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `USER_SERVICE_PERMS` ")+
      "modify column `SERVICE_NAME` char(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `VGUEST_RESOURCES` ")+
      "modify column `STATION_NAME` char(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `WEBAPI_AUTHS` ")+
      "modify column `TICKET` char(41) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `WEBAPI_AUTHS` ")+
      "modify column `LOGIN_NAME` char(255) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `WEBAPI_AUTHS` ")+
      "modify column `IPV4_ADDRESS` char(16) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `WEB_CONNECTIONS` ")+
      "modify column `LOGIN_NAME` char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table `WEB_CONNECTIONS` ")+
      "modify column `IP_ADDRESS` char(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 296
  //
  if((cur_schema==296)&&(set_schema<cur_schema)) {
    sql=QString("alter table `STATIONS` add column ")+
      "`BACKUP_DIR` char(255) after `TIME_OFFSET`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `STATIONS` add column ")+
      "`BACKUP_LIFE` int default 0 after `BACKUP_DIR`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 295
  //
  if((cur_schema==295)&&(set_schema<cur_schema)) {
    if(!DropTable("IMPORTER_LINES",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 294
  //
  if((cur_schema==294)&&(set_schema<cur_schema)) {
    sql=QString("select `KEY_NAME` from `FEEDS`");
    q=new RDSqlQuery(sql);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_FLG";
      tablename.replace(" ","_");
      sql=QString("create table if not exists `")+tablename+"` ("+
	"`ID` int unsigned primary key auto_increment,"+
	"`CAST_ID` int unsigned,"+
	"`ACCESS_DATE` date,"+
	"`ACCESS_COUNT` int unsigned default 0,"+
	"index `CAST_ID_IDX` (`CAST_ID`,`ACCESS_DATE`))"+
	" charset latin1 collate latin1_swedish_ci"+
	db_table_create_postfix;
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }

      sql=QString("select ")+
	"`CAST_ID`,"+       // 00
	"`ACCESS_DATE`,"+   // 01
	"`ACCESS_COUNT` "+  // 02
	"from `CAST_DOWNLOADS` where "+
	"`FEED_KEY_NAME`='"+RDEscapeString(q->value(0).toString())+"' "+
	"order by `CAST_ID`,`ACCESS_DATE`";
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	sql=QString("insert into `")+tablename+"` set "+
	  QString::asprintf("`CAST_ID`=%u,",q1->value(0).toUInt())+
	  "`ACCESS_DATE`='"+
	  RDEscapeString(q1->value(1).toDate().toString("yyyy-MM-dd"))+"',"+
	  QString::asprintf("`ACCESS_COUNT`=%u",q1->value(2).toUInt());
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }

    if(!DropTable("CAST_DOWNLOADS",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 293
  //
  if((cur_schema==293)&&(set_schema<cur_schema)) {
    sql=QString("select `NAME` from `SERVICES`");
    q=new RDSqlQuery(sql);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_STACK";
      tablename.replace(" ","_");
      sql=QString("create table `")+tablename+"` ("+
	"`SCHED_STACK_ID` int unsigned not null primary key,"+
	"`CART` int unsigned not null,"+
	"`ARTIST` varchar(255),"+
	"`SCHED_CODES` varchar(255),"+
	"`SCHEDULED_AT` datetime default '1000-01-01 00:00:00')"+
	" charset latin1 collate latin1_swedish_ci"+
	db_table_create_postfix;
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
      sql=QString("select ")+
	"`SCHED_STACK_ID`,"+  // 00
	"`CART`,"+            // 01
	"`ARTIST`,"+          // 02
	"`SCHED_CODES`,"+     // 03
	"`SCHEDULED_AT` "+    // 04
	"from `STACK_LINES` where "+
	"`SERVICE_NAME`='"+RDEscapeString(q->value(0).toString())+"'";
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	sql=QString("insert into `")+tablename+"` set "+
	  QString::asprintf("`SCHED_STACK_ID`=%u,",q1->value(0).toUInt())+
	  QString::asprintf("`CART`=%u,",q1->value(1).toUInt())+
	  "`ARTIST`='"+RDEscapeString(q1->value(2).toString())+"',"+
	  "`SCHED_CODES`='"+RDEscapeString(q1->value(3).toString())+"',"+
	  "`SCHEDULED_AT`='"+RDEscapeString(q1->value(4).toDateTime().
					  toString("yyyy-MM-dd hh:mm:ss"))+"'";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      sql=QString("update `")+tablename+"` set "+
	"`SCHEDULED_AT`='1000-01-01 00:00:00' where "+
	"`SCHEDULED_AT`='1752-09-14 00:00:00'";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    if(!DropTable("STACK_LINES",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 292
  //
  if((cur_schema==292)&&(set_schema<cur_schema)) {
    sql=QString("select `NAME` from `CLOCKS`");
    q=new RDSqlQuery(sql);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_RULES";
      tablename.replace(" ","_");
      sql=QString("create table `")+tablename+"` ("+
	"`CODE` varchar(10) not null primary key,"+
	"`MAX_ROW` int unsigned,"+
	"`MIN_WAIT` int unsigned,"+
	"`NOT_AFTER` varchar(10),"+
	"`OR_AFTER` varchar(10),"+
	"`OR_AFTER_II` varchar(10))"+
	" charset latin1 collate latin1_swedish_ci"+
	db_table_create_postfix;
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }

      sql=QString("select ")+
	"`CODE`,"+         // 00
	"`MAX_ROW`,"+      // 01
	"`MIN_WAIT`,"+     // 02
	"`NOT_AFTER`,"+    // 03
	"`OR_AFTER`,"+     // 04
	"`OR_AFTER_II` "+  // 05
	"from RULE_LINES where "+
	"`CLOCK_NAME`='"+RDEscapeString(q->value(0).toString())+"' "+
	"order by `CODE`";
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	sql=QString("insert into `")+tablename+"` set "+
	  "`CODE`='"+RDEscapeString(q1->value(0).toString())+"',"+
	  QString::asprintf("`MAX_ROW`=%u,",q1->value(1).toUInt())+
	  QString::asprintf("`MIN_WAIT`=%u,",q1->value(2).toUInt())+
	  "`NOT_AFTER`='"+RDEscapeString(q1->value(3).toString())+"',"+
	  "`OR_AFTER`='"+RDEscapeString(q1->value(4).toString())+"',"+
	  "`OR_AFTER_II`='"+RDEscapeString(q1->value(5).toString())+"'";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }

    if(!DropTable("RULE_LINES",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }


  //
  // Revert 291
  //
  if((cur_schema==291)&&(set_schema<cur_schema)) {
    sql=QString("select `NAME` from `CLOCKS`");
    q=new RDSqlQuery(sql);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_CLK";
      tablename.replace(" ","_");
      sql=QString("create table `")+tablename+"` ("+
	"`ID` int unsigned auto_increment not null primary key,"+
	"`EVENT_NAME` char(64) not null,"+
	"`START_TIME` int not null,"+
	"`LENGTH` int not null,"+
	"index `EVENT_NAME_IDX` (`EVENT_NAME`))"+
	" charset latin1 collate latin1_swedish_ci"+
	db_table_create_postfix;
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }

      sql=QString("select ")+
	"`EVENT_NAME`,"+  // 00
	"`START_TIME`,"+  // 01
	"`LENGTH` "+      // 02
	"from `CLOCK_LINES` where "+
	"`CLOCK_NAME`='"+RDEscapeString(q->value(0).toString())+"' "+
	"order by `START_TIME`";
      q1=new RDSqlQuery(sql);
      while(q1->next()) {
	sql=QString("insert into `")+tablename+"` set "+
	  "`EVENT_NAME`='"+RDEscapeString(q1->value(0).toString())+"',"+
	  QString::asprintf("`START_TIME`=%d,",q1->value(1).toInt())+
	  QString::asprintf("`LENGTH`=%d",q1->value(2).toInt());
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }
    delete q;

    if(!DropTable("CLOCK_LINES",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 290
  //
  if((cur_schema==290)&&(set_schema<cur_schema)) {
    sql=QString("select `NAME` from `EVENTS`");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      //
      // PreImport Events
      //
      QString tablename=q->value(0).toString()+"_PRE";
      tablename.replace(" ","_");
      sql=QString("create table if not exists `")+tablename+"` ("+
	"`ID` int not null primary key,"+
	"`COUNT` int not null,"+
	"`TYPE` int default 0,"+
	"`SOURCE` int not null,"+
	"`START_TIME` int,"+
	"`GRACE_TIME` int default 0,"+
	"`CART_NUMBER` int UNSIGNED not null default 0,"+
	"`TIME_TYPE` int not null,"+
	"`POST_POINT` enum('N','Y') default 'N',"+
	"`TRANS_TYPE` int not null,"+
	"`START_POINT` int not null default -1,"+
	"`END_POINT` int not null default -1,"+
	"`FADEUP_POINT` int default -1,"+
	QString::asprintf("`FADEUP_GAIN` int default %d,",RD_FADE_DEPTH)+
	"`FADEDOWN_POINT` int default -1,"+
	QString::asprintf("`FADEDOWN_GAIN` int default %d,",RD_FADE_DEPTH)+
	"`SEGUE_START_POINT` int not null default -1,"+
	"`SEGUE_END_POINT` int not null default -1,"+
	QString::asprintf("`SEGUE_GAIN` int default %d,",RD_FADE_DEPTH)+
	"`DUCK_UP_GAIN` int default 0,"+
	"`DUCK_DOWN_GAIN` int default 0,"+
	"`COMMENT` CHAR(255),"+
	"`LABEL` CHAR(64),"+
	"`ORIGIN_USER` char(255),"+
	"`ORIGIN_DATETIME` datetime,"+
	"`EVENT_LENGTH` int default -1,"+
	"`LINK_EVENT_NAME` char(64),"+
	"`LINK_START_TIME` int,"+
	"`LINK_LENGTH` int default 0,"+
	"`LINK_START_SLOP` int default 0,"+
	"`LINK_END_SLOP` int default 0,"+
	"`LINK_ID` int default -1,"+
	"`LINK_EMBEDDED` enum('N','Y') default 'N',"+
	"`EXT_START_TIME` time,"+
	"`EXT_LENGTH` int,"+
	"`EXT_CART_NAME` char(32),"+
	"`EXT_DATA` char(32),"+
	"`EXT_EVENT_ID` char(32),"+
	"`EXT_ANNC_TYPE` char(8),"+
	"index `COUNT_IDX` (`COUNT`),"+
	"index `CART_NUMBER_IDX` (`CART_NUMBER`),"+
	"index `LABEL_IDX` (`LABEL`))"+
	" charset latin1 collate latin1_swedish_ci"+
	db_table_create_postfix;
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
      sql=QString("select ")+
	"`COUNT`,"+           // 00
	"`EVENT_TYPE`,"+      // 01
	"`CART_NUMBER`,"+     // 02
	"`TRANS_TYPE`,"+      // 03
	"`MARKER_COMMENT` "+  // 04
	"from `EVENT_LINES` where "+
	"`EVENT_NAME`='"+RDEscapeString(q->value(0).toString())+"' && "+
	"`TYPE`=0";
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	sql=QString("insert into `")+tablename+"` set "+
	  "`SOURCE`=0,"+
	  "`TIME_TYPE`=0,"+
	  QString::asprintf("`ID`=%d,",1+q1->value(0).toInt())+
	  QString::asprintf("`COUNT`=%d,",q1->value(0).toInt())+
	  QString::asprintf("`TYPE`=%d,",q1->value(1).toInt())+
	  QString::asprintf("`CART_NUMBER`=%u,",q1->value(2).toUInt())+
	  QString::asprintf("`TRANS_TYPE`=%d,",q1->value(3).toInt())+
	  "`COMMENT`='"+RDEscapeString(q1->value(4).toString())+"'";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;

      //
      // PostImport Events
      //
      tablename=q->value(0).toString()+"_POST";
      tablename.replace(" ","_");
      sql=QString("create table if not exists `")+tablename+"` ("+
	"`ID` int not null primary key,"+
	"`COUNT` int not null,"+
	"`TYPE` int default 0,"+
	"`SOURCE` int not null,"+
	"`START_TIME` int,"+
	"`GRACE_TIME` int default 0,"+
	"`CART_NUMBER` int UNSIGNED not null default 0,"+
	"`TIME_TYPE` int not null,"+
	"`POST_POINT` enum('N','Y') default 'N',"+
	"`TRANS_TYPE` int not null,"+
	"`START_POINT` int not null default -1,"+
	"`END_POINT` int not null default -1,"+
	"`FADEUP_POINT` int default -1,"+
	QString::asprintf("`FADEUP_GAIN` int default %d,",RD_FADE_DEPTH)+
	"`FADEDOWN_POINT` int default -1,"+
	QString::asprintf("`FADEDOWN_GAIN` int default %d,",RD_FADE_DEPTH)+
	"`SEGUE_START_POINT` int not null default -1,"+
	"`SEGUE_END_POINT` int not null default -1,"+
	QString::asprintf("`SEGUE_GAIN` int default %d,",RD_FADE_DEPTH)+
	"`DUCK_UP_GAIN` int default 0,"+
	"`DUCK_DOWN_GAIN` int default 0,"+
	"`COMMENT` CHAR(255),"+
	"`LABEL` CHAR(64),"+
	"`ORIGIN_USER` char(255),"+
	"`ORIGIN_DATETIME` datetime,"+
	"`EVENT_LENGTH` int default -1,"+
	"`LINK_EVENT_NAME` char(64),"+
	"`LINK_START_TIME` int,"+
	"`LINK_LENGTH` int default 0,"+
	"`LINK_START_SLOP` int default 0,"+
	"`LINK_END_SLOP` int default 0,"+
	"`LINK_ID` int default -1,"+
	"`LINK_EMBEDDED` enum('N','Y') default 'N',"+
	"`EXT_START_TIME` time,"+
	"`EXT_LENGTH` int,"+
	"`EXT_CART_NAME` char(32),"+
	"`EXT_DATA` char(32),"+
	"`EXT_EVENT_ID` char(32),"+
	"`EXT_ANNC_TYPE` char(8),"+
	"index `COUNT_IDX` (`COUNT`),"+
	"index `CART_NUMBER_IDX` (`CART_NUMBER`),"+
	"index `LABEL_IDX` (`LABEL`))"+
	" charset latin1 collate latin1_swedish_ci"+
	db_table_create_postfix;
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
      sql=QString("select ")+
	"`COUNT`,"+           // 00
	"`EVENT_TYPE`,"+      // 01
	"`CART_NUMBER`,"+     // 02
	"`TRANS_TYPE`,"+      // 03
	"`MARKER_COMMENT` "+  // 04
	"from `EVENT_LINES` where "+
	"`EVENT_NAME`='"+RDEscapeString(q->value(0).toString())+"' && "+
	"`TYPE`=1";
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	sql=QString("insert into `")+tablename+"` set "+
	  "`SOURCE`=0,"+
	  "`TIME_TYPE`=0,"+
	  QString::asprintf("`ID`=%d,",1+q1->value(0).toInt())+
	  QString::asprintf("`COUNT`=%d,",q1->value(0).toInt())+
	  QString::asprintf("`TYPE`=%d,",q1->value(1).toInt())+
	  QString::asprintf("`CART_NUMBER`=%u,",q1->value(2).toUInt())+
	  QString::asprintf("`TRANS_TYPE`=%d,",q1->value(3).toInt())+
	  "`COMMENT`='"+RDEscapeString(q1->value(4).toString())+"'";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }
    delete q;
    if(!DropTable("EVENT_LINES",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 289
  //
  if((cur_schema==289)&&(set_schema<cur_schema)) {
    sql=QString("select `NAME` from `SERVICES`");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_SRT";
      tablename.replace(" ","_");

      sql=QString("create table if not exists `")+tablename+"` ("+
	"`ID` int unsigned auto_increment primary key,"+
	"`LENGTH` int,"+
	"`LOG_NAME` char(64),"+
	"`LOG_ID` int,"+
	"`CART_NUMBER` int unsigned,"+
	"`CUT_NUMBER` int,"+
	"`TITLE` char(255),"+
	"`ARTIST` char(255),"+
	"`PUBLISHER` char(64),"+
	"`COMPOSER` char(64),"+
	"`USER_DEFINED` char(255),"+
	"`SONG_ID` char(32),"+
	"`ALBUM` char(255),"+
	"`LABEL` char(64),"+
	"`CONDUCTOR` char(64),"+
	"`USAGE_CODE` int,"+
	"`DESCRIPTION` char(64),"+
	"`OUTCUE` char(64),"+
	"`ISRC` char(12),"+
	"`ISCI` char(32),"+
	"`STATION_NAME` char(64),"+
	"`EVENT_DATETIME` datetime,"+
	"`SCHEDULED_TIME` time,"+
	"`EVENT_TYPE` int,"+
	"`EVENT_SOURCE` int,"+
	"`PLAY_SOURCE` int,"+
	"`START_SOURCE` int default 0,"+
	"`ONAIR_FLAG` enum('N','Y') default 'N',"+
	"`EXT_START_TIME` time,"+
	"`EXT_LENGTH` int,"+
	"`EXT_CART_NAME` char(32),"+
	"`EXT_DATA` char(32),"+
	"`EXT_EVENT_ID` char(8),"+
	"`EXT_ANNC_TYPE` char(8),"+
	"index `EVENT_DATETIME_IDX` (`EVENT_DATETIME`))"+
	" charset latin1 collate latin1_swedish_ci"+
	db_table_create_postfix;
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }

      sql=QString("select ")+
	"`LENGTH`,"+          // 00
	"`LOG_NAME`,"+        // 01
	"`LOG_ID`,"+          // 02
	"`CART_NUMBER`,"+     // 03
	"`CUT_NUMBER`,"+      // 04
	"`TITLE`,"+           // 05
	"`ARTIST`,"+          // 06
	"`PUBLISHER`,"+       // 07
	"`COMPOSER`,"+        // 08
	"`USER_DEFINED`,"+    // 09
	"`SONG_ID`,"+         // 10
	"`ALBUM`,"+           // 11
	"`LABEL`,"+           // 12
	"`CONDUCTOR`,"+       // 13
	"`USAGE_CODE`,"+      // 14
	"`DESCRIPTION`,"+     // 15
	"`OUTCUE`,"+          // 16
	"`ISRC`,"+            // 17
	"`ISCI`,"+            // 18
	"`STATION_NAME`,"+    // 19
	"`EVENT_DATETIME`,"+  // 20
	"`SCHEDULED_TIME`,"+  // 21
	"`EVENT_TYPE`,"+      // 22
	"`EVENT_SOURCE`,"+    // 23
	"`PLAY_SOURCE`,"+     // 24
	"`START_SOURCE`,"+    // 25
	"`ONAIR_FLAG`,"+      // 26
	"`EXT_START_TIME`,"+  // 27
	"`EXT_LENGTH`,"+      // 28
	"`EXT_CART_NAME`,"+   // 29
	"`EXT_DATA`,"+        // 30
	"`EXT_EVENT_ID`,"+    // 31
	"`EXT_ANNC_TYPE` "+   // 32
	"from `ELR_LINES` where "+
	"`SERVICE_NAME`='"+RDEscapeString(q->value(0).toString())+"'";
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	sql=QString("insert into `")+tablename+"` set "+
	  QString::asprintf("`LENGTH`=%d,",q1->value(0).toInt())+
	  "`LOG_NAME`='"+RDEscapeString(q1->value(1).toString())+"',"+
	  QString::asprintf("`LOG_ID`=%d,",q1->value(2).toInt())+
	  QString::asprintf("`CART_NUMBER`=%u,",q1->value(3).toUInt())+
	  QString::asprintf("`CUT_NUMBER`=%d,",q1->value(4).toInt())+
	  "`TITLE`='"+RDEscapeString(q1->value(5).toString())+"',"+
	  "`ARTIST`='"+RDEscapeString(q1->value(6).toString())+"',"+
	  "`PUBLISHER`='"+RDEscapeString(q1->value(7).toString())+"',"+
	  "`COMPOSER`='"+RDEscapeString(q1->value(8).toString())+"',"+
	  "`USER_DEFINED`='"+RDEscapeString(q1->value(9).toString())+"',"+
	  "`SONG_ID`='"+RDEscapeString(q1->value(10).toString())+"',"+
	  "`ALBUM`='"+RDEscapeString(q1->value(11).toString())+"',"+
	  "`LABEL`='"+RDEscapeString(q1->value(12).toString())+"',"+
	  "`CONDUCTOR`='"+RDEscapeString(q1->value(13).toString())+"',"+
	  "`USAGE_CODE`='"+RDEscapeString(q1->value(14).toString())+"',"+
	  "`DESCRIPTION`='"+RDEscapeString(q1->value(15).toString())+"',"+
	  "`OUTCUE`='"+RDEscapeString(q1->value(16).toString())+"',"+
	  "`ISRC`='"+RDEscapeString(q1->value(17).toString())+"',"+
	  "`ISCI`='"+RDEscapeString(q1->value(18).toString())+"',"+
	  "`STATION_NAME`='"+RDEscapeString(q1->value(19).toString())+"',"+
	  "`EVENT_DATETIME`="+
	  RDCheckDateTime(q1->value(20).toDateTime(),"yyyy-MM-dd hh:mm:ss")+","+
	  "`SCHEDULED_TIME`="+
	  RDCheckDateTime(q1->value(21).toTime(),"hh:mm:ss")+","+
	  QString::asprintf("`EVENT_TYPE`=%d,",q1->value(22).toInt())+
	  QString::asprintf("`EVENT_SOURCE`=%d,",q1->value(23).toInt())+
	  QString::asprintf("`PLAY_SOURCE`=%d,",q1->value(24).toInt())+
	  QString::asprintf("`START_SOURCE`=%d,",q1->value(25).toInt())+
	  "`ONAIR_FLAG`='"+RDEscapeString(q1->value(26).toString())+"',"+
	  "`EXT_START_TIME`="+
	  RDCheckDateTime(q1->value(27).toTime(),"hh:mm:ss")+","+
	  QString::asprintf("`EXT_LENGTH`=%d,",q1->value(28).toInt())+
	  "`EXT_CART_NAME`='"+RDEscapeString(q1->value(29).toString())+"',"+
	  "`EXT_DATA`='"+RDEscapeString(q1->value(30).toString())+"',"+
	  "`EXT_EVENT_ID`='"+RDEscapeString(q1->value(31).toString())+"',"+
	  "`EXT_ANNC_TYPE`='"+RDEscapeString(q1->value(32).toString())+"'";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }
    delete q;

    if(!DropTable("ELR_LINES",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 288
  //
  if((cur_schema==288)&&(set_schema<cur_schema)) {
    sql=QString("select `NAME` from `LOGS`");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_LOG";
      tablename.replace(" ","_");
      sql=QString("create table if not exists `")+tablename+"` ("+
	"`ID` int not null primary key,"+
	"`COUNT` int not null,"+
	"`TYPE` int default 0,"+
	"`SOURCE` int not null,"+
	"`START_TIME` int,"+
	"`GRACE_TIME` int default 0,"+
	"`CART_NUMBER` int UNSIGNED not null default 0,"+
	"`TIME_TYPE` int not null,"+
	"`POST_POINT` enum('N','Y') default 'N',"+
	"`TRANS_TYPE` int not null,"+
	"`START_POINT` int not null default -1,"+
	"`END_POINT` int not null default -1,"+
	"`FADEUP_POINT` int default -1,"+
	QString::asprintf("`FADEUP_GAIN` int default %d,",RD_FADE_DEPTH)+
	"`FADEDOWN_POINT` int default -1,"+
	QString::asprintf("`FADEDOWN_GAIN` int default %d,",RD_FADE_DEPTH)+
	"`SEGUE_START_POINT` int not null default -1,"+
	"`SEGUE_END_POINT` int not null default -1,"+
	QString::asprintf("`SEGUE_GAIN` int default %d,",RD_FADE_DEPTH)+
	"`DUCK_UP_GAIN` int default 0,"+
	"`DUCK_DOWN_GAIN` int default 0,"+
	"`COMMENT` char(255),"+
	"`LABEL` char(64),"+
	"`ORIGIN_USER` char(255),"+
	"`ORIGIN_DATETIME` datetime,"+
	"`EVENT_LENGTH` int default -1,"+
	"`LINK_EVENT_NAME` char(64),"+
	"`LINK_START_TIME` int,"+
	"`LINK_LENGTH` int default 0,"+
	"`LINK_START_SLOP` int default 0,"+
	"`LINK_END_SLOP` int default 0,"+
	"`LINK_ID` int default -1,"+
	"`LINK_EMBEDDED` enum('N','Y') default 'N',"+
	"`EXT_START_TIME` time,"+
	"`EXT_LENGTH` int,"+
	"`EXT_CART_NAME` char(32),"+
	"`EXT_DATA` char(32),"+
	"`EXT_EVENT_ID` char(32),"+
	"`EXT_ANNC_TYPE` char(8),"+
	"index `COUNT_IDX` (`COUNT`),"+
	"index `CART_NUMBER_IDX` (`CART_NUMBER`),"+
	"index `LABEL_IDX` (`LABEL`))"+
	" charset latin1 collate latin1_swedish_ci"+
	db_table_create_postfix;
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }

      sql=QString("select ")+
	"`LINE_ID`,"+            // 00
	"`COUNT`,"+              // 01
	"`TYPE`,"+               // 02
	"`SOURCE`,"+             // 03
	"`START_TIME`,"+         // 04
	"`GRACE_TIME`,"+         // 05
	"`CART_NUMBER`,"+        // 06
	"`TIME_TYPE`,"+          // 07
	"`POST_POINT`,"+         // 08
	"`TRANS_TYPE`,"+         // 09
	"`START_POINT`,"+        // 10
	"`END_POINT`,"+          // 11
	"`FADEUP_POINT`,"+       // 12
	"`FADEUP_GAIN`,"+        // 13
	"`FADEDOWN_POINT`,"+     // 14
	"`FADEDOWN_GAIN`,"+      // 15
	"`SEGUE_START_POINT`,"+  // 16
	"`SEGUE_END_POINT`,"+    // 17
	"`SEGUE_GAIN`,"+         // 18
	"`DUCK_UP_GAIN`,"+       // 19
	"`DUCK_DOWN_GAIN`,"+     // 20
	"`COMMENT`,"+            // 21
	"`LABEL`,"+              // 22
	"`ORIGIN_USER`,"+        // 23
	"`ORIGIN_DATETIME`,"+    // 24
	"`EVENT_LENGTH`,"+       // 25
	"`LINK_EVENT_NAME`,"+    // 26
	"`LINK_START_TIME`,"+    // 27
	"`LINK_LENGTH`,"+        // 28
	"`LINK_START_SLOP`,"+    // 29
	"`LINK_END_SLOP`,"+      // 30
	"`LINK_ID`,"+            // 31
	"`LINK_EMBEDDED`,"+      // 32
	"`EXT_START_TIME`,"+     // 33
	"`EXT_LENGTH`,"+         // 34
	"`EXT_CART_NAME`,"+      // 35
	"`EXT_DATA`,"+           // 36
	"`EXT_EVENT_ID`,"+       // 37
	"`EXT_ANNC_TYPE` "+      // 38
	"from `LOG_LINES` where "+
	"`LOG_NAME`='"+RDEscapeString(q->value(0).toString())+"' "+
	"order by `COUNT`";
      q1=new RDSqlQuery(sql);
      while(q1->next()) {
	sql=QString("insert into `")+tablename+"` set "+
	  QString::asprintf("`ID`=%d,",q1->value(0).toInt())+
	  QString::asprintf("`COUNT`=%d,",q1->value(1).toInt())+
	  QString::asprintf("`TYPE`=%d,",q1->value(2).toInt())+
	  QString::asprintf("`SOURCE`=%d,",q1->value(3).toInt())+
	  QString::asprintf("`START_TIME`=%d,",q1->value(4).toInt())+
	  QString::asprintf("`GRACE_TIME`=%d,",q1->value(5).toInt())+
	  QString::asprintf("`CART_NUMBER`=%u,",q1->value(6).toUInt())+
	  QString::asprintf("`TIME_TYPE`=%d,",q1->value(7).toInt())+
	  "`POST_POINT`='"+RDEscapeString(q1->value(8).toString())+"',"+
	  QString::asprintf("`TRANS_TYPE`=%d,",q1->value(9).toInt())+
	  QString::asprintf("`START_POINT`=%d,",q1->value(10).toInt())+
	  QString::asprintf("`END_POINT`=%d,",q1->value(11).toInt())+
	  QString::asprintf("`FADEUP_POINT`=%d,",q1->value(12).toInt())+
	  QString::asprintf("`FADEUP_GAIN`=%d,",q1->value(13).toInt())+
	  QString::asprintf("`FADEDOWN_POINT`=%d,",q1->value(14).toInt())+
	  QString::asprintf("`FADEDOWN_GAIN`=%d,",q1->value(15).toInt())+
	  QString::asprintf("`SEGUE_START_POINT`=%d,",q1->value(16).toInt())+
	  QString::asprintf("`SEGUE_END_POINT`=%d,",q1->value(17).toInt())+
	  QString::asprintf("`SEGUE_GAIN`=%d,",q1->value(18).toInt())+
	  QString::asprintf("`DUCK_UP_GAIN`=%d,",q1->value(19).toInt())+
	  QString::asprintf("`DUCK_DOWN_GAIN`=%d,",q1->value(20).toInt())+
	  "`COMMENT`='"+RDEscapeString(q1->value(21).toString())+"',"+
	  "`LABEL`='"+RDEscapeString(q1->value(22).toString())+"',"+
	  "`ORIGIN_USER`='"+RDEscapeString(q1->value(23).toString())+"',"+
	  "`ORIGIN_DATETIME`="+
	  RDCheckDateTime(q1->value(24).toDateTime(),"yyyy-MM-dd hh:mm:ss")+","+
	  QString::asprintf("`EVENT_LENGTH`=%d,",q1->value(25).toInt())+
	  "`LINK_EVENT_NAME`='"+RDEscapeString(q1->value(26).toString())+"',"+
	  QString::asprintf("`LINK_START_TIME`=%d,",q1->value(27).toInt())+
	  QString::asprintf("`LINK_LENGTH`=%d,",q1->value(28).toInt())+
	  QString::asprintf("`LINK_START_SLOP`=%d,",q1->value(29).toInt())+
	  QString::asprintf("`LINK_END_SLOP`=%d,",q1->value(30).toInt())+
	  QString::asprintf("`LINK_ID`=%d,",q1->value(31).toInt())+
	  "`LINK_EMBEDDED`='"+RDEscapeString(q1->value(32).toString())+"',"+
	  "`EXT_START_TIME`="+
	  RDCheckDateTime(q1->value(33).toTime(),"hh:mm:ss")+","+
	  QString::asprintf("`EXT_LENGTH`=%d,",q1->value(34).toInt())+
	  "`EXT_CART_NAME`='"+RDEscapeString(q1->value(35).toString())+"',"+
	  "`EXT_DATA`='"+RDEscapeString(q1->value(36).toString())+"',"+
	  "`EXT_EVENT_ID`='"+RDEscapeString(q1->value(37).toString())+"',"+
	  "`EXT_ANNC_TYPE`='"+RDEscapeString(q1->value(38).toString())+"'";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }
    delete q;

    if(!DropTable("LOG_LINES",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 287
  //
  if((cur_schema==287)&&(set_schema<cur_schema)) {
    DropColumn("STATIONS","JACK_PORTS");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 286
  //
  if((cur_schema==286)&&(set_schema<cur_schema)) {
    sql=QString("alter table `SERVICES` add ")+
      "column `TFC_LENGTH_OFFSET` int after `TFC_LEN_SECONDS_LENGTH`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `SERVICES` add ")+
      "column `TFC_LENGTH_LENGTH` int after `TFC_LENGTH_OFFSET`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `SERVICES` add ")+
      "column `MUS_LENGTH_OFFSET` int after `MUS_LEN_SECONDS_LENGTH`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `SERVICES` add ")+
      "column `MUS_LENGTH_LENGTH` int after `MUS_LENGTH_OFFSET`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 285
  //
  if((cur_schema==285)&&(set_schema<cur_schema)) {
    sql=QString("alter table `CART` add column `ISRC` char(12) after `YEAR`");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `DECKS` add ")+
      "column `DEFAULT_SAMPRATE` int default 44100 after `DEFAULT_CHANNELS`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDLIBRARY` add ")+
      "column `INPUT_STREAM` int default 0 after `INPUT_CARD`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDLIBRARY` add ")+
      "column `OUTPUT_STREAM` int default 0 after `OUTPUT_CARD`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDLIBRARY` add ")+
      "column `RECORD_GPI` int default -1 after `TRIM_THRESHOLD`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDLIBRARY` add ")+
      "column `PLAY_GPI` int default -1 after `RECORD_GPI`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDLIBRARY` add ")+
      "column `STOP_GPI` int default -1 after `PLAY_GPI`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDLIBRARY` add ")+
      "column `DEFAULT_SAMPRATE` int default 44100 after `DEFAULT_CHANNELS`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDLOGEDIT` add ")+
      "column `SAMPRATE` int unsigned default 44100 after `FORMAT`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `SERVICES` add ")+
      "column `TFC_START_OFFSET` int after `TFC_TITLE_LENGTH`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `SERVICES` add ")+
      "column `TFC_START_LENGTH` int after `TFC_START_OFFSET`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `SERVICES` add ")+
      "column `MUS_START_OFFSET` int after `MUS_TITLE_LENGTH`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `SERVICES` add ")+
      "column `MUS_START_LENGTH` int after `MUS_START_OFFSET`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 284
  //
  if((cur_schema==284)&&(set_schema<cur_schema)) {
    for(int i=9;i>=0;i--) {
      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`STOP_RML%d` char(255) after `STATION`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`START_RML%d` char(255) after `STATION`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`PORT%d` int default 0 after `STATION`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`CARD%d` int default 0 after `STATION`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    sql=QString("alter table `RDAIRPLAY` add column ")+
      "`OP_MODE` int default 2 after `TRANS_LENGTH`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDAIRPLAY` add column ")+
      "`START_MODE` int default 0 after `OP_MODE`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `RDPANEL` add column `INSTANCE` int unsigned ")+
      "not null default 0 after `STATION`";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    for(int i=9;i>=6;i--) {
      sql=QString("alter table `RDPANEL` add column ")+
	QString::asprintf("`STOP_RML%d` char(255) after `INSTANCE`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDPANEL` add column ")+
	QString::asprintf("`START_RML%d` char(255) after `INSTANCE`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDPANEL` add column ")+
	QString::asprintf("`PORT%d` int default 0 after `INSTANCE`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDPANEL` add column ")+
	QString::asprintf("`CARD%d` int default 0 after `INSTANCE`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    for(int i=3;i>=2;i--) {
      sql=QString("alter table `RDPANEL` add column ")+
	QString::asprintf("`STOP_RML%d` char(255) after `INSTANCE`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDPANEL` add column ")+
	QString::asprintf("`START_RML%d` char(255) after `INSTANCE`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDPANEL` add column ")+
	QString::asprintf("`PORT%d` int default 0 after `INSTANCE`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDPANEL` add column ")+
	QString::asprintf("`CARD%d` int default 0 after `INSTANCE`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    sql=QString("alter table `MATRICES` alter column `PORT` drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `MATRICES` alter column `PORT_2` drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `MATRICES` alter column `INPUTS` drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `MATRICES` alter column `OUTPUTS` drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `MATRICES` alter column `GPIS` drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `MATRICES` alter column `GPOS` drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `REPLICATORS` alter column `TYPE_ID` drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 283
  //
  if((cur_schema==283)&&(set_schema<cur_schema)) {
    sql=QString("create table if not exists `AUDIO_PORTS` (")+
      "`ID` int unsigned not null primary key AUTO_INCREMENT,"+
      "`STATION_NAME` char(64) not null,"+
      "`CARD_NUMBER` int not null,"+
      "`CLOCK_SOURCE` int default 0,"+
      "`INPUT_0_LEVEL` int default 0,"+
      "`INPUT_0_TYPE` int default 0,"+
      "`INPUT_0_MODE` int default 0,"+
      "`INPUT_1_LEVEL` int default 0,"+
      "`INPUT_1_TYPE` int default 0,"+
      "`INPUT_1_MODE` int default 0,"+
      "`INPUT_2_LEVEL` int default 0,"+
      "`INPUT_2_TYPE` int default 0,"+
      "`INPUT_2_MODE` int default 0,"+
      "`INPUT_3_LEVEL` int default 0,"+
      "`INPUT_3_TYPE` int default 0,"+
      "`INPUT_3_MODE` int default 0,"+
      "`INPUT_4_LEVEL` int default 0,"+
      "`INPUT_4_TYPE` int default 0,"+
      "`INPUT_4_MODE` int default 0,"+
      "`INPUT_5_LEVEL` int default 0,"+
      "`INPUT_5_TYPE` int default 0,"+
      "`INPUT_5_MODE` int default 0,"+
      "`INPUT_6_LEVEL` int default 0,"+
      "`INPUT_6_TYPE` int default 0,"+
      "`INPUT_6_MODE` int default 0,"+
      "`INPUT_7_LEVEL` int default 0,"+
      "`INPUT_7_TYPE` int default 0,"+
      "`INPUT_7_MODE` int default 0,"+
      "`OUTPUT_0_LEVEL` int default 0,"+
      "`OUTPUT_1_LEVEL` int default 0,"+
      "`OUTPUT_2_LEVEL` int default 0,"+
      "`OUTPUT_3_LEVEL` int default 0,"+
      "`OUTPUT_4_LEVEL` int default 0,"+
      "`OUTPUT_5_LEVEL` int default 0,"+
      "`OUTPUT_6_LEVEL` int default 0,"+
      "`OUTPUT_7_LEVEL` int default 0,"+
      "index `STATION_NAME_IDX` (`STATION_NAME`),"+
      "index `CARD_NUMBER_IDX` (`CARD_NUMBER`))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("select `NAME` from `STATIONS`");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(int i=0;i<8;i++) {
	sql=QString("insert into `AUDIO_PORTS` set ")+
	  "`STATION_NAME`='"+RDEscapeString(q->value(0).toString())+"',"+
	  QString::asprintf("`CARD_NUMBER`=%d",i);
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
    }
    delete q;
    sql=QString("select ")+
      "`STATION_NAME`,"+  // 00
      "`CARD_NUMBER`,"+   // 01
      "`PORT_NUMBER`,"+   // 02
      "`LEVEL`,"+         // 03
      "`TYPE`,"+          // 04
      "`MODE` "+          // 05
      "from `AUDIO_INPUTS` where `PORT_NUMBER`<8";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update `AUDIO_PORTS` set ")+
	QString::asprintf("`INPUT_%d_LEVEL`=%d,",
			  q->value(2).toInt(),q->value(3).toInt())+
	QString::asprintf("`INPUT_%d_TYPE`=%d,",
			  q->value(2).toInt(),q->value(4).toInt())+
	QString::asprintf("`INPUT_%d_MODE`=%d  where ",
			  q->value(2).toInt(),q->value(5).toInt())+
	"STATION_NAME='"+RDEscapeString(q->value(0).toString())+"' && "+
	QString::asprintf("`CARD_NUMBER`=%d",q->value(1).toInt());
    }
    delete q;
    if(!DropTable("AUDIO_INPUTS",err_msg)) {
      return false;
    }

    sql=QString("select ")+
      "`STATION_NAME`,"+  // 00
      "`CARD_NUMBER`,"+   // 01
      "`PORT_NUMBER`,"+   // 02
      "`LEVEL` "+         // 03
      "from `AUDIO_OUTPUTS` where `PORT_NUMBER`<8";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update `AUDIO_PORTS` set ")+
	QString::asprintf("`OUTPUT_%d_LEVEL`=%d where ",
			  q->value(2).toInt(),q->value(3).toInt())+
	"`STATION_NAME`='"+RDEscapeString(q->value(0).toString())+"' && "+
	QString::asprintf("`CARD_NUMBER`=%d",q->value(1).toInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;
    if(!DropTable("AUDIO_OUTPUTS",err_msg)) {
      return false;
    }

    sql=QString("select ")+
      "`STATION_NAME`,"+  // 00
      "`CARD_NUMBER`,"+   // 01
      "`CLOCK_SOURCE` "+  // 02
      "from `AUDIO_CARDS` where `CARD_NUMBER`<8";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update `AUDIO_PORTS` set ")+
	QString::asprintf("`CLOCK_SOURCE`=%d where ",q->value(2).toInt())+
	"`STATION_NAME`='"+RDEscapeString(q->value(0).toString())+"' && "+
	QString::asprintf("`CARD_NUMBER`=%d",q->value(1).toInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;
    DropColumn("AUDIO_CARDS","CLOCK_SOURCE");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 282
  //
  if((cur_schema==282)&&(set_schema<cur_schema)) {
    for(int i=7;i>=0;i--) {
      sql=QString("alter table `STATIONS` add column ")+
	QString::asprintf("`CARD%d_OUTPUTS` int default -1 after `ALSA_VERSION`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `STATIONS` add column ")+
	QString::asprintf("`CARD%d_INPUTS` int default -1 after `ALSA_VERSION`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `STATIONS` add column ")+
	QString::asprintf("`CARD%d_NAME` char(64) after `ALSA_VERSION`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `STATIONS` add column ")+
	QString::asprintf("`CARD%d_DRIVER` int default 0 after `ALSA_VERSION`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    if(!DropTable("AUDIO_CARDS",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 281
  //
  if((cur_schema==281)&&(set_schema<cur_schema)) {
    DropColumn("RDAIRPLAY","VIRTUAL_EXIT_CODE");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 280
  //
  if((cur_schema==280)&&(set_schema<cur_schema)) {
    sql=QString("delete from `LOG_MODES` where `MACHINE`>=100");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 279
  //
  if((cur_schema==279)&&(set_schema<cur_schema)) {
    sql=QString("delete from `RDAIRPLAY_CHANNELS` where `INSTANCE`>=100");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 278
  //
  if((cur_schema==278)&&(set_schema<cur_schema)) {
    for(int i=2;i>=0;i--) {
      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`LOG_RML%d` char(255) after `DESCRIPTION_TEMPLATE`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`UDP_STRING%d` char(255) after `DESCRIPTION_TEMPLATE`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`UDP_PORT%d` int unsigned after `DESCRIPTION_TEMPLATE`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`UDP_ADDR%d` char(255) after `DESCRIPTION_TEMPLATE`",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`LOG%d_START_MODE` int default 0 ",i)+
	"after `AUDITION_PREROLL`";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`LOG%d_NEXT_CART` int unsigned default 0 ",i)+
	"after `AUDITION_PREROLL`";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`LOG%d_NOW_CART` int unsigned default 0 ",i)+
	"after `AUDITION_PREROLL`";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`LOG%d_LOG_LINE` int default -1 ",i)+
	"after `AUDITION_PREROLL`";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`LOG%d_LOG_ID` int default -1 ",i)+
	"after `AUDITION_PREROLL`";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`LOG%d_RUNNING` enum('N','Y') default 'N' ",i)+
	"after `AUDITION_PREROLL`";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`LOG%d_CURRENT_LOG` char(64) ",i)+
	"after `AUDITION_PREROLL`";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`LOG%d_LOG_NAME` char(64) ",i)+
	"after `AUDITION_PREROLL`";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `RDAIRPLAY` add column ")+
	QString::asprintf("`LOG%d_AUTO_RESTART` enum('N','Y') default 'N' ",i)+
	"after `AUDITION_PREROLL`";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    sql=QString("select ")+
      "`STATION_NAME`,"+   // 00
      "`MACHINE`,"+        // 01
      "`START_MODE`,"+     // 02
      "`AUTO_RESTART`,"+   // 03
      "`LOG_NAME`,"+       // 04
      "`CURRENT_LOG`,"+    // 05
      "`RUNNING`,"+        // 06
      "`LOG_ID`,"+         // 07
      "`LOG_LINE`,"+       // 08
      "`NOW_CART`,"+       // 09
      "`NEXT_CART`,"+      // 10
      "`UDP_ADDR`,"+       // 11
      "`UDP_PORT`,"+       // 12
      "`UDP_STRING`,"+     // 13
      "`LOG_RML` "+        // 14
      "from `LOG_MACHINES` where "+
      "`MACHINE`<3";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update `RDAIRPLAY` set ")+
	QString::asprintf("`UDP_ADDR%d`='",q->value(1).toInt())+
	RDEscapeString(q->value(11).toString())+"',"+
	QString::asprintf("`UDP_PORT%d`=%u,",
			  q->value(1).toInt(),q->value(12).toUInt())+
	QString::asprintf("`UDP_STRING%d`='",q->value(1).toInt())+
	RDEscapeString(q->value(13).toString())+"',"+
	QString::asprintf("`LOG_RML%d`='",q->value(1).toInt())+
	RDEscapeString(q->value(14).toString())+"',"+
	QString::asprintf("`LOG%d_START_MODE`=%d,",
			  q->value(1).toInt(),q->value(2).toInt())+
	QString::asprintf("`LOG%d_AUTO_RESTART`='",q->value(1).toInt())+
	RDEscapeString(q->value(3).toString())+"',"+
	QString::asprintf("`LOG%d_LOG_NAME`='",q->value(1).toInt())+
	RDEscapeString(q->value(4).toString())+"',"+
	QString::asprintf("`LOG%d_CURRENT_LOG`='",q->value(1).toInt())+
	RDEscapeString(q->value(5).toString())+"',"+
	QString::asprintf("`LOG%d_RUNNING`='",q->value(1).toInt())+
	RDEscapeString(q->value(6).toString())+"',"+
	QString::asprintf("`LOG%d_LOG_ID`=%d,",
			  q->value(1).toInt(),q->value(7).toInt())+
	QString::asprintf("`LOG%d_LOG_LINE`=%d,",
			  q->value(1).toInt(),q->value(8).toInt())+
	QString::asprintf("`LOG%d_NOW_CART`=%d,",
			  q->value(1).toInt(),q->value(9).toInt())+
	QString::asprintf("`LOG%d_NEXT_CART`=%d ",
			  q->value(1).toInt(),q->value(10).toInt())+
	"where `STATION`='"+RDEscapeString(q->value(0).toString())+"'";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    if(!DropTable("LOG_MACHINES")) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 277
  //
  if((cur_schema==277)&&(set_schema<cur_schema)) {
    DropColumn("USERS","WEBGET_LOGIN_PRIV");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 276
  //
  if((cur_schema==276)&&(set_schema<cur_schema)) {
    DropColumn("SYSTEM","NOTIFICATION_ADDRESS");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 275
  //
  if((cur_schema==275)&&(set_schema<cur_schema)) {
    DropColumn("SERVICES","LOG_SHELFLIFE_ORIGIN");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 274
  //
  if((cur_schema==274)&&(set_schema<cur_schema)) {
    sql=QString("alter table `LOGS` drop index `LOCK_GUID_IDX`");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    DropColumn("LOGS","LOCK_GUID");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 273
  //
  if((cur_schema==273)&&(set_schema<cur_schema)) {
    DropColumn("LOGS","LOCK_DATETIME");
    DropColumn("LOGS","LOCK_IPV4_ADDRESS");
    DropColumn("LOGS","LOCK_STATION_NAME");
    DropColumn("LOGS","LOCK_USER_NAME");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 272
  //
  if((cur_schema==272)&&(set_schema<cur_schema)) {
    if(!DropTable("USER_SERVICE_PERMS",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 271
  //
  if((cur_schema==271)&&(set_schema<cur_schema)) {
    DropColumn("DROPBOXES","SEGUE_LEVEL");
    DropColumn("DROPBOXES","SEGUE_LENGTH");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 270
  //
  if((cur_schema==270)&&(set_schema<cur_schema)) {
    DropColumn("STATIONS","SHORT_NAME");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 269
  //
  if((cur_schema==269)&&(set_schema<cur_schema)) {
    DropColumn("GROUPS","DEFAULT_CUT_LIFE");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 268
  //
  if((cur_schema==268)&&(set_schema<cur_schema)) {
    DropColumn("DROPBOXES","FORCE_TO_MONO");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 267
  //
  if((cur_schema==267)&&(set_schema<cur_schema)) {
    DropColumn("CUTS","ORIGIN_LOGIN_NAME");
    DropColumn("CUTS","SOURCE_HOSTNAME");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 266
  //
  if((cur_schema==266)&&(set_schema<cur_schema)) {
    // Nothing to do here as this is a pseudo-schema change.

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 265
  //
  if((cur_schema==265)&&(set_schema<cur_schema)) {
    DropColumn("SYSTEM","SHOW_USER_LIST");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 264
  //
  if((cur_schema==264)&&(set_schema<cur_schema)) {
    DropColumn("SYSTEM","FIX_DUP_CART_TITLES");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 263
  //
  if((cur_schema==263)&&(set_schema<cur_schema)) {
    // Nothing to do here as this is a pseudo-schema change.

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 262
  //
  if((cur_schema==262)&&(set_schema<cur_schema)) {
    DropColumn("USERS","LOCAL_AUTH");
    DropColumn("USERS","PAM_SERVICE");

    sql=QString("drop index `IPV4_ADDRESS_IDX` on `STATIONS`");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 261
  //
  if((cur_schema==261)&&(set_schema<cur_schema)) {
    DropColumn("CUTS","SHA1_HASH");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 260
  //
  if((cur_schema==260)&&(set_schema<cur_schema)) {
    DropColumn("USERS","WEBAPI_AUTH_TIMEOUT");
    if(!DropTable("WEBAPI_AUTHS",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 259
  //
  if((cur_schema==259)&&(set_schema<cur_schema)) {
    for(int i=0;i<168;i++) {
      sql=QString::asprintf("alter table `SERVICES` add column `CLOCK%d` char(64)",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    sql=QString("select `SERVICE_NAME`,`HOUR`,`CLOCK_NAME` from `SERVICE_CLOCKS`");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update `SERVICES` set ")+
	QString::asprintf("`CLOCK%d`=",q->value(1).toInt());
      if(q->value(2).isNull()) {
	sql+="null ";
      }
      else {
	sql+="'"+RDEscapeString(q->value(2).toString())+"' ";
      }
      sql+="where `NAME`='"+RDEscapeString(q->value(0).toString())+"'";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    if(!DropTable("SERVICE_CLOCKS",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 258
  //
  if((cur_schema==258)&&(set_schema<cur_schema)) {
    sql=QString("select `NAME` from `LOGS`");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table ")+
	"`"+tablename+"_LOG` "+
	"modify column `CART_NUMBER` int unsigned not null";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 257
  //
  if((cur_schema==257)&&(set_schema<cur_schema)) {
    /*
     * Disabled because the constraint assumes null date/time values are
     * indicated by zeroed fields --i.e. '0000-00-00 00:00:00'-- which
     * are not legal in later versions of MySQL.
     *
    sql=QString("alter table `LOGS` modify column `LINK_DATETIME` datetime not null");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `LOGS` modify column `START_DATE` date not null");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table `LOGS` modify column `END_DATE` date not null");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    */
    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 256
  //
  if((cur_schema==256)&&(set_schema<cur_schema)) {
    if(!DropTable("CUT_EVENTS",err_msg)) {
      return false;
    }

    if(!DropTable("DECK_EVENTS",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 255
  //
  if((cur_schema==255)&&(set_schema<cur_schema)) {
    sql=QString("select `NAME` from `SERVICES`");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      DropColumn(tablename+"_SRT","DESCRIPTION");
      DropColumn(tablename+"_SRT","OUTCUE");
    }
    delete q;

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 254
  //
  if((cur_schema==254)&&(set_schema<cur_schema)) {
    DropColumn("CUTS","PLAY_ORDER");
    DropColumn("CART","USE_WEIGHTING");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 253
  //
  if((cur_schema==253)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 252
  //
  if((cur_schema==252)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 251
  //
  if((cur_schema==251)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 250
  //
  if((cur_schema==250)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 249
  //
  if((cur_schema==249)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 248
  //
  if((cur_schema==248)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 247
  //
  if((cur_schema==247)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 246
  //
  if((cur_schema==246)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 245
  //
  if((cur_schema==245)&&(set_schema<cur_schema)) {
    DropColumn("RDLIBRARY","READ_ISRC");

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 244
  //
  if((cur_schema==244)&&(set_schema<cur_schema)) {
    sql=QString("alter table `JACK_CLIENTS` modify column `COMMAND_LINE` ")+
      "char(255) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 243
  //
  if((cur_schema==243)&&(set_schema<cur_schema)) {
    DropColumn("STATIONS","HAVE_MP4_DECODE");

    WriteSchemaVersion(--cur_schema);
  }

  *err_msg="ok";
  return true;
}
