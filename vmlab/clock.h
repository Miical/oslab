#ifndef CLOCK_H_
#define CLOCK_H_

#include "block.h"

void block_read_b(block_sector_t sector, void* buffer);
void block_write_b(block_sector_t sector, void* buffer);
void show_cache();

#endif
