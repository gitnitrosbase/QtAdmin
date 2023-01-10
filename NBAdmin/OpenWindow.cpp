#pragma once

#include "OpenWindow.hpp"

OpenWindow::OpenWindow(QWidget* parent) : QWidget(parent), ui(new Ui::OpenWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/favicon.ico"));
    connect(ui->Open, SIGNAL(clicked()), this, SLOT(OpenDatabase()));
    connect(ui->Cancel, SIGNAL(clicked()), this, SLOT(cancel_clicked()));
}

void OpenWindow::cancel_clicked()
{
    this->close();
}

void OpenWindow::OpenDatabase()
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

void OpenWindow::on_pushButton_clicked()
{
    QString Filename = QFileDialog::getExistingDirectory(
                this,tr("Open Path"),
                "C:\\"
                );
    ui->InputPath->setText(Filename);
}
