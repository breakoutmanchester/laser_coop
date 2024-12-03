#define NUM_RELAYS_DIGITAL 18 // Number of digital relays
#define NUM_RELAYS_ANALOG 16 // Number of analog relays
#define NUM_LIGHT_SENSORS 31 // Number of light sensors

// Digital relay pins
const int digitalRelayPins[NUM_RELAYS_DIGITAL] = {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50};

// Analog relay pins
const int analogRelayPins[NUM_RELAYS_ANALOG] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};

// Light sensor pins //11, 12, 18 has been taken out
const int lightSensorPins[NUM_LIGHT_SENSORS] = {
    3, 4, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 19, 20,
    21, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 34, 35
};

void setup() {
  Serial.begin(9600);
  // Set digital relay pins as OUTPUT and set them to HIGH
  for (int i = 0; i < NUM_RELAYS_DIGITAL; i++) {
    pinMode(digitalRelayPins[i], OUTPUT);
    digitalWrite(digitalRelayPins[i], HIGH);
    Serial.println(digitalRelayPins[i]);
  
  }

//d9 58
  Serial.println("Analog:");
  // Set analog relay pins as OUTPUT and set them to HIGH
  for (int i = 0; i < NUM_RELAYS_ANALOG; i++) {
    pinMode(analogRelayPins[i], OUTPUT);
    digitalWrite(analogRelayPins[i], HIGH);
    Serial.println(analogRelayPins[i]);
  }

  // Set light sensor pins as INPUT
  for (int i = 0; i < NUM_LIGHT_SENSORS; i++) {
    pinMode(lightSensorPins[i], INPUT);
  }

  // Initialize Serial communication
 
}

void loop() {
  // Read state of pin 3 and print to console
  for(int x = 0; x < NUM_LIGHT_SENSORS; x++)
  {
    int pin3State = digitalRead(lightSensorPins[x]);
    
    if (pin3State == HIGH)
    {
      Serial.print(lightSensorPins[x]);
      Serial.print(" ");
    }
  }
  Serial.println();
  delay(50);
}
