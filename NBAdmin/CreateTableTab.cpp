#include "CreateTableTab.hpp"
#include "MessageWindow.hpp"

CreateTableTab::CreateTableTab(QWidget* parent) : QWidget(parent), ui(new Ui::CreateTableTab)
{
    ui->setupUi(this);
    this->setObjectName("CreateTableTab");
    ui->tableWidget->setColumnCount(headerTable_.count());
    ui->tableWidget->setHorizontalHeaderLabels(headerTable_);

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

QString CreateTableTab::getType(QJsonObject obj)
{
    int precision = obj.find("precision")->toInt();
    int scale = obj.find("scale")->toInt();
    switch (obj.find("type")->toInt()) {
    case 1:
        if(precision >0 && scale == 0) return QString("varchar(" + QString::number(precision) + ")");
        if(precision >0 && scale == 1) return QString("char(" + QString::number(precision) + ")");
        if(precision == -1) return QString("varchar(MAX)");
        break;
    case 2:
        return QString("int"); break;
    case 3:
        if(precision == -1) return QString("bigint");
        break;
    case 4:
        if(precision == -1) return QString("double");
        break;
    case 5:
//        if(precision > 0) return QString("datetime");
        if(precision <= 3) return QString("datetime(%1)").arg(precision);
        else return QString("datetime2(%1)").arg(precision);
        break;
    case 6:
        return QString("bit");
        break;
    case 7:
        if(precision == -1) return QString("date");
//        if(precision == -1) return QString("datetime2(MAX)");
//        else return QString("datetime2(%1)").arg(precision);
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
        if (precision == 0 && scale == 0) return QString("nvarchar");
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
    return QString(" ");
}

void CreateTableTab::addRow()
{
    QLineEdit* nameLineEdit = new QLineEdit();
    QComboBox* typesComboBox = new QComboBox();
    QComboBox* FKTableComboBox = new QComboBox();

    backLineEdit_ = nameLineEdit;

    QCheckBox* PKCheckBox = new QCheckBox();
    QCheckBox* FKCheckBox = new QCheckBox();
    QCheckBox* identityCheckBox = new QCheckBox();
    QCheckBox* notnullCheckBox = new QCheckBox();
    QLineEdit* defaultValue = new QLineEdit();
    QPushButton* rmPushButton = new QPushButton();

    typesComboBox->setEditable(true);
    typesComboBox->setStyleSheet("background-color: #ffffff");
    FKTableComboBox->setStyleSheet("background-color: #ffffff");

    identityCheckBox->setToolTip("The Identity option is not available:\neither the table already has an Identity field,\nor this field does not belong to the INT and BIGINT data types");
    identityCheckBox->setToolTipDuration(1000000);

    for (auto item : fieldsTypes_)
    {
        typesComboBox->addItem(item);
    }

    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 0, nameLineEdit);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 1, typesComboBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 2, PKCheckBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 3, FKCheckBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 4, FKTableComboBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 5, identityCheckBox);    
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 6, notnullCheckBox);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 7, defaultValue);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 8, rmPushButton);

    ui->tableWidget->setVerticalHeaderItem(ui->tableWidget->rowCount()-1, new QTableWidgetItem());

    QObject::connect(nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkToAddRow(QString)));

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
    connect(typesComboBox, QOverload<int>::of(&QComboBox::activated), [=](int index)
    {
        QComboBox* currentFKComboBox = (QComboBox*)ui->tableWidget->cellWidget(ui->tableWidget->currentRow(), 4);
        currentFKComboBox->clear();

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

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if(reply->error() == QNetworkReply::NoError)
        {
            QString strReply = reply->readAll();
            QJsonArray tables = QJsonDocument::fromJson(strReply.toUtf8()).object().find("data")->toArray();
            for (auto item_table : tables)
            {
                if (item_table.toObject().find("type")->toInt() == 2)
                {
                    QJsonArray fields = item_table.toObject().find("fields")->toArray();
                    for (auto item_field : fields)
                    {
                        if (getType(item_field.toObject()) == ((QComboBox*)ui->tableWidget->cellWidget(ui->tableWidget->currentRow(), 1))->currentText()
                                && item_field.toObject().find("subtype")->toInt() == 1)
                        {
                            currentFKComboBox->addItem(item_table.toObject().find("tablename")->toString());
                        }
                    }
                }
            }
            FKTableComboBox->setCurrentIndex(0);
        }
    });

    ui->tableWidget->setColumnWidth(1, 140);

    ui->tableWidget->resizeColumnToContents(2);
    ui->tableWidget->resizeColumnToContents(3);
    ui->tableWidget->resizeColumnToContents(5);
    ui->tableWidget->resizeColumnToContents(6);
}

