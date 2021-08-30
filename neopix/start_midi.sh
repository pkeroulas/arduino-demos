#!/bin/bash
./ttymidi/ttymidi -s /dev/ttyACM0 & # start ttyMIDI at 115200
sleep 1
#timidity -iA &                    # start some ALSA compatible MIDI program
aconnect -i                       # list available MIDI input clients
aconnect -o                       # list available MIDI output clients
#aconnect 128:0 129:0              # where 128 and 129 are the client numbers for
LD_PRELOAD=/usr/lib/x86_64-linux-gnu/alsa-lib/libasound_module_conf_pulse.so python ./midi_controller.py
killall ttymidi
