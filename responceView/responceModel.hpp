#pragma once

#include <QAbstractTableModel>
#include <QString>
#include <iostream>

class ResponceView : public QAbstractTableModel
{
    Q_OBJECT
public:
    ResponceView(QObject *parent = nullptr) :QAbstractTableModel(parent)
    {
    }
//    void setSize(int column)
//    {
//        columnCount_ = column;

//        data_.resize(columnCount_);

//    }
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return data_.size();
    }
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return 0;
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        QVariant var;
        if (role == Qt::DisplayRole) return data_.at(index.row()).at(index.column());

        return var;
    }
    void addRow(QList<QString> &array)
    {
        std::cout<< "add" <<std::endl;
//        if (data_.size() % 1000 == 0)
//        {
//            this->beginResetModel();
//            data_.push_back(array);
//            this->endResetModel();
//        }
//        else data_.push_back(array);
        this->beginResetModel();
        data_.push_back(array);
        this->endResetModel();
        std::cout<<data_.size()<<std::endl;
    }
    ~ResponceView()
    {
    }

public:
    //std::vector<std::vector<QString> > data_ = {{"1", "2", "3", "4", "5", "6"}};
    QList<QList<QString> > data_ = {{"1", "2", "3", "4", "5", "6"}};
    int columnCount_ = 0;
};
