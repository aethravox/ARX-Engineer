// ==============================================================================
// zen/src/zen_dict_c.h — Runtime de diccionarios (header-only, incluido en codegen.cpp)
// ==============================================================================
#pragma once
#include <stdlib.h>
#include <string.h>

typedef struct {
    char** keys;
    char** values;
    long  count;
    long  capacity;
} zen_dict_t;

static inline void* __zen_dict_create(void) {
    zen_dict_t* d = (zen_dict_t*)calloc(1, sizeof(zen_dict_t));
    d->capacity = 8;
    d->keys = (char**)calloc(8, sizeof(char*));
    d->values = (char**)calloc(8, sizeof(char*));
    d->count = 0;
    return d;
}

static inline void __zen_dict_set(void* raw, const char* key, const char* value) {
    zen_dict_t* d = (zen_dict_t*)raw;
    if (!d || !key || !value) return;
    for (long i = 0; i < d->count; i++) {
        if (d->keys[i] && strcmp(d->keys[i], key) == 0) {
            free(d->values[i]);
            d->values[i] = strdup(value);
            return;
        }
    }
    if (d->count >= d->capacity) {
        d->capacity *= 2;
        d->keys = (char**)realloc(d->keys, d->capacity * sizeof(char*));
        d->values = (char**)realloc(d->values, d->capacity * sizeof(char*));
    }
    d->keys[d->count] = strdup(key);
    d->values[d->count] = strdup(value);
    d->count++;
}

static inline const char* __zen_dict_get(void* raw, const char* key) {
    zen_dict_t* d = (zen_dict_t*)raw;
    if (!d || !key) return "";
    for (long i = 0; i < d->count; i++) {
        if (d->keys[i] && strcmp(d->keys[i], key) == 0)
            return d->values[i] ? d->values[i] : "";
    }
    return "";
}

static inline int __zen_dict_has(void* raw, const char* key) {
    zen_dict_t* d = (zen_dict_t*)raw;
    if (!d || !key) return 0;
    for (long i = 0; i < d->count; i++) {
        if (d->keys[i] && strcmp(d->keys[i], key) == 0) return 1;
    }
    return 0;
}

static inline long __zen_dict_size(void* raw) {
    zen_dict_t* d = (zen_dict_t*)raw;
    return d ? d->count : 0;
}

static inline void __zen_dict_remove(void* raw, const char* key) {
    zen_dict_t* d = (zen_dict_t*)raw;
    if (!d || !key) return;
    for (long i = 0; i < d->count; i++) {
        if (d->keys[i] && strcmp(d->keys[i], key) == 0) {
            free(d->keys[i]);
            free(d->values[i]);
            for (long j = i; j < d->count - 1; j++) {
                d->keys[j] = d->keys[j + 1];
                d->values[j] = d->values[j + 1];
            }
            d->count--;
            return;
        }
    }
}

static inline const char* __zen_dict_keys(void* raw) {
    zen_dict_t* d = (zen_dict_t*)raw;
    if (!d || d->count == 0) return "";
    static char buf[4096];
    buf[0] = 0;
    for (long i = 0; i < d->count; i++) {
        if (i > 0) strcat(buf, ",");
        if (d->keys[i]) strcat(buf, d->keys[i]);
    }
    return buf;
}

static inline const char* __zen_dict_values(void* raw) {
    zen_dict_t* d = (zen_dict_t*)raw;
    if (!d || d->count == 0) return "";
    static char buf[4096];
    buf[0] = 0;
    for (long i = 0; i < d->count; i++) {
        if (i > 0) strcat(buf, ",");
        if (d->values[i]) strcat(buf, d->values[i]);
    }
    return buf;
}
