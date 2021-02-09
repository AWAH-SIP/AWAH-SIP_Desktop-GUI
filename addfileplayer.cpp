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

#include "addfileplayer.h"
#include "ui_addfileplayer.h"
#include <QFileDialog>
#include <QMessageBox>

AddFilePlayer::AddFilePlayer(QWidget *parent, CmdFacade *lib) :
    QDialog(parent),
    ui(new Ui::AddFilePlayer),
    m_cmdFacade(lib)
{
    ui->setupUi(this);
}

AddFilePlayer::~AddFilePlayer()
{
    delete ui;
}




void AddFilePlayer::on_pushButton_browse_clicked()
{
    fileName = QFileDialog::getOpenFileName(this,
             tr("Open Sound File"), QDir::homePath(), tr("wav Files (*.wav)"));
    ui->lineEdit_FilePath->setText(fileName);
}

void AddFilePlayer::on_pushButton_close_clicked()
{
    AddFilePlayer::close();
}

void AddFilePlayer::on_pushButton_ok_clicked()
{
    if(playername.isEmpty()){
        QMessageBox msgBox;
        msgBox.setText("The Player needs a Name.");
        msgBox.exec();
    }
    else if (fileName.isEmpty()){
        QMessageBox msgBox;
        msgBox.setText("Please select a file first.");
        msgBox.exec();
    }
    else{
    m_cmdFacade->addFilePlayer(playername,fileName);
    AddFilePlayer::close();
    }
}

void AddFilePlayer::on_lineEdit_PlayerName_textChanged(const QString &arg1)
{
    playername = arg1;
}

void AddFilePlayer::on_lineEdit_FilePath_textChanged(const QString &arg1)
{
    fileName = arg1;
}
