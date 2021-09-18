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

// TODO: define HAS_WHITE and select NEO_GRBW
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

byte colorMix(byte c1, byte c2, uint8_t i, uint8_t fade) {
    return c1*i/fade + c2*(fade-i)/fade;
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
    while (Serial.available()) {
        serialProcessLine();

        int id = cmd_buf[SERIAL_CMD_INDEX_ID];
        if ((id != MY_ID) && (id != ARDUINO_ID_ALL)) { // is it for me?
            break;
        }

        int cmd = cmd_buf[SERIAL_CMD_INDEX_CMD];
        r2 = cmd_buf[SERIAL_CMD_INDEX_RED];
        g2 = cmd_buf[SERIAL_CMD_INDEX_GREEN];
        b2 = cmd_buf[SERIAL_CMD_INDEX_BLUE];
        w2 = cmd_buf[SERIAL_CMD_INDEX_WHITE];

        switch(cmd) {
            case NEO_FX_RESET:
                colorFill(0,0,0,NEO_NUM_LEDS);
                break;
            case NEO_FX_WAVE:
                if ((r1!=r2) || (g1!=g2) || (b1!=b2)) {
                    colorWave(r1,g1,b1,r2,g2,b2,NEO_SPEED);
                    r1 = r2; g1 = g2; b1 = b2;
                }
                break;
            case NEO_FX_FIRE:
                break;
            case NEO_FX_FLASH:
                // not for paws
                break;
            case NEO_FX_METEOR:
                break;
            case NEO_FX_FILL:
                colorFill(r2,g2,b2,10);
                break;
        }

    }

    delay(NEO_SPEED);
}
