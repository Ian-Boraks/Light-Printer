#include "image.h"
#include <algorithm>

image::image(int *imageArray, int width, int height)
{
    _imageArray = imageArray;
    _width = width;
    _height = height;
}

void image::getPixelValue(int x, int y, int *r, int *g, int *b, int *a)
{
    int pixel = _imageArray[x * _width + y];

    *r = (pixel >> 24) & 0xFF;
    *g = (pixel >> 16) & 0xFF;
    *b = (pixel >> 8) & 0xFF;
    *a = pixel & 0xFF;
}

void image::getSize(int *width, int *height)
{
    *width = _width;
    *height = _height;
}