#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <tchar.h>

// 手动定义M_PI，确保代码可移植性
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 增大地图大小
#define WIDTH 1200
#define HEIGHT 900
#define BLOCK_SIZE 30
#define MAX_LENGTH 300
#define MAX_OBSTACLES 30
#define MAX_FOODS 5 // 定义最大食物数量
#define MAX_STARS 300 // 定义最大星星数量

// 前向声明
class Snake;
class Food;
class Obstacle;

// 星星类
class Star {
public:
    int x;
    int y;
    int brightness; // 星星亮度
    int changeRate; // 亮度变化速率
    int moveX;      // x 方向移动速度
    int moveY;      // y 方向移动速度

    Star() {
        x = rand() % WIDTH;
        y = rand() % HEIGHT;
        brightness = rand() % 256;
        changeRate = (rand() % 3) - 1; // -1, 0, 1
        if (changeRate == 0) {
            changeRate = 1;
        }
        moveX = (rand() % 3) - 1; // -1, 0, 1
        moveY = (rand() % 3) - 1; // -1, 0, 1
    }

    void update() {
        brightness += changeRate;
        if (brightness > 255) {
            brightness = 255;
            changeRate = -changeRate;
        }
        else if (brightness < 0) {
            brightness = 0;
            changeRate = -changeRate;
        }

        x += moveX;
        y += moveY;

        // 边界处理
        if (x < 0) x = WIDTH;
        if (x > WIDTH) x = 0;
        if (y < 0) y = HEIGHT;
        if (y > HEIGHT) y = 0;
    }
};

// 蛇类
class Snake {
public:
    int x[MAX_LENGTH];
    int y[MAX_LENGTH];
    int length;
    int direction;

    Snake() {
        length = 3;
        direction = 1; // 1: 右 2: 下 3: 左 4: 上
        for (int i = 0; i < length; i++) {
            x[i] = (WIDTH / 2 - i * BLOCK_SIZE);
            y[i] = HEIGHT / 2;
        }
    }

    void move() {
        for (int i = length - 1; i > 0; i--) {
            x[i] = x[i - 1];
            y[i] = y[i - 1];
        }
        switch (direction) {
        case 1:
            x[0] += BLOCK_SIZE;
            break;
        case 2:
            y[0] += BLOCK_SIZE;
            break;
        case 3:
            x[0] -= BLOCK_SIZE;
            break;
        case 4:
            y[0] -= BLOCK_SIZE;
            break;
        }
    }
};

// 食物类
class Food {
public:
    int x[MAX_FOODS];
    int y[MAX_FOODS];
    int count; // 记录当前食物的数量
    double angle[MAX_FOODS]; // 旋转角度

    Food();
    void init(Snake& snake, Obstacle& obstacle);
    void update();
    void regenerateFood(int index, Snake& snake, Obstacle& obstacle);

private:
    bool isSnakeBody(Snake& snake, int x, int y);
    bool isObstacle(Obstacle& obstacle, int x, int y);
    bool isFoodDuplicate(int index);
};

// 障碍物类
class Obstacle {
public:
    int x[MAX_OBSTACLES];
    int y[MAX_OBSTACLES];
    int count;

    Obstacle();
    void init(Snake& snake, Food& food);

private:
    bool isSnakeBody(Snake& snake, int x, int y);
    bool isFood(Food& food, int x, int y);
};

// 食物类方法实现
Food::Food() {
    count = 0;
}

