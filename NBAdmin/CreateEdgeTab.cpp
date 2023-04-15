#include "CreateEdgeTab.hpp"
#include "MessageWindow.hpp"

CreateEdgeTab::CreateEdgeTab(QWidget* parent) : QWidget(parent), ui(new Ui::CreateEdgeTab)
{
    ui->setupUi(this);
}

CreateEdgeTab::~CreateEdgeTab()
{
    delete ui;
}

void CreateEdgeTab::SetCurrentDatabase(QString name, int port)
{
    ui->label->setText("Create Edge in " + name);
    tableName_ = name;
    port_ = port;
}

void CreateEdgeTab::SetTables(QStringList & tables)
{
    ui->pathFrom->clear();
    ui->pathTo->clear();

    ui->pathFrom->addItems(tables);
    ui->pathTo->addItems(tables);
}

void CreateEdgeTab::on_Create_clicked()
{
    QString name = ui->edgeName->text();

    if (name == "")
    {
        MessageWindow* message = new MessageWindow(this);
        message->setWindowTitle("Warning");
        message->setText(QString("Please, enter correct edge name!"));
        message->setAttribute(Qt::WA_DeleteOnClose);
        message->show();
        return;
    }
    else if (name.at(0).isNumber())
    {
        MessageWindow* message = new MessageWindow(this);
        message->setWindowTitle("Warning");
        message->setText(QString("Please, enter correct edge name!"));
        message->setAttribute(Qt::WA_DeleteOnClose);
        message->show();
        return;
    }
    else
    {
        auto nullIterAlp = alp_.find('!');
        for (auto symbol : name)
        {
            if (nullIterAlp == alp_.find(symbol.toLower().toLatin1()))
            {
                MessageWindow* message = new MessageWindow(this);
                message->setWindowTitle("Warning");
                message->setText(QString("Please, enter correct edge name!"));
                message->setAttribute(Qt::WA_DeleteOnClose);
                message->show();
                return;
            }
        }
    }

    QString query = QString("CREATE TABLE %1 AS EDGE %2 %3 ;").arg(ui->edgeName->text()).arg(ui->pathFrom->currentText()).arg(ui->pathTo->currentText());

    std::cout<<query.toStdString()<<std::endl;

    NB_HANDLE connection = nb_connect( u"127.0.0.1", port_, u"TESTUSER", u"1234" );
    nb_execute_sql(connection, query.toStdU16String().c_str(), query.size());
    if (!check_query(connection))
    {
        nb_disconnect(connection);
        return;
    }
    nb_disconnect(connection);
    this->close();
    emit refreshTree();
}


void CreateEdgeTab::on_Cancel_clicked()
{
    ui->edgeName->clear();
    ui->pathFrom->clear();
    ui->pathTo->clear();
    this->close();
}

bool CreateEdgeTab::check_query(NB_HANDLE connection)
{
    if (nb_errno(connection) == NB_OK) return true;
    else
    {
        MessageWindow* message = new MessageWindow(this);
        message->setWindowTitle("Warning");
        message->setText(QString(nb_err_text_utf8( connection )));
        message->setAttribute(Qt::WA_DeleteOnClose);
        message->show();
        std::cout << "ERROR: " << nb_errno( connection ) << ": " << nb_err_text_utf8( connection ) << std::endl;
        return false;
    }
}
