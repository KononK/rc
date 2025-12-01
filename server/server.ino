#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

void setup() {
  Serial.begin(9600);
  Dabble.begin("MyEsp32GamePad");
}

void loop() {
  Dabble.processInput();

  if (GamePad.isUpPressed()) {
    Serial.println("Up Pressed");
  }
  if (GamePad.isDownPressed()) {
    Serial.println("Down Pressed");
  }
  if (GamePad.isLeftPressed()) {
    Serial.println("Left Pressed");
  }
  if (GamePad.isRightPressed()) {
    Serial.println("Right Pressed");
  }
  if (GamePad.isCirclePressed()) {
    Serial.println("Circle Pressed");
  }
  if (GamePad.getAngle() != 0 || GamePad.getRadius() != 0) {
    Serial.print("Angle: ");
    Serial.print(GamePad.getAngle());
    Serial.print(", Radius: ");
    Serial.println(GamePad.getRadius());
  }

  delay(50);
}
