#include <iostream>
#include <conio.h>
#include <windows.h>

#include <mmsystem.h>
#include <fstream>
#pragma comment(lib, "winmm.lib")

#define SCREEN_HEIGHT 26
#define WIN_WIDTH 100

using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;

string playerName;
int playerAge;
bool bgmOn = true;

void gotoxy(int x, int y) {
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}

void setCursor(bool visible) {
    CONSOLE_CURSOR_INFO lpCursor;
    lpCursor.bVisible = visible;
    lpCursor.dwSize = 1;
    SetConsoleCursorInfo(console, &lpCursor);
}

void getPlayerInfo() {
    system("cls");
    SetConsoleTextAttribute(console, 11);
    gotoxy(WIN_WIDTH / 4, 2); cout << "===============================================";
    gotoxy(WIN_WIDTH / 4, 3); cout << "||             WELCOME TO THE GAME          ||";
    gotoxy(WIN_WIDTH / 4, 4); SetConsoleTextAttribute(console, 14);
    cout << "||             << CAR RACE GAME >>          ||";
    SetConsoleTextAttribute(console, 11);
    gotoxy(WIN_WIDTH / 4, 5); cout << "===============================================";

    SetConsoleTextAttribute(console, 15);
    gotoxy(WIN_WIDTH / 4, 7); cout << "Enter your Name: ";
    cin.ignore();
    getline(cin, playerName);

    gotoxy(WIN_WIDTH / 4, 8); cout << "Enter your Age: ";
    cin >> playerAge;

    gotoxy(WIN_WIDTH / 4, 10);
    SetConsoleTextAttribute(console, 10);
    cout << "Hello " << playerName << "! Get ready to race!";
    Sleep(2000);
}

class Utils {
public:
    static void drawBorder() {
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            for (int j = 0; j < 17; j++) {
                gotoxy(j, i); SetConsoleTextAttribute(console, 6); cout << "-";
                gotoxy(WIN_WIDTH - j, i); cout << "-";
            }
        }
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            gotoxy(WIN_WIDTH, i); cout << "-";
        }
    }

    static void gameOver(int score, int highScore) {
        mciSendString("stop bgm", NULL, 0, NULL);
        mciSendString("close bgm", NULL, 0, NULL);
        system("cls");

        SetConsoleTextAttribute(console, 12);
        gotoxy(WIN_WIDTH / 4, 6); cout << "===============================";
        gotoxy(WIN_WIDTH / 4, 7); cout << "          GAME OVER!           ";
        gotoxy(WIN_WIDTH / 4, 8); cout << "===============================";

        SetConsoleTextAttribute(console, 15);
        gotoxy(WIN_WIDTH / 4, 10); cout << "Player: " << playerName << " | Age: " << playerAge;
        gotoxy(WIN_WIDTH / 4, 11); cout << "Your Score: " << score;
        gotoxy(WIN_WIDTH / 4, 12); cout << "High Score: " << highScore;

        gotoxy(WIN_WIDTH / 4, 14);
        SetConsoleTextAttribute(console, 14);
        cout << "Press any key to return to Main Menu...";
        getch();
    }

    static void instructions() {
        system("cls");
        SetConsoleTextAttribute(console, 14);
        gotoxy(WIN_WIDTH / 4, 4); cout << "=========== INSTRUCTIONS ===========";
        SetConsoleTextAttribute(console, 15);
        gotoxy(WIN_WIDTH / 4, 6); cout << "1. Avoid incoming cars on the track.";
        gotoxy(WIN_WIDTH / 4, 7); cout << "2. Move using 'A' (Left) and 'D' (Right).";
        gotoxy(WIN_WIDTH / 4, 8); cout << "3. Press 'ESC' to quit during gameplay.";
        gotoxy(WIN_WIDTH / 4, 10);
        SetConsoleTextAttribute(console, 14);
        cout << "Have fun and drive safe!";

        gotoxy(WIN_WIDTH / 4, 13);
        SetConsoleTextAttribute(console, 11);
        cout << "Press any key to go back to the Main Menu.";
        getch();
    }

    static int loadHighScore() {
        ifstream file("mygame.txt");
        int highScore = 0;
        if (file) file >> highScore;
        return highScore;
    }

    static void saveHighScore(int score) {
        ofstream file("mygame.txt");
        if (file) file << score;
    }
};

