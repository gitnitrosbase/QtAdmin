#pragma once


#include "nitrosbase.h"
#include <iostream>
#include <mutex>
#include <atomic>
#include <vector>
#include <thread>
#include <chrono>

#ifdef _DEBUG
#pragma comment( lib, "nbclient-dbg.lib" )
#else
#pragma comment( lib, "nbclient.lib" )
#endif

#pragma comment( lib, "nbclient.lib" )

struct MyException : public std::exception
{
    std::string s;
    MyException( std::string ss ) : s( ss ) {}
    ~MyException() throw ( ) {} // Updated
    const char *what() const throw( ) { return s.c_str(); }
};

#pragma pack()
struct queryresultbase
{
    NB_HANDLE connection, filebuffer, recordsstream;
    std::atomic<int> state = 0;
    std::atomic<int> needstop = 0;
    std::atomic<int> needstopall = 0;
    std::atomic<int> ready = true;
    int qcount = 0;
    NB_ERROR_TYPE err = NB_OK;

    void wait4thread() {
        needstop = 1;
        state.wait( 0 );
    }
    void lock()
    {
        needstop = 1;
        //needstop.notify_one();
        state.wait( 0 );
    }
    void unlock() {
        needstop = 0;
        needstop.notify_one();
    }
};

struct fielddescr {
    std::string name;
    NB_DATA_TYPE type;
    NB_DATA_TYPE internaltype;
    int offset;
    int precision;
    bool datetime2microsec;
};

struct queryres {
    int type, count;
    std::vector<fielddescr> fields;
    std::vector<size_t> records;
    bool ready = false;
};

struct queryresult : queryresultbase
{

    std::vector<queryres> queries;
    std::string errstr;

    void clear() {
        err = NB_OK;
        queries.clear();
        qcount = 0;
        ready = false;
        state = 0;
        needstopall = false;
    }

    void wait4thread() {
        state.wait( 0 );
    }
    void lock()
    {
        if ( state == 3 )
            return;
        state = 2;
        state.notify_one();
        state.wait( 2 );
    }
    void unlock() {
        state = 1;
        state.notify_one();
    }
};

#pragma pack()

extern "C"
{
    NB_EXTERN void *nb_connect2( const char16_t *host, int sockport, const char16_t *user, const char16_t *password );
    queryresultbase *nb_prepare_result( NB_HANDLE connection, int connectid );
    void nb_getresults( queryresultbase *resultbase );
    void nb_deleteres( queryresultbase *resultbase );
    void nb_clearres( queryresultbase *resultbase );

    NB_EXTERN char *nb_getrecordptr( NB_HANDLE filenum, size_t offset );
    NB_EXTERN NB_ERROR_TYPE nb_field_value_ext( NB_HANDLE connection, NBValue *v, char *recptr, fielddescr &fd );
}

#define MAXCONNECT 64

extern struct queryresultbase;
struct ConnectDescriptor
{
    NB_HANDLE connect;
    int port;
    queryresultbase *result;
    std::mutex mtx;
};

struct ConnectPool3
{
    ConnectDescriptor connects[MAXCONNECT];
    ConnectPool3()
    {
        for ( int i = 0; i < MAXCONNECT; i++ )
        {
            auto &d = connects[i];
            d.connect = nullptr;
            d.port = -1;
            d.result = nullptr;
        }
    }
    ~ConnectPool3()
    {
        CloseAllConnects();
    }

    ConnectDescriptor &GetConnect( int idconnect, int port )
    {
        check( idconnect );
        ConnectDescriptor &d = connects[idconnect];
        if ( d.connect != nullptr && d.port != port )
            CloseConnect( idconnect );
        if ( d.connect == nullptr )
        {
            d.connect = nb_connect2( u"127.0.0.1", port, u"TESTUSER", u"1234" );
            if ( d.connect == nullptr )
                throw MyException( "Connection error" );
            d.port = port;
            d.result = nullptr;
        }

        return d;
    }

    void check( int idconnect ) {
        if ( idconnect < 0 || idconnect >= MAXCONNECT )
            throw std::runtime_error( "idconnect must be in [0...63]" );
    }

    queryresultbase *InitResult( int idconnect, int port )
    {
        ConnectDescriptor &d = GetConnect( idconnect, port );
        if ( d.result )
        {
            d.result->needstopall = true;
            d.result->ready.wait( 0 );
            nb_clearres( d.result );
        }
        else
            d.result = nb_prepare_result( d.connect, idconnect );
        return d.result;
    }

