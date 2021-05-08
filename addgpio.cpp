#include "addgpio.h"
#include "ui_addgpio.h"

addGPIO::addGPIO(QWidget *parent, CmdFacade *lib) :
    QDialog(parent),
    ui(new Ui::addGPIO),
    m_cmdFacade(lib)
{
    ui->setupUi(this);
    ui->comboBox_6->addItem("Virtual GPIO");
    ui->comboBox_6->addItem("And Gate");
    ui->comboBox_6->addItem("OR Gate");
    ui->spinBox_InputsVirutal->setMinimum(0);
    ui->spinBox_InputsVirutal->setMaximum(16);
    ui->spinBox_OutpustVirtual->setMinimum(0);
    ui->spinBox_OutpustVirtual->setMaximum(16);
    ui->spinBox_InputOr->setMinimum(1);
    ui->spinBox_InputOr->setMaximum(8);
    ui->spinBox_InputsAnd->setMinimum(1);
    ui->spinBox_InputsAnd->setMaximum(8);

}

addGPIO::~addGPIO()
{
    delete ui;
}
void addGPIO::on_pushButton_ok_clicked()
{
    switch (ui->stackedWidget->currentIndex()) {
        case 0:
            qDebug() << "Add Virtual GPIO: " << ui->lineEdit_nameVirutal->text() << "inputs: " << ui->spinBox_InputsVirutal->value() << "outputs: " << ui->spinBox_OutpustVirtual->value();
            break;
        case 1:
            qDebug() << "Add Virtual AND Gate: " << ui->lineEdit_NameAnd->text() << "inputs: " << ui->spinBox_InputsAnd->value();
            break;
        case 2:
            qDebug() << "Add Virtual Or Gate: " << ui->lineEdit_NameOr->text() << "inputs: " << ui->spinBox_InputOr->value();
            break;
    }
    addGPIO::close();
}

void addGPIO::on_pushButton_cancel_clicked()
{
    addGPIO::close();
}

void addGPIO::on_comboBox_6_currentIndexChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}
