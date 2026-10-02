// ==============================================================================
// src/assets/glb_loader.cpp — Cargador de GLB con texturas.
// ==============================================================================
#include "glb_loader.hpp"
#include "core/logging.hpp"

#include <fstream>
#include <cstring>
#include <cmath>
#include <sstream>
#include <algorithm>

// stb_image para decodificar imágenes embebidas
#include "stb_image.h"

namespace arx {

// ---- Mini JSON parser (igual que antes) ----
struct JsonValue {
    enum Type { Null, Bool, Number, String, Array, Object };
    Type type = Null;
    bool boolVal = false;
    double numVal = 0;
    std::string strVal;
    std::vector<JsonValue> arrVal;
    std::vector<std::string> objKeys;
    std::vector<JsonValue> objVals;

    const JsonValue* find(const std::string& key) const {
        if (type != Object) return nullptr;
        for (size_t i = 0; i < objKeys.size(); ++i) {
            if (objKeys[i] == key) return &objVals[i];
        }
        return nullptr;
    }
};

struct JsonParser {
    const char* s;
    const char* end;

    void skipWs() {
        while (s < end && (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r')) s++;
    }

    JsonValue parseValue() {
        skipWs();
        if (s >= end) return JsonValue{};
        char c = *s;
        if (c == '"') return parseString();
        if (c == '{') return parseObject();
        if (c == '[') return parseArray();
        if (c == 't' || c == 'f') return parseBool();
        if (c == 'n') return parseNull();
        return parseNumber();
    }

    JsonValue parseString() {
        JsonValue v; v.type = JsonValue::String;
        s++;
        while (s < end && *s != '"') {
            if (*s == '\\' && s + 1 < end) {
                s++;
                char e = *s;
                if (e == 'n') v.strVal += '\n';
                else if (e == 't') v.strVal += '\t';
                else if (e == 'r') v.strVal += '\r';
                else if (e == '"') v.strVal += '"';
                else if (e == '\\') v.strVal += '\\';
                else if (e == '/') v.strVal += '/';
                else v.strVal += e;
            } else {
                v.strVal += *s;
            }
            s++;
        }
        if (s < end) s++;
        return v;
    }

    JsonValue parseNumber() {
        JsonValue v; v.type = JsonValue::Number;
        const char* start = s;
        while (s < end && (*s == '-' || *s == '+' || *s == '.' ||
               (*s >= '0' && *s <= '9') || *s == 'e' || *s == 'E')) s++;
        std::string num(start, s);
        v.numVal = std::stod(num);
        return v;
    }

    JsonValue parseBool() {
        JsonValue v; v.type = JsonValue::Bool;
        if (s + 4 <= end && std::strncmp(s, "true", 4) == 0) { v.boolVal = true; s += 4; }
        else if (s + 5 <= end && std::strncmp(s, "false", 5) == 0) { v.boolVal = false; s += 5; }
        return v;
    }

    JsonValue parseNull() {
        JsonValue v; v.type = JsonValue::Null;
        if (s + 4 <= end && std::strncmp(s, "null", 4) == 0) s += 4;
        return v;
    }

    JsonValue parseArray() {
        JsonValue v; v.type = JsonValue::Array;
        s++;
        skipWs();
        if (s < end && *s == ']') { s++; return v; }
        while (s < end) {
            v.arrVal.push_back(parseValue());
            skipWs();
            if (s < end && *s == ',') { s++; continue; }
            if (s < end && *s == ']') { s++; break; }
            break;
        }
        return v;
    }

    JsonValue parseObject() {
        JsonValue v; v.type = JsonValue::Object;
        s++;
        skipWs();
        if (s < end && *s == '}') { s++; return v; }
        while (s < end) {
            skipWs();
            if (*s != '"') break;
            JsonValue key = parseString();
            v.objKeys.push_back(key.strVal);
            skipWs();
            if (s < end && *s == ':') s++;
            v.objVals.push_back(parseValue());
            skipWs();
            if (s < end && *s == ',') { s++; continue; }
            if (s < end && *s == '}') { s++; break; }
            break;
        }
        return v;
    }
};

// ---- GLB types ----
struct GLBAccessor {
    int bufferView = -1;
    int componentType = 0;
    int count = 0;
    std::string type;
};

struct GLBBufferView {
    int buffer = 0;
    int byteOffset = 0;
    int byteLength = 0;
    int byteStride = 0;
    int target = 0;
};

struct GLBPrimitive {
    int indices = -1;
    int material = -1;
    int posAccessor = -1;
    int normAccessor = -1;
    int uvAccessor = -1;
};

struct GLBMeshDef {
    std::string name;
    std::vector<GLBPrimitive> primitives;
};

struct GLBMaterial {
    float baseColor[4] = {0.8f, 0.8f, 0.8f, 1.0f};
    int baseColorTexture = -1;  // índice de textura, -1 si no hay
};

// ---- Helper: leer accessor como array de floats ----
static std::vector<float> readAccessorFloats(
    const GLBAccessor& acc,
    const std::vector<GLBBufferView>& views,
    const std::vector<uint8_t>& binData)
{
    std::vector<float> result;
    if (acc.bufferView < 0 || acc.bufferView >= (int)views.size()) return result;

    const auto& view = views[acc.bufferView];
    if (view.buffer != 0) return result;

    int compSize = 0;
    switch (acc.componentType) {
        case 5120: compSize = 1; break;
        case 5121: compSize = 1; break;
        case 5122: compSize = 2; break;
        case 5123: compSize = 2; break;
        case 5125: compSize = 4; break;
        case 5126: compSize = 4; break;
        default: return result;
    }

    int numComponents = 1;
    if (acc.type == "VEC2") numComponents = 2;
    else if (acc.type == "VEC3") numComponents = 3;
    else if (acc.type == "VEC4") numComponents = 4;

    int stride = view.byteStride > 0 ? view.byteStride : (compSize * numComponents);
    int offset = view.byteOffset;

    result.reserve(acc.count * numComponents);

    for (int i = 0; i < acc.count; ++i) {
        const uint8_t* ptr = binData.data() + offset + i * stride;
        for (int c = 0; c < numComponents; ++c) {
            float val = 0;
            switch (acc.componentType) {
                case 5126: { std::memcpy(&val, ptr + c * 4, 4); break; }
                case 5123: { uint16_t u; std::memcpy(&u, ptr + c * 2, 2); val = (float)u; break; }
                case 5121: { val = (float)ptr[c]; break; }
                case 5122: { int16_t t; std::memcpy(&t, ptr + c * 2, 2); val = (float)t; break; }
                case 5120: { val = (float)((int8_t)ptr[c]); break; }
                case 5125: { uint32_t u; std::memcpy(&u, ptr + c * 4, 4); val = (float)u; break; }
            }
            result.push_back(val);
        }
    }
    return result;
}

static std::vector<uint32_t> readAccessorIndices(
    const GLBAccessor& acc,
    const std::vector<GLBBufferView>& views,
    const std::vector<uint8_t>& binData)
{
    std::vector<uint32_t> result;
    if (acc.bufferView < 0 || acc.bufferView >= (int)views.size()) return result;

    const auto& view = views[acc.bufferView];
    if (view.buffer != 0) return result;

    int compSize = 0;
    switch (acc.componentType) {
        case 5121: compSize = 1; break;
        case 5123: compSize = 2; break;
        case 5125: compSize = 4; break;
        default: return result;
    }

    int offset = view.byteOffset;
    result.reserve(acc.count);

    for (int i = 0; i < acc.count; ++i) {
        const uint8_t* ptr = binData.data() + offset + i * compSize;
        uint32_t idx = 0;
        switch (acc.componentType) {
            case 5121: idx = ptr[0]; break;
            case 5123: { uint16_t u; std::memcpy(&u, ptr, 2); idx = u; break; }
            case 5125: { std::memcpy(&idx, ptr, 4); break; }
        }
        result.push_back(idx);
    }
    return result;
}

// ---- Implementación principal ----
GLBResult load_glb(const std::string& path) {
    GLBResult result;

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        result.error = "No se pudo abrir: " + path;
        return result;
    }

    std::vector<uint8_t> fileData((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
    file.close();

    if (fileData.size() < 12) {
        result.error = "Archivo muy pequeño para ser GLB";
        return result;
    }

    if (std::strncmp((const char*)fileData.data(), "glTF", 4) != 0) {
        result.error = "Magic no es glTF";
        return result;
    }

    uint32_t version;
    std::memcpy(&version, fileData.data() + 4, 4);
    if (version != 2) {
        result.error = "Solo soportamos GLB version 2";
        return result;
    }

    uint32_t totalLength;
    std::memcpy(&totalLength, fileData.data() + 8, 4);

    size_t offset = 12;
    std::string jsonStr;
    std::vector<uint8_t> binData;

    while (offset + 8 <= fileData.size()) {
        uint32_t chunkLength;
        std::memcpy(&chunkLength, fileData.data() + offset, 4);
        uint32_t chunkType;
        std::memcpy(&chunkType, fileData.data() + offset + 4, 4);

        if (offset + 8 + chunkLength > fileData.size()) {
            result.error = "Chunk se sale del archivo";
            return result;
        }

        const uint8_t* chunkData = fileData.data() + offset + 8;

        if (chunkType == 0x4E4F534A) {  // "JSON"
            jsonStr.assign((const char*)chunkData, chunkLength);
            jsonStr.push_back('\0');
        } else if (chunkType == 0x004E4942) {  // "BIN\0"
            binData.assign(chunkData, chunkData + chunkLength);
        }

        offset += 8 + chunkLength;
    }

    if (jsonStr.empty()) {
        result.error = "No se encontró chunk JSON";
        return result;
    }

    ARX_LOG_INFO("GLB: JSON {} bytes, BIN {} bytes", jsonStr.size(), binData.size());

    JsonParser parser{jsonStr.data(), jsonStr.data() + jsonStr.size()};
    JsonValue root = parser.parseValue();

    if (root.type != JsonValue::Object) {
        result.error = "JSON root no es object";
        return result;
    }

    // Leer accessors[]
    std::vector<GLBAccessor> accessors;
    if (auto* accs = root.find("accessors")) {
        for (auto& a : accs->arrVal) {
            GLBAccessor acc;
            if (auto* bv = a.find("bufferView")) acc.bufferView = (int)bv->numVal;
            if (auto* ct = a.find("componentType")) acc.componentType = (int)ct->numVal;
            if (auto* c = a.find("count")) acc.count = (int)c->numVal;
            if (auto* t = a.find("type")) acc.type = t->strVal;
            accessors.push_back(acc);
        }
    }

    // Leer bufferViews[]
    std::vector<GLBBufferView> views;
    if (auto* bvs = root.find("bufferViews")) {
        for (auto& b : bvs->arrVal) {
            GLBBufferView view;
            if (auto* buf = b.find("buffer")) view.buffer = (int)buf->numVal;
            if (auto* bo = b.find("byteOffset")) view.byteOffset = (int)bo->numVal;
            if (auto* bl = b.find("byteLength")) view.byteLength = (int)bl->numVal;
            if (auto* bs = b.find("byteStride")) view.byteStride = (int)bs->numVal;
            if (auto* t = b.find("target")) view.target = (int)t->numVal;
            views.push_back(view);
        }
    }

    // Leer meshes[]
    std::vector<GLBMeshDef> meshDefs;
    if (auto* meshes = root.find("meshes")) {
        for (auto& m : meshes->arrVal) {
            GLBMeshDef md;
            if (auto* n = m.find("name")) md.name = n->strVal;
            if (auto* prims = m.find("primitives")) {
                for (auto& p : prims->arrVal) {
                    GLBPrimitive prim;
                    if (auto* idx = p.find("indices")) prim.indices = (int)idx->numVal;
                    if (auto* mat = p.find("material")) prim.material = (int)mat->numVal;
                    if (auto* attrs = p.find("attributes")) {
                        if (auto* pos = attrs->find("POSITION")) prim.posAccessor = (int)pos->numVal;
                        if (auto* norm = attrs->find("NORMAL")) prim.normAccessor = (int)norm->numVal;
                        if (auto* uv = attrs->find("TEXCOORD_0")) prim.uvAccessor = (int)uv->numVal;
                    }
                    md.primitives.push_back(prim);
                }
            }
            meshDefs.push_back(md);
        }
    }

    // Leer materials[]
    std::vector<GLBMaterial> materials;
    if (auto* mats = root.find("materials")) {
        for (auto& m : mats->arrVal) {
            GLBMaterial mat;
            if (auto* pbr = m.find("pbrMetallicRoughness")) {
                if (auto* bcf = pbr->find("baseColorFactor")) {
                    if (bcf->arrVal.size() >= 4) {
                        mat.baseColor[0] = (float)bcf->arrVal[0].numVal;
                        mat.baseColor[1] = (float)bcf->arrVal[1].numVal;
                        mat.baseColor[2] = (float)bcf->arrVal[2].numVal;
                        mat.baseColor[3] = (float)bcf->arrVal[3].numVal;
                    }
                }
                if (auto* bct = pbr->find("baseColorTexture")) {
                    if (auto* idx = bct->find("index")) mat.baseColorTexture = (int)idx->numVal;
                }
            }
            materials.push_back(mat);
        }
    }

    // FASE 14B: Leer textures[]
    if (auto* texs = root.find("textures")) {
        for (auto& t : texs->arrVal) {
            GLBTexture tex;
            if (auto* src = t.find("source")) tex.image_index = (int)src->numVal;
            if (auto* smp = t.find("sampler")) tex.sampler_index = (int)smp->numVal;
            result.textures.push_back(tex);
        }
    }

    // FASE 14B: Leer images[] y decodificar PNG/JPEG embebidos
    if (auto* imgs = root.find("images")) {
        for (auto& img : imgs->arrVal) {
            GLBImage image;
            if (auto* n = img.find("name")) image.name = n->strVal;
            if (auto* mt = img.find("mimeType")) image.mime_type = mt->strVal;
            if (auto* bv = img.find("bufferView")) image.buffer_view = (int)bv->numVal;
            if (auto* uri = img.find("uri")) image.uri = uri->strVal;

            // Si está embebida (bufferView), decodificar
            if (image.buffer_view >= 0 && image.buffer_view < (int)views.size()) {
                const auto& view = views[image.buffer_view];
                if (view.buffer == 0) {
                    int offset = view.byteOffset;
                    int length = view.byteLength;
                    if (offset + length <= (int)binData.size()) {
                        // Decodificar con stb_image
                        int w, h, ch;
                        stbi_uc* pixels = stbi_load_from_memory(
                            binData.data() + offset, length, &w, &h, &ch, 4);
                        if (pixels) {
                            image.width = w;
                            image.height = h;
                            image.channels = 4;
                            image.pixels.assign(pixels, pixels + (size_t)w * h * 4);
                            stbi_image_free(pixels);
                            ARX_LOG_INFO("GLB: imagen '{}' decodificada {}x{} ({} bytes en GLB)",
                                         image.name, w, h, length);
                        } else {
                            ARX_LOG_WARN("GLB: no se pudo decodificar imagen '{}'", image.name);
                        }
                    }
                }
            }
            // TODO: data URI (base64) y URI externa - fases futuras

            result.images.push_back(std::move(image));
        }
    }

    ARX_LOG_INFO("GLB: {} accessors, {} bufferViews, {} meshes, {} materials, {} textures, {} images",
                 accessors.size(), views.size(), meshDefs.size(), materials.size(),
                 result.textures.size(), result.images.size());

    // Procesar cada mesh
    for (auto& md : meshDefs) {
        for (auto& prim : md.primitives) {
            GLBMesh mesh;
            mesh.name = md.name;
            mesh.material_index = prim.material;

            // Material
            if (prim.material >= 0 && prim.material < (int)materials.size()) {
                std::memcpy(mesh.albedo, materials[prim.material].baseColor, 4 * sizeof(float));
                // Asociar textura si el material tiene baseColorTexture
                int texIdx = materials[prim.material].baseColorTexture;
                if (texIdx >= 0 && texIdx < (int)result.textures.size()) {
                    mesh.texture_index = texIdx;
                }
            }

            // Posiciones
            std::vector<float> positions;
            if (prim.posAccessor >= 0 && prim.posAccessor < (int)accessors.size()) {
                positions = readAccessorFloats(accessors[prim.posAccessor], views, binData);
            }

            // Normales
            std::vector<float> normals;
            if (prim.normAccessor >= 0 && prim.normAccessor < (int)accessors.size()) {
                normals = readAccessorFloats(accessors[prim.normAccessor], views, binData);
            }

            // UVs
            std::vector<float> uvs;
            if (prim.uvAccessor >= 0 && prim.uvAccessor < (int)accessors.size()) {
                uvs = readAccessorFloats(accessors[prim.uvAccessor], views, binData);
                mesh.has_uvs = !uvs.empty();
            }

            // Índices
            if (prim.indices >= 0 && prim.indices < (int)accessors.size()) {
                mesh.indices = readAccessorIndices(accessors[prim.indices], views, binData);
            }

            // Combinar en vertices: pos(3) + normal(3) + uv(2) = 8 floats
            int vertexCount = (int)positions.size() / 3;
            mesh.vertices.reserve(vertexCount * 8);

            for (int i = 0; i < vertexCount; ++i) {
                mesh.vertices.push_back(positions[i * 3 + 0]);
                mesh.vertices.push_back(positions[i * 3 + 1]);
                mesh.vertices.push_back(positions[i * 3 + 2]);
                if (i * 3 + 2 < (int)normals.size()) {
                    mesh.vertices.push_back(normals[i * 3 + 0]);
                    mesh.vertices.push_back(normals[i * 3 + 1]);
                    mesh.vertices.push_back(normals[i * 3 + 2]);
                } else {
                    mesh.vertices.push_back(0);
                    mesh.vertices.push_back(1);
                    mesh.vertices.push_back(0);
                }
                if (mesh.has_uvs && i * 2 + 1 < (int)uvs.size()) {
                    mesh.vertices.push_back(uvs[i * 2 + 0]);
                    mesh.vertices.push_back(uvs[i * 2 + 1]);
                } else {
                    mesh.vertices.push_back(0);
                    mesh.vertices.push_back(0);
                }
            }

            if (mesh.indices.empty() && vertexCount > 0) {
                mesh.indices.resize(vertexCount);
                for (int i = 0; i < vertexCount; ++i) mesh.indices[i] = i;
            }

            ARX_LOG_INFO("GLB: mesh '{}' - {} verts, {} indices, tex={} uv={}",
                         mesh.name, vertexCount, (int)mesh.indices.size(),
                         mesh.texture_index, mesh.has_uvs ? "yes" : "no");

            result.meshes.push_back(std::move(mesh));
        }
    }

    return result;
}

} // namespace arx
