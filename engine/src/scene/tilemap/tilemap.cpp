#include "render/renderer.hpp"
#include "scene/scene_tree.hpp"
// ==============================================================================
// src/scene/tilemap/tilemap.cpp
// ==============================================================================
#include "tilemap.hpp"

#include <cmath>

namespace arx {

void VoxTileMap::set_cell(int x, int y, int tile_id) {
    int64_t key = pack(x, y);
    if (tile_id < 0) cells_.erase(key);
    else             cells_[key] = tile_id;
}

int VoxTileMap::get_cell(int x, int y) const {
    auto it = cells_.find(pack(x, y));
    return it != cells_.end() ? it->second : -1;
}

void VoxTileMap::clear() { cells_.clear(); }

void VoxTileMap::draw() {
    if (!tileset_ || !tileset_->get_texture()) return;
    auto* tree = get_tree();
    if (!tree || !tree->get_renderer()) return;
    auto& r = *tree->get_renderer();
    auto tex = tileset_->get_texture();

    for (const auto& [key, tile_id] : cells_) {
        int x = (int32_t)(key >> 32);
        int y = (int32_t)(key & 0xFFFFFFFF);
        Vector2 world_pos = map_to_world(x, y);
        const VoxTileSet::Tile* tile = tileset_->get_tile(tile_id);
        if (!tile) continue;
        r.draw_sprite_2d(tex, world_pos, tile->size, 0.0f,
                         Color::white, &tile->region);
    }
}

} // namespace arx
