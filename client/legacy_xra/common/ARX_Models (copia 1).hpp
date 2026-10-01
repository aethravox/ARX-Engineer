// =============================================================
// ARX ENGINE - Modulo de Carga de Modelos 3D (Assimp)
// Carga modelos .obj, .fbx, .gltf, .dae, .stl, etc.
// =============================================================
#ifndef ARX_MODELS_HPP
#define ARX_MODELS_HPP

#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"
#include "raylib.h"
#include "rlgl.h"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <cfloat>

namespace ARX {

struct ARXModelVertex {
    Vector3 position;
    Vector3 normal;
    Vector2 texcoord;
    Color color = {255, 255, 255, 255};
};

struct ARXMesh {
    std::vector<ARXModelVertex> vertices;
    std::vector<unsigned short> indices;
    Texture2D texture = {0}; // Textura cargada para este mesh
    bool hasTexture = false;
    std::string name;
    Color diffuseColor = {255, 255, 255, 255};

    void Draw() const {
        if (vertices.empty() || indices.empty()) return;

        // Activar textura si existe
        if (hasTexture) rlEnableTexture(texture.id);

        rlBegin(RL_TRIANGLES);
        for (unsigned short idx : indices) {
            const auto& v = vertices[idx];
            rlColor4ub(v.color.r, v.color.g, v.color.b, v.color.a);
            rlTexCoord2f(v.texcoord.x, v.texcoord.y);
            rlNormal3f(v.normal.x, v.normal.y, v.normal.z);
            rlVertex3f(v.position.x, v.position.y, v.position.z);
        }
        rlEnd();

        if (hasTexture) rlDisableTexture();
    }

    void DrawWires(Color wireColor) const {
        rlBegin(RL_LINES);
        rlColor4ub(wireColor.r, wireColor.g, wireColor.b, wireColor.a);
        for (size_t i = 0; i < indices.size(); i += 3) {
            const auto& v0 = vertices[indices[i]];
            const auto& v1 = vertices[indices[i+1]];
            const auto& v2 = vertices[indices[i+2]];
            rlVertex3f(v0.position.x, v0.position.y, v0.position.z);
            rlVertex3f(v1.position.x, v1.position.y, v1.position.z);
            rlVertex3f(v1.position.x, v1.position.y, v1.position.z);
            rlVertex3f(v2.position.x, v2.position.y, v2.position.z);
            rlVertex3f(v2.position.x, v2.position.y, v2.position.z);
            rlVertex3f(v0.position.x, v0.position.y, v0.position.z);
        }
        rlEnd();
    }
};

class ARXModel {
public:
    std::vector<ARXMesh> meshes;
    Vector3 position = {0, 0, 0};
    Vector3 rotation = {0, 0, 0}; // Euler en grados
    Vector3 scale = {1, 1, 1};
    bool loaded = false;

    bool Load(const std::string& path) {
        Assimp::Importer importer;
        // Flags optimizados para Raylib (Triangulate es obligatorio)
        unsigned int flags = aiProcess_Triangulate | aiProcess_FlipUVs | 
                             aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices;

        const aiScene* scene = importer.ReadFile(path, flags);
        if (!scene || !scene->mRootNode) {
            std::cout << "[ARXModel] Error: " << importer.GetErrorString() << std::endl;
            return false;
        }

        std::string modelDir = path.substr(0, path.find_last_of("/\\") + 1);
        ProcessNode(scene->mRootNode, scene, modelDir);

        loaded = true;
        return true;
    }

    void Draw() const {
        if (!loaded) return;
        rlPushMatrix();
        rlTranslatef(position.x, position.y, position.z);
        rlRotatef(rotation.y, 0, 1, 0);
        rlRotatef(rotation.x, 1, 0, 0);
        rlRotatef(rotation.z, 0, 0, 1);
        rlScalef(scale.x, scale.y, scale.z);

        for (const auto& mesh : meshes) mesh.Draw();
        rlPopMatrix();
    }

    void DrawWires(Color wireColor) const {
        if (!loaded) return;
        rlPushMatrix();
        rlTranslatef(position.x, position.y, position.z);
        rlRotatef(rotation.y, 0, 1, 0);
        rlRotatef(rotation.x, 1, 0, 0);
        rlRotatef(rotation.z, 0, 0, 1);
        rlScalef(scale.x, scale.y, scale.z);

        for (const auto& mesh : meshes) mesh.DrawWires(wireColor);
        rlPopMatrix();
    }

private:
    void ProcessNode(aiNode* node, const aiScene* scene, const std::string& dir) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(ProcessMesh(mesh, scene, dir));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene, dir);
        }
    }

    ARXMesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& dir) {
        ARXMesh result;
        
        // Carga de Material / Textura
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
            aiString texPath;
            if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
                std::string fullPath = dir + texPath.C_Str();
                result.texture = LoadTexture(fullPath.c_str());
                result.hasTexture = (result.texture.id > 0);
            }
        }

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            ARXModelVertex v;
            v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            v.normal = mesh->HasNormals() ? Vector3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z} : Vector3{0,1,0};
            v.texcoord = mesh->mTextureCoords[0] ? Vector2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y} : Vector2{0,0};
            v.color = WHITE;
            result.vertices.push_back(v);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) result.indices.push_back(face.mIndices[j]);
        }
        return result;
    }
};

class ARXModelManager {
public:
    static inline std::unordered_map<std::string, std::unique_ptr<ARXModel>> modelCache;
    static ARXModel* Load(const std::string& path) {
        if (modelCache.count(path)) return modelCache[path].get();
        auto model = std::make_unique<ARXModel>();
        if (model->Load(path)) return (modelCache[path] = std::move(model)).get();
        return nullptr;
    }
    static void Clear() { modelCache.clear(); }
    static void ClearAll() { modelCache.clear(); }
};

} // namespace ARX

#endif
