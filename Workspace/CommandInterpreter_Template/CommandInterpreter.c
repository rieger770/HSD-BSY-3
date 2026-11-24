/*CommandInterpreter.c*/
#include <string.h>
#include "CommandInterpreter.h"

// module internal members
static TCmdHndTable const *gcmds = 0; // table of available commands
static int (*gpPutChar)(int);         // function pointer for a put-char-function
static TParameters gParams;           // struct that holds all entered params to last cmd
static char gInput[MAX_INPUTLEN];     // module internal input buffer
static int gInputLen = 0;             // length of input string

// welcome message
static char *cWelcomePrint = "CmdHnd: Welcome!";
static char *cInputSym = "\n>";
// error messages
static char *cErrPrint = "Error in Print(...) -> null pointer in parameter\n";

//////////////////////////////
// interface module functions
//////////////////////////////

void Init(TCmdHndTable const cmds[], int (*PutCh)(int))
{
   memset(gInput, 0, sizeof(gInput)); // clear input buffer
   gInputLen = 0;                     // reset input length
   gcmds = cmds;                      // store command table
   gpPutChar = PutCh;                 // store put-char-function pointer
   Print(cWelcomePrint);
   Print(cInputSym);
}

// pre declaration
static void evaluateCmd();
static void checkInput();

int Process(char const ch)
{
   if (ch == cESC)
   {
      return cESC;
   }

   if(ch == '\n' || ch == '\r')
   {
      checkInput(); // evaluate the completed input line
      // clear buffer for next command input
      gInputLen = 0;
      gInput[0] = '\0';
      Print(cInputSym);
   }
   else if (gInputLen < MAX_INPUTLEN - 1)
   {
      // copy char into input buffer
      gInput[gInputLen++] = ch;
      gInput[gInputLen] = '\0';
      // echo char via the put-char-function
      char const echo[2] = {ch, '\0'};
      Print(echo);
   }

   return ch;
}

TParameters GetParams()
{
   return gParams;
}

void Print(char const *str)
{
   // check parameter
   if (str == 0)
   {
      Print(cErrPrint);
      return;
   }

   int i = 0;
   if (gpPutChar != 0)
   { // put char function pointer given?
      while (str[i] != '\0')
      { // iterate given string
         gpPutChar(str[i++]);
      }
   }
}

/////////////////////////////
// internal module functions
/////////////////////////////


// returns the index of the first non-space char starting at index i
static int SkipSpaces(int i)
{
   while(gInput[i] == ' ') // skip spaces
   {
      i++;
   }
   return i;
}

// evaluates a completed command
static void evaluateCmd()
{
   memset(&gParams, 0, sizeof(gParams)); // clear params of last command
   int i = 0;                            // read position in gInput
   int n = 0;                            // number of tokens found

   // split input into tokens: params[0] = command, params[1..] = parameters
   while(1)
   {
      i = SkipSpaces(i);
      if(gInput[i] == '\0') // end of input
      {
         break;
      }

      if(n > MAX_PARAMS) // command + MAX_PARAMS parameters allowed
      {
         Print("\nError: too many parameters");
         return;
      }

      int j = 0;
      while(gInput[i] != ' ' && gInput[i] != '\0')
      {
         if(j >= MAX_PARAMLEN)
         {
            Print("\nError: parameter too long");
            return;
         }
         gParams.params[n][j++] = gInput[i++];
      }
      gParams.params[n][j] = '\0';
      n++;
   }

   if(gcmds == 0)
   {
      return;
   }

   // search command in table and execute it
   for(int k = 0; gcmds[k].func != 0; k++)
   {
      if(strcmp(gcmds[k].command, gParams.params[0]) == 0)
      {
         // execute command and show its return value (0 = error)
         if(gcmds[k].func() != 0)
         {
            Print("\nOK");
         }
         else
         {
            Print("\nERROR");
         }
         return;
      }
   }

   Print("\nError: unknown command '");
   Print(gParams.params[0]);
   Print("'");
}

// checks the input for correctness
static void checkInput()
{
   const int i = SkipSpaces(0);

   if(gInput[i] == '\0') // check if command is given
   {
      Print("\n");
   }
   else
   {
      evaluateCmd(); // call evaluation
   }

}
