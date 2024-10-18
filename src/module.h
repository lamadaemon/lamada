#ifndef LIBLAMADA_MEMORY_HEADER_MODULE_H
#define LIBLAMADA_MEMORY_HEADER_MODULE_H

#include "prelude.h"

struct runtime_module {
    char* name;
    char* full_name;
    void* base;
    usize_t size;

    void* dl_handle;
};


typedef struct runtime_module runtime_module_t;

#define MODLOOKUP_MATCHFIRST (1 << 1)
#define MODLOOKUP_FAILONMULTIPLE 2 (1 << 2)
#define MODLOOKUP_MATCHFULL (1 << 3)
#define MODLOOKUP_COMBINED (1 << 4)

runtime_module_t* lamada_lookup_module(char* name, mask8 options);
runtime_module_t* lamada_enumerate_modules();

#define SYMLOOKUP_MATCHFIRST (1 << 1)
#define SYMLOOKUP_FAILONMULTIPLE (1 << 2)
#define SYMLOOKUP_BYNAME (1 << 3)
#define SYMLOOKUP_BYPATTERN (1 << 4)
#define SYMLOOKUP_BYLEADINGBYTES (1 << 5)
#define SYMLOOKUP_PRESERVED1 (1 << 6)
#define SYMLOOKUP_PRESERVED2 (1 << 7)

/**
 * Lookup a symbol from a runtime_module.
 * NULL will be returned if fail.
 */
void* lamada_lookup_symbol(runtime_module_t* mod, char* data, mask8 options);

#endif
