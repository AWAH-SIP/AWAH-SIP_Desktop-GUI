#include "logwindow.h"
#include "ui_logwindow.h"
#include "qscrollbar.h"

LogWindow::LogWindow(QWidget *parent, CmdFacade *lib) :
    QDialog(parent),
    ui(new Ui::LogWindow),
     m_cmdFacade(lib)
{
    ui->setupUi(this);
    QSettings settings("awah", "AWAHSipDesktopGUI");
    restoreGeometry(settings.value("LogWindow/Geometry").toByteArray());

    QStringList newlog = m_cmdFacade->readNewestLog();
    QString str = newlog.join("\n");
    ui->textEdit_log->setText(str);
    if(settings.value("LogWindow/autoscroll").toBool()){
        ui->checkBox_autoscroll->setChecked(1);
        QScrollBar *sb = ui->textEdit_log->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}

LogWindow::~LogWindow()
{
    delete ui;
}


void LogWindow::OnNewLogEntry(QString message)
{
    ui->textEdit_log->append(message);
    if(ui->checkBox_autoscroll->isChecked()){
        QScrollBar *sb = ui->textEdit_log->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}

void LogWindow::closeEvent(QCloseEvent* event)
{
    QSettings settings("awah", "AWAHSipDesktopGUI");
    settings.setValue("LogWindow/Geometry", saveGeometry());
    settings.setValue("LogWindow/autoscroll", ui->checkBox_autoscroll->isChecked());
    QDialog::closeEvent(event);
}

void LogWindow::on_checkBox_autoscroll_stateChanged(int arg1)
{
    if(arg1){
        QScrollBar *sb = ui->textEdit_log->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}
