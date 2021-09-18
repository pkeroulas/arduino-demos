#!/bin/bash

shoot() {
    python arduino_client.py $1 $2 $3 0  0 0
    sleep $4
    python arduino_client.py $1 $2 $3 $3 0 0
    sleep $4
    python arduino_client.py $1 $2 0  $3 0 0
    sleep $4
    python arduino_client.py $1 $2 0  $3 $3 0
    sleep $4
    python arduino_client.py $1 $2 0  0  $3 0
    sleep $4
    python arduino_client.py $1 $2 $3 0  $3 0
    sleep $4
}

#shoot 1 1 1 0.1
#shoot 1 1 100 0.1
#shoot 1 1 255 0.01
reset(){
    python arduino_client.py 0 0 0 0 0 0
    sleep 1
}
wave(){
    echo wave
    python arduino_client.py 0 1 0 0 100 0
    sleep 1
}
fire(){
    echo fire
    python arduino_client.py 0 2 50 50 0 0
    sleep 1
}
flash(){
    python arduino_client.py 0 4 100 100 100 0
    sleep 1
}
meteor(){
    echo fire
    python arduino_client.py 0 3 0 100 0 0
    sleep 1
}

reset

echo "--------------"
wave
flash
flash
reset

sleep 1

echo "--------------"
fire
flash
flash
reset

sleep 1

echo "--------------"
meteor
flash
flash
reset

sleep 1

echo "--------------"
wave
sleep 1
fire
sleep 1
meteor
sleep 1

echo "--------------"
fire
sleep 1
meteor
sleep 1
wave
sleep 1

echo "--------------"
meteor
sleep 1
wave
sleep 1
fire
sleep 1

echo "--------------"
wave
sleep 1
meteor
sleep 1
fire
sleep 1

python arduino_client.py "kill"
