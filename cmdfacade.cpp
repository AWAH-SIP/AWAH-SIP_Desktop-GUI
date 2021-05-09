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

        m_AudioDevices.clear();
        QJsonArray audioDevArr = cmd.getReturnData()["audioDevicesArray"].toArray();
        for (auto && audioDev : qAsConst(audioDevArr)) {
            QJsonObject entryObj = audioDev.toObject();
            s_IODevices entry;
            entry.fromJSON(entryObj);
            m_AudioDevices.append(entry);
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

        m_gpioDevices.clear();
        QJsonArray gpioDevArr = cmd.getReturnData()["gpioDevicesArray"].toArray();
        for (auto && gpioDev : qAsConst(gpioDevArr)) {
            QJsonObject entryObj = gpioDev.toObject();
            s_IODevices entry;
            entry.fromJSON(entryObj);
            m_gpioDevices.append(entry);
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

void CmdFacade::modifyAccount(int index, QString accountName, QString server, QString user, QString password, QString filePlayPath, QString fileRecPath,  bool fixedJitterBuffer, uint fixedJitterBufferValue) const
{
    QJsonObject obj, data;
    obj["command"] = "modifyAccount";
    data["index"] = index;
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

void CmdFacade::removeAccount(int index) const
{
    QJsonObject obj, data;
    obj["command"] = "removeAccount";
    data["index"] = index;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}

QList <s_account>* CmdFacade::getAccounts()
{
   return &m_Accounts;
}

void CmdFacade::makeCall(QString number, int AccID) const
{
    QJsonObject obj, data;
    obj["command"] = "makeCall";
    data["number"] = number;
    data["AccID"] = AccID;
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

int CmdFacade::addAudioDevice(int recordDevId, int playbackDevId) const
{
    QJsonObject obj, data;
    obj["command"] = "addAudioDevice";
    data["recordDevId"] = recordDevId;
    data["playbackDevId"] = playbackDevId;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return -1;
    } else {
        return cmd.getReturnData()["returnValue"].toInt();
    }
}

int CmdFacade::removeAudioDevice(int DevIndex) const
{
    QJsonObject obj, data;
    obj["command"] = "removeAudioDevice";
    data["DevIndex"] = DevIndex;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return -1;
    } else {
        return cmd.getReturnData()["returnValue"].toInt();
    }
}

int CmdFacade::addFilePlayer(QString Name, QString File) const
{
    QJsonObject obj, data;
    obj["command"] = "addFilePlayer";
    data["Name"] = Name;
    data["File"] = File;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return -1;
    } else {
        return cmd.getReturnData()["returnValue"].toInt();
    }
}

int CmdFacade::addFileRecorder(QString File) const
{
    QJsonObject obj, data;
    obj["command"] = "addFileRecorder";
    data["File"] = File;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return -1;
    } else {
        return cmd.getReturnData()["returnValue"].toInt();
    }
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

int CmdFacade::addToneGen(int freq) const
{
    QJsonObject obj, data;
    obj["command"] = "addToneGen";
    data["freq"] = freq;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return -1;
    } else {
        return cmd.getReturnData()["returnValue"].toInt();
    }
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

QList<s_IODevices>* CmdFacade::getAudioDevices()
{
    return &m_AudioDevices;
}


// Public API - Buddies
bool CmdFacade::registerBuddy(int AccID, QString buddyUrl) const
{
    QJsonObject obj, data;
    obj["command"] = "registerBuddy";
    data["AccID"] = AccID;
    data["buddyUrl"] = buddyUrl;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return false;
    } else {
        return cmd.getReturnData()["returnValue"].toBool();
    }
}

bool CmdFacade::deleteBuddy(int AccID, QString buddyUrl) const
{
    QJsonObject obj, data;
    obj["command"] = "deleteBuddy";
    data["AccID"] = AccID;
    data["buddyUrl"] = buddyUrl;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return false;
    } else {
        return cmd.getReturnData()["returnValue"].toBool();
    }
}


// Public API - Codecs
QStringList CmdFacade::listCodec()
{
    QJsonObject obj, data;
    obj["command"] = "listCodec";
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(!cmd.hasError()) {
        m_listCodec.clear();
        QJsonArray codecArr = cmd.getReturnData()["codecArray"].toArray();
        for (auto && codecEntry : qAsConst(codecArr)) {
            m_listCodec.append(codecEntry.toString());
        }
    }
    return m_listCodec;
}

void CmdFacade::selectCodec(QString selectedcodec) const
{
    QJsonObject obj, data;
    obj["command"] = "selectCodec";
    data["selectedcodec"] = selectedcodec;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}

const QJsonObject CmdFacade::getCodecParam(QString codecId) const
{
    QJsonObject obj, data;
    obj["command"] = "getCodecParam";
    data["codecId"] = codecId;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return QJsonObject();
    } else {
        return cmd.getReturnData()["codecParamObj"].toObject();
    }
}

int CmdFacade::setCodecParam(QString codecId, QJsonObject codecParam) const
{
    QJsonObject obj, data;
    obj["command"] = "setCodecParam";
    data["codecId"] = codecId;
    data["codecParam"] = codecParam;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return -1;
    } else {
        return cmd.getReturnData()["returnValue"].toInt();
    }
}

// Public API - GpioDeviceManager
void CmdFacade::createGpioDev(DeviceType type, uint inCount, uint outCount, QString devName)
{
    QJsonObject obj, data;
    obj["command"] = "createGpioDev";
    data["type"] = type;
    data["inCount"] = (int) inCount;
    data["outCount"] = (int) outCount;
    data["devName"] = devName;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
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

const QList<s_IODevices>& CmdFacade::getGpioDevices() const
{
    return m_gpioDevices;
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

void CmdFacade::setSCodecPriorities(QJsonObject Codecpriority)
{
    QJsonObject obj, data;
    obj["command"] = "setSCodecPriorities";
    data["Codecpriority"] = Codecpriority;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient, true);
    cmd.execute();
}
