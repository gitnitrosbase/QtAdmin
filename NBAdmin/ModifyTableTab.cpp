#include "ModifyTableTab.hpp"
#include "MessageWindow.hpp"

ModifyTableTab::ModifyTableTab(QWidget *parent)
    : QWidget{parent}, ui(new Ui::ModifyTableTab)
{
    ui->setupUi(this);
    this->setObjectName("ModifyTableTab");
    ui->tableWidget->setColumnCount(headerTable.length());
    ui->tableWidget->setHorizontalHeaderLabels(headerTable);
}

ModifyTableTab::~ModifyTableTab()
{
    delete ui;
}

void ModifyTableTab::checkToAddRow(QString text)
{
    if(text != "" && backLineEdit_ != nullptr && backLineEdit_->text() == text)
    {
        addRow();
    }
}

QString ModifyTableTab::precisionCheck(QJsonObject obj) {
    int precision = obj.find("precision")->toInt();
    int scale = obj.find("scale")->toInt();
    switch (obj.find("type")->toInt())
    {
        case 1:
           return QString("");
           break;
        case 2:
            return "int";
            break;
        case 3:
            if(precision == -1) return QString("bigint");
            break;
        case 4:
            if(precision == -1) return QString("double");
            break;
        case 5:
            if(precision > 0) return QString("datetime");
            break;
        case 6:
            return QString("");
            break;
        case 7:
            if(precision == -1) return QString("date");
            break;
        case 8:
            return QString("");
            break;
        case 9:
            if(precision > 0 && scale == 1)
            {
                return QString("binary(" + QString::number(precision) + ")");
            }
            if(precision > 0 && scale == 0)
            {
                return QString("varbinary(" + QString::number(precision) + ")");
            }
            if(precision == -1)
            {
                return QString("varbinary(MAX)");
            }
            break;
        case 10:
            if(precision >0 && scale == 0)
            {
                return QString("nvarchar(" + QString::number(precision) + ")");
            }
            if(precision >0 && scale == 1)
            {
               return QString("nchar(" + QString::number(precision) + ")");
            }
            if(precision == -1)
            {
                return QString("nvarchar(MAX)");
            }
            break;
        case 11:
            if(precision == -1) return QString("rowversion");
            break;
        case 12:
            return QString("decimal(" + QString::number(precision) + "," + QString::number(scale) + ")");
            break;
        default:
           break;
    }
    return QString("");
}

void ModifyTableTab::printFromdb()
{
    ui->lineEdit->setText(currentTable_);
    ui->lineEdit->setReadOnly(true);

    QJsonObject tableNames;

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
            QJsonArray tables = QJsonDocument::fromJson(strReply.toUtf8()).object().find("data")->toArray();

            for (auto item : tables)
            {
                if (currentTable_ == item.toObject().find("tablename")->toString())
                {
                    for (auto fields : item.toObject().find("fields")->toArray())
                    {
                        QLineEdit* nameLineEdit = new QLineEdit();
                        QComboBox* typesComboBox = new QComboBox();
                        QComboBox* FKTableComboBox = new QComboBox();
                        QCheckBox* PKCheckBox = new QCheckBox();
                        QCheckBox* FKCheckBox = new QCheckBox();
                        QCheckBox* identityCheckBox = new QCheckBox();
                        QCheckBox* notnullCheckBox = new QCheckBox();

                        typesComboBox->setStyleSheet("background-color: #ffffff");
                        FKTableComboBox->setStyleSheet("background-color: #ffffff");

                        nameLineEdit->setText(fields.toObject().find("name")->toString());
                        typesComboBox->addItem(precisionCheck(fields.toObject()));
                        if(fields.toObject().find("subtype")->toInt() == 1)
                        {
                            PKCheckBox->setCheckState(Qt::CheckState::Checked);
                            PKFlag_ = true;
                        }
                        if (fields.toObject().find("linktable")->toString() != "")
                        {
                            FKCheckBox->setCheckState(Qt::CheckState::Checked);
                            FKTableComboBox->addItem(fields.toObject().find("linktable")->toString());
                        }
                        if (fields.toObject().find("seed")->toInt() != 0 && fields.toObject().find("increment")->toInt() != 0)
                        {
                            identityCheckBox->setCheckState(Qt::CheckState::Checked);
                            identityFlag_ = true;
                        }
                        if(fields.toObject().find("nullable")->toInt() == 0) notnullCheckBox->setCheckState(Qt::CheckState::Checked);

                        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
                        ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 0, nameLineEdit);
                        ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 1, typesComboBox);
                        ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 2, PKCheckBox);
                        ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 3, FKCheckBox);
                        ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 4, FKTableComboBox);
                        ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 5, identityCheckBox);
                        ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 6, notnullCheckBox);

                        ui->tableWidget->setVerticalHeaderItem(ui->tableWidget->rowCount()-1, new QTableWidgetItem());

                        nameLineEdit->setReadOnly(true);
                        PKCheckBox->setEnabled(false);
                        FKCheckBox->setEnabled(false);
                        identityCheckBox->setEnabled(false);
                        notnullCheckBox->setEnabled(false);
                        //ui->tableWidget->resizeColumnsToContents();
                    }
                    break;
                }
            }
            fieldCount_ = ui->tableWidget->rowCount();
            std::cout<<"row count = "<<ui->tableWidget->rowCount()<<std::endl;
            std::cout<<"fieldCount_ = "<<fieldCount_<<std::endl;

            addRow();
        }
        reply->deleteLater();
    });
}

