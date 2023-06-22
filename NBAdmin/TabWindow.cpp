#include "TabWindow.hpp"

TabWindow::TabWindow(QWidget* parent) : QWidget(parent) ,ui(new Ui::TabWindow)
{
    ui->setupUi(this);

    textEdit_ = new QsciScintilla;
    sqlLexer_ = new QsciLexerSQL;
    textEdit_->setUtf8(true);
    ui->verticalLayout_->insertWidget(0, textEdit_);
    textEdit_->setLexer(sqlLexer_);
    //! Текущая строка кода и ее подсветка
    textEdit_->setCaretLineVisible(true);
    textEdit_->setCaretLineBackgroundColor(QColor("#eee"));
    //! Выравнивание
    textEdit_->setAutoIndent(true);
    textEdit_->setIndentationGuides(false);
    textEdit_->setIndentationsUseTabs(true);
    textEdit_->setIndentationWidth(4);
    //! margin это полоска слева, на которой обычно распологаются breakpoints
    textEdit_->setMarginsBackgroundColor(QColor("#eee"));
    textEdit_->setMarginLineNumbers(1, true);
    textEdit_->setMarginWidth(1, 15);
    //! Авто-дополнение кода в зависимости от источника
    textEdit_->setAutoCompletionThreshold(1);
    textEdit_->setAutoCompletionReplaceWord(true);
    textEdit_->setAutoCompletionSource(QsciScintilla::AcsAll);
    textEdit_->setAutoCompletionCaseSensitivity(false);

    api_ = new QsciAPIs(sqlLexer_);
    api_->add("SELECT");
    api_->add("FROM");
    api_->prepare();

    //! Подсветка соответствий скобок
    textEdit_->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    textEdit_->setMatchedBraceBackgroundColor(Qt::yellow);
    textEdit_->setUnmatchedBraceForegroundColor(QColor("#0085c7"));

    //! Подсветка синтаксиса ключевых слов
    sqlLexer_->setColor(QColor("#0085c7"), QsciLexerSQL::Keyword);

    connect(ui->comboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){setCurrentIndex(index);});
    ui->tableWidget_->setLocale(QLocale::Russian);

    timer_ = new QTimer();
    connect(timer_, SIGNAL(timeout()), this, SLOT(modelTimerSlot()));

    connect(textEdit_, SIGNAL(linesChanged()), this, SLOT(setLeftMargin()));
}

TabWindow::~TabWindow()
{
    delete api_;
    delete ui;
    delete textEdit_;
    delete sqlLexer_;
    delete timer_;

    for (auto item : models_)
    {
        delete item;
    }
    models_.clear();
}

void TabWindow::setLeftMargin()
{
    int rc = textEdit_->lines();
    if ( rc >= 0 && rc < 10 ) textEdit_->setMarginWidth(1, 15);
    else if ( rc >= 10 && rc < 100 ) textEdit_->setMarginWidth(1, 23);
    else if ( rc >= 100 && rc < 1000 ) textEdit_->setMarginWidth(1, 30);
    else if ( rc >= 1000 && rc < 10000 ) textEdit_->setMarginWidth(1, 37);
    else if ( rc >= 10000 && rc < 100000 ) textEdit_->setMarginWidth(1, 42);
    else textEdit_->setMarginWidth(1, 100);
}

void TabWindow::modelTimerSlot()
{
    reqTypesList_.clear();
    firstPaintModelFlag_ = true;
    int isallready = 0, numberOfReady = 0;
    NB_HANDLE connection = nullptr;

    try
    {
        nb_check_result(tabNumber_, &isallready, &numberOfReady, &connection);

        if (numberOfReady < 0)
        {
            emit turnOnRunButtonSignal();
            return;
        }

        for (int i = models_.size(); i < numberOfReady; i += 1)
        {
            ResponceView* model = new ResponceView();

            // get info about subquery

            ConnectInfo info = GetConnectInfo(tabNumber_, i);

            if (info.queryError != NB_OK)
            {
                size_t errorBufLen;
                std::string errorStr;
                const char* errorBuf = nb_err_text_utf8(connection, &errorBufLen);
                for (size_t i = 0; i < errorBufLen; i += 1) errorStr += errorBuf[i];
                model->setError(errorStr);
            }

            if (info.queryType != "SELECT")
            {
                model->errFlag_ = true;
                model->rowsAffected_ = info.rowsAffected;
            }
            else
            {
                model->rowCount_ = info.rowsAffected;
            }
            model->setQueryInfo(tabNumber_, i);

            models_.push_back(model);
            reqTypesList_.push_back(QString("Result %1: ").arg(i + 1)
                /* get query type */
                + QString::fromStdString(info.queryType)
                + QString(" => "));
        }

        // add input queries to comboBox
        ui->comboBox_->addItems(reqTypesList_);

        if (isallready != 0)
        {
            timer_->stop();
            emit refresh_tree();
            emit turnOnRunButtonSignal();
        }
        else
        {
            timer_->stop();
            timer_->start(300);
        }
    }
    catch (...)
    {
//        std::wcout << e.what() << std::endl;
    }
}

void TabWindow::keyPressEvent(QKeyEvent *event)
{
    if ( event->key() == 67 )
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
    if ( event->key() == Qt::Key_F5 ) push_button_run_clicked();
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
    emit turnOffRunButtonSignal();

    firstPaintModelFlag_ = true;
    if (buffers_ != nullptr) delete buffers_;
    if (startIndexes_ != nullptr) delete startIndexes_;

    flag_ = false;
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
    nb_execute_sql_fortab_utf8(tabNumber_, dbPort_, executeText.toStdString().c_str(), executeText.toStdString().size());

    timer_->start(0);

    flag_ = true;
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
