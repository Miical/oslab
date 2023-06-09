#include <stdio.h>
#include "block.h"

#define UNUSED_VALUE(v) (void)(v);

void block_read(block_sector_t sector, void* buffer) {
    UNUSED_VALUE(buffer);
    // printf("Read Page #%u\n", sector);
}

void block_write(block_sector_t sector, void* buffer) {
    UNUSED_VALUE(buffer);
    // printf("Write Page #%u\n", sector);
}
