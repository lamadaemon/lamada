#include "mem.h"
#include <string.h>
#include <stdlib.h>

PTR lamada_allocate(size_t size) {
    PTR buff = malloc(size);
    memset(buff, 0, size);

    return buff;
}

void lamada_release(PTR* target) {
    if (*target) {
        free(*target);
        (*target) = NULL;
    }
}

void lamada_clean(PTR ptr, size_t len) {
    memset(ptr, 0, len);
}
