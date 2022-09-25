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

#include "makecall.h"
#include "ui_makecall.h"
#include "codecsettings.h"
#include "qmessagebox.h"

MakeCall::MakeCall(QWidget *parent, CmdFacade *lib, int AccID) :
    QDialog(parent),
    ui(new Ui::MakeCall),
    m_cmdFacade(lib), m_AccID(AccID)
{
    ui->setupUi(this);
    m_codecs = m_cmdFacade->getActiveCodecs();
    for(auto& codec : m_codecs){
        ui->selcectCodecBox->addItem(codec.displayName);
    }
    m_callHistoryModel = new CallHistoryModel(this, m_cmdFacade, &m_AccID);
    ui->tableView->setModel(m_callHistoryModel);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setColumnWidth(0, 30);
    ui->tableView->setColumnWidth(1, 30);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);

    m_buddyListModel = new BuddyListModel(this, m_cmdFacade, &m_AccID);
    ui->tableView_2->setModel(m_buddyListModel);
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->horizontalHeader()->setStretchLastSection(true);
    ui->tableView_2->setSelectionMode(QAbstractItemView::NoSelection);

    const s_account* account = m_cmdFacade->getAccountByID(AccID);
    if(!account->CallHistory.isEmpty()) {                           // load newest callhistory entry into ui
        QString URI = account->CallHistory.at(0).callUri;
        int startPos = URI.indexOf("sip:") + 4;
        int endPos = URI.indexOf('@');
        int length = endPos - startPos;
        ui->lineEditNumber->setText(URI.mid(startPos, length));
        m_selectedCodec = account->CallHistory.at(0).codec;
        int index = ui->selcectCodecBox->findText(m_selectedCodec.displayName);
        ui->selcectCodecBox->setCurrentIndex(index);
    }

    connect(ui->tableView, SIGNAL(pressed(const QModelIndex &)),
            m_callHistoryModel, SLOT(onTableClicked(const QModelIndex &)));

    connect(m_callHistoryModel, SIGNAL(SetParamFromHistory(s_codec,QString)),
            this, SLOT(on_ParamChanged(s_codec,QString)));

    connect(ui->tableView_2, SIGNAL(pressed(const QModelIndex &)),
            m_buddyListModel, SLOT(onTableClicked(const QModelIndex &)));

    connect(m_buddyListModel, SIGNAL(SetParamFromHistory(s_codec,QString)),
            this, SLOT(on_ParamChanged(s_codec,QString)));

}

MakeCall::~MakeCall()
{
    delete ui;
}

void MakeCall::on_pushButton_cancel_clicked()
{
    MakeCall::close();
}

void MakeCall::on_pushButton_Call_clicked()
{
    if(ui->selcectCodecBox->currentIndex()== -1){
        QMessageBox::warning(this,"Codec not found","Please select a Codec");
        return;
    }
    m_cmdFacade->makeCall(ui->lineEditNumber->text(),m_AccID,m_selectedCodec);
    MakeCall::close();
}

void MakeCall::on_pushButton_settings_clicked()
{
    CodecSettings settings(m_selectedCodec,this, m_cmdFacade);
    settings.setModal(true);
    settings.exec();
}

void MakeCall::on_ParamChanged(s_codec Codec, QString Number)
{
    ui->lineEditNumber->setText(Number);
    m_selectedCodec = Codec;
    int index = ui->selcectCodecBox->findText(m_selectedCodec.displayName);
    ui->selcectCodecBox->setCurrentIndex(index);
}

void MakeCall::on_selcectCodecBox_currentIndexChanged(int index)
{
    if(index > -1){
        m_selectedCodec = m_codecs.at(index);
    }
}


// ************************** Call histroy model ********************
CallHistoryModel::CallHistoryModel(QObject *parent, CmdFacade *lib, int *AccID)
    : QAbstractTableModel(parent), m_cmdFacade(lib), m_AccID(AccID)
{
    m_account = m_cmdFacade->getAccountByID(*m_AccID);
}

int CallHistoryModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_account->CallHistory.size();
}

int CallHistoryModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 5;
}

