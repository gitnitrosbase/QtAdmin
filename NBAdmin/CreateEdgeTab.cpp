#include "CreateEdgeTab.hpp"

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
    QString query = QString("CREATE TABLE %1 AS EDGE %2 %3 ;").arg(ui->edgeName->text()).arg(ui->pathFrom->currentText()).arg(ui->pathTo->currentText());

    std::cout<<query.toStdString()<<std::endl;

    NB_HANDLE connection = nb_connect( u"127.0.0.1", port_, u"TESTUSER", u"1234" );
    nb_execute_sql(connection, query.toStdU16String().c_str(), query.count());
    check_error(connection);
    nb_disconnect(connection);

    this->close();
}


void CreateEdgeTab::on_Cancel_clicked()
{
    ui->edgeName->clear();
    ui->pathFrom->clear();
    ui->pathTo->clear();
    this->close();
}

