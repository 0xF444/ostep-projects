#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../lib/String.h"
#define MAX_LINE_SIZE 300
#define CHUNK_SIZE 8192
void PrintFileContents(CStr file)
{
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        printf("wcat: cannot open file\n");
        exit(1); // Exit whenever a file is not found
    }
    // fseek(fp, 0L, SEEK_END);
    // uint64_t sz = ftell(fp);
    // rewind(fp);
    // char str[sz];
    // while (fgets(str, MAX_LINE_SIZE, fp) != NULL) // fgets will read a line by line not exceeding the specified size
    // {
    //     printf("%s", str);
    // }

    char intermediate_buf[CHUNK_SIZE];
    int bytes_read = 0;
    while (((bytes_read = fread(intermediate_buf, sizeof(char), CHUNK_SIZE, fp)) > 0))
    {
        fwrite(intermediate_buf, sizeof(char), bytes_read, stdout); // fwrite goes out of bounds if used with max_chunk, fread is fine
    }
    fclose(fp);
}
int main(int argc, CStr argv[])
{
    if (argc < 2)
    {
        // printf("Usage: ./wcat <file>");
    }
    else
    {
        for (int cnt = 1; cnt < argc; cnt++)
        {
            PrintFileContents(argv[cnt]);
        }
    }
    return 0;
}