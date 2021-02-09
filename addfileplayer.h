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

#ifndef ADDFILEPLAYER_H
#define ADDFILEPLAYER_H

#include <QDialog>
#include "lib/types.h"
#include "cmdfacade.h"

namespace Ui {
class AddFilePlayer;
}

class AddFilePlayer : public QDialog
{
    Q_OBJECT

public:
    explicit AddFilePlayer(QWidget *parent = nullptr,CmdFacade *lib = nullptr);
    ~AddFilePlayer();

private slots:

    void on_pushButton_browse_clicked();

    void on_pushButton_close_clicked();

    void on_pushButton_ok_clicked();

    void on_lineEdit_PlayerName_textChanged(const QString &arg1);

    void on_lineEdit_FilePath_textChanged(const QString &arg1);

private:
    Ui::AddFilePlayer *ui;
    CmdFacade *m_cmdFacade;
    QString fileName, playername;
};

#endif // ADDFILEPLAYER_H
