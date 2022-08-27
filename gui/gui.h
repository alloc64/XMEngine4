/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _GUI_H
#define _GUI_H

#include "../main.h"

#define _YES 1
#define _NO 2
#define _NOTHING -1

extern Texture texture;
extern Window window;
extern Font basicFont;
extern Networking net;
extern Network netw;

struct Resolutions {
    int w, h, b;
};

inline Resolutions *GetResolutions(int &iNumResolutions) {
    int iCurrMode = 0;
    int iConMode = 0;

    DEVMODE devmode;
    ZeroMemory(&devmode, sizeof(DEVMODE));
    devmode.dmSize = sizeof(DEVMODE);
    devmode.dmDriverExtra = 0;

    while (EnumDisplaySettingsExA(NULL, iCurrMode, &devmode, 0)) iCurrMode++;
    Resolutions *resolution = new Resolutions[iCurrMode];
    iCurrMode = 0;

    while (EnumDisplaySettingsExA(NULL, iCurrMode, &devmode, 0)) {
        if (devmode.dmBitsPerPel == 32
            && devmode.dmPelsWidth >= 640
            && devmode.dmPelsHeight >= 480) {
            resolution[iConMode].w = devmode.dmPelsWidth;
            resolution[iConMode].h = devmode.dmPelsHeight;
            resolution[iConMode].b = devmode.dmBitsPerPel;
            iConMode++;
        }
        iCurrMode++;
    }
    iNumResolutions = iConMode;
    return resolution;
}

inline vec2 GetPosition(vec2 point) {
    vec2 finPoint;
    finPoint.x = (((float) window.w / 640.0f) * point.x);
    finPoint.y = window.h - (((float) window.h / 480.0f) * point.y);

    return finPoint;
}

inline void _GetCursorPos(POINT *p) {
    POINT mouse;
    GetCursorPos(&mouse);
    ScreenToClient(window.hWnd, &mouse);
    *p = mouse;
}

class ButtonFonted {
public:
    ButtonFonted() {
        this->min = vec2(0);
        this->max = vec2(0);
        this->center = vec2(0);
        strcpy(this->description, "");
    }

    bool Load(vec2 center, char *desc, Font ft_font, color4f hcolor, color4f ncolor, bool centered) {
        vec2 size = vec2(strlen(desc) * ft_font.fontHeight / 2, ft_font.fontHeight + 2);
        this->center = center;
        this->min = center;
        this->max = center + size;

        if (centered) this->center.x += (window.w) / 2;
        strcpy(this->description, desc);
        this->hcolor = hcolor;
        this->ncolor = ncolor;
        return true;
    }

    vec2 min;
    vec2 max;
    vec2 center;

    color4f hcolor;
    color4f ncolor;
    char description[1024];

    inline bool clicked() {
        glPushMatrix();
        {
            POINT mPos;
            _GetCursorPos(&mPos);
            mPos.y = window.h - mPos.y;

            if (mPos.x >= min.x && mPos.x <= max.x && mPos.y >= max.y && mPos.y <= min.y) {
                glPrint(hcolor.r, hcolor.g, hcolor.b, hcolor.a, basicFont, center.x, center.y, description);

                if (!GetKeyState(VK_LBUTTON) & 0x80) return false;
                if (GetKeyState(VK_LBUTTON) & 0x80) {
                    glPopMatrix();
                    return true;
                }
                if (!GetAsyncKeyState(VK_LBUTTON) & 0x80) return false;
            } else {
                glPrint(ncolor.r, ncolor.g, ncolor.b, ncolor.a, basicFont, center.x, center.y, description);
            }
        }
        glPopMatrix();
        return false;
    }

private:

};


class ButtonFonted2 {
public:

    inline bool Load(vec2 center, char *desc, Font ft_font, color4f hcolor, color4f ncolor, bool centered, int id = 0) {
        vec2 size = vec2(strlen(desc) * ft_font.fontHeight / 2, ft_font.fontHeight + 2);
        this->center = center;
        this->min = center;
        this->max = center + size;
        this->ft_font = ft_font;
        this->id = id;

        if (centered) this->center.x += (window.w) / 2;
        strcpy(this->description, desc);
        this->hcolor = hcolor;
        this->ncolor = ncolor;

        return true;
    }

    vec2 min;
    vec2 max;
    vec2 center;
    Font ft_font;
    int id;

    color4f hcolor;
    color4f ncolor;
    char description[1024];

    inline void setDescription(char *desc) {
        if (!desc) return;
        strcpy(this->description, desc);
        this->max = this->center + vec2(strlen(desc) * ft_font.fontHeight / 2, ft_font.fontHeight + 2);
    }

    inline bool clicked() {
        glPushMatrix();
        {
            POINT mPos;
            _GetCursorPos(&mPos);
            mPos.y = window.h - mPos.y;

            if (mPos.x >= min.x && mPos.x <= max.x && mPos.y >= min.y && mPos.y <= max.y) {
                glPrintn(hcolor.r, hcolor.g, hcolor.b, hcolor.a, ft_font, center.x, center.y + ft_font.fontHeight,
                         description);

                if (!GetKeyState(VK_LBUTTON) & 0x80) return false;
                if (GetKeyState(VK_LBUTTON) & 0x80) {
                    glPopMatrix();
                    return true;
                }
                if (!GetAsyncKeyState(VK_LBUTTON) & 0x80) return false;
            } else {
                glPrintn(ncolor.r, ncolor.g, ncolor.b, ncolor.a, ft_font, center.x, center.y + ft_font.fontHeight,
                         description);
            }
        }
        glPopMatrix();
        return false;
    }

private:

};

class ButtonFontedWithDescription {
public:

    inline bool
    Load(vec2 bcenter, vec2 center, const char *buttonText, char *desc, Font ft_font, color4f hcolor, color4f ncolor,
         color4f descColor) {
        vec2 size = vec2(strlen(desc) * ft_font.fontHeight / 2, ft_font.fontHeight + 2);
        this->center = center;
        this->bcenter = bcenter;
        this->min = bcenter;
        this->max = bcenter + size;
        this->ft_font = ft_font;

        strcpy(this->buttonText, buttonText);
        strcpy(this->description, desc);
        this->hcolor = hcolor;
        this->ncolor = ncolor;
        this->dcolor = descColor;

        return true;
    }

    inline void SetButtonText(char *str) {
        strcpy(buttonText, str);
    }

    vec2 min;
    vec2 max;
    vec2 center;
    vec2 bcenter;
    Font ft_font;

    color4f hcolor;
    color4f ncolor;
    color4f dcolor;
    char buttonText[1024];
    char description[1024];

    inline bool clicked() {
        glPushMatrix();
        {
            glPrintn(dcolor.r, dcolor.g, dcolor.b, dcolor.a, ft_font, center.x, center.y + ft_font.fontHeight,
                     description);

            POINT mPos;
            _GetCursorPos(&mPos);
            mPos.y = window.h - mPos.y;

            if (mPos.x >= min.x && mPos.x <= max.x && mPos.y >= min.y && mPos.y <= max.y) {
                glPrintn(hcolor.r, hcolor.g, hcolor.b, hcolor.a, ft_font, bcenter.x, bcenter.y + ft_font.fontHeight,
                         buttonText);

                if (!GetKeyState(VK_LBUTTON) & 0x80) return false;
                if (GetKeyState(VK_LBUTTON) & 0x80) {
                    glPopMatrix();
                    return true;
                }
                if (!GetAsyncKeyState(VK_LBUTTON) & 0x80) return false;
            } else {
                glPrintn(ncolor.r, ncolor.g, ncolor.b, ncolor.a, ft_font, bcenter.x, bcenter.y + ft_font.fontHeight,
                         buttonText);
            }
        }
        glPopMatrix();
        return false;
    }

private:

};

