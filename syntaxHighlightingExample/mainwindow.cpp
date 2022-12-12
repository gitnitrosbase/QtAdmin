#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString input = ui->input->toPlainText();

    if (input.at(input.size()-1) != " ") input+=" ";

    QString word;
    QString words;
        int j;
        for (int i = 0;i<input.size();i+=1)
        {
            word = "";
            j = i;
            while(input.at(j) != " ")
            {
                word+=input.at(j);
                j+=1;
            }
            auto nullIter = alp.find("-1");
            if (alp.find(word) != nullIter)
            {
                words+=paintText(word, alp.find(word)->second);
                words+="\n";
            }
            i = j;
        }

    ui->output->setText(words);
}
