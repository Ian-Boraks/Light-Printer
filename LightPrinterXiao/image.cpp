#include "image.h"
#include <stdint.h>
#include <algorithm>

/**
 * Constructor for the image class.
 * Initializes an image object with the given pixel data array, width, and height.
 *
 * @param imageArray Pointer to the array containing pixel data in 32-bit RGBA format.
 * @param width The width of the image in pixels.
 * @param height The height of the image in pixels.
 */
image::image(uint32_t *imageArray, int width, int height)
{
    _imageArray = imageArray; // Store the pointer to the image data array
    _width = width;           // Set the image width
    _height = height;         // Set the image height
}

/**
 * Retrieves the RGBA values of a pixel at a specified location in the image.
 *
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param r Pointer to store the red component of the pixel's color.
 * @param g Pointer to store the green component of the pixel's color.
 * @param b Pointer to store the blue component of the pixel's color.
 * @param a Pointer to store the alpha component (transparency) of the pixel's color.
 */
void image::getPixelValue(int x, int y, int *r, int *g, int *b, int *a)
{
    // Read the pixel at (x, y) and swap byte order from BGRA to RGBA
    uint32_t pixel = __builtin_bswap32(_imageArray[x * _width + y]);

    *r = (pixel >> 24) & 0xFF; // Extract the red component
    *g = (pixel >> 16) & 0xFF; // Extract the green component
    *b = (pixel >> 8) & 0xFF;  // Extract the blue component
    *a = pixel & 0xFF;         // Extract the alpha component
}

/**
 * Retrieves the size of the image.
 *
 * @param width Pointer to store the width of the image.
 * @param height Pointer to store the height of the image.
 */
void image::getSize(int *width, int *height)
{
    *width = _width;   // Provide the current image width
    *height = _height; // Provide the current image height
}
