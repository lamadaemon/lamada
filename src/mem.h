#ifndef LIBLAMADA_MEMORY_HEADER_MEM_H
#define LIBLAMADA_MEMORY_HEADER_MEM_H

#include "prelude.h"

/**
 * Allocate a clean buffer on heap.
 */
PTR lamada_allocate(size_t len);

/**
 * Release a buffer on heap and set the variable to NULL.
 */
void lamada_release(PTR* ptr);

/**
 * Shortcut for memset(ptr, 0, len);
 */
void lamada_clean(PTR ptr, size_t len);

#endif
