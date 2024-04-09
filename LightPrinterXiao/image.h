class image
{
public:
    explicit image(int *imageArray, int width, int height);

    void getSize(int *width, int *height);
    void getPixelValue(int x, int y, int *r, int *g, int *b, int *a);

    int getWidth() { return _width; }
    int getHeight() { return _height; }

private:
    int *_imageArray;
    int _width;
    int _height;
};