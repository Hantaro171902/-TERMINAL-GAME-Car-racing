#pragma once

#include <string>
#include <vector>

class Board {
    private:
        /* data */
        int x, y;           // top-left coordinate (1-based for move_cursor)
        int w, h;
        int hudRow;
        std::vector<std::vector<char>> board;

    public:

        Board(int x, int y, int w, int h, int hudRow = 0);
        ~Board() = default;
  
        void clear();
        void setCell(int r, int c, char ch);
        char getCell(int r, int c) const;
        void draw() const;
        int getX() const { return x; }
        int getY() const { return y; }
        int getWidth() const { return w; }
        int getHeight() const { return h; }
}