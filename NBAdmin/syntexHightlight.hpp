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

private:
    QColor commentColor_ = QColor("#008000");
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
            {"ON ",QColor("#0085c7")},
            {"VARCHAR",QColor("#0085c7")},
            {"PRIMARY KEY ",QColor("#0085c7")},
            {"NOT",QColor("#0085c7")},
            {"NULL",QColor("#0085c7")},
            {"INT ",QColor("#0085c7")},
            {"IDENTITY",QColor("#0085c7")},
            {"FLOAT ",QColor("#0085c7")},
            {"VARBINARY ",QColor("#0085c7")},
            {"NVARCHAR",QColor("#0085c7")},
            {"DECIMAL",QColor("#0085c7")},
            {"DATE ",QColor("#0085c7")},
            {"DATETIME ",QColor("#0085c7")},
            {"BIT ",QColor("#0085c7")},
            {"DOUBLE ",QColor("#0085c7")},

        };
};
