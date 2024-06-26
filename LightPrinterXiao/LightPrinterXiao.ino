#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "image.h"      // Custom image class for handling image operations
#include "imageArray.h" // Image data arrays

image img1(microsoft_logo, microsoft_logo_WIDTH, microsoft_logo_HEIGHT);

// Button pins
const int actionButton = 10; // Button for action (e.g., setting corners)
const int printButton = 9;   // Button for printing (e.g., displaying colors)

// Variables to track the extremes of coordinates
int xMin = INT_MAX;
int yMin = INT_MAX;
int xMax = INT_MIN;
int yMax = INT_MIN;

// Variables to store the current x and y coordinates
int x, y;
char xHolder[5];
char yHolder[5];

bool idle = false;

// Initialize the NeoPixel strip
Adafruit_NeoPixel *light = new Adafruit_NeoPixel(1, 5, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial1.begin(420000);
  // Serial1.setTimeout(10);

  // Serial.begin(115200);

  light->begin();
  light->setBrightness(50);
  light->clear();
  light->show();
  delay(500);

  pinMode(actionButton, INPUT_PULLUP);
  pinMode(printButton, INPUT_PULLUP);
}

void loop()
{
  char numberHolder[10] = {NULL}; // Buffer to hold incoming Serial data

  // Read from Serial1 if available
  while (Serial1.available() > 0)
  {
    Serial1.readBytes(numberHolder, 10);

    // Validate the incoming data format
    if (numberHolder[0] != 'X' && numberHolder[5] != 'Y')
    {
      light->clear(); // Clear the strip if data is invalid
      light->show();  // Update the strip
      return;
    }

    // DATA is in the format "X1234Y5678"
    // Extract the x and y coordinates from the data
    memcpy(xHolder, &numberHolder[1], 4);
    memcpy(yHolder, &numberHolder[6], 4);

    xHolder[4] = '\0'; // Null-terminate the string
    yHolder[4] = '\0'; // Null-terminate the string

    x = atoi(xHolder); // Convert x-coordinate string to integer
    y = atoi(yHolder); // Convert y-coordinate string to integer
  }

  if (digitalRead(actionButton) == LOW)
  {
    // Update minimum and maximum coordinates
    xMin = min(xMin, x);
    yMin = min(yMin, y);
    xMax = max(xMax, x);
    yMax = max(yMax, y);

    // Serial.println("DEST");
  }
  else if (digitalRead(printButton) == LOW)
  {
    idle = false;

    // Map x and y to image coordinates
    int xMapped = map(x, xMin, xMax, 0, img1.getWidth());
    int yMapped = map(y, yMin, yMax, 0, img1.getHeight());

    // Check if mapped coordinates are within bounds
    if (xMapped < 0 || xMapped >= img1.getWidth() || yMapped < 0 || yMapped >= img1.getHeight())
      return; // Exit if out of bounds

    int r, g, b, a;                                       // Variables to hold color values
    img1.getPixelValue(xMapped, yMapped, &r, &g, &b, &a); // Retrieve color values
    light->setPixelColor(0, light->Color(r, g, b));       // Set color of the single pixel
    light->show();                                        // Update the strip to apply changes

    // Serial.printf("X%dY%d", x, y); // Send color values to the ESP32
    // Serial.println(" at COLORS");
  }
  else if (!idle) // If currently not idle
  {
    light->clear(); // Clears the strip
    light->show();  // Update the strip
    delay(200);

    idle = true; // Set the state to idle
  }
  else
  {
    light->clear();
    delay(200);
  }
}
