#include "CreateTableTab.hpp"


CreateTableTab::CreateTableTab(QWidget* parent) : QWidget(parent), ui(new Ui::CreateTableTab)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(headerTable.count());
    ui->tableWidget->setHorizontalHeaderLabels(headerTable);

}
CreateTableTab::~CreateTableTab()
{
    delete ui;
}

void CreateTableTab::SetCurrentDatabase(QString& name, int port)
{
    ui->label->setText("Create Table in " + name);
    port_ = port;
    addRow();
}

void CreateTableTab::addRow()
{
    QLineEdit* nameLineEdit = new QLineEdit();
    QComboBox* typesComboBox = new QComboBox();
    QComboBox* FKTableComboBox = new QComboBox();

    QCheckBox* PKCheckBox = new QCheckBox();
    QCheckBox* FKCheckBox = new QCheckBox();
    QCheckBox* identityCheckBox = new QCheckBox();
    QCheckBox* notnullCheckBox = new QCheckBox();
    QPushButton* rmPushButton = new QPushButton();

    typesComboBox->setStyleSheet("background-color: #ffffff");
    FKTableComboBox->setStyleSheet("background-color: #ffffff");

    for (auto item : fieldsTypes_)
    {
        typesComboBox->addItem(item);
    }

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
            QFile replyFile("./replyFile");
            replyFile.open(QIODevice::WriteOnly);
            replyFile.write(strReply.toUtf8());

            QJsonArray tables = QJsonDocument::fromJson(strReply.toUtf8()).object().find("data")->toArray();
            for (auto item : tables)
            {
                if (item.toObject().find("type")->toInt() == 2) FKTableComboBox->addItem(item.toObject().find("tablename")->toString());
            }
            //FKTableComboBox->insertItem(0, "none");
            FKTableComboBox->setCurrentIndex(0);
        }
    });

    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 0, nameLineEdit);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 1, typesComboBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 2, PKCheckBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 3, FKCheckBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 4, FKTableComboBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 5, identityCheckBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 6, notnullCheckBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 8, rmPushButton);

    ui->tableWidget->setVerticalHeaderItem(ui->tableWidget->rowCount()-1, new QTableWidgetItem());

    connect(rmPushButton, SIGNAL(clicked()), this, SLOT(rmRow()));
    connect(typesComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){checkIdentity(index);});
    connect(identityCheckBox, QOverload<int>::of(&QCheckBox::stateChanged), this, [=](int state){blockOtherIdentity(identityCheckBox,state);});
    connect(PKCheckBox, QOverload<int>::of(&QCheckBox::stateChanged), this, [=](int state)
    {
        if (state == Qt::CheckState::Checked)
        {
            FKCheckBox->setEnabled(false);
            notnullCheckBox->setCheckState(Qt::CheckState::Checked);
        }
        else FKCheckBox->setEnabled(true);

        if (state == Qt::CheckState::Checked)
        for (int i = 0; i< ui->tableWidget->rowCount(); i+=1)
        {
            if (PKCheckBox != dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 2)))
            {
                dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 2))->setEnabled(false);
            }
        }
        else for (int i = 0; i< ui->tableWidget->rowCount(); i+=1) dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 2))->setEnabled(true);
    });
}

void CreateTableTab::rmRow()
{
    if (ui->tableWidget->rowCount() > 1) ui->tableWidget->removeRow(ui->tableWidget->currentRow());
}

