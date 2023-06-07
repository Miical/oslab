#include "memlib.h"
#include "mm.h"

#define MAXN 10

void *p[MAXN];
const int sz[MAXN * 2] = {
    44, 2, 8, 11, 33, 51, 23, 2, 7, 200,
    39, 900, 30, 123, 48, 39, 600, 345, 123, 4
};

int main() {
    printf("%u\n", sizeof(void *));
    mem_init();
    mm_init();

    for (int i = 0; i < MAXN; i++) {
        p[i] = mm_malloc(sz[i]);
    }
    for (int i = 0; i < MAXN; i++) {
        mm_free(p[i]);
    }

    for (int i = 0; i < MAXN; i++) {
        p[i] = mm_malloc(sz[i + MAXN]);
    }
    for (int i = 0; i < MAXN; i++) {
        mm_free(p[i]);
    }
    return 0;
}