void ModifyTableTab::on_saveButton_clicked()
{
    QString queryStr = "";
    int identityFlag = 0;
    for (int i = fieldCount_; i < ui->tableWidget->rowCount(); i+=1)
    {
        QString columnName = dynamic_cast<QLineEdit*>(ui->tableWidget->cellWidget(i, 0))->text();
        QString typeName = dynamic_cast<QComboBox*>(ui->tableWidget->cellWidget(i, 1))->currentText();
        bool checkPK = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 2))->isChecked();
        bool checkFK = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 3))->isChecked();
        QString nameFK = "";
        if (checkFK == true) nameFK = dynamic_cast<QComboBox*>(ui->tableWidget->cellWidget(i, 4))->currentText();
        bool checkIdentity = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 5))->isChecked();
        bool checkNullable = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 6))->isChecked();
        //QString comment = ui->tableWidget->itemAt(i, 7)->text();

        QString subQueryStr = QString("ALTER TABLE %1 ADD %2").arg(currentTable_).arg(columnName);

        subQueryStr+=" ";
        subQueryStr+=typeName;
        subQueryStr+=" ";

        if (ui->lineEdit->text() == "")
        {
            MessageWindow* message = new MessageWindow(this);
            message->setWindowTitle("Warning");
            message->setText(QString("Please enter table name"));
            message->setAttribute(Qt::WA_DeleteOnClose);
            message->show();
            return;
        }

        QRegularExpression rx(
                              "^varchar$|"
                              "^int$|"
                              "^bigint$|"
                              "^double$|"
                              "^datetime$|"
                              "^datetime2$|"
                              "^bit$|"
                              "^date$|"
                              "^varbinary$|"
                              "^nvarchar$|"
                              "^rowversion$|"
                              "^decimal$|"
                              "^binary\\((?:[1-9]|[1-4][0-9]|50)\\)$|"
                              "^char\\(([1-9]|10)\\)$|"
                              "^datetime2\\(([1-7]\\))$|"
                              "^decimal\\((?:[1-9]|[1][1-8])\\,[0-9]\\)$"
                              "^nchar\\(([1-9]|10)\\)$|"
                              "^nvarchar\\(([1-9]|[1-4][0-9]|50)\\)$|"
                              "^nvarchar\\(MAX\\)$|"
                              "^varbinary\\(([1-9]|[1-4][0-9]|50)\\)$|"
                              "^varbinary\\(MAX\\)$|"
                              "^varchar\\(([1-9]|[1-4][0-9]|50)\\)$|"
                              "^varchar\\(MAX\\)$"
                              , QRegularExpression::CaseInsensitiveOption);

        QRegularExpressionMatch match = rx.match(typeName);
        bool hasMatch = match.hasMatch();
        if(!hasMatch)
        {
            MessageWindow* message = new MessageWindow(this);
            message->setWindowTitle("Warning");
            message->setText(QString("Invalid type name!"));
            message->setAttribute(Qt::WA_DeleteOnClose);
            message->show();
            return;
        }

        if (checkPK) subQueryStr+= "PRIMARY KEY ";
        if (checkIdentity && (typeName == "int" || typeName == "bigint"))
        {
            subQueryStr+= QString("IDENTITY (%1,%2) ").arg(ui->SeedLineEdit->text()).arg(ui->IncrementLineEdit->text());
            identityFlag+=1;
        }
        if (checkFK) subQueryStr+= QString("FOREIGN KEY(%1) REFERENCES %2 ").arg(columnName).arg(nameFK);
        if (checkNullable) subQueryStr += "NOT NULL ";

        queryStr += subQueryStr;
        queryStr += ";";
    }

    if (identityFlag > 1)
    {
        MessageWindow* message = new MessageWindow(this);
        message->setWindowTitle("Warning");
        message->setText(QString("Only one identity column is allowed per table"));
        message->setAttribute(Qt::WA_DeleteOnClose);
        message->show();
        return;
    }

    std::cout<<queryStr.toStdString()<<std::endl;

    NB_HANDLE connection = nb_connect( u"127.0.0.1", port_, u"TESTUSER", u"1234" );
    nb_execute_sql(connection, queryStr.toStdU16String().c_str(), queryStr.count());
    if (!check_query(connection))
    {
        nb_disconnect(connection);
        return;
    }
    nb_disconnect(connection);

    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    printFromdb();

    MessageWindow* message = new MessageWindow(this);
    message->setWindowTitle("Warning");
    message->setText(QString("The table has been changed"));
    message->setAttribute(Qt::WA_DeleteOnClose);
    message->show();
    emit refreshTree();
}

