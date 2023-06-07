#include "memlib.h"
#include "mm.h"

#define MAXN 30

void *p[MAXN];
const int sz[MAXN] = {
    44, 2, 8, 11, 33, 51, 23, 2, 7, 200,
    39, 900, 30, 123, 48, 39, 600, 345, 123, 4,
    1000, 2000, 3212, 222, 4212, 123, 900, 799, 124, 5999
};

int main() {
    mem_init();
    mm_init();

    int total_sz = 0;
    for (int i = 0; i < MAXN; i++) {
        total_sz += sz[i];
        p[i] = mm_malloc(sz[i]);
    }

    for (int i = 0; i < MAXN; i++) {
        mm_free(p[i]);
    }

    printf("total space = %d, heapsize = %d, utilization = %.2lf%%\n",
        total_sz,
        mem_heapsize(),
        1.0 * total_sz / mem_heapsize() * 100);

    return 0;
}
