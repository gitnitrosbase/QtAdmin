#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>

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
//    ui->textEdit->append(paintText("sample teaxt with color", 0));
//    ui->textEdit->append(paintText("sample teaxt with color", 1));
//    ui->textEdit->append(paintText("sample teaxt with color", 2));

//    QFile file("output.txt");
//    file.open(QIODevice::ReadWrite);
//    if (file.isOpen())
//    {
//        file.write(ui->textEdit->toPlainText().toUtf8());
//    }
//    file.close();


    QString word;
    QString str = ui->textEdit->toPlainText();
    int j;
    for (int i = 0; i<str.size(); i+=1)
    {
        j = i;
        word = "";
        while (str.at(j) != " " && j < str.size()) word += str.at(j);


    }




}


void MainWindow::on_textEdit_textChanged()
{
//    QString word;
//    QString str = ui->textEdit->toPlainText();
//    int j;
//    for (int i = 0;i<ui->textEdit->toPlainText().size();i+=1)
//    {
//        word = "";
//        j = i;
//        //for (int j = i;j<ui->textEdit->toPlainText().size();j+=1)
//        while(str.at(j) != " " && j < ui->textEdit->toPlainText().size())
//        {
//            word+=str.at(j);
//            j+=1;
//        }
//        auto iterNull = alp.find("-1");
//        if (alp.find(word) != iterNull)
//        {
//            str.remove(i,j);
//            str.insert(i, paintText(word, 0));
//        }
//        i = j;
//    }
    //надо найти подстроку в строке textEdit, удалить оригинал и вставить html код
}

