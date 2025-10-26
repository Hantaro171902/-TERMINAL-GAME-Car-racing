#include "enermy.hpp"
#include <iostream>
#include "utils.hpp"

using namespace std;

Enermy::Enermy(int startX, int startY) : x(startX), y(startY) {
 
}

void Enermy::draw() {
  for (int i = 0; i < shape.size(); i++) {
    move_cursor(x, y + i);
    cout << shape[i];
  }
}

void Enermy::erase() {
  for (int i = 0; i < shape.size(); i++) {
    move_cursor(x, y + i);
    cout << "    ";
  }
}
void Enermy::moveDown() {
  erase();
  y += 1; // Move down by 1 unit
  draw();
}

int Enermy::getX() const {
  return x;
}

int Enermy::getY() const {
  return y;
}