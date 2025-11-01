#pragma once
#include <SFML/Graphics.hpp>
#include "../entities/Snake.hpp"

class Hud {
public:
    Hud();
    void draw(sf::RenderWindow& win, const Snake& s1, const Snake& s2, int msLeft);
    void drawStartScreen(sf::RenderWindow& window_,const sf::Texture& start_screen_texture_,int max_length);
    void drawWinnerScreen(sf::RenderWindow& window_,int winner,int score_p1,int score_p2);
private:
    // Textures

    // sf::RenderWindow window_;
    //Fonts
    sf::Font font_;
};
