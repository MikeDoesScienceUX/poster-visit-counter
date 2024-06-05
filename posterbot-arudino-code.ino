#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Ultrasonic sensor pins
#define TRIG_PIN 9
#define ECHO_PIN 8

// Enter/Exit counts
int enterCount = 0;
int exitCount = 0;
bool objectDetected = false;
bool previousObjectDetected = false;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Initial display setup
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println(F("poster buddy"));
  display.drawRect(60, 30, 30, 30, SSD1306_WHITE); // Head
  display.drawCircle(70, 40, 3, SSD1306_WHITE); // Left eye
  display.drawCircle(90, 40, 3, SSD1306_WHITE); // Right eye
  display.drawRect(75, 50, 10, 5, SSD1306_WHITE); // Mouth
  display.display();
  delay(5000); // Pause for 5 seconds
  updateDisplay();
}

void loop() {
  // Clear the trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Set the trigger pin HIGH for 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the echo pin, and calculate distance
  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2;

  // Check if an object is detected within a certain range (e.g., 10 cm)
  if (distance < 10 && distance > 0) {
    objectDetected = true;
  } else {
    objectDetected = false;
  }

  // Count entrances and exits
  if (objectDetected && !previousObjectDetected) {
    enterCount++;
    updateDisplay();
  } else if (!objectDetected && previousObjectDetected) {
    exitCount++;
    updateDisplay();
  }

  previousObjectDetected = objectDetected;

  delay(100); // Small delay to avoid bouncing
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Enter Count:"));
  display.setCursor(0, 10);
  display.print(enterCount);
  display.setCursor(0, 20);
  display.println(F("Exit Count:"));
  display.setCursor(0, 30);
  display.print(exitCount);
  display.display();
}
