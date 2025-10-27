#pragma once

#include <string>
#include <vector>

class Enermy {
private:
  	/* data */
  	int x, y;
    int w, h;
    float speed;
    bool active;
    std::vector<std::string> shape;
  	// std::vector<std::string> shape = {
  	//   "[╦───╦]"
  	//   " ║███║ "
  	//   "[╬═══╬]"
  	//   " ╚───╝ "
  	// };
public:
    Enermy(int startX = 0, int startY = 0);
    ~Enermy() = default;

    void draw() const;
    void erase() const;
    void moveDown();
    void spawn(int sx, int sy, float sp);
    bool isActive() const;
    int getX() const;
    int getY() const;
    int getW() const;
    int getH() const;
};
