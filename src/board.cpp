#include "board.hpp"
#include <iostream>
#include "utils.hpp"

#define ROAD_SYMBOL '|'
#define ROAD_WIDTH 80
#define ROAD_HEIGHT 25

using namespace std;

void Board::draw() {
  for (int i = 0; i < height; i++) {
    move_cursor(0, i);
    cout << road[i];
  }
}