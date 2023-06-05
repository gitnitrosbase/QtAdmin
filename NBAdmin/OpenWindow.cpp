#include "OpenWindow.hpp"
#include "MessageWindow.hpp"

OpenWindow::OpenWindow(QWidget* parent) : QWidget(parent), ui(new Ui::OpenWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/favicon.ico"));
    connect(ui->Open, SIGNAL(clicked()), this, SLOT(OpenDatabase()));
    connect(ui->Cancel, SIGNAL(clicked()), this, SLOT(cancel_clicked()));
}

void OpenWindow::cancel_clicked()
{
    clear_fields();
    this->close();
}

void OpenWindow::clear_fields()
{
    ui->InputName->setText("");
    ui->InputPort->setText("");
    ui->InputPath->setText("");
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

    int dbport = ui->InputPort->text().toInt();
    if (dbport <= 0)
    {
        MessageWindow* message = new MessageWindow(this);
        message->setWindowTitle("Warning");
        message->setText(QString("Please, enter correct database name!"));
        message->setAttribute(Qt::WA_DeleteOnClose);
        message->show();
        return;
    }
    obj["port"] = dbport;
    obj["dbname"] = dbname.toStdString().c_str();
    obj["dbpath"] = ui->InputPath->text().toStdString().c_str();
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    QNetworkReply *reply = mgr->post(request, data);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError)
    {
        QString responce = reply->readAll();

        std::cout<<responce.toStdString()<<std::endl;

        MessageWindow* message = new MessageWindow(this);
        message->setWindowTitle("Warning");
        QString text;
        {
            QJsonObject jsonObject = QJsonDocument::fromJson(responce.toUtf8()).object();
            if ( jsonObject.find("err")->toInt() == 0) text = "";
            else text = jsonObject.find("msg")->toString();
        }
        message->setText(text);
        message->setAttribute(Qt::WA_DeleteOnClose);
        message->show();
        this->close();

        emit refreshTree();
        clear_fields();
    }
}

void OpenWindow::on_buttonPath_clicked()
{
    QString Filename = QFileDialog::getExistingDirectory(
                this,tr("Open Path"),
                "C:\\"
                );
    ui->InputPath->setText(Filename);
}

