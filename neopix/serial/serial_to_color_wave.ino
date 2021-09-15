#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define ARDUINO_ID_ALL 0
#define ARDUINO_ID_MY  1

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

uint8_t r1,g1,b1; // background color
uint8_t r2,g2,b2; // foreground color

// -----------------------------------------------------------------------------
// SERIAL

#define SERIAL_INDEX_HEADER 0
#define SERIAL_INDEX_ID     (1-1)
#define SERIAL_INDEX_CMD    (2-1)
#define SERIAL_INDEX_RED    (3-1)
#define SERIAL_INDEX_GREEN  (4-1)
#define SERIAL_INDEX_BLUE   (5-1)
#define SERIAL_LEN          (6-1)

bool serialProcessLine(byte buf) {
}

// -----------------------------------------------------------------------------
// MAIN

void setup() {
    strip.begin();
    strip.setBrightness(50);
    strip.show(); // Initialize all pixels to 'off'

    Serial.begin(115200);
    delay(10);
    Serial.print("ID "); Serial.print(String(ARDUINO_ID_MY)); Serial.println(" init");
    delay(10);
}

void loop() {
    while (Serial.available()) {
        delay(5);
        strip.setPixelColor(0,0,100,0);
        Serial.readStringUntil('!');
        strip.setPixelColor(1,0,100,0);
        byte buf[SERIAL_LEN];
        Serial.readBytes(buf, SERIAL_LEN);
        strip.setPixelColor(2,0,100,0);
        if ((buf[SERIAL_INDEX_ID] != ARDUINO_ID_MY) && (buf[SERIAL_INDEX_ID] != ARDUINO_ID_ALL)) { // is it for me?
            break;
        }
        strip.setPixelColor(3,0,100,0);
        Serial.println(String((char *)buf));

        r2 = buf[SERIAL_INDEX_RED];
        g2 = buf[SERIAL_INDEX_GREEN];
        b2 = buf[SERIAL_INDEX_BLUE];
        if ((r1!=r2) || (g1!=g2) || (b1!=b2)) {
            strip.setPixelColor(0,r2,g2,b2);
            //colorWave(r1,g1,b1,r2,g2,b2,SPEED);
            r1 = r2; g1 = g2; b1 = b2;
        }
        strip.show();
    }

    delay(SPEED);
}
