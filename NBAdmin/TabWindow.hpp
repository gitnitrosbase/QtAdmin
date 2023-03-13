#pragma once

#include <QWidget>
#include <QString>
#include <QPushButton>
#include <QGridLayout>
#include <QTextEdit>
#include <QTableWidget>
#include <QComboBox>
#include <QMessageBox>
#include <QSplitter>
#include <QStatusBar>

#include <QTableView>
#include <QStandardItemModel>

#include <vector>
#include <string>
#include <thread>
#include <ctime>

#include "nb-samples.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

#include "SyntexHightlight.hpp"
#include "ResponceView.hpp"

#include "ui_TabWindow.h"
#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class TabWindow; }
QT_END_NAMESPACE

class TabWindow : public QWidget
{
    Q_OBJECT
public:
    explicit TabWindow(QWidget* parent = nullptr);

    ~TabWindow();

signals:
    void refresh_tree();

public slots:
    QString textFromTextEdit();

    void setText(QString input);

    void setCurrentIndex(int index);

    void push_button_run_clicked();

private:
    std::vector<std::string> getParsedQuery(std::string str);

    bool check_query(NB_HANDLE connection);

    QString from_nbvalue(NBValue value);

    QString exec_select_query(NB_HANDLE connection, QStandardItemModel* model, QString query);

public:
    QList<QString> input_queries_;
    QList<ResponceView*> models_;
    QStringList reqTypesList_;
    QMenu* rightClickMenu_ = nullptr;
    QFont font;
    SyntaxHighlighter* syntaxHighLight_ = nullptr;
    QList<QString> reqTypes_ = {"NONE", "SELECT", "INSERT", "UPDATE", "DELETE", "TRANSACTION", "ANOTHER"};
    Ui::TabWindow* ui;
    bool flag_ = true;
    int tabNumber_;
    QStatusBar* bar_;

public:
    int dbPort_;
    QString queryFromFile_ = "";
};
