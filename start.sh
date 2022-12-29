#!/bin/bash
cd tasbot
coresLeft=$(( $1 + 7999 ))
while [ $coresLeft -ge 8000 ]; do
    xterm -e ./playfun --helper $coresLeft &
    sleep 1
    echo "Started core $coresLeft"
    final="$final $coresLeft"
    coresLeft=$(( $coresLeft-1 ));
done;
./playfun --master $final