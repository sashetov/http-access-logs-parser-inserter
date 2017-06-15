#ifndef __HTTPACCESS_MYSQL__
#include "htlog_mysql.h"
void finish_with_error( MYSQL *con ) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}
MYSQL * get_my_conn( char *my_hostname, char *my_user, char *my_password, char * my_db, int my_port  ){
  MYSQL *con = mysql_init(NULL);
  if (con == NULL) {
    fprintf(stderr, "mysql_init() failed\n");
    exit(1);
  }
  if (mysql_real_connect(con, my_hostname, my_user, my_password, my_db, my_port, NULL, 0) == NULL) {
    finish_with_error(con);
  }
  return con;
}
struct mysql_domain_resultset* get_real_did_results( MYSQL * con, int possible_did ) {
  struct mysql_domain_resultset * results  =  malloc(sizeof(struct mysql_domain_resultset ));
  char * query_real_did = "SELECT d1.did, d1.uid, d1.domain_name FROM domains d1 JOIN domains d2 on d1.did=d2.alias_of WHERE d1.alias_of=0 AND d1.did=%d OR d2.did=%d;";
  char * query = malloc(snprintf(NULL, 0, query_real_did, possible_did, possible_did ) + 1);
  sprintf(query, query_real_did, possible_did, possible_did );
  if (mysql_query(con, query)){
    finish_with_error(con);
  }
  MYSQL_RES *result = mysql_store_result(con);
  if (result == NULL) {
    finish_with_error(con);
  }
  int num_fields = mysql_num_fields(result);
  int did, uid;
  char * domain_name;
  MYSQL_ROW row;
  while ((row = mysql_fetch_row(result))) { 
    did = atoi(row[0]? row[0] : "0");
    uid = atoi(row[1]? row[1] : "0");
    (* results).domain_name= malloc(snprintf(NULL, 0, "%s", row[2]) + 1);
    sprintf((* results).domain_name, "%s", row[2]);
    break;
  }
  (* results).did          = did;
  (* results).uid          = uid;
  mysql_free_result(result);
  //mysql_close(con);
  return results;
}
int get_did_for_domain_name( MYSQL* con, char * domain_name ) {
  char * query_possible_id = "SELECT did FROM domains WHERE domain_name=\"%s\" LIMIT 1;";
  char * query = malloc(snprintf(NULL, 0, query_possible_id, domain_name ) + 1);
  sprintf(query, query_possible_id, domain_name );
  if (mysql_query(con, query)){
    finish_with_error(con);
  }
  MYSQL_RES *result = mysql_store_result(con);
  if (result == NULL) {
    finish_with_error(con);
  }
  int num_fields = mysql_num_fields(result);
  int did;
  MYSQL_ROW row;
  while ((row = mysql_fetch_row(result))) { 
    did = atoi(row[0]? row[0] : "0");
    //only first is needed
    break;
  }
  mysql_free_result(result);
  mysql_close(con);
  return did;
}
struct mysql_domain_resultset * get_real_domain_results ( char * domain_name ){
  MYSQL *con = get_my_conn( MY_DB_SELECTS_HOST, MY_USERNAME, MY_PASSWORD,
                            MY_DB_SELECTS_DBNAME, MY_DB_SELECTS_PORT );
  int possible_did = get_did_for_domain_name( con, domain_name );
  struct mysql_domain_resultset * res = get_real_did_results( con, possible_did );
  return res;
}
int get_real_did( char * domain_name ) {
  struct mysql_domain_resultset * drs = get_real_domain_results( domain_name );
  return drs->did;
}
void print_metric_node_details ( node * n ) {
  printf( "%s %d\n", n->name, n->nval );
}
void print_ip_node_details( node *n ) {
  char * ip = n->name;
  char * country_name = (char *) get_geoip_country(0, ip);
  unsigned long n_ip = get_numeric_ip(ip);
  printf( "%s %s %d %lu \n", country_name,  n->name, n->nval, n_ip );
}
void print_ip_sql( void * arg ){
  sql_node_t * sqln = (sql_node_t *)arg;
  char * ip = sqln->n->name;
  unsigned long n_ip = get_numeric_ip(ip);
  char * numstr_template = "%lu,";
  size_t len = strlen(sqln->sql);
  char * ipnum = malloc( snprintf(NULL, 0, numstr_template, n_ip) + 1 );
  sprintf( ipnum, numstr_template, n_ip );
  sqln->sql= (char *)realloc( sqln->sql, snprintf(NULL,0,"%s%s",sqln->sql, ipnum) + 1 );
  sprintf( sqln->sql,"%s%s", sqln->sql, ipnum);
}
int insert_h_metrics( httpaccess_metrics *h_metrics ) {
  linked_list_t* uniq_ips = ht_get_all_keys(h_metrics->client_ips);
  char * sql_prefix = "INSERT INTO statistics_entities.ips(ipv4) VALUES (";
  char * sql_suffix = ");";
  sql_node_t * sql_n = (sql_node_t *) malloc( sizeof(sql_node_t) );
  sql_n->n = (node *) malloc(sizeof(node));
  sql_n->sql =  (char*) malloc(strlen(sql_prefix));
  strcpy( sql_n->sql, sql_prefix );
  iterate_all_linklist_nodes( uniq_ips, print_ip_sql, (void *) sql_n );
  size_t len = strlen(sql_n->sql);
  //chop off the last char
  sql_n->sql[strlen(sql_n->sql)-1] = '\0';
  sql_n->sql= (char *)realloc( sql_n->sql, strlen(sql_n->sql) + 2 + 1 );
  sprintf( sql_n->sql,"%s%s", sql_n->sql, ");");
  printf("%s \n", sql_n->sql );
  return 0;
}
int iterate_all_linklist_nodes( linked_list_t* linkedl, void *cb(void *), void * arg ){
  int c = list_count(linkedl);
  list_entry_t* head = linkedl->head;
  list_entry_t* tail = linkedl->tail;
  list_entry_t* next = head->next;
  sql_node_t* sqln   = (sql_node_t *) arg;
  sqln->n            = (node *) next->value;
  while( next != tail ){
    cb((void*) sqln );
    next = next->next;
    sqln->n = (node *) next->value;
  }
  cb((void*) sqln );
};
#endif
