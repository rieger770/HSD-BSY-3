#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "directory.h"

#define TEST_DIR "dir_test"

int main(void)
{
    // create test dir and change into it
    AddDir(TEST_DIR);
    ChangeToDir(TEST_DIR);

    // safety check: only continue if we are really inside the test dir,
    // otherwise RemoveAll would delete the wrong directory
    char cwd[PATH_MAX];
    char const *last = getcwd(cwd, sizeof cwd) ? strrchr(cwd, '/') : NULL;
    if (last == NULL || strcmp(last + 1, TEST_DIR) != 0)
    {
        fprintf(stderr, "not inside %s, abort!\n", TEST_DIR);
        return 1;
    }

    // AddDir / AddFile with default rights
    printf("--- AddDir / AddFile ---\n");
    AddDir("a");
    AddDir("a/b");
    AddFile("f1.txt");
    AddFile("a/f2.txt");

    // SetAccessRights: new entries get 0700
    printf("--- SetAccessRights(0700) ---\n");
    SetAccessRights(0700);
    AddDir("a/b/private");
    AddFile("a/b/f3.txt");

    // error case: dir already exists
    printf("--- error case: AddDir(\"a\") again ---\n");
    AddDir("a");

    printf("--- PrintDir ---\n");
    PrintDir();

    printf("--- RemoveAll ---\n");
    RemoveAll();
    PrintDir();     // should print no entries

    // clean up test dir itself
    ChangeToDir("..");
    rmdir(TEST_DIR);

    return 0;
}
