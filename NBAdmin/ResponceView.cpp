#pragma once

#include "ResponceView.hpp"

ResponceView::ResponceView(QObject *parent)
{

}
void ResponceView::setQueryInfo(int connectIndex, int queryIndex)
{
    connectIndex_ = connectIndex;
    queryIndex_ = queryIndex;
    horizontalHeader_ = PrintResultInfo2(nbpool.connects[connectIndex_].result, queryIndex_);
    rowCount_ = GetRowCount(nbpool.connects[connectIndex_].result, queryIndex_);
    std::cout<<"rowCount - "<<rowCount_<<std::endl;

    for (int i = 0; i< horizontalHeader_.size(); i+=1)
    {
        this->setHeaderData(i, Qt::Horizontal, QString::fromStdString(horizontalHeader_.at(i)));
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
        return QString::fromStdString(horizontalHeader_.at(section));
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

    if(role != Qt::DisplayRole) return QVariant();

    QString var;

    if (index.column() <= horizontalHeader_.size() && index.row() <= rowCount_)
    {
        NBValue v = GetItemFromTable(nbpool.connects[connectIndex_].result, queryIndex_, index.column(), index.row());

        std::string output = "";

        if (v.null == true) return QString("null");

        switch ( v.type )
        {
            case NB_DATA_INT: output = std::to_string(v.intv); break;
            case NB_DATA_DATETIME: for (int i = 0; i< v.len; i+=1) output += v.str[i]; break;
            case NB_DATA_STRING: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
            case NB_DATA_U16STRING: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
            case NB_DATA_DECIMAL: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
            case NB_DATA_INT64: output = std::to_string(v.int64v); break;
            case NB_DATA_DOUBLE: output = std::to_string(v.dbl); break;
            case NB_DATA_BOOL: output = ( ( v.intv ) ? "TRUE" : "FALSE" ); break;
            case NB_DATA_BINARY: output += *(&v.str); break;
            case NB_DATA_DATE: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
            case NB_DATA_NONE : output = "none"; break;
            case NB_DATA_ROWVERSION: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
            case NB_DATA_URI: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
        }
        //--------------

        return QString::fromStdString(output);
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
QString ResponceView::from_nbvalue(NBValue &v)
{
    std::string output = "";

    if (v.null == true) return QString("null");

    switch ( v.type )
    {
        case NB_DATA_INT: output = std::to_string(v.intv); break;
        case NB_DATA_DATETIME: for (int i = 0; i< v.len; i+=1) output += v.str[i]; break;
        case NB_DATA_STRING: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
        case NB_DATA_U16STRING: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
        case NB_DATA_DECIMAL: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
        case NB_DATA_INT64: output = std::to_string(v.int64v); break;
        case NB_DATA_DOUBLE: output = std::to_string(v.dbl); break;
        case NB_DATA_BOOL: output = ( ( v.intv ) ? "TRUE" : "FALSE" ); break;
        case NB_DATA_BINARY: output += *(&v.str); break;
        case NB_DATA_DATE: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
        case NB_DATA_NONE : output = "none"; break;
        case NB_DATA_ROWVERSION: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
        case NB_DATA_URI: for (int i = 0; i< v.len; i+=1) output+= v.str[i]; break;
    }
    return QString::fromStdString(output);
}
