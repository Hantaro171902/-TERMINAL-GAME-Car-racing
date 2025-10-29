#pragma once

#include <string>
#include <vector>

class Enermy {
    private:
        /* data */
        std::vector<std::string> shape;
        int laneIdx;
        std::vector<int> laneXs;
        float yf;   // y float
        float speed;
        int w, h;
        bool active;
        // std::vector<std::string> shape = {
        //   "[╦───╦]"
        //   " ║███║ "
        //   "[╬═══╬]"
        //   " ╚───╝ "
        // };
    public:
        Enermy();
        ~Enermy() = default;

        void draw() const;
        void erase() const;

        // spawn into lane index (0..2), sy is top Y (can be negative), sp is speed rows/sec
        void spawnLane(int laneIndex, const std::vector<int>& laneXs, int sy, float sp);

        // update by dt seconds (float-based vertical movement)
        void update(float dt);

        bool isActive() const;
        int getX() const;
        int getY() const;
        int getW() const;
        int getH() const;
        int getLaneIndex() const;

        void deactivate();
};
