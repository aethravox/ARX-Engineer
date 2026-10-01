// ==============================================================================
// src/modules/register_module_types.hpp — Declaración del registro central.
// ==============================================================================
#pragma once

namespace arx {

// Registra todas las clases del motor en ClassDB.
// Definida en register_module_types.cpp.
void register_module_types();

// Desregistra (stub: ClassDB no lo soporta en esta versión).
void unregister_module_types();

} // namespace arx
