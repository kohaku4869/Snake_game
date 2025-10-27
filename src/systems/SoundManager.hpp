#pragma once
#include <SFML/Audio.hpp>
#include "core/Types.hpp"
#include <map>
#include <string>



class SoundManager {
public:
    SoundManager();
    void playSound(SoundType type);
    void playTheme(); // Phát nhạc theme
    void stopTheme(); // Dừng nhạc theme
    void setVolume(float volume);
    void setMuted(bool muted);

private:
    std::map<SoundType, sf::SoundBuffer> sound_buffers_;
    std::map<SoundType, sf::Sound> sounds_;
    sf::Music theme_music_; // Nhạc theme
    float volume_;
    bool muted_;
    
    void loadSound(SoundType type, const std::string& filename);
};

