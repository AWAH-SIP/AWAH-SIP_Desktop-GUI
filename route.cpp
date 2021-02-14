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

#include "route.h"

#include <QSettings>


Route::Route(CmdFacade *lib, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Route), m_cmdFacade(lib)
{
    ui->setupUi(this);
    m_AudioRoutesModel = new AudioRouteModel(m_cmdFacade, ui, this);
    m_RotHeaderView = new RotatedHeaderView(Qt::Horizontal);
    ui->tableView_audioRoutes->setHorizontalHeader(m_RotHeaderView);
    ui->tableView_audioRoutes->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_audioRoutes->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_audioRoutes->verticalHeader()->setDefaultAlignment(Qt::AlignRight);
    ui->tableView_audioRoutes->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_audioRoutes->setModel(m_AudioRoutesModel);

    QSettings settings("awah", "AWAHSipDesktopGUI");
    restoreGeometry(settings.value("RoutingWindow/Geometry").toByteArray());

    connect(ui->tableView_audioRoutes, SIGNAL(pressed(const QModelIndex &)),
            m_AudioRoutesModel, SLOT(onTableClicked(const QModelIndex &)));

    qRegisterMetaType<QList<s_audioRoutes>>("QList<s_audioRoutes>");
    connect(lib, SIGNAL(audioRoutesChanged(QList<s_audioRoutes>)),
            m_AudioRoutesModel, SLOT(onRoutesChanged(QList<s_audioRoutes>)));

    qRegisterMetaType<s_audioPortList>("s_audioPortList");
    connect(lib, SIGNAL(audioRoutesTableChanged(const s_audioPortList&)),
            m_AudioRoutesModel, SLOT(onTableChanged(const s_audioPortList&)));
}

Route::~Route()
{
    delete ui;
    delete m_AudioRoutesModel;
    delete m_RotHeaderView;
}


void Route::closeEvent(QCloseEvent* event)
{
    QSettings settings("awah", "AWAHSipDesktopGUI");
    settings.setValue("RoutingWindow/Geometry", saveGeometry());
    QDialog::closeEvent(event);
}
