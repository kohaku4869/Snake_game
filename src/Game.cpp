#include "Game.hpp"
#include "core/Config.hpp"
#include "grid/Grid.hpp"
#include "systems/Collision.hpp"
#include "entities/Bullet.hpp"
#include "entities/Apple.hpp"
#include "entities/BuffItem.hpp"
#include "ui/Hud.hpp"
#include <algorithm>
#include <limits>
#include <fstream>
#include <iostream>

Game::Game()
: window_(sf::VideoMode(cfg::GRID_W*cfg::CELL, cfg::GRID_H*cfg::CELL), "Snake Battle"),
  p1_(1, sf::Color::Green, {5, 5}, Dir::Right, cfg::SNAKE_INIT_LEN),
  p2_(2, sf::Color::Cyan,  {cfg::GRID_W-6, cfg::GRID_H-6}, Dir::Left, cfg::SNAKE_INIT_LEN)
{
	window_.setFramerateLimit(cfg::FPS);
	initPlayers();
	
	// Load background
	try {
		background_texture_.loadFromFile("assets/NewTexture/background.jpg");
		// start_screen_texture_.loadFromFile("assets/NewTexture/background.jpg");
	} catch (const std::exception& e) {
		std::cerr << "Error loading background textures: " << e.what() << std::endl;
	}
	
	// Load max length from file
	max_length_ = loadMaxLength();
	
	// Khởi tạo STL containers
	player_scores_[1] = 0;
	player_scores_[2] = 0;
	
	state_ = State::Start; // Bắt đầu với màn hình start
	
	// Phát nhạc theme
	try {
		sound_manager_.playTheme();
	} catch (const std::exception& e) {
		std::cerr << "Error playing theme music: " << e.what() << std::endl;
	}
}
void Game::initPlayers() {
	p1_ = Snake(1, sf::Color::Green, {5, 5}, Dir::Right, cfg::SNAKE_INIT_LEN);
	p2_ = Snake(2, sf::Color::Cyan,  {cfg::GRID_W-6, cfg::GRID_H-6}, Dir::Left, cfg::SNAKE_INIT_LEN);
	p1_.setKeyMap(sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::W);
	p2_.setKeyMap(sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Left, sf::Keyboard::Right, sf::Keyboard::Up);
}

void Game::resetMatch() {
	bullets_.clear();
	items_.clear();
	occupied_cells_.clear();
	round_ms_left_ = cfg::ROUND_TIME_S * 1000;
	
	// Reset rắn về vị trí ban đầu
	initPlayers();
	
	// Reset boost time
	p1_.resetBoostTime();
	p2_.resetBoostTime();
}

void Game::run() {
	sf::Clock clk;
	while (window_.isOpen()) {
		sf::Event e;
		while (window_.pollEvent(e)) {
			if (e.type == sf::Event::Closed) window_.close();
			if (e.type == sf::Event::KeyPressed) {
				if (state_ == State::Start) {
					// Chuyển từ start screen sang game
					sound_manager_.playSound(SoundType::ButtonClick);
					state_ = State::InGame;
					resetMatch();
				} else if (state_ == State::Result) {
					// Restart game khi nhấn phím bất kỳ
					sound_manager_.playSound(SoundType::ButtonClick);
					state_ = State::InGame;
					resetMatch();
				}
			}
		}
		int dt = clk.restart().asMilliseconds();
		update(dt);
		draw();
	}
}

