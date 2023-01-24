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

#include <QTableView>
#include <QStandardItemModel>

#include <vector>
#include <string>

#include <ctime>

#include "nb-samples.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

#include "syntexHightlight.hpp"

//#include "ui_TabWindow.h"

//QT_BEGIN_NAMESPACE
//namespace Ui { class TabWindow; }
//QT_END_NAMESPACE

class TabWindow : public QWidget
{
    Q_OBJECT
public:
    explicit TabWindow(QWidget* parent = nullptr);

    ~TabWindow();

public slots:
    void selectThousandQuery(QString table);

    QString textFromTextEdit();

    void setText(QString input);

    void setCurrentIndex(int index);

    void push_button_run_clicked();

private:

    void cleanMemory(std::vector<QStandardItemModel*> models);

    bool check_query(NB_HANDLE connection);

    QString from_nbvalue(NBValue value);

    void exec_select_query(NB_HANDLE connection, QStandardItemModel* model, QString query);

public:
    QTableView* tableWidget_ = nullptr;
    QGridLayout* gridLayout_ = nullptr;
    QWidget* subWidget_ = nullptr;
    QGridLayout* subLayout_ = nullptr;
    QTextEdit* textEdit_ = nullptr;
    QComboBox* comboBox_ = nullptr;
    QSplitter* splitter_ = nullptr;
    std::vector<std::string> input_queries_;
    std::vector<QStandardItemModel*> models_;
    QMenu* rightClickMenu_ = nullptr;
    QFont font;
    SyntaxHighlighter* syntaxHighLight_ = nullptr;

    //Ui::TabWindow* ui;

public:
    int dbPort_;
    QString queryFromFile_ = "";
};
