//#include <Arduino.h>

#include "Dezibot.h"

Dezibot dezibot = Dezibot();

void receivedCallback(String &msg) {
  if (msg == "vorn") {
    dezibot.motion.move();
  } else if (msg == "links") {
    dezibot.motion.rotateAntiClockwise();
  } else if (msg == "rechts") {
    dezibot.motion.rotateClockwise();
  } else if (msg == "stop") {
    dezibot.motion.stop();
  }
}
void setup() {
  dezibot.begin();
  dezibot.communication.begin();
  dezibot.communication.setGroupNumber(5);
  dezibot.communication.onReceive(&receivedCallback);
}

void loop() {
  dezibot.display.print("Hello from\nEmpfaengerbot!");
  delay(2000);
  dezibot.display.clear();
  dezibot.display.print("Ready to receive instructions!");
  delay(5000);
  dezibot.display.clear();
}