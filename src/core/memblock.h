#ifndef CORE_MEM_BLOCK_H
#define CORE_MEM_BLOCK_H

#include <stdint.h>

typedef struct {
	char *mem;
	uint32_t size;
} memblock;

int core_memblock_init(memblock *block, uint32_t initial_size);

int core_memblock_check_size(memblock *block, uint32_t size);

void core_memblock_free(memblock *block);

#endif // CORE_MEM_BLOCK_H
