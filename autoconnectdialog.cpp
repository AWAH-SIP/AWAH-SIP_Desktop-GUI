#include "autoconnectdialog.h"
#include "ui_autoconnectdialog.h"

autoconnectdialog::autoconnectdialog(QWidget *parent, CmdFacade *lib, const s_account *account) :
    QDialog(parent),
    ui(new Ui::autoconnectdialog),
    m_cmdFacade(lib), m_Account(account)
{
    ui->setupUi(this);
    m_buddies = m_cmdFacade->getBuddies();
    buddyUID = m_Account->autoconnectToBuddyUID;
    ui->comboBox->addItem("no autoconnect");
    for (auto && buddy : m_buddies) {
        if(buddy.accUid == m_Account->uid){
            ui->comboBox->addItem(buddy.Name);
        }
        if(buddy.uid == m_Account->autoconnectToBuddyUID){
            int index = ui->comboBox->findText(buddy.Name);
            if ( index != -1 ) { // -1 for not found
                ui->comboBox->setCurrentIndex(index);
            }
        }
    }
}

autoconnectdialog::~autoconnectdialog()
{
    delete ui;
}


void autoconnectdialog::on_pushButtonOk_clicked()
{
    m_cmdFacade->modifyAccount(m_Account->uid, m_Account->name, m_Account->serverURI, m_Account->user,"" ,m_Account->FilePlayPath, m_Account->FileRecordPath, m_Account->FileRecordRXonly,
                               m_Account->fixedJitterBuffer, m_Account->fixedJitterBufferValue, buddyUID, true);
     autoconnectdialog::close();
}


void autoconnectdialog::on_pushButtonCancel_clicked()
{
    autoconnectdialog::close();
}


void autoconnectdialog::on_comboBox_currentTextChanged(const QString &arg1)
{
    for (auto && buddy : m_buddies) {
        if(buddy.Name == arg1){
            buddyUID = buddy.uid;
        }
    }
}

