#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include "AWAHSipDesktopGUI.h"

namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = nullptr);
    ~ConnectDialog();

private slots:
    void on_pushButton_connect_clicked();

    void on_pushButton_close_clicked();

    void on_Connected();

    void on_Disconnected();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_spinBox_valueChanged(int arg1);

private:
    Ui::ConnectDialog *ui;
    AWAHSipDesktopGUI *DesktopGui;
    WebsocketClient *m_websocketClient;
    QUrl wsUrl;
};

#endif // CONNECTDIALOG_H
