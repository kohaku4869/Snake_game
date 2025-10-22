#pragma once
#include <SFML/Graphics.hpp>
#include "../entities/Snake.hpp"

class Hud {
public:
    Hud();
    void draw(sf::RenderWindow& win, const Snake& s1, const Snake& s2, int msLeft);

private:
    sf::Font font_;
};
