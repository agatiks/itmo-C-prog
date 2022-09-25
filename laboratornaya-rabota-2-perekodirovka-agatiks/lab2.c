#include<stdio.h>
#include<string.h>
#include<stdint.h>

int inputEncode(FILE *in);

uint32_t getUnicode(FILE *in, int encode);

void getStart(FILE *in, int encoding);

int toInt(char *string);

void setBOM(FILE *out, int encoding);

void putUnicode(FILE *out, uint32_t ch, int encoding);

void unicodeToUTF8(FILE *out, uint32_t ch);

void unicodeToUTF16(FILE *out, uint32_t ch, int endian);

void endianOut(int ch, int endian, FILE *out);

int endianIn(int ch, int endian, FILE *in);

uint32_t utf32toUnicode(FILE *in, int ch, int endian);

uint32_t utf8toUnicode(FILE *in, int ch);

uint32_t utf16toUnicode(FILE *in, int ch, int endian);

void unicodeToUTF32(FILE *out, uint32_t ch, int endian);

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Wrong number of arguments.");
        return 1;
    }
    FILE *in = fopen(argv[1], "r");
    if (in == NULL) {
        printf("Can't find or open file named \"%s\" for reading.", argv[1]);
        return 1;
    }
    int startEncoding = inputEncode(in);
    int endEncoding = toInt(argv[3]);
    if (endEncoding == -1) {
        printf("Wrong type of output encoding");
        fclose(in);
        return 1;
    }
    getStart(in, startEncoding);
    FILE *out = fopen(argv[2], "w");
    if (out == NULL) {
        printf("Can't find or open file named \"%s\" for writing.", argv[2]);
        fclose(in);
        return 1;
    }
    uint32_t ch = getUnicode(in, startEncoding);
    setBOM(out, endEncoding);
    while(ch != EOF) {
        putUnicode(out, ch, endEncoding);
        ch = getUnicode(in, startEncoding);
    }
    fclose(in);
    fclose(out);
    return 0;
}

void setBOM(FILE *out, int encoding)
{
    switch (encoding) {
    case(0): {
    }
    case(1): {
        fputc(0xef, out);
        fputc(0xbb, out);
        fputc(0xbf, out);
        break;
    }
    case(2): {
        fputc(0xff, out);
        fputc(0xfe, out);
        break;
    }
    case(3): {
        fputc(0xfe, out);
        fputc(0xff, out);
        break;
    }
    case(4): {
        fputc(0xff, out);
        fputc(0xfe, out);
        fputc(0x00, out);
        fputc(0x00, out);
        break;
    }
    case(5): {
        fputc(0x00, out);
        fputc(0x00, out);
        fputc(0xfe, out);
        fputc(0xff, out);
        break;
    }
    default:
        break;
    }
}

void putUnicode(FILE *out, uint32_t ch, int encoding)
{
    switch (encoding) {
    case(0): {
        unicodeToUTF8(out, ch);
        break;
    }
    case(1): {
        unicodeToUTF8(out, ch);
        break;
    }
    case(2): {
        unicodeToUTF16(out, ch, 0);
        break;
    }
    case(3): {
        unicodeToUTF16(out, ch, 1);
        break;
    }
    case(4): {
        unicodeToUTF32(out, ch, 0);
        break;
    }
    case(5): {
        unicodeToUTF32(out, ch, 1);
        break;
    }
    default: {
        break;
    }
    }
}

void unicodeToUTF32(FILE *out, uint32_t ch, int endian)
{
    if (endian) {
        endianOut(ch > 0xffff ? ch >> 16 : 0, endian, out);
        endianOut(ch & 0xffff, endian, out);
    } else {
        endianOut(ch & 0xffff, endian, out);
        endianOut(ch > 0xffff ? ch >> 16 : 0, endian, out);
    }
}

void unicodeToUTF16(FILE *out, uint32_t ch, int endian)
{
    if (ch < 0x10000) {
        endianOut(ch, endian, out);
    } else {
        ch -= 0x10000;
        endianOut(0xd800 + (int)(ch >> 10), endian, out);
        endianOut(0xdc00 + (int)(ch & 0x3ff), endian, out);
    }
}

void endianOut(int ch, int endian, FILE *out)
{
    int start = ch > 0xff ? ch >> 8 : 0;
    int end = ch & 0xff;
    if (endian) {
        fputc(start, out);
        fputc(end, out);
    } else {
        fputc(end, out);
        fputc(start, out);
    }
}

void unicodeToUTF8(FILE *out, uint32_t ch)
{
    if (ch <= 0x7f) {
        fputc(ch, out);
    } else if (ch <= 0x7ff) {
        fputc(0xc0 + (int)(ch >> 6), out);
        fputc(0x80 + (int)(ch & 0x3f), out);
    } else if (ch <= 0xffff) {
        fputc(0xe0 + (int)(ch >> 12), out);
        fputc(0x80 + (int)((ch >> 6) & 0x3f), out);
        fputc(0x80 + (int)(ch & 0x3f), out);
    } else if (ch <= 0x1fffff) {
        fputc(0xe0 + (int)(ch >> 18), out);
        fputc(0x80 + (int)((ch >> 12) & 0x3f), out);
        fputc(0x80 + (int)((ch >> 6) & 0x3f), out);
        fputc(0x80 + (int)(ch & 0x3f), out);
    } else {
        printf("Unicode don't use symbol with code %i and other codes >0x1fffff\n", ch);
        fputc(0, out);
    }
}

