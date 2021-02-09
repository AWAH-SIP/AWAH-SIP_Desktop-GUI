/*
 * Copyright (C) 2016 - 2021 Andy Weiss, Adi Hilber
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef AWAHSIPDESKTOPGUI_H
#define AWAHSIPDESKTOPGUI_H

#include <QMainWindow>
#include <QtGui>
#include <QtWidgets>
#include <QMessageBox>
#include <QDebug>
#include "sipStateModel.h"
#include "audioroutemodel.h"
#include "route.h"
#include "logwindow.h"
#include "cmdfacade.h"
#include "websocketclient.h"

#define APPVERSION


namespace Ui {
class AWAHSipDesktopGUI;
}



class AWAHSipDesktopGUI : public QMainWindow
{
    Q_OBJECT


public:
    explicit AWAHSipDesktopGUI(QWidget *parent = 0);
    ~AWAHSipDesktopGUI();
    void Restart(bool Abort);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::AWAHSipDesktopGUI *ui;
    CmdFacade *m_cmdFacade;
    WebsocketClient *m_websocketClient;
    int callId;
    QString autoRegister;
    SipStateModel *SIPstate;
    AudioRouteModel *m_AudioRoutesModel;
    RotatedHeaderView *m_RotHeaderView;
    QList<s_account> *m_Accounts;
    Route *m_uiRouteWindow;
    LogWindow *m_uiLogWindow;




private slots:
    void Onsip_status(int accountid, int sipstatus ,QString statusText);
    void OnNewMessage(QString type, QByteArray message);

    void on_actionAudi_I_O_triggered();
    void on_actionAWAH_triggered();
    void on_actionAccounts_triggered();
    void on_actionRouting_triggered();
    void on_actionSettings_triggered();
    void on_actionLog_triggered();
    void on_actionBuddies_triggered();
};

#endif // AWAHSIPDESKTOPGUI_H
