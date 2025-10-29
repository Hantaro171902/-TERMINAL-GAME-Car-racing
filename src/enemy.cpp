#include "enemy.hpp"
#include <iostream>
#include <cmath>
#include "utils.hpp"

using namespace std;

Enermy::Enermy()
    : laneIdx(0), yf(0.0f), speed(0.0f), w(0), h(0), active(false)
{
    shape = {
        "[╦───╦]",
        " ║███║ ",
        "[╬═══╬]",
        " ╚───╝ "    
    };
    h = (int)shape.size();
    w = (int)shape[0].size();
}

void Enermy::spawnLane(int laneIndex, const vector<int>& laneXs_, int sy, float sp) {
    laneIdx = laneIndex;
    laneXs = laneXs_;
    yf = (float)sy;
    speed = sp;
    active = true;
}

void Enermy::update(float dt) {
    if (!active) return;
    yf += speed * dt;
}

void Enermy::draw() const {
    if (!active || laneXs.empty()) return;
    int xi = laneXs[laneIdx];
    int yi = (int)round(yf);
    for (int i = 0; i < h; ++i) {
        move_cursor(xi, yi + i);
        cout << shape[i];
    }
}

void Enermy::erase() const {
    if (!active || laneXs.empty()) return;
    int xi = laneXs[laneIdx];
    int yi = (int)round(yf);
    for (int i = 0; i < h; ++i) {
        move_cursor(xi, yi + i);
        cout << string(w, ' ');
    }
}

bool Enermy::isActive() const { return active; }
int Enermy::getX() const { return laneXs.empty() ? 0 : laneXs[laneIdx]; }
int Enermy::getY() const { return (int)round(yf); }
int Enermy::getW() const { return w; }
int Enermy::getH() const { return h; }
int Enermy::getLaneIndex() const { return laneIdx; }
void Enermy::deactivate() { active = false; }