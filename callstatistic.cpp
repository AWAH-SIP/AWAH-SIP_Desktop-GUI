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

#include "callstatistic.h"
#include "ui_callstatistic.h"

CallStatistic::CallStatistic(QWidget *parent, CmdFacade *lib, int AccID, int CallID) :
    QDialog(parent),
    ui(new Ui::CallStatistic),
    m_cmdFacade(lib), m_AccID(AccID), m_CallID(CallID)
{
    ui->setupUi(this);

    statistics = m_cmdFacade->dumpCall(CallID,AccID);
    statmodel= new StringListModel(statistics);
    ui->listView->setAlternatingRowColors(true);
    ui->listView->setModel(statmodel);

    refreshTimer = new QTimer(this);
    refreshTimer->setInterval(1000);
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
    connect(this, SIGNAL(SignalDataChanged(QStringList)),statmodel, SLOT(onDataChanged(QStringList)));
    refreshTimer->start();

    mapmodel = new MapModel();
    map= m_cmdFacade->getStreamInfo(m_CallID,m_AccID) ;
    mapmodel->setMap(&map);
    ui->tableView->setModel(mapmodel);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();

}

CallStatistic::~CallStatistic()
{
    delete ui;
}

void CallStatistic::on_pushButton_close_clicked()
{
    CallStatistic::close();
}

void CallStatistic::timeoutSlot()
{
    statistics = m_cmdFacade->dumpCall(m_CallID,m_AccID);
    emit SignalDataChanged(statistics);

}



// ************************************** Stringlist Model Statistics **************************

int StringListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return stringList.count();
}

QVariant StringListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= stringList.size())
        return QVariant();

    if (role == Qt::DisplayRole)
        return stringList.at(index.row());
    else
        return QVariant();
}

void StringListModel::onDataChanged(QStringList statistic){
    stringList = statistic;
    emit dataChanged(index(0, 0),
                     index(stringList.count(), stringList.count()));
}


// ************************************** Map Model Call info **************************

MapModel::MapModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_map = NULL;
}

int MapModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    if (m_map)
        return m_map->count();
    return 0;
}

int MapModel::columnCount(const QModelIndex & parent) const
{
     Q_UNUSED(parent);
    return 2;
}

QVariant MapModel::data(const QModelIndex& index, int role) const
{
    if (!m_map)
        return QVariant();
    if (index.row() < 0 ||
        index.row() >= m_map->count() ||
        role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0)
        return m_map->keys().at(index.row());
    if (index.column() == 1)
        return m_map->values().at(index.row());
    return QVariant();
}
