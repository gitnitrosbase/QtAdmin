    #include "CreateIndexTab.hpp"

CreateIndexTab::CreateIndexTab(QWidget* parent) : QWidget(parent), ui(new Ui::CreateIndexTab)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList{"Column", ""});
}
void CreateIndexTab::SetCurrentDatabase(QString name, int port)
{
    ui->label->setText("Create Index in " + name);
    tableName_ = name;
    port_ = port;

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    const QUrl url(address_);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject obj;
    obj["cmd"] = 8;
    obj["port"] = port_;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    QNetworkReply *reply = mgr->post(request, data);
    connect(reply, &QNetworkReply::finished, [=]()
    {
        if(reply->error() == QNetworkReply::NoError)
        {
            QString strReply = reply->readAll();
            QFile replyFile("./replyFileIndex");
            replyFile.open(QIODevice::WriteOnly);
            replyFile.write(strReply.toUtf8());

            QJsonArray tables = QJsonDocument::fromJson(strReply.toUtf8()).object().find("data")->toArray();

            for (auto item : tables)
            {
                if (tableName_ == item.toObject().find("tablename")->toString())
                {
                    for (auto fields : item.toObject().find("fields")->toArray())
                    {
                        if (fields.toObject().find("linktable")->toString() == "" && fields.toObject().find("subtype")->toInt() == 0)
                        {
                            tableList_ += fields.toObject().find("name")->toString();
                        }
                    }
                    break;
                }
            }
        }
        reply->deleteLater();
        on_pushButton_clicked();
    });
}
CreateIndexTab::~CreateIndexTab()
{
    delete ui;
}

void CreateIndexTab::on_pushButton_2_clicked()
{
    QString query = QString("CREATE INDEX %1 ON %2 (").arg(ui->lineEdit->text()).arg(tableName_);

    for (int i = 0; i< ui->tableWidget->rowCount(); i+=1)
    {
        QString columnIndex = dynamic_cast<QComboBox*>(ui->tableWidget->cellWidget(i, 0))->currentText();
        query+=columnIndex;
        query+=",";
    }
    query.resize(query.count() - 1);
    query+=");";

    NB_HANDLE connection = nb_connect( u"127.0.0.1", port_, u"TESTUSER", u"1234" );
    nb_execute_sql(connection, query.toStdU16String().c_str(), query.count());
    check_error(connection);
    nb_disconnect(connection);
    QMessageBox::information(this, "", " The index has been created");
}

void CreateIndexTab::on_pushButton_clicked()
{
    if (ui->tableWidget->rowCount() == tableList_.count()) return;

    if (tableList_.count() <= 0)
    {
        ui->pushButton->setEnabled(false);
        return;
    }

    QComboBox* comboBox = new QComboBox();
    QPushButton* button = new QPushButton("");

    connect(button, SIGNAL(clicked()), this, SLOT(rmRow()));
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){setCurrenttableList(index);});


    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 0, comboBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 1, button);

    comboBox->addItems(tableList_);

}

void CreateIndexTab::rmRow()
{
    if (ui->tableWidget->rowCount() > 0)
    {
        ui->pushButton->setEnabled(true);

        if (ui->tableWidget->rowCount() > 1)
        {
            tableList_ += dynamic_cast<QComboBox*>(ui->tableWidget->cellWidget(ui->tableWidget->currentRow(), 0))->currentText();
            ui->tableWidget->removeRow(ui->tableWidget->currentRow());
        }
    }

//    for (int i = 0; i < ui->tableWidget->rowCount(); i+=1)
//    {
//        dynamic_cast<QComboBox*>(ui->tableWidget->cellWidget(ui->tableWidget->currentRow(), 0))->clear();
//        dynamic_cast<QComboBox*>(ui->tableWidget->cellWidget(ui->tableWidget->currentRow(), 0))->addItems(tableList_);
//    }
}

void CreateIndexTab::setCurrenttableList(int index)
{
//    std::cout<<ui->tableWidget->currentRow()<<std::endl;
//    if (ui->tableWidget->currentRow() >= 0) std::cout<<dynamic_cast<QComboBox*>(ui->tableWidget->cellWidget(ui->tableWidget->currentRow(), 0))->currentText().toStdString()<<" --> "<<tableList_.at(index).toStdString()<<std::endl;


//    if (ui->tableWidget->rowCount() > 0)tableList_+= dynamic_cast<QComboBox*>(ui->tableWidget->cellWidget(ui->tableWidget->currentRow(), 0))->currentText();
//    //std::cout<<index<<std::endl;
//    //tableList_.removeAt(0 + std::find(tableList_.begin(), tableList_.end(), ));
//    tableList_.removeAt(index);

//    for (int i = 0; i < ui->tableWidget->rowCount(); i+=1)
//    {
//        if (std::find(tableList_.begin(), tableList_.end(), dynamic_cast<QComboBox*>(ui->tableWidget->cellWidget(i, 0))->currentText())))
//    }
}
