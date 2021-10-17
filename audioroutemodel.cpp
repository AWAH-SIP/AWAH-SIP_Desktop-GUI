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

#include "audioroutemodel.h"
#include <QGuiApplication>
#include <qtablewidget.h>

AudioRouteModel::AudioRouteModel(CmdFacade *lib, Ui::Route *uiParent, QObject *parent)
                : QAbstractTableModel(parent), m_cmdFacade(lib), m_uiParent(uiParent)
{
    onTableChanged(m_cmdFacade->getConfPortsList());
    onRoutesChanged(m_cmdFacade->getAudioRoutes());
}

int AudioRouteModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_audioPortList.srcPorts.size();
}

int AudioRouteModel::columnCount(const QModelIndex & /*parent*/) const
{
    return m_rowWidth;
}

QVariant AudioRouteModel::data(const QModelIndex &index, int role) const
{
    int idx = index.row() * m_rowWidth + index.column();

    switch (role) {
    case Qt::DisplayRole:
        if(m_routes.at(idx) != 0){
            if(!factTodB(m_routes.at(idx))) return QString("  ");
            else return QString().number(factTodB(m_routes.at(idx)), 'g', 5);
        }
        else{
            return QString(" ");
        }
        break;

    case Qt::TextAlignmentRole:
        return int(Qt::AlignCenter);
        break;

    case Qt::BackgroundColorRole:
        if(m_routes.at(idx) != 0){
            if(m_routesPersistant.at(idx))
                return QBrush(QColor(113, 183, 144));
            else
                return QBrush(QColor(145, 157, 157));
        }
        break;

    default:
        break;
    }
    return QVariant();
}

QVariant AudioRouteModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);
    if (role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal)
            return QString(QString::number(section) + ": " + m_audioPortList.destPorts.at(section).name);
        else
            return QString(QString::number(section) + ": " + m_audioPortList.srcPorts.at(section).name);
    }
    return QVariant();
}

void AudioRouteModel::onRoutesChanged(QList<s_audioRoutes> routes)
{
    m_routesOriginal = routes;
    emit layoutAboutToBeChanged();
    m_routes.clear();
    m_routes.resize(m_audioPortList.srcPorts.size() * m_rowWidth);
    m_routesPersistant.clear();
    m_routesPersistant.resize(m_audioPortList.srcPorts.size() * m_rowWidth);

    for(auto& route : routes){
        int idx = m_srcSlotMap.value(route.srcSlot, 0) * m_rowWidth + m_destSlotMap.value(route.destSlot, 0);
        m_routes[idx] = route.level;
        m_routesPersistant[idx] = route.persistant;
    }
    emit layoutChanged();
    emit dataChanged(index(0, 0),
                     index(m_rowWidth, m_rowWidth));  // update whole view
}

void AudioRouteModel::onTableClicked(const QModelIndex &index)
{
    Qt::MouseButtons pressedBtn = QGuiApplication::mouseButtons();
    int idx = index.row() * m_rowWidth + index.column();
    switch (pressedBtn) {
    case Qt::LeftButton:
    case Qt::LeftButton|Qt::RightButton:
        if(m_routes.at(idx) == 0){
            setCrosspoint(index);
        }else{
            deleteCrosspoint(index);
        }
        break;
    case Qt::RightButton:
        if(m_routes.at(idx) != 0){
            changeCrosspointLevel(index);
        }
        break;
    default:
        break;
    }
    m_uiParent->tableView_audioRoutes->clearSelection();
}

void AudioRouteModel::onVertHeaderDoubleClicked(const int logicalIndex)
{
    bool ok;
    QString customLabel = QInputDialog::getText(m_uiParent->tableView_audioRoutes, tr("Edit channellabel"),
                                            tr("Edit name:"), QLineEdit::Normal,
                                            m_audioPortList.srcPorts.at(logicalIndex).name, &ok);
     if (ok){
         m_cmdFacade->changeConfportsrcName(m_audioPortList.srcPorts.at(logicalIndex).pjName, customLabel);
     }
}

