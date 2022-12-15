#pragma once

#include <QWidget>
#include <QSyntaxHighlighter>
#include <vector>
#include <utility>
#include <QRegularExpression>

class SyntaxHighlighter;

class SyntaxHighlighter : public QSyntaxHighlighter {

public:
    SyntaxHighlighter( QTextDocument* parent ) : QSyntaxHighlighter(parent) {}

    void highlightBlock( const QString& text )
    {
//        QTextCharFormat fmt;
//        int LEN;

//        for (auto item : alp)
//        {
//            fmt.setForeground(item.second);
//            LEN = item.first.length();
//            for( int index = text.indexOf( item.first ); 0 <= index; index = text.indexOf( item.first, index + LEN ))
//            {
//                setFormat( index, LEN, fmt );
//            }
//        }

        QTextCharFormat multiLineCommentFormat;
        multiLineCommentFormat.setForeground(Qt::red);

        QRegularExpression startExpression("/\\*");
        QRegularExpression endExpression("\\*/");

        setCurrentBlockState(0);

        int startIndex = 0;
        if (previousBlockState() != 1)
            startIndex = text.indexOf(startExpression);

        while (startIndex >= 0) {
            QRegularExpressionMatch endMatch;
            int endIndex = text.indexOf(endExpression, startIndex, &endMatch);
            int commentLength;
            if (endIndex == -1) {
                setCurrentBlockState(1);
                commentLength = text.length() - startIndex;
            } else {
                commentLength = endIndex - startIndex
                                + endMatch.capturedLength();
            }
            setFormat(startIndex, commentLength, multiLineCommentFormat);
            startIndex = text.indexOf(startExpression,
                                      startIndex + commentLength);
        }

    }

private:
    std::vector<std::pair<QString, Qt::GlobalColor> > alp =
    {
        {" SELECT ",Qt::blue},
        {" FROM ",Qt::blue},
        {" TOP ",Qt::blue},
        {" WHERE ",Qt::blue},
        {" CREATE ",Qt::blue},
        {" DELETE ",Qt::blue},
        {" INSERT ",Qt::blue},
        {" ALTER ",Qt::blue},
        {" TABLE ",Qt::blue},
        {" INDEX ",Qt::blue},
        {" AS ",Qt::blue},
        {" EDGE ",Qt::blue},
        {" DROP ",Qt::blue},
        {" BULK ",Qt::blue},
        {" UPDATE ",Qt::blue},
        {" TRANCATE ",Qt::blue},
        {" BACKUP ",Qt::blue},
        {" SET ",Qt::blue},
        {" FROM ",Qt::blue},
        {" JOIN ",Qt::blue},
        {" ON ",Qt::blue},
        {" LEFT ",Qt::blue},
        {" RIGHT ",Qt::blue},
        {" ADD ",Qt::blue},
        {" VALUE ",Qt::blue},
        {" VALUES ",Qt::blue},
        {" WITH ",Qt::blue},
        {" DATABASE ",Qt::blue},
        {" ORDER ",Qt::blue},
        {" BY ",Qt::blue},
        {" DISTINCT ",Qt::blue},
        {" THEN ",Qt::blue},
        {"ON",Qt::blue}
    };
};
