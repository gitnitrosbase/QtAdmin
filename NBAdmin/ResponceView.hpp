#pragma once

#include <QAbstractTableModel>
#include <QString>

class ResponceView : public QAbstractTableModel
{
    Q_OBJECT
public:
    ResponceView(QObject *parent = nullptr);

    void setSize(int column);

    int rowCount(const QModelIndex &parent = QModelIndex());

    int columnCount(const QModelIndex &parent = QModelIndex());

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole);

    void addRow(std::vector<QString> &array);

    ~ResponceView();

public:
    std::vector<std::vector<QString> >* data_;
    int columnCount_ = 0;
};