class ButtonTextured {
public:
    ButtonTextured() {
        this->min = vec2(0);
        this->max = vec2(0);
        this->center = vec2(0);
        this->texID = 0;
    }

    ButtonTextured(vec2 min, vec2 max, char *texPath, char *hovTexPath, color4f hcolor, color4f ncolor, bool centered) {
        this->min = GetPosition(min);
        this->max = GetPosition(max);
        this->center = GetPosition((min + max) / 2);
        if (centered) {
            this->min.x += (window.w) / 2;
            this->max.x += (window.w) / 2;
            this->center.x += (window.w) / 2;
        }
        this->texID = texture.LoadTexture(texPath, 0, 0);
        this->texHoverID = texture.LoadTexture(hovTexPath, 0, 0);
        if (this->texID == -1) printf("error: button texture %s not found!\n", texPath);
        if (this->texHoverID == -1) useHoverTexture = false; else useHoverTexture = true;

        this->hcolor = hcolor;
        this->ncolor = ncolor;
    }

    vec2 min;
    vec2 max;
    vec2 center;

    GLuint texID;
    GLuint texHoverID;

    color4f hcolor;
    color4f ncolor;

    bool useHoverTexture;

    inline bool clicked() {
        glPushMatrix();
        {
            POINT mPos;
            _GetCursorPos(&mPos);
            mPos.y = window.h - mPos.y;
            glAlphaFunc(GL_GREATER, 0.1f);
            glEnable(GL_ALPHA_TEST);

            if (mPos.x >= min.x && mPos.x <= max.x && mPos.y >= max.y && mPos.y <= min.y) {
                if (useHoverTexture) glBindTexture(GL_TEXTURE_2D, texHoverID);
                else
                    glColor4f(hcolor.r, hcolor.g, hcolor.b, hcolor.a);

                float quad[] =
                        {
                                0.0f, 0.0f, min.x, min.y, 0.0f,
                                1.0f, 0.0f, min.x, max.y, 0.0f,
                                1.0f, -1.0f, max.x, max.y, 0.0f,
                                0.0f, -1.0f, max.x, min.y, 0.0f
                        };

                glInterleavedArrays(GL_T2F_V3F, 0, quad);
                glDrawArrays(GL_QUADS, 0, 4);

                if (!GetKeyState(VK_LBUTTON) & 0x80) return false;
                if (GetKeyState(VK_LBUTTON) & 0x80) return true;
                if (!GetAsyncKeyState(VK_LBUTTON) & 0x80) return false;
            } else {
                glColor4f(ncolor.r, ncolor.g, ncolor.b, ncolor.a);
                glBindTexture(GL_TEXTURE_2D, texID);

                float quad[] =
                        {
                                0.0f, 0.0f, min.x, min.y, 0.0f,
                                1.0f, 0.0f, min.x, max.y, 0.0f,
                                1.0f, -1.0f, max.x, max.y, 0.0f,
                                0.0f, -1.0f, max.x, min.y, 0.0f
                        };

                glInterleavedArrays(GL_T2F_V3F, 0, quad);
                glDrawArrays(GL_QUADS, 0, 4);


            }

            glDisable(GL_ALPHA_TEST);
            glColor4f(ncolor.r, ncolor.g, ncolor.b, ncolor.a);
        }
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
        return false;
    }

private:

};

class Image {
public:
    bool Load(char *path, vec2 pos, vec2 size, color4f color, bool centered) {
        this->texID = texture.LoadTexture(path, 0, 0);
        this->size = size;
        this->pos = centered ? vec2(((window.w / 2) - size.x / 2) + pos.x, pos.y) : pos;
        this->color = color;

        if (this->texID == -1) {
            printf("error: couldn't load image %s\n", path);
            return false;
        }

        return true;
    }

    void Draw(void) {
        glPushMatrix();
        {
            glAlphaFunc(GL_GREATER, 0.1f);
            glEnable(GL_ALPHA_TEST);
            glEnable(GL_BLEND);
            glColor4f(color.r, color.g, color.b, color.a);
            glTranslatef(pos.x, pos.y, 0);
            glBindTexture(GL_TEXTURE_2D, this->texID);

            float quad[] =
                    {
                            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, size.x, 0.0f, 0.0f,
                            1.0f, -1.0f, size.x, size.y, 0.0f,
                            0.0f, -1.0f, 0.0f, size.y, 0.0f
                    };

            glInterleavedArrays(GL_T2F_V3F, 0, quad);
            glDrawArrays(GL_QUADS, 0, 4);

            glDisable(GL_BLEND);
            glDisable(GL_ALPHA_TEST);
        }
        glPopMatrix();
    }

    GLuint texID;
    color4f color;
    vec2 size;
    vec2 pos;

private:
};

class Image2 {
public:
    bool Load(char *path, vec2 pos, vec2 size, color4f color, bool centered) {
        this->texID = texture.LoadTexture(path, 0, 0);
        this->size = size;
        this->pos = centered ? vec2(((window.w / 2) - size.x / 2) + pos.x, pos.y) : GetPosition(pos);
        this->color = color;

        if (this->texID == -1) {
            printf("error: couldn't load image %s\n", path);
            return false;
        }

        return true;
    }

    void Draw(void) {
        glPushMatrix();
        {
            glAlphaFunc(GL_GREATER, 0.1f);
            glEnable(GL_ALPHA_TEST);
            glEnable(GL_BLEND);
            glColor4f(color.r, color.g, color.b, color.a);
            glTranslatef(pos.x, pos.y, 0);
            glBindTexture(GL_TEXTURE_2D, this->texID);

            float quad[] =
                    {
                            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, size.x, 0.0f, 0.0f,
                            1.0f, -1.0f, size.x, size.y, 0.0f,
                            0.0f, -1.0f, 0.0f, size.y, 0.0f
                    };

            glInterleavedArrays(GL_T2F_V3F, 0, quad);
            glDrawArrays(GL_QUADS, 0, 4);

            glDisable(GL_BLEND);
            glDisable(GL_ALPHA_TEST);
        }
        glPopMatrix();
    }

    GLuint texID;
    color4f color;
    vec2 size;
    vec2 pos;

private:
};

class BackgroundImage {
public:
    bool Load(char *path, color4f color) {
        this->color = color;
        this->texID = texture.LoadTexture(path, 0, 0);
        if (this->texID == -1) {
            printf("error: couldn't load BG image %s\n", path);
            return false;
        }
        return true;
    }

    void Draw(void) {
        glAlphaFunc(GL_GREATER, 0.1f);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);
        glColor4f(color.r, color.g, color.b, color.a);
        glBindTexture(GL_TEXTURE_2D, texID);
        float quad[] =
                {
                        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                        1.0f, 0.0f, window.w, 0.0f, 0.0f,
                        1.0f, -1.0f, window.w, window.h, 0.0f,
                        0.0f, -1.0f, 0.0f, window.h, 0.0f
                };

