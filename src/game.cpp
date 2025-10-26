#include "game.hpp"
#include "utils.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

void Game::init() {
    score = 0;
    level = 1;
    gameOver = false;
}

void Game::update() {
    // Game logic to update score, level, and check for game over
}

void Game::render() {
    clearScreen();
    cout << "Score: " << score << "  Level: " << level << endl;
    // Additional rendering logic
}

bool Game::isGameOver() const {
    return gameOver;
}

int Game::getScore() const {
    return score;
}

int Game::getLevel() const {
    return level;
}