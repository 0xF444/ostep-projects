#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include "../lib/String.h"
#include "../lib/Arena.h"
typedef struct __attribute__((packed)) byte_occur
{
    uint32_t count;
    char pivot;
} ByteOccur;

typedef struct file_mapped
{
    char *buf;
    uint64_t *sizes_array; // Size of each file
    uint64_t total_size_of_files;
    FILE **fds; // Array of file descriptors
} MyFileMapping;

MyFileMapping ReadFilesToBuf(int number_of_files, CStr *filenames_vector)
{
    MyFileMapping files;
    struct stat st;
    // Initialize the arrays length with number_of_files
    files.fds = (FILE **)malloc(sizeof(files.fds) * number_of_files);
    files.sizes_array = (uint64_t *)malloc(sizeof(files.sizes_array) * number_of_files);
    files.total_size_of_files = 0;
    for (int i = 0; i < number_of_files; i++)
    {
        /* Loop 1: read in the files and calculate their sizes */
        files.fds[i] = fopen(filenames_vector[i], "rb");
        if (!files.fds[i])
        {
            exit(1);
        }
        stat(filenames_vector[i], &st);
        files.sizes_array[i] = st.st_size;
        files.total_size_of_files += (uint64_t)st.st_size;
    }
    files.buf = (char *)malloc(files.total_size_of_files); // We can just cast it as ByteOccur
    uint64_t file_offset = 0;
    for (int i = 0; i < number_of_files; i++)
    {
        /* Loop 2: read every file into the intermediate buffer */
        fread(files.buf + file_offset, files.sizes_array[i], 1, files.fds[i]);
        file_offset += files.sizes_array[i];
    }
    return files;
}
int RunLengthDecodeFile(int argc, CStr *argv)
{

    MyFileMapping files = ReadFilesToBuf(argc - 1, argv + 1); // Make sure not to pass in argv[0]

    if (files.total_size_of_files % sizeof(ByteOccur) == 0) // That's because we want to read in the data as an array of ByteOccur
    {
        ByteOccur *encoded_data = (ByteOccur *)files.buf; // This is the trick!!!
        uint64_t size_of_encoded_data = files.total_size_of_files / sizeof(ByteOccur);
        for (size_t i = 0; i < size_of_encoded_data; i++)
        {
            // printf("current count: %d\n", encoded_data[i].count);
            for (size_t j = 0; j < encoded_data[i].count; j++)
            {

                // printf("%c\n", encoded_data[i].pivot);
                fwrite(&encoded_data[i].pivot, 1, 1, stdout);
            }

            // fwrite(&encoded_data[i].pivot, sizeof(encoded_data[i].pivot), encoded_data[i].count, stdout); // if you need it in a file, just redirect it lol
        }
    }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }
    else
    {
        RunLengthDecodeFile(argc, argv);
    }

    return 0;
}