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

#include "makecall.h"
#include "ui_makecall.h"
#include "codecsettings.h"

MakeCall::MakeCall(QWidget *parent, CmdFacade *lib, int AccID) :
    QDialog(parent),
    ui(new Ui::MakeCall),
    m_cmdFacade(lib), m_AccID(AccID)
{
    ui->setupUi(this);
    ui->selcectCodecBox->addItems(m_cmdFacade->listCodec());
    CallHistory = new CallHistoryModel(this, m_cmdFacade, &m_AccID);
    ui->tableView->setModel(CallHistory);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setColumnWidth(0, 30);
    ui->tableView->setColumnWidth(1, 30);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);

    const s_account* account = m_cmdFacade->getAccountByID(AccID);
    QString URI = account->CallHistory.at(0).callUri;
    int startPos = URI.indexOf("sip:") + 4;
    int endPos = URI.indexOf('@');
    int length = endPos - startPos;
    ui->lineEditNumber->setText(URI.mid(startPos, length));

    for(int i=0;i<ui->selcectCodecBox->count();i++){            // we have to search the entry because we don't have the full name e.g. only "opus" instead of "opus/48000/2"
        if(ui->selcectCodecBox->itemText(i).contains(account->CallHistory.at(0).codec)){
            ui->selcectCodecBox->setCurrentIndex(i);
            break;
        }
    }

    connect(ui->tableView, SIGNAL(pressed(const QModelIndex &)),
            CallHistory, SLOT(onTableClicked(const QModelIndex &)));

    connect(CallHistory, SIGNAL(SetParamFromHistory(QString,QString)),
            this, SLOT(on_ParamChanged(QString,QString)));

}

MakeCall::~MakeCall()
{
    delete ui;
}

void MakeCall::on_pushButton_cancel_clicked()
{
    MakeCall::close();
}

void MakeCall::on_pushButton_Call_clicked()
{
    m_cmdFacade->selectCodec(ui->selcectCodecBox->currentText());
    m_cmdFacade->makeCall(ui->lineEditNumber->text(),m_AccID);
    MakeCall::close();
}

void MakeCall::on_pushButton_settings_clicked()
{
    CodecSettings settings(nullptr, m_cmdFacade,ui->selcectCodecBox->currentText());
    settings.setModal(true);
    settings.exec();
}

void MakeCall::on_ParamChanged(QString Codec, QString Number)
{
    ui->lineEditNumber->setText(Number);

    for(int i=0;i<ui->selcectCodecBox->count();i++){            // we have to search the entry because we don't have the full name e.g. only "opus" instead of "opus/48000/2"
        if(ui->selcectCodecBox->itemText(i).contains(Codec)){
            ui->selcectCodecBox->setCurrentIndex(i);
            break;
        }
    }
}



// ************************** Call histroy model ********************
CallHistoryModel::CallHistoryModel(QObject *parent, CmdFacade *lib, int *AccID)
    : QAbstractTableModel(parent), m_cmdFacade(lib), m_AccID(AccID)
{
    account = m_cmdFacade->getAccountByID(*m_AccID);
}

int CallHistoryModel::rowCount(const QModelIndex & /*parent*/) const
{

   return account->CallHistory.size();
}

int CallHistoryModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 5;
}

QVariant CallHistoryModel::data(const QModelIndex &index, int role) const
{
    int startPos, endPos, length;
    QString URI;
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        switch (index.column()) {
            case 1:
                return  account->CallHistory.at(index.row()).count;
                break;
            case 2:
                URI = account->CallHistory.at(index.row()).callUri;
                startPos = URI.indexOf("sip:") + 4;
                endPos = URI.indexOf('@');
                length = endPos - startPos;
                if(startPos > 4){               // if the URI has a name extract it
                    return QString(URI.mid(1,startPos -8)) + " " + URI.mid(startPos, length);
                }
                else return URI.mid(startPos, length);                   // extract the number of the whole uri

                break;
            case 3:
                return account->CallHistory.at(index.row()).codec;
                break;
            case 4:
                return QDateTime::fromTime_t(account->CallHistory.at(index.row()).duration).toUTC().toString("hh:mm:ss");
                break;

    }

    if (role== Qt::DecorationRole){
           QIcon icon;
           switch(index.column()){
                case 0:
                    if(account->CallHistory.at(index.row()).outgoing){
                        icon = QIcon(":/new/icons/images/call [#189].svg");
                    }
                    else{
                        icon = QIcon(":/new/icons/images/call [#190].svg");
                    }
                    return icon;
                    break;

          }
    }
    return QVariant();
}

QVariant CallHistoryModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section) {
            case 1:
                return QString(" ");
                break;
            case 2:
                return QString("Number");
                break;
            case 3:
                return QString("codec");
                break;
            case 4:
                return QString("duration");
                break;
            default:
                return QVariant();
        }
    }

    return QVariant();
}



void CallHistoryModel::refresh() {
    emit dataChanged(index(0, 0),
                     index(rowCount(), columnCount()));  // update whole view
    emit layoutChanged();
  }


void CallHistoryModel::onTableClicked(const QModelIndex &index)
{
    int startPos, endPos, length;
    QString URI;
    if (index.isValid()) {
        URI = account->CallHistory.at(index.row()).callUri;
        startPos = URI.indexOf("sip:") + 4;
        endPos = URI.indexOf('@');
        length = endPos - startPos;
        emit SetParamFromHistory(account->CallHistory.at(index.row()).codec,URI.mid(startPos, length));
    }

}

