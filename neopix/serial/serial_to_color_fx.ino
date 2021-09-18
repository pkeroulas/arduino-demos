#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// -----------------------------------------------------------------------------
// PUPPET

#define ARDUINO_ID_ALL    0
#define ARDUINO_ID_LEFT_FRONT_PAW    1
#define ARDUINO_ID_RIGHT_FRONT_PAW   2
#define ARDUINO_ID_LEFT_BACK_PAW     3
#define ARDUINO_ID_RIGHT_BACK_PAW    4
#define ARDUINO_ID_BELLY  5
#define ARDUINO_ID_BACK   6
#define ARDUINO_ID_HEAD   7

// EDIT THIS:
int MY_ID = ARDUINO_ID_LEFT_FRONT_PAW;

// -----------------------------------------------------------------------------
// NEOPIXEL

#define NEO_CTRL_PIN 6
#define NEO_NUM_LEDS 180
#define NEO_SPEED 10
#define NEO_FADE  100

#define NEO_FX_RESET  0
#define NEO_FX_WAVE   1
#define NEO_FX_FIRE   2
#define NEO_FX_METEOR 3
#define NEO_FX_FLASH  4
#define NEO_FX_FILL   5

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEO_NUM_LEDS, NEO_CTRL_PIN, NEO_GRB + NEO_KHZ800);

// TODO: define HAS_WHITE and select NEO_GRBW for paws, back and head
//       Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRBW + NEO_KHZ800);
//       strip.setPixelColor(Pixel, strip.Color(red, green, blue));

uint8_t r1,g1,b1,w1; // background color
uint8_t r2,g2,b2,w2; // foreground color

void colorWave(byte red1, byte green1, byte blue1, byte red2, byte green2, byte blue2,int SpeedDelay) {
    // Fill the dots one after the other with a color
    for(uint16_t i=0; i<strip.numPixels()+NEO_FADE; i++) {
        for (uint8_t k=0; k<NEO_FADE; k++)
        {
            strip.setPixelColor(
                i-NEO_FADE+k,
                colorMix(red1, red2, k, NEO_FADE),
                colorMix(green1, green2, k, NEO_FADE),
                colorMix(blue1, blue2, k, NEO_FADE)
            );
        }
        strip.show();
        delay(SpeedDelay);
    }
}

void colorFill(byte r, byte g, byte b, int len) {
    for(uint16_t i=0; i<len; i++) {
        strip.setPixelColor(i, r, g, b);
    }
    strip.show();
}

int meteor_index = 0;

