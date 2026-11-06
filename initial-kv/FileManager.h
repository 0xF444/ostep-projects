#include "HashTable.h"
#include <unistd.h>
typedef struct __attribute__((packed)) file_header
{
    u16 magic;
    u64 number_of_existing_entries;
} KVFileHeader;
typedef struct __attribute__((packed)) file_entry
{
    s32 key;
    Fnv64_t stored_hash;
    u64 value_string_length;
    char value[];
} KVFileEntry;

// TODO: create a function that initially creates the file if it doesn't exist
s32 internal_check_if_file_exists()
{
    FILE *file = fopen("db.bin", "r");
    if (file != NULL)
    {
        fclose(file);
        return 0;
    }
    return -1;
}

s32 internal_create_file_header()
{
    if (internal_check_if_file_exists() == -1)
    {
        return -1;
    }
    FILE *file = fopen("db.bin", "a+");
    if (!file)
    {
        return -1;
    }
    else
    {
        KVFileHeader new_file_header = {.magic = 0xf444, .number_of_existing_entries = 0};
        fwrite(&new_file_header, sizeof(new_file_header), 1, file);
        return 0;
    }
}
// TODO: add a way to parse the file header.