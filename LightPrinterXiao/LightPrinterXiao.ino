#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "image.h"       // Custom image class for handling image operations
#include "imageArrays.h" // Image data arrays

// Create an image object from image data with a resolution of 32x32
image img1(image1, 32, 32);

// Button pins
const int actionButton = 10; // Button for action (e.g., setting corners)
const int printButton = 9;   // Button for printing (e.g., displaying colors)

// Coordinates for top-left and bottom-right corners
int topLeft[2] = {0, 100};
int bottomRight[2] = {100, 0};

// Variables to track the extremes of coordinates
int xMin = INT_MAX;
int yMin = INT_MAX;
int xMax = INT_MIN;
int yMax = INT_MIN;

int x, y;        // Variables to store the current x and y coordinates
char xHolder[5]; // Buffer to hold the x-coordinate string
char yHolder[5]; // Buffer to hold the y-coordinate string

bool idle = false; // State to check if the device is idle

// Initialize the NeoPixel strip
Adafruit_NeoPixel *light = new Adafruit_NeoPixel(1, 5, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial1.begin(420000);
  Serial1.setTimeout(10);

  light->begin();
  light->setBrightness(50);
  light->clear();
  light->show();

  pinMode(actionButton, INPUT_PULLUP);
  pinMode(printButton, INPUT_PULLUP);
}

void loop()
{
  light->clear();
  char numberHolder[10] = {NULL}; // Buffer to hold incoming Serial data

  // Read from Serial1 if available
  while (Serial1.available() > 0)
  {
    Serial1.readBytes(numberHolder, 10);

    // Validate the incoming data format
    if (numberHolder[0] != 'X' && numberHolder[5] != 'Y')
      break;

    // DATA is in the format "X1234Y5678"
    // Extract the x and y coordinates from the data
    memcpy(xHolder, &numberHolder[1], 4);
    memcpy(yHolder, &numberHolder[6], 4);

    xHolder[4] = '\0'; // Null-terminate the string
    yHolder[4] = '\0'; // Null-terminate the string

    x = atoi(xHolder); // Convert x-coordinate string to integer
    y = atoi(yHolder); // Convert y-coordinate string to integer
  }

  // Check if action button is pressed
  if (digitalRead(actionButton) == LOW)
  {
    // Update minimum and maximum coordinates
    xMin = min(xMin, x);
    yMin = min(yMin, y);
    xMax = max(xMax, x);
    yMax = max(yMax, y);

    // Set the corrected corner points
    topLeft[0] = xMin;
    topLeft[1] = yMax;
    bottomRight[0] = xMax;
    bottomRight[1] = yMin;
  }
  else if (digitalRead(printButton) == LOW) // Check if print button is pressed
  {
    idle = false; // Reset the idle state

    // Map x and y to image coordinates
    int xMapped = map(x, topLeft[0], bottomRight[0], 0, img1.getWidth());
    int yMapped = map(y, topLeft[1], bottomRight[1], 0, img1.getHeight());

    // Check if mapped coordinates are within bounds
    if (xMapped < 0 || xMapped >= img1.getWidth() || yMapped < 0 || yMapped >= img1.getHeight())
      return; // Exit if out of bounds

    int r, g, b, a;                                       // Variables to hold color values
    img1.getPixelValue(xMapped, yMapped, &r, &g, &b, &a); // Retrieve color values
    light->setPixelColor(0, light->Color(r, g, b));       // Set color of the single pixel
    light->show();                                        // Update the strip to apply changes
  }
  else if (!idle) // If currently not idle
  {
    light->fill(); // Fill the strip (turn all pixels to the set color)
    light->show(); // Update the strip
    idle = true;   // Set the state to idle
  }
}