class Car {
private:
    int carPos;
    int carColor;
    char car[4][4];
public:
    char car1[4][4] = {
        {' ', '±', '±', ' '},
        {'±', '±', '±', '±'},
        {' ', '±', '±', ' '},
        {'±', '±', '±', '±'}
    };

    char car2[4][4] = {
        {' ', '*', '*', ' '},
        {'*', '*', '*', '*'},
        {' ', '*', '*', ' '},
        {'*', '*', '*', '*'}
    };

    char car3[4][4] = {
        {' ', '#', '#', ' '},
        {'#', '#', '#', '#'},
        {' ', '#', '#', ' '},
        {'#', '#', '#', '#'}
    };

    Car(int selectedCar = 1, int color = 11) {
        carColor = color;
        carPos = WIN_WIDTH / 2;
        if (selectedCar == 1) setCarDesign(car1);
        else if (selectedCar == 2) setCarDesign(car2);
        else setCarDesign(car3);
    }

    void setCarDesign(char design[4][4]) {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                car[i][j] = design[i][j];
    }

    void drawCar() {
        SetConsoleTextAttribute(console, carColor);
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) {
                gotoxy(j + carPos, i + 22);
                cout << car[i][j];
            }
    }

    void eraseCar() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) {
                gotoxy(j + carPos, i + 22);
                cout << " ";
            }
    }

    void moveLeft() {
        if (carPos > 18) {
            carPos -= 4;
            PlaySound(TEXT("car_move_left.wav"), NULL, SND_FILENAME | SND_ASYNC);
        }
    }

    void moveRight() {
        if (carPos < 75) {
            carPos += 4;
            PlaySound(TEXT("car_move_right.wav"), NULL, SND_FILENAME | SND_ASYNC);
        }
    }

    int getPosition() {
        return carPos;
    }
};

class Enemy {
private:
    int enemyX, enemyY;
    bool active;
    int speed;

public:
    Enemy() { reset(); }

    void generate() {
        enemyX = 17 + rand() % 58;
    }

    void drawEnemy() {
        if (active) {
            SetConsoleTextAttribute(console, 12);
            gotoxy(enemyX, enemyY); cout << "****";
            gotoxy(enemyX, enemyY + 1); cout << " ** ";
            gotoxy(enemyX, enemyY + 2); cout << "****";
            gotoxy(enemyX, enemyY + 3); cout << " ** ";
        }
    }

    void eraseEnemy() {
        if (active) {
            for (int i = 0; i < 4; i++) {
                gotoxy(enemyX, enemyY + i); cout << "    ";
            }
        }
    }

    void move() {
        if (active) enemyY += speed;
    }

    void reset() {
        active = true;
        enemyY = 1;
        //speed = 1;
        generate();
    }
    
    void setSpeed(int s) { speed = s; }

    void increaseSpeed() { speed += 1; }

    bool checkCollision(int carPos) {
        return (enemyY + 4 >= 23 && (enemyX + 4 - carPos >= 0 && enemyX + 4 - carPos < 9));
    }

    bool isOffScreen() {
        return enemyY > SCREEN_HEIGHT - 4;
    }

    int getSpeed() { return speed; }
};

class Game {
private:
    Car car;
    Enemy enemies[3];
    int activeEnemies;
    int score, level, highScore;
    int currentSpeedLevel;
public:
    Game(int carChoice, int color) : car(carChoice, color) {
        score = 0;
        level = 1;
        currentSpeedLevel = 0;
        activeEnemies = 2;
        highScore = Utils::loadHighScore();
    }

    void updateScore() {
        gotoxy(WIN_WIDTH + 7, 5);
        SetConsoleTextAttribute(console, 15);
        cout << "Player: " << playerName;
        gotoxy(WIN_WIDTH + 7, 6); cout << "Score : " << score;
        gotoxy(WIN_WIDTH + 7, 7); cout << "Level : " << level;
        gotoxy(WIN_WIDTH + 7, 8); cout << "High  : " << highScore;
    }

