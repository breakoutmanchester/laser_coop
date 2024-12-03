const int numSensors = 32;
const int numRelays = 32;

// Define sensor to relay mapping 
// (sensor , laser)
int sensorRelayMap[][2] = {
  {3, 36}, {20, 39}, {14, 40}, {11, 58}, {31, 62},
  {24, 41}, {34, 42}, {21, 45},
  {4, 46}, {17, 47}, {15, 48}, {16, 49}, {35, 50},
  {19, 54}, {8, 55}, {7, 60}, {26, 61},
  {12, 63}, {23, 64}, {6, 66}, {25, 67},
  {29, 68}, {32, 69},
  // Add the rest of your mappings
};

// Define the laser colors
String laserColors[numSensors] = {
  "red", "red", "red", "blue", "blue",
  "blue", "green", "red",
  "red", "blue", "red", "red", "blue",
  "red", "red", "green", "blue",
  "blue", "green", "red", "red",
  "red", "green",
  // Fill in the rest according to your setup
};

bool activeLasers[numSensors] = {false}; // Tracks which lasers are currently active
bool validLaser[numSensors] = {true}; // Tracks which lasers are aligned and valid

String currentColor = "blue";
int currentLevel = 1;
bool levelStarted = false;
bool alignmentChecked = false; // Ensure laser alignment is checked only once at the start
int blueActive, greenActive, redActive; // Number of lasers to activate per color

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < numRelays; i++) {
    pinMode(sensorRelayMap[i][1], OUTPUT);
    digitalWrite(sensorRelayMap[i][1], LOW); // Initially, all lasers are off
  }
  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorRelayMap[i][0], INPUT); // Sensors are inputs
  }
}

void loop() {
  if (!alignmentChecked) {
    checkLaserAlignment(); // Check laser alignment at the start of the game
    alignmentChecked = true; // Ensure this check is only done once
  }

  if (!levelStarted) {
    startLevel(currentLevel);
  }

  checkAndHandleLaserBreaks();
}

void checkLaserAlignment() {
  Serial.println("Checking laser alignment...");
  // Turn on all lasers
  for (int i = 0; i < numSensors; i++) {
    if (i < sizeof(sensorRelayMap)/sizeof(sensorRelayMap[0])) {
      digitalWrite(sensorRelayMap[i][1], HIGH);
    } else {
      Serial.print("Error: Index ");
      Serial.print(i);
      Serial.println(" exceeds sensorRelayMap size.");
    }
  }

  delay(1000); // Allow time for all lasers to stabilize

  // Check each sensor multiple times to account for any fluctuation
  for (int i = 0; i < numSensors; i++) {
    if (i < sizeof(sensorRelayMap)/sizeof(sensorRelayMap[0])) {
      int sensorReadings = 0;
      for (int j = 0; j < 10; j++) { // Read sensor 10 times
        if (digitalRead(sensorRelayMap[i][0]) == HIGH) { // Change to HIGH
          sensorReadings++;
        }
        delay(10); // Short delay between readings
      }

      if (sensorReadings >= 8) { // Consider valid if the sensor read HIGH 8 or more times out of 10
        Serial.print("Sensor-relay pair ");
        Serial.print(sensorRelayMap[i][0]);
        Serial.print(" - ");
        Serial.print(sensorRelayMap[i][1]);
        Serial.println(" is out of alignment. Excluding from level generation.");
        validLaser[i] = false; // Mark this pair as invalid
      } else {
        validLaser[i] = true; // Mark this pair as valid
      }
      digitalWrite(sensorRelayMap[i][1], LOW); // Turn off the laser
    } else {
      Serial.print("Error: Index ");
      Serial.print(i);
      Serial.println(" exceeds sensorRelayMap size.");
    }
  }
}

