#ifndef BUDDYDIALOG_H
#define BUDDYDIALOG_H

#include <QDialog>
#include "cmdfacade.h"
#include "lib/types.h"
#include "codecsettings.h"

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

    void on_comboBox_codec_currentIndexChanged(const QString &arg1);

private:
    Ui::buddydialog *ui;
    CmdFacade* m_lib;
    s_buddy& m_buddy;
    QList<s_account>* m_accounts;
    CodecSettings* m_CodecSettings = nullptr;
    s_codec m_selectedCodec;

};

#endif // BUDDYDIALOG_H
