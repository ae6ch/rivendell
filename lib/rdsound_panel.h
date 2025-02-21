// rdsound_panel.h
//
// The sound panel widget
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

#ifndef RDSOUND_PANEL_H
#define RDSOUND_PANEL_H

#include <QByteArray>
#include <QDateTime>
#include <QLabel>
#include <QList>
#include <QMap>
#include <QSignalMapper>

#include <rdbutton_dialog.h>
#include <rdbutton_panel.h>
#include <rdcart.h>
#include <rdcart_dialog.h>
#include <rdcombobox.h>
#include <rdevent_player.h>
#include <rdpanel_button.h>
#include <rdpushbutton.h>
#include <rdwidget.h>

//
// Widget Settings
//
#define PANEL_BUTTON_SIZE_X 88
#define PANEL_BUTTON_SIZE_Y 80
#define PANEL_SCAN_INTERVAL 10000

class RDSoundPanel : public RDWidget
{
 Q_OBJECT
 public:
  RDSoundPanel(int station_panels,int user_panels,bool flash,
	       const QString &caption,const QString &label_template,
	       bool extended,RDEventPlayer *player,RDCartDialog *cart_dialog,
	       QWidget *parent=0);
  ~RDSoundPanel();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  bool dumpPanelUpdates() const;
  void setDumpPanelUpdates(bool state);
  int card(int outnum) const;
  void setCard(int outnum,int card);
  int port(int outnum) const;
  void setPort(int outnum,int port);
  QString outputText(int outnum) const;
  void setOutputText(int outnum,const QString &text);
  void setRmls(int outnum,const QString &start_rml,const QString &stop_rml);
  void setLogName(const QString &logname);
  void setSvcName(const QString &svcname);
  void play(RDAirPlayConf::PanelType type,int panel,int row,int col,
	    RDLogLine::StartSource src,int mport=-1,
	    bool pause_when_finished=false);
  bool pause(RDAirPlayConf::PanelType type,int panel,int row,int col,
	     int mport=-1);
  void stop(RDAirPlayConf::PanelType type,int panel,int row,int col,
  int mport=-1,bool pause_when_finished=false,int fade_out=0);
  void channelStop(int mport);
  void setText(RDAirPlayConf::PanelType type,int panel,int row,int col,
	       const QString &str);
  void setColor(RDAirPlayConf::PanelType type,int panel,int row,int col,
		const QColor &color);
  void duckVolume(RDAirPlayConf::PanelType type,int panel,int row,int col,
		  int level,int fade,int mport=-1);
  RDAirPlayConf::ActionMode actionMode() const;
  void setActionMode(RDAirPlayConf::ActionMode mode);
  bool pauseEnabled() const;
  void setPauseEnabled(bool state);
  int currentNumber() const;
  RDAirPlayConf::PanelType currentType() const;
  QByteArray json(const QString &owner) const;

 public slots:
  void setButton(RDAirPlayConf::PanelType type,int panel,int row,int col,
		 unsigned cartnum,const QString &title="");
  void acceptCartDrop(int row,int col,unsigned cartnum,const QColor &color,
		      const QString &);
  void changeUser();
  void tickClock();
  void panelUp();
  void panelDown();

 signals:
  void tick();
  void buttonFlash(bool state);
  void selectClicked(unsigned cartnum,int row,int col);
  void channelStarted(int mport,int card,int port);
  void channelStopped(int mport,int card,int port);

 private slots:
  void panelActivatedData(int n);
  void playmodeActivatedData(int n);
  void resetClickedData();
  void allClickedData();
  void setupClickedData();
  void buttonClickedData(int pnum,int col,int row);
  void stateChangedData(int id,RDPlayDeck::State state);
  void hookEndData(int id);
  void timescalingSupportedData(int card,bool state);
  void panelSetupData();
  void onairFlagChangedData(bool state);
  void notificationReceivedData(RDNotification *notify);

 protected:
  void resizeEvent(QResizeEvent *e);
  void wheelEvent(QWheelEvent *e);

 private:
  void PlayButton(RDAirPlayConf::PanelType type,int panel,int row,int col,
		  RDLogLine::StartSource src,bool hookmode,int mport=-1,
                  bool pause_when_finished=false);
  bool PlayAudio(RDPanelButton *button,RDCart *cart,bool hookmode,int mport=-1);
  void PlayMacro(RDPanelButton *button,RDCart *cart);
  void PauseButton(RDAirPlayConf::PanelType type,int panel,int row,int col,
		   int mport=-1);
  void StopButton(RDAirPlayConf::PanelType type,int panel,int row,int col,
               int mport=-1,bool pause_when_finished=false,int fade_out=0);
  void StopButton(int id);
  void StopButton(RDPlayDeck *deck);
  void UpdatePanels(const QString &username);
  void ShowPanel(RDAirPlayConf::PanelType type,int offset);
  void SaveButton(RDAirPlayConf::PanelType type,int panel,int row,int col);
  int GetFreeButtonDeck();
  int GetFreeOutput();
  void LogPlayEvent(unsigned cartnum,int cutnum);
  void LogTraffic(RDPanelButton *button);
  void LogTrafficMacro(RDPanelButton *button);
  void LogLine(QString str);
  void Playing(int id);
  void Paused(int id);
  void Stopped(int id);
  void ClearChannel(int id);
  void ClearReset();
  QString PanelTag(int index);
  QString PanelOwner(RDAirPlayConf::PanelType type);
  QString PanelName(RDAirPlayConf::PanelType type,int panel_num);
  RDPanelButton *GetVisibleButton(int row,int col) const;
  QString ButtonSqlFields() const;
  void ApplyButtonFields(RDPanelButton *button,RDSqlQuery *q);
  void UpdateButton(RDPanelButton *button);
  QMap<QString,QList<RDButtonPanel *> > panel_arrays;
  RDButtonPanel *panel_current_panel;
  RDComboBox *panel_selector_box;
  QComboBox *panel_playmode_box;
  RDPushButton *panel_setup_button;
  RDPushButton *panel_reset_button;
  RDPushButton *panel_all_button;
  RDAirPlayConf::PanelType panel_type;
  RDButtonDialog *panel_button_dialog;
  QString panel_tablename;
  QString panel_name_tablename;
  int panel_number;
  int panel_station_panels;
  int panel_user_panels;
  QString panel_label_template;
  bool panel_setup_mode;
  bool panel_reset_mode;
  QWidget *panel_parent;
  int panel_card[RD_SOUNDPANEL_MAX_OUTPUTS];
  int panel_port[RD_SOUNDPANEL_MAX_OUTPUTS];
  int panel_cue_card;
  int panel_cue_port;
  QString panel_output_text[RD_SOUNDPANEL_MAX_OUTPUTS];
  QString panel_start_rml[RD_SOUNDPANEL_MAX_OUTPUTS];
  QString panel_stop_rml[RD_SOUNDPANEL_MAX_OUTPUTS];
  RDPanelButton *panel_active_buttons[RD_MAX_STREAMS];
  bool panel_flash;
  int panel_flash_count;
  bool panel_flash_state;
  QString panel_logfile;
  bool panel_timescaling_supported[RD_SOUNDPANEL_MAX_OUTPUTS];
  RDEventPlayer *panel_event_player;
  QString panel_svcname;
  RDAirPlayConf::ActionMode panel_action_mode;
  bool panel_pause_enabled;
  int panel_button_columns;
  int panel_button_rows;
  RDCartDialog *panel_cart_dialog;
  bool panel_onair_flag;
  QString panel_caption;
  bool panel_dump_panel_updates;
  bool panel_started;
};

#endif  // RDSOUND_PANEL_H
