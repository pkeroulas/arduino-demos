#!/bin/bash

shoot() {
    python arduino_client.py $1 $2 $3 0  0
    sleep $4
    python arduino_client.py $1 $2 $3 $3 0
    sleep $4
    python arduino_client.py $1 $2 0  $3 0
    sleep $4
    python arduino_client.py $1 $2 0  $3 $3
    sleep $4
    python arduino_client.py $1 $2 0  0  $3
    sleep $4
    python arduino_client.py $1 $2 $3 0  $3
    sleep $4
}

id=1
cmd=1
delay=0.1
a=100

shoot $id $cmd $a $delay

# errors
shoot $id 10 $a $delay
shoot 33  $cmd $a $delay
shoot $id 1 10 $delay

python arduino_client.py "kill"
