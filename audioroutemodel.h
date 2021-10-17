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

#ifndef AUDIOROUTEMODEL_H
#define AUDIOROUTEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include "ui_route.h"
#include <QHeaderView>
#include <QPainter>
#include <QInputDialog>

#include "lib/types.h"
#include "cmdfacade.h"


class AudioRouteModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    AudioRouteModel(CmdFacade* lib, Ui::Route *uiParent, QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public slots:
    void onRoutesChanged(QList<s_audioRoutes> routes);
    void onTableClicked(const QModelIndex &index);
    void onTableChanged(const s_audioPortList &portList);
    void onVertHeaderDoubleClicked(const int logicalIndex);
    void onHorHeaderDoubleClicked(const int logicalIndex);

private:
    void setCrosspoint(const QModelIndex &index);
    void changeCrosspointLevel(const QModelIndex &index);
    void deleteCrosspoint(const QModelIndex &index);
    CmdFacade* m_cmdFacade;
    Ui::Route *m_uiParent;
    s_audioPortList m_audioPortList;
    int m_rowWidth;
    QVector<float> m_routes;
    QVector<bool> m_routesPersistant;
    QMap<int, int> m_srcSlotMap;
    QMap<int, int> m_destSlotMap;
    QList<s_audioRoutes> m_routesOriginal;
};

//a header view that renders text vertically
class RotatedHeaderView : public QHeaderView {
public:
    explicit RotatedHeaderView(Qt::Orientation orientation, QWidget *parent = nullptr);
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;
    QSize sectionSizeFromContents(int logicalIndex) const override;
};

inline float dBtoFact(const float dB)
{
   float factor = pow(10, (dB/20));
   return factor;
}

inline float factTodB(const float fact)
{
   float dB = 20 * log10f(fact);
   return dB;
}

#endif // AUDIOROUTEMODEL_H
