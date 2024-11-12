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

#define MODLOOKUP_PRESERVED1 (1 << 0)
#define MODLOOKUP_PRESERVED2 (1 << 1)
#define MODLOOKUP_FAILONMULTIPLE (1 << 2)
#define MODLOOKUP_MATCHFULL (1 << 3)
#define MODLOOKUP_COMBINED (1 << 4)
#define MODLOOKUP_XONLY (1 << 5)
#define MODLOOKUP_PRESERVED3 (1 << 6)
#define MODLOOKUP_PRESERVED4 (1 << 7)

#define MODPERMS_READ (1 << 0)
#define MODPERMS_WRITE (1 << 1)
#define MODPERMS_EXECUTE (1 << 2)
#define MODPERMS_PRIVATE (1 << 3)

/**
 * Lookup information of a specified loaded module.
 * NULL will be returned if fails.
 */
runtime_module_t* lamada_lookup_module(char* name, mask8 options);

/**
 * Cleanup runtime_module_t after use.
 */
void lamada_release_module(runtime_module_t* mod);

#define SYMLOOKUP_PRESERVED1 (1 << 0)
#define SYMLOOKUP_PRESERVED2 (1 << 1)
#define SYMLOOKUP_FAILONMULTIPLE (1 << 2)
#define SYMLOOKUP_BYNAME (1 << 3)
#define SYMLOOKUP_BYPATTERN (1 << 4)
#define SYMLOOKUP_BYEXPORTNAME (1 << 5)
#define SYMLOOKUP_STRPATTERN (1 << 6)
#define SYMLOOKUP_PRESERVED4 (1 << 7)

/**
 * Lookup a symbol from a runtime_module.
 * NULL will be returned if fails.
 */
void* lamada_lookup_symbol(runtime_module_t* mod, char* data, mask8 options);

#endif
