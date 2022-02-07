#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "clui.h"

#define SIZE 110

#define CHAR_TO_NUM(c) c-48

int OpenSquare(int blockNumber);

int CountOfBombsNearSquare(int blockNumber);

void AddMines();

void NewGameButton();

void QuitButton();

void PlaceFlagButton();

void SelectSquareButton();

void PrintFlagInsideSquare();

void PrintBombsInsideSquare();

void ChangeDifficultlyLevel();

void ModeButton(bool modeClicked);

void PrintBombLeft(int column);

void ClearLine(int line, int column);

void PrintToolbar(int row, int column);

void PrintCountOfBombsInsideSquare(int blockNumber);

void ClickGuide(int button, int row, int column, bool modeClicked);

void PrintMapBorders(int firstPointOfMapx, int firstPointOfMapy);

void GameLogic(char inputCharacter, int windowRow, int windowColumn);

void PrintNumbersInsideSquare(int firstPointOfMapx, int firstPointOfMapy, bool showMines);

bool g_mineSquare[SIZE];
bool g_selectedSquare[SIZE];
int g_flagSquare[SIZE];
int g_numberMines = 6;
int g_onPressed;
int g_width = 5;
int g_height = 5;
const int g_SQUARE_WIDTH = 5;

int main() {
    int windowRow, windowColumn;
    unsigned char input;

    srand(time(0));

    // Initialize library
    init_clui();

    // Get size of terminal with 2 functions for rows and cols
    windowRow = get_window_rows();
    windowColumn = get_window_cols();

    // Checking size of terminal window
    while (windowRow < 30 || windowColumn < 92) {
        cursor_to_pos(1, 1);
        printf("OOPS! YOUR WINDOW MUST BIGGER.\n");
        printf("Increase the window size and then press any key...\n");
        getch();
        windowRow = get_window_rows();
        windowColumn = get_window_cols();
    }

    // Preparing game
    clear_screen();
    AddMines();
    PrintMapBorders(22, 4);
    PrintNumbersInsideSquare(22, 4, 0);
    PrintToolbar(windowRow, windowColumn);
    PrintBombLeft(windowColumn);
    SelectSquareButton();
    PlaceFlagButton();
    ModeButton(false);
    NewGameButton();
    QuitButton();
    ClickGuide(g_onPressed, windowRow, windowColumn, 0);
    if (g_onPressed == 0 || g_onPressed == 1)
        restore_cursor();

    while (true) {
        input = getch();

        // clear error line
        save_cursor();
        ClearLine(2, windowColumn);
        restore_cursor();

        // Valid input characters
        if (input == 13 || input == 8 || input == 27 || (input >= '0' && input <= '9') || input == 'w' || input == 'W'
            || input == 's' || input == 'S' || input == 'a' || input == 'A' || input == 'd' || input == 'D')
            GameLogic(input, windowRow, windowColumn);

        // Arrow keys pressed
        else if (input == 224) {
            input = getch();
            GameLogic(input, windowRow, windowColumn);
        }
    }
}

// Adding Bombs in the game map
void AddMines() {
    int blockNumber;
    for (int i = 0; i < g_numberMines; ++i) {
        blockNumber = rand() % (g_height * g_width) + 1;

        // Avoiding duplicate number
        if (g_mineSquare[blockNumber] == true) {
            i--;
            continue;
        }

        g_mineSquare[blockNumber] = 1;
    }
}

int CountOfBombsNearSquare(int blockNumber) {
    int result = 0;

    for (int i = blockNumber - g_width - 1; i <= (blockNumber - g_width - 1) + 2 * g_width; i += g_width) {
        // Avoiding AIOOB
        if (i < 0)
            continue;
        if (i > g_width * g_height)
            break;

        // Special squares on the left
        if (blockNumber % g_width == 1) {
            for (int j = i + 1; j <= i + 2; ++j) {
                if (g_mineSquare[j])
                    ++result;
            }
        }

        // Special squares on the right
        else if (blockNumber % g_width == 0) {
            for (int j = i; j <= i + 1; ++j) {

                if (g_mineSquare[j])
                    ++result;
            }

        // Common squares
        } else {
            for (int j = i; j <= i + 2; ++j) {
                if (g_mineSquare[j])
                    ++result;
            }
        }
    }
    return result;
}

