#pragma once
#include <SFML/System.hpp>

using Cell = sf::Vector2i;   // ô lưới

enum class BuffType {
    Shield,
    X2,
    Speed,
    TripleShot,
    LaserShot,
    HomingShot
};

enum class BulletType {
    Normal,     // Đạn bình thường
    Triple,     // Đạn 3 tia
    Laser,      // Đạn laser
    Homing      // Đạn đuổi
};