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
        if (dbName_ != "" && dbPort_ != "")
        {
            QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
            const QUrl url(address_);
            QNetworkRequest request(url);
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            QJsonObject obj;
            obj["cmd"] = 5;
            obj["port"] = dbPort_.toInt();
            obj["dbname"] = dbName_.toStdString().c_str();
            obj["dbpath"] = "";
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
    }

    void on_Cancel_clicked()
    {
        this->close();
    }

private slots:
    void on_pushButton_clicked()
    {
        QString Filename = QFileDialog::getExistingDirectory(
                    this,tr("Open Path"),
                    "C:\\"
                    );
       ui->InputPath->setText(Filename);
    }

private:
    Ui::BackupWindow* ui;
public:
    QString address_ = "http://127.0.0.1:8008/api3";
    QString dbName_;
    QString dbPort_;

};
