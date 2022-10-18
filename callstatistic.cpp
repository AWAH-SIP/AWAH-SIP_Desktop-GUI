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
    QSettings settings("awah", "AWAHSipDesktopGUI");
    restoreGeometry(settings.value("CallStatistic/Geometry").toByteArray());
    m_statModel = new StatisticModel(&m_callinfo,this);
    connect(m_cmdFacade, &CmdFacade::callInfo, this, &CallStatistic::on_callInfo);
    ui->tableView->setModel(m_statModel);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    ui->textEdit->setText(m_cmdFacade->getSDP(CallID,AccID));
    ui->textEdit->setReadOnly(true);
}

CallStatistic::~CallStatistic()
{
    delete ui;
}

void CallStatistic::on_pushButton_close_clicked()
{
    CallStatistic::close();
}

void CallStatistic::on_callInfo(int accId, int callId, QJsonObject callInfo)
{
    if(m_AccID == accId && m_CallID == callId){
        m_callinfo = callInfo;
        emit m_statModel->layoutChanged();
        ui->tableView->resizeColumnsToContents();
    }
}

void CallStatistic::closeEvent(QCloseEvent* event)
{
    QSettings settings("awah", "AWAHSipDesktopGUI");
    settings.setValue("CallStatistic/Geometry", saveGeometry());
    QDialog::closeEvent(event);
}


// ************************************** Model Call info **************************

StatisticModel::StatisticModel(QJsonObject *callinfo, QObject *parent) :
    QAbstractTableModel(parent), m_callinfo(callinfo)
{

}

int StatisticModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    if (m_callinfo)
        return m_callinfo->count();
    return 0;
}

int StatisticModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant StatisticModel::data(const QModelIndex& index, int role) const
{
    if (!m_callinfo)
        return QVariant();
    if (index.row() < 0 ||
        index.row() >= m_callinfo->count() ||
        role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0)
        return m_callinfo->keys().at(index.row());
    if (index.column() == 1)
        return m_callinfo->value(m_callinfo->keys().at(index.row()));
    return QVariant();
}
