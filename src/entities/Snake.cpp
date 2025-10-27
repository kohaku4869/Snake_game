#include "Snake.hpp"
#include "Bullet.hpp"
#include "../core/Config.hpp"
#include "../grid/Grid.hpp"
#include <cmath>
#include <map>
#include <set>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {
    float cellsPerSec(const Snake& s) {
        float base = cfg::SNAKE_SPEED_CPS;
        float factor = 1.f;
        return base * factor;
    }
}

Snake::Snake(int id, sf::Color color, Cell start, Dir d, int init_len)
: id_(id), color_(color), length_(init_len) {
    // Load textures based on snake color
    try {
        if (id == 1) {
            // Player 1 - Blue snake
            head_texture_.loadFromFile("assets/NewTexture/BlueHead.png");
            body_texture_.loadFromFile("assets/NewTexture/BlueBody.png");
        } else if (id == 2) {
            // Player 2 - Yellow snake
            head_texture_.loadFromFile("assets/NewTexture/YellowHead.png");
            body_texture_.loadFromFile("assets/NewTexture/YellowBody.png");
        } else {
            // Default - Green snake
            head_texture_.loadFromFile("assets/Textures/SnakeHead.png");
            body_texture_.loadFromFile("assets/Textures/SnakeBody.png");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading snake textures for player " << id << ": " << e.what() << std::endl;
    }
    
    // Khởi tạo body_px_ từ vị trí grid
    body_px_.clear();
    sf::Vector2f start_px = Grid::toPixel(start);
    body_px_.push_front(start_px);
    
    // Tạo thân rắn theo hướng ngược lại
    for (int i = 1; i < init_len; ++i) {
        sf::Vector2f prev = body_px_.back();
        sf::Vector2f offset;
        switch (d) {
            case Dir::Right: offset = {-cfg::CELL, 0}; break;
            case Dir::Left: offset = {cfg::CELL, 0}; break;
            case Dir::Down: offset = {0, -cfg::CELL}; break;
            case Dir::Up: offset = {0, cfg::CELL}; break;
        }
        body_px_.push_back(prev + offset);
    }
    
    // Khởi tạo hướng di chuyển
    switch (d) {
        case Dir::Up: heading_rad_ = -M_PI/2; break;
        case Dir::Down: heading_rad_ = M_PI/2; break;
        case Dir::Left: heading_rad_ = M_PI; break;
        case Dir::Right: heading_rad_ = 0; break;
    }
    
    segment_spacing_px_ = cfg::SNAKE_RADIUS_PX * 1.6f; // Giảm khoảng cách để thân khớp nhau hơn
}

void Snake::setKeyMap(sf::Keyboard::Key up, sf::Keyboard::Key down,
sf::Keyboard::Key left, sf::Keyboard::Key right,
sf::Keyboard::Key shoot) {
    keys.up = up; keys.down = down; keys.left = left; keys.right = right; keys.shoot = shoot;
}

void Snake::handleInput() {
    // Xử lý input để thay đổi hướng di chuyển
    float turn_speed = cfg::SNAKE_TURN_DEG_PER_S * M_PI / 180.f; // chuyển độ/s thành rad/s
    float dt = 1.0f / cfg::FPS; // sử dụng FPS thực tế từ config
    
    if (sf::Keyboard::isKeyPressed(keys.left)) {
        heading_rad_ -= turn_speed * dt;
    }
    if (sf::Keyboard::isKeyPressed(keys.right)) {
        heading_rad_ += turn_speed * dt;
    }
    
    // Xử lý boost (phím down cho P2, phím S cho P1)
    bool boost_pressed = sf::Keyboard::isKeyPressed(keys.down);
    if (boost_pressed && !prev_boost_pressed_) {
        startBoost();
    } else if (!boost_pressed && prev_boost_pressed_) {
        stopBoost();
    }
    prev_boost_pressed_ = boost_pressed;
    
    // Xử lý bắn (edge trigger)
    bool shoot_pressed = sf::Keyboard::isKeyPressed(keys.shoot);
    if (shoot_pressed && !prev_shoot_pressed_) {
        shoot_edge_ = true;
    }
    prev_shoot_pressed_ = shoot_pressed;
}

void Snake::tickBuffs(int dt_ms) {
    if (buffs_.x2_ms > 0) {
        buffs_.x2_ms -= dt_ms;
        if (buffs_.x2_ms < 0) buffs_.x2_ms = 0;
    }
    if (buffs_.speed_ms > 0) {
        buffs_.speed_ms -= dt_ms;
        if (buffs_.speed_ms < 0) buffs_.speed_ms = 0;
    }
    
    // Buff đạn không cần tick vì sử dụng hệ thống đạn
    
    // Xử lý boost tracking
    if (is_boosting_) {
        current_boost_session_ms_ += dt_ms;
        total_boost_time_ms_ += dt_ms;
        
        // Kiểm tra nếu vượt quá 3 giây boost
        if (total_boost_time_ms_ >= 3000) {
            // Phạt -1 độ dài và reset boost time
            shrink(1);
            total_boost_time_ms_ = 0;
        }
    }
}

void Snake::update(int dt_ms) {
    if (!alive_) return;

    // cooldown bắn
    if (shoot_cd_ms_ > 0) {
        shoot_cd_ms_ -= dt_ms;
        if (shoot_cd_ms_ < 0) shoot_cd_ms_ = 0;
    }

    // tốc độ di chuyển theo buff speed và boost
    float speed_multiplier = 1.f;
    if (buffs_.speed_ms > 0) speed_multiplier *= cfg::SPEED_BOOST_FACTOR;
    if (is_boosting_) speed_multiplier *= 1.5f; // Boost tạm thời tăng 50% tốc độ
    
    float speed_px_per_s = cfg::SNAKE_SPEED_PX * speed_multiplier;
    float pixelsToMove = (speed_px_per_s * dt_ms) / 1000.f;
    move_acc_px_ += pixelsToMove;

    // Di chuyển đầu rắn
    if (move_acc_px_ >= segment_spacing_px_) {
        int steps = (int)(move_acc_px_ / segment_spacing_px_);
        move_acc_px_ -= steps * segment_spacing_px_;
        
        for (int i = 0; i < steps && alive_; ++i) {
            sf::Vector2f head = body_px_.front();
            sf::Vector2f new_head = head + sf::Vector2f(
                std::cos(heading_rad_) * segment_spacing_px_,
                std::sin(heading_rad_) * segment_spacing_px_
            );
            
            body_px_.push_front(new_head);
            
            if (pending_grow_ > 0) {
                --pending_grow_;
            } else if (!body_px_.empty()) {
                body_px_.pop_back();
            }
            
            length_ = (int)body_px_.size();
            if (length_ <= 0) { alive_ = false; break; }
        }
    }
}

void Snake::draw(sf::RenderWindow& win) const {
    if (!alive_) return;
    for (size_t i = 0; i < body_px_.size(); ++i) {
        if (i == 0) {
            // Vẽ đầu rắn với texture và xoay theo hướng di chuyển
            sf::Sprite head_sprite(head_texture_);
            head_sprite.setPosition(body_px_[i] - sf::Vector2f(cfg::SNAKE_RADIUS_PX, cfg::SNAKE_RADIUS_PX));
            head_sprite.setScale(2.0f * cfg::SNAKE_RADIUS_PX / head_texture_.getSize().x, 
                                2.0f * cfg::SNAKE_RADIUS_PX / head_texture_.getSize().y);
            
            // Xoay đầu rắn theo hướng di chuyển
            head_sprite.setRotation(heading_rad_ * 180.0f / M_PI);
            head_sprite.setOrigin(head_texture_.getSize().x / 2.0f, head_texture_.getSize().y / 2.0f);
            head_sprite.setPosition(body_px_[i]);
            
            win.draw(head_sprite);
        } else {
            // Vẽ thân rắn với texture và xoay theo hướng từ segment trước
            sf::Sprite body_sprite(body_texture_);
            
            // Tính hướng từ segment hiện tại đến segment trước (ngược lại để khớp)
            float angle = 0.0f;
            if (i > 0) {
                sf::Vector2f direction = body_px_[i] - body_px_[i-1];
                if (direction.x != 0 || direction.y != 0) {
                    angle = std::atan2(direction.y, direction.x) * 180.0f / M_PI;
                }
            }
            
            body_sprite.setScale(2.0f * cfg::SNAKE_RADIUS_PX / body_texture_.getSize().x, 
                               2.0f * cfg::SNAKE_RADIUS_PX / body_texture_.getSize().y);
            body_sprite.setRotation(angle);
            body_sprite.setOrigin(body_texture_.getSize().x / 2.0f, body_texture_.getSize().y / 2.0f);
            body_sprite.setPosition(body_px_[i]);
            
            win.draw(body_sprite);
        }
    }
}

void Snake::grow(int k) {
    if (k <= 0) return;
    pending_grow_ += k;
}

void Snake::shrink(int k) {
    if (k <= 0 || !alive_) return;
    // khiên chặn một lần trừ duy nhất
    if (buffs_.shield > 0) {
        --buffs_.shield;
        return;
    }
    for (int i = 0; i < k && !body_px_.empty(); ++i) {
        body_px_.pop_back();
    }
    length_ = (int)body_px_.size();
    if (length_ <= 1) alive_ = false;
}

bool Snake::tryShoot() {
    if (!alive_) return false;
    if (shoot_cd_ms_ > 0) return false;
    if (!shoot_edge_) return false;
    
    // bắn tiêu hao đuôi, không bị khiên chặn
    if (length_ <= cfg::SHOOT_COST + 1) return false;
    
    // Kiểm tra xem có buff đạn nào đang hoạt động không
    bool hasAnyBulletBuff = hasTripleShot() || hasLaserShot() || hasHomingShot();
    
    // Nếu không có buff đạn, kiểm tra lại độ dài trước khi bắn
    if (!hasAnyBulletBuff && length_ <= cfg::SHOOT_COST + 1) return false;
    
    shoot_cd_ms_ = cfg::SHOOT_COOLDOWN_MS;
    shoot_edge_ = false; // reset edge trigger

    return true;
}

void Snake::collidePenalty(int k) {
    shrink(k);
}

void Snake::teleportTo(const sf::Vector2f& newPos) {
    if (!alive_ || body_px_.empty()) return;
    
    // Tính toán offset từ vị trí cũ đến vị trí mới
    sf::Vector2f offset = newPos - body_px_.front();
    
    // Di chuyển toàn bộ thân rắn theo offset
    for (auto& pos : body_px_) {
        pos += offset;
    }
}

void Snake::startBoost() {
    if (!alive_) return;
    is_boosting_ = true;
    current_boost_session_ms_ = 0;
}

void Snake::stopBoost() {
    is_boosting_ = false;
    current_boost_session_ms_ = 0;
}

void Snake::resetBoostTime() {
    is_boosting_ = false;
    total_boost_time_ms_ = 0;
    current_boost_session_ms_ = 0;
}


std::vector<Bullet> Snake::createBullets() {
    std::vector<Bullet> bullets;
    
    if (!alive_) return bullets;
    
    // Sử dụng STL containers để quản lý bullet types
    std::map<BulletType, int> bulletAmmo = {
        {BulletType::Laser, buffs_.laser_shot_ammo},
        {BulletType::Homing, buffs_.homing_shot_ammo},
        {BulletType::Triple, buffs_.triple_shot_ammo}
    };
    
    // Sử dụng set để xác định loại đạn ưu tiên
    std::set<BulletType> availableTypes;
    for (const auto& [type, ammo] : bulletAmmo) {
        if (ammo > 0) {
            availableTypes.insert(type);
        }
    }
    
    sf::Vector2f headPos = headPx();
    float heading = headingRad();
    
    // Xử lý theo thứ tự ưu tiên
    if (availableTypes.count(BulletType::Laser)) {
        // Laser shot - ưu tiên cao nhất
        useLaserShot();
        bullets.emplace_back(id_, headPos, heading, BulletType::Laser);
    } else if (availableTypes.count(BulletType::Homing)) {
        // Homing shot - ưu tiên thứ hai
        useHomingShot();
        bullets.emplace_back(id_, headPos, heading, BulletType::Homing);
    } else if (availableTypes.count(BulletType::Triple)) {
        // Triple shot - ưu tiên thứ ba
        useTripleShot();
        // Tạo 3 viên đạn với góc khác nhau
        float angleOffset = M_PI / 6; // 30 độ
        for (int i = 0; i < 3; ++i) {
            float angle = heading + (i - 1) * angleOffset;
            bullets.emplace_back(id_, headPos, angle, BulletType::Triple);
        }
    } else {
        // Đạn bình thường
        bullets.emplace_back(id_, headPos, heading, BulletType::Normal);
    }
    
    return bullets;
}