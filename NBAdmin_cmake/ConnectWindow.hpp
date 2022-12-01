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
#include <windows.h>
#include <lmcons.h>

class ConnectWindow : public QWidget
{
    Q_OBJECT
public:
    ConnectWindow(QWidget* parent = nullptr) : QWidget(parent)
    {
        gridLayout_ = new QGridLayout(this);
        dbName_ = new QLineEdit(this);
        dbPort_ = new QLineEdit(this);
        dbPath_ = new QLineEdit(this);
        pushButton_ = new QPushButton(this);
        gridLayout_->addWidget(dbName_);
        gridLayout_->addWidget(dbPort_);
        gridLayout_->addWidget(dbPath_);
        gridLayout_->addWidget(pushButton_);
        connect(pushButton_, SIGNAL(clicked()), this, SLOT(createDataBase()));

        setWindowIcon(QIcon(":/images/favicon.ico"));
    }
    ~ConnectWindow() = default;
public slots:
    QString fromTCHAR(TCHAR* name, int size)
    {
        QString output;
        for (int i = 0; i< size; i+=1)
        {
            output+=name[i];
        }
        return output;
    }
    void createDataBase()
    {
        QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
        const QUrl url(address_);
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QJsonObject obj;
        obj["cmd"] = 3;
        obj["port"] = dbPort_->text().toInt();
        obj["dbname"] = dbName_->text().toStdString().c_str();
        obj["dbpath"] = dbPath_->text().toStdString().c_str();
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

private:
    QLineEdit* dbName_ = nullptr;
    QLineEdit* dbPort_ = nullptr;
    QLineEdit* dbPath_ = nullptr;
    QPushButton* pushButton_ = nullptr;
    QGridLayout* gridLayout_ = nullptr;
public:
    QString address_;
};
