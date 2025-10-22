#include "SoundManager.hpp"
#include <iostream>

SoundManager::SoundManager() : volume_(30.0f), muted_(false) {
    // Load all sound files
    loadSound(SoundType::SnakeMove, "assets/Sounds/SnakeMove.wav");
    loadSound(SoundType::SnakeEat, "assets/Sounds/SnakeEat.wav");
    loadSound(SoundType::SnakeDie, "assets/Sounds/SnakeDie.wav");
    loadSound(SoundType::ButtonClick, "assets/Sounds/ButtonClick.wav");
    loadSound(SoundType::ButtonOver, "assets/Sounds/ButtonOver.wav");
    
    // Load theme music - sử dụng sf::Music để stream MP3
    if (theme_music_.openFromFile("assets/Sounds/Theme.ogg")) {
        theme_music_.setVolume(volume_);
        theme_music_.setLoop(true); // Lặp lại nhạc theme
        std::cout << "Successfully loaded OGG theme music" << std::endl;
    } else {
        // Fallback: thử load file WAV nếu MP3 không hoạt động
        std::cout << "MP3 not supported, trying WAV..." << std::endl;
        if (theme_music_.openFromFile("assets/Sounds/Theme.wav")) {
            theme_music_.setVolume(volume_);
            theme_music_.setLoop(true);
            std::cout << "Successfully loaded WAV theme music" << std::endl;
        } else {
            std::cout << "Warning: Could not load any theme music file" << std::endl;
        }
    }
}

void SoundManager::loadSound(SoundType type, const std::string& filename) {
    if (sound_buffers_[type].loadFromFile(filename)) {
        sounds_[type].setBuffer(sound_buffers_[type]);
        sounds_[type].setVolume(volume_);
    }
}

void SoundManager::playSound(SoundType type) {
    if (!muted_ && sounds_.find(type) != sounds_.end()) {
        sounds_[type].play();
    }
}

void SoundManager::setVolume(float volume) {
    volume_ = std::max(0.0f, std::min(100.0f, volume));
    for (auto& pair : sounds_) {
        pair.second.setVolume(volume_);
    }
}

void SoundManager::setMuted(bool muted) {
    muted_ = muted;
}

void SoundManager::playTheme() {
    if (!muted_ && theme_music_.getStatus() != sf::Music::Playing) {
        theme_music_.play();
    }
}

void SoundManager::stopTheme() {
    if (theme_music_.getStatus() == sf::Music::Playing) {
        theme_music_.stop();
    }
}

