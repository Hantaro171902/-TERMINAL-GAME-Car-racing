// Updated game.cpp with narrow board (9 cells wide, ~20 printed chars) like reference
#include "game.hpp"
#include "board.hpp"
#include "car.hpp"
#include "enemy.hpp"
#include "renderer.hpp"
#include "utils.hpp"
#include "color.hpp"
#include "cursor_input.hpp"

#include <iostream>
#include <fstream> // For file output
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include <iomanip>

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
    cerr << "Terminal size: cols=" << cols << ", rows=" << rows << endl;

    // Total inner cells: 64 - 2 borders = 62 (3x20 lanes + 2 dashes)
    // For 5 lanes, we need 4 separators. Each lane is 10 cells wide (20 printed chars).
    // 5 lanes * 10 cells/lane + 4 separators * 1 cell/separator = 54 cells
    int brdW = 54; // 5 lanes * 10 cells/lane + 4 separators
    int brdH = 20;

    // Auto-adjust if needed
    int printedWidth = 2 * brdW + 2;
    if (brdH + printedWidth + 30 > cols) {
        brdW = max(30, (cols - 40) / 2);
    }

    int brdX = 4;
    int brdY = 3;

    delete board;
    board = new Board(brdX, brdY, brdW, brdH, 0);
    board->clear();

    // Lanes: 5 lanes, each 10 cells wide. Separators are 1 cell wide.
    // Lane 0: cells 0-9 (center 4)
    // Separator 0: cell 10
    // Lane 1: cells 11-20 (center 15)
    // Separator 1: cell 21
    // Lane 2: cells 22-31 (center 26)
    // Separator 2: cell 32
    // Lane 3: cells 33-42 (center 37)
    // Separator 3: cell 43
    // Lane 4: cells 44-53 (center 48)

    lanes.clear();
    int w = board->getWidth();  // 54

    // Center columns for each lane (approximate, adjusted for sprite width later)
    int c0 = 4;   // Lane 0 center
    int c1 = 15;  // Lane 1 center
    int c2 = 26;  // Lane 2 center
    int c3 = 37;  // Lane 3 center
    int c4 = 48;  // Lane 4 center

    auto cell_to_x = [&](int colIndex)->int{
        return brdX + 1 + 2 * colIndex; // Each cell is 2 printed characters
    };

    // Player starts in middle lane (index 2)
    delete player;
    player = new Car(2, brdY + brdH - 6);
    player->setBaseY(brdY + brdH - 6);

    int spriteW = player->getW();

    int laneX0 = cell_to_x(c0) - (spriteW / 2);
    int laneX1 = cell_to_x(c1) - (spriteW / 2);
    int laneX2 = cell_to_x(c2) - (spriteW / 2);
    int laneX3 = cell_to_x(c3) - (spriteW / 2);
    int laneX4 = cell_to_x(c4) - (spriteW / 2);

    // Clamp
    auto clampToBoard = [&](int xVal)->int{
        int leftBound = brdX + 1;
        int rightBound = brdX + 1 + 2 * (w - 1) - (spriteW - 1);
        if (xVal < leftBound) return leftBound;
        if (xVal > rightBound) return rightBound;
        return xVal;
    };

    lanes.push_back(clampToBoard(laneX0));
    lanes.push_back(clampToBoard(laneX1));
    lanes.push_back(clampToBoard(laneX2));
    lanes.push_back(clampToBoard(laneX3));
    lanes.push_back(clampToBoard(laneX4));

    player->setLanes(lanes);

    // Enemies
    enemies.clear();
    enemies.resize(8);
    spawnTimer = 0.0f;
    spawnInterval = 3.0f; // Start with a longer interval
    baseSpeed = 1.0f;     // Start with a slower speed

    score = 0;
    level = 1;
    elapsedSeconds = 0;
    startTime = chrono::steady_clock::now();

    running = true;
    cerr << "init() completed. running = " << (running ? "true" : "false") << endl;
}


