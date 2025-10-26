#pragma once
#include <deque>
#include <vector>
#include <SFML/Graphics.hpp>
#include "../core/Types.hpp"

// Forward declaration để tránh circular dependency
class Bullet;

struct BuffState {
	int shield = 0;
	int x2_ms  = 0;
	int speed_ms = 0;
	
	// Buff đạn - hệ thống đạn (2 phát mỗi loại)
	int triple_shot_ammo = 0;  // Số đạn 3 tia còn lại
	int laser_shot_ammo = 0;   // Số đạn laser còn lại
	int homing_shot_ammo = 0;  // Số đạn đuổi còn lại
};

// Hướng cũ vẫn giữ để tương thích tạm thời ở nơi khác nếu cần
enum class Dir { Up, Down, Left, Right };

class Snake {
public:
	Snake(int id, sf::Color color, Cell start, Dir d, int init_len);

	void setKeyMap(sf::Keyboard::Key up, sf::Keyboard::Key down,
	               sf::Keyboard::Key left, sf::Keyboard::Key right,
	               sf::Keyboard::Key shoot);

	void handleInput();               // quay trái/phải + đọc phím bắn (edge)
	void update(int dt_ms);           // di chuyển liên tục theo px/s (có buff)
	void draw(sf::RenderWindow& win) const;

	void grow(int k);
	void shrink(int k);
	bool alive() const { return alive_; }

	// bắn: trả về true nếu bắn được (edge-trigger + cooldown + đủ độ dài)
	bool tryShoot();
	
	// Tạo đạn với STL containers và tổ chức tốt hơn
	std::vector<Bullet> createBullets();

	// truy cập
	const sf::Vector2f& headPx() const { return body_px_.front(); }
	const std::deque<sf::Vector2f>& bodyPx() const { return body_px_; }
	int length() const { return length_; }
	int id() const { return id_; }

	// buff
	void giveShield(int count=1) { buffs_.shield += count; }
	void giveX2(int ms) { buffs_.x2_ms = ms; }
	void giveSpeed(int ms) { buffs_.speed_ms = ms; }
	bool hasX2() const { return buffs_.x2_ms > 0; }
	
	// buff đạn - hệ thống đạn
	void giveTripleShot(int ammo = 3) { buffs_.triple_shot_ammo += ammo; }
	void giveLaserShot(int ammo = 3) { buffs_.laser_shot_ammo += ammo; }
	void giveHomingShot(int ammo = 3) { buffs_.homing_shot_ammo += ammo; }
	bool hasTripleShot() const { return buffs_.triple_shot_ammo > 0; }
	bool hasLaserShot() const { return buffs_.laser_shot_ammo > 0; }
	bool hasHomingShot() const { return buffs_.homing_shot_ammo > 0; }
	
	// Sử dụng đạn
	void useTripleShot() { if (buffs_.triple_shot_ammo > 0) buffs_.triple_shot_ammo--; }
	void useLaserShot() { if (buffs_.laser_shot_ammo > 0) buffs_.laser_shot_ammo--; }
	void useHomingShot() { if (buffs_.homing_shot_ammo > 0) buffs_.homing_shot_ammo--; }
	
	// boost system
	void startBoost();
	void stopBoost();
	void resetBoostTime();
	bool isBoosting() const { return is_boosting_; }
	int getTotalBoostTime() const { return total_boost_time_ms_; }

	// góc hướng (radian)
	float headingRad() const { return heading_rad_; }

	// tick buff timers
	void tickBuffs(int dt_ms);

	// phạt đụng tường/thân
	void collidePenalty(int k);
	
	// teleport đầu rắn đến vị trí mới
	void teleportTo(const sf::Vector2f& newPos);

	// input keys
	struct Keys { sf::Keyboard::Key up,down,left,right,shoot; } keys;

private:
	int id_;
	sf::Color color_;
	std::deque<sf::Vector2f> body_px_; // head = front, chuỗi điểm tâm thân rắn
	int pending_grow_ = 0;
	int length_;
	bool alive_ = true;
	
	// Textures
	sf::Texture head_texture_;
	sf::Texture body_texture_;

	BuffState buffs_;

	int shoot_cd_ms_ = 0;
	bool prev_shoot_pressed_ = false;
	bool shoot_edge_ = false; // set trong handleInput, reset khi tryShoot thành công

	// Boost system
	bool is_boosting_ = false;
	int total_boost_time_ms_ = 0;  // tổng thời gian boost đã sử dụng
	int current_boost_session_ms_ = 0;  // thời gian boost trong session hiện tại
	bool prev_boost_pressed_ = false;

	float move_acc_px_ = 0.f;           // tích lũy quãng đường đầu di chuyển
	float segment_spacing_px_ = 0.f;    // khoảng cách giữa các điểm thân
	float heading_rad_ = 0.f;           // hướng di chuyển hiện tại
};
