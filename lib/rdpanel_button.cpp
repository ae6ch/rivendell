// rdpanel_button.cpp
//
// Component class for sound panel widgets.
//
//   (C) Copyright 2002-2024 Fred Gleason <fredg@paravelsystems.com>
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

#include <QDrag>
#include <QJsonObject>
#include <QPainter>

#include <rdcartdrag.h>
#include <rdconf.h>

#include "rdpanel_button.h"

RDPanelButton::RDPanelButton(int row,int col,RDStation *station,bool flash,
			     QWidget *parent)
  : RDPushButton(parent)
{
  button_db_id=-1;
  button_row=row;
  button_col=col;
  button_station=station;
  button_parent=parent;
  button_flash=flash;
  button_flash_state=false;
  button_hook_mode=false;
  button_move_count=-1;
  button_allow_drags=false;
  clear();
  setFocusPolicy(Qt::NoFocus);
}


void RDPanelButton::clear()
{
  setText("");
  setOutputText("");
  button_color=Qt::lightGray;
  button_default_color=Qt::lightGray;
  setCart(0);
  button_db_id=-1;
  button_deck=-1;
  button_output=-1;
  button_play_deck=NULL;
  button_start_time=QTime();
  button_end_time=QTime();
  button_active_length=0;
  button_length[0]=0;
  button_length[1]=0;
  button_msecs=-1;
  button_flashing=false;
  button_flash_state=false;
  button_state=false;
  button_start_source=RDLogLine::StartUnknown;
  button_pause_when_finished=false;
  button_duck_volume=0;
}


int RDPanelButton::row() const
{
  return button_row;
}


int RDPanelButton::column() const
{
  return button_col;
}


int RDPanelButton::dbId() const
{
  return button_db_id;
}


void RDPanelButton::setDbId(int id)
{
  button_db_id=id;
}


QString RDPanelButton::text() const
{
  return button_text;
}


void RDPanelButton::setText(const QString &text)
{
  button_text=text;
  WriteKeycap(-1);
}


QString RDPanelButton::outputText() const
{
  return button_output_text;
}


void RDPanelButton::setOutputText(const QString &text)
{
  button_output_text=text;
}


bool RDPanelButton::state() const
{
  return button_state;
}


void RDPanelButton::setState(bool state)
{
  button_state=state;
}


QColor RDPanelButton::defaultColor() const
{
  return button_default_color;
}


void RDPanelButton::setDefaultColor(QColor color)
{
  button_default_color=color;
  if(playDeck()==NULL) {
    setColor(color);
  }
}


RDLogLine::StartSource RDPanelButton::startSource() const
{
  return button_start_source;
}


void RDPanelButton::setStartSource(RDLogLine::StartSource src)
{
  button_start_source=src;
}


void RDPanelButton::setColor(QColor color)
{
  button_color=color;
  if(button_flash) {
    if(color==palette().color(QPalette::Background)) {
      button_flashing=false;
    }
    else {
      button_flashing=true;
    }
  }
  WriteKeycap(button_msecs);
}


unsigned RDPanelButton::cart() const
{
  return button_cart;
}


void RDPanelButton::setCart(unsigned cart)
{
  button_cart=cart;
}


QString RDPanelButton::cutName() const
{
  return button_cutname;
}


void RDPanelButton::setCutName(const QString &name)
{
  button_cutname=name;
}


int RDPanelButton::deck() const
{
  return button_deck;
}


void RDPanelButton::setDeck(int deck)
{
  button_deck=deck;
}


RDPlayDeck *RDPanelButton::playDeck() const
{
  return button_play_deck;
}


void RDPanelButton::setPlayDeck(RDPlayDeck *deck)
{
  button_play_deck=deck;
}


QTime RDPanelButton::startTime() const
{
  return button_start_time;
}


void RDPanelButton::setStartTime(QTime time)
{
  button_start_time=time;
  button_end_time=button_start_time.addMSecs(button_active_length);
}


bool RDPanelButton::hookMode() const
{
  return button_hook_mode;
}


void RDPanelButton::setHookMode(bool state)
{
  button_hook_mode=state;
}


int RDPanelButton::output() const
{
  return button_output;
}


void RDPanelButton::setOutput(int outnum)
{
  button_output=outnum;
}


int RDPanelButton::length(bool hookmode) const
{
  return button_length[hookmode];
}


void RDPanelButton::setLength(bool hookmode, int msecs)
{
  button_length[hookmode]=msecs;
}


void RDPanelButton::setActiveLength(int msecs)
{
  button_active_length=msecs;
  button_end_time=button_start_time.addMSecs(button_active_length);
  if((button_start_time.isNull())&&(button_cart!=0)) {
    WriteKeycap(-1);
  }
}


