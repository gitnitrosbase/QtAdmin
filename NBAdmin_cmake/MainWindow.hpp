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
#include <utility>
#include <algorithm>

#include <windows.h>

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
        push_button_plus_clicked();
        filling_tree();
        connect(ui->Add, SIGNAL(clicked()), this, SLOT(push_button_plus_clicked()));
        connect(ui->Run, SIGNAL(clicked()), this, SLOT(push_button_run_clicked()));
        ui->treeWidget->setHeaderLabel("Databases");
        connectWindow_ = new ConnectWindow();
     }
    ~MainWindow() = default;

private slots:
    void push_button_run_clicked()
    {
        if (ui->tabWidget->count() > 0 && currentDatabase_ != "")
        {
            TabWindow* currentTab = dynamic_cast<TabWindow*>(ui->tabWidget->currentWidget());
            currentTab->dbPort_ = dbList_[currentDatabase_];
            currentTab->push_button_run_clicked();
            std::cout<<"run query"<<std::endl;
            //filling_tree();
        }

    }
    void push_button_plus_clicked()
    {
        ui->tabWidget->insertTab(ui->tabWidget->count(), new TabWindow(this), QString("Query " + QString::number(ui->tabWidget->count()+1)));
    }
    void push_button_minus_clicked()
    {
        ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
    }

    void on_tabWidget_tabCloseRequested(int index)
    {
        ui->tabWidget->removeTab(index);
    }

    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem*)
    {
        QString dbName = current->text(0);
        if(dbList_.count(dbName) > 0 && dbList_.find(dbName) != dbList_.find("-1"))
        {
            ui->label_2->setText(dbName);
            currentDatabase_ = dbName;
        }

    }

    void on_actionCreate_database_triggered()
    {
        connectWindow_->show();
    }
    void on_actionOpen_database_triggered()
    {
        std::cout<<"open database"<<std::endl;
    }
    void on_actionRefresh_triggered()
    {
        filling_tree();
    }

private:
    void filling_tree()
    {
        ui->treeWidget->takeTopLevelItem(0);

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

                dbList_.clear();
                for (auto item_db : tmpArray)
                {
                    QTreeWidgetItem* dbName = new QTreeWidgetItem();
                    dbName->setText(0, item_db.toObject().find("dbname").value().toString());
                    username->addChild(dbName);
                    dbList_.insert(std::pair<QString, int>(item_db.toObject().find("dbname").value().toString(), item_db.toObject().find("port").value().toInt()));

                    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
                    QUrl url(finalPath);
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
//                          QMessageBox::information(this, "", copy_reply_table.toJson());
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
    std::map<QString, int> dbList_;
    Ui::MainWindow *ui;
    QString currentDatabase_ = "";

    ConnectWindow* connectWindow_ = nullptr;

};
