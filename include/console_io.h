#ifndef CONSOLE_IO_H
#define CONSOLE_IO_H

#include <iostream>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

//! \brief Arrow key code list
enum arrowKey { UP, DOWN, LEFT, RIGHT, OTHER };

//! \brief Clear the console screen and place the cursor at the top left
inline void clearConsole() noexcept {
    std::cout << "\x1b[2J\x1b[0;0f" << std::flush;
}

//! \brief Clear the line from the cursor position to the end of line
inline void clearEOL() noexcept {
    std::cout << "\x1b[K" << std::flush;
}

//! \brief Set cursor position to (x,y) in the console
//! \param x Horizontal position or column
//! \param y Vertical position or row
inline void gotoxy(int x, int y) noexcept {
    std::cout << "\x1b[" << y << ';' << x << 'f' << std::flush;
}

//! \brief Non-blocking keyboard input detection
//! \return Number of bytes waiting in the keyboard buffer
inline int keypressed() {
    static const int STDIN = 0;
    int bytesWaiting;

    static struct InitTerminal {
        InitTerminal() {
            termios term;
            if (tcgetattr(STDIN, &term) != 0) {
                throw std::runtime_error("Failed to get terminal attributes");
            }
            term.c_lflag &= static_cast<tcflag_t>(~ICANON);
            if (tcsetattr(STDIN, TCSANOW, &term) != 0) {
                throw std::runtime_error("Failed to set terminal attributes");
            }
            setbuf(stdin, nullptr);
            std::cin.sync_with_stdio();
        }
    } init;
    (void)init; // Ensure initialization happens

    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

//! \brief Return the arrow key code from the keyboard buffer if any
//! \return arrowKey code among UP, DOWN, LEFT, RIGHT, OTHER enum values
inline arrowKey getArrowKey() {
    char c;
    arrowKey code = OTHER;

    // Read first character
    c = getchar();
    // Test for escape sequence
    if (c == '\x1b') {
        // Read second character '['
        c = getchar();
        if (c == '[') {
            // Read third character with arrow key code
            c = getchar();
            switch (c) {
                case 'A':
                    code = UP;
                    break;
                case 'B':
                    code = DOWN;
                    break;
                case 'C':
                    code = RIGHT;
                    break;
                case 'D':
                    code = LEFT;
                    break;
                default:
                    code = OTHER;
            }
        }
    }

    // Vider le buffer en mode non-bloquant
    while (keypressed() > 0) {
        getchar();
    }

    return code;
}

//! \brief Wait for a single key press and return the key code
//! \return Key code of the pressed key
inline int getch() {
    int c = 0;
    struct termios org_opts, new_opts;

    //----- store current settings -------------
    if (tcgetattr(STDIN_FILENO, &org_opts) != 0) {
        throw std::runtime_error("Failed to get terminal attributes");
    }
    //----- set new terminal parameters --------
    memcpy(&new_opts, &org_opts, sizeof(new_opts));
    new_opts.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO | ECHOE | ECHOK |
                                                 ECHONL | ECHOPRT | ECHOKE | ICRNL));
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_opts) != 0) {
        throw std::runtime_error("Failed to set terminal attributes");
    }
    //------ wait for a single key -------------
    c = getchar();
    //------ restore current settings ----------
    if (tcsetattr(STDIN_FILENO, TCSANOW, &org_opts) != 0) {
        throw std::runtime_error("Failed to restore terminal attributes");
    }

    return c;
}

#endif // CONSOLE_IO_H