#include <NewPing.h>
#include <SPI.h>
#include <SD.h>

// Pins for ultrasonic sensors
#define TRIGGER_PIN_LEFT 7
#define ECHO_PIN_LEFT 6
#define TRIGGER_PIN_RIGHT 9
#define ECHO_PIN_RIGHT 8

// Pin for SD card chip select
#define SD_CS 10

// Onboard LED pin
#define LED_PIN LED_BUILTIN

// SD file
File logFile;

// NewPing setup for ultrasonic sensors
NewPing sonarLeft(TRIGGER_PIN_LEFT, ECHO_PIN_LEFT, 200); // Maximum distance set to 200 cm
NewPing sonarRight(TRIGGER_PIN_RIGHT, ECHO_PIN_RIGHT, 200);

int peopleCount = 0;
unsigned long lastLeftTrigger = 0;
unsigned long lastRightTrigger = 0;

void setup() {
  Serial.begin(9600);

  // Initialize the SD card
  Serial.println(F("Initializing SD card..."));
  if (!SD.begin(SD_CS)) {
    Serial.println(F("Card failed, or not present"));
    while (1); // Don't proceed, loop forever
  }
  Serial.println(F("SD card initialized."));
  
  // Open the log file in write mode to initialize it and write the column headers
  logFile = SD.open("log.csv", FILE_WRITE);
  if (logFile) {
    if (logFile.size() == 0) {
      logFile.println("Type,Timestamp");
      Serial.println(F("Log file created and headers written."));
    }
    logFile.close();
  } else {
    Serial.println(F("Error opening log file"));
  }

  // Initialize the ultrasonic sensors
  Serial.println(F("Initializing ultrasonic sensors..."));

  // Initialize the LED pin
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  unsigned long currentTime = millis() / 1000; // Get current time in seconds since the Arduino was powered on

  // Read distances from ultrasonic sensors
  int distanceLeft = sonarLeft.ping_cm();
  int distanceRight = sonarRight.ping_cm();

  // Check for sensor triggers and update timestamps
  if (distanceLeft < 50) {
    lastLeftTrigger = millis();
  }
  if (distanceRight < 50) {
    lastRightTrigger = millis();
  }

  // Detect exit: left sensor triggered first, then right sensor
  if (lastLeftTrigger != 0 && lastRightTrigger != 0 && lastLeftTrigger < lastRightTrigger) {
    logEvent("Exit", currentTime);
    peopleCount--;
    blinkLED(2); // Blink LED twice for exit
    delay(1000); // Debounce delay
    lastLeftTrigger = 0;
    lastRightTrigger = 0;
  }

  // Detect entrance: right sensor triggered first, then left sensor
  else if (lastLeftTrigger != 0 && lastRightTrigger != 0 && lastRightTrigger < lastLeftTrigger) {
    logEvent("Entrance", currentTime);
    peopleCount++;
    blinkLED(1); // Blink LED once for entrance
    delay(1000); // Debounce delay
    lastLeftTrigger = 0;
    lastRightTrigger = 0;
  }

  delay(100);
}

void logEvent(String eventType, unsigned long timestamp) {
  logFile = SD.open("data.csv", FILE_WRITE);
  if (logFile) {
    logFile.print(eventType);
    logFile.print(",");
    logFile.println(timestamp);
    logFile.close();
    Serial.print("Logged: ");
    Serial.print(eventType);
    Serial.print(", Time: ");
    Serial.println(timestamp);
  } else {
    Serial.println("Error opening log file");
  }
}

void blinkLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH); // Turn the LED on
    delay(200); // Wait for 200 milliseconds
    digitalWrite(LED_PIN, LOW); // Turn the LED off
    delay(200); // Wait for 200 milliseconds
  }
}
