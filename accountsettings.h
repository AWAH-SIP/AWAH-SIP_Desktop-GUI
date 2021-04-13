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

#ifndef ACCOUNTSETTINGS_H
#define ACCOUNTSETTINGS_H

#include <QDialog>
#include "lib/types.h"
#include "cmdfacade.h"
#include <QStringListModel>

class AccModel : public QAbstractTableModel
{
    Q_OBJECT;
public:
    AccModel(QObject *parent = nullptr, CmdFacade *lib = nullptr);
    void setActiveAccounts(QList <s_account> *accounts);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void refresh();
    QList <int> modifiedAcc;
private:
    QList <s_account> *m_AccountList;
    CmdFacade *m_cmdFacade;

};




namespace Ui {
class accountsettings;
}

class accountsettings : public QDialog
{
    Q_OBJECT

public:
    explicit accountsettings(QWidget *parent = nullptr, CmdFacade *lib = nullptr);
    ~accountsettings();

private slots:
    void on_pushButton_add_clicked();

    void on_pushButton_delete_clicked();

    void on_pushButton_close_clicked();

    void on_pushButton_edit_clicked();

    void on_AccountsChanged(QList <s_account>* Accounts);

    void on_tableView_doubleClicked(const QModelIndex &index);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::accountsettings *ui;
    CmdFacade *m_cmdFacade;
    QList <s_account> *m_AccList;
    AccModel *accModel;
};

#endif // ACCOUNTSETTINGS_H
