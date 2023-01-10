#include "ConnectWindow.hpp"

ConnectWindow::ConnectWindow(QWidget* parent) : QDialog(parent), ui(new Ui::ConnectWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/favicon.ico"));
}
void ConnectWindow::createDatabase()
{

}

void ConnectWindow::on_buttonBox_accepted()
{
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    const QUrl url(address_);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject obj;
    obj["cmd"] = 3;
    obj["dbname"] = ui->lineEdit->text().toStdString().c_str();
    obj["port"] = ui->lineEdit_2->text().toInt();
    obj["dbpath"] = ui->InputPath->text().toStdString().c_str();
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

void ConnectWindow::on_buttonBox_rejected()
{
    this->close();
}

void ConnectWindow::on_pushButton_clicked()
{
    QString Filename = QFileDialog::getExistingDirectory(this,tr("Open Path"),"C:/");
    ui->InputPath->setText(Filename);
}
