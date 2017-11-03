#!/bin/bash
THREAD_POOL_SIZE=40
TOTAL_THREADS_TO_LAUNCH=200
let RESULT=$TOTAL_THREADS_TO_LAUNCH
let TIMES=100
let i=1
while [ $i -le $TIMES ] ; do 
  let RESULT=$(./worker-pool $THREAD_POOL_SIZE $TOTAL_THREADS_TO_LAUNCH 2>/dev/null | cut -d ' ' -f 2);
  if [ $RESULT -eq $TOTAL_THREADS_TO_LAUNCH ] ; then 
    echo $i $RESULT==$TOTAL_THREADS_TO_LAUNCH;
  else
    echo $i $RESULT!=$TOTAL_THREADS_TO_LAUNCH;
    exit $i;
  fi;
  let i++;
done;
exit 0;
