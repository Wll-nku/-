#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// �����ͼ��С
#define WIDTH 800
#define HEIGHT 600
#define BLOCK_SIZE 20
#define MAX_LENGTH 200

// �ߵĽṹ��
struct Snake {
    int x[MAX_LENGTH];
    int y[MAX_LENGTH];
    int length;
    int direction;
};

// ʳ��Ľṹ��
struct Food {
    int x;
    int y;
};

// �ж��Ƿ����ߵ�����
bool isSnakeBody(Snake& snake, int x, int y) {
    for (int i = 0; i < snake.length; i++) {
        if (snake.x[i] == x && snake.y[i] == y) {
            return true;
        }
    }
    return false;
}

// ��ʼ����
void initSnake(Snake& snake) {
    snake.length = 3;
    snake.direction = 1; // 1: �� 2: �� 3: �� 4: ��
    for (int i = 0; i < snake.length; i++) {
        snake.x[i] = (WIDTH / 2 - i * BLOCK_SIZE);
        snake.y[i] = HEIGHT / 2;
    }
}

// ��ʼ��ʳ��
void initFood(Food& food, Snake& snake) {
    srand(time(NULL));
    do {
        food.x = rand() % (WIDTH / BLOCK_SIZE) * BLOCK_SIZE;
        food.y = rand() % (HEIGHT / BLOCK_SIZE) * BLOCK_SIZE;
    } while (isSnakeBody(snake, food.x, food.y));
}

// �����ߺ�ʳ��
void draw(Snake& snake, Food& food) {
    cleardevice();
    // ����ʳ��
    setfillcolor(YELLOW);
    fillrectangle(food.x, food.y, food.x + BLOCK_SIZE, food.y + BLOCK_SIZE);
    // ������
    setfillcolor(GREEN);
    for (int i = 0; i < snake.length; i++) {
        fillrectangle(snake.x[i], snake.y[i], snake.x[i] + BLOCK_SIZE, snake.y[i] + BLOCK_SIZE);
    }
}

// �ƶ���
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

// ����Ƿ�Ե�ʳ��
bool checkEat(Snake& snake, Food& food) {
    if (snake.x[0] == food.x && snake.y[0] == food.y) {
        snake.length++;
        initFood(food, snake);
        return true;
    }
    return false;
}

// ����Ƿ�ײ��ǽ�ڻ��Լ�
bool checkCollision(Snake& snake) {
    if (snake.x[0] < 0 || snake.x[0] >= WIDTH || snake.y[0] < 0 || snake.y[0] >= HEIGHT) {
        return true;
    }
    for (int i = 1; i < snake.length; i++) {
        if (snake.x[0] == snake.x[i] && snake.y[0] == snake.y[i]) {
            return true;
        }
    }
    return false;
}

// ����������룬ָ����ǰ��
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
    initSnake(snake);
    initFood(food, snake);

    while (1) {
        draw(snake, food);
        moveSnake(snake);
        checkEat(snake, food);
        if (checkCollision(snake)) {
            break;
        }
        handleMouseInput(snake);
        // ������ʱ�������ߵ��ƶ��ٶ�
        Sleep(400);
    }

    closegraph();
    return 0;
}