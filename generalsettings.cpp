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
    QSettings settings("awah", "AWAHSipDesktopGUI");
    restoreGeometry(settings.value("GeneralSettings/Geometry").toByteArray());
    QModelIndex index;
    m_settings = m_cmdFacade->getSettings();
    m_GlobalSettings = m_settings->value("GlobalSettings").toObject();
    m_AudioSettings =  m_settings->value("AudioSettings").toObject();
    m_SIPSettings = m_settings->value("SIPSettings").toObject();

    // *************** Global Tab ***************
    settingsmodel = new QStandardItemModel(m_GlobalSettings.size(), 2, this);
    ui->tableView->setModel(settingsmodel);

    SettingsDelegate *delegate = new SettingsDelegate(&m_GlobalSettings, &m_editedSettings, ui->tableView);
    ui->tableView->setItemDelegate(delegate);

    QJsonObject::iterator j;
    int row = 0;
    for (j = m_GlobalSettings.begin(); j != m_GlobalSettings.end(); ++j) {
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

    // *************** SIP Tab ***************
    sipmodel = new QStandardItemModel(m_SIPSettings.size(), 2, this);
    ui->tableView_sip->setModel(sipmodel);

    SettingsDelegate *SIPdelegate = new SettingsDelegate(&m_SIPSettings, &m_editedSettings, ui->tableView_sip);
    ui->tableView_sip->setItemDelegate(SIPdelegate);

    row = 0;
    for (j = m_SIPSettings.begin(); j != m_SIPSettings.end(); ++j) {
            index = sipmodel->index(row, 0, QModelIndex());
            sipmodel->setData(index,j.key());
            index = sipmodel->index(row, 1, QModelIndex());
            sipmodel->setData(index,j.value().toObject());
            ui->tableView_sip->openPersistentEditor(index);
            row++;
    }

    ui->tableView_sip->resizeColumnsToContents();
    ui->tableView_sip->horizontalHeader()->setStretchLastSection(true);
    ui->tableView_sip->horizontalHeader()->hide();
    ui->tableView_sip->verticalHeader()->hide();
    ui->tableView_sip->resizeRowsToContents();
    ui->tableView_sip->show();

    // *************** Audio Tab ***************
    audiomodel = new QStandardItemModel(m_AudioSettings.size(), 2, this);
    ui->tableView_audio->setModel(audiomodel);

    SettingsDelegate *Audiodelegate = new SettingsDelegate(&m_AudioSettings, &m_editedSettings, ui->tableView_audio);
    ui->tableView_audio->setItemDelegate(Audiodelegate);

    row = 0;
    for (j = m_AudioSettings.begin(); j != m_AudioSettings.end(); ++j) {
            index = audiomodel->index(row, 0, QModelIndex());
            audiomodel->setData(index,j.key());
            index = audiomodel->index(row, 1, QModelIndex());
            audiomodel->setData(index,j.value().toObject());
            ui->tableView_audio->openPersistentEditor(index);
            row++;
    }

    ui->tableView_audio->resizeColumnsToContents();
    ui->tableView_audio->horizontalHeader()->setStretchLastSection(true);
    ui->tableView_audio->horizontalHeader()->hide();
    ui->tableView_audio->verticalHeader()->hide();
    ui->tableView_audio->resizeRowsToContents();
    ui->tableView_audio->show();


    // ****************** Codec Priorities *************
     m_codecPrio = m_cmdFacade->getCodecPriorities();
     codecpriomodel = new QStandardItemModel(m_codecPrio.count(), 2, this);
     ui->tableView_codecprio->setModel(codecpriomodel);

     SettingsDelegate *codecdelegate = new SettingsDelegate(&m_codecPrio, &m_editedCodecPrio, ui->tableView_codecprio);
     ui->tableView_codecprio->setItemDelegate(codecdelegate);

     row = 0;
     QJsonObject::iterator codec;
     for (codec = m_codecPrio.begin(); codec != m_codecPrio.end(); ++codec) {
             index = codecpriomodel->index(row, 0, QModelIndex());
             codecpriomodel->setData(index,codec.key());
             index = codecpriomodel->index(row, 1, QModelIndex());
             codecpriomodel->setData(index,codec.value().toObject());
             ui->tableView_codecprio->openPersistentEditor(index);
             row++;
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
    if(!m_editedCodecPrio.empty()){
        m_cmdFacade->setCodecPriorities(m_editedCodecPrio);
    }
    if(!m_editedSettings.empty()){
        m_cmdFacade->setSettings(m_editedSettings);
        QMessageBox::information(this,"Warning","Some settings need a restart in order to get active");
    }
    GeneralSettings::close();
}

void GeneralSettings::on_pushButton_cancel_clicked()
{
    GeneralSettings::close();
}

void GeneralSettings::closeEvent(QCloseEvent* event)
{
    QSettings settings("awah", "AWAHSipDesktopGUI");
    settings.setValue("GeneralSettings/Geometry", saveGeometry());
    QDialog::closeEvent(event);
}

