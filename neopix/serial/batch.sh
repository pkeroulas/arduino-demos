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

shoot 1 1 1 0.1
shoot 1 1 100 0.1
shoot 1 1 255 0.01

python arduino_client.py 1 1 0 0 0
python arduino_client.py "kill"
