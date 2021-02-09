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

#include "accountsettings.h"
#include "ui_accountsettings.h"
#include "accountdialog.h"

accountsettings::accountsettings(QWidget *parent, CmdFacade *lib):
    QDialog(parent),
    ui(new Ui::accountsettings),
    m_cmdFacade(lib)
{
    ui->setupUi(this);
    m_AccList = m_cmdFacade->getAccounts();
    accModel = new AccModel();
    accModel->setActiveAccounts(m_AccList);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->setModel(accModel);
}

accountsettings::~accountsettings()
{
    delete ui;
}



// *********************** Device Model needed to display the accounts ***************

AccModel::AccModel(QObject *parent, CmdFacade *lib)
    : QAbstractTableModel(parent), m_cmdFacade(lib)
{

}

void AccModel::setActiveAccounts(QList <s_account> *accounts)
{
    m_AccountList = accounts;
}

int AccModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_AccountList->count();
}

int AccModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 6;
}

QVariant AccModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        switch (index.column()) {
            case 0:
                return m_AccountList->at(index.row()).name;
                break;
            case 1:
                return m_AccountList->at(index.row()).serverURI;
                break;
            case 2:
                return m_AccountList->at(index.row()).user;
                break;
            case 3:
                return "*****";
                break;
            case 4:
                return m_AccountList->at(index.row()).FilePlayPath;
                break;
            case 5:
                return m_AccountList->at(index.row()).FileRecordPath;
                break;
    }

    return QVariant();
}

QVariant AccModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section) {
            case 0:
                return QString("Name");
                break;
            case 1:
                return QString("Server URI");
                break;
            case 2:
                return QString("Username");
                break;
            case 3:
                return QString("Password");
                break;
            case 4:
                return QString("Anouncement");
                break;
            case 5:
                return QString("Record directory");
                break;
            default:
                return QVariant();
        }
    }

    return QVariant();
}

bool AccModel::setData(const QModelIndex &index, const QVariant &value, int role)
  {
    s_account editedAcc;
    editedAcc = m_AccountList->at(index.row());
      if (role == Qt::EditRole) {

          switch(index.column()) {
           case 0:
                 editedAcc.name = value.toString();
                 break;
           case 1:
                 editedAcc.serverURI = value.toString();
                 break;
           case 2:
                 editedAcc.user = value.toString();
                 break;
           case 3:
                 editedAcc.password = value.toString();
                 break;
           case 4:
                editedAcc.FilePlayPath = value.toString();
                break;
           case 5:
                editedAcc.FileRecordPath = value.toString();
                break;
          }
          m_AccountList->replace(index.row(),editedAcc);

          int i = modifiedAcc.indexOf(index.row());     // only add to list if it does not exist
            if (i == -1)
            {
                modifiedAcc.append(index.row());
            }
          return true;
      }
      return false;
  }

Qt::ItemFlags AccModel::flags(const QModelIndex &index) const
 {
    if (!index.isValid())
    {
        return Qt::ItemIsEnabled;
    }
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
 }


void AccModel::refresh() {
    emit dataChanged(index(0, 0),
                     index(rowCount(), columnCount()));  // update whole view
    emit layoutChanged();
  }

void accountsettings::on_pushButton_add_clicked()
{
   s_account newacc;
   newacc.serverURI = "siperver.url";
   newacc.user = "username";
   newacc.password = "password";
    AccountDialog accdialog(this,&newacc);
    accdialog.setModal(true);
    accdialog.setWindowTitle("add Account");
    accdialog.exec();
    newacc = accdialog.getAccount();
    if(newacc.name != ""){
        m_cmdFacade->createAccount(newacc.name,newacc.serverURI,newacc.user,newacc.password, newacc.FilePlayPath, newacc.FileRecordPath);
        m_AccList = m_cmdFacade->getAccounts();
        accModel->refresh();
    }
}

void accountsettings::on_pushButton_delete_clicked()
{
   ui->tableView->closePersistentEditor(ui->tableView->selectionModel()->currentIndex());
   m_cmdFacade->removeAccount(ui->tableView->selectionModel()->currentIndex().row());
   m_AccList = m_cmdFacade->getAccounts();
   accModel->refresh();
}

void accountsettings::on_pushButton_close_clicked()
{
    accountsettings::close();
    for( int i = 0;i<accModel->modifiedAcc.count(); i++)                // modify edited accounts
        {
            m_cmdFacade->modifyAccount(accModel->modifiedAcc.at(i),m_AccList->at(accModel->modifiedAcc.at(i)).name,m_AccList->at(accModel->modifiedAcc.at(i)).serverURI,m_AccList->at(accModel->modifiedAcc.at(i)).user,m_AccList->at(accModel->modifiedAcc.at(i)).password, m_AccList->at(accModel->modifiedAcc.at(i)).FilePlayPath, m_AccList->at(accModel->modifiedAcc.at(i)).FileRecordPath);
        }
}


void accountsettings::on_pushButton_edit_clicked()
{
    s_account editacc;
    ui->tableView->closePersistentEditor(ui->tableView->selectionModel()->currentIndex());
    editacc = m_AccList->at(ui->tableView->selectionModel()->currentIndex().row());
    AccountDialog accdialog(this,&editacc);
    accdialog.setModal(true);
    accdialog.setWindowTitle("edit Account");
    accdialog.exec();
    editacc = accdialog.getAccount();
    if(editacc.name != ""){                             // TODO: add better Check if something in acoout was edited
        m_cmdFacade->modifyAccount(ui->tableView->selectionModel()->currentIndex().row(), editacc.name,editacc.serverURI,editacc.user,editacc.password, editacc.FilePlayPath, editacc.FileRecordPath);
        m_AccList = m_cmdFacade->getAccounts();
        accModel->refresh();
    }
}
