#pragma once

#include <QAbstractTableModel>
#include <QString>
#include "nb-samples.h"
#include <iostream>


inline std::string convertToHex(const char16_t* input)
{
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setfill('0');

    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(input);
    for (int i = 0; i < input[i] != '\0'; ++i)
    {
        oss << static_cast<unsigned int>(ptr[i]);
    }

    return oss.str();
}

inline std::string convertToHex8(const char* input)
{
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setfill('0');

    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(input);
    for (int i = 0; i < 8; ++i)
    {
        oss << static_cast<unsigned int>(ptr[i]);
    }

    return oss.str();
}

//QList<QList<QList<QList<QString> > > > buffer_{64};
//QList<QList<int> > startIndexes_;

//inline void writeToBuffer(int connectIndex, int queryIndex, int startRow)
//{

//}
//inline QString getFromBuffer(int connectIndex, int queryIndex, int rowIndex, int columnIndex)
//{

//}


//inline QList<QList<QString> > GetBuffer(int connectIndex, int queryIndex, int rowIndex)
//{
//    QList<QList<QString> > output;

//    int querytype, rowsaffected, queryerror;
//    nb_stop_and_settorecord( connectIndex, queryIndex, rowIndex, &querytype, &rowsaffected, &queryerror );

//    int isallready, numberOfReady;
//    NB_HANDLE connection;
//    do
//    {
//        nb_check_result( connectIndex, &isallready, &numberOfReady, &connection );
//    } while ( numberOfReady == 0 );


//    for ( int i = 0; i < std::min(rowsaffected, 1000); i += 1 )
//    {
//        NBValue value{};

//        QList<QString> rowList;

//        if ( nb_fetch_row( connection ) != NB_OK ) return output;

//        int columnCount =  nb_field_count( connection );
//        for (int j = 0; j < columnCount; j += 1)
//        {
//            nb_field_value( connection, j, &value );

//            if ( value.null == true )
//            {
//                rowList.push_back("null");
//                continue;
//            }

//            switch ( value.type )
//            {
//            case NB_DATA_INT: rowList.push_back(QString::number( value.intv )); break;
//            case NB_DATA_DATETIME: rowList.push_back(QString( value.str )) ; break;
//            case NB_DATA_STRING: rowList.push_back(QString::fromUtf8( value.str, value.len )); break;
//            case NB_DATA_U16STRING:  rowList.push_back(QString::fromUtf16( (char16_t*)value.str, value.len/2 )); break;
//            case NB_DATA_DECIMAL: rowList.push_back(QString( value.str )); break;
//            case NB_DATA_INT64: rowList.push_back(QString::number( value.int64v )); break;
//            case NB_DATA_DOUBLE: rowList.push_back(QString::number( value.dbl )); break;
//            case NB_DATA_BOOL: rowList.push_back( ( ( value.intv ) ? "TRUE" : "FALSE" )); break;
//            case NB_DATA_BINARY:
//            {
//                QString tmp;
//                QByteArray utf8Data = QString::fromUtf8(value.str, value.len).toUtf8();
//                for (int i = 0; i < utf8Data.size() && i < 2048; i += 1 ) tmp += QString::number( (quint8)utf8Data.at(i), 16 );
//                rowList.push_back(tmp);
//                break;
//            }
//            case NB_DATA_DATE: rowList.push_back(QString( value.str )); break;
//            case NB_DATA_NONE : rowList.push_back( "none"); break;
//            case NB_DATA_ROWVERSION:
//            {
//                rowList.push_back( QString::fromUtf16( (char16_t*)value.str, value.len/2 ));
//                break;
//            }
//            case NB_DATA_URI: rowList.push_back(QString( value.str )); break;
//            }
//        }
//        output.push_back(rowList);
//    }

//    nb_continue_work( connectIndex );

//    return output;
//}


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
