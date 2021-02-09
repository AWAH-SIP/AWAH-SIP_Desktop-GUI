#include "logwindow.h"
#include "ui_logwindow.h"
#include "qscrollbar.h"

LogWindow::LogWindow(QWidget *parent, CmdFacade *lib) :
    QDialog(parent),
    ui(new Ui::LogWindow),
     m_cmdFacade(lib)
{
    ui->setupUi(this);
    QStringList newlog = m_cmdFacade->readNewestLog();
    QString str = newlog.join("\n");
    ui->textEdit_log->setText(str);
    ui->checkBox_autoscroll->setChecked(1);
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

