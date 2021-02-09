#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QtWebSockets>

QT_FORWARD_DECLARE_CLASS(CmdFacade)
QT_FORWARD_DECLARE_CLASS(Command)

#define REPLY_TIMEOUT 60000

class WebsocketClient : public QObject
{
    Q_OBJECT
public:
    explicit WebsocketClient(QObject *parent = nullptr, CmdFacade *lib = nullptr);
    void openConnection(QUrl &url);
    void closeConnection();
    void testEcho();

signals:
    void closed();

private slots:
    void onConnected();
    void onDisconnected();
    void processMessage(const QString &message);
    // API Signals
    void regStateChanged(QJsonObject &data);
    void sipStatus(QJsonObject &data);
    void callStateChanged(QJsonObject &data);
    void buddyStatus(QJsonObject &data);
    //void message(QJsonObject &data);
    void logMessage(QJsonObject &data);
    void audioRoutesChanged(QJsonObject &data);
    void audioRoutesTableChanged(QJsonObject &data);

private:
    void sendCommand(QJsonObject &completeRequestObj);
    quint32 addPendingCommand(Command *cmd);

    CmdFacade *m_cmdFacade;
    QWebSocket m_webSocket;
    QUrl m_url;
    bool m_connected = false;
    quint32 m_reqcounter = 0;
    QMap<quint32, Command*> m_cmdRequestQueue;

    friend class Command;
};

class Command : public QObject
{
    Q_OBJECT
public:
    explicit Command(QJsonObject &reqObj, QObject *parent = nullptr, WebsocketClient *wsc = nullptr);
    bool execute();
    QJsonObject& getReturnData() { return m_returnData; };
    QJsonObject& getReturnError() { return m_returnError; };
    bool hasError() { return m_error; };

signals:
    void requestReturned();

private slots:
    void returned(QJsonObject &data, QJsonObject &error);

private:
    WebsocketClient *m_websocketClient;
    quint32 m_commandNr;
    QJsonObject m_reqObj;
    QJsonObject m_returnData;
    QJsonObject m_returnError;
    bool m_error = false;

    friend class WebsocketClient;
};

#endif // WEBSOCKETCLIENT_H