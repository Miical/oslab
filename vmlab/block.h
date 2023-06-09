#ifndef BLOCK_H_
#define BLOCK_H_

#define BLOCK_SECTOR_SIZE 512
typedef unsigned int block_sector_t;

void block_read(block_sector_t sector, void* buffer);
void block_write(block_sector_t sector, void* buffer);

#endif
