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

#ifndef CALLSTATISTIC_H
#define CALLSTATISTIC_H

#include <QDialog>
#include <QTimer>
#include "lib/types.h"
#include "cmdfacade.h"
#include <QStringListModel>


class StatisticModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    enum MapRoles {
        KeyRole = Qt::UserRole + 1,
        ValueRole
    };

    explicit StatisticModel(QJsonObject *callinfo, QObject *parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private:
    QJsonObject* m_callinfo;
};



namespace Ui {
class CallStatistic;
}

class CallStatistic : public QDialog
{
    Q_OBJECT

public:
    explicit CallStatistic(QWidget *parent = nullptr,CmdFacade *lib = nullptr, int AccID = 0, int CallID = 0);
    ~CallStatistic();

signals:
    void SignalDataChanged(QStringList statistic);

private slots:
    void on_pushButton_close_clicked();
    void on_callInfo(int accId, int callId, QJsonObject callInfo);

private:
    Ui::CallStatistic *ui;
    CmdFacade *m_cmdFacade;
    QStringList statistics;
    QAbstractItemModel *statmodel;
    int m_AccID;
    int m_CallID;
    StatisticModel *m_statModel;
    QJsonObject m_callinfo = {{"waiting for Callinfo", " "}};
};



#endif // CALLSTATISTIC_H
