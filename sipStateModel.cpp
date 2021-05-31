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

#include "sipStateModel.h"
#include <QMessageBox>
#include <QIcon>
#include "makecall.h"
#include "callstatistic.h"

SipStateModel::SipStateModel(QObject *parent, QWidget *parentWidget, CmdFacade *lib)
    : QAbstractTableModel(parent), m_parentWidget(parentWidget), m_cmdFacade(lib)
{

}

SipStateModel::~SipStateModel()
{
    qDeleteAll(*m_CallStatistics);
    m_CallStatistics->clear();
}

void SipStateModel::setActiveAccounts(QList <s_account> *accounts)
{
    m_AccountList = accounts;
}

int SipStateModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_AccountList->count();
}

int SipStateModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 6;
}

QVariant SipStateModel::data(const QModelIndex &index, int role) const
{    
    switch (index.column()) {
    case 0:                                                         // column 0
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return m_AccountList->at(index.row()).name;
        break;

    case 1:                                                         // column 1
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return m_AccountList->at(index.row()).user;
        break;
    case 2:                                                         // column 2
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return QString::number(m_AccountList->at(index.row()).SIPStatusCode) + " " + m_AccountList->at(index.row()).SIPStatusText;
        if(role == Qt::BackgroundColorRole){
            if(m_AccountList->at(index.row()).SIPStatusCode<200)
                return QBrush(QColor(145, 157, 157));   // stone

            else if(m_AccountList->at(index.row()).SIPStatusCode<300)
                return QBrush(QColor(113, 183, 144));   // green

            else if(m_AccountList->at(index.row()).SIPStatusCode>299)
                return QBrush(QColor(229, 85, 79));     // red
        }
        break;

    case 3:                                                        // column 3

        if(m_AccountList->at(index.row()).CallList.isEmpty())
        {
            if (role == Qt::DisplayRole || role == Qt::EditRole){
                return QString("Idle ") + m_AccountList->at(index.row()).callStatusLastReason;
                for(auto& account :* m_AccountList){
                   qDebug() << "last reason " << account.callStatusLastReason;
                }
            }
            if(role == Qt::BackgroundColorRole)
                return QVariant();
            break;
        }

        else
            switch(m_AccountList->at(index.row()).CallList.first()["CallStatusCode"].toInt()){
            case 0:
                if (role == Qt::DisplayRole || role == Qt::EditRole)
                    return QString("Disconnected ");
                if(role == Qt::BackgroundColorRole)
                    return QVariant();
                break;

            case 1:
                if (role == Qt::DisplayRole || role == Qt::EditRole)
                    return "Calling "+ m_AccountList->at(index.row()).CallList.first()["ConnectedTo"].toString();
                if(role == Qt::BackgroundColorRole)
                    return QBrush(QColor(179, 213, 125));  // lime
                break;

            case 2:
                if (role == Qt::DisplayRole || role == Qt::EditRole)
                    return QString("Incoming Call from ") + m_AccountList->at(index.row()).CallList.first()["ConnectedTo"].toString();
                if(role == Qt::BackgroundColorRole)
                    return QBrush(QColor(179, 213, 125));  // lime
                break;

            case 3:
                if (role == Qt::DisplayRole || role == Qt::EditRole)
                    return QString("Connecting ")+ m_AccountList->at(index.row()).CallList.first()["CallStatusText"].toString();
                if(role == Qt::BackgroundColorRole)
                    return QBrush(QColor(179, 213, 125));  // lime
                break;

            case 4:
                if (role == Qt::DisplayRole || role == Qt::EditRole)
                    return "Connecting ";
                if(role == Qt::BackgroundColorRole)
                    return QBrush(QColor(179, 213, 125));  // lime
                break;

            case 5:
                if(m_AccountList->at(index.row()).CallList.count()>1){                 // multiple calls connected
                    if (role == Qt::DisplayRole || role == Qt::EditRole)
                        return QString::number(m_AccountList->at(index.row()).CallList.count()) + " calls connected";
                    if(role == Qt::BackgroundColorRole)
                        return QBrush(QColor(230, 166, 93));  // peach
                }
                else{
                    if (role == Qt::DisplayRole || role == Qt::EditRole)
                        return "Connected to " + m_AccountList->at(index.row()).CallList.first()["ConnectedTo"].toString();

                    if(role == Qt::BackgroundColorRole)
                        return QBrush(QColor(113, 183, 144));   // green
                }
                break;

            case 6:
                if (role == Qt::DisplayRole || role == Qt::EditRole)
                    return QString("Disconnected ") + m_AccountList->at(index.row()).CallList.first()["CallStatusText"].toString();
                if(role == Qt::BackgroundColorRole)
                    return QVariant();
                break;

            case 7:     // RX media lost
                if (role == Qt::DisplayRole || role == Qt::EditRole)
                    return  m_AccountList->at(index.row()).CallList.first()["CallStatusText"].toString();
                if(role == Qt::BackgroundColorRole)
                    return QBrush(QColor(179, 213, 125));  // lime
                break;
            case 8:     // RX media lost
                if (role == Qt::DisplayRole || role == Qt::EditRole)
                    return  QString("trying to hang up");
                if(role == Qt::BackgroundColorRole)
                    return QBrush(QColor(179, 213, 125));  // lime
                break;
            }

        break;
    }

    if (role== Qt::DecorationRole){
        QIcon icon;
        switch(index.column()){
        case 4:
            if(m_AccountList->at(index.row()).CallList.count()== 0){
                icon = QIcon(":/new/icons/images/call [#192].svg");
            }
            else{
                icon = QIcon(":/new/icons/images/call [#188].svg");
            }
            return icon;
            break;
        case 5:
            icon = QIcon(":/new/icons/images/important_message [#1448].svg");
            return icon;
        }
    }
    return QVariant();
}

