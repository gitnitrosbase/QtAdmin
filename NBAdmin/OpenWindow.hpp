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

#include <ui_OpenWindow.h>
QT_BEGIN_NAMESPACE
namespace Ui { class OpenWindow; }
QT_END_NAMESPACE

class OpenWindow : public QWidget
{
    Q_OBJECT
public:
    OpenWindow(QWidget* parent = nullptr);

    ~OpenWindow() = default;

public slots:
    void cancel_clicked();

    void OpenDatabase();

private slots:
    void on_pushButton_clicked();

public:
    QString address_ = "http://127.0.0.1:8008/api3";
    Ui::OpenWindow* ui;
};
