#pragma once
#include "../core/Types.hpp"
#include "../core/Config.hpp"

class Grid {
public:
    static bool inside(const Cell& c) {
        return c.x >= 0 && c.x < cfg::GRID_W && c.y >= 0 && c.y < cfg::GRID_H;
    }
    static sf::Vector2f toPixel(const Cell& c) {
        return { c.x * cfg::CELL * 1.f, c.y * cfg::CELL * 1.f };
    }
    static Cell toCell(const sf::Vector2f& pos) {
        return { static_cast<int>(pos.x / cfg::CELL), static_cast<int>(pos.y / cfg::CELL) };
    }
};