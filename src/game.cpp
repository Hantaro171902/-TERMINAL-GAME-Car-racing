#include "game.hpp"
#include "utils.hpp"
#include "color.hpp"
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

Game::Game()
    : board(nullptr), player(nullptr), cols(80), rows(24),
      score(0), level(1), running(true),
      spawnTimer(0.0f), spawnInterval(1.0f), baseSpeed(1.0f)
{ }

Game::~Game() {
    delete board;
    delete player;
}

void Game::init() {
    clearScreen();
    int termCols = 80, termRows = 24;
    get_terminal_size(termCols, termRows);
    cols = termCols; rows = termRows;

    int brdW = min(60, cols - 20);
    int brdX = max(2, (cols - brdW) / 2);
    int brdY = 2;
    int brdH = min(22, rows - 6);

    board = new Board(brdX, brdY, brdW, brdH);
    board->clear();

    // draw initial dashed center line into board (as characters) by toggling in update
    player = new Car(brdX + brdW / 2 - 3, brdY + brdH - 4);

    enemies.resize(6);
    spawnTimer = 0.0f;
    spawnInterval = 1.1f;
    baseSpeed = 1.0f;
    score = 0; level = 1;
}

void Game::run() {
    using clock = chrono::steady_clock;
    auto last = clock::now();
    running = true;

    hideCursor();
    while (running) {
        auto now = clock::now();
        float dt = chrono::duration<float>(now - last).count();
        if (dt < 1.0f/40.0f) { // aim 40 fps
            this_thread::sleep_for(chrono::milliseconds(4));
            continue;
        }
        last = now;

        processInput();
        update(dt);
        render();
    }
    showCursor();

    // Game over screen
    clearScreen();
    move_cursor(cols/2 - 10, rows/2 - 1);
    cout << "=== GAME OVER ===";
    move_cursor(cols/2 - 10, rows/2 + 1);
    cout << "Score: " << score << "  Level: " << level;
    move_cursor(1, rows);
}

void Game::processInput() {
    if (kbhit()) {
        int ch = getch();
        if (ch == 'a' || ch == 'A') player->moveLeft(board->getX() + 2);
        else if (ch == 'd' || ch == 'D') player->moveRight(board->getX() + board->getWidth() - 2);
        else if (ch == 'q' || ch == 'Q' || ch == 27) running = false;
        else if (ch == 'p' || ch == 'P') {
            move_cursor(1, rows);
            cout << "Paused. Press P to resume.";
            while (true) {
                if (kbhit()) {
                    int c = getch();
                    if (c == 'p' || c == 'P') break;
                    if (c == 'q' || c == 'Q' || c == 27) { running = false; break; }
                }
                sleep_ms(80);
            }
        }
    }
}

void Game::update(float dt) {
    // spawn control
    spawnTimer += dt;
    if (spawnTimer >= spawnInterval) {
        // find free slot
        for (auto &e : enemies) {
            if (!e.isActive()) {
                int left = board->getX() + 2;
                int right = board->getX() + board->getWidth() - e.getW() - 2;
                int sx = random_range(left, right);
                e.spawn(sx, board->getY() - 3, baseSpeed + (level - 1) * 0.4f);
                break;
            }
        }
        spawnTimer = 0.0f;
        spawnInterval = max(0.45f, spawnInterval - 0.01f);
    }

    // update enemies (simple integer movement)
    for (auto &e : enemies) {
        if (!e.isActive()) continue;
        // move down based on speed and dt; we'll step by integer rows for simplicity
        static float accum = 0.0f; // small per-enemy accumulator would be ideal, but keep simple
        accum += dt * e.getH() * e.getW(); // small noisy accumulator so enemies feel different
        e.moveDown(); // one step per update; you can refine with floats if you want
        // if passed bottom
        if (e.getY() > board->getY() + board->getHeight()) {
            e.erase();
            // deactivate by moving y way off and toggling active (we used spawn()/isActive())
            // reinitialize by setting active false through spawn to false - not provided; do manual:
            // naive: mark inactive by moving x,y off-screen
            // But Enermy does not expose deactivate; simplest: recreate by setting y negative and active false by re-spawning new Enermy
            e = Enermy(); // reset slot
            score++;
            if (score % 6 == 0) {
                level++;
                baseSpeed += 0.5f;
            }
        }
        // collision
        if (checkCollision(*player, e)) {
            running = false;
            return;
        }
    }

    // animate center dashed line inside board: shift pattern each update
    // we'll toggle every call by clearing board and drawing a dashed center
    board->clear();
    int centerCol = board->getWidth() / 2;
    static int dashOffset = 0;
    dashOffset = (dashOffset + 1) % 4;
    for (int r = 0; r < board->getHeight(); ++r) {
        if ((r + dashOffset) % 4 == 0) board->setCell(r, centerCol, '|');
    }
}

void Game::render() {
    clearScreen();
    // HUD
    move_cursor(2, 1);
    cout << "Terminal Racer  -  Score: " << score << "  Level: " << level << "  (A/D move, P pause, Q quit)";

    // board (road)
    board->draw();

    // draw enemies and player
    for (auto &e : enemies) if (e.isActive()) e.draw();
    player->draw();

    move_cursor(1, rows);
    cout.flush();
}

bool Game::checkCollision(const Car &c, const Enermy &e) const {
    if (!e.isActive()) return false;
    int ax1 = c.getX(), ay1 = c.getY(), ax2 = ax1 + c.getW() - 1, ay2 = ay1 + c.getH() - 1;
    int bx1 = e.getX(), by1 = e.getY(), bx2 = bx1 + e.getW() - 1, by2 = by1 + e.getH() - 1;
    bool horiz = !(ax2 < bx1 || bx2 < ax1);
    bool vert  = !(ay2 < by1 || by2 < ay1);
    return horiz && vert;
}