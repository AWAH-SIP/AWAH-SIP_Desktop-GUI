#ifndef BUDDYDIALOG_H
#define BUDDYDIALOG_H

#include <QDialog>
#include "cmdfacade.h"
#include "lib/types.h"

namespace Ui {
class buddydialog;
}

class buddydialog : public QDialog
{
    Q_OBJECT

public:
    explicit buddydialog(CmdFacade *lib, s_buddy &buddy, QWidget *parent = nullptr);
    ~buddydialog();

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_ok_clicked();

    void on_pushButton_settings_clicked();

private:
    Ui::buddydialog *ui;
    CmdFacade* m_lib;
    s_buddy& m_buddy;
    QList<s_account>* m_accounts;

};

#endif // BUDDYDIALOG_H