    void CloseConnect( int idconnect )
    {
        ConnectDescriptor &d = connects[idconnect];
        if ( d.connect )
        {
            if ( d.result )
            {
                d.result->needstopall = true;
                d.result->ready.wait( 0 );
                nb_deleteres( d.result );
            }
            nb_disconnect( d.connect );
            d.connect = nullptr;
            d.port = -1;
            d.result = nullptr;
        }
    }

    void CloseAllConnects() {
        for ( int i = 0; i < MAXCONNECT; i++ )
            CloseConnect( i );
    }

    void CloseConnectsToPort( int port ) {
        for ( int i = 0; i < MAXCONNECT; i++ )
        {
            if ( connects[i].port == port )
                CloseConnect( i );
        }
    }
};

extern ConnectPool3 nbpool;

inline NB_HANDLE getconnect( int idconnect, int port )
{
    ConnectDescriptor &d = nbpool.GetConnect( idconnect, port );
    return d.connect;
}

inline void closeconnect( int idconnect, int port )
{
    if ( idconnect == 10000 )
        nbpool.CloseAllConnects();
    else
        nbpool.CloseConnect( idconnect );
}

inline void closeconnect( int port )
{
    nbpool.CloseConnectsToPort( port );
}

inline void AsyncProcessQuery( queryresultbase *result, const char *str, size_t len )
{
    result->ready = false;
    nb_execute_sql_utf8( result->connection, str, len );
    //check_error( result->connection );
    nb_getresults( result );
}

struct lockres
{
    queryresultbase *result;
    lockres( queryresultbase *result ) : result( result ) {
        result->lock();
    }
    ~lockres() {
        result->unlock();
    }
};


inline void PrintResultInfo( queryresultbase *resultbase, int skip )
{
    std::cout << "---------------------------\n";

    queryresult *result = (queryresult *)resultbase;
    int count = result->queries.size();
    for ( int i = skip; i < count; i++ )
    {
        queryres &res = result->queries[i];
        std::cout << "query " << i << std::endl;
        std::cout << "type " << res.type << std::endl;
        std::cout << "ready " << res.ready << std::endl;
        int err = ( i < count - 1 ) ? 0 : result->err;
        std::cout << "err " << err << std::endl;
        if ( err )
            std::cout << "msg " << result->errstr << std::endl;

        if ( res.type != SQL_QUERY_SELECT )
            std::cout << "rowsaffected " << res.count << std::endl;
        else
            std::cout << "rowscount " << res.records.size() << std::endl;

        std::cout << "fields" << std::endl;

        int fcount = res.fields.size();
        for ( int j = 0; j < fcount; j++ )
        {
            auto &f = res.fields[j];
            std::cout << f.name << std::endl;
        }
    }
}

inline void PrintValue( const NBValue &v )
{
    if ( v.null )
        std::cout << "NULL";
    else
    {
        switch ( v.type )
        {
            case NB_DATA_INT: std::cout << v.intv; break;
            case NB_DATA_DATETIME:
            case NB_DATA_STRING:
            case NB_DATA_U16STRING:
                std::cout.write( v.str, v.len ); break;
            case NB_DATA_DECIMAL:
                std::cout.write( v.str, v.len ); break;
            case NB_DATA_INT64: std::cout << v.int64v; break;
            case NB_DATA_DOUBLE: std::cout << v.dbl; break;
            case NB_DATA_BOOL: std::cout << ( ( v.intv ) ? "TRUE" : "FALSE" ); break;
            case NB_DATA_BINARY:
                std::cout.write( v.str, v.len ); break;
        }
    }
}

