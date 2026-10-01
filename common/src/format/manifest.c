// arx/format/manifest.c — Implementacion del parser de manifest (v2 con skip correcto).

#include "arx/format/manifest.h"
#include "arx/format/jsmn.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// ============================================================
// Helpers
// ============================================================

static bool json_equals(const char* json, const jsmntok_t* tok, const char* s) {
    if (tok->type != JSMN_STRING) return false;
    if (tok->end - tok->start != (int)strlen(s)) return false;
    return strncmp(json + tok->start, s, tok->end - tok->start) == 0;
}

static bool json_to_bool(const char* json, const jsmntok_t* tok) {
    if (tok->type != JSMN_PRIMITIVE) return false;
    return json[tok->start] == 't';
}

static void json_to_str(const char* json, const jsmntok_t* tok,
                         char* out, size_t out_size) {
    if (tok->type != JSMN_STRING && tok->type != JSMN_PRIMITIVE) {
        if (out_size > 0) out[0] = 0;
        return;
    }
    int len = tok->end - tok->start;
    if ((size_t)len >= out_size) len = (int)out_size - 1;
    memcpy(out, json + tok->start, len);
    out[len] = 0;
}

static int json_to_int(const char* json, const jsmntok_t* tok) {
    if (tok->type != JSMN_PRIMITIVE) return 0;
    char buf[32];
    int len = tok->end - tok->start;
    if (len > 31) len = 31;
    memcpy(buf, json + tok->start, len);
    buf[len] = 0;
    return atoi(buf);
}

static int64_t json_to_int64(const char* json, const jsmntok_t* tok) {
    if (tok->type != JSMN_PRIMITIVE) return 0;
    char buf[32];
    int len = tok->end - tok->start;
    if (len > 31) len = 31;
    memcpy(buf, json + tok->start, len);
    buf[len] = 0;
    return (int64_t)strtoll(buf, NULL, 10);
}

// Cuenta cuantos tokens ocupa un valor (incluyendo el mismo y todos sus hijos recursivamente).
// Esto es necesario porque jsmn no da un "next sibling" directo; tenemos que saltar
// todos los tokens del sub-arbol.
static int count_value_size(const jsmntok_t* tok) {
    if (!tok) return 0;
    if (tok->type == JSMN_OBJECT || tok->type == JSMN_ARRAY) {
        // Un objeto/array de size N tiene N hijos (cada uno es un valor completo).
        // Pero los keys tambien son tokens (en JSMN_OBJECT, los hijos vienen en pares key/value).
        // Para OBJECT, size = numero de pares; cada par = 1 token key + count_value_size(value).
        // Para ARRAY, size = numero de elementos; cada elemento = count_value_size(element).
        int total = 1;  // este token
        const jsmntok_t* child = tok + 1;
        if (tok->type == JSMN_OBJECT) {
            for (int i = 0; i < tok->size; i++) {
                total += 1;  // key
                total += count_value_size(child + 1);  // value
                child = child + 1 + count_value_size(child + 1);
                // ajustar child al siguiente key
                // hmm esto es recursivo, mejor hacerlo bien
            }
        } else {  // JSMN_ARRAY
            for (int i = 0; i < tok->size; i++) {
                int sub = count_value_size(child);
                total += sub;
                child += sub;
            }
        }
        return total;
    }
    // STRING o PRIMITIVE: solo 1 token
    return 1;
}

// Version mas simple y robusta de count_value_size:
// Como jsmn genera tokens en orden DFS (pre-order), el sub-arbol de un token
// es: el token mismo + todos los tokens consecutivos cuyos rangos [start,end]
// estan dentro del rango del token padre.
static int skip_subtree(const jsmntok_t* tokens, int start_idx, int total) {
    if (start_idx >= total) return 0;
    const jsmntok_t* t = &tokens[start_idx];
    int end = t->end;
    int i = start_idx + 1;
    while (i < total && tokens[i].start < end) {
        i++;
    }
    return i - start_idx;
}

// ============================================================
// Parser de la seccion "permissions"
// ============================================================

