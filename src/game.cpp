#include "game.hpp"
#include "board.hpp"
#include "car.hpp"
#include "enemy.hpp"
#include "renderer.hpp"
#include "utils.hpp"
#include "color.hpp"
#include "cursor_input.hpp"

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>

using namespace std;

Game::Game()
    : board(nullptr), player(nullptr),
      score(0), level(1), running(false),
      spawnTimer(0.0f), spawnInterval(1.0f), baseSpeed(3.5f),
      elapsedSeconds(0)
{
    renderer = new Renderer();
}

Game::~Game() {
    delete board;
    delete player;
    delete renderer;
}

void Game::init() {
    clearScreen();
    int cols = 80, rows = 24;
    get_terminal_size(cols, rows);

    // place board on the left with a margin, like the Othello reference
    int leftMargin = 4;
    int brdW = min(40, cols - 36); // leave room for HUD on right
    int brdH = min(22, rows - 6);
    int brdX = leftMargin;
    int brdY = 3;

    // create board and clear it
    delete board;
    board = new Board(brdX, brdY, brdW, brdH, 0);
    board->clear();

    // lanes: compute 3 lane center x positions inside the board.
    // Avoid using the exact board center (where dash will be) by shifting lanes.
    lanes.clear();
    int innerLeft = brdX + 3;
    int innerRight = brdX + brdW - 6;
    int laneA = innerLeft;
    int laneB = (innerLeft + innerRight) / 2;
    int laneC = innerRight;
    lanes.push_back(laneA);
    lanes.push_back(laneB);
    lanes.push_back(laneC);

    // create player (start in middle lane)
    delete player;
    player = new Car(1, brdY + brdH - 6); // lane set later
    player->setBaseY(brdY + brdH - 6);
    player->setLanes(lanes);

    // prepare enemy pool
    enemies.clear();
    enemies.resize(8);
    for (auto &e : enemies) { /* default constructed */ }

    spawnTimer = 0.0f;
    spawnInterval = 1.0f;
    baseSpeed = 3.5f;
    score = 0;
    level = 1;
    running = true;
    startTime = chrono::steady_clock::now();
}


void Game::processInput() {
    // consume all pending keys this frame (non-blocking)
    while (kbhit()) {
        InputKey key = getInputKey(); // safe: we already checked kbhit()
        switch (key) {
            case InputKey::LEFT:
                player->moveLeft();
                break;

            case InputKey::RIGHT:
                player->moveRight();
                break;

            case InputKey::UP:
            case InputKey::DOWN:
                // not used in lane-based racer — ignore for now
                break;

            case InputKey::ENTER:
                // if you later want a special action on ENTER, handle here
                break;

            case InputKey::R:
                // reset game (re-init)
                init();
                return; // after re-init, skip processing further pending keys
                // NOTE: if you prefer a softer "reset" method, call that instead

            case InputKey::Q:
            case InputKey::ESC:
                running = false;
                return;

            case InputKey::PAUSE:
                // pause loop
                move_cursor(1, get_terminal_rows());
                cout << "Paused. Press P to resume.";
                cout.flush();
                while (true) {
                    if (kbhit()) {
                        InputKey pk = getInputKey();
                        if (pk == InputKey::PAUSE) break;
                        if (pk == InputKey::Q || pk == InputKey::ESC) { running = false; break; }
                    }
                    sleep_ms(60);
                }
                break;

            case InputKey::LEFT_BRACKET:
                // optional: step debug or move cursor in editor
                break;
            case InputKey::RIGHT_BRACKET:
                break;

            case InputKey::NONE:
            default:
                // ignore unknown/none
                break;
        }
    }
}

void Game::spawnEnemyInLane(int laneIndex) {
    // find free slot
    for (auto &e : enemies) {
        if (!e.isActive()) {
            int sx = lanes[laneIndex];
            int sy = board->getY() - 4; // above visible board
            float sp = baseSpeed + (level - 1) * 0.6f + ((float)random_range(0,100)/200.0f);
            e.spawnLane(laneIndex, lanes, sy, sp);
            return;
        }
    }
}

