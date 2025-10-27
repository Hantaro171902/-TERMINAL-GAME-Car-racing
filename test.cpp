// terminal_racer.cpp
// Build: g++ -std=c++17 terminal_racer.cpp -O2 -o terminal_racer
// Run: ./terminal_racer
#include <bits/stdc++.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
using namespace std;

/* ----------------- Utilities (terminal / input) ----------------- */
static struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    cout << "\033[?25h"; // show cursor
    cout.flush();
}
void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON); // no echo, non-canonical
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    // make stdin non-blocking
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    cout << "\033[?25l"; // hide cursor
}

void clear_screen() {
    cout << "\033[2J\033[H";
}

void move_cursor(int x, int y) {
    // x,y are 1-based for ANSI
    cout << "\033[" << y << ";" << x << "H";
}

int get_terminal_cols() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) return 80;
    return w.ws_col;
}
int get_terminal_rows() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) return 24;
    return w.ws_row;
}

// read a single char if available (non-blocking), returns -1 if none
int read_input_char() {
    char c;
    ssize_t n = read(STDIN_FILENO, &c, 1);
    if (n == 1) return (unsigned char)c;
    return -1;
}

/* ----------------- Basic types ----------------- */
struct Rect {
    int x, y, w, h;
};

int clampi(int v, int a, int b){ return (v < a ? a : (v > b ? b : v)); }
int rand_range(int a, int b){ return rand() % (b - a + 1) + a; }

/* ----------------- Car ----------------- */
class Car {
public:
    int x, y;             // top-left column,row (1-based printing uses move_cursor)
    vector<string> spr;   // shape lines
    int w, h;
    Car(int startX = 0, int startY = 0) {
        spr = {
            "  __ ",
            " /__\\",
            " |[]|"
        };
        w = (int)spr[0].size();
        h = (int)spr.size();
        x = startX; y = startY;
    }
    void draw() const {
        for (int i = 0; i < h; ++i) {
            move_cursor(x, y + i);
            cout << spr[i];
        }
    }
    void erase() const {
        for (int i = 0; i < h; ++i) {
            move_cursor(x, y + i);
            cout << string(w, ' ');
        }
    }
    void moveLeft(int leftBound) {
        x = max(leftBound, x - 3);
    }
    void moveRight(int rightBound) {
        x = min(rightBound - w + 1, x + 3);
    }
    Rect bbox() const { return {x, y, w, h}; }
};

/* ----------------- Enemy ----------------- */
class Enemy {
public:
    float yf;            // y as float for smooth movement
    int x;               // x column (int)
    vector<string> spr;
    int w, h;
    float speed;         // rows per second
    bool active;
    Enemy(): yf(0), x(0), w(0), h(0), speed(1.0f), active(false) {
        spr = { " [##] ", "  <>  " };
        w = (int)spr[0].size();
        h = (int)spr.size();
    }
    void spawn(int sx, float startY, float sp) {
        x = sx;
        yf = startY;
        speed = sp;
        active = true;
    }
    void update(float dt) {
        if (!active) return;
        yf += speed * dt;
    }
    void draw() const {
        if (!active) return;
        int yi = (int)round(yf);
        for (int i = 0; i < h; ++i) {
            move_cursor(x, yi + i);
            cout << spr[i];
        }
    }
    Rect bbox() const {
        return { x, (int)round(yf), w, h };
    }
    void deactivate(){ active = false; }
};

/* ----------------- Road ----------------- */
class Road {
public:
    int x, y, width, height;
    int dashOffset;
    Road(int sx, int sy, int w, int h): x(sx), y(sy), width(w), height(h), dashOffset(0) {}
    void scroll(){ dashOffset = (dashOffset + 1) % 4; }
    void draw() const {
        // left and right borders and dashed center line
        for (int r = 0; r < height; ++r) {
            int row = y + r;
            move_cursor(x, row);
            cout << "║";
            move_cursor(x + width - 1, row);
            cout << "║";

            // dashed center: every 4th row depending on dashOffset
            if ((r + dashOffset) % 4 == 0) {
                move_cursor(x + width/2, row);
                cout << "│";
            }
        }
        // top/bottom caps (single char)
        move_cursor(x, y - 1);
        cout << "╔";
        for(int i = 1; i < width-1; ++i) cout << "═";
        cout << "╗";
        move_cursor(x, y + height);
        cout << "╚";
        for(int i = 1; i < width-1; ++i) cout << "═";
        cout << "╝";
    }
};

/* ----------------- Collision ----------------- */
bool rects_intersect(const Rect &a, const Rect &b) {
    int ax2 = a.x + a.w - 1, ay2 = a.y + a.h - 1;
    int bx2 = b.x + b.w - 1, by2 = b.y + b.h - 1;
    bool horiz = !(ax2 < b.x || bx2 < a.x);
    bool vert  = !(ay2 < b.y || by2 < a.y);
    return horiz && vert;
}