void PrintMapBorders(int firstPointOfMapx, int firstPointOfMapy) {

    int cursorRow = firstPointOfMapy;
    int cursorColumn = firstPointOfMapx - 1;

    cursor_to_pos(cursorRow, cursorColumn);

    // Printing top of the map ===========
    for (int i = 0; i < g_SQUARE_WIDTH * g_width + g_width + 3; ++i) {
        printf("=");
    }

    // Printing rows -------------
    for (int i = 0; i < g_width - 1; ++i) {
        cursorRow += 2;
        cursor_to_pos(cursorRow, cursorColumn + 2);

        for (int j = 0; j < g_SQUARE_WIDTH * g_width + g_width - 1; ++j) {
            printf("-");
        }
    }

    // Printing columns ||||||||||
    cursorRow = firstPointOfMapy + 1;
    cursorColumn = firstPointOfMapx;

    for (int i = 0; i < g_width + 1; ++i) {
        for (int j = 0; j < g_height; ++j) {
            cursor_to_pos(cursorRow, cursorColumn);
            printf("|");
            cursorRow += 2;
        }
        cursorColumn += g_SQUARE_WIDTH + 1;
        cursorRow = firstPointOfMapy + 1;
    }

    // Printing bottom =========
    cursorRow = firstPointOfMapy + 2 * g_height;
    cursorColumn = firstPointOfMapx - 1;
    cursor_to_pos(cursorRow, cursorColumn);

    for (int i = 0; i < g_SQUARE_WIDTH * g_width + g_width + 3; ++i) {
        printf("=");
    }

    // Printing right and left border
    cursorRow = firstPointOfMapy + 1;
    cursorColumn = firstPointOfMapx - 1;
    cursor_to_pos(cursorRow, cursorColumn);

    for (int i = 0; i < 2 * g_height - 1; ++i) {
        printf("|");
        ++cursorRow;
        cursor_to_pos(cursorRow, cursorColumn);
    }

    cursorRow = firstPointOfMapy + 1;
    cursorColumn = firstPointOfMapx + (g_SQUARE_WIDTH * g_width + g_width + 1);
    cursor_to_pos(cursorRow, cursorColumn);

    for (int i = 0; i < 2 * g_height - 1; ++i) {
        printf("|");
        ++cursorRow;
        cursor_to_pos(cursorRow, cursorColumn);
    }

/* The map will be like this
        =================================
        ||     |     |     |     |     ||
        |------------------------------ |
        ||     |     |     |     |     ||
        |------------------------------ |
        ||     |     |     |     |     ||
        |------------------------------ |
        ||     |     |     |     |     ||
        |------------------------------ |
        ||     |     |     |     |     ||
        =================================
*/
}

void PrintNumbersInsideSquare(int firstPointOfMapx, int firstPointOfMapy, bool showMines) {

    change_color_rgb(138, 144, 158);
    int cursorRow = firstPointOfMapy + 1;
    int cursorColumn = firstPointOfMapx + 2;

    cursor_to_pos(cursorRow, cursorColumn);

    for (int i = 1; i <= g_width * g_height; ++i) {

        // Printing number of each square
        if (g_selectedSquare[i] == false) {
            if (i < 10)
                printf("0%d", i);
            else
                printf("%d", i);

            if (showMines && g_mineSquare[i]) {
                PrintBombsInsideSquare();
            }
            // Printing flag
            else if (g_flagSquare[i])
                PrintFlagInsideSquare();
        } else
            PrintCountOfBombsInsideSquare(i);

        // Changing position of cursor
        if (i % g_width == 0) {
            cursorColumn = firstPointOfMapx + 2;
            cursorRow += 2;

        } else
            cursorColumn += g_SQUARE_WIDTH + 1;

        cursor_to_pos(cursorRow, cursorColumn);
    }
    reset_color();
}

// Print "F" on the right of number: 35F
void PrintFlagInsideSquare() {
    change_color_rgb(224, 61, 115);
    printf("F");
    change_color_rgb(138, 144, 158);
    // Moving back the cursor to avoid any problem for other numbers when printing
    cursor_backward(1);
}

