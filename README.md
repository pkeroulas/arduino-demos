# Arduino Demos

## Hardware

- Arduino UNO, ATmega328
- [ADAfruit DataLogger Shield](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-data-logger-shield.pdf)

## Software
- OS: Ubuntu 18.04
- Arduino v1.8.9
- microcom

## Bootloader

TODO

## Compile & upload any demo

```sh
arduino --upload clock_to_RGB/clock_to_RGB.ino
```

## Serial port

```sh
microcom -s 115200 -p /dev/ttyAMC0
```

## Demos

### Clock to RGB

Based on  arduino-1.8.9/libraries/RTClib/examples/pcf8523/pcf8523.ino 

- Use Data Loger shield with cell coin to power the RTC
- Use a RGB LED on PWM pins solder on top of the shield
- Turn red during night
- Turn green during day

Compile has 2 steps:

- uncomment the following line to init the RTC with compile time: `// rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));` then compile & upload
- comment the same line, compile & upload, so that RTC isn't adjust on every next startup

Use `microcom` to debug read time. RTC may drift overtime and needs to
be re-adjuted.

###  Serial to RGB

Control manually from python script:

```sh
pip install pyserial
./serial_to_RGB/color_to_serial 100 0 0
./serial_to_RGB/color_to_serial
```
Control with MIDI controller

```sh
pip install pygame
python ./serial_to_RGB/midi_to_serial.py
```

Troubleshoot:

Had to: `LD_PRELOAD=/usr/lib/x86_64-linux-gnu/alsa-lib/libasound_module_conf_pulse.so`
