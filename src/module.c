#include "module.h"
#include "mem.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static char* trim_string(char* str) {
    char* original = str;
    size_t last_white = 0;
    size_t len = strlen(str);

    while ((*str == ' ' || *str == '\t') && last_white < len) {
        last_white ++;
        str ++;
    }

    size_t buffer_size = len - last_white + 1;
    char* new = (char*) lamada_allocate(buffer_size);
    
    memcpy(new, str, len - last_white);
    new[buffer_size - 1] = '\0';

    free(original);
    return new;
}

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
    char* buff;
    struct mmap_entry entry;
    
    buff = strtok(line, "-");
    printf("Comps: %s, ", buff);
    entry.start = strtol(buff, NULL, 16);

    buff = strtok(NULL, " ");
    printf("%s, ", buff);
    entry.end = strtol(buff, NULL, 16);
    
    buff = strtok(NULL, " ");
    printf("%s, ", buff);
    entry.perms = 0;

    buff = strtok(NULL, " ");
    printf("%s, ", buff);
    entry.offset = strtol(buff, NULL, 16);
    
    buff = strtok(NULL, ":");
    printf("%s, ", buff);
    entry.dev_major = strtol(buff, NULL, 16);
    
    buff = strtok(NULL, " ");
    printf("%s, ", buff);
    entry.dev_minor = strtol(buff, NULL, 16);
    
    buff = strtok(NULL, " ");
    printf("%s, ", buff);
    entry.inode = strtol(buff, NULL, 16);
    
    buff = strtok(NULL, "\n");
    printf("%s \n", buff);
    entry.path = trim_string(strdup(buff));

    return entry;
}

static void release_map_entry(struct mmap_entry* entry) {
    free(entry->path);
}

runtime_module_t* lamada_lookup_module(char *name, uint8_t options) {
    FILE* maps_handle = fopen("/proc/self/maps", "r");
    if (!maps_handle) {
        return NULL;
    }

    char* line = NULL;
    size_t size = 0;
    ssize_t len = 0;

    runtime_module_t* result = lamada_allocate(sizeof(runtime_module_t));   

    BOOL chunk_started;
    while ((len = getline(&line, &size, maps_handle)) != -1) {
        char* occur_ptr;
        if ((occur_ptr = strstr(line, name)) != NULL) {
            struct mmap_entry entry = parse_map_entry(line);
            
            if (!result->base) {
                result->base = (void*) entry.start;
                result->full_name = entry.path;
                result->name = name;
            } else {
                release_map_entry(&entry);
            }

            result->size += entry.end - entry.start;
       
        } else {
            if (chunk_started) {
                break;
            }
        }
    }

    free(line);
    fclose(maps_handle);

    return result;
}

int main() {
    runtime_module_t* module = lamada_lookup_module("libc.so", 0);
    printf("path %s, base %p, size %llu\n", module->full_name, module->base, module->size);

    free(module);
    return 0;

}

runtime_module_t* lamada_enumerate_modules() {

}

void* lamada_lookup_symbol(runtime_module_t *mod, char *data, uint8_t options) {
}
