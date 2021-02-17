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
            ui->label_connstate->setText("trying to connect");
            ui->pushButton_connect->setText("disconnect");
            m_websocketClient->openConnection(wsUrl);
        }
        else{
            ui->label_connstate->setText("invalid Adress!!");
        }
        m_websocketClient->testEcho();
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
}

void ConnectDialog::on_Disconnected(){
     ui->label_connstate->setText("connection closed");
     ui->pushButton_connect->setText("connect");
     DesktopGui->deleteLater();
}

void ConnectDialog::on_lineEdit_textChanged(const QString &arg1)
{
    wsUrl.setHost(arg1);
}

void ConnectDialog::on_spinBox_valueChanged(int arg1)
{
    wsUrl.setPort(arg1);
}
