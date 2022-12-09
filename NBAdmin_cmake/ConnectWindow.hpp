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

#include "ui_creadb.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CreaDB; }
QT_END_NAMESPACE


class CreaDB : public QDialog
{
    Q_OBJECT
public:
    CreaDB(QWidget* parent = nullptr) : QDialog(parent), ui(new Ui::CreaDB)
    {
        ui->setupUi(this);

        setWindowIcon(QIcon(":/images/favicon.ico"));
    }
    ~CreaDB() = default;
public slots:
    void createDatabase()
    {

    }

private slots:
    void on_buttonBox_accepted()
    {
        QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
        const QUrl url(address_);
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QJsonObject obj;
        obj["cmd"] = 3;
        obj["dbname"] = ui->lineEdit->text().toStdString().c_str();
        obj["port"] = ui->lineEdit_2->text().toInt();
        obj["dbpath"] = ui->lineEdit_3->text().toStdString().c_str();
        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();
        QNetworkReply *reply = mgr->post(request, data);
        connect(reply, &QNetworkReply::finished, [=]()
        {
            if (reply->error() == QNetworkReply::NoError) std::cout<<"db create"<<std::endl;
            else std::cout<<"error!"<<std::endl;

            reply->deleteLater();
        });
        this->close();
    }



    void on_buttonBox_rejected()
    {
        this->close();
    }

private:
    Ui::CreaDB* ui;
public:
    QString address_ = "http://127.0.0.1:8008/api3";
};
