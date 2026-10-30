#include "../MyCLib/Arena.h"
#include "../MyCLib/String.h"
#include "../MyCLib/Structures.h"
#include "../MyCLib/Types.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "CommandParser.h"
s32 execute_commands(Command_t *commands)
{
    for (size_t i = 0; i < commands->num_of_commands; i++)
    {
        switch (commands->commands_start_ptr[i].instruction)
        {
        case 'd': // Delete
        case 'a': // All
        case 'p': // Put
        case 'c': // Clear
        }
    }
}
int main(int argc, char **argv)
{

    if (argc < 2)
    {
        exit(0);
    }
    else
    {
        Command_t *commands = parse_commands(argc, argv);
        execute_commands(commands);
    }
}