#pragma once

#include <QWidget>
#include <QString>
#include <QPushButton>
#include <QGridLayout>
#include <QTextEdit>
#include <QTableWidget>
#include <QComboBox>
#include <QMessageBox>

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

class TabWindow : public QWidget
{
    Q_OBJECT
public:
    explicit TabWindow(QWidget* parent = nullptr);

    ~TabWindow() = default;

public slots:
    void selectThousandQuery(QString table);

    QString textFromTextEdit();

    void setText(QString input);

    void setCurrentIndex(int index);

    void push_button_run_clicked();

private:
    bool check_query(NB_HANDLE connection);

    QString from_nbvalue(NBValue value);

    void exec_select_query(NB_HANDLE connection, QStandardItemModel* model, QString query);

public:
    QTableView* tableWidget_ = nullptr;
    QGridLayout* gridLayout_ = nullptr;
    QTextEdit* textEdit_ = nullptr;
    QComboBox* comboBox_ = nullptr;
    std::vector<std::string> input_queries_;
    std::vector<QStandardItemModel*> models_;
    QMenu* rightClickMenu_ = nullptr;
    QFont font;
    SyntaxHighlighter* syntaxHighLight_ = nullptr;


public:
    int dbPort_;
    QString queryFromFile_ = "";
};