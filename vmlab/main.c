#include <stdlib.h>
#include "clock.h"

#define MAX_PAGE 15
#define SEED 0x1f82b3a3

int main() {
    srand(SEED);
    for (int i = 0; i < 299; i++) {
        if (rand() % 2) {
            block_read_b(rand() % MAX_PAGE, NULL);
        } else {
            block_write_b(rand() % MAX_PAGE, NULL);
        }
        show_cache();
    }
    return 0;
}
