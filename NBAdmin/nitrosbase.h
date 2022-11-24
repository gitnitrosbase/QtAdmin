#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef void *NB_HANDLE;

// порт NitrosBase по умолчанию
const int NB_DEFAULT_PORT = 3020;

enum NB_ERROR_TYPE
{
    NB_OK,
    NB_NO_DATA,
    NB_PLAN,

    NB_ERROR_FILE = 100,
    NB_ERROR_MEM,
    NB_ERROR_ARGS,
    NB_ERROR_CPPFUNC,
    NB_ERROR_QUERY,
    NB_ERROR_CONNECT,
    NB_ERROR_CONNECTION_LOST,
    NB_ERROR_TRANSACTION_CONFLICT,
    NB_ERROR_FUNCTION_ORDER,

    NB_ERROR_COMPUTING,
    NB_ERROR_DATA_CONVERT,
    NB_ERROR_UNIQUE_OR_KEY,

    NB_ERROR_ANOTHER = 200
};

#define NB_ERROR_TYPE_DEF
#define NB_SUCCESS NB_OK
#define NB_INVALID_HANDLE nullptr
#define NB_INFINITE_TIME -1

enum SQL_QUERY_TYPE
{
    SQL_QUERY_NONE,
    SQL_QUERY_SELECT,
    SQL_QUERY_INSERT,
    SQL_QUERY_UPDATE,
    SQL_QUERY_DELETE,
    SQL_QUERY_TRANSACTION,
    SQL_QUERY_ANOTHER
};

enum NB_DATA_TYPE
{
    NB_DATA_NONE,
    //database fieldschema types
    NB_DATA_STRING,
    NB_DATA_INT,
    NB_DATA_INT64,
    NB_DATA_DOUBLE,
    NB_DATA_DATETIME,
    NB_DATA_BOOL,
    NB_DATA_DATE,
    NB_DATA_URI,
    NB_DATA_BINARY,
    NB_DATA_U16STRING,
    NB_DATA_ROWVERSION,
    NB_DATA_DECIMAL,
};

#pragma pack(4)
struct NBValue
{
    NB_DATA_TYPE type;
    union
    {
        int32_t intv;
        int64_t int64v;
        double dbl;

        struct
        {
            char *str;
            int32_t len;
            uint64_t blobnextpart;
            int blobfield;
        };
    };
    bool null;
};
#pragma pack()

enum JSONOP
{
    JSON_INSERT_UPDATE,
    JSON_DELETE,
};

enum NB_CONN_OPTIONS
{
    NB_OPT_TIMEOUT,          // настройка таймаута ожидания ответа на запрос
    NB_OPT_AVGINTASDOUBLE    //avg(intfield) возвращает double значение если NB_OPT_AVGINTASDOUBLE = 1 и возвращает int значение (как MSSQL) если равно 0
};

#   ifdef __linux__
#		define NB_EXTERN __attribute__((visibility("default")))
#   elif defined(_WIN32)
#		define NB_EXTERN __declspec(dllexport)
#   else
#       error Not supported OS!
#   endif

enum DATABASE_QUERY_TYPE
{
    DATABASE_PROCEDURE_WITH_PARAMS,

    DATABASE_UPDATE, //reserved
    DATABASE_REPLACE, //reserved
    DATABASE_DELETE, //reserved

    DATABASE_SQL_QUERY,
    //DATABASE_SQL_QUERY_WITH_PARAMS,
    DATABASE_SQL_QUERYEXECUTIONPLAN,

    //prepared queries
    DATABASE_SQL_QUERY_PREPARE,
    DATABASE_SQL_EXEC_PREPARED,
    DATABASE_SQL_CLOSE_PREPARED,

    //bulk mode
    DATABASE_BULK_START,
    DATABASE_BULK_STOP,
    DATABASE_BULK_GETINFO,

    DATABASE_BULKINSERT, //reserved
    DATABASE_INSERTFROMLOG,
    DATABASE_UPDATEFROMLOG,
    DATABASE_DELETEFROMLOG,

    DATABASE_INSERTLINKFROMLOG,
    DATABASE_DELETELINKFROMLOG,
};

