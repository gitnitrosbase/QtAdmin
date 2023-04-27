#pragma once
#include "nitrosbase.h"
#include <iostream>
#include <vector>

#ifndef TESTPROJECT
#pragma comment( lib, "nbclient.lib" )
#endif

#pragma comment( lib, "nbadminclient.lib" )

extern "C"
{
NB_EXTERN NB_ERROR_TYPE nb_execute_sql_fortab( int tabnum, int port, const char16_t *query, size_t length );
NB_EXTERN NB_ERROR_TYPE nb_execute_sql_fortab_utf8( int tabnum, int port, const char *query, size_t length );
NB_EXTERN NB_ERROR_TYPE nb_check_result( int tabnum, int *isallready, int *numberOfReady, NB_HANDLE *connect4read );
NB_EXTERN NB_ERROR_TYPE nb_stop_and_settorecord( int tabnum, int resultnum, int startrecord, int *querytype, int *rowsaffected, int *queryerror );
NB_EXTERN NB_ERROR_TYPE nb_continue_work( int tabnum );
}

template<size_t N> inline NB_ERROR_TYPE nb_execute_sql( NB_HANDLE connection, const char16_t( &str )[N] )
{
    return nb_execute_sql( connection, (const char16_t *)str, N - 1 );
}

template<size_t N> inline NB_ERROR_TYPE nb_prepare_query( NB_HANDLE connection, const char16_t( &str )[N], NB_HANDLE *queryh )
{
    return nb_prepare_query( connection, (const char16_t *)str, N - 1, queryh );
}

#ifndef strlen16defined
inline int strlen16( const char16_t *strarg )
{
    const char16_t *str = strarg;
    for ( ; *str; ++str ); // empty body
    return str - strarg;
    //return (int)wcslen( (wchar_t *)( strarg ) );
}
#endif

// перегрузка оператора << для удобства вывода значений типа NBValue
inline std::ostream &operator << ( std::ostream &s, const NBValue &v )
{
    if ( v.null )
        s << "NULL";
    else
    {
        switch ( v.type )
        {
        case NB_DATA_ROWVERSION: break;
        case NB_DATA_URI:break;
        case NB_DATA_NONE:break;
        case NB_DATA_INT: s << v.intv; break;
        case NB_DATA_DATE:break;
        case NB_DATA_DATETIME:break;
        case NB_DATA_STRING:break;
        case NB_DATA_U16STRING: s.write( v.str, v.len ); break;
        case NB_DATA_DECIMAL: s.write(v.str, v.len); break;
        case NB_DATA_INT64: s << v.int64v; break;
        case NB_DATA_DOUBLE: s << v.dbl; break;
        case NB_DATA_BOOL: s << ( ( v.intv ) ? "TRUE" : "FALSE" ); break;
        case NB_DATA_BINARY: s << v.str; break;
        }
    }
    return s;
}

// печать сообщения об ошибке, полученной от БД
inline void check_error( NB_HANDLE connection )
{
    if ( nb_errno( connection ) )
    {
        std::cout << "ERROR: " << nb_errno( connection ) << ": " << nb_err_text_utf8( connection ) << std::endl;
        exit( 1 );
    }
}

inline void print_query_result( NB_HANDLE connection )
{
    check_error( connection );

    while ( nb_fetch_row( connection ) == NB_OK )
    {
        std::cout << "------------------------------\n";

        NBValue name, v;
        int fieldcount = nb_field_count( connection );
        for ( int i = 0; i < fieldcount; i++ )
        {
            nb_field_name_utf8( connection, i, &name );
            nb_field_value_utf8( connection, i, &v );
            std::cout << name << ": " << v << std::endl;
        }
    }
    check_error( connection );
}

