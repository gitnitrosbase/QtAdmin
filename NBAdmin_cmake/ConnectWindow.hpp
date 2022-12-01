#pragma once

#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>

#include <iostream>
#include <windows.h>
#include <lmcons.h>

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

        setWindowIcon(QIcon(":/images/favicon.ico"));
    }
    ~ConnectWindow() = default;
public slots:
    QString fromTCHAR(TCHAR* name, int size)
    {
        QString output;
        for (int i = 0; i< size; i+=1)
        {
            output+=name[i];
        }
        return output;
    }
    // пока не работает(не выполняется команда виндовс)
    void createDataBase()
    {
        TCHAR name [ UNLEN + 1 ];
        DWORD size = UNLEN + 1;
        QString qname;

        GetUserName((TCHAR*)name, &size);
        qname = fromTCHAR(name, size);

        if (dbName_->text() != "" && dbPort_->text() != "")
        {
            QString str;
            str = "cd C:/Users/";
            str+=qname;
            str+="/AppData/Local/nitrosbaseuni/bin/ ; ./nbase.exe ";
            str+=dbName_->text();
            str+=" -c create -p ";
            str+=dbPort_->text();
            std::cout<<str.toStdString()<<std::endl;
            if (dbPath_->text() != "")
            {
                str+=" < ";
                str+=dbPath_->text();
                system(str.toStdString().c_str());
            }
            str+=" > res.txt";
            system(str.toStdString().c_str());
        }
        this->close();
    }

private:
    QLineEdit* dbName_ = nullptr;
    QLineEdit* dbPort_ = nullptr;
    QLineEdit* dbPath_ = nullptr;
    QPushButton* pushButton_ = nullptr;
    QGridLayout* gridLayout_ = nullptr;
};
