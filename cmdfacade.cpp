#include "cmdfacade.h"

CmdFacade::CmdFacade(QObject *parent, WebsocketClient *wsClient) : QObject(parent), m_wsClient(wsClient)
{

}

/*  Idee wenn accounts geupdated über chenged slot...
const s_account* CmdFacade::getAccountByID(int ID){
    for(auto& account : m_getAccounts){
        if(account.AccID == ID){
            return &account;
        }
    }
    return nullptr;
}
*/

void CmdFacade::initializeVariables(){
    QJsonObject obj, data;
    obj["command"] = "getAllVariables";
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(!cmd.hasError()) {
        m_Accounts.clear();
        QJsonArray accountsArr = cmd.getReturnData()["accountsArray"].toArray();
        for (auto && account : qAsConst(accountsArr)) {
            QJsonObject entryObj = account.toObject();
            s_account entry;
            entry.fromJSON(entryObj);
            m_Accounts.append(entry);
            emit StateChanged();
        }
        m_AudioRoutes.clear();
        QJsonArray audioRoutesArr = cmd.getReturnData()["audioRoutesArray"].toArray();
        for (auto && audioRoute : qAsConst(audioRoutesArr)) {
            QJsonObject entryObj = audioRoute.toObject();
            s_audioRoutes entry;
            entry.fromJSON(entryObj);
            m_AudioRoutes.append(entry);
        }

        m_IoDevices.clear();
        QJsonArray ioDevArr = cmd.getReturnData()["ioDevicesArray"].toArray();
        for (auto && ioDev : qAsConst(ioDevArr)) {
            QJsonObject entryObj = ioDev.toObject();
            s_IODevices entry;
            entry.fromJSON(entryObj);
            m_IoDevices.append(entry);
        }

        QJsonObject audioPortListObj = cmd.getReturnData()["confPortList"].toObject();
        m_getConfPortsList.fromJSON(audioPortListObj);

        m_gpioRoutes.clear();
        QJsonArray gpioRoutesArr = cmd.getReturnData()["gpioRoutesArray"].toArray();
        for (auto && gpioRoute : qAsConst(gpioRoutesArr)) {
            QJsonObject entryObj = gpioRoute.toObject();
            s_gpioRoute entry;
            entry.fromJSON(entryObj);
            m_gpioRoutes.append(entry);
        }

        QJsonObject gpioPortListObj = cmd.getReturnData()["gpioPortList"].toObject();
        m_gpioPortList.fromJSON(gpioPortListObj);

        m_getGpioStates.clear();
        QJsonArray gpioStatesArr = cmd.getReturnData()["gpioStatesArray"].toArray();
        for (auto && gpioState: qAsConst(gpioStatesArr)) {
            if(gpioState.isObject()) {
                QJsonObject gpioStateObj = gpioState.toObject();
                m_getGpioStates[gpioStateObj["slotID"].toString()] = gpioStateObj["state"].toBool();
            }
        }

        m_buddyList.clear();
        QJsonArray buddyArr = cmd.getReturnData()["buddyArray"].toArray();
        for (auto && buddyEntry : qAsConst(buddyArr)) {
            s_buddy buddy;
            buddy.fromJSON(buddyEntry.toObject());
            m_buddyList.append(buddy);

        }
    }
}

// Public API - Accounts
void CmdFacade::createAccount(QString accountName, QString server, QString user, QString password, QString filePlayPath, QString fileRecPath, bool fixedJitterBuffer, uint fixedJitterBufferValue) const
{
    QJsonObject obj, data;
    obj["command"] = "createAccount";
    data["accountName"] = accountName;
    data["server"] = server;
    data["user"] = user;
    data["password"] = password;
    data["filePlayPath"] = filePlayPath;
    data["fileRecPath"] = fileRecPath;
    data["fixedJitterBuffer"] = fixedJitterBuffer;
    data["fixedJitterBufferValue"] = (int)fixedJitterBufferValue;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}

void CmdFacade::modifyAccount(QString uid, QString accountName, QString server, QString user, QString password, QString filePlayPath, QString fileRecPath,  bool fixedJitterBuffer, uint fixedJitterBufferValue) const
{
    QJsonObject obj, data;
    obj["command"] = "modifyAccount";
    data["uid"] = uid;
    data["accountName"] = accountName;
    data["server"] = server;
    data["user"] = user;
    data["password"] = password;
    data["filePlayPath"] = filePlayPath;
    data["fileRecPath"] = fileRecPath;
    data["fixedJitterBuffer"] = fixedJitterBuffer;
    data["fixedJitterBufferValue"] = (int)fixedJitterBufferValue;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}

