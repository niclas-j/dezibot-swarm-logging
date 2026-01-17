#include <Arduino.h>

int i = 0;

void setup() {
    Serial.begin(115200);
}

void loop() {
    Serial.print("Sender loop: ");
    Serial.println(i);
    i++;
    delay(1000);
}
