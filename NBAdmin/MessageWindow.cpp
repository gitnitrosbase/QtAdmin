#include "MessageWindow.hpp"
#include "ui_MessageWindow.h"

MessageWindow::MessageWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageWindow)
{
    ui->setupUi(this);
}

MessageWindow::~MessageWindow()
{
    delete ui;
}

void MessageWindow::setText(QString str)
{
    ui->text->setText(str);
}
