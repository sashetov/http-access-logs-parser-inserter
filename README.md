```
Usage:
./cloudstats {OPTIONAL_OPTS} [REQUIRED_OPTS] OBJECT
 OPTIONAL_OPS:= { -s|--sql_logs_path SQL_LOGS_DIR 
 }
REQUIRED_OPTS:= [ -h|--mysql_host MYSQL_HOSTNAME 
                  -u|--mysql_user MYSQL_USER 
                  -P|--mysql_port MYSQL_PORT 
                  -p|--mysql_password MYSQL_PASSWORD 
                ]
        OBJECT= LOGDIR_PATH
NOTES:
LOGDIR_PATH  - relative or full path to directory containing logfiles named DOMAIN.NAME.log and containing httpd access logs in apache combined format
SQL_LOGS_DIR - relative or full path to directory where to dump logs of sql statements run for each domain name, each in its own log file
```
