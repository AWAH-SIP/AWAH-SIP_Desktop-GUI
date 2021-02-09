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

#ifndef CODECSETTINGS_H
#define CODECSETTINGS_H

#include <QDialog>
#include "lib/types.h"
#include "cmdfacade.h"
#include <QStringListModel>
#include <QStyledItemDelegate>
#include <QComboBox>
#include <QStandardItemModel>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QHBoxLayout>

namespace Ui {
class CodecSettings;
}

class CodecSettings : public QDialog
{
    Q_OBJECT

public:
    explicit CodecSettings(QWidget *parent = nullptr, CmdFacade *lib = nullptr, QString codecID = "");
    ~CodecSettings();
private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

private:
    Ui::CodecSettings *ui;
    CmdFacade *m_cmdFacade ;
    QString m_codecID;
    QMap<QString, s_setting > m_map;
    QStandardItemModel *settingsmodel;
};





#endif // CODECSETTINGS_H
