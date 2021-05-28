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

#include "addgpio.h"
#include "ui_addgpio.h"
#include <qmessagebox.h>


addGPIO::addGPIO(QWidget *parent, CmdFacade *lib) :
    QDialog(parent),
    ui(new Ui::addGPIO),
    m_cmdFacade(lib)
{
    ui->setupUi(this);
    QSettings settings("awah", "AWAHSipDesktopGUI");
    restoreGeometry(settings.value("GeneralSettings/Geometry").toByteArray());
    m_GpioDevTypes = m_cmdFacade->getGpioDevTypes();
    QJsonObject::iterator j;
    for (j = m_GpioDevTypes.begin(); j != m_GpioDevTypes.end(); ++j) {
        ui->comboBox_devTypes->addItem(j.key());
    }
}

addGPIO::~addGPIO()
{
    delete ui;
}

void addGPIO::on_pushButton_ok_clicked()
{
    QString errorMsg;
    QJsonObject newDevice;
    newDevice["devType"] = m_devType;
    newDevice["parameter"] = m_editedParam;
    errorMsg = m_cmdFacade->createGpioDev(newDevice);
    if(errorMsg == ""){
        addGPIO::close();
    }
    else{
        QMessageBox::warning(this,"device not created",errorMsg);
    }
}

void addGPIO::on_pushButton_cancel_clicked()
{
    addGPIO::close();
}


void addGPIO::on_comboBox_devTypes_currentIndexChanged(const QString &arg1)
{
    m_editedParam = QJsonObject();
    m_GpioDev = m_GpioDevTypes.value(arg1)["parameter"].toObject();
    m_devType = (DeviceType)m_GpioDevTypes.value(arg1)["devType"].toInt();

    if(m_GpioDevModel !=nullptr){
        delete m_GpioDevModel;
    }

    m_GpioDevModel = new QStandardItemModel(m_GpioDev.size(), 2, this);
    ui->tableView->setModel(m_GpioDevModel);

    if(m_GPIdelegate !=nullptr){
        delete m_GPIdelegate;
    }
    m_GPIdelegate = new SettingsDelegate(&m_GpioDev, &m_editedParam, ui->tableView);
    ui->tableView->setItemDelegate(m_GPIdelegate);
    QJsonObject::iterator j;
    QModelIndex index;
    int row = 0;
    for (j = m_GpioDev.begin(); j != m_GpioDev.end(); ++j) {
            index = m_GpioDevModel->index(row, 0, QModelIndex());
            m_GpioDevModel->setData(index,j.key());
            index = m_GpioDevModel->index(row, 1, QModelIndex());
            m_GpioDevModel->setData(index,j.value().toObject());
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
