#include "memblock.h"

#include <stdlib.h>

int core_memblock_init(memblock *block, uint32_t initial_size)
{
    block->mem = calloc(sizeof(char), initial_size);
    if (!block->mem) {
        block->size = 0;
        return 0;
    }
    block->size = sizeof(char) * initial_size;
    return 1;
}

int core_memblock_check_size(memblock *block, uint32_t size)
{
    if (size <= block->size) {
        return 1;
    }
    char *new_mem = realloc(block->mem, sizeof(char) * size);
    if (new_mem == NULL) {
        return 0;
    }
    block->mem = new_mem;
    block->size = sizeof(char) * size;
    return 1;
}

void core_memblock_free(memblock *block)
{
    free(block->mem);
    block->size = 0;
}