QVariant SipStateModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section) {
            case 0:
                return QString("Name");
                break;
            case 1:
                return QString("User");
                break;
            case 2:
                return QString("SIP state");
                break;
            case 3:
                return QString("Call State");
                break;
            default:
                return QVariant();
        }
    }

    return QVariant();
}



void SipStateModel::refresh() {
    emit dataChanged(index(0, 0),
                     index(rowCount(), columnCount()));  // update whole view
    emit layoutChanged();
  }

void SipStateModel::onTableClicked(const QModelIndex &index)
{
    QString message;
    if (index.isValid() && index.column()== 4) {

        switch (m_AccountList->at(index.row()).CallList.count()) {
        case 0:
        {
            MakeCall makeCall(m_parentWidget, m_cmdFacade, m_AccountList->at(index.row()).AccID);
            makeCall.setModal(true);
            makeCall.exec();
            break;
        }
        case 1:
        {
            message = "Connected to: " + m_AccountList->at(index.row()).CallList.first()["ConnectedTo"].toString()
                    + "\n\n Do you really want to disconnect this call?";
            int ret = QMessageBox::question(m_parentWidget, tr("AWAHSip Call"),tr(message.toLocal8Bit()));
            if(ret == QMessageBox::Yes){
                m_cmdFacade->hangupCall(m_AccountList->at(index.row()).CallList.first()["callId"].toInt(), m_AccountList->at(index.row()).AccID);
            }
            break;
        }
        default:
            message = QString::number(m_AccountList->at(index.row()).CallList.count()) + " calls connected!! \n\n Do you really want to disconnect all calls?";
            int ret = QMessageBox::question(m_parentWidget, tr("AWAHSip Call"),tr(message.toLocal8Bit()));
            if(ret == QMessageBox::Yes){
                for (int i = 0; i< m_AccountList->at(index.row()).CallList.count(); i++){
                    m_cmdFacade->hangupCall(m_AccountList->at(index.row()).CallList.at(i)["callId"].toInt(), m_AccountList->at(index.row()).AccID);
                }
            }
            break;
        }
    }

   else if (index.isValid() && index.column()== 5) {
       if (m_AccountList->at(index.row()).CallList.count()){
            CallStatistic *callstat = nullptr;
            callstat   = new CallStatistic(m_parentWidget, m_cmdFacade, m_AccountList->at(index.row()).AccID, m_AccountList->at(index.row()).CallList.first()["callId"].toInt());
            m_CallStatistics->append(callstat);
            callstat->show();
       }
   }
}
