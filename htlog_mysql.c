#ifndef __HTTPACCESS_MYSQL__
#include "htlog_mysql.h"
void free_sql_node( sql_node_t * n ){
  free_node( n->n, "sql_node" );
  free( n->sql );
  free( n );
}
void free_sql_name_vers_node( sql_name_version_node_t * n){
  free_name_version_node(n->n, "sql_nv_node");
  free( n->sql );
  free( n );
}
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
  free(query);
  return did;
}
char ** get_domain_aliases( MYSQL * con, char * domain_name ){
  char ** hostnames = (char **)malloc( sizeof(char *));
  char * query_template = "SET @did=(SELECT did FROM domains where domain_name ='%s'); SELECT domain_name FROM domains WHERE alias_of=@did OR did=@did;";
  char * query = (char *) malloc(snprintf(NULL, 0, query_template, domain_name ) + 1);
  sprintf(query, query_template, domain_name );
  if (mysql_query(con, query)){
    finish_with_error(con);
  }
  MYSQL_RES *result = mysql_store_result(con);
  int num_fields = mysql_num_fields(result);
  MYSQL_ROW row;
  int i = 0;
  while ((row = mysql_fetch_row(result))) { 
    char * hostname = row[0] ? row[0] : (char *)"";
    hostnames[i] = (char *) malloc(strlen(hostname)+1);
    strcpy( hostnames[i], hostname );
    i++;
  }
  free(query);
  return hostnames;
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
  free(query);
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
  char * ipnum = (char *)malloc( snprintf(NULL, 0, numstr_template, n_ip) + 1 );
  sprintf( ipnum, numstr_template, n_ip );
  sqln->sql= (char *)realloc( sqln->sql, snprintf(NULL,0,"%s%s",sqln->sql, ipnum) + 1 );
  sprintf( sqln->sql,"%s%s", sqln->sql, ipnum);
  sqln->num_rows++;
  free(ipnum);
}
void build_name_sql( void * arg ){
  sql_node_t * sqln = (sql_node_t *)arg;
  char * name = sqln->n->name;
  char * loc_template = "\"%s\",";
  char * name_quoted= (char * ) malloc( snprintf(NULL, 0, loc_template, name) + 1 );
  sprintf( name_quoted, loc_template, name );
  sqln->sql= (char *)realloc( sqln->sql, snprintf(NULL,0,"%s%s",sqln->sql, name_quoted) + 1 );
  sprintf( sqln->sql,"%s%s", sqln->sql, name_quoted );
  sqln->num_rows++;
  free(name_quoted);
}
void build_params_sql( void * arg ){
  int i;
  sql_node_t * sqln = (sql_node_t *)arg;
  char * name = sqln->n->name;
  int num_params = get_num_params(name);
  url_params_t ** params= url_params_init( name, num_params );
  char * nv_template= "(\"%s\",\"%s\"),";
  for(i = 0; i< num_params; i++){
    if( strcmp( params[i]->key, "" ) == 0 ){
      continue;
    }
    if( strcmp( params[i]->value, "" ) == 0 ){
      continue;
    }
    char * kv_quoted= (char * ) malloc( snprintf( NULL, 0, nv_template, params[i]->key, params[i]->value ) + 1 );
    sprintf( kv_quoted, nv_template, params[i]->key, params[i]->value );
    sqln->sql= (char *) realloc( sqln->sql, snprintf(NULL,0,"%s%s",sqln->sql, kv_quoted) + 1 );
    sprintf( sqln->sql,"%s%s", sqln->sql, kv_quoted);
    sqln->num_rows++;
    free(kv_quoted);
  }
  //url_params_free( params, num_params ); //TODO: something is fucked here
}
void build_name_vers_sql( void * arg ){
  sql_name_version_node_t * sqln = (sql_name_version_node_t *)arg;
  char * name= sqln->n->name;
  char * vers= sqln->n->version;
  char * nv_template= "(\"%s\",\"%s\"),";
  char * name_vers_quoted= (char * ) malloc( snprintf(NULL, 0, nv_template, name, vers) + 1 );
  sprintf( name_vers_quoted, nv_template, name, vers );
  sqln->sql= (char *)realloc( sqln->sql, snprintf(NULL,0,"%s%s",sqln->sql, name_vers_quoted) + 1 );
  sprintf( sqln->sql,"%s%s", sqln->sql, name_vers_quoted);
  sqln->num_rows++;
  free(name_vers_quoted);
}
sql_node_t * get_name_insert_sql(hashtable_t * table, char * table_name, void * builder_func ){
  linked_list_t* uniq_keys_ll = ht_get_all_keys( table );
  char * sql_prefix_template = "INSERT INTO %s(name) VALUES (";
  char * sql_prefix = (char * ) malloc(snprintf(NULL, 0, sql_prefix_template, table_name) + 1);
  sprintf(sql_prefix, sql_prefix_template, table_name );
  char * sql_suffix = ");";
  sql_node_t * sql_n= (sql_node_t *) malloc( sizeof(sql_node_t) );
  sql_n->n = (node *) malloc(sizeof(node));
  sql_n->sql =  (char*) malloc(strlen(sql_prefix)+1);
  strcpy( sql_n->sql, sql_prefix );
  int r = iterate_all_linklist_nodes( uniq_keys_ll, (void* (*)(void*)) builder_func, (void *) sql_n );
  //chop off the last char
  sql_n->sql[strlen(sql_n->sql)-1] = '\0';
  sql_n->sql= (char *) realloc( sql_n->sql, strlen(sql_n->sql) + 2 + 1 );
  sprintf( sql_n->sql, "%s%s", sql_n->sql, ");");
  return sql_n;
}
sql_name_version_node_t * get_name_version_insert_sql( hashtable_t * table, char * table_name, void * builder_func ){
  linked_list_t* uniq_keys_ll= ht_get_all_keys(table);
  char * sql_prefix_template = "INSERT INTO %s(name,version) VALUES ";
  char * sql_prefix = (char * ) malloc(snprintf(NULL, 0, sql_prefix_template, table_name) + 1);
  sprintf(sql_prefix, sql_prefix_template, table_name );
  char * sql_suffix = ";";
  sql_name_version_node_t * sql_n = (sql_name_version_node_t *) malloc( sizeof(sql_name_version_node_t) );
  sql_n->n = (name_version_node_t *) malloc(sizeof(name_version_node_t));
  sql_n->sql =  (char*) malloc(strlen(sql_prefix)+1);
  strcpy( sql_n->sql, sql_prefix );
  iterate_all_linklist_nv_nodes( table, uniq_keys_ll, (void* (*)(void*)) builder_func, (void *) sql_n );
  //chop off the last char
  sql_n->sql[strlen(sql_n->sql)-1] = '\0';
  sql_n->sql= (char *) realloc( sql_n->sql, strlen(sql_n->sql) + 1 + 1 );
  sprintf( sql_n->sql, "%s%s", sql_n->sql, sql_suffix);
  return sql_n;
}
sql_name_version_node_t * get_key_value_insert_sql( hashtable_t * table, char * table_name, void * builder_func ){
  linked_list_t* uniq_keys_ll= ht_get_all_keys(table);
  char * sql_prefix_template = "INSERT INTO %s(k,val) VALUES ";
  char * sql_prefix = (char * ) malloc(snprintf(NULL, 0, sql_prefix_template, table_name) + 1);
  sprintf(sql_prefix, sql_prefix_template, table_name );
  char * sql_suffix = ";";
  sql_name_version_node_t * sql_n = (sql_name_version_node_t *) malloc( sizeof(sql_name_version_node_t) );
  sql_n->sql =  (char*) malloc(strlen(sql_prefix)+1);
  strcpy( sql_n->sql, sql_prefix );
  iterate_all_linklist_nv_nodes( table, uniq_keys_ll, (void* (*)(void*)) builder_func, (void *) sql_n );
  sql_n->sql[strlen(sql_n->sql)-1] = '\0';
  sql_n->sql= (char *) realloc( sql_n->sql, strlen(sql_n->sql) + 1 + 1 );
  sprintf( sql_n->sql, "%s%s", sql_n->sql, sql_suffix);
  return sql_n;
}
int insert_h_metrics( httpaccess_metrics *h_metrics ) {
  //sql_node_t * ips_sql_n = get_name_insert_sql( h_metrics->client_ips, "httpstats_clients.ips", (void *) build_ips_sql);
  //sql_node_t * countries_sql_n = get_name_insert_sql( h_metrics->client_geo_locations, "httpstats_clients.locations", (void *) build_name_sql);
  //sql_name_version_node_t * devices_sql_n = get_name_version_insert_sql( h_metrics->client_devices, "httpstats_clients.devices", (void * ) build_name_vers_sql);
  //sql_name_version_node_t * oses_sql_n = get_name_version_insert_sql( h_metrics->client_oses, "httpstats_clients.oses",  (void * ) build_name_vers_sql);
  //sql_name_version_node_t * browsers_sql_n = get_name_version_insert_sql( h_metrics->client_browsers, "httpstats_clients.browsers",  (void * ) build_name_vers_sql);
  //sql_node_t * pages_sql_n = get_name_insert_sql( h_metrics->page_paths, "httpstats_pages.pages_paths", (void *) build_name_sql);
  //sql_node_t * internref_pages_sql_n = get_name_insert_sql( h_metrics->internref_pathstrings, "httpstats_pages.pages_paths", (void *) build_name_sql);
  //sql_node_t * ref_pages_sql_n = get_name_insert_sql( h_metrics->referer_pathstrings, "httpstats_pages.pages_paths", (void *) build_name_sql);
  //sql_node_t * external_hostnames_sql_n = get_name_insert_sql( h_metrics->referer_hostnames, "httpstats_pages.external_domains", (void *) build_name_sql);
  //sql_name_version_node_t * referer_params_sql_n = get_key_value_insert_sql( h_metrics->referer_paramstrings, "httpstats_pages.url_params", (void *) build_params_sql );
  sql_name_version_node_t * internref_params_sql_n = get_key_value_insert_sql( h_metrics->internref_paramstrings, "httpstats_pages.url_params", (void *) build_params_sql );
  //printf( "%s %d\n", ips_sql_n->sql, ips_sql_n->num_rows );
  //printf( "%s %d\n", countries_sql_n->sql, countries_sql_n->num_rows );
  //printf( "%s %d\n", devices_sql_n->sql, devices_sql_n->num_rows );
  //printf( "%s %d\n", oses_sql_n->sql, oses_sql_n->num_rows );
  //printf( "%s %d\n", browsers_sql_n->sql, browsers_sql_n->num_rows );
  //printf( "%s %d\n", pages_sql_n->sql, pages_sql_n->num_rows );
  //printf( "%s %d\n", ref_pages_sql_n->sql, ref_pages_sql_n->num_rows );
  //printf( "%s %d\n", referer_params_sql_n->sql, referer_params_sql_n->num_rows );
  printf( "%s %d\n", internref_params_sql_n->sql, internref_params_sql_n->num_rows );
  //printf( "%s %d\n", external_hostnames_sql_n->sql, external_hostnames_sql_n->num_rows );
  //free_sql_node(ips_sql_n);
  //free_sql_node(countries_sql_n);
  //free_sql_node(pages_sql_n);
  //free_sql_node(ref_pages_sql_n);
  //free_sql_node(external_hostnames_sql_n);
  //free_sql_name_vers_node(referer_params_sql_n);
  free_sql_name_vers_node(internref_params_sql_n);
  //free_sql_name_vers_node(devices_sql_n);
  //free_sql_name_vers_node(oses_sql_n);
  return 0;
}
int iterate_all_linklist_nodes( linked_list_t* linkedl, void *cb(void *), void * arg ){
  int c = list_count(linkedl);
  sql_node_t* sqln   = (sql_node_t *) arg;
  sqln->num_rows = 0;
  sqln->n = node_init("a",0); //empty string messes up free at later point
  if(c == 0) {
    return 0;
  }
  list_entry_t* head = linkedl->head;
  list_entry_t* tail = linkedl->tail;
  list_entry_t* next = head->next;
  sqln->n            = (node *) next->value;
  while( next != tail ){
    cb((void*) sqln );
    next = next->next;
    sqln->n = (node *) next->value;
  }
  cb((void*) sqln );
  return c;
};
int iterate_all_linklist_nv_nodes( hashtable_t * table, linked_list_t* linkedl, void *cb(void *), void * arg ){
  int c = list_count(linkedl);
  sql_name_version_node_t * sqln   = (sql_name_version_node_t *) arg;
  sqln->num_rows = 0;
  if(c == 0) {
    sqln->n = name_version_node_init(" "," ",0); //empty string messes up free at later point
    return 0;
  }
  list_entry_t* head = linkedl->head;
  list_entry_t* tail = linkedl->tail;
  list_entry_t* next = head->next;
  char * key_template = "%s_%s";
  char * name = ((name_version_node_t *) next->value)->name;
  char * version = ((name_version_node_t *) next->value)->version;
  char * key = (char *) malloc( snprintf( NULL, 0, key_template, name, version) + 1);
  sprintf( key, key_template, name, version);
  //size_t * size = (size_t*) (sizeof(name_version_node_t) + strlen(name) + 1 + strlen(version) + 1 );
  size_t * size = (size_t*) sizeof(name_version_node_t);
  sqln->n   = (name_version_node_t *) ht_get( table, key, strlen( key )+1, size );
  while( next != tail ){
    cb((void*) sqln );
    next = next->next;
    key = ((name_version_node_t *) next->value)->name;
    sqln->n = (name_version_node_t *) ht_get( table, key, strlen( key )+1, size );
  }
  cb((void*) sqln );
  free(key);
};
#endif
