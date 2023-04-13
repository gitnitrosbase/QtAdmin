#include "ConnectWindow.hpp"

#include "MessageWindow.hpp"

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

void ConnectWindow::clear_fields()
{
    ui->lineEdit->setText("");
    ui->lineEdit_2->setText("");
    ui->InputPath->setText("");
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
        MessageWindow* message = new MessageWindow(this);
        message->setWindowTitle("Warning");
        message->setText(QString("Please, enter correct database name!"));
        message->setAttribute(Qt::WA_DeleteOnClose);
        message->show();
        return;
    }
    else
    {
        auto nullIterAlp = alp_.find('!');
        for (auto symbol : dbname)
        {
            if (nullIterAlp == alp_.find(symbol.toLower().toLatin1()))
            {
                MessageWindow* message = new MessageWindow(this);
                message->setWindowTitle("Warning");
                message->setText(QString("Please, enter correct database name!"));
                message->setAttribute(Qt::WA_DeleteOnClose);
                message->show();
                return;
            }
        }
    }

    int dbport = ui->lineEdit_2->text().toInt();
    if (dbport <= 0)
    {
        MessageWindow* message = new MessageWindow(this);
        message->setWindowTitle("Warning");
        message->setText(QString("Please, enter current database port!"));
        message->setAttribute(Qt::WA_DeleteOnClose);
        message->show();
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

        emit refreshTree();

        reply->deleteLater();
    });

    this->close();
    MessageWindow* message = new MessageWindow(this);
    message->setWindowTitle("Warning");
    message->setText(QString("The database has been created"));
    message->setAttribute(Qt::WA_DeleteOnClose);
    message->show();

    clear_fields();
}

void ConnectWindow::on_Cancel_clicked()
{
    clear_fields();
    this->close();
}