void CmdFacade::removeAccount(QString uid) const
{
    QJsonObject obj, data;
    obj["command"] = "removeAccount";
    data["uid"] = uid;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}

QList <s_account>* CmdFacade::getAccounts()
{
   return &m_Accounts;
}

void CmdFacade::makeCall(QString number, int AccID, s_codec codec) const
{
    QJsonObject obj, data;
    obj["command"] = "makeCall";
    data["number"] = number;
    data["AccID"] = AccID;
    data["codec"] = codec.toJSON();
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}

void CmdFacade::hangupCall(int callId, int AccID) const
{
    QJsonObject obj, data;
    obj["command"] = "hangupCall";
    data["callId"] = callId;
    data["AccID"] = AccID;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}

void CmdFacade::acceptCall(int callId, int AccID) const
{
    QJsonObject obj, data;
    obj["command"] = "acceptCall";
    data["callId"] = callId;
    data["AccID"] = AccID;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}

void CmdFacade::holdCall(int callId, int AccID) const
{
    QJsonObject obj, data;
    obj["command"] = "holdCall";
    data["callId"] = callId;
    data["AccID"] = AccID;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}

void CmdFacade::transferCall(int callId, int AccID, QString destination) const
{
    QJsonObject obj, data;
    obj["command"] = "transferCall";
    data["callId"] = callId;
    data["AccID"] = AccID;
    data["destination"] = destination;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}

const QList<s_callHistory>* CmdFacade::getCallHistory(int AccID)
{
    QJsonObject obj, data;
    obj["command"] = "getCallHistory";
    data["AccID"] = AccID;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    m_getCallHistory.clear();
    cmd.execute();
    if(cmd.hasError()) {
        return &m_getCallHistory;
    } else {
        QJsonArray callHistoryArr = cmd.getReturnData()["callHistoryList"].toArray();
        for (auto && callHistEntry : qAsConst(callHistoryArr)) {
            QJsonObject entryObj = callHistEntry.toObject();
            s_callHistory entry;
            entry.fromJSON(entryObj);
            m_getCallHistory.append(entry);
        }
        return &m_getCallHistory;
    }
}

const s_account* CmdFacade::getAccountByID(int ID)
{
    QJsonObject obj, data;
    obj["command"] = "getAccountByID";
    data["ID"] = ID;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    m_getCallHistory.clear();
    cmd.execute();
    if(cmd.hasError()) {
        return nullptr;
    } else {
        QJsonObject accountObj = cmd.getReturnData()["Account"].toObject();
        return m_getAccountByID.fromJSON(accountObj);
    }
}

s_account* CmdFacade::getAccountByUID(QString uid){
    for(auto& account : m_Accounts){
        if(account.uid == uid){
           return &account;
        }
    }
    return nullptr;
}

// Public API - AudioRouter
QList <s_audioRoutes> CmdFacade::getAudioRoutes()
{
    return m_AudioRoutes;
}

QStringList CmdFacade::listInputSoundDev()
{
    QJsonObject obj, data;
    obj["command"] = "listInputSoundDev";
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(!cmd.hasError()) {
        m_listSoundDev.clear();
        QJsonArray soundDevArr = cmd.getReturnData()["soundInputDeviceArray"].toArray();
        for (auto && soundDev : qAsConst(soundDevArr)) {
            m_listSoundDev.append(soundDev.toString());
        }
    }
    return m_listSoundDev;
}

QStringList CmdFacade::listOutputSoundDev()
{
    QJsonObject obj, data;
    obj["command"] = "listOutputSoundDev";
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(!cmd.hasError()) {
        m_listSoundDev.clear();
        QJsonArray soundDevArr = cmd.getReturnData()["soundOutputDeviceArray"].toArray();
        for (auto && soundDev : qAsConst(soundDevArr)) {
            m_listSoundDev.append(soundDev.toString());
        }
    }
    return m_listSoundDev;
}

