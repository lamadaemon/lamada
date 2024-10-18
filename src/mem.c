#include "mem.h"
#include <string.h>
#include <stdlib.h>

void* lamada_allocate(size_t size) {
    void* buff = malloc(size);
    memset(buff, 0, size);

    return buff;
}
