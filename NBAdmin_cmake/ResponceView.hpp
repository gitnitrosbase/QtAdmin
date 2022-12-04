#pragma once

#include <QAbstractTableModel>
#include <QString>

#include <QList>
#include <QStandardItem>

#include <vector>
#include <string>

class ResponceView : public QAbstractTableModel
{
    Q_OBJECT
public:
    ResponceView(QObject *parent = nullptr) : QAbstractTableModel(parent)
    {

    }
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
//        QList<QStandardItem*> list;
//        for (int i = 0; i<value.size(); i+=1)
//        {
//            for (int j = 0; j<value.at(0).size(); j+=1)
//            {
//                QStandardItem* item = new QStandardItem(i,j);
//                item->setText(QString::fromStdString(responces_.at(i).at(j)));
//                list.append(item);
//            }
//        }
    }



private:
    int rows_ = 0;
    int columns_ = 0;
    std::vector<std::vector<std::string> > responces_;
signals:
    void editCompleted(const QString &);
};
