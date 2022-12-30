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
#include <QList>

#include <QRegularExpression>
#include <QSyntaxHighlighter>

#include <vector>
#include <iostream>
#include <utility>
#include <algorithm>
#include <map>

#include "TabWindow.hpp"
#include "ConnectWindow.hpp"
#include "BackupWindow.hpp"
#include "OpenWindow.hpp"
#include "RestoreWindow.hpp"
#include "CreateTableTab.hpp"
#include "CreateEdgeTab.hpp"
#include "CreateIndexTab.hpp"
#include "ui_mainwindow.h"

#include <windows.h>

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
        QStringList headers = {"Databases"};
        ui->treeWidget->setHeaderLabels(headers);
        connectWindow_ = new ConnectWindow();
        openWindow_ = new OpenWindow();
        backupWindow_ = new BackupWindow();
        restoreWindow_ = new RestoreWindow();
        ui->Add->setIcon(QIcon(":/images/AddTab.svg"));
        ui->Run->setIcon(QIcon(":/images/RunbtnPic.svg"));

        ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

        filling_tree();
        menu_ = new QMenu(this);
        refreshAction_ = new QAction(trUtf8("Refresh"), this);
        stopAction_ = new QAction(trUtf8("Stop"), this);
        startAction_ = new QAction(trUtf8("Start"), this);
        backupAction_ = new QAction(trUtf8("Backup"), this);
        restoreAction_ = new QAction(trUtf8("Restore"), this);
        deleteAction_ = new QAction(trUtf8("Delete"), this);
        databaseInfoAction_ = new QAction(trUtf8("Database info"), this);
        createTableAction_ = new QAction(trUtf8("Create table"), this);
        createEdgeAction_ = new QAction(trUtf8("Create edge"), this);
        createIndexAction_ = new QAction(trUtf8("Create index"), this);

        selectAction_ = new QAction(trUtf8("Select * top 1000"), this);
        modifyStructAction_ = new QAction(trUtf8("Modify structure"), this);
        deleteTableAction_ = new QAction(trUtf8("Delete table"), this);
     }
    ~MainWindow() = default;

