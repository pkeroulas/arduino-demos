#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// -----------------------------------------------------------------------------
// NEOPIXEL

#define NEO_CTRL_PIN 6
#define NEO_FADE  100

#define STRIP_TYPE NEO_GRB + NEO_KHZ800
#define NUM_LEDS 200
#define SEGMENT_SIZE 50

Adafruit_NeoPixel strip;
uint8_t r,g,b,w; // foreground color

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

int meteor_index = 0;

bool meteor(byte red, byte green, byte blue, byte white, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {
    for(int i = meteor_index; i < NUM_LEDS+25; i++) { // this loop is more like a timeline
        meteor_index = i;
        // fade brightness all LEDs one step
        for(int j=0; j<NUM_LEDS; j++) { // this loop is a real iterator along the strip
            if( (!meteorRandomDecay) || (random(10)>5) ) {
                fadeToBlack(NUM_LEDS-j, meteorTrailDecay );
            }
        }

        // draw meteor only, full color
        for(int j = 0; j < meteorSize; j++) {
            if( ( i-j <NUM_LEDS) && (i-j>=0) ) {
                strip.setPixelColor(NUM_LEDS-(i-j), red, green, blue, white);
            }
        }

        strip.show();
        delay(SpeedDelay);
    }
    meteor_index = 0;
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

//---------------------------------------------------

void showStrip() {
    strip.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay){
    for(int i = 0; i < NUM_LEDS-EyeSize-2; i++) {
        setAll(0,0,0);
        setPixel(i, red/10, green/10, blue/10);
        for(int j = 1; j <= EyeSize; j++) {
            setPixel(i+j, red, green, blue);
        }
        setPixel(i+EyeSize+1, red/10, green/10, blue/10);
        showStrip();
        delay(SpeedDelay);
    }

    delay(ReturnDelay);

    for(int i = NUM_LEDS-EyeSize-2; i > 0; i--) {
        setAll(0,0,0);
        setPixel(i, red/10, green/10, blue/10);
        for(int j = 1; j <= EyeSize; j++) {
            setPixel(i+j, red, green, blue);
        }
        setPixel(i+EyeSize+1, red/10, green/10, blue/10);
        showStrip();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
}

void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
    setAll(0,0,0);

    for (int i=0; i<Count; i++) {
        setPixel(random(NUM_LEDS),random(0,255),random(0,255),random(0,255));
        showStrip();
        delay(SpeedDelay);
        if(OnlyOne) {
            setAll(0,0,0);
        }
    }

    delay(SpeedDelay);
}

void RunningLights(byte red, byte green, byte blue, int WaveDelay, bool direction) {
    int Position=0;
    for(int j=0; j<NUM_LEDS*2; j++)
    {
        if (direction)
            Position++;
        else
            Position--;

        for(int i=0; i<NUM_LEDS; i++) {
            // sine wave, 3 offset waves make a rainbow!
            //float level = sin(i+Position) * 127 + 128;
            //setPixel(i,level,0,0);
            //float level = sin(i+Position) * 127 + 128;
            setPixel(i,((sin(i+Position) * 127 + 128)/255)*red,
                    ((sin(i+Position) * 127 + 128)/255)*green,
                    ((sin(i+Position) * 127 + 128)/255)*blue);
        }
        showStrip();
        delay(WaveDelay);
    }
}

void theaterChaseRainbow(int SpeedDelay) {
    byte *c;
    int start = random(200);

    for (int j=start; j < start+50; j++) {     // colors in the wheel
        for (int q=0; q < 3; q++) {
            for (int i=0; i < NUM_LEDS; i=i+3) {
                c = Wheel( (i+j) % 255);
                setPixel(i+q, *c, *(c+1), *(c+2));    //turn every third pixel on
            }
            showStrip();

            delay(SpeedDelay);

            for (int i=0; i < NUM_LEDS; i=i+3) {
                setPixel(i+q, 0,0,0);        //turn every third pixel off
            }
        }
    }
}

byte * Wheel(byte WheelPos) {
    static byte c[3];

    if(WheelPos < 85) {
        c[0]=WheelPos * 3;
        c[1]=255 - WheelPos * 3;
        c[2]=0;
    } else if(WheelPos < 170) {
        WheelPos -= 85;
        c[0]=255 - WheelPos * 3;
        c[1]=0;
        c[2]=WheelPos * 3;
    } else {
        WheelPos -= 170;
        c[0]=0;
        c[1]=WheelPos * 3;
        c[2]=255 - WheelPos * 3;
    }

    return c;
}

void BouncingColoredBalls(int BallCount, byte colors[][3], int time) {
    float Gravity = -9.81;
    int StartHeight = 1;

    float Height[BallCount];
    float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
    float ImpactVelocity[BallCount];
    float TimeSinceLastBounce[BallCount];
    int   Position[BallCount];
    long  ClockTimeSinceLastBounce[BallCount];
    float Dampening[BallCount];

    for (int i = 0 ; i < BallCount ; i++) {
        ClockTimeSinceLastBounce[i] = millis();
        Height[i] = StartHeight;
        Position[i] = 0;
        ImpactVelocity[i] = ImpactVelocityStart;
        TimeSinceLastBounce[i] = 0;
        Dampening[i] = 0.90 - float(i)/pow(BallCount,2);
    }

    for (int j = 0 ; j < time ; j++) {
        for (int i = 0 ; i < BallCount ; i++) {
            TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
            Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;

            if ( Height[i] < 0 ) {
                Height[i] = 0;
                ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
                ClockTimeSinceLastBounce[i] = millis();

                if ( ImpactVelocity[i] < 0.01 ) {
                    ImpactVelocity[i] = ImpactVelocityStart;
                }
            }
            Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
        }

        for (int i = 0 ; i < BallCount ; i++) {
            setPixel(Position[i],colors[i][0],colors[i][1],colors[i][2]);
        }

        showStrip();
        setAll(0,0,0);
    }
}
// -----------------------------------------------------------------------------
// MAIN

int loopCounter = 0;
long randNumber;

void setup() {
    strip = Adafruit_NeoPixel(NUM_LEDS, NEO_CTRL_PIN, STRIP_TYPE);
    strip.begin();
    strip.setBrightness(50);
    strip.show(); // Initialize all pixels to 'off'

    delay(10);
    randomSeed(analogRead(0));
}

void loop() {
    switch (random(9)) {
        case 0:
            r = 120; g = 0; b = 0; // red
            break;
        case 1:
            r = 0; g = 120; b = 0; // green
            break;
        case 2:
            r = 0; g = 0; b = 120; // blue
            break;
        case 3:
            r = 100; g = 50; b = 0; // orange
            break;
        case 4:
            r = 0; g = 100; b = 80; // cyan
            break;
        case 5:
            r = 100; g = 0; b = 50; // pink
            break;
        case 6:
            r = 30; g = 0; b = 100; // purple
            break;
        case 7:
            r = 60; g = 80; b = 0; // orange
            break;
        case 8:
            r = 100; g = 100; b = 100; // white
            break;
    }

    switch (random(6)) {
        case 0:
            meteor(r, g, b, w, SEGMENT_SIZE, 150, true, 0);
            break;
        case 1:
            CylonBounce(r, g, b, SEGMENT_SIZE, 10, 50);
            break;
        case 2:
            for (uint8_t k=0; k<10; k++)
                TwinkleRandom(20, 100, false);
            break;
        case 3:
            RunningLights(g, r, b, 50, false);
            break;
        case 4:
            theaterChaseRainbow(100);
            break;
        case 5:
            byte colors[3][3] = { {0xff, 0,0},
                {0xff, 0xff, 0xff},
                {0   , 0   , 0xff} };

            BouncingColoredBalls(3, colors, 300);
            BouncingColoredBalls(3, colors, 300);
            BouncingColoredBalls(3, colors, 300);
            break;
    }
}
