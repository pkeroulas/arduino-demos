#!/usr/bin/python

#---------------------------------------------------------
import platform

os_name = platform.system()
if os_name == 'Linux':
    SYSLOG_FILE = '/dev/log'
elif os_name == 'Darwin':
    SYSLOG_FILE = '/var/run/syslog'
else:
    print('ERROR:unknow os' + os_name)
    exit(1)

#---------------------------------------------------------

import logging
import logging.handlers
my_logger = logging.getLogger('MyClientLogger')
my_logger.setLevel(logging.DEBUG)
handler = logging.handlers.SysLogHandler(address = SYSLOG_FILE)
my_logger.addHandler(handler)

def mylogger(msg):
    line = 'ArduinoClient: ' + str(msg)
    my_logger.debug(line)
    print(line)

def usage():
    mylogger('Usage: arduino_client.py <arduino_ID> <cmd> <R> <G> <B>')

#---------------------------------------------------------
import socket, subprocess, time, os

SOCKETFILE = "/tmp/socketname"

def socketSend(msg):
    try:
        s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        s.connect(SOCKETFILE)
        s.send(msg.encode())
        data = s.recv(1024)
        s.close()
        line = "".join( chr(x) for x in bytearray(data) )
        mylogger('<<< ' + line.replace('\r\n',''))
    except Exception as e:
        print(e)
        return False
    return True

def socketServerCheck():
    if os.path.exists(SOCKETFILE):
        if socketSend('ping'):
            mylogger('Server OK')
            return;
        os.remove(SOCKETFILE)

    script_path = os.path.dirname(os.path.realpath(__file__))
    mylogger('Server down')
    server_cmd = '/usr/bin/python ' + script_path + '/arduino_server.py'
    mylogger('Restarting ' + server_cmd)
    subprocess.Popen(server_cmd , stdout=None, shell=True)
    i = 0
    while not os.path.exists(SOCKETFILE):
        time.sleep(1)
        i += 1
        mylogger('.')
        if i > 5:
            mylogger('Server not found, EXIT')
            exit(1)

#---------------------------------------------------------
import sys

if len(sys.argv) < 7:
    if len(sys.argv) == 2:
        if sys.argv[1] == 'start':
            socketServerCheck()
        elif sys.argv[1] == 'kill':
            if os.path.exists(SOCKETFILE):
                mylogger('kill server')
                socketSend('kill')
            else:
                mylogger('server is down')
        elif sys.argv[1] == 'ping':
            mylogger('ping server')
            socketSend('ping')
        exit(0)

    mylogger('ERROR Not enough args')
    usage()
    exit(1)

socketServerCheck()

ID  = int(sys.argv[1])
cmd = int(sys.argv[2])
r   = int(sys.argv[3])
g   = int(sys.argv[4])
b   = int(sys.argv[5])
w   = int(sys.argv[6])
s   = ID + cmd + r + g + b + w

msg = str(ID) + ',' + str(cmd) + ',' + str(r) + ','+ str(g) + ',' + str(b) + ',' + str(w) + ',' + str(s) + '\n'
mylogger('>>> ID:'+str(ID) + ' CMD:'+str(cmd) + ' RGBW:['+str(r)+','+str(g)+','+str(b)+','+str(w)+']  sum:'+ str(s))
socketSend(msg)
