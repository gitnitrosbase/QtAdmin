#include "TabWindow.hpp"

TabWindow::TabWindow(QWidget* parent) : QWidget(parent)
{
    gridLayout_ = new QGridLayout(this);
    splitter_ = new QSplitter(Qt::Vertical, this);
    textEdit_ = new QTextEdit(this);
    subWidget_ = new QWidget(this);
    comboBox_ = new QComboBox(this);
    tableWidget_ = new QTableView(this);
    subLayout_ = new QGridLayout();

    subWidget_->setLayout(subLayout_);
    subLayout_->addWidget(comboBox_, 0, 0);
    subLayout_->addWidget(tableWidget_, 1, 0);

    splitter_->addWidget(textEdit_);
    splitter_->addWidget(subWidget_);

    gridLayout_->addWidget(splitter_, 0, 0);


    QFont font;
    font.setPixelSize(16);
    textEdit_->setFont(font);
    syntaxHighLight_ = new SyntaxHighlighter(textEdit_->document());
}

void TabWindow::selectThousandQuery(QString table)
{
    NB_HANDLE connection;
}

QString TabWindow::textFromTextEdit()
{
    return textEdit_->toPlainText();
}

void TabWindow::setText(QString input)
{
    std::cout<<input.toStdString()<<std::endl;
    textEdit_->setText(input);
}

void TabWindow::setCurrentIndex(int index)
{
    std::cout<<"start set current"<<std::endl;
    tableWidget_->setModel(models_.at(index));
}
void TabWindow::push_button_run_clicked()
{
    int start = clock();
    std::cout<<"start click"<<std::endl;

    for (auto item : models_) delete item;
    models_.clear();


    input_queries_.clear();

    delete comboBox_;
    comboBox_ = new QComboBox(this);
    gridLayout_->addWidget(comboBox_, 1, 0);
    connect(comboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){setCurrentIndex(index);});

    std::string tmp = textEdit_->toPlainText().toStdString();

    std::string::size_type beg = 0;
            for (auto end = 0; (end = tmp.find(';', end)) != std::string::npos; ++end)
            {
                input_queries_.push_back(tmp.substr(beg, end - beg));
                beg = end + 1;
            }

            input_queries_.push_back(tmp.substr(beg));


    for (int i = 0; i<int(input_queries_.size());i+=1)
    {
        models_.push_back(new QStandardItemModel());

        NB_HANDLE connection = nb_connect( u"127.0.0.1", dbPort_, u"TESTUSER", u"1234" );
        check_error(connection);
        exec_select_query(connection, models_.back(), QString::fromStdString(input_queries_.at(i)));
        nb_disconnect(connection);
        comboBox_->addItem(QString::fromStdString(input_queries_.at(i)));
    }
    std::cout<<"end click"<<std::endl;

    int end = clock();
    int t = (end - start) / CLOCKS_PER_SEC;
    std::cout<<t<<std::endl;
}

bool TabWindow::check_query(NB_HANDLE connection)
{
    if (nb_errno(connection) == NB_OK) return true;
    else
    {
        QMessageBox::warning(this, "WARNING", nb_err_text_utf8( connection ));
        std::cout << "ERROR: " << nb_errno( connection ) << ": " << nb_err_text_utf8( connection ) << std::endl;
        return false;
    }
}
QString TabWindow::from_nbvalue(NBValue value)
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

void TabWindow::exec_select_query(NB_HANDLE connection, QStandardItemModel* model, QString query)
{
    QStringList headers = {};
    QList<QStandardItem*> tmp;
    std::cout<<"start exec"<<std::endl;
    nb_execute_sql(connection , query.toStdU16String().c_str(), size_t(query.size()));
    check_query(connection);
    int field_count = nb_field_count(connection);
    for (int i = 0;i< field_count; i+=1)
    {
        NBValue name;
        nb_field_name(connection, i, &name);
        tmp.push_back(new QStandardItem(from_nbvalue(name)));
        headers += from_nbvalue(name);
    }

    model->setColumnCount(field_count);
    model->setHorizontalHeaderLabels(headers);

    int row_count = 0;
    while (nb_fetch_row(connection) == NB_OK)
    {
        tmp = {};
        NBValue value;
        row_count+=1;
        for (int column_i = 0; column_i<field_count; column_i+=1)
        {
            nb_field_value_utf8(connection, column_i, &value);
            tmp.push_back(new QStandardItem(from_nbvalue(value)));
        }
        model->appendRow(tmp);
    }
    std::cout<<"end exec"<<std::endl;
}
