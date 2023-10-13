// rdcae.cpp
//
// Connection to the Rivendell Core Audio Engine
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <syslog.h>

#include "rdapplication.h"
#include "rdcae.h"
#include "rddb.h"
#include "rddebug.h"
#include "rdescape_string.h"

//
// Uncomment this to enable latency profiling in the syslog
//
// #define DEBUG_LATENCY

__RDCaeMeterPoint::__RDCaeMeterPoint()
{
  levels[0]=RD_MUTE_DEPTH;
  levels[1]=RD_MUTE_DEPTH;
}




RDCae::RDCae(RDStation *station,RDConfig *config,QObject *parent)
  : QObject(parent)
{
  //  int flags=-1;

  cae_station=station;
  cae_config=config;
  cae_connected=false;
  cae_next_serial_number=1;
  argnum=0;
  argptr=0;

  //
  // Control Connection
  //
  cae_socket=new QUdpSocket(this);
  connect(cae_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  cae_keepalive_timer=new QTimer(this);
  cae_keepalive_timer->setSingleShot(false);
  connect(cae_keepalive_timer,SIGNAL(timeout()),this,SLOT(keepaliveData()));

  /*
  //
  // TCP Connection
  //
  if((cae_socket=socket(AF_INET,SOCK_STREAM,0))<0) {
    rda->syslog(LOG_WARNING,"unable to allocate TCP socket [%s]",
		strerror(errno));
  }
  if((flags=fcntl(cae_socket,F_GETFL,NULL))>=0) {
    flags=flags|O_NONBLOCK;
    if(fcntl(cae_socket,F_SETFL,flags)<0) {
      rda->syslog(LOG_WARNING,"unable to set TCP socket to non-blocking [%s]",
		  strerror(errno));
    }
  }
  */

  //
  // Meter Connection
  //
  cae_meter_socket=new QUdpSocket(this);
  cae_meter_base_port=cae_config->meterBasePort();
  cae_meter_port_range=cae_config->meterPortRange();
  if(cae_meter_port_range>999) {
    cae_meter_port_range=999;
  }
  for(int16_t i=cae_meter_base_port;
      i<(cae_meter_base_port+cae_meter_port_range);i++) {
    if(cae_meter_socket->bind(QHostAddress(),i)) {
      i=(cae_meter_base_port+cae_meter_port_range)+1;
    }
  }

  //
  // Initialize Data Structures
  //
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_PORTS;j++) {
      input_status[i][j]=false;
      for(unsigned k=0;k<2;k++) {
	cae_input_levels[i][j][k]=-10000;
	cae_output_levels[i][j][k]=-10000;
	//	cae_stream_output_levels[i][j][k]=-10000;
      }
      for(int k=0;k<RD_MAX_STREAMS;k++) {
	cae_output_status_flags[i][j][k]=false;
      }
    }
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      cae_handle[i][j]=-1;
      cae_output_positions[i][j]=0;
    }
  }

  //
  // The Clock Timer
  //
  QTimer *timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(clockData()));
  timer->start(RD_METER_UPDATE_INTERVAL);
}


RDCae::~RDCae() {
  delete cae_socket;
  //  close(cae_socket);
}


void RDCae::connectToHost(int timeout_msecs)
{
  if(timeout_msecs<0) {
    timeout_msecs=RD_CAED_TIMEOUT_INTERVAL;
  }
  if(timeout_msecs>0) {
    SendCommand(QString::asprintf("TO %d",timeout_msecs));
    cae_keepalive_timer->start(timeout_msecs/3);
  }
}

