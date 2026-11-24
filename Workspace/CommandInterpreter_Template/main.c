#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CommandInterpreter.h"
#include "CommandTable.h"


int main(int const argc, char const* const* argv){

   int ch = 0;  //int, so EOF can be distinguished from a valid char

   //initialize the command interpreter
   Init(GetCmdHndTable(), putchar);

   //read characters from input
   do {
      ch = getchar();			// wait for key
   } while(ch != EOF && Process((char)ch) != cESC);	//end of input or ESC typed?

	return EXIT_SUCCESS;
}
