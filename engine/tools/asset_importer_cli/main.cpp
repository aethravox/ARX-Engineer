// ==============================================================================
// tools/asset_importer_cli/main.cpp — CLI para importar assets.
// ==============================================================================
#include "asset_pipeline/asset_pipeline.hpp"
#include "core/logging.hpp"

#include <iostream>
#include <string>
#include <unordered_map>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Uso: arx_import <source> <target> [--type texture|mesh|audio|font|shader] [options]\n";
        return 1;
    }

    std::string source = argv[1];
    std::string target = argv[2];
    std::string type;
    std::unordered_map<std::string, std::string> opts;

    for (int i = 3; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--type" && i + 1 < argc) type = argv[++i];
        else if (a == "--compression" && i + 1 < argc) opts["compression"] = argv[++i];
        else if (a == "--mipmaps" && i + 1 < argc) opts["mipmaps"] = argv[++i];
        else if (a == "--size" && i + 1 < argc) opts["size"] = argv[++i];
        else if (a == "--format" && i + 1 < argc) opts["format"] = argv[++i];
    }

    auto& pipeline = arx::AssetPipeline::instance();
    if (type == "texture") pipeline.register_importer(std::make_shared<arx::TextureImporter>());
    else if (type == "mesh") pipeline.register_importer(std::make_shared<arx::MeshImporter>());
    else if (type == "audio") pipeline.register_importer(std::make_shared<arx::AudioImporter>());
    else if (type == "font") pipeline.register_importer(std::make_shared<arx::FontImporter>());
    else if (type == "shader") pipeline.register_importer(std::make_shared<arx::ShaderImporter>());

    if (!pipeline.import(source, target, opts)) {
        std::cerr << "Error importando " << source << "\n";
        return 1;
    }
    std::cout << "OK: " << source << " -> " << target << "\n";
    return 0;
}
