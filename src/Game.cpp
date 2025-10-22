#include "Game.hpp"
#include "core/Config.hpp"
#include "grid/Grid.hpp"
#include "systems/Collision.hpp"
#include "entities/Bullet.hpp"
#include <algorithm>
#include <algorithm>
#include <limits>

Game::Game()
: window_(sf::VideoMode(cfg::GRID_W*cfg::CELL, cfg::GRID_H*cfg::CELL), "Snake Battle"),
  p1_(1, sf::Color::Green, {5, 5}, Dir::Right, cfg::SNAKE_INIT_LEN),
  p2_(2, sf::Color::Cyan,  {cfg::GRID_W-6, cfg::GRID_H-6}, Dir::Left, cfg::SNAKE_INIT_LEN)
{
	window_.setFramerateLimit(cfg::FPS);
	p1_.setKeyMap(sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::W);
	p2_.setKeyMap(sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Left, sf::Keyboard::Right, sf::Keyboard::Up);
	
	// Load apple textures from NewTexture
	apple_texture_.loadFromFile("assets/NewTexture/FoodApple.png");
	green_apple_texture_.loadFromFile("assets/NewTexture/GreenApple.png");
	pink_apple_texture_.loadFromFile("assets/NewTexture/PinkApple.png");
	
	// Load background
	background_texture_.loadFromFile("assets/NewTexture/background.jpg");
	
	state_ = State::InGame; // demo vào game luôn
	resetMatch();
	
	// Phát nhạc theme
	sound_manager_.playTheme();
}

