#include "Hud.hpp"
#include "../core/Config.hpp"
#include <string>
#include <cstdio>


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