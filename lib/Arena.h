#ifndef ARENA_H
#define ARENA_H
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#define ARENA_INITIAL_SIZE 256
#define ARENA_DEFAULT_ALIGNMENT 2 * (sizeof(void *))
ArrayDefine(char);
typedef struct arena
{
    char *mem_block_base;
    char *mem_block_end;
    size_t current_pos;
    

} Arena_t;

typedef enum rc
{
    ARENA_SUCCESS = 0,
    ARENA_FAIL = 1
} ARENA_RETURN_CODE;

ARENA_RETURN_CODE ArenaInit(Arena_t *);
void *ArenaAllocAligned(Arena_t *, size_t, size_t);
void *ArenaAlloc(Arena_t *, size_t);
void ArenaFree(Arena_t *);
#endif