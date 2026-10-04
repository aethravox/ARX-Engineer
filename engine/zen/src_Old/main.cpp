// ============================================================
// Zen Programming Language - CLI Entry Point
// v1.2: #lang, --platform, while, for-each, repeat, break/continue
// Compilacion 100% nativa con LLVM — sin dependencia de Clang
// ============================================================

#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "zen_linker.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <unistd.h>  // getpid() para .o unicos en /tmp

void printBanner() {
    std::cout << "\033[32m";
    std::cout << "  Zen Lang v1.2 - Compilador nativo con LLVM\n";
    std::cout << "\033[0m\n";
    std::cout << "  Bilingue ES/EN | Sin tipos | Sin punto y coma\n";
    std::cout << "  Compilacion nativa sin Clang | Plataformas condicionales\n\n";
}

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo: " + path);
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::string baseName(const std::string& path) {
    size_t lastSlash = path.find_last_of("/\\");
    std::string filename = (lastSlash == std::string::npos) ? path : path.substr(lastSlash + 1);
    size_t lastDot = filename.find_last_of('.');
    return (lastDot == std::string::npos) ? filename : filename.substr(0, lastDot);
}

int runCommand(const std::string& cmd) {
    int ret = std::system(cmd.c_str());
    return WEXITSTATUS(ret);
}

void cleanupFile(const std::string& path) {
    std::remove(path.c_str());
}

// Obtener el directorio de un path
std::string getDir(const std::string& path) {
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash == std::string::npos) return ".";
    return path.substr(0, lastSlash);
}

// Imports: resolver importar "file" .zhn y .zen
std::vector<std::string> importedFiles;

std::string resolveImports(const std::string& source, const std::string& baseDir) {
    std::string result;
    std::istringstream ss(source);
    std::string line;
    while (std::getline(ss, line)) {
        std::string trimmed = line;
        size_t start = trimmed.find_first_not_of(" \t");
        if (start != std::string::npos) trimmed = trimmed.substr(start);
        bool isImport = false;
        std::string importPath;
        if (trimmed.size() >= 9 && trimmed.substr(0, 9) == "importar ") {
            isImport = true;
            importPath = trimmed.substr(9);
        } else if (trimmed.size() >= 7 && trimmed.substr(0, 7) == "import ") {
            isImport = true;
            importPath = trimmed.substr(7);
        }
        if (isImport) {
            // Buscar comillas dobles
            char q = 34;
            size_t q1 = importPath.find(q);
            size_t q2 = importPath.find(q, q1 + 1);
            if (q1 != std::string::npos && q2 != std::string::npos) {
                importPath = importPath.substr(q1 + 1, q2 - q1 - 1);
            } else {
                size_t h = importPath.find(35); // #
                if (h != std::string::npos) importPath = importPath.substr(0, h);
                size_t s = importPath.find_first_not_of(" \t");
                size_t e = importPath.find_last_not_of(" \t\r\n");
                if (s != std::string::npos) importPath = importPath.substr(s, e - s + 1);
            }
            // Probar .zhn primero, luego .zen
            std::string fullPath = baseDir + "/" + importPath;
            std::string tryPath = fullPath;
            if (importPath.find(46) == std::string::npos) { // no tiene punto
                tryPath = fullPath + ".zhn";
                FILE* f = fopen(tryPath.c_str(), "r");
                if (!f) {
                    tryPath = fullPath + ".zen";
                    f = fopen(tryPath.c_str(), "r");
                    if (!f) { tryPath = fullPath; }
                    else { fclose(f); }
                } else { fclose(f); }
            }
            // Evitar imports circulares
            bool already = false;
            for (const auto& imp : importedFiles) {
                if (imp == tryPath) { already = true; break; }
            }
            if (!already) {
                importedFiles.push_back(tryPath);
                std::string impSource = readFile(tryPath);
                std::string impDir = getDir(tryPath);
                result += "# Import: " + tryPath + "\n";
                result += resolveImports(impSource, impDir);
                result += "\n# --- end of import ---\n\n";
            }
        } else {
            result += line + "\n";
        }
    }
    return result;
}

// Detectar la plataforma actual
std::string detectHostPlatform() {
#if defined(_WIN32) || defined(_WIN64)
    return "windows";
#elif defined(__ANDROID__)
    return "android";
#elif defined(__APPLE__) && defined(__MACH__)
    return "macos";
#elif defined(__linux__)
    return "linux";
#else
    return "unknown";
#endif
}

