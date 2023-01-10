#include "ModifyTableTab.hpp"

ModifyTableTab::ModifyTableTab(QWidget *parent)
    : QWidget{parent}, ui(new Ui::ModifyTableTab)
{
    ui->setupUi(this);
}

ModifyTableTab::~ModifyTableTab()
{
    delete ui;
}
