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

WebsocketClient::WebsocketClient(QObject *parent, CmdFacade *lib) : QObject(parent), m_cmdFacade(lib)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &WebsocketClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &WebsocketClient::onDisconnected);
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
}

void WebsocketClient::onDisconnected()
{
    qDebug() << "WebSocket disconnected";
    disconnect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WebsocketClient::processMessage);
    m_connected = false;
    emit closed();
}

void WebsocketClient::processMessage(const QString &message)
{
    QJsonObject jObj, ret;
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
                } else qDebug() << "WebsocketError: " << "Unknown Return, Command eventually already expired." << Qt::endl << jObj;
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
        emit m_cmdFacade->regStateChanged(accId, status);
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::sipStatus(QJsonObject &data)
{
    int accId, status;
    QString remoteUri;
    if(jCheckInt(accId, data["accId"]) && jCheckInt(status, data["status"]) && jCheckString(remoteUri, data["remoteUri"])) {
        emit m_cmdFacade->signalSipStatus(accId, status, remoteUri);
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::callStateChanged(QJsonObject &data)
{
    int accID, role, callId, state, lastStatusCode;
    bool remoteofferer;
    long calldur;
    QString statustxt, remoteUri, calldurStr;
    if(     jCheckInt(accID, data["accID"]) &&
            jCheckInt(role, data["role"]) &&
            jCheckInt(callId, data["callId"]) &&
            jCheckBool(remoteofferer, data["remoteofferer"]) &&
            jCheckString(calldurStr, data["calldur"]) &&
            jCheckInt(state, data["state"]) &&
            jCheckInt(lastStatusCode, data["lastStatusCode"]) &&
            jCheckString(statustxt, data["statustxt"]) &&
            jCheckString(remoteUri, data["remoteUri"]))
    {
        calldur = calldurStr.toLong();
        emit m_cmdFacade->callStateChanged(accID, role, callId, remoteofferer, calldur, state, lastStatusCode, statustxt, remoteUri);
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::buddyStatus(QJsonObject &data)
{
    QString buddy;
    int status;
    if(jCheckString(buddy, data["buddy"]) && jCheckInt(status, data["status"])) {
        emit m_cmdFacade->signalBuddyStatus(buddy, status);
    } else {
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
    QList<s_audioRoutes> audioRoutes;
    QJsonArray audioRoutesArr;
    if(jCheckArray(audioRoutesArr, data["audioRoutes"])) {
        for (auto && audioRoute: qAsConst(audioRoutesArr)) {
            if(audioRoute.isObject()) {
                s_audioRoutes entry;
                audioRoutes.append(*entry.fromJSON(audioRoute.toObject()));
            }
        }
        emit m_cmdFacade->audioRoutesChanged(audioRoutes);
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::audioRoutesTableChanged(QJsonObject &data)
{
    s_audioPortList portList;
    QJsonObject portListObj;
    if(jCheckObject(portListObj, data["portList"])) {
        portList.fromJSON(portListObj);
        emit m_cmdFacade->audioRoutesTableChanged(portList);
    } else {
        qDebug() << "WebsocketError: " << "Signal " << __FUNCTION__ << " : Parameters not accepted";
    }
}

void WebsocketClient::sendCommand(QJsonObject &completeRequestObj)
{
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



Command::Command(QJsonObject &reqObj, QObject *parent, WebsocketClient *wsc) : QObject(parent), m_websocketClient(wsc), m_reqObj(reqObj)
{
    m_commandNr = m_websocketClient->addPendingCommand(this);
    m_reqObj["cmdID"] = QString::number(m_commandNr);
}

bool Command::execute()
{
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
