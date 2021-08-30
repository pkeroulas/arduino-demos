# Arduino Demos

## Hardware

- Arduino UNO, ATmega328

## Software

- OS: Ubuntu 20.04
- Arduino v1.8.9
- microcom
- python (for external control)

## Compile & upload any demo

```sh
arduino --upload clock_to_RGB/clock_to_RGB.ino --port /dev/ttyACM0
```

## Serial port

```sh
microcom -s 115200 -p /dev/ttyACM0
```

## Demos

### Clock to RGB


`clock_to_RGB/clock_to_RGB.ino` is based on `arduino-1.8.9/libraries/RTClib/examples/pcf8523/pcf8523.ino `.
For later version, clone [RTClib](https://github.com/adafruit/RTClib) in `arduino-1.8.13/libraries/`.

- Use [ADAfruit DataLogger Shield](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-data-logger-shield.pdf)
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
Control with MIDI controller like the [Kork nanoKontrol2](https://www.korg.com/us/products/computergear/nanokontrol2/)

```sh
pip install pygame #tested with 1.9.6
python ./serial_to_RGB/midi_to_serial.py
```

Troubleshoot:
Had to prepend: `LD_PRELOAD=/usr/lib/x86_64-linux-gnu/alsa-lib/libasound_module_conf_pulse.so`

###  Neopixel

Install [Adafruit NeoPixel](https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-installation)
in `<arduino_dir>/libraries`.

For MIDI control, Arduino UNO microcontroller doesn't have USB capability, thus doesn't support MIDI natively.
[Arduino-MIDI](https://www.arduino.cc/reference/en/libraries/midi-library/) library can still be used but
using serial port only for transport. And A MIDI-to-serial-converter like
[timidity ](https://github.com/lathoub/Arduino-USBMIDI.git) must be installed on the host PC.

Execute `start_midi.sh` to launch that converter and the [Kork](https://www.korg.com/us/products/computergear/nanokontrol2/)-to-Arduino bridge.
