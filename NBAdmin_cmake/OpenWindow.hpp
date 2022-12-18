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

#include <ui_OpenDb.h>
QT_BEGIN_NAMESPACE
namespace Ui { class OpenWindow; }
QT_END_NAMESPACE



class OpenWindow : public QWidget
{
    Q_OBJECT
public:
    OpenWindow(QWidget* parent = nullptr) : QWidget(parent), ui(new Ui::OpenWindow)
    {
        ui->setupUi(this);
        setWindowIcon(QIcon(":/images/favicon.ico"));
        connect(ui->Open, SIGNAL(clicked()), this, SLOT(OpenDatabase()));
        connect(ui->Cancel, SIGNAL(clicked()), this, SLOT(cancel_clicked()));
    }
    ~OpenWindow() = default;
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
        obj["dbname"] = ui->InputName->text().toStdString().c_str();
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

private slots:
    void on_pushButton_clicked()
    {
        QString Filename = QFileDialog::getExistingDirectory(
                    this,tr("Open Path"),
                    "C:\\"
                    );
       ui->InputPath->setText(Filename);
    }

public:
    QString address_ = "http://127.0.0.1:8008/api3";
    Ui::OpenWindow* ui;
};
