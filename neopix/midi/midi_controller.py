#!/usr/bin/python
#
# KorgNanoKontroler handler to arduino through ttymidi

from pygame import midi
import serial
import time
import os

#----------------------------------------------------------------------------
# midi init

midi.init()
if (midi.get_default_input_id() == -1):
    print("exit")
    exit(-1)
for i in range(midi.get_count()):
    print "Devices Info " + str(i) + ": "+ str(midi.get_device_info(i))

OFF=0x80
ON=0x90
INDEX_STATUS = 0
INDEX_CHANNEL = 1
INDEX_VALUE = 2
buffer_size = 200

korg = midi.Input(3 ,buffer_size)
arduino_rx = midi.Input(4 ,buffer_size)
arduino_tx = midi.Output(5 ,buffer_size)

korg_rgb = [0,0,0]
korg_update = False

#----------------------------------------------------------------------------
# midi read loop

arduino_rgb = [0,0,0]
arduino_update = False

# button ID (= MIDI channel): [rgb_index, rgb_value]
buttons_rgb_map = {
        32: [0, 127],
        48: [0, 64],
        64: [0, 0],
        33: [1, 127],
        49: [1, 64],
        65: [1, 0],
        34: [2, 127],
        50: [2, 64],
        66: [2, 0]
    }

while True:
    time.sleep(0.1)
    try:
        if korg.poll():
            buffer = korg.read(buffer_size)
            for entry in buffer:
                midi_entry = entry[0]
                midi_channel = midi_entry[INDEX_CHANNEL]
                print "korg    : "+ str(entry)
                """
                if midi_channel < 3:
                    korg_rgb[midi_channel] = midi_entry[INDEX_VALUE]
                    korg_update = True
                """
                if midi_channel in buttons_rgb_map.keys() and midi_entry[INDEX_VALUE] == 127:
                    korg_rgb[buttons_rgb_map[midi_channel][0]] = buttons_rgb_map[midi_channel][1]
                    korg_update = True

            if korg_update:
                print "korg    : "+ str(entry)  + " rgb : "+ str(korg_rgb)
                for j in range(3):
                    arduino_tx.write_short(ON, j, korg_rgb[j])
                korg_update = False

        if arduino_rx.poll():
            buffer = arduino_rx.read(buffer_size)
            for entry in buffer:
                midi_entry = entry[0]
                midi_channel = midi_entry[INDEX_CHANNEL]
                if midi_channel < 3:
                    arduino_rgb[midi_channel] = midi_entry[INDEX_VALUE]
                    arduino_update = True
                print "arduino : "+ str(entry)
            if arduino_update:
                print "arduino : "+ str(entry)  + " rgb : "+ str(arduino_rgb)
                arduino_update = False

    except Exception as e:
        print e
        kork.close()
        arduino_rx.close()
        arduino_tx.close()
        exit()
