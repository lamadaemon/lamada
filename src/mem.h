#ifndef LIBLAMADA_MEMORY_HEADER_MEM_H
#define LIBLAMADA_MEMORY_HEADER_MEM_H

#include "prelude.h"

/**
 * Allocate a clean buffer on heap.
 */
PTR lamada_allocate(size_t len);
void lamada_release(PTR* ptr);
void lamada_clean(PTR ptr, size_t len);

#endif