void PrintBombsInsideSquare() {
    change_color_rgb(245, 66, 147);
    printf("*");
    change_color_rgb(138, 144, 158);
    // Moving back the cursor to avoid any problem for other numbers when printing
    cursor_backward(1);
}

void PrintCountOfBombsInsideSquare(int blockNumber) {
    int count = CountOfBombsNearSquare(blockNumber);

    if (count == 0)
        return;

    if (count == 1)
        change_color_rgb(36, 108, 224);

    else if (count == 2)
        change_color_rgb(0, 224, 63);

    else if (count == 3)
        change_color_rgb(255, 247, 0);

    else if (count == 4)
        change_color_rgb(201, 104, 18);

    else
        change_color_rgb(224, 52, 0);

    cursor_forward(1);
    printf("%d", count);

    change_color_rgb(138, 144, 158);
}

void PrintBombLeft(int column) {
    cursor_to_pos(1, column - 15);
    printf("Bombs Left: %d", g_numberMines - g_flagSquare[101]);
}

int OpenSquare(int blockNumber) {
    //0: has already opened
    //1: exit function
    //2: booooom!

    // Avoid AIOOB
    if (g_selectedSquare[blockNumber] || blockNumber < 0 || blockNumber > g_width * g_height)
        return 0;

    if (g_mineSquare[blockNumber])
        return 2;

    g_selectedSquare[blockNumber] = true;

    // Recursively opening other empty squares
    if (CountOfBombsNearSquare(blockNumber) == 0) {
        // Special squares on the left
        if (blockNumber % g_width == 1) {
            OpenSquare(blockNumber - g_width);
            OpenSquare(blockNumber - g_width + 1);
            OpenSquare(blockNumber + 1);
            OpenSquare(blockNumber + g_width + 1);
            OpenSquare(blockNumber + g_width);
        }

        // Special squares on the right
        else if (blockNumber % g_width == 0) {
            OpenSquare(blockNumber - g_width - 1);
            OpenSquare(blockNumber - g_width);
            OpenSquare(blockNumber - 1);
            OpenSquare(blockNumber + g_width - 1);
            OpenSquare(blockNumber + g_width);

        // Common squares
        } else {
            OpenSquare(blockNumber - g_width - 1);
            OpenSquare(blockNumber - g_width);
            OpenSquare(blockNumber - g_width + 1);
            OpenSquare(blockNumber - 1);
            OpenSquare(blockNumber + 1);
            OpenSquare(blockNumber + g_width - 1);
            OpenSquare(blockNumber + g_width + 1);
            OpenSquare(blockNumber + g_width);
        }
    }
    return 1;
}

