#pragma once

#include <QAbstractTableModel>
#include <QString>

#include <vector>
#include <string>

class ResponceView : public QAbstractTableModel
{
    Q_OBJECT
public:
    ResponceView(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return rows_;
    }
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return columns_;
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        QVariant var;
        if (role == Qt::DisplayRole) return QString::fromStdString(responces_.at(index.row()).at(index.column()));
        return var;
    }
    void setResponce(std::vector<std::vector<std::string> > value)
    {
        responces_ = value;
    }

private:
    int rows_ = 0;
    int columns_ = 0;
    std::vector<std::vector<std::string> > responces_;
signals:
    void editCompleted(const QString &);
};
