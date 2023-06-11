#pragma once

#include <QAbstractTableModel>
#include <QString>
#include <QColor>
#include "nb-samples.h"
#include <iostream>


inline std::string convertToHex(const char16_t* input, size_t len)
{
    std::ostringstream oss;
    oss << std::hex << std::uppercase;

    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(input);
    for (size_t i = 0; i < len; ++i)
    {
        unsigned int hexValue = static_cast<unsigned int>(ptr[i]);
        oss << std::setw(2) << std::setfill('0') << hexValue;
    }

    return oss.str();
}

inline std::string convertToHex8(const char* input)
{
    std::ostringstream oss;
    oss << std::hex << std::uppercase;

    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(input);
    for (int i = 0; i < 8; ++i)
    {
        oss << static_cast<unsigned int>(ptr[i]);
    }

    return oss.str();
}

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

    QString getFieldValue1(int idconnect, int resnum, int rowIndex, int columnIndex) const;

    std::vector<QString> getHoryzontalHeader1(int idconnect, int resnum);

    ~ResponceView();

public:
    std::vector<QString> horizontalHeader_;
    int rowCount_ = 0;
    int connectIndex_ = 0;
    int queryIndex_ = 0;
    bool errFlag_ = false;
    int rowsAffected_ = 0;
    std::string errStr_;
};
