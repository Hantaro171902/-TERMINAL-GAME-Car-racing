#pragma once

#include <string>
#include <vector>

class Car {
    private:
        /* data */
        std::vector<int> lanes;
        int laneIndex;
        int y;
        std::vector<std::string> shape;
        int h, w;
        // std::vector<std::string> shape = {
        //   "  v v  "
        //   " ╔───╗ "
        //   "[╬═══╬]"
        //   " ║███║ "
        //   "[╩───╩]"
        // };

    public:
        Car(int startLane, int baseY);
        void setLanes(const std::vector<int>& laneXs);
        void draw() const;
        void erase() const;
        void moveLeft();
        void moveRight();
        int getX() const;
        int getY() const;
        int getW() const;
        int getH() const;
        int getLaneIndex() const;
        void setBaseY(int by);
};