inline void PrintResultRecordsPart( queryresultbase *resultbase, int resnum, int recordstartindex, int recordendindex )
{
    queryresult *result = (queryresult *)resultbase;
    try
    {
        queryres &res = result->queries[resnum];
        int recodscount = (int)res.records.size();

        std::cout << "---------------------------\n";
        std::cout << "odata.count " << recodscount << std::endl;

        for ( int i = recordstartindex; i < std::min( recordendindex, recodscount ); i++ )
        {
            size_t recoffset = res.records[i];
            std::cout << "---------------------------\n";
            for ( int j = 0; j < res.fields.size(); j++ )
            {
                NBValue v;
                fielddescr &fd = res.fields[j];
                char *recpr = nb_getrecordptr( result->recordsstream, recoffset );
                nb_field_value_ext( result->connection, &v, recpr, fd );
                std::cout << j << " ";
                PrintValue( v );
                std::cout << std::endl;
            }
        }
    }
    catch ( std::exception &e )
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

inline void WaitAndPrintResult( queryresultbase *result, int skip )
{
    while ( true )
    {
        lockres lk( result );
        if ( !result->ready && result->qcount == 0 )
            continue;
        PrintResultInfo( result, skip );
        break;
    }
}

extern  queryresultbase *results[MAXCONNECT];
extern std::mutex mtxs[MAXCONNECT];
extern int ports[MAXCONNECT];

inline void ExecSqlASYNC( int idconnect, int port, const std::string &query )
{
    try
    {
        NB_HANDLE connection;
        nbpool.check( idconnect );
        const std::lock_guard<std::mutex> lock( nbpool.connects[idconnect].mtx );

        auto result = nbpool.InitResult( idconnect, port );
        result->ready = false;
        std::thread( AsyncProcessQuery, result, query.c_str(), query.length() ).detach();
        result->wait4thread();

        WaitAndPrintResult( result, 0 );
    }
    catch ( std::exception &e )
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

inline void GetSqlResulsASYNC( int idconnect, int port, int skip )
{
    try
    {
        nbpool.check( idconnect );
        auto &d = nbpool.connects[idconnect];
        if ( d.result == nullptr )
            throw std::runtime_error( "GetODATA must be called after query" );
        const std::lock_guard<std::mutex> lock( d.mtx );

        WaitAndPrintResult( d.result, skip );
    }
    catch ( std::exception &e )
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

inline void GetSqlRecordsASYNC( int idconnect, int resnum, int recordstartindex, int recordendindex)
{
    try
    {
        nbpool.check( idconnect );
        auto &d = nbpool.connects[idconnect];
        if ( d.result == nullptr )
            throw std::runtime_error( "GetODATA must be called after query" );
        const std::lock_guard<std::mutex> lock( d.mtx );

        lockres lk( d.result );

        PrintResultRecordsPart( d.result, resnum, recordstartindex, recordendindex );
    }
    catch ( std::exception &e )
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

//-------------------------------------------------

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

//--------------------------------------------------

inline std::vector<std::string> PrintResultInfo2( queryresultbase *resultbase, int queryIndex )
{
    std::vector<std::string> outputHeader;

    queryresult *result = (queryresult *)resultbase;
    int count = result->queries.size();

    std::cout<<queryIndex<<std::endl;

    queryres res = result->queries.at(queryIndex);

    int err = ( queryIndex < count - 1 ) ? 0 : result->err;

    if ( err )
        std::cout << "msg " << result->errstr << std::endl;

    if ( res.type != SQL_QUERY_SELECT )
        std::cout << "rowsaffected " << res.count << std::endl;
    else
        std::cout << "rowscount " << res.records.size() << std::endl;

    int fcount = res.fields.size();
    for ( int j = 0; j < fcount; j++ )
    {
        auto f = res.fields[j];
        std::cout << f.name << std::endl;
        outputHeader.push_back(std::string(f.name));
    }

    return outputHeader;
}

inline int GetRowCount(queryresultbase *resultbase, int queryIndex )
{
    queryresult *result = (queryresult *)resultbase;

    int rowCount = 0;

    while ( true )
    {
        lockres lk( result );
        if ( !result->ready && result->qcount == 0 )
            continue;

        int count = result->queries.size();

        queryres &res = result->queries[queryIndex];

        int err = ( queryIndex < count - 1 ) ? 0 : result->err;

        if ( err )
            std::cout << "msg " << result->errstr << std::endl;

        if ( res.type != SQL_QUERY_SELECT ) rowCount = res.count;
        else rowCount = res.records.size();


        break;
    }

    std::cout<<rowCount<<std::endl;

    return rowCount;
}

inline NBValue GetItemFromTable(queryresultbase *resultbase, int resnum, size_t columnIndex, size_t rowIndex)
{
    NBValue v;
    queryresult *result = (queryresult *)resultbase;
    try
    {
        queryres res = result->queries[resnum];
        size_t recoffset = res.records[rowIndex];
        fielddescr &fd = res.fields[columnIndex];
        char *recpr = nb_getrecordptr( result->recordsstream, recoffset );
        nb_field_value_ext( result->connection, &v, recpr, fd );

    }
    catch ( std::exception &e )
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
    return v;
}


inline std::string GetQueryType(int connectIndex, int queryIndex )
{
    std::string output;

    queryresult *result = (queryresult *)nbpool.connects[connectIndex].result;
    int count = result->queries.size();

    queryres res = result->queries.at(queryIndex);

    int err = ( queryIndex < count - 1 ) ? 0 : result->err;

    if ( err )
        std::cout << "msg " << result->errstr << std::endl;

    switch (res.type)
    {
    case 0: output = "NONE"; break;
    case 1: output = "SELECT"; break;
    case 2: output = "INSERT"; break;
    case 3: output = "UPDATE"; break;
    case 4: output = "DELETE"; break;
    case 5: output = "TRANSACTION"; break;
    default: output = "ANOTHER"; break;
    };
    return output;
}
