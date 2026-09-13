#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "../lib/String.h"
#include "../lib/Arena.h"
#include <sys/stat.h>
#define CHUNK_SIZE 8192
typedef struct __attribute__((packed)) character_occurrence
{
    uint32_t count;
    char pivot;
} CharOccur;
int RunLengthEncodeFile(int argc, CStr *argv)
{
    struct stat st;
    /* Array definitions have argc - 1 because we don't want argv[0] */
    uint64_t sizes[argc - 1], total_size = 0;
    FILE *in_fds[argc - 1];
    for (int i = 1; i < argc; i++)
    {
        /* Loop 1: read in the files and calculate their sizes */
        in_fds[i - 1] = fopen(argv[i], "r");
        if (!in_fds[i - 1])
        {
            exit(1);
        }
        stat(argv[i], &st);
        sizes[i - 1] = st.st_size;
        total_size += (uint64_t)st.st_size;
    }
    char *intermediate_buf = (char *)malloc(total_size);

    uint64_t file_offset = 0;
    for (int i = 1; i < argc; i++)
    {
        /* Loop 2: read every file into the intermediate buffer */
        fread(intermediate_buf + file_offset, sizes[i - 1], 1, in_fds[i - 1]);
        file_offset += sizes[i - 1];
    }

    FILE *out_fd = fopen("fileout.rle", "wb");
    for (uint64_t i = 0; i < total_size;)
    {
        /* Loop 3: Logic of RLE */

        CharOccur byte_pivot = {.pivot = intermediate_buf[i], .count = 0};
        for (size_t j = i; j < total_size && intermediate_buf[j] == byte_pivot.pivot; j++)
        {
            byte_pivot.count++;
        }
        fwrite(&byte_pivot, sizeof(byte_pivot), 1, out_fd); // Change file descriptor here
        i += (byte_pivot.count);
    }
    for (int i = 1; i < argc; i++)
    {
        /* Loop 3: Close all file descriptors*/
        fclose(in_fds[i - 1]);
    }

    fclose(out_fd);
    free(intermediate_buf);
    return 0;
}
int main(int argc, char **argv)
{
    /* Zipping is RLE (Run Length Encoding) */
    /* https://en.wikipedia.org/wiki/Run-length_encoding */
    if (argc < 2)
    {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }
    else
    {
        RunLengthEncodeFile(argc, argv);
    }

    return 0;
}
