#include<stdio.h>
#include<string.h>
#ifdef _WIN32
#include<conio.h>
#else
int getch(void);
#endif

void readLine(char *buf, int maxLen);
unsigned char getKey(int x);
int havFile(char *fname);
int fileWrite(char *inFileName, char *outFileName, unsigned char key);
int getExtWithKey(char fileName[], unsigned char key, char ext[]);

void havKey(void)
{
    char choice,fileName[100],deFileName[100],fext[10];
    unsigned char key;
    int exErr,err;

    printf("\nEnter file Name (with extension):");
    readLine(fileName, sizeof(fileName));
    while(!havFile(fileName))
    {
        printf("\n\aFile Not Found! Make sure you have entered correct file name with extension.\nPlease enter again: ");
        readLine(fileName, sizeof(fileName));
    }
    key=getKey(0);
    printf("\nEnter New File Name: ");
    readLine(deFileName, sizeof(deFileName));

    printf("\nWant to use automatic file type detection?\n\tY=Yes\n\tAnyKey=No");
    choice=getch();
    if(choice=='y' || choice=='Y')
    {
        exErr=getExtWithKey(fileName,key,fext);
        if(exErr<0)
        {
            printf("\nAutomatic file type detection failed!\nPlease Enter File extension:");
            readLine(fext, sizeof(fext));
            strcat(deFileName,".");
            strcat(deFileName,fext);
        }

        else
        {
            strcat(deFileName,fext);
            printf("\nFile Type Detected: %s",fext);
        }
    }

    else
    {
        printf("\nEnter file extension: ");
        readLine(fext, sizeof(fext));
        strcat(deFileName,".");
        strcat(deFileName,fext);
    }

    printf("\nMaking File...");
    err=fileWrite(fileName,deFileName,key);
    if(err)
        printf("\n\aOperation Failed.");
    else
        printf("\nOperation Complete!");
}
