#include "module.h"
#include "mem.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct mmap_entry {
    size_t start;
    size_t end;
    mask8 perms; // NOT YET IMPL
    size_t offset;
    uint8_t dev_major;
    uint8_t dev_minor;
    uint32_t inode;
    char* path;
};

static struct mmap_entry parse_map_entry(char* line) {
    char* begin;
    char* end;
    char* perms;
    char* offset;
    char* device_maj;
    char* device_min;
    char* inode;
    char* path;

    begin = strtok(line, "-");
    end = strtok(NULL, " ");
    perms = strtok(NULL, " ");
    offset = strtok(NULL, " ");
    device_maj = strtok(NULL, ":");
    device_min = strtok(NULL, " ");
    inode = strtok(NULL, " ");
    path = strtok(NULL, " ");

    struct mmap_entry entry;

    entry.start = strtol(begin, NULL, 16);
    entry.end = strtol(end, NULL, 16);
    // TODO Parse perms 
    entry.offset = strtol(offset, NULL, 10);
    entry.dev_minor = strtol(device_min, NULL, 16);
    entry.dev_major = strtol(device_maj, NULL, 16);
    entry.inode = strtol(inode, NULL, 16);
    entry.path = path;

    free(begin);
    free(end);
    free(perms);
    free(offset);
    free(device_maj);
    free(device_min);
    free(inode);

    return entry;
}

runtime_module_t* lamada_lookup_module(char *name, uint8_t options) {
    FILE* maps_handle = fopen("/proc/self/maps", "r");
    if (!maps_handle) {
        return NULL;
    }

    char* line;
    size_t size;
    ssize_t len;

    runtime_module_t* result = lamada_allocate(sizeof(runtime_module_t));   

    BOOL chunk_started;
    while ((len = getline(&line, &size, maps_handle)) != -1) {
        char* occur_ptr;
        if ((occur_ptr = strstr(line, name)) != NULL) {
            printf("%llu nq %u = %u Matched from: %s", occur_ptr, NULL, occur_ptr != NULL, line);
            struct mmap_entry entry = parse_map_entry(line);

            if (result->base) {
                result->base = (void*) entry.start;
                result->full_name = entry.path;
                result->name = name;
            }

            result->size += entry.end - entry.start;
       
            free(line);
        } else {
            free(line);

            if (chunk_started) {
                break;
            }
        }
    }

    return result;
}

int main() {
    runtime_module_t* module = lamada_lookup_module("libc.so", 0);
    printf("path %s, base %llu\n", module->full_name, module->base);

    free(module);
    return 0;

}

runtime_module_t* lamada_enumerate_modules() {

}

void* lamada_lookup_symbol(runtime_module_t *mod, char *data, uint8_t options) {
}
