// ==============================================================================
// zen/src/zen_dict_runtime.c — Runtime functions for Zen dictionaries.
//
// zen_dict_t is a simple hash map: { keys[], values[], count, capacity }
// All keys and values are i8* (strings). Numbers are boxed as strings.
// ==============================================================================
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    char** keys;
    char** values;
    long  count;
    long  capacity;
} zen_dict_t;

// Create a new dict (called from malloc'd 64 bytes, need to init)
static zen_dict_t* zen_dict_init(void* raw) {
    zen_dict_t* d = (zen_dict_t*)raw;
    if (!d) return NULL;
    d->keys = NULL;
    d->values = NULL;
    d->count = 0;
    d->capacity = 0;
    return d;
}

void __zen_dict_set(void* raw, const char* key, const char* value) {
    zen_dict_t* d = zen_dict_init(raw);
    if (!d || !key || !value) return;

    // Check if key already exists
    for (long i = 0; i < d->count; i++) {
        if (d->keys[i] && strcmp(d->keys[i], key) == 0) {
            // Update value
            free(d->values[i]);
            d->values[i] = strdup(value);
            return;
        }
    }

    // Grow if needed
    if (d->count >= d->capacity) {
        d->capacity = d->capacity ? d->capacity * 2 : 8;
        d->keys = (char**)realloc(d->keys, d->capacity * sizeof(char*));
        d->values = (char**)realloc(d->values, d->capacity * sizeof(char*));
    }

    d->keys[d->count] = strdup(key);
    d->values[d->count] = strdup(value);
    d->count++;
}

const char* __zen_dict_get(void* raw, const char* key) {
    zen_dict_t* d = zen_dict_init(raw);
    if (!d || !key) return "";
    for (long i = 0; i < d->count; i++) {
        if (d->keys[i] && strcmp(d->keys[i], key) == 0) {
            return d->values[i] ? d->values[i] : "";
        }
    }
    return "";
}

int __zen_dict_has(void* raw, const char* key) {
    zen_dict_t* d = zen_dict_init(raw);
    if (!d || !key) return 0;
    for (long i = 0; i < d->count; i++) {
        if (d->keys[i] && strcmp(d->keys[i], key) == 0) return 1;
    }
    return 0;
}

long __zen_dict_size(void* raw) {
    zen_dict_t* d = zen_dict_init(raw);
    return d ? d->count : 0;
}

void __zen_dict_remove(void* raw, const char* key) {
    zen_dict_t* d = zen_dict_init(raw);
    if (!d || !key) return;
    for (long i = 0; i < d->count; i++) {
        if (d->keys[i] && strcmp(d->keys[i], key) == 0) {
            free(d->keys[i]);
            free(d->values[i]);
            // Shift remaining
            for (long j = i; j < d->count - 1; j++) {
                d->keys[j] = d->keys[j + 1];
                d->values[j] = d->values[j + 1];
            }
            d->count--;
            return;
        }
    }
}

// Returns keys as comma-separated string
const char* __zen_dict_keys(void* raw) {
    zen_dict_t* d = zen_dict_init(raw);
    if (!d || d->count == 0) return "";
    // Build comma-separated string
    static char buf[4096];
    buf[0] = 0;
    for (long i = 0; i < d->count; i++) {
        if (i > 0) strcat(buf, ",");
        if (d->keys[i]) strcat(buf, d->keys[i]);
    }
    return buf;
}

// Returns values as comma-separated string
const char* __zen_dict_values(void* raw) {
    zen_dict_t* d = zen_dict_init(raw);
    if (!d || d->count == 0) return "";
    static char buf[4096];
    buf[0] = 0;
    for (long i = 0; i < d->count; i++) {
        if (i > 0) strcat(buf, ",");
        if (d->values[i]) strcat(buf, d->values[i]);
    }
    return buf;
}
