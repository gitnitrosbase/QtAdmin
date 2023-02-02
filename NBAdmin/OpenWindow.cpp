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

    QString dbname = ui->InputName->text();
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

    int dbport = ui->InputPort->text().toInt();
    if (dbport <= 0)
    {
        QMessageBox::warning(this, "Warning", "Please, enter current database port!");
        return;
    }
    obj["port"] = dbport;
    obj["dbname"] = dbname.toStdString().c_str();
    obj["dbpath"] = ui->InputPath->text().toStdString().c_str();
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    QNetworkReply *reply = mgr->post(request, data);
    connect(reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() == QNetworkReply::NoError) std::cout<<reply->readAll().toStdString()<<std::endl;
        else std::cout<<"error!"<<std::endl;

        reply->deleteLater();
    });
    this->close();

    QMessageBox::information(this, "", "The database has been opened");

    emit refreshTree();
}

void OpenWindow::on_buttonPath_clicked()
{
    QString Filename = QFileDialog::getExistingDirectory(
                this,tr("Open Path"),
                "C:\\"
                );
    ui->InputPath->setText(Filename);
}

