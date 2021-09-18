#!/usr/bin/python
import os
import sys
import time
import glob
import platform

os_name = platform.system()
if os_name == 'Linux':
    SERIAL_DEV_BASE = '/dev/ttyACM*'
    SYSLOG_FILE = '/dev/log'
elif os_name == 'Darwin':
    SERIAL_DEV_BASE = '/dev/tty.usbmodemFA*'
    SYSLOG_FILE = '/var/run/syslog'
else:
    print('ERROR:unknow os' + os_name)
    exit(1)

#---------------------------------------------------------
import logging
import logging.handlers

my_logger = logging.getLogger('MyServerLogger')
my_logger.setLevel(logging.DEBUG)
handler = logging.handlers.SysLogHandler(address = SYSLOG_FILE)
my_logger.addHandler(handler)

def mylogger(msg):
    line = 'ArduinoServerrrr: ' + str(msg)
    my_logger.debug(line)
    print(line)

#---------------------------------------------------------
import socket

SOCKETFILE = "/tmp/socketname"
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

def socketOpen():
    global sock
    try:
        os.remove(SOCKETFILE)
    except OSError:
        pass
    sock.bind(SOCKETFILE)
    sock.listen(1)

def socketRead():
    global sock
    conn, addr = sock.accept()
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
    devices_names = glob.glob(SERIAL_DEV_BASE)

    mylogger('Devices' + str(devices_names))
    for dev in devices_names:
        devices_serial.append(serial.Serial(dev, 115200, timeout=1))

    for i, ser in enumerate(devices_serial):
        line = ser.readline();
        while not 'init' in line:
            mylogger(devices_names[i] + '.')
            time.sleep(1) # bootloader, init(), etc.
            line = ser.readline().replace('\r\n','')
        mylogger(devices_names[i] + ' <<< ' + line)

def serialClose():
    global devices_names, devices_serial
    for i, ser in enumerate(devices_serial):
        ser.close()
        mylogger(devices_names[i] + 'close')

def serialSend(msg):
    global devices_names, devices_serial
    for i, ser in enumerate(devices_serial):
        ser.flush()
        ser.write(msg)

def serialReceive(msg):
    global devices_names, devices_serial
    sum_orig = sum([int(v) for v in msg.split(',')])

    for i, ser in enumerate(devices_serial):
        line = ser.readline().replace('\r\n','')
        mylogger(devices_names[i] + ' <<< sum:' + line)
        if not int(line) == sum_orig:
            mylogger(devices_names[i] + ' ERROR: wrong checksum')


#---------------------------------------------------------
# Main

serialOpen()
socketOpen()

try:
    while True:
        msg = socketRead()
        if msg == '':
            mylogger('socket <<< empty')
        elif msg == 'kill':
            mylogger('socket <<< kill')
            break
        else:
            mylogger('socket <<< ' + msg.replace('\n',''))
            serialSend(msg)
            time.sleep(0.01)
            serialReceive(msg)
except Exception as e:
    print e

os.remove(SOCKETFILE)
serialClose()