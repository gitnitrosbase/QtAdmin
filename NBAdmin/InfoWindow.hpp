#pragma once

#include <QWidget>

#include "ui_InfoWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class InfoWindow; }
QT_END_NAMESPACE

class InfoWindow : public QWidget
{
    Q_OBJECT
public:
    InfoWindow(QWidget* parent = nullptr) : QWidget(parent), ui(new Ui::InfoWindow)
    {
        ui->setupUi(this);
    }
    ~InfoWindow()
    {
        delete ui;
    }
    void setInformation(QString name, int port, QString version)
    {
        ui->txtName->setText(name);
        ui->portNumber->setText(QString::number(port));
        ui->dbVersionTxt->setText(version);
    }
private slots:
    void on_infoOk_clicked()
    {
        this->close();
    }

private:
    Ui::InfoWindow* ui;
};
