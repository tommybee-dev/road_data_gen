#ifndef __ROAD_NAME_MAIN_H__
#define __ROAD_NAME_MAIN_H__


void pre_load_scheme(char *filename, char *elem, char *subelem, char **text);

void load_depth2(char *filename, char *elem, char *subelem, char **text);
void load_depth1(char *filename, char *elem, char **text);

int smpl_unzip(char *zipname);

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include "sqlite3.h"
#include "iniparser.h"
//#include "simplezip.h"


//#define TEST_DOWNLOAD
#define SECTION1 "URLINFO"
#define SECTION2 "FILEINFO"
#define SECTION3 "DATABASE"
#define SECTION4 "ORAINFO"

#define ROAD_DOWNLOAD        1
#define ROAD_UNZIP           2
#define ROAD_SQLITE          4
#define ROAD_ORACLE          8
#define ROAD_TEST           16


/*LOG LEVELS   */
typedef enum  
{   
  LOG_DEFAULT,   
  LOG_INFO,   
  LOG_ERROR,   
  LOG_DEBUG   
} LOG_LEVEL;

typedef struct table_scheme_s {
    char database[FILENAME_MAX];
    char table_name[FILENAME_MAX];
    char txt_path[FILENAME_MAX];
    char sql[10240];
} table_scheme_t;

typedef struct table_move_s {
    char sqlite_dbase[FILENAME_MAX];
    char tns[FILENAME_MAX];
    char user[512];
    char passwd[512];
    char select_buf[10240];
    char insert_buf[10240];
} table_move_t;

#define DEFULT_DB_CHARSET      "ISO8859-1"

#define INI_FILE "road.name.ini"
/*#define INI_FILE "test.ini"*/

#define SKIP_PEER_VERIFICATION
//#define SKIP_HOSTNAME_VERIFICATION
//#define WITH_CONV

int has_hangul(char *input_text);
int file_exists(const char *fname);
void load_ini(const char *szFileName);
void unload_ini();
int convStr( char *outBuf, size_t *outLength, char *inBuf, size_t inLength, const char *sEnc, const char *tEnc );

//int smpl_unzip(char *zipname);
void read_ini(const char *szSection, const char* szKey, const char *szDefaultValue, char **output_buf_ptr);
int download_http(char* szDownUrl, char *szFileName);
void create_dir(char *dir);
void LOG_TRACE(LOG_LEVEL lvl, const char *fmt, ... );
void encrypt_db(char *key, char *charset, sqlite3 *db);

char* strsep_cust(char** stringp, const char* delim);

//void pre_load_scheme(char *filename);
//int smpl_unzip(char *zipname);

int create_bldg_table(table_scheme_t *table_scheme);
int insrt_bldg_table(table_scheme_t *table_scheme);

int create_addr_table(table_scheme_t *table_scheme);
int insrt_addr_table(table_scheme_t *table_scheme);

int create_rdcode_table(table_scheme_t *table_scheme);
int insrt_rdcode_table(table_scheme_t *table_scheme);

void trim(char * const a);
bool bind_text(sqlite3 *db, sqlite3_stmt *stmt, int index, char *pstr, int sz);
int conv_utf8_kr(char *szResult, char **output_buf_ptr);
int conv_kr_utf8(char *inStr, char **output_buf_ptr);

void open_iconv(const char *src, const char *dest);
int iconv_utf8_kr(char *inStr, char **output_buf_ptr);
void close_iconv();

void log_print_error(const char* message, ...);


int ora_test_road (void);
int add_addr (table_move_t *table_scheme);

#ifdef __cplusplus
}
#endif

#endif
