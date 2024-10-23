#ifndef LIBLAMADA_HEADER_PRELUDE_H
#define LIBLAMADA_HEADER_PRELUDE_H

#include <stdint.h>
#include <stddef.h>

#ifndef LamadaExport
#define LamadaExport [[gnu::visibility("default")]] [[gnu::unused]]
#endif

#ifndef LamadaDeprecated
#define LamadaDeprecated(n) [[deprecated("n")]]
#endif

#define usize_t size_t

#define mask8 uint8_t
#define mask16 uint16_t
#define mask32 uint32_t
#define mask64 uint64_t
#define BOOL uint8_t
#define TRUE 1
#define FALSE 0

#define PTR void*

#define MASKED(a,b) ((a & b) != 0)
#define UNITYPE(a) ((void*) a)
#define DATACLS(a,b) {memset(&b, 0, sizeof(a))}


#endif
