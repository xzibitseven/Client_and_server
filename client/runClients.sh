#!/bin/bash

for i in $(seq 1 1 10)
do
   TEST="./client client_$i 7777 $i"
   gnome-terminal -e "bash -c \"$TEST; exec bash\""
done

