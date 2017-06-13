#ifndef __HTTPACCESS_MYSQL__
#define MY_DB_SELECTS_DBNAME "cluster"
#define MY_DB_SELECTS_HOST   "localhost"
#define MY_DB_SELECTS_PORT   3307
#define MY_USERNAME          "root"
#define MY_PASSWORD          "qqq"
#include <my_global.h>
#include <mysql.h>
#define __HTTPACCESS_MYSQL__
struct mysql_domain_resultset {
  int did;
  int uid;
  char * domain_name;
};
MYSQL * get_my_conn( char *my_hostname, char *my_user, char *my_password,
                     char * my_db, int my_port  );
int get_did_for_domain_name( MYSQL *con, char * domain_name );
struct mysql_domain_resultset * get_domain_results( char * domain_name );
int get_real_did( char * domain_name );
void finish_with_error( MYSQL *con );
#endif
