#pragma once

#include <string>
#include <vector>

class Car {
    private:
        /* data */
        std::vector<std::string> shape;
        std::vector<int> lanes; // absolute x positions for each lane
        int laneIndex = 1;      // 0..2
        int y = 0;              // top row
        int w = 0, h = 0;
        // std::vector<std::string> shape = {
        //   "  v v  "
        //   " ╔───╗ "
        //   "[╬═══╬]"
        //   " ║███║ "
        //   "[╩───╩]"
        // };

    public:
        Car() = default;
        Car(int startLane, int baseY);

        ~Car() = default;

        void setLanes(const std::vector<int>& laneXs);
        void draw() const;
        void erase() const;

        // move between lanes (index based)
        void moveLeft();
        void moveRight();

        int getX() const;
        int getY() const;
        int getW() const;
        int getH() const;
        int getLaneIndex() const;

        void setBaseY(int by); // set vertical position (top row of sprite)

};