   void checkLevelUp() {
        int newLevel = score / 20;
        if (newLevel > currentSpeedLevel) {
            currentSpeedLevel = newLevel;
            level = newLevel + 1;
            int newSpeed = 1 + currentSpeedLevel;

            for (int i = 0; i < activeEnemies; i++) {
                enemies[i].setSpeed(newSpeed);
            }

            if (score >= 30 && activeEnemies < 3) {
                enemies[2].reset();
                enemies[2].setSpeed(newSpeed);
                activeEnemies = 3;
            }
    }
}

    void play() {
        PlaySound(TEXT("mixkit-car-start-ignition-1559.wav"), NULL, SND_FILENAME | SND_SYNC);
        if (bgmOn) {
            mciSendString("open \"car_background.mp3\" type mpegvideo alias bgm", NULL, 0, NULL);
            mciSendString("play bgm repeat", NULL, 0, NULL);
        }

        enemies[0].reset(); enemies[1].reset();
        enemies[0].setSpeed(1); enemies[1].setSpeed(1);
        system("cls");
        Utils::drawBorder();
        updateScore();

        for (int i = 3; i > 0; --i) {
            gotoxy(WIN_WIDTH / 2, SCREEN_HEIGHT / 2);
            SetConsoleTextAttribute(console, 14);
            cout << i;
            Sleep(1000);
            system("cls");
            Utils::drawBorder();
            updateScore();
        }

        gotoxy(WIN_WIDTH / 2, SCREEN_HEIGHT / 2);
        SetConsoleTextAttribute(console, 10);
        cout << "Go!";
        Sleep(500);
        system("cls");

        Utils::drawBorder();
        updateScore();

        while (true) {
            if (kbhit()) {
                char ch = getch();
                if (ch == 'A' || ch == 'a') car.moveLeft();
                if (ch == 'l' || ch == 'L') car.moveRight();
                if (ch == 27) break;
            }

            car.drawCar();
            for (int i = 0; i < activeEnemies ; i++) {
                enemies[i].drawEnemy();
                if (enemies[i].checkCollision(car.getPosition())) {
                    Utils::gameOver(score, highScore);
                    if (score > highScore) {
                        highScore = score;
                        Utils::saveHighScore(highScore);
                    }
                    return;
                }
            }

            Sleep(50);
            car.eraseCar();
            for (int i = 0; i < activeEnemies; i++) {
                enemies[i].eraseEnemy();
                enemies[i].move();
                if (enemies[i].isOffScreen()) {
                    enemies[i].reset();
                    enemies[i].setSpeed(1 + currentSpeedLevel);
                    score++;
                    PlaySound(TEXT("score_point.wav"), NULL, SND_FILENAME | SND_ASYNC);
                    updateScore();
                }
            }

            checkLevelUp();
        }

        if (bgmOn) {
            mciSendString("stop bgm", NULL, 0, NULL);
            mciSendString("close bgm", NULL, 0, NULL);
        }
    }
};

