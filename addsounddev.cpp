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

#include "addsounddev.h"
#include "ui_addsounddev.h"

addSoundDev::addSoundDev(QWidget *parent, CmdFacade *lib) :
    QDialog(parent),
    ui(new Ui::addSoundDev)
{
    ui->setupUi(this);
    m_cmdFacade = lib;
    ui->comboBox_devType->addItem("Sound device");
    ui->comboBox_devType->addItem("Generator");
    ui->comboBox_devType->addItem("File player");
    ui->comboBox_PbDev->addItems(m_cmdFacade->listOutputSoundDev());
    ui->comboBox_RecDev->addItems(m_cmdFacade->listInputSoundDev());
    ui->spinBox_GenFreq->setMinimum(20);
    ui->spinBox_GenFreq->setMaximum(20000);
    ui->spinBox_GenFreq->setValue(440);


}

addSoundDev::~addSoundDev()
{
    delete ui;
}


void addSoundDev::on_pushButton_addDev_clicked()
{
    switch (ui->stackedWidget->currentIndex()) {
        case 0:
        m_cmdFacade->addAudioDevice(m_cmdFacade->getSoundDevID(ui->comboBox_RecDev->currentText()),m_cmdFacade->getSoundDevID(ui->comboBox_PbDev->currentText()));
        break;
    case 1:
        m_cmdFacade->addToneGen(ui->spinBox_GenFreq->value());
        break;
    case 2:
        m_cmdFacade->addFilePlayer(ui->lineEdit_playerName->text(),ui->lineEdit_playerPath->text());
        break;
    default:
        break;

    }

    addSoundDev::close();
}

void addSoundDev::on_pushButton_cancel_clicked()
{
    addSoundDev::close();
}

void addSoundDev::on_comboBox_devType_currentIndexChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}
