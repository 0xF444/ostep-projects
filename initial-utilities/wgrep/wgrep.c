#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "lib/String.h"
#include <stdbool.h>
StringArray get_lines(CStr file, bool is_file_passed)
{
    FILE *input_src;
    if (is_file_passed)
    {
        input_src = fopen(file, "r");
    }
    else
    {
        input_src = stdin;
    }
    if (!input_src)
    {
        printf("wgrep: cannot open file\n");
        exit(1); // Exit whenever a file is not found
    }
    int bytes_read;
    char *line = NULL;
    size_t read_size = 0;
    StringArray lines_read = {0};
    while ((bytes_read = getline(&line, &read_size, input_src)) != -1)
    {
        line[strcspn(line, "\n")] = '\0';
        ArrayAppend(String, lines_read, CStringToString(line));
    }
    if (input_src != stdin)
        close(input_src);
    return lines_read;
}

void print_filtered_lines(StringArray lines, CStr target)
{
    for (size_t i = 0; i < lines.count; i++)
    {
        size_tArray found_indices = StringFindAllOccurrencesCStringTarget(&lines.items[i], target);
        if ((found_indices.capacity | (size_t)found_indices.items | found_indices.count) == NULL)
        {
            continue;
        }
        if (found_indices.items > 0)
        {
            CStr line = StringToCString(lines.items[i]);
            printf("%s\n", line);
            free(line);
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }
    else if (argc == 2)
    {
        // This means target was given but the file itself was piped via stdin
        StringArray lines = get_lines(NULL, false);
        print_filtered_lines(lines, argv[1]);
    }
    else
    {
        // arg1 should have the pattern and arg2 for the file itself.
        StringArray lines = {0};
        lines = get_lines(argv[2], true); // Lines are read, figure out how the target is passed
        print_filtered_lines(lines, argv[1]);
    }
}
