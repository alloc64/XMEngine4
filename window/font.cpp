/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../main.h"
#include <vector>
#include <string>

bool CreateFontList(FT_Face face, char ch, GLuint fontList, GLuint *textureBase) {
    FT_Glyph glyph = NULL;

    if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT)) {
        printf("error: FT_Load_Glyph failed!\n");
        return false;
    }

    if (FT_Get_Glyph(face->glyph, &glyph)) {
        printf("error: FT_Get_Glyph failed!\n");
        return false;
    }

    FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph) glyph;

    FT_Bitmap &bitmap = bitmap_glyph->bitmap;
    int width = next(bitmap.width);
    int height = next(bitmap.rows);
    GLubyte *fontTexData = new GLubyte[3 * width * height];
    if (!fontTexData) return false;

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            fontTexData[2 * (i + j * width)] = fontTexData[2 * (i + j * width) + 1] = (i >= bitmap.width ||
                                                                                       j >= bitmap.rows) ? 0
                                                                                                         : bitmap.buffer[
                                                                                              i + bitmap.width * j];
        }
    }

    glBindTexture(GL_TEXTURE_2D, textureBase[ch]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, fontTexData);

    delete[] fontTexData;

    glNewList(fontList + ch, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, textureBase[ch]);

    glPushMatrix();
    glTranslatef(bitmap_glyph->left, 0, 0);
    glTranslatef(0, bitmap_glyph->top - bitmap.rows, 0);
    float x = (float) bitmap.width / (float) width, y = (float) bitmap.rows / (float) height;

    glBegin(GL_QUADS);
    glTexCoord2d(0, 0);
    glVertex2f(0, bitmap.rows);
    glTexCoord2d(0, y);
    glVertex2f(0, 0);
    glTexCoord2d(x, y);
    glVertex2f(bitmap.width, 0);
    glTexCoord2d(x, 0);
    glVertex2f(bitmap.width, bitmap.rows);
    glEnd();

    glPopMatrix();
    glTranslatef(face->glyph->advance.x >> 6, 0, 0);
    glBitmap(0, 0, 0, 0, face->glyph->advance.x >> 6, 0, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glEndList();

    return true;
}


bool Font::Load(char *path, unsigned int height) {
    if (!path) return false;
    if (height < 0) return false;

    FT_Library library = NULL;
    FT_Face face = NULL;
    fontHeight = (float) height / .63f;
    GLuint *fontTextures = new GLuint[129];
    if (!fontTextures) return false;

    if (FT_Init_FreeType(&library)) {
        printf("error: FT_Init_FreeType failed!\n");
        return false;
    }

    if (FT_New_Face(library, path, 0, &face)) {
        printf("error: FT_New_Face failed, font %s not found!\n", path);
        return false;
    }

    FT_Set_Char_Size(face, height << 6, height << 6, 96, 96);
    fontListBase = glGenLists(128);
    glGenTextures(128, fontTextures);
    for (unsigned char i = 0; i < 128; i++) {
        if (!CreateFontList(face, i, fontListBase, fontTextures)) {
            printf("error: could not create FontList in font %s!\n", path);
            return false;
        }
    }
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    printf("ok: font %s loaded correctly!\n", path);

    return true;
}

void glPrint(float r, float g, float b, float a, Font ft_font, float x, float y, char *fmt, ...) {
    if (!fmt) return;
    glPushMatrix();
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, window.w, 0, window.h);
        glMatrixMode(GL_MODELVIEW);
        {
            char text[4096];
            va_list ap;

            if (!fmt) *text = 0;
            else {
                va_start(ap, fmt);
                vsprintf(text, fmt, ap);
                va_end(ap);
            }

            const char *start_line = text;
            vector <string> lines;
            const char *ch;
            for (ch = text; *ch; ch++) {
                if (*ch == '\n') {
                    string line;
                    for (const char *n = start_line; n < ch; n++) line.append(1, *n);
                    lines.push_back(line);
                    start_line = ch + 1;
                }
            }
            if (start_line) {
                string line;
                for (const char *n = start_line; n < ch; n++) line.append(1, *n);
                lines.push_back(line);
            }

            glDisable(GL_LIGHTING);
            glEnable(GL_TEXTURE_2D);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glListBase(ft_font.fontListBase);
            glColor4f(r, g, b, a);

            for (int i = 0; i < lines.size(); i++) {
                glPushMatrix();
                vec2 realPos = GetPosition(vec2(x, (y - ft_font.fontHeight * i)));
                glTranslatef(realPos.x, realPos.y - ft_font.fontHeight, 0);
                glCallLists(lines[i].length(), GL_UNSIGNED_BYTE, lines[i].c_str());
                glPopMatrix();
            }
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
        }
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glColor4f(1, 1, 1, 1);
    }
    glPopMatrix();
}

void glPrintn(float r, float g, float b, float a, Font ft_font, float x, float y, char *fmt, ...) {
    if (!fmt) return;
    glPushMatrix();
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, window.w, 0, window.h);
        glMatrixMode(GL_MODELVIEW);
        {
            char text[4096];

            va_list ap;
            va_start(ap, fmt);
            vsprintf(text, fmt, ap);
            va_end(ap);

            const char *start_line = text;
            vector <string> lines;
            const char *ch;
            for (ch = text; *ch; ch++) {
                if (*ch == '\n') {
                    string line;
                    for (const char *n = start_line; n < ch; n++) line.append(1, *n);
                    lines.push_back(line);
                    start_line = ch + 1;
                }
            }
            if (start_line) {
                string line;
                for (const char *n = start_line; n < ch; n++) line.append(1, *n);
                lines.push_back(line);
            }

            glDisable(GL_LIGHTING);
            glEnable(GL_TEXTURE_2D);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glListBase(ft_font.fontListBase);
            glColor4f(r, g, b, a);
            for (int i = 0; i < lines.size(); i++) {
                glPushMatrix();
                vec2 realPos = vec2(x, (y - (ft_font.fontHeight * i)));
                glTranslatef(realPos.x, realPos.y - ft_font.fontHeight, 0);
                glCallLists(lines[i].length(), GL_UNSIGNED_BYTE, lines[i].c_str());
                glPopMatrix();
            }
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
        }
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glColor4f(1, 1, 1, 1);
    }
    glPopMatrix();
}
