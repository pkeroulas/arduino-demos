//include <USB-MIDI.h>
#include <MIDI.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define NUM_LEDS 180
#define SPEED 50
#define FADE  100

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

MIDI_CREATE_DEFAULT_INSTANCE();
int noteON = 144;//144 = 10010000 in binary, note on command
int noteOFF = 128;//128 = 10000000 in binary, note off command

uint8_t r,g,b = 20;

// -----------------------------------------------------------------------------
// see documentation here:
// https://github.com/FortySevenEffects/arduino_midi_library/wiki/Using-Callbacks

void handleNoteOn(byte channel, byte pitch, byte velocity) {
    if (pitch == 0) r = velocity<<1;
    if (pitch == 1) g = velocity<<1;
    if (pitch == 2) b = velocity<<1;

    if (pitch < 3) MIDImessage(noteON, pitch, velocity);
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
    if (pitch == 0) r = 0;
    if (pitch == 1) g = 0;
    if (pitch == 2) b = 0;

    if (pitch < 3) MIDImessage(noteOFF, pitch, velocity);
}

//send MIDI message through USB port
void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  Serial.write(command);//send note on or note off command
  Serial.write(MIDInote);//send pitch data
  Serial.write(MIDIvelocity);//send velocity data
}

void setup() {
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);

    // Initiate MIDI communications, listen to all channels
    MIDI.begin(0);
    Serial.begin(115200); // for ttymidi

    strip.begin();
    strip.setBrightness(50);
    strip.show(); // Initialize all pixels to 'off'
}

uint8_t i = 0;
void loop() {
    // Call MIDI.read the fastest you can for real-time performance.
    //MIDImessage(noteON, 48+i, velocity);//turn note on
    MIDI.read();

    /*
    colorWave(0,0,0,250,0,0,SPEED);
    colorWave(255,0,0,0,250,0,SPEED);
    colorWave(0,255,0,0,0,250,SPEED);
    colorWave(0,0,255,0,0,0,SPEED);
    */

    strip.setPixelColor(i,0,0,0);
    for (uint8_t k=1; k<5; k++)
        strip.setPixelColor(i+k,r,g,b);
    i++;
    if (i>200) i=0;

    strip.show();
    delay(SPEED>>1);
}

void colorWave(byte red1, byte green1, byte blue1, byte red2, byte green2, byte blue2,int SpeedDelay) {
    // Fill the dots one after the other with a color
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        //strip.setPixelColor(i,red2,green2,blue2);
        for (uint8_t k=0; k<FADE; k++)
        {
            strip.setPixelColor(
                i-FADE+k,
                colorMix(red1, red2, k, FADE),
                colorMix(green1, green2, k, FADE),
                colorMix(blue1, blue2, k, FADE)
            );
        }
        strip.show();
        delay(SpeedDelay);
    }
}

byte colorMix(byte c1, byte c2, uint8_t i, uint8_t fade) {
    return c1*i/fade + c2*(fade-i)/fade;
}
