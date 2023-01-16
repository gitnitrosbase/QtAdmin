#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
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
    deleteIndexAction_ = new QAction(trUtf8("Delete Index"), this);
    deleteEdgeAction_ = new QAction(trUtf8("Delete Edge"), this);

    selectEdgeAction_ = new QAction(trUtf8("Select * top 1000"), this);

    connect(refreshAction_, &QAction::triggered, this, &MainWindow::filling_tree_slot);
    connect(stopAction_, &QAction::triggered, this, &MainWindow::on_actionStopTrig);
    connect(startAction_, &QAction::triggered, this, &MainWindow::on_actionStartTrig);
    connect(backupAction_, &QAction::triggered, this, &MainWindow::on_actionBackupTrig);
    connect(restoreAction_, &QAction::triggered, this, &MainWindow::on_actionRestoreTrig);
    connect(deleteAction_, &QAction::triggered, this, &MainWindow::on_actionDeleteDatabaseTrig);
    connect(databaseInfoAction_, &QAction::triggered, this, &MainWindow::on_actionDatabaseInfoTrig);
    connect(createTableAction_, &QAction::triggered, this, &MainWindow::on_actionCreateTableTrig);
    connect(createEdgeAction_, &QAction::triggered, this, &MainWindow::on_actionCreateEdgeTrig);
    connect(createIndexAction_, &QAction::triggered, this, &MainWindow::on_actionCreateIndexTrig);
    connect(selectAction_, &QAction::triggered, this, &MainWindow::on_tableSelectActionTrig);
    connect(modifyStructAction_, &QAction::triggered, this, &MainWindow::on_modifyStructActionTrig);
    connect(deleteTableAction_, &QAction::triggered, this, &MainWindow::on_deleteTableActionTrig);
    connect(deleteEdgeAction_, &QAction::triggered, this, &MainWindow::on_actionDeleteEdgeTrig);
    connect(deleteIndexAction_, &QAction::triggered, this, &MainWindow::on_actionDeleteIndexTrig);
    connect(selectEdgeAction_, &QAction::triggered, this, &MainWindow::on_actionSelectEdgeTrig);
 }

void MainWindow::filling_tree_slot()
{
    filling_tree();
}

void MainWindow::showContextMenu(const QPoint point)
{
    menu_->clear();

    QString currentItem = "";

    QString parentItem = "";

    if (ui->treeWidget->currentItem() != ui->treeWidget->topLevelItem(0))
    {
        for (int i = 0; i<ui->treeWidget->currentItem()->parent()->text(0); i+=1)
        {
            if (ui->treeWidget->currentItem()->parent()->text(0)[i] != " ") parentItem += ui->treeWidget->currentItem()->parent()->text(0)[i];
            else break;
        }
    }

    for (int i = 0; i<ui->treeWidget->currentItem()->text(0).count(); i+=1)
    {
        if (ui->treeWidget->currentItem()->text(0)[i] != " ") currentItem += ui->treeWidget->currentItem()->text(0)[i];
        else break;
    }

    bool tableFlag = false;
    for (auto item : tables_)
    {
        if (item->text(0) == ui->treeWidget->currentItem()->text(0))
        {
            tableFlag = true;
            break;
        }
    }

    bool dbFlag = false;
    for (auto item : dbList_)
    {
        if (ui->treeWidget->currentItem()->text(0) == item.first)
        {
            dbFlag = true;
            break;
        }
    }

    if (dbFlag)
    {
        menu_->addAction(refreshAction_);
        menu_->addAction(stopAction_);
        menu_->addAction(startAction_);
        menu_->addAction(backupAction_);
        menu_->addAction(restoreAction_);
        menu_->addAction(deleteAction_);
        menu_->addAction(databaseInfoAction_);
    }
    else if (parentItem == "Indexes")
    {
        std::cout<<parentItem.toStdString()<<std::endl;
        menu_->addAction(deleteIndexAction_);
    }
    else if (parentItem == "Edges")
    {
        menu_->addAction(selectEdgeAction_);
        menu_->addAction(deleteEdgeAction_);
    }
    else if (currentItem == "Tables")
    {
        menu_->addAction(createTableAction_);
    }
    else if (currentItem == "Edges")
    {
        menu_->addAction(createEdgeAction_);
    }
    else if (currentItem == "Indexes")
    {
        menu_->addAction(createIndexAction_);
    }
    else if (tableFlag)
    {
        menu_->addAction(selectAction_);
        //menu_->addAction(createTableAction_);
        menu_->addAction(modifyStructAction_);
        menu_->addAction(deleteTableAction_);
    }

    menu_->popup(ui->treeWidget->viewport()->mapToGlobal(point));
}

