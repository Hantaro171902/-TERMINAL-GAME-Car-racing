#pragma once

#include <string>
#include <vector>

class Game {
  private:
    /* data */
    int width;
    int height;
    int score;
    int level;
    bool gameOver;

  public:
    Game(int w, int h) : width(w), height(h), score(0), level(1), gameOver(false) {}
    
    void init();
    void update();
    void render();
    bool isGameOver() const { return gameOver; }
    int getScore() const { return score; }
    int getLevel() const { return level; }
};