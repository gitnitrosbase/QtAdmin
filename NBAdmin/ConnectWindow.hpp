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
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_pushButton_clicked();

private:
    Ui::ConnectWindow* ui;
public:
    QString address_ = "http://127.0.0.1:8008/api3";
};
