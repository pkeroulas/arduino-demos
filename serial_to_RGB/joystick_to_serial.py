#!/usr/bin/python

import pygame
import serial
import sys
import time

"""
DragonRise USB Joystick with SNES layout

UP:     axix 1: -1
DOWN:   axix 1: 1
LEFT:   axis 0: -1
RIGTH:  axis 0: 1

A:      Button 1
B:      Button 2
X:      Button 3
Y:      Button 4

L:      Button 6
R:      Button 7
Start:  Button 8
Select: Button 9
"""
amp=100
step=2
#BUTTONS=['A','B','X','Y','','','L','R','Start','Select']
BUTTONS=['R','Y','B','V','','','W','W','K','K']
COLORS={}

def set_colors(amp):
    global COLORS
    COLORS={
            'K':[0,0,0],
            'W':[amp,amp,amp],
            'R':[amp,0,0],
            'Y':[amp,amp/2,0],
            'B':[0,0,amp],
            'V':[0,amp,0]
            }

def toString(r, g, b):
    return str(r) + ',' + str(g) + ',' + str(b) + '\n'

def send(c):
    rgb = toString(c[0],c[1],c[2])
    print('>>> ' + rgb)
    ser.write(rgb);
    time.sleep(0.01)
    line = ser.readline()
    print('<<< ' + line)


pygame.init()
clock = pygame.time.Clock()
pygame.joystick.init()
j0 = pygame.joystick.Joystick(0)
j0.init()
prev_button = 0

ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
print('port: ' + ser.name)

while not 'Init' in ser.readline():
    print('.')
    time.sleep(1) # bootloader, init(), etc.

set_colors(amp)
send(COLORS['K']);

while True:
    for event in pygame.event.get():
        if event.type == pygame.JOYBUTTONDOWN:
            for i in range(len(BUTTONS)):
                button = j0.get_button(i)
                if (button > 0):
                    print(BUTTONS[i])
                    prev_button = BUTTONS[i]
                    send(COLORS[prev_button])
        elif event.type == pygame.JOYBUTTONUP:
            #print("Joystick button released.")
            pass
        elif event.type == pygame.JOYAXISMOTION: # all the time
            h = j0.get_axis(0)
            v = j0.get_axis(1)
            if (h == 0) and (v==0):
                break
            elif (h < 0):
                print("LEFT")
            elif (h > 0):
                print("RIGHT")
                print("")
            if (v < 0):
                print("UP")
                amp = 200 if ((amp + step) > 200) else amp + step
            elif (v > 0):
                print("DOWN")
                amp = 0 if ((amp - step) < 0) else amp - step
            print('amp= ' + str(amp))
            set_colors(amp)
            send(COLORS[prev_button])

    clock.tick(20) # Limit to 20 frames per second.

ser.close();
pygame.quit()
