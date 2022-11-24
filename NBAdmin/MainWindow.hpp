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
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include <vector>
#include <iostream>

//#include <windows.h>
//#include <Lmcons.h>

#include "TabWindow.hpp"
#include "ConnectWindow.hpp"

#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr) : QMainWindow(parent), ui(new Ui::MainWindow)
    {
        ui->setupUi(this);

        ui->tabWidget->insertTab(0,new TabWindow(this), QString("Query " + QString::number(ui->tabWidget->count())));

        filling_tree();

        ui->Run->setIcon(QIcon(":/images/RunBtnPic.svg"));

        connect(ui->Add, SIGNAL(clicked()), this, SLOT(push_button_plus_clicked()));
        //connect(pushButton1_, SIGNAL(clicked()), this, SLOT(push_button_minus_clicked()));
        connect(createConnect_, SIGNAL(triggered()), this, SLOT(create_connect_clicked()));
        connect(ui->Run, SIGNAL(clicked()), this, SLOT(push_button_run_clicked()));
     }
    ~MainWindow()
    {

    }

private slots:
    void push_button_run_clicked()
    {
        QString dbName = ui->treeWidget->currentItem()->text(0);

        // Кастыль
        TabWindow* currentTab = dynamic_cast<TabWindow*>(ui->tabWidget->currentWidget());

        currentTab->dbName = dbName;
        // currentTab->dbPort =
        if(dbName != "") currentTab->push_button_run_clicked();

        // Кастяль закончился
    }
    void push_button_plus_clicked()
    {
        ui->tabWidget->insertTab(ui->tabWidget->count(), new TabWindow(this), QString("Query " + QString::number(ui->tabWidget->count())));
    }
    void push_button_minus_clicked()
    {
        ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
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
            const QString ipAddress = "127.0.0.1";
            const QString pcPort = "8008";

            QString finalPath;
            finalPath += "http://";
            finalPath += ipAddress;
            finalPath += ":";
            finalPath += pcPort;
            finalPath += "/api3";

            QTreeWidgetItem* username = new QTreeWidgetItem();
            username->setText(0,"Local");
            ui->treeWidget->addTopLevelItem(username);


            QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
            const QUrl url(finalPath);
            QNetworkRequest request(url);
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            QJsonObject obj;
            obj["cmd"] = 7;
            QJsonDocument doc(obj);
            QByteArray data = doc.toJson();
            QNetworkReply *reply = mgr->post(request, data);
            connect(reply, &QNetworkReply::finished, [=]()
            {
                if(reply->error() == QNetworkReply::NoError)
                {
                    QJsonDocument copyReply = QJsonDocument::fromJson(reply->readAll());
                    QJsonObject Responce = copyReply.object();
                    QJsonArray tmpArray = Responce["list"].toArray();

                    for (auto item_db : tmpArray)
                    {
                        QTreeWidgetItem* dbName = new QTreeWidgetItem();
                        dbName->setText(0, item_db.toObject().find("dbname").value().toString());
                        username->addChild(dbName);

                        QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
                        const QUrl url(QStringLiteral("http://127.0.0.1:8008/api3"));
                        QNetworkRequest request(url);
                        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
                        QJsonObject obj;
                        obj["cmd"] = 8;
                        obj["port"] = item_db.toObject().find("port").value().toInt();
                        QJsonDocument doc(obj);
                        QByteArray data = doc.toJson();
                        QNetworkReply *reply_table = mgr->post(request, data);
                        connect(reply_table, &QNetworkReply::finished, [=]()
                        {
                            if(reply_table->error() == QNetworkReply::NoError)
                            {

                                QJsonDocument copy_reply_table = QJsonDocument::fromJson(reply_table->readAll());
                                QJsonObject Responce = copy_reply_table.object();

                                QJsonArray tmpArray = Responce["data"].toArray();
                                for (auto item_table : tmpArray)
                                {
                                    QTreeWidgetItem* table_name = new QTreeWidgetItem();
                                    table_name->setText(0,item_table.toObject().find("tablename").value().toString());
                                    dbName->addChild(table_name);
                                    QJsonArray fields_array = item_table.toObject().find("fields")->toArray();
                                    for (auto item_field : fields_array)
                                    {
                                        QTreeWidgetItem* field = new QTreeWidgetItem();
                                        field->setText(0, item_field.toObject().find("name")->toString());
                                        table_name->addChild(field);
                                    }

                                }
                            }

                            reply_table->deleteLater();
                        });
                    }
                }
                reply->deleteLater();
            });

        }

private:

    QAction* createConnect_ = nullptr;

    ConnectWindow* connectWindow_ = nullptr;

    std::map<QString, int> dbList_;



    Ui::MainWindow *ui;
};
