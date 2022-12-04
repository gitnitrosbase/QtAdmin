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
#include <QFile>
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
#include "backupWindow.hpp"
#include "OpenWindow.hpp"
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
        openWindow_ = new OpenWindow();
        backupWindow_ = new BackupWindow();
        ui->Add->setIcon(QIcon(":/images/AddTab.svg"));
        ui->Run->setIcon(QIcon(":/images/RunbtnPic.svg"));
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
        }
    }
    void push_button_plus_clicked()
    {
        ui->tabWidget->insertTab(ui->tabWidget->count(), new TabWindow(this), QString("Query " + QString::number(ui->tabWidget->count()+1)));
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
        connectWindow_->setWindowTitle("Create database");
        connectWindow_->address_ = this->address_;
    }
    void on_actionOpen_database_triggered()
    {
        openWindow_->show();
        openWindow_->setWindowTitle("Open database");
        openWindow_->address_ = this->address_;
    }
    void on_actionRefresh_triggered()
    {
        filling_tree();
    }
    void on_actionStop_triggered()
    {
        QMessageBox::warning(this, "Title", "Dont work");
    }
    void on_actionStart_triggered()
    {
        QMessageBox::warning(this, "Title", "Dont work");
    }
    void on_actionBackup_triggered()
    {
        backupWindow_->show();
        backupWindow_->setWindowTitle("Backup");
        backupWindow_->address_ = this->address_;
    }
    void on_actionRestore_triggered()
    {
        QMessageBox::warning(this, "Title", "Dont work");
    }
    void on_actionDatabase_Info_triggered()
    {
        if (currentDatabase_ != "")
        {
            QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
            const QUrl url(address_);
            QNetworkRequest request(url);
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            QJsonObject obj;
            obj["cmd"] = 8;
            obj["port"] = dbList_.find(currentDatabase_)->second;
            QJsonDocument doc(obj);
            QByteArray data = doc.toJson();
            QNetworkReply *reply = mgr->post(request, data);
            connect(reply, &QNetworkReply::finished, [=]()
            {
                if (reply->error() == QNetworkReply::NoError)
                {
                    QJsonDocument reply_doc = QJsonDocument::fromJson(reply->readAll());
                    QString output;
                    output+="Name: ";
                    output+=currentDatabase_;
                    output+="\nPort: ";
                    int port = dbList_[currentDatabase_];
                    output+=QString::number(port);
                    output+="\nVersion Database: ";
                    QJsonObject reply_obj = reply_doc.object();
                    QString str = reply_obj.find("version").value().toString();
                    output+=str;
                    QMessageBox::information(this, "Database info", output);
                }
                reply->deleteLater();
            });
        }
        else QMessageBox::warning(this, "Warning", "Select database");
    }
    void on_actionInfo_triggered()
    {
        on_actionDatabase_Info_triggered();
    }
    void on_actionContacts_triggered()
    {
        QMessageBox::information(this,"Contacts", "Email: support@nitrosbase.com");
    }

    void on_actionNew_query_triggered()
    {
        push_button_plus_clicked();
    }

    void on_actionClose_query_triggered()
    {
        ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
    }

    void on_actionRun_query_triggered()
    {
        push_button_run_clicked();
    }

