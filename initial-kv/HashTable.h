#include "fnv.h"
#include <stdio.h>
#include <stdlib.h>
#include "../MyCLib/Types.h"
#include "../MyCLib/String.h"
#include <string.h>
#define HASH_TABLE_INIT_SIZE 64
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
typedef enum
{
    SLOT_EMPTY = 0,
    SLOT_OCCUPIED,
    SLOT_DELETED
} HTSlotState_t;
typedef struct HTentry
{
    int key;
    char *value;
    HTSlotState_t state;
    Fnv64_t stored_hash;
} HashTableEntry_t;

typedef struct hashtable
{
    HashTableEntry_t *buf;
    size_t count;             // occupied slots
    size_t capacity_in_bytes; // calculates the actual allocated bytes
    size_t max_entry_slots;
} HashTable;
Fnv64_t calculate_hash(u32 key)
{
    char *key_str;
    return fnv_64a_str(itoa(key, key_str, 10), FNV1A_64_INIT);
}
HashTable *hash_table_create(size_t size)
{
    HashTable *ht = (HashTable *)malloc((sizeof(HashTable))); // this is a structure to be returned
    if (size != 0)
    {
        ht->buf = (HashTableEntry_t *)calloc(size, sizeof(HashTableEntry_t)); // starts zeroed out
        ht->count = 0;
        ht->capacity_in_bytes = sizeof(HashTableEntry_t) * size;
    }
    else
    {
        ht->buf = (HashTableEntry_t *)calloc(HASH_TABLE_INIT_SIZE, sizeof(HashTableEntry_t *));
        ht->capacity_in_bytes = sizeof(HashTableEntry_t *) * size;
        ht->count = 0;
    }
    ht->max_entry_slots = ht->capacity_in_bytes / sizeof(*ht->buf);
    return ht;
}
s32 hash_table_destroy(HashTable *ht)
{
    free(ht->buf);
    free(ht);
    return 0;
}
u32 internal_threshold_check_hash_table_size(HashTable *ht)
{
    if ((ht->capacity_in_bytes) * 3 == ((ht->count) + 1) * 4)
    {
        return 1;
    }
    return 0;
}
s64 internal_find_duplicate_entry(HashTable *ht, HashTableEntry_t entry)
{

    for (size_t i = 0; i < ht->max_entry_slots; i++)
    {
        if (ht->buf[i].key == entry.key && ht->buf[i].state == SLOT_OCCUPIED)
            return i;
        else
            return -1;
    }
}
s64 internal_resize_hash_table(HashTable *ht)
{
    HashTableEntry_t *new_buf = calloc(sizeof(*ht->buf), 2 * ht->capacity_in_bytes);

    for (size_t i = 0; i < ht->max_entry_slots; i++)
    {
        if(ht->buf[i].state = SLOT_OCCUPIED)
    }

    ht->buf = new_buf;
}
s32 insert_into_hash_table(HashTable *ht, int key, char *value, u8 resize_flag)
{

    if (internal_threshold_check_hash_table_size(ht) && resize_flag)
    {
        internal_resize_hash_table(ht);
    }
    Fnv64_t hash = calculate_hash(key);

    size_t target_indx = hash % ht->max_entry_slots;
    HashTableEntry_t new_entry = {.key = key, .value = value, .stored_hash = hash, .state = SLOT_OCCUPIED};
    size_t cnt = 0, first_tombstone = SIZE_MAX;
    for (size_t i = target_indx; cnt < ht->max_entry_slots; i = (i + 1) % ht->max_entry_slots, cnt++)
    {
        if (ht->buf[i].state == SLOT_EMPTY)
        {
            if (first_tombstone == SIZE_MAX)
            {
                ht->buf[i] = new_entry;
                ht->count++;
                return 0;
            }
            else
            {
                ht->buf[first_tombstone] = new_entry;
                ht->count++;
                return 0;
            }
        }
        else if (ht->buf[i].state == SLOT_DELETED)
        {
            if (first_tombstone == SIZE_MAX)
            {
                first_tombstone = i;
                continue;
            }
            else
                continue;
        }
        else if (ht->buf[i].state == SLOT_OCCUPIED)
        {
            if (ht->buf[i].key == new_entry.key)
            {
                ht->buf[i] = new_entry;
                return 0;
            }
            else
                continue;
        }
    }
    if (cnt == ht->max_entry_slots)
    {
        ht->buf[first_tombstone] = new_entry;
        ht->count++;
        return 0;
    }
    return -1;
}
s32 get_from_hash_table() {}
