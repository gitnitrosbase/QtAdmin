#include "TabWindow.hpp"

ConnectPool3 nbpool;


TabWindow::TabWindow(QWidget* parent) : QWidget(parent) ,ui(new Ui::TabWindow)
{
    bar_ = new QStatusBar(this);
    ui->setupUi(this);

    textEdit_ = new QsciScintilla;
    sqlLexer_ = new QsciLexerSQL;
    textEdit_->setUtf8(true);
    ui->verticalLayout_->insertWidget(0, textEdit_);
    textEdit_->setLexer(sqlLexer_);
    //! Текущая строка кода и ее подсветка
    textEdit_->setCaretLineVisible(true);
    textEdit_->setCaretLineBackgroundColor(QColor("gainsboro"));
    //! Выравнивание
    textEdit_->setAutoIndent(true);
    textEdit_->setIndentationGuides(false);
    textEdit_->setIndentationsUseTabs(true);
    textEdit_->setIndentationWidth(4);
    //! margin это полоска слева, на которой обычно распологаются breakpoints
    textEdit_->setMarginsBackgroundColor(QColor("gainsboro"));
    textEdit_->setMarginLineNumbers(1, true);
    textEdit_->setMarginWidth(1, 50);
    //! Авто-дополнение кода в зависимости от источника
    textEdit_->setAutoCompletionSource(QsciScintilla::AcsAll);
    textEdit_->setAutoCompletionCaseSensitivity(true);
    textEdit_->setAutoCompletionReplaceWord(true);
    textEdit_->setAutoCompletionUseSingle(QsciScintilla::AcusAlways);
    textEdit_->setAutoCompletionThreshold(0);
    //! Подсветка соответствий скобок
    textEdit_->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    textEdit_->setMatchedBraceBackgroundColor(Qt::yellow);
    textEdit_->setUnmatchedBraceForegroundColor(Qt::blue);

    connect(ui->comboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){setCurrentIndex(index);});
    ui->statusContainer->addWidget(bar_);
    bar_->addPermanentWidget(ui->label_left,1);
    bar_->addPermanentWidget(ui->label_right,1);
    ui->label_left->setText("left");
    ui->label_right->setText("right");
    ui->tableWidget_->setLocale(QLocale::Russian);
}


TabWindow::~TabWindow()
{
    delete ui;
    delete textEdit_;
    delete sqlLexer_;

    for (auto item : models_)
    {
        delete item;
    }
    models_.clear();
}

void TabWindow::keyPressEvent(QKeyEvent *event)
{

    if (event->key() == 67)
    {
        std::string buffer_;

        QItemSelectionModel * selection = ui->tableWidget_->selectionModel();
        QModelIndexList indexes = selection->selectedIndexes();

        QModelIndex previous = indexes.first();
        foreach(const QModelIndex &current, indexes)
        {
            QVariant data = ui->tableWidget_->model()->data(current);
            std::string text = data.toString().toStdString();
            if (current.column() == 0)
            {
                buffer_.push_back('\n');
            }
            else
            {
                buffer_.push_back('\t');
            }

            buffer_ += text;
            previous = current;
        }
        buffer_.erase(buffer_.begin(), buffer_.begin()+1);

        std::ofstream out;

        buffer_.erase(std::remove(buffer_.begin(), buffer_.end(), '\0'), buffer_.end());

        out.open("outputBuffer.csv");
        if (out.is_open())
        {
            out << buffer_ << std::endl;
        }
        QApplication::clipboard()->setText(QString::fromStdString(buffer_));
    }
}

QString TabWindow::textFromTextEdit()
{
    return textEdit_->text();
}

void TabWindow::setText(QString input)
{
    textEdit_->setText(input);
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

    // get text from TextEdit

    QString executeText;
    std::cout<<textEdit_->selectedText().toStdString()<<std::endl;
    if (textEdit_->hasSelectedText()) executeText = textEdit_->selectedText();
    else executeText = textEdit_->text();

    // fill models

    std::thread th1([=]()
    {
        ExecSqlASYNC2(tabNumber_ , dbPort_, executeText.toStdString());

        std::vector<std::string> queryesVector = getParsedQuery(executeText.toStdString());

        for (auto item : queryesVector) item = item.substr(0, item.size() > 60 ? 60 : item.size());

        for (int i = 0; i < GetCountAnswer(tabNumber_); i+=1)
        {
            ResponceView* model = new ResponceView();

            std::string err = GetError(tabNumber_, i);
            if (err != "") model->setError(err);

            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
            GetSqlResulsASYNC(tabNumber_, dbPort_, i);

            model->setQueryInfo(tabNumber_, i);
            models_.push_back(model);
            reqTypesList_.push_back(QString("Result %1: ").arg(i+1) + QString::fromStdString(GetQueryType(tabNumber_,i)) + " => " + QString::fromStdString(queryesVector.at(i)));
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

std::vector<std::string> TabWindow::getParsedQuery(std::string str)
{
    std::vector<std::string> output;

    size_t pos = str.find('\n');
    while (pos != std::string::npos) {
        str.erase(pos, 1);
        pos = str.find('\n');
    }
    pos = str.find('\t');
    while (pos != std::string::npos) {
        str.erase(pos, 1);
        pos = str.find('\t');
    }

    if (str.back() != ';') str.push_back(';');

    std::string::size_type beg = 0;
    for (size_t end = 0; (end = str.find(';', end)) != std::string::npos; end += 1)
    {
        std::string tmp = str.substr(beg, end - beg);
        output.push_back(tmp);
        beg = end + 1;
    }

    return output;
}
