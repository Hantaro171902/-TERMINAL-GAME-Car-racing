#include "enermy.hpp"
#include <iostream>
#include "utils.hpp"

using namespace std;

Enermy::Enermy(int startX, int startY)
    : x(startX), y(startY), speed(1.0f), active(false)
{
    shape = {
        "[╦───╦]"
        " ║███║ "
        "[╬═══╬]"
        " ╚───╝ "    
    };
    h = (int)shape.size();
    w = (int)shape[0].size();
}

void Enermy::draw() const {
    if (!active) return;
    for (int i = 0; i < h; ++i) {
        move_cursor(x, y + i);
        cout << shape[i];
    }
}

void Enermy::erase() const {
    if (!active) return;
    for (int i = 0; i < h; ++i) {
        move_cursor(x, y + i);
        cout << string(w, ' ');
    }
}

void Enermy::moveDown() {
    if (!active) return;
    erase();
    y += 1;
    draw();
}

void Enermy::spawn(int sx, int sy, float sp) {
    x = sx; y = sy; speed = sp; active = true;
}

bool Enermy::isActive() const { return active; }
int Enermy::getX() const { return x; }
int Enermy::getY() const { return y; }
int Enermy::getW() const { return w; }
int Enermy::getH() const { return h; }