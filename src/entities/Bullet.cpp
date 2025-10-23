#include "Bullet.hpp"
#include "Snake.hpp"
#include <cmath>
#include "../core/Config.hpp"

static sf::Vector2f polarB(float r, float a){ return { r*std::cos(a), r*std::sin(a) }; }

Bullet::Bullet(int ownerId, sf::Vector2f posPx, float headingRad, BulletType type)
: ownerId_(ownerId), pos_(posPx), heading_rad_(headingRad), type_(type) {}

Bullet::Bullet(int ownerId, sf::Vector2f posPx, float headingRad, BulletType type, int targetId)
: ownerId_(ownerId), pos_(posPx), heading_rad_(headingRad), type_(type), target_snake_id_(targetId) {}

void Bullet::setTarget(const sf::Vector2f& targetPos) {
    target_pos_ = targetPos;
    has_target_ = true;
}

void Bullet::update(int dt_ms) {
	if (!alive_) return;
	
	float speed = cfg::BULLET_SPEED_PX;
	
	// Đạn laser có tốc độ siêu nhanh (3x tốc độ bình thường)
	if (type_ == BulletType::Laser) {
		speed *= 5.0f;
	}
	// Đạn đuổi có tốc độ 50% và tự động đuổi theo mục tiêu
	else if (type_ == BulletType::Homing && has_target_) {
		speed *= homing_speed_factor_;
		
		// Tính hướng đến mục tiêu
		sf::Vector2f direction = target_pos_ - pos_;
		float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
		
		if (distance > 0.1f) { // Tránh chia cho 0
			direction.x /= distance;
			direction.y /= distance;
			heading_rad_ = std::atan2(direction.y, direction.x);
		}
	}
	
	float dist = speed * (dt_ms / 1000.f);
	pos_ += polarB(dist, heading_rad_);
	
	// Kiểm tra va chạm với tường - tất cả đạn đều biến mất khi chạm tường
	if (pos_.x < 0 || pos_.y < 0 || pos_.x >= cfg::WORLD_W_PX || pos_.y >= cfg::WORLD_H_PX) {
		alive_ = false;
	}
}

void Bullet::draw(sf::RenderWindow& win) const {
	if (!alive_) return;
	
	sf::Color bulletColor = sf::Color::White;
	
	// Màu sắc khác nhau cho từng loại đạn
	switch (type_) {
		case BulletType::Normal:
			bulletColor = sf::Color::White;
			break;
		case BulletType::Triple:
			bulletColor = sf::Color::Yellow;
			break;
		case BulletType::Laser:
			bulletColor = sf::Color(0, 0, 255); // Xanh dương đậm
			break;
		case BulletType::Homing:
			bulletColor = sf::Color::Magenta;
			break;
	}
	
	sf::CircleShape r(cfg::BULLET_RADIUS_PX);
	r.setOrigin(cfg::BULLET_RADIUS_PX, cfg::BULLET_RADIUS_PX);
	r.setPosition(pos_);
	r.setFillColor(bulletColor);
	win.draw(r);
}