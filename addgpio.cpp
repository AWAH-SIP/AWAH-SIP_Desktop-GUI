#include "addgpio.h"
#include "ui_addgpio.h"
#include <qmessagebox.h>


addGPIO::addGPIO(QWidget *parent, CmdFacade *lib) :
    QDialog(parent),
    ui(new Ui::addGPIO),
    m_cmdFacade(lib)
{
    ui->setupUi(this);
    QSettings settings("awah", "AWAHSipDesktopGUI");
    restoreGeometry(settings.value("GeneralSettings/Geometry").toByteArray());
    m_GpioDevTypes = m_cmdFacade->getGpioDevTypes();
    QJsonObject::iterator j;
    for (j = m_GpioDevTypes.begin(); j != m_GpioDevTypes.end(); ++j) {
        ui->comboBox_devTypes->addItem(j.key());
    }
}

addGPIO::~addGPIO()
{
    delete ui;
}
void addGPIO::on_pushButton_ok_clicked()
{                              
                                // get default param from the GpioDevTypes in case not every parameter is edited
    QString devName = m_GpioDev["Name"].toObject()["value"].toString();
    uint inCount = m_GpioDev["Inputs"].toObject()["value"].toInt();
    uint outCount = m_GpioDev["Outputs"].toObject()["value"].toInt();

                                // replace default param with de edited ones
    devName = m_editedParam["Name"].toString();
    inCount = m_editedParam["Inputs"].toInt();
    outCount = m_editedParam["Outputs"].toInt();

    if(devName != ""){
        m_cmdFacade->createGpioDev(m_devType,inCount,outCount,devName);
        addGPIO::close();
    }
    else{
       QMessageBox::information(this,"empty Name","Please enter a Name for the device");
    }


}

void addGPIO::on_pushButton_cancel_clicked()
{
    addGPIO::close();
}


void addGPIO::on_comboBox_devTypes_currentIndexChanged(const QString &arg1)
{
    m_editedParam = QJsonObject();
    m_GpioDev = m_GpioDevTypes.value(arg1)["parameter"].toObject();
    m_devType = (DeviceType)m_GpioDevTypes.value(arg1)["devType"].toInt();

    if(m_GpioDevModel !=nullptr){
        delete m_GpioDevModel;
    }

    m_GpioDevModel = new QStandardItemModel(m_GpioDev.size(), 2, this);
    ui->tableView->setModel(m_GpioDevModel);

    if(m_GPIdelegate !=nullptr){
        delete m_GPIdelegate;
    }
    m_GPIdelegate = new SettingsDelegate(&m_GpioDev, &m_editedParam, ui->tableView);
    ui->tableView->setItemDelegate(m_GPIdelegate);
    QJsonObject::iterator j;
    QModelIndex index;
    int row = 0;
    for (j = m_GpioDev.begin(); j != m_GpioDev.end(); ++j) {
            index = m_GpioDevModel->index(row, 0, QModelIndex());
            m_GpioDevModel->setData(index,j.key());
            index = m_GpioDevModel->index(row, 1, QModelIndex());
            m_GpioDevModel->setData(index,j.value().toObject());
            ui->tableView->openPersistentEditor(index);
            row++;
    }

    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->resizeRowsToContents();
    ui->tableView->show();
}
