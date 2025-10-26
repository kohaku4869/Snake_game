#pragma once
#include <vector>
#include "../entities/BaseItem.hpp"
#include "../entities/Snake.hpp"

class Spawner {
public:
    void update(int dt_ms,
                const std::vector<Snake*>& snakes,
                std::vector<std::unique_ptr<BaseItem>>& items);

private:
    int apple_acc_ms_ = 0;
    int buff_acc_ms_ = 0;
};
