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

#include "TabWindow.hpp"
#include "ConnectWindow.hpp"
#include "BackupWindow.hpp"
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
        connect(ui->Add, SIGNAL(clicked()), this, SLOT(push_button_plus_clicked()));
        connect(ui->Run, SIGNAL(clicked()), this, SLOT(push_button_run_clicked()));
        QStringList headers = {"Databases", "Type", "Nullable", "Link"};
        ui->treeWidget->setHeaderLabels(headers);
        connectWindow_ = new ConnectWindow();
        openWindow_ = new OpenWindow();
        backupWindow_ = new BackupWindow();
        ui->Add->setIcon(QIcon(":/images/AddTab.svg"));
        ui->Run->setIcon(QIcon(":/images/RunbtnPic.svg"));

        ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

        filling_tree();
     }
    ~MainWindow() = default;

private slots:
    void filling_tree_slot()
    {
        std::cout<<"check"<<std::endl;
        filling_tree();
    }

    void showContextMenu(const QPoint point)
    {
        QMenu * menu = new QMenu(this);

        QAction * refreshAction = new QAction(trUtf8("Refresh"), this);
        QAction * stopAction = new QAction(trUtf8("Stop"), this);
        QAction * startAction = new QAction(trUtf8("Start"), this);
        QAction * backupAction = new QAction(trUtf8("Backup"), this);
        QAction * restoreAction = new QAction(trUtf8("Restore"), this);
        QAction * deleteAction = new QAction(trUtf8("Delete"), this);
        QAction * databaseInfoAction = new QAction(trUtf8("Database info"), this);

        connect(refreshAction, SIGNAL(triggered()), this, SLOT(filling_tree_slot()));
        connect(stopAction, SIGNAL(triggered()), this, SLOT(on_actionStop_triggered()));
        connect(startAction, SIGNAL(triggered()), this, SLOT(on_actionStart_triggered()));
        connect(backupAction, SIGNAL(triggered()), this, SLOT(on_actionBackup_triggered()));
        //connect(restoreAction, SIGNAL(triggered()), this, SLOT(on_actionStop_triggered()));
        connect(deleteAction, SIGNAL(triggered()), this, SLOT(on_actionDelete_database_triggered()));
        connect(databaseInfoAction, SIGNAL(triggered()), this, SLOT(on_actionDatabase_Info_triggered()));

        menu->addAction(refreshAction);
        menu->addAction(stopAction);
        menu->addAction(startAction);
        menu->addAction(backupAction);
        menu->addAction(restoreAction);
        menu->addAction(deleteAction);
        menu->addAction(databaseInfoAction);

        menu->popup(ui->treeWidget->viewport()->mapToGlobal(point));
    }

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
    void on_actionDelete_database_triggered()
    {
        QString name = "";
        QString tmp = dbList_.find(currentDatabase_)->first;
        for (auto item : tmp) if (item != " ") name += item; else break;

        QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
        const QUrl url(address_);
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QJsonObject obj;
        obj["cmd"] = 10;
        obj["port"] = dbList_.find(currentDatabase_)->second;
        obj["dbname"] = name;
        obj["dbpath"] = "";
        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();
        QNetworkReply *reply = mgr->post(request, data);
        connect(reply, &QNetworkReply::finished, [=]()
        {
            if (reply->error() == QNetworkReply::NoError)
            {
                QJsonDocument reply_doc = QJsonDocument::fromJson(reply->readAll());
                QFile file("answer.txt");
                file.open(QIODevice::ReadWrite);
                file.write(reply_doc.toJson());
                file.close();
            }
            reply->deleteLater();
        });
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
        QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
        const QUrl url(address_);
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QJsonObject obj;
        obj["cmd"] = 4;
        obj["port"] = dbList_.find(currentDatabase_)->second;
        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();
        QNetworkReply *reply = mgr->post(request, data);
        connect(reply, &QNetworkReply::finished, [=]()
        {
            if (reply->error() == QNetworkReply::NoError)
            {
                QJsonDocument reply_doc = QJsonDocument::fromJson(reply->readAll());

                QFile file("answerstartstop.txt");
                file.open(QIODevice::ReadWrite);
                file.write(reply_doc.toJson());
                file.close();

            }
            reply->deleteLater();
        });

        filling_tree();

    }
    void on_actionStart_triggered()
    {
        QString name = "";
        QString tmp = dbList_.find(currentDatabase_)->first;
        for (auto item : tmp) if (item != " ") name += item; else break;

        QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
        const QUrl url(address_);
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QJsonObject obj;
        obj["cmd"] = 1;
        obj["port"] = dbList_.find(currentDatabase_)->second;
        obj["dbname"] = name;
        obj["dbpath"] = "";
        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();
        QNetworkReply *reply = mgr->post(request, data);
        connect(reply, &QNetworkReply::finished, [=]()
        {
            if (reply->error() == QNetworkReply::NoError)
            {
                QJsonDocument reply_doc = QJsonDocument::fromJson(reply->readAll());

                QFile file("answerstartstop.txt");
                file.open(QIODevice::ReadWrite);
                file.write(reply_doc.toJson());
                file.close();
            }
            reply->deleteLater();
        });

        filling_tree();
    }
    void on_actionBackup_triggered()
    {
        backupWindow_->show();
        backupWindow_->setWindowTitle("Backup");
        backupWindow_->address_ = this->address_;
    }
    void on_actionRestore_triggered()
    {
        // создаем бд из бекапа (пример json лежит в skype)
//        QString name = "";
//        QString tmp = dbList_.find(currentDatabase_)->first;
//        for (auto item : tmp) if (item != " ") name += item; else break;

//        QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
//        const QUrl url(address_);
//        QNetworkRequest request(url);
//        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
//        QJsonObject obj;
//        obj["cmd"] = 10;
//        obj["port"] = dbList_.find(currentDatabase_)->second;
//        obj["dbname"] = name;
//        obj["dbpath"] = "";
//        QJsonDocument doc(obj);
//        QByteArray data = doc.toJson();
//        QNetworkReply *reply = mgr->post(request, data);
//        connect(reply, &QNetworkReply::finished, [=]()
//        {
//            if (reply->error() == QNetworkReply::NoError)
//            {
//                QJsonDocument reply_doc = QJsonDocument::fromJson(reply->readAll());
//                QFile file("answer.txt");
//                file.open(QIODevice::ReadWrite);
//                file.write(reply_doc.toJson());
//                file.close();
//            }
//            reply->deleteLater();
//        });
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

    void on_actionOpen_triggered()
    {
        TabWindow* currentTab = dynamic_cast<TabWindow*>(ui->tabWidget->currentWidget());

        QString path = "C:/Users/nk/Documents/query.txt";
        QFile file(path);
        file.open(QIODevice::ReadWrite);
        if(file.isOpen())
        {
            QString tmp = file.readAll();
            currentTab->setText(tmp);
        }
        else
        {
            QMessageBox::warning(this, "Warning", "Could not open the file");
        }
        file.close();
    }

    void on_actionSave_triggered()
    {
        TabWindow* currentTab = dynamic_cast<TabWindow*>(ui->tabWidget->currentWidget());

        QString path = "C:/Users/nk/Documents/query_save.txt";
        QFile file(path);
        file.open(QIODevice::ReadWrite);
        if(file.isOpen())
        {
            file.write(currentTab->textFromTextEdit().toUtf8());
        }
        else
        {
            QMessageBox::warning(this, "Warning", "Could not open the file");
        }
        file.close();
    }

private:

    void setAddress()
    {
        QFile file(":/config.conf");
        file.open(QIODevice::ReadOnly);
        if (file.isOpen())
        {
            QJsonDocument jsonDoc = QJsonDocument::fromBinaryData(file.readAll());
            QJsonObject jsonObject = jsonDoc.object();
            QString address = "http://";
            address+=jsonDoc["ip"].toString();
            address+=":";
            address+=jsonDoc["port"].toString();
            address+="/api3";
            address_ = address;
        }
        file.close();
    }

    QString paintText(QString str, int color)
    {
        QString output;
        switch (color)
        {
            case 0 : output = QString("<span style=\" color:#ff0000;\">%1</span>").arg(str); break;
            case 1 : output = QString("<span style=\" color:#008000;\">%1</span>").arg(str); break;
            case 2 : output = QString("<span style=\" color:#0000FF;\">%1</span>").arg(str); break;
        }
        return output;
    }

    QString nullCheck(int index)
    {
        if (index == 0) return "NOT NULL";
        else if (index == 1) return "NULL";
        else return "???";
    }

    QString linkCheck(QString input)
    {
        //if (input != "") return QString(" -> " + input);
        return input;
    }

    QString runCheck(bool input)
    {
        if (input) return "RUN";
        else return "STOPED";
    }


    void filling_tree()
    {
        ui->treeWidget->takeTopLevelItem(0);
        ui->treeWidget->setColumnCount(4);
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
                    dbName->setText(0, QString(item_db.toObject().find("dbname").value().toString()
                                               + " "
                                               + QString::number(item_db.toObject().find("port").value().toInt())
                                               ));
                    username->addChild(dbName);

                    if (item_db.toObject().find("run").value().toBool() == true) dbName->setIcon(0, QIcon(":/images/true.png"));
                    else if (item_db.toObject().find("run").value().toBool() == false) dbName->setIcon(0, QIcon(":/images/false.png"));

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
                                        field->setText(0, QString(item_field.toObject().find("name")->toString()
                                                                  //+ " ["
                                                                  //+ QString::fromStdString(fieldsTypes_.at(item_field.toObject().find("name")->toInt()))
                                                                  //+ " "
                                                                  //+ nullCheck(item_field.toObject().find("nullable")->toInt())
                                                                  //+ "]"
                                                                  //+ linkCheck(item_field.toObject().find("linktable")->toString())
                                                                  ));
                                        field->setText(1, QString::fromStdString(fieldsTypes_.at(item_field.toObject().find("type")->toInt())));
                                        field->setText(2, nullCheck(item_field.toObject().find("nullable")->toInt()));
                                        field->setText(3, linkCheck(item_field.toObject().find("linktable")->toString()));
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
                                        field->setText(0, QString(item_field.toObject().find("name")->toString()
                                                                  //+ " ["
                                                                  //+ QString::fromStdString(fieldsTypes_.at(item_field.toObject().find("name")->toInt()))
                                                                  //+ "]"
                                                                  //+ linkCheck(item_field.toObject().find("linktable")->toString())
                                                                  ));
                                        field->setText(1, QString::fromStdString(fieldsTypes_.at(item_field.toObject().find("type")->toInt())));
                                        field->setText(2, nullCheck(item_field.toObject().find("nullable")->toInt()));
                                        field->setText(3, linkCheck(item_field.toObject().find("linktable")->toString()));
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
    QMenu* rightClickMenu_ = nullptr;
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
