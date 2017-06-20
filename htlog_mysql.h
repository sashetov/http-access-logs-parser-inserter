#ifndef __HTTPACCESS_MYSQL__
#ifdef __cplusplus
extern "C" {
#endif
#define MY_DB_SELECTS_DBNAME "cluster"
#define MY_DB_SELECTS_HOST   "185.52.26.79"
#define MY_DB_SELECTS_PORT   3309
#define MY_USERNAME          "root"
#define MY_PASSWORD          "qqq"
#define __HTTPACCESS_MYSQL__
#include <my_global.h>
#include <mysql.h>
#if !defined( __HTTPACCESS_METRICS__ ) || ! defined ( __LOG_LINE__ )
#include "htlog_processing.h"
#endif
#ifndef HL_LINKLIST_H
#include "linklist.h"
#endif
typedef struct mysql_domain_resultset {
  int did;
  int uid;
  char * domain_name;
} mysql_domain_resultset_t;
typedef struct sql_node {
  node * n;
  char * sql;
} sql_node_t;
void finish_with_error( MYSQL *con );
MYSQL * get_my_conn( char *my_hostname, char *my_user, char *my_password, char * my_db, int my_port  );
struct mysql_domain_resultset * get_real_did_results( MYSQL * con, int possible_did );
int get_did_for_domain_name( MYSQL* con, char * domain_name );
mysql_domain_resultset_t * get_real_did_uid_from_possible( MYSQL * con, int possible_did );
mysql_domain_resultset_t * get_real_did_uid( char * domain_name );
void print_metric_node_details ( node * n );
void build_ips_sql( void * arg );
void build_locations_sql( void * arg );
sql_node_t * get_ips_insert_sql( httpaccess_metrics* h_metrics );
sql_node_t * get_countries_insert_sql( httpaccess_metrics* h_metrics );
int insert_h_metrics(httpaccess_metrics *h_metrics);
int iterate_all_linklist_nodes( linked_list_t* linkedl, void *cb(void *), void * arg);
#ifdef __cplusplus
}
#endif
#endif
