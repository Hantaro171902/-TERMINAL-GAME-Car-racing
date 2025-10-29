#include "board.hpp"
#include <iostream>
#include "utils.hpp"
#include "color.hpp"

#define ROAD_SYMBOL '|'
#define ROAD_WIDTH 80
#define ROAD_HEIGHT 25

using namespace std;

Board::Board(int sx, int sy, int sw, int sh, int hud)
    : x(sx), y(sy), w(sw), h(sh), hudRow(hud)
{
    board.assign(h, vector<char>(w, ' '));
}

void Board::clear() {
    for (int r = 0; r < h; ++r)
        for (int c = 0; c < w; ++c)
            board[r][c] = ' ';
}

void Board::setCell(int r, int c, char ch) {
    if (r >= 0 && r < h && c >= 0 && c < w) board[r][c] = ch;
}

char Board::getCell(int r, int c) const {
    if (r >= 0 && r < h && c >= 0 && c < w) return board[r][c];
    return ' ';
}

void Board::draw() const {
    // top border
    move_cursor(x, y);
    cout << SYMBOL_DOUBLE_TOP_LEFT;
    for (int i = 0; i < 2 * w; ++i) cout << SYMBOL_DOUBLE_HORIZONTAL;
    cout << SYMBOL_DOUBLE_TOP_RIGHT;

    // rows
    for (int r = 0; r < h; ++r) {
        int rowY = y + 1 + r;
        move_cursor(x, rowY);
        cout << SYMBOL_DOUBLE_VERTICAL;
        for (int c = 0; c < w; ++c) {
            char ch = board[r][c];
            // simple coloring choice: road walls/borders printed outside; center dash printed by Game/Board via setCell
            if (ch != ' ') {
                setTextColor(TextColor::WHITE);
                cout << ch << ' ';
                resetTextColor();
            } else {
                cout << ' ' << ' ';
            }
        }
        cout << SYMBOL_DOUBLE_VERTICAL;
    }

    // bottom border
    move_cursor(x, y + 1 + h);
    cout << SYMBOL_DOUBLE_BOTTOM_LEFT;
    for (int i = 0; i < 2 * w; ++i) cout << SYMBOL_DOUBLE_HORIZONTAL;
    cout << SYMBOL_DOUBLE_BOTTOM_RIGHT;

    // place cursor safe below
    move_cursor(1, y + h + 3);
    cout.flush();
}