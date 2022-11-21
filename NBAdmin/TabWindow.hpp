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
        pushButton_run_ = new QPushButton("Run ->", this);
        textEdit_ = new QTextEdit(this);
        tableWidget_ = new QTableWidget(this);
        comboBox_ = new QComboBox(this);

        gridLayout_->addWidget(textEdit_, 0, 0, 1, 0);
        gridLayout_->addWidget(pushButton_run_, 1, 0);
        gridLayout_->addWidget(comboBox_, 1, 1);
        gridLayout_->addWidget(tableWidget_, 2, 0,1,0);

        pushButton_run_->setMaximumWidth(60);

        connect(pushButton_run_, SIGNAL(clicked()), this, SLOT(push_button_run_clicked()));
        connect(comboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){ setCurrentIndex1(index); });
    }
    ~TabWindow()
    {
    }

private slots:

    void setCurrentIndex1(int index)
    {
        print_result_to_table(responces_.at(index), *tableWidget_);
    }
    void push_button_run_clicked()
    {
        QString tmp = textEdit_->toPlainText();
        input_queries_.push_back("");
        for (int i = 0; i<tmp.size();i+=1)
        {
            if (tmp.at(i) != ";") input_queries_.back()+=tmp.at(i);
            else {input_queries_.push_back(""); if (tmp.at(i) == "\n" || tmp.at(i) == "\t" || tmp.at(i) == " " || tmp.at(i) == tmp.at(tmp.size()-1)) i+=1;}
        }
        for (int i = 0; i<int(input_queries_.size());i+=1)
        {
            NB_HANDLE connection = nb_connect( u"127.0.0.1", 3020, u"TESTUSER", u"1234" );
            check_error(connection);
            responces_.push_back(exec_select_query(connection, input_queries_.at(i)));
            nb_disconnect(connection);
            comboBox_->addItem(input_queries_.at(i));
        }
    }


private:

    void print_result_to_table(std::vector<std::vector<std::string> > output, QTableWidget& tableWidget)
    {
        tableWidget.setColumnCount(output[0].size());
        tableWidget.setRowCount(output.size());
        QStringList header_list;
        for (int i = 0;i<int(output[0].size());i+=1)
        {
            header_list.push_back(QString::fromStdString(output[0][i]));
        }
        tableWidget.setHorizontalHeaderLabels(header_list);
        output.pop_back();
        for (unsigned long long i = 0;i<output.size();i+=1)
        {
            for (int j = 0;j<int(output[i].size());j+=1)
            {
                tableWidget.setItem(i,j, new QTableWidgetItem(QString::fromStdString(output[i][j])));
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

    std::string from_nbvalue(NBValue value)
    {
        std::string output = "";

        if (value.type == NB_DATA_U16STRING)
        {
            for (int i = 0; i < value.len; i += 1) output += value.str[i];
        }
        else if (value.type == NB_DATA_INT64)
        {
            output += value.int64v;
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
            tmp.push_back(from_nbvalue(name));
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
                tmp.push_back(from_nbvalue(value));
            }
            output.push_back(tmp);
        }
        return output;
    }

private:
    QTableWidget* tableWidget_ = nullptr;
    QPushButton* pushButton_run_ = nullptr;
    QGridLayout* gridLayout_ = nullptr;
    QTextEdit* textEdit_ = nullptr;
    QComboBox* comboBox_ = nullptr;

    std::vector<QString> input_queries_;
    std::vector<std::vector<std::vector<std::string> > > responces_;
};
