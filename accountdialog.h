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

#ifndef AccountDialog_H
#define AccountDialog_H

#include <QDialog>
#include "lib/types.h"

namespace Ui {
class AccountDialog;
}

class AccountDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AccountDialog(QWidget *parent = nullptr,  s_account *account = nullptr);
    ~AccountDialog();
    s_account getAccount();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_lineEdit_Name_textChanged(const QString &arg1);

    void on_lineEdit_server_textChanged(const QString &arg1);

    void on_lineEdit_user_textChanged(const QString &arg1);

    void on_lineEdit_password_textChanged(const QString &arg1);

    void on_pushButtonbrowserec_clicked();

    void on_pushButton_browsepath_clicked();

    void on_checkBox_enablerec_stateChanged(int arg1);

    void on_checkBox_enableplayer_stateChanged(int arg1);

    void on_lineEdit_recdir_textChanged(const QString &arg1);

    void on_lineEdit_playpath_textChanged(const QString &arg1);

private:
    Ui::AccountDialog *ui;
    s_account *m_s_account, editedaccount;
};

#endif // AccountDialog_H
