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
    history = settings.value("history").value<QList<QUrl>>();
    if(!history.isEmpty()){
        wsUrl = history.first();
    }
    QList<QUrl>::iterator i;
    for (i = history.begin(); i != history.end(); ++i){
        ui->listWidget->addItem(i->toDisplayString());
    }
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
    settings.setValue("history", QVariant::fromValue(history));
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
    ConnectDialog::close();
}


void ConnectDialog::on_Connected(){
    DesktopGui = new AWAHSipDesktopGUI(nullptr, m_websocketClient);
    ui->label_connstate->setText("connected");
    DesktopGui->show();
    this->hide();

    if (history.lastIndexOf(wsUrl) == -1){              // only add url to history if it does not exist
        history.prepend(wsUrl);
        ui->listWidget->insertItem(0, wsUrl.toDisplayString());
        if (history.count() > 10){                      // restrict history to 10 entrys
            history.removeLast();
        }
    }
    else history.move(history.lastIndexOf(wsUrl.toDisplayString()),0);    // move existing entry to the first entry

    while(ui->listWidget->count()>0)        // we have to clear the list widget and refill it in order to update it
    {
      delete ui->listWidget->takeItem(0);
    }

    QList<QUrl>::iterator i;
    for (i = history.begin(); i != history.end(); ++i){
        ui->listWidget->addItem(i->toDisplayString());
    }
    wsUrl = history.at(0);
    ui->lineEdit->setText(wsUrl.host());
    ui->spinBox->setValue(wsUrl.port());
    m_timeoutTimer->stop();
}

void ConnectDialog::on_Disconnected(){
     ui->label_connstate->setText("connection closed");
     ui->pushButton_connect->setText("connect");
     if(DesktopGui->isVisible()){
        DesktopGui->close();
     }
     this->show();
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
    m_timeoutTimer->stop();
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

void ConnectDialog::on_listWidget_currentRowChanged(int currentRow)
{
        if(currentRow >0 && currentRow < history.count()){
            wsUrl = history.at(currentRow);
            ui->lineEdit->setText(wsUrl.host());
            ui->spinBox->setValue(wsUrl.port());
        }
}
