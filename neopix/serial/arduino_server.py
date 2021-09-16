#!/usr/bin/python
#

import os
import sys
import time
import glob
import platform

#---------------------------------------------------------
import logging
import logging.handlers

my_logger = logging.getLogger('MyServerLogger')
my_logger.setLevel(logging.DEBUG)
handler = logging.handlers.SysLogHandler(address = '/dev/log')
my_logger.addHandler(handler)

def mylogger(header, msg):
    line = header + ': ' + str(msg)
    my_logger.debug(line)
    print(line)

#---------------------------------------------------------
import socket

s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
try:
    os.remove("/tmp/socketname")
except OSError:
    pass
s.bind("/tmp/socketname")
s.listen(1)

def socketRead():
    conn, addr = s.accept()
    while True:
        data = conn.recv(1024)
        if not data: break
        conn.send('OK')
        conn.close()
        return data.encode('ascii')
    conn.close()
    return ''

#---------------------------------------------------------
import serial

devices_names = []
devices_serial = []

# auto detect all the arduinos
def serialOpen():
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

def serialClose():
    global devices_names, devices_serial
    for i, ser in enumerate(devices_serial):
        ser.close()
        mylogger(devices_names[i], 'close')

def serialSend(msg):
    global devices_names, devices_serial
    for i, ser in enumerate(devices_serial):
        ser.flush()
        ser.write('!'+msg)

def serialReceive():
    global devices_names, devices_serial
    time.sleep(0.1)
    for i, ser in enumerate(devices_serial):
        line = ser.readline()
        mylogger(devices_names[i], '<<< ' + str([ord(c) for c in line]))

#---------------------------------------------------------
# Main

serialOpen()

try:
    while True:
        msg = socketRead()
        if msg == '':
            mylogger('socket', '<<< empty')
        elif msg == 'kill':
            mylogger('socket', '<<< kill')
            break
        else:
            mylogger('socket', '<<<' + str([ord(i) for i in msg]))
            serialSend(msg)
            serialReceive()
except Exception as e:
    print e

serialClose()
