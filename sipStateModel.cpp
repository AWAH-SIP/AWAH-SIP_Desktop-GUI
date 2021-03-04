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

#define PJSUA_MAX_ACC 32  // TODO: Remove this!

SipStateModel::SipStateModel(QObject *parent, CmdFacade *lib)
    : QAbstractTableModel(parent), m_cmdFacade(lib)
{
    SIPStatusText.fill("Waiting for SIP message ...",PJSUA_MAX_ACC);
    SIPStatusCode.fill(0,PJSUA_MAX_ACC);
    CallStatusText.fill("Idle ...",PJSUA_MAX_ACC);
    CallStatusCode.fill(0,PJSUA_MAX_ACC);
    connect(m_cmdFacade, &CmdFacade::callInfo, this, &SipStateModel::on_callInfo);
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
    return 7;       // 7th emtpy column to fill unused space
}

QVariant SipStateModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        switch (index.column()) {
            case 0:
                return m_AccountList->at(index.row()).name;
                break;
            case 1:
                return m_AccountList->at(index.row()).user;
                break;
            case 2:
                return  SIPStatusText.at(m_AccountList->at(index.row()).AccID);
                break;
            case 3:
                if(m_AccountList->at(index.row()).CallList.count()>1){
                    return "Multiple calls connected! ";
                }
                else return CallStatusText.at(m_AccountList->at(index.row()).AccID);
                break;
    }

    if(role == Qt::BackgroundColorRole)
        if (index.column()== 2) {
            if(SIPStatusCode.at(m_AccountList->at(index.row()).AccID)<200)
            return QBrush(QColor(102, 153, 255)); // light blue

            else if(SIPStatusCode.at(m_AccountList->at(index.row()).AccID)<300)
            return QBrush(QColor(153, 255, 153));  // light green

            else if(SIPStatusCode.at(m_AccountList->at(index.row()).AccID)>299)
            return QBrush(QColor(255, 153, 153)); // light red
        }
    if(role == Qt::BackgroundColorRole)
        if (index.column()== 3) {
            if(CallStatusCode.at(m_AccountList->at(index.row()).AccID)==0 ||CallStatusCode.at(m_AccountList->at(index.row()).AccID)==6){
                if (m_AccountList->at(index.row()).CallList.count()== 0)
                    return QVariant();
                else return QBrush(QColor(153, 255, 153));
            }
            else if(CallStatusCode.at(m_AccountList->at(index.row()).AccID)==5  && m_AccountList->at(index.row()).CallList.count()==1)
                return QBrush(QColor(153, 255, 153));  // light green if only one call is connected

            else if(CallStatusCode.at(m_AccountList->at(index.row()).AccID)==5 && m_AccountList->at(index.row()).CallList.count()>1)
              return QBrush(QColor(255, 194, 153));     // light orange if more than one call is connected

            else
            return QBrush(QColor(255, 255, 153));   // light yellow
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

void SipStateModel::Onsip_status(int accountid, int sipstatus ,QString statusText)
{
    QString Temp;
    Temp.append(QString::number(sipstatus));
    Temp.append(" ");
    Temp.append(statusText);
    if(accountid<PJSUA_MAX_ACC)
    {
        SIPStatusText.replace(accountid, Temp);
        SIPStatusCode.replace(accountid, sipstatus);
    }
}

void SipStateModel::OnCallStateChanged(int accID, int role, int callId, bool remoteofferer, long calldur, int state, int lastStatusCode, QString statustxt, QString remoteUri)
{
    Q_UNUSED(role);
    Q_UNUSED(calldur);
    Q_UNUSED(remoteofferer);
    Q_UNUSED(lastStatusCode);
    Q_UNUSED(callId);
    QString CallTxt;
    if(accID<PJSUA_MAX_ACC)
    {
        switch(state){
         case 0:
           CallTxt = "Disconnected ";
           break;

         case 1:
            CallTxt = "Calling ";
            break;

         case 2:
            CallTxt = "Incoming Call from " + remoteUri;
            break;

         case 3:
            CallTxt = "Connecting "+ statustxt;
           break;

        case 4:
            CallTxt = "Connecting ";
            break;

        case 5:
            CallTxt = "Connected to " + remoteUri;
            break;

        case 6:
           CallTxt = "Disconnected " + statustxt;
           break;
        }

        CallStatusText.replace(accID, CallTxt);
        CallStatusCode.replace(accID, state);
        refresh();
    }
}

void SipStateModel::onTableClicked(const QModelIndex &index)
{
    if (index.isValid() && index.column()== 4) {
        if(m_AccountList->at(index.row()).CallList.count()== 0){
            MakeCall makeCall(nullptr,m_cmdFacade,m_AccountList->at(index.row()).AccID);
            makeCall.setModal(true);
            makeCall.exec();
            }
        else if (m_AccountList->at(index.row()).CallList.count()>=1){
            QString message;
            if(m_AccountList->at(index.row()).CallList.count()>1){
                message = "multiple calls connected!! \n\n Do you really want to dissconect all calls?";
            }
            else message = "Connected to: " + QString::number(m_AccountList->at(index.row()).CallList.first()) //heavily changed only to bring it to compile...
                                                       + "\n\n Do you really want to dissconect this call?";
            int ret = QMessageBox::question(nullptr, tr("AWAHSip Call"),
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
            CallStatistic callstat(nullptr,m_cmdFacade, m_AccountList->at(index.row()).AccID, m_AccountList->at(index.row()).CallList.first());
            callstat.setModal(false);
            callstat.exec();
       }
   }
}


void SipStateModel::on_callInfo(int accId, int callId, QJsonObject callInfo){
    QString callstate, ConnTo;
    callstate = callInfo["Status:"].toString();
    if(callstate == "[CONFIRMED] "){
        ConnTo = callInfo["Connected to:"].toString();
        CallStatusText.replace(accId, QString("Connected to: ") +ConnTo);
        CallStatusCode.replace(accId, 5);
        refresh();
    }
}
