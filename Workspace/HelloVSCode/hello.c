#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// unterschiedliche Ausgabefunktionen
static void PrintArgs(char *const *const argv, int const argc)
{
   assert(argv != 0 && argc > 0);

   fprintf(stdout, "number: %d\n", argc);

   for (int i = 0; i < argc; ++i)
   {
      fputs(argv[i], stdout);
      fputc('\n', stdout);
      puts(argv[i]);
   }

   // -0.5 Pkte
   char out[256] = {};

   sprintf(out, "address of param: %p\n", (void *)&argc);
   fprintf(stderr, "%s", out);
   fputs(argv[0], stdout);
}

int main(int argc, char **argv)
{
   puts("Hello VS Code");
   PrintArgs(argv, argc);

   // stdout soll vollgepuffert werden
   // es wird ein interner Puffer mit entsprechender Groesze erstellt
   // Param 2 und Param 4 werden fuer einen eigenen Puffer benötigt.
   setvbuff(stdout, NULL, _IOFBF, 0);
   PrintArgs(argv, argc);
   
   return EXIT_SUCCESS;
}