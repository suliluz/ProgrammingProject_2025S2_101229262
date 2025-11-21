#pragma once

#include <string>
#include <fstream>
#include <iostream>

enum class WindowSize {
    SMALL,      // 800x600
    MEDIUM,     // 1024x768
    LARGE,      // 1280x960
    FULLSCREEN  // Fullscreen mode
};

enum class TextSpeed {
    SLOW,       // 0.08s per character
    NORMAL,     // 0.05s per character
    FAST,       // 0.03s per character
    INSTANT     // No delay
};

class Settings {
private:
    WindowSize windowSize;
    TextSpeed textSpeed;
    float masterVolume;
    bool autoSave;

public:
    Settings()
        : windowSize(WindowSize::MEDIUM),
          textSpeed(TextSpeed::NORMAL),
          masterVolume(0.7f),
          autoSave(true) {}

    // Getters
    WindowSize getWindowSize() const { return windowSize; }
    TextSpeed getTextSpeed() const { return textSpeed; }
    float getMasterVolume() const { return masterVolume; }
    bool isAutoSaveEnabled() const { return autoSave; }

    // Setters
    void setWindowSize(WindowSize size) { windowSize = size; }
    void setTextSpeed(TextSpeed speed) { textSpeed = speed; }
    void setMasterVolume(float volume) {
        if (volume < 0.0f) volume = 0.0f;
        if (volume > 1.0f) volume = 1.0f;
        masterVolume = volume;
    }
    void setAutoSave(bool enabled) { autoSave = enabled; }

    // Get actual window dimensions
    void getWindowDimensions(unsigned int& width, unsigned int& height) const {
        switch (windowSize) {
            case WindowSize::SMALL:
                width = 800;
                height = 600;
                break;
            case WindowSize::MEDIUM:
                width = 1024;
                height = 768;
                break;
            case WindowSize::LARGE:
                width = 1280;
                height = 960;
                break;
            case WindowSize::FULLSCREEN:
                width = 1920;
                height = 1080;
                break;
        }
    }

    // Get text speed as time interval
    float getTextSpeedInterval() const {
        switch (textSpeed) {
            case TextSpeed::SLOW:
                return 0.08f;
            case TextSpeed::NORMAL:
                return 0.05f;
            case TextSpeed::FAST:
                return 0.03f;
            case TextSpeed::INSTANT:
                return 0.0f;
        }
        return 0.05f;
    }

    // Get text speed multiplier
    float getTextSpeedMultiplier() const {
        switch (textSpeed) {
            case TextSpeed::SLOW:
                return 0.625f;  // 0.05 / 0.08
            case TextSpeed::NORMAL:
                return 1.0f;
            case TextSpeed::FAST:
                return 1.667f;  // 0.05 / 0.03
            case TextSpeed::INSTANT:
                return 100.0f;  // Effectively instant
        }
        return 1.0f;
    }

    // Get display strings
    std::string getWindowSizeString() const {
        switch (windowSize) {
            case WindowSize::SMALL: return "800x600";
            case WindowSize::MEDIUM: return "1024x768";
            case WindowSize::LARGE: return "1280x960";
            case WindowSize::FULLSCREEN: return "Fullscreen";
        }
        return "Unknown";
    }

    std::string getTextSpeedString() const {
        switch (textSpeed) {
            case TextSpeed::SLOW: return "Slow";
            case TextSpeed::NORMAL: return "Normal";
            case TextSpeed::FAST: return "Fast";
            case TextSpeed::INSTANT: return "Instant";
        }
        return "Unknown";
    }

    // Save/Load
    bool save(const std::string& filename = "settings.dat") const {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to save settings!" << std::endl;
            return false;
        }

        int ws = static_cast<int>(windowSize);
        int ts = static_cast<int>(textSpeed);

        file.write(reinterpret_cast<const char*>(&ws), sizeof(ws));
        file.write(reinterpret_cast<const char*>(&ts), sizeof(ts));
        file.write(reinterpret_cast<const char*>(&masterVolume), sizeof(masterVolume));
        file.write(reinterpret_cast<const char*>(&autoSave), sizeof(autoSave));

        file.close();
        std::cout << "Settings saved!" << std::endl;
        return true;
    }

    bool load(const std::string& filename = "settings.dat") {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;  // Use defaults if no settings file exists
        }

        int ws, ts;

        file.read(reinterpret_cast<char*>(&ws), sizeof(ws));
        file.read(reinterpret_cast<char*>(&ts), sizeof(ts));
        file.read(reinterpret_cast<char*>(&masterVolume), sizeof(masterVolume));
        file.read(reinterpret_cast<char*>(&autoSave), sizeof(autoSave));

        windowSize = static_cast<WindowSize>(ws);
        textSpeed = static_cast<TextSpeed>(ts);

        file.close();
        std::cout << "Settings loaded!" << std::endl;
        return true;
    }
};