void Food::init(Snake& snake, Obstacle& obstacle) {
    count = rand() % MAX_FOODS + 1; // 随机生成 1 到 MAX_FOODS 个食物
    int maxAttempts = 1000; // 最大尝试次数，避免死循环
    for (int i = 0; i < count; i++) {
        int attempts = 0;
        do {
            x[i] = rand() % (WIDTH / BLOCK_SIZE) * BLOCK_SIZE;
            y[i] = rand() % (HEIGHT / BLOCK_SIZE) * BLOCK_SIZE;
            attempts++;
            if (attempts > maxAttempts) {
                // 超出最大尝试次数，输出错误信息
                MessageBox(NULL, _T("食物初始化失败，可能地图空间不足！"), _T("错误"), MB_OK | MB_ICONERROR);
                return;
            }
        } while (isSnakeBody(snake, x[i], y[i]) ||
            isObstacle(obstacle, x[i], y[i]) ||
            isFoodDuplicate(i));
        angle[i] = 0; // 初始角度为 0
    }
}

void Food::update() {
    for (int i = 0; i < count; i++) {
        angle[i] += 0.05; // 每次增加 0.05 弧度
        if (angle[i] > 2 * M_PI) {
            angle[i] -= 2 * M_PI;
        }
    }
}

void Food::regenerateFood(int index, Snake& snake, Obstacle& obstacle) {
    int maxAttempts = 1000; // 最大尝试次数，避免死循环
    int attempts = 0;
    do {
        x[index] = rand() % (WIDTH / BLOCK_SIZE) * BLOCK_SIZE;
        y[index] = rand() % (HEIGHT / BLOCK_SIZE) * BLOCK_SIZE;
        attempts++;
        if (attempts > maxAttempts) {
            // 超出最大尝试次数，输出错误信息
            MessageBox(NULL, _T("食物重新生成失败，可能地图空间不足！"), _T("错误"), MB_OK | MB_ICONERROR);
            return;
        }
    } while (isSnakeBody(snake, x[index], y[index]) ||
        isObstacle(obstacle, x[index], y[index]) ||
        isFoodDuplicate(index));
    angle[index] = 0; // 初始角度为 0
}

bool Food::isSnakeBody(Snake& snake, int x, int y) {
    for (int i = 0; i < snake.length; i++) {
        if (snake.x[i] == x && snake.y[i] == y) {
            return true;
        }
    }
    return false;
}

bool Food::isObstacle(Obstacle& obstacle, int x, int y) {
    for (int i = 0; i < obstacle.count; i++) {
        if (obstacle.x[i] == x && obstacle.y[i] == y) {
            return true;
        }
    }
    return false;
}

bool Food::isFoodDuplicate(int index) {
    for (int i = 0; i < index; i++) {
        if (x[i] == x[index] && y[i] == y[index]) {
            return true;
        }
    }
    return false;
}

// 障碍物类方法实现
Obstacle::Obstacle() {
    count = 0;
}

void Obstacle::init(Snake& snake, Food& food) {
    count = rand() % MAX_OBSTACLES + 5; // 随机生成 5 到 MAX_OBSTACLES 个障碍物
    int maxAttempts = 1000; // 最大尝试次数，避免死循环
    for (int i = 0; i < count; i++) {
        int attempts = 0;
        do {
            x[i] = rand() % (WIDTH / BLOCK_SIZE) * BLOCK_SIZE;
            y[i] = rand() % (HEIGHT / BLOCK_SIZE) * BLOCK_SIZE;
            attempts++;
            if (attempts > maxAttempts) {
                // 超出最大尝试次数，输出错误信息
                MessageBox(NULL, _T("障碍物初始化失败，可能地图空间不足！"), _T("错误"), MB_OK | MB_ICONERROR);
                return;
            }
        } while (isSnakeBody(snake, x[i], y[i]) ||
            isFood(food, x[i], y[i]));
    }
}

bool Obstacle::isSnakeBody(Snake& snake, int x, int y) {
    for (int i = 0; i < snake.length; i++) {
        if (snake.x[i] == x && snake.y[i] == y) {
            return true;
        }
    }
    return false;
}

bool Obstacle::isFood(Food& food, int x, int y) {
    for (int i = 0; i < food.count; i++) {
        if (x == food.x[i] && y == food.y[i]) {
            return true;
        }
    }
    return false;
}