inline void prepare_testdb( NB_HANDLE connection )
{
    nb_execute_sql( connection, uR"(
        CREATE TABLE person (id nvarchar primary key, name nvarchar, lastname nvarchar, age bigint, city nvarchar, income int, dbl double, dtime datetime);
        CREATE TABLE car (id nvarchar primary key, model nvarchar, year int, color nvarchar, number nvarchar );
        CREATE TABLE friends AS EDGE person person;
        CREATE TABLE owner AS EDGE car person;
    
        BULK INSERT person (id, name, lastname, age, city, income, dbl, dtime) FROM '../samples.data/sntest1/person.csv' WITH (FIRSTROW = 2, FIELDTERMINATOR = ',');
        BULK INSERT car (id, model, year, color, number) FROM '../samples.data/sntest1/car.csv' WITH (FIRSTROW = 2, FIELDTERMINATOR = ',');
        BULK INSERT owner FROM '../samples.data/sntest1/owner.csv' WITH (FIRSTROW = 2, FIELDTERMINATOR = ',');
        BULK INSERT friends FROM '../samples.data/sntest1/friends.csv' WITH (FIRSTROW = 2, FIELDTERMINATOR = ',');
            
        CREATE INDEX p_age ON person (age);
        CREATE INDEX p_city ON person (city);
        CREATE INDEX c_model ON car (model);
    )" );
}

/* =========================================================== */

inline std::vector<NBValue> getHoryzontalHeader(int idconnect, int resnum)
{
    std::vector<NBValue> output;
    int querytype, rowsaffected, queryerror;
    nb_stop_and_settorecord( idconnect, resnum, 0, &querytype, &rowsaffected, &queryerror );

    NBValue value{};

    int isallready, numberOfReady;
    NB_HANDLE connection;
    do
    {
        nb_check_result( idconnect, &isallready, &numberOfReady, &connection );
    } while ( numberOfReady == 0 );


    if ( nb_fetch_row( connection ) != NB_OK ) return output;

    int fieldcount = nb_field_count( connection );
    for (int i = 0; i < fieldcount; i += 1)
    {
        nb_field_name( connection, i, &value );
        output.push_back(value);
    }

    nb_continue_work( idconnect );
    return output;
}

inline NBValue getFieldName(int idconnect, int resnum, int columnIndex)
{
    int querytype, rowsaffected, queryerror;
    nb_stop_and_settorecord( idconnect, resnum, 0, &querytype, &rowsaffected, &queryerror );

    int isallready, numberOfReady;
    NB_HANDLE connection;
    do
    {
        nb_check_result( idconnect, &isallready, &numberOfReady, &connection );
    } while ( numberOfReady == 0 );

    NBValue value{};

    if ( nb_fetch_row( connection ) != NB_OK ) return value;

    nb_field_name( connection, columnIndex, &value );

    nb_continue_work( idconnect );
    return value;
}

inline NBValue getFieldValue(int idconnect, int resnum, int rowIndex, int columnIndex)
{
    int querytype, rowsaffected, queryerror;
    nb_stop_and_settorecord( idconnect, resnum, rowIndex, &querytype, &rowsaffected, &queryerror );

    int isallready, numberOfReady;
    NB_HANDLE connection;
    do
    {
        nb_check_result( idconnect, &isallready, &numberOfReady, &connection );
    } while ( numberOfReady == 0 );

    NBValue value{};

    if ( nb_fetch_row( connection ) != NB_OK ) return value;

    nb_field_value( connection, columnIndex, &value );

    nb_continue_work( idconnect );
    return value;
}

struct ConnectInfo
{
    std::string queryType;
    int rowsAffected;
    int queryError;
};

inline ConnectInfo GetConnectInfo(int connectIndex, int queryIndex )
{
    ConnectInfo output;

    int querytype, rowsaffected, queryerror, recordstartindex = 0;
    nb_stop_and_settorecord( connectIndex, queryIndex, recordstartindex, &querytype, &rowsaffected, &queryerror );

    output.queryError = queryerror;
    output.rowsAffected = rowsaffected;

    switch (querytype)
    {
    case 0: output.queryType = "ERROR"; break;
    case 1: output.queryType = "SELECT"; break;
    case 2: output.queryType = "INSERT"; break;
    case 3: output.queryType = "UPDATE"; break;
    case 4: output.queryType = "DELETE"; break;
    case 5: output.queryType = "TRANSACTION"; break;
    default: output.queryType = "ANOTHER"; break;
    }
    nb_continue_work( connectIndex );
    return output;
}
