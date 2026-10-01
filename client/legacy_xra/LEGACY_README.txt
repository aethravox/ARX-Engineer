# =============================================================
# ARX ENGINE v2.1 - GUIA DE COMPILACION Y USO
# =============================================================

## LIBRERIAS INTEGRADAS (desde third_Party)

| Libreria | Proposito | Formatos soportados |
|----------|-----------|---------------------|
| **Raylib** | Renderizado, ventana, input, audio | Graficos 2D/3D, audio (.ogg, .wav, .mp3, .flac), texturas |
| **Jolt Physics** | Fisica | Cajas, esferas, cilindros, compuestos |
| **Assimp** | Carga de modelos 3D | .obj, .fbx, .gltf, .dae, .stl, .blend |
| **ImGui** | UI inmediata | Paneles, sliders, botones, ventanas |
| **GLM** | Matematicas 3D | Vectores, matrices, quaterniones |
| **zlib** | Compresion | Paquetes .asx |

> NOTA: A partir de v2.1, el audio se gestiona completamente a traves de Raylib.
> Se elimino la dependencia de MiniAudio. Todo el audio (basico y avanzado)
> se maneja con las funciones nativas de Raylib (InitAudioDevice, LoadSound,
> LoadMusicStream, etc.).

## ESTRUCTURA DEL PROYECTO

```
ARX_Engine/
├── Motor/                    # Editor del motor (crear escenas)
│   └── platform/
│       ├── windows/          # Windows (MinGW + SCons/Make)
│       ├── linux/            # Linux (g++ + SCons/Make)
│       └── android/          # Android (NDK + CMake + Java/Kotlin)
├── Cliente/                  # Visor (ejecutar juegos exportados)
│   └── platform/
│       ├── windows/
│       ├── linux/
│       └── android/
├── common/                   # Headers de integracion de third_party
│   ├── ARX_ThirdParty.hpp    # Master include
│   ├── ARX_Physics.hpp       # Jolt Physics wrapper
│   ├── ARX_Models.hpp        # Assimp model loading
│   ├── ARX_IMGUI.hpp         # ImGui integration
│   ├── ARX_Audio.hpp         # Raylib Audio wrapper (reemplaza MiniAudio)
│   └── ARX_Math.hpp          # GLM math utilities
├── third_Party/              # Librerias precompiladas (NO EDITAR)
│   ├── Windows/              # .a para Windows MinGW
│   ├── Linux/                # .a para Linux x64
│   ├── Android_64_Bits/      # .a para Android arm64-v8a
│   └── Include/              # Headers de todas las librerias
│       ├── raylib/
│       ├── assimp/
│       ├── glm/
│       ├── imgui/
│       └── Jolt/
├── build/                    # Scripts de build unificados
│   ├── build_windows.bat
│   ├── build_linux.sh
│   └── build_android.sh
├── icons/                    # Iconos de la aplicacion
│   ├── android/              # PNGs para Android (mipmap-*dpi)
│   ├── windows/              # .ico para Windows
│   └── linux/                # .png para Linux (.desktop)
├── android/                  # Proyecto Android (Java/Kotlin)
│   ├── Motor/                # Activity del Motor
│   │   ├── java/com/arx/engine/motor/MainActivity.java
│   │   ├── kotlin/com/arx/engine/motor/MainActivity.kt
│   │   └── AndroidManifest.xml
│   └── Cliente/              # Activity del Cliente
│       ├── java/com/arx/engine/cliente/MainActivity.java
│       ├── kotlin/com/arx/engine/cliente/MainActivity.kt
│       └── AndroidManifest.xml
├── CMakeLists.txt            # CMake raiz (multi-plataforma)
└── README.txt
```

## COMPILACION

### Windows (MinGW + SCons)
1. Instalar MinGW-w64 y SCons
2. `cd build`
3. `build_windows.bat`
4. Motor: `Motor/platform/windows/bin/windows/ARX_Motor.exe`
5. Cliente: `Cliente/platform/windows/bin/windows/ARX_Cliente.exe`

### Windows (MinGW + Make directo)
```
cd Motor/platform/windows && scons
cd ../../.. && cd Cliente/platform/windows && scons
```

### Linux (g++ + Make)
1. `chmod +x build/build_linux.sh`
2. `cd build && ./build_linux.sh`
3. Motor: `Motor/platform/linux/bin/linux/ARX_Motor`
4. Cliente: `Cliente/platform/linux/bin/linux/ARX_Cliente`

