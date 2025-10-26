#include "renderer.hpp"

#include <iostream>
#include <set>
#include <iomanip>

using namespace std;

static bool contains(const vector<pair<int,int>> & v, int x, int y)
{
    for (auto &p : v)
        if (p.first == x && p.second == y) return true;
    return false;
}

void Renderer::drawSideMenu(int score, int elapsedSeconds, int boardSize) const
{
  
}

void Renderer::drawInstructions(int x, int y) const
{
    move_cursor(x, y);
    setTextColor(TextColor::WHITE);
    cout << "╔═════════════════════════════════════╗";
    move_cursor(x, y + 1);
    cout << "║ ↑←↓→  : Move cursor    ENTER : Place║";
    move_cursor(x, y + 2);
    cout << "║ WASD  : Move cursor    R     : Reset║";
    move_cursor(x, y + 3);
    cout << "║ Q     : Quit game      ESC   : Quit ║";
    move_cursor(x, y + 4);
    cout << "╚═════════════════════════════════════╝";
    resetTextColor();
}
