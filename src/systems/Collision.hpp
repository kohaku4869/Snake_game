#pragma once
#include <deque>
#include <SFML/Graphics.hpp>
#include "../core/Types.hpp"
#include "../entities/Snake.hpp"
#include "../entities/Bullet.hpp"
#include "../entities/BaseItem.hpp"
#include <vector>

namespace collision {
	// đạn trúng rắn? trả về (snake*, hitHead?) theo khoảng cách hình tròn
	std::pair<Snake*, bool> bulletHitSnake(const Bullet& b, std::vector<Snake*>& snakes);

	// rắn ăn item theo khoảng cách
	int snakeEatItem(Snake& s, std::vector<std::unique_ptr<BaseItem>>& items);

	// rắn đụng tường/thân?
	void snakeSelfAndWall(Snake& s, const std::vector<Snake*>& snakes, int penalty);
}
