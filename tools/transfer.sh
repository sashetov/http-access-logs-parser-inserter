#!/bin/bash
. tools/common.inc.sh
function __main__ {
  if ! [ -f $ENV_FILE ]; then 
    echo "environment file ($ENV_FILE) not present, please create it";
    exit 1;
  else
    . $ENV_FILE
  fi;
  if ! [ -x $BIN_FILE ]; then 
    echo 'binary not present, please run make';
    exit 1;
  fi;
  for HOSTNUM in 001 002 003; do {
    SSH_URL="${SSH_URL_PRE}${HOSTNUM}${SSH_URL_POST}";
    printdo "ssh $SSH_URL 'mkdir -p $PROGRAM_BASEDIR/{lib,bin}'";
    printdo "ssh $SSH_URL 'yum -y install glibc-2.17 libmaxminddb-1.2.0-1.el7.x86_64 boost-regex-1.53.0-27.el7.x86_64 yaml-cpp-0.5.1-6.el7.x86_64 libicu-50.1.2-15.el7.x86_64'";
    ldd $BIN_FILE 2>&1  | \
    sed -r 's/^(.+=>)?[^\/]+(\/|linux-vdso.so.1)([^(=]+)(\(|=>).+/\2\3/g' |\
    grep -vE 'vdso.so.1|libc.so.6|ld-linux|libm.so|libpthread.so|librt.so|libdl.so.2|libmaxminddb.so.0|libboost_regex.so.1.53.0|libyaml-cpp.so.0.5|libicu' |\
    while read LIBPATH; do {
      DIRNAME=$(dirname $LIBPATH);
      printdo "scp $LIBPATH $SSH_URL:$DIRNAME/";
      printdo "scp $LIBPATH $SSH_URL:$PROGRAM_BASEDIR/lib/";
      if [ "$DIRNAME" != "/usr/lib64" ]; then
        if [ "$DIRNAME" != "/lib64" ]; then {
          printdo "ssh $SSH_URL 'cd /usr/lib64 && ln -s ${LIBPATH};';"
        }; fi;
      fi;
    }; done;
    printdo "scp cloudstats $SSH_URL:$PROGRAM_BASEDIR/bin/;";
    printdo "scp uap_regexes.yaml $SSH_URL:$PROGRAM_BASEDIR/bin/;";
    printdo "scp search_engines $SSH_URL:$PROGRAM_BASEDIR/bin/;";
  }; done;
}
__main__ $*
