#include "ConnectWindow.hpp"

ConnectWindow::ConnectWindow(QWidget* parent) : QDialog(parent), ui(new Ui::ConnectWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/favicon.ico"));
}

void ConnectWindow::on_pushButton_clicked()
{
    QString Filename = QFileDialog::getExistingDirectory(this,tr("Open Path"),"C:/");
    ui->InputPath->setText(Filename);
}

void ConnectWindow::on_Create_clicked()
{
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    const QUrl url(address_);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject obj;
    obj["cmd"] = 3;

    QString dbname = ui->lineEdit->text().toStdString().c_str();
    if (dbname == "")
    {
        QMessageBox::warning(this, "Warning", "Please, enter correct database name!");
        return;
    }
    else
    {
        auto nullIterAlp = alp_.find('!');
        for (auto symbol : dbname)
        {
            if (nullIterAlp == alp_.find(symbol.toLower().toLatin1()))
            {
                QMessageBox::warning(this, "Warning", "Please, enter correct database name!");
                return;
            }
        }
    }

    int dbport = ui->lineEdit_2->text().toInt();
    if (dbport <= 0)
    {
        QMessageBox::warning(this, "Warning", "Please, enter current database port!");
        return;
    }
    obj["dbname"] = dbname;
    obj["port"] = dbport;
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

    QMessageBox::information(this, "", "The database has been created");

    emit refreshTree();
}


void ConnectWindow::on_Cancel_clicked()
{
    this->close();
}

