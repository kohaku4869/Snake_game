#include "Item.hpp"
#include "../grid/Grid.hpp"
#include <cmath>

// Khởi tạo static members
sf::Texture Item::apple_texture_;
sf::Texture Item::shield_texture_;
sf::Texture Item::x2_texture_;
sf::Texture Item::speed_texture_;
sf::Texture Item::triple_shot_texture_;
sf::Texture Item::laser_shot_texture_;
sf::Texture Item::homing_shot_texture_;
bool Item::textures_initialized_ = false;

Item::Item(ItemKind kind, Cell cell, int lifetime_ms)
    : kind_(kind), cell_(cell), lifetime_ms_(lifetime_ms), consumed_(false) {
    // Khởi tạo textures nếu chưa được khởi tạo
    if (!textures_initialized_) {
        initializeTextures();
    }
}

void Item::initializeTextures() {
    if (textures_initialized_) return;
    
    // Load textures cho các loại item từ file hình ảnh
    apple_texture_.loadFromFile("assets/NewTexture/FoodApple.png");
    
    // Load textures cho buff items từ file hình ảnh
    shield_texture_.loadFromFile("assets/Item/Shield.png");
    x2_texture_.loadFromFile("assets/Item/X2.png");
    speed_texture_.loadFromFile("assets/Item/SpeedBoost.png");
    triple_shot_texture_.loadFromFile("assets/Item/TripleShot.png");
    laser_shot_texture_.loadFromFile("assets/Item/LaserBeam.png");
    homing_shot_texture_.loadFromFile("assets/Item/HomingShot.png");
    
    textures_initialized_ = true;
}

void Item::update(int dt_ms) {
    if (consumed_) return;
    
    // Chỉ buff items mới có thời gian sống
    if (kind_ != ItemKind::Apple && lifetime_ms_ > 0) {
        lifetime_ms_ -= dt_ms;
        if (lifetime_ms_ <= 0) {
            consumed_ = true;
        }
    }
}

void Item::draw(sf::RenderWindow& window) const {
    if (consumed_) return;
    
    sf::Vector2f pos = getPosition();
    
    if (kind_ == ItemKind::Apple) {
        // Vẽ táo với texture
        sf::Sprite sprite;
        sprite.setTexture(apple_texture_);
        
        // Scale và position - tăng kích thước táo
        float scale = 5.0f * cfg::ITEM_RADIUS_PX / sprite.getTexture()->getSize().x;
        sprite.setScale(scale, scale);
        sprite.setOrigin(sprite.getTexture()->getSize().x / 2.0f, sprite.getTexture()->getSize().y / 2.0f);
        sprite.setPosition(pos);
        window.draw(sprite);
    } else {
        // Vẽ buff items với texture tương ứng
        sf::Sprite sprite;
        switch (kind_) {
            case ItemKind::Shield:
                sprite.setTexture(shield_texture_);
                break;
            case ItemKind::X2:
                sprite.setTexture(x2_texture_);
                break;
            case ItemKind::Speed:
                sprite.setTexture(speed_texture_);
                break;
            case ItemKind::TripleShot:
                sprite.setTexture(triple_shot_texture_);
                break;
            case ItemKind::LaserShot:
                sprite.setTexture(laser_shot_texture_);
                break;
            case ItemKind::HomingShot:
                sprite.setTexture(homing_shot_texture_);
                break;
            default:
                break;
        }
        
        // Scale và position - tăng kích thước buff items
        float scale = 4.4f * cfg::ITEM_RADIUS_PX / sprite.getTexture()->getSize().x;
        sprite.setScale(scale, scale);
        sprite.setOrigin(sprite.getTexture()->getSize().x / 2.0f, sprite.getTexture()->getSize().y / 2.0f);
        sprite.setPosition(pos);
        window.draw(sprite);
    }
}

bool Item::isAlive() const {
    return !consumed_ && (kind_ == ItemKind::Apple || lifetime_ms_ > 0);
}

sf::Vector2f Item::getPosition() const {
    return Grid::toPixel(cell_) + sf::Vector2f(cfg::CELL / 2.0f, cfg::CELL / 2.0f);
}

float Item::getRadius() const {
    return cfg::ITEM_RADIUS_PX;
}

sf::Color Item::getColor() const {
    switch (kind_) {
        case ItemKind::Apple: return sf::Color::Red;
        case ItemKind::Shield: return sf::Color::Yellow;
        case ItemKind::X2: return sf::Color::Magenta;
        case ItemKind::Speed: return sf::Color::Blue;
        case ItemKind::TripleShot: return sf::Color::Yellow;
        case ItemKind::LaserShot: return sf::Color::Red;
        case ItemKind::HomingShot: return sf::Color::Magenta;
        default: return sf::Color::White;
    }
}
