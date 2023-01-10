#include "CreateIndexTab.hpp"

CreateIndexTab::CreateIndexTab(QWidget* parent) : QWidget(parent), ui(new Ui::CreateIndexTab)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList{"Column", ""});
    on_pushButton_clicked();
}
void CreateIndexTab::SetCurrentDatabase(QString name, int port)
{
    ui->label->setText("Create Index in " + name);
    tableName_ = name;
    port_ = port;
}
CreateIndexTab::~CreateIndexTab()
{
    delete ui;
}

void CreateIndexTab::on_pushButton_2_clicked()
{
    try
    {
        QString query = QString("CREATE INDEX %1 ON %2 (").arg(ui->lineEdit->text()).arg(tableName_);

        for (int i = 0; i< ui->tableWidget->rowCount(); i+=1)
        {
            QString columnIndex = dynamic_cast<QLineEdit*>(ui->tableWidget->cellWidget(i, 0))->text();
            query+=columnIndex;
            query+=",";
        }
        query.resize(query.count() - 1);
        query+=");";

        std::cout<<query.toStdString()<<std::endl;

        NB_HANDLE connection = nb_connect( u"127.0.0.1", port_, u"TESTUSER", u"1234" );
        nb_execute_sql(connection, query.toStdU16String().c_str(), query.count());
        check_error(connection);
        nb_disconnect(connection);
    }
    catch (...)
    {
        QMessageBox::warning(this, "Warning", "Please repeat your query");
    }
}

void CreateIndexTab::on_pushButton_clicked()
{
    QLineEdit* lineEdit = new QLineEdit();
    QPushButton* button = new QPushButton("X");

    connect(button, SIGNAL(clicked()), this, SLOT(rmRow()));

    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 0, lineEdit);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 1, button);
}

void CreateIndexTab::rmRow()
{
    if (ui->tableWidget->rowCount() > 0)
    {
        if (ui->tableWidget->rowCount() > 1) ui->tableWidget->removeRow(ui->tableWidget->currentRow());
    }
}
