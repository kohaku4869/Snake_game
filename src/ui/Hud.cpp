#include "Hud.hpp"
#include "../core/Config.hpp"
#include <string>
#include <cstdio>
#include <iostream>


Hud::Hud() {
    // Try loading a font; if fails, we will draw nothing but game still runs.
    font_.loadFromFile("assets/fonts/arial.ttf");
}


static std::string msToClock(int ms) {
    if (ms < 0) ms = 0;
    int s = ms / 1000; int m = s / 60; s %= 60;
    char buf[16]; snprintf(buf, sizeof(buf), "%02d:%02d", m, s);
    return buf;
}


void Hud::draw(sf::RenderWindow& win, const Snake& s1, const Snake& s2, int msLeft) {
    if (!font_.getInfo().family.empty()) {
        sf::Text t1, t2, t3, t4, t5;
        t1.setFont(font_); t2.setFont(font_); t3.setFont(font_); t4.setFont(font_); t5.setFont(font_);
        t1.setCharacterSize(18); t2.setCharacterSize(18); t3.setCharacterSize(22); t4.setCharacterSize(14); t5.setCharacterSize(14);
        t1.setFillColor(sf::Color(135, 206, 235)); t2.setFillColor(sf::Color::Yellow); t3.setFillColor(sf::Color::White);
        t4.setFillColor(sf::Color(135, 206, 235)); t5.setFillColor(sf::Color::Yellow);

        // Thông tin cơ bản
        t1.setString("P1 length: " + std::to_string(s1.length()));
        t2.setString("P2 length: " + std::to_string(s2.length()));
        t3.setString(msToClock(msLeft));
        
        // Thông tin boost
        int p1_boost_remaining = 3000 - s1.getTotalBoostTime();
        int p2_boost_remaining = 3000 - s2.getTotalBoostTime();
        t4.setString("P1 boost: " + std::to_string(p1_boost_remaining / 1000) + "s");
        t5.setString("P2 boost: " + std::to_string(p2_boost_remaining / 1000) + "s");

        t1.setPosition(8.f, 4.f);
        t2.setPosition(200.f, 4.f);
        t3.setPosition((float)cfg::GRID_W*cfg::CELL - 90.f, 4.f);
        t4.setPosition(8.f, 25.f);
        t5.setPosition(200.f, 25.f);

        win.draw(t1); win.draw(t2); win.draw(t3); win.draw(t4); win.draw(t5);
    }
}

void centerSprite(sf::Sprite& sprite, const sf::RenderWindow& window, float offsetY = 0.f) {
    // Lấy kích thước ảnh (theo pixel gốc)
    sf::FloatRect bounds = sprite.getLocalBounds();

    // Đặt origin về tâm ảnh
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    // Lấy kích thước cửa sổ
    sf::Vector2u winSize = window.getSize();
    float winW = static_cast<float>(winSize.x);
    float winH = static_cast<float>(winSize.y);

    // Đặt vị trí chính giữa (có thể dịch lên xuống bằng offsetY)
    sprite.setPosition(winW / 2.f, winH / 2.f + offsetY);
}

