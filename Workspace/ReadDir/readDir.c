//ist ein Feature-Test-Makro, das die Definition
//bestimmter Funktionen und Strukturen in den Header-Dateien
//ermöglicht, die sonst möglicherweise nicht verfügbar wären.
//z.B -> DT_REG, DT_DIR, d_type in struct dirent
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

//error messages
static char const* const cUsage       = "usage: ReadDirectory [path]\n";
static char const* const cErrOpenDir  = "error in opendir";
static char const* const cErrReadDir  = "error in readdir!";
static char const* const cErrCloseDir = "error in closedir!";

int main (int argc, char* argv[]) {
   int entries = 0;
   char* dirName = ".";  //current working direktory

   //check command line
   if (argc > 2)
   {
      fputs(cUsage,stderr);
      return EXIT_FAILURE;
   }

   if (argc == 2)
   {
      dirName = argv[1];
   }

   //to do

   //explizites Ruecksetzen der errno-Variablen
   errno = 0;

   //verzeichnis oeffnen
   DIR* pDir = opendir(dirName);
   if (pDir == NULL)
   {
      perror(cErrOpenDir);
      if(errno != 0)
      {
         //error message selbver konstruieren
         fprintf(stderr, "errno: %d -> %s\n", errno, strerror(errno));
      }
      return EXIT_FAILURE;
   }

   //lesen der einzelnen Eintraege
   errno = 0;
   struct dirent* pDirEntry = NULL;

   while((pDirEntry = readdir(pDir)) != NULL)
   {
      switch(pDirEntry->d_type)
      {
         case DT_REG:
            printf("file:              ");
            break;
         
         case DT_DIR: 
            printf("directory:         ");
            break;
         
         default:
            printf("unkown entry:      ");
            break;
      }
      printf("%20s [inode: %8ld] [len: %d]\n", pDirEntry->d_name, pDirEntry->d_ino, pDirEntry->d_reclen);
      entries++;
   }

   if(errno != 0)
   {
      perror(cErrReadDir);
      errno = 0;
      closedir(pDir);
      return EXIT_FAILURE;
   }

   //Verzeichnis schliessen
   if(closedir(pDir) != 0)
   {
      perror(cErrCloseDir);
      return EXIT_FAILURE;
   }

   printf("--- %d entries in directory ---\n", entries);

   //output current working directory
   char path[PATH_MAX] = {0};
   //to do
   if(getcwd(path, PATH_MAX) != 0)
   {
      puts(path);
      printf("path max: %d\n", PATH_MAX);
   }

   return EXIT_SUCCESS;
}
