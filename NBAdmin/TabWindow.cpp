#include "TabWindow.hpp"

TabWindow::TabWindow(QWidget* parent) : QWidget(parent)
  //,ui(new Ui::TabWindow)
{
    //ui->setupUi(this);
    this->setObjectName("TabWindow");
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

    subLayout_->setSpacing(0);
    gridLayout_->setSpacing(0);

    subLayout_->setMargin(0);
    gridLayout_->setMargin(0);

    comboBox_->setStyleSheet(styles_);
    textEdit_->setStyleSheet(styles_);
    tableWidget_->setStyleSheet(styles_);

    textEdit_->setFrameStyle(0);
    tableWidget_->setFrameStyle(0);

    QFont font;
    font.setPixelSize(16);
    textEdit_->setFont(font);
    syntaxHighLight_ = new SyntaxHighlighter(textEdit_->document());
}

TabWindow::~TabWindow()
{
    //delete ui;

    for (auto item : models_)
    {
        delete item;
    }
    models_.clear();
}

void TabWindow::cleanMemory(std::vector<QStandardItemModel*> models)
{

}

QString TabWindow::textFromTextEdit()
{
    return textEdit_->toPlainText();
}

void TabWindow::setText(QString input)
{
    textEdit_->setText(input);
}

void TabWindow::setCurrentIndex(int index)
{
    tableWidget_->setModel(models_.at(index));
    if (models_.at(index)->columnCount() == 1 && models_.at(index)->rowCount() == 1)
    {
        tableWidget_->resizeColumnToContents(0);
    }
}
void TabWindow::push_button_run_clicked()
{
    int start = clock();

    input_queries_.clear();
    reqTypesList_.clear();

    delete comboBox_;
    comboBox_ = new QComboBox(this);
    subLayout_->addWidget(comboBox_, 0, 0);
    connect(comboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){setCurrentIndex(index);});

    comboBox_->setStyleSheet(styles_);

    std::string tmp;
    if (textEdit_->textCursor().selectedText().count() == 0) tmp = textEdit_->toPlainText().toStdString();
    else tmp = textEdit_->textCursor().selectedText().toStdString();
    if (tmp.back() != ';') tmp.push_back(';');
    std::string::size_type beg = 0;
    for (auto end = 0; (end = tmp.find(';', end)) != std::string::npos; ++end)
    {
        std::string str = tmp.substr(beg, end - beg);
        input_queries_.push_back(QString::fromStdString(str));
        beg = end + 1;
    }

    for (auto &item : input_queries_)
    {
        while (item.at(0) == '\n' ||  item.at(0) == '\t' || item.at(0) == ' ')
        {
            item.remove(0,1);
        }
    }

    // remove all models in answers

    for (auto item : models_)
    {
        delete item;
    }
    models_.clear();

    // create new empty models
    for (auto &item : input_queries_)
    {
        models_.push_back(new QStandardItemModel);
    }

    // fill empty models
    if (models_.count() == input_queries_.count())
    {
        std::thread th1([=]()
        {
            for (int i = 0; i < models_.count(); i+=1)
            {
                NB_HANDLE connection = nb_connect( u"127.0.0.1", dbPort_, u"TESTUSER", u"1234" );
                check_query(connection);
                reqTypesList_.push_back(exec_select_query(connection, models_.at(i), input_queries_.at(i)));
                if (reqTypesList_.back() == "ANOTHER") emit refresh_tree();
                check_query(connection);
                nb_disconnect(connection);
            }
        });

        th1.join();
    }

    // add input queries to comboBox
    comboBox_->addItems(reqTypesList_);

    int end = clock();
    int t = (end - start) / CLOCKS_PER_SEC;
    std::cout<<t<<std::endl;
}

//QList<QStandardItemModel*> TabWindow::get_models_from_db(QList<QString> &input_queries)
//{
//    QList<QStandardItemModel*> output;

//    for (int i = 0; i < input_queries; i+=1)
//    {
//        NB_HANDLE connection = nb_connect( u"127.0.0.1", dbPort_, u"TESTUSER", u"1234" );
//        check_query(connection);
//        reqTypesList.push_back(exec_select_query(connection, models_.at(i), input_queries_.at(i)));
//        check_query(connection);
//        nb_disconnect(connection);
//    }
//    return output;
//}

bool TabWindow::check_query(NB_HANDLE connection)
{
    if (nb_errno(connection) == NB_OK) return true;
    else
    {
        //QMessageBox::warning(this, "WARNING", nb_err_text_utf8( connection ));
        //std::cout << "ERROR: " << nb_errno( connection ) << ": " << nb_err_text_utf8( connection ) << std::endl;
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
