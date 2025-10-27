#include "car.hpp"
#include <iostream>
#include <utils.hpp>

using namespace std;


Car::Car(int startX, int startY)
    : x(startX), y(startY)
{
    shape = {
		"  v v  "
		" ╔───╗ "
		"[╬═══╬]"
		" ║███║ "
		"[╩───╩]"
    };
    h = (int)shape.size();
    w = (int)shape[0].size();
}

void Car::draw() const {
    for (int i = 0; i < h; ++i) {
        move_cursor(x, y + i);
        cout << shape[i];
    }
}

void Car::erase() const {
    for (int i = 0; i < h; ++i) {
        move_cursor(x, y + i);
        cout << string(w, ' ');
    }
}

void Car::moveLeft(int leftBound) {
    if (x - 3 >= leftBound) {
        erase();
        x -= 3;
        draw();
    }
}

void Car::moveRight(int rightBound) {
    if (x + w + 3 <= rightBound) {
        erase();
        x += 3;
        draw();
    }
}