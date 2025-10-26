/**
 * @file BaseItem.hpp
 * @brief Lớp cơ sở BaseItem - đại diện cho tất cả các vật phẩm trong game
 * @details Lớp cơ sở cho tất cả các loại item (táo, buff) với các phương thức ảo
 */

#pragma once
#include <SFML/Graphics.hpp>
#include "../core/Types.hpp"
#include "../core/Config.hpp"
#include <memory>
/**
 * @class BaseItem
 * @brief Lớp cơ sở cho tất cả các item trong game
 * @details Cung cấp interface chung cho tất cả các loại item
 */
class BaseItem {
public:
    BaseItem(Cell cell, int lifetime_ms = 0);
    virtual ~BaseItem() = default;

    // Các phương thức ảo để các lớp con override
    virtual void update(int dt_ms) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual bool isAlive() const = 0;
    virtual void consume() = 0;
    virtual bool isConsumed() const = 0;

    // Các phương thức chung
    sf::Vector2f getPosition() const;
    float getRadius() const;
    Cell getCell() const { return cell_; }

    // Factory method để tạo item theo loại
    // static std::unique_ptr<BaseItem> createItem(int itemType, Cell cell, int lifetime_ms = 0);

protected:
    Cell cell_;
    int lifetime_ms_;
    bool consumed_ = false;

    // Helper method để load texture
    static bool loadTexture(sf::Texture& texture, const std::string& path);
};

