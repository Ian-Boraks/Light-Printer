#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "image.h"
#include "imageArrays.h"

image img1(image1, 32, 32);

const int actionButton = 10;
const int printButton = 9;

int topLeft[2] = {0, 100};
int bottomRight[2] = {100, 0};

int xMin = INT_MAX;
int yMin = INT_MAX;
int xMax = INT_MIN;
int yMax = INT_MIN;

int x, y;
char xHolder[5];
char yHolder[5];

bool idle = false;

Adafruit_NeoPixel *light = new Adafruit_NeoPixel(1, 5, NEO_GRB + NEO_KHZ800);

void cornerCorrection(int *_topLeft, int *_bottomRight)
{
  int temp;

  if (_topLeft[0] > _bottomRight[0])
  {
    temp = _topLeft[0];
    _topLeft[0] = _bottomRight[0];
    _bottomRight[0] = temp;
  }

  if (_topLeft[1] > _bottomRight[1])
  {
    temp = _topLeft[1];
    _topLeft[1] = _bottomRight[1];
    _bottomRight[1] = temp;
  }
}

void setup()
{
  Serial1.begin(420000);
  Serial1.setTimeout(10);
  // Serial.begin(115200);

  digitalWrite(LED_BUILTIN, HIGH);

  light->begin();
  light->setBrightness(50);
  light->clear();
  light->show();

  pinMode(actionButton, INPUT_PULLUP);
  pinMode(printButton, INPUT_PULLUP);
}

void loop()
{
  // delay(10);
  light->clear();
  char numberHolder[10] = {NULL};

  while (Serial1.available() > 0)
  {
    Serial1.readBytes(numberHolder, 10);

    if (numberHolder[0] != 'X' && numberHolder[5] != 'Y')
      break;

    memcpy(&xHolder, &numberHolder[1], 4);
    memcpy(&yHolder, &numberHolder[6], 4);

    yHolder[5] = '\0';
    xHolder[5] = '\0';

    x = atoi(xHolder);
    y = atoi(yHolder);
  }

  if (digitalRead(actionButton) == 0)
  {
    if (x < xMin)
      xMin = x;
    if (y < yMin)
      yMin = y;
    if (x > xMax)
      xMax = x;
    if (y > yMax)
      yMax = y;

    topLeft[0] = xMin;
    topLeft[1] = yMax;
    bottomRight[0] = xMax;
    bottomRight[1] = yMin;

    // Serial.printf("TLx%i TLy%i BRx%i BRl%i \n", topLeft[0], topLeft[1], bottomRight[0], bottomRight[1]);
  }
  else if (digitalRead(printButton) == 0)
  {
    idle = false;

    int xMapped = map(x, topLeft[0], bottomRight[0], 0, img1.getWidth());
    int yMapped = map(y, topLeft[1], bottomRight[1], 0, img1.getHeight());

    if (xMapped < 0 || xMapped >= img1.getWidth() || yMapped < 0 || yMapped >= img1.getHeight())
      return;

    int r, g, b, a;
    img1.getPixelValue(xMapped, yMapped, &r, &g, &b, &a);
    light->setPixelColor(0, light->Color(r, g, b));
    light->show();

    // Serial.printf("%R%i G%i B%i A%i \n", r, g, b, a);
  }
  else if (!idle)
  {
    light->fill();
    light->show();
    idle = true;
  }
}