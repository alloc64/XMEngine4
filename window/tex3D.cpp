/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../main.h"

Texture3D::Texture3D() {
    pixels = NULL;
    width = 0;
    height = 0;
    depth = 0;
    nMipMaps = 0;
    format = FORMAT_NONE;
    ownsMemory = true;
}

int Texture3D::getWidth(int mipMapLevel) {
    int a = width >> mipMapLevel;
    return (a == 0) ? 1 : a;
}

int Texture3D::getHeight(int mipMapLevel) {
    int a = height >> mipMapLevel;
    return (a == 0) ? 1 : a;
}

int Texture3D::getDepth(int mipMapLevel) {
    int a = depth >> mipMapLevel;
    return (a == 0) ? 1 : a;
}

int Texture3D::getMipMappedSize(int firstMipMapLevel, int nMipMapLevels, FORMAT srcFormat) {
    int w = getWidth(firstMipMapLevel) << 1, h = getHeight(firstMipMapLevel) << 1, d = getDepth(firstMipMapLevel) << 1;
    int level = 0, size = 0;

    if (srcFormat == 0) srcFormat = format;

    while (level < nMipMapLevels && (w != 1 || h != 1 || d != 1)) {
        if (w > 1) w >>= 1;
        if (h > 1) h >>= 1;
        if (d > 1) d >>= 1;
        if (isCompressedFormat(srcFormat)) size += ((w + 3) >> 2) * ((h + 3) >> 2) * d; else size += w * h * d;
        level++;
    }

    if (isCompressedFormat(srcFormat)) size *= getBytesPerBlock(srcFormat); else size *= getBytesPerPixel(srcFormat);
    return size;
}


GLuint Texture3D::Load(char *fileName, bool useRGBA) {
    DDS header;
    if (!glTexImage3DEXT) glTexImage3DEXT = (PFNGLTEXIMAGE3DEXTPROC) wglGetProcAddress("glTexImage3DEXT");
    FILE *file = fopen(fileName, "rb");
    if (!file) return 0;

    fread(&header, 1, sizeof(header), file);

    width = header.width;
    height = header.height;
    depth = header.depth;
    format = (header.bpp == 8) ? FORMAT_I8 : ((header.bpp == 24) ? FORMAT_RGB8 : FORMAT_RGBA8);
    nMipMaps = (header.nMipMaps == 0) ? 1 : header.nMipMaps;

    unsigned int size = getMipMappedSize(0, nMipMaps);
    pixels = new unsigned char[size];

    fread(pixels, 1, size, file);
    fclose(file);

    if (useRGBA && header.bpp >= 24) {
        int nChannels = header.bpp / 8;
        flipChannels(pixels, size / nChannels, nChannels);
    }

    glEnable(GL_TEXTURE_3D);
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_3D, texID);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int nChannels = getChannels(format);

    int mipMapLevel = 0;
    unsigned char *src;
    while ((src = getImage(mipMapLevel))) {
        glTexImage3DEXT(GL_TEXTURE_3D,
                        mipMapLevel,
                        internalFormats[format],
                        getWidth(mipMapLevel),
                        getHeight(mipMapLevel),
                        getDepth(mipMapLevel),
                        0,
                        type[nChannels],
                        GL_UNSIGNED_BYTE, pixels);
        mipMapLevel++;
    }

    glDisable(GL_TEXTURE_3D);

    printf("ok: 3D texture %s loaded sucessfully!\n", fileName);
    return texID;
}
