#!/usr/bin/python
#
# Send RGB values to arduino through serial port
#
# Usage:
# $ ./color_to_serial 100 0 0
# Rainbow:
# $ ./color_to_serial

import serial
import sys
import time

ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
print('port: ' + ser.name)

while not 'Init' in ser.readline():
    print('.')
    time.sleep(1) # bootloader, init(), etc.

def toString(r, g, b):
    return str(r) + ',' + str(g) + ',' + str(b) + '\n'

def send(r, g, b):
    rgb = toString(r, g, b)
    print('>>> ' + rgb)
    ser.write(rgb);
    time.sleep(0.01)
    line = ser.readline()
    print('<<< ' + line)

if len(sys.argv) == 4:
    send(sys.argv[1],sys.argv[2],sys.argv[3])
else:
    # rainbow
    send(0, 0, 0);
    amp = 200
    for i in range(amp):
        send(i, 0, amp - i);
    for i in range(amp):
        send(amp - i, i, 0);
    for i in range(amp):
        send(0, amp - i, i);
    send(0, 0, 0);

ser.close();
