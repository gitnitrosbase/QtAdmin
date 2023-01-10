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
#include <QMessageBox>

#include "ui_CreateTableTab.h"
#include "nb-samples.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CreateTableTab; }
QT_END_NAMESPACE

class CreateTableTab : public QWidget
{
    Q_OBJECT
public:
    explicit CreateTableTab(QWidget* parent = nullptr);
    ~CreateTableTab();

    void SetCurrentDatabase(QString& name, int port);

public slots:
    void addRow();

    void rmRow();

    void checkToAddRow();

    void on_pushButton_2_clicked();

private:
    QStringList headerTable = {"Name", "Type", "PK", "FK", "FK table", "Identity", "NOT NULL" , "Comment", ""};
    Ui::CreateTableTab* ui;
public:
    QString currentDatabase_ = "";
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
