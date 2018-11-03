#include "VoidBot.h"
#include "system/SetupSystem.h"
#include <string.h>
#include <stdio.h>

int main(int ArgC, char** ArgV)
{
	char * ch = strrchr(ArgV[0], '/');
	char * path = new char[ch - ArgV[0] + 1];
	strncpy(path, ArgV[0], ch - ArgV[0]);
	path[ch - ArgV[0]] = 0;
	chdir(path);
	delete [] path;
	
	muscle::CompleteSetupSystem css;
	VoidBot bot;
	int ReturnCode = bot.Run(ArgC, ArgV);
	fprintf (stderr, "Exiting AtrusBot with return code %d.\n", ReturnCode);
	return ReturnCode;
}

