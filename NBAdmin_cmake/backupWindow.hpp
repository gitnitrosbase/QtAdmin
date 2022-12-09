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

#include <iostream>

#include "ui_backup.h"
QT_BEGIN_NAMESPACE
namespace Ui { class BackupWindow; }
QT_END_NAMESPACE



class BackupWindow : public QWidget
{
    Q_OBJECT
public:
    BackupWindow(QWidget* parent = nullptr) : QWidget(parent), ui(new Ui::BackupWindow)
    {

        ui->setupUi(this);
        connect(ui->Open, SIGNAL(clicked()), this, SLOT(backupDatabase()));
        setWindowIcon(QIcon(":/images/favicon.ico"));
    }
    ~BackupWindow() = default;
public slots:
    void backupDatabase()
    {
        QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
        const QUrl url(address_);
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QJsonObject obj;
        obj["cmd"] = 5;
//        obj["port"] = dbPort_->text().toInt();
//        obj["dbname"] = dbName_->text().toStdString().c_str();
//        obj["dbpath"] = dbPath_->text().toStdString().c_str();
        obj["backuppath"] = ui->InputPath->text().toStdString().c_str();
        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();
        QNetworkReply *reply = mgr->post(request, data);
        connect(reply, &QNetworkReply::finished, [=]()
        {
            if (reply->error() == QNetworkReply::NoError) std::cout<<"db open"<<std::endl;
            else std::cout<<"error!"<<std::endl;

            reply->deleteLater();
        });
        this->close();
    }

private:
    Ui::BackupWindow* ui;
public:
    QString address_ = "http://127.0.0.1:8008/api3";
    int currnetPort_;
    QString currentName_;
};