### Linux (manual con Makefile)
```
cd Motor/platform/linux && make -f Makefile.linux
cd ../../../Cliente/platform/linux && make -f Makefile.linux
```

### Android (NDK + CMake)
1. `export ANDROID_NDK=/ruta/al/ndk`
2. `chmod +x build/build_android.sh`
3. `cd build && ./build_android.sh`

### CMake (multi-plataforma)
```
# Desde la raiz del proyecto:
mkdir build_cmake && cd build_cmake

# Linux:
cmake .. && make -j$(nproc)

# Windows:
cmake -G "MinGW Makefiles" .. && mingw32-make

# Android:
cmake -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-21 .. && make
```

## FORMATO .AEX

```
tipo:NOMBRE|prop1:valor1|prop2:valor2|...
```

### Tipos 2D:
- **fondo**: `tipo:fondo|color:azul_oscuro`
- **texto**: `tipo:texto|val:Hola|x:100|y:50|color:blanco|tam:20`
- **boton**: `tipo:boton|val:Click|x:100|y:200|color:verde|accion:ir a nivel1`
- **rectangulo**: `tipo:rectangulo|x:100|y:100|ancho:200|alto:100|color:rojo|relleno:si|grosorBorde:2`
- **rectanguloRedondeado**: `tipo:rectanguloRedondeado|x:100|y:100|ancho:200|alto:50|radio:0.3|color:azul|relleno:si`
- **circulo**: `tipo:circulo|x:400|y:300|radio:50|color:amarillo|relleno:si`
- **triangulo**: `tipo:triangulo|x1:100|y1:100|x2:200|y2:200|x3:300|y3:100|color:verde|relleno:no`
- **linea**: `tipo:linea|x1:0|y1:0|x2:100|y2:100|grosor:3|color:blanco`
- **punto**: `tipo:punto|x:100|y:100|color:rojo|tam:6`
- **gradiente**: `tipo:gradiente|x:0|y:0|ancho:200|alto:100|color1:negro|color2:blanco|direccion:v`
- **imagen**: `tipo:imagen|ruta:foto.png|x:100|y:100|ancho:200|alto:150`
- **barraProgreso**: `tipo:barraProgreso|x:100|y:100|ancho:200|alto:20|valor:65|max:100|color:verde|fondo:gris`
- **checkbox**: `tipo:checkbox|x:100|y:100|texto:Opcion|marcado:si|color:blanco`
- **cajatexto**: `tipo:cajatexto|x:100|y:100|ancho:200|alto:30|texto:Hola|placeholder:Escribe|color:blanco`
- **tooltip**: `tipo:tooltip|texto:Info|x:100|y:100|color:amarillo`
- **slider**: `tipo:slider|x:100|y:100|ancho:200|alto:20|min:0|max:100|valor:50|label:Volumen|color:blanco`
- **audio**: `tipo:audio|ruta:musica.ogg|volumen:0.7|loop:si|autoplay:si`
- **sonido**: `tipo:sonido|ruta:click.wav|volumen:0.5|autoplay:si`
- **audio_avanzado**: `tipo:audio_avanzado|tipo_audio:musica|ruta:musica.mp3|volumen:0.8|loop:si|autoplay:si`

### Tipos 3D:
- **cubo**: `tipo:cubo|x:0|y:0|z:0|ancho:2|alto:2|profundo:2|color:rojo|relleno:si`
- **esfera**: `tipo:esfera|x:0|y:0|z:0|radio:1|color:azul|relleno:si`
- **plano**: `tipo:plano|x:0|y:0|z:0|ancho:10|profundo:10|color:gris`
- **cuadricula**: `tipo:cuadricula|tamX:10|tamZ:10|divisiones:10|color:gris`
- **cilindro**: `tipo:cilindro|x:0|y:2|z:0|radio:1|altura:3|color:verde|relleno:no`
- **modelo3d**: `tipo:modelo3d|ruta:model.obj|x:0|y:0|z:0|rx:0|ry:0|rz:0|sx:1|sy:1|sz:1|wireframe:no`

### Tipos de Fisica (Jolt):
- **rigidbody**: `tipo:rigidbody|id:caja1|forma:box|hx:0.5|hy:0.5|hz:0.5|x:0|y:10|z:0|tipo:dinamico|masa:1.0|friccion:0.5|rebote:0.3`
- **gravedad**: `tipo:gravedad|gx:0|gy:-9.81|gz:0`