void Game::update(int dt) {
	if (state_ != State::InGame) return;

	// input
	p1_.handleInput();
	p2_.handleInput();

	// update snake (move + buffs)
	p1_.tickBuffs(dt);  p1_.update(dt);
	p2_.tickBuffs(dt);  p2_.update(dt);

	// Xử lý bắn đạn cho cả hai người chơi
	if (p1_.tryShoot()) {
		handleShooting(p1_, p2_);
	}
	if (p2_.tryShoot()) {
		handleShooting(p2_, p1_);
	}

	// Cập nhật bullets và items
	updateBullets(dt);
	updateItems(dt);
	
	// Cập nhật STL containers
	updatePlayerScores();
	updateOccupiedCells();
	
	// Xử lý tạo items thủ công
	handleItemCreation();

	// Tạo vector snakes để truyền vào collision functions
	std::vector<Snake*> snakes{ &p1_, &p2_ };

	// collision: rắn–thân
	collision::snakeSelfAndWall(p1_, snakes, cfg::HIT_HEAD_PENALTY);
	collision::snakeSelfAndWall(p2_, snakes, cfg::HIT_HEAD_PENALTY);

	// rắn ăn item
	int p1_eaten = collision::snakeEatItem(p1_, items_);
	int p2_eaten = collision::snakeEatItem(p2_, items_);
	
	// Phát âm thanh khi ăn
	if (p1_eaten > 0) {
		sound_manager_.playSound(SoundType::SnakeEat);
	}
	if (p2_eaten > 0) {
		sound_manager_.playSound(SoundType::SnakeEat);
	}

	// đạn trúng rắn
	for (auto& b : bullets_) {
		auto [hitSnake, headHit] = collision::bulletHitSnake(b, snakes);
		if (hitSnake) {
			if(hitSnake->hasShield()) {
				hitSnake->useShield();
			}
			else if (hitSnake->alive()) {
				int dmg = headHit ? cfg::HIT_HEAD_PENALTY : cfg::HIT_BODY_PENALTY;
				if (b.type() == BulletType::Laser) {
					dmg *= 2;
				}
				hitSnake->shrink(dmg);
			}
			b.kill();
		}
	}

	// win/lose
	if (!p1_.alive() || !p2_.alive()) {
		// Phát âm thanh khi rắn chết
		if (!p1_.alive()) sound_manager_.playSound(SoundType::SnakeDie);
		if (!p2_.alive()) sound_manager_.playSound(SoundType::SnakeDie);
		
		// Cập nhật max length nếu cần
		int current_max = std::max(p1_.length(), p2_.length());
		if (current_max > max_length_) {
			max_length_ = current_max;
			saveMaxLength(max_length_);
		}
		
		state_ = State::Result;
	}

	// timer
	round_ms_left_ -= dt;
	if (round_ms_left_ <= 0) state_ = State::Result;
}

void Game::draw() {
	window_.clear(sf::Color(30,30,30));
	if (state_ == State::Start) {
		hud_.drawStartScreen(window_,background_texture_,loadMaxLength());
	} else if (state_ == State::InGame) {
		// Draw background
		sf::Sprite background_sprite(background_texture_);
		background_sprite.setScale(
			(float)window_.getSize().x / background_texture_.getSize().x,
			(float)window_.getSize().y / background_texture_.getSize().y
		);
		window_.draw(background_sprite);
		// vẽ items
		for (auto& it : items_) {
			it->draw(window_);
		}

		p1_.draw(window_);
		p2_.draw(window_);

		for (auto& b : bullets_) b.draw(window_);

		hud_.draw(window_, p1_, p2_, round_ms_left_);
	} else if (state_ == State::Result) {
		hud_.drawWinnerScreen(window_,getWinner());
	}

	window_.display();
}

int Game::getWinner() const {
    if (!p1_.alive() && !p2_.alive()) return 0; // tie
    if (!p1_.alive()) return 2; // P2 wins
    if (!p2_.alive()) return 1; // P1 wins
    if (round_ms_left_ <= 0) {
        // Time's up - compare lengths
        if (p1_.length() > p2_.length()) return 1;
        if (p2_.length() > p1_.length()) return 2;
        return 0; // tie
    }
    return -1; // game still ongoing
}

