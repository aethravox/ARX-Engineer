# Formato .aex — ARX Executable Package

> **Estado:** Especificación v1.0 (borrador)
> **Última actualización:** 2025-09-05

Este documento define el formato binario `.aex`: el paquete firmado que
distribuye un creador y que el **ARX Client** ejecuta.

---

## 1. Filosofía

- **Firmado criptográficamente de extremo a extremo** (Ed25519).
- **Sandboxed por permisos explícitos** declarados en el manifest.
- **Autocontenido**: un solo archivo lleva bytecode + assets + manifest.
- **Verificable sin ejecutar**: el cliente rechaza paquetes con firma
  inválida o permisos no aprobados antes de tocar el FS o la red.
- **Compactable**: assets comprimidos con zstd, scripts compilados a
  bytecode Luau (o transpilados a C++ y compilados a nativo).

---

## 2. Cabecera binaria

Todos los campos little-endian. La cabecera ocupa **64 bytes** exactos.

| Offset | Tamaño | Campo | Valor / Descripción |
|--------|--------|-------|---------------------|
| 0  | 4  | `magic`            | `"AEX1"` (0x41 0x45 0x58 0x31) |
| 4  | 2  | `format_version`   | `1` (uint16) |
| 6  | 2  | `flags`            | bitmask: bit0=compressed, bit1=encrypted, bit2=AOT_compiled |
| 8  | 8  | `created_unix`     | timestamp Unix (int64) |
| 16 | 8  | `manifest_offset`  | offset al bloque manifest (int64) |
| 24 | 8  | `manifest_size`    | tamaño del manifest en bytes (int64) |
| 32 | 8  | `assets_offset`    | offset al bloque de assets (int64) |
| 40 | 8  | `assets_size`      | tamaño del bloque assets (int64) |
| 48 | 8  | `scripts_offset`   | offset al bloque de scripts (int64) |
| 56 | 8  | `scripts_size`     | tamaño del bloque scripts (int64) |

Después de la cabecera vienen, en orden:
1. **Manifest** (JSON utf-8, sin comprimir)
2. **Signature block** (firma del manifest + assets + scripts)
3. **Assets block** (tar.zstd opcionalmente)
4. **Scripts block** (bytecode Luau o nativo compilado AOT)

---

## 3. Signature block

Se ubica inmediatamente después del manifest. Estructura:

| Offset relativo | Tamaño | Campo |
|-----------------|--------|-------|
| 0  | 32 | `creator_pubkey` (Ed25519 public key) |
| 32 | 64 | `creator_signature` (Ed25519 over `sig_payload`) |
| 96 | 32 | `engine_pubkey` (opcional, para builds firmadas por ARX) |
| 128| 64 | `engine_signature` (opcional) |
| 192| 32 | `sha256(manifest)` |
| 224| 32 | `sha256(assets_block)` |
| 256| 32 | `sha256(scripts_block)` |
| 288| 4  | `sig_block_version` (=1) |

**Total signature block:** 292 bytes.

### `sig_payload`

Es el contenido firmado por el creador con su clave privada Ed25519:

```
sig_payload = magic
            || format_version
            || flags
            || created_unix
            || sha256(manifest)
            || sha256(assets_block)
            || sha256(scripts_block)
```

Esto garantiza que **cualquier modificación** de manifest, assets o scripts
invalida la firma.

---

## 4. Manifest (JSON)

```json
{
  "schema": 1,
  "package": {
    "name": "mi-juego",
    "version": "1.0.0",
    "semver_compatible": true,
    "creator": {
      "name": "Aethravox Studios",
      "id": "aethravox",
      "pubkey_fingerprint": "sha256:abcd...7890"
    }
  },
  "engine": {
    "min_version": "0.0.1",
    "max_version": null
  },
  "permissions": {
    "network": {
      "allowed": true,
      "endpoints": ["*.mi-juego.com:7777", "192.168.0.0/16"]
    },
    "filesystem": {
      "allowed": true,
      "scope": "sandbox:/data/<package_name>/"
    },
    "audio": { "allowed": true },
    "camera": { "allowed": false },
    "microphone": { "allowed": false },
    "location": { "allowed": false },
    "clipboard": { "allowed": true, "read": true, "write": true },
    "notifications": { "allowed": false },
    "fullscreen": { "allowed": true },
    "sleep_block": { "allowed": true }
  },
  "assets": {
    "entry_scene": "scenes/main.scene",
    "asset_count": 42,
    "total_size_uncompressed": 134217728
  },
  "scripts": {
    "entry": "main.luau",
    "type": "luau_bytecode_v3",
    "aot_compiled": false
  },
  "metadata": {
    "title": "Mi Juego",
    "description": "Un juego de ejemplo",
    "language": "es",
    "icon": "assets://icon.png",
    "tags": ["platformer", "indie"]
  }
}
```