        glInterleavedArrays(GL_T2F_V3F, 0, quad);
        glDrawArrays(GL_QUADS, 0, 4);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);

    }

    color4f color;
    GLuint texID;
private:

};


class Text {
public:
    bool Load(char *desc, color4f fontColor, vec2 position, Font ft_font) {
        strcpy(this->desc, desc);
        strcpy(this->origdesc, desc);
        this->position = position;
        this->ft_font = ft_font;
        this->fontColor = fontColor;
        return true;
    }

    void SetText(char *text) {
        strcpy(this->desc, text);
    }

    void AddToText(char *text) {
        strcat(this->desc, text);
    }

    void SetDefaultText() {
        strcpy(this->desc, this->origdesc);
    }

    void Draw(void) {
        glPrintn(fontColor.r, fontColor.g, fontColor.b, fontColor.a, ft_font, position.x, position.y, desc);
        return;
    }

    char desc[1024];
    char origdesc[1024];
    vec2 position;
    Font ft_font;
    color4f fontColor;

private:

};

class InputBox {
public:
    bool Load(char *path, char *onClick, char *onHover, char *inputBoxDesc, color4f descFontColor, color4f fontColor,
              vec2 size, vec2 position, Font ft_font, int fontMargin, int shortTextLenght, bool centered,
              bool passButton) {
        this->texID = texture.LoadTexture(path, 0, 0);
        this->OnClickTexID = texture.LoadTexture(onClick, 0, 0);
        this->OnHoverTexID = texture.LoadTexture(onHover, 0, 0);
        strcpy(this->desc, "");
        strcpy(this->outputText, "");
        strcpy(this->inputBoxdesc, inputBoxDesc);
        this->size = size;
        this->position = centered ? vec2((window.w / 2) - size.x / 2, (vec2(0, position.y)).y) : (position);
        this->realPos = centered ? vec2(((window.w / 2) - size.x / 2), (vec2(0, position.y)).y) : vec2(position.x,
                                                                                                       position.y);
        this->ft_font = ft_font;
        this->fontMargin = fontMargin;
        this->shortTextLenght = shortTextLenght;
        this->bActive = false;
        this->bCentered = centered;
        this->descFontColor = descFontColor;
        this->fontColor = fontColor;
        this->bPassButton = passButton;
        this->bChanged = false;
        this->bChanging = false;

        if (this->texID == -1 || this->OnClickTexID == -1 || this->OnHoverTexID == -1) {
            printf("error: couldn't load inputbox images!\n");
            return false;
        }

        return true;
    }

    void Draw(void) {
        glDisable(GL_DEPTH_TEST);
        glPushMatrix();
        {
            POINT mPos;
            _GetCursorPos(&mPos);
            mPos.y = window.h - mPos.y;

            if (mPos.x >= position.x && mPos.x <= position.x + size.x && mPos.y >= (position.y - fontMargin) &&
                mPos.y <= (position.y - fontMargin) + size.y && GetKeyState(VK_LBUTTON) & 0x80 && !bDisable)
                bActive = true;
            if (!(mPos.x >= position.x && mPos.x <= position.x + size.x && mPos.y >= (position.y - fontMargin) &&
                  mPos.y <= (position.y - fontMargin) + size.y) && GetKeyState(VK_LBUTTON) & 0x80)
                bActive = false;

            bCentered ? glPrintn(descFontColor.r, descFontColor.g, descFontColor.b, descFontColor.a, ft_font,
                                 realPos.x - strlen(inputBoxdesc) * ft_font.fontHeight / 2, realPos.y, inputBoxdesc)
                      : glPrintn(descFontColor.r, descFontColor.g, descFontColor.b, descFontColor.a, ft_font,
                                 realPos.x - strlen(inputBoxdesc) * ft_font.fontHeight / 2, realPos.y, inputBoxdesc);

            glAlphaFunc(GL_GREATER, 0.1f);
            glEnable(GL_ALPHA_TEST);

            if (bActive) {
                glPushMatrix();
                {
                    glTranslatef(position.x, position.y - fontMargin, 0);
                    glBindTexture(GL_TEXTURE_2D, OnClickTexID);
                    float quad[] =
                            {
                                    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                    1.0f, 0.0f, size.x, 0.0f, 0.0f,
                                    1.0f, -1.0f, size.x, size.y, 0.0f,
                                    0.0f, -1.0f, 0.0f, size.y, 0.0f
                            };

                    glInterleavedArrays(GL_T2F_V3F, 0, quad);
                    glDrawArrays(GL_QUADS, 0, 4);

                    string text;
                    char string[2];
                    char ch = window.GetChar();

                    if (ch != 0) {
                        bChanging = true;
                        if (ch == 8) {
                            short int size = strlen(desc);
                            if (size != 0) {
                                desc[size - 1] = '\0';
                                outputText[strlen(outputText) - 1] = '\0';
                            }
                        } else {
                            sprintf(string, "%c", ch);
                            strcat(desc, string);
                            strcat(outputText, string);
                        }
                        window.SetChar(0);
                    } else {
                        /*if(GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('V') && !bPaste)
                        {
                            if (OpenClipboard(NULL))
                            {
                                HANDLE clip = GetClipboardData(CF_TEXT);
                                text = (LPSTR)GlobalLock(clip);
                                GlobalUnlock(clip);
                                CloseClipboard();
                            }
                            strcat(desc, text.c_str());
                            strcat(outputText, text.c_str());
                            bPaste = true;
                        }  */
                    }

                    if (!GetAsyncKeyState(VK_CONTROL) && !GetAsyncKeyState('V') && bPaste) bPaste = false;
                }
                glPopMatrix();

                short int size = strlen(desc);
                if (!bPassButton) {
                    if (size >= shortTextLenght) {
                        shortDesc[shortTextLenght] = '\0';
                        int j = 0;
                        for (int i = size - shortTextLenght; i < size; i++) {
                            shortDesc[j++] = desc[i];
                        }
                    }
                } else {
                    if (size >= shortTextLenght) {
                        shortDesc[shortTextLenght] = '\0';
                        int j = 0;
                        for (int i = size - shortTextLenght; i < size; i++) {
                            shortDesc[j++] = '*';
                        }
                    } else {
                        for (int i = 0; i < strlen(desc); i++) {
                            desc[i] = '*';
                        }
                    }
                }
                bCentered ? glPrintn(fontColor.r, fontColor.g, fontColor.b, fontColor.a, ft_font,
                                     realPos.x + (ft_font.fontHeight / 2), realPos.y,
                                     size >= shortTextLenght ? shortDesc : desc)
                          : glPrintn(fontColor.r, fontColor.g, fontColor.b, fontColor.a, ft_font,
                                     realPos.x + (ft_font.fontHeight / 2), realPos.y,
                                     size >= shortTextLenght ? shortDesc : desc);

            } else {
                bChanged = false;
                if (bChanging) {
                    bChanging = false;
                    bChanged = true;
                }
                glPushMatrix();
                {
                    glTranslatef(position.x, position.y - fontMargin, 0);
                    if (mPos.x >= position.x && mPos.x <= position.x + size.x
                        && mPos.y >= (position.y - fontMargin) && mPos.y <= (position.y - fontMargin) + size.y) {
                        glBindTexture(GL_TEXTURE_2D, OnHoverTexID);
                    } else {
                        glBindTexture(GL_TEXTURE_2D, texID);
                    }
                    float quad[] =
                            {
                                    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                    1.0f, 0.0f, size.x, 0.0f, 0.0f,
                                    1.0f, -1.0f, size.x, size.y, 0.0f,
                                    0.0f, -1.0f, 0.0f, size.y, 0.0f
                            };

                    glInterleavedArrays(GL_T2F_V3F, 0, quad);
                    glDrawArrays(GL_QUADS, 0, 4);
                }
                glPopMatrix();
                short int size = strlen(desc);
                bCentered ? glPrintn(fontColor.r, fontColor.g, fontColor.b, fontColor.a, ft_font,
                                     realPos.x + (ft_font.fontHeight / 2), realPos.y,
                                     size >= shortTextLenght ? shortDesc : desc)
                          : glPrintn(fontColor.r, fontColor.g, fontColor.b, fontColor.a, ft_font,
                                     realPos.x + (ft_font.fontHeight / 2), realPos.y,
                                     size >= shortTextLenght ? shortDesc : desc);

            }
        }

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_ALPHA_TEST);
        glPopMatrix();
    }

    void Disable() {
        bDisable = true;
    }

    void Enable() {
        bDisable = false;
    }

    char *GetText() {
        return outputText;
    }

    void SetText(char *input) {
        strcpy(outputText, input);
        for (int i = 0; i < shortTextLenght; i++) {
            if (!bPassButton)
                shortDesc[i] = input[i];
            else
                shortDesc[i] = '*';
        }
        shortDesc[shortTextLenght] = '\0';

        if (bPassButton) {
            for (int i = 0; i < strlen(input); i++) {
                desc[i] = '*';
            }
            desc[strlen(input)] = '\0';
        } else {
            strcpy(desc, input);
        }
        return;
    }

    void SetActive() {
        bActive = true;
    }

    void UnsetActive() {
        bActive = false;
    }

    bool IsActive() {
        return bActive;
    }

    bool Changed() {
        return bChanged;
    }

    char desc[1024];
    char shortDesc[256];
    char inputBoxdesc[256];
    char outputText[4096];
    GLuint texID;
    GLuint OnClickTexID;
    GLuint OnHoverTexID;
    vec2 size;
    vec2 position;
    vec2 realPos;
    Font ft_font;
    color4f descFontColor;
    color4f fontColor;
    int fontMargin;
    int shortTextLenght;
    bool bActive;
    bool bCentered;
    bool bPassButton;
    bool bDisable;
    bool bPaste;
    bool bChanged;
    bool bChanging;

