#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "block.h"

#define UNUSED_VALUE(v) (void)(v);

#define MAX_CACHE 8

static int hit_num, miss_num;
struct cache_item {
  block_sector_t sector;
  bool valid, accessed, dirty;
  char data[BLOCK_SECTOR_SIZE];
} cache[MAX_CACHE];

static int bpointer;
static void cache_evict(int id);
static void cache_insert(block_sector_t sector, int id);
static int cache_hit(block_sector_t sector);
static int cache_miss(block_sector_t sector);
static int access_buffer(block_sector_t sector);

static void cache_evict(int id) {
  printf("(Evict page #%d)\n", cache[id].sector);
  assert(cache[id].valid);

  if (cache[id].dirty) {
    printf("(Write back page #%d)\n", cache[id].sector);
    block_write(cache[id].sector, cache[id].data);
  }
  cache[id].valid = false;
}

static void cache_insert(block_sector_t sector, int id) {
  if (cache[id].valid)
    cache_evict(id);

  cache[id].sector = sector;
  cache[id].valid = true;
  cache[id].dirty = false;
  cache[id].accessed = false;
  block_read(sector, cache[id].data);
}

static int cache_hit(block_sector_t sector) {
  for (int i = 0; i < MAX_CACHE; i++)
    if (cache[i].valid && cache[i].sector == sector)
      return i;
  return -1;
}

static int cache_miss(block_sector_t sector) {
  for (int i = 0; i < MAX_CACHE; i++) {
    if (!cache[bpointer].valid || !cache[bpointer].accessed) {
      cache_insert(sector, bpointer);
      return bpointer;
    }
    else {
      cache[bpointer].accessed = false;
      bpointer = (bpointer + 1) % MAX_CACHE;
    }
  }
  cache_insert(sector, bpointer);
  return bpointer;
}

static int access_buffer(block_sector_t sector) {
  int cache_id = -1;
  if ((cache_id = cache_hit(sector)) == -1) {
    printf("(miss)\n");
    miss_num++;
    cache_id = cache_miss(sector);
  } else {
    hit_num++;
    printf("(hit)\n");
  }
  if (cache_id != -1)
    cache[cache_id].accessed = true;

  return cache_id;
}

void block_read_b(block_sector_t sector, void* buffer) {
  printf("Read Page #%u ", sector);
  int cache_id = access_buffer(sector);
  if (buffer != NULL)
    memcpy(buffer, cache[cache_id].data, BLOCK_SECTOR_SIZE);
}

void block_write_b(block_sector_t sector, void* buffer) {
  printf("Write Page #%u ", sector);
  int cache_id = access_buffer(sector);
  if (buffer != NULL)
    memcpy(cache[cache_id].data, buffer, BLOCK_SECTOR_SIZE);
  cache[cache_id].dirty = true;
}

void write_back_all() {
  for (int i = 0; i < MAX_CACHE; i++)
    if (cache[i].valid)
      cache_evict(i);
}

void show_cache() {
    printf("[ ");
    for (int i = 0; i < MAX_CACHE; i++) {
        if (cache[i].valid) {
            printf("#%-2d ", cache[i].sector);
        } else {
            printf(" __ ");
        }
    }
    printf("]\nPage fault rate = %.2lf%%\n\n", 100.0 * miss_num / (hit_num + miss_num));
}
