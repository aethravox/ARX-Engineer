// arx/format/tar.h — Parser simplificado de TAR (POSIX ustar).
//
// Disenado para leer el assets block dentro de un .aex.
// Soporta:
//   - Regular files (typeflag '0' o '\0')
//   - Nombres hasta 100 chars (sin prefijo)
//   - Sizes en octal (hasta 11 digitos)
//   - Chequeo de checksum (opcional, solo warning si falla)
//
// No soporta (no lo necesitamos para .aex):
//   - Long names (prefix + name > 100)
//   - Symlinks, directories, devices
//   - GNU extensions
//   - pax extended headers

#ifndef ARX_FORMAT_TAR_H
#define ARX_FORMAT_TAR_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const uint8_t* data;     // bloque TAR completo
    size_t         size;     // tamano total del bloque
    size_t         cursor;   // posicion actual (offset al proximo header)
} TarReader;

typedef struct {
    char        name[101];   // nombre del archivo (null-terminated)
    size_t      size;        // tamano del archivo en bytes
    const uint8_t* data;    // puntero al contenido (dentro del bloque TAR)
    char        typeflag;    // '0' o '\0' = regular file
    int         valid;       // 1 si la entrada es valida, 0 si EOF o invalida
} TarEntry;

// Inicializa un reader sobre un bloque TAR.
void tar_reader_init(TarReader* r, const uint8_t* data, size_t size);

// Lee la siguiente entrada. Retorna 1 si hay entrada, 0 si EOF.
// Si retorna 1, *out_entry queda lleno (data apunta dentro del bloque TAR).
int tar_reader_next(TarReader* r, TarEntry* out_entry);

// Busca un archivo por nombre (path relativo).
// Si lo encuentra, retorna 1 y llena *out_entry.
// Si no, retorna 0.
int tar_reader_find(TarReader* r, const char* path, TarEntry* out_entry);

#ifdef __cplusplus
}
#endif

#endif // ARX_FORMAT_TAR_H