/*
void RDCae::connectHost()
{
  int count=10;
  QTimer *timer=new QTimer(this);
  struct sockaddr_in sa;

  connect(timer,SIGNAL(timeout()),this,SLOT(readyData()));
  timer->start(CAE_POLL_INTERVAL);
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_port=htons(CAED_TCP_PORT);
  sa.sin_addr.s_addr=htonl(cae_station->caeAddress(cae_config).toIPv4Address());
  while((::connect(cae_socket,(struct sockaddr *)(&sa),sizeof(sa))!=0)&&
	(--count>0)) {
    usleep(100000);
  } 
  usleep(100000);
  if(count>0) {
    SendCommand("PW "+cae_config->password()+"!");
    for(int i=0;i<RD_MAX_CARDS;i++) {
      SendCommand(QString::asprintf("TS %d!",i));
      for(int j=0;j<RD_MAX_PORTS;j++) {
	SendCommand(QString::asprintf("IS %d %d!",i,j));
      }
    }
  }
}
*/


void RDCae::enableMetering(QList<int> *cards)
{
  QString cmd=QString::asprintf("ME %u",cae_meter_socket->localPort());
  for(int i=0;i<cards->size();i++) {
    if(cards->at(i)>=0) {
      bool found=false;
      for(int j=0;j<i;j++) {
	if(cards->at(i)==cards->at(j)) {
	  found=true;
	}
      }
      if(!found) {
	cmd+=QString::asprintf(" %d",cards->at(i));
      }
    }
  }
  SendCommand(cmd);
}


int RDCae::startPlayback(const QString &cutname,int cardnum,int portnum,
			 int start_pos,int end_pos,int speed)
{
  int serial=cae_next_serial_number++;

  cae_stream_output_levels[serial]=new __RDCaeMeterPoint();
  SendCommand(QString::asprintf("PY %d %s %d %d %d %d %d",
				serial,cutname.toUtf8().constData(),
				cardnum,portnum,start_pos,end_pos,100000));
  emit playStarted(serial);

  return serial;
}


void RDCae::positionPlay(int serial,int pos)
{
  SendCommand(QString::asprintf("PP %d %d",serial,pos));
  emit playPositioned(serial,pos);
}


void RDCae::pausePlayback(int serial)
{
  SendCommand(QString::asprintf("PE %d",serial));
}


void RDCae::resumePlayback(int serial)
{
  SendCommand(QString::asprintf("PR %d",serial));
}


void RDCae::stopPlayback(int serial)
{
  SendCommand(QString::asprintf("SP %d",serial));
  if(cae_stream_output_levels.value(serial)!=NULL) {
    delete cae_stream_output_levels.value(serial);
    cae_stream_output_levels.remove(serial);
  }
  emit playbackStopped(serial);
}

/*
bool RDCae::loadPlay(int card,QString name,int *stream,int *handle)
{
#ifdef DEBUG_LATENCY
  rda->syslog(LOG_NOTICE,"RDCae::loadPlay() starts",handle);
#endif  // DEBUG_LATENCY

  int count=0;

  SendCommand("LP "+QString::asprintf(" %d ",card)+name+"!");

  //
  // This is really warty, but needed to make the method 'synchronous'
  // with respect to CAE.
  //
  *stream=-2;
  *handle=-1;
  while(*stream==-2) {
    readyData(stream,handle,name);
    usleep(1000);
    count++;
  }
  if(count>1000) {
    rda->syslog(LOG_ERR,
		"*** LoadPlay: CAE took %d mS to return stream for %s ***",
		count,name.toUtf8().constData());
  }
  cae_handle[card][*stream]=*handle;
  cae_pos[card][*stream]=0xFFFFFFFF;

  // CAE Daemon sends back a stream of -1 if there is an issue with allocating it
  // such as file missing, etc.
  if(*stream < 0) {
      return false;
  }

#ifdef DEBUG_LATENCY
  rda->syslog(LOG_NOTICE,"RDCae::loadPlay(handle=%d) ends",*handle);
#endif  // DEBUG_LATENCY

  return true;
}


void RDCae::unloadPlay(int handle)
{
#ifdef DEBUG_LATENCY
  rda->syslog(LOG_NOTICE,"RDCae::unloadPlay(handle=%d) starts",handle);
#endif  // DEBUG_LATENCY
  SendCommand(QString::asprintf("UP %d!",handle));
}


void RDCae::positionPlay(int handle,int msec)
{
  if(msec<0) {
    return;
  }
  SendCommand(QString::asprintf("PP %d %u!",handle,msec));
}


void RDCae::play(int handle,unsigned length,int speed,bool pitch)
{
#ifdef DEBUG_LATENCY
  rda->syslog(LOG_NOTICE,"RDCae::play(handle=%d) starts",handle);
#endif  // DEBUG_LATENCY

  int pitch_state=0;

  if(pitch) {
    pitch_state=1;
  }
  SendCommand(QString::asprintf("PY %d %u %d %d!",
				handle,length,speed,pitch_state));
}


void RDCae::stopPlay(int handle)
{
#ifdef DEBUG_LATENCY
  rda->syslog(LOG_NOTICE,"RDCae::stopPlay(handle=%d) starts",handle);
#endif  // DEBUG_LATENCY
  SendCommand(QString::asprintf("SP %d!",handle));
}
*/

