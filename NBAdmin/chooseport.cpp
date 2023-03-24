#include "chooseport.h"
#include "ui_chooseport.h"

ChoosePort::ChoosePort(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChoosePort)
{
    ui->setupUi(this);
}

ChoosePort::~ChoosePort()
{
    delete ui;
}
