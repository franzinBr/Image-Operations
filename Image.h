#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <cstdint>
#include <cstddef>

enum ImageType { PNG, JPG, BMP };

class Image
{
    private:
        /* data */
        ImageType getType(const char *filename);
    public:
        uint8_t* data = NULL;
        size_t size = 0;
        int w, h, channels;

        Image(const char* filename);
        Image(int w, int h, int channels);
        Image(const Image& img);
        ~Image();

        bool open(const char *filename);
        bool write(const char *filename);

        Image& conv_simple_to_zero(uint8_t channel, uint32_t ker_w, uint32_t ker_h, double ker[], uint32_t cr, uint32_t cc);

        Image& diffmap(Image& img);

        Image& colorMask(float r, float g, float b);
        Image& grayScale();
};

#endif // IMAGE_H