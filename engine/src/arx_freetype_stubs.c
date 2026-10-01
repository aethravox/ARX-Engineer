/* arx_freetype_stubs.c - Stubs para funciones de FreeType que no se compilan
   porque ftgzip.c, ftlzw.c y ftzopen.c estan deshabilitados.
   NO definir renderer_classes (ya existen en ftsdfrend.c, ftbsdf.c, etc.) */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FT_StreamRec_* FT_Stream;
typedef int FT_Error;
typedef unsigned int FT_UInt;
typedef unsigned long FT_ULong;
typedef unsigned char FT_Byte;

/* Stub: FT_Stream_OpenGzip - normalmente en ftgzip.c (deshabilitado) */
FT_Error FT_Stream_OpenGzip(FT_Stream stream, FT_Stream source) {
    (void)stream; (void)source;
    return -1;
}

/* Stub: FT_Stream_OpenLZW - normalmente en ftlzw.c (deshabilitado) */
FT_Error FT_Stream_OpenLZW(FT_Stream stream, FT_Stream source) {
    (void)stream; (void)source;
    return -1;
}

/* Stub: FT_Gzip_Uncompress - normalmente en ftgzip.c (deshabilitado) */
FT_Error FT_Gzip_Uncompress(FT_Byte* dest, FT_ULong* dest_len,
                             const FT_Byte* source, FT_ULong source_len) {
    (void)dest; (void)dest_len; (void)source; (void)source_len;
    return -1;
}

#ifdef __cplusplus
}
#endif
