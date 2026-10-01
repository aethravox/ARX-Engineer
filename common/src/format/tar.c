// arx/format/tar.c — Implementacion del parser TAR.

#include "arx/format/tar.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Lee un campo octal de N bytes (null o space terminated).
static size_t parse_octal(const char* s, size_t n) {
    size_t v = 0;
    for (size_t i = 0; i < n; i++) {
        char c = s[i];
        if (c == 0 || c == ' ') break;
        if (c < '0' || c > '7') continue;  // ignorar no-octal
        v = (v << 3) | (size_t)(c - '0');
    }
    return v;
}

void tar_reader_init(TarReader* r, const uint8_t* data, size_t size) {
    r->data = data;
    r->size = size;
    r->cursor = 0;
}

int tar_reader_next(TarReader* r, TarEntry* out_entry) {
    if (!r || !out_entry) return 0;
    if (r->cursor + 512 > r->size) return 0;

    const uint8_t* hdr = r->data + r->cursor;

    // EOF: bloque de puros ceros.
    int all_zero = 1;
    for (int i = 0; i < 512; i++) {
        if (hdr[i] != 0) { all_zero = 0; break; }
    }
    if (all_zero) return 0;

    memset(out_entry, 0, sizeof(*out_entry));

    // name (100 bytes en offset 0)
    memcpy(out_entry->name, hdr, 100);
    out_entry->name[100] = 0;

    // size (12 bytes octal en offset 124)
    char size_field[13];
    memcpy(size_field, hdr + 124, 12);
    size_field[12] = 0;
    out_entry->size = parse_octal(size_field, 12);

    // typeflag (1 byte en offset 156)
    out_entry->typeflag = (char)hdr[156];

    // data empieza despues del header (offset 512)
    out_entry->data = hdr + 512;
    out_entry->valid = 1;

    // Avanzar cursor: header + data padded a 512.
    size_t padded = (out_entry->size + 511) & ~((size_t)511);
    r->cursor += 512 + padded;

    return 1;
}

int tar_reader_find(TarReader* r, const char* path, TarEntry* out_entry) {
    if (!r || !path || !out_entry) return 0;
    TarReader tmp;
    tar_reader_init(&tmp, r->data, r->size);
    while (tar_reader_next(&tmp, out_entry)) {
        if (out_entry->typeflag == '0' || out_entry->typeflag == 0) {
            if (strcmp(out_entry->name, path) == 0) {
                return 1;
            }
        }
    }
    return 0;
}