void Game::update(float dt) {
    // update timers
    spawnTimer += dt;
    auto now = chrono::steady_clock::now();
    elapsedSeconds = (int)chrono::duration_cast<chrono::seconds>(now - startTime).count();

    // spawn logic: spawn randomly in lanes every spawnInterval
    if (spawnTimer >= spawnInterval) {
        int lane = random_range(0, 2);
        spawnEnemyInLane(lane);
        spawnTimer = 0.0f;
        spawnInterval = max(0.45f, spawnInterval - 0.005f); // slowly increase spawn rate
    }

    // update enemies
    for (auto &e : enemies) {
        if (!e.isActive()) continue;
        // erase previous, update, then draw later in render
        e.update(dt);

        // if passed bottom of board
        if (e.getY() > board->getY() + board->getHeight()) {
            e.deactivate();
            score++;
            if (score % 6 == 0) {
                level++;
                baseSpeed += 0.5f;
            }
        }
    }

    // collision detection (player vs any active enemy)
    for (auto &e : enemies) {
        if (!e.isActive()) continue;
        // bounding box collision
        int ax1 = player->getX();
        int ay1 = player->getY();
        int ax2 = ax1 + player->getW() - 1;
        int ay2 = ay1 + player->getH() - 1;

        int bx1 = e.getX();
        int by1 = e.getY();
        int bx2 = bx1 + e.getW() - 1;
        int by2 = by1 + e.getH() - 1;

        bool horiz = !(ax2 < bx1 || bx2 < ax1);
        bool vert  = !(ay2 < by1 || by2 < ay1);
        if (horiz && vert) {
            running = false;
            return;
        }
    }
}

void Game::render() {
    clearScreen();

    // HUD top-left title
    move_cursor(2,1);
    setTextColor(TextColor::YELLOW);
    cout << "Terminal Racer (left)  —  Score: " << score;
    resetTextColor();

    // draw board at its x,y
    board->draw();

    // draw dashed center line inside board (animated)
    static int dashOffset = 0;
    dashOffset = (dashOffset + 1) % 4;
    int centerCol = board->getWidth() / 2;
    for (int r = 0; r < board->getHeight(); ++r) {
        if ((r + dashOffset) % 4 == 0) {
            board->setCell(r, centerCol, '|');
        } else {
            // ensure empty if not dash
            char c = board->getCell(r, centerCol);
            if (c == '|') board->setCell(r, centerCol, ' ');
        }
    }

    // re-draw board after altering center col cells
    board->draw();

    // draw enemies and player
    for (auto &e : enemies) {
        if (e.isActive()) e.draw();
    }
    player->draw();

    // Render side menu on the right of board
    int hudX = board->getX() + board->getWidth() + 4;
    int hudY = board->getY() + 1;
    renderer->drawSideMenu(hudX, hudY, score, elapsedSeconds, level, baseSpeed);

    move_cursor(1, get_terminal_rows());
    cout.flush();
}

void Game::run() {
    if (!board) init();

    hideCursor();
    using clock = chrono::steady_clock;
    auto last = clock::now();
    startTime = last;

    while (running) {
        auto now = clock::now();
        float dt = chrono::duration<float>(now - last).count();
        if (dt < 1.0f/60.0f) { this_thread::sleep_for(chrono::milliseconds(3)); continue; }
        last = now;

        processInput();
        update(dt);
        render();
    }

    // game over display
    clearScreen();
    int cols = 80, rows = 24;
    get_terminal_size(cols, rows);
    move_cursor(cols/2 - 12, rows/2 - 1);
    cout << "=====  GAME OVER  =====";
    move_cursor(cols/2 - 10, rows/2 + 1);
    cout << "Score: " << score << "  Level: " << level;
    move_cursor(1, rows);
    showCursor();
}
