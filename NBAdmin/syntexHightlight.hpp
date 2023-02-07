#pragma once

#include <QWidget>
#include <QSyntaxHighlighter>
#include <vector>
#include <utility>
#include <QRegularExpression>

class SyntaxHighlighter : public QSyntaxHighlighter {

public:
    SyntaxHighlighter( QTextDocument* parent );

    void highlightBlock( const QString& text );
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

private:
    std::vector<std::pair<QString, QColor> > alp =
        {
            {"SELECT ",QColor("#0085c7")},
            {"FROM ",QColor("#0085c7")},
            {"TOP ",QColor("#0085c7")},
            {"WHERE ",QColor("#0085c7")},
            {"CREATE ",QColor("#0085c7")},
            {"DELETE ",QColor("#0085c7")},
            {"INSERT ",QColor("#0085c7")},
            {"ALTER ",QColor("#0085c7")},
            {"TABLE ",QColor("#0085c7")},
            {"INDEX ",QColor("#0085c7")},
            {"AS ",QColor("#0085c7")},
            {"EDGE ",QColor("#0085c7")},
            {"DROP ",QColor("#0085c7")},
            {"BULK ",QColor("#0085c7")},
            {"UPDATE ",QColor("#0085c7")},
            {"TRANCATE ",QColor("#0085c7")},
            {"BACKUP ",QColor("#0085c7")},
            {"SET ",QColor("#0085c7")},
            {"FROM ",QColor("#0085c7")},
            {"JOIN ",QColor("#0085c7")},
            {"ON ",QColor("#0085c7")},
            {"LEFT ",QColor("#0085c7")},
            {"RIGHT ",QColor("#0085c7")},
            {"ADD ",QColor("#0085c7")},
            {"VALUE ",QColor("#0085c7")},
            {"VALUES ",QColor("#0085c7")},
            {"WITH ",QColor("#0085c7")},
            {"DATABASE ",QColor("#0085c7")},
            {"ORDER ",QColor("#0085c7")},
            {"BY ",QColor("#0085c7")},
            {"DISTINCT ",QColor("#0085c7")},
            {"THEN ",QColor("#0085c7")},
            {"ON ",QColor("#0085c7")}
        };
};
