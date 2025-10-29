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
        void processInput();
        void update(float dt);
        void render();
        
        // helper to spawn into a lane
        void spawnEnemyInLane(int laneIndex);

    private:
        // world objects
        Board* board;               // owned
        Car* player;                // owned
        Renderer* renderer;         // owned
        std::vector<Enermy> enemies;

        // lanes (absolute X positions of 3 lanes, computed in init)
        std::vector<int> lanes;

        // gameplay state
        int score;
        int level;
        bool running;

        // spawn / difficulty
        float spawnTimer;
        float spawnInterval;
        float baseSpeed;

        // timing / HUD
        std::chrono::steady_clock::time_point startTime;
        int elapsedSeconds;

    public:
        Game();
        ~Game();

        void init();
        void run();
};