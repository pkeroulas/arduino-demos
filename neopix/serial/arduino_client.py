#!/usr/bin/python

import sys

#---------------------------------------------------------
import logging
import logging.handlers
my_logger = logging.getLogger('MyClientLogger')
my_logger.setLevel(logging.DEBUG)
handler = logging.handlers.SysLogHandler(address = '/dev/log')
my_logger.addHandler(handler)

def mylogger(header, msg):
    line = header + ': ' + str(msg)
    my_logger.debug(line)
    print(line)

#---------------------------------------------------------
import socket

def socketSend(msg):
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.connect("/tmp/socketname")
    s.send(msg.encode())
    data = s.recv(1024)
    s.close()
    mylogger('Send', 'Received ' + repr(data))

#---------------------------------------------------------

def usage():
    mylogger('Usage:', 'arduino_client.py <arduino_ID> <cmd> <R> <G> <B>')

if len(sys.argv) < 6:
    if len(sys.argv) == 2 and sys.argv[1] == 'kill':
        mylogger('Args', 'kill server')
        socketSend('kill')
        exit(0)

    mylogger('ERROR', 'Not enough args')
    usage()
    exit(-1)

ID  = int(sys.argv[1])
cmd = int(sys.argv[2])
r   = int(sys.argv[3])
g   = int(sys.argv[4])
b   = int(sys.argv[5])

# detect and translate forbidden values: '\n', '!'
for v in [ord('!'), ord('\n')]: # 10 or 33
    if cmd == v or ID == v:
        mylogger('ERROR', 'wrong value for ID/cmd')
        usage()
        exit(-1)
    if r == v: r = v-1
    if g == v: g = v-1
    if b == v: b = v-1

mylogger('Args', '>>> ID:'+str(ID) + ' CMD:'+str(cmd) + ' RGB:['+str(r)+','+str(g)+','+str(b)+']')
msg = chr(ID) + chr(cmd) + chr(r) + chr(g) + chr(b)
socketSend(msg)
