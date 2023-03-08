#pragma once

#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileDialog>

#include "MessageWindow.hpp"

#include <iostream>

#include "ui_BackupWindow.h"
QT_BEGIN_NAMESPACE
namespace Ui { class BackupWindow; }
QT_END_NAMESPACE

class BackupWindow : public QWidget
{
    Q_OBJECT
public:
    BackupWindow(QWidget* parent = nullptr);
    ~BackupWindow() = default;
public slots:
    void backupDatabase();

    void on_Cancel_clicked();

private slots:
    void on_pushButton_clicked();

private:
    Ui::BackupWindow* ui;
public:
    QString address_ = "http://127.0.0.1:8008/api3";
    QString dbName_;
    QString dbPort_;

};
