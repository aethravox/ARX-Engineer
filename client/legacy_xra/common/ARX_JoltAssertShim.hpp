// =============================================================
// ARX ENGINE - Jolt Assert/Trace shim (Android cross-link)
// =============================================================
#pragma once

// Este shim arregla errores de link tipo:
// undefined symbol: JPH::AssertFailed / JPH::AssertFailedParamHelper
// cuando Jolt compila pero no exporta su implementación de asserts.

#include "Jolt/Core/IssueReporting.h"

// Resolver undefined symbols en cross-link.
// Definimos los símbolos esperados por IssueReporting.h dentro de namespace JPH.
namespace JPH {

// Tipos explícitos (evita errores del editor / include parcial)
using uint = unsigned int;
using TraceFunction = void (*)(const char* inFMT, ...);
using AssertFailedFunction = bool(*)(const char* inExpression, const char* inMessage, const char* inFile, uint inLine);

inline AssertFailedFunction AssertFailed = [](const char* inExpression,
                                               const char* inMessage,
                                               const char* inFile,
                                               uint inLine) -> bool {
    (void)inExpression;
    (void)inMessage;
    (void)inFile;
    (void)inLine;
    return false;
};

inline TraceFunction Trace = [](const char* inFMT, ...) {
    (void)inFMT;
};

} // namespace JPH
