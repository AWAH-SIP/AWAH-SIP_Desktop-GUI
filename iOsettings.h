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

#ifndef IOSETTINGS_H
#define IOSETTINGS_H

#include <QDialog>
#include "lib/types.h"
#include "cmdfacade.h"
#include <QStringListModel>
#include "addsounddev.h"

class DevModel;

namespace Ui {
class IOSettings;
}

class IOSettings : public QDialog
{
    Q_OBJECT

public:
    explicit IOSettings(QWidget *parent = nullptr, CmdFacade *lib = nullptr);
    ~IOSettings();

private slots:
    void on_pushButton_add_snd_dev_clicked();

    void on_pushButton_remove_dev_clicked();

    void on_pushButton_add_GPIO_clicked();

    void ioDevicesChanged(QList<s_IODevices>& ioDevice);

    void on_pushButton_close_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::IOSettings *ui;
    CmdFacade *m_cmdFacade;
    QList <s_IODevices> &m_DeviceList;
    DevModel *devModel;
    addSoundDev* m_addSounddev = nullptr;
};


class DevModel : public QAbstractTableModel
{
    Q_OBJECT;
public:
    DevModel(QList <s_IODevices>& devList, QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void refresh();
private:
    QList <s_IODevices> &m_DeviceList;

};


#endif // IOSETTINGS_H
