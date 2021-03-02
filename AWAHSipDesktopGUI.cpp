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


#include "AWAHSipDesktopGUI.h"
#include "ui_AWAHSipDesktopGUI.h"
#include "iOsettings.h"
#include "accountsettings.h"
#include "generalsettings.h"



AWAHSipDesktopGUI::AWAHSipDesktopGUI(QWidget *parent, WebsocketClient *WebSocket) :
    QMainWindow(parent),
    ui(new Ui::AWAHSipDesktopGUI),
    m_websocketClient(WebSocket)
{
    //CmdFacade::prepareLib();

    QSettings settings("awah", "AWAHSipDesktopGUI");
    QCoreApplication::setOrganizationName("awah");
    QCoreApplication::setOrganizationDomain("awah.ch");
    QCoreApplication::setApplicationName("AWAH-Sip Desktop GUI");

    qDebug() << "Settings stored under:" << settings.fileName();
    settings.setPath(QSettings::Format::NativeFormat,QSettings::UserScope,"settings/");


    m_cmdFacade = new CmdFacade(this, m_websocketClient);
    m_websocketClient->setCmdFacade(m_cmdFacade);

    ui->setupUi(this);

    SIPstate = new SipStateModel(this, m_cmdFacade);
    m_Accounts = m_cmdFacade->getAccounts();
    SIPstate->setActiveAccounts(m_Accounts);

    ui->tableView->setModel(SIPstate);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setColumnWidth(2, 200);
    ui->tableView->setColumnWidth(3, 250);
    ui->tableView->setColumnWidth(4, 30);
    ui->tableView->setColumnWidth(5, 30);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);

    restoreGeometry(settings.value("MainWindow/Geometry").toByteArray());

    connect(m_cmdFacade, &CmdFacade::AccountsChanged, this, &AWAHSipDesktopGUI::AccountsChanged);

    connect(ui->tableView, SIGNAL(pressed(const QModelIndex &)),
            SIPstate, SLOT(onTableClicked(const QModelIndex &)));

    connect(m_cmdFacade, SIGNAL(signalSipStatus(int, int, QString )),
            SIPstate, SLOT (Onsip_status(int, int,QString)));

    connect(m_cmdFacade, SIGNAL(signalSipStatus(int, int, QString )),
           this, SLOT (Onsip_status(int, int,QString)));

    connect(m_cmdFacade, SIGNAL(callStateChanged(int, int, int,bool,long, int, int, QString,QString)),
           SIPstate, SLOT(OnCallStateChanged(int, int, int,bool,long, int, int, QString,QString)));

//       connect(pjsua, SIGNAL(signalBuddyStatus(QString,int)),
//               this, SIGNAL (buddy_status(QString,int)));

    connect(m_cmdFacade, SIGNAL(signalMessage(QString,QByteArray)),
           this, SLOT(OnNewMessage(QString,QByteArray)));

    m_uiRouteWindow = new Route(m_cmdFacade, this);
    m_uiLogWindow = new LogWindow(this, m_cmdFacade);

    connect(m_cmdFacade, SIGNAL(logMessage(QString)),
           m_uiLogWindow, SLOT(OnNewLogEntry(QString)));

}


AWAHSipDesktopGUI::~AWAHSipDesktopGUI()
{
    delete ui;
    delete m_uiRouteWindow;
    delete SIPstate;
    m_cmdFacade->disconnect();
    delete m_cmdFacade;
}


void AWAHSipDesktopGUI::closeEvent(QCloseEvent *event)
{
    QSettings settings("awah", "AWAHSipDesktopGUI");
    settings.setValue("MainWindow/Geometry", saveGeometry());
    m_websocketClient->closeConnection();
    QMainWindow::closeEvent(event);
}

void AWAHSipDesktopGUI::Onsip_status(int accountid, int sipstatus ,QString statusText)
{
    Q_UNUSED(accountid);
    Q_UNUSED(sipstatus);
    Q_UNUSED(statusText);
    SIPstate->refresh();
}

void AWAHSipDesktopGUI::OnNewMessage(QString type, QByteArray message)
{
   qDebug() << "message recieved" << type << message;
}

void AWAHSipDesktopGUI::on_actionAWAH_triggered()
{
    m_websocketClient->testEcho();
    QMessageBox::information(this,"About","AWAH SIP Client test client \n\nwritten by Adi Hilber and Andy Weiss");
}



void AWAHSipDesktopGUI::on_actionAudi_I_O_triggered()
{
    IOSettings iosetting(this,m_cmdFacade);
    iosetting.setModal(true);
    iosetting.exec();
}



void AWAHSipDesktopGUI::on_actionAccounts_triggered()
{
    accountsettings settings(this,m_cmdFacade);
    settings.setModal(true);
    settings.exec();
    m_Accounts = m_cmdFacade->getAccounts();
    SIPstate->refresh();
}

void AWAHSipDesktopGUI::on_actionRouting_triggered()
{
    if(m_uiRouteWindow->isHidden())
        m_uiRouteWindow->show();
    else
        m_uiRouteWindow->hide();
}


void AWAHSipDesktopGUI::on_actionSettings_triggered()
{
    GeneralSettings settings(this, m_cmdFacade);
    settings.setModal(true);
    settings.exec();
}

void AWAHSipDesktopGUI::on_actionLog_triggered()
{
    if(m_uiLogWindow->isHidden())
        m_uiLogWindow->show();
    else
        m_uiLogWindow->hide();
}

void AWAHSipDesktopGUI::on_actionBuddies_triggered()
{
    QMessageBox::information(this,"Sorry","Sorry you have to wait for a future release");
}

void AWAHSipDesktopGUI::AccountsChanged(QList<s_account> *Accounts)
{
    m_Accounts = Accounts;
    SIPstate->refresh();
}
