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

#include "codecsettings.h"
#include "ui_codecsettings.h"
#include <QStyledItemDelegate>
#include "editorwidget.h"

CodecSettings::CodecSettings(QWidget *parent, CmdFacade *lib, QString codecID) :
    QDialog(parent),
    ui(new Ui::CodecSettings),
    m_cmdFacade(lib), m_codecID(codecID)
{
    ui->setupUi(this);
    m_codecsetting = lib->getCodecParam(m_codecID);
    QModelIndex index;

    settingsmodel = new QStandardItemModel(m_codecsetting.count(), 2, this);
    ui->tableView->setModel(settingsmodel);

    SettingsDelegate *delegate = new SettingsDelegate(&m_codecsetting, &m_editedsetting, ui->tableView);

    ui->tableView->setItemDelegate(delegate);
    QJsonObject::iterator j;
    int row = 0;
    for (j = m_codecsetting.begin(); j != m_codecsetting.end(); ++j) {
            index = settingsmodel->index(row, 0, QModelIndex());
            settingsmodel->setData(index,j.key());
            index = settingsmodel->index(row, 1, QModelIndex());
            settingsmodel->setData(index,j.value().toObject());
            ui->tableView->openPersistentEditor(index);
            row++;
    }

    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->resizeRowsToContents();
    ui->tableView->show();


}

CodecSettings::~CodecSettings()
{
    delete ui;
}

void CodecSettings::on_pushButton_ok_clicked()
{
    m_cmdFacade->setCodecParam(m_codecID, m_editedsetting);
    CodecSettings::close();
}

void CodecSettings::on_pushButton_cancel_clicked()
{
    CodecSettings::close();
}


