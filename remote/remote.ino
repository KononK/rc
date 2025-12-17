#include <BleGamepad.h>

// --- Configuration ---
// Set the name for the BLE Gamepad
#define GAMEPAD_NAME "KononK GamePad"

// --- Pin Definitions ---
// Define the GPIO pins for the 12 buttons
const int buttonPins[] = {
  16, 17, 18, 19, 21, 22, 23, 25, 26, 27
};

const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

// Define the GPIO pins for the 6 analog axes
const int analogPins[] = {
  36, // Left Stick X
  37, // Left Stick Y
  38, // Right Stick X
  39, // Right Stick Y
  32, // Left Trigger
  33  // Right Trigger
};
const int numAxes = sizeof(analogPins) / sizeof(analogPins[0]);

// Create a BleGamepad object with the specified name and capabilities
// Parameters: name, vendorID, productID, batteryLevel
// We enable all buttons and all axes.
BleGamepad bleGamepad(GAMEPAD_NAME);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE Gamepad Example");

  pinMode(LED_BUILTIN, OUTPUT);

  // --- Initialize Buttons ---
  // We use INPUT_PULLUP to avoid needing external pull-down resistors.
  // A button press will connect the pin to ground, making the input LOW.
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // --- Initialize Analog Axes ---
  // The ESP32 ADC has a 12-bit resolution (0-4095)
  // The BleGamepad library expects values from -32767 to 32767 for axes.
  // We will need to map these values in the loop.
  for (int i = 0; i < numAxes; i++) {
    pinMode(analogPins[i], INPUT);
  }
  
  // Start the BLE gamepad services
  bleGamepad.begin();
  Serial.println("BLE Gamepad started. Waiting for connections...");
}

void loop() {
  // Check if the gamepad is connected to a device
  if (bleGamepad.isConnected()) {
    digitalWrite(LED_BUILTIN, HIGH);
    
    // --- Read and Process Buttons ---
    for (int i = 0; i < numButtons; i++) {
      // Read the state of the button. digitalRead will be LOW if pressed due to INPUT_PULLUP.
      bool isPressed = (digitalRead(buttonPins[i]) == LOW);
      
      // The button numbers in the library are 1-based.
      int buttonNumber = i + 1;
      
      // Check if the state has changed to avoid sending redundant data
      if (isPressed) {
        bleGamepad.press(buttonNumber);
        Serial.printf("Button %d Pressed\n", buttonNumber);
      } else {
        bleGamepad.release(buttonNumber);
        Serial.printf("Button %d Released\n", buttonNumber);
      }
    }

    // --- Read and Process Analog Axes ---
    int analogValues[numAxes];
    for (int i = 0; i < numAxes; i++) {
      // Read the 12-bit analog value (0-4095)
      analogValues[i] = analogRead(analogPins[i]);
    }

    // Map the 12-bit ADC values (0-4095) to the 16-bit signed range (-32767 to 32767)
    // A simple map function can do this.
    // Note: Joysticks often don't center perfectly at 2048. You may need to add
    // a "deadzone" to prevent drift when the stick is neutral.
    // For this example, we do a direct mapping.
    int mappedX = map(analogValues[0], 0, 4095, -32767, 32767);
    int mappedY = map(analogValues[1], 0, 4095, -32767, 32767);
    int mappedRx = map(analogValues[2], 0, 4095, -32767, 32767);
    int mappedRy = map(analogValues[3], 0, 4095, -32767, 32767);
    int mappedZ = map(analogValues[4], 0, 4095, -32767, 32767); // Left Trigger
    int mappedRz = map(analogValues[5], 0, 4095, -32767, 32767); // Right Trigger
    
    // Set the values for each axis
    bleGamepad.setX(mappedX);
    bleGamepad.setY(mappedY);
    bleGamepad.setRX(mappedRx);
    bleGamepad.setRY(mappedRy);
    bleGamepad.setZ(mappedZ);       // Corresponds to the 5th axis
    bleGamepad.setRZ(mappedRz);     // Corresponds to the 6th axis

    // The library automatically sends the report if any value has changed.
    // To force a send, you can use bleGamepad.sendReport();
  } else {
    if ((millis() / 1000) % 2 == 0) {
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  // Add a small delay to prevent spamming and allow the ESP32 to handle other tasks
  delay(10); 
}
