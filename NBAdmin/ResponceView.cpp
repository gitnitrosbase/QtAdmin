#pragma once

#include "ResponceView.hpp"

ResponceView::ResponceView(QObject *parent)
{

}
void ResponceView::setQueryInfo(int connectIndex, int queryIndex)
{
    connectIndex_ = connectIndex;
    queryIndex_ = queryIndex;
    horizontalHeader_ = getHoryzontalHeader(connectIndex, queryIndex);

    for (int i = 0; i< horizontalHeader_.size(); i+=1)
    {
        this->setHeaderData(i, Qt::Horizontal, fromNBValue(horizontalHeader_.at(i)));
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
        QString tmp = fromNBValue(horizontalHeader_.at(section));
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
        return fromNBValue(getFieldValue(connectIndex_, queryIndex_, index.row(), index.column()));
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

QString ResponceView::fromNBValue(const NBValue &v) const
{
    QString output = "";

    if ( v.null == true ) return QString("null");

    switch ( v.type )
    {
    case NB_DATA_INT: output = QString::number( v.intv ); break;
    case NB_DATA_DATETIME: output = QString( v.str ) ; break;
    case NB_DATA_STRING: output= QString::fromUtf8( v.str, v.len ); break;
    case NB_DATA_U16STRING:  output= QString::fromUtf16( (char16_t*)v.str, v.len/2 ); break;
    case NB_DATA_DECIMAL: output = QString( v.str ); break;
    case NB_DATA_INT64: output = QString::number( v.int64v ); break;
    case NB_DATA_DOUBLE: output = QString::number( v.dbl ); break;
    case NB_DATA_BOOL: output = ( ( v.intv ) ? "TRUE" : "FALSE" ); break;
    case NB_DATA_BINARY:
    {
        QByteArray utf8Data = QString::fromUtf8(v.str, v.len).toUtf8();
        for (int i = 0; i < utf8Data.size() && i < 2048; i += 1 ) output += QString::number( (quint8)utf8Data.at(i), 16 );
        break;
    }
    case NB_DATA_DATE: output = QString( v.str ); break;
    case NB_DATA_NONE : output = "none"; break;
    case NB_DATA_ROWVERSION: output= QString( v.str ); break;
    case NB_DATA_URI: output = QString( v.str ); break;
    }
    return output;
}
