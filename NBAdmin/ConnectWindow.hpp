#pragma once

#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileDialog>
#include <QMessageBox>

#include <set>
#include <iostream>

#include "ui_ConnectWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ConnectWindow; }
QT_END_NAMESPACE


class ConnectWindow : public QDialog
{
    Q_OBJECT
public:
    ConnectWindow(QWidget* parent = nullptr);
    ~ConnectWindow() = default;
public slots:
    void createDatabase();

private slots:
    void on_pushButton_clicked();

    void on_Create_clicked();

    void on_Cancel_clicked();

private:
    Ui::ConnectWindow* ui;
    std::set<char> alp_ = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9','0','_','-'};
public:
    QString address_ = "http://127.0.0.1:8008/api3";
};
