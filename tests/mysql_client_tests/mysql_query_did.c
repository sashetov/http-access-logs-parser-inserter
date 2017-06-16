#include <my_global.h>
#define _GNU_SOURCE
#include <mysql.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define BUF_SIZE 1024

struct linux_dirent {
  long           d_ino;
  off_t          d_off;
  unsigned short d_reclen;
  char           d_name[];
};
struct mysql_resultset {
  int did;
  int uid;
  char * domain_name;
};
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
  //mysql_close(con);
  return did;
}
struct mysql_resultset * get_real_did_results( MYSQL * con, int possible_did ) {
  struct mysql_resultset *results =  malloc(sizeof(struct mysql_resultset));
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
char ** get_n_hostnames_from_dir( int n, char * logs_dir_full_path, char ** hostnames ) {
  int fd, nread;
  char buf[BUF_SIZE];
  struct linux_dirent *d;
  int bpos;
  char d_type;
  int i =0;
  fd = open( logs_dir_full_path, O_RDONLY | O_DIRECTORY);
  if (fd == -1) {
    handle_error("open");
  }
  for ( ; ; ) {
    if( i >= n ){
      break;
    }
    nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
    if (nread == -1) {
      handle_error("getdents");
    }
    if (nread == 0) {
      break;
    }
    for (bpos = 0; bpos < nread;) {
      if( i >= n ){
        break;
      }
      d = (struct linux_dirent *) (buf + bpos);
      d_type = *(buf + bpos + d->d_reclen - 1);
      if(d_type == DT_REG){
        hostnames[i] = malloc(snprintf(NULL, 0, "%.*s",strlen(d->d_name)-4, d->d_name) + 1);
        sprintf(hostnames[i], "%.*s",strlen(d->d_name)-4, d->d_name);
        i++;
      }
      bpos += d->d_reclen;
    }
  }
  return hostnames;
}
int main(int argc, char **argv) {
  if ( argc < 7 ){
    // hostname user password db_name logs_dir_path num_hostnames
    printf("insufficient args %d < 7\n", argc);
    printf("%s mysql_hostname mysql_port mysql_user mysql_password mysql_dbname domain_name\n", argv[0] );
    exit(-1);
  }
  int i;
  char * mysql_hostname = argv[1];
  char * mysql_port     = argv[2];
  char * mysql_user     = argv[3];
  char * mysql_password = argv[4];
  char * mysql_dbname   = argv[5];
  //char * logs_dir_path  = argv[6];
  char * hostname = argv[6];
  //int n = atoi(argv[7]);
  int max_hostname_len=256;
  //printf("%s %s %s %s %s %d\n", mysql_hostname, mysql_port, mysql_user, mysql_password, mysql_dbname, logs_dir_path, n );
  printf("%s %s %s %s %s %s\n", mysql_hostname, mysql_port,  mysql_user, mysql_password, mysql_dbname, hostname );
  //char* hostnames[n];
  // preallocate space for strings
  //for( i = 0; i < n; i++ ){
  //  hostnames[i] = malloc( max_hostname_len * sizeof *hostnames[i] );
  //}
  //get_n_hostnames_from_dir( n, logs_dir_path, hostnames );
  int possible_did;
  struct mysql_resultset * my_results;
  int port = (int)strtol( mysql_port, NULL, 0 );
  MYSQL * conn = get_my_conn( mysql_hostname, mysql_user, mysql_password, mysql_dbname, port );
  printf("%s %s %s\n","domain_name", "did", "uid");
  possible_did = get_did_for_domain_name( conn, hostname );
  my_results = get_real_did_results( conn, possible_did );
  printf("%s %d %d\n",hostname, (* my_results).did, (* my_results).uid);
  //for( i =0; i< n; i++) {
  //  //conn = get_my_conn(mysql_hostname, mysql_user, mysql_password, mysql_dbname );
  //  possible_did = get_did_for_domain_name(conn, hostnames[i]);
  //  //conn = get_my_conn(mysql_hostname, mysql_user, mysql_password, mysql_dbname );
  //  my_results = get_real_did_results( conn, possible_did );
  //  printf("%s %d %d\n",hostnames[i], (* my_results).did, (* my_results).uid);
  //}
  mysql_close(conn);
  exit(0);
}
