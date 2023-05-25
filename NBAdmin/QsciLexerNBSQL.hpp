#pragma once

#include <Qsci/qscilexercustom.h>


class QsciLexerNBSQL : public QsciLexerCustom
{
    Q_OBJECT
public:
    explicit QsciLexerNBSQL(QObject *parent = 0);
    ~QsciLexerNBSQL();

    void styleText(int start, int end)
    {
        if(!editor()) return;

        char * data = new char[end - start + 1];
        editor()->SendScintilla(QsciScintilla::SCI_GETTEXTRANGE, start, end, data);
        QString source(data);
        delete [] data;
        if(source.isEmpty()) return;

        paintKeywords(source, start);
        paintComments(source, start);
    }
    void paintKeywords(const QString &source, int start)
    {
        foreach(QString word, keywordsList)
        { // перебираем ключевые слова
            if(source.contains(word))
            {
                int p = source.count(word); // считаем вхождения
                int index = 0; // начнем считать индексы c 0
                while(p != 0)
                {
                    int begin = source.indexOf(word, index); // считаем индекс вхождения
                    index = begin+1; // задаем точку отсчета для следущей итерации

                    startStyling(start + begin); // начнем стилизировать с индекса вхождения
                    setStyling(word.length(), Keyword); // для длины word.length задаем стиль Keyword
                    startStyling(start + begin); // заканчиваем стилизацию

                    p--;
                }
            }
        }
    }
    void paintComments(const QString &source, int start)
    {
        int p = source.count(";");
        if(p == 0) return;
        int index = 0;
        while(p != 0)
        {
            int begin = source.indexOf(";", index);
            int length=0;
            index = begin+1;

            for(int k = begin; source[k] != '\n'; k+=1) length++;

            startStyling(start + begin);
            setStyling(length, Comment);
            startStyling(start + begin);

            p-=1;
        }
    }
    const char * language() const
    {
        return "NBSQL";
    }
    QColor defaultColor(int style) const
    {
        switch(style)
        {
        case String: return Qt::lightGray;
        case Comment: return Qt::green;
        case Keyword: return QColor("#0085c7");
        default: Qt::black;
        }

    }
    QString description(int style) const
    {
        switch(style)
        {
        case Default: return "Default";
        case Comment: return "Comment";
        case Keyword: return "Keyword";
        }
    return QString::number(style);
    }

    enum {
        Default = 0,
        Comment = 1,
        Keyword = 2,
        String = 3
    };
private:
    QsciLexerNBSQL(const QsciLexerNBSQL &);
    QsciLexerNBSQL &operator=(const QsciLexerNBSQL &);
    QStringList keywordsList;

};
