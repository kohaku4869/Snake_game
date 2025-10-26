#include "Collision.hpp"
#include "../core/Config.hpp"
#include "../entities/Apple.hpp"
#include "../entities/BuffItem.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

static float dist2(const sf::Vector2f& a, const sf::Vector2f& b){ float dx=a.x-b.x, dy=a.y-b.y; return dx*dx+dy*dy; }

std::pair<Snake*, bool> collision::bulletHitSnake(const Bullet& b, std::vector<Snake*>& snakes) {
	if (!b.alive()) return {nullptr, false};
	const sf::Vector2f bp = b.pos();
	const float sumR2_head = (cfg::BULLET_RADIUS_PX + cfg::SNAKE_RADIUS_PX) * (cfg::BULLET_RADIUS_PX + cfg::SNAKE_RADIUS_PX);
	
	for (auto* s : snakes) {
		if (s->id() == b.owner()) continue;
		const auto& body = s->bodyPx();
		if (body.empty()) continue;
		// head
		if (dist2(body.front(), bp) <= sumR2_head) return {s, true};
		// body
		for (size_t i = 1; i < body.size(); ++i) {
			if (dist2(body[i], bp) <= sumR2_head) return {s, false};
		}
	}
	return {nullptr, false};
}

int collision::snakeEatItem(Snake& s, std::vector<std::unique_ptr<BaseItem>>& items) {
	int eaten = 0;
	if (!s.alive()) return eaten;
	sf::Vector2f h = s.headPx();
	float rad = cfg::SNAKE_RADIUS_PX + cfg::ITEM_RADIUS_PX;
	float rad2 = rad*rad;
	for (size_t i = 0; i < items.size();) {
		if (!items[i]->isAlive()) {
			++i;
			continue;
		}
		
		sf::Vector2f ip = items[i]->getPosition();
		if (dist2(ip, h) <= rad2) {
			// Sử dụng dynamic_cast để xác định loại item
			if (auto* apple = dynamic_cast<Apple*>(items[i].get())) {
				// Táo thường = tăng độ dài
				s.grow(s.hasX2() ? 2 : 1);
			} else if (auto* buffItem = dynamic_cast<BuffItem*>(items[i].get())) {
				// Xử lý buff items
				switch (buffItem->getBuffType()) {
					case BuffType::Shield: s.giveShield(1); break;
					case BuffType::X2: s.giveX2(30000); break;
					case BuffType::Speed: s.giveSpeed(5000); break;
					case BuffType::TripleShot: s.giveTripleShot(3); break;
					case BuffType::LaserShot: s.giveLaserShot(3); break;
					case BuffType::HomingShot: s.giveHomingShot(3); break;
				}
			}
			items[i]->consume();
			items.erase(items.begin() + i);
			++eaten;
		} else ++i;
	}
	return eaten;
}

void collision::snakeSelfAndWall(Snake& s, const std::vector<Snake*>& snakes, int penalty) {
	if (!s.alive()) return;
	sf::Vector2f h = s.headPx();
	// tường - teleport qua tường đối diện thay vì phạt
	if (h.x < 0) { 
		// Teleport từ bên trái sang bên phải
		sf::Vector2f newPos = {cfg::WORLD_W_PX - cfg::CELL, h.y};
		s.teleportTo(newPos);
		return; 
	}
	if (h.x > cfg::WORLD_W_PX) { 
		// Teleport từ bên phải sang bên trái
		sf::Vector2f newPos = {cfg::CELL, h.y};
		s.teleportTo(newPos);
		return; 
	}
	if (h.y < 0) { 
		// Teleport từ trên xuống dưới
		sf::Vector2f newPos = {h.x, cfg::WORLD_H_PX - cfg::CELL};
		s.teleportTo(newPos);
		return; 
	}
	if (h.y > cfg::WORLD_H_PX) { 
		// Teleport từ dưới lên trên
		sf::Vector2f newPos = {h.x, cfg::CELL};
		s.teleportTo(newPos);
		return; 
	}
	// đụng thân mình
	const auto& me = s.bodyPx();
	float minGap2 = (cfg::SNAKE_RADIUS_PX * 1.0f) * (cfg::SNAKE_RADIUS_PX * 1.0f);
	// bỏ qua vài đốt đầu để tránh tự-đâm khi vừa khởi động/quay gắt
	for (size_t i = 6; i < me.size(); ++i) {
		if (dist2(me[i], h) <= minGap2) { s.collidePenalty(penalty); return; }
	}
	// đụng rắn khác
	for (auto* other : snakes) {
		if (other == &s) continue;
		for (const auto& p : other->bodyPx()) {
			if (dist2(p, h) <= minGap2) { s.collidePenalty(penalty); return; }
		}
	}
}