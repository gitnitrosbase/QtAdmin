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
#include <QStatusBar>
#include <QClipboard>
#include <QMimeData>
#include <QLocale>

#include <QTableView>
#include <QStandardItemModel>

#include <vector>
#include <string>
#include <thread>
#include <ctime>
#include <fstream>

#include "nb-samples.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

#include "Qsci/qsciscintilla.h"
#include "Qsci/qscilexersql.h"

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

    void keyPressEvent(QKeyEvent *event) override;

public:
    QList<QString> input_queries_;
    QList<ResponceView*> models_;
    QStringList reqTypesList_;
    QMenu* rightClickMenu_ = nullptr;
    QFont font;
    QList<QString> reqTypes_ = {"NONE", "SELECT", "INSERT", "UPDATE", "DELETE", "TRANSACTION", "ANOTHER"};
    Ui::TabWindow* ui;
    bool flag_ = true;
    int tabNumber_ = 1;
    QStatusBar* bar_;
    QsciScintilla* textEdit_ = nullptr;
    QsciLexerSQL* sqlLexer_ = nullptr;
    NB_HANDLE* connection_ = nullptr;

public:
    int dbPort_;
    QString queryFromFile_ = "";
};
