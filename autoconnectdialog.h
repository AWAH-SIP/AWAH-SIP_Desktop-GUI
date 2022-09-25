#ifndef AUTOCONNECTDIALOG_H
#define AUTOCONNECTDIALOG_H

#include <QDialog>
#include "lib/types.h"
#include "cmdfacade.h"

namespace Ui {
class autoconnectdialog;
}

class autoconnectdialog : public QDialog
{
    Q_OBJECT

public:
    explicit autoconnectdialog(QWidget *parent = nullptr,CmdFacade *lib = nullptr, const s_account *account = nullptr);
    ~autoconnectdialog();

private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_pushButtonOk_clicked();

    void on_pushButtonCancel_clicked();

private:
    Ui::autoconnectdialog *ui;
    CmdFacade *m_cmdFacade;
    const s_account *m_Account;
    QList<s_buddy> m_buddies;
    QString buddyUID;
};

#endif // AUTOCONNECTDIALOG_H
