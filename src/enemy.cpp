#include "enemy.hpp"
#include <iostream>
#include <cmath>
#include "utils.hpp"

using namespace std;

Enemy::Enemy()
    : laneIdx(0), yf(0.0f), speed(0.0f), w(5), h(4), active(false)
{
    shape = {
        "[╦───╦]",
        " ║███║ ",
        "[╬═══╬]",
        " ╚───╝ "    
    };
    h = (int)shape.size();
    // w = (int)shape[0].size();
    w = 5;
}

void Enemy::spawnLane(int laneIndex, const vector<int>& laneXs_, int sy, float sp) {
    laneIdx = laneIndex;
    laneXs = laneXs_;
    yf = (float)sy;
    speed = sp;
    active = true;
}

void Enemy::update(float dt) {
    if (!active) return;
    yf += speed * dt * 10.0f;  // Scale for pixel-like move (1 line per frame ~)
}

void Enemy::draw() const {
    if (!active || laneXs.empty()) return;
    int xi = laneXs[laneIdx];
    int yi = (int)round(yf);
    for (int i = 0; i < h; ++i) {
        if (yi + i >= 0) {  // Like ref: only if >=0
            move_cursor(xi, yi + i);
            cout << shape[i];
        }
    }
}

void Enemy::erase() const {
    if (!active || laneXs.empty()) return;
    int xi = laneXs[laneIdx];
    int yi = (int)round(yf);
    for (int i = 0; i < h; ++i) {
        if (yi + i >= 0 && yi + i <= 24) {  // Erase visible only
            move_cursor(xi, yi + i);
            cout << string(w, ' ');
        }
    }
}

bool Enemy::isActive() const { return active; }
int Enemy::getX() const { return laneXs.empty() ? 0 : laneXs[laneIdx]; }
int Enemy::getY() const { return (int)round(yf); }
int Enemy::getW() const { return w; }
int Enemy::getH() const { return h; }
int Enemy::getLaneIndex() const { return laneIdx; }
void Enemy::deactivate() { active = false; }