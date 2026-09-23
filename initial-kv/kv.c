#include "../MyCLib/Arena.h"
#include "../MyCLib/String.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "CommandParser.h"
int main(int argc, char **argv)
{

    if (argc < 2)
    {
        exit(0);
    }
    else
    {
        // TODO: Think of a way to return the same data with the length of commands parsed
        Command_t *commands = ParseCommands(argc, argv);
    }
}