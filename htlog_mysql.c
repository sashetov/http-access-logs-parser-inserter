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
int get_did_for_domain_name( MYSQL * con,  char * domain_name ) {
  char * query_possible_id = "SELECT did FROM domains WHERE domain_name=\"%s\" LIMIT 1;";
  char * query = (char * ) malloc(snprintf(NULL, 0, query_possible_id, domain_name ) + 1);
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
  //mysql_close(con);
  return did;
}
mysql_domain_resultset_t * get_real_did_uid_from_possible( MYSQL * con, int possible_did ) {
  mysql_domain_resultset_t *results =  ( mysql_domain_resultset_t * ) malloc(
      sizeof(mysql_domain_resultset_t) );
  char * query_real_did = "SELECT d1.did, d1.uid, d1.domain_name FROM domains d1 JOIN domains d2 on d1.did=d2.alias_of WHERE d1.alias_of=0 AND d1.did=%d OR d2.did=%d;";
  char * query = (char *) malloc(snprintf(NULL, 0, query_real_did, possible_did, possible_did ) + 1);
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
    (* results).domain_name= (char *)malloc(snprintf(NULL, 0, "%s", row[2]) + 1);
    sprintf((* results).domain_name, "%s", row[2]);
    break;
  }
  (* results).did          = did;
  (* results).uid          = uid;
  mysql_free_result(result);
  return results;
}
mysql_domain_resultset_t * get_real_did_uid( char * domain_name ) {
  MYSQL * conn = get_my_conn( MY_DB_SELECTS_HOST, MY_USERNAME, MY_PASSWORD, MY_DB_SELECTS_DBNAME, MY_DB_SELECTS_PORT );
  int possible_did = get_did_for_domain_name( conn, domain_name );
  mysql_domain_resultset_t * drs = get_real_did_uid_from_possible( conn, possible_did );
  mysql_close(conn);
  return drs;
}
void print_metric_node_details ( node * n ) {
  printf( "%s %d\n", n->name, n->nval );
}
void build_ips_sql( void * arg ){
  sql_node_t * sqln = (sql_node_t *)arg;
  char * ip = sqln->n->name;
  unsigned long n_ip = get_numeric_ip(ip);
  char * numstr_template = "%lu,";
  size_t len = strlen(sqln->sql);
  char * ipnum = (char *)malloc( snprintf(NULL, 0, numstr_template, n_ip) + 1 );
  sprintf( ipnum, numstr_template, n_ip );
  sqln->sql= (char *)realloc( sqln->sql, snprintf(NULL,0,"%s%s",sqln->sql, ipnum) + 1 );
  sprintf( sqln->sql,"%s%s", sqln->sql, ipnum);
}
void build_locations_sql( void * arg ){
  sql_node_t * sqln = (sql_node_t *)arg;
  char * country = sqln->n->name;
  char * loc_template = "\"%s\",";
  size_t len = strlen(sqln->sql);
  char * country_quoted= (char * )malloc( snprintf(NULL, 0, loc_template, country) + 1 );
  sprintf( country_quoted, loc_template, country );
  sqln->sql= (char *)realloc( sqln->sql, snprintf(NULL,0,"%s%s",sqln->sql, country_quoted) + 1 );
  sprintf( sqln->sql,"%s%s", sqln->sql, country_quoted );
}
sql_node_t * get_ips_insert_sql( httpaccess_metrics* h_metrics ){
  linked_list_t* uniq_ips = ht_get_all_keys(h_metrics->client_ips);
  char * sql_prefix = "INSERT INTO httpstats_clients.ips(ipv4) VALUES (";
  char * sql_suffix = ");";
  sql_node_t * ips_sql_n= (sql_node_t *) malloc( sizeof(sql_node_t) );
  ips_sql_n->n = (node *) malloc(sizeof(node));
  ips_sql_n->sql =  (char*) malloc(strlen(sql_prefix));
  strcpy( ips_sql_n->sql, sql_prefix );
  iterate_all_linklist_nodes( uniq_ips, (void* (*)(void*)) build_ips_sql, ips_sql_n );
  size_t len = strlen(ips_sql_n->sql);
  //chop off the last char
  ips_sql_n->sql[strlen(ips_sql_n->sql)-1] = '\0';
  ips_sql_n->sql= (char *)realloc( ips_sql_n->sql, strlen(ips_sql_n->sql) + 2 + 1 );
  sprintf( ips_sql_n->sql,"%s%s", ips_sql_n->sql, ");");
  return ips_sql_n;
}
sql_node_t * get_countries_insert_sql( httpaccess_metrics* h_metrics ){
  linked_list_t* uniq_locations = ht_get_all_keys(h_metrics->client_geo_locations);
  char * sql_prefix = "INSERT INTO httpstats_clients.locations(name) VALUES (";
  char * sql_suffix = ");";
  sql_node_t * countries_sql_n= (sql_node_t *) malloc( sizeof(sql_node_t) );
  countries_sql_n->n = (node *) malloc(sizeof(node));
  countries_sql_n->sql =  (char*) malloc(strlen(sql_prefix));
  strcpy( countries_sql_n->sql, sql_prefix );
  iterate_all_linklist_nodes( uniq_locations, (void* (*)(void*)) build_locations_sql, (void *) countries_sql_n );
  size_t len = strlen(countries_sql_n->sql);
  //chop off the last char
  countries_sql_n->sql[strlen(countries_sql_n->sql)-1] = '\0';
  countries_sql_n->sql= (char *)realloc( countries_sql_n->sql, strlen(countries_sql_n->sql) + 2 + 1 );
  sprintf( countries_sql_n->sql,"%s%s", countries_sql_n->sql, ");");
  return countries_sql_n;
}
int insert_h_metrics( httpaccess_metrics *h_metrics ) {
  sql_node_t * ips_sql_n = get_ips_insert_sql( h_metrics );
  sql_node_t * countries_sql_n = get_countries_insert_sql( h_metrics );
  //printf("%s\n%s\n", ips_sql_n->sql, countries_sql_n->sql);
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
