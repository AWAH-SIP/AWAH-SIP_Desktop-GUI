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
#include "addgpio.h"
#include <qmessagebox.h>

IOSettings::IOSettings(QWidget *parent, CmdFacade *lib) :
    QDialog(parent),
    ui(new Ui::IOSettings),
    m_cmdFacade(lib),
    m_DeviceList(m_cmdFacade->getIoDevices())
{
    ui->setupUi(this);
    QSettings settings("awah", "AWAHSipDesktopGUI");
    restoreGeometry(settings.value("IOSettings/Geometry").toByteArray());
    devModel = new DevModel(m_DeviceList, this);
    ui->tableView->setModel(devModel);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(m_cmdFacade, &CmdFacade::ioDevicesChanged, this, &IOSettings::ioDevicesChanged);
}

IOSettings::~IOSettings()
{
    delete ui;
    delete devModel;
    if(m_addSounddev != nullptr){
        delete m_addSounddev;
    }
}

void IOSettings::on_pushButton_add_snd_dev_clicked()
{ 
    m_addSounddev = new addSoundDev(this,m_cmdFacade);
    m_addSounddev->setModal(true);
    m_addSounddev->exec();
}

void IOSettings::on_pushButton_remove_dev_clicked()
{
    QModelIndexList indexList = ui->tableView->selectionModel()->selectedIndexes();
    int row;
    foreach (QModelIndex index, indexList) {
        row = index.row();
    }
    switch (m_DeviceList.at(row).devicetype) {
    case SoundDevice:
    case TestToneGenerator:
    case FilePlayer:
    case FileRecorder:
        m_cmdFacade->removeAudioDevice(m_DeviceList.at(row).uid);
        break;
    case VirtualGpioDevice:
    case LogicAndGpioDevice:
    case LogicOrGpioDevice:
    case AccountGpioDevice:
    case LinuxGpioDevice:
    case AudioCrosspointDevice:
        m_cmdFacade->removeGpioDevice(m_DeviceList.at(row).uid);
        break;
    default:
        QMessageBox::warning(this,"","Don't know what to do with this unknown device");
        break;
    }
}

void IOSettings::on_pushButton_add_GPIO_clicked()
{
    addGPIO gpio(this,m_cmdFacade);
    gpio.setModal(true);
    gpio.exec();
}

void IOSettings::ioDevicesChanged(QList<s_IODevices>& ioDevice){
    m_DeviceList = ioDevice;
    devModel->refresh();
    ui->tableView->viewport()->update();
    ui->tableView->resizeColumnsToContents();
}

void IOSettings::on_pushButton_close_clicked()
{
    IOSettings::close();
}

void IOSettings::closeEvent(QCloseEvent* event)
{
    QSettings settings("awah", "AWAHSipDesktopGUI");
    settings.setValue("IOSettings/Geometry", saveGeometry());
    QDialog::closeEvent(event);
}

// *********************** Device Model needed to display the sound devices ***************

DevModel::DevModel(QList<s_IODevices> &devList, QObject *parent)
    : QAbstractTableModel(parent),
      m_DeviceList(devList)
{

}

int DevModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_DeviceList.count();
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
            switch(m_DeviceList.at(index.row()).devicetype){
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
            case VirtualGpioDevice:
                return "Virtual GPIO";
                break;
            case LogicAndGpioDevice:
                return "Logic AND";
                break;
            case LogicOrGpioDevice:
                return "Logic OR";
                break;
            case AccountGpioDevice:
                return "Logic Account";
                break;
            case LinuxGpioDevice:
                return "Linux GPIO Device";
                break;
            case AudioCrosspointDevice:
                return "Audio Crosspoint Device";
                break;
            default:
                return "Unknown Device";
            }

        case 1:
            return m_DeviceList.at(index.row()).inputname;
            break;
        case 2:

            break;
        }
    if(role == Qt::BackgroundColorRole){
        if(m_DeviceList.at(index.row()).devicetype== SoundDevice)
            if(m_DeviceList.at(index.row()).PBDevID ==-1 || m_DeviceList.at(index.row()).RecDevID == -1)
                return QBrush(QColor(255, 153, 153)); // light red
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
                return QString("Name");
            case 2:
                return QString(" ");
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
