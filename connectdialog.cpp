#include "connectdialog.h"
#include "ui_connectdialog.h"
#include "AWAHSipDesktopGUI.h"

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
    m_websocketClient = new WebsocketClient(this);
    QSettings settings("awah", "AWAHSipDesktopGUI");
    wsUrl = settings.value("websocketUrl", QUrl("ws://127.0.0.1:2924")).toUrl();
    ui->lineEdit->setText(wsUrl.host());
    ui->spinBox->setValue(wsUrl.port());
    connect(m_websocketClient, SIGNAL (open()), this, SLOT( on_Connected()));
    connect(m_websocketClient, SIGNAL (closed()), this, SLOT( on_Disconnected()));
    connect(m_websocketClient, SIGNAL (errorOccurred(QAbstractSocket::SocketError)), this, SLOT( on_Error(QAbstractSocket::SocketError)));
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setInterval(5000);
    connect(m_timeoutTimer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
}

ConnectDialog::~ConnectDialog()
{
    QSettings settings("awah", "AWAHSipDesktopGUI");
    settings.setValue("websocketUrl", wsUrl);
    delete ui;
}

void ConnectDialog::on_pushButton_connect_clicked()
{
    if(m_websocketClient->isConnected()){
        m_websocketClient->closeConnection();
    }
    else{
        if(wsUrl.isValid()){
            ui->label_connstate->setText("trying to connect...");
            ui->pushButton_connect->setText("disconnect");
            m_websocketClient->openConnection(wsUrl);
            m_timeoutTimer->start();
        }
        else{
            ui->label_connstate->setText("invalid Adress!!");
        }
    }
}

void ConnectDialog::on_pushButton_close_clicked()
{
    delete DesktopGui;
    ConnectDialog::close();
}


void ConnectDialog::on_Connected(){
    DesktopGui = new AWAHSipDesktopGUI(this, m_websocketClient);
    ui->label_connstate->setText("connected");
    DesktopGui->show();
    m_timeoutTimer->stop();
}

void ConnectDialog::on_Disconnected(){
     ui->label_connstate->setText("connection closed");
     ui->pushButton_connect->setText("connect");
     if(DesktopGui != nullptr){
        DesktopGui->close();
     }
}

void ConnectDialog::on_Error(QAbstractSocket::SocketError error){
    QString tmp;
    switch (error) {
    case QAbstractSocket::RemoteHostClosedError:
      break;
    case QAbstractSocket::HostNotFoundError:
       tmp = "The host was not found.";
      break;
    case QAbstractSocket::ConnectionRefusedError:
      tmp = "The connection was refused by the peer. ";
      break;
    default:
      tmp = "Error opening connection";
    }
    ui->label_connstate->setText(tmp);
    m_websocketClient->closeConnection();
}

void ConnectDialog::on_lineEdit_textChanged(const QString &arg1)
{
    wsUrl.setHost(arg1);
}

void ConnectDialog::on_spinBox_valueChanged(int arg1)
{
    wsUrl.setPort(arg1);
}

void ConnectDialog::timeoutSlot()
{
    ui->label_connstate->setText("Connection timed out");
    ui->pushButton_connect->setText("connect");
    m_websocketClient->closeConnection();
}
