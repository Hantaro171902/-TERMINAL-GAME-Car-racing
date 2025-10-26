#pragma once

#include <string>
#include <vector>

#include "board.hpp"
#include "color.hpp"
#include "utils.hpp"

struct MoveRecord {
    int row;
    int col;
};

class Renderer {
public:

    void drawSideMenu(int score, ,int elapsedSeconds, int boardSize) const;
    void drawInstructions(int x, int y) const;
   
};