// The (dead)brain of the game!
void GameLogic(char inputCharacter, int windowRow, int windowColumn) {
    static int s_blockNumber;
    static bool s_modeClicked = false;
    int countSelectedSquare = 0;

    // Select square
    if (g_onPressed == 0) {
        // Up
        if (inputCharacter == 72 || inputCharacter == 'w' || inputCharacter == 'W') {
            ++g_onPressed;
            s_blockNumber = 0;
            PlaceFlagButton();
            SelectSquareButton();
            ClickGuide(g_onPressed, windowRow, windowColumn, 0);
            restore_cursor();
            return;
        }

        // Number
        else if (inputCharacter >= '0' && inputCharacter <= '9') {

            // Number out of range
            if (s_blockNumber * 10 + CHAR_TO_NUM(inputCharacter) > g_width * g_height)
                return;

            s_blockNumber = s_blockNumber * 10 + CHAR_TO_NUM(inputCharacter);

            if (s_blockNumber == 0)
                return;

            cursor_to_pos(6, 15);
            printf("%d", s_blockNumber);

            return;
        }

        // Enter pressed
        else if (inputCharacter == 13) {
            // Error | flag on
            if (g_flagSquare[s_blockNumber]) {
                cursor_to_pos(2, 0);
                change_color_rgb(0, 224, 142);
                printf("First remove the flag!");
                reset_color();
                s_blockNumber = 0;
                return;
            }

            int return_Func = OpenSquare(s_blockNumber);

            // Square has already opened
            if (return_Func == 0) {
                s_blockNumber = 0;
                return;
            }

            // Booom!
            else if (return_Func == 2) {
                change_background_color(5);
                cursor_to_pos(6, 15);
                printf("%d", s_blockNumber);
                reset_color();
                delay(300);
                clear_screen();
                PrintMapBorders(22, 4);
                PrintNumbersInsideSquare(22, 4, 1);
                play_beep();
                cursor_to_pos(1, 1);
                change_background_color(5);
                printf("Game Over");
                delay(8000);
                quit();
            }

            // Square opened
            else {
                change_background_color(3);
                cursor_to_pos(6, 15);
                printf("%d", s_blockNumber);
                reset_color();
                s_blockNumber = 0;

                for (int i = 1; i <= g_width * g_height; ++i) {
                    if (g_selectedSquare[i])
                        ++countSelectedSquare;
                }

                // Checking the wining
                if (countSelectedSquare == g_width * g_height - g_numberMines) {
                    clear_screen();
                    PrintMapBorders(22, 4);
                    PrintNumbersInsideSquare(22, 4, 1);
                    play_beep();
                    cursor_to_pos(1, 1);
                    change_background_color(3);
                    printf("Win");
                    delay(8000);
                    quit();
                }
            }
        }

        // Backspace pressed
        else if (inputCharacter == 8) {
            s_blockNumber /= 10;

            if (get_cursor_x() >= 15) {
                cursor_backward(1);
                printf(" ");
                cursor_backward(1);
            }
            return;
        } else
            return;
    }

    // Place flag
    else if (g_onPressed == 1) {
        // Up
        if (inputCharacter == 72 || inputCharacter == 'w' || inputCharacter == 'W') {
            ++g_onPressed;
            s_blockNumber = 0;
            PlaceFlagButton();
            ModeButton(false);
            ClickGuide(g_onPressed, windowRow, windowColumn, 0);
            return;
        }

        // Down
        else if (inputCharacter == 80 || inputCharacter == 's' || inputCharacter == 'S') {
            --g_onPressed;
            s_blockNumber = 0;
            PlaceFlagButton();
            SelectSquareButton();
            ClickGuide(g_onPressed, windowRow, windowColumn, 0);
            restore_cursor();
            return;
        }

        // Number
        else if (inputCharacter >= '0' && inputCharacter <= '9') {
            // adad kharej mahdoode
            if (s_blockNumber * 10 + CHAR_TO_NUM(inputCharacter) > g_width * g_height)
                return;

            s_blockNumber = s_blockNumber * 10 + CHAR_TO_NUM(inputCharacter);

            if (s_blockNumber == 0)
                return;

            cursor_to_pos(4, 15);
            printf("%d", s_blockNumber);

            return;
        }

        // Enter pressed
        else if (inputCharacter == 13) {
            // Error
            if (g_selectedSquare[s_blockNumber]) {
                cursor_to_pos(2, 0);
                change_color_rgb(0, 224, 142);
                printf("You cannot place a flag in selected square!");
                reset_color();
                s_blockNumber = 0;
                return;
            }

            if (g_flagSquare[s_blockNumber] == 0) {
                if (g_flagSquare[101] >= g_numberMines) {
                    // Error
                    cursor_to_pos(2, 0);
                    change_color_rgb(0, 224, 142);
                    printf("All of the flags have used!");
                    reset_color();
                    s_blockNumber = 0;
                    return;
                }
                // Saving count of flags used
                ++g_flagSquare[101];

            } else
                --g_flagSquare[101];

            g_flagSquare[s_blockNumber] = !g_flagSquare[s_blockNumber];
            s_blockNumber = 0;
        }

        // Backspace pressed
        else if (inputCharacter == 8) {
            s_blockNumber /= 10;

            if (get_cursor_x() >= 15) {
                cursor_backward(1);
                printf(" ");
                cursor_backward(1);
            }
            return;
        } else
            return;
    }

    // Mode
    else if (g_onPressed == 2) {
        // Down
        if ((inputCharacter == 80 || inputCharacter == 's' || inputCharacter == 'S') && !s_modeClicked) {
            --g_onPressed;
            PlaceFlagButton();
            ModeButton(false);
            ClickGuide(g_onPressed, windowRow, windowColumn, 0);
            restore_cursor();
            return;
        }

        // Enter pressed
        else if (inputCharacter == 13) {
            s_modeClicked = true;
            ModeButton(s_modeClicked);
            NewGameButton();
            QuitButton();
            ClickGuide(g_onPressed, windowRow, windowColumn, 1);
            return;
        }

        // Esc
        else if (inputCharacter == 27) {
            ClearLine(1, windowColumn);
            s_modeClicked = false;
            ModeButton(false);
            NewGameButton();
            QuitButton();
            ClickGuide(g_onPressed, windowRow, windowColumn, 0);
            return;
        }

        // Right
        else if (inputCharacter == 77 || inputCharacter == 'd' || inputCharacter == 'D') {
            if (s_modeClicked)
                ++g_onPressed;
            else
                g_onPressed = 6;

            ModeButton(s_modeClicked);
            NewGameButton();
            ClickGuide(g_onPressed, windowRow, windowColumn, s_modeClicked);
            return;
        } else
            return;
    }

    // Easy
    else if (g_onPressed == 3) {
        // Enter pressed
        if (inputCharacter == 13) {
            ChangeDifficultlyLevel();
            s_modeClicked = false;
            g_onPressed = 2;
        }

        // Esc
        else if (inputCharacter == 27) {
            ClearLine(1, windowColumn);
            g_onPressed = 2;
            s_modeClicked = false;
            ModeButton(false);
            NewGameButton();
            QuitButton();
            return;
        }

        // Right
        else if (inputCharacter == 77 || inputCharacter == 'd' || inputCharacter == 'D') {
            ++g_onPressed;
            ModeButton(true);
            ClickGuide(g_onPressed, windowRow, windowColumn, 1);
            return;
        }

        // Left
        else if (inputCharacter == 75 || inputCharacter == 'a' || inputCharacter == 'A') {
            --g_onPressed;
            ModeButton(true);
            ClickGuide(g_onPressed, windowRow, windowColumn, 1);
            return;
        } else
            return;
    }

    // Normal
    else if (g_onPressed == 4) {
        // Enter pressed
        if (inputCharacter == 13) {
            ChangeDifficultlyLevel();
            s_modeClicked = false;
            g_onPressed = 2;
        }

        // Esc
        else if (inputCharacter == 27) {
            ClearLine(1, windowColumn);
            g_onPressed = 2;
            s_modeClicked = false;
            ModeButton(false);
            NewGameButton();
            QuitButton();
            return;
        }

        // Right
        else if (inputCharacter == 77 || inputCharacter == 'd' || inputCharacter == 'D') {
            ++g_onPressed;
            ModeButton(true);
            ClickGuide(g_onPressed, windowRow, windowColumn, 1);
            return;
        }

        // Left
        else if (inputCharacter == 75 || inputCharacter == 'a' || inputCharacter == 'A') {
            --g_onPressed;
            ModeButton(true);
            ClickGuide(g_onPressed, windowRow, windowColumn, 1);
            return;
        } else
            return;
    }

    // Hard
    else if (g_onPressed == 5) {
        // Enter pressed
        if (inputCharacter == 13) {
            ChangeDifficultlyLevel();
            s_modeClicked = false;
            g_onPressed = 2;
        }

        // Esc
        else if (inputCharacter == 27) {
            ClearLine(1, windowColumn);
            g_onPressed = 2;
            s_modeClicked = false;
            ModeButton(false);
            NewGameButton();
            QuitButton();
            return;
        }
        // Left
        else if (inputCharacter == 75 || inputCharacter == 'a' || inputCharacter == 'A') {
            --g_onPressed;
            ModeButton(true);
            ClickGuide(g_onPressed, windowRow, windowColumn, 1);
            return;
        } else
            return;
    }

    // New game
    else if (g_onPressed == 6) {
        // Right
        if (inputCharacter == 77 || inputCharacter == 'd' || inputCharacter == 'D') {
            ++g_onPressed;
            ModeButton(false);
            NewGameButton();
            QuitButton();
            ClickGuide(g_onPressed, windowRow, windowColumn, 0);
            return;
        }

        // Left
        else if (inputCharacter == 75 || inputCharacter == 'a' || inputCharacter == 'A') {
            g_onPressed = 2;
            ModeButton(false);
            NewGameButton();
            ClickGuide(g_onPressed, windowRow, windowColumn, 0);
            return;
        }

        // Enter
        else if (inputCharacter == 13) {
            for (int i = 0; i < SIZE; ++i) {
                g_flagSquare[i] = 0;
                g_mineSquare[i] = 0;
                g_selectedSquare[i] = 0;
            }
            AddMines();
        } else
            return;
    }

    // Quit
    else if (g_onPressed == 7) {
        // Left
        if (inputCharacter == 75 || inputCharacter == 'a' || inputCharacter == 'A') {
            --g_onPressed;
            ModeButton(false);
            NewGameButton();
            QuitButton();
            ClickGuide(g_onPressed, windowRow, windowColumn, 0);
            return;
        }
        // Enter
        else if (inputCharacter == 13) {
            quit();
        } else
            return;
    }

    clear_screen();
    PrintMapBorders(22, 4);
    PrintNumbersInsideSquare(22, 4, 0);
    PrintToolbar(windowRow, windowColumn);
    PrintBombLeft(windowColumn);
    SelectSquareButton();
    PlaceFlagButton();
    ModeButton(s_modeClicked);
    NewGameButton();
    QuitButton();
    ClickGuide(g_onPressed, windowRow, windowColumn, 0);
    if (g_onPressed == 0 || g_onPressed == 1)
        restore_cursor();
}

