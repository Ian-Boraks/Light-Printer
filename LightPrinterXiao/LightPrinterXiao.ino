#include <Arduino.h>
#include "image.h"
#include "imageArrays.h"

const image img1(image1, 2, 2);

const int actionButton = 0;
const int printButton = 1;

int topLeft[2] = {0, 0};
int bottomRight[2] = {0, 0};

AdafruitNeopixel light = AdafruitNeopixel(1, 6, NEO_GRB + NEO_KHZ800);

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

int setup()
{
    Serial.begin(9600);
    light.begin();
    light.show();

    pinMode(actionButton, INPUT);
    pinMode(printButton, INPUT);
}

int loop()
{
    if (Serial.available() > 0)
    {
        int x = Serial.parseInt();
        int y = Serial.parseInt();

        if (actionButton == 1)
        {
            if (topLeft[0] == 0 && topLeft[1] == 0)
            {
                topLeft[0] = x;
                topLeft[1] = y;
            }
            else
            {
                bottomRight[0] = x;
                bottomRight[1] = y;

                cornerCorrection(topLeft, bottomRight);
            }
        }

        else if (printButton == 1)
        {
            int xMapped = map(x, topLeft[0], bottomRight[0], 0, img1.getWidth());
            int yMapped = map(y, topLeft[1], bottomRight[1], 0, img1.getHeight());

            int r, g, b, a;
            img1.getPixelValue(xMapped, yMapped, &r, &g, &b, &a);
            light.setPixelColor(0, r, g, b);
            light.setBrightness(a);
            light.show();
        }
    }
}