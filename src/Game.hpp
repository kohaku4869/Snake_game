#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include "entities/Snake.hpp"
#include "entities/Bullet.hpp"
#include "entities/BaseItem.hpp"
#include "entities/Apple.hpp"
#include "entities/BuffItem.hpp"
#include "systems/Spawner.hpp"
#include "systems/SoundManager.hpp"
#include "ui/Hud.hpp"

enum class State { Start, Menu, InGame, Result };
struct Vec2iLess {
    bool operator()(const sf::Vector2i& a, const sf::Vector2i& b) const noexcept {
        if (a.x != b.x) return a.x < b.x;   // so sánh theo x trước
        return a.y < b.y;                   // rồi theo y
    }
};

class Game {
public:
    Game();
    void run();

private:
    void resetMatch();
    void update(int dt_ms);
    void draw();
    int getWinner() const; // 1 = P1 win, 2 = P2 win, 0 = tie
    void initPlayers();

    // File I/O functions for max length
    void saveMaxLength(int length);
    int loadMaxLength();
    
    // STL container management functions
    void updatePlayerScores();
    void updateOccupiedCells();
    void addGameEvent(const std::string& event);
    void processGameEvents();
    
    // Item management functions using inheritance
    void createAppleAt(Cell cell);
    void createBuffAt(BuffType type, Cell cell, int lifetime_ms);
    void removeDeadItems();
    void handleItemCreation(); // Tạo items thủ công
    
    // Helper functions để giảm code trùng lặp
    void handleShooting(Snake& shooter, Snake& target);
    void updateBullets(int dt_ms);
    void updateItems(int dt_ms);

    sf::RenderWindow window_;
    State state_ = State::Menu;

    Snake p1_;
    Snake p2_;
    std::vector<Bullet> bullets_;
    std::vector<std::unique_ptr<BaseItem>> items_;
    Spawner spawner_;
    Hud hud_;
    SoundManager sound_manager_;
    
    // STL containers để quản lý dữ liệu game
    std::map<int, int> player_scores_;      // Map lưu điểm số của từng player
    std::set<sf::Vector2i, Vec2iLess> occupied_cells_;
    std::queue<std::string> game_events_;        // Queue lưu các sự kiện game
    
    // Textures
    sf::Texture background_texture_;
    sf::Texture start_screen_texture_;

    int round_ms_left_;
    int max_length_ = 0; // Lưu trữ chiều dài tối đa
};
