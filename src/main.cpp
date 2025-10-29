#include <iostream>
#include <cstdlib>
#include <csignal>
#include <vector>
#include "game.hpp"
#include "color.hpp"
#include "utils.hpp"
#include "renderer.hpp"

using namespace std;

void cleanup(int signum) {
    // Avoid heavy terminal functions here in signal handler; keep it simple.
    // We'll restore cursor in main cleanup path instead.
    // But a best-effort call:
    showCursor();
    exit(signum);
}

int main() {
    // Register signals (okay for now, but see note below)
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);

    Game game;
    game.init();   // <<--- important: initialize resources before running
    game.run();

    showCursor();
    return 0;
}
