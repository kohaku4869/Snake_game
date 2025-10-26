#pragma once
#include <SFML/Graphics.hpp>
#include "../core/Types.hpp"

// Forward declaration để tránh circular dependency
class Snake;



class Bullet {
public:
	Bullet(int ownerId, sf::Vector2f posPx, float headingRad, BulletType type = BulletType::Normal);
	Bullet(int ownerId, sf::Vector2f posPx, float headingRad, BulletType type, int targetId);

	void update(int dt_ms);
	void draw(sf::RenderWindow& win) const;
	bool alive() const { return alive_; }
	int owner() const { return ownerId_; }
	void kill() { alive_ = false; }
	const sf::Vector2f& pos() const { return pos_; }
	BulletType type() const { return type_; }
	
	// Cho đạn đuổi
	void setTarget(const sf::Vector2f& targetPos);
	void setTargetSnake(int snakeId) { target_snake_id_ = snakeId; }
	void updateHomingTarget(const std::vector<Snake*>& snakes);

private:
	int ownerId_;
	sf::Vector2f pos_;   // pixel
	float heading_rad_ = 0.f;
	bool alive_ = true;
	BulletType type_ = BulletType::Normal;
	
	// Cho đạn đuổi
	int target_snake_id_ = -1;
	sf::Vector2f target_pos_;
	bool has_target_ = false;
};