void MainWindow::push_button_run_clicked()
{
    if (ui->tabWidget->count() > 0 && currentDatabase_ != "")
    {
        TabWindow* currentTab = dynamic_cast<TabWindow*>(ui->tabWidget->currentWidget());
        currentTab->dbPort_ = dbList_[currentDatabase_];
        currentTab->push_button_run_clicked();
    }
}
void MainWindow::push_button_plus_clicked()
{
    ui->tabWidget->insertTab(ui->tabWidget->count(), new TabWindow(this), QString("Query " + QString::number(ui->tabWidget->count()+1)));
}
void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->removeTab(index);
}
void MainWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem*)
{
    QString dbName = current->text(0);
    if(dbList_.count(dbName) > 0 && dbList_.find(dbName) != dbList_.find("-1"))
    {
        ui->label_2->setText(dbName);
        currentDatabase_ = dbName;
    }
}

void MainWindow::on_actionSelectEdgeTrig()
{
    QString fromId = "";
    QString toId = "";

    for(int i = ui->treeWidget->currentItem()->child(0)->text(0).count()-1; i >=0; i-=1)
    {
        if(ui->treeWidget->currentItem()->child(0)->text(0).at(i) != " ") fromId += ui->treeWidget->currentItem()->child(0)->text(0).at(i); else break;
    }
    for(int i = ui->treeWidget->currentItem()->child(1)->text(0).count()-1; i >=0; i-=1)
    {
        if(ui->treeWidget->currentItem()->child(1)->text(0).at(i) != " ") toId += ui->treeWidget->currentItem()->child(1)->text(0).at(i); else break;
    }

    std::reverse(fromId.begin(), fromId.end());
    std::reverse(toId.begin(), toId.end());

    QString tmp = QString("SELECT TOP 1000 t1.id, t2.id FROM %1 t JOIN %2 t1 on t.fromid = t1.id JOIN %3 t2 on t.toid = t2.id;")
            .arg(ui->treeWidget->currentItem()->text(0))
            .arg(fromId)
            .arg(toId);
    std::cout<<tmp.toStdString()<<std::endl;

    push_button_plus_clicked();
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
    TabWindow* currentTab = dynamic_cast<TabWindow*>(ui->tabWidget->currentWidget());
    //if (std::string(typeid(&currentTab).name()) == std::string("TabWindow"))
    currentTab->textEdit_->setText(tmp);
    push_button_run_clicked();
}

void MainWindow::on_actionDeleteEdgeTrig()
{
    std::cout<<"delete edge"<<std::endl;
    NB_HANDLE connection = nb_connect( u"127.0.0.1", dbList_[currentDatabase_], u"TESTUSER", u"1234" );
    check_error(connection);
    QString tmp = QString("DROP TABLE %1").arg(ui->treeWidget->currentItem()->text(0));
    nb_execute_sql(connection, tmp.toStdU16String().c_str(), tmp.size());
    nb_disconnect(connection);
}

void MainWindow::on_actionDeleteIndexTrig()
{
    std::cout<<"delete index"<<std::endl;
    NB_HANDLE connection = nb_connect( u"127.0.0.1", dbList_[currentDatabase_], u"TESTUSER", u"1234" );
    check_error(connection);
    QString tmp = QString("DROP INDEX %1 [ON %2 ]")
            .arg(ui->treeWidget->currentItem()->text(0))
            .arg(ui->treeWidget->currentItem()->parent()->parent()->text(0));
    std::cout<<tmp.toStdString()<<std::endl;
    nb_execute_sql(connection, tmp.toStdU16String().c_str(), tmp.size());
    nb_disconnect(connection);
}

void MainWindow::on_deleteTableActionTrig()
{
    std::cout<<"delete table"<<std::endl;
    NB_HANDLE connection = nb_connect( u"127.0.0.1", dbList_[currentDatabase_], u"TESTUSER", u"1234" );
    check_error(connection);
    QString tmp = QString("DROP TABLE %1").arg(ui->treeWidget->currentItem()->text(0));
    nb_execute_sql(connection, tmp.toStdU16String().c_str(), tmp.size());
    nb_disconnect(connection);
}

void MainWindow::on_modifyStructActionTrig()
{
    QMessageBox::warning(this, "", "");
}

