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
int insert_h_metrics( httpaccess_metrics *h_metrics ) {
  linked_list_t* uniq_ips = ht_get_all_keys(h_metrics->client_ips);
  iterate_all_linklist_nodes( uniq_ips );
  return 0;
}

int iterate_all_linklist_nodes( linked_list_t* linkedl ){
  int c = list_count(linkedl);
  list_entry_t* head = linkedl->head;
  list_entry_t* tail = linkedl->tail;
  list_entry_t* next = head->next;
  node * n = (node *) next->value;
  while( next != tail ){
    printf( "%s %d\n", n->name, n->nval );
    next = next->next;
    n = (node *) next->value;
  }
  printf( "%s %d\n", n->name, n->nval );
};
#endif
