#include<stdio.h>
#include<string.h>
#ifdef _WIN32
#include<windows.h>
#include<conio.h>
#else
#include<unistd.h>
#include<termios.h>
void Sleep(int ms) { usleep(ms * 1000); }
int getch(void) {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

void readLine(char *buf, int maxLen)
{
    if (fgets(buf, maxLen, stdin) != NULL)
    {
        size_t len = strlen(buf);
        while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r'))
        {
            buf[--len] = '\0';
        }
    }
    else
    {
        buf[0] = '\0';
    }
}

int getInt(void)
{
    char arr[30];
    readLine(arr, sizeof(arr));
    int i, sum = 0, len = (int)strlen(arr);
    if (len == 0)
        return -1;

    for (i = 0; i < len; i++)
    {
        if (arr[i] > 47 && arr[i] < 58)
            sum = sum * 10 + arr[i] - 48;
        else
        {
            sum = -1;
            break;
        }
    }
    return sum;
}

unsigned char getKey(int x)
{
    int k;
    unsigned char key;
    do
    {
        printf("\nEnter %s key (0-255): ", x > 0 ? "a" : "the");
        k = getInt();
    }
    while (k < 0 || k > 255);
    key = (unsigned char)k;
    return key;
}

int havFile(char *fname)
{
    FILE *ftest = fopen(fname, "rb");
    if (ftest != NULL)
    {
        fclose(ftest);
        return 1;
    }
    return 0;
}

int fileWrite(char *inFileName, char *outFileName, unsigned char key)
{
    FILE *frd, *fwr;
    unsigned char buffer[65536];
    size_t bytesRead, i;
    int isFirstChunk = 1;

    frd = fopen(inFileName, "rb");
    if (frd == NULL)
    {
        printf("\nFile Reading ERROR: %s", inFileName);
        return 1;
    }

    fwr = fopen(outFileName, "wb");
    if (fwr == NULL)
    {
        printf("\nFile Writing ERROR: %s", outFileName);
        fclose(frd);
        return 1;
    }

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), frd)) > 0)
    {
        if (isFirstChunk)
        {
            size_t xorCount = bytesRead < 128 ? bytesRead : 128;
            for (i = 0; i < xorCount; i++)
            {
                buffer[i] ^= key;
            }
            isFirstChunk = 0;
        }

        if (fwrite(buffer, 1, bytesRead, fwr) != bytesRead)
        {
            printf("\nFile Writing ERROR!");
            fclose(fwr);
            fclose(frd);
            return 1;
        }
    }

    fclose(fwr);
    fclose(frd);
    return 0;
}

