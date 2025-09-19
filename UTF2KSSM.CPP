
#include <stdio.h>

static const unsigned short _cho_ridx[] = {
    2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20
};
static const unsigned short _jung_ridx[] = {
    3, 4, 5, 6, 7, 10, 11, 12, 13, 14, 15, 18, 19, 20, 21, 22, 23, 26, 27, 28, 29
};
static const unsigned short _jong_ridx[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29
};

static const unsigned char flag1 = 0xE0; // 11100000
static const unsigned char flag2 = 0x80; // 10000000
static const unsigned char cho_  = 0x0F; // 00001111
static const unsigned char jung_ = 0x3F; // 00111111
static const unsigned char kssm_ = 0x1F; // 00011111

int main(int argc, char *argv[]) {
static const unsigned char* progname = "utf2kssm";

    if (argc < 3) {
        printf("UTF-8 to KSSM converter by @gcjjyy\n");
        printf("Usage: %s [infile] [output]\n", progname);

        return 0;
    }


    FILE *in  = fopen(argv[1], "r");
    FILE *out = fopen(argv[2], "w");

    while (!feof(in)) {
        char ch;
        if (fread(&ch, 1, 1, in) == 0) {
            break;
        }
        int pos = ftell(in);

        if ((ch & flag1)== flag1) {
            char ch2;
            fread(&ch2, 1, 1, in);

            if ((ch2 & flag2) == flag2) {
                char ch3;
                fread(&ch3, 1, 1, in);

                unsigned short code = ((ch  & cho_) << 12) |
                                      ((ch2 & jung_) << 6) |
                                      (ch3  & jung_);

                if (code >= 0xac00 && code <= 0xd7a3) {
                    code -= 0xac00;

                    unsigned short jong = code % 28;
                    code /= 28;
                    unsigned short jung = code % 21;
                    unsigned short cho = code / 21;

                    jong = _jong_ridx[jong];
                    jung = _jung_ridx[jung];
                    cho  = _cho_ridx[cho];


                    unsigned short kssm = (1 << 15) |
                                          ((cho & kssm_) << 10) |
                                          ((jung & kssm_) << 5) |
                                          (jong & kssm_);
                    unsigned char b1 = (kssm >> 8) & 0xff;
                    unsigned char b2 = kssm & 0xff;

                    fwrite(&b1, 1, 1, out);
                    fwrite(&b2, 1, 1, out);

                    continue;
                }
            }
        }

        fseek(in, pos, SEEK_SET);
        fwrite(&ch, 1, 1, out);
    }

    fclose(in);
    fclose(out);

    printf("done.\n");
    return 0;
}
