#include "addgpio.h"
#include "ui_addgpio.h"

addGPIO::addGPIO(QWidget *parent, CmdFacade *lib) :
    QDialog(parent),
    ui(new Ui::addGPIO),
    m_cmdFacade(lib)
{
    ui->setupUi(this);
    ui->comboBox_6->addItem("Web Socket GPIO");
    ui->comboBox_Direction->addItem("input");
    ui->comboBox_Direction->addItem("output");
    ui->comboBox_Direction->addItem("input/output");
    ui->comboBox_Type->addItem("normal");
    ui->comboBox_Type->addItem("inverted");
}

addGPIO::~addGPIO()
{
    delete ui;
}
void addGPIO::on_pushButton_ok_clicked()
{
    addGPIO::close();
}

void addGPIO::on_pushButton_cancel_clicked()
{
    addGPIO::close();
}
