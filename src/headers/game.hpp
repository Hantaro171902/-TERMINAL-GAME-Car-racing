#pragma once

#include <string>
#include <vector>
#include <chrono>
#include "board.hpp"
#include "car.hpp"
#include "enemy.hpp"

class Renderer;
class Game {
  
    private:
        Board* board;
        Car* player;
        std::vector<Enemy> enemies;
        Renderer* renderer;
        std::vector<int> lanes;
        int score;
        int level;
        bool running;
        float spawnTimer;
        float spawnInterval;
        float baseSpeed;
        int elapsedSeconds;
        std::chrono::steady_clock::time_point startTime;

        void spawnEnemyInLane(int laneIndex);

    public:
        Game();
        ~Game();
        void init();
        void processInput();
        void update(float dt);
        void render();
        void run();
};