void RDCae::loadRecord(int card,int stream,QString name,
		       AudioCoding coding,int chan,int samp_rate,
		       int bit_rate)
{
  // printf("RDCae::loadRecord(%d,%d,%s,%d,%d,%d,%d)\n",
  //	 card,stream,(const char *)name,coding,chan,samp_rate,bit_rate);
  SendCommand(QString::asprintf("LR %d %d %d %d %d %d %s",
				card,stream,(int)coding,chan,samp_rate,
				bit_rate,name.toUtf8().constData()));
}


void RDCae::unloadRecord(int card,int stream)

{
  SendCommand(QString::asprintf("UR %d %d",card,stream));
}


void RDCae::record(int card,int stream,unsigned length,int threshold)
{
  SendCommand(QString::asprintf("RD %d %d %u %d",
				card,stream,length,threshold));
}


void RDCae::stopRecord(int card,int stream)
{
  SendCommand(QString::asprintf("SR %d %d",card,stream));
}


void RDCae::setOutputVolume(int card,int stream,int port,int level)
{
  SendCommand(QString::asprintf("OV %d %d %d %d",card,stream,port,level));
}


void RDCae::setOutputPort(int card,int stream,int port)
{
  SendCommand(QString::asprintf("OP %d %d %d 0",card,stream,port));
}


void RDCae::fadeOutputVolume(int card,int stream,int port,int level,int length)
{
  SendCommand(QString::asprintf("FV %d %d %d %d %d",
				card,stream,port,level,length));
}


void RDCae::setPassthroughVolume(int card,int in_port,int out_port,int level)
{
  SendCommand(QString::asprintf("AL %d %d %d %d",card,in_port,out_port,level));
}


bool RDCae::inputStatus(int card,int port) const
{
  return input_status[card][port];
}


void RDCae::inputMeterUpdate(int card,int port,short levels[2])
{
  UpdateMeters();
  levels[0]=cae_input_levels[card][port][0];
  levels[1]=cae_input_levels[card][port][1];
}


void RDCae::outputMeterUpdate(int card,int port,short levels[2])
{
  UpdateMeters();
  levels[0]=cae_output_levels[card][port][0];
  levels[1]=cae_output_levels[card][port][1];
}


void RDCae::outputStreamMeterUpdate(int serial,short levels[2])
{
  __RDCaeMeterPoint *mp=NULL;

  UpdateMeters();
  if((mp=cae_stream_output_levels.value(serial))!=NULL) {
    levels[0]=mp->levels[0];
    levels[1]=mp->levels[1];
  }
  //  levels[0]=cae_stream_output_levels[card][stream][0];
  //  levels[1]=cae_stream_output_levels[card][stream][1];
}


