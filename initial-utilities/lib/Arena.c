#include "Arena.h"

ARENA_RETURN_CODE ArenaInit(Arena_t *arena)
{
    arena->mem_block_base = (char *)malloc(ARENA_INITIAL_SIZE);
    if (!(arena->mem_block_base))
    {
        return ARENA_FAIL;
    }
    arena->mem_block_end = arena->mem_block_base + ARENA_INITIAL_SIZE; // Initially speaking, this should be at the end
    arena->current_pos = 0;
}

void *ArenaAllocAligned(Arena_t *arena, size_t type_size, size_t align)
{
    uintptr_t aligned_current_addr = AlignForward_Internal((arena->mem_block_base + arena->current_pos) + type_size, ARENA_DEFAULT_ALIGNMENT);
    if (aligned_current_addr >= arena->mem_block_end) // Check if current_pos has overcame the buffer.
    {
        size_t current_size = arena->mem_block_end - arena->mem_block_base;
        char *new_ptr = realloc(arena->mem_block_base, 2 * current_size); // Double the buffer each time you surpass it.
        arena->mem_block_end = new_ptr + (2 * current_size);
        arena->mem_block_base = new_ptr;
    }
    uintptr_t aligned_offset = aligned_current_addr - (uintptr_t)arena->mem_block_base;
    arena->current_pos += aligned_offset;
    return (arena->mem_block_base + arena->current_pos) - aligned_offset;
}
uintptr_t AlignForward_Internal(uintptr_t ptr, size_t align)
{
    size_t mod = ptr % align;
    size_t offset_needed = (align - mod) % align;
    return (ptr + offset_needed);
}
void ArenaFree(Arena_t *arena)
{
    free(arena->mem_block_base);
    arena->mem_block_base = -1;
}
void *ArenaAlloc(Arena_t *arena, size_t size)
{
    return ArenaAllocAligned(arena, size, ARENA_DEFAULT_ALIGNMENT);
}

// TODO: test with other structs that fill up the arena.