void Game::saveMaxLength(int length) {
    try {
        std::ofstream file("max_length.txt");
        if (file.is_open()) {
            file << length;
            file.close();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error saving max length: " << e.what() << std::endl;
    }
}

int Game::loadMaxLength() {
    try {
        std::ifstream file("max_length.txt");
        if (file.is_open()) {
            int length;
            file >> length;
            file.close();
            return length;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading max length: " << e.what() << std::endl;
    }
    return 0; // Default value if file doesn't exist or error
}

void Game::updatePlayerScores() {
    // Cập nhật điểm số dựa trên độ dài rắn
    player_scores_[1] = p1_.length();
    player_scores_[2] = p2_.length();
}

void Game::updateOccupiedCells() {
    // Xóa tất cả các ô đã chiếm
    occupied_cells_.clear();
    
    // Thêm các ô mà rắn đang chiếm
    const auto& p1_body = p1_.bodyPx();
    const auto& p2_body = p2_.bodyPx();
    
    for (const auto& pos : p1_body) {
        Cell cell = Grid::toCell(pos);
        occupied_cells_.insert(cell);
    }
    
    for (const auto& pos : p2_body) {
        Cell cell = Grid::toCell(pos);
        occupied_cells_.insert(cell);
    }
    
    // Thêm các ô mà items đang chiếm
    for (const auto& item : items_) {
        if (item->isAlive()) {
            occupied_cells_.insert(item->getCell());
        }
    }
}

void Game::createAppleAt(Cell cell) {
    // Sử dụng trực tiếp lớp Apple
    items_.push_back(std::make_unique<Apple>(cell));
}

void Game::createBuffAt(BuffType type, Cell cell, int lifetime_ms) {
    items_.push_back(std::make_unique<BuffItem>(type, cell, lifetime_ms));
}

void Game::removeDeadItems() {
    // Sử dụng STL algorithm để loại bỏ items đã chết
    items_.erase(
        std::remove_if(items_.begin(), items_.end(),
            [](const std::unique_ptr<BaseItem>& item) {
                return !item->isAlive();
            }),
        items_.end()
    );
}

void Game::handleItemCreation() {
    // Tạo items thủ công khi nhấn phím (ví dụ: Space để tạo apple, B để tạo buff)
    static bool space_pressed = false;
    static bool b_pressed = false;
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !space_pressed) {
        space_pressed = true;
        // Tạo apple ở vị trí ngẫu nhiên
        Cell randomCell = {rand() % cfg::GRID_W, rand() % cfg::GRID_H};
        createAppleAt(randomCell);
    } else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        space_pressed = false;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::B) && !b_pressed) {
        b_pressed = true;
        // Tạo buff ngẫu nhiên
        Cell randomCell = {rand() % cfg::GRID_W, rand() % cfg::GRID_H};
        BuffType randomBuff = static_cast<BuffType>(rand() % 6);
        createBuffAt(randomBuff, randomCell, 10000); // 10 giây
    } else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
        b_pressed = false;
    }
}

// Helper function để xử lý bắn đạn, giảm code trùng lặp
void Game::handleShooting(Snake& shooter, Snake& target) {
    // Sử dụng hàm với STL containers
    auto newBullets = shooter.createBullets();
    for (auto& bullet : newBullets) {
        // Đặt mục tiêu cho đạn đuổi
        if (bullet.type() == BulletType::Homing) {
            bullet.setTarget(target.headPx());
        }
        bullets_.push_back(bullet);
    }
    sound_manager_.playSound(SoundType::ButtonClick);
}

// Helper function để cập nhật bullets
void Game::updateBullets(int dt_ms) {
	std::vector<Snake*> snakes{ &p1_, &p2_ };
    for (auto& b : bullets_) {
    	b.updateHomingTarget(snakes);
        b.update(dt_ms);
    }
    bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(), [](auto& x){return !x.alive();}), bullets_.end());
}

// Helper function để cập nhật items
void Game::updateItems(int dt_ms) {
    // Cập nhật items
    for (auto& it : items_) it->update(dt_ms);
    
	// Sử dụng hàm mới để loại bỏ items đã chết
	removeDeadItems();

    // Spawn items
    std::vector<Snake*> snakes{ &p1_, &p2_ };
    spawner_.update(dt_ms, snakes, items_);
}
