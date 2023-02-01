#include "BackupWindow.hpp"

BackupWindow::BackupWindow(QWidget* parent) : QWidget(parent), ui(new Ui::BackupWindow)
{
    ui->setupUi(this);
    connect(ui->Open, SIGNAL(clicked()), this, SLOT(backupDatabase()));
    setWindowIcon(QIcon(":/images/favicon.ico"));
}

void BackupWindow::backupDatabase()
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
            if (reply->error() == QNetworkReply::NoError) std::cout<<reply->readAll().toStdString()<<std::endl;

            reply->deleteLater();
        });
        this->close();
    }
}

void BackupWindow::on_Cancel_clicked()
{
    this->close();
}

void BackupWindow::on_pushButton_clicked()
{
    QString Filename = QFileDialog::getExistingDirectory(
                this,tr("Open Path"),
                "C:\\"
                );
   ui->InputPath->setText(Filename);
}
