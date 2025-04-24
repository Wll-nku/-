#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <tchar.h>

// 增大地图大小
#define WIDTH 800
#define HEIGHT 600
#define BLOCK_SIZE 20
#define MAX_LENGTH 200
#define MAX_OBSTACLES 20
#define MAX_FOODS 5 // 定义最大食物数量

// 蛇的结构体
struct Snake {
    int x[MAX_LENGTH];
    int y[MAX_LENGTH];
    int length;
    int direction;
};

// 食物的结构体，修改为可以存储多个食物的信息
struct Food {
    int x[MAX_FOODS];
    int y[MAX_FOODS];
    int count; // 记录当前食物的数量
};

// 障碍物的结构体
struct Obstacle {
    int x[MAX_OBSTACLES];
    int y[MAX_OBSTACLES];
    int count;
};

// 判断是否是蛇的身体
bool isSnakeBody(Snake& snake, int x, int y) {
    for (int i = 0; i < snake.length; i++) {
        if (snake.x[i] == x && snake.y[i] == y) {
            return true;
        }
    }
    return false;
}

// 判断是否是障碍物
bool isObstacle(Obstacle& obstacle, int x, int y) {
    for (int i = 0; i < obstacle.count; i++) {
        if (obstacle.x[i] == x && obstacle.y[i] == y) {
            return true;
        }
    }
    return false;
}

// 判断食物位置是否重复
bool isFoodDuplicate(Food& food, int newX, int newY, int index) {
    for (int i = 0; i < index; i++) {
        if (food.x[i] == newX && food.y[i] == newY) {
            return true;
        }
    }
    return false;
}

// 初始化蛇
void initSnake(Snake& snake) {
    snake.length = 3;
    snake.direction = 1; // 1: 右 2: 下 3: 左 4: 上
    for (int i = 0; i < snake.length; i++) {
        snake.x[i] = (WIDTH / 2 - i * BLOCK_SIZE);
        snake.y[i] = HEIGHT / 2;
    }
}

// 初始化食物，生成多个食物并确保它们的位置合法
void initFood(Food& food, Snake& snake, Obstacle& obstacle) {
    srand(time(NULL));
    food.count = rand() % MAX_FOODS + 1; // 随机生成 1 到 MAX_FOODS 个食物
    for (int i = 0; i < food.count; i++) {
        do {
            food.x[i] = rand() % (WIDTH / BLOCK_SIZE) * BLOCK_SIZE;
            food.y[i] = rand() % (HEIGHT / BLOCK_SIZE) * BLOCK_SIZE;
        } while (isSnakeBody(snake, food.x[i], food.y[i]) ||
            isObstacle(obstacle, food.x[i], food.y[i]) ||
            isFoodDuplicate(food, food.x[i], food.y[i], i));
    }
}

// 初始化障碍物
void initObstacles(Obstacle& obstacle, Snake& snake, Food& food) {
    srand(time(NULL));
    obstacle.count = rand() % MAX_OBSTACLES + 5; // 随机生成 5 到 MAX_OBSTACLES 个障碍物
    for (int i = 0; i < obstacle.count; i++) {
        do {
            obstacle.x[i] = rand() % (WIDTH / BLOCK_SIZE) * BLOCK_SIZE;
            obstacle.y[i] = rand() % (HEIGHT / BLOCK_SIZE) * BLOCK_SIZE;
        } while (isSnakeBody(snake, obstacle.x[i], obstacle.y[i]) ||
            [](Food& f, int ox, int oy) {
                for (int j = 0; j < f.count; j++) {
                    if (ox == f.x[j] && oy == f.y[j]) {
                        return true;
                    }
                }
                return false;
            }(food, obstacle.x[i], obstacle.y[i]));
    }
}

// 绘制线性渐变背景
void drawGradientBackground() {
    COLORREF startColor = RGB(255, 0, 0); // 起始颜色为红色
    COLORREF endColor = RGB(0, 0, 255);   // 结束颜色为蓝色
    for (int y = 0; y < HEIGHT; y++) {
        double ratio = (double)y / HEIGHT;
        int r = (int)((1 - ratio) * GetRValue(startColor) + ratio * GetRValue(endColor));
        int g = (int)((1 - ratio) * GetGValue(startColor) + ratio * GetGValue(endColor));
        int b = (int)((1 - ratio) * GetBValue(startColor) + ratio * GetBValue(endColor));
        COLORREF currentColor = RGB(r, g, b);
        setlinecolor(currentColor);
        line(0, y, WIDTH, y);
    }
}