// 判断是否吃到食物，检查蛇头是否和任意一个食物位置重合
bool checkEat(Snake& snake, Food& food) {
    for (int i = 0; i < food.count; i++) {
        if (snake.x[0] == food.x[i] && snake.y[0] == food.y[i]) {
            return true;
        }
    }
    return false;
}

// 判断是否撞到障碍物
bool checkHitObstacle(Snake& snake, Obstacle& obstacle) {
    for (int i = 0; i < obstacle.count; i++) {
        if (snake.x[0] == obstacle.x[i] && snake.y[0] == obstacle.y[i]) {
            return true;
        }
    }
    return false;
}

// 绘制蛇的眼睛
void drawSnakeEyes(int x, int y, int direction) {
    int eyeSize = 4;
    switch (direction) {
    case 1: // 右
        setfillcolor(BLACK);
        fillcircle(x + BLOCK_SIZE * 3 / 4, y + BLOCK_SIZE / 4, eyeSize);
        fillcircle(x + BLOCK_SIZE * 3 / 4, y + BLOCK_SIZE * 3 / 4, eyeSize);
        break;
    case 2: // 下
        setfillcolor(BLACK);
        fillcircle(x + BLOCK_SIZE / 4, y + BLOCK_SIZE * 3 / 4, eyeSize);
        fillcircle(x + BLOCK_SIZE * 3 / 4, y + BLOCK_SIZE * 3 / 4, eyeSize);
        break;
    case 3: // 左
        setfillcolor(BLACK);
        fillcircle(x + BLOCK_SIZE / 4, y + BLOCK_SIZE / 4, eyeSize);
        fillcircle(x + BLOCK_SIZE / 4, y + BLOCK_SIZE * 3 / 4, eyeSize);
        break;
    case 4: // 上
        setfillcolor(BLACK);
        fillcircle(x + BLOCK_SIZE / 4, y + BLOCK_SIZE / 4, eyeSize);
        fillcircle(x + BLOCK_SIZE * 3 / 4, y + BLOCK_SIZE / 4, eyeSize);
        break;
    }
}

// 绘制旋转的五角星
void drawRotatedStar(int x, int y, double angle) {
    int r1 = BLOCK_SIZE / 2; // 外圆半径
    int r2 = BLOCK_SIZE / 5; // 内圆半径
    POINT pts[10];
    int vertexArray[20];

    for (int i = 0; i < 10; i++) {
        double a = i * 2 * M_PI / 10 + angle;
        int r = (i % 2 == 0) ? r1 : r2;
        pts[i].x = static_cast<int>(x + r * cos(a));
        pts[i].y = static_cast<int>(y + r * sin(a));
        vertexArray[2 * i] = pts[i].x;
        vertexArray[2 * i + 1] = pts[i].y;
    }

    COLORREF yellowColor = RGB(255, 255, 0);
    setfillcolor(yellowColor);
    fillpoly(10, vertexArray);
}

// 绘制蛇、食物、障碍物和得分信息
void draw(Snake& snake, Food& food, Obstacle& obstacle, int score, Star stars[]) {
    BeginBatchDraw();
    cleardevice();
    for (int i = 0; i < MAX_STARS; i++) {
        setfillcolor(RGB(stars[i].brightness, stars[i].brightness, stars[i].brightness));
        solidcircle(stars[i].x, stars[i].y, 1);
    }
    // 绘制得分信息
    TCHAR scoreText[20];
    _stprintf_s(scoreText, _T("Score: %d"), score);
    settextcolor(WHITE);
    settextstyle(30, 0, _T("宋体"));
    outtextxy(WIDTH - 200, 10, scoreText);

    // 绘制食物，循环绘制多个食物
    for (int i = 0; i < food.count; i++) {
        drawRotatedStar(food.x[i] + BLOCK_SIZE / 2, food.y[i] + BLOCK_SIZE / 2, food.angle[i]);
    }

    // 绘制蛇
    for (int i = 0; i < snake.length; i++) {
        int r = 0;
        int g = 255 * (double)i / snake.length;
        int b = 0;
        COLORREF color = RGB(r, g, b);
        setfillcolor(color);
        // 绘制圆角矩形
        roundrect(snake.x[i], snake.y[i], snake.x[i] + BLOCK_SIZE, snake.y[i] + BLOCK_SIZE, 8, 8);
        if (i == 0) {
            drawSnakeEyes(snake.x[i], snake.y[i], snake.direction);
        }
    }

    // 绘制障碍物
    setfillcolor(RED);
    for (int i = 0; i < obstacle.count; i++) {
        fillrectangle(obstacle.x[i], obstacle.y[i], obstacle.x[i] + BLOCK_SIZE, obstacle.y[i] + BLOCK_SIZE);
    }
    EndBatchDraw();
}

