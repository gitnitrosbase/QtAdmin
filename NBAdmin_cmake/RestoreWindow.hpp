#pragma once

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileDialog>


#include "ui_restore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RestoreWindow; }
QT_END_NAMESPACE

class RestoreWindow : public QWidget
{
    Q_OBJECT
public:
    RestoreWindow(QWidget* parent = nullptr) : QWidget(parent), ui(new Ui::RestoreWindow)
    {
        ui->setupUi(this);
        setWindowIcon(QIcon(":/images/favicon.ico"));
    }
    ~RestoreWindow()
    {}

private slots:
    void on_pushButton_clicked()
    {
        QString Filename = QFileDialog::getExistingDirectory(this,tr("Open Path"), "C:/" );
        ui->InputPath->setText(Filename);
    }

    void on_Open_clicked()
    {
        QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
        const QUrl url(address_);
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QJsonObject obj;
        obj["cmd"] = 10;
        obj["port"] = Port_;
        obj["dbname"] = Name_.toStdString().c_str();
        obj["dbpath"] = "";
        obj["backuppath"] = ui->InputPath->text().toStdString().c_str();
        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();
        QNetworkReply *reply = mgr->post(request, data);
        connect(reply, &QNetworkReply::finished, [=]()
        {
            if (reply->error() == QNetworkReply::NoError)
            {
                QJsonDocument reply_doc = QJsonDocument::fromJson(reply->readAll());
                QFile file("answer.txt");
                file.open(QIODevice::ReadWrite);
                file.write(reply_doc.toJson());
                file.close();
            }
            reply->deleteLater();
        });
    }

    void on_Cancel_clicked()
    {
        this->close();
    }

private:
    Ui::RestoreWindow* ui;

public:
    int Port_;
    QString Name_;
    QString address_ = "http://127.0.0.1:8008/api3";
};
