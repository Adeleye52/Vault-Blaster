#include<stdio.h>

void readLine(char *buf, int maxLen);
int fileWrite(char *inFileName, char *outFileName, unsigned char key);

void manuExt(char *enFileName)
{
    char fext[10],deFileName[100];
    int i,err;

    printf("\nEnter Decrypted File Extension: ");
    readLine(fext, sizeof(fext));

    printf("\n\nMaking file.....");
    for(i=0; i<256; i++)
    {
        sprintf(deFileName,"KEY-%d.%s",i,fext);
        err=fileWrite(enFileName,deFileName,(unsigned char)i);
        if(err)
        {
            printf("\a\nOperation Failed.");
            break;
        }
        else
            printf("\n%d of 256 Completed.",i+1);
    }
    if(i>255)
        printf("\nOperation Complete!");
}
