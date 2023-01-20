#pragma once

#include <QMessageBox>
#include <QWidget>
#include <QStringList>

#include "ui_CreateEdgeTab.h"
#include "nb-samples.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CreateEdgeTab; }
QT_END_NAMESPACE

class CreateEdgeTab : public QWidget
{
    Q_OBJECT
public:
    explicit CreateEdgeTab(QWidget* parent = nullptr);

    ~CreateEdgeTab();

    void SetCurrentDatabase(QString name, int port);

    void SetTables(QStringList & tables);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::CreateEdgeTab* ui;
    QString tableName_ = "";
    int port_ = 0;
    QStringList tables_ = {};
};