#include "gpioroutemodel.h"


GpioRouteModel::GpioRouteModel(CmdFacade *lib, Ui::Route *uiParent, QObject *parent)
                : QAbstractTableModel(parent), m_cmdFacade(lib), m_uiParent(uiParent)
{
    onTableChanged(m_cmdFacade->getGpioPortsList());
    onRoutesChanged(m_cmdFacade->getGpioRoutes());
    onStatesChanged(m_cmdFacade->getGpioStates());
}

int GpioRouteModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_gpioPortList.srcPorts.size();
}

int GpioRouteModel::columnCount(const QModelIndex & /*parent*/) const
{
    return m_rowWidth;
}

QVariant GpioRouteModel::data(const QModelIndex &index, int role) const
{
    int idx = index.row() * m_rowWidth + index.column();

    switch (role) {
    case Qt::DisplayRole:
        if(m_routes.at(idx)){
            if(m_routesInverted.at(idx))
                return QString("!");
            else
                return QString(" ");
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
                return QBrush(Qt::darkGreen);
            else
                return QBrush(Qt::blue);
        }
        break;

    default:
        break;
    }
    return QVariant();
}

QVariant GpioRouteModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);
    bool active = false;
    switch (role) {
    case Qt::DisplayRole:
        if(orientation == Qt::Horizontal)
            return QString(QString::number(section) + ": " + m_gpioPortList.destPorts.at(section).name);
        else
            return QString(QString::number(section) + ": " + m_gpioPortList.srcPorts.at(section).name);
        break;
    case Qt::BackgroundColorRole:
        if(orientation == Qt::Horizontal) {
            active = m_destStateMap.value(m_gpioPortList.destPorts.at(section).slotId, false);
        }else {
            active = m_srcStateMap.value(m_gpioPortList.srcPorts.at(section).slotId, false);
        }
        return active ? QBrush(QColor(0, 191, 0)) : QVariant();
        break;
    default:
        break;
    }
    return QVariant();
}

void GpioRouteModel::onRoutesChanged(QList<s_gpioRoute> routes)
{
    m_routesOriginal = routes;
    emit layoutAboutToBeChanged();
    m_routes.clear();
    m_routes.resize(m_gpioPortList.srcPorts.size() * m_rowWidth);
    m_routesInverted.clear();
    m_routesInverted.resize(m_gpioPortList.srcPorts.size() * m_rowWidth);
    m_routesPersistant.clear();
    m_routesPersistant.resize(m_gpioPortList.srcPorts.size() * m_rowWidth);

    for(auto& route : routes){
        int idx = m_srcSlotMap.value(route.srcSlotId, 0) * m_rowWidth + m_destSlotMap.value(route.destSlotId, 0);
        m_routes[idx] = true;
        m_routesInverted[idx] = route.inverted;
        m_routesPersistant[idx] = route.persistant;
    }
    emit layoutChanged();
    emit dataChanged(index(0, 0),
                     index(m_rowWidth, m_rowWidth));  // update whole view
}

void GpioRouteModel::onTableClicked(const QModelIndex &index)
{
    Qt::MouseButtons pressedBtn = QGuiApplication::mouseButtons();
    int idx = index.row() * m_rowWidth + index.column();

    switch (pressedBtn) {
    case Qt::LeftButton:
    case Qt::LeftButton|Qt::RightButton:
        if(m_routes.at(idx) == false){
            setCrosspoint(index);
        }else{
            deleteCrosspoint(index);
        }
        break;
    case Qt::RightButton:
        if(m_routes.at(idx) == true){
            changeCrosspointInversion(index);
        }
        break;
    default:
        break;
    }
    m_uiParent->tableView_audioRoutes->clearSelection();
}

void GpioRouteModel::onTableChanged(const s_gpioPortList& portList)
{
    emit layoutAboutToBeChanged();
    m_gpioPortList = portList;
    m_rowWidth = m_gpioPortList.destPorts.size();
    m_srcSlotMap.clear();
    for(int i=0; i< m_gpioPortList.srcPorts.size(); i++){
        m_srcSlotMap[m_gpioPortList.srcPorts.at(i).slotId] = i;
    }
    m_destSlotMap.clear();
    for(int i=0; i< m_gpioPortList.destPorts.size(); i++){
        m_destSlotMap[m_gpioPortList.destPorts.at(i).slotId] = i;
    }
    onRoutesChanged(m_routesOriginal);
}

void GpioRouteModel::onStatesChanged(const QMap<QString, bool> changedGpios)
{
    emit layoutAboutToBeChanged();
    QMapIterator<QString, bool> i(changedGpios);
    while (i.hasNext()) {
        i.next();
        if(i.key().startsWith("GI:")) {
            m_srcStateMap[i.key()] = i.value();
        } else {
            m_destStateMap[i.key()] = i.value();
        }
    }
    emit layoutChanged();
    emit dataChanged(index(0, 0),
                     index(m_rowWidth, m_rowWidth));  // update whole view
}

void GpioRouteModel::setCrosspoint(const QModelIndex &index)
{
    bool ok = false;
    bool inv;
    if(m_uiParent->defInversion_checkBox->isChecked()){
        ok = true;
        inv = false;
    }else{
        QString invStr = QInputDialog::getItem(m_uiParent->tableView_gpioRoutes,
                                               tr("Set Crosspoint from %1 to %2").arg(m_gpioPortList.srcPorts.at(index.row()).name).arg(m_gpioPortList.destPorts.at(index.column()).name),
                                               tr("Signal Inversion:"), {"normal", "inverted"}, 0, false, &ok);
        inv = (invStr == "inverted") ? true : false;
    }
    if (ok){
        m_cmdFacade->connectGpioPort(m_srcSlotMap.key(index.row()), m_destSlotMap.key(index.column()), inv, !m_uiParent->gpioTmpRoute_checkBox->isChecked());
    }
}

void GpioRouteModel::changeCrosspointInversion(const QModelIndex &index)
{
    int idx = index.row() * m_rowWidth + index.column();
    bool ok, inv;
    QString invStr = QInputDialog::getItem(m_uiParent->tableView_gpioRoutes,
                                           tr("Change Inversion from %1 to %2").arg(m_gpioPortList.srcPorts.at(index.row()).name).arg(m_gpioPortList.destPorts.at(index.column()).name),
                                           tr("Signal Inversion:"), {"normal", "inverted"},
                                           m_routesInverted.at(idx) ? 1 : 0, false, &ok);
    inv = (invStr == "inverted") ? true : false;
    if (ok && inv != m_routesInverted.at(idx)){
        m_cmdFacade->changeGpioCrosspoint(m_srcSlotMap.key(index.row()), m_destSlotMap.key(index.column()), inv);
    }
}

void GpioRouteModel::deleteCrosspoint(const QModelIndex &index)
{
    m_cmdFacade->disconnectGpioPort(m_srcSlotMap.key(index.row()), m_destSlotMap.key(index.column()));
}
