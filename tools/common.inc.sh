ENV_FILE=".env";
function real_cwd {
 LINKPATH=$(readlink -f .);
 echo $LINKPATH;
 return $?;
}
function printdo { # short for print_and_do_cmd
  echo "$*";
  /usr/bin/bash -c "$*" 2>&1 > /dev/null;
}
