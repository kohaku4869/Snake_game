#include "BaseItem.hpp"
#include "Apple.hpp"
#include "BuffItem.hpp"
#include "../grid/Grid.hpp"
#include <memory>

BaseItem::BaseItem(Cell cell, int lifetime_ms)
    : cell_(cell), lifetime_ms_(lifetime_ms), consumed_(false) {
}

sf::Vector2f BaseItem::getPosition() const {
    return Grid::toPixel(cell_) + sf::Vector2f(cfg::CELL / 2.0f, cfg::CELL / 2.0f);
}

float BaseItem::getRadius() const {
    return cfg::ITEM_RADIUS_PX;
}

bool BaseItem::loadTexture(sf::Texture& texture, const std::string& path) {
    try {
        return texture.loadFromFile(path);
    } catch (const std::exception& e) {
        return false;
    }
}

