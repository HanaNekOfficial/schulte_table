// Written by HanaNeko

#include <algorithm>
#include <cassert>
#include <graphics.h>
#include <random>

const int FPS = 60;

enum SurfaceMode { mainMenu, selectMode, inGame, gameOver };
SurfaceMode surfaceMode = selectMode;

enum GameStat { lose, win };
GameStat gameStat = lose;

int tableSize = 4;    // size of table (4, 5, 6)
int grid[8][8];       // table of numbers
int targetNumber = 0; // the number need to be found currently

bool mouseInRectangle(const RECT rect, ExMessage msg) {
    if (msg.x < rect.left || msg.x > rect.right)
        return false;
    if (msg.y < rect.top || msg.y > rect.bottom)
        return false;
    return true;
}

void createGrid() {
    int num[50];
    assert(tableSize * tableSize <= 50);
    for (int i = 0; i < tableSize * tableSize; i++) {
        num[i] = i + 1;
    }

    std::random_device rd;
    std::mt19937 rng(rd());

    std::shuffle(num, num + tableSize * tableSize, rng);

    for (int i = 0; i < tableSize * tableSize; i++) {
        grid[i / tableSize][i % tableSize] = num[i];
    }

    targetNumber = 1;
}

// select mode
void surface1() {
    RECT rect_selectMode = {0, 210, 1280, 310};
    RECT rect_4x4 = {290, 360, 490, 560};
    RECT rect_5x5 = {540, 360, 740, 560};
    RECT rect_6x6 = {790, 360, 990, 560};
    RECT rect_back = {50, 50, 150, 100};

    ExMessage msg;

    while (peekmessage(&msg)) {
        if (msg.message == WM_LBUTTONDOWN) {
            if (mouseInRectangle(rect_back, msg)) {
                surfaceMode = mainMenu;
            }
            if (mouseInRectangle(rect_4x4, msg)) {
                surfaceMode = inGame;
                tableSize = 4;
                createGrid();
            }
            if (mouseInRectangle(rect_5x5, msg)) {
                surfaceMode = inGame;
                tableSize = 5;
                createGrid();
            }
            if (mouseInRectangle(rect_6x6, msg)) {
                surfaceMode = inGame;
                tableSize = 6;
                createGrid();
            }
        }
    }

    cleardevice();

    rectangle(290, 360, 490, 560);
    rectangle(540, 360, 740, 560);
    rectangle(790, 360, 990, 560);
    rectangle(50, 50, 150, 100);

    LOGFONT f;
    gettextstyle(&f);
    f.lfHeight = 64;
    _tcscpy(f.lfFaceName, _T("Consolas"));
    f.lfQuality = ANTIALIASED_QUALITY;
    settextstyle(&f);

    drawtext(_T("Select mode:"), &rect_selectMode,
             DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("4x4"), &rect_4x4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("5x5"), &rect_5x5, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("6x6"), &rect_6x6, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    f.lfHeight = 30;
    settextstyle(&f);
    drawtext(_T("BACK"), &rect_back, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

// in game
void surface2() {
    // 100, 60, 700, 660
    RECT rect_grid[8][8];

    for (int i = 0; i <= tableSize; i++) {
        for (int j = 0; j <= tableSize; j++) {
            rect_grid[i][j] = {100 + 600 / tableSize * i,
                               60 + 600 / tableSize * j,
                               100 + 600 / tableSize * (i + 1),
                               60 + 600 / tableSize * (j + 1)};
        }
    }

    ExMessage msg;

    while (peekmessage(&msg)) {
        if (msg.message == WM_LBUTTONDOWN) {

            for (int i = 0; i < tableSize; i++) {
                for (int j = 0; j < tableSize; j++) {
                    if (mouseInRectangle(rect_grid[i][j], msg)) {
                        if (grid[i][j] == targetNumber) {
                            targetNumber++;
                        } else { // Wrong number, player loses
                            surfaceMode = gameOver;
                            gameStat = lose;
                        }
                    }
                }
            }
        }
    }

    // draw grids
    for (int i = 0; i <= tableSize; i++) {
        line(100 + 600 / tableSize * i, 60, 100 + 600 / tableSize * i,
             660); // vertical lines
        line(100, 60 + 600 / tableSize * i, 700,
             60 + 600 / tableSize * i); // horizontal lines
    }

    // draw Numbers
    LOGFONT f;
    gettextstyle(&f);
    f.lfHeight = 64 * 6 / tableSize;
    _tcscpy(f.lfFaceName, _T("Consolas"));
    f.lfQuality = ANTIALIASED_QUALITY;
    settextstyle(&f);
    for (int i = 0; i < tableSize; i++) {
        for (int j = 0; j < tableSize; j++) {
            auto num = grid[i][j];
            if (num < targetNumber) {
                settextcolor(0x0032CD32); // lime green
            } else {
                settextcolor(0x00FFFFFF); // white
            }

            drawtext(std::to_string(num).c_str(), &rect_grid[i][j],
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }
    settextcolor(0x00FFFFFF);

    // All numbers found, player wins
    if (targetNumber > tableSize * tableSize) {
        surfaceMode = gameOver;
        gameStat = win;
    }
}

// game over
void surface3() {
    // 100, 60, 700, 660
    RECT rect_grid[8][8];
    RECT rect_result = {800, 60, 1200, 150};

    for (int i = 0; i < tableSize; i++) {
        for (int j = 0; j < tableSize; j++) {
            rect_grid[i][j] = {100 + 600 / tableSize * i,
                               60 + 600 / tableSize * j,
                               100 + 600 / tableSize * (i + 1),
                               60 + 600 / tableSize * (j + 1)};
        }
    }

    ExMessage msg;

    while (peekmessage(&msg)) {
    }

    // draw grids
    for (int i = 0; i <= tableSize; i++) {
        line(100 + 600 / tableSize * i, 60, 100 + 600 / tableSize * i,
             660); // vertical lines
        line(100, 60 + 600 / tableSize * i, 700,
             60 + 600 / tableSize * i); // horizontal lines
    }

    // draw Numbers
    LOGFONT f;
    gettextstyle(&f);
    f.lfHeight = 64 * 6 / tableSize;
    _tcscpy(f.lfFaceName, _T("Consolas"));
    f.lfQuality = ANTIALIASED_QUALITY;
    settextstyle(&f);
    for (int i = 0; i < tableSize; i++) {
        for (int j = 0; j < tableSize; j++) {
            auto num = grid[i][j];
            if (num < targetNumber) {
                settextcolor(0x0032CD32); // lime green
            } else {
                settextcolor(0x00FFFFFF); // white
            }
            drawtext(std::to_string(num).c_str(), &rect_grid[i][j],
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }
    settextcolor(0x00FFFFFF);

    // draw result
    f.lfHeight = 96;
    settextstyle(&f);
    if (gameStat == win)
        drawtext(_T("You win!"), &rect_result,
                 DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    else if (gameStat == lose)
        drawtext(_T("You lose!"), &rect_result,
                 DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

int main() {
    initgraph(1280, 720);

    BeginBatchDraw();

    while (true) {
        DWORD start_time = GetTickCount();

        cleardevice();

        switch (surfaceMode) {
        case mainMenu:
            break;
        case selectMode:
            surface1();
            break;
        case inGame:
            surface2();
            break;
        case gameOver:
            surface3();
            break;
        }

        FlushBatchDraw();

        DWORD end_time = GetTickCount();
        DWORD delta_time = end_time - start_time;
        if (delta_time < 1000 / FPS) {
            Sleep(1000 / FPS - delta_time);
        }
    }

    EndBatchDraw();

    return 0;
}