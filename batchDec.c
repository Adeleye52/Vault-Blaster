#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#ifdef _WIN32
#include<windows.h>
#include<conio.h>
#define PATH_SEP '\\'
#else
#include<dirent.h>
#include<sys/stat.h>
#define PATH_SEP '/'
int getch(void);
#endif

void readLine(char *buf, int maxLen);
unsigned char getKey(int x);
int fileWrite(char *inFileName, char *outFileName, unsigned char key);
int getExtWithKey(char fileName[], unsigned char key, char ext[]);
int getExtWithConfidence(char fileName[], char ext[], int *outConfidence);

static int hasBinExtension(const char *name)
{
    size_t len = strlen(name);
    if (len < 5)
        return 0;
    const char *ext = name + len - 4;
    return (ext[0] == '.' &&
            tolower((unsigned char)ext[1]) == 'b' &&
            tolower((unsigned char)ext[2]) == 'i' &&
            tolower((unsigned char)ext[3]) == 'n');
}

static void cleanFolderPath(char *path)
{
    size_t len = strlen(path);
    if (len >= 2 && ((path[0] == '"' && path[len - 1] == '"') || (path[0] == '\'' && path[len - 1] == '\'')))
    {
        memmove(path, path + 1, len - 2);
        path[len - 2] = '\0';
        len -= 2;
    }
    while (len > 1 && (path[len - 1] == '\\' || path[len - 1] == '/'))
    {
        if (len == 3 && path[1] == ':')
            break;
        path[--len] = '\0';
    }
    if (len == 0)
    {
        strcpy(path, ".");
    }
}

static void makeDir(const char *dirPath)
{
#ifdef _WIN32
    CreateDirectoryA(dirPath, NULL);
#else
    mkdir(dirPath, 0755);
#endif
}

static void stripBinExt(const char *fileName, char *baseName, size_t maxLen)
{
    size_t len = strlen(fileName);
    size_t copyLen = (len >= 4 && hasBinExtension(fileName)) ? (len - 4) : len;
    if (copyLen >= maxLen)
        copyLen = maxLen - 1;
    memcpy(baseName, fileName, copyLen);
    baseName[copyLen] = '\0';
}

static int listBinFiles(const char *folderPath, char ***outFiles)
{
    int count = 0;
    int capacity = 64;
    char **files = (char **)malloc(sizeof(char *) * capacity);
    if (!files)
        return -1;

#ifdef _WIN32
    char searchPattern[1024];
    WIN32_FIND_DATAA findData;
    HANDLE hFind;

    snprintf(searchPattern, sizeof(searchPattern), "%s\\*", folderPath);
    hFind = FindFirstFileA(searchPattern, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        free(files);
        return -1;
    }

    do
    {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            if (hasBinExtension(findData.cFileName))
            {
                if (count >= capacity)
                {
                    capacity *= 2;
                    char **newFiles = (char **)realloc(files, sizeof(char *) * capacity);
                    if (!newFiles)
                        break;
                    files = newFiles;
                }
                files[count] = (char *)malloc(strlen(findData.cFileName) + 1);
                if (files[count])
                {
                    strcpy(files[count], findData.cFileName);
                    count++;
                }
            }
        }
    }
    while (FindNextFileA(hFind, &findData) != 0);

    FindClose(hFind);
#else
    DIR *dir = opendir(folderPath);
    struct dirent *entry;
    if (!dir)
    {
        free(files);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (hasBinExtension(entry->d_name))
        {
            if (count >= capacity)
            {
                capacity *= 2;
                char **newFiles = (char **)realloc(files, sizeof(char *) * capacity);
                if (!newFiles)
                    break;
                files = newFiles;
            }
            files[count] = (char *)malloc(strlen(entry->d_name) + 1);
            if (files[count])
            {
                strcpy(files[count], entry->d_name);
                count++;
            }
        }
    }

    closedir(dir);
#endif

    *outFiles = files;
    return count;
}

static void freeBinFiles(char **files, int count)
{
    int i;
    if (!files)
        return;
    for (i = 0; i < count; i++)
    {
        free(files[i]);
    }
    free(files);
}

