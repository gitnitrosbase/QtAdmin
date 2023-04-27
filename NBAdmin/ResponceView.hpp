#pragma once

#include <QAbstractTableModel>
#include <QString>
#include "nb-samples.h"
#include <iostream>

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

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QString fromNBValue(const NBValue &v) const;

    ~ResponceView();

public:
    std::vector<NBValue> horizontalHeader_;
    int rowCount_ = 0;
    int connectIndex_ = 0;
    int queryIndex_ = 0;
    bool errFlag_ = false;
    int rowsAffected_ = 0;
    std::string errStr_;
};
