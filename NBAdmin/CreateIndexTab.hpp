#pragma once

#include <QMessageBox>
#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QStringList>
#include <QFile>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

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
    QString address_ = "http://127.0.0.1:8008/api3";
    QStringList tableList_ = {};
};
