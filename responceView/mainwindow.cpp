#include "mainwindow.h"
#include "./ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    std::cout<<"start"<<std::endl;
    model_ = new ResponceView();
    std::cout<<"create"<<std::endl;
    ui->tableView->setModel(model_);
    std::cout<<"create1"<<std::endl;
}

MainWindow::~MainWindow()
{
    delete model_;
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    clock_t start = clock();
    QList<QString> tmp = {"1", "2", "3", "4", "5", "6"};

    model_->insertColumns(0,6);

    for (int i = 0; i < ui->lineEdit_3->text().toInt(); i+=1) model_->addRow(tmp);
    clock_t end = clock();

    double seconds = (double)(end - start);
    std::cout<< seconds <<std::endl;
}

