#include "TabWindow.hpp"

ConnectPool3 nbpool;


TabWindow::TabWindow(QWidget* parent) : QWidget(parent) ,ui(new Ui::TabWindow)
{
    ui->setupUi(this);

    QFont font;
    font.setPixelSize(16);
    ui->textEdit_->setFont(font);
    syntaxHighLight_ = new SyntaxHighlighter(ui->textEdit_->document());
    connect(ui->comboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){setCurrentIndex(index);});
}

TabWindow::~TabWindow()
{
    delete ui;

    for (auto item : models_)
    {
        delete item;
    }
    models_.clear();
}

QString TabWindow::textFromTextEdit()
{
    return ui->textEdit_->toPlainText();
}

void TabWindow::setText(QString input)
{
    ui->textEdit_->setText(input);
}

void TabWindow::setCurrentIndex(int index)
{
    if (!flag_) return;

    ui->tableWidget_->setModel(models_.at(index));
    if (models_.at(index)->columnCount() == 1 && models_.at(index)->rowCount() == 1)
    {
        ui->tableWidget_->resizeColumnToContents(0);
    }
}
void TabWindow::push_button_run_clicked()
{
    flag_ = false;

    int start = clock();

    input_queries_.clear();

    ui->comboBox_->clear();

    reqTypesList_.clear();


    // remove all models in answers

    for (auto item : models_)
    {
        delete item;
    }
    models_.clear();

    // fill models

    std::thread th1([=]()
    {
        int queryCount = ExecSqlASYNC2(tabNumber_ , dbPort_, ui->textEdit_->toPlainText().toStdString());

        for (int i = 0; i < GetCountAnswer(tabNumber_); i+=1)
        {
            std::cout<<"check"<<std::endl;
            ResponceView* model = new ResponceView();

            std::string err = GetError(tabNumber_, i);
            if (err != "") model->setError(err);

            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
            GetSqlResulsASYNC(tabNumber_, dbPort_, i);

            model->setQueryInfo(tabNumber_, i);
            models_.push_back(model);
            reqTypesList_.push_back(QString("Result %1: ").arg(i+1) + QString::fromStdString(GetQueryType(tabNumber_,i)));
            if ( GetQueryType(tabNumber_,i) == "ANOTHER")
            {
                refresh_tree();
            }
        }

    });
    th1.join();
    flag_ = true;

    // add input queries to comboBox
    ui->comboBox_->addItems(reqTypesList_);

    int end = clock();
    int t = (end - start);
    std::cout<<"\n\ntime -- "<<t<< " ms\n\n"<<std::endl;
}

bool TabWindow::check_query(NB_HANDLE connection)
{
    if (nb_errno(connection) == NB_OK) return true;
    else
    {
        std::cout << "ERROR: " << nb_errno( connection ) << ": " << nb_err_text_utf8( connection ) << std::endl;
        return false;
    }
}
QString TabWindow::from_nbvalue(NBValue v)
{
    std::string output = "";

    if (v.null == true) return QString("null");

    switch ( v.type )
    {
        case NB_DATA_INT: output = std::to_string(v.intv); break;
        case NB_DATA_DATETIME: for (int i = 0; i< v.len; i+=1) output += v.str[i]; break;
        case NB_DATA_STRING: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
        case NB_DATA_U16STRING: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
        case NB_DATA_DECIMAL: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
        case NB_DATA_INT64: output = std::to_string(v.int64v); break;
        case NB_DATA_DOUBLE: output = std::to_string(v.dbl); break;
        case NB_DATA_BOOL: output = ( ( v.intv ) ? "TRUE" : "FALSE" ); break;
        case NB_DATA_BINARY: output += *(&v.str); break;
        case NB_DATA_DATE: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
        case NB_DATA_NONE : output = "none"; break;
        case NB_DATA_ROWVERSION: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
        case NB_DATA_URI: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
    }
    return QString::fromStdString(output);
}

QString TabWindow::exec_select_query(NB_HANDLE connection, QStandardItemModel* model, QString query)
{
    QStringList headers = {};
    QList<QStandardItem*> tmp;
    nb_execute_sql(connection , query.toStdU16String().c_str(), size_t(query.size()));

    if (check_query(connection) == false)
    {
        tmp.push_back(new QStandardItem(QString::number(nb_errno( connection )) + ": " + QString(nb_err_text_utf8( connection ))));
        model->appendRow(tmp);
        return reqTypes_.at(0);
    }

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
    int reqType, tmp2;
    nb_get_change_info(connection, &reqType, &tmp2);
    return reqTypes_.at(reqType);
}
