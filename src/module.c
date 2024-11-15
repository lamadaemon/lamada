#include "module.h"
#include "mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h> 

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

    lamada_release(UNITYPE(&original));
    return new;
}

struct mmap_entry {
    size_t start;
    size_t end;
    mask8 perms;
    size_t offset;
    uint8_t dev_major;
    uint8_t dev_minor;
    uint32_t inode;
    char* path;
};

static struct mmap_entry parse_map_entry(char* line) {
    char* buff;
    struct mmap_entry entry;

    lamada_clean(&entry, sizeof(struct mmap_entry));
    
    buff = strtok(line, "-");
    entry.start = strtol(buff, NULL, 16);

    buff = strtok(NULL, " ");
    entry.end = strtol(buff, NULL, 16);
    
    buff = strtok(NULL, " ");
    
    for (uint8_t i = 0; i < 4; i ++) {
        if (buff[i] != '-') {
            entry.perms |= (1 << (i));
        }
    }

    buff = strtok(NULL, " ");
    entry.offset = strtol(buff, NULL, 16);
    
    buff = strtok(NULL, ":");
    entry.dev_major = strtol(buff, NULL, 16);
    
    buff = strtok(NULL, " ");
    entry.dev_minor = strtol(buff, NULL, 16);
    
    buff = strtok(NULL, " ");
    entry.inode = strtol(buff, NULL, 16);
    
    buff = strtok(NULL, "\n");
    entry.path = trim_string(strdup(buff));

    return entry;
}

static void release_map_entry(struct mmap_entry* entry) {
    free(entry->path);
}

runtime_module_t* lamada_lookup_module(char* name, uint8_t options) {

    // check conflict options
    if (MASKED(options, MODLOOKUP_COMBINED) && MASKED(options, MODLOOKUP_FAILONMULTIPLE)) {
        return NULL;
    }

    if (!name) {
        return NULL;
    }

    FILE* maps_handle = fopen("/proc/self/maps", "r");
    if (!maps_handle) {
        return NULL;
    }

    char* line = NULL;
    size_t size = 0;
    ssize_t len = 0;

    runtime_module_t* result = lamada_allocate(sizeof(runtime_module_t));

    BOOL chunk_started = FALSE;
    while ((len = getline(&line, &size, maps_handle)) != -1) {
        char* occur_ptr;

        if ((occur_ptr = strstr(line, name)) != NULL) {    
            if (MASKED(options, MODLOOKUP_MATCHFULL) && occur_ptr != line) {
                goto match_failed;
            }
            struct mmap_entry entry = parse_map_entry(line);
 
            if (MASKED(options, MODLOOKUP_XONLY) && !MASKED(entry.perms, MODPERMS_EXECUTE)) {
                release_map_entry(&entry);
                continue; 
            }
 

            if (MASKED(options, MODLOOKUP_COMBINED)) {
                chunk_started = TRUE;

                if (!result->base) {
                    result->base = (PTR) entry.start;
                    result->full_name = entry.path;
                    result->name = name;
                } else {
                    release_map_entry(&entry);
                }


                // its ok to access remaining data because release will only release path
                result->size += entry.end - entry.start;
            } else {
               if (MASKED(options, MODLOOKUP_FAILONMULTIPLE) && result->base) {
                    release_map_entry(&entry);
                    lamada_release(UNITYPE(&result));

                    goto finalize;
               }

               result->base = (PTR) entry.start;
               result->full_name = entry.path;
               result->name = name;
            }
       
        } else {
match_failed:
            if (chunk_started) {
                break;
            }
        }
    }

    if (!result->base) {
        lamada_release(UNITYPE(&result));
    } else {
        PTR handle = dlopen(result->full_name, RTLD_NOLOAD);
        if (!handle) {
            handle = dlopen(result->full_name, RTLD_LOCAL); 
        }

        result->dl_handle = handle;
    }

finalize:
    lamada_release(UNITYPE(&line));
    fclose(maps_handle);

    return result;
}

void lamada_release_module(runtime_module_t* mod) {
    free(mod->full_name);
    free(mod);
}

struct pattern_entry {
    BOOL generic;
    uint8_t remaining;
    uint8_t byte;
};

struct pattern_entry* compile_pattern(char* pattern) {
    uint8_t len = (strlen(pattern) + 1) / 3;
    struct pattern_entry* result = lamada_allocate(sizeof(struct pattern_entry) * len);
    char* in = strdup(pattern);
    char* byte = strtok(in, " ");

    
    uint8_t curr = 0;
    while (curr < len) {
        if (byte[0] == '?' || byte[0] == '*') {
            result[curr].generic = TRUE;
        } else { 
            result[curr].byte = strtol(byte, NULL, 16);
        }

        result[curr].remaining = len - curr;
        curr ++;
        byte = strtok(NULL, " ");
    }

    return result;
}

PTR lamada_lookup_symbol(runtime_module_t* mod, char* data, uint8_t options) {
    if (MASKED(options, SYMLOOKUP_BYEXPORTNAME)) {
        return dlsym(mod->dl_handle, data);
    }

    if (MASKED(options, SYMLOOKUP_BYPATTERN)) {
        // strstr without length may cause problems 
        if (MASKED(options, SYMLOOKUP_STRPATTERN)) {
            struct pattern_entry* pattern = compile_pattern(data);
            uint8_t pattern_size = pattern[0].remaining;

            size_t i = 0;

            for (i = 0; i < mod->size; i++) {
                if (mod->size - i < pattern_size) {
                    break;
                }

                for (size_t j = 0; j < pattern_size; j++) {

                    if (!pattern[j].generic && pattern[j].byte != ((uint8_t*) mod->base)[i + j]) {
                        goto loop2_end;
                    }
                }

                // matched
                
                free(pattern);
                return (PTR)((size_t) (mod->base) + i);

loop2_end:
                continue;
            }

            free(pattern);
        } else {
            uint8_t pattern_size = *((uint8_t*) data);

            for (size_t i = 0; i < mod->size; i++) {
                if (data[1] == ((int8_t*) mod->base)[i]) {
                    // Loop starts from 1 since the first byte is already checked
                    for (size_t j = 1; j < pattern_size; j++) {
                        // data starts from 1, since the first byte is length
                        if (data[j + 1] != ((int8_t*) mod->base)[i + j]) {
                            goto loop1_end;
                        }
                    }

                    // matched
                    
                    return (PTR)((size_t) (mod->base) + i);
                }

loop1_end:
                continue;
            }
        }
    }

    return NULL;
}

