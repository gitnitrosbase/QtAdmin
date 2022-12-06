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

#include <vector>
#include <string>

#include <ctime>

#include "nb-samples.h"
#include "ResponceView.hpp"

class TabWindow : public QWidget
{
    Q_OBJECT
public:
    explicit TabWindow(QWidget* parent = nullptr) : QWidget(parent)
    {
        gridLayout_ = new QGridLayout(this);
        textEdit_ = new QTextEdit(this);
        tableWidget_ = new QTableView(this);
        comboBox_ = new QComboBox(this);
        gridLayout_->addWidget(textEdit_, 0, 0);
        gridLayout_->addWidget(comboBox_, 1, 0);
        gridLayout_->addWidget(tableWidget_, 2, 0);
    }
    ~TabWindow() = default;

public slots:
    void setCurrentIndex(int index)
    {
        std::cout<<"start set current"<<std::endl;
        tableWidget_->setModel(currentModel_);
        std::cout<<"end set current"<<index<<std::endl;
    }
    void push_button_run_clicked()
    {
        int start = clock();

        std::cout<<"start click"<<std::endl;
        input_queries_.clear();
        responces_.clear();
        delete comboBox_;
        comboBox_ = new QComboBox(this);
        gridLayout_->addWidget(comboBox_, 1, 0);
        connect(comboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){setCurrentIndex(index);});

        QString tmp = textEdit_->toPlainText();
        QString query;
        if (tmp.back() != ";") tmp.append(";");
        for (int i = 0; i<tmp.size();i+=1)
        {
            if (tmp.at(i) != ";") query += tmp.at(i);
            else
            {
                input_queries_.push_back(query.toStdString());
                query = "";
                while(tmp.at(i+1) == " " || tmp.at(i+1) == "\t" || tmp.at(i+1) == "\n") i+=1;
            }
        }
        for (int i = 0; i<int(input_queries_.size());i+=1)
        {
            NB_HANDLE connection = nb_connect( u"127.0.0.1", dbPort_, u"TESTUSER", u"1234" );
            check_error(connection);
            exec_select_query(connection, QString::fromStdString(input_queries_.at(i)));
            nb_disconnect(connection);
            comboBox_->addItem(QString::fromStdString(input_queries_.at(i)));
        }
        std::cout<<"end click"<<std::endl;

        int end = clock();
        int t = (end - start) / CLOCKS_PER_SEC;
        std::cout<<t<<std::endl;
    }

private:

//    void print_to_model(QStandardItemModel* model, std::vector<std::vector<std::string> > &input)
//    {
//        std::cout<<"start print"<<std::endl;
////        for (int i = 0; i<input.size(); i+=1)
////        {
////            QList<QStandardItem*> list;
////            for (int j = 0; j<input.at(0).size(); j+=1)
////            {
////                QStandardItem* item = new QStandardItem(i,j);
////                item->setText(QString::fromStdString(input.at(i).at(j)));
////                list.append(item);

////            }
////            model->appendRow(list);
////        }

//        if (currentModel_ != nullptr) delete currentModel_;
//        currentModel_ = new ResponceView();
//        tableWidget_->setModel(currentModel_);


//        std::cout<<"end print"<<std::endl;
//    }

    bool check_query(NB_HANDLE connection)
    {
        if (nb_errno(connection) == NB_OK) return true;
        else
        {
            QMessageBox::warning(this, "WARNING", nb_err_text_utf8( connection ));
            std::cout << "ERROR: " << nb_errno( connection ) << ": " << nb_err_text_utf8( connection ) << std::endl;
            return false;
        }
    }
    QString from_nbvalue(NBValue value)
    {
        QString output = "";
        if (value.type == NB_DATA_U16STRING)
        {
            for (int i = 0; i < value.len; i += 1) output += value.str[i];
        }
        else if (value.type == NB_DATA_INT64)
        {
            output += QString::number(value.int64v);
        }
        else if (value.type == NB_DATA_INT)
        {
            output+= QString::number(value.intv);
        }
        return output;
    }

    void exec_select_query(NB_HANDLE connection, QString query)
    {
        std::vector<QString> tmp;
        std::cout<<"start exec"<<std::endl;
        nb_execute_sql(connection , query.toStdU16String().c_str(), size_t(query.size()));
        check_query(connection);
        int field_count = nb_field_count(connection);
        for (int i = 0;i< field_count; i+=1)
        {
            NBValue name;
            nb_field_name(connection, i, &name);
            tmp.push_back(from_nbvalue(name));
        }

        std::cout<<"write header"<<std::endl;

        currentModel_->setSize(field_count);
        currentModel_->addRow(tmp);

        std::cout<<"write header"<<std::endl;



        int row_count = 0;
        while (nb_fetch_row(connection) == NB_OK)
        {
            tmp = {};
            NBValue value;
            row_count+=1;
            for (int column_i = 0; column_i<field_count; column_i+=1)
            {
                nb_field_value_utf8(connection, column_i, &value);
                tmp.push_back(from_nbvalue(value));
            }
            currentModel_->addRow(tmp);
        }
        std::cout<<"end exec"<<std::endl;
    }

private:
    QTableView* tableWidget_ = nullptr;
    QGridLayout* gridLayout_ = nullptr;
    QTextEdit* textEdit_ = nullptr;
    QComboBox* comboBox_ = nullptr;

    std::vector<std::string> input_queries_;
    std::vector<std::vector<std::vector<std::string> > > responces_;

    ResponceView* currentModel_ = nullptr;
public:
    int dbPort_;
};