void AudioRouteModel::onHorHeaderDoubleClicked(const int logicalIndex)
{
    bool ok;
    QString customLabel = QInputDialog::getText(m_uiParent->tableView_audioRoutes, tr("Edit channellabel"),
                                            tr("Edit name:"), QLineEdit::Normal,
                                            m_audioPortList.destPorts.at(logicalIndex).name, &ok);
     if (ok){
         m_cmdFacade->changeConfportdstName(m_audioPortList.destPorts.at(logicalIndex).pjName, customLabel);
     }
}

void AudioRouteModel::onTableChanged(const s_audioPortList& portList)
{
    emit layoutAboutToBeChanged();
    m_audioPortList = portList;
    m_rowWidth = m_audioPortList.destPorts.size();
    m_srcSlotMap.clear();
    for(int i=0; i< m_audioPortList.srcPorts.size(); i++){
        m_srcSlotMap[m_audioPortList.srcPorts.at(i).slot] = i;
    }
    m_destSlotMap.clear();
    for(int i=0; i< m_audioPortList.destPorts.size(); i++){
        m_destSlotMap[m_audioPortList.destPorts.at(i).slot] = i;
    }
    onRoutesChanged(m_routesOriginal);
}

void AudioRouteModel::setCrosspoint(const QModelIndex &index)
{
    bool ok = false;
    double d;
    if(m_uiParent->defVolume_checkBox->isChecked()){
        ok = true;
        d = m_uiParent->devVolume_spinBox->value();
    }else{
        d = QInputDialog::getDouble(m_uiParent->tableView_audioRoutes,
                                       tr("Set Crosspoint from %1 to %2").arg(m_audioPortList.srcPorts.at(index.row()).name).arg(m_audioPortList.destPorts.at(index.column()).name),
                                       tr("Volume in dB:"), 0, -144, 24, 1, &ok,
                                       Qt::WindowFlags());
    }
    if (ok){
        m_cmdFacade->connectConfPort(m_srcSlotMap.key(index.row()), m_destSlotMap.key(index.column()), dBtoFact(d), !m_uiParent->tempRoute_checkBox->isChecked());
    }
}

void AudioRouteModel::changeCrosspointLevel(const QModelIndex &index)
{
    int idx = index.row() * m_rowWidth + index.column();
    bool ok;
    double d = QInputDialog::getDouble(m_uiParent->tableView_audioRoutes,
                                       tr("Change Volume from %1 to %2").arg(m_audioPortList.srcPorts.at(index.row()).name).arg(m_audioPortList.destPorts.at(index.column()).name),
                                       tr("Volume in dB:"), 0,-144, 24, 1, &ok,
                                       Qt::WindowFlags());
    if (ok && dBtoFact(d) != m_routes.at(idx)){
        m_cmdFacade->changeConfPortLevel(m_srcSlotMap.key(index.row()), m_destSlotMap.key(index.column()), dBtoFact(d));
    }
}

void AudioRouteModel::deleteCrosspoint(const QModelIndex &index)
{
    m_cmdFacade->disconnectConfPort(m_srcSlotMap.key(index.row()), m_destSlotMap.key(index.column()));
}

/////*** RotateHeaderView Class ***/////

RotatedHeaderView::RotatedHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{

}

void RotatedHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    QPointF rectCenter = QRectF(rect).center();
    painter->save();
    //rotate around rectCenter
    painter->translate(rectCenter.x(), rectCenter.y());
    painter->rotate(-90.0);
    painter->translate(-rectCenter.x(), -rectCenter.y());
    //apply the same transformation on the rect
    QRect rectCopy = painter->worldTransform().mapRect(rect);
    //use base paintSection method after applying required transformations
    QHeaderView::paintSection(painter, rectCopy, logicalIndex);
    painter->restore();
}

QSize RotatedHeaderView::sectionSizeFromContents(int logicalIndex) const
{
    //get sizeHint from base sizeHint method
    QSize val = QHeaderView::sectionSizeFromContents(logicalIndex);
    //swap height and width
    return QSize(val.height(), val.width());
}
