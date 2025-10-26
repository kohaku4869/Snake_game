/**
 * @file Apple.hpp
 * @brief Lớp Apple - kế thừa từ BaseItem
 * @details Đại diện cho táo trong game, có thể tăng độ dài rắn
 */

#pragma once
#include "BaseItem.hpp"

/**
 * @class Apple
 * @brief Lớp đại diện cho táo trong game
 * @details Kế thừa từ BaseItem, có thể tăng độ dài rắn khi ăn
 */
class Apple : public BaseItem {
public:
    Apple(Cell cell);
    virtual ~Apple() = default;
    
    // Override các phương thức ảo
    void update(int dt_ms) override;
    void draw(sf::RenderWindow& window) const override;
    bool isAlive() const override;
    void consume() override;
    bool isConsumed() const override;
    
    // Phương thức riêng của Apple
    int getGrowthValue() const { return 1; }

private:
    static sf::Texture apple_texture_;
    static bool texture_loaded_;
    
    void loadTexture();
};

