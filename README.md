# ARX — Motor de apps y juegos nativo C++

> **ARX Engineer** crea apps/juegos en C++ + ARXScript (o Luau).
> Exporta a `.aex`, un paquete **firmado criptográficamente** y **sandboxeado por permisos**.
> **ARX Client** (runtime ligero, ~10MB) verifica la firma y ejecuta el paquete en un sandbox seguro.
>
> **Sin servidor propio.** El creador puede conectar su juego a su propio server (MMO, multiplayer, API REST, lo que sea).
> El cliente solo garantiza que el paquete viene de quien dice ser y que no pide más permisos de los aprobados.

---

## Arquitectura

```
┌───────────────────────────────────────────────────────┐
│  ARX ENGINEER  (editor + motor, C++ + ImGui)          │
│  ├─ ECS + scene tree 2D/3D                            │
│  ├─ Renderer OpenGL 3.3+ (Vulkan experimental)        │
│  ├─ ARXScript (lexer+parser+VM+transpiler AOT a C++)  │
│  ├─ Luau embebido (sandboxing nativo de Roblox)       │
│  ├─ Editor ImGui (docks, viewport, inspector)         │
│  ├─ Physics (Bullet), Audio, Input, Networking        │
│  └─ Export → .aex firmado                             │
└───────────────────────────────────────────────────────┘
                          │
                          ▼  (distribución P2P, sin server propio)
┌───────────────────────────────────────────────────────┐
│  .aex  (paquete firmado con Ed25519)                  │
│  ├─ Manifest JSON + permisos explícitos               │
│  ├─ Signature block (Ed25519 + SHA256 hashes)         │
│  ├─ Assets comprimidos con zstd (tar)                 │
│  └─ Scripts (Luau bytecode o AOT nativo)              │
└───────────────────────────────────────────────────────┘
                          │
                          ▼
┌───────────────────────────────────────────────────────┐
│  ARX CLIENT  (runtime ~10MB, C++)                     │
│  ├─ Verifica firma Ed25519 antes de ejecutar          │
│  ├─ Lee manifest → pide permisos al usuario           │
│  ├─ Sandbox: solo lo que el manifest aprueba          │
│  ├─ Carga assets + scripts en memoria                 │
│  ├─ Ejecuta VM Luau (o nativo si AOT) + render loop   │
│  └─ Si el juego pide red → se conecta a donde quiera  │
└───────────────────────────────────────────────────────┘
                          │
                          ▼  (si el juego pide permiso de red)
                ╔═══════════════════════════════╗
                ║  SERVER DEL CREADOR           ║
                ║  (Go, Rust, Node, C++, etc.)  ║
                ║  El cliente no impone nada.   ║
                ╚═══════════════════════════════╝
```

---

## Diferenciador

> **"Descarga mi .aex de 5MB, el cliente lo verifica criptográficamente, lo ejecuta sandboxed, y si el creador lo firmó con su clave privada, sabes que es auténtico."**

Esto **no existe hoy** en motores comerciales:

- **Más abierto que App Store** (sin review process, distribución P2P)
- **Más seguro que .exe suelto** (sandbox + firma criptográfica)
- **Más liviano que Unity/Electron** (cliente 10MB vs 200MB+)
- **Más poderoso que Pico-8** (red, 3D, MMO, scripting serio)
- **Sin vendor lock-in** (estándares abiertos, MIT)

---

## Estructura del repositorio

```
ARX/
├── README.md                    # este archivo
├── engine/                      # ARX Engineer (editor + motor)
│   ├── src/                     # 17k LOC ya migrados
│   │   ├── core/                # types, math, object, variant, logging
│   │   ├── os/                  # abstracción OS + windowing
│   │   ├── platforms/           # windows/linux/web/android
│   │   ├── render/              # renderer + opengl/ + shaders
│   │   ├── scene/               # node, scene_tree, 2D/, 3D/, resources/
│   │   ├── physics/             # physics_server + bullet/ wrapper
│   │   ├── audio/               # audio_server (WAV/MP3/OGG)
│   │   ├── input/               # input + input_map
│   │   ├── arxscript/           # lexer, parser, AST, transpiler → C++
│   │   ├── editor/              # editor_main + gui/ + icons/
│   │   ├── export/              # export_plugin + exporters/
│   │   └── assets/              # splash, icons, logos
│   ├── thirdparty/              # stack Godot (Bullet, FreeType, ENet, etc.)
│   ├── CMakeLists.txt
│   ├── CMakePresets.json
│   └── LEGACY_README.md         # README del ARX Engine previo (referencia)
│
├── client/                      # ARX Client (runtime que ejecuta .aex)
│   ├── src/
│   │   ├── core/
│   │   ├── loader/              # carga + verifica firma del .aex
│   │   ├── sandbox/             # enforcement de permisos del manifest
│   │   ├── runtime/             # bootstrap VM + render loop
│   │   ├── render/              # renderer mínimo (OpenGL)
│   │   ├── audio/
│   │   ├── net/                 # networking con permisos
│   │   └── platform/            # linux/windows/android/web
│   ├── thirdparty/              # miniaudio, Luau, glad (mínimo)
│   └── docs/
│
├── format/                      # spec + lib del .aex firmado
│   ├── spec/SPEC.md             # especificación formal del formato
│   ├── include/                 # headers C de la lib
│   ├── src/                     # implementación
│   └── tests/                   # round-trip tests
│
├── tools/                       # CLI tools
│   ├── arx_sign/                # firmar .aex con Ed25519
│   ├── arx_pack/                # empaquetar .arx → .aex
│   └── arx_verify/              # verificar firma sin ejecutar
│
├── common/                      # código compartido engine + client
│   ├── include/arx/
│   │   ├── format/aex.h         # API del .aex
│   │   └── crypto/ed25519.h     # wrapper Ed25519
│   └── src/
│
├── docs/                        # docs globales
│   ├── architecture/
│   ├── aex_format/
│   ├── arxscript/
│   ├── sandbox_model/
│   └── building/
│
├── build/                       # scripts de build
│   ├── cmake/
│   └── scripts/
│
├── tests/
└── examples/
    ├── hello_world/
    ├── platformer_2d/
    └── mmo_client/
```

