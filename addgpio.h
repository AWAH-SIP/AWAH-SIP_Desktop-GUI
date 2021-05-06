#ifndef ADDGPIO_H
#define ADDGPIO_H

#include <QDialog>
#include "lib/types.h"
#include "cmdfacade.h"


namespace Ui {
class addGPIO;
}

class addGPIO : public QDialog
{
    Q_OBJECT

public:
    explicit addGPIO(QWidget *parent = nullptr, CmdFacade *lib = nullptr);
    ~addGPIO();

private slots:

    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

private:
    Ui::addGPIO *ui;
    CmdFacade *m_cmdFacade;
};

#endif // ADDGPIO_H