private:

};

class ButtonTextured2 {
public:
    ButtonTextured2() {

    }

    bool Load(char *texPath, char *texHoverPath, vec2 size, vec2 pos, bool centered) {
        this->texID = texture.LoadTexture(texPath, 0, 0);
        this->OnHoverTexID = texture.LoadTexture(texHoverPath, 0, 0);
        this->bCentered = centered;
        this->position = centered ? vec2(((window.w / 2) - (size.x / 2)) + pos.x, GetPosition(vec2(0, pos.y)).y)
                                  : GetPosition(pos);
        this->size = size;

        return true;
    }

    bool clicked(void) {
        glPushMatrix();
        glAlphaFunc(GL_GREATER, 0.1f);
        glEnable(GL_ALPHA_TEST);
        POINT mPos;
        _GetCursorPos(&mPos);
        mPos.y = window.h - mPos.y;
        glTranslatef(position.x, position.y, 0);
        if (mPos.x >= position.x && mPos.x <= position.x + size.x
            && mPos.y >= position.y && mPos.y <= position.y + size.y) {
            glBindTexture(GL_TEXTURE_2D, OnHoverTexID);
            float quad[] =
                    {
                            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, size.x, 0.0f, 0.0f,
                            1.0f, -1.0f, size.x, size.y, 0.0f,
                            0.0f, -1.0f, 0.0f, size.y, 0.0f
                    };

            glInterleavedArrays(GL_T2F_V3F, 0, quad);
            glDrawArrays(GL_QUADS, 0, 4);
            if (!GetKeyState(VK_LBUTTON) & 0x80) return false;
            if (GetKeyState(VK_LBUTTON) & 0x80) {
                glPopMatrix();
                return true;
            }
            if (!GetAsyncKeyState(VK_LBUTTON) & 0x80) return false;
        } else {
            glBindTexture(GL_TEXTURE_2D, texID);
            float quad[] =
                    {
                            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, size.x, 0.0f, 0.0f,
                            1.0f, -1.0f, size.x, size.y, 0.0f,
                            0.0f, -1.0f, 0.0f, size.y, 0.0f
                    };

            glInterleavedArrays(GL_T2F_V3F, 0, quad);
            glDrawArrays(GL_QUADS, 0, 4);
        }
        glDisable(GL_ALPHA_TEST);
        glPopMatrix();
        return false;
    }

    vec2 position;
    vec2 size;
    bool bCentered;
    GLuint texID;
    GLuint OnHoverTexID;
private:
};

class ButtonTextured3 {
public:
    ButtonTextured3() {

    }

    bool Load(char *texPath, char *texHoverPath, vec2 size, vec2 pos) {
        this->texID = texture.LoadTexture(texPath, 0, 0);
        this->OnHoverTexID = texture.LoadTexture(texHoverPath, 0, 0);
        this->position = pos - size;

        this->size = size;

        if (this->OnHoverTexID == -1 || this->texID == -1) return false;

        return true;
    }

    bool clicked(void) {
        glPushMatrix();
        glAlphaFunc(GL_GREATER, 0.1f);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);
        POINT mPos;
        _GetCursorPos(&mPos);
        mPos.y = window.h - mPos.y;
        glTranslatef(position.x, position.y, 0);
        if (mPos.x >= position.x && mPos.x <= position.x + size.x
            && mPos.y >= position.y && mPos.y <= position.y + size.y) {
            glBindTexture(GL_TEXTURE_2D, OnHoverTexID);
            float quad[] =
                    {
                            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, size.x, 0.0f, 0.0f,
                            1.0f, -1.0f, size.x, size.y, 0.0f,
                            0.0f, -1.0f, 0.0f, size.y, 0.0f
                    };

            glInterleavedArrays(GL_T2F_V3F, 0, quad);
            glDrawArrays(GL_QUADS, 0, 4);
            if (!GetKeyState(VK_LBUTTON) & 0x80) {
                glDisable(GL_ALPHA_TEST);
                glDisable(GL_BLEND);
                return false;
            }
            if (GetKeyState(VK_LBUTTON) & 0x80) {
                glDisable(GL_ALPHA_TEST);
                glDisable(GL_BLEND);
                glPopMatrix();
                return true;
            }
            if (!GetKeyState(VK_LBUTTON) & 0x80) {
                glDisable(GL_ALPHA_TEST);
                glDisable(GL_BLEND);
                return false;
            }
        } else {
            glBindTexture(GL_TEXTURE_2D, texID);
            float quad[] =
                    {
                            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, size.x, 0.0f, 0.0f,
                            1.0f, -1.0f, size.x, size.y, 0.0f,
                            0.0f, -1.0f, 0.0f, size.y, 0.0f
                    };

            glInterleavedArrays(GL_T2F_V3F, 0, quad);
            glDrawArrays(GL_QUADS, 0, 4);
        }
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
        glPopMatrix();
        return false;
    }

    vec2 position;
    vec2 size;
    GLuint texID;
    GLuint OnHoverTexID;