int main(int argc, char* argv[]) {
    CodeGen::initializeTargets();
    printBanner();

    if (argc < 2) {
        std::cout << "Uso: zen <archivo.zen> [opciones]\n\n";
        std::cout << "Opciones:\n";
        std::cout << "  --ir        Mostrar el LLVM IR generado\n";
        std::cout << "  --obj       Generar archivo objeto (.o)\n";
        std::cout << "  --build     Compilar a ejecutable nativo\n";
        std::cout << "  --run       Compilar y ejecutar\n";
        std::cout << "  --platform <p>  Compilar para plataforma (windows/linux/macos/android)\n";
        std::cout << "  (sin opciones) Genera archivo .ll\n\n";
        std::cout << "Directivas:\n";
        std::cout << "  #lang es    Forzar idioma español\n";
        std::cout << "  #lang en    Forzar idioma ingles\n\n";
        std::cout << "Ejemplo plataforma:\n";
        std::cout << "  plataforma windows\n";
        std::cout << "      muestra \"Ejecutandose en Windows!\"\n";
        std::cout << "  plataforma linux\n";
        std::cout << "      muestra \"Ejecutandose en Linux!\"\n";
        std::cout << "  plataforma android\n";
        std::cout << "      muestra \"Ejecutandose en Android!\"\n\n";
        return 1;
    }

    std::string inputFile = argv[1];
    bool showIR   = false;
    bool runAfter = false;
    bool buildExe = false;
    bool emitObj  = false;
    bool doLink   = false;
    std::string platformOverride;

    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--ir")     showIR   = true;
        if (arg == "--run")    runAfter = true;
        if (arg == "--build")  buildExe = true;
        if (arg == "--obj")    emitObj  = true;
        if (arg == "--link")   doLink   = true;
        if (arg == "--platform" && i + 1 < argc) {
            i++;
            platformOverride = argv[i];
            std::transform(platformOverride.begin(), platformOverride.end(),
                           platformOverride.begin(), ::tolower);
        }
    }

    // Determinar plataforma target
    std::string targetPlatform = platformOverride.empty() ? detectHostPlatform() : platformOverride;

    try {
        // 1. Leer archivo
        std::cout << "\033[90m[1/4] Leyendo: \033[0m" << inputFile << "\n";
        std::string source = readFile(inputFile);
        importedFiles.clear();
        importedFiles.push_back(inputFile);
        std::string baseDir = getDir(inputFile);
        // Insertar marker de archivo inicial para que el lexer trackee filename
        source = "# Import: " + inputFile + "\n" + source;
        source = resolveImports(source, baseDir);

        // 2. Lexico
        std::cout << "\033[90m[2/4] Tokenizando...\033[0m\n";
        Lexer lexer(source);
        auto lexResult = lexer.tokenize();
        std::cout << "\033[90m      " << lexResult.tokens.size() << " tokens generados\033[0m\n";

        if (!lexResult.lang.empty()) {
            std::cout << "\033[90m      Idioma: \033[0m" << lexResult.lang;
            if (lexResult.lang == "es") std::cout << " (español)";
            else if (lexResult.lang == "en") std::cout << " (english)";
            std::cout << "\n";
        }

        // 3. Parser
        std::cout << "\033[90m[3/4] Parseando...\033[0m\n";
        Parser parser(lexResult.tokens);
        auto ast = parser.parse();
        std::cout << "\033[90m      " << ast.size() << " nodos en el AST\033[0m\n";

        // 4. Generacion de LLVM IR
        std::cout << "\033[90m[4/4] Generando LLVM IR...\033[0m\n";
        std::cout << "\033[90m      Plataforma target: \033[0m" << targetPlatform << "\n";
        CodeGen codegen(targetPlatform);
        codegen.generate(ast);

        if (codegen.verify()) {
            std::cerr << "\033[31mERROR: El modulo LLVM generado es invalido.\033[0m\n";
            // El IR se mantiene en memoria en codegen.getIR() — no escribir al disco del proyecto
            return 1;
        }

        std::string ir = codegen.getIR();
        std::string outBase = baseName(inputFile);

        // --- Modo --ir ---
        if (showIR) {
            std::cout << "\033[33m--- LLVM IR ---\033[0m\n";
            std::cout << ir << "\n";
            std::cout << "\033[33m--- Fin IR ---\033[0m\n\n";
            return 0;
        }

        // --- Modo por defecto: mostrar resumen del IR (en RAM, no al disco) ---
        // El IR completo vive en codegen.getIR() y solo se materializa a /tmp si hace falta
        // para --obj / --build / --run (que lo pasan a LLVM directamente).
        {
            std::cout << "\033[32m   OK: \033[0m" << outBase << " IR generado ("
                      << ir.size() << " bytes, en RAM)\n\n";
        }

        // --- Modo --obj ---
        // El .o se escribe donde el usuario espera (junto al .zen) — el .ll nunca toca disco
        if (emitObj) {
            std::string objFile = outBase + ".o";
            std::cout << "\033[90mGenerando codigo objeto nativo...\033[0m\n";
            if (!codegen.emitObjectFile(objFile)) {
                std::cerr << "\033[31mERROR al generar codigo objeto.\033[0m\n";
                return 1;
            }
            std::cout << "\033[32m   OK: \033[0m" << objFile << " generado\n\n";
            return 0;
        }


        // --- Modo --link (compilar .o + linkear a ejecutable) ---
        if (doLink) {
            std::string objFile = outBase + ".o";
            std::string exeFile = outBase;
            if (targetPlatform == "windows") exeFile += ".exe";
            else if (targetPlatform == "web") exeFile += ".html";

            std::cout << "Compilando a codigo objeto...\n";
            if (!codegen.emitObjectFile(objFile)) {
                std::cerr << "ERROR al generar codigo objeto.\n";
                return 1;
            }
            std::cout << "   OK: " << objFile << " generado\n";

            std::cout << "Linkeando (plataforma: " << targetPlatform << ")...\n";
            auto reqLibs = codegen.getRequiredLibs();
            std::vector<std::string> libs(reqLibs.begin(), reqLibs.end());

            std::string link_error;
            if (!zen::link_object(objFile, exeFile, targetPlatform, libs, link_error)) {
                std::cerr << "ERROR al linkear: " << link_error << "\n";
                cleanupFile(objFile);  // limpiar .o temporal
                return 1;
            }
            cleanupFile(objFile);  // limpiar .o temporal tras linkeo exitoso
            std::cout << "   OK: " << exeFile << " generado\n\n";
            return 0;
        }

        // --- Modo --build / --run ---
        // El .o se genera temporalmente en /tmp, NO en el directorio del proyecto
        if (buildExe || runAfter) {
            // Generar .o en /tmp para no ensuciar el directorio del proyecto
            // (cuando se haga el refactor de compilacion modular, cada .zen → su .o en /tmp/zen_build/)
            std::string objFile = "/tmp/zen_" + outBase + "_" + std::to_string(getpid()) + ".o";
            std::string exeFile = outBase;

            std::cout << "\033[90mCompilando a codigo objeto nativo...\033[0m\n";
            if (!codegen.emitObjectFile(objFile)) {
                std::cerr << "\033[31mERROR al generar codigo objeto.\033[0m\n";
                cleanupFile(objFile);
                return 1;
            }
            std::cout << "\033[32m   OK: \033[0m" << objFile << " generado\n";

            std::string linkCmd = "cc " + objFile + " -o " + exeFile;

            // Agregar librerias externas (FFI)
            // Buscar en thirdparty/<lib>/lib/lib<lib>.a
            // Pero NO para librerias del sistema (libm, libc, etc.)
            auto reqLibs = codegen.getRequiredLibs();
            for (const auto& lib : reqLibs) {
                // Saltar librerias del sistema que ya vienen con cc
                if (lib == "libc" || lib == "libm" || lib == "c" || lib == "m" || lib == "libLLVM") continue;
                std::string libPath = "thirdparty/" + lib + "/lib";
                linkCmd += " -L" + libPath + " -l" + lib;
            }
            // Dependencias especiales para librerias conocidas
            for (const auto& lib : reqLibs) {
                if (lib == "raylib") {
                    // raylib en Linux necesita OpenGL, X11, etc.
                    linkCmd += " -lGL -lX11 -lpthread -ldl -lrt";
                }
            }
            // -lm al final para que el linker resuelva dependencias de librerias externas
            linkCmd += " -lm";
            // Siempre agregar libzen.a (tiene zen_strtod_safe, zen_concat, __zen_num_to_str, zen_callN, etc.)
            linkCmd += " build/libzen.a -L/usr/lib/llvm-14/lib -lLLVM-14 -lstdc++ -lpthread";

            std::cout << "\033[90mLinkeando: \033[0m" << linkCmd << "\n";

            int ret = runCommand(linkCmd);
            if (ret != 0) {
                std::cerr << "\033[31mERROR al linkear. Asegurate de tener cc/gcc instalado.\033[0m\n";
                cleanupFile(objFile);
                return 1;
            }

            cleanupFile(objFile);
            std::cout << "\033[32m   Compilado exitosamente: \033[0m" << exeFile << "\n\n";

            if (runAfter) {
                std::cout << "\033[33m--- Ejecutando ---\033[0m\n";
                std::string runCmd = "./" + exeFile;
                ret = runCommand(runCmd);
                std::cout << "\033[33m--- Fin ejecucion (codigo: " << ret << ") ---\033[0m\n";
                return ret;
            }

            return 0;
        }

        // Sin flags
        std::cout << "Para compilar:\n";
        std::cout << "  zen " << inputFile << " --build    # genera ejecutable\n";
        std::cout << "  zen " << inputFile << " --run      # compila y ejecuta\n";
        std::cout << "  zen " << inputFile << " --platform linux --build\n\n";

    } catch (const std::exception& e) {
        std::cerr << "\033[31m\n  ERROR: \033[0m" << e.what() << "\n\n";
        return 1;
    }

    return 0;
}
