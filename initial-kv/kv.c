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
        Command_t *commands = ParseCommands(argc, argv);
        // for (size_t i = 0; i < commands->num_of_commands; i++)
        // {
        //     printf("Instruction: %c\n", commands->commands_start_ptr[i].instruction);
        //     printf("Key: %d\n", commands->commands_start_ptr[i].key);
        //     printf("Value: %s\n", commands->commands_start_ptr[i].value);
        //     printf("--------------------------\n");
        // }
    }
}