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

    QSettings settings("awah", "AWAHSipDesktopGUI");
    QCoreApplication::setOrganizationName("awah");
    QCoreApplication::setOrganizationDomain("awah.ch");
    QCoreApplication::setApplicationName("AWAH-Sip Desktop GUI");

    qDebug() << "Settings stored under:" << settings.fileName();
    settings.setPath(QSettings::Format::NativeFormat,QSettings::UserScope,"settings/");


    m_cmdFacade = new CmdFacade(this, m_websocketClient);
    m_websocketClient->setCmdFacade(m_cmdFacade);
    m_cmdFacade->initializeVariables();
    ui->setupUi(this);

    SIPstate = new SipStateModel(this, this, m_cmdFacade);
    m_Accounts = m_cmdFacade->getAccounts();
    SIPstate->setActiveAccounts(m_Accounts);

    ui->tableView->setModel(SIPstate);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setColumnWidth(0, 100);
    ui->tableView->setColumnWidth(1, 80);
    ui->tableView->setColumnWidth(2, 200);
    ui->tableView->setColumnWidth(3, 280);
    ui->tableView->setColumnWidth(4, 30);
    ui->tableView->setColumnWidth(5, 30);
    ui->tableView->setColumnWidth(6, 100);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);

    restoreGeometry(settings.value("MainWindow/Geometry").toByteArray());

    connect(m_cmdFacade, &CmdFacade::AccountsChanged, this, &AWAHSipDesktopGUI::AccountsChanged);
    connect(m_cmdFacade, &CmdFacade::StateChanged, this, &AWAHSipDesktopGUI::OnStateChanged);

    connect(ui->tableView, SIGNAL(pressed(QModelIndex)),
            SIPstate, SLOT(onTableClicked(QModelIndex)));

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
    delete m_uiLogWindow;
    //delete SIPstate;
    m_cmdFacade->disconnect();
    delete m_cmdFacade;
    delete m_buddySettings;

}


void AWAHSipDesktopGUI::closeEvent(QCloseEvent *event)
{
    QSettings settings("awah", "AWAHSipDesktopGUI");
    settings.setValue("MainWindow/Geometry", saveGeometry());
    m_websocketClient->closeConnection();
    m_uiLogWindow->close();
    m_uiRouteWindow->close();
    delete SIPstate;
    if (m_IOSettings != nullptr){
        delete m_IOSettings;
    }
    if (m_generalSettings != nullptr){
        delete m_generalSettings;
    }
    if (m_accountSettings != nullptr){
        delete m_accountSettings;
    }
    if (m_buddySettings != nullptr){
        delete m_buddySettings;
    }
    QMainWindow::closeEvent(event);
}

void AWAHSipDesktopGUI::OnStateChanged()
{
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
    m_IOSettings = new IOSettings(this,m_cmdFacade);
    m_IOSettings->setModal(true);
    m_IOSettings->exec();
}



void AWAHSipDesktopGUI::on_actionAccounts_triggered()
{
    m_accountSettings = new accountsettings(this,m_cmdFacade);
    m_accountSettings->setModal(true);
    m_accountSettings->exec();
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
    m_generalSettings = new GeneralSettings (this, m_cmdFacade);
    m_generalSettings->setModal(true);
    m_generalSettings->exec();
}

void AWAHSipDesktopGUI::on_actionLog_triggered()
{
    if(m_uiLogWindow->isHidden()){
        m_uiLogWindow->show();
        QSettings settings("awah", "AWAHSipDesktopGUI");
        m_uiLogWindow->restoreGeometry(settings.value("LogWindow/Geometry").toByteArray());
    }
    else
        m_uiLogWindow->hide();
}

void AWAHSipDesktopGUI::on_actionBuddies_triggered()
{
    m_buddySettings = new BuddySettings (this, m_cmdFacade);
    m_buddySettings->setModal(true);
    m_buddySettings->exec();
}

void AWAHSipDesktopGUI::AccountsChanged(QList<s_account> *Accounts)
{
    m_Accounts = Accounts;
    SIPstate->refresh();
}
