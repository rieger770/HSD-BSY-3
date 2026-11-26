#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include "directory.h"

#define TEST_DIR "dir_test"

static void PrintMode(char const *path)
{
    struct stat st;
    if (stat(path, &st) != 0)
    {
        perror("error in stat!");
        return;
    }
    printf("  %-14s %04o\n", path, (unsigned)(st.st_mode & 07777));
}

int main(void)
{
    umask(0022);

    AddDir(TEST_DIR);
    ChangeToDir(TEST_DIR);

    char cwd[PATH_MAX];
    char const *last = getcwd(cwd, sizeof cwd) ? strrchr(cwd, '/') : NULL;
    if (last == NULL || strcmp(last + 1, TEST_DIR) != 0)
    {
        fprintf(stderr, "not inside %s, abort!\n", TEST_DIR);
        return 1;
    }

    printf("--- AddDir / AddFile (default rights) ---\n");
    AddDir("a");
    AddDir("a/b");
    AddFile("f1.txt");
    AddFile("a/f2.txt");

    // SetAccessRights: new entries get 0700
    printf("--- SetAccessRights(0700) ---\n");
    SetAccessRights(0700);
    AddDir("a/b/private");
    AddFile("a/b/f3.txt");

    // SetAccessRights: new entries get 0644
    printf("--- SetAccessRights(0644) ---\n");
    SetAccessRights(0644);
    AddFile("a/b/f4.txt");

    // check the rights of the created entries
    printf("--- access rights ---\n");
    PrintMode("a");
    PrintMode("a/b");
    PrintMode("f1.txt");
    PrintMode("a/f2.txt");
    PrintMode("a/b/private");
    PrintMode("a/b/f3.txt");
    PrintMode("a/b/f4.txt");

    // error cases -> messages on stderr
    printf("--- error case: AddDir(\"a\") again ---\n");
    fflush(stdout);
    AddDir("a");

    printf("--- error case: AddFile(\"f1.txt\") again ---\n");
    fflush(stdout);
    AddFile("f1.txt");

    printf("--- error case: AddDir in missing dir ---\n");
    fflush(stdout);
    AddDir("missing/x");

    printf("--- error case: ChangeToDir(\"missing\") ---\n");
    fflush(stdout);
    ChangeToDir("missing");

    printf("--- error case: ChangeToDir(\"f1.txt\") (no dir) ---\n");
    fflush(stdout);
    ChangeToDir("f1.txt");

    // ChangeToDir into sub dir and back
    printf("--- ChangeToDir(\"a/b\") + PrintDir ---\n");
    ChangeToDir("a/b");
    PrintDir();
    ChangeToDir("../..");

    printf("--- PrintDir (whole test dir) ---\n");
    PrintDir();

    printf("--- RemoveAll ---\n");
    RemoveAll();
    PrintDir();     // should print no entries

    // clean up test dir itself
    ChangeToDir("..");
    rmdir(TEST_DIR);

    return 0;
}
