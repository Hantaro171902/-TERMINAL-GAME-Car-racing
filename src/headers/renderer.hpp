#pragma once
#include <string>

class Renderer {
public:
    void drawSideMenu(int hudX, int hudY, int score, int elapsedSeconds, int level, float speed) const;
    void drawInstructions(int x, int y) const;

private:
    std::string fmtTime(int secs) const;
};