void CmdFacade::addAudioDevice(int recordDevId, int playbackDevId) const
{
    QJsonObject obj, data;
    obj["command"] = "addAudioDevice";
    data["recordDevId"] = recordDevId;
    data["playbackDevId"] = playbackDevId;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
}

void CmdFacade::removeAudioDevice(QString uid) const
{
    QJsonObject obj, data;
    obj["command"] = "removeAudioDevice";
    data["DevUid"] = uid;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
}


void CmdFacade::addFilePlayer(QString Name, QString File) const
{
    QJsonObject obj, data;
    obj["command"] = "addFilePlayer";
    data["Name"] = Name;
    data["File"] = File;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
}

void CmdFacade::addFileRecorder(QString File) const
{
    QJsonObject obj, data;
    obj["command"] = "addFileRecorder";
    data["File"] = File;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
}

const s_audioPortList& CmdFacade::getConfPortsList()
{
    return m_getConfPortsList;
}

int CmdFacade::connectConfPort(int src_slot, int sink_slot, float level, bool persistant) const
{
    QJsonObject obj, data;
    obj["command"] = "connectConfPort";
    data["src_slot"] = src_slot;
    data["sink_slot"] = sink_slot;
    data["level"] = QString::number(level);
    data["persistant"] = persistant;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return -1;
    } else {
        return cmd.getReturnData()["returnValue"].toInt();
    }
}

int CmdFacade::disconnectConfPort(int src_slot, int sink_slot) const
{
    QJsonObject obj, data;
    obj["command"] = "disconnectConfPort";
    data["src_slot"] = src_slot;
    data["sink_slot"] = sink_slot;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return -1;
    } else {
        return cmd.getReturnData()["returnValue"].toInt();
    }
}

int CmdFacade::changeConfPortLevel(int src_slot, int sink_slot, float level) const
{
    QJsonObject obj, data;
    obj["command"] = "changeConfPortLevel";
    data["src_slot"] = src_slot;
    data["sink_slot"] = sink_slot;
    data["level"] = QString::number(level);
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return -1;
    } else {
        return cmd.getReturnData()["returnValue"].toInt();
    }
}

void CmdFacade::addToneGen(int freq) const
{
    QJsonObject obj, data;
    obj["command"] = "addToneGen";
    data["freq"] = freq;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
}

int CmdFacade::getSoundDevID(QString DeviceName)
{
    QJsonObject obj, data;
    obj["command"] = "getSoundDevID";
    data["DeviceName"] = DeviceName;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return -1;
    } else {
        return cmd.getReturnData()["returnValue"].toInt();
    }
}

void CmdFacade::changeConfportsrcName(const QString portName, const QString customName) const
{
    QJsonObject obj, data;
    obj["command"] = "changeConfportsrcName";
    data["portName"] = portName;
    data["customName"] = customName;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
}

void CmdFacade::changeConfportdstName(const QString portName, const QString customName) const
{
    QJsonObject obj, data;
    obj["command"] = "changeConfportdstName";
    data["portName"] = portName;
    data["customName"] = customName;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
}

QList<s_IODevices>& CmdFacade::getIoDevices()
{
    return m_IoDevices;
}


// Public API - Buddies
void CmdFacade::addBuddy(QString buddyUrl, QString name, QString accUid, QJsonObject codec)  const
{
    QJsonObject obj, data;
    obj["command"] = "addBuddy";
    data["buddyUrl"] = buddyUrl;
    data["name"] = name;
    data["accUid"] = accUid;
    data["codec"] = codec;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
}

void CmdFacade::editBuddy(QString buddyUrl, QString name, QString accUid, QJsonObject codec, QString uid) const
{
    QJsonObject obj, data;
    obj["command"] = "editBuddy";
    data["buddyUrl"] = buddyUrl;
    data["name"] = name;
    data["accUid"] = accUid;
    data["codec"] = codec;
    data["uid"] = uid;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
}

void CmdFacade::removeBuddy(QString uid) const
{
    QJsonObject obj, data;
    obj["command"] = "removeBuddy";
    data["buddyUid"] = uid;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
}


const QList<s_buddy>& CmdFacade::getBuddies() const
{
    return m_buddyList;
}

// Public API - Codecs

