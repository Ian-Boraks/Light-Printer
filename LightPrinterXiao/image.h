#include <stdint.h>

class image
{
public:
    explicit image(uint32_t *imageArray, int width, int height);

    void getSize(int *width, int *height);
    void getPixelValue(int x, int y, int *r, int *g, int *b, int *a);

    int getWidth() { return _width; }
    int getHeight() { return _height; }

private:
    uint32_t *_imageArray;
    int _width;
    int _height;
};