---

## Tecnologías

| Componente | Tecnología |
|------------|-----------|
| Lenguaje del motor | C++20 |
| Build system | CMake 3.20+ |
| Renderer | OpenGL 3.3+ / GLES 3.0 (Vulkan experimental) |
| Física | Bullet 3.24 |
| Audio | miniaudio / minimp3 / libvorbis |
| UI del editor | Dear ImGui + docking |
| Scripting | **ARXScript** (transpiler AOT a C++) + **Luau** (sandboxing) |
| Networking | ENet + wslay (WebSocket) + mbedtls (TLS) |
| Criptografía | Ed25519 (libsodium o fallback) + SHA-256 |
| Compresión | zstd + zlib |
| Carga de modelos | Assimp |
| Math | GLM |

---

## Formato .aex — el corazón del proyecto

Ver **[format/spec/SPEC.md](format/spec/SPEC.md)** para la especificación completa.

Resumen:

- Cabecera de 64 bytes (magic `AEX1` + offsets)
- Manifest JSON con **permisos explícitos** (network, FS, audio, cámara, etc.)
- Signature block de 292 bytes con **firma Ed25519** del creador
- Assets comprimidos con zstd (formato TAR interno)
- Scripts como bytecode Luau o AOT compilado a nativo

El cliente verifica **todo** antes de ejecutar: hashes SHA256 + firma Ed25519 + permisos del manifest.

---

## Estado del proyecto

| Componente | Estado |
|------------|--------|
| ARX Engineer (editor) | ✅ Migrado desde ARX Engine previo (17k LOC) |
| ARXScript (lexer+parser+VM+transpiler) | ✅ Migrado |
| Renderer OpenGL + postprocess | ✅ Migrado |
| Editor ImGui completo | ✅ Migrado |
| Exporters Win/Linux/Android/Web | ✅ Migrado |
| ARX Client (runtime) | 🚧 Boceto (legacy XRA preservado como referencia) |
| Formato .aex (spec + lib) | 🚧 Spec v1.0 listo, lib pendiente |
| Tools (arx_sign, arx_pack, arx_verify) | 🚧 Esqueletos con TODOs |
| Sandbox enforcement | ⏳ Pendiente |
| Tests | ⏳ Pendiente |
| Ejemplos (hello_world, mmo_client) | ⏳ Pendiente |

---

## Roadmap

### Fase 1 — Fundación (en curso)
- [x] Migrar código previo a la nueva estructura
- [x] Spec del formato .aex v1.0
- [ ] Implementar `arx::format` (lib C para .aex)
- [ ] Implementar `arx::crypto::ed25519` (wrapper libsodium/fallback)
- [ ] CLI `arx_sign` + `arx_verify` funcionales

### Fase 2 — Cliente mínimo
- [ ] ARX Client con render OpenGL + Luau VM
- [ ] Cargar `.aex`, verificar firma, ejecutar `hello_world.luau`
- [ ] Sandbox: bloquear FS/red/audio si manifest no lo aprueba
- [ ] Build para Linux + Windows

### Fase 3 — Integración con el editor
- [ ] Botón "Export → .aex" en ARX Engineer
- [ ] Pipeline: ARXScript → Luau bytecode (o C++ AOT) → empaquetar
- [ ] Sistema de claves del creador (keypair management en el editor)

### Fase 4 — Multiplataforma
- [ ] Android (NDK + GLES 3.0)
- [ ] Web (Emscripten + WebGL 2)
- [ ] iOS (futuro)

### Fase 5 — Ecosistema
- [ ] Trust store: usuario marca claves como de confianza
- [ ] CLI `arx_pack` para empaquetar sin abrir el editor
- [ ] Ejemplos: hello_world, platformer_2d, mmo_client
- [ ] Documentación web interactiva

---

## Licencia

MIT. Las librerías third-party conservan sus licencias originales.

---

## Créditos

**Aethravox Studios** — diseño y desarrollo.

Basado en trabajo previo de ARX Engine (17k LOC ya migrados) y XRA_Engine
(arquitectura Cliente/Motor que inspiró la separación actual).