void MainWindow::on_tableSelectActionTrig()
{
    if (ui->tabWidget->count() > 0 && currentDatabase_ != "")
    {
        push_button_plus_clicked();
        ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
        TabWindow* currentTab = dynamic_cast<TabWindow*>(ui->tabWidget->currentWidget());
        //if (std::string(typeid(&currentTab).name()) == std::string("TabWindow"))
        currentTab->textEdit_->setText(QString("SELECT TOP 1000 * FROM " + ui->treeWidget->currentItem()->text(0)));
    }
    push_button_run_clicked();
}

void MainWindow::on_actionCreateEdgeTrig()
{
    if (currentDatabase_ != "")
    {
        CreateEdgeTab* tmp = new CreateEdgeTab(this);
        ui->tabWidget->insertTab(ui->tabWidget->count(), tmp, QString("Create Edge"));
        tmp->SetCurrentDatabase(currentDatabase_, dbList_.find(currentDatabase_)->second);

        QStringList tables = {};
        for (int i = 0; i < ui->treeWidget->currentItem()->parent()->child(0)->childCount(); i+=1)
        {
            tables.push_back(ui->treeWidget->currentItem()->parent()->child(0)->child(i)->text(0));
        }
        tmp->SetTables(tables);
    }
}
void MainWindow::on_actionCreateIndexTrig()
{
    if (currentDatabase_ != "")
    {
        CreateIndexTab* tmp = new CreateIndexTab(this);
        ui->tabWidget->insertTab(ui->tabWidget->count(), tmp, QString("Create Index"));
        tmp->SetCurrentDatabase(QString(ui->treeWidget->currentItem()->parent()->text(0)), dbList_.find(currentDatabase_)->second);
    }
}
void MainWindow::on_actionCreateTableTrig()
{
    if (currentDatabase_ != "")
    {
        CreateTableTab* tmp = new CreateTableTab(this);
        ui->tabWidget->addTab(tmp, QString("Create Table"));
        tmp->SetCurrentDatabase(currentDatabase_, dbList_.find(currentDatabase_)->second);
    }

}

void MainWindow::on_actionDeleteDatabaseTrig()
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

void MainWindow::on_actionCreateDatabaseTrig()
{
    connectWindow_->show();
    connectWindow_->setWindowTitle("Create database");
    connectWindow_->address_ = this->address_;
}
void MainWindow::on_actionOpenDatabaseTrig()
{
    openWindow_->show();
    openWindow_->setWindowTitle("Open database");
    openWindow_->address_ = this->address_;
}
void MainWindow::on_actionRefreshTrig()
{
    filling_tree();
}
void MainWindow::on_actionStopTrig()
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
void MainWindow::on_actionStartTrig()
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
void MainWindow::on_actionBackupTrig()
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
void MainWindow::on_actionRestoreTrig()
{
    QString name = "";
    QString tmp = dbList_.find(currentDatabase_)->first;
    for (auto item : tmp) if (item != " ") name += item; else break;

    restoreWindow_->show();
    restoreWindow_->Name_ = name;
    restoreWindow_->Port_ = dbList_.find(currentDatabase_)->second;
}
void MainWindow::on_actionDatabaseInfoTrig()
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
void MainWindow::on_actionInfoTrig()
{
    on_actionDatabaseInfoTrig();
}
void MainWindow::on_actionContactsTrig()
{
    QMessageBox::information(this,"Contacts", "Email: support@nitrosbase.com");
}

void MainWindow::on_actionNewQueryTrig()
{
    push_button_plus_clicked();
}

void MainWindow::on_actionCloseQueryTrig()
{
    ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
}

void MainWindow::on_actionRunQueryTrig()
{
    push_button_run_clicked();
}

void MainWindow::on_actionOpenTrig()
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

void MainWindow::on_actionSaveTrig()
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

void MainWindow::setAddress()
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

QString MainWindow::paintText(QString str, int color)
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

QString MainWindow::nullCheck(int index)
{
    if (index == 0) return ", NOT NULL";
    else if (index == 1) return "";
    else return "???";
}

QString MainWindow::linkCheck(QString input)
{
    if (input != "") return QString(" -> " + input);
    return input;
}

QString MainWindow::runCheck(bool input)
{
    if (input) return "RUN";
    else return "STOPED";
}


void MainWindow::filling_tree()
{
    QString userName = "";
    DWORD size=1024;
    char buf[1024];
    GetUserName(buf, &size);
    userName = buf;

    userName_ = userName;

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
                                                              +" "
                                                              + QString(nullCheck(item_field.toObject().find("nullable")->toInt()))
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