private slots:

    void filling_tree_slot()
    {
        filling_tree();
    }

    void showContextMenu(const QPoint point)
    {
        menu_->clear();



        connect(refreshAction_, SIGNAL(triggered()), this, SLOT(filling_tree_slot()));
        connect(stopAction_, SIGNAL(triggered()), this, SLOT(on_actionStop_triggered()));
        connect(startAction_, SIGNAL(triggered()), this, SLOT(on_actionStart_triggered()));
        connect(backupAction_, SIGNAL(triggered()), this, SLOT(on_actionBackup_triggered()));
        connect(restoreAction_, SIGNAL(triggered()), this, SLOT(on_actionRestore_triggered()));
        connect(deleteAction_, SIGNAL(triggered()), this, SLOT(on_actionDelete_database_triggered()));
        connect(databaseInfoAction_, SIGNAL(triggered()), this, SLOT(on_actionDatabase_Info_triggered()));
        connect(createTableAction_, SIGNAL(triggered()), this, SLOT(on_actionCreateTable_triggeted()));
        connect(createEdgeAction_, SIGNAL(triggered()), this, SLOT(on_actionCreateEdge_triggered()));
        connect(createIndexAction_, SIGNAL(triggered()), this, SLOT(on_actionCreateIndex_triggered()));
        connect(selectAction_, SIGNAL(triggered()), this, SLOT(on_selectAction_triggered()));
        connect(modifyStructAction_, SIGNAL(triggered()), this, SLOT(on_modifyStructAction_triggered()));
        connect(deleteTableAction_, SIGNAL(triggered()), this, SLOT(on_deleteTableAction_triggered()));

        QString currentItem = ui->treeWidget->currentItem()->text(0);

        QString currentItem1 = "";

        for (int i = 0; i<currentItem.count(); i+=1)
        {
            if (currentItem[i] != " ") currentItem1 +=currentItem[i];
            else break;
        }

        bool tmpFlag = false;
        for (auto item : tables_)
        {
            if (item->text(0) == ui->treeWidget->currentItem()->text(0)) tmpFlag = true;
        }

        auto nullIter = dbList_.find("-1");
        if (dbList_.find(currentItem) != nullIter)
        {
            menu_->addAction(refreshAction_);
            menu_->addAction(stopAction_);
            menu_->addAction(startAction_);
            menu_->addAction(backupAction_);
            menu_->addAction(restoreAction_);
            menu_->addAction(deleteAction_);
            menu_->addAction(databaseInfoAction_);
        }
        else if (currentItem1 == "Tables")
        {
            menu_->addAction(createTableAction_);
        }
        else if (currentItem1 == "Edges")
        {
            menu_->addAction(createEdgeAction_);
        }
        else if (currentItem1 == "Indexes")
        {
            menu_->addAction(createIndexAction_);
        }
        else if (tmpFlag)
        {
            menu_->addAction(selectAction_);
            menu_->addAction(createTableAction_);
            menu_->addAction(modifyStructAction_);
            menu_->addAction(deleteAction_);
        }

        menu_->popup(ui->treeWidget->viewport()->mapToGlobal(point));
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

    void on_deleteTableAction_triggered()
    {
        QMessageBox::warning(this, "", "");
    }

    void on_modifyStructAction_triggered()
    {
        QMessageBox::warning(this, "", "");
    }

    void on_selectAction_triggered()
    {
        if (ui->tabWidget->count() > 0 && currentDatabase_ != "")
        {
            TabWindow* currentTab = dynamic_cast<TabWindow*>(ui->tabWidget->currentWidget());
            currentTab->textEdit_->setText(QString("SELECT * TOP 1000 FROM " + ui->treeWidget->currentItem()->text(0)));
        }
    }

    void on_actionCreateEdge_triggered()
    {
        if (currentDatabase_ != "")
        {
            CreateEdgeTab* tmp = new CreateEdgeTab(this);
            ui->tabWidget->insertTab(ui->tabWidget->count(), tmp, QString("Create Edge"));
            tmp->SetCurrentDatabase(currentDatabase_, dbList_.find(currentDatabase_)->second);
        }
    }
    void on_actionCreateIndex_triggered()
    {
        if (currentDatabase_ != "")
        {
            CreateIndexTab* tmp = new CreateIndexTab(this);
            ui->tabWidget->insertTab(ui->tabWidget->count(), tmp, QString("Create Index"));
            tmp->SetCurrentDatabase(currentDatabase_, dbList_.find(currentDatabase_)->second);
        }
    }
    void on_actionCreateTable_triggeted()
    {
        if (currentDatabase_ != "")
        {
            CreateTableTab* tmp = new CreateTableTab(this);
            ui->tabWidget->addTab(tmp, QString("Create Table"));
            tmp->SetCurrentDatabase(currentDatabase_, dbList_.find(currentDatabase_)->second);
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
        QString dbName;
        for (int i = 0; i<currentDatabase_.count();i+=1)
        {
            if (currentDatabase_.at(i) != " ") dbName+=currentDatabase_.at(i);
            else break;
        }

        backupWindow_->show();
        backupWindow_->setWindowTitle("Backup");
        backupWindow_->address_ = this->address_;
        backupWindow_->dbName_ = dbName;
        backupWindow_->dbPort_ = QString::number(dbList_.find(currentDatabase_)->second);
        std::cout<<backupWindow_->dbName_.toStdString()<<std::endl;
        std::cout<<backupWindow_->dbPort_.toInt()<<std::endl;
    }
    void on_actionRestore_triggered()
    {        
        QString name = "";
        QString tmp = dbList_.find(currentDatabase_)->first;
        for (auto item : tmp) if (item != " ") name += item; else break;

        restoreWindow_->show();
        restoreWindow_->Name_ = name;
        restoreWindow_->Port_ = dbList_.find(currentDatabase_)->second;
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

        QString path = QFileDialog::getOpenFileName(this, tr("Open"), "C:/");
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

        QString path = QFileDialog::getSaveFileName(this, tr("Save"), "C:/");;
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
        if (index == 0) return ", NOT NULL";
        else if (index == 1) return "";
        else return "???";
    }

    QString linkCheck(QString input)
    {
        if (input != "") return QString(" -> " + input);
        return input;
    }

    QString runCheck(bool input)
    {
        if (input) return "RUN";
        else return "STOPED";
    }


    void filling_tree()
    {


        QString userName = "";
        DWORD size=1024;
        char buf[1024];
        GetUserName(buf, &size);
        userName = buf;

        if (dynamic_cast<QTreeWidgetItem*>(ui->treeWidget->currentItem()) == dynamic_cast<QTreeWidgetItem*>(ui->treeWidget->topLevelItem(0)) && ui->treeWidget->topLevelItemCount() != 0)
        {
            return;
        }
        tables_.clear();
        ui->treeWidget->takeTopLevelItem(0);
        QTreeWidgetItem* username = new QTreeWidgetItem();
        username->setText(0, userName);
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

                                    QTreeWidgetItem* columnItem = new QTreeWidgetItem();
                                    columnItem->setText(0, "Columns");
                                    table_name->addChild(columnItem);

                                    QTreeWidgetItem* indexItem = new QTreeWidgetItem();
                                    indexItem->setText(0, "Indexes");
                                    table_name->addChild(indexItem);

                                    QJsonArray indexesArray = Responce["indexes"].toArray();
                                    tables_.push_back(table_name);

                                    for (auto item_field : fields_array)
                                    {
                                        QTreeWidgetItem* field = new QTreeWidgetItem();

                                        field->setText(0, QString(item_field.toObject().find("name")->toString()
                                                                  + "  ( "
                                                                  + fieldsTypes_.find(item_field.toObject().find("type")->toInt())->second
                                                                  + " "
                                                                  + nullCheck(item_field.toObject().find("nullable")->toInt())
                                                                  + ") "
                                                                  + linkCheck(item_field.toObject().find("linktable")->toString())
                                                                  ));
                                        columnItem->addChild(field);
                                    }

                                    columnItem->setText(0, QString("Columns (" + QString::number(fields_array.count()) + ")"));

                                    int indexesCount = 0;
                                    for (auto item_field : indexesArray)
                                    {
                                        if (item_field.toObject().find("table")->toString() == table_name->text(0))
                                        {
                                            indexesCount += 1;
                                            QTreeWidgetItem* field = new QTreeWidgetItem();

                                            field->setText(0, QString(item_field.toObject().find("name")->toString()));

                                            QJsonArray indexesArray = item_field.toObject().find("fields")->toArray();

                                            for (auto item_index : indexesArray)
                                            {
                                                QTreeWidgetItem* treeItem = new QTreeWidgetItem();
                                                treeItem->setText(0, item_index.toString());

                                                field->addChild(treeItem);
                                            }

                                            indexItem->addChild(field);
                                        }
                                    }
                                    indexItem->setText(0, "Indexes (" + QString::number(indexesCount) + ")");

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
                                                                  + "  ( "
                                                                  + (fieldsTypes_.find(item_field.toObject().find("type")->toInt())->second)
                                                                  + " )  "
                                                                  + linkCheck(item_field.toObject().find("linktable")->toString())
                                                                  ));
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
    RestoreWindow* restoreWindow_ = nullptr;
    QMenu* rightClickMenu_ = nullptr;
    std::map<int, QString> fieldsTypes_ = {
        {1, "varchar"},
        {2, "int"},
        {3, "bigint"},
        {4, "double"},
        {5, "datetime"},
        {6, "bit"},
        {7, "date"},
        {9, "varbinary"},
        {10, "nvarchar"},
        {11, "rowversion"},
        {12, "decimal"}
    };

    QMenu * menu_ = nullptr;
    QAction * refreshAction_ = nullptr;
    QAction * stopAction_ = nullptr;
    QAction * startAction_ = nullptr;
    QAction * backupAction_ = nullptr;
    QAction * restoreAction_ = nullptr;
    QAction * deleteAction_ = nullptr;
    QAction * databaseInfoAction_ = nullptr;
    QAction * createTableAction_ = nullptr;
    QAction * createEdgeAction_ = nullptr;
    QAction * createIndexAction_ = nullptr;
    QAction * selectAction_ = nullptr;
    QAction * modifyStructAction_ = nullptr;
    QAction * deleteTableAction_ = nullptr;
    QList<QTreeWidgetItem*> tables_;
};
