#include "car.hpp"
#include <iostream>
#include <utils.hpp>

using namespace std;

Car::Car(int startX, int startY) : x(startX), y(startY) {
 
}

void Car::draw() {
  for (int i = 0; i < shape.size(); i++) {
    move_cursor(x, y + i);
    cout << shape[i];
  }
}

void Car::erase() {
  for (int i = 0; i < shape.size(); i++) {
    move_cursor(x, y + i);
    cout << "    ";
  }
}

void Car::moveLeft() {
  if (x > 0) {
    erase();
    x -= 2; // Move left by 2 units
    draw();
  }
}

void Car::moveRight() {
  if (x < 70) { // Assuming terminal width is 80 and car width is 10
    erase();
    x += 2; // Move right by 2 units
    draw();
  }
}
