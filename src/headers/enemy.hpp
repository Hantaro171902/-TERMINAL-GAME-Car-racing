#pragma once

#include <string>
#include <vector>
#include <iostream>

class Enemy {
    private:
        /* data */
		int laneIdx;
		float yf;
		float speed;
		std::vector<int> laneXs;
		std::vector<std::string> shape;
		int w, h;
		bool active;
		int boardBottom;  // New: to clip drawing at board bottom
        // std::vector<std::string> shape = {
        //   "[╦───╦]"
        //   " ║███║ "
        //   "[╬═══╬]"
        //   " ╚───╝ "
        // };
    public:
        Enemy();
		void spawnLane(int laneIndex, const std::vector<int>& laneXs, int sy, float sp, int bottomY);
		void update(float dt);
		void draw() const;
		void erase() const;
		bool isActive() const;
		int getX() const;
		int getY() const;
		int getW() const;
		int getH() const;
		int getLaneIndex() const;
		void deactivate();
};