void CreateTableTab::checkToAddRow(QString text)
{
    if(text != "" && backLineEdit_ != nullptr && backLineEdit_->text() == text)
    {
        addRow();
    }
}

void CreateTableTab::rmRow()
{
    if (ui->tableWidget->rowCount() > 1)
    {
        if (backLineEdit_ == ui->tableWidget->cellWidget(ui->tableWidget->currentRow(), 0))
        {
            backLineEdit_ = dynamic_cast<QLineEdit*>(ui->tableWidget->cellWidget(ui->tableWidget->currentRow()-1, 0));
        }
        ui->tableWidget->removeRow(ui->tableWidget->currentRow());
    }
}
 void CreateTableTab::on_pushButton_2_clicked()
{
    QString name = ui->lineEdit->text();

    if (name == "")
    {
        MessageWindow* message = new MessageWindow(this);
        message->setWindowTitle("Warning");
        message->setText(QString("Please, enter correct table name!"));
        message->setAttribute(Qt::WA_DeleteOnClose);
        message->show();
        return;
    }
    else if (name.at(0).isNumber())
    {
        MessageWindow* message = new MessageWindow(this);
        message->setWindowTitle("Warning");
        message->setText(QString("Please, enter correct table name!"));
        message->setAttribute(Qt::WA_DeleteOnClose);
        message->show();
        return;
    }
    else
    {
        auto nullIterAlp = alp_.find('!');
        for (auto symbol : name)
        {
            if (nullIterAlp == alp_.find(symbol.toLower().toLatin1()))
            {
                MessageWindow* message = new MessageWindow(this);
                message->setWindowTitle("Warning");
                message->setText(QString("Please, enter correct table name!"));
                message->setAttribute(Qt::WA_DeleteOnClose);
                message->show();
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

        if (i == ui->tableWidget->rowCount()-1 && columnName == "") break;

        QString typeName = dynamic_cast<QComboBox*>(ui->tableWidget->cellWidget(i, 1))->currentText();
        bool checkPK = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 2))->isChecked();
        bool checkFK = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 3))->isChecked() && dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 3))->isEnabled();
        QString nameFK = "";
        if (checkFK == true) nameFK = dynamic_cast<QComboBox*>(ui->tableWidget->cellWidget(i, 4))->currentText();
        bool checkIdentity = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 5))->isChecked();
        bool checkNullable = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 6))->isChecked();
        QString defaultValue = dynamic_cast<QLineEdit*>(ui->tableWidget->cellWidget(i, 7))->text();

        QString subQueryStr = columnName;
        subQueryStr+=" ";

        QRegularExpression rx(
                            "^int$|"
                            "^bigint$|"
                            "^double$|"
                            "^datetime$|"
                            "^bit$|"
                            "^date$|"
                            "^rowversion$|"
                            "^char\\((?:[1-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-2][0-9][0-9][0-9][0-9]|[3][0-2][0-6][0-9][0-9]|32700)\\)$|"
                            "^binary\\((?:[1-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-2][0-9][0-9][0-9][0-9]|[3][0-2][0-6][0-9][0-9]|32700)\\)$|"
                            "^datetime2\\(([1-7]\\))$|"
                            "^decimal\\((([1-9])|([1-3][0-8])),(([0-9])|([0-9][0-9]))\\)$|"
                            "^nchar\\((?:[1-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-2][0-9][0-9][0-9][0-9]|[3][0-2][0-6][0-9][0-9]|32700)\\)$|"
                            "^nvarchar\\((?:[1-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-2][0-9][0-9][0-9][0-9]|[3][0-2][0-6][0-9][0-9]|32700)\\)$|"
                            "^nvarchar\\(MAX\\)$|"
                            "^varbinary\\((?:[1-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-2][0-9][0-9][0-9][0-9]|[3][0-2][0-6][0-9][0-9]|32700)\\)$|"
                            "^varbinary\\(MAX\\)$|"
                            "^varchar\\((?:[1-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-2][0-9][0-9][0-9][0-9]|[3][0-2][0-6][0-9][0-9]|32700)\\)$|"
                            "^varchar\\(MAX\\)$"
                            ,QRegularExpression::CaseInsensitiveOption);

        QRegularExpressionMatch match = rx.match(typeName);
        bool hasMatch = match.hasMatch();


        QRegularExpression decimalRegex("^decimal\\((([0-9])|([1-9][0-9])),(([0-9])|([0-9][0-9]))\\)");
        QRegularExpressionMatch matchDecimalRegex = decimalRegex.match(typeName);
        if (matchDecimalRegex.hasMatch())
        {
            std::string tmp1 = typeName.toStdString();
            std::vector<int> numbers;
            std::string numStr = "";
            for (char c : tmp1)
            {
                if (isdigit(c))
                {
                    numStr += c;
                }
                else if (!numStr.empty())
                {
                    numbers.push_back(stoi(numStr));
                    numStr = "";
                }
            }
            if (!numStr.empty())
            {
                numbers.push_back(stoi(numStr));
            }
            if (0 > numbers.at(1) || numbers.at(1) > numbers.at(0))
            {
                hasMatch = false;
            }
        }

        if(!hasMatch)
        {
            MessageWindow* message = new MessageWindow(this);
            message->setWindowTitle("Warning");
            message->setText(QString("Invalid type name!"));
            message->setAttribute(Qt::WA_DeleteOnClose);
            message->show();
            return;
        }

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

        bool columnSeedCorrent = true;
        bool columnIncrementCorrent = true;
        int columnSeed = ui->SeedLineEdit->text().toInt(&columnSeedCorrent);
        int columnIdentity = ui->SeedLineEdit->text().toInt(&columnIncrementCorrent);

        if ( columnSeed < 1 || columnIdentity < 1 || !columnIncrementCorrent || !columnSeedCorrent )
        {
            MessageWindow* message = new MessageWindow(this);
            message->setWindowTitle("Warning");
            message->setText(QString("Please, enter current identity information!"));
            message->setAttribute(Qt::WA_DeleteOnClose);
            message->show();
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

        if (defaultValue != "") subQueryStr += QString(" DEFAULT '%1'").arg(defaultValue);

        if (ui->tableWidget->rowCount()-1 != i) subQueryStr+=" , ";
        queryStr += subQueryStr;
    }

    queryStr += ");";

    if (identityFlag > 1)
    {
        MessageWindow* message = new MessageWindow(this);
        message->setWindowTitle("Warning");
        message->setText(QString("Only one identity column is allowed per table"));
        message->setAttribute(Qt::WA_DeleteOnClose);
        message->show();
        return;
    }

    NB_HANDLE connection = nb_connect( u"127.0.0.1", port_, u"TESTUSER", u"1234" );
    nb_execute_sql(connection, queryStr.toStdU16String().c_str(), queryStr.size());
    if (!check_query(connection))
    {
        nb_disconnect(connection);
        return;
    }
    nb_disconnect(connection);

    MessageWindow* message = new MessageWindow(this);
    message->setWindowTitle("Warning");
    message->setText(QString("The table has been created"));
    message->setAttribute(Qt::WA_DeleteOnClose);
    message->show();

    emit refreshTree();
}

void CreateTableTab::on_addColumnButton_clicked()
{
    addRow();
}

void CreateTableTab::checkIdentity(int index)
{
    QCheckBox* checkBoxLink = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(ui->tableWidget->currentRow(), 5));

    if (fieldsTypes_.at(index) == "int" || fieldsTypes_.at(index) == "bigint")
    {
        checkBoxLink->setEnabled(true);
    }
    else
    {
        checkBoxLink->setChecked(false);
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
    else for (int i = 0; i< ui->tableWidget->rowCount(); i+=1)
    {
        if (
                dynamic_cast<QComboBox*>(ui->tableWidget->cellWidget(i, 1))->currentText() == "bigint"||
                dynamic_cast<QComboBox*>(ui->tableWidget->cellWidget(i, 1))->currentText() == "int"
                )
            dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 5))->setEnabled(true);
    }
}

bool CreateTableTab::check_query(NB_HANDLE connection)
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

void CreateTableTab::on_pushButton_3_clicked()
{
    emit closeThisTab();
}

