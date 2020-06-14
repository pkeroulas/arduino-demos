#!/usr/bin/python
#
# Read 3 first sliders of Kork NonoKontrol Midi controller
# and write to serial port

from pygame import midi
import serial
import time
import os

#----------------------------------------------------------------------------
# korg nanoKontroller2 mapping

"""
N_SCENE = 9
channel_name_tab = {}
for i in range(N_SCENE) : channel_name_tab[16+i] = 'knob_'+str(i)
for i in range(N_SCENE) : channel_name_tab[i] = 'slider_'+str(i)
for i in range(N_SCENE) : channel_name_tab[32+i] = 's_button_'+str(i)
for i in range(N_SCENE) : channel_name_tab[48+i] = 'm_button_'+str(i)
for i in range(N_SCENE) : channel_name_tab[64+i] = 'r_button_'+str(i)
print channel_name_tab
"""

#----------------------------------------------------------------------------
# midi init

midi.init()
print "Devices ID : "+ str(midi.get_default_input_id())
print "Devices Info : "+ str(midi.get_device_info(midi.get_default_input_id()))
if (midi.get_default_input_id() == -1):
    print("exit")
    exit(-1)

INDEX_STATUS = 0
INDEX_CHANNEL = 1
INDEX_VALUE = 2
buffer_size = 200

controller = midi.Input(midi.get_default_input_id(),buffer_size)
# In case there are more than MIDI controller
#controller = midi.Input(3 ,buffer_size)

#----------------------------------------------------------------------------
# serial init

ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
print('port: ' + ser.name)

while not 'Init' in ser.readline():
    print('.')
    time.sleep(1) # bootloader, init(), etc.

def toString(r, g, b):
    return str(r) + ',' + str(g) + ',' + str(b) + '\n'

def send(rgb):
    rgb = toString(rgb[0], rgb[1], rgb[2])
    #print('>>> ' + rgb)
    ser.write(rgb);
    time.sleep(0.01)
    #line = ser.readline()
    #print('<<< ' + line)

rgb = [0,0,0]

while True:
    time.sleep(0.01)
    try:
        if not controller.poll():
            continue

        buffer = controller.read(buffer_size)
        for entry in buffer:
            midi_entry = entry[0]
            midi_channel  = midi_entry[INDEX_CHANNEL]

            if midi_channel < 3:
                rgb[midi_channel]  = midi_entry[INDEX_VALUE]
                print str(midi_entry) + ' rgb => ' + str(rgb)
                send(rgb)

    except Exception as e:
        print e
        controller.close()
        exit()

