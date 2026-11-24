//ist ein Feature-Test-Makro, das die Definition
//bestimmter Funktionen und Strukturen in den Header-Dateien
//ermöglicht, die sonst möglicherweise nicht verfügbar wären.
//z.B -> DT_REG, DT_DIR, d_type in struct dirent
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>


//makros

//error messages
static char const* const cUsage       = "usage: ReadDirectory [path]\n";
static char const* const cErrOpenDir  = "error in opendir";
static char const* const cErrReadDir  = "error in readdir!";
static char const* const cErrCloseDir = "error in closedir!";
static char const* const cErrChangeDir = "error in chdir!";
static char const* const cErrCWD = "invalid current working directory!";
static char const* const cErrMkDir = "error in mkdir!";
static char const* const cErrFOpen = "error in fopen!";
static char const* const cErrRmDir = "error in rmdir!";
static char const* const cErrRemove = "error in remove!";

// access rights for new directories and files (default 0755)
static mode_t sMode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;



void ChangeToDir(char const * const path)
{
    // check if changing directory worked
    if(chdir(path) == 0)
    {
        return;
    }
    else
    {
        perror(cErrChangeDir);
    }
}

static void PrintRecursive(char const *path, int depth)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    { 
        perror(cErrOpenDir); 
        return; 
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        // skip current and parent dir
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }

        // print data name
        printf("%*s%s\n", depth * 2, "", entry->d_name);
        
        // start recursion if data is a dir
        if (entry->d_type == DT_DIR)
        {
            char sub[PATH_MAX];
            snprintf(sub, sizeof(sub), "%s/%s", path, entry->d_name);
            PrintRecursive(sub, depth + 1);
        }
    }
    closedir(dir);
}

void PrintDir()
{
    // get current working directory
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof cwd) == NULL)
    {
        perror(cErrCWD);
        return;
    }

    printf("Contents of working directory: %s\n", cwd);
    // call recursive helper func
    PrintRecursive(".", 0);

}

void AddDir(char const * const name)
{
    // create new dir with current access rights
    if (mkdir(name, sMode) != 0)
    {
        perror(cErrMkDir);
    }
}

void AddFile(char const * const name)
{
    FILE *data = fopen(name, "w");
    if (data == NULL) {
        perror(cErrFOpen);
        return;
    }
    fclose(data);
}

static void RemoveRecursive(char const *path)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror(cErrOpenDir);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char sub[PATH_MAX];
        snprintf(sub, sizeof(sub), "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_DIR)
        {
            RemoveRecursive(sub);      // empty the subdirectory first
            if (rmdir(sub) != 0)       // then delete it
                perror(cErrRmDir);
        }
        else
        {
            if (remove(sub) != 0)      // full path instead of just the name
                perror(cErrRemove);
        }
    }
    closedir(dir);
}

void RemoveAll()
{
    RemoveRecursive(".");
}

void SetAccessRights(mode_t const mode)
{
    sMode = mode;
}
