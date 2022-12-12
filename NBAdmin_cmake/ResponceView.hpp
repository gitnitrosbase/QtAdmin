#pragma once

#include <QAbstractTableModel>
#include <QString>


class ResponceView : public QAbstractTableModel
{
    Q_OBJECT
public:
    ResponceView(QObject *parent = nullptr) :QAbstractTableModel(parent)
    {

    }
    void setSize(int column)
    {
        columnCount_ = column;
        if (data_ != nullptr) delete data_;
        data_ = new std::vector<std::vector<QString> >;
        data_->resize(columnCount_);
    }
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return data_->size();
    }
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return columnCount_;
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        QVariant var;
        if (role == Qt::DisplayRole) return data_->at(index.row()).at(index.column());

        return var;
    }
    void addRow(std::vector<QString> &array)
    {
        if (data_->size() % 1000 == 0)
        {
            this->beginResetModel();
            data_->push_back(array);
            this->endResetModel();
        }
        else data_->push_back(array);
    }
    ~ResponceView()
    {
        delete data_;
    }

public:
    std::vector<std::vector<QString> >* data_;
    int columnCount_ = 0;
};