static void parse_permission_detail(const char* json, const jsmntok_t* perm_obj,
                                       const jsmntok_t* tokens, int total,
                                       ArxPermissionDetail* d) {
    if (perm_obj->type != JSMN_OBJECT) return;

    // Iterar pares key/value del objeto permiso.
    int idx = (int)(perm_obj - tokens) + 1;
    int end_off = perm_obj->end;

    for (int i = 0; i < perm_obj->size; i++) {
        if (idx >= total) break;
        const jsmntok_t* key = &tokens[idx];
        const jsmntok_t* val = &tokens[idx + 1];
        if (key->start >= end_off) break;

        if (json_equals(json, key, "allowed")) {
            d->allowed = json_to_bool(json, val);
        } else if (json_equals(json, key, "scope")) {
            json_to_str(json, val, d->fs_scope, sizeof(d->fs_scope));
        } else if (json_equals(json, key, "read")) {
            d->clipboard_read = json_to_bool(json, val);
        } else if (json_equals(json, key, "write")) {
            d->clipboard_write = json_to_bool(json, val);
        } else if (json_equals(json, key, "endpoints")) {
            if (val->type == JSMN_ARRAY) {
                int arr_idx = (int)(val - tokens) + 1;
                int arr_end = val->end;
                int count = 0;
                while (arr_idx < total && tokens[arr_idx].start < arr_end &&
                       count < 16) {
                    if (tokens[arr_idx].type == JSMN_STRING) {
                        json_to_str(json, &tokens[arr_idx],
                                     d->endpoints[count],
                                     sizeof(d->endpoints[count]));
                        count++;
                    }
                    arr_idx += skip_subtree(tokens, arr_idx, total);
                }
                d->endpoint_count = count;
            }
        }
        // Saltar al siguiente par key/value
        idx += 1 + skip_subtree(tokens, idx + 1, total);
    }
}

// ============================================================
// Parser principal
// ============================================================

