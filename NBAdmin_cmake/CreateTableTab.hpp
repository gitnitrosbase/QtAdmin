#pragma once

#include <QWidget>

#include <QTableWidget>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
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
        ui->tableWidget->setHorizontalHeaderLabels(headerTable);
    }
    ~CreateTableTab()
    {
        delete ui;
    }

    void SetCurrentDatabase(QString& name)
    {
        ui->label->setText("Create Table in " + name);
    }
private:
    QStringList headerTable = {"Name", "Type", "PK", "FK", "FK table", "Identity", "NOT NULL" , ""};
    Ui::CreateTableTab* ui;
public:
    QString currentDatabase_ = "";
};
