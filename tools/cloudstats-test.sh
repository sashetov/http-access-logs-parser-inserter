#!/bin/bash
. tools/common.inc.sh
export ASAN_OPTIONS=symbolize=1;
export ASAN_SYMBOLIZER_PATH=$(which llvm-symbolizer);
function __main__ {
  if ! [ -f $ENV_FILE ]; then 
    echo "environment file ($ENV_FILE) not present, please create it";
    exit 1;
  else
    . $ENV_FILE
  fi;
  if ! [ -x $BIN_FILE ]; then 
    echo "binary $BIN_FILE not present, please run make";
    exit 1;
  fi;
  printdo "gdb -ex r -ex bt -args ./${BIN_FILE} -h ${MYSQL_HOST} -P ${MYSQL_PORT} -u ${MYSQL_USER} -p ${MYSQL_PASS} ${LOGFILES_DIR}"
}
__main__ $*