enum DATABASE_QUERY_SUBTYPE
{
    EXT_DATA_NONE,
    EXT_DATA_JSON,
    EXT_DATA_JSON_FILE,
    EXT_RESERVED,
    EXT_RESERVED2,
    EXT_RESERVED3,
    EXT_RESERVED4,
    DATABASE_CSV,
    DATABASE_CSV_FILE,
    EXT_DATA_DELETE,

    EXT_PREPARE_INTPARAM,
    EXT_PREPARE_INT64PARAM,
    EXT_PREPARE_DOUBLEPARAM,
    EXT_PREPARE_DECIMALPARAM,
};

enum DATABASE_COMMONPROC
{
    PROC_GETTABLES,
    PROC_GETTABLESCHEMA,
    PROC_GETINDEXES,
    PROC_COUNTALLRECORDS,
    PROC_GETTABLEINDEXES,
};

extern "C" {

    //CONNECT TO DB

    NB_EXTERN NB_HANDLE nb_connect( const char16_t *host, int sockport, const char16_t *user = nullptr, const char16_t *password = nullptr );
    NB_EXTERN NB_HANDLE nb_connect_utf8( const char *host, int sockport, const char *user = nullptr, const char *password = nullptr );
    NB_EXTERN NB_HANDLE nb_clone_connect( NB_HANDLE connection );
    NB_EXTERN NB_ERROR_TYPE nb_disconnect( NB_HANDLE connection );
    NB_EXTERN size_t nb_get_connectid( NB_HANDLE connection );


    //EXECUTE QUERIES

    NB_EXTERN NB_ERROR_TYPE nb_execute_sql( NB_HANDLE connection, const char16_t *query, size_t length );
    NB_EXTERN NB_ERROR_TYPE nb_execute_sql_utf8( NB_HANDLE connection, const char *query, size_t length );
    NB_EXTERN NB_ERROR_TYPE nb_get_change_info( NB_HANDLE connection, int *changetype, int *changecount );

    NB_EXTERN NB_ERROR_TYPE nb_execute_sql_with_params( NB_HANDLE connection, const char16_t *query, size_t length );
    NB_EXTERN NB_ERROR_TYPE nb_execute_sql_with_params_utf8( NB_HANDLE connection, const char *query, size_t length );
    //GET QUERY RESULT

    NB_EXTERN NB_ERROR_TYPE nb_fetch_row( NB_HANDLE connection );
    
    NB_EXTERN NB_ERROR_TYPE nb_field_value( NB_HANDLE connection, int fieldnum, NBValue *v );
    NB_EXTERN NB_ERROR_TYPE nb_field_value_utf8( NB_HANDLE connection, int fieldnum, NBValue *v );
    NB_EXTERN NB_ERROR_TYPE nb_field_value_utf16( NB_HANDLE connection, int fieldnum, NBValue *v );
    NB_EXTERN NB_ERROR_TYPE nb_blob_nextpart( NB_HANDLE connection, NBValue *value );
    NB_EXTERN NB_ERROR_TYPE nb_blob_nextpart_utf8( NB_HANDLE connection, NBValue *value );

    NB_EXTERN int nb_field_count( NB_HANDLE connection );
    NB_EXTERN NB_ERROR_TYPE nb_field_name( NB_HANDLE connection, int fieldnum, NBValue *name );
    NB_EXTERN NB_ERROR_TYPE nb_field_name_utf8( NB_HANDLE connection, int fieldnum, NBValue *name );
    NB_EXTERN NB_DATA_TYPE nb_field_type( NB_HANDLE connection, int fieldnum );
    
    //READ MULTIPLE RESULT SETS

    NB_EXTERN bool nb_nextresult( NB_HANDLE connection );

    //ERROR PROCESSING

    NB_EXTERN NB_ERROR_TYPE nb_errno( NB_HANDLE connection );
    NB_EXTERN const char16_t * nb_err_text( NB_HANDLE connection, size_t * len = nullptr );
    NB_EXTERN const char * nb_err_text_utf8( NB_HANDLE connection, size_t * len = nullptr );

    //GET SCHEMA

    NB_EXTERN NB_ERROR_TYPE nb_get_tableslist( NB_HANDLE connection );
    //NB_EXTERN NB_ERROR_TYPE nb_get_tableschema( NB_HANDLE connection, const char *query, size_t length );
    NB_EXTERN NB_ERROR_TYPE nb_get_tableschema( NB_HANDLE connection, const char16_t *query, size_t length );
    NB_EXTERN NB_ERROR_TYPE nb_get_indexschema( NB_HANDLE connection );
    NB_EXTERN NB_ERROR_TYPE nb_get_tableindexschema( NB_HANDLE connection, const char16_t *query, size_t length );
    NB_EXTERN NB_ERROR_TYPE nb_get_tableindexschema_utf8( NB_HANDLE connection, const char *query, size_t length );


    //PREPARED QUERIES AND QURIES WITH PARAMETERS

    NB_EXTERN NB_ERROR_TYPE nb_prepare_query( NB_HANDLE connection, const char16_t *querystring, size_t length, NB_HANDLE *queryh );
    NB_EXTERN NB_ERROR_TYPE nb_prepare_query_utf8( NB_HANDLE connection, const char *querystring, size_t length, NB_HANDLE *queryh );
    NB_EXTERN NB_ERROR_TYPE nb_start_execute_prep( NB_HANDLE connection, NB_HANDLE queryh );
    NB_EXTERN NB_ERROR_TYPE nb_write_param_int( NB_HANDLE connection, int value );
    NB_EXTERN NB_ERROR_TYPE nb_write_param_int64( NB_HANDLE connection, int64_t value );
    NB_EXTERN NB_ERROR_TYPE nb_write_param_double( NB_HANDLE connection, double value );
    NB_EXTERN NB_ERROR_TYPE nb_write_param_str( NB_HANDLE connection, const char16_t *str, size_t len );
    NB_EXTERN NB_ERROR_TYPE nb_write_param_str_utf8( NB_HANDLE connection, const char *str8, size_t len );
    NB_EXTERN NB_ERROR_TYPE nb_write_param_binary( NB_HANDLE connection, const void *ptr, size_t len );
    NB_EXTERN NB_ERROR_TYPE nb_start_param_blob( NB_HANDLE connection, NB_DATA_TYPE datatype );
    NB_EXTERN NB_ERROR_TYPE nb_write_blob_part( NB_HANDLE connection, const void *ptr, size_t len );
    NB_EXTERN NB_ERROR_TYPE nb_end_param_blob( NB_HANDLE connection );
    NB_EXTERN NB_ERROR_TYPE nb_write_param_null( NB_HANDLE connection );

    NB_EXTERN NB_ERROR_TYPE nb_write_param_literal(NB_HANDLE connection, const char16_t* str, size_t len, NB_DATA_TYPE type);
    NB_EXTERN NB_ERROR_TYPE nb_write_param_literal_utf8(NB_HANDLE connection, const char* str, size_t len, NB_DATA_TYPE type);


    NB_EXTERN NB_ERROR_TYPE nb_execute_prep( NB_HANDLE connection );
    NB_EXTERN NB_ERROR_TYPE nb_close_prepared_query( NB_HANDLE connection, NB_HANDLE queryh );

    //BULK INSERT

    NB_EXTERN NB_ERROR_TYPE nb_start_bulk( NB_HANDLE connection );
    NB_EXTERN NB_ERROR_TYPE nb_get_bulk_info( NB_HANDLE connection );
    NB_EXTERN NB_ERROR_TYPE nb_stop_bulk( NB_HANDLE connection );

    //MISC

    NB_EXTERN NB_ERROR_TYPE nb_execute_sql_async( NB_HANDLE connection, const char16_t *query, size_t length );
    NB_EXTERN NB_ERROR_TYPE nb_get_queryexecutionplan( NB_HANDLE connection, const char16_t *query, size_t length );
    NB_EXTERN NB_ERROR_TYPE nb_isdataready( NB_HANDLE connection );
    NB_EXTERN void nb_wait4queryend( NB_HANDLE connection );
    NB_EXTERN void nb_setoption( NB_HANDLE connection, NB_CONN_OPTIONS option, int value );
    NB_EXTERN int nb_getoption( NB_HANDLE connection, NB_CONN_OPTIONS option );
    NB_EXTERN const char16_t *nb_get_build( NB_HANDLE connection );
    NB_EXTERN const char *nb_get_build_utf8( NB_HANDLE connection );
    NB_EXTERN size_t nb_blob_chunck_size( NB_HANDLE connection );
}

