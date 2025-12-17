#include <Bluepad32.h>

// Define the maximum number of controllers Bluepad32 supports
#define MAX_CONTROLLERS BP32_MAX_GAMEPADS

// Array to store pointers to connected controllers
ControllerPtr connectedControllers[MAX_CONTROLLERS];

bool _isControllerConnected = false;

// --- 1. CALLBACK: When a Controller Connects ---
void onConnectedController(ControllerPtr ctl) {
  // Find the first available slot to store the pointer
  for (int i = 0; i < MAX_CONTROLLERS; i++) {
    if (connectedControllers[i] == nullptr) {
      connectedControllers[i] = ctl;
      // Use the correct function to get the controller name
      Serial.printf("✅ Controller %d connected: %s\n", i, ctl->getModelName());
      _isControllerConnected = true;
      break;
    }
  }
}

// --- 2. CALLBACK: When a Controller Disconnects ---
void onDisconnectedController(ControllerPtr ctl) {
  // Clear the slot when the controller is disconnected
  for (int i = 0; i < MAX_CONTROLLERS; i++) {
    if (connectedControllers[i] == ctl) {
      Serial.printf("❌ Controller %d disconnected.\n", i);
      connectedControllers[i] = nullptr;
      _isControllerConnected = false;
      break;
    }
  }
}

// --- 3. Function to Read and Print Controller Data ---
void readAndPrintData(ControllerPtr ctl) {
  if (ctl == nullptr) {
    return; // Do nothing if the slot is empty
  }

  // --- Read and print button/joystick states ---
  // Note: Values are standardized for cross-controller compatibility.
  Serial.printf("BTN: 0x%04x | LX: %d, LY: %d | RX: %d, RY: %d | L2: %d, R2: %d | ",
    ctl->buttons(),       // Bitmask of all digital buttons
    ctl->axisX(),         // Left Stick X (-512 to 512)
    ctl->axisY(),         // Left Stick Y (-512 to 512)
    ctl->axisRX(),        // Right Stick X (-512 to 512)
    ctl->axisRY(),        // Right Stick Y (-512 to 512)
    ctl->brake()          // Left Trigger/L2 (0 to 1023)
    ctl->throttle()       // Right Trigger/R2 (0 to 1023)
  );

  // Check specific button states
  if (ctl->a()) {
    Serial.print("A_pressed "); // 'A' (Xbox/Switch) or Cross (PS)
  }
  if (ctl->miscStart()) {
    Serial.print("START_pressed"); 
  }

  Serial.println();
}

// --- Arduino Setup and Loop ---

void setup() {
    // Initialize the built-in LED (usually Active-Low)
    pinMode(LED_BUILTIN, OUTPUT); 
    digitalWrite(LED_BUILTIN, LOW); // Turn LED OFF

    Serial.begin(115200);
    
    // Wait for USB CDC to initialize (critical for ESP32-C3 serial output)
    unsigned long startWait = millis();
    while (!Serial && (millis() - startWait < 5000)) {
        delay(10);
    }

    // 1. Register the connection/disconnection callbacks
    BP32.setup(onConnectedController, onDisconnectedController);
    
    Serial.println("--- Bluepad32 Host Ready. Start pairing your controller now. ---");
}

void loop() {
    // This MUST be called frequently to process Bluetooth events
    BP32.update();

    // Iterate over all possible controller slots and read data
    for (int i = 0; i < MAX_CONTROLLERS; i++) {
        readAndPrintData(connectedControllers[i]);
    }

    if (_isControllerConnected) {
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
        if ((millis() / 1000) % 2 == 0) {
            digitalWrite(LED_BUILTIN, LOW);
        } else {
            digitalWrite(LED_BUILTIN, HIGH);
        }
    }

    // Small delay to prevent flooding the Serial Monitor
    delay(50);
}