void ModifyTableTab::addRow()
{
    QLineEdit* nameLineEdit = new QLineEdit();
    QComboBox* typesComboBox = new QComboBox();
    QComboBox* FKTableComboBox = new QComboBox();
    QCheckBox* PKCheckBox = new QCheckBox();
    QCheckBox* FKCheckBox = new QCheckBox();
    QCheckBox* identityCheckBox = new QCheckBox();
    QCheckBox* notnullCheckBox = new QCheckBox();
    QPushButton* rmPushButton = new QPushButton("");

    backLineEdit_ = nameLineEdit;
    typesComboBox->setEditable(true);
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
            FKTableComboBox->setCurrentIndex(0);
        }
    });
    std::cout<<ui->tableWidget->rowCount()<<std::endl;
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 0, nameLineEdit);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 1, typesComboBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 2, PKCheckBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 3, FKCheckBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 4, FKTableComboBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 5, identityCheckBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 6, notnullCheckBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 8, rmPushButton);

    QObject::connect(nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkToAddRow(QString)));

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
        for (int i = fieldCount_; i< ui->tableWidget->rowCount(); i+=1)
        {
            if (PKCheckBox != dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 2)))
            {
                dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 2))->setEnabled(false);
            }
        }
        else for (int i = fieldCount_; i< ui->tableWidget->rowCount(); i+=1) dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 2))->setEnabled(true);
    });
    if (PKFlag_) PKCheckBox->setEnabled(false);
    if (identityFlag_) identityCheckBox->setEnabled(false);
}


void ModifyTableTab::rmRow()
{
    if (ui->tableWidget->rowCount() > 1) ui->tableWidget->removeRow(ui->tableWidget->currentRow());
}

//void ModifyTableTab::on_addRowButton_clicked()
//{
//    addRow();
//}

void ModifyTableTab::checkIdentity(int index)
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
    }
}

void ModifyTableTab::blockOtherIdentity(QCheckBox* item, int state)
{

    if (state == Qt::CheckState::Checked)
    for (int i = 0; i< ui->tableWidget->rowCount(); i+=1)
    {
        if (item != dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 5)))
        {
            dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 5))->setEnabled(false);
        }
    }
    else for (int i = fieldCount_; i< ui->tableWidget->rowCount(); i+=1) dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 5))->setEnabled(true);
}

bool ModifyTableTab::check_query(NB_HANDLE connection)
{
    if (nb_errno(connection) == NB_OK) return true;
    else
    {
        MessageWindow* message = new MessageWindow(this);
        message->setWindowTitle("Warning");
        message->setText(QString(nb_err_text_utf8( connection )));
        message->setAttribute(Qt::WA_DeleteOnClose);
        message->show();
        std::cout << "ERROR: " << nb_errno( connection ) << ": " << nb_err_text_utf8( connection ) << std::endl;
        return false;
    }
}