void ChangeDifficultlyLevel() {
    for (int i = 0; i < SIZE; ++i) {
        g_flagSquare[i] = 0;
        g_mineSquare[i] = 0;
        g_selectedSquare[i] = 0;
    }
    // Ez
    if (g_onPressed == 3) {
        g_numberMines = 6;
        g_width = 5;
        g_height = 5;
    }

    // Normal
    else if (g_onPressed == 4) {
        g_numberMines = 8;
        g_width = 7;
        g_height = 7;
    }

    // Hard
    else if (g_onPressed == 5) {
        g_numberMines = 16;
        g_width = 10;
        g_height = 10;
    }
    AddMines();
}

void PrintToolbar(int row, int column) {
    // Bottom bar
    cursor_to_pos(row - 3, 0);
    for (int i = 0; i < column; ++i)
        printf("-");

    reset_color();

    // Top bar
    cursor_to_pos(3, 0);

    for (int i = 0; i < column; ++i)
        printf("-");
}

void PlaceFlagButton() {
    cursor_to_pos(4, 0);

    if (g_onPressed == 1) {
        change_color_rgb(235, 161, 52);
        printf("place a flag: ");
        save_cursor();

    } else {
        change_color_rgb(255, 255, 255);
        printf("place a flag: ");
    }

    cursor_to_pos(4, 15);
    printf("   ");

    reset_color();
}