void batchDecrypt(void)
{
    char folderPath[512];
    char outDir[600];
    char fallbackExt[16] = "";
    char **binFiles = NULL;
    int fileCount = 0;
    int i, succeeded = 0, failed = 0;
    unsigned char key = 0;
    int keyFound = 0;
    char choice;

    printf("\n\n=== Batch Decrypt All .bin Files in Folder ===");
    printf("\nEnter folder path (or press Enter for current folder): ");
    readLine(folderPath, sizeof(folderPath));
    cleanFolderPath(folderPath);

    fileCount = listBinFiles(folderPath, &binFiles);
    if (fileCount < 0)
    {
        printf("\n\aError: Could not open folder \"%s\"!", folderPath);
        return;
    }
    if (fileCount == 0)
    {
        printf("\n\aNo .bin files found in \"%s\"!", folderPath);
        freeBinFiles(binFiles, fileCount);
        return;
    }

    printf("\nFound %d .bin file(s) in \"%s\".\n", fileCount, folderPath);
    printf("\nHow should the key be determined?\n\t1. Auto-detect key from .bin files (Recommended)\n\t2. Enter the key manually (0-255)");
    choice = getch();
    while (!(choice == '1' || choice == '2'))
    {
        printf("\n\aPlease select 1 or 2:\n\t1. Auto-detect key\n\t2. Enter key manually");
        choice = getch();
    }

    if (choice == '1')
    {
        int bestKey = -1;
        int bestConf = 0;
        char bestExt[10] = "";
        int bestIdx = -1;

        printf("\n\nScanning .bin files to detect key...");
        for (i = 0; i < fileCount; i++)
        {
            char fullPath[1024];
            char detExt[10];
            int conf = 0;
            int k;

            snprintf(fullPath, sizeof(fullPath), "%s%c%s", folderPath, PATH_SEP, binFiles[i]);
            k = getExtWithConfidence(fullPath, detExt, &conf);
            if (k >= 0 && conf > bestConf)
            {
                bestConf = conf;
                bestKey = k;
                strcpy(bestExt, detExt);
                bestIdx = i;
                if (bestConf >= 4)
                    break;
            }
        }

        if (bestKey >= 0)
        {
            key = (unsigned char)bestKey;
            keyFound = 1;
            printf("\nAuto-detected KEY: %d (matched %s from \"%s\")\n", (int)key, bestExt, binFiles[bestIdx]);
        }
        else
        {
            printf("\n\aCould not auto-detect key from file headers. Please enter the key manually.");
        }
    }

    if (!keyFound)
    {
        key = getKey(0);
    }

    snprintf(outDir, sizeof(outDir), "%s%cdecrypted", folderPath, PATH_SEP);
    makeDir(outDir);

    printf("\nDecrypting %d file(s) with KEY %d into \"%s\"...\n", fileCount, (int)key, outDir);

    for (i = 0; i < fileCount; i++)
    {
        char inPath[1024];
        char outPath[1024];
        char baseName[512];
        char fext[16];
        int extConf;

        snprintf(inPath, sizeof(inPath), "%s%c%s", folderPath, PATH_SEP, binFiles[i]);
        stripBinExt(binFiles[i], baseName, sizeof(baseName));

        extConf = getExtWithKey(inPath, key, fext);
        if (extConf < 0)
        {
            if (fallbackExt[0] == '\0')
            {
                char userExt[16];
                printf("\nCould not auto-detect extension for \"%s\".", binFiles[i]);
                printf("\nEnter fallback extension for unrecognized files (Press Enter for .jpg): ");
                readLine(userExt, sizeof(userExt));
                if (userExt[0] == '\0')
                {
                    strcpy(fallbackExt, ".jpg");
                }
                else if (userExt[0] == '.')
                {
                    snprintf(fallbackExt, sizeof(fallbackExt), "%s", userExt);
                }
                else
                {
                    snprintf(fallbackExt, sizeof(fallbackExt), ".%s", userExt);
                }
            }
            strcpy(fext, fallbackExt);
        }

        snprintf(outPath, sizeof(outPath), "%s%c%s%s", outDir, PATH_SEP, baseName, fext);

        if (fileWrite(inPath, outPath, key) == 0)
        {
            succeeded++;
            printf("\n[%d/%d] %s -> decrypted%c%s%s [OK]", i + 1, fileCount, binFiles[i], PATH_SEP, baseName, fext);
        }
        else
        {
            failed++;
            printf("\n[%d/%d] %s -> FAILED", i + 1, fileCount, binFiles[i]);
        }
    }

    printf("\n\n========================================");
    printf("\nBatch Decryption Complete!");
    printf("\nKey used        : %d", (int)key);
    printf("\nTotal .bin files: %d", fileCount);
    printf("\nSucceeded       : %d", succeeded);
    printf("\nFailed          : %d", failed);
    printf("\nSaved to        : %s", outDir);
    printf("\n========================================\n");

    freeBinFiles(binFiles, fileCount);
}
