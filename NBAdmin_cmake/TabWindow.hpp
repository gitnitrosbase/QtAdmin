#pragma once

#include <QWidget>
#include <QString>
#include <QPushButton>
#include <QGridLayout>
#include <QTextEdit>
#include <QTableWidget>
#include <QComboBox>
#include <QMessageBox>

#include <vector>
#include <string>

#include "nb-samples.h"

class TabWindow : public QWidget
{
    Q_OBJECT
public:
    explicit TabWindow(QWidget* parent = nullptr) : QWidget(parent)
    {
        gridLayout_ = new QGridLayout(this);
        textEdit_ = new QTextEdit(this);
        tableWidget_ = new QTableWidget(this);
        comboBox_ = new QComboBox(this);
        gridLayout_->addWidget(textEdit_, 0, 0);
        gridLayout_->addWidget(comboBox_, 1, 0);
        gridLayout_->addWidget(tableWidget_, 2, 0);
        connect(comboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){setCurrentIndex(index);});
    }
    ~TabWindow() = default;

public slots:

    void setCurrentIndex(int index)
    {
        print_result_to_table(responces_.at(index), *tableWidget_);
    }
    void push_button_run_clicked()
    {
//        std::cout<<"check1"<<std::endl;
//        for (int i = 0;i<comboBox_->count();i+=1 )
//        {
//            comboBox_->removeItem(i);
//        }
//        std::cout<<"check2"<<std::endl;
//        input_queries_.clear();
//        std::cout<<"check3"<<std::endl;
//        responces_.clear();
//        std::cout<<"check4"<<std::endl;
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
            responces_.push_back(exec_select_query(connection, QString::fromStdString(input_queries_.at(i))));
            nb_disconnect(connection);
            comboBox_->addItem(QString::fromStdString(input_queries_.at(i)));
        }
        std::cout<<"==="<<std::endl;
        for (auto item : input_queries_)
        {
            std::cout<<item<<std::endl;
        }
        std::cout<<"==="<<std::endl;
        std::cout<<comboBox_->count()<<std::endl;
    }

private:

    void print_result_to_table(std::vector<std::vector<std::string> > output, QTableWidget& tableWidget)
    {
        tableWidget.setColumnCount(output[0].size());
        tableWidget.setRowCount(output.size()-1);
        QStringList header_list;
        for (int i = 0;i<int(output[0].size());i+=1)
        {
            header_list.push_back(QString::fromStdString(output[0][i]));
        }
        tableWidget.setHorizontalHeaderLabels(header_list);
        for (unsigned long long i = 1;i<output.size();i+=1)
        {
            for (int j = 0;j<int(output[i].size());j+=1)
            {
                tableWidget.setItem(i-1,j, new QTableWidgetItem(QString::fromStdString(output[i][j])));
            }
        }

    }
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
    std::vector<std::vector<std::string> > exec_select_query(NB_HANDLE connection, QString query)
    {
        std::vector<std::vector<std::string> > output;
        std::vector<std::string> tmp;
        nb_execute_sql(connection , query.toStdU16String().c_str(), size_t(query.size()));
        check_query(connection);
        int field_count = nb_field_count(connection);
        for (int i = 0;i< field_count; i+=1)
        {
            NBValue name;
            nb_field_name(connection, i, &name);
            tmp.push_back(from_nbvalue(name).toStdString());
        }
        output.push_back(tmp);
        int row_count = 0;
        while (nb_fetch_row(connection) == NB_OK)
        {
            tmp = {};
            NBValue value;
            row_count+=1;
            for (int column_i = 0; column_i<field_count; column_i+=1)
            {
                nb_field_value_utf8(connection, column_i, &value);
                tmp.push_back(from_nbvalue(value).toStdString());
            }
            output.push_back(tmp);
        }
        return output;
    }

private:
    QTableWidget* tableWidget_ = nullptr;
    QGridLayout* gridLayout_ = nullptr;
    QTextEdit* textEdit_ = nullptr;
    QComboBox* comboBox_ = nullptr;

    std::vector<std::string> input_queries_;
    std::vector<std::vector<std::vector<std::string> > > responces_;
public:
    int dbPort_;
};