Parametros de rigidbody:
- `id`: Identificador unico (requerido)
- `forma`: box, sphere, cylinder
- `hx/hy/hz`: Half-sizes (box) o radio (sphere) o radio/height (cylinder)
- `tipo`: dinamico, estatico, cinematico, sensor
- `masa`: Masa en kg (solo dinamicos)

Para que un cubo/esfera/cilindro se mueva con la fisica, anadele `id:nombre` y crea un rigidbody con el mismo id.

## CONTROLES DEL MOTOR

| Tecla | Accion |
|-------|--------|
| **E** | Abrir/Cerrar editor de escenas |
| **P** | Publicar (generar .asx) |
| **G** | Activar panel de debug ImGui |
| **F5** | Recargar escena |
| **Ctrl+S** | Guardar en editor |
| **Ctrl+Delete** | Borrar ultima linea en editor |
| **Rueda mouse** | Scroll en editor |
| **ESC** | Salir |

## COMO USAR LAS LIBRERIAS EN TUS JUEGOS

Los headers de integracion estan en `common/`. Incluyelos en tu codigo:

```cpp
// Fisica con Jolt
#include "ARX_Physics.hpp"
RenderEngine::InitPhysics(true);
RenderEngine::StepPhysics(GetFrameTime());

// Cargar modelos 3D con Assimp
#include "ARX_Models.hpp"
ARX::ARXModel* model = ARX::ARXModelManager::Load("mi_modelo.obj");
model->Draw();

// UI con ImGui
#include "ARX_IMGUI.hpp"
RenderEngine::InitImGui(true);
ARX::ARXImGui::NewFrame(width, height);
// ... crear widgets de ImGui ...
ARX::ARXImGui::Render(width, height);

// Audio con Raylib (reemplaza MiniAudio)
#include "ARX_Audio.hpp"
ARX::ARXAudioManager::PlayMusic("musica.ogg", 0.8f, true);
ARX::ARXAudioManager::PlaySound("explosion.wav");
// Actualizar streams de musica cada frame:
ARX::ARXAudioManager::UpdateMusicStreams();

// Matematicas con GLM
#include "ARX_Math.hpp"
Vector3 pos = ARX::Math::LerpV3({0,0,0}, {10,5,0}, 0.5f);
Quaternion rot = ARX::Math::QuaternionFromEuler(0, 45, 0);
Matrix view = ARX::Math::LookAt(eye, target, {0,1,0});
```

O incluye todo de una vez:
```cpp
#include "ARX_ThirdParty.hpp"
```

## CAMBIOS EN v2.1

1. **Eliminada dependencia de MiniAudio**: Todo el audio se gestiona ahora
   unicamente a traves de la API de Raylib (Sound, Music, InitAudioDevice).
2. `ARX_Audio.hpp` reescrito para usar Raylib en vez de MiniAudio.
3. `ARX_ThirdParty.hpp` actualizado (sin include de miniaudio.h).
4. `AethravoxGraphics.hpp` actualizado en todas las plataformas.
5. Corregido: `Motor/platform/windows/graphics/AethravoxGraphics.hpp` ahora
   contiene el codigo completo (antes estaba vacio).
6. Creados archivos faltantes del Motor para Windows: `main.cpp`, `resources/`.
7. Eliminada referencia a `miniaudio/` en CMakeLists.txt.
8. Agregados archivos Java y Kotlin para Android (Activities, Manifests).
9. Agregados iconos de aplicacion para Android (.png), Windows (.ico), Linux (.png).
10. `UpdateMusicStreams()` ahora tambien actualiza streams de ARXAudioManager.

## CAMBIOS EN v2.0

1. Integracion completa de third_Party (assimp, imgui, Jolt, glm)
2. No requiere instalacion externa de librerias
3. Nuevo elemento `modelo3d` para cargar modelos .obj/.fbx/.gltf
4. Nuevos elementos `rigidbody` y `gravedad` para fisica con Jolt
5. Nuevo elemento `audio_avanzado` (ahora usa Raylib en vez de MiniAudio)
6. Panel de debug ImGui (tecla G)
7. Fisica en el game loop (los cubos/esferas con `id` se mueven)
8. CMakeLists.txt raiz para build multi-plataforma
9. Scripts de build unificados en build/
10. Headers comunes en common/ para desarrollo de juegos
11. IsType3D() helper para separar elementos 2D/3D
12. InitImGui(), InitPhysics(), InitAudioAvanzado() en RenderEngine
