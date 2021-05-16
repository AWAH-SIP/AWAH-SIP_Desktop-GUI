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
    QString Temp;
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        switch (index.column()) {
            case 0:
                return m_AccountList->at(index.row()).name;
                break;
            case 1:
                return m_AccountList->at(index.row()).user;
                break;
            case 2:
                Temp.append(QString::number(m_AccountList->at(index.row()).SIPStatusCode));
                Temp.append(" ");
                Temp.append(m_AccountList->at(index.row()).SIPStatusText);
                return  Temp;
                break;
            case 3:
                if(m_AccountList->at(index.row()).CallList.count()>1){
                    return "Multiple calls connected! ";
                }
                else{
                    QString CallTxt;
                    switch(m_AccountList->at(index.row()).CallStatusCode){
                     case 0:
                       CallTxt = "Disconnected ";
                       break;

                     case 1:
                        CallTxt = "Calling ";
                        break;

                     case 2:
                        CallTxt = "Incoming Call from " + m_AccountList->at(index.row()).ConnectedTo;
                        break;

                     case 3:
                        CallTxt = "Connecting "+ m_AccountList->at(index.row()).CallStatusText;
                       break;

                    case 4:
                        CallTxt = "Connecting ";
                        break;

                    case 5:
                        CallTxt = "Connected to " + m_AccountList->at(index.row()).ConnectedTo;
                        break;

                    case 6:
                       CallTxt = "Disconnected " + m_AccountList->at(index.row()).CallStatusText;
                       break;

                    case 7:     // RX media lost
                       CallTxt =  m_AccountList->at(index.row()).CallStatusText;
                       break;
                    }
                    return CallTxt;
                }

                break;
    }

    if(role == Qt::BackgroundColorRole)
        if (index.column()== 2) {
            if(m_AccountList->at(index.row()).SIPStatusCode<200)
            return QBrush(QColor(145, 157, 157));   // stone

            else if(m_AccountList->at(index.row()).SIPStatusCode<300)
            return QBrush(QColor(113, 183, 144));   // green

            else if(m_AccountList->at(index.row()).SIPStatusCode>299)
            return QBrush(QColor(229, 85, 79));     // red
        }
    if(role == Qt::BackgroundColorRole)
        if (index.column()== 3) {
            if(m_AccountList->at(index.row()).CallStatusCode==0 ||m_AccountList->at(index.row()).CallStatusCode==6){
                if (m_AccountList->at(index.row()).CallList.count()== 0)
                    return QVariant();
                else return QBrush(QColor(230, 166, 93));   // If from multiple Calls one is disconnected
            }
            else if(m_AccountList->at(index.row()).CallStatusCode==5  && m_AccountList->at(index.row()).CallList.count()==1)
                return QBrush(QColor(113, 183, 144));   // green if only one call is connected

            else if(m_AccountList->at(index.row()).CallStatusCode==5 && m_AccountList->at(index.row()).CallList.count()>1)
              return QBrush(QColor(230, 166, 93));      // peach if more than one call is connected

            else
            return QBrush(QColor(179, 213, 125));       // lime
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
    if (index.isValid() && index.column()== 4) {
        if(m_AccountList->at(index.row()).CallList.count()== 0){
            MakeCall makeCall(m_parentWidget, m_cmdFacade, m_AccountList->at(index.row()).AccID);
            makeCall.setModal(true);
            makeCall.exec();
            }
        else if (m_AccountList->at(index.row()).CallList.count()>=1){
            QString message;
            if(m_AccountList->at(index.row()).CallList.count()>1){
                message = "multiple calls connected!! \n\n Do you really want to disconnect all calls?";
            }
            else message = "Connected to: " + QString::number(m_AccountList->at(index.row()).CallList.first()) //heavily changed only to bring it to compile...
                                                       + "\n\n Do you really want to disconnect this call?";
            int ret = QMessageBox::question(m_parentWidget, tr("AWAHSip Call"),
                                   tr(message.toLocal8Bit()));
            if(ret == QMessageBox::Yes){

                for (int i = 0; i< m_AccountList->at(index.row()).CallList.count(); i++){
                    m_cmdFacade->hangupCall(m_AccountList->at(index.row()).CallList.at(i), m_AccountList->at(index.row()).AccID);
                }
            }
        }
        else if (m_AccountList->at(index.row()).CallList.count()>1){
                                                                            // todo implement menu for more than one call

        }
     }
   if (index.isValid() && index.column()== 5) {
       if (m_AccountList->at(index.row()).CallList.count()){
            CallStatistic *callstat = nullptr;
            callstat   = new CallStatistic(m_parentWidget, m_cmdFacade, m_AccountList->at(index.row()).AccID, m_AccountList->at(index.row()).CallList.first());
            m_CallStatistics->append(callstat);
            callstat->show();

       }
   }
}
