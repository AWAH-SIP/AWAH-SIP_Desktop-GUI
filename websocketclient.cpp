#include "websocketclient.h"
#include "cmdfacade.h"
#include <limits.h>

static bool jCheckBool(bool &ret, QJsonValueRef val) {
    if(val.isBool()){
        ret = val.toBool();
        return true;
    } else return false;
}

static bool jCheckInt(int &ret, QJsonValueRef val) {
    if(val.isDouble()){
        ret = val.toInt();
        return true;
    } else return false;
}

static bool jCheckString(QString &ret, QJsonValueRef val) {
    if(val.isString()){
        ret = val.toString();
        return true;
    } else return false;
}

static bool jCheckArray(QJsonArray &ret, QJsonValueRef val) {
    if(val.isArray()){
        ret = val.toArray();
        return true;
    } else return false;
}

static bool jCheckObject(QJsonObject &ret, QJsonValueRef val) {
    if(val.isObject()){
        ret = val.toObject();
        return true;
    } else return false;
}

static bool objectFromString(const QString& in, QJsonObject &obj)
{
    QJsonDocument doc = QJsonDocument::fromJson(in.toUtf8());
    // check validity of the document
    if(!doc.isNull()) {
        if(doc.isObject()) {
            obj = doc.object();
        } else {
            qDebug() << "Document is not an object" << Qt::endl;
            return false;
        }
    } else {
        qDebug() << "Invalid JSON...\n" << in << Qt::endl;
        return false;
    }
    return true;
}

WebsocketClient::WebsocketClient(QObject *parent) : QObject(parent)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &WebsocketClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &WebsocketClient::onDisconnected);
    connect(&m_webSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT (onError(QAbstractSocket::SocketError)));
}

void WebsocketClient::setCmdFacade(CmdFacade *lib)
{
    m_cmdFacade = lib;
}

void WebsocketClient::openConnection(QUrl &url)
{
    m_url = url;
    closeConnection();
    m_webSocket.open(QUrl(m_url));
}

void WebsocketClient::closeConnection()
{
    if(m_connected) {
        m_webSocket.close();
    }
}

void WebsocketClient::testEcho()
{
    QJsonObject obj, data;
    obj["command"] = "echo";
    data["TestKey"] = "this is a test!!!";
    obj["data"] = data;
    Command cmd(obj, this, this);
    qDebug() << "Echotest started: now";
    cmd.execute();
    qDebug() << "Echotest finished: " << cmd.getReturnData() << cmd.getReturnError();
}

void WebsocketClient::onConnected()
{
    qDebug() << "WebSocket connected";
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WebsocketClient::processMessage);
    m_connected = true;
    emit open();
}

void WebsocketClient::onDisconnected()
{
    qDebug() << "WebSocket disconnected";
    disconnect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WebsocketClient::processMessage);
    m_connected = false;
    emit closed();
}

void WebsocketClient::onError(QAbstractSocket::SocketError error)
{
    qDebug() << "Websocket error" << error;
    emit errorOccurred(error);
}

void WebsocketClient::processMessage(const QString &message)
{
    QJsonObject jObj, ret;
    qDebug() << "Websocket:  RX: " << message;
    if(objectFromString(message, jObj)) {
        QString command, signal;
        QJsonObject data, error;
        if(jCheckString(signal, jObj["signal"]) && jCheckObject(data, jObj["data"])) {
            if(!QMetaObject::invokeMethod(this, signal.toStdString().c_str(), Qt::DirectConnection, Q_ARG(QJsonObject &, data))) {
                qDebug() << "WebsocketError: " << "Recieved unknown signal.";
            }
        }else if(jCheckString(command, jObj["command"]) && jCheckObject(data, jObj["data"]) && jCheckObject(error, jObj["error"])) {
            QString cmdID;
            if(jCheckString(cmdID, jObj["cmdID"])){
                if(m_cmdRequestQueue.contains(cmdID.toUInt())) {
                    m_cmdRequestQueue.find(cmdID.toUInt()).value()->returned(data, error);
                    m_cmdRequestQueue.remove(cmdID.toUInt());
                } else {
                    if (!data.isEmpty())
                        qDebug() << "WebsocketError: " << "Unknown Return, Command eventually already expired." << Qt::endl << jObj;
                }
            } else qDebug() << "WebsocketError: " << "Unknown Command, not a Return Value." << Qt::endl << jObj;
        } else {
            qDebug() << "WebsocketError: " << "JSON does not have a 'signal' or 'command' and a 'data' Object" << Qt::endl << jObj;

        }
    } else {
        qDebug() << "WebsocketError: " << "Not valid JSON" << Qt::endl << message;

    }
}

