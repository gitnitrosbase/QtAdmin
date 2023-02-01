#pragma once

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileDialog>


#include "ui_RestoreWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RestoreWindow; }
QT_END_NAMESPACE

class RestoreWindow : public QWidget
{
    Q_OBJECT
public:
    RestoreWindow(QWidget* parent = nullptr);

    ~RestoreWindow() = default;

private slots:
    void on_pushButton_clicked();

    void on_Cancel_clicked();

    void on_Restore_clicked();

private:
    Ui::RestoreWindow* ui;

public:
    int Port_;
    QString Name_;
    QString address_ = "http://127.0.0.1:8008/api3";
};
