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

    void initializeVariables();

    // Public API - Accounts
    void createAccount(QString accountName, QString server, QString user, QString password, QString filePlayPath, QString fileRecPath, bool fixedJitterBuffer, uint fixedJitterBufferValue) const;
    void modifyAccount(int index, QString accountName, QString server, QString user, QString password, QString filePlayPath, QString fileRecPath, bool fixedJitterBuffer, uint fixedJitterBufferValue) const;
    void removeAccount(int index) const;
    QList <s_account>* getAccounts();
    void makeCall(QString number, int AccID) const;
    void hangupCall(int callId, int AccID) const;
    void acceptCall(int callId, int AccID) const;
    void holdCall(int callId, int AccID) const;
    void transferCall(int callId, int AccID, QString destination) const;
    const QList<s_callHistory>* getCallHistory(int AccID);
    const s_account* getAccountByID(int ID);

    QList<s_IODevices>& getIoDevices();

    // Public API - AudioRouter
    QList <s_audioRoutes> getAudioRoutes();
    QStringList listInputSoundDev();
    QStringList listOutputSoundDev();
    int addAudioDevice(int recordDevId, int playbackDevId) const;
    int removeAudioDevice(int DevIndex) const;
    int addFilePlayer(QString Name, QString File) const;
    int addFileRecorder(QString File) const;
    const s_audioPortList& getConfPortsList();
    int connectConfPort(int src_slot, int sink_slot, float level, bool persistant = true) const;
    int disconnectConfPort(int src_slot, int sink_slot) const;
    int changeConfPortLevel(int src_slot, int sink_slot, float level) const;
    int addToneGen(int freq) const;
    int getSoundDevID(QString DeviceName);

    // Public API - Buddies
    bool registerBuddy(int AccID, QString buddyUrl) const;
    bool deleteBuddy(int AccID, QString buddyUrl) const;

    // Public API - Codecs
    QStringList listCodec();
    void selectCodec(QString selectedcodec) const;
    const QJsonObject getCodecParam(QString codecId) const;
    int setCodecParam(QString codecId, QJsonObject codecParam) const;

    // Public API - GpioDeviceManager
    const QString createGpioDev(QJsonObject &newDev);
    void removeGpioDevice(QString uid);
    const QList<s_IODevices>& getGpioDevices() const;
    const QJsonObject getGpioDevTypes() const;

    // Public API - GpioRouter
    const QList<s_gpioRoute>& getGpioRoutes();
    const s_gpioPortList& getGpioPortsList();
    void connectGpioPort(QString srcSlotId, QString destSlotId, bool inverted, bool persistant = true);
    void disconnectGpioPort(QString srcSlotId, QString destSlotId);
    void changeGpioCrosspoint(QString srcSlotId, QString destSlotId, bool inverted);
    const QMap<QString, bool> getGpioStates();

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
    void StateChanged();
    void signalBuddyStatus(QString buddy, int status);
    void signalMessage(QString type, QByteArray message);
    void logMessage(QString msg);
    void audioRoutesChanged(QList<s_audioRoutes> audioRoutes);
    void audioRoutesTableChanged(const s_audioPortList& portList);
    void AccountsChanged(QList <s_account> *Accounts);
    void ioDevicesChanged(QList<s_IODevices>& ioDev);
    void callInfo(int accId, int callId,QJsonObject callInfo);
    void gpioRoutesChanged(const QList<s_gpioRoute>& routes);
    void gpioRoutesTableChanged(const s_gpioPortList& portList);
    void gpioStatesChanged(const QMap<QString, bool> changedGpios);

private:
    WebsocketClient *m_wsClient;
    // Vars for API-Calling
    QList<s_account> m_Accounts;
    QList<s_callHistory> m_getCallHistory;
    s_account m_getAccountByID;             //TODO: Der muss weg der wird nur probleme machen...
    QList <s_audioRoutes> m_AudioRoutes;
    QStringList m_listSoundDev;
    s_audioPortList m_getConfPortsList;
    QList<s_IODevices> m_IoDevices;
    QStringList m_listCodec;
    QStringList m_readNewestLog;
    QJsonObject m_getSettings;
    QJsonObject m_getCodecPriorities;
    QList<s_gpioRoute> m_gpioRoutes;
    s_gpioPortList m_gpioPortList;
    QMap<QString, bool> m_changedGpios;
    QMap<QString, bool> m_getGpioStates;

    friend class WebsocketClient;
};

#endif // CMDFACADE_H
