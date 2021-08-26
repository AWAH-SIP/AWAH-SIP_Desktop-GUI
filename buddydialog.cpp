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
    for(auto &codec :  m_lib->getActiveCodecs()){
            ui->comboBox_codec->addItem(codec.displayName);
    }
    int index = ui->comboBox_codec->findText(m_buddy.codec.displayName);
    ui->comboBox_codec->setCurrentIndex(index);

    for(auto& account : *m_lib->getAccounts()){
        ui->comboBox_account->addItem(account.name);
    }

}

buddydialog::~buddydialog()
{
    delete ui;
}

void buddydialog::on_pushButton_cancel_clicked()
{
    m_buddy.buddyUrl = "";
    buddydialog::close();
}

void buddydialog::on_pushButton_ok_clicked()
{
    m_buddy.Name = ui->lineEdit_name->text();
    m_buddy.buddyUrl = ui->lineEdit_number->text();
    m_lib->getAccountByID(ui->comboBox_account->currentIndex());
    for(auto& account : *m_lib->getAccounts()){
        if (account.name == ui->comboBox_account->currentText()){
            m_buddy.accUid = account.uid;
            break;
        }
    }
    for(auto &codec :  m_lib->getActiveCodecs()){
            if(ui->comboBox_codec->currentText()== codec.displayName){
                m_buddy.codec = codec;
            }
    }
    buddydialog::close();
}

void buddydialog::on_pushButton_settings_clicked()
{

}
