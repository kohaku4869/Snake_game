/**
 * @file Item.hpp
 * @brief Lớp Item - đại diện cho các vật phẩm trong game (táo, buff)
 * @details Bao gồm táo thường và các buff: Shield, X2, Speed, TripleShot, LaserShot, HomingShot
 */

#pragma once
#include <SFML/Graphics.hpp>
#include "../core/Types.hpp"
#include "../core/Config.hpp"

// Các loại item trong game
enum class ItemKind { 
    Apple,      // Táo thường - tăng độ dài rắn
    Shield,     // Khiên - bảo vệ khỏi sát thương
    X2,         // Nhân đôi - ăn táo được +2 đốt thay vì +1
    Speed,      // Tăng tốc - di chuyển nhanh hơn
    TripleShot, // Đạn 3 tia - bắn 3 viên đạn cùng lúc
    LaserShot,  // Đạn laser - tia sát thương tức thì
    HomingShot  // Đạn đuổi - đạn tự tìm mục tiêu
};

/**
 * @class Item
 * @brief Lớp đại diện cho một item trong game
 * @details Mỗi item có vị trí, loại, thời gian sống (đối với buff) và trạng thái
 */
class Item {
public:
    Item(ItemKind kind, Cell cell, int lifetime_ms = 0);
    
    // Cập nhật trạng thái item (chủ yếu cho buff có thời gian sống)
    void update(int dt_ms);
    
    // Vẽ item lên màn hình
    void draw(sf::RenderWindow& window) const;
    
    // Kiểm tra item còn sống không (chỉ buff mới có thời gian sống)
    bool isAlive() const;
    
    // Lấy vị trí pixel của item
    sf::Vector2f getPosition() const;
    
    // Lấy bán kính va chạm
    float getRadius() const;
    
    // Lấy màu sắc để vẽ
    sf::Color getColor() const;
    
    // Lấy loại item
    ItemKind getKind() const { return kind_; }
    
    // Lấy cell position
    Cell getCell() const { return cell_; }
    
    // Đánh dấu item đã bị ăn
    void consume() { consumed_ = true; }
    bool isConsumed() const { return consumed_; }

private:
    ItemKind kind_;
    Cell cell_;
    int lifetime_ms_;
    bool consumed_ = false;
    
    // Textures cho các loại item khác nhau
    static sf::Texture apple_texture_;
    static sf::Texture shield_texture_;
    static sf::Texture x2_texture_;
    static sf::Texture speed_texture_;
    static sf::Texture triple_shot_texture_;
    static sf::Texture laser_shot_texture_;
    static sf::Texture homing_shot_texture_;
    
    // Khởi tạo textures (chỉ gọi một lần)
    static void initializeTextures();
    static bool textures_initialized_;
};
