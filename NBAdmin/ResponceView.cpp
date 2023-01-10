#pragma once

#include "ResponceView.hpp"

ResponceView::ResponceView(QObject *parent = nullptr)
{

}
void ResponceView::setSize(int column)
{
    columnCount_ = column;
    if (data_ != nullptr) delete data_;
    data_ = new std::vector<std::vector<QString> >;
    data_->resize(columnCount_);
}
int ResponceView::rowCount(const QModelIndex &parent = QModelIndex())
{
    return data_->size();
}
int ResponceView::columnCount(const QModelIndex &parent = QModelIndex())
{
    return columnCount_;
}
QVariant ResponceView::data(const QModelIndex &index, int role = Qt::DisplayRole)
{
    QVariant var;
    if (role == Qt::DisplayRole) return data_->at(index.row()).at(index.column());

    return var;
}
void ResponceView::addRow(std::vector<QString> &array)
{
    if (data_->size() % 1000 == 0)
    {
        this->beginResetModel();
        data_->push_back(array);
        this->endResetModel();
    }
    else data_->push_back(array);
}
ResponceView::~ResponceView()
{
    delete data_;
}
