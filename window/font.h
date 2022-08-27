/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _FONT_H
#define _FONT_H

inline int next(int a) {
    int rval = 1;
    while (rval < a) rval <<= 1;
    return rval;
}

class Font {
public:
    bool Load(char *path, unsigned int height);

    GLuint fontListBase;
    float fontHeight;
private:

};

void glPrint(float r, float g, float b, float a, Font ft_font, float x, float y, char *fmt, ...);

//nema prepocitavani pro rozliseni, pouzita jenom v gui pro tlacitka a input boxy ktery jsou centerovany zevnitr
void glPrintn(float r, float g, float b, float a, Font ft_font, float x, float y, char *fmt, ...);

#endif
