// Written by HanaNeko

#include <algorithm>
#include <cassert>
#include <graphics.h>
#include <random>

const int FPS = 60;

enum SurfaceMode { mainMenu, selectMode, inGame, gameOver };
SurfaceMode surfaceMode = mainMenu;

enum GameStat { lose, win };
GameStat gameStat = lose;

int tableSize = 4;    // size of table (4, 5, 6)
int grid[8][8];       // table of numbers
int targetNumber = 0; // the number need to be found currently
int wrongNum;         // the number player found incorrectly
bool useHint;         // player requires a hint
bool usedHint;        // player used hint(s) in current game
bool hintText;
DWORD gameStartTime;
DWORD gameNowTime;
DWORD gameLastTime;
DWORD gameEndTime;

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
    gameLastTime = gameStartTime = GetTickCount();
    useHint = false;
    usedHint = false;
    hintText = false;
}

// main menu
void surface0() {
    RECT rect_title = {0, 150, 1280, 250};
    RECT rect_start = {440, 360, 840, 460};
    RECT rect_exit = {440, 500, 840, 600};
    RECT rect_author = {20, 660, 1280, 720};

    ExMessage msg;

    while (peekmessage(&msg)) {
        if (msg.message == WM_LBUTTONDOWN) {
            if (mouseInRectangle(rect_start, msg)) {
                surfaceMode = selectMode;
            }
            if (mouseInRectangle(rect_exit, msg)) {
                exit(0);
            }
        }
    }

    // draw title
    LOGFONT f;
    gettextstyle(&f);
    f.lfHeight = 96;
    _tcscpy(f.lfFaceName, _T("Consolas"));
    f.lfQuality = ANTIALIASED_QUALITY;
    settextstyle(&f);
    drawtext(_T("SCHULTE TABLE"), &rect_title,
             DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // draw buttons
    rectangle(440, 360, 840, 460);
    rectangle(440, 500, 840, 600);
    f.lfHeight = 48;
    settextstyle(&f);
    drawtext(_T("Start"), &rect_start, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("Exit"), &rect_exit, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // draw author
    f.lfHeight = 24;
    settextstyle(&f);
    drawtext(_T("Copyright 2024 by HanaNekOfficial"), &rect_author,
             DT_VCENTER | DT_SINGLELINE);
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
    RECT rect_retry = {825, 440, 1175, 540};
    RECT rect_mainMenu = {825, 560, 1175, 660};
    RECT rect_time = {850, 200, 1150, 400};
    RECT rect_hint = {100, 660, 700, 700};

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
                            useHint = false;
                            gameLastTime = GetTickCount();
                        } else if (grid[i][j] >
                                   targetNumber) { // Wrong number, player loses
                            surfaceMode = gameOver;
                            gameStat = lose;
                            wrongNum = grid[i][j];
                            gameEndTime = GetTickCount();
                        }
                    }
                }
            }

            if (mouseInRectangle(rect_retry, msg)) {
                surfaceMode = inGame;
                createGrid();
            }
            if (mouseInRectangle(rect_mainMenu, msg)) {
                surfaceMode = mainMenu;
            }
        }

        if (msg.message == WM_KEYDOWN) {
            if (msg.vkcode == 0x52) { // R
                surfaceMode = inGame;
                createGrid();
            }
            if (msg.vkcode == 0x4D) { // M
                surfaceMode = mainMenu;
            }
            if (msg.vkcode == 0x48) { // H
                usedHint = useHint = true;
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

    // draw time
    LOGFONT f;
    gettextstyle(&f);
    f.lfHeight = 96;
    settextstyle(&f);
    gameNowTime = GetTickCount();
    DWORD gamePassTime = gameNowTime - gameStartTime;
    std::string str = std::to_string(gamePassTime / 1000) + '.' +
                      (gamePassTime % 100 < 10 ? "0" : "") +
                      std::to_string(gamePassTime % 100) + 's';
    if (usedHint)
        settextcolor(0x0000FFFF); // yellow
    else
        settextcolor(0x00FFFFFF);
    drawtext(str.c_str(), &rect_time, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // draw Numbers
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
            } else if (num == targetNumber && useHint) {
                settextcolor(0x0000FFFF); // yellow
            } else {
                settextcolor(0x00FFFFFF); // white
            }

            drawtext(std::to_string(num).c_str(), &rect_grid[i][j],
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }
    settextcolor(0x00FFFFFF);

    // draw buttons
    rectangle(825, 440, 1175, 540);
    rectangle(825, 560, 1175, 660);
    f.lfHeight = 48;
    settextstyle(&f);
    drawtext(_T("Retry (R)"), &rect_retry,
             DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("Main Menu (M)"), &rect_mainMenu,
             DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // draw hint text
    if (hintText || gameNowTime - gameLastTime > 1500 * tableSize) {
        f.lfHeight = 32;
        settextstyle(&f);
        settextcolor(0x00FFFFFF);
        drawtext(_T("Press H to get a hint"), &rect_hint,
                 DT_VCENTER | DT_SINGLELINE);
        hintText = true;
    }

    // All numbers found, player wins
    if (targetNumber > tableSize * tableSize) {
        surfaceMode = gameOver;
        gameStat = win;
        gameEndTime = GetTickCount();
    }
}

// game over
void surface3() {
    // 100, 60, 700, 660
    RECT rect_grid[8][8];
    RECT rect_result = {800, 60, 1200, 150};
    RECT rect_retry = {825, 440, 1175, 540};
    RECT rect_mainMenu = {825, 560, 1175, 660};
    RECT rect_time = {850, 200, 1150, 400};
    RECT rect_hint = {100, 660, 700, 700};

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
        if (msg.message == WM_LBUTTONDOWN) {
            if (mouseInRectangle(rect_retry, msg)) {
                surfaceMode = inGame;
                createGrid();
            }
            if (mouseInRectangle(rect_mainMenu, msg)) {
                surfaceMode = mainMenu;
            }
        }

        if (msg.message == WM_KEYDOWN) {
            if (msg.vkcode == 0x52) { // R
                surfaceMode = inGame;
                createGrid();
            }
            if (msg.vkcode == 0x4D) { // M
                surfaceMode = mainMenu;
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
            } else if (num == wrongNum) {
                settextcolor(0x00000FF); // red
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
    if (gameStat == win) {
        if (usedHint) {
            settextcolor(0x0000FFFF); // yellow
            drawtext(_T("You won?"), &rect_result,
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        } else {
            settextcolor(0x0032CD32); // lime green
            drawtext(_T("You won!"), &rect_result,
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    } else if (gameStat == lose) {
        settextcolor(0x00000FF); // red
        drawtext(_T("You lost!"), &rect_result,
                 DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    settextcolor(0x00FFFFFF);

    // draw buttons
    rectangle(825, 440, 1175, 540);
    rectangle(825, 560, 1175, 660);
    f.lfHeight = 48;
    settextstyle(&f);
    drawtext(_T("Retry (R)"), &rect_retry,
             DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("Main Menu (M)"), &rect_mainMenu,
             DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // draw time
    f.lfHeight = 96;
    settextstyle(&f);
    DWORD gamePassTime = gameEndTime - gameStartTime;
    if (surfaceMode != gameOver)
        gamePassTime = 0;
    std::string str = std::to_string(gamePassTime / 1000) + '.' +
                      (gamePassTime % 100 < 10 ? "0" : "") +
                      std::to_string(gamePassTime % 100) + 's';
    if (usedHint)
        settextcolor(0x0000FFFF); // yellow
    else
        settextcolor(0x00FFFFFF);
    drawtext(str.c_str(), &rect_time, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // draw hint text
    if (hintText) {
        f.lfHeight = 32;
        settextstyle(&f);
        settextcolor(0x00FFFFFF);
        drawtext(_T("Press H to get a hint"), &rect_hint,
                 DT_VCENTER | DT_SINGLELINE);
    }
}

int main() {
    initgraph(1280, 720);

    HWND hWnd = GetHWnd();
    SetWindowText(hWnd, _T("Schulte Table"));

    BeginBatchDraw();

    while (true) {
        DWORD start_time = GetTickCount();

        cleardevice();

        switch (surfaceMode) {
        case mainMenu:
            surface0();
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

        // fix FPS
        DWORD end_time = GetTickCount();
        DWORD delta_time = end_time - start_time;
        if (delta_time < 1000 / FPS) {
            Sleep(1000 / FPS - delta_time);
        }
    }

    EndBatchDraw();

    return 0;
}