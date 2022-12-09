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

#include <ui_OpenDb.h>
QT_BEGIN_NAMESPACE
namespace Ui { class OpenDB; }
QT_END_NAMESPACE



class OpenDB : public QWidget
{
    Q_OBJECT
public:
    OpenDB(QWidget* parent = nullptr) : QWidget(parent), ui(new Ui::OpenDB)
    {
        ui->setupUi(this);
        setWindowIcon(QIcon(":/images/favicon.ico"));
        connect(ui->Open, SIGNAL(clicked()), this, SLOT(OpenDatabase()));
        connect(ui->Cancel, SIGNAL(clicked()), this, SLOT(cancel_clicked()));
    }
    ~OpenDB() = default;
public slots:
    void cancel_clicked()
    {
        this->close();
    }

    void OpenDatabase()
    {
        QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
        const QUrl url(address_);
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QJsonObject obj;
        obj["cmd"] = 1;
        obj["port"] = ui->InputPort->text().toInt();
        obj["dbname"] = ui->InputName->text().toInt();
        obj["dbpath"] = ui->InputPath->text().toStdString().c_str();
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

public:
    QString address_ = "http://127.0.0.1:8008/api3";
    Ui::OpenDB* ui;
};
