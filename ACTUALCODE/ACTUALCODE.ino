const int numSensors = 32;
const int numRelays = 32;

// Define sensor to relay mapping
int sensorRelayMap[][2] = {
  {3, 36}, {13, 37}, {30, 38}, {20, 39}, {14, 40},
  {24, 41}, {34, 42}, {11, 43}, {5, 44}, {21, 45},
  {4, 46}, {17, 47}, {15, 48}, {16, 49}, {35, 50},
  {19, 54}, {8, 55}, {18, 57}, {7, 60}, {26, 61},
  {31, 62}, {12, 63}, {23, 64}, {6, 66}, {25, 67},
  {29, 68}, {32, 69},
  // Add the rest of your mappings
};

// Define the laser colors
String laserColors[numSensors] = {
  "red", "red", "red", "red", "red", 
  "blue", "green", "red", "green", "red",
  "red", "blue", "red", "red", "blue",
  "red", "red", "red", "green", "blue",
  "blue", "blue", "green", "red", "red",
  "red", "green",
  // Fill in the rest according to your setup
};

bool activeLasers[numSensors] = {false}; // Tracks which lasers are currently active

String currentColor = "blue";
int currentLevel = 1;
bool levelStarted = false;
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
  if (!levelStarted) {
    startLevel(currentLevel);
  }

  checkAndHandleLaserBreaks();
}

void startLevel(int level) {
  Serial.print("Starting level ");
  Serial.println(level);

  // Reset active lasers for the new level
  for (int i = 0; i < numSensors; i++) {
    activeLasers[i] = false; // Initially mark all lasers as inactive
    digitalWrite(sensorRelayMap[i][1], LOW); // Ensure all lasers are off before activating new ones
  }

  delay(1000); // Delay to ensure all relays are off before turning some back on

  // Determine the number of lasers to activate for each color based on the level
  blueActive = min(1 + (level - 1) / 2, 6);
  greenActive = min(1 + (level - 1) / 3, 5);
  redActive = min(level * 2 + 1, 16);

  // Activate the lasers for this level
  activateLasers("blue", blueActive);
  activateLasers("green", greenActive);
  activateLasers("red", redActive);

  levelStarted = true;
}

void activateLasers(String color, int count) {
  int activated = 0;
  Serial.println("Activating " + color + " lasers:");
  for (int i = 0; i < numSensors && activated < count; i++) {
    if (laserColors[i] == color) {
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
      if (laserColors[i] != currentColor) {
        // Wrong color laser broken, reset the level
        Serial.println("Wrong color laser broken. Resetting level...");
        Serial.println("INCOMPLETE");
        resetLevel();
        return; // Exit early since we're resetting
      } else {
        digitalWrite(sensorRelayMap[i][1], LOW); // Turn off the broken laser
        activeLasers[i] = false; // Mark as no longer active
        Serial.print(currentColor);
        Serial.print(" laser broken at Sensor ");
        Serial.println(sensorRelayMap[i][0]);
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
  // Check if we've reached the end of the color sequence for the level
  if (currentColor == "red") {
    Serial.println("Level complete. Proceeding to next level after a short break...");
    Serial.println("COMPLETE");
    delay(5000); // 5-second delay to celebrate and prepare for the next level
    
    currentLevel++; // Move to the next level
    currentColor = "blue"; // Reset to start with blue lasers for the new level
    levelStarted = false; // Flag the level as not started to trigger setup in the next loop iteration
    // Reset the active and broken states for all lasers for the new level
    for (int i = 0; i < numSensors; i++) {
      activeLasers[i] = false;
      digitalWrite(sensorRelayMap[i][1], LOW); // Ensure all lasers are turned off before the next level starts
    }
    Serial.print("Starting level ");
    Serial.println(currentLevel);
  } else {
    // If not at the end of the level, move to the next color in the sequence
    if (currentColor == "blue") {
      currentColor = "green";
    } else if (currentColor == "green") {
      currentColor = "red";
    }
    // Reset the active state for lasers of the next color without turning them off,
    // since they should already be off if broken in the correct order.
    for (int i = 0; i < numSensors; i++) {
      if (laserColors[i] == currentColor) {
        activeLasers[i] = true;
      }
    }
    Serial.println("Advance to " + currentColor + " lasers.");
    levelStarted = true; // Start checking for breaks in the next color immediately
  }
}

void resetLevel() {
  // Immediately turn off all lasers
  for (int i = 0; i < numSensors; i++) {
    digitalWrite(sensorRelayMap[i][1], LOW); // Turn off the laser
    activeLasers[i] = false; // Mark as no longer active
  }
  Serial.println("Wrong color laser broken. Resetting level...");

  delay(5000); // 5-second delay before starting the new level

  // Reset variables for a new level start
  currentColor = "blue"; // Reset to start with blue lasers
  levelStarted = false; // Flag the start of the level in the next loop iteration
  
  // Optionally, reinitialize any other variables or states needed for the level start
  
  Serial.println("Starting new level after reset.");
}

