#pragma once

#include "ResponceView.hpp"

ResponceView::ResponceView(QObject *parent)
{

}
void ResponceView::setQueryInfo(int connectIndex, int queryIndex)
{
    connectIndex_ = connectIndex;
    queryIndex_ = queryIndex;
    horizontalHeader_ = getHoryzontalHeader1(connectIndex, queryIndex);

    for (int i = 0; i< horizontalHeader_.size(); i+=1)
    {
        this->setHeaderData(i, Qt::Horizontal, horizontalHeader_.at(i));
    }
}
QVariant ResponceView::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (errFlag_) return QVariant("");

    if (orientation == Qt::Vertical && role == Qt::DisplayRole)
    {
        return QString("%1").arg(section + 1);
    }
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        QString tmp = horizontalHeader_.at(section);
        return tmp;
    }
    return QVariant();
}
int ResponceView::rowCount(const QModelIndex &parent) const
{
    if (errFlag_) return 1;

    return rowCount_;
}
int ResponceView::columnCount(const QModelIndex &parent) const
{
    if (errFlag_) return 1;
    return horizontalHeader_.size();
}
QVariant ResponceView::data(const QModelIndex &index, int role) const
{
    if (errFlag_ && role == Qt::DisplayRole) return (QString::fromStdString(errStr_));
    if (role == Qt::DisplayRole && rowsAffected_ != 0) return QString("Rows affected: %1").arg(rowsAffected_);

    if(role != Qt::DisplayRole) return QVariant();

    QString var;

    if (index.column() <= horizontalHeader_.size() && index.row() <= rowCount_)
    {
        return getFieldValue1(connectIndex_, queryIndex_, index.row(), index.column());
    }
    return var;
}
ResponceView::~ResponceView()
{

}
void ResponceView::setError(std::string& errStr)
{
    errFlag_ = true;
    errStr_ = errStr;
}

QString toHex(const QString& str) {
    QByteArray data = str.toUtf8();
    return data.toHex();
}

QString ResponceView::getFieldValue1(int idconnect, int resnum, int rowIndex, int columnIndex) const
{
    QString output;
    int querytype, rowsaffected, queryerror;
    nb_stop_and_settorecord( idconnect, resnum, rowIndex, &querytype, &rowsaffected, &queryerror );

    int isallready, numberOfReady;
    NB_HANDLE connection;
    do
    {
        nb_check_result( idconnect, &isallready, &numberOfReady, &connection );
    } while ( numberOfReady == 0 );

    NBValue value{};

    if ( nb_fetch_row( connection ) != NB_OK ) return QString();

    nb_field_value( connection, columnIndex, &value );

    if ( value.null == true ) return QString("null");

    switch ( value.type )
    {
    case NB_DATA_INT: output = QString::number( value.intv ); break;
    case NB_DATA_DATETIME: output = QString( value.str ) ; break;
    case NB_DATA_STRING: output= QString::fromUtf8( value.str, value.len ); break;
    case NB_DATA_U16STRING:  output= QString::fromUtf16( (char16_t*)value.str, value.len/2 ); break;
    case NB_DATA_DECIMAL: output = QString( value.str ); break;
    case NB_DATA_INT64: output = QString::number( value.int64v ); break;
    case NB_DATA_DOUBLE: output = QString::number( value.dbl ); break;
    case NB_DATA_BOOL: output = ( ( value.intv ) ? "TRUE" : "FALSE" ); break;
    case NB_DATA_BINARY:
    {
        output += "0x";
        output += QString::fromStdString(convertToHex((char16_t*)value.str, value.len));

        break;
    }
    case NB_DATA_DATE: output = QString( value.str ); break;
    case NB_DATA_NONE : output = "none"; break;
    case NB_DATA_ROWVERSION:
    {
        output += "0x";
        output += QString::fromStdString(convertToHex8((char*)value.str));

        break;
    }
    case NB_DATA_URI: output = QString( value.str ); break;
    }

    nb_continue_work( idconnect );
    return output;
}

std::vector<QString> ResponceView::getHoryzontalHeader1(int idconnect, int resnum)
{
    std::vector<QString> output;
    int querytype, rowsaffected, queryerror;
    nb_stop_and_settorecord( idconnect, resnum, 0, &querytype, &rowsaffected, &queryerror );

    NBValue value{};
    QString strValue;

    int isallready, numberOfReady;
    NB_HANDLE connection;
    do
    {
        nb_check_result( idconnect, &isallready, &numberOfReady, &connection );
    } while ( numberOfReady == 0 );


    int fieldcount = nb_field_count( connection );
    if( fieldcount == 0 )
    {
        output.push_back("(No name)");
    }
    else
    {
        for (int i = 0; i < fieldcount; i += 1)
        {
            nb_field_name( connection, i, &value );

            switch ( value.type )
            {
            case NB_DATA_INT: strValue = QString::number( value.intv ); break;
            case NB_DATA_DATETIME: strValue = QString( value.str ) ; break;
            case NB_DATA_STRING: strValue= QString::fromUtf8( value.str, value.len ); break;
            case NB_DATA_U16STRING:  strValue= QString::fromUtf16( (char16_t*)value.str, value.len/2 ); break;
            case NB_DATA_DECIMAL: strValue = QString( value.str ); break;
            case NB_DATA_INT64: strValue = QString::number( value.int64v ); break;
            case NB_DATA_DOUBLE: strValue = QString::number( value.dbl ); break;
            case NB_DATA_BOOL: strValue = ( ( value.intv ) ? "TRUE" : "FALSE" ); break;
            case NB_DATA_BINARY:
            {
                QByteArray utf8Data = QString::fromUtf8(value.str, value.len).toUtf8();
                for (int i = 0; i < utf8Data.size() && i < 2048; i += 1 ) strValue += QString::number( (quint8)utf8Data.at(i), 16 );
                break;
            }
            case NB_DATA_DATE: strValue = QString( value.str ); break;
            case NB_DATA_NONE : strValue = "none"; break;
            case NB_DATA_ROWVERSION: strValue= QString( value.str ); break;
            case NB_DATA_URI: strValue = QString( value.str ); break;
            default: strValue = "(No name)";
            }

            if ( strValue == "" ) strValue = "(No name)";

            output.push_back(strValue);
        }
    }

    nb_continue_work( idconnect );
    return output;
}
