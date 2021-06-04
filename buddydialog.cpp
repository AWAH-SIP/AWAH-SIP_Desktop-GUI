#include "buddydialog.h"
#include "ui_buddydialog.h"

buddydialog::buddydialog(CmdFacade *lib, s_buddy &buddy, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::buddydialog),
    m_lib(lib),
    m_buddy(buddy)
{
    ui->setupUi(this);
    ui->lineEdit_name->setText(m_buddy.Name);
    ui->lineEdit_number->setText(m_buddy.buddyUrl);
    ui->comboBox_codec->addItems(m_lib->listCodec());
    for(auto& account : *lib->getAccounts()){
        ui->comboBox_account->addItem(account.name);
        ui->comboBox_connect_account->addItem(QString("autoconnect from: ") + account.name);
    }
}

buddydialog::~buddydialog()
{
    delete ui;
}

void buddydialog::on_pushButton_cancel_clicked()
{
    buddydialog::close();
}

void buddydialog::on_pushButton_ok_clicked()
{

}

void buddydialog::on_pushButton_settings_clicked()
{

}
