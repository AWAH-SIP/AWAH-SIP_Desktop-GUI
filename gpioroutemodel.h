#ifndef GPIOROUTEMODEL_H
#define GPIOROUTEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include "ui_route.h"
#include <QInputDialog>

#include "lib/types.h"
#include "cmdfacade.h"

class GpioRouteModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    GpioRouteModel(CmdFacade* lib, Ui::Route *uiParent, QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public slots:
    void onRoutesChanged(QList<s_gpioRoute> routes);
    void onTableClicked(const QModelIndex &index);
    void onTableChanged(const s_gpioPortList &portList);
    void onStatesChanged(const QMap<QString, bool> changedGpios);

private:
    void setCrosspoint(const QModelIndex &index);
    void changeCrosspointInversion(const QModelIndex &index);
    void deleteCrosspoint(const QModelIndex &index);
    CmdFacade* m_cmdFacade;
    Ui::Route *m_uiParent;
    s_gpioPortList m_gpioPortList;
    int m_rowWidth;
    QVector<bool> m_routes;
    QVector<bool> m_routesInverted;
    QVector<bool> m_routesPersistant;
    QMap<QString, int> m_srcSlotMap;
    QMap<QString, int> m_destSlotMap;
    QMap<QString, bool> m_srcStateMap;
    QMap<QString, bool> m_destStateMap;
    QList<s_gpioRoute> m_routesOriginal;
};

#endif // GPIOROUTEMODEL_H
