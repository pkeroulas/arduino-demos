#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// -----------------------------------------------------------------------------
// NEOPIXEL

#define NEO_CTRL_PIN 6
#define NEO_NUM_LEDS_MAX 180
#define NEO_SPEED 10
#define NEO_FADE  100

#define NEO_FX_RESET  0
#define NEO_FX_WAVE   1
#define NEO_FX_FIRE   2
#define NEO_FX_METEOR 3
#define NEO_FX_FLASH  4
#define NEO_FX_FILL   5
#define NEO_FX_COLOR  6
#define NEO_FX_MAX    6

int strip_type = NEO_GRB + NEO_KHZ800;
int NEO_NUM_LEDS;
Adafruit_NeoPixel strip;

uint8_t neo_current_fx;
uint8_t r1,g1,b1,w1; // background color
uint8_t r2,g2,b2,w2; // foreground color

void colorWave(byte red1, byte green1, byte blue1, byte white1, byte red2, byte green2, byte blue2, byte white2, int SpeedDelay) {
    // Fill the dots one after the other with a color
    for(uint16_t i=0; i<strip.numPixels()+NEO_FADE; i++) {
        for (uint8_t k=0; k<NEO_FADE; k++)
        {
            strip.setPixelColor(
                i-NEO_FADE+k,
                colorMix(red1, red2, k, NEO_FADE),
                colorMix(green1, green2, k, NEO_FADE),
                colorMix(blue1, blue2, k, NEO_FADE),
                colorMix(white1, white2, k, NEO_FADE)
            );
        }
        strip.show();
        delay(SpeedDelay);
    }
}

void colorFill(byte r, byte g, byte b, byte w, int len) {
    for(uint16_t i=0; i<len; i++) {
        strip.setPixelColor(i, r, g, b, w);
    }
    strip.show();
}

int meteor_index = 0;

bool meteor(byte red, byte green, byte blue, byte white, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {
    for(int i = meteor_index; i < NEO_NUM_LEDS+25; i++) { // this loop is more like a timeline
        meteor_index = i;
        // fade brightness all LEDs one step
        for(int j=0; j<NEO_NUM_LEDS; j++) { // this loop is a real iterator along the strip
            if( (!meteorRandomDecay) || (random(10)>5) ) {
                fadeToBlack(NEO_NUM_LEDS-j, meteorTrailDecay );
            }
        }

        // draw meteor only, full color
        for(int j = 0; j < meteorSize; j++) {
            if( ( i-j <NEO_NUM_LEDS) && (i-j>=0) ) {
                strip.setPixelColor(NEO_NUM_LEDS-(i-j), red, green, blue, white);
            }
        }

        strip.show();
        delay(SpeedDelay);
    }
    return true;
}

void fadeToBlack(int ledNo, byte fadeValue) {
    // NeoPixel
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;

    oldColor = strip.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (int) b-(b*fadeValue/256);

    strip.setPixelColor(ledNo, r, g, b, 0);
}

byte colorMix(byte c1, byte c2, uint8_t i, uint8_t fade) {
    return c1*i/fade + c2*(fade-i)/fade;
}

void Fire(int Cooling, int Sparking, int SpeedDelay) {
    static byte heat[NEO_NUM_LEDS_MAX];
    int cooldown;

    // Step 1.  Cool down every cell a little
    for( int i = 0; i < NEO_NUM_LEDS; i++) {
        cooldown = random(0, ((Cooling * 10) / NEO_NUM_LEDS) + 2);

        if(cooldown>heat[i]) {
            heat[i]=0;
        } else {
            heat[i]=heat[i]-cooldown;
        }
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NEO_NUM_LEDS - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' near the bottom
    if( random(255) < Sparking ) {
        int y = random(7);
        heat[y] = heat[y] + random(160,255);
        //heat[y] = random(160,255);
    }

    // Step 4.  Convert heat to LED colors
    for( int j = 0; j < NEO_NUM_LEDS; j++) {
        setPixelHeatColor(j, heat[j] );
    }

    strip.show();
    delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
    // Scale 'heat' down from 0-255 to 0-191
    byte t192 = round((temperature/255.0)*191);

    // calculate ramp up from
    byte heatramp = t192 & 0x3F; // 0..63
    heatramp <<= 2; // scale up to 0..252

    // figure out which third of the spectrum we're in:
    if( t192 > 0x80) {                     // hottest
        strip.setPixelColor(Pixel, 255, 255, heatramp, 0);
    } else if( t192 > 0x40 ) {             // middle
        strip.setPixelColor(Pixel, 255, heatramp, 0, 0);
    } else {                               // coolest
        strip.setPixelColor(Pixel, heatramp, 0, 0, 0);
    }
}

void flash(byte r, byte g, byte b, byte w) {
    // pre flash
    colorFill(r<<1, g<<1, b<<1, w<<1, NEO_NUM_LEDS);
    delay(NEO_SPEED);
    colorFill(0, 0, 0, 0, NEO_NUM_LEDS);
    delay(3*NEO_SPEED);

    // fast fade-in fade-out
    colorFill(r<<2, g<<2, b<<2, w<<2, NEO_NUM_LEDS);
    delay(3*NEO_SPEED);
    colorFill(r<<1, g<<1, b<<1, w<<1, NEO_NUM_LEDS);
    delay(3*NEO_SPEED);
    colorFill(r, g, b, w, NEO_NUM_LEDS);
    delay(6*NEO_SPEED);
    colorFill(r<<1, g<<1, b<<1, w<<1, NEO_NUM_LEDS);
    delay(3*NEO_SPEED);
    colorFill(r<<2, g<<2, b<<2, w<<2, NEO_NUM_LEDS);
    delay(3*NEO_SPEED);
    colorFill(0, 0, 0, 0, NEO_NUM_LEDS);
}

// -----------------------------------------------------------------------------
// MAIN

void setup() {
    NEO_NUM_LEDS = 120;
    strip = Adafruit_NeoPixel(NEO_NUM_LEDS, NEO_CTRL_PIN, strip_type);
    strip.begin();
    strip.setBrightness(50);
    colorFill(0,0,0,0, NEO_NUM_LEDS);
    strip.show(); // Initialize all pixels to 'off'
    r1 = g1 = b1 = w1 = r2 = g2 = b2 = w2 = 0;

    delay(10);
    neo_current_fx = NEO_FX_FIRE;
}

void loop() {
    switch(neo_current_fx) {
        case NEO_FX_FIRE:
            Fire(55,120,1); //was initial fire
            break;
        case NEO_FX_FILL: // restore background
            if ((r1!=r2) || (g1!=g2) || (b1!=b2) || (w1!=w2)) {
                colorFill(r1, g1, b1, w1, NEO_NUM_LEDS);
            }
            break;
        case NEO_FX_METEOR:
            bool finished = meteor(r2, g2, b2, w2, 10, 150, true, 0);
            if(finished)
                neo_current_fx = NEO_FX_RESET;
            break;
    }

    delay(NEO_SPEED); // in ms
}
