#pragma once

#include <QWidget>

#include <QTableWidget>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QComboBox>
#include <QStringList>

#include "ui_CreateTableTab.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CreateTableTab; }
QT_END_NAMESPACE

class CreateTableTab : public QWidget
{
    Q_OBJECT
public:
    explicit CreateTableTab(QWidget* parent = nullptr) : QWidget(parent), ui(new Ui::CreateTableTab)
    {
        ui->setupUi(this);

//        addRowButton_ = new QPushButton(this);
//        rmRowButton_ = new QPushButton(this);



        ui->tableWidget->setColumnCount(headerTable.count());
        ui->tableWidget->setHorizontalHeaderLabels(headerTable);

        addRow();

        connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(addRow()));
    }
    ~CreateTableTab()
    {
        delete ui;
    }


    void SetCurrentDatabase(QString& name)
    {
        ui->label->setText("Create Table in " + name);
    }

public slots:
    void addRow()
    {
        QComboBox* typesComboBox = new QComboBox();
        QComboBox* FKTableComboBox = new QComboBox();

        for (auto item : fieldsTypes_)
        {
            typesComboBox->addItem(item);
        }



        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 1, typesComboBox);
        ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 4, FKTableComboBox);
    }

private:
    QStringList headerTable = {"Name", "Type", "PK", "FK", "FK table", "Identity", "NOT NULL" , ""};
    Ui::CreateTableTab* ui;
public:
    QString currentDatabase_ = "";
    QPushButton* addRowButton_ = nullptr;
    QPushButton* rmRowButton_ = nullptr;

    std::vector<QString> fieldsTypes_ = {
        "varchar",
        "int",
        "bigint",
        "double",
        "datetime",
        "bit",
        "date",
        "varbinary",
        "nvarchar",
        "rowversion",
        "decimal"
    };
};
