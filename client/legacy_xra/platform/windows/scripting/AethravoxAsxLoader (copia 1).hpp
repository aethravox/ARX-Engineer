#ifndef AETHRAVOX_ASX_LOADER_HPP
#define AETHRAVOX_ASX_LOADER_HPP

#include <unordered_map>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <zlib.h>
#include <cstdint>
#include <cstring>

class AethravoxAsxLoader {
public:
    static inline std::unordered_map<std::string, std::string> MemoriaVirtual;

    static bool CargarPaqueteCompleto(const std::string& ruta) {
        std::ifstream file(ruta, std::ios::binary);
        if (!file.is_open()) {
            std::cout << "[AsxLoader] No se pudo abrir: " << ruta << std::endl;
            return false;
        }

        // Verificar firma
        char magic[13] = {0};
        file.read(magic, 12);
        if (std::strcmp(magic, "AETHRAVOX_V1") != 0) {
            std::cout << "[AsxLoader] Firma invalida en: " << ruta << std::endl;
            return false;
        }

        uint32_t numFiles = 0;
        file.read(reinterpret_cast<char*>(&numFiles), 4);
        std::cout << "[AsxLoader] Archivos en paquete: " << numFiles << std::endl;

        MemoriaVirtual.clear();

        for (uint32_t i = 0; i < numFiles; ++i) {
            uint16_t nameLen = 0;
            file.read(reinterpret_cast<char*>(&nameLen), 2);

            if (nameLen == 0 || nameLen > 256) {
                std::cout << "[AsxLoader] Error: nombre de archivo invalido (#" << i << ")" << std::endl;
                return false;
            }

            std::string name(nameLen, ' ');
            file.read(&name[0], nameLen);

            uint64_t compSize = 0;
            file.read(reinterpret_cast<char*>(&compSize), 8);

            if (compSize == 0 || compSize > 50 * 1024 * 1024) {  // Max 50MB
                std::cout << "[AsxLoader] Error: tamano comprimido invalido para: " << name << std::endl;
                return false;
            }

            std::vector<unsigned char> compData(compSize);
            file.read(reinterpret_cast<char*>(compData.data()), compSize);

            if (!file.good()) {
                std::cout << "[AsxLoader] Error leyendo datos de: " << name << std::endl;
                return false;
            }

            std::string descomprimido = Descomprimir(compData);
            if (descomprimido.empty()) {
                std::cout << "[AsxLoader] Error descomprimiendo: " << name << std::endl;
                return false;
            }

            MemoriaVirtual[name] = descomprimido;
            std::cout << "[AsxLoader] Cargado: " << name
                      << " (" << descomprimido.size() << " bytes)" << std::endl;
        }

        return !MemoriaVirtual.empty();
    }

private:
    static std::string Descomprimir(const std::vector<unsigned char>& data) {
        z_stream zs;
        std::memset(&zs, 0, sizeof(zs));
        if (inflateInit(&zs) != Z_OK) {
            std::cout << "[AsxLoader] inflateInit fallo" << std::endl;
            return "";
        }

        zs.next_in = reinterpret_cast<Bytef*>(const_cast<unsigned char*>(data.data()));
        zs.avail_in = (uInt)data.size();

        int ret;
        char buffer[32768];
        std::string out;

        do {
            zs.next_out = reinterpret_cast<Bytef*>(buffer);
            zs.avail_out = sizeof(buffer);
            ret = inflate(&zs, Z_NO_FLUSH);

            if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
                std::cout << "[AsxLoader] Error de descompresion: " << ret << std::endl;
                inflateEnd(&zs);
                return "";
            }

            size_t have = sizeof(buffer) - zs.avail_out;
            out.append(buffer, have);
        } while (ret != Z_STREAM_END);

        inflateEnd(&zs);
        return out;
    }
};

#endif
