#pragma once

#include <QWidget>
#include <QSyntaxHighlighter>

class SyntaxHighlighter;

class SyntaxHighlighter : public QSyntaxHighlighter {

public:
    SyntaxHighlighter( QTextDocument* parent ) : QSyntaxHighlighter(parent) {}

    void highlightBlock( const QString& text )
    {
        if( m_highlightedString.isEmpty() ) {
            return;
        }

        QTextCharFormat fmt;
        fmt.setForeground(Qt::blue);

        const int LEN = m_highlightedString.length();
        for( int index = text.indexOf( m_highlightedString ); 0 <= index; index = text.indexOf( m_highlightedString, index + LEN ))
        {
            setFormat( index, LEN, fmt );
        }
    }

    void setHighlightedString( const QString& str )
    {
        m_highlightedString = str;
        rehighlight();
    }

private:
    QString m_highlightedString;
};
