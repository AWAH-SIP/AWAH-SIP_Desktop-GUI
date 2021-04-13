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

class DevModel : public QAbstractTableModel
{
    Q_OBJECT;
public:
    DevModel(QObject *parent = nullptr);
    void setActiveDevices(QList <s_audioDevices> *devices);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void refresh();
private:
    QList <s_audioDevices> *m_DeviceList;

};




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

    void on_pushButton_add_generator_clicked();

    void on_pushButton_remove_dev_clicked();

    void on_pushButton_add_play_clicked();

    void AudioDevicesChanged(QList<s_audioDevices>* audioDev);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::IOSettings *ui;
    CmdFacade *m_cmdFacade;
    QList <s_audioDevices> *m_DeviceList;
    DevModel *devModel;
};



#endif // IOSETTINGS_H
