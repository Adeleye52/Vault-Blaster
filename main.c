#include<stdio.h>
#ifdef _WIN32
#include<conio.h>
#include<windows.h>
#else
int getch(void);
#endif

void noKey(void);
void havKey(void);
void batchDecrypt(void);
void about(void);

int main()
{
    int x=1,y;
    char choice;
#ifdef _WIN32
    SetConsoleTitle("Vault Blaster");
    system("COLOR 0A");
#endif
    while(x)
    {
        do
        {
            y=0;
            printf("Choose an option:\n\t1. I don\'t know the key (Single file).\n\t2. I know the key (Single file).\n\t3. Batch decrypt all .bin files in a folder.\n\n\t4. About the program.");
            choice=getch();
            if(choice=='1')
                noKey();
            else if(choice=='2')
                havKey();
            else if(choice=='3')
                batchDecrypt();
            else if(choice=='4')
                about();
            else
            {
                printf("\a\n\nInvalid Selection.\n\n");
                y=1;
            }
        }
        while(y);

        printf("\n\nDo you want to start over?\n\tY=Yes\n\tAnyKey=No");
        choice=getch();
        if(!(choice=='Y' || choice=='y'))
            x=0;
        else
            printf("\n\n");
    }
    printf("\n\n\t\tPress a key to EXIT.");
    getch();
    return 0;
}