//API-Signals
void WebsocketClient::regStateChanged(QJsonObject &data)
{
    int accId;
    bool status;
    if(jCheckInt(accId, data["accId"]) && jCheckBool(status, data["status"]) ) {
        emit m_cmdFacade->StateChanged();;
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::sipStatus(QJsonObject &data)
{
    int accId, status;
    QString remoteUri;
    if(jCheckInt(accId, data["accId"]) && jCheckInt(status, data["status"]) && jCheckString(remoteUri, data["remoteUri"])) {
        for(auto& account : m_cmdFacade->m_Accounts){
            if(account.AccID == accId){
                account.SIPStatusCode = status;
                account.SIPStatusText = remoteUri;
            }
         }
        emit m_cmdFacade->StateChanged();
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::callStateChanged(QJsonObject &data)
{
    int accID, role, callId, state, lastStatusCode;
    bool remoteofferer;
    QString statustxt, remoteUri, calldurStr;
    if(     jCheckInt(accID, data["accId"]) &&
            jCheckInt(role, data["role"]) &&
            jCheckInt(callId, data["callId"]) &&
            jCheckBool(remoteofferer, data["remoteofferer"]) &&
            jCheckInt(state, data["state"]) &&
            jCheckInt(lastStatusCode, data["lastStatusCode"]) &&
            jCheckString(statustxt, data["statustxt"]) &&
            jCheckString(remoteUri, data["remoteUri"]))
    {
        for(auto& account : m_cmdFacade->m_Accounts){
            if(account.AccID == accID){
                account.callStatusLastReason = statustxt;
                for(auto& call : account.CallList){
                    if(call["callId"].toInt() == callId ){
                        call["CallStatusText"] = statustxt;
                        call["CallStatusCode"] = state;
                        call["ConnectedTo"] = remoteUri;
                    }
                }
            }
         }
        emit m_cmdFacade->StateChanged();
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::buddyStatus(QJsonObject &data)
{
    QString buddyURL;
    int status;
    if(jCheckString(buddyURL, data["buddy"]) && jCheckInt(status, data["status"])) {
        int startPos = buddyURL.indexOf("sip:") + 4;
        int endPos = buddyURL.indexOf('@');
        int length = endPos - startPos;
        QString URL = (buddyURL.mid(startPos, length));
        for(auto &buddy : m_cmdFacade->m_buddyList){
            if(URL == buddy.buddyUrl){
                buddy.status = status;
            }
        }
        emit m_cmdFacade->signalBuddyStatus();
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::BuddyEntryChanged(QJsonObject &data)
{
    QJsonArray buddyArray;
    if(jCheckArray(buddyArray, data["buddies"])) {
        m_cmdFacade->m_buddyList.clear();
        for (auto && jbuddy: qAsConst(buddyArray)) {
            if(jbuddy.isObject()) {
                s_buddy buddy;
                m_cmdFacade->m_buddyList.append(*buddy.fromJSON(jbuddy.toObject()));
            }
        }
        emit m_cmdFacade->buddyEntryChanged();
    }else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::logMessage(QJsonObject &data)
{
    QString msg;
    if(jCheckString(msg, data["msg"]) ) {
        emit m_cmdFacade->logMessage(msg);
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::audioRoutesChanged(QJsonObject &data)
{
    QJsonArray audioRoutesArr;
    if(jCheckArray(audioRoutesArr, data["audioRoutes"])) {
        m_cmdFacade->m_AudioRoutes.clear();
        for (auto && audioRoute: qAsConst(audioRoutesArr)) {
            if(audioRoute.isObject()) {
                s_audioRoutes entry;
                m_cmdFacade->m_AudioRoutes.append(*entry.fromJSON(audioRoute.toObject()));
            }
        }
        emit m_cmdFacade->audioRoutesChanged(m_cmdFacade->m_AudioRoutes);
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::audioRoutesTableChanged(QJsonObject &data)
{
    QJsonObject portListObj;
    if(jCheckObject(portListObj, data["portList"])) {
        m_cmdFacade->m_getConfPortsList.fromJSON(portListObj);
        emit m_cmdFacade->audioRoutesTableChanged(m_cmdFacade->m_getConfPortsList);
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::confportLevelChanged(QJsonObject &data)
{
    QJsonObject routeObj;
    s_audioRoutes ChangedRoute;
    if(jCheckObject(routeObj, data["confportLevelChanged"])) {
        ChangedRoute.fromJSON(routeObj);
        QMutableListIterator<s_audioRoutes> i(m_cmdFacade->m_AudioRoutes);
        while(i.hasNext()){
            s_audioRoutes& route = i.next();
            if(route.srcSlot == ChangedRoute.srcSlot && route.destSlot == ChangedRoute.destSlot){
                route = ChangedRoute;
                emit m_cmdFacade->audioRoutesChanged(m_cmdFacade->m_AudioRoutes);
            }
        }
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::AccountsChanged(QJsonObject &data){
    QJsonArray accountsArr;
    if(jCheckArray(accountsArr, data["Accounts"])){
        m_cmdFacade->m_Accounts.clear();
        for (auto && account : qAsConst(accountsArr)) {
            QJsonObject entryObj = account.toObject();
            s_account entry;
            entry.fromJSON(entryObj);
            m_cmdFacade->m_Accounts.append(entry);
        }
    }
    emit m_cmdFacade->AccountsChanged(&m_cmdFacade->m_Accounts);
}

void WebsocketClient::ioDevicesChanged(QJsonObject &data){
    QJsonArray ioDevArr;
    if(jCheckArray(ioDevArr, data["ioDevices"])){
        m_cmdFacade->m_IoDevices.clear();
        for (auto && device : qAsConst(ioDevArr)) {
            QJsonObject deviceObj = device.toObject();
            s_IODevices ioDevice;
            ioDevice.fromJSON(deviceObj);
            m_cmdFacade->m_IoDevices.append(ioDevice);
        }
    }
    emit m_cmdFacade->ioDevicesChanged(m_cmdFacade->m_IoDevices);
}

void WebsocketClient::callInfo(QJsonObject &data){
    int accId, callId;
    QJsonObject callInfo;
    if(jCheckObject(callInfo, data["callInfo"]) && jCheckInt(accId, data["accId"]) && jCheckInt(callId, data["callId"])) {
        emit m_cmdFacade->callInfo(accId, callId, callInfo);
    }
}

void WebsocketClient::gpioRoutesChanged(QJsonObject &data)
{
    QJsonArray gpioRoutesArr;
    if(jCheckArray(gpioRoutesArr, data["gpioRoutes"])) {
        m_cmdFacade->m_gpioRoutes.clear();
        for (auto && gpioRoute: qAsConst(gpioRoutesArr)) {
            if(gpioRoute.isObject()) {
                s_gpioRoute entry;
                m_cmdFacade->m_gpioRoutes.append(*entry.fromJSON(gpioRoute.toObject()));
            }
        }
        emit m_cmdFacade->gpioRoutesChanged(m_cmdFacade->m_gpioRoutes);
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::gpioRoutesTableChanged(QJsonObject &data)
{
    QJsonObject portListObj;
    if(jCheckObject(portListObj, data["portList"])) {
        m_cmdFacade->m_gpioPortList.fromJSON(portListObj);
        emit m_cmdFacade->gpioRoutesTableChanged(m_cmdFacade->m_gpioPortList);
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::gpioStatesChanged(QJsonObject &data)
{
    QJsonArray gpioStatesArr;
    if(jCheckArray(gpioStatesArr, data["gpioStates"])) {
        m_cmdFacade->m_changedGpios.clear();
        for (auto && gpioState: qAsConst(gpioStatesArr)) {
            if(gpioState.isObject()) {
                QJsonObject gpioStateObj = gpioState.toObject();
                m_cmdFacade->m_changedGpios[gpioStateObj["slotID"].toString()] = gpioStateObj["state"].toBool();
            }
        }
        emit m_cmdFacade->gpioStatesChanged(m_cmdFacade->m_changedGpios);
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}


void WebsocketClient::sendCommand(QJsonObject &completeRequestObj)
{
    qDebug() << "Websocket:  TX: " << completeRequestObj;
    m_webSocket.sendTextMessage(QJsonDocument(completeRequestObj).toJson(QJsonDocument::Compact));
}

quint32 WebsocketClient::addPendingCommand(Command *cmd)
{
    if(m_reqcounter == std::numeric_limits<quint32>::max())
        m_reqcounter = 1;
    else m_reqcounter++;
    m_cmdRequestQueue.insert(m_reqcounter, cmd);
    return m_reqcounter;
}


bool WebsocketClient::isConnected(){
    return m_connected;
}

// ************************** Implementation of Command Class **************************
Command::Command(QJsonObject &reqObj, QObject *parent, WebsocketClient *wsc, bool noAnswer) :
            QObject(parent), m_websocketClient(wsc), m_reqObj(reqObj), m_noAnswer(noAnswer)
{
    if (!m_noAnswer)    m_commandNr = m_websocketClient->addPendingCommand(this);
    m_reqObj["cmdID"] = QString::number(m_commandNr);
}

bool Command::execute()
{
    if (m_noAnswer) {
        m_websocketClient->sendCommand(m_reqObj);
        return true;
    } else {
        QEventLoop loop;
        QTimer timer;

        timer.setSingleShot(true);
        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        connect(this, &Command::requestReturned, &loop, &QEventLoop::quit);

        timer.start(REPLY_TIMEOUT);
        m_websocketClient->sendCommand(m_reqObj);
        loop.exec();

        if (timer.isActive()) {
            timer.stop();
            return true;
        } else {
            m_error = true;
            return false;
        }
    }
}

void Command::returned(QJsonObject &data, QJsonObject &error)
{
    m_returnData = data;
    m_returnError = error;
    bool errorVal;
    if (jCheckBool(errorVal, error["code"])) {
        m_error = errorVal;
    } else {
        m_error = true;
    }
    emit requestReturned();
}

