#include "Dezibot.h"

Dezibot dezibot = Dezibot();

void setup()
{
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  delay(1000);
  Serial.println("Starting Dezibot...");
  
  dezibot.begin();
  dezibot.communication.begin();
  dezibot.communication.setGroupNumber(5);
  Serial.println("Setup complete!");
}

void loop()
{
  Direction tilt = dezibot.motion.detection.getTiltDirection();
  
  switch (tilt)
  {
  case Front:
    Serial.println("Tilt: Front");
    dezibot.multiColorLight.setTopLeds(GREEN);
    dezibot.communication.sendMessage("vorn");
    break;
  case Left:
    Serial.println("Tilt: Left");
    dezibot.multiColorLight.setTopLeds(YELLOW);
    dezibot.communication.sendMessage("links");
    break;
  case Right:
    Serial.println("Tilt: Right");
    dezibot.multiColorLight.setTopLeds(TURQUOISE);
    dezibot.communication.sendMessage("rechts");
    break;
  case Back:
    Serial.println("Tilt: Back");
    dezibot.multiColorLight.setTopLeds(BLUE);
    dezibot.communication.sendMessage("stop");
    break;
  case Flipped:
    Serial.println("Tilt: Flipped");
    dezibot.multiColorLight.setTopLeds(PINK);
    dezibot.communication.sendMessage("stop");
    break;
  case Neutral:
    Serial.println("Tilt: Neutral");
    dezibot.multiColorLight.turnOffLed();
    dezibot.communication.sendMessage("stop");
    break;
  case Error:
    Serial.println("Tilt: Error");
    dezibot.multiColorLight.setTopLeds(RED);
    dezibot.communication.sendMessage("stop");
    break;
  }

  delay(100);
}
