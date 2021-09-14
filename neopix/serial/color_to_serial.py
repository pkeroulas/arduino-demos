#!/usr/bin/python
#
# Usage:
# $ ./color_to_serial 100 0 0
# Rainbow:
# $ ./color_to_serial

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
    my_logger.debug(header + ': ' + str(msg))
    print(msg)

devices_serial = []
# auto detect all the arduinos
def openDevices():
    os = platform.system()
    if os == 'Linux':
        devices_names = glob.glob('/dev/ttyACM*')
    elif os == 'Darwin':
        devices_names = glob.glob('/dev/tty.usbmodemFA*')
    else
        mylogger('ERROR', 'unknow os' + os)
        exit(-1)

    mylogger('devices', devices_names)
    for names in devices_names:
        devices_serial.append(serial.Serial('/dev/ttyACM0', 115200, timeout=1))

def closeDevices():
    mylogger('devices', 'close')
    for ser in devices_serial:
        ser.close()

#while not 'Init' in ser.readline():
#    mylogger('.')
#    time.sleep(1) # bootloader, init(), etc.

def toString(header, ID, cmd, r, g, b):
    return chr(header) + chr(ID) + chr(cmd) + chr(r) + chr(b) + chr(b)

def send(ID, cmd, r, g, b):
    msg = toString(0xAA, ID, cmd, r, g, b)
    for ser in devices_serial:
        ser.write(msg)
    time.sleep(0.01)
    #line = ser.readline()
    #mylogger('<<<', line)

if len(sys.argv) < 6:
    mylogger('ERROR', 'Not enough args')
    exit(-1)

ID  = int(sys.argv[1])
cmd = int(sys.argv[2])
r   = int(sys.argv[3])
g   = int(sys.argv[4])
b   = int(sys.argv[5])
openDevices()
mylogger('>>>', 'ID:'+str(ID) + ' CMD:'+str(cmd) + ' CMD:['+str(r)+','+str(g)+','+str(b)+']')
send(ID, cmd, r, g, b)
closeDevices()
