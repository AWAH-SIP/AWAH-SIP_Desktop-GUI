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

#include "iOsettings.h"
#include "ui_iOsettings.h"
#include <QInputDialog>
#include "addsounddev.h"
#include "addfileplayer.h"


IOSettings::IOSettings(QWidget *parent, CmdFacade *lib) :
    QDialog(parent),
    ui(new Ui::IOSettings),
    m_cmdFacade(lib)
{
    ui->setupUi(this);
    QSettings settings("awah", "AWAHSipDesktopGUI");
    restoreGeometry(settings.value("IOSettings/Geometry").toByteArray());
    m_DeviceList = m_cmdFacade->getAudioDevices();

    devModel = new DevModel();
    devModel->setActiveDevices(m_DeviceList);

    ui->tableView->setModel(devModel);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(m_cmdFacade, &CmdFacade::AudioDevicesChanged, this, &IOSettings::AudioDevicesChanged);
}

IOSettings::~IOSettings()
{
    delete ui;
    delete devModel;
}

void IOSettings::on_pushButton_add_snd_dev_clicked()
{ 
    addSoundDev sounddev(this,m_cmdFacade);
    sounddev.setModal(true);
    sounddev.exec();
}

void IOSettings::on_pushButton_add_generator_clicked()
{
    bool ok ;
    int freq = QInputDialog::getInt(0,"add Generator","Frequency in Hz", 1000, 50, 20000, 1, &ok);
    if (ok)
    {
        m_cmdFacade->addToneGen(freq);
    }
}

void IOSettings::on_pushButton_remove_dev_clicked()
{
    QModelIndexList indexList = ui->tableView->selectionModel()->selectedIndexes();
    int row;
    foreach (QModelIndex index, indexList) {
        row = index.row();
    }
       m_cmdFacade->removeAudioDevice(row);
}

void IOSettings::on_pushButton_add_play_clicked()
{
    AddFilePlayer fileplayer(this,m_cmdFacade);
    fileplayer.setModal(true);
    fileplayer.exec();
}

void IOSettings::AudioDevicesChanged(QList<s_audioDevices>* audioDev){
    m_DeviceList = audioDev;
    devModel->refresh();
    ui->tableView->viewport()->update();
    ui->tableView->resizeColumnsToContents();
}

void IOSettings::closeEvent(QCloseEvent* event)
{
    QSettings settings("awah", "AWAHSipDesktopGUI");
    settings.setValue("IOSettings/Geometry", saveGeometry());
    QDialog::closeEvent(event);
}

// *********************** Device Model needed to display the sound devices ***************

DevModel::DevModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

void DevModel::setActiveDevices(QList <s_audioDevices> *devices)
{
    m_DeviceList = devices;
}

int DevModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_DeviceList->count();
}

int DevModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 3;
}

QVariant DevModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        switch (index.column()) {
            case 0:
                switch(m_DeviceList->at(index.row()).devicetype){
                    case SoundDevice:
                        return "Sound Device";
                        break;
                    case TestToneGenerator:
                        return "Generator";
                        break;
                    case FilePlayer:
                        return "File Player";
                        break;
                    case FileRecorder:
                        return "File Recorder";
                        break;
                }

                if(m_DeviceList->at(index.row()).devicetype== SoundDevice)
                    return "Sound Device";
                if(m_DeviceList->at(index.row()).devicetype== TestToneGenerator)
                    return "Generator";
                break;
            case 1:
                return m_DeviceList->at(index.row()).inputname;
                break;
            case 2:
                return m_DeviceList->at(index.row()).outputame;
                break;
    }

    return QVariant();
}

QVariant DevModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section) {
            case 0:
                return QString("Type");
            case 1:
                return QString("Inputname");
            case 2:
                return QString("Outputname");
            default:
                return QVariant();
        }
    }

    return QVariant();
}

void DevModel::refresh() {
    emit dataChanged(index(0, 0),
                     index(rowCount(), columnCount()));  // update whole view
    emit layoutChanged();
  }
