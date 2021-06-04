#include "buddysettings.h"
#include "ui_buddysettings.h"


BuddySettings::BuddySettings(QWidget *parent, CmdFacade *lib) :
    QDialog(parent),
    ui(new Ui::BuddySettings),
    m_cmdFacade(lib)
{
    ui->setupUi(this);
}

BuddySettings::~BuddySettings()
{
    delete m_buddyDialog;
    delete ui;
}

void BuddySettings::on_pushButton_close_clicked()
{
    BuddySettings::close();
}

void BuddySettings::on_pushButton_remove_clicked()
{

}

void BuddySettings::on_pushButton_edit_clicked()
{

}

void BuddySettings::on_pushButton_add_clicked()
{
    s_buddy newBuddy;
    m_buddyDialog = new buddydialog(m_cmdFacade,newBuddy,this);
    m_buddyDialog->exec();
    m_buddyDialog->setModal(true);
    // todo cmd facade add new buddy;
}
