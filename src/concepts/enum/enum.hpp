#pragma once
#include <cstdint>

enum class Color : uint8_t {
	Red,
	Green,
	Blue
};

inline bool isPrimaryColor(Color color) {
    switch (color) {
        case Color::Red:
        case Color::Green:
        case Color::Blue:
            return true;
        default:
            return false;
    }
}

