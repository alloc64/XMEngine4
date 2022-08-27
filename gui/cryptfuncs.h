/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _CRYPTFUNCS_H
#define _CRYPTFUNCS_H

inline char to_hex(char code) {
    static char hex[] = "0123456789abcdef";
    return hex[code & 15];
}

inline char from_hex(char ch) {
    return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

inline int GetFileSize(char *path) {
    FILE *pFile = fopen(path, "rb");
    if (!pFile) return -1;

    fseek(pFile, 0, SEEK_END);
    int fileSize = ftell(pFile);
    fclose(pFile);

    if (fileSize < 1) return -1;

    return fileSize;
}

static const unsigned char map[256] = {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 62, 255, 255, 255, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255,
        255, 254, 255, 255, 255, 0, 1, 2, 3, 4, 5, 6,
        7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
        19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255,
        255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
        37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
        49, 50, 51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255};

inline char *base64_decode(char *in, int len) {
    unsigned long t = 0, x = 0, y = 0, z = 0;
    unsigned char c = 0;
    unsigned char *out = new unsigned char[len * 3];
    int g = 0;
    g = 3;
    for (x = y = z = t = 0; x < len; x++) {
        c = map[in[x] & 0xFF];
        if (c == 255) continue;
        if (c == 254) {
            c = 0;
            g--;
        }
        t = (t << 6) | c;
        if (++y == 4) {
            out[z++] = (unsigned char) ((t >> 16) & 255);
            if (g > 1) out[z++] = (unsigned char) ((t >> 8) & 255);
            if (g > 2) out[z++] = (unsigned char) (t & 255);
            y = t = 0;
        }
    }
    if (y != 0) return NULL;

    return (char *) out;
}


inline char *base64_encode(char *content, int length) {
    static const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.";
    int prepare;
    int ret_len;
    int temp = 0;
    char *f = NULL;
    int tmp = 0;
    char changed[4];
    int i = 0;
    ret_len = length / 3;
    temp = length % 3;
    if (temp > 0) ret_len += 1;
    char *ret = new char[ret_len * 4 + 1];
    if (!ret) return NULL;

    f = ret;
    while (tmp < length) {
        temp = 0;
        prepare = 0;
        memset(changed, '\0', 4);
        while (temp < 3) {
            if (tmp >= length) break;
            prepare = ((prepare << 8) | (content[tmp] & 0xFF));
            tmp++;
            temp++;
        }
        prepare = (prepare << ((3 - temp) * 8));
        for (i = 0; i < 4; i++) {
            if (temp < i) changed[i] = 0x40; else changed[i] = (prepare >> ((3 - i) * 6)) & 0x3F;
            *f = base[changed[i]];
            f++;
        }
    }
    *f = '\0';

    return ret;
}

inline char *urlencode(char *str) {
    char *pstr = str, *buf = new char[strlen(str) * 3 + 1], *pbuf = buf;
    while (*pstr) {
        if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~')
            *pbuf++ = *pstr;
        else if (*pstr == ' ')
            *pbuf++ = '+';
        else
            *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
        pstr++;
    }

    *pbuf = '\0';

    return buf;
}


inline char *urldecode(char *str) {
    char *pstr = str, *buf = new char[strlen(str) + 1], *pbuf = buf;
    while (*pstr) {
        if (*pstr == '%') {
            if (pstr[1] && pstr[2]) {
                *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
                pstr += 2;
            }
        } else if (*pstr == '+') {
            *pbuf++ = ' ';
        } else {
            *pbuf++ = *pstr;
        }
        pstr++;

    }
    *pbuf = '\0';
    return buf;
}


inline void decodeblock(unsigned char in[4], unsigned char out[3]) {
    out[0] = (unsigned char) (in[0] << 2 | in[1] >> 4);
    out[1] = (unsigned char) (in[1] << 4 | in[2] >> 2);
    out[2] = (unsigned char) (((in[2] << 6) & 0xc0) | in[3]);
}

inline char *base64_decode(char *str) {
    unsigned char in[4], out[3], v;
    unsigned int i, len, pos = 0;
    char *tmp = str;
    char *result = str; //(char *)malloc(strlen(str) + 1);
    static const char cd64[] = "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

    if (!result) return NULL;

    while (*tmp) {
        for (len = 0, i = 0; i < 4 && *tmp; i++) {
            v = 0;

            while (*tmp && v == 0) {
                v = (unsigned char) (*tmp);
                v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[v - 43]);

                if (v) v = (unsigned char) ((v == '$') ? 0 : v - 61);

                tmp++;
            }

            if (*tmp) {
                len++;
                if (v) in[i] = (unsigned char) (v - 1);
            } else
                in[i] = 0;
        }

        if (len) {
            decodeblock(in, out);
            for (i = 0; i < len - 1; i++) result[pos++] = out[i];
        }
    }

    result[pos] = '\0';

    return result;
}

inline bool isValidEmail(const char *address) {
    int count = 0;
    const char *c, *domain;
    static char *rfc822_specials = "()<>@,;:\\\"[]";
    for (c = address; *c; c++) {
        if (*c == '\"' && (c == address || *(c - 1) == '.' || *(c - 1) == '\"')) {
            while (*++c) {
                if (*c == '\"') break;
                if (*c == '\\' && (*++c == ' ')) continue;
                if (*c < ' ' || *c >= 127) return false;
            }
            if (!*c++) return false;
            if (*c == '@') break;
            if (*c != '.') return false;
            continue;
        }
        if (*c == '@') break;
        if (*c <= ' ' || *c >= 127) return false;
        if (strchr(rfc822_specials, *c)) return false;
    }
    if (c == address || *(c - 1) == '.') return false;

    if (!*(domain = ++c)) return false;
    do {
        if (*c == '.') {
            if (c == domain || *(c - 1) == '.') return false;
            count++;
        }
        if (*c <= ' ' || *c >= 127) return false;
        if (strchr(rfc822_specials, *c)) return false;
    } while (*++c);

    return (count >= 1);
}


inline bool IsValidNick(const char *nick) {
    int len = strlen(nick);
    int count = 0;
    static const int base_length = 62;
    static const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    for (int i = 0; i < len; i++) {
        for (int j = 0; j < base_length; j++) {
            if (nick[i] == base[j]) count++;
        }
    }

    return (count == len);
}

inline char *trim(char *s) {
    char *ptr;
    if (!s) return NULL;
    if (!*s) return s;
    for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);
    ptr[1] = '\0';
    return s;
}

inline bool IsValidName(char *name) {
    int nameLength = strlen(name);
    int count = 0;

    for (int i = 0; i < nameLength; i++) {
        if (i + 1 <= nameLength) {
            if (name[i] == ' ' && name[i + 1]) {
                count++;
            }
        }
    }

    return (count == 1);
}

#endif
