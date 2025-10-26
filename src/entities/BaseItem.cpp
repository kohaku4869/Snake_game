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

std::unique_ptr<BaseItem> BaseItem::createItem(int itemType, Cell cell, int lifetime_ms) {
    switch (itemType) {
        case 0: // Apple
            return std::make_unique<Apple>(cell);
        case 1: // Shield
            return std::make_unique<BuffItem>(BuffType::Shield, cell, lifetime_ms);
        case 2: // X2
            return std::make_unique<BuffItem>(BuffType::X2, cell, lifetime_ms);
        case 3: // Speed
            return std::make_unique<BuffItem>(BuffType::Speed, cell, lifetime_ms);
        case 4: // TripleShot
            return std::make_unique<BuffItem>(BuffType::TripleShot, cell, lifetime_ms);
        case 5: // LaserShot
            return std::make_unique<BuffItem>(BuffType::LaserShot, cell, lifetime_ms);
        case 6: // HomingShot
            return std::make_unique<BuffItem>(BuffType::HomingShot, cell, lifetime_ms);
        default:
            return std::make_unique<Apple>(cell);
    }
}

