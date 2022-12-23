#pragma once

#include <iostream>

#include <QWidget>

#include <QTableWidget>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QComboBox>
#include <QStringList>
#include <QFile>
#include <QCheckBox>
#include <QStyle>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QLineEdit>

#include "ui_CreateTableTab.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CreateTableTab; }
QT_END_NAMESPACE

class CreateTableTab : public QWidget
{
    Q_OBJECT
public:
    explicit CreateTableTab(QWidget* parent = nullptr) : QWidget(parent), ui(new Ui::CreateTableTab)
    {
        ui->setupUi(this);

        ui->tableWidget->setColumnCount(headerTable.count());
        ui->tableWidget->setHorizontalHeaderLabels(headerTable);
    }
    ~CreateTableTab()
    {
        delete ui;
    }


    void SetCurrentDatabase(QString& name, int port)
    {
        ui->label->setText("Create Table in " + name);
        port_ = port;

        std::cout<<port_<<std::endl;

        addRow();
    }

public slots:
    void addRow()
    {
        QLineEdit* nameLineEdit = new QLineEdit();
        QComboBox* typesComboBox = new QComboBox();
        QComboBox* FKTableComboBox = new QComboBox();
        QCheckBox* PKCheckBox = new QCheckBox();
        QCheckBox* FKCheckBox = new QCheckBox();
        QCheckBox* identityCheckBox = new QCheckBox();
        QCheckBox* notnullCheckBox = new QCheckBox();
        QPushButton* rmPushButton = new QPushButton("X");

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
                    std::cout<<item.toObject().find("tablename")->toString().toStdString()<<std::endl;
                    FKTableComboBox->addItem(item.toObject().find("tablename")->toString());
                }
                FKTableComboBox->insertItem(0, "none");
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

        connect(nameLineEdit, SIGNAL(returnPressed()), this, SLOT(checkToAddRow()));
        connect(rmPushButton, SIGNAL(clicked()), this, SLOT(rmRow()));
    }

    void rmRow()
    {
        ui->tableWidget->removeRow(ui->tableWidget->currentRow());
    }

    void checkToAddRow()
    {
        if (ui->tableWidget->currentRow() >= ui->tableWidget->rowCount()-1) addRow();
    }

private:
    QStringList headerTable = {"Name", "Type", "PK", "FK", "FK table", "Identity", "NOT NULL" , "Comment", ""};
    Ui::CreateTableTab* ui;
public:
    QString currentDatabase_ = "";
    QPushButton* addRowButton_ = nullptr;
    QPushButton* rmRowButton_ = nullptr;
    QString address_ = "http://127.0.0.1:8008/api3";
    int port_ = 0;
    std::vector<QString> fieldsTypes_ = {
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