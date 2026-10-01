// =============================================================
// ARX ENGINE - Modulo de Debug para Jolt Physics
// Manejo de errores, aserciones y reportes de la API
// =============================================================
#ifndef ARX_JOLT_DEBUG_HPP
#define ARX_JOLT_DEBUG_HPP

#include "Jolt/Jolt.h"
#include "Jolt/Core/IssueReporting.h"
#include <iostream>

// Abrimos el namespace de Jolt para definir el callback
JPH_NAMESPACE_BEGIN

/**
 * Callback personalizado que se dispara cuando Jolt detecta una operacion invalida.
 * @return true para que el depurador intente romper la ejecucion en esa linea.
 */
inline bool ARXAssertFailedCallback(const char* inExpression, const char* inMessage, const char* inFile, uint inLine)
{
    std::cerr << "\n[!] ------------------------------------" << std::endl;
    std::cerr << "[ARX_PHYSICS_ASSERT_FAILED]" << std::endl;
    std::cerr << "  Expresion: " << inExpression << std::endl;
    
    if (inMessage && inMessage[0] != '\0')
        std::cerr << "  Mensaje:   " << inMessage << std::endl;
        
    std::cerr << "  Archivo:   " << inFile << std::endl;
    std::cerr << "  Linea:     " << inLine << std::endl;
    std::cerr << "----------------------------------------\n" << std::endl;
    std::cerr.flush();

    // Retornar true permite que, si estas en modo Debug, el IDE se detenga justo aqui.
    return true; 
}

JPH_NAMESPACE_END

namespace ARX {

class ARXPhysicsDebug {
public:
    static void SetupCallbacks() {
        // Registramos nuestro callback en la fabrica de Jolt
        JPH::AssertFailed = JPH::ARXAssertFailedCallback;
    }
};

} // namespace ARX

#endif
