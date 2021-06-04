#ifndef BUDDYSETTINGS_H
#define BUDDYSETTINGS_H

#include <QDialog>
#include "cmdfacade.h"
#include "buddydialog.h"

namespace Ui {
class BuddySettings;
}

class BuddySettings : public QDialog
{
    Q_OBJECT

public:
    explicit BuddySettings(QWidget *parent = nullptr, CmdFacade *lib = nullptr);
    ~BuddySettings();

private slots:
    void on_pushButton_close_clicked();

    void on_pushButton_remove_clicked();

    void on_pushButton_edit_clicked();

    void on_pushButton_add_clicked();

private:
    Ui::BuddySettings *ui;
    CmdFacade* m_cmdFacade;
    buddydialog* m_buddyDialog;
    s_buddy m_buddyEdit;
};

#endif // BUDDYSETTINGS_H