unsigned RDCae::playPosition(int handle)
{
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      if(cae_handle[i][j]==handle) {
	return cae_pos[i][j];
      }
    }
  }
  return 0;
}


void RDCae::requestTimescale(int card)
{
  //  SendCommand(QString::asprintf("TS %d!",card));
}


bool RDCae::playPortActive(int card,int port,int except_stream)
{
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    if(cae_output_status_flags[card][port][i]&&(i!=except_stream)) {
      return true;
    }
  }
  return false;
}


void RDCae::setPlayPortActive(int card,int port,int stream)
{
  cae_output_status_flags[card][port][stream]=true;
}


void RDCae::readyReadData()
{
  char data[1501];
  int n;

  while((n=cae_socket->readDatagram(data,1500))>0) {
    ProcessCommand(QString::fromUtf8(QByteArray(data,n)));
  }
}

/*
void RDCae::readyData()
{
  readyData(0,0,"");
}


void RDCae::readyData(int *stream,int *handle,QString name)
{
  char buf[256];
  int c;
  RDCmdCache cmd;

  if(stream==NULL) {
    for(unsigned i=0;i<delayed_cmds.size();i++) {
      DispatchCommand(&delayed_cmds[i]);
    }
    delayed_cmds.clear();
  }

  while((c=read(cae_socket,buf,256))>0) {
    buf[c]=0;
    for(int i=0;i<c;i++) {
      if(buf[i]==' ') {
	if(argnum<CAE_MAX_ARGS) {
	  args[argnum][argptr]=0;
	  argnum++;
	  argptr=0;
	}
	else {
	  if(debug) {
	    printf("Argument list truncated!\n");
	  }
	}
      }
      if(buf[i]=='!') {
	args[argnum++][argptr]=0;
	if(stream==NULL) {
	  cmd.load(args,argnum,argptr);
	  //
	  // ************************************
	  // printf("DISPATCHING: ");
	  //for(int z=0;z<cmd.argNum();z++) {
	  //  printf(" %s",cmd.arg(z));
	  // }
	  printf("\n");
	  // ************************************
	  //
	  DispatchCommand(&cmd);
	}
	else {
	  if(!strcmp(args[0],"LP")) {
	    if(QString(args[2])==name) {
	      sscanf(args[3],"%d",stream);
	      sscanf(args[4],"%d",handle);
	    }
	  }
	  else {
	    cmd.load(args,argnum,argptr);
	    delayed_cmds.push_back(cmd);
	  }
	}
	argnum=0;
	argptr=0;
	if(cae_socket==-1) {
	  return;
	}
      }
      if((isgraph(buf[i]))&&(buf[i]!='!')) {
	if(argptr<CAE_MAX_LENGTH) {
	  args[argnum][argptr]=buf[i];
	  argptr++;
	}
	else {
	  if(debug) {
	    printf("WARNING: argument truncated!\n");
	  }
	}
      }
    }
  }
}
*/

void RDCae::clockData()
{
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      if(cae_handle[i][j]>=0) {
	if(cae_output_positions[i][j]!=cae_pos[i][j]) {
	  emit playPositionChanged(cae_handle[i][j],
				   cae_output_positions[i][j]);
	  cae_pos[i][j]=cae_output_positions[i][j];
	}
      }
    }
  }
}


void RDCae::keepaliveData()
{
  SendCommand("TH");
}


void RDCae::SendCommand(const QString &cmd)
{
  cae_socket->writeDatagram(cmd.toUtf8(),
		   rda->station()->caeAddress(rda->config()),RD_CAED_PORT);
}

/*
void RDCae::SendCommand(QString cmd)
{
  int len=cmd.toUtf8().length();
  int n=write(cae_socket,cmd.toUtf8(),cmd.toUtf8().length());
  if(n!=len) {
    rda->syslog(LOG_WARNING,"RDCae lost %d bytes when sending \"%s\"",
		len-n,cmd.toUtf8().constData());
  }
}
*/