bool arx_manifest_parse(ArxManifest* out, const char* json) {
    if (!out || !json) return false;
    memset(out, 0, sizeof(*out));

    jsmn_parser p;
    jsmn_init(&p);

    int needed = jsmn_parse(&p, json, strlen(json), NULL, 0);
    if (needed < 0) return false;
    if (needed > 8192) needed = 8192;

    jsmntok_t* tokens = malloc(sizeof(jsmntok_t) * (size_t)needed);
    if (!tokens) return false;

    jsmn_init(&p);
    int n = jsmn_parse(&p, json, strlen(json), tokens, needed);
    if (n < 0 || tokens[0].type != JSMN_OBJECT) {
        free(tokens);
        return false;
    }

    out->valid = true;

    // Iterar claves del root object (DFS, usando skip_subtree para saltar valores compuestos).
    int idx = 1;
    while (idx < n) {
        const jsmntok_t* key = &tokens[idx];
        const jsmntok_t* val = &tokens[idx + 1];
        if (key->type != JSMN_STRING) break;

        int val_skip = skip_subtree(tokens, idx + 1, n);

        if (json_equals(json, key, "schema")) {
            out->schema = json_to_int(json, val);
        } else if (json_equals(json, key, "package")) {
            // Iterar hijos del objeto package
            int p_idx = idx + 2;
            int p_end = val->end;
            for (int i = 0; i < val->size; i++) {
                if (p_idx >= n || tokens[p_idx].start >= p_end) break;
                const jsmntok_t* pk = &tokens[p_idx];
                const jsmntok_t* pv = &tokens[p_idx + 1];
                if (json_equals(json, pk, "name")) {
                    json_to_str(json, pv, out->package_name, sizeof(out->package_name));
                } else if (json_equals(json, pk, "version")) {
                    json_to_str(json, pv, out->package_version, sizeof(out->package_version));
                }
                p_idx += 1 + skip_subtree(tokens, p_idx + 1, n);
            }
        } else if (json_equals(json, key, "engine")) {
            int p_idx = idx + 2;
            int p_end = val->end;
            for (int i = 0; i < val->size; i++) {
                if (p_idx >= n || tokens[p_idx].start >= p_end) break;
                const jsmntok_t* pk = &tokens[p_idx];
                const jsmntok_t* pv = &tokens[p_idx + 1];
                if (json_equals(json, pk, "min_version")) {
                    json_to_str(json, pv, out->engine_min_version,
                                 sizeof(out->engine_min_version));
                }
                p_idx += 1 + skip_subtree(tokens, p_idx + 1, n);
            }
        } else if (json_equals(json, key, "permissions")) {
            // Iterar pares perm_name/perm_obj
            int p_idx = idx + 2;
            int p_end = val->end;
            for (int i = 0; i < val->size; i++) {
                if (p_idx >= n || tokens[p_idx].start >= p_end) break;
                const jsmntok_t* pk = &tokens[p_idx];
                const jsmntok_t* pv = &tokens[p_idx + 1];

                // Nombre del permiso
                char name_buf[64] = {0};
                json_to_str(json, pk, name_buf, sizeof(name_buf));
                int perm_id = arx_permission_from_name(name_buf);

                if (perm_id >= 0 && pv->type == JSMN_OBJECT) {
                    parse_permission_detail(json, pv, tokens, n,
                                              &out->permissions[perm_id]);
                }
                p_idx += 1 + skip_subtree(tokens, p_idx + 1, n);
            }
        } else if (json_equals(json, key, "assets")) {
            int p_idx = idx + 2;
            int p_end = val->end;
            for (int i = 0; i < val->size; i++) {
                if (p_idx >= n || tokens[p_idx].start >= p_end) break;
                const jsmntok_t* pk = &tokens[p_idx];
                const jsmntok_t* pv = &tokens[p_idx + 1];
                if (json_equals(json, pk, "entry_scene")) {
                    json_to_str(json, pv, out->entry_scene, sizeof(out->entry_scene));
                } else if (json_equals(json, pk, "asset_count")) {
                    out->asset_count = json_to_int(json, pv);
                } else if (json_equals(json, pk, "total_size_uncompressed")) {
                    out->total_size_uncompressed = json_to_int64(json, pv);
                }
                p_idx += 1 + skip_subtree(tokens, p_idx + 1, n);
            }
        } else if (json_equals(json, key, "scripts")) {
            int p_idx = idx + 2;
            int p_end = val->end;
            for (int i = 0; i < val->size; i++) {
                if (p_idx >= n || tokens[p_idx].start >= p_end) break;
                const jsmntok_t* pk = &tokens[p_idx];
                const jsmntok_t* pv = &tokens[p_idx + 1];
                if (json_equals(json, pk, "entry")) {
                    json_to_str(json, pv, out->scripts_entry, sizeof(out->scripts_entry));
                } else if (json_equals(json, pk, "type")) {
                    char buf[64] = {0};
                    json_to_str(json, pv, buf, sizeof(buf));
                    if (strcmp(buf, "luau_bytecode_v3") == 0) out->scripts_type = 1;
                    else if (strcmp(buf, "aot_native") == 0) out->scripts_type = 2;
                    else if (strcmp(buf, "arxscript_cpp_transpiled") == 0 ||
                              strcmp(buf, "arxscript_source") == 0) out->scripts_type = 3;
                } else if (json_equals(json, pk, "aot_compiled")) {
                    out->aot_compiled = json_to_bool(json, pv);
                }
                p_idx += 1 + skip_subtree(tokens, p_idx + 1, n);
            }
        } else if (json_equals(json, key, "metadata")) {
            int p_idx = idx + 2;
            int p_end = val->end;
            for (int i = 0; i < val->size; i++) {
                if (p_idx >= n || tokens[p_idx].start >= p_end) break;
                const jsmntok_t* pk = &tokens[p_idx];
                const jsmntok_t* pv = &tokens[p_idx + 1];
                if (json_equals(json, pk, "title")) {
                    json_to_str(json, pv, out->title, sizeof(out->title));
                } else if (json_equals(json, pk, "description")) {
                    json_to_str(json, pv, out->description, sizeof(out->description));
                } else if (json_equals(json, pk, "language")) {
                    json_to_str(json, pv, out->language, sizeof(out->language));
                } else if (json_equals(json, pk, "icon")) {
                    json_to_str(json, pv, out->icon_path, sizeof(out->icon_path));
                }
                p_idx += 1 + skip_subtree(tokens, p_idx + 1, n);
            }
        }

        idx += 1 + val_skip;
    }

    free(tokens);
    return true;
}