/* ----------------- Game ----------------- */
class Game {
    int cols, rows;
    Road *road;
    Car *player;
    vector<Enemy> enemies;
    float enemySpawnTimer;
    float spawnInterval;
    float baseEnemySpeed;
    int score;
    int level;
    bool running;
    chrono::steady_clock::time_point lastFrame;
public:
    Game(): road(nullptr), player(nullptr), enemySpawnTimer(0),
            spawnInterval(1.2f), baseEnemySpeed(3.0f),
            score(0), level(1), running(true)
    {
        cols = get_terminal_cols();
        rows = get_terminal_rows();
    }
    ~Game(){
        delete road;
        delete player;
    }
    void init() {
        clear_screen();
        cols = get_terminal_cols();
        rows = get_terminal_rows();
        // choose road area (centered)
        int rw = min(60, cols - 20);
        int rx = max(2, (cols - rw)/2);
        int ry = 2;
        int rh = min(22, rows - 6);
        road = new Road(rx, ry, rw, rh);
        // player start
        player = new Car(rx + rw/2 - 3, ry + rh - 4);
        // pre-create enemy pool
        enemies.resize(8);
        enemySpawnTimer = 0.0f;
        spawnInterval = 1.0f;
        baseEnemySpeed = 3.6f;
        score = 0;
        level = 1;
        lastFrame = chrono::steady_clock::now();
    }
    void spawn_enemy() {
        // find inactive slot
        for (auto &e : enemies) {
            if (!e.active) {
                int leftBound = road->x + 2;
                int rightBound = road->x + road->width - e.w - 2;
                int sx = rand_range(leftBound, rightBound);
                float sp = baseEnemySpeed + (level - 1) * 0.8f + ((float)rand_range(0,100)/200.0f);
                e.spawn(sx, (float)road->y - 3.0f, sp);
                return;
            }
        }
        // no free slot -> optionally increase pool
    }
    void update(float dt) {
        // input
        int ch = read_input_char();
        if (ch != -1) {
            if (ch == 'a' || ch == 'A' || ch == 68) player->moveLeft(road->x + 2); // left arrow fallback 68?
            if (ch == 'd' || ch == 'D' || ch == 67) player->moveRight(road->x + road->width - 3);
            if (ch == 'q' || ch == 'Q' ) running = false;
            if (ch == 27) running = false; // ESC
            if (ch == 'p' || ch == 'P') pause_loop();
        }

        // spawn logic
        enemySpawnTimer += dt;
        if (enemySpawnTimer >= spawnInterval) {
            spawn_enemy();
            enemySpawnTimer = 0;
            // slowly reduce interval to increase difficulty
            spawnInterval = max(0.45f, spawnInterval - 0.01f);
        }

        // update enemies
        for (auto &e : enemies) {
            if (!e.active) continue;
            e.update(dt);
            // scored / passed bottom
            if (e.bbox().y > road->y + road->height) {
                e.deactivate();
                score += 1;
                if (score % 6 == 0) {
                    level++;
                    baseEnemySpeed += 0.6f;
                }
            }
            // collision
            if (rects_intersect(player->bbox(), e.bbox())) {
                running = false;
            }
        }

        // road scroll illusion
        road->scroll();
    }
    void render() {
        clear_screen();
        // HUD
        move_cursor(2,1);
        cout << "Terminal Racer  —  Score: " << score << "  Level: " << level << "    (A/D or arrows to move, P pause, Q quit)";
        // draw road & actors
        road->draw();
        player->draw();
        for (const auto &e : enemies) if (e.active) e.draw();

        // small ground at bottom
        move_cursor(1, rows);
        cout.flush();
    }
    void run() {
        lastFrame = chrono::steady_clock::now();
        while (running) {
            auto now = chrono::steady_clock::now();
            float dt = chrono::duration<float>(now - lastFrame).count();
            if (dt < 1.0f/60.0f) { // cap to ~60 FPS busy-wait style
                this_thread::sleep_for(chrono::milliseconds(3));
                continue;
            }
            lastFrame = now;
            update(dt);
            render();
        }
        // game over screen
        clear_screen();
        move_cursor(cols/2 - 12, rows/2 - 1);
        cout << "===== GAME OVER =====";
        move_cursor(cols/2 - 10, rows/2 + 1);
        cout << "Score: " << score << "   Level: " << level;
        move_cursor(cols/2 - 16, rows/2 + 3);
        cout << "Press any key to exit...";
        cout.flush();
        // wait for key
        while (read_input_char() == -1) { this_thread::sleep_for(chrono::milliseconds(30)); }
    }
    void pause_loop() {
        move_cursor(1, get_terminal_rows());
        cout << "Paused. Press 'p' to resume.";
        cout.flush();
        while (true) {
            int c = read_input_char();
            if (c == 'p' || c == 'P') break;
            if (c == 'q' || c == 'Q' || c == 27) { running = false; break; }
            this_thread::sleep_for(chrono::milliseconds(60));
        }
    }
};

/* ----------------- main ----------------- */
int main(){
    srand((unsigned)time(nullptr));
    enable_raw_mode();

    Game g;
    g.init();
    g.run();

    // terminal restored by atexit -> disable_raw_mode will run
    return 0;
}