void RDCae::ProcessCommand(const QString &cmd)
{
  QStringList f0=cmd.split(" ",QString::SkipEmptyParts);
  bool ok=false;
  int serial;

  if((f0.at(0)=="SP")&&(f0.size()==2)) {   // Playback Stopped
    serial=f0.at(1).toInt(&ok);
    if(ok&&(serial>0)) {
      if(cae_stream_output_levels.value(serial)!=NULL) {
	delete cae_stream_output_levels.value(serial);
	cae_stream_output_levels.remove(serial);
      }
      emit playbackStopped(serial);
    }
  }
}

/*
void RDCae::DispatchCommand(RDCmdCache *cmd)
{
  int pos;
  int card;

  if(!strcmp(cmd->arg(0),"PW")) {   // Password Response
    if(cmd->arg(1)[0]=='+') {
      emit isConnected(true);
    }
    else {
      emit isConnected(false);
    }
  }

  if(!strcmp(cmd->arg(0),"LP")) {   // Load Play
    int handle=GetHandle(cmd->arg(4));
    int card=CardNumber(cmd->arg(1));
    int stream=StreamNumber(cmd->arg(3));
    rda->syslog(LOG_ERR,"*** RDCae::DispatchCommand: received unhandled play stream from CAE, handle=%d, card=%d, stream=%d, name=\"%s\" ***",
		handle,card,stream,cmd->arg(2));
    
    unloadPlay(handle);
  }

  if(!strcmp(cmd->arg(0),"UP")) {   // Unload Play
    if(cmd->arg(2)[0]=='+') {
      int handle=GetHandle(cmd->arg(1));
      for(int i=0;i<RD_MAX_CARDS;i++) {
	for(int j=0;j<RD_MAX_STREAMS;j++) {
	  if(cae_handle[i][j]==handle) {
	    cae_handle[i][j]=-1;
	    for(unsigned k=0;k<RD_MAX_PORTS;k++) {
	      cae_output_status_flags[i][k][j]=false;
	    }
	  }
	}
      }
#ifdef DEBUG_LATENCY
  rda->syslog(LOG_NOTICE,"RDCae::unloadPlay(handle=%d) ends ",
	      GetHandle(cmd->arg(1)));
#endif  // DEBUG_LATENCY
      emit playUnloaded(handle);
    }
  }

  if(!strcmp(cmd->arg(0),"PP")) {   // Position Play
    if(cmd->arg(3)[0]=='+') {
      int handle=GetHandle(cmd->arg(1));
      sscanf(cmd->arg(2),"%u",&pos);
      for(int i=0;i<RD_MAX_CARDS;i++) {
	for(int j=0;j<RD_MAX_STREAMS;j++) {
	  if(cae_handle[i][j]==handle) {
	    emit playPositionChanged(handle,pos);
	  }
	}
      }
      emit playPositioned(handle,pos);
    }
  }

  if(!strcmp(cmd->arg(0),"PY")) {   // Play
    if(cmd->arg(4)[0]=='+') {
#ifdef DEBUG_LATENCY
      rda->syslog(LOG_NOTICE,"RDCae::play(handle=%d) ends",
		  GetHandle(cmd->arg(1)));
#endif  // DEBUG_LATENCY
      emit playing(GetHandle(cmd->arg(1)));
    }
  }

  if(!strcmp(cmd->arg(0),"SP")) {   // Stop Play
    if(cmd->arg(2)[0]=='+') {
#ifdef DEBUG_LATENCY
  rda->syslog(LOG_NOTICE,"RDCae::stopPlay(handle=%d) ends ",
	      GetHandle(cmd->arg(1)));
#endif  // DEBUG_LATENCY
      emit playStopped(GetHandle(cmd->arg(1)));
    }
  }

  if(!strcmp(cmd->arg(0),"TS")) {   // Timescale Supported
    if(sscanf(cmd->arg(1),"%d",&card)==1) {
      if(cmd->arg(2)[0]=='+') {
	emit timescalingSupported(card,true);
      }
      else {
	emit timescalingSupported(card,false);
      }
    }
  }

  if(!strcmp(cmd->arg(0),"LR")) {   // Load Record
    if(cmd->arg(8)[0]=='+') {
      emit recordLoaded(CardNumber(cmd->arg(1)),StreamNumber(cmd->arg(2)));
    }
  }

  if(!strcmp(cmd->arg(0),"UR")) {   // Unload Record
    if(cmd->arg(4)[0]=='+') {
      emit recordUnloaded(CardNumber(cmd->arg(1)),StreamNumber(cmd->arg(2)),
			  QString(cmd->arg(3)).toUInt());
    }
  }

  if(!strcmp(cmd->arg(0),"RD")) {   // Record
  }

  if(!strcmp(cmd->arg(0),"RS")) {   // Record Start
    if(cmd->arg(3)[0]=='+') {
      emit recording(CardNumber(cmd->arg(1)),StreamNumber(cmd->arg(2)));
    }
  }

  if(!strcmp(cmd->arg(0),"SR")) {   // Stop Record
    if(cmd->arg(3)[0]=='+') {
      emit recordStopped(CardNumber(cmd->arg(1)),StreamNumber(cmd->arg(2)));
    }
  }

  if(!strcmp(cmd->arg(0),"IS")) {   // Input Status
    switch(cmd->arg(3)[0]) {
	case '0':
	  emit inputStatusChanged(CardNumber(cmd->arg(1)),
				  StreamNumber(cmd->arg(2)),true);
	  input_status[CardNumber(cmd->arg(1))][StreamNumber(cmd->arg(2))]=
	    true;
	  break;

	case '1':
	  emit inputStatusChanged(CardNumber(cmd->arg(1)),
				  StreamNumber(cmd->arg(2)),false);
	  input_status[CardNumber(cmd->arg(1))][StreamNumber(cmd->arg(2))]=
	    false;
	  break;
    }
  }
}
*/