int matchHeader(const unsigned char dFA[12], char tExt[])
{
    if (dFA[0]==0x00 && dFA[1]==0x00 && dFA[2]==0x00 && (dFA[3]==0x18 || dFA[3]==0x14 || dFA[3]==0x1c) && dFA[4]==0x66 && dFA[5]==0x74 && dFA[6]==0x79 && dFA[7]==0x70 && dFA[8]==0x33 && dFA[9]==0x67 && dFA[10]==0x70 && dFA[11]==0x35)
    {
        strcpy(tExt, ".mp4");
        return 12;
    }
    else if (dFA[0]==0x00 && dFA[1]==0x00 && dFA[2]==0x00 && (dFA[3]==0x18 || dFA[3]==0x14 || dFA[3]==0x1c) && dFA[4]==0x66 && dFA[5]==0x74 && dFA[6]==0x79 && dFA[7]==0x70 && dFA[8]==0x4d && dFA[9]==0x53 && dFA[10]==0x4e && dFA[11]==0x56)
    {
        strcpy(tExt, ".mp4");
        return 12;
    }
    else if (dFA[0]==0x00 && dFA[1]==0x00 && dFA[2]==0x00 && (dFA[3]==0x18 || dFA[3]==0x14 || dFA[3]==0x1c) && dFA[4]==0x66 && dFA[5]==0x74 && dFA[6]==0x79 && dFA[7]==0x70 && dFA[8]==0x69 && dFA[9]==0x73 && dFA[10]==0x6f && dFA[11]==0x6d)
    {
        strcpy(tExt, ".mp4");
        return 12;
    }
    else if (dFA[0]==0x00 && dFA[1]==0x00 && dFA[2]==0x00 && dFA[3]==0x1c && dFA[4]==0x66 && dFA[5]==0x74 && dFA[6]==0x79 && dFA[7]==0x70 && dFA[8]==0x33 && dFA[9]==0x67 && dFA[10]==0x32 && dFA[11]==0x61)
    {
        strcpy(tExt, ".3g2");
        return 12;
    }
    else if (dFA[0]==0x00 && dFA[1]==0x00 && dFA[2]==0x00 && dFA[3]==0x14 && dFA[4]==0x66 && dFA[5]==0x74 && dFA[6]==0x79 && dFA[7]==0x70 && dFA[8]==0x71 && dFA[9]==0x74 && dFA[10]==0x20 && dFA[11]==0x20)
    {
        strcpy(tExt, ".mov");
        return 12;
    }
    else if (dFA[0]==0x00 && dFA[1]==0x00 && dFA[2]==0x00 && dFA[3]==0x20 && dFA[4]==0x66 && dFA[5]==0x74 && dFA[6]==0x79 && dFA[7]==0x70 && dFA[8]==0x4d && dFA[9]==0x34 && dFA[10]==0x56 && dFA[11]==0x20)
    {
        strcpy(tExt, ".m4v");
        return 12;
    }
    else if (dFA[0]==0x00 && dFA[1]==0x00 && dFA[2]==0x00 && dFA[3]==0x18 && dFA[4]==0x66 && dFA[5]==0x74 && dFA[6]==0x79 && dFA[7]==0x70 && dFA[8]==0x6d && dFA[9]==0x70 && dFA[10]==0x34 && dFA[11]==0x32)
    {
        strcpy(tExt, ".mp4");
        return 12;
    }
    else if (dFA[0]==0x00 && dFA[1]==0x00 && dFA[2]==0x00 && dFA[3]==0x14 && dFA[4]==0x66 && dFA[5]==0x74 && dFA[6]==0x79 && dFA[7]==0x70 && dFA[8]==0x33 && dFA[9]==0x67 && dFA[10]==0x70)
    {
        strcpy(tExt, ".3gp");
        return 11;
    }
    else if (dFA[0]==0x00 && dFA[1]==0x00 && dFA[2]==0x00 && dFA[3]==0x18 && dFA[4]==0x66 && dFA[5]==0x74 && dFA[6]==0x79 && dFA[7]==0x70)
    {
        strcpy(tExt, ".mp4");
        return 8;
    }
    else if (dFA[0]==0x66 && dFA[1]==0x74 && dFA[2]==0x79 && dFA[3]==0x70 && dFA[4]==0x71 && dFA[5]==0x74 && dFA[6]==0x20 && dFA[7]==0x20)
    {
        strcpy(tExt, ".mov");
        return 8;
    }
    else if (dFA[0]==0x66 && dFA[1]==0x74 && dFA[2]==0x79 && dFA[3]==0x70 && dFA[4]==0x6d && dFA[5]==0x70 && dFA[6]==0x34 && dFA[7]==0x32)
    {
        strcpy(tExt, ".m4v");
        return 8;
    }
    else if (dFA[0]==0x66 && dFA[1]==0x74 && dFA[2]==0x79 && dFA[3]==0x70 && dFA[4]==0x69 && dFA[5]==0x73 && dFA[6]==0x6f && dFA[7]==0x6d)
    {
        strcpy(tExt, ".mp4");
        return 8;
    }
    else if (dFA[0]==0x66 && dFA[1]==0x74 && dFA[2]==0x79 && dFA[3]==0x70 && dFA[4]==0x33 && dFA[5]==0x67 && dFA[6]==0x70 && dFA[7]==0x35)
    {
        strcpy(tExt, ".mp4");
        return 8;
    }
    else if (dFA[0]==0x66 && dFA[1]==0x74 && dFA[2]==0x79 && dFA[3]==0x70 && dFA[4]==0x4d && dFA[5]==0x53 && dFA[6]==0x4e && dFA[7]==0x56)
    {
        strcpy(tExt, ".mp4");
        return 8;
    }
    else if (dFA[0]==0x89 && dFA[1]==0x50 && dFA[2]==0x4e && dFA[3]==0x47 && dFA[4]==0x0d && dFA[5]==0x0a && dFA[6]==0x1a && dFA[7]==0x0a)
    {
        strcpy(tExt, ".png");
        return 8;
    }
    else if (dFA[0]==0x1a && dFA[1]==0x45 && dFA[2]==0xdf && dFA[3]==0xa3 && dFA[4]==0x93 && dFA[5]==0x42 && dFA[6]==0x82 && dFA[7]==0x88)
    {
        strcpy(tExt, ".mkv");
        return 8;
    }
    else if (dFA[0]==0x30 && dFA[1]==0x26 && dFA[2]==0xb2 && dFA[3]==0x75 && dFA[4]==0x8e && dFA[5]==0x66 && dFA[6]==0xcf && dFA[7]==0x11)
    {
        strcpy(tExt, ".wmv");
        return 8;
    }
    else if (dFA[0]==0x52 && dFA[1]==0x49 && dFA[2]==0x46 && dFA[3]==0x46 && dFA[8]==0x41 && dFA[9]==0x56 && dFA[10]==0x49 && dFA[11]==0x20)
    {
        strcpy(tExt, ".avi");
        return 8;
    }
    else if (dFA[0]==0x3c && dFA[1]==0x3f && dFA[2]==0x78 && dFA[3]==0x6d && dFA[4]==0x6c && dFA[5]==0x20)
    {
        strcpy(tExt, ".svg");
        return 6;
    }
    else if ((dFA[0]==0x47 && dFA[1]==0x49 && dFA[2]==0x46 && dFA[3]==0x38 && dFA[4]==0x39 && dFA[5]==0x61) || (dFA[0]==0x47 && dFA[1]==0x49 && dFA[2]==0x46 && dFA[3]==0x38 && dFA[4]==0x37 && dFA[5]==0x61))
    {
        strcpy(tExt, ".gif");
        return 6;
    }
    else if (dFA[0]==0x1a && dFA[1]==0x45 && dFA[2]==0xdf && dFA[3]==0xa3)
    {
        strcpy(tExt, ".webm");
        return 4;
    }
    else if (dFA[0]==0x52 && dFA[1]==0x49 && dFA[2]==0x46 && dFA[3]==0x46)
    {
        strcpy(tExt, ".webp");
        return 4;
    }
    else if (dFA[0]==0x00 && dFA[1]==0x00 && dFA[2]==0x01 && dFA[3]==0xba)
    {
        strcpy(tExt, ".vob");
        return 4;
    }
    else if (dFA[0]==0x6d && dFA[1]==0x6f && dFA[2]==0x6f && dFA[3]==0x76)
    {
        strcpy(tExt, ".mov");
        return 4;
    }
    else if (dFA[0]==0x66 && dFA[1]==0x72 && dFA[2]==0x65 && dFA[3]==0x65)
    {
        strcpy(tExt, ".mov");
        return 4;
    }
    else if (dFA[0]==0x6d && dFA[1]==0x64 && dFA[2]==0x61 && dFA[3]==0x74)
    {
        strcpy(tExt, ".mov");
        return 4;
    }
    else if (dFA[0]==0x77 && dFA[1]==0x69 && dFA[2]==0x64 && dFA[3]==0x65)
    {
        strcpy(tExt, ".mov");
        return 4;
    }
    else if (dFA[0]==0x70 && dFA[1]==0x6e && dFA[2]==0x6f && dFA[3]==0x74)
    {
        strcpy(tExt, ".mov");
        return 4;
    }
    else if (dFA[0]==0x73 && dFA[1]==0x6b && dFA[2]==0x69 && dFA[3]==0x70)
    {
        strcpy(tExt, ".mov");
        return 4;
    }
    else if (dFA[0]==0x00 && dFA[1]==0x00 && dFA[2]==0x01 && dFA[3]==0xb3)
    {
        strcpy(tExt, ".mpg");
        return 4;
    }
    else if (dFA[0]==0xff && dFA[1]==0xd8 && dFA[2]==0xff && (dFA[3]==0xe0 || dFA[3]==0xe1 || dFA[3]==0xe8 || dFA[3]==0xdb || dFA[3]==0xee))
    {
        strcpy(tExt, ".jpg");
        return 4;
    }
    else if (dFA[0]==0x46 && dFA[1]==0x4c && dFA[2]==0x56 && dFA[3]==0x01)
    {
        strcpy(tExt, ".flv");
        return 4;
    }
    else if (dFA[0]==0xff && dFA[1]==0xd8 && dFA[2]==0xff)
    {
        strcpy(tExt, ".jpg");
        return 3;
    }
    else if (dFA[0]==0x42 && dFA[1]==0x4d)
    {
        strcpy(tExt, ".bmp");
        return 2;
    }

    return 0;
}

