/**
 * @file BuffItem.hpp
 * @brief Lớp BuffItem - kế thừa từ BaseItem
 * @details Đại diện cho các buff trong game (Shield, X2, Speed, etc.)
 */

#pragma once
#include "BaseItem.hpp"
#include "core/Types.hpp"
/**
 * @class BuffItem
 * @brief Lớp đại diện cho các buff trong game
 * @details Kế thừa từ BaseItem, có thời gian sống và hiệu ứng đặc biệt
 */
class BuffItem : public BaseItem {
public:
    BuffItem(BuffType type, Cell cell, int lifetime_ms);
    virtual ~BuffItem() = default;
    
    // Override các phương thức ảo
    void update(int dt_ms) override;
    void draw(sf::RenderWindow& window) const override;
    bool isAlive() const override;
    void consume() override;
    bool isConsumed() const override;
    
    // Phương thức riêng của BuffItem
    BuffType getBuffType() const { return buff_type_; }
    sf::Color getColor() const;

private:
    BuffType buff_type_;
    
    // Textures cho các loại buff
    static sf::Texture shield_texture_;
    static sf::Texture x2_texture_;
    static sf::Texture speed_texture_;
    static sf::Texture triple_shot_texture_;
    static sf::Texture laser_shot_texture_;
    static sf::Texture homing_shot_texture_;
    
    static bool textures_loaded_;
    
    void loadTextures();
    sf::Texture* getTexture() const;
};