void Game::processInput() {
    InputKey key = getInputKey();
    switch (key) {
        case InputKey::LEFT:
            player->moveLeft();
            break;
        case InputKey::RIGHT:
            player->moveRight();
            break;
        case InputKey::UP:
        case InputKey::DOWN:
            break;
        case InputKey::ENTER:
            break;
        case InputKey::R:
            init();
            return;
        case InputKey::Q:
        case InputKey::ESC:
            // Debug print to check if Q or ESC is being detected
            cerr << "Detected Q or ESC. Setting running to false." << endl;
            running = false;
            return;
        case InputKey::PAUSE:
            move_cursor(1, get_terminal_rows());
            cout << "Paused. Press P to resume.";
            cout.flush();
            while (true) {
                InputKey pk = getInputKey();
                if (pk == InputKey::PAUSE) break;
                if (pk == InputKey::Q || pk == InputKey::ESC) { running = false; break; }
                sleep_ms(60);
            }
            break;
        case InputKey::LEFT_BRACKET:
        case InputKey::RIGHT_BRACKET:
            break;
        case InputKey::NONE:
        default:
            break;
    }
}

void Game::spawnEnemyInLane(int laneIndex) {
    for (auto &e : enemies) {
        if (!e.isActive()) {
            int sx = lanes[laneIndex];
            int sy = board->getY() - 20;  // Spawn much further up
            float sp = baseSpeed + (level - 1) * 0.1f + ((float)random_range(0, 100) / 200.0f);
            e.spawnLane(laneIndex, lanes, sy, sp);
            return;
        }
    }
}

void Game::update(float dt) {
    spawnTimer += dt;
    auto now = chrono::steady_clock::now();
    elapsedSeconds = (int)chrono::duration_cast<chrono::seconds>(now - startTime).count();

    if (spawnTimer >= spawnInterval) {
        int lane = random_range(0, 4); // Now 5 lanes (0-4)
        spawnEnemyInLane(lane);
        spawnTimer = 0.0f;
        spawnInterval = max(0.45f, spawnInterval - 0.001f);
    }

    for (auto &e : enemies) {
        if (!e.isActive()) continue;
        e.update(dt);
    }

    // collision (simple overlap like ref)
    // for (auto &e : enemies) {
    //     if (!e.isActive()) continue;

    //     // if (e.getY() < board->getY() - e.getH() / 2) continue; // Re-enabled

    //     int ax1 = player->getX();
    //     int ay1 = player->getY();
    //     int ax2 = ax1 + player->getW() - 1;
    //     int ay2 = ay1 + player->getH() - 1;

    //     int bx1 = e.getX();
    //     int by1 = e.getY();
    //     int bx2 = bx1 + e.getW() - 1;
    //     int by2 = by1 + e.getH() - 1;

    //     bool horiz = !(ax2 < bx1 || bx2 < ax1);
    //     bool vert  = !(ay2 < by1 || by2 < ay1);

    //     if (horiz && vert) {
    //         running = false;
    //         return;
    //     }

    //     if (e.getY() > board->getY() + board->getHeight()) {
    //         e.deactivate();
    //         score++;
    //         if (score % 6 == 0) {
    //             level++;
    //             baseSpeed += 0.1f; // Increase speed more gradually
    //         }
    //     }
    // }
}

void Game::render() {
    clearScreen();

    // Title like ref (simple)
    move_cursor(2,1);
    setTextColor(TextColor::YELLOW);
    cout << "Terminal Racer — Score: " << score;
    resetTextColor();

    // Draw board (narrow)
    board->draw();

    // Animated dashes for 5 lanes (4 separators)
    static int dashOffset = 0; // To animate the dashes
    dashOffset = (dashOffset + 1) % 4; // Cycle 0, 1, 2, 3

    // Separator columns (based on 10 cells per lane)
    int sepCols[] = {10, 21, 32, 43};

    for (int r = 0; r < board->getHeight(); ++r) {
        for (int i = 0; i < 4; ++i) { // For each of the 4 separators
            int col = sepCols[i];
            // Animate: dash appears every 4 rows, offset by dashOffset
            if ((r + dashOffset) % 4 == 0) {
                board->setCell(r, col, '|');
            }
            else {
                board->setCell(r, col, ' ');
            }
        }
    }
    board->draw();  // Redraw with dashes

    // Draw enemies and player (direct print like ref)
    for (auto &e : enemies) {
        if (e.isActive()) e.draw();
    }
    player->draw();

    // HUD right of narrow board (starts ~ col 25)
    int hudX = board->getX() + 2 * board->getWidth() + 3;  // After ║ + space
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
        last = now;

        processInput();
        update(dt);
        render();

        float frame_time = 1.0f / 10.0f;  // Slower like ref Sleep(100) ~10 FPS
        float sleep_duration = frame_time - dt;
        if (sleep_duration > 0.0f) {
            this_thread::sleep_for(chrono::duration<float>(sleep_duration));
        }
    }

    // Game over
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