private:
    void filling_tree()
    {
        ui->treeWidget->takeTopLevelItem(0);
        QTreeWidgetItem* username = new QTreeWidgetItem();
        username->setText(0,"Local");
        ui->treeWidget->addTopLevelItem(username);
        QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
        const QUrl url(address_);
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

                int i = 0;

                for (auto item_db : tmpArray)
                {
                    QTreeWidgetItem* dbName = new QTreeWidgetItem();
                    dbName->setText(0, QString(item_db.toObject().find("dbname").value().toString() + " " + QString::number(item_db.toObject().find("port").value().toInt())));
                    username->addChild(dbName);
                    dbList_.insert(std::pair<QString, int>(QString(item_db.toObject().find("dbname").value().toString() + " " + QString::number(item_db.toObject().find("port").value().toInt())), item_db.toObject().find("port").value().toInt()));
                    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
                    QUrl url(address_);
                    QNetworkRequest request(url);
                    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
                    QJsonObject obj;
                    obj["cmd"] = 8;
                    obj["port"] = item_db.toObject().find("port").value().toInt();
                    QJsonDocument doc(obj);
                    QByteArray data = doc.toJson();
                    QNetworkReply *reply_table = mgr->post(request, data);

                    i+=1;

                    connect(reply_table, &QNetworkReply::finished, [=]()
                    {
                        if(reply_table->error() == QNetworkReply::NoError)
                        {
                            QJsonDocument copy_reply_table = QJsonDocument::fromJson(reply_table->readAll());
                            QJsonObject Responce = copy_reply_table.object();
                            QJsonArray tmpArray = Responce["data"].toArray();

                            QFile filename(QString("file" + QString::number(i) + ".txt"));
                            filename.open(QIODevice::ReadWrite);
                            filename.write(copy_reply_table.toJson());
                            filename.close();

                            QTreeWidgetItem* dbTables = new QTreeWidgetItem();
                            dbName->addChild(dbTables);

                            QTreeWidgetItem* dbEdges = new QTreeWidgetItem();
                            dbName->addChild(dbEdges);

                            int tablesCount = 0;
                            int edgesCount = 0;

                            for (auto item_table : tmpArray)
                            {
                                if (item_table.toObject().find("type").value().toInt() == 2)
                                {
                                    QTreeWidgetItem* table_name = new QTreeWidgetItem();
                                    table_name->setText(0,item_table.toObject().find("tablename").value().toString());
                                    dbTables->addChild(table_name);
                                    QJsonArray fields_array = item_table.toObject().find("fields")->toArray();
                                    for (auto item_field : fields_array)
                                    {
                                        QTreeWidgetItem* field = new QTreeWidgetItem();
                                        field->setText(0, QString(item_field.toObject().find("name")->toString() + " [" + QString::fromStdString(fieldsTypes_.at(item_field.toObject().find("name")->toInt())) + "]"));
                                        table_name->addChild(field);
                                    }

                                    tablesCount+=1;
                                }
                                else if (item_table.toObject().find("type").value().toInt() == 3)
                                {
                                    QTreeWidgetItem* table_name = new QTreeWidgetItem();
                                    table_name->setText(0,item_table.toObject().find("tablename").value().toString());
                                    dbEdges->addChild(table_name);
                                    QJsonArray fields_array = item_table.toObject().find("fields")->toArray();
                                    for (auto item_field : fields_array)
                                    {
                                        QTreeWidgetItem* field = new QTreeWidgetItem();
                                        field->setText(0, QString(item_field.toObject().find("name")->toString()));
                                        table_name->addChild(field);
                                    }

                                    edgesCount+=1;
                                }
                            }
                            dbTables->setText(0, QString("Tables (" + QString::number(tablesCount) + ")"));
                            dbEdges->setText(0, QString("Edges (" + QString::number(edgesCount) + ")"));
                        }
                        reply_table->deleteLater();
                    });
                }
            }
            reply->deleteLater();
        });
    }

private:
    QString address_ = "http://127.0.0.1:8008/api3";
    std::map<QString, int> dbList_;
    Ui::MainWindow *ui;
    QString currentDatabase_ = "";
    ConnectWindow* connectWindow_ = nullptr;
    OpenWindow* openWindow_ = nullptr;
    BackupWindow* backupWindow_ = nullptr;
    std::vector<std::string> fieldsTypes_ = {
        "varchar",
        "int",
        "bigint",
        "double",
        "datetime",
        "bit",
        "date",
        "varbinary",
        "nvarchar",
        "rowversion",
        "decimal"
    };
};
