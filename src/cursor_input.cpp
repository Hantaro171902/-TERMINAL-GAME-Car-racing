#include "cursor_input.hpp"
#include "utils.hpp"
#include <iostream>
#include <termios.h>
#include <unistd.h>     // for read()
#include <fcntl.h>      // for fcntl()

using namespace std;

InputKey getInputKey() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    InputKey key = InputKey::NONE;
    char ch = 0;

    // Set non-blocking
    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    if (read(STDIN_FILENO, &ch, 1) > 0) {
        if (ch == '\033') { // Arrow key
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) > 0 && read(STDIN_FILENO, &seq[1], 1) > 0) {
                if (seq[0] == '[') {
                    switch (seq[1]) {
                        case 'A': key = InputKey::UP; break;
                        case 'B': key = InputKey::DOWN; break;
                        case 'C': key = InputKey::RIGHT; break;
                        case 'D': key = InputKey::LEFT; break;
                    }
                }
            }
        } else {
            switch (ch) {
                case 'w': case 'W': key = InputKey::UP; break;
                case 's': case 'S': key = InputKey::DOWN; break;
                case 'a': case 'A': key = InputKey::LEFT; break;
                case 'd': case 'D': key = InputKey::RIGHT; break;
                case '\n': key = InputKey::ENTER; break;
                case 27:   key = InputKey::ESC; break;
                case 'q': case 'Q': key = InputKey::Q; break;
                case 'p': case 'P': key = InputKey::PAUSE; break;
                case 'r': case 'R': key = InputKey::R; break;
                case '[': key = InputKey::LEFT_BRACKET; break;
                case ']': key = InputKey::RIGHT_BRACKET; break;
                default:   key = InputKey::NONE; break;
            }
        }
    }

    // Restore blocking and terminal settings
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return key;
}

// void playSound(SoundEffect effect) {
//     switch (effect) {
//         case CLICK:   system("aplay sounds/click.wav &"); break;
//         case VICTORY: system("aplay sounds/victory.wav &"); break;
//         case DEFEAT:  system("aplay sounds/defeat.wav &"); break;
//     }
// }