int selectCar() {
    system("cls"); 
    SetConsoleTextAttribute(console, 14);
    gotoxy(WIN_WIDTH / 4, 2); cout << "========================================";
    gotoxy(WIN_WIDTH / 4, 3); cout << "            CHOOSE YOUR CAR";
    gotoxy(WIN_WIDTH / 4, 4); cout << "========================================";

    SetConsoleTextAttribute(console, 11);

    gotoxy(WIN_WIDTH / 4, 5); cout << "1.";
    gotoxy(WIN_WIDTH / 4 + 3, 5); cout << " ±± ";
    gotoxy(WIN_WIDTH / 4 + 3, 6); cout << "±±±±";
    gotoxy(WIN_WIDTH / 4 + 3, 7); cout << " ±± ";
    gotoxy(WIN_WIDTH / 4 + 3, 8); cout << "±±±±";

    gotoxy(WIN_WIDTH / 4 + 20, 5); cout << "2.";
    gotoxy(WIN_WIDTH / 4 + 23, 5); cout << " ** ";
    gotoxy(WIN_WIDTH / 4 + 23, 6); cout << "****";
    gotoxy(WIN_WIDTH / 4 + 23, 7); cout << " ** ";
    gotoxy(WIN_WIDTH / 4 + 23, 8); cout << "****";

    gotoxy(WIN_WIDTH / 4 + 40, 5); cout << "3.";
    gotoxy(WIN_WIDTH / 4 + 43, 5); cout << " ## ";
    gotoxy(WIN_WIDTH / 4 + 43, 6); cout << "####";
    gotoxy(WIN_WIDTH / 4 + 43, 7); cout << " ## ";
    gotoxy(WIN_WIDTH / 4 + 43, 8); cout << "####";

    SetConsoleTextAttribute(console, 15);
    gotoxy(WIN_WIDTH / 4, 10);
    cout << "Enter 1, 2, or 3: ";

    int choice;
    do {
        cin >> choice;
    } while (choice < 1 || choice > 3);

    return choice;
    
}
int selectCarColor() {
    system("cls");
    SetConsoleTextAttribute(console, 14);
    gotoxy(WIN_WIDTH / 4, 2); cout << "========================================";
    gotoxy(WIN_WIDTH / 4, 3); cout << "           CHOOSE CAR COLOR";
    gotoxy(WIN_WIDTH / 4, 4); cout << "========================================";

    SetConsoleTextAttribute(console, 9);
    gotoxy(WIN_WIDTH / 4, 6); cout << "1. Blue";
    SetConsoleTextAttribute(console, 12);
    gotoxy(WIN_WIDTH / 4, 7); cout << "2. Red";
    SetConsoleTextAttribute(console, 10);
    gotoxy(WIN_WIDTH / 4, 8); cout << "3. Green";
    SetConsoleTextAttribute(console, 11);
    gotoxy(WIN_WIDTH / 4, 9); cout << "4. Cyan";
    SetConsoleTextAttribute(console, 13);
    gotoxy(WIN_WIDTH / 4, 10); cout << "5. Pink";

    SetConsoleTextAttribute(console, 15);
    gotoxy(WIN_WIDTH / 4, 12); cout << "Enter 1-5: ";

    int choice;
    do {
        cin >> choice;
    } while (choice < 1 || choice > 5);

    switch (choice) {
        case 1: return 9;  // Blue
        case 2: return 12; // Red
        case 3: return 10; // Green
        case 4: return 11; // Cyan
        case 5: return 13; // Pink
    }

    return 15; // Default
}

void showMenu() {
    while (true) {
        system("cls");
        SetConsoleTextAttribute(console, 14);
        gotoxy(WIN_WIDTH / 4, SCREEN_HEIGHT / 4); cout << "============================";
        gotoxy(WIN_WIDTH / 4, SCREEN_HEIGHT / 4 + 1); cout << "     << CAR RACING GAME >>   ";
        gotoxy(WIN_WIDTH / 4, SCREEN_HEIGHT / 4 + 2); cout << "============================";
        SetConsoleTextAttribute(console, 15);
        gotoxy(WIN_WIDTH / 4, SCREEN_HEIGHT / 4 + 4); cout << "1. Start Game";
        gotoxy(WIN_WIDTH / 4, SCREEN_HEIGHT / 4 + 5); cout << "2. Instructions";
        gotoxy(WIN_WIDTH / 4, SCREEN_HEIGHT / 4 + 6); cout << "3. Toggle Music (Currently " << (bgmOn ? "ON" : "OFF") << ")";
        gotoxy(WIN_WIDTH / 4, SCREEN_HEIGHT / 4 + 7); cout << "4. Quit";
        gotoxy(WIN_WIDTH / 4, SCREEN_HEIGHT / 4 + 9); cout << "Select option: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                getPlayerInfo();
                {
                    int carChoice = selectCar();
                    int carColor = selectCarColor();  
       			Game game(carChoice, carColor);  
        			game.play();
       			break;
                }
            case 2: Utils::instructions(); break;
            case 3: bgmOn = !bgmOn; break;
            case 4: exit(0); break;
            default: break;
        }
    }
}

int main() {
    setCursor(false);
    showMenu();
    return 0;
}