bool meteor(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {
    for(int i = meteor_index; i < NEO_NUM_LEDS+25; i++) { // this loop is more like a timeline
        meteor_index = i;
        if (Serial.available()) {
            return false;
        }

        //for(int i = 0; i < NEO_NUM_LEDS; i++) {
        // fade brightness all LEDs one step
        for(int j=0; j<NEO_NUM_LEDS; j++) { // this loop is a real iterator along the strip
            if( (!meteorRandomDecay) || (random(10)>5) ) {
                fadeToBlack(NEO_NUM_LEDS-j, meteorTrailDecay );
            }
        }

        // draw meteor only, full color
        for(int j = 0; j < meteorSize; j++) {
            if( ( i-j <NEO_NUM_LEDS) && (i-j>=0) ) {
                strip.setPixelColor(NEO_NUM_LEDS-(i-j), red, green, blue);
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

    strip.setPixelColor(ledNo, r,g,b);
}

byte colorMix(byte c1, byte c2, uint8_t i, uint8_t fade) {
    return c1*i/fade + c2*(fade-i)/fade;
}

void Fire(int Cooling, int Sparking, int SpeedDelay) {
    static byte heat[NEO_NUM_LEDS];
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
        strip.setPixelColor(Pixel, 255, 255, heatramp);
    } else if( t192 > 0x40 ) {             // middle
        strip.setPixelColor(Pixel, 255, heatramp, 0);
    } else {                               // coolest
        strip.setPixelColor(Pixel, heatramp, 0, 0);
    }
}

// -----------------------------------------------------------------------------
// SERIAL

#define SERIAL_CMD_INDEX_ID     0
#define SERIAL_CMD_INDEX_CMD    1
#define SERIAL_CMD_INDEX_RED    2
#define SERIAL_CMD_INDEX_GREEN  3
#define SERIAL_CMD_INDEX_BLUE   4
#define SERIAL_CMD_INDEX_WHITE  5
#define SERIAL_CMD_LEN          6

int cmd_buf[SERIAL_CMD_LEN] = {0,0,0,0,0,0};
int neo_current_fx =  NEO_FX_RESET;

void serialEvent() {
    while (Serial.available()) {
        serialProcessLine();

        int id = cmd_buf[SERIAL_CMD_INDEX_ID];
        if ((id != MY_ID) && (id != ARDUINO_ID_ALL)) { // is it for me?
            break;
        }

        int cmd = cmd_buf[SERIAL_CMD_INDEX_CMD];
        if(cmd != NEO_FX_FLASH) {// flash short and temporary, don't change fx and background
            r2 = cmd_buf[SERIAL_CMD_INDEX_RED];
            g2 = cmd_buf[SERIAL_CMD_INDEX_GREEN];
            b2 = cmd_buf[SERIAL_CMD_INDEX_BLUE];
            w2 = cmd_buf[SERIAL_CMD_INDEX_WHITE];
        }

        switch(cmd) {
            case NEO_FX_RESET:
                neo_current_fx = NEO_FX_FILL;
                colorFill(0,0,0,NEO_NUM_LEDS);
                r1 = g1 = b1 = r2 = g2 = b2 = 0; // this is new backgroud
                break;
            case NEO_FX_WAVE: // this is long but don't interrupt
                if ((r1!=r2) || (g1!=g2) || (b1!=b2)) {
                    colorWave(r1,g1,b1,r2,g2,b2,NEO_SPEED);
                    r1 = r2; g1 = g2; b1 = b2;
                    neo_current_fx = NEO_FX_FILL;
                }
                break;
            case NEO_FX_FIRE: // let's do it in main loop, so it can be interruptable
                neo_current_fx = NEO_FX_FIRE;
                break;
            case NEO_FX_FLASH: // this is short and temporary, don't change fx
                // TODO should probably just use W chan
                colorFill(cmd_buf[SERIAL_CMD_INDEX_RED],cmd_buf[SERIAL_CMD_INDEX_GREEN],cmd_buf[SERIAL_CMD_INDEX_BLUE],NEO_NUM_LEDS);
                delay(NEO_SPEED);
                colorFill(0,0,0,NEO_NUM_LEDS);
                delay(NEO_SPEED); delay(NEO_SPEED); delay(NEO_SPEED);
                colorFill(r2,g2,g2,NEO_NUM_LEDS);
                delay(NEO_SPEED); delay(NEO_SPEED); delay(NEO_SPEED);
                colorFill(0,0,0,NEO_NUM_LEDS);
                break;
            case NEO_FX_METEOR: // let's do it in main loop, so it can be interruptable
                colorFill(0,0,0,NEO_NUM_LEDS);
                neo_current_fx = NEO_FX_METEOR;
                meteor_index = 0;
                break;
            case NEO_FX_FILL:
                neo_current_fx = NEO_FX_FILL;
                colorFill(r2,g2,b2,NEO_NUM_LEDS);
                r1 = r2; g1 = g2; b1 = b2; // this is new background
                break;
        }

    }
}

bool serialProcessLine() {
    delay(5);
    byte string [20];
    byte size = Serial.readBytes(string, 20);
    string[size] = 0; // Add the final 0 to end the C string

    int i = 0;
    int checksum = 0;
    char* ptr = strtok(string, ",");
    while (ptr != NULL) {
        cmd_buf[i] = atoi(ptr);
        checksum += cmd_buf[i];
        i++;
        ptr = strtok(NULL, ",");
    }

    // debug
    Serial.println(String(checksum));
    /*
    for (int i=0; i<SERIAL_CMD_LEN) {
        Serial.print(String(" cmd_buf["));
        Serial.print(String(i));
        Serial.print(String("]:"));
        Serial.print(String(cmd_buf[i]));
        Serial.print(",");
    }
    */
}

// -----------------------------------------------------------------------------
// MAIN

void setup() {
    strip.begin();
    strip.setBrightness(50);
    strip.show(); // Initialize all pixels to 'off'

    Serial.begin(115200);
    Serial.setTimeout(10);
    delay(10);
    Serial.print("ID "); Serial.print(String(MY_ID)); Serial.println(" init");
    delay(10);
}

void loop() {
    switch(neo_current_fx) {
        case NEO_FX_FIRE:
            // Fire(55,120,1); is real fire
            Fire(r2,g2,0);
            break;
        case NEO_FX_FILL: // restore background
            if ((r1!=r2) || (g1!=g2) || (b1!=b2)) {
                colorFill(r1,g1,b1,NEO_NUM_LEDS);
            }
            break;
        case NEO_FX_METEOR:
            bool finished =meteor(r2,g2,b2,10, 150, true, 5);
            if(finished)
                neo_current_fx = NEO_FX_RESET;
            break;
    }

    delay(NEO_SPEED);
}
