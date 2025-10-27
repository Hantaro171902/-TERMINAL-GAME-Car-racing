#pragma once

#include <string>
#include <vector>
#include "board.hpp"
#include "car.hpp"
#include "enermy.hpp"

class Game {
  
private:
    void processInput();
    void update(float dt);
    void render();
    bool checkCollision(const Car &c, const Enermy &e) const;

private:
    Board *board;
    Car *player;
    std::vector<Enermy> enemies;

    int cols, rows;
    int score;
    int level;
    bool running;

    float spawnTimer;
    float spawnInterval;
    float baseSpeed;

public:
    Game();
    ~Game();

    void init();
    void run();
};