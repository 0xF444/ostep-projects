#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "../lib/String.h"
#include "../lib/Arena.h"
#define CHUNK_SIZE 8192
typedef struct __attribute__((packed)) character_occurrence
{
    uint32_t count;
    char pivot;
} CharOccur;
int RunLengthEncodeFile(CStr filename)
{
    FILE *in_fp = fopen(filename, "r");
    if (!in_fp)
    {
        return 1;
    }
    else
    {

        char *out_filename = (char *)malloc(strlen(filename) + strlen(".rle") + 1);
        memset(out_filename, 0, strlen(filename) + strlen(".rle"));
        strncpy(out_filename, filename, strlen(filename));
        strncat(out_filename, ".rle", 4);

        char *intermediate_buf = (char *)malloc(CHUNK_SIZE);
        FILE *out_fp = fopen(out_filename, "wb");
        int bytes_read = 0;
        while (((bytes_read = fread(intermediate_buf, sizeof(char), CHUNK_SIZE, in_fp)) > 0)) // Chunking loop for better reading into the buffer.
        {
            for (int i = 0; i < bytes_read;)
            {
                if (!isalnum((unsigned char)intermediate_buf[i]))
                {
                    i++; // Ignore noncharacters because not advancing them will cause you to stop at them.
                    continue;
                }
                CharOccur character_pivot = {.pivot = intermediate_buf[i], .count = 0};
                for (size_t j = i; intermediate_buf[j] == character_pivot.pivot && isalpha(intermediate_buf[j]); j++)
                {
                    character_pivot.count++;
                }
                size_t written = fwrite(&character_pivot, sizeof(character_pivot), 1, stdout);
                i += (character_pivot.count);
            }
        }
        fclose(out_fp);
        free(out_filename);
        free(intermediate_buf);
    }
    fclose(in_fp);
}
int main(int argc, char **argv)
{
    /* Zipping is RLE (Run Length Encoding) */
    /* https://en.wikipedia.org/wiki/Run-length_encoding */
    if (argc < 2)
    {
        printf("wzip: file1 [file2 ...]\n");
    }
    else
    {
        for (size_t i = 1; i < argc; i++)
        {
            /* TODO: Make it concat the files passed in and treat it as ONE source */
            RunLengthEncodeFile(argv[i]);
        }
    }

    return 0;
}