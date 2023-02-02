#pragma once

#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileDialog>

#include <iostream>
#include <set>

#include <ui_OpenWindow.h>
QT_BEGIN_NAMESPACE
namespace Ui { class OpenWindow; }
QT_END_NAMESPACE

class OpenWindow : public QWidget
{
    Q_OBJECT
public:
    OpenWindow(QWidget* parent = nullptr);

    ~OpenWindow() = default;
signals:
    void refreshTree();

public slots:
    void cancel_clicked();

    void OpenDatabase();

private slots:
    void on_buttonPath_clicked();

public:
    QString address_ = "http://127.0.0.1:8008/api3";
    std::set<char> alp_ = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9','0','_','-'};
    Ui::OpenWindow* ui;
};
