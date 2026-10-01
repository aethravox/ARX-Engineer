// ==============================================================================
// src/scene/tilemap/tilemap.hpp — VoxTileMap 2D + VoxTileSet.
// ==============================================================================
#pragma once

#include "scene/2d/vox2d.hpp"
#include "render/texture.hpp"
#include "core/types.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace arx {

// VoxTileSet — colección de tiles definidos por región de una textura atlas.
class VoxTileSet : public Object {
public:
    ARX_CLASS(VoxTileSet, Object);
public:

    struct Tile {
        int     id = 0;
        Rect2   region;          // Coordenadas en píxeles en la textura atlas.
        std::string name;
        bool    collision = false;
        Vector2 size{16, 16};
    };

    void set_texture(std::shared_ptr<Texture> tex) { texture_ = tex; }
    std::shared_ptr<Texture> get_texture() const { return texture_; }

    void add_tile(const Tile& t) { tiles_[t.id] = t; }
    const Tile* get_tile(int id) const {
        auto it = tiles_.find(id);
        return it != tiles_.end() ? &it->second : nullptr;
    }
    int get_tile_count() const { return (int)tiles_.size(); }

    void set_tile_size(Vector2 s) { tile_size_ = s; }
    Vector2 get_tile_size() const { return tile_size_; }

private:
    std::shared_ptr<Texture> texture_;
    std::unordered_map<int, Tile> tiles_;
    Vector2 tile_size_{16, 16};
};

// VoxTileMap — grilla de tiles.
class VoxTileMap : public Vox2D {
public:
    ARX_CLASS(VoxTileMap, Vox2D);
public:

    void set_tileset(std::shared_ptr<VoxTileSet> ts) { tileset_ = ts; }
    std::shared_ptr<VoxTileSet> get_tileset() const { return tileset_; }

    void set_cell_size(Vector2 s) { cell_size_ = s; }
    Vector2 get_cell_size() const { return cell_size_; }

    // Coordenadas de celda.
    void set_cell(int x, int y, int tile_id);
    int  get_cell(int x, int y) const;
    void clear();

    // Iteración.
    int  get_width()  const { return width_; }
    int  get_height() const { return height_; }

    // Conversión cell ↔ world.
    Vector2 map_to_world(int x, int y) const {
        return position_ + Vector2(x * cell_size_.x, y * cell_size_.y);
    }
    std::pair<int,int> world_to_map(Vector2 p) const {
        Vector2 local = p - position_;
        return { (int)std::floor(local.x / cell_size_.x),
                 (int)std::floor(local.y / cell_size_.y) };
    }

    // Render (override draw()).
    void draw() override;

private:
    std::shared_ptr<VoxTileSet> tileset_;
    std::unordered_map<int64_t, int> cells_;   // packed (x,y) → tile_id
    Vector2 cell_size_{16, 16};
    int width_  = 256;
    int height_ = 256;

    static int64_t pack(int x, int y) {
        return ((int64_t)x << 32) | (uint32_t)y;
    }
};

} // namespace arx
