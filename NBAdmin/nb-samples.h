#pragma once
#include "nitrosbase.h"
#include <iostream>
#include <string>

//#ifndef TESTPROJECT
//#pragma comment( lib, "nbclient.lib" )
//#endif


template<size_t N> inline NB_ERROR_TYPE nb_execute_sql( NB_HANDLE connection, const char16_t( &str )[N] )
{
    return nb_execute_sql( connection, (const char16_t *)str, N - 1 );
}

template<size_t N> inline NB_ERROR_TYPE nb_prepare_query( NB_HANDLE connection, const char16_t( &str )[N], NB_HANDLE *queryh )
{
    return nb_prepare_query( connection, (const char16_t *)str, N - 1, queryh );
}

// перегрузка оператора << для удобства вывода значений типа NBValue
inline std::ostream &operator << ( std::ostream &s, const NBValue &v )
{
    if ( v.null )
        s << "NULL";
    else
    {
        switch ( v.type )
        {
            case NB_DATA_INT: s << v.intv; break;
            case NB_DATA_DATETIME:
            case NB_DATA_STRING:
            case NB_DATA_U16STRING:
                s.write( v.str, v.len ); break;
            case NB_DATA_DECIMAL:
                s.write(v.str, v.len); break;
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
        CREATE TABLE car (id nvarchar primary key, model nvarchar, year int, color nvarchar, number nvarchar, owner nvarchar, FOREIGN KEY (owner) REFERENCES person );
        CREATE TABLE friends AS EDGE person person;
        CREATE TABLE owner AS EDGE car person;
    
        BULK INSERT person (id, name, lastname, age, city, income, dbl, dtime) FROM '../samples.data/sntest1/person.csv' WITH (FIRSTROW = 2, FIELDTERMINATOR = ',');
        BULK INSERT car (id, model, year, color, number, owner) FROM '../samples.data/sntest1/car.csv' WITH (FIRSTROW = 2, FIELDTERMINATOR = ',');
        BULK INSERT owner FROM '../samples.data/sntest1/owner.csv' WITH (FIRSTROW = 2, FIELDTERMINATOR = ',');
        BULK INSERT friends FROM '../samples.data/sntest1/friends.csv' WITH (FIRSTROW = 2, FIELDTERMINATOR = ',');
            
        CREATE INDEX p_age ON person (age);
        CREATE INDEX p_city ON person (city);
        CREATE INDEX c_model ON car (model);
    )" );
}

