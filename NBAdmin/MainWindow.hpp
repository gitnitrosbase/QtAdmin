#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QString>
#include <QPushButton>
#include <QTextBrowser>
#include <QGridLayout>
#include <QTextEdit>
#include <QTreeWidget>
#include <QMessageBox>
#include <QDir>
#include <QDirIterator>
#include <QToolBar>
#include <QMainWindow>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QList>
#include <QTimer>

#include <QRegularExpression>
#include <QSyntaxHighlighter>

#include <vector>
#include <iostream>
#include <utility>
#include <algorithm>
#include <map>
#include <typeinfo>

#include "TabWindow.hpp"
#include "ConnectWindow.hpp"
#include "BackupWindow.hpp"
#include "OpenWindow.hpp"
#include "RestoreWindow.hpp"
#include "CreateTableTab.hpp"
#include "CreateEdgeTab.hpp"
#include "CreateIndexTab.hpp"
#include "ModifyTableTab.hpp"
#include "InfoWindow.hpp"

#include "ui_MainWindow.h"

#include <windows.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() = default;

private slots:

    void filling_tree_slot();

    void showContextMenu(const QPoint point);

    void push_button_run_clicked();

    void push_button_plus_clicked();

    void on_tabWidget_tabCloseRequested(int index);

    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem*);

    void on_actionCreateDBQueryTrig();

    void on_actionSelectEdgeTrig();

    void on_actionDeleteEdgeTrig();

    void on_actionDeleteIndexTrig();

    void on_deleteTableActionTrig();

    void on_modifyTableActionTrig();

    void on_tableSelectActionTrig();

    void on_actionCreateEdgeTrig();

    void on_actionCreateIndexTrig();

    void on_actionCreateTableTrig();

    void on_actionDeleteDatabaseTrig();

//----------------------------------

    void on_on_actionCreateTableTrig_triggered();

    void on_actionOpen_database_triggered();

    void on_actionRefresh_triggered();

    void on_actionStop_triggered();

    void on_actionStart_triggered();

    void on_actionBackup_triggered();

    void on_actionRestore_triggered();

    void on_actionDatabase_Info_triggered();

    void on_actionNew_query_triggered();

    void on_actionClose_query_triggered();

    void on_actionRun_query_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionInfo_triggered();

    void on_actionContacts_triggered();

    void on_treeWidget_itemExpanded(QTreeWidgetItem *item);

private:
    void setAddress();

    QString paintText(QString str, int color);

    QString nullCheck(int index);

    QString linkCheck(QString input);

    QString runCheck(bool input);

    void filling_tree();

private:
    QString userName_ = "";
    QString address_ = "http://127.0.0.1:8008/api3";
    std::map<QString, int> dbList_;
    Ui::MainWindow *ui;
    QString currentDatabase_ = "";
    ConnectWindow* connectWindow_ = nullptr;
    InfoWindow* infoWindow_;
    OpenWindow* openWindow_ = nullptr;
    BackupWindow* backupWindow_ = nullptr;
    RestoreWindow* restoreWindow_ = nullptr;
    QMenu* rightClickMenu_ = nullptr;
    std::map<int, QString> fieldsTypes_ = {
        {1, "varchar"},
        {2, "int"},
        {3, "bigint"},
        {4, "double"},
        {5, "datetime"},
        {6, "bit"},
        {7, "date"},
        {9, "varbinary"},
        {10, "nvarchar"},
        {11, "rowversion"},
        {12, "decimal"}
    };

    QMenu * menu_ = nullptr;
    QAction * refreshAction_ = nullptr;
    QAction * stopAction_ = nullptr;
    QAction * startAction_ = nullptr;
    QAction * backupAction_ = nullptr;
    QAction * restoreAction_ = nullptr;
    QAction * deleteAction_ = nullptr;
    QAction * databaseInfoAction_ = nullptr;
    QAction * createTableAction_ = nullptr;
    QAction * createEdgeAction_ = nullptr;
    QAction * createIndexAction_ = nullptr;
    QAction * selectAction_ = nullptr;
    QAction * modifyTableAction_ = nullptr;
    QAction * deleteTableAction_ = nullptr;
    QAction * deleteEdgeAction_ = nullptr;
    QAction * deleteIndexAction_ = nullptr;
    QAction * selectEdgeAction_ = nullptr;
    QAction * createDBQueryAction_ = nullptr;

    QList<QTreeWidgetItem*> tables_;
};