int toInt(char *str)
{
    int res = 0;
    if(strlen(str) == 1) {
        res = (int)str[0] - 48;
    } else {
        return -1;
    }
    if (res >= 0 && res <= 5) {
        return res;
    } else {
        return -1;
    }
}

void getStart(FILE *in, int encoding)
{
    int pos = 0;
    if (encoding == 1) {
        pos = 3;
    } else if (encoding == 2 || encoding == 3) {
        pos = 2;
    } else if (encoding == 4 || encoding == 5) {
        pos = 4;
    } else {
        pos = 0;
    }
    fseek(in, pos, SEEK_SET);
}

int inputEncode(FILE *in)
{
    int temp = fgetc(in);
    if(temp == 0xff) {
        temp = fgetc(in);
        if(temp == 0xfe) {
            temp = fgetc(in);
            if(temp == 0x00) {
                temp = fgetc(in);
                if(temp == 0x00) {
                    rewind(in);
                    return 4;
                }
            }
            else {
                rewind(in);
                return 2;
            }
        }
        else {
            rewind(in);
            return 0;
        }
    }
    else if (temp == 0xef) {
        temp = fgetc(in);
        if(temp == 0xbb) {
            temp = fgetc(in);
            if(temp == 0xbf) {
                rewind(in);
                return 1;
            }
        }
    }
    else if (temp == 0xfe) {
        temp = fgetc(in);
        if (temp == 0xff) {
            rewind(in);
            return 3;
        }
    }
    else if (temp == 0x00) {
        temp = fgetc(in);
        if (temp == 0x00) {
            temp = fgetc(in);
            if (temp == 0xfe) {
                temp = fgetc(in);
                if (temp == 0xff) {
                    rewind(in);
                    return 5;
                }
            }
        }
    }
    rewind(in);
    return 0;
}

uint32_t getUnicode(FILE *in, int encode)
{
    int ch = fgetc(in);
    if (ch == EOF) {
        return ch;
    }
    switch (encode) {
    case (0):
        return utf8toUnicode(in, ch);
    case (1):
        return utf8toUnicode(in, ch);
    case (2):
        return utf16toUnicode(in, ch, 0);
    case (3):
        return utf16toUnicode(in, ch, 1);
    case (4):
        return utf32toUnicode(in, ch, 0);
    case (5):
        return utf32toUnicode(in, ch, 1);
    default:
        break;
    }
}

uint32_t utf32toUnicode(FILE *in, int ch, int endian)
{
    uint32_t res = endianIn(ch, endian, in);
    ch = getc(in);
    if (endian) {
        res = (res << 16) + endianIn(ch, endian, in);
    } else {
        res = (endianIn(ch, endian, in) << 16) + res;
    }
    return res;
}

uint32_t utf16toUnicode(FILE *in, int ch, int endian)
{
    ch = endianIn(ch, endian, in);
    if (ch < 0xd800 || ch > 0xdfff) {
        return ch;
    } else {
        uint32_t res = (ch - 0xd800) * 0x0400;
        ch = fgetc(in);
        ch = endianIn(ch, endian, in);
        ch -= 0xdc00;
        res = res + ch + 0x10000;
        return res;
    }
}

int endianIn(int ch, int endian, FILE *in)
{
    if (endian) {
        return (ch << 8) + fgetc(in);
    } else {
        return (fgetc(in) << 8) + ch;
    }
}

uint32_t utf8toUnicode(FILE *in, int ch)
{
    if (ch <= 0x7f) {
        return ch;
    }
    if ((ch & 0xc0) == 0xc0) { // значит, что битовая запись символа 11......
        int len = 1;
        ch = fgetc(in);
        while((ch & 0xc0) == 0x80 && ch != EOF) {
            ch = fgetc(in);
            len++;
        }
        fseek(in, -len-1+(ch == EOF), SEEK_CUR);
        ch = fgetc(in);
        int mask = 0xff >> (len+1); // маска для получения значения
        int checkMask = 0xff - (mask + (1 << (7 - len)));
        if ((ch & (0xff - mask)) == checkMask) {
            uint32_t res = ch & 0x1f;
            for (int i = 0; i < len - 1; i++) {
                ch = fgetc(in);
                res = (res << 6) + (ch & 0x3f);
            }
            return res;
        } else {
            printf("UTF-8 encoding error, file can be incorrect\n");
            fseek(in, len-1, SEEK_CUR);
            return 0;
            // произошла ошибка так как длина не совпадает с ожидаемой
            // но при такой обработке перешли на следуюий ожидаемо корректный кусок
            // пока не знаю, как декодировать в символы из диапазона 0xDC80..0xDCFF
            // звучит очень трудно
        }

    } else {
        // что-то странное конечно, наверное ошибка,
        // описание не с 0 и не с 11, значит неверное
        // решила что буду возвращать символы из диапазона 0xDC80..0xDCFF
        printf("Code %i became simbol /u+%04x\n", ch, 0xDC00 + ch);
        return 0xDC00 + ch;
    }
}