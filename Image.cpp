#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define BYTE_BOUND(value) value < 0 ? 0 : (value > 255 ? 255 : value)
#define NORMALIZE(value) value >= 0 && value <= 1 ? value : (value > 1 ? 1 : 0) 
#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"



Image::Image(const char* filename)
{
    if(open(filename))
    {
        printf("Read %s\n", filename);
        size = w*h*channels;
    }
    else {
		printf("Failed to read %s\n", filename);
	}
    
}

Image::Image(int w, int h,int channels): w(w), h(h), channels(channels)
{
    size = w*h*channels;
    data = new uint8_t[size];
}


Image::Image(int w, int h, int channels, int* defaultValues): Image(w, h, channels)
{
    for(int i = 0; i < size; i += channels)
        {
            for(int j = 0; j < channels; j++)
            {
                data[i+j] = defaultValues[j];
            }
        }   
}


Image::Image(const Image& img): Image(img.w, img.h, img.channels)
{
    memcpy(data, img.data, size);
}

Image::~Image()
{
    stbi_image_free(data);
}

bool Image::open(const char *filename)
{
    data = stbi_load(filename, &w, &h, &channels, 0);
	return data != NULL;
}

bool Image::write(const char *filename)
{
    ImageType type = getType(filename);
    int success;
    switch(type)
    {
        case PNG: 
            success = stbi_write_png(filename, w, h, channels, data, w*channels);
            break;
        case BMP:
            success = stbi_write_bmp(filename, w, h, channels, data);
            break;
        case JPG:
            success = stbi_write_jpg(filename, w, h, channels, data, 100);
            break;
    }
    return success != 0;

}

ImageType Image::getType(const char *filename)
{
    const char* ext = strrchr(filename, '.');
    if(ext != nullptr)
    {
        if(strcmp(ext, ".png") == 0) 
        {
			return PNG;
		}
		else if(strcmp(ext, ".jpg") == 0) 
        {
			return JPG;
		}
		else if(strcmp(ext, ".bmp") == 0) 
        {
			return BMP;
		}
    }
    return PNG;
}

Image& Image::colorMask(float r, float g, float b)
{
    if(channels < 3) 
    {
        printf("It is not possible to perform a masking operation on an image with less than 3 channels\n");
        return *this;
    }

    r = NORMALIZE(r);
    g = NORMALIZE(g);
    b = NORMALIZE(b);

    for(int i = 0; i < size; i += channels)
    {
        data[i] *= r;
        data[i+1] *= g;
        data[i+2] *= b;
    }
    return *this;

}

Image& Image::grayScale()
{
    if(channels < 3) return *this;

    for(int i = 0; i < size; i +=channels)
    {
        int grayValue = 0.2126*data[i] + 0.7152*data[i+1] + 0.0722*data[i+2];
        memset(data+i, grayValue, 3);
    }
    return *this;

}

Image& Image::diffmap(Image& img)
{
    int compare_width = fmin(w, img.w);
    int compare_height = fmin(h, img.h);
    int compare_channels = fmin(channels, img.channels);
    for(uint32_t i=0; i<compare_height; ++i) {
		for(uint32_t j=0; j<compare_width; ++j) {
			for(uint8_t k=0; k<compare_channels; ++k) {
				data[(i*w+j)*channels+k] = BYTE_BOUND(abs(data[(i*w+j)*channels+k] - img.data[(i*img.w+j)*img.channels+k]));
			}
		}
	}
	return *this;
}

Image& Image::conv_simple_to_zero(uint8_t channel, uint32_t ker_w, uint32_t ker_h, double ker[], uint32_t cr, uint32_t cc)
{
    uint8_t new_data[w*h];
    uint64_t center = cr*ker_w + cc;
    for(uint64_t k=channel; k<size; k+=channels)
    {
        double c = 0;
        long i;
        for(i = -((long)cr); i < (long)(ker_h-cr); ++i)
        {
            long row = ((long)(k/channels))/w-i;
            if(row < 0 || row > h-1) continue;

            for(long j = -((long)cc); j < (long)(ker_w-cc); ++j)
            {
                long col = ((long)(k/channels))%w-j;
                if(col < 0 || col > w-1) continue;

                c += ker[center+i*(long)(ker_w+j)]*data[(row*w+col)*channels+channel];

            }
        }
        new_data[k/channels] = (uint8_t)BYTE_BOUND(round(c));
    }
    for(uint64_t k=channel; k<size; k+=channels)
    {
        data[k] = new_data[k/channels];
    }
    return *this;
}