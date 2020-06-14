int red_light_pin= 11;
int green_light_pin = 10;
int blue_light_pin = 9;

const int BLACK[3] = {0, 0, 0};

void setup() {
    Serial.begin(115200);
    Serial.println("Init...");

    pinMode(red_light_pin, OUTPUT);
    pinMode(green_light_pin, OUTPUT);
    pinMode(blue_light_pin, OUTPUT);
    RGB_color(BLACK);
}

void loop() {
    while (Serial.available()) {
        delay(5);
        String r  = Serial.readStringUntil(',');
        String g = Serial.readStringUntil(',');
        String b  = Serial.readStringUntil('\n');
        Serial.print("rgb:"); Serial.print(r);
        Serial.print(","); Serial.print(g);
        Serial.print(","); Serial.println(b);

        int RGB[3];
        RGB[0] = r.toInt();
        RGB[1] = g.toInt();
        RGB[2] = b.toInt();
        RGB_color(RGB);
    }

    delay(5);
}

void RGB_color(int color[])
{
    analogWrite(red_light_pin, 255-color[0]);
    analogWrite(green_light_pin, 255-color[1]);
    analogWrite(blue_light_pin, 255-color[2]);
}