int getExtWithKey(char fileName[], unsigned char key, char ext[])
{
    unsigned int j;
    FILE *frd;
    unsigned char fileArr[12], dFA[12];
    int lf;

    frd = fopen(fileName, "rb");
    if (frd == NULL)
        return -1;
    if (fread(fileArr, 1, 12, frd) < 12)
    {
        fclose(frd);
        return -1;
    }
    fclose(frd);

    for (j = 0; j < 12; j++)
    {
        dFA[j] = fileArr[j] ^ key;
    }

    lf = matchHeader(dFA, ext);
    if (lf > 0)
        return lf;

    strcpy(ext, "NFND");
    return -1;
}

int getExtWithConfidence(char fileName[], char ext[], int *outConfidence)
{
    int key = -1, bestLf = 0;
    unsigned int i, j;
    FILE *frd;
    char tExt[6] = "NFND";
    char curExt[6];
    unsigned char fileArr[12], dFA[12];

    frd = fopen(fileName, "rb");
    if (frd == NULL)
    {
        if (outConfidence) *outConfidence = 0;
        return -1;
    }
    if (fread(fileArr, 1, 12, frd) < 12)
    {
        fclose(frd);
        if (outConfidence) *outConfidence = 0;
        return -1;
    }
    fclose(frd);

    for (i = 0; i < 256; i++)
    {
        int curLf;
        for (j = 0; j < 12; j++)
        {
            dFA[j] = fileArr[j] ^ (unsigned char)i;
        }

        curLf = matchHeader(dFA, curExt);
        if (curLf > bestLf)
        {
            bestLf = curLf;
            key = (int)i;
            strcpy(tExt, curExt);
            if (bestLf >= 11)
                break;
        }
    }

    strcpy(ext, tExt);
    if (outConfidence)
        *outConfidence = bestLf;
    return key;
}

int getExt(char fileName[], char ext[])
{
    return getExtWithConfidence(fileName, ext, NULL);
}

void typEff(char str[])
{
    int i, len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        if (!(str[i] == ' ' || str[i] == '\n'))
            Sleep(150);
        printf("%c", str[i]);
    }
}
