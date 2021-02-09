#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QDialog>
#include "lib/types.h"
#include "cmdfacade.h"

namespace Ui {
class LogWindow;
}

class LogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LogWindow(QWidget *parent = nullptr, CmdFacade *lib = nullptr);
    ~LogWindow();

public slots:
    void OnNewLogEntry(QString message);

private:
    Ui::LogWindow *ui;
    CmdFacade *m_cmdFacade;
};

#endif // LOGWINDOW_H
