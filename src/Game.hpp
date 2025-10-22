#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "entities/Snake.hpp"
#include "entities/Bullet.hpp"
#include "entities/Item.hpp"
#include "systems/Spawner.hpp"
#include "systems/SoundManager.hpp"
#include "ui/Hud.hpp"

enum class State { Menu, InGame, Result };

class Game {
public:
    Game();
    void run();

private:
    void resetMatch();
    void update(int dt_ms);
    void draw();
    void drawWinnerScreen();
    int getWinner() const; // 1 = P1 win, 2 = P2 win, 0 = tie
    
    // Helper functions để giảm code trùng lặp
    void handleShooting(Snake& shooter, Snake& target);
    void updateBullets(int dt_ms);
    void updateItems(int dt_ms);

    sf::RenderWindow window_;
    State state_ = State::Menu;

    Snake p1_;
    Snake p2_;
    std::vector<Bullet> bullets_;
    std::vector<Item> items_;
    Spawner spawner_;
    Hud hud_;
    SoundManager sound_manager_;
    
    // Textures
    sf::Texture apple_texture_;
    sf::Texture green_apple_texture_;
    sf::Texture pink_apple_texture_;
    sf::Texture background_texture_;

    int round_ms_left_;
};
