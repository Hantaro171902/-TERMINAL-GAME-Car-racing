#pragma once

#include <string>
#include <vector>

class Car {
    private:
        /* data */
        int x, y;
        int w, h;
        std::vector<std::string> shape;
        // std::vector<std::string> shape = {
        //   "  v v  "
        //   " ╔───╗ "
        //   "[╬═══╬]"
        //   " ║███║ "
        //   "[╩───╩]"
        // };

    public:
        Car(int startX = 0, int startY = 0);
        ~Car() = default;
  
        void draw() const;
        void erase() const;
        void moveLeft(int leftBound);
        void moveRight(int rightBound);
        int getX() const { return x; }
        int getY() const { return y; }
        int getW() const { return w; }
        int getH() const { return h; }

};
