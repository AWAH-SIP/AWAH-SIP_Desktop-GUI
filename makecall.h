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

#ifndef MAKECALL_H
#define MAKECALL_H

#include <QDialog>
#include "lib/types.h"
#include "cmdfacade.h"
#include <QStringListModel>

class BuddyModel;

class CallHistoryModel : public QAbstractTableModel
{
    Q_OBJECT;
public:
    CallHistoryModel(QObject *parent = nullptr,CmdFacade *lib = nullptr, int *AccID = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void refresh();

public slots:
    void onTableClicked(const QModelIndex &index);

signals:
    void SetParamFromHistory(s_codec Codec, QString Number);

private:
    CmdFacade *m_cmdFacade;
    int *m_AccID;
    const s_account* m_account;
};

 //**************************************************************************


class BuddyListModel : public QAbstractTableModel
{
    Q_OBJECT;
public:
    BuddyListModel(QObject *parent = nullptr,CmdFacade *lib = nullptr, int *AccID = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void refresh();

public slots:
    void onTableClicked(const QModelIndex &index);

signals:
    void SetParamFromHistory(s_codec Codec, QString Number);

private:
    CmdFacade *m_cmdFacade;
    int *m_AccID;
    const s_account* m_account;
     QList<s_buddy> m_buddies;
};


// **************************************************************************
namespace Ui {
class MakeCall;
}

class MakeCall : public QDialog
{
    Q_OBJECT

public:
    explicit MakeCall(QWidget *parent = nullptr, CmdFacade *lib = nullptr, int AccID = 0);
    ~MakeCall();

public slots:
    void on_ParamChanged(s_codec Codec, QString Number);

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_Call_clicked();

    void on_pushButton_settings_clicked();

    void on_selcectCodecBox_currentIndexChanged(int index);

private:
    Ui::MakeCall *ui;
    CmdFacade *m_cmdFacade ;
    int m_AccID;
    QList<s_codec> m_codecs;
    CallHistoryModel *m_callHistoryModel;
    BuddyListModel *m_buddyListModel;
    s_codec m_selectedCodec;
};

#endif // MAKECALL_H
