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

#ifndef SIPSTATEMODEL_H
#define SIPSTATEMODEL_H

#include <QDialog>
#include "lib/types.h"
#include "cmdfacade.h"
#include <QStringListModel>



class SipStateModel : public QAbstractTableModel
{
    Q_OBJECT;
public:
    SipStateModel(QObject *parent = nullptr,CmdFacade *lib = nullptr);
    void setActiveAccounts(QList <s_account> *accounts);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void refresh();
public slots:
    void Onsip_status(int accountid, int sipstatus ,QString statusText);
    void OnCallStateChanged(int accID, int role, int callId, bool remoteofferer, long calldur, int state, int lastStatusCode, QString statustxt, QString remoteUri);
    void onTableClicked(const QModelIndex &index);

private:
    QList <s_account> *m_AccountList;
    CmdFacade *m_cmdFacade;
    QVector <QString> SIPStatusText;
    QVector <int> SIPStatusCode;
    QVector <QString> CallStatusText;
    QVector <int> CallStatusCode;
    QMap <int, int> m_callID;

};

#endif // SIPSTATEMODEL_H
