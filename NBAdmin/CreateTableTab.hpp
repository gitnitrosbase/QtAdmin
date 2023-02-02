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
    void on_pushButton_2_clicked();
    void checkIdentity(int index);
    void blockOtherIdentity(QCheckBox* item, int state);
    bool check_query(NB_HANDLE connection);

signals:
    void refreshTree();

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
    std::set<char> alp_ = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9','0','_','-'};
private slots:
    void on_addColumnButton_clicked();
};