private:
};


class Progressbar {
public:
    bool Load(vec2 pos, vec2 size, vec2 textPos, Font ft_font, float percents, float lineWidth, color4f color) {
        this->position = pos;
        this->size = size;
        this->percents = percents;
        this->lineWidth = lineWidth;
        this->color = color;
        this->textPos = textPos;
        this->ft_font = ft_font;
        strcpy(this->pbText, "");

        return true;
    }

    void SetPercents(float per) {
        this->percents = per;
        return;
    }

    void SetMessage(char *mess) {
        strcpy(pbText, mess);
        return;
    }

    void Draw(void) {
        glPushMatrix();
        {
            glTranslatef(position.x, position.y, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glColor4f(color.r, color.g, color.b, color.a);
            vec3 line[] =
                    {
                            vec3(size.x, 0.0, 0.0),
                            vec3(size.y / 100 * percents, 0.0, 0.0)
                    };
            glLineWidth(lineWidth);
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, line);
            glDrawArrays(GL_LINES, 0, 2);
            glDisableClientState(GL_VERTEX_ARRAY);
            glLineWidth(1.0);
            glColor4f(1, 1, 1, 1);
        }
        glPopMatrix();
        glPrintn(1, 1, 1, 1, ft_font, textPos.x, textPos.y + ft_font.fontHeight, pbText);
    }

    vec2 position;
    vec2 size;
    vec2 textPos;
    char pbText[1024];
    float percents;
    float lineWidth;
    color4f color;
    Font ft_font;
private:

};


class ProgressbarTextured {
public:
    bool Load(char *barTexturePath,
              char *backgroundPath,
              vec2 pos,
              Font ft_font,
              float percents,
              vec2 barSize,
              color4f color,
              char *pbText,
              vec2 textPos) {
        this->position = pos;
        this->percents = percents;
        this->barSize = barSize;
        this->color = color;
        this->textPos = textPos;
        this->ft_font = ft_font;

        strcpy(this->pbText, pbText);
        this->texID = texture.LoadTexture(barTexturePath, 0, 0);
        this->bgTexID = texture.LoadTexture(backgroundPath, 0, 0);

        return true;
    }

    void SetPercents(float per) {
        this->percents = per;
        return;
    }

    void SetMessage(char *mess) {
        strcpy(pbText, mess);
        return;
    }

    void Draw(void) {
        glPushMatrix();
        {
            glTranslatef(position.x, position.y, 0);
            glBindTexture(GL_TEXTURE_2D, 0);

            glAlphaFunc(GL_GREATER, 0.1f);
            glEnable(GL_ALPHA_TEST);
            glEnable(GL_BLEND);

            glBindTexture(GL_TEXTURE_2D, bgTexID);
            float bquad[] =
                    {
                            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, barSize.x - 1.3, 0.0f, 0.0f,
                            1.0f, -1.0f, barSize.x + 1.3, barSize.y - 1.3, 0.0f,
                            0.0f, -1.0f, 0.0f, barSize.y + 1.3, 0.0f
                    };

            glInterleavedArrays(GL_T2F_V3F, 0, bquad);
            glDrawArrays(GL_QUADS, 0, 4);

            glBindTexture(GL_TEXTURE_2D, texID);
            float quad[] =
                    {
                            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, barSize.x / 100 * percents, 0.0f, 0.0f,
                            1.0f, -1.0f, barSize.x / 100 * percents, barSize.y, 0.0f,
                            0.0f, -1.0f, 0.0f, barSize.y, 0.0f
                    };

            glInterleavedArrays(GL_T2F_V3F, 0, quad);
            glDrawArrays(GL_QUADS, 0, 4);

            glDisable(GL_BLEND);
            glDisable(GL_ALPHA_TEST);

        }
        glPopMatrix();
        glPrintn(color.r, color.g, color.b, color.a, ft_font, textPos.x, textPos.y + ft_font.fontHeight, pbText);
    }

    vec2 position;
    vec2 size;
    vec2 textPos;
    vec2 barSize;
    GLuint texID;
    GLuint bgTexID;
    char pbText[1024];
    float percents;
    float lineWidth;
    color4f color;
    Font ft_font;
private:

};


class Line {
public:
    bool Load(vec2 pos, vec2 size, float lineWidth, color4f color) {
        this->position = pos;
        this->size = size;
        this->lineWidth = lineWidth;
        this->color = color;
        return true;
    }

    void Draw(void) {
        glPushMatrix();
        {
            glTranslatef(position.x, position.y, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glColor4f(color.r, color.g, color.b, color.a);
            vec3 line[] =
                    {
                            vec3(size.x, 0.0, 0.0),
                            vec3(size.y, 0.0, 0.0)
                    };
            glLineWidth(lineWidth);
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, sizeof(vec3), line);
            glDrawArrays(GL_LINES, 0, 2);
            glDisableClientState(GL_VERTEX_ARRAY);
            glLineWidth(1.0);
            glColor4f(1, 1, 1, 1);
        }
        glPopMatrix();
    }

    vec2 position;
    vec2 size;
    float lineWidth;
    color4f color;
private:

};

class LineVertical {
public:
    bool Load(vec2 pos, vec2 size, float lineWidth, color4f color) {
        this->position = pos;
        this->size = size;
        this->lineWidth = lineWidth;
        this->color = color;
        return true;
    }

    void Draw(void) {
        glPushMatrix();
        {
            glTranslatef(position.x, position.y, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glColor4f(color.r, color.g, color.b, color.a);
            vec3 line[] =
                    {
                            vec3(0.0, size.x, 0.0),
                            vec3(0.0, size.y, 0.0)
                    };
            glLineWidth(lineWidth);
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, sizeof(vec3), line);
            glDrawArrays(GL_LINES, 0, 2);
            glDisableClientState(GL_VERTEX_ARRAY);
            glLineWidth(1.0);
            glColor4f(1, 1, 1, 1);
        }
        glPopMatrix();
    }

    vec2 position;
    vec2 size;
    float lineWidth;
    color4f color;
private:

};

class Message {
public:
    bool Create(char *bgPath, char *message, char *button1, char *button2, Font ft_font, vec2 size, color4f color,
                color4f bhcolor, color4f bncolor) {
        if ((this->texID = texture.LoadTexture(bgPath, 0, 0)) == -1) return false;
        if (!button[0].Load(vec2(window.w / 2 - size.x / 4, (window.h / 2 - size.y / 2.5)), button1, ft_font, bhcolor,
                            bncolor, false))
            return false;
        if (!button[1].Load(vec2(window.w / 2 + size.x / 4, (window.h / 2 - size.y / 2.5)), button2, ft_font, bhcolor,
                            bncolor, false))
            return false;
        strcpy(this->message, message);
        this->size = size;
        this->color = color;
        this->ft_font = ft_font;
        bReturnFlag = false;
        return true;
    }

    ButtonFonted2 button[2];
    char message[1024];
    color4f color;
    Font ft_font;
    vec2 size;
    GLuint texID;
    bool bReturnFlag;

