# ==============================================================================
# tools/CMakeLists.txt — CLI tools
# ==============================================================================

# Linter de ARXScript
add_executable(arxscript_lint arxscript_lint/main.cpp)
target_link_libraries(arxscript_lint PRIVATE arx_core)

# Formatter de ARXScript
add_executable(arxscript_format arxscript_format/main.cpp)
target_link_libraries(arxscript_format PRIVATE arx_core)

# Builder de export templates
add_executable(export_template_builder export_template_builder/main.cpp)
target_link_libraries(export_template_builder PRIVATE arx_core)

# CLI de asset pipeline
add_executable(arx_import asset_importer_cli/main.cpp)
target_link_libraries(arx_import PRIVATE arx_core)

# Compilador de scenes
add_executable(scene_compiler scene_compiler/main.cpp)
target_link_libraries(scene_compiler PRIVATE arx_core)

# Exporter de proyectos (CLI)
add_executable(project_exporter project_exporter/main.cpp)
target_link_libraries(project_exporter PRIVATE arx_core)

# ==============================================================================
# CLI tools para .aex (firmar, empaquetar, verificar)
# ==============================================================================

# arx_keygen: generar par de claves Ed25519
add_executable(arx_keygen arx_keygen/main.cpp)
target_link_libraries(arx_keygen PRIVATE arx_core ${ARX_COMMON_LIB})
target_include_directories(arx_keygen PRIVATE ${CMAKE_SOURCE_DIR}/../common/include)

# arx_pack: empaquetar directorio en .aex firmado
add_executable(arx_pack arx_pack/main.cpp)
target_link_libraries(arx_pack PRIVATE arx_core ${ARX_COMMON_LIB})
target_include_directories(arx_pack PRIVATE ${CMAKE_SOURCE_DIR}/../common/include)

# arx_verify: verificar firma de un .aex
add_executable(arx_verify arx_verify/main.cpp)
target_link_libraries(arx_verify PRIVATE arx_core ${ARX_COMMON_LIB})
target_include_directories(arx_verify PRIVATE ${CMAKE_SOURCE_DIR}/../common/include)