---

## 5. Assets block

Formato interno: **TAR** simplificado + compresión **zstd**.

Si `flags & 0x01`:
- Bloque = `zstd_decompress(raw_block)` → TAR stream
Si no:
- Bloque = TAR stream directo

Cada archivo en el TAR tiene:
- Ruta relativa (sin `../`, sin absoluas)
- Tamaño
- Datos
- SHA256 por archivo (extended header)

### Rutas válidas

```
assets/textures/...
assets/audio/...
assets/scenes/...
assets/fonts/...
assets/models/...
assets/shaders/...
```

---

## 6. Scripts block

Depende de `scripts.type`:

### `luau_bytecode_v3`
- Bytecode compilado de Luau (formato v3, portable entre arquitecturas)
- Cargado por la VM Luau embebida en el cliente
- Hot-reloadable solo en modo desarrollo

### `aot_native`
- Binario nativo (`.so`/`.dll`/`.dylib`) compilado para la plataforma
- Cargado como plugin dinámico
- Máxima performance, sin VM
- Requiere builds separadas por plataforma

### `arxscript_cpp_transpiled`
- Código C++ transpilado desde ARXScript por el export del editor
- Compilado a nativo en build time
- Combinado con el runtime del cliente

---

## 7. Verificación (al cargar un .aex)

El cliente hace esto **antes de ejecutar nada**:

```
1. Leer 64 bytes de cabecera
2. Validar magic == "AEX1"
3. Validar format_version soportado
4. Leer manifest (JSON) + parsear
5. Verificar permisos del manifest contra política del usuario
   - Si pide algo no aprobado → preguntar al usuario
   - Si usuario rechaza → abortar
6. Leer signature block
7. Recalcular sha256(manifest) y comparar con signature block
8. Recalcular sha256(assets_block) y comparar
9. Recalcular sha256(scripts_block) y comparar
10. Reconstruir sig_payload y verificar firma Ed25519 con creator_pubkey
11. (Opcional) Verificar que creator_pubkey está en trust store del usuario
12. Recién ahora: descomprimir assets, cargar scripts, iniciar VM
```

Si **cualquier paso falla**, el paquete se rechaza y se loguea el motivo.

---

## 8. Magic bytes

| Extensión | Magic | ASCII |
|-----------|-------|-------|
| `.aex`    | `0x41 0x45 0x58 0x31` | `AEX1` |
| `.arx`    | `0x41 0x52 0x58 0x50` | `ARXP` (proyecto editable) |
| `.ascn`   | `0x41 0x52 0x58 0x53` | `ARXS` (escena individual) |

---

## 9. Consideraciones de seguridad

- **Ed25519** se elige por: claves pequeñas (32 bytes), firmas pequeñas
  (64 bytes), verificación rápida, sin curvas frágiles.
- **No hay obfuscación**: la seguridad está en la firma, no en el
  cifrado. Si el creador quiere ofuscar assets, puede aplicar XOR/AES
  con clave derivada de la pubkey, pero eso es opcional.
- **Reproducibilidad**: dado el mismo manifest + assets + scripts + clave
  privada, la firma es determinista.
- **Revocación**: futura v2 añadirá un campo `revocation_seed` para
  permitir al creador revocar versiones anteriores vía lista pública.

---

## 10. Roadmap del formato

- [x] v1.0 spec (este documento)
- [ ] Implementación `arx::format` en `common/`
- [ ] CLI `arx_sign` para firmar .aex
- [ ] CLI `arx_verify` para verificar .aex sin ejecutarlo
- [ ] CLI `arx_pack` para empaquetar `.arx` → `.aex`
- [ ] Tests de round-trip (crear, firmar, verificar, ejecutar)
- [ ] v1.1: cifrado opcional de assets
- [ ] v2.0: revocación + claves rotativas
