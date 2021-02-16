#ifndef CMDFACADE_H
#define CMDFACADE_H

#include <QObject>
#include "lib/types.h"
#include "websocketclient.h"

class CmdFacade : public QObject
{
    Q_OBJECT

public:
    explicit CmdFacade(QObject *parent = nullptr, WebsocketClient *wsClient = nullptr);

    // Public API - Accounts
    void createAccount(QString accountName, QString server, QString user, QString password, QString filePlayPath, QString fileRecPath) const;
    void modifyAccount(int index, QString accountName, QString server, QString user, QString password, QString filePlayPath, QString fileRecPath) const;
    void removeAccount(int index) const;
    QList <s_account>* getAccounts();
    void makeCall(QString number, int AccID) const;
    void hangupCall(int callId, int AccID) const;
    void acceptCall(int callId, int AccID) const;
    void holdCall(int callId, int AccID) const;
    void transferCall(int callId, int AccID, QString destination) const;
    QJsonObject getCallInfo(int callID, int AccID) const;
    const QList<s_callHistory>* getCallHistory(int AccID);
    const s_account* getAccountByID(int ID);

    // Public API - AudioRouter
    QList <s_audioRoutes> getAudioRoutes();
    QStringList listSoundDev();
    int addAudioDevice(int recordDevId, int playbackDevId) const;
    int removeAudioDevice(int DevIndex) const;
    int addFilePlayer(QString Name, QString File) const;
    int addFileRecorder(QString File) const;
    const s_audioPortList& getConfPortsList();
    int connectConfPort(int src_slot, int sink_slot, float level, bool persistant = true) const;
    int disconnectConfPort(int src_slot, int sink_slot) const;
    int changeConfPortLevel(int src_slot, int sink_slot, float level) const;
    int addToneGen(int freq) const;
    QList<s_audioDevices>* getAudioDevices();

    // Public API - Buddies
    bool registerBuddy(int AccID, QString buddyUrl) const;
    bool deleteBuddy(int AccID, QString buddyUrl) const;

    // Public API - Codecs
    QStringList listCodec();
    void selectCodec(QString selectedcodec) const;
    const QJsonObject getCodecParam(QString codecId) const;
    int setCodecParam(QString codecId, QJsonObject codecParam) const;

    // Public API - Log
    QStringList readNewestLog();

    // Public API - MessageManager
    void sendDtmf(int callId, int AccID, QString num) const;

    // Public API - Settings
    const QJsonObject* getSettings();
    void setSettings(QJsonObject editedSetting);
    const QJsonObject getCodecPriorities();
    void setSCodecPriorities(QJsonObject Codecpriority);

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
    WebsocketClient *m_wsClient;
    // Vars for API-Calling
    QList<s_account> m_getAccounts;
    QList<s_callHistory> m_getCallHistory;
    s_account m_getAccountByID;             //TODO: Der muss weg der wird nur probleme machen...
    QList <s_audioRoutes> m_getAudioRoutes;
    QStringList m_listSoundDev;
    s_audioPortList m_getConfPortsList;
    QList<s_audioDevices> m_getAudioDevices;
    QStringList m_listCodec;
    QStringList m_readNewestLog;
    QJsonObject m_getSettings;
    QJsonObject m_getCodecPriorities;

    friend class WebsocketClient;
};

#endif // CMDFACADE_H