void Game::resetMatch() {
	bullets_.clear();
	items_.clear();
	round_ms_left_ = cfg::ROUND_TIME_S * 1000;
	
	// Reset rắn về vị trí ban đầu
	p1_ = Snake(1, sf::Color::Green, {5, 5}, Dir::Right, cfg::SNAKE_INIT_LEN);
	p2_ = Snake(2, sf::Color::Cyan, {cfg::GRID_W-6, cfg::GRID_H-6}, Dir::Left, cfg::SNAKE_INIT_LEN);
	p1_.setKeyMap(sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::W);
	p2_.setKeyMap(sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Left, sf::Keyboard::Right, sf::Keyboard::Up);
	
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
			if (e.type == sf::Event::KeyPressed && state_ == State::Result) {
				// Restart game khi nhấn phím bất kỳ
				sound_manager_.playSound(SoundType::ButtonClick);
				state_ = State::InGame;
				resetMatch();
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

	// Tạo vector snakes để truyền vào collision functions
	std::vector<Snake*> snakes{ &p1_, &p2_ };

	// collision: rắn–tường/thân (–2)
	collision::snakeSelfAndWall(p1_, snakes, cfg::HIT_HEAD_PENALTY);
	collision::snakeSelfAndWall(p2_, snakes, cfg::HIT_HEAD_PENALTY);

	// rắn ăn item
	int p1_eaten = collision::snakeEatItem(p1_, items_);
	int p2_eaten = collision::snakeEatItem(p2_, items_);
	
	// Phát âm thanh khi ăn
	if (p1_eaten > 0) sound_manager_.playSound(SoundType::SnakeEat);
	if (p2_eaten > 0) sound_manager_.playSound(SoundType::SnakeEat);

	// đạn trúng rắn
	for (auto& b : bullets_) {
		auto [hitSnake, headHit] = collision::bulletHitSnake(b, snakes);
		if (hitSnake) {
			if (hitSnake->alive()) {
				int dmg = headHit ? cfg::HIT_HEAD_PENALTY : cfg::HIT_BODY_PENALTY;
				// Buff đạn gây sát thương x1 (giảm từ x2 xuống x1)
				if (b.type() == BulletType::Homing || b.type() == BulletType::Triple) {
					dmg *= 1; // Không tăng sát thương
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
		state_ = State::Result;
	}

	// timer
	round_ms_left_ -= dt;
	if (round_ms_left_ <= 0) state_ = State::Result;
}

void Game::draw() {
	window_.clear(sf::Color(30,30,30));

	if (state_ == State::InGame) {
		// Draw background
		sf::Sprite background_sprite(background_texture_);
		background_sprite.setScale(
			(float)window_.getSize().x / background_texture_.getSize().x,
			(float)window_.getSize().y / background_texture_.getSize().y
		);
		window_.draw(background_sprite);
		// vẽ items
		for (auto& it : items_) {
			it.draw(window_);
		}

		p1_.draw(window_);
		p2_.draw(window_);

		for (auto& b : bullets_) b.draw(window_);

		hud_.draw(window_, p1_, p2_, round_ms_left_);
	} else if (state_ == State::Result) {
		drawWinnerScreen();
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

void Game::drawWinnerScreen() {
    int winner = getWinner();
    if (winner == -1) return; // game still ongoing
    
    // Tạo font để hiển thị
    sf::Font font;
    if (!font.loadFromFile("assets/fonts/arial.ttf")) return;
    
    sf::Text winnerText, restartText;
    winnerText.setFont(font);
    restartText.setFont(font);
    
    winnerText.setCharacterSize(48);
    restartText.setCharacterSize(24);
    
    // Xác định thông báo chiến thắng và màu sắc
    if (winner == 1) {
        winnerText.setString("P1 WIN!");
        winnerText.setFillColor(sf::Color(135, 206, 235)); // Xanh dương nhạt (màu P1)
    } else if (winner == 2) {
        winnerText.setString("P2 WIN!");
        winnerText.setFillColor(sf::Color::Yellow); // Vàng (màu P2)
    } else {
        winnerText.setString("TIE!");
        winnerText.setFillColor(sf::Color::White); // Trắng cho hòa
    }
    
    restartText.setFillColor(sf::Color::Green);
    
    restartText.setString("Press any button to restart");
    
    // Căn giữa màn hình
    sf::FloatRect winnerBounds = winnerText.getLocalBounds();
    sf::FloatRect restartBounds = restartText.getLocalBounds();
    
    winnerText.setPosition(
        (window_.getSize().x - winnerBounds.width) / 2,
        (window_.getSize().y - winnerBounds.height) / 2 - 50
    );
    
    restartText.setPosition(
        (window_.getSize().x - restartBounds.width) / 2,
        winnerText.getPosition().y + winnerBounds.height + 30
    );
    
    window_.draw(winnerText);
    window_.draw(restartText);
}

// Helper function để xử lý bắn đạn, giảm code trùng lặp
void Game::handleShooting(Snake& shooter, Snake& target) {
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
    for (auto& b : bullets_) {
        // Cập nhật mục tiêu cho đạn đuổi
        if (b.type() == BulletType::Homing) {
            // Tìm rắn địch gần nhất
            Snake* closestEnemy = nullptr;
            float closestDist2 = std::numeric_limits<float>::max();
            
            std::vector<Snake*> snakes{ &p1_, &p2_ };
            for (auto* s : snakes) {
                if (s->id() == b.owner() || !s->alive()) continue;
                const auto& body = s->bodyPx();
                if (body.empty()) continue;
                
                float dist2_to_head = (b.pos().x - body.front().x) * (b.pos().x - body.front().x) + 
                                    (b.pos().y - body.front().y) * (b.pos().y - body.front().y);
                if (dist2_to_head < closestDist2) {
                    closestDist2 = dist2_to_head;
                    closestEnemy = s;
                }
            }
            
            // Cập nhật mục tiêu cho đạn đuổi
            if (closestEnemy) {
                b.setTarget(closestEnemy->headPx());
            }
        }
        
        b.update(dt_ms);
    }
    bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(), [](auto& x){return !x.alive();}), bullets_.end());
}

// Helper function để cập nhật items
void Game::updateItems(int dt_ms) {
    // Cập nhật items
    for (auto& it : items_) it.update(dt_ms);
    items_.erase(std::remove_if(items_.begin(), items_.end(), [](auto& x){return !x.isAlive();}), items_.end());

    // Spawn items
    std::vector<Snake*> snakes{ &p1_, &p2_ };
    spawner_.update(dt_ms, snakes, items_);
}