void RDPanelButton::reset()
{
  if(button_length[button_hook_mode]>0) {
    setActiveLength(button_length[button_hook_mode]);
  }
  else {
    setActiveLength(button_length[0]);
  }
  button_start_time=QTime();
  button_end_time=QTime();
  setColor(button_default_color);
  if(button_cart!=0) {
    WriteKeycap(-1);
  }
  setDeck(-1);
  button_play_deck=NULL;
}


bool RDPanelButton::pauseWhenFinished() const
{
return button_pause_when_finished;
}


void RDPanelButton::setPauseWhenFinished(bool pause_when_finished)
{
  button_pause_when_finished=pause_when_finished;
}


void RDPanelButton::resetCounter()
{
  WriteKeycap(-1);
}


void RDPanelButton::setAllowDrags(bool state)
{
  button_allow_drags=state;
}


void RDPanelButton::setDuckVolume(int level)
{
  button_duck_volume=level;
}


int RDPanelButton::duckVolume() const
{
  return button_duck_volume;
}


bool RDPanelButton::isEmpty() const
{
  return button_cart==0;
}


bool RDPanelButton::isActive() const
{
  return button_play_deck!=NULL;
}


QJsonValue RDPanelButton::json() const
{
  QJsonObject jo0;

  jo0.insert("column",button_col);
  jo0.insert("row",button_row);
  if(isEmpty()) {
    jo0.insert("cart",QJsonValue());
    jo0.insert("defaultColor",QJsonValue());
    jo0.insert("length",QJsonValue());
    jo0.insert("hookLength",QJsonValue());
    jo0.insert("label",QJsonValue());
  }
  else {
    jo0.insert("cart",(int)button_cart);
    jo0.insert("defaultColor",button_default_color.name());
    jo0.insert("length",RDGetTimeLength(button_length[0],true,false));
    jo0.insert("hookLength",RDGetTimeLength(button_length[1],true,false));
    jo0.insert("label",button_text);
  }

  return jo0;
}


void RDPanelButton::setVisible(bool state)
{
  RDPushButton::setVisible(state);
}




void RDPanelButton::tickClock()
{
  int msecs;
  QTime current_time=
    QTime::currentTime().addMSecs(button_station->timeOffset());
  if((button_start_time.isNull())||(current_time>button_end_time)||
     ((msecs=current_time.msecsTo(button_end_time))==button_msecs)) {
    return;
  }
  button_msecs=msecs;
  WriteKeycap(msecs);
}


void RDPanelButton::flashButton(bool state)
{
  if(state==button_flash_state) {
    return;
  }
  button_flash_state=state;
  if(button_flashing&&button_state) {
    WriteKeycap(button_msecs);
  }
}


void RDPanelButton::keyPressEvent(QKeyEvent *e)
{
  QWidget::keyPressEvent(e);
}


void RDPanelButton::keyReleaseEvent(QKeyEvent *e)
{
  QWidget::keyReleaseEvent(e);
}


void RDPanelButton::resizeEvent(QResizeEvent *e)
{
  setIconSize(QSize(size().width()-2,size().height()-2));
}


void RDPanelButton::mousePressEvent(QMouseEvent *e)
{
  button_move_count=10;
  QPushButton::mousePressEvent(e);
}


void RDPanelButton::mouseMoveEvent(QMouseEvent *e)
{
  button_move_count--;
  if(button_allow_drags&&(button_move_count==0)) {
    QPushButton::mouseReleaseEvent(e);
    QDrag *drag=new QDrag(this);
    RDCartDrag *cd=new RDCartDrag(button_cart,button_text,button_color);
    drag->setMimeData(cd);
    RDCart *cart=new RDCart(button_cart);
    switch(cart->type()) {
    case RDCart::Audio:
      drag->setPixmap(rda->iconEngine()->typeIcon(RDLogLine::Cart));
      break;

    case RDCart::Macro:
      drag->setPixmap(rda->iconEngine()->typeIcon(RDLogLine::Macro));
      break;

    case RDCart::All:
      break;
    }
    drag->exec();
  }
}


void RDPanelButton::mouseReleaseEvent(QMouseEvent *e)
{
  button_move_count=-1;
  QPushButton::mouseReleaseEvent(e);
}


void RDPanelButton::dragEnterEvent(QDragEnterEvent *e)
{
  if(RDCartDrag::canDecode(e->mimeData())&&button_allow_drags&&
     ((button_play_deck==NULL)||(button_play_deck->state()==RDPlayDeck::Stopped))) {
    e->accept();
  }
}


void RDPanelButton::dropEvent(QDropEvent *e)
{
  unsigned cartnum;
  QColor color;
  QString title;

  if(RDCartDrag::decode(e->mimeData(),&cartnum,&color,&title)) {
    emit cartDropped(button_row,button_col,cartnum,color,title);
  }
}


