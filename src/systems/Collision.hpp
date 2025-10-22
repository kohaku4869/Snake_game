#pragma once
#include <deque>
#include <SFML/Graphics.hpp>
#include "../core/Types.hpp"
#include "../entities/Snake.hpp"
#include "../entities/Bullet.hpp"
#include "../entities/Item.hpp"
#include <vector>

namespace collision {
	// đạn trúng rắn? trả về (snake*, hitHead?) theo khoảng cách hình tròn
	std::pair<Snake*, bool> bulletHitSnake(const Bullet& b, std::vector<Snake*>& snakes);

	// xử lý đạn laser - gây sát thương tức thì trên đường thẳng
	void handleLaserShot(Snake& shooter, std::vector<Snake*>& snakes);
	
	// vẽ tia laser màu xanh
	void drawLaserShot(sf::RenderWindow& window, Snake& shooter);

	// rắn ăn item theo khoảng cách
	int snakeEatItem(Snake& s, std::vector<Item>& items);

	// rắn đụng tường/thân?
	void snakeSelfAndWall(Snake& s, const std::vector<Snake*>& snakes, int penalty);
}
