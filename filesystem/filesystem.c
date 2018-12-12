#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>

#define PATH_SIZE 100 

static char commstr[20] = "/";

void Readline(void);
static int ListFiles(char *drive);
static int changeDir(char *path);
static int removeFile(char *name);
static int renameFile(char *old_name, char *new_name);
static int makeDir(char *name);
static int readFile(char *name);
static int writeFile(char *file1, char *file2);

static int writeFile(char *file1, char *file2)
{
    //implement file content copy to a different file
    char *buffer;
    long Size;
    size_t retread, retwrite;
    FILE * fp_read;
    FILE * fp_write;
    
    fp_read = fopen(file1, "rb");
    if(fp_read == NULL)
    {
        printf("unable to open the specified file\n");
        return -1;
    }

    fseek(fp_read, 0, SEEK_END);
    Size = ftell(fp_read);
    rewind(fp_read);

    buffer = (char *)malloc(sizeof(char)* Size);
    if(buffer == NULL)
    {
        printf("Memory error\n");
        return -1;
    }

    retread = fread(buffer, 1, Size, fp_read);
    if(retread != Size)
    {
        printf("Reading error\n");
        return -1;
    }
    
    fclose(fp_read);
   
    fp_write = fopen(file2, "wb");
    if(fp_write == NULL)
    {
        printf("unable to open the specified file\n");
        return -1;
    }
    
    retwrite = fwrite(buffer, 1, Size, fp_write);
    if(retwrite != Size)
    {
        printf("Reading error\n");
        return -1;
    }
    
    fclose(fp_write);
    free(buffer);

    return 0;
}

static int readFile(char *name)
{
    char *buffer;
    long Size;
    size_t ret;
    FILE * fp;

    fp = fopen(name, "rb");
    if(fp == NULL)
    {
        printf("unable to open the specified file\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    Size = ftell(fp);
    rewind(fp);

    buffer = (char *)malloc(sizeof(char)* Size);
    if(buffer == NULL)
    {
        printf("Memory error\n");
        return -1;
    }

    ret = fread(buffer, 1, Size, fp);
    if(ret != Size)
    {
        printf("Reading error\n");
        return -1;
    }

    printf("%s",buffer);
    fclose(fp);
    free(buffer);
    return 0;
}

static int renameFile(char *old_name, char *new_name)
{
    int ret;

    ret = rename(old_name, new_name);
    if(ret!=0)
    {
        printf("Error renaming the file to %s\n", new_name);
        return -1;
    }
    return 0;
}

static int makeDir(char *path)
{
    int ret;

    ret = mkdir(path, 0777);
    if(ret!=0)
    {
        printf("Error creating a new directory %s\n", path);
        return -1;
    }
    return 0;
}

static int changeDir(char *path)
{
    int ret;
    
    ret = chdir(path);
    if(ret!=0)
    {
        printf("no such file or directory %s\n", path);
        return -1;
    }
    return 0;
}

static int removeFile(char *name)
{
    int ret;
    
    ret = rmdir(name);
    if(ret!=0)
    {
        ret = remove(name);
        if(ret != 0)
        {
            printf("no such file or directory %s\n", name);
            return -1;
        }
    }
    return 0;
}

void Readline(void)
{
    unsigned char ucChar;
    unsigned int ulIdx = 0;
    
    while(1)
    {
        ucChar = getc(stdin);
        
        if((ucChar == 127) || (ucChar == 8))
        {
            //Erase the last character from the input line
            printf("\b \b");
            ulIdx--;
            commstr[ulIdx] = '\0';
        }
        else if((ucChar == '\r') || (ucChar == '\n'))
        {
            //printf("\n");
            return;
        }
        else
        {

            commstr[ulIdx++] = ucChar;
            commstr[ulIdx] = '\0';
        }
    }
}

static int ListFiles(char *path)
{
    int fcount = 0;
    DIR *dirp;
    struct dirent *pDirent;

    dirp = opendir(path);

    if(dirp == NULL)
    {
        printf("Error on opendir %s\n", path);
        return -1;
    }

    while((pDirent = readdir(dirp)) != NULL)
    {
        fcount++;
        printf("\n%s", pDirent->d_name);
    }

    closedir(dirp);
    return(fcount);
}

int main(int argc, char * argv[])
{
    char * root_path;
    char cwd[PATH_SIZE];
    char *cmdstr;
    char *pathstr;
    char *str3 = NULL;
    char *search = " ";

    if(argc != 2)
    {
       printf("missing argument..\nRight way is %s <path to USB MSD>\n", argv[0]);
       return -1;
    }

    else
    {
       memset(root_path, 0, sizeof(root_path));
       root_path = argv[1];
       printf("%s\n", root_path);
    }

    ListFiles(root_path);

    while(1)
    {
        memset(cwd, 0, sizeof(cwd));
        if(getcwd(cwd, sizeof(cwd)) == NULL)
        {
            printf("couldn't get current path\n");
            return -1;
        }

        printf("\n%s>", cwd);
        Readline();
        
        cmdstr = strtok(commstr, search);
        
        if(strcmp(cmdstr,"ls") == 0)
        {
            ListFiles(cwd);
        }
        else if(strcmp(cmdstr, "rm") == 0)
        {
            cmdstr = strtok(NULL, search);
            removeFile(cmdstr);
        }
        else if(strcmp(cmdstr, "cd") == 0)
        {
            cmdstr = strtok(NULL, search);
            changeDir(cmdstr);
        }
        else if(strcmp(cmdstr, "mkdir") == 0)
        {
            cmdstr = strtok(NULL, search);
            makeDir(cmdstr);
        }
        else if(strcmp(cmdstr, "gohome") == 0)
        {
            changeDir(root_path);
        }
        else if(strcmp(cmdstr, "rename") == 0)
        {
            cmdstr = strtok(NULL, search);
            str3 = strtok(NULL, search);

            renameFile(cmdstr, str3);
        }
        else if(strcmp(cmdstr, "read") == 0)
        {
            cmdstr = strtok(NULL, search);
            readFile(cmdstr);
        }
        else if(strcmp(cmdstr, "write") == 0)
        {
            cmdstr = strtok(NULL, search);
            str3 = strtok(NULL, search);

            writeFile(cmdstr, str3);
        }
        else
        {
            printf("\ninvalid command\n");
        }

    }
    return 0;
}