// 绘制蛇、食物、障碍物和得分信息
void draw(Snake& snake, Food& food, Obstacle& obstacle, int score) {
    drawGradientBackground();
    // 绘制得分信息
    TCHAR scoreText[20];
    _stprintf_s(scoreText, _T("Score: %d"), score);
    settextcolor(WHITE);
    settextstyle(30, 0, _T("宋体"));
    outtextxy(WIDTH - 200, 10, scoreText);

    // 绘制食物，循环绘制多个食物
    setfillcolor(YELLOW);
    for (int i = 0; i < food.count; i++) {
        fillrectangle(food.x[i], food.y[i], food.x[i] + BLOCK_SIZE, food.y[i] + BLOCK_SIZE);
    }
    // 绘制蛇
    setfillcolor(GREEN);
    for (int i = 0; i < snake.length; i++) {
        fillrectangle(snake.x[i], snake.y[i], snake.x[i] + BLOCK_SIZE, snake.y[i] + BLOCK_SIZE);
    }
    // 绘制障碍物
    setfillcolor(RED);
    for (int i = 0; i < obstacle.count; i++) {
        fillrectangle(obstacle.x[i], obstacle.y[i], obstacle.x[i] + BLOCK_SIZE, obstacle.y[i] + BLOCK_SIZE);
    }
}

// 移动蛇
void moveSnake(Snake& snake) {
    for (int i = snake.length - 1; i > 0; i--) {
        snake.x[i] = snake.x[i - 1];
        snake.y[i] = snake.y[i - 1];
    }
    switch (snake.direction) {
    case 1:
        snake.x[0] += BLOCK_SIZE;
        break;
    case 2:
        snake.y[0] += BLOCK_SIZE;
        break;
    case 3:
        snake.x[0] -= BLOCK_SIZE;
        break;
    case 4:
        snake.y[0] -= BLOCK_SIZE;
        break;
    }
}

// 检查是否吃到食物，检查蛇头是否和任意一个食物位置重合
bool checkEat(Snake& snake, Food& food, Obstacle& obstacle, int& score) {
    for (int i = 0; i < food.count; i++) {
        if (snake.x[0] == food.x[i] && snake.y[0] == food.y[i]) {
            snake.length++;
            score++; // 吃到食物得分加 1
            // 重新生成该食物
            do {
                food.x[i] = rand() % (WIDTH / BLOCK_SIZE) * BLOCK_SIZE;
                food.y[i] = rand() % (HEIGHT / BLOCK_SIZE) * BLOCK_SIZE;
            } while (isSnakeBody(snake, food.x[i], food.y[i]) ||
                isObstacle(obstacle, food.x[i], food.y[i]) ||
                isFoodDuplicate(food, food.x[i], food.y[i], i));
            return true;
        }
    }
    return false;
}

// 检查是否撞到墙壁、自己或障碍物
bool checkCollision(Snake& snake, Obstacle& obstacle) {
    if (snake.x[0] < 0 || snake.x[0] >= WIDTH || snake.y[0] < 0 || snake.y[0] >= HEIGHT) {
        return true;
    }
    for (int i = 1; i < snake.length; i++) {
        if (snake.x[0] == snake.x[i] && snake.y[0] == snake.y[i]) {
            return true;
        }
    }
    for (int i = 0; i < obstacle.count; i++) {
        if (snake.x[0] == obstacle.x[i] && snake.y[0] == obstacle.y[i]) {
            return true;
        }
    }
    return false;
}

// 处理鼠标输入，指引蛇前进
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

int main() {
    initgraph(WIDTH, HEIGHT);

    Snake snake;
    Food food;
    Obstacle obstacle;
    int score = 0;

    initSnake(snake);
    initFood(food, snake, obstacle);
    initObstacles(obstacle, snake, food);

    while (1) {
        draw(snake, food, obstacle, score);
        moveSnake(snake);
        checkEat(snake, food, obstacle, score);
        if (checkCollision(snake, obstacle)) {
            break;
        }
        handleMouseInput(snake);
        // 增加延时，减慢蛇的移动速度
        Sleep(400);
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
    return 0;
}