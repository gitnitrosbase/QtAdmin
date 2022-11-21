#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QString>
#include <QPushButton>
#include <QTextBrowser>
#include <QGridLayout>
#include <QTextEdit>
#include <QTreeWidget>
#include <QMessageBox>
#include <QDir>
#include <QDirIterator>
#include <QToolBar>
#include <QMainWindow>

#include <vector>
#include <iostream>

#include <windows.h>
#include <Lmcons.h>

#include "TabWindow.hpp"
#include "ConnectWindow.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr) : QMainWindow(parent)
    {
        gridLayout_ = new QGridLayout(this);
        tabWidget_ = new QTabWidget(this);
        pushButton_ = new QPushButton("+", this);
        pushButton1_ = new QPushButton("-", this);
        treeWidget_ = new QTreeWidget(this);
        toolBar_ = new QToolBar(this);
        createConnect_ = new QAction("Connect", this);


        QWidget* window = new QWidget();
        window->setLayout(gridLayout_);
        setCentralWidget(window);

        addToolBar(toolBar_);
        toolBar_->addAction(createConnect_);

        gridLayout_->addWidget(treeWidget_, 0, 0, 0 ,1);
        gridLayout_->addWidget(pushButton_, 0, 1);
        gridLayout_->addWidget(pushButton1_, 0, 2);
        gridLayout_->addWidget(tabWidget_, 1, 1, 2, 2);

        treeWidget_->setMaximumWidth(200);
        tabWidget_->insertTab(0,new TabWindow(this), "Query 1");

        filling_tree();



        connect(pushButton_, SIGNAL(clicked()), this, SLOT(push_button_plus_clicked()));
        connect(pushButton1_, SIGNAL(clicked()), this, SLOT(push_button_minus_clicked()));
        connect(createConnect_, SIGNAL(triggered()), this, SLOT(create_connect_clicked()));
     }
    ~MainWindow() = default;

private slots:
    void push_button_plus_clicked()
    {
        queryCount+=1;
        tabWidget_->insertTab(queryCount, new TabWindow(this), QString("Query " + QString::number(queryCount)));
        tabWidget_->setCurrentIndex(queryCount-1);
    }
    void push_button_minus_clicked()
    {
        if (queryCount >= 0)
        {
            queryCount-=1;
            tabWidget_->removeTab(tabWidget_->currentIndex());
        }
    }

    void create_connect_clicked()
    {
        std::cout<<"check"<<std::endl;
        connectWindow_ = new ConnectWindow();
        connectWindow_->show();
    }

private:
    void filling_tree()
    {

        QString name = qgetenv("USER");
        if (name.isEmpty()) name = qgetenv("USERNAME");
        QTreeWidgetItem* username = new QTreeWidgetItem();
        username->setText(0,name);
        treeWidget_->addTopLevelItem(username);

        name = QString("C:/Users/" + name + "/AppData/Local/nitrosbaseuni/data");
        QFileInfoList list = QDir(name).entryInfoList();
        for (int i = 0; i < list.size(); i += 1)
        {
            if (list.at(i).fileName() != "." && list.at(i).fileName() != ".." && list.at(i).fileName() != ".")
            {
                QTreeWidgetItem* dbs_name = new QTreeWidgetItem();
                dbs_name->setText(0,list.at(i).fileName());
                username->addChild(dbs_name);
            }
        }
    }

private:
    QTabWidget* tabWidget_ = nullptr;
    QGridLayout* gridLayout_ = nullptr;
    QPushButton* pushButton_ = nullptr;
    QPushButton* pushButton1_ = nullptr;
    QTreeWidget* treeWidget_ = nullptr;
    QToolBar* toolBar_ = nullptr;

    QAction* createConnect_ = nullptr;

    ConnectWindow* connectWindow_ = nullptr;

    int queryCount = 1;
};
