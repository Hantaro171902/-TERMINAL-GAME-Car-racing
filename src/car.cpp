#include "car.hpp"
#include <iostream>
#include <utils.hpp>

using namespace std;

Car::Car(int startLane, int baseY)
    : laneIndex(startLane), y(baseY)
{
    shape = {
		"  v v  ",
		" ╔───╗ ",
		"[╬═══╬]",
		" ║███║ ",
		"[╩───╩]"
    };
    h = (int)shape.size();
    w = (int)shape[0].size();
}

void Car::setLanes(const vector<int>& laneXs) {
    lanes = laneXs;
    if (laneIndex < 0) laneIndex = 0;
    if (laneIndex >= (int)lanes.size()) laneIndex = (int)lanes.size() - 1;
}

void Car::draw() const {
    if (lanes.empty()) return;
    int x = lanes[laneIndex];
    for (int i = 0; i < h; ++i) {
        move_cursor(x, y + i);
        cout << shape[i];
    }
}

void Car::erase() const {
    if (lanes.empty()) return;
    int x = lanes[laneIndex];
    for (int i = 0; i < h; ++i) {
        move_cursor(x, y + i);
        cout << string(w, ' ');
    }
}

void Car::moveLeft() {
    if (laneIndex > 0) {
        erase();
        laneIndex--;
        draw();
    }
}

void Car::moveRight() {
    if (laneIndex + 1 < (int)lanes.size()) {
        erase();
        laneIndex++;
        draw();
    }
}

int Car::getX() const { return lanes.empty() ? 0 : lanes[laneIndex]; }
int Car::getY() const { return y; }
int Car::getW() const { return w; }
int Car::getH() const { return h; }
int Car::getLaneIndex() const { return laneIndex; }
void Car::setBaseY(int by) { y = by; }