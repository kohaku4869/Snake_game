#include "Apple.hpp"
#include <cmath>
#include <iostream>
// Khởi tạo static members
sf::Texture Apple::apple_texture_;
bool Apple::texture_loaded_ = false;

Apple::Apple(Cell cell) : BaseItem(cell, 0) {
    loadTexture();
}

void Apple::loadTexture() {
    if (!texture_loaded_) {
        try {
            if (BaseItem::loadTexture(apple_texture_, "assets/NewTexture/FoodApple.png")) {
                texture_loaded_ = true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error loading apple texture: " << e.what() << std::endl;
        }
    }
}

void Apple::update(int dt_ms) {
    // Apple không cần update gì đặc biệt
}

void Apple::draw(sf::RenderWindow& window) const {
    if (consumed_) return;

    sf::Vector2f pos = getPosition();

    if (texture_loaded_) {
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
        // Fallback: vẽ hình tròn đỏ
        sf::CircleShape circle(cfg::ITEM_RADIUS_PX);
        circle.setFillColor(sf::Color::Red);
        circle.setOrigin(cfg::ITEM_RADIUS_PX, cfg::ITEM_RADIUS_PX);
        circle.setPosition(pos);
        window.draw(circle);
    }
}

bool Apple::isAlive() const {
    return !consumed_;
}

void Apple::consume() {
    consumed_ = true;
}

bool Apple::isConsumed() const {
    return consumed_;
}