// 处理鼠标输入
void handleMouseInput(Snake& snake) {
    MOUSEMSG msg;
    while (MouseHit()) {
        msg = GetMouseMsg();
        if (msg.uMsg == WM_LBUTTONDOWN) {
            int clickX = msg.x;
            int clickY = msg.y;
            int headX = snake.x[0];
            int headY = snake.y[0];

            if (abs(clickX - headX) > abs(clickY - headY)) {
                if (clickX > headX && snake.direction != 3) {
                    snake.direction = 1;
                }
                else if (clickX < headX && snake.direction != 1) {
                    snake.direction = 3;
                }
            }
            else {
                if (clickY > headY && snake.direction != 4) {
                    snake.direction = 2;
                }
                else if (clickY < headY && snake.direction != 2) {
                    snake.direction = 4;
                }
            }
        }
    }
}

// 贪吃蛇游戏主函数
void playSnakeGame() {
    initgraph(WIDTH, HEIGHT);
    srand((unsigned)time(NULL));

    Snake snake;
    Food food;
    Obstacle obstacle;
    Star stars[MAX_STARS];

    for (int i = 0; i < MAX_STARS; i++) {
        stars[i] = Star();
    }

    food.init(snake, obstacle);
    obstacle.init(snake, food);

    int score = 0;

    while (1) {
        for (int i = 0; i < MAX_STARS; i++) {
            stars[i].update();
        }
        food.update();

        // 处理鼠标输入
        handleMouseInput(snake);

        snake.move();

        if (checkEat(snake, food)) {
            score++;
            snake.length++;
            for (int i = 0; i < food.count; i++) {
                if (snake.x[0] == food.x[i] && snake.y[0] == food.y[i]) {
                    food.regenerateFood(i, snake, obstacle);
                    break;
                }
            }
        }

        // 检查是否撞到边界、自己或障碍物
        if (snake.x[0] < 0 || snake.x[0] >= WIDTH || snake.y[0] < 0 || snake.y[0] >= HEIGHT ||
            checkHitObstacle(snake, obstacle)) {
            break;
        }
        for (int i = 1; i < snake.length; i++) {
            if (snake.x[0] == snake.x[i] && snake.y[0] == snake.y[i]) {
                break;
            }
        }

        draw(snake, food, obstacle, score, stars);
        Sleep(250);
    }
    // 显示游戏结束信息和最终得分
    settextcolor(RED);
    settextstyle(60, 0, _T("宋体"));
    outtextxy(WIDTH / 2 - 150, HEIGHT / 2 - 30, _T("Game Over!"));
    TCHAR finalScoreText[20];
    _stprintf_s(finalScoreText, _T("Final Score: %d"), score);
    settextcolor(WHITE);
    settextstyle(30, 0, _T("宋体"));
    outtextxy(WIDTH / 2 - 100, HEIGHT / 2 + 30, finalScoreText);
    Sleep(2000); // 等待 2 秒
    closegraph();
}
