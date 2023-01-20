#pragma once

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
#include <QStringList>

#include <vector>
#include <iostream>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QLineEdit>
#include <QMessageBox>

#include "ui_ModifyTableTab.h"

#include "nb-samples.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ModifyTableTab; }
QT_END_NAMESPACE

class ModifyTableTab : public QWidget
{
    Q_OBJECT
public:
    explicit ModifyTableTab(QWidget *parent = nullptr);
    ~ModifyTableTab();

public slots:
    void addRow();
    void rmRow();
    //void checkToAddRow();
    void on_saveButton_clicked();
    void printFromdb();

public:
    int fieldCount_ = 0;
    int port_ = 0;
    QString currentTable_ = "";
    QString query_ = "";
    QString address_ = "http://127.0.0.1:8008/api3";
    bool identityFlag_ = false;
    bool PKFlag_ = false;

private slots:
    void on_addRowButton_clicked();

    void checkIdentity(int index);

    void blockOtherIdentity(QCheckBox* item, int state);

private:
    QStringList headerTable = {"Name", "Type", "PK", "FK", "FK table", "Identity", "NOT NULL" , "Comment", ""};
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
    Ui::ModifyTableTab* ui;
};