    int Draw(void) {
        glPushMatrix();
        {
            glAlphaFunc(GL_GREATER, 0.1f);
            glEnable(GL_ALPHA_TEST);
            glEnable(GL_BLEND);
            glColor4f(color.r, color.g, color.b, color.a);

            glTranslatef(window.w / 2 - size.x / 2, window.h / 2 - size.y / 2, 0);
            glBindTexture(GL_TEXTURE_2D, texID);
            float quad[] =
                    {
                            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, size.x, 0.0f, 0.0f,
                            1.0f, -1.0f, size.x, size.y, 0.0f,
                            0.0f, -1.0f, 0.0f, size.y, 0.0f
                    };

            glInterleavedArrays(GL_T2F_V3F, 0, quad);
            glDrawArrays(GL_QUADS, 0, 4);
            glColor4f(1, 1, 1, 1);
            glDisable(GL_BLEND);
            glDisable(GL_ALPHA_TEST);
        }
        glPopMatrix();

        glPrintn(color.r, color.g, color.b, color.a,
                 ft_font,
                 window.w / 2 - size.x / 2.5,
                 window.h / 2 + size.y / 2.0,
                 message);
        int iRetFlag = _NOTHING;

        if (button[0].clicked()) iRetFlag = _YES;
        if (button[1].clicked()) iRetFlag = _NO;
        return iRetFlag;
    }

private:

};


class Message2 {
public:
    bool Create(char *bgPath, char *message,
                char *button,
                Font ft_font, vec2 size,
                vec2 butpos,
                color4f color, color4f bhcolor,
                color4f bncolor) {
        if ((this->texID = texture.LoadTexture(bgPath, 0, 0)) == -1) return false;
        if (!butt.Load(vec2(window.w / 2 - butpos.x, (window.h / 2 - butpos.y)), button, ft_font, bhcolor, bncolor,
                       false))
            return false;
        strcpy(this->message, message);
        this->size = size;
        this->color = color;
        this->ft_font = ft_font;
        return true;
    }

    ButtonFonted2 butt;
    char message[1024];
    color4f color;
    Font ft_font;
    vec2 size;
    GLuint texID;

    void SetMessage(char *msg) {
        strcpy(message, msg);
    }

    bool Draw(void) {
        glPushMatrix();
        {
            glAlphaFunc(GL_GREATER, 0.1f);
            glEnable(GL_ALPHA_TEST);
            glEnable(GL_BLEND);
            glColor4f(color.r, color.g, color.b, color.a);

            glTranslatef(window.w / 2 - size.x / 2, window.h / 2 - size.y / 2, 0);
            glBindTexture(GL_TEXTURE_2D, texID);
            float quad[] =
                    {
                            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, size.x, 0.0f, 0.0f,
                            1.0f, -1.0f, size.x, size.y, 0.0f,
                            0.0f, -1.0f, 0.0f, size.y, 0.0f
                    };

            glInterleavedArrays(GL_T2F_V3F, 0, quad);
            glDrawArrays(GL_QUADS, 0, 4);
            glColor4f(1, 1, 1, 1);
            glDisable(GL_BLEND);
            glDisable(GL_ALPHA_TEST);
        }
        glPopMatrix();

        glPrintn(color.r, color.g, color.b, color.a,
                 ft_font,
                 window.w / 2 - size.x / 2.3,
                 window.h / 2 + size.y / 2.5,
                 message);

        return butt.clicked();
    }

private:

};


class Message3 {
public:
    bool Create(char *bgPath,
                char *message,
                char *button,
                Font ft_font,
                vec2 size,
                vec2 bgpos,
                vec2 butpos,
                color4f color, color4f bhcolor,
                color4f bncolor) {
        if ((this->texID = texture.LoadTexture(bgPath, 0, 0)) == -1) return false;
        if (!butt.Load(vec2(window.w / 2 - butpos.x, (window.h / 2 - butpos.y)), button, ft_font, bhcolor, bncolor,
                       false))
            return false;
        strcpy(this->message, message);
        this->bgpos = bgpos;
        this->size = size;
        this->color = color;
        this->ft_font = ft_font;
        return true;
    }

    ButtonFonted2 butt;
    char message[1024];
    color4f color;
    Font ft_font;
    vec2 size;
    vec2 bgpos;
    GLuint texID;

    void SetMessage(char *msg) {
        strcpy(message, msg);
    }

    bool Draw(void) {
        glPushMatrix();
        {
            glAlphaFunc(GL_GREATER, 0.1f);
            glEnable(GL_ALPHA_TEST);
            glEnable(GL_BLEND);
            glColor4f(color.r, color.g, color.b, color.a);

            glTranslatef(bgpos.x - size.x / 2, bgpos.y + size.y / 2, 0);
            glBindTexture(GL_TEXTURE_2D, texID);
            float quad[] =
                    {
                            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, size.x, 0.0f, 0.0f,
                            1.0f, -1.0f, size.x, size.y, 0.0f,
                            0.0f, -1.0f, 0.0f, size.y, 0.0f
                    };

            glInterleavedArrays(GL_T2F_V3F, 0, quad);
            glDrawArrays(GL_QUADS, 0, 4);
            glColor4f(1, 1, 1, 1);
            glDisable(GL_BLEND);
            glDisable(GL_ALPHA_TEST);
        }
        glPopMatrix();

        glPrintn(color.r, color.g, color.b, color.a,
                 ft_font,
                 window.w / 2 - size.x / 2.3,
                 window.h / 2 + size.y / 2.5,
                 message);

        return butt.clicked();
    }

private:

};


class CheckBox {
public:
    bool
    Load(char *texPath, char *texHoverPath, char *texCheckedPath, char *descrition, vec2 size, vec2 pos, Font ft_font) {
        this->texID = texture.LoadTexture(texPath, 0, 0);
        this->OnHoverTexID = texture.LoadTexture(texHoverPath, 0, 0);
        this->CheckedTexID = texture.LoadTexture(texCheckedPath, 0, 0);
        if (this->texID == -1 || this->OnHoverTexID == -1 || this->CheckedTexID == -1) return false;
        this->position = pos - size;
        this->size = size;
        strcpy(this->description, descrition);
        this->bClicked = false;
        this->ft_font = ft_font;

        return true;
    }

    bool checked(void) {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.1f);
        POINT mPos;
        _GetCursorPos(&mPos);
        mPos.y = window.h - mPos.y;

        glPrintn(1, 1, 1, 1, ft_font, position.x + size.x * 1.5,
                 position.y + size.y / 2.0 + ft_font.fontHeight - ft_font.fontHeight / 4, description);


