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

#include "generalsettings.h"
#include "ui_generalsettings.h"
#include <QStyledItemDelegate>
#include "editorwidget.h"
#include "QMessageBox"


GeneralSettings::GeneralSettings(QWidget *parent, CmdFacade *lib) :
    QDialog(parent),
    ui(new Ui::GeneralSettings),
    m_cmdFacade(lib)
{
    ui->setupUi(this);
    QModelIndex index;
    m_map = m_cmdFacade->getSettings();

    // *************** General Tab ***************
    settingsmodel = new QStandardItemModel(m_map.count(), 2, this);
    ui->tableView->setModel(settingsmodel);

    SettingsDelegate *delegate = new SettingsDelegate(ui->tableView);
    delegate->setMap(&m_map);
    ui->tableView->setItemDelegate(delegate);

    for (int row = 0; row < m_map.count(); row++)
    {
        index = settingsmodel->index(row, 0, QModelIndex());
        settingsmodel->setData(index,m_map.keys().at(row));
    }
    for (int row = 0; row < m_map.count(); row++)
    {
        index = settingsmodel->index(row, 1, QModelIndex());
        settingsmodel->setData(index,m_map.values().at(row).value);
        ui->tableView->openPersistentEditor(index);
    }

    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->resizeRowsToContents();
    ui->tableView->show();

    // ****************** Codec Priorities *************
     m_codecs = m_cmdFacade->getCodecPriorities();
     codecpriomodel = new QStandardItemModel(m_codecs.count(), 2, this);
     ui->tableView_codecprio->setModel(codecpriomodel);

     SettingsDelegate *codecdelegate = new SettingsDelegate(ui->tableView_codecprio);
     codecdelegate->setMap(&m_codecs);
     ui->tableView_codecprio->setItemDelegate(codecdelegate);

     for (int row = 0; row < m_codecs.count(); row++)
     {
         index = codecpriomodel->index(row, 0, QModelIndex());
         codecpriomodel->setData(index,m_codecs.keys().at(row));
     }
     for (int row = 0; row < m_codecs.count(); row++)
     {
         index = codecpriomodel->index(row, 1, QModelIndex());
         codecpriomodel->setData(index,m_codecs.values().at(row).value);
         ui->tableView_codecprio->openPersistentEditor(index);
     }

     ui->tableView_codecprio->resizeColumnsToContents();
     ui->tableView_codecprio->horizontalHeader()->setStretchLastSection(true);
     ui->tableView_codecprio->horizontalHeader()->hide();
     ui->tableView_codecprio->verticalHeader()->hide();
     ui->tableView_codecprio->resizeRowsToContents();
     ui->tableView_codecprio->show();
}

GeneralSettings::~GeneralSettings()
{
    delete ui;
}

void GeneralSettings::on_pushButton_ok_clicked()
{
    m_cmdFacade->setSettings(m_map);
    m_cmdFacade->setSCodecPriorities(m_codecs);
     QMessageBox::information(this,"Warning","Some settings need a restart in order to get active");
    GeneralSettings::close();
}

void GeneralSettings::on_pushButton_cancel_clicked()
{
    GeneralSettings::close();
}