void CreateTableTab::on_pushButton_2_clicked()
{
    QString name = ui->lineEdit->text();

    if (name == "")
    {
        QMessageBox::warning(this, "Warning", "Please, enter correct table name!");
        return;
    }
    else if (name.at(0).isNumber())
    {
        QMessageBox::warning(this, "Warning", "Please, enter correct table name!");
        return;
    }
    else
    {
        auto nullIterAlp = alp_.find('!');
        for (auto symbol : name)
        {
            if (nullIterAlp == alp_.find(symbol.toLower().toLatin1()))
            {
                QMessageBox::warning(this, "Warning", "Please, enter correct table name!");
                return;
            }
        }
    }

    QString queryStr = "CREATE TABLE ";
    queryStr += ui->lineEdit->text();
    queryStr += "( ";

    int identityFlag = 0;

    for (int i = 0; i < ui->tableWidget->rowCount(); i+=1)
    {
        QString columnName = dynamic_cast<QLineEdit*>(ui->tableWidget->cellWidget(i, 0))->text();
        QString typeName = dynamic_cast<QComboBox*>(ui->tableWidget->cellWidget(i, 1))->currentText();
        bool checkPK = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 2))->isChecked();
        bool checkFK = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 3))->isChecked() && dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 3))->isEnabled();
        QString nameFK = "";
        if (checkFK == true) nameFK = dynamic_cast<QComboBox*>(ui->tableWidget->cellWidget(i, 4))->currentText();
        bool checkIdentity = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 5))->isChecked();
        bool checkNullable = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 6))->isChecked();
        //QString comment = ui->tableWidget->itemAt(i, 7)->text();
        //std::cout<<columnName.toStdString()<<typeName.toStdString()<<checkPK<<checkFK<<checkIdentity<<checkNullable<<std::endl;

        QString subQueryStr = columnName;
        subQueryStr+=" ";
        subQueryStr+=typeName;
        subQueryStr+=" ";

        if (ui->lineEdit->text() == "")
        {
            QMessageBox::warning(this, "Warning", "Please enter table name");
            return;
        }

        int columnSeed = ui->SeedLineEdit->text().toInt();
        int columnIdentity = ui->SeedLineEdit->text().toInt();
        if (columnSeed <= 0 && columnIdentity <= 0)
        {
            QMessageBox::warning(this, "Warning", "Please, enter current identity information!");
            return;
        }

        if (checkPK) subQueryStr+= "PRIMARY KEY ";
        if (checkIdentity && (typeName == "int" || typeName == "bigint"))
        {
            subQueryStr+= QString("IDENTITY (%1,%2) ").arg(columnSeed).arg(columnIdentity);
            identityFlag+=1;
        }
        if (checkFK) subQueryStr+= QString("FOREIGN KEY(%1) REFERENCES %2 ").arg(columnName).arg(nameFK);
        if (checkNullable) subQueryStr += "NOT NULL ";
        if (ui->tableWidget->rowCount()-1 != i) subQueryStr+=" , ";
        queryStr += subQueryStr;
    }

    queryStr += ");";

    if (identityFlag > 1)
    {
        QMessageBox::warning(this, "Warning", "Only one identity column is allowed per table");
        return;
    }

    NB_HANDLE connection = nb_connect( u"127.0.0.1", port_, u"TESTUSER", u"1234" );
    nb_execute_sql(connection, queryStr.toStdU16String().c_str(), queryStr.count());
    if (!check_query(connection))
    {
        nb_disconnect(connection);
        return;
    }
    nb_disconnect(connection);
    nb_disconnect(connection);

    QMessageBox::information(this, "", " The table has been created");

    emit refreshTree();
}

void CreateTableTab::on_addColumnButton_clicked()
{
    addRow();
}

void CreateTableTab::checkIdentity(int index)
{
    std::cout<<index<<std::endl;
    QCheckBox* checkBoxLink = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(ui->tableWidget->currentRow(), 5));

    if (index == 1 || index == 2)
    {
        checkBoxLink->setEnabled(true);
    }
    else
    {
        checkBoxLink->setEnabled(false);
        checkBoxLink->setEnabled(false);
    }
}

void CreateTableTab::blockOtherIdentity(QCheckBox* item, int state)
{
    if (state == Qt::CheckState::Checked)
    for (int i = 0; i< ui->tableWidget->rowCount(); i+=1)
    {
        if (item != dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 5)))
        {
            dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 5))->setEnabled(false);
        }
    }
    else for (int i = 0; i< ui->tableWidget->rowCount(); i+=1) dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 5))->setEnabled(true);
}

bool CreateTableTab::check_query(NB_HANDLE connection)
{
    if (nb_errno(connection) == NB_OK) return true;
    else
    {
        QMessageBox::warning(this, "WARNING", nb_err_text_utf8( connection ));
        std::cout << "ERROR: " << nb_errno( connection ) << ": " << nb_err_text_utf8( connection ) << std::endl;
        return false;
    }
}
