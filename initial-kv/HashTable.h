#include "fnv.h"
#include <stdio.h>
#include <stdlib.h>
#include "../MyCLib/Types.h"
#define HASH_TABLE_INIT_SIZE 64
typedef struct hashtable
{
    char *buf;
    size_t count;    // current max index
    size_t capacity; // calculates the actual allocated bytes
} HashTable;
/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.

 */
char *itoa(int value, char *result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36)
    {
        *result = '\0';
        return result;
    }

    char *ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do
    {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
    } while (value);

    // Apply negative sign
    if (tmp_value < 0)
        *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr)
    {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

HashTable *hash_table_create(size_t size)
{
    HashTable *ht = (HashTable *)malloc((sizeof(HashTable))); // this is a structure to be returned
    if (size != 0)
    {
        ht->buf = (char *)calloc(size, sizeof(char *));
        ht->count = 0;
        ht->capacity = sizeof(char *) * size;
    }
    else
    {
        ht->buf = (char *)calloc(HASH_TABLE_INIT_SIZE, sizeof(char *));
        ht->capacity = sizeof(char *) * size;
        ht->count = 0;
    }
    return ht;
}
s32 hash_table_destroy(HashTable *ht)
{
    free(ht->buf);
    free(ht);
    return 0;
}
s32 insert_into_hash_table(HashTable *ht, int key, char *value)
{
    char *key_str;
    Fnv64_t hash = fnv_64a_str(itoa(key, key_str, 10), FNV1A_64_INIT); // hash is calculated, check if the hash table is 3/4 full or if a collsion is found.
    
}