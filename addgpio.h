#ifndef ADDGPIO_H
#define ADDGPIO_H

#include <QDialog>
#include "lib/types.h"
#include "cmdfacade.h"
#include <QStandardItemModel>
#include "editorwidget.h"


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

    void on_comboBox_devTypes_currentIndexChanged(const QString &arg1);

private:
    Ui::addGPIO *ui;
    CmdFacade *m_cmdFacade;
    QJsonObject m_GpioDevTypes, m_GpioDev, m_editedParam;
    DeviceType m_devType;
    QStandardItemModel *m_GpioDevModel = nullptr;
    SettingsDelegate *m_GPIdelegate = nullptr;
};

#endif // ADDGPIO_H
