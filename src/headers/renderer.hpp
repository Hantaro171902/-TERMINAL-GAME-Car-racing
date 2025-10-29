#pragma once
#include <string>

class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;

    // Draw the right-side HUD: timer (seconds), score, level and speed
    // hudX/hudY are absolute screen coords (1-based)
    void drawSideMenu(int hudX, int hudY, int score, int elapsedSeconds, int level, float speed) const;

    // small instruction box (x,y top-left)
    void drawInstructions(int x, int y) const;
};