        if (!bClicked) {
            glPushMatrix();
            {
                glTranslatef(position.x, position.y, 0);
                if (mPos.x >= position.x && mPos.x <= position.x + size.x && mPos.y >= position.y &&
                    mPos.y <= position.y + size.y) {
                    glBindTexture(GL_TEXTURE_2D, OnHoverTexID);
                    float quad[] =
                            {
                                    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                    1.0f, 0.0f, size.x, 0.0f, 0.0f,
                                    1.0f, -1.0f, size.x, size.y, 0.0f,
                                    0.0f, -1.0f, 0.0f, size.y, 0.0f
                            };

                    glInterleavedArrays(GL_T2F_V3F, 0, quad);
                    glDrawArrays(GL_QUADS, 0, 4);
                    if (!GetKeyState(VK_LBUTTON) & 0x80) bClicked = false;
                    if (GetKeyState(VK_LBUTTON) & 0x80) {
                        bClicked = true;
                    }
                    if (!GetAsyncKeyState(VK_LBUTTON) & 0x80) bClicked = false;
                } else {
                    glBindTexture(GL_TEXTURE_2D, texID);
                    float quad[] =
                            {
                                    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                    1.0f, 0.0f, size.x, 0.0f, 0.0f,
                                    1.0f, -1.0f, size.x, size.y, 0.0f,
                                    0.0f, -1.0f, 0.0f, size.y, 0.0f
                            };

                    glInterleavedArrays(GL_T2F_V3F, 0, quad);
                    glDrawArrays(GL_QUADS, 0, 4);
                }
            }
            glPopMatrix();
        } else {
            glPushMatrix();
            {
                glTranslatef(position.x, position.y, 0);
                if ((mPos.x >= position.x && mPos.x <= position.x + size.x && mPos.y >= position.y &&
                     mPos.y <= position.y + size.y) && GetKeyState(VK_LBUTTON) & 0x80)
                    bClicked = false;
                glBindTexture(GL_TEXTURE_2D, CheckedTexID);
                float quad[] =
                        {
                                0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                1.0f, 0.0f, size.x, 0.0f, 0.0f,
                                1.0f, -1.0f, size.x, size.y, 0.0f,
                                0.0f, -1.0f, 0.0f, size.y, 0.0f
                        };

                glInterleavedArrays(GL_T2F_V3F, 0, quad);
                glDrawArrays(GL_QUADS, 0, 4);
            }
            glPopMatrix();
        }
        glDisable(GL_ALPHA_TEST);
        return bClicked;
    }

    void Checked(bool value) {
        bClicked = value;
    }

    vec2 size;
    vec2 position;

    bool bClicked;
    char description[1024];
    Font ft_font;

    GLuint CheckedTexID;
    GLuint OnHoverTexID;
    GLuint texID;

private:


};

class Gui {
public:
    Gui();

    ~Gui();

    bool Load(void);

    void Draw(void);

    void RegisterForm(void);

    void ShowLoginMenu(void);

    void ShowMainMenu(void);

    void Options(void);

    bool SaveOptions(void);

    bool DeleteServerFromList(int id);

    bool CreateServerItem(int i);

    bool RefreshServerItems();

    void ServerMenu(void);

    void QuickConnect(void);

    void AddServer(void);

    inline void UpdatePlayerNick(void) {
        if (!bUpdateNick) return;
        bUpdateNick = false;
        char *nick = myAccUserNick.GetText();

        if (!strcmp(nick, " ") || !nick || strlen(nick) < 5) {
            SetMessageEx("Bad nick! Only [A-z-0-9] is allowed!");
            myAccUserNick.SetText(cUserNick);
            return;
        }

        char *header = new char[(strlen(updatePlayerNickHeader) + strlen(updatePlayerNickHeader)) * 2];
        sprintf(header, updatePlayerNickHeader, base64_encode(nick, strlen(nick)), GetSESSIONID());

        char *ret = net.Send(header);
        int msgtype = GetMessageType(ret);

        if (msgtype == -1) {
            SetMessageEx("Can't update nick this time!");
            //pesheq, zadnej return kvuli uvolneni pameti
        } else {
            if (msgtype == 9 && nick) {
                SetMessageEx("Nick updated!");
                strcpy(cUserNick, nick);
                userAvatarName.setDescription(nick);
            }
        }

        if (header) delete[] header;
    }

    inline void SetPlayerID(int uid) {
        playerID = uid;
        return;
    }

    inline bool IsPlayerDead(void) {
        //printf("netw.playerPositions[GetPlayerID()].dead = %d\n", (int)netw.playerPositions[GetPlayerID()].dead);
        return netw.playerPositions[GetPlayerID()].dead;
    }

    inline int GetPlayerID(void) {
        return playerID;
    }

    inline void SetMaxPlayersOnServer(int maxPlayers) {
        maxPlayersOnServer = maxPlayers;
        return;
    }

    inline int GetMaxPlayersOnServer(void) {
        return maxPlayersOnServer;
    }

    inline char *GetPlayerImprint(void) {
        return playerImprint;
    }

    inline void SetPlayerImprint(char *imp) {
        strcpy(playerImprint, imp);
    }

    int playerID;
    int maxPlayersOnServer;
    char playerImprint[16];

    inline char *LoadLogins(void) {
        FILE *pFile = fopen("gui/E3Wds.usc", "rb");
        if (!pFile) return NULL;

        fseek(pFile, 0, SEEK_END);
        int fileSize = ftell(pFile) / 4;
        rewind(pFile);

        int *logins = new int[fileSize];
        char *charLogins = new char[fileSize];

        if (!logins || !charLogins) return NULL;

        fread(logins, fileSize, sizeof(int), pFile);

        fclose(pFile);

        for (int i = 0; i < fileSize; i++) charLogins[i] = logins[i] / 10;
        charLogins[fileSize] = '\0';

        return charLogins;
    }

    inline bool SaveLogins(void) {
        if (bSaveLogins) {
            FILE *pFile = fopen("gui/E3Wds.usc", "wb");
            if (!pFile) return false;
            char logins[1024];
            sprintf(logins, "%s|%s", mailBox.GetText(), passBox.GetText());
            int loginsize = strlen(logins);
            int *intLogins = new int[loginsize];
            for (int i = 0; i < loginsize; i++) intLogins[i] = (int) logins[i] * 10;
            fwrite(intLogins, sizeof(int), loginsize, pFile);
            fclose(pFile);
            bSaveLogins = false;
        }
        return true;
    }

    inline void SetDefaultMessage(void) {
        showMessage = true;
        strcpy(cUserMessage, "Unidentified error!");
    }

    inline int GetMessageType(char *message) {
        for (int i = 0; i < strlen(message); i++) {
            if (message[i] == '|') {
                char *ptr = strstr(message, "|");
                if (ptr) {
                    return atoi(ptr + 1);
                }
                break;
            }
        }
        return -1;
    }

    inline void SetMessageEx(char *message) {
        if (!message) return;
        showMessage = true;
        strcpy(cUserMessage, message);
    }

    // prepsat, nap!cuu ;)
    inline int SetMessage(char *message) {
        if (!message) {
            strcpy(cUserMessage, "(NULL)");
            return -1;
        }

        if (!strcmp(message, " ")) return -1;

        char msg[1024];
        int msgnum;
        if (strstr(message, "|")) {
            for (int i = 0; i < strlen(message); i++) {
                if (message[i] == '|') {
                    msg[i] = '\0';
                    msgnum = (int) (message[i + 1] - 48);
                    break;
                }
                msg[i] = message[i];
            }
            msg[strlen(message)] = '\0';
        }

        strcpy(cUserMessage, msg);
        showMessage = true;
        return msgnum;
    }

    inline void SetMessageByType(int type) {
        char *internalError = "Internal error! We apologize to you .. :(";
        switch (type) {
            case 3: {
                SetMessageEx("Invalid mail!");
            }
                break;

            case 4: {
                //zalogovano, user je v menu
                SetMessageEx(" ");
            }
                break;

            case 5: {
                SetMessageEx("Bad mail or password!");
            }
                break;

            case 7: // spadena db
            case 8: // invalid inputs
            {
                SetMessageEx(internalError);
            }
                break;

            default: {
                SetMessageEx("Unidentified error!");
            }
                break;
        }
    }