int RDCae::CardNumber(const char *arg)
{
  int n=-1;

  sscanf(arg,"%d",&n);
  return n;
}


int RDCae::StreamNumber(const char *arg)
{
  int n=-1;

  sscanf(arg,"%d",&n);
  return n;
}


int RDCae::GetHandle(const char *arg)
{
  int n=-1;

  sscanf(arg,"%d",&n);
  return n;
}


void RDCae::UpdateMeters()
{
  char msg[1501];
  int n;
  QStringList args;
  __RDCaeMeterPoint *mp=NULL;

  while((n=cae_meter_socket->readDatagram(msg,1500))>0) {
    msg[n]=0;
    args=QString(msg).split(" ");
    if(args[0]=="ML") {
      if(args.size()==6) {
	if(args[1]=="I") {
	  cae_input_levels[args[2].toInt()][args[3].toInt()][0]=args[4].toInt();
	  cae_input_levels[args[2].toInt()][args[3].toInt()][1]=args[5].toInt();
	}
	if(args[1]=="O") {
	  cae_output_levels[args[2].toInt()][args[3].toInt()][0]=
	    args[4].toInt();
	  cae_output_levels[args[2].toInt()][args[3].toInt()][1]=
	    args[5].toInt();
	}
      }
    }
    if(args[0]=="MO") {
      if(args.size()==4) {
	if((mp=cae_stream_output_levels.value(args[1].toInt()))!=NULL) {
	  mp->levels[0]=args[2].toInt();
	  mp->levels[1]=args[3].toInt();
	}
      }
    }
    if(args[0]=="MP") {
      if(args.size()==4) {
	cae_output_positions[args[1].toInt()][args[2].toInt()]=args[3].toUInt();
      }
    }
  }
}
