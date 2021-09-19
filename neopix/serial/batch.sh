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
}
wave(){
    echo wave
    python arduino_client.py 0 1 0 0 0 0
}
fire(){
    echo fire
    python arduino_client.py 0 2 50 50 0 0
}
meteor(){
    echo meteor
    python arduino_client.py 0 3 0 0 0 0
}
flash(){
    echo flash
    python arduino_client.py 0 4 100 100 100 0
}
fill(){
    echo fill
    python arduino_client.py 0 5 10 10 10 0
}
purple(){
    echo fill
    python arduino_client.py 0 6 50 0 100 0
}
cyan(){
    echo fill
    python arduino_client.py 0 6 0 80 80 0
}
orange(){
    echo fill
    python arduino_client.py 0 6 80 80 0 0
}
pause(){
    echo "........ pause... press key"
    read whatev
}

sequence() {
    reset

    #echo "--------------"
    #wave
    #flash
    #flash
    #reset

    sleep 1
    pause

    fire
    flash
    flash
    reset

    pause

    meteor
    flash
    flash
    reset

    pause

    wave
    pause
    fire
    sleep 1
    meteor

    pause

    fire
    sleep 1
    meteor
    sleep 1
    wave
    pause

    meteor
    sleep 1
    wave
    sleep 1
    fire
    pause

    wave
    pause
    meteor
    sleep 1
    fire

    pause

    python arduino_client.py "kill"
}
