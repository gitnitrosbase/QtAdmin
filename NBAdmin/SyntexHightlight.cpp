#pragma once

#include "syntexHightlight.hpp"

SyntaxHighlighter::SyntaxHighlighter( QTextDocument* parent ) : QSyntaxHighlighter(parent) {}

void SyntaxHighlighter::highlightBlock( const QString& text )
{
    QTextCharFormat fmt;
    int LEN;

    for (auto item_s : alp)
    {
        auto item_u = item_s;
        auto item_l = item_s;
        fmt.setForeground(item_u.second);
        LEN = item_u.first.length();
        for( int index = text.indexOf( item_u.first ); 0 <= index ; index = text.indexOf( item_u.first, index + LEN ))
        {
            setFormat( index, LEN, fmt );
        }
        item_l.first = item_l.first.toLower();
        for( int index = text.indexOf( item_l.first ); 0 <= index; index = text.indexOf( item_l.first, index + LEN ))
        {
            setFormat( index, LEN, fmt );
        }
    }
}
//        QTextCharFormat multiLineCommentFormat;
//        multiLineCommentFormat.setForeground(Qt::red);

//        QRegularExpression startExpression("/\\*");
//        QRegularExpression endExpression("\\*/");

//        setCurrentBlockState(0);

//        int startIndex = 0;
//        if (previousBlockState() != 1)
//            startIndex = text.indexOf(startExpression);

//        while (startIndex >= 0) {
//            QRegularExpressionMatch endMatch;
//            int endIndex = text.indexOf(endExpression, startIndex, &endMatch);
//            int commentLength;
//            if (endIndex == -1) {
//                setCurrentBlockState(1);
//                commentLength = text.length() - startIndex;
//            } else {
//                commentLength = endIndex - startIndex
//                                + endMatch.capturedLength();
//            }
//            setFormat(startIndex, commentLength, multiLineCommentFormat);
//            startIndex = text.indexOf(startExpression,
//                                      startIndex + commentLength);
//        }
