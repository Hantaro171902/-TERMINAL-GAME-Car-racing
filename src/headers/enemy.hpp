#pragma once

#include <string>
#include <vector>

class Enermy {
private:
  /* data */
  int x, y;
  std::vector<std::string> shape = {
    "[╦───╦]"
    " ║███║ "
    "[╬═══╬]"
    " ╚───╝ "
  };
public:
  Enermy(int startX, int startY) : x(startX), y(startY);
  
  void draw();
  void erase();
  void moveDown();
  int getX() const;
  int getY() const;
};
