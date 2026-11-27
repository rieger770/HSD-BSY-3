#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>    
#include <unistd.h>    
#include <fcntl.h>     
#include <errno.h>
#include <string.h>
#include <limits.h>    
#include <sys/stat.h>  


#include "directory.h"

//error messages
static char const* const cErrOpenDir  = "error in opendir";
static char const* const cErrChangeDir = "error in chdir!";
static char const* const cErrCWD = "invalid current working directory!";
static char const* const cErrMkDir = "error in mkdir!";
static char const* const cErrFOpen = "error in open!";
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
        // e.g. path does not exist / is no dir -> message on stderr
        perror(cErrChangeDir);
    }
}

// helper: prints all entries of path, indented by depth (2 spaces per level)
static void PrintRecursive(char const *path, int depth)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror(cErrOpenDir);
        return;
    }

    // read entries one by one until NULL (end of directory)
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
            // build relative path of sub dir: "<path>/<name>"
            char sub[PATH_MAX];
            snprintf(sub, sizeof(sub), "%s/%s", path, entry->d_name);
            PrintRecursive(sub, depth + 1);
        }
    }
    // release directory stream
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
    // call recursive helper func, start at "." with depth 0
    PrintRecursive(".", 0);

}

void AddDir(char const * const name)
{
    // create new dir with current access rights (umask still applies)
    if (mkdir(name, sMode) != 0)
    {
        perror(cErrMkDir);
    }
}

void AddFile(char const * const name)
{
    // create empty file with current access rights
    // open() instead of fopen() -> fopen() always uses 0666
    // O_EXCL -> error if file already exists
    int fd = open(name, O_WRONLY | O_CREAT | O_EXCL, sMode);
    if (fd == -1)
    {
        perror(cErrFOpen);
        return;
    }
    // file stays empty -> close immediately
    close(fd);
}

// helper: deletes everything inside path (path itself is kept)
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

        // build relative path of entry: "<path>/<name>"
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
    // start at current working directory
    RemoveRecursive(".");
}

void SetAccessRights(mode_t const mode)
{
    // only stored -> used by next AddDir / AddFile calls
    sMode = mode;
}