void RDPanelButton::WriteKeycap(int msecs)
{
  QString text=button_text;
  QPixmap *pix=new QPixmap(PANEL_BUTTON_SIZE_X-2,PANEL_BUTTON_SIZE_Y-2);
  QPainter *p=new QPainter(pix);
  if(button_state) {
    if(button_flash) {
      if(button_flash_state) {
	p->fillRect(0,0,pix->width(),pix->height(),button_color);
	p->setPen(RDGetTextColor(button_color));
      }
      else {
	p->fillRect(0,0,pix->width(),pix->height(),button_default_color);
	p->setPen(RDGetTextColor(button_default_color));
      }
    }
    else {
      p->fillRect(0,0,pix->width(),pix->height(),button_color);
      p->setPen(RDGetTextColor(button_color));
    }
  }
  else {
    p->fillRect(0,0,pix->width(),pix->height(),button_color);
    p->setPen(RDGetTextColor(button_color));
  }

  //
  // Button Title
  //
  QFontMetrics m(buttonFont());
  p->setFont(buttonFont());
  
  p->drawText(RDPANEL_BUTTON_MARGIN,m.lineSpacing(),
	      GetNextLine(&text,m,pix->width()-2-3*RDPANEL_BUTTON_MARGIN));
  p->drawText(RDPANEL_BUTTON_MARGIN,2*m.lineSpacing(),
	      GetNextLine(&text,m,pix->width()-2-3*RDPANEL_BUTTON_MARGIN));
  p->drawText(RDPANEL_BUTTON_MARGIN,3*m.lineSpacing(),
	      GetNextLine(&text,m,pix->width()-2-3*RDPANEL_BUTTON_MARGIN));

  //
  // Time Field & Output Text
  //
  if(!button_text.isEmpty()) {
    if(msecs<0) {
      p->setFont(smallTimerFont());
      if(button_pause_when_finished) {
        p->drawText(RDPANEL_BUTTON_MARGIN,pix->height()-2-RDPANEL_BUTTON_MARGIN,"Finished");
        }
      else {
	if(button_active_length>=0) {
	  QString lenstr=RDGetTimeLength(button_active_length+1000,true,false);
	  p->drawText(pix->width()-p->fontMetrics().width(lenstr)-
		      RDPANEL_BUTTON_MARGIN-2,
		      pix->height()-2-RDPANEL_BUTTON_MARGIN,
		      lenstr);
	}
	else {
	  p->drawText(RDPANEL_BUTTON_MARGIN,pix->height()-2-
		      RDPANEL_BUTTON_MARGIN,tr("No Audio"));
	}
      }
    }
    else {
      QString lenstr=RDGetTimeLength(1000+msecs,true,false);
      p->drawText(pix->width()-p->fontMetrics().width(lenstr)-
		  RDPANEL_BUTTON_MARGIN-2,
		  pix->height()-2-RDPANEL_BUTTON_MARGIN,
		  lenstr);
      p->setFont(bigLabelFont());
      p->drawText((pix->width()-p->fontMetrics().width(button_output_text))/2,
		  74*pix->height()/100,
      		  button_output_text);
    }
  }
  p->end();
  setIcon(*pix);
  delete p;
  delete pix;
  update();
}


QString RDPanelButton::WrapText(QString text,int *lines)
{
  QFontMetrics fm(font());
  QString str;
  QString residue = text;
  bool space_found=false;
  int l;

  *lines=0;
  if(!text.isEmpty()) {	  
    while(!residue.isEmpty()) {
      space_found=false;
      for(int i=(int)residue.length();i>=0;i--) {
	      if((i==((int)residue.length()))||(residue.at(i).isSpace())) {
	  if(fm.boundingRect(residue.left(i)).width()<=width()-6) {
	    space_found=true;
	    if(!str.isEmpty()) {
	      str+="\n";
	      if(++*lines==3) {
		return str;
	      }
	    }
	    str+=residue.left(i);
	    if(i==(int)residue.length()) {
	      return str;
	    }
	    residue=residue.right(residue.length()-i-1);
	  }
	}
      }
      if(!space_found) {
	l=residue.length();
	for(int i=l;i>=0;i--) {
	  if(fm.boundingRect(residue.left(i)).width()<=(width()-6)) {
	    if(!str.isEmpty()) {
	      str+="\n";
	      if(++*lines==3) {
		return str;
	      }
	    }
	    str+=residue.left(i);
	    if(i==(int)residue.length()) {
	      return str;
	    }
	    residue=residue.right(residue.length()-i-1);
	  }
	}
      }
    }
  }
  return text;
}


QString RDPanelButton::GetNextLine(QString *str,const QFontMetrics &m,int len)
{
  QString ret;

  for(int i=0;i<str->length();i++) {
    if(m.width(str->left(i))>len) {
      int l=i;
      while((!str->at(l--).isSpace())&&(l>=0));
      if(l>0) {
	ret=str->left(l+1);
	*str=str->right(str->length()-l-1).trimmed();
	return ret;
      }
    }
  }
  ret=*str;
  *str="";
  return ret;
}
