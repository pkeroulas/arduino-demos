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

uint8_t r1,g1,b1,w1; // background color
uint8_t r2,g2,b2,w2; // foreground color

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
    Serial.print("ID "); Serial.print(String(ARDUINO_ID_MY)); Serial.println(" init");
    delay(10);
}

void loop() {
    while (Serial.available()) {
        serialProcessLine();

        int id = cmd_buf[SERIAL_CMD_INDEX_ID];
        if ((id != ARDUINO_ID_MY) && (id != ARDUINO_ID_ALL)) { // is it for me?
            break;
        }

        int cmd = cmd_buf[SERIAL_CMD_INDEX_CMD];
        r2 = cmd_buf[SERIAL_CMD_INDEX_RED];
        g2 = cmd_buf[SERIAL_CMD_INDEX_GREEN];
        b2 = cmd_buf[SERIAL_CMD_INDEX_BLUE];
        w2 = cmd_buf[SERIAL_CMD_INDEX_WHITE];

        // debug strip.setPixelColor(0,r2,g2,b2); strip.show();
        if ((r1!=r2) || (g1!=g2) || (b1!=b2)) {
            colorWave(r1,g1,b1,r2,g2,b2,SPEED);
            r1 = r2; g1 = g2; b1 = b2;
        }
    }

    delay(SPEED);
}