    void LoginUser();

    inline void LogoutUser() {
        char head[1024];
        printf("%s\n", GetSESSIONID());
        sprintf(head, logoutHeader, GetSESSIONID());
        char *ret = net.Send(head, false);
        printf("%s\n", ret);
        if (GetMessageType(ret) == 6) {
            bShowLoginMenu = true;
            bDownloadedAvatar = false;
        } else {
            printf("error: cannot unlog user!\n");
        }
        bLogoutUser = false;
    }

    void RegisterUser();

    inline void SetSESSIONID(char *in) {
        if (cSessionID) strcpy(cSessionID, in);
    }

    inline char *GetSESSIONID(void) {
        return cSessionID;
    }


    void UploadAvatar(void);

    bool GetUserInfo(char *info);

    inline bool GetAvatarPath(void) {
        bBrowseAvatar = false;
        OPENFILENAME ofn;
        char szFile[1024];
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "JPEG images *.jpg\0*.jpg\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FILEMUSTEXIST;
        int j = 0;
        if (GetOpenFileName(&ofn) == TRUE) {
            //strcpy(shortAvatarPath, "");
            //for(int i=0; i < strlen(szFile); i++)
            //{
            //    if(i > strlen(szFile)/2)
            //    {
            //        shortAvatarPath[j] = szFile[i];
            //        j++;
            //    }
            //}
            avatarPath = (char *) szFile;
            myAccUserAvatarPath.SetText(avatarPath);
            return true;
        } else {
            return false;
        }
    }

    inline void MessageBox(char *msg) {
        if (!bHandleMsgCalled) {
            handleMessage.SetMessage(msg);
            bHandleMsgCalled = true;
        }
    }

    char cUserID[256];
    char cUserName[256];
    char cUserMail[256];
    char cUserNick[256];
    char cUserAvatarPath[1024];
    int iUserAccType;
    char cSessionID[256];

    char cUserMessage[1024];
    bool showMessage;
    bool bLoginUser;
    bool bLogoutUser;
    bool bExitWindow;
    bool bUpdateNick;

    bool bShowExitMSGBox;
    bool bShowLoginMenu;
    bool bSaveLogins;

    bool bLoadLevel;
    bool bLevelLoaded;
    bool bShowQuickConnect;

    bool bDownloadedAvatar;

    bool bShowGUI;
    bool bShowMenuWithServers;
    bool bConnect;
    bool bAddServer;
    bool bShowAddServer;

    bool bChangedRes;
    int iCurrResolution;
    int bUseShadows;
    int iCurrShadowType;
    bool bUseBumpMapping;
    bool bUseVsync;
    bool bUseDOF;
    bool bUseAO;
    bool bFullscreen;
    bool bChangedFullscreen;

    bool bClickedDelete;
    bool bClicked[7];
    bool bShowOptions;
    bool bSaveOptions;
    bool bSendRegistration;
    bool bRefreshServerList;
    bool bRefreshServerListClicked;
    bool bUploadAvatar;
    bool bShowUploadBox;

    Resolutions *resolutions;

    //Login menu
    Image2 inputFormsBG;
    Image mainMenuBG;
    InputBox mailBox;
    InputBox passBox;
    ButtonTextured3 sendButton;
    ButtonTextured3 registerButton;
    ButtonTextured3 exitButton;
    ButtonTextured3 manageAccButton;
    CheckBox autologinCheckBox;
    Font inputBoxFont;

    //Register menu
    Image2 registerMenuBG;
    ButtonTextured3 closeRegisterMenuButton;
    Text registerMeText;
    Line registerMeLine;
    InputBox inputNick;
    InputBox inputName;
    InputBox inputPass;
    InputBox inputMail;
    CheckBox agreeCheckBox;
    ButtonTextured3 sendRegistrationButton;
    bool bRegisterMe;
    bool bAgreeBoxChecked;


    //Main menu
    ButtonFonted2 findServersButton;
    ButtonFonted2 myAccountButton;
    ButtonFonted2 optionsButton;
    ButtonFonted2 extrasButton;
    ButtonFonted2 mainMenuExitButton;
    ButtonTextured3 closeServersButton;
    Image2 gameNameImg;

    //Options
    ButtonFontedWithDescription resolutionButton;
    ButtonFontedWithDescription fullscreenButton;
    ButtonFontedWithDescription shadowsButton;
    ButtonFontedWithDescription bumpmappingButton;
    ButtonFontedWithDescription vsyncButton;
    ButtonFontedWithDescription dofButton;
    ButtonFontedWithDescription bloomButton;
    ButtonFonted2 backFromOptionsButton;
    ButtonFonted2 saveOptionsButton;
    Message saveSettingsBox;
    Image2 optionsBG;

    //Server menu
    Line topLine;
    Line bottomLine;
    Image2 serversBG;
    ButtonFontedWithDescription localhostButton;
    ButtonFonted2 server;
    ButtonFonted2 mod;
    ButtonFonted2 players;
    ButtonFonted2 ping;
    ButtonFonted2 addServer;
    ButtonFonted2 refreshServers;
    ButtonFonted2 quickConnect;
    InputBox serverIpInputBox;
    Image2 quickConnectBG;
    Text quickConText;
    ButtonFonted2 connectToServer;
    ButtonTextured3 closeQuickConnectButton;
    Text addServerText;
    ButtonFonted2 addServerToList;
    Font serverItemFont;

    Server serverItem[30];
    ButtonTextured3 serverItemDeleteButton[30];
    ButtonFonted2 serverItemButton[30];
    Text serverPing[30];
    Text serverNumPlayers[30];
    Text serverMode[30];
    int iNumServers;

    //myAcc menu
    void MyAccount(void);

    bool bShowMyAcc;
    Text myAccText;
    Image2 myAccBG;
    Line myAccTopLine;
    Line myAccBottomLine;
    ButtonTextured3 closeMyAccButton;
    Image myAccUserAvatar;
    InputBox myAccUserNick;
    InputBox myAccUserAvatarPath;
    ButtonFonted2 myAccUploadAvatar;
    ButtonFonted2 myAccEntry0;
    ButtonFonted2 myAccEntry1;
    ButtonFonted2 myAccEntry2;
    ButtonFonted2 backFromMyAcc;
    ButtonFonted2 saveMyAcc;
    ButtonFonted2 myAccBrowseAvatar;
    char *avatarPath;
    char shortAvatarPath[255];
    bool bBrowseAvatar;
    bool bAvatarClicked;
    Text myAccStats;
    Line myAccStatsLine;
    Text myAccHeadshots;
    Text myAccKills;
    Text myAccDeaths;
    Text myAccAssists;
    Text myAccWins;
    Text myAccLoses;
    LineVertical myAccVertLine;
    Text myAccPlayedTime;
    Text myAccAccuracy;
    Text myAccRank;


    //other
    ButtonFonted2 userAvatarName;
    Image userAvatar;
    Image2 loadingBG;
    color4f globalButtonColor;

    //user msg handles
    Message3 handleMessage;
    bool bHandleMsgCalled;

    ButtonFonted2 logoutButton;
    Message exitMessageBox;
    Progressbar mainProgressBar;

    BackgroundImage bgImage;

    int loginSocket;
    vec2 minWindow;
private:
};


#endif