QList<s_codec> CmdFacade::getActiveCodecs()
{
    QList<s_codec> codecList;
    QJsonObject obj, data;
    obj["command"] = "getActiveCodecs";
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(!cmd.hasError()) {
        QJsonArray codecArr = cmd.getReturnData()["codecArray"].toArray();
        for (auto && codecEntry : qAsConst(codecArr)) {
            s_codec codec;
            codec.fromJSON(codecEntry.toObject());
            codecList.append(codec);
        }
    }
    return codecList;
}

// Public API - GpioDeviceManager
const QString CmdFacade::createGpioDev(QJsonObject &newDev)
{
    QJsonObject obj, data;
    QString errorMsg;
    obj["command"] = "createGpioDev";
    data["newDev"] =newDev;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(!cmd.hasError()) {
        errorMsg = cmd.getReturnData()["returnValue"].toString();
    }
    return errorMsg;
}

void CmdFacade::removeGpioDevice(QString uid)
{
    QJsonObject obj, data;
    obj["command"] = "removeGpioDevice";
    data["uid"] = uid;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}

const QJsonObject CmdFacade::getGpioDevTypes() const
{
    QJsonObject obj, data, GpioDevTypes;
    obj["command"] = "getGpioDevTypes";
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(!cmd.hasError()) {
        GpioDevTypes = cmd.getReturnData()["GpioDevTypes"].toObject();
    }
    return GpioDevTypes;
}


// Public API - GpioRouter
const QList<s_gpioRoute>& CmdFacade::getGpioRoutes()
{
    return m_gpioRoutes;
}

const s_gpioPortList& CmdFacade::getGpioPortsList()
{
    return m_gpioPortList;
}

void CmdFacade::connectGpioPort(QString srcSlotId, QString destSlotId, bool inverted, bool persistant)
{
    QJsonObject obj, data;
    obj["command"] = "connectGpioPort";
    data["srcSlotId"] = srcSlotId;
    data["destSlotId"] = destSlotId;
    data["inverted"] = inverted;
    data["persistant"] = persistant;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}

void CmdFacade::disconnectGpioPort(QString srcSlotId, QString destSlotId)
{
    QJsonObject obj, data;
    obj["command"] = "disconnectGpioPort";
    data["srcSlotId"] = srcSlotId;
    data["destSlotId"] = destSlotId;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}

void CmdFacade::changeGpioCrosspoint(QString srcSlotId, QString destSlotId, bool inverted)
{
    QJsonObject obj, data;
    obj["command"] = "changeGpioCrosspoint";
    data["srcSlotId"] = srcSlotId;
    data["destSlotId"] = destSlotId;
    data["inverted"] = inverted;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}

const QMap<QString, bool> CmdFacade::getGpioStates()
{
    return m_getGpioStates;
}

// Public API - Log
QStringList CmdFacade::readNewestLog()
{
    QJsonObject obj, data;
    obj["command"] = "readNewestLog";
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(!cmd.hasError()) {
        m_readNewestLog.clear();
        QJsonArray logArr = cmd.getReturnData()["logArray"].toArray();
        for (auto && logEntry : qAsConst(logArr)) {
            m_readNewestLog.append(logEntry.toString());
        }
    }
    return m_readNewestLog;
}


// Public API - MessageManager
void CmdFacade::sendDtmf(int callId, int AccID, QString num) const
{
    QJsonObject obj, data;
    obj["command"] = "sendDtmf";
    data["callId"] = callId;
    data["AccID"] = AccID;
    data["num"] = num;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}


// Public API - Settings
const QJsonObject* CmdFacade::getSettings()
{
    QJsonObject obj, data;
    obj["command"] = "getSettings";
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(!cmd.hasError()) {
        m_getSettings = cmd.getReturnData()["settingsObj"].toObject();
    }
    return &m_getSettings;
}

void CmdFacade::setSettings(QJsonObject editedSetting)
{
    QJsonObject obj, data;
    obj["command"] = "setSettings";
    data["editedSetting"] = editedSetting;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}

const QJsonObject CmdFacade::getCodecPriorities()
{
    QJsonObject obj, data;
    obj["command"] = "getCodecPriorities";
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(!cmd.hasError()) {
        m_getCodecPriorities = cmd.getReturnData()["codecPrioritiesObj"].toObject();
    }
    return m_getCodecPriorities;
}

void CmdFacade::setCodecPriorities(QJsonObject Codecpriority)
{
    QJsonObject obj, data;
    obj["command"] = "setCodecPriorities";
    data["Codecpriority"] = Codecpriority;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}
