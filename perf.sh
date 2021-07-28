#!/bin/bash

SECONDS=0
# do some work

# SHELL=/usr/software/bin/bash /u/jsung/bin/parallel -j 0 --wd . --env ndmpcopy_cron_parallel --eta ndmpcopy_cron_parallel ::: local

res1=$(date +%s.%N)


echo_var(){
    echo $1
    ./client_1 http://localhost/lab2.html 8090
    return 0
}
export -f echo_var


gcc webserver.c -o webserver
gcc client_1.c -o client_1

./webserver&
seq -f "n%04g" 1 10000 | parallel -P 10 echo_var {}
#./client_1 http://localhost/lab2.html 8090
#47878ms (10,000)
#5868ms (1000)
#872ms (100)
#433ms (10)

#2sec (10)
#1sec (100)
#13sec (1000)
#

res2=$(date +%s.%N)
dt=$(echo "$res2 - $res1" | bc)
dd=$(echo "$dt/86400" | bc)
dt2=$(echo "$dt-86400*$dd" | bc)
dh=$(echo "$dt2/3600" | bc)
dt3=$(echo "$dt2-3600*$dh" | bc)
dm=$(echo "$dt3/60" | bc)
ds=$(echo "$dt3-60*$dm" | bc)

printf "Total runtime: %d:%02d:%02d:%02.4f\n" $dd $dh $dm $ds

ELAPSED="Elapsed: $(($SECONDS / 3600))hrs $((($SECONDS / 60) % 60))min $(($SECONDS % 60))sec"
echo $ELAPSED
#duration=$SECONDS
#echo "$(($duration % 60))  seconds elapsed."

exit 0
