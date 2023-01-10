#pragma once

#include <QMessageBox>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>

#include "ui_CreateIndexTab.h"
#include "nb-samples.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CreateIndexTab; }
QT_END_NAMESPACE

class CreateIndexTab : public QWidget
{
    Q_OBJECT
public:
    explicit CreateIndexTab(QWidget* parent = nullptr);
    void SetCurrentDatabase(QString name, int port);
    ~CreateIndexTab();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void rmRow();

private:
    Ui::CreateIndexTab* ui;
    QString tableName_ = "";
    int port_ = 0;
};
