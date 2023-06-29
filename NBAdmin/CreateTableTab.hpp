#pragma once

#include <iostream>
#include <set>

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
#include <QRegularExpression>
#include <QRegularExpressionMatch>
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

#include "MessageWindow.hpp"

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
    void on_pushButton_2_clicked();
    void checkIdentity(int index);
    void blockOtherIdentity(QCheckBox* item, int state);
    bool check_query(NB_HANDLE connection);
    QString getType(QJsonObject obj);
private slots:
    void checkToAddRow(QString text);

signals:
    void refreshTree();
    void closeThisTab();

private:
    QStringList headerTable_ = {"Name", "Type", "PK", "FK", "FK table", "Identity", "NOT NULL" , "Default", ""};
    Ui::CreateTableTab* ui;
    QLineEdit* backLineEdit_ = nullptr;
public:
    QString currentDatabase_ = "";
    QString address_ = "http://127.0.0.1:8008/api3";
    int port_ = 0;
    std::vector<QString> fieldsTypes_ = {
        "bigint",
        "bit",
        "binary(50)",
        "char(10)",
        "date",
        "datetime",
        "datetime2(7)",
        "decimal(18,0)",
        "double",
        "int",
        "nchar(10)",
        "nvarchar(50)",
        "nvarchar(MAX)",
        "rowversion",
        "varbinary(50)",
        "varbinary(MAX)",
        "varchar(50)",
        "varchar(MAX)",
    };
    std::set<char> alp_ = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9','0','_','-'};
private slots:
    void on_addColumnButton_clicked();
    void on_pushButton_3_clicked();
};
