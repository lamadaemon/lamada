#include "mem.h"
#include "prelude.h"
#include <string.h>
#include <stdlib.h>

PTR lamada_allocate(size_t size) {
    PTR buff = malloc(size);
    memset(buff, 0, size);

    return buff;
}

void lamada_free(PTR* target) {
    if (*target) {
        free(*target);
        (*target) = NULL;
    }
}
