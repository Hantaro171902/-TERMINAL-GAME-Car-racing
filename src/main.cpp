#include "game.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    srand(static_cast<unsigned>(time(nullptr)));  // Seed random
    Game game;
    game.init();
    game.run();
    return 0;
}