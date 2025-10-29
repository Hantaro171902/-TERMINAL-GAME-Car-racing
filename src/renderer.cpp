// renderer.cpp
#include "renderer.hpp"
#include "utils.hpp"
#include "color.hpp"
#include <iostream>
#include <iomanip>
#include <cstdio>

using namespace std;

string Renderer::fmtTime(int secs) const {
    int m = secs / 60;
    int s = secs % 60;
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d", m, s);
    return string(buf);
}

void Renderer::drawSideMenu(int hudX, int hudY, int score, int elapsedSeconds, int level, float speed) const
{
    // draw small boxed hud
    move_cursor(hudX, hudY);
    setTextColor(TextColor::YELLOW);
    cout << "╔════════════════════════╗";
    move_cursor(hudX, hudY + 1);
    cout << "║ Time  " << setw(16) << fmtTime(elapsedSeconds) << " ║";  // Fixed: setw(8)
    move_cursor(hudX, hudY + 2);
    cout << "║ Score " << setw(16) << score << " ║";  // Fixed: setw(8)
    move_cursor(hudX, hudY + 3);
    cout << "║ Level " << setw(16) << level << " ║";  // Fixed: setw(8)
    move_cursor(hudX, hudY + 4);
    // show speed with 2 decimals
    cout << "║ Speed " << setw(16) << fixed << setprecision(1) << speed << " ║";
    move_cursor(hudX, hudY + 5);
    cout << "╚════════════════════════╝";
    resetTextColor();

    // draw instructions box under HUD (leave some vertical spacing)
    drawInstructions(hudX, hudY + 7);
}

void Renderer::drawInstructions(int x, int y) const
{
    move_cursor(x, y);
    setTextColor(TextColor::WHITE);
    cout << "╔════════════════════════╗";
    move_cursor(x, y + 1);
    cout << "║ A / ←  : Move left     ║";
    move_cursor(x, y + 2);
    cout << "║ D / →  : Move right    ║";
    move_cursor(x, y + 3);
    cout << "║ P      : Pause         ║";
    move_cursor(x, y + 4);
    cout << "║ Q / ESC: Quit          ║";
    move_cursor(x, y + 5);
    cout << "╚════════════════════════╝";
    resetTextColor();
}