// ============================================================
// API de permisos
// ============================================================

bool arx_manifest_has_permission(const ArxManifest* m, ArxPermission perm) {
    if (!m || perm < 0 || perm >= ARX_PERM_COUNT) return false;
    return m->permissions[perm].allowed;
}

const ArxPermissionDetail* arx_manifest_get_permission(const ArxManifest* m,
                                                          ArxPermission perm) {
    if (!m || perm < 0 || perm >= ARX_PERM_COUNT) return NULL;
    return &m->permissions[perm];
}

bool arx_manifest_check_endpoint(const ArxManifest* m, const char* host_port) {
    if (!m || !host_port) return false;
    const ArxPermissionDetail* d = &m->permissions[ARX_PERM_NETWORK];
    if (!d->allowed) return false;
    if (d->endpoint_count == 0) return true;

    char req_host[256] = {0};
    const char* req_port = NULL;
    const char* colon = strchr(host_port, ':');
    if (colon) {
        size_t hlen = (size_t)(colon - host_port);
        if (hlen >= sizeof(req_host)) hlen = sizeof(req_host) - 1;
        memcpy(req_host, host_port, hlen);
        req_host[hlen] = 0;
        req_port = colon + 1;
    } else {
        strncpy(req_host, host_port, sizeof(req_host) - 1);
    }

    for (int i = 0; i < d->endpoint_count; i++) {
        const char* ep = d->endpoints[i];
        if (ep[0] == 0) continue;
        if (strcmp(ep, host_port) == 0) return true;

        if (ep[0] == '*' && ep[1] == '.') {
            char ep_host[256] = {0};
            const char* ep_port = NULL;
            const char* ecolon = strchr(ep, ':');
            if (ecolon) {
                size_t elen = (size_t)(ecolon - ep);
                if (elen >= sizeof(ep_host)) elen = sizeof(ep_host) - 1;
                memcpy(ep_host, ep, elen);
                ep_host[elen] = 0;
                ep_port = ecolon + 1;
            } else {
                strncpy(ep_host, ep, sizeof(ep_host) - 1);
            }

            const char* ep_suffix = ep_host + 1;
            size_t req_hlen = strlen(req_host);
            size_t suf_len = strlen(ep_suffix);
            if (req_hlen > suf_len &&
                strcmp(req_host + req_hlen - suf_len, ep_suffix) == 0) {
                if (ep_port) {
                    if (req_port && strcmp(ep_port, req_port) == 0) return true;
                } else {
                    return true;
                }
            }
        }
    }
    return false;
}

const char* arx_permission_name(ArxPermission perm) {
    switch (perm) {
        case ARX_PERM_NETWORK:       return "network";
        case ARX_PERM_FILESYSTEM:    return "filesystem";
        case ARX_PERM_AUDIO:         return "audio";
        case ARX_PERM_CAMERA:        return "camera";
        case ARX_PERM_MICROPHONE:    return "microphone";
        case ARX_PERM_LOCATION:      return "location";
        case ARX_PERM_CLIPBOARD:     return "clipboard";
        case ARX_PERM_NOTIFICATIONS: return "notifications";
        case ARX_PERM_FULLSCREEN:    return "fullscreen";
        case ARX_PERM_SLEEP_BLOCK:   return "sleep_block";
    }
    return "unknown";
}

int arx_permission_from_name(const char* name) {
    if (!name) return -1;
    for (int i = 0; i < ARX_PERM_COUNT; i++) {
        if (strcmp(name, arx_permission_name((ArxPermission)i)) == 0) return i;
    }
    return -1;
}