void startLevel(int level) {
  Serial.print("Starting level ");
  Serial.println(level);

  // Reset active lasers for the new level
  for (int i = 0; i < numSensors; i++) {
    activeLasers[i] = false; // Initially mark all lasers as inactive
    digitalWrite(sensorRelayMap[i][1], LOW); // Ensure all lasers are off before activating new ones
  }

  delay(2000); // Delay to ensure all relays are off before turning some back on

  // Determine the number of lasers to activate for each color based on the level
  blueActive = min(1 + (level - 1) / 2, 6);
  greenActive = min(1 + (level - 1) / 3, 5);
  redActive = min(level * 2 + 1, 16);

  // Activate the lasers for this level
  activateLasers("blue", blueActive);
  activateLasers("green", greenActive);
  activateLasers("red", redActive);
  delay(2000);
  levelStarted = true;
}

void activateLasers(String color, int count) {
  int activated = 0;
  Serial.println("Activating " + color + " lasers:");
  for (int i = 0; i < numSensors && activated < count; i++) {
    if (laserColors[i] == color && validLaser[i]) {
      digitalWrite(sensorRelayMap[i][1], HIGH); // Activate the laser
      activeLasers[i] = true; // Mark as active
      activated++;
      Serial.print("Laser ");
      Serial.print(color);
      Serial.print(" activated at Sensor ");
      Serial.print(sensorRelayMap[i][0]);
      Serial.print(", Relay ");
      Serial.println(sensorRelayMap[i][1]);
    }
  }
  
}

void checkAndHandleLaserBreaks() {
  for (int i = 0; i < numSensors; i++) {
    if (activeLasers[i] && digitalRead(sensorRelayMap[i][0]) == HIGH) { // If the laser is broken
      digitalWrite(sensorRelayMap[i][1], LOW); // Turn off the broken laser
      activeLasers[i] = false; // Mark as no longer active
      Serial.print("Laser broken at Sensor ");
      Serial.print(sensorRelayMap[i][0]);
      Serial.print(", Relay ");
      Serial.print(sensorRelayMap[i][1]);
      Serial.print(" (");
      Serial.print(laserColors[i]);
      Serial.println(").");

      if (laserColors[i] != currentColor) {
        // Wrong color laser broken, log the specific sensor/relay & color, then reset the level
        Serial.print("Wrong color laser (");
        Serial.print(laserColors[i]);
        Serial.print(") broken at Sensor ");
        Serial.print(sensorRelayMap[i][0]);
        Serial.print(", Relay ");
        Serial.print(sensorRelayMap[i][1]);
        Serial.println(". Resetting level...");
        delay(1000);
        Serial.println("INCOMPLETE");
        resetLevel();
        return; // Exit early since we're resetting
      }
    }
  }

  // Check if all lasers of the current color are broken, then prepare for the next color or level
  if (areAllCurrentColorLasersHandled()) {
    advanceToNextColorOrLevel();
  }
}

bool areAllCurrentColorLasersHandled() {
  for (int i = 0; i < numSensors; i++) {
    if (laserColors[i] == currentColor && activeLasers[i]) {
      return false; // Found an unhandled laser of the current color
    }
  }
  return true; // All lasers of the current color have been handled
}

void advanceToNextColorOrLevel() {
  if (currentColor == "red" && areAllCurrentColorLasersHandled()) {
    Serial.println("COMPLETE");
    delay(5000); // Wait for 5 seconds before starting the next level

    currentLevel++;
    currentColor = "blue"; // Reset to start with blue lasers for the new level
    levelStarted = false; // Will trigger level setup on the next loop iteration
    Serial.print("Starting level ");
    Serial.println(currentLevel);
    return; // Early return to ensure the new level setup is triggered in the next loop
  }

  if (currentColor == "blue") {
    currentColor = "green";
  } else if (currentColor == "green") {
    currentColor = "red";
  }

  Serial.println("Advance to " + currentColor + " lasers.");
  // No need to set levelStarted to false here as we're continuing the same level
}

void resetLevel() {
  // Immediately turn off all lasers and mark them as inactive
  for (int i = 0; i < numSensors; i++) {
    digitalWrite(sensorRelayMap[i][1], LOW);
    activeLasers[i] = false;
  }
  Serial.println("Level reset. Starting again after a short break...");

  delay(5000); // Wait for 5 seconds before retrying the level

  currentColor = "blue"; // Ensure we start with the first color group
  levelStarted = false; // Trigger the level setup in the next loop iteration
}
