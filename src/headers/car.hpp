#pragma once

#include <string>
#include <vector>

class Car {
  private:
    /* data */
    int x, y;
    std::vector<std::string> shape = {
      "  v v  "
      " ╔───╗ "
      "[╬═══╬]"
      " ║███║ "
      "[╩───╩]"
    };

  public:
    Car(int startX, int startY) : x(startX), y(startY);
    
    void draw();
    void erase();
    void moveLeft();
    void moveRight();
    int getX() const;
    int getY() const;

};
