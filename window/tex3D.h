/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _TEX3D_H
#define _TEX3D_H

enum FORMAT {
    FORMAT_NONE = 0,

    FORMAT_I8 = 1,
    FORMAT_IA8 = 2,
    FORMAT_RGB8 = 3,
    FORMAT_RGBA8 = 4,

    FORMAT_I16 = 5,
    FORMAT_RG16 = 6,
    FORMAT_RGB16 = 7,
    FORMAT_RGBA16 = 8,

    FORMAT_I16F = 9,
    FORMAT_RG16F = 10,
    FORMAT_RGB16F = 11,
    FORMAT_RGBA16F = 12,

    FORMAT_I32F = 13,
    FORMAT_RG32F = 14,
    FORMAT_RGB32F = 15,
    FORMAT_RGBA32F = 16,
    // Packed formats
    FORMAT_RGB565 = 17,
    // Compressed formats
    FORMAT_DXT1 = 18,
    FORMAT_DXT3 = 19,
    FORMAT_DXT5 = 20,
    FORMAT_3DC = 21,


    // TODO: Fix these
    FORMAT_D16 = 22,
    FORMAT_D24 = 23,
};
struct DDS {
    unsigned int ddsIdentifier;
    unsigned char junk[8];
    unsigned int height;
    unsigned int width;

    unsigned int pitch;
    unsigned int depth;
    unsigned int nMipMaps;

    unsigned char junk3[52];
    unsigned int fourCC;
    unsigned int bpp;
    unsigned char junk4[36];
};

class Texture3D {
protected:
    unsigned char *pixels;
    int width, height, depth;
    int nMipMaps;
    FORMAT format;
    bool ownsMemory;

public:
    Texture3D();

    void free();

    int getMipMappedSize(int firstMipMapLevel = 0, int nMipMapLevels = 0x7fffffff, FORMAT srcFormat = FORMAT_NONE);

    int getWidth(int mipMapLevel = 0);

    int getHeight(int mipMapLevel = 0);

    int getDepth(int mipMapLevel = 0);

    unsigned char *getImage(int mipMapLevel) {
        return (mipMapLevel < nMipMaps) ? pixels + getMipMappedSize(0, mipMapLevel) : NULL;
    }

public:
    GLuint Load(char *fileName, bool useRGBA = true);

};


inline int getBytesPerBlock(FORMAT format) {
    return (format == FORMAT_DXT1) ? 8 : 16;
}

inline bool isCompressedFormat(FORMAT format) {
    return (format >= FORMAT_DXT1) && (format <= FORMAT_3DC);
}

inline void flipChannels(unsigned char *pixels, int nPixels, int channels) {
    unsigned char tmp;
    do {
        tmp = pixels[2];
        pixels[2] = pixels[0];
        pixels[0] = tmp;
        pixels += channels;
    } while (nPixels--);
}

inline int getBytesPerPixel(FORMAT format) {
    switch (format) {
        case FORMAT_I8:
            return 1;
        case FORMAT_IA8:
        case FORMAT_I16:
        case FORMAT_I16F:
        case FORMAT_RGB565:
        case FORMAT_D16:
            return 2;
        case FORMAT_RGB8:
        case FORMAT_D24:
            return 3;
        case FORMAT_RGBA8:
        case FORMAT_RG16:
        case FORMAT_RG16F:
        case FORMAT_I32F:
            return 4;
        case FORMAT_RGB16:
        case FORMAT_RGB16F:
            return 6;
        case FORMAT_RG32F:
            return 8;
        case FORMAT_RGBA16:
        case FORMAT_RGBA16F:
        case FORMAT_RGB32F:
            return 12;
        case FORMAT_RGBA32F:
            return 16;
        default:
            return 0;
    }
}


inline int getChannels(const FORMAT format) {
    switch (format) {
        case FORMAT_I8:
        case FORMAT_I16:
        case FORMAT_I16F:
        case FORMAT_I32F:
        case FORMAT_D16:
        case FORMAT_D24:
            return 1;
        case FORMAT_IA8:
        case FORMAT_RG16:
        case FORMAT_RG16F:
        case FORMAT_RG32F:
        case FORMAT_3DC:
            return 2;
        case FORMAT_RGB8:
        case FORMAT_RGB16:
        case FORMAT_RGB16F:
        case FORMAT_RGB32F:
        case FORMAT_RGB565:
        case FORMAT_DXT1:
            return 3;
        case FORMAT_RGBA8:
        case FORMAT_RGBA16:
        case FORMAT_RGBA16F:
        case FORMAT_RGBA32F:
        case FORMAT_DXT3:
        case FORMAT_DXT5:
            return 4;
        default:
            return 0;
    }
}

static int type[] = {0, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA};
static int internalFormats[] = {
        0,
        GL_INTENSITY,
        GL_LUMINANCE_ALPHA,
        GL_RGB8,
        GL_RGBA8,

        GL_INTENSITY16,
        0, // Unsupported
        GL_RGB16,
        GL_RGBA16,

        GL_INTENSITY_FLOAT16_ATI,
        0, // Unsupported
        GL_RGB_FLOAT16_ATI,
        GL_RGBA_FLOAT16_ATI,

        GL_INTENSITY_FLOAT32_ATI,
        0, // Unsupported
        GL_RGB_FLOAT32_ATI,
        GL_RGBA_FLOAT32_ATI,
        0, // Unsupported
        GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
        GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
        GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
        GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI,
        0, // Unsupported
        0, // Unsupported
};


#endif
