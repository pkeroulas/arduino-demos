#include <MIDI.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// -----------------------------------------------------------------------------
// NEOPIXEL

#define PIN 6
#define NUM_LEDS 180
#define SPEED 10
#define FADE  100

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void colorWave(byte red1, byte green1, byte blue1, byte red2, byte green2, byte blue2,int SpeedDelay) {
    // Fill the dots one after the other with a color
    for(uint16_t i=0; i<strip.numPixels()+FADE; i++) {
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

// -----------------------------------------------------------------------------
// MIDI

MIDI_CREATE_DEFAULT_INSTANCE();
int noteON = 144;//144 = 10010000 in binary, note on command
int noteOFF = 128;//128 = 10000000 in binary, note off command

uint8_t r1,g1,b1; // background color
uint8_t r2,g2,b2; // foreground color

// https://github.com/FortySevenEffects/arduino_midi_library/wiki/Using-Callbacks
void handleNoteOn(byte channel, byte pitch, byte velocity) {
    // pitch is color index rgb
    switch(pitch) {
       case 0: r2 = velocity<<1; break;
       case 1: g2 = velocity<<1; break;
       case 2: b2 = velocity<<1; break;
       default: return;
    }
    MIDImessage(noteON, pitch, velocity);
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
    switch(pitch) {
       case 0: r2 = 0; break;
       case 1: g2 = 0; break;
       case 2: b2 = 0; break;
       default: return;
    }
    MIDImessage(noteON, pitch, velocity);
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
    // Initiate MIDI communications, listen to channel 0
    MIDI.begin(0);
    Serial.begin(115200); // 115200 for ttymidi

    strip.begin();
    strip.setBrightness(50);
    strip.show(); // Initialize all pixels to 'off'
}

uint8_t i = 0;
void loop() {
    MIDI.read();
    delay(SPEED);

    //MIDI-control wave
    if ((r1!=r2) || (g1!=g2) || (b1!=b2)) {
        colorWave(r1,g1,b1,r2,g2,b2,SPEED);
        r1 = r2; g1 = g2; b1 = b2;
    }

    /*
    colorWave(0,0,0,255,0,0,SPEED); // black to red
    colorWave(255,0,0,0,255,0,SPEED); // red to green
    colorWave(0,255,0,0,0,255,SPEED); // green to blue
    colorWave(0,0,255,0,0,0,SPEED); // greon to blue
    */

    /* baton */
    //strip.setPixelColor(i,0,0,0);
    //for (uint8_t k=1; k<5; k++)
    //    strip.setPixelColor(i+k,r2,g2,b2);
    //i++;
    //if (i>200) i=0;
    //strip.show();
    //delay(SPEED>>1);
}
