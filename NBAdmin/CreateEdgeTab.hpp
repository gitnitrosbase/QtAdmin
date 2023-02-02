#pragma once

#include <QMessageBox>
#include <QWidget>
#include <QStringList>
#include <set>

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

signals:
    void refreshTree();

private slots:
    void on_Create_clicked();
    void on_Cancel_clicked();
    bool check_query(NB_HANDLE connection);

private:
    Ui::CreateEdgeTab* ui;
    QString tableName_ = "";
    int port_ = 0;
    QStringList tables_ = {};
    std::set<char> alp_ = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9','0','_','-'};

};
