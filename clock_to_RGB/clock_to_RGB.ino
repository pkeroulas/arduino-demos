// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// from arduino-1.8.9/libraries/RTClib/examples/pcf8523/pcf8523.ino 
// Adafruit data logger shield:
// https://cdn-learn.adafruit.com/downloads/pdf/adafruit-data-logger-shield.pdf
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// Date and time functions using a PCF8523 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

RTC_PCF8523 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

int RED[3] = {255, 0, 0};
int GREEN[3] = {0, 255, 0};
int BLACK[3] = {0, 0, 0};
int color_amplitude = 50; // max=255
int red_light_pin= 11;
int green_light_pin = 10;
int blue_light_pin = 9;

void setup () {

    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(red_light_pin, OUTPUT);
    pinMode(green_light_pin, OUTPUT);
    pinMode(blue_light_pin, OUTPUT);
    RGB_color(BLACK);

    while (!Serial) {
        delay(1);  // for Leonardo/Micro/Zero
    }
    Serial.begin(115200);

    Serial.println("RTC init...");
    if (! rtc.begin()) {
        Serial.println("Couldn't find RTC");
        //while (1);
    }

    Serial.println("...");
    delay(300);

    if (! rtc.initialized()) {
        Serial.println("RTC is NOT running!");
        // following line sets the RTC to the date & time this sketch was compiled
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2015, 1, 21, 3, 0, 0));
    }

    // This line sets the RTC with an explicit date & time, for example to set
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    rtc.calibrate(PCF8523_TwoHours, 1);
    //rtc.calibrate(PCF8523_OneMinute, 1);

    Serial.println("RTC init OK!");
}

void RGB_color(int color[])
{
    const int r = color[0] * color_amplitude / 256;
    const int g = color[1] * color_amplitude / 256;
    const int b = color[2] * color_amplitude / 256;
    analogWrite(red_light_pin,   255 - r);
    analogWrite(green_light_pin, 255 - g);
    analogWrite(blue_light_pin,  255 - b);
}

void loop () {
    DateTime now = rtc.now();

    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(" - ");
    Serial.print(now.hour());
    Serial.print(":");
    Serial.print(now.minute());
    Serial.print(":");
    Serial.print(now.second());
    Serial.println();

    // weekend
    /*
    if((now.dayOfTheWeek() == 0) || (now.dayOfTheWeek() == 6))
    {
        // night (red) < 7:35 < day (green) < 21h < night(red)
        if ((now.hour() >= 7) && (now.hour() < 21)) { // green zone
           if ((now.hour() == 7) && (now.minute() < 35)) { RGB_color(RED); } // exception
           else { RGB_color(GREEN); }
        }
        else { RGB_color(RED); }
    }
    else // week
    {
    */
        // night (red) < 7:15 < day (green) < 20h53 < night(red)
        if ((now.hour() >= 7) && (now.hour() <= 20)) { // green zone
           if ((now.hour() == 7) && (now.minute() < 15)) { RGB_color(RED); } // exception
           else if ((now.hour() == 20) && (now.minute() > 53)) { RGB_color(RED); } // exception
           else { RGB_color(GREEN); }
        }
        else { RGB_color(RED); }
    //}

    digitalWrite(LED_BUILTIN, HIGH);
    delay(5);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
}
