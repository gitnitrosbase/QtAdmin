#pragma once

#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>

#include <windows.h>

class ConnectWindow : public QWidget
{
    Q_OBJECT
public:
    ConnectWindow(QWidget* parent = nullptr) : QWidget(parent)
    {
        gridLayout_ = new QGridLayout(this);
        dbName_ = new QLineEdit(this);
        dbPort_ = new QLineEdit(this);
        dbPath_ = new QLineEdit(this);
        pushButton_ = new QPushButton(this);
        gridLayout_->addWidget(dbName_);
        gridLayout_->addWidget(dbPort_);
        gridLayout_->addWidget(dbPath_);
        gridLayout_->addWidget(pushButton_);

        connect(pushButton_, SIGNAL(clicked()), this, SLOT(createDataBase()));
    }
    ~ConnectWindow() = default;
public slots:
    void createDataBase()
    {
        if (dbName_->text() != "" && dbPort_->text() != "")
        {
            QString str("nbase.exe ");
            str+=dbName_->text();
            str+=" -c create -p ";
            str+=dbPort_->text();
            if (dbPath_->text() == "")
            {
                system(str.toStdString().c_str());
            }
            else
            {
                str+=" < ";
                str+=dbPath_->text();
                system(str.toStdString().c_str());
            }
        }
    }

private:
    QLineEdit* dbName_ = nullptr;
    QLineEdit* dbPort_ = nullptr;
    QLineEdit* dbPath_ = nullptr;
    QPushButton* pushButton_ = nullptr;
    QGridLayout* gridLayout_ = nullptr;
};
