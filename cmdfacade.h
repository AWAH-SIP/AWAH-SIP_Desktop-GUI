#ifndef CMDFACADE_H
#define CMDFACADE_H

#include <QObject>
#include "lib/types.h"

class CmdFacade : public QObject
{
    Q_OBJECT

public:
    explicit CmdFacade(QObject *parent = nullptr);

    // Public API - Accounts
    int createAccount(QString accountName, QString server, QString user, QString password, QString filePlayPath, QString fileRecPath) const
        { return 0; };
    int modifyAccount(int index, QString accountName, QString server, QString user, QString password, QString filePlayPath, QString fileRecPath) const
        { return 0; };
    void removeAccount(int index) const { return; };
    QList <s_account>* getAccounts() {  return &m_getAccounts; };
    void makeCall(QString number, int AccID) const { return; };
    bool hangupCall(int callId, int AccID) const { return true; };
    void acceptCall(int callId, int AccID) const { return; };
    bool holdCall(int callId, int AccID) const { return true; };
    bool transferCall(int callId, int AccID, QString destination) const { return true; };
    QStringList dumpCall(int callId, int AccID) const { QStringList ret; return ret; };
    QMap<QString, int> getStreamInfo(int callID, int AccID) const { QMap<QString, int> ret; return ret; };
    const QList<s_callHistory>* getCallHistory(int AccID) const { return &m_getCallHistory; };
    const s_account* getAccountByID(int ID);

    // Public API - AudioRouter
    QList <s_audioRoutes> getAudioRoutes() const {return m_getAudioRoutes; };
    QStringList listSoundDev() const { return m_listSoundDev; };
    int addAudioDevice(int recordDevId, int playbackDevId) const { return 0; };
    int removeAudioDevice(int DevIndex) const { return 0; };
    int addFilePlayer(QString Name, QString File) const { return 0; };
    int addFileRecorder(QString File) const { return 0; };
    const s_audioPortList& getConfPortsList() const { return m_getConfPortsList; };
    int connectConfPort(int src_slot, int sink_slot, float level, bool persistant = true) const
        { return 0; };
    int disconnectConfPort(int src_slot, int sink_slot) const { return 0; };
    int changeConfPortLevel(int src_slot, int sink_slot, float level) const { return 0; };
    int addToneGen(int freq) const { return 0; };
    QList<s_audioDevices>* getAudioDevices() {  return &m_getAudioDevices; };

    // Public API - Buddies
    bool registerBuddy(int AccID, QString buddyUrl) const { return true; };
    bool deleteBuddy(int AccID, QString buddyUrl) const { return true; };

    // Public API - Codecs
    QStringList listCodec() const { return m_listCodec; };
    void selectCodec(QString selectedcodec) const { return; };
    const QMap<QString, s_setting> getCodecParam(QString codecId) const { return m_getCodecParam; };
    int setCodecParam(QString codecId, QMap<QString, s_setting> map) const { return 0; };

    // Public API - Log
    QStringList readNewestLog() const { return m_readNewestLog;};

    // Public API - MessageManager
    void sendDtmf(int callId, int AccID, QString num) const { return; };

    // Public API - Settings
    const QMap<QString, s_setting> getSettings() const { return m_getSettings; };
    void setSettings(QMap<QString, s_setting> map) { return; } ;
    const QMap<QString, s_setting> getCodecPriorities() const {  return m_getCodecPriorities; };
    void setSCodecPriorities(QMap<QString, s_setting> map) { return; } ;

signals:
    void regStateChanged(int accId, bool status);
    void signalSipStatus(int accId, int status, QString remoteUri);
    void callStateChanged(int accID, int role, int callId, bool remoteofferer, long calldur, int state, int lastStatusCode, QString statustxt, QString remoteUri);
    void signalBuddyStatus(QString buddy, int status);
    void signalMessage(QString type, QByteArray message);
    void logMessage(QString msg);
    void audioRoutesChanged(QList<s_audioRoutes> audioRoutes);
    void audioRoutesTableChanged(const s_audioPortList& portList);

private:

    // Vars for API-Calling
    QList<s_account> m_getAccounts;
    QList<s_callHistory> m_getCallHistory;
    QList <s_audioRoutes> m_getAudioRoutes;
    QStringList m_listSoundDev;
    s_audioPortList m_getConfPortsList;
    QList<s_audioDevices> m_getAudioDevices;
    QStringList m_listCodec;
    QMap<QString, s_setting> m_getCodecParam;
    QStringList m_readNewestLog;
    QMap<QString, s_setting> m_getSettings;
    QMap<QString, s_setting> m_getCodecPriorities;

};

#endif // CMDFACADE_H