void SelectSquareButton() {
    cursor_to_pos(6, 0);

    if (g_onPressed == 0) {
        change_color_rgb(235, 161, 52);
        printf("open a block: ");
        save_cursor();

    } else {
        change_color_rgb(255, 255, 255);
        printf("open a block: ");
    }

    cursor_to_pos(6, 15);
    printf("   ");

    reset_color();
}

void ModeButton(bool modeClicked) {
    if (modeClicked)
        change_background_color(1);

    cursor_to_pos(1, 0);

    if (g_onPressed == 2) {
        change_color_rgb(235, 161, 52);
        printf("Mode");

    } else {
        change_color_rgb(255, 255, 255);
        printf("Mode");
    }

    if (modeClicked) {
        if (g_onPressed == 2) {
            change_color_rgb(255, 255, 255);
            printf(" ->");
            printf(" Easy");
            printf(" Normal");
            printf(" Hard");

        } else if (g_onPressed == 3) {
            change_color_rgb(255, 255, 255);
            printf(" ->");
            change_color_rgb(0, 224, 22);
            printf(" Easy");
            change_color_rgb(255, 255, 255);
            printf(" Normal");
            printf(" Hard");

        } else if (g_onPressed == 4) {
            change_color_rgb(255, 255, 255);
            printf(" ->");
            printf(" Easy");
            change_color_rgb(255, 233, 92);
            printf(" Normal");
            change_color_rgb(255, 255, 255);
            printf(" Hard");

        } else if (g_onPressed == 5) {
            change_color_rgb(255, 255, 255);
            printf(" ->");
            printf(" Easy");
            printf(" Normal");
            change_color_rgb(255, 75, 51);
            printf(" Hard");
        }
    }
    reset_color();
}

