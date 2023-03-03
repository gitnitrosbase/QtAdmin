#pragma once

#include <QAbstractTableModel>
#include <QString>
#include "nb-samples.h"

class ResponceView : public QAbstractTableModel
{
    Q_OBJECT
public:
    ResponceView(QObject *parent = nullptr);

    void setError(std::string &errStr);

    void setQueryInfo(int connectIndex, int queryIndex);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QString from_nbvalue(NBValue &v);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    ~ResponceView();

public:
    std::vector<std::string> horizontalHeader_;
    int rowCount_ = 0;
    int connectIndex_ = 0;
    int queryIndex_ = 0;
    bool errFlag_ = false;
    std::string errStr_;
};
