#ifndef _CMD_PARSER
#define _CMD_PARSER
#include "../MyCLib/String.h"
#define NUMBER_OF_COMMAND_FIELDS 3
enum COMMAND_INDEX
{
    INSTRUCTION_INDEX = 0,
    KEY_INDEX,
    VALUE_INDEX
};
typedef struct command
{
    char instruction;
    int key;
    char *value;
} InternalCommand_t;

typedef struct
{
    InternalCommand_t *commands_start_ptr;
    size_t num_of_commands;
} Command_t;

Command_t *ParseCommands(int argc, char **argv)
{
    InternalCommand_t *commands = (InternalCommand_t *)malloc((argc - 1) * sizeof(InternalCommand_t));
    Command_t *commands_returned = (Command_t *)malloc(sizeof(Command_t));
    String commands_string[argc - 1];
    StringViewArray commands_parsed_views[argc - 1];
    for (size_t i = 1, j = 0; i < argc; i++, j++) // Loop 1: Tokenize commands into views.
    {
        commands_string[j] = CStringToString(argv[i]);                       // commands should contain String type
        commands_parsed_views[j] = StringTokenize(commands_string + j, ","); // each command is tokenized by the ',' delim
    }
    for (size_t i = 0; i < argc - 1; i++) // Loop 2: Initialize command vectors to be returned.
    {
        commands[i].instruction = (char)*(commands_parsed_views[i].items[INSTRUCTION_INDEX].base); // Take only the first character of the base buffer
        char *intermediate_key = StringToCString(commands_parsed_views[i].items[KEY_INDEX]);       // Convert the key into a cstring
        switch (commands[i].instruction)
        {
        case 'p':
            commands[i].key = atoi(intermediate_key);
            free(intermediate_key);
            commands[i].value = StringToCString(commands_parsed_views[i].items[VALUE_INDEX]);
            break;
        case 'd':
        case 'g':
            commands[i].key = atoi(intermediate_key);
            free(intermediate_key);
            commands[i].value = NULL;
            break;
        case 'c':
        case 'a':
            break;
        default:
            exit(1);
            break;
        }
    }
    commands_returned->commands_start_ptr = commands;
    commands_returned->num_of_commands = argc - 1;
    return commands_returned;
}
#endif