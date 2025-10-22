#include "Spawner.hpp"
#include "../core/Config.hpp"
#include <random>
#include <unordered_set>

static std::mt19937& rng() {
	static std::random_device rd; static std::mt19937 gen(rd());
	return gen;
}

static Cell randomFreeCell(const std::vector<Snake*>& snakes,
const std::vector<Item>& items) {
	std::unordered_set<long long> occ;
	auto key = [](const Cell& c){ return (long long)c.x << 32 | (unsigned)c.y; };
	for (auto s : snakes) {
		for (auto& p : s->bodyPx()) {
			Cell c{ (int)std::floor(p.x / cfg::CELL), (int)std::floor(p.y / cfg::CELL) };
			occ.insert(key(c));
		}
	}
	for (const auto& it : items) {
		if (it.isAlive()) {
			occ.insert(key(it.getCell()));
		}
	}

	std::uniform_int_distribution<int> dx(0, cfg::GRID_W-1), dy(0, cfg::GRID_H-1);
	for (int tries = 0; tries < 2000; ++tries) {
		Cell c{dx(rng()), dy(rng())};
		if (!occ.count(key(c))) return c;
	}
	return {1,1}; // fallback
}

void Spawner::update(int dt_ms, const std::vector<Snake*>& snakes, std::vector<Item>& items) {
	// Cập nhật thời gian sống của buff và xóa buff hết hạn
	for (auto it = items.begin(); it != items.end();) {
		it->update(dt_ms);
		if (!it->isAlive()) {
			it = items.erase(it);
			continue;
		}
		++it;
	}

	// Đếm số táo và buff hiện tại
	int apple_count = 0;
	int buff_count = 0;
	for (const auto& item : items) {
		if (item.getKind() == ItemKind::Apple) {
			apple_count++;
		} else {
			buff_count++;
		}
	}

	// Spawn táo (2s một quả, tối đa 3 quả)
	apple_acc_ms_ += dt_ms;
	if (apple_acc_ms_ >= cfg::APPLE_SPAWN_EVERY_MS && apple_count < cfg::MAX_APPLES_ON_SCREEN) {
		apple_acc_ms_ = 0;
		Cell cell = randomFreeCell(snakes, items);
		
		// Chỉ spawn táo thường
		items.emplace_back(ItemKind::Apple, cell, 0);
	}

	// Spawn buff (10s một buff, tối đa 1 buff)
	buff_acc_ms_ += dt_ms;
	if (buff_acc_ms_ >= cfg::BUFF_SPAWN_EVERY_MS && buff_count < cfg::MAX_BUFFS_ON_SCREEN) {
		buff_acc_ms_ = 0;
		Cell cell = randomFreeCell(snakes, items);
		
		// Chọn loại buff ngẫu nhiên (bao gồm cả buff đạn)
		std::uniform_int_distribution<int> t(0, 5);
		int k = t(rng());
		ItemKind kind;
		switch (k) {
			case 0: kind = ItemKind::Shield; break;
			case 1: kind = ItemKind::X2; break;
			case 2: kind = ItemKind::Speed; break;
			case 3: kind = ItemKind::TripleShot; break;
			case 4: kind = ItemKind::LaserShot; break;
			case 5: kind = ItemKind::HomingShot; break;
			default: kind = ItemKind::Shield; break;
		}
		
		items.emplace_back(kind, cell, cfg::BUFF_LIFETIME_MS);
	}
}