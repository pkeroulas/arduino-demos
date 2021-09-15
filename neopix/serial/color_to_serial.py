#!/usr/bin/python
#

import serial
import sys
import time
import glob
import platform

import logging
import logging.handlers
my_logger = logging.getLogger('MyLogger')
my_logger.setLevel(logging.DEBUG)
handler = logging.handlers.SysLogHandler(address = '/dev/log')
my_logger.addHandler(handler)

def mylogger(header, msg):
    line = header + ': ' + str(msg)
    my_logger.debug(line)
    print(line)

devices_names = []
devices_serial = []

# auto detect all the arduinos
def openDevices():
    global devices_names, devices_serial
    os = platform.system()
    if os == 'Linux':
        devices_names = glob.glob('/dev/ttyACM*')
    elif os == 'Darwin':
        devices_names = glob.glob('/dev/tty.usbmodemFA*')
    else:
        mylogger('ERROR', 'unknow os' + os)
        exit(-1)

    mylogger('Devices', devices_names)
    for dev in devices_names:
        devices_serial.append(serial.Serial(dev, 115200, timeout=1))

    for i, ser in enumerate(devices_serial):
        line = ser.readline();
        while not 'init' in line:
            mylogger(devices_names[i],'.')
            time.sleep(1) # bootloader, init(), etc.
            line = ser.readline();
        mylogger(devices_names[i], '<<< ' + line)

def closeDevices():
    global devices_names, devices_serial
    for i, ser in enumerate(devices_serial):
        ser.close()
        mylogger(devices_names[i], 'close')

def send(ID, cmd, r, g, b):
    global devices_names, devices_serial

    msg = '!' + chr(ID) + chr(cmd) + chr(r) + chr(g) + chr(b)
    for i, ser in enumerate(devices_serial):
        ser.flush()
        ser.write(msg)

def receive():
    global devices_names, devices_serial
    time.sleep(0.1)
    for i, ser in enumerate(devices_serial):
        line = ser.readline()
        mylogger(devices_names[i], '<<< ' + str([ord(c) for c in line]))

if len(sys.argv) < 6:
    mylogger('ERROR', 'Not enough args')
    exit(-1)
ID  = int(sys.argv[1])
cmd = int(sys.argv[2])
r   = int(sys.argv[3])
g   = int(sys.argv[4])
b   = int(sys.argv[5])

if cmd == 10 or ID ==10:
    mylogger('ERROR', 'wrong value for ID/cmd')
    exit(-1)
# forbidden values: '\n' '!'
if r == 10: r == 9
if g == 10: g == 9
if b == 10: b == 9
if r == 33: r == 34
if g == 33: g == 34
if b == 33: b == 34
mylogger('Args', '>>> ID:'+str(ID) + ' CMD:'+str(cmd) + ' RGB:['+str(r)+','+str(g)+','+str(b)+']')

openDevices()
send(ID, cmd, r, g, b)
receive()
closeDevices()
