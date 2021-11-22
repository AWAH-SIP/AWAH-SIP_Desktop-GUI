#include "buddydialog.h"
#include "ui_buddydialog.h"
#include "codecsettings.h"

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
    s_account* buddyaccount = nullptr;
    buddyaccount = m_lib->getAccountByUID(m_buddy.accUid);
    if (buddyaccount != nullptr){
        ui->comboBox_account->setCurrentText(buddyaccount->name);
    }
    m_selectedCodec = m_buddy.codec;
}

buddydialog::~buddydialog()
{
    if(m_CodecSettings != nullptr){
        delete m_CodecSettings;
    }
    delete ui;
}

void buddydialog::on_pushButton_cancel_clicked()
{
    m_buddy = s_buddy();
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
    m_buddy.codec = m_selectedCodec;
    buddydialog::close();
}

void buddydialog::on_pushButton_settings_clicked()
{
    if(m_CodecSettings == nullptr){
        m_CodecSettings = new CodecSettings(m_selectedCodec,this,m_lib);
    }
    m_CodecSettings->setModal(true);
    m_CodecSettings->exec();
}

void buddydialog::on_comboBox_codec_currentIndexChanged(const QString &arg1)
{
    for(auto &codec :  m_lib->getActiveCodecs()){
        if(codec.displayName == arg1){
            m_selectedCodec = codec;
        }
    }
}