QVariant CallHistoryModel::data(const QModelIndex &index, int role) const
{
    int startPos, endPos, length;
    QString URI;
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        switch (index.column()) {
        case 1:
            return  m_account->CallHistory.at(index.row()).count;
            break;
        case 2:
            URI = m_account->CallHistory.at(index.row()).callUri;
            startPos = URI.indexOf("sip:") + 4;
            endPos = URI.indexOf('@');
            length = endPos - startPos;
            if(startPos > 4){               // if the URI has a name extract it
                return QString(URI.mid(1,startPos -8)) + " " + URI.mid(startPos, length);
            }
            else return URI.mid(startPos, length);                   // extract the number of the whole uri

            break;
        case 3:
            return m_account->CallHistory.at(index.row()).codec.displayName;
            break;
        case 4:
            return QDateTime::fromTime_t(m_account->CallHistory.at(index.row()).duration).toUTC().toString("hh:mm:ss");
            break;
        }

    if (role== Qt::DecorationRole){
        QIcon icon;
        switch(index.column()){
        case 0:
            if(m_account->CallHistory.at(index.row()).outgoing){
                icon = QIcon(":/new/icons/images/call [#189].svg");
            }
            else{
                icon = QIcon(":/new/icons/images/call [#190].svg");
            }
            return icon;
            break;
        }
    }
    return QVariant();
}

QVariant CallHistoryModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section) {
        case 1:
            return QString(" ");
            break;
        case 2:
            return QString("Number");
            break;
        case 3:
            return QString("Codec");
            break;
        case 4:
            return QString("duration");
            break;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

void CallHistoryModel::refresh() {
    emit dataChanged(index(0, 0),
                     index(rowCount(), columnCount()));  // update whole view
    emit layoutChanged();
  }

void CallHistoryModel::onTableClicked(const QModelIndex &index)
{
    int startPos, endPos, length;
    QString URI;
    if (index.isValid()) {
        URI = m_account->CallHistory.at(index.row()).callUri;
        startPos = URI.indexOf("sip:") + 4;
        endPos = URI.indexOf('@');
        length = endPos - startPos;
        emit SetParamFromHistory(m_account->CallHistory.at(index.row()).codec,URI.mid(startPos, length));
    }
}


// ************************** Buddy model ********************
BuddyListModel::BuddyListModel(QObject *parent, CmdFacade *lib, int *AccID)
    : QAbstractTableModel(parent), m_cmdFacade(lib), m_AccID(AccID)
{
    m_account = m_cmdFacade->getAccountByID(*m_AccID);
    for (auto && buddy : m_cmdFacade->getBuddies()) {
        s_account* account = m_cmdFacade->getAccountByUID(buddy.accUid);
        if(account != nullptr && account->serverURI == m_account->serverURI){                     // only append buddys that are on the same server
            m_buddies.append(buddy);
        }
    }

}

int BuddyListModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_buddies.size();
}

int BuddyListModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 3;
}

QVariant BuddyListModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        switch (index.column()) {
        case 0:
            return  m_buddies.at(index.row()).Name;
            break;
        case 1:
            return m_buddies.at(index.row()).codec.displayName;

            break;
        case 2:
            switch (m_buddies.at(index.row()).status) {
                case unknown:
                return QString("unknown");
                break;
            case online:
                return QString("online");
                break;
            case busy:
                return QString("busy");
                break;
            default:
                break;
            }
            break;
        }
    if (role == Qt::BackgroundColorRole){
        switch(m_buddies.at(index.row()).status) {
        case unknown:
            return QBrush(QColor(145, 157, 157));   // stone
            break;
        case online:
            return QBrush(QColor(113, 183, 144));   // green
            break;
        case busy:
            return QBrush(QColor(229, 85, 79));     // red
            break;
        }
    }

    return QVariant();
}

QVariant BuddyListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section) {
        case 0:
            return QString("Buddy");
            break;
        case 1:
            return QString("codec");
            break;
        case 2:
            return QString("state");
            break;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

void BuddyListModel::refresh() {
    emit dataChanged(index(0, 0),
                     index(rowCount(), columnCount()));  // update whole view
    emit layoutChanged();
  }

void BuddyListModel::onTableClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        emit SetParamFromHistory(m_buddies.at(index.row()).codec,m_buddies.at(index.row()).buddyUrl);
    }
}


