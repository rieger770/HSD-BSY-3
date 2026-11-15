/*CommandTable.c*/
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include "CommandTable.h"
#include "CommandInterpreter.h"

//return code for command functions
int const OK = 1;
int const NOK = 0;

//internal command functions
static int CmdHelp(void);
static int CmdParamInt(void);
static int CmdParamFloat(void);
static int CmdParamString(void);
static int CmdAddInt(void);

//internal helper functions
static int ParseInt(char const* str, int* value);
static int IsFloat(char const* str);
static void PrintInt(long long value);

//CommandTable
static TCmdHndTable const cmds[] = {
	{CmdHelp,       "help",  "show help text"},
	{CmdHelp,       "?",     "show help text"},
	{CmdParamInt,   "int",   "[value] first parameter as integer"},
	{CmdParamFloat, "float", "[value] first parameter as float"},
	{CmdParamString,"string","[value] first parameter as string"},
	{CmdAddInt,     "addint","[value1] [value2] adds two integer values"},
	{0,0,0}  //end criteria
};


//returns address of command table
TCmdHndTable const* GetCmdHndTable()
{
	return cmds;
}


//////////////////////////////
// internal helper functions
//////////////////////////////

//converts a string to an integer
//returns OK and stores the number in value if the whole string is a valid int,
//otherwise NOK (value stays unchanged)
static int ParseInt(char const* str, int* value)
{
	int ret = NOK;	//return value
	char* end = 0;	//set by strtol to the first char that is not part of the number

	errno = 0;
	long const number = strtol(str, &end, 10);	//10 = decimal system

	//valid only if at least one char was read, the whole string was consumed
	//and the number fits into an int
	if(end != str && *end == '\0' &&
		errno != ERANGE && number >= INT_MIN && number <= INT_MAX)
	{
		*value = (int)number;
		ret = OK;
	}

	return ret;
}


//checks if the whole string is a valid floating point number
//returns OK if it is, otherwise NOK
static int IsFloat(char const* str)
{
	int ret = NOK;	//return value
	char* end = 0;	//set by strtof to the first char that is not part of the number

	strtof(str, &end);	//only the read position is needed, not the value

	//valid only if at least one char was read and the whole string was consumed
	if(end != str && *end == '\0')
	{
		ret = OK;
	}

	return ret;
}


//converts an integer to a string and prints it via Print
//(helper function, because printf and co. are prohibited)
static void PrintInt(long long value)
{
	char buf[24];             //enough for a 64 bit number incl. sign
	int i = sizeof(buf) - 1;  //fill buffer from the back
	//work with the absolute value (unsigned, so the smallest negative number fits too)
	unsigned long long v = (value < 0) ? 0ULL - (unsigned long long)value
	                                   : (unsigned long long)value;

	buf[i] = '\0';
	do // parsing v as positive value
	{
		buf[--i] = (char)('0' + v % 10);  //last digit -> character
		v /= 10;                          //remove last digit
	} while(v != 0);

	if(value < 0) // adding sign if value is negative.
	{
		buf[--i] = '-';
	}

	Print(&buf[i]);  //print from first used position
}


//////////////////////////////
// internal command functions
//////////////////////////////

//prints help text of all commands
static int CmdHelp(void){
	Print("\n(press <ESC> to exit)");
	Print("\navailable commands:");

	//iterate command table until end criteria
	for(int i = 0; cmds[i].func != 0; i++)
	{
		Print("\n");
		Print(cmds[i].command);
		Print("\t");
		Print(cmds[i].description);
	}

	return OK;
}


//interprets 1st parameter as integer and prints it
static int CmdParamInt(void){
	int ret = NOK;	//return value
	int value = 0;	//converted parameter

	TParameters p = GetParams();

	//further parameters are ignored
	if(ParseInt(p.params[1], &value) == OK)
	{
		Print("\nParameter: ");
		PrintInt(value);
		ret = OK;
	}

	return ret;
}


//interprets 1st parameter as float and prints it
static int CmdParamFloat(void){
	int ret = NOK;  //return value

	TParameters p = GetParams();

	//further parameters are ignored
	if(IsFloat(p.params[1]) == OK)
	{
		//the parameter is a valid float, so its text can be printed as it is
		Print("\nParameter: ");
		Print(p.params[1]);
		ret = OK;
	}

	return ret;
}


//interprets 1st parameter as string and prints it
static int CmdParamString(void)
{
	int ret = NOK;	//return value

	TParameters p = GetParams();

	//further parameters are ignored
	if(p.params[1][0] != '\0')	//parameter given?
	{
		Print("\nParameter: ");
		Print(p.params[1]);
		ret = OK;
	}

	return ret;
}


//adds two integer parameter and prints the result
static int CmdAddInt(void)
{
	int ret = NOK;  //return value
	int val1 = 0;   //1st summand
	int val2 = 0;   //2nd summand

	TParameters p = GetParams();

	//both parameters have to be valid integers, further parameters are ignored
	if(ParseInt(p.params[1], &val1) == OK && ParseInt(p.params[2], &val2) == OK)
	{
		//calculate in long long, so the sum of two ints can not overflow
		long long const res = (long long)val1 + val2;

		Print("\nResult: ");
		PrintInt(val1); // helper func, because printf is prohibited
		Print(" + ");
		PrintInt(val2);
		Print(" = ");
		PrintInt(res);
		ret = OK;
	}

	return ret;
}
