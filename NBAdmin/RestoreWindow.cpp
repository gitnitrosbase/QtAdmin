#pragma once

#include "RestoreWindow.hpp"

RestoreWindow::RestoreWindow(QWidget* parent) : QWidget(parent), ui(new Ui::RestoreWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/favicon.ico"));
}

void RestoreWindow::on_pushButton_clicked()
{
    QString Filename = QFileDialog::getExistingDirectory(this,tr("Open Path"), "C:/" );
    ui->InputPath->setText(Filename);
}

void RestoreWindow::on_Open_clicked()
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

void RestoreWindow::on_Cancel_clicked()
{
    this->close();
}
