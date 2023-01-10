#pragma once

#include <QWidget>

#include "ui_ModifyTableTab.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ModifyTableTab; }
QT_END_NAMESPACE

class ModifyTableTab : public QWidget
{
    Q_OBJECT
public:
    explicit ModifyTableTab(QWidget *parent = nullptr);
    ~ModifyTableTab();

public slots:

public:
    int port_ = 0;
    QString currentTable_ = "";
    QString query_ = "";
private:
    Ui::ModifyTableTab* ui;
};
