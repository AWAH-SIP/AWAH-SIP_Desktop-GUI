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

// Public API - Accounts
int CmdFacade::createAccount(QString accountName, QString server, QString user, QString password, QString filePlayPath, QString fileRecPath) const
{
    QJsonObject obj, data;
    obj["command"] = "createAccount";
    data["accountName"] = accountName;
    data["server"] = server;
    data["user"] = user;
    data["password"] = password;
    data["filePlayPath"] = filePlayPath;
    data["fileRecPath"] = fileRecPath;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return -1;
    } else {
        return cmd.getReturnData()["returnValue"].toInt();
    }
}

int CmdFacade::modifyAccount(int index, QString accountName, QString server, QString user, QString password, QString filePlayPath, QString fileRecPath) const
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
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return -1;
    } else {
        return cmd.getReturnData()["returnValue"].toInt();
    }
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
    QJsonObject obj, data;
    obj["command"] = "getAccounts";
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return &m_getAccounts;
    } else {
        m_getAccounts.clear();
        QJsonArray accountsArr = cmd.getReturnData()["accountsArray"].toArray();
        for (auto && account : qAsConst(accountsArr)) {
            QJsonObject entryObj = account.toObject();
            s_account entry;
            entry.fromJSON(entryObj);
            m_getAccounts.append(entry);
        }
        return &m_getAccounts;
    }
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

bool CmdFacade::hangupCall(int callId, int AccID) const
{
    QJsonObject obj, data;
    obj["command"] = "hangupCall";
    data["callId"] = callId;
    data["AccID"] = AccID;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return false;
    } else {
        return cmd.getReturnData()["returnValue"].toBool();
    }
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

bool CmdFacade::holdCall(int callId, int AccID) const
{
    QJsonObject obj, data;
    obj["command"] = "holdCall";
    data["callId"] = callId;
    data["AccID"] = AccID;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return false;
    } else {
        return cmd.getReturnData()["returnValue"].toBool();
    }
}

bool CmdFacade::transferCall(int callId, int AccID, QString destination) const
{
    QJsonObject obj, data;
    obj["command"] = "transferCall";
    data["callId"] = callId;
    data["AccID"] = AccID;
    data["destination"] = destination;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return false;
    } else {
        return cmd.getReturnData()["returnValue"].toBool();
    }
}

QJsonObject CmdFacade::getCallInfo(int callID, int AccID) const
{
    QJsonObject obj, data;
    obj["command"] = "getCallInfo";
    data["callId"] = callID;
    data["AccID"] = AccID;
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(cmd.hasError()) {
        return QJsonObject();
    } else {
        return cmd.getReturnData()["callHistoryObj"].toObject();
    }
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
    QJsonObject obj, data;
    obj["command"] = "getAudioRoutes";
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(!cmd.hasError()) {
        m_getAudioRoutes.clear();
        QJsonArray audioRoutesArr = cmd.getReturnData()["audioRoutesArray"].toArray();
        for (auto && audioRoute : qAsConst(audioRoutesArr)) {
            QJsonObject entryObj = audioRoute.toObject();
            s_audioRoutes entry;
            entry.fromJSON(entryObj);
            m_getAudioRoutes.append(entry);
        }
    }
    return m_getAudioRoutes;
}

QStringList CmdFacade::listSoundDev()
{
    QJsonObject obj, data;
    obj["command"] = "listSoundDev";
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(!cmd.hasError()) {
        m_listSoundDev.clear();
        QJsonArray soundDevArr = cmd.getReturnData()["soundDeviceArray"].toArray();
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
    QJsonObject obj, data;
    obj["command"] = "getConfPortsList";
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(!cmd.hasError()) {
        QJsonObject confPortListObj = cmd.getReturnData()["confPortList"].toObject();
        m_getConfPortsList.fromJSON(confPortListObj);
    }
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
    data["level"] = level;
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

QList<s_audioDevices>* CmdFacade::getAudioDevices()
{
    QJsonObject obj, data;
    obj["command"] = "getAudioDevices";
    obj["data"] = data;
    Command cmd(obj, this->parent(), m_wsClient);
    cmd.execute();
    if(!cmd.hasError()) {
        m_getAudioDevices.clear();
        QJsonArray audioDevArr = cmd.getReturnData()["audioDevicesArray"].toArray();
        for (auto && audioDev : qAsConst(audioDevArr)) {
            QJsonObject entryObj = audioDev.toObject();
            s_audioDevices entry;
            entry.fromJSON(entryObj);
            m_getAudioDevices.append(entry);
        }
    }
    return &m_getAudioDevices;
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
        m_getCodecPriorities = cmd.getReturnData()["settingsObj"].toObject();
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