void Hud::drawStartScreen(sf::RenderWindow& window_,const sf::Texture& start_screen_texture_,int max_length) {
    // Vẽ background
    sf::Sprite background_sprite(start_screen_texture_);
    background_sprite.setScale(
        (float)window_.getSize().x / start_screen_texture_.getSize().x,
        (float)window_.getSize().y / start_screen_texture_.getSize().y
    );
    window_.draw(background_sprite);

    //Vẽ Title
    sf::Texture title_texture;
    title_texture.loadFromFile("assets/NewTexture/title.png");
    sf::Sprite sprite_title;
    sprite_title.setTexture(title_texture);
    sprite_title.setPosition(400, 200);  // đặt ảnh ở vị trí (x=400, y=200)
    sprite_title.setScale(0.5f, 0.5f);
    centerSprite(sprite_title, window_, -50.f);

    sf::Font font;
    try {
        if (!font.loadFromFile("assets/fonts/arial.ttf")) return;
    } catch (const std::exception& e) {
        std::cerr << "Error loading font in drawStartScreen: " << e.what() << std::endl;
        return;
    }


    // Max Length display
    sf::Text maxLengthText;
    maxLengthText.setFont(font);
    maxLengthText.setString("Max Length: " + std::to_string(max_length));
    maxLengthText.setCharacterSize(32);
    maxLengthText.setFillColor(sf::Color::White);

    // Căn giữa Max Length ở phía dưới chính giữa
    sf::FloatRect maxLengthBounds = maxLengthText.getLocalBounds();
    maxLengthText.setPosition(
        (window_.getSize().x - maxLengthBounds.width) / 2,
        window_.getSize().y * 3 / 4
    );

    // Hướng dẫn
    sf::Text instructionText;
    instructionText.setFont(font);
    instructionText.setString("Press any key to start");
    instructionText.setCharacterSize(24);
    instructionText.setFillColor(sf::Color::Green);

    // Căn giữa hướng dẫn
    sf::FloatRect instructionBounds = instructionText.getLocalBounds();
    instructionText.setPosition(
        (window_.getSize().x - instructionBounds.width) / 2,
        maxLengthText.getPosition().y + 50
    );

    // window_.draw(titleText);
    window_.draw(sprite_title);
    window_.draw(maxLengthText);
    window_.draw(instructionText);

    // Hiển thị thông tin về items
}

void Hud::drawWinnerScreen(sf::RenderWindow& window_ ,int winner,int score_p1,int score_p2) {
    if (winner == -1) return; // game still ongoing

    // Tạo font để hiển thị
    sf::Font font;
    try {
        if (!font.loadFromFile("assets/fonts/arial.ttf")) return;
    } catch (const std::exception& e) {
        std::cerr << "Error loading font in drawWinnerScreen: " << e.what() << std::endl;
        return;
    }

    sf::Text winnerText, restartText, scoreText_p1, scoreText_dash, scoreText_p2;
    winnerText.setFont(font);
    restartText.setFont(font);
    scoreText_p1.setFont(font);
    scoreText_dash.setFont(font);
    scoreText_p2.setFont(font);

    winnerText.setCharacterSize(48);
    restartText.setCharacterSize(24);
    scoreText_p1.setCharacterSize(36);
    scoreText_dash.setCharacterSize(36);
    scoreText_p2.setCharacterSize(36);

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

    // Thiết lập tỉ số
    scoreText_p1.setString(std::to_string(score_p1));
    scoreText_p1.setFillColor(sf::Color(135, 206, 235)); // Màu P1
    scoreText_dash.setString("  -  ");
    scoreText_dash.setFillColor(sf::Color::White); // Màu trắng
    scoreText_p2.setString(std::to_string(score_p2));
    scoreText_p2.setFillColor(sf::Color::Yellow); // Màu P2

    // Căn giữa màn hình
    sf::FloatRect winnerBounds = winnerText.getLocalBounds();
    sf::FloatRect restartBounds = restartText.getLocalBounds();

    winnerText.setPosition(
        (window_.getSize().x - winnerBounds.width) / 2,
        (window_.getSize().y - winnerBounds.height) / 2 - 50
    );

    // Tính toán vị trí cho tỉ số (dưới winner text, trên restart text)
    sf::FloatRect p1Bounds = scoreText_p1.getLocalBounds();
    sf::FloatRect dashBounds = scoreText_dash.getLocalBounds();
    sf::FloatRect p2Bounds = scoreText_p2.getLocalBounds();
    
    float totalWidth = p1Bounds.width + dashBounds.width + p2Bounds.width;
    float startX = (window_.getSize().x - totalWidth) / 2;
    
    scoreText_p1.setPosition(startX, winnerText.getPosition().y + winnerBounds.height + 20);
    scoreText_dash.setPosition(startX + p1Bounds.width, scoreText_p1.getPosition().y);
    scoreText_p2.setPosition(startX + p1Bounds.width + dashBounds.width, scoreText_p1.getPosition().y);

    // Đặt restart text dưới score text với khoảng cách đủ xa
    restartText.setPosition(
        (window_.getSize().x - restartBounds.width) / 2,
        scoreText_p1.getPosition().y + p1Bounds.height + 40
    );

    window_.draw(winnerText);
    window_.draw(scoreText_p1);
    window_.draw(scoreText_dash);
    window_.draw(scoreText_p2);
    window_.draw(restartText);
}