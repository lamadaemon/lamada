#ifndef LIBLAMADA_MEMORY_HEADER_MEM_H
#define LIBLAMADA_MEMORY_HEADER_MEM_H

#include "prelude.h"

/**
 * Allocate a clean buffer on heap.
 */
void* lamada_allocate(size_t len);

#endif
