#include "lamada/module.h"
#include <stdlib.h>
#include <stdio.h>

int main() {
    runtime_module_t* module = lamada_lookup_module("libc.so", MODLOOKUP_COMBINED | MODLOOKUP_XONLY);
    if (module) {
        printf("call lamada_lookup_module(MODLOOKUP_XONLY | MODLOOKUP_COMBINED): path %s, base %p, size %lu\n", module->full_name, module->base, module->size);
    } else {
        printf("FAIL (NOT FOUND OR ERROR)\n");
    }

    void* fopen_fn3 = lamada_lookup_symbol(module, "fopen", SYMLOOKUP_BYEXPORTNAME);
    runtime_module_t* libasan = lamada_lookup_module("libasan.so", MODLOOKUP_COMBINED | MODLOOKUP_XONLY);
    void* fopen_fn = lamada_lookup_symbol(libasan, "f3 0f 1e fa 48 8d 05 4d 16 10 00 80 38 00 74 10 ff 25 62 e3 0e 00 ?? 2e 0f", SYMLOOKUP_BYPATTERN | SYMLOOKUP_STRPATTERN);


    FILE* file = ((FILE* (*)(char*, char*)) fopen_fn)("/proc/self/maps", "r");

    char* line = NULL;
    size_t size = 0;
    ssize_t len = 0;

    while ((len = getline(&line, &size, file)) != -1) {
        printf("%s", line);
    }

    fclose(file);
    free(line);

    printf("by exportname: %p, offset from base %lu (size = %lu), by strpattern: %p\n", fopen_fn3, ((uint64_t) fopen_fn) - ((uint64_t) libasan->base), module->size, fopen_fn);
    
    free(libasan->full_name);
    free(libasan);
    free(module->full_name);
    free(module);
    return 0;

}