void NewGameButton() {
    cursor_forward(1);

    change_color_rgb(138, 144, 158);
    printf("| ");

    if (g_onPressed == 6) {
        change_color_rgb(235, 161, 52);
        printf("New Game");

    } else {
        change_color_rgb(255, 255, 255);
        printf("New Game");
    }

    reset_color();
}

void QuitButton() {
    cursor_forward(1);

    change_color_rgb(138, 144, 158);
    printf("| ");

    if (g_onPressed == 7) {
        change_color_rgb(255, 36, 36);
        printf("Quit");

    } else {
        change_color_rgb(255, 255, 255);
        printf("Quit");
    }

    reset_color();
}

void ClickGuide(int button, int row, int column, bool modeClicked) {
    //0= open square
    //1= place flag
    //2= mode
    //3= easy
    //4= normal
    //5= hard
    //6= new game
    //7= quit
    ClearLine(row - 1, column);

    cursor_to_pos(row - 1, 0);

    if (button == 0) {
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("ENTER");
        reset_color();
        printf(" ok");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("W");
        reset_color();
        printf(" go up");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("0-9");
        reset_color();
        printf(" enter number");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("BACKSPACE");
        reset_color();
        printf(" clear number");

    } else if (button == 1) {
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("ENTER");
        reset_color();
        printf(" ok");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("W");
        reset_color();
        printf(" go up");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("S");
        reset_color();
        printf(" go down");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("0-9");
        reset_color();
        printf(" enter number");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("BACKSPACE");
        reset_color();
        printf(" clear number");

    } else if (button == 2) {

        if (!modeClicked) {
            change_background_color(6);
            change_color_rgb(0, 0, 0);
            printf("ENTER");
            reset_color();
            printf(" ok");
            cursor_forward(3);
        }

        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("D");
        reset_color();
        printf(" go right");

        if (modeClicked) {
            cursor_forward(3);
            change_background_color(6);
            change_color_rgb(0, 0, 0);
            printf("ESC");
            reset_color();
            printf(" exit");
        }

        if (!modeClicked) {
            cursor_forward(3);
            change_background_color(6);
            change_color_rgb(0, 0, 0);
            printf("S");
            reset_color();
            printf(" go down");
        }
    } else if (button == 3) {
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("ENTER");
        reset_color();
        printf(" ok");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("D");
        reset_color();
        printf(" go right");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("A");
        reset_color();
        printf(" go left");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("ESC");
        reset_color();
        printf(" exit");

    } else if (button == 4) {
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("ENTER");
        reset_color();
        printf(" ok");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("D");
        reset_color();
        printf(" go right");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("A");
        reset_color();
        printf(" go left");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("ESC");
        reset_color();
        printf(" exit");

    } else if (button == 5) {
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("ENTER");
        reset_color();
        printf(" ok");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("A");
        reset_color();
        printf(" go left");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("ESC");
        reset_color();
        printf(" exit");

    } else if (button == 6) {
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("ENTER");
        reset_color();
        printf(" ok");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("D");
        reset_color();
        printf(" go right");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("A");
        reset_color();
        printf(" go left");

    } else if (button == 7) {
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("ENTER");
        reset_color();
        printf(" ok");
        cursor_forward(3);
        change_background_color(6);
        change_color_rgb(0, 0, 0);
        printf("A");
        reset_color();
        printf(" go left");
    }
}

void ClearLine(int line, int column) {
    cursor_to_pos(line, 1);
    for (int i = 0; i < column; ++i)
        printf(" ");
}