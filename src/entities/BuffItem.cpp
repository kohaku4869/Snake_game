#include "BuffItem.hpp"
#include <cmath>
#include <iostream>

// Khởi tạo static members
sf::Texture BuffItem::shield_texture_;
sf::Texture BuffItem::x2_texture_;
sf::Texture BuffItem::speed_texture_;
sf::Texture BuffItem::triple_shot_texture_;
sf::Texture BuffItem::laser_shot_texture_;
sf::Texture BuffItem::homing_shot_texture_;
bool BuffItem::textures_loaded_ = false;

BuffItem::BuffItem(BuffType type, Cell cell, int lifetime_ms)
    : BaseItem(cell, lifetime_ms), buff_type_(type) {
    loadTextures();
}

void BuffItem::loadTextures() {
    if (!textures_loaded_) {
        try {
            BaseItem::loadTexture(shield_texture_, "assets/Item/Shield.png");
            BaseItem::loadTexture(x2_texture_, "assets/Item/X2.png");
            BaseItem::loadTexture(speed_texture_, "assets/Item/SpeedBoost.png");
            BaseItem::loadTexture(triple_shot_texture_, "assets/Item/TripleShot.png");
            BaseItem::loadTexture(laser_shot_texture_, "assets/Item/LaserBeam.png");
            BaseItem::loadTexture(homing_shot_texture_, "assets/Item/HomingShot.png");
            textures_loaded_ = true;
        } catch (const std::exception& e) {
            std::cerr << "Error loading buff textures: " << e.what() << std::endl;
        }
    }
}

sf::Texture* BuffItem::getTexture() const {
    switch (buff_type_) {
        case BuffType::Shield:
            return &shield_texture_;
        case BuffType::X2:
            return &x2_texture_;
        case BuffType::Speed:
            return &speed_texture_;
        case BuffType::TripleShot:
            return &triple_shot_texture_;
        case BuffType::LaserShot:
            return &laser_shot_texture_;
        case BuffType::HomingShot:
            return &homing_shot_texture_;
        default:
            return nullptr;
    }
}

void BuffItem::update(int dt_ms) {
    if (consumed_) return;

    // Buff items có thời gian sống
    if (lifetime_ms_ > 0) {
        lifetime_ms_ -= dt_ms;
        if (lifetime_ms_ <= 0) {
            consumed_ = true;
        }
    }
}

void BuffItem::draw(sf::RenderWindow& window) const {
    if (consumed_) return;

    sf::Vector2f pos = getPosition();
    sf::Texture* texture = getTexture();

    if (texture && texture->getSize().x > 0) {
        // Vẽ buff với texture
        sf::Sprite sprite;
        sprite.setTexture(*texture);

        // Scale và position - tăng kích thước buff items
        float scale = 4.4f * cfg::ITEM_RADIUS_PX / sprite.getTexture()->getSize().x;
        sprite.setScale(scale, scale);
        sprite.setOrigin(sprite.getTexture()->getSize().x / 2.0f, sprite.getTexture()->getSize().y / 2.0f);
        sprite.setPosition(pos);
        window.draw(sprite);
    } else {
        // Fallback: vẽ hình tròn với màu tương ứng
        sf::CircleShape circle(cfg::ITEM_RADIUS_PX);
        circle.setFillColor(getColor());
        circle.setOrigin(cfg::ITEM_RADIUS_PX, cfg::ITEM_RADIUS_PX);
        circle.setPosition(pos);
        window.draw(circle);
    }
}

bool BuffItem::isAlive() const {
    return !consumed_ && lifetime_ms_ > 0;
}

void BuffItem::consume() {
    consumed_ = true;
}

bool BuffItem::isConsumed() const {
    return consumed_;
}

sf::Color BuffItem::getColor() const {
    switch (buff_type_) {
        case BuffType::Shield: return sf::Color::Yellow;
        case BuffType::X2: return sf::Color::Magenta;
        case BuffType::Speed: return sf::Color::Blue;
        case BuffType::TripleShot: return sf::Color::Yellow;
        case BuffType::LaserShot: return sf::Color::Red;
        case BuffType::HomingShot: return sf::Color::Magenta;
        default: return sf::Color::White;
    }
}
