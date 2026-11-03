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
    char key_str[16];
    return fnv_64a_str(itoa(key, key_str, 10), FNV1A_64_INIT);
}
HashTable *hash_table_create(size_t num_of_slots_required)
{
    HashTable *ht = (HashTable *)malloc((sizeof(HashTable))); // this is a structure to be returned
    if (num_of_slots_required != 0)
    {
        ht->buf = (HashTableEntry_t *)calloc(num_of_slots_required, sizeof(*ht->buf)); // starts zeroed out
        ht->count = 0;
        ht->capacity_in_bytes = sizeof(*ht->buf) * num_of_slots_required;
    }
    else
    {
        ht->buf = (HashTableEntry_t *)calloc(HASH_TABLE_INIT_SIZE, sizeof(*ht->buf));
        ht->capacity_in_bytes = sizeof(*ht->buf) * HASH_TABLE_INIT_SIZE;
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
    if (((ht->count) + 1) * 4 >= (ht->max_entry_slots) * 3)
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
    }
    return -1;
}
s64 internal_resize_hash_table(HashTable *old_ht)
{
    // TODO: figure out a way to rehash old entries to the new buffer
    HashTable *new_ht = hash_table_create(2 * old_ht->max_entry_slots);
    for (size_t i = 0; i < old_ht->max_entry_slots; i++)
    {
        if (old_ht->buf[i].state == SLOT_OCCUPIED)
        {
            insert_into_hash_table(new_ht, old_ht->buf[i].key, old_ht->buf[i].value);
        }
    }
    free(old_ht->buf);
    *old_ht = *new_ht;
    free(new_ht);
}
s32 insert_into_hash_table(HashTable *ht, int key, char *value)
{

    if (internal_threshold_check_hash_table_size(ht))
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
char *get_from_hash_table(HashTable *ht, int key)
{
    Fnv64_t input_hash = calculate_hash(key);
    usize target_indx = input_hash % ht->max_entry_slots;
    for (size_t i = 0, j = target_indx; i < ht->max_entry_slots; i++, j = (j + 1) % ht->max_entry_slots)
    {
        if (ht->buf[j].state == SLOT_OCCUPIED && ht->buf[j].stored_hash == input_hash && ht->buf[j].key == key)
        {
            return ht->buf[j].value;
        }
        else if (ht->buf[j].state == SLOT_EMPTY) // it was never used to begin with, the key cannot exist throughout the sequence
        {
            return NULL;
        }
    }
    return NULL;
}
s32 delete_from_hash_table(HashTable *ht, int key)
{
    Fnv64_t input_hash = calculate_hash(key);
    usize target_indx = input_hash % ht->max_entry_slots;
    for (size_t i = 0, j = target_indx; i < ht->max_entry_slots; i++, j = (j + 1) % ht->max_entry_slots)
    {
        if (ht->buf[j].state == SLOT_OCCUPIED && ht->buf[j].stored_hash == input_hash && ht->buf[j].key == key) // the value exists
        {
            ht->buf[j].state = SLOT_DELETED;
            ht->count--;
            return 0;
        }
        else if (ht->buf[j].state == SLOT_EMPTY) // it was never used to begin with, the key cannot exist throughout the sequence
        {
            return -1;
        }
    }
    return NULL;
}
