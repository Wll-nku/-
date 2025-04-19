#include <graphics.h>
#include <conio.h>
#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480
#define PADDLE_WIDTH 80
#define PADDLE_HEIGHT 10
#define BALL_RADIUS 10

int main() {
    // 初始化图形窗口
    initgraph(WIDTH, HEIGHT);

    // 挡板的初始位置
    int paddleX = (WIDTH - PADDLE_WIDTH) / 2;
    int paddleY = HEIGHT - PADDLE_HEIGHT - 20;

    // 小球的初始位置和速度
    int ballX = WIDTH / 2;
    int ballY = HEIGHT / 2;
    int ballVx = 3;
    int ballVy = 3;

    while (1) {
        // 清屏
        cleardevice();

        // 绘制挡板
        setfillcolor(YELLOW);
        fillrectangle(paddleX, paddleY, paddleX + PADDLE_WIDTH, paddleY + PADDLE_HEIGHT);

        // 绘制小球
        setfillcolor(RED);
        fillcircle(ballX, ballY, BALL_RADIUS);

        // 小球移动
        ballX += ballVx;
        ballY += ballVy;

        // 小球碰到左右边界反弹
        if (ballX - BALL_RADIUS <= 0 || ballX + BALL_RADIUS >= WIDTH) {
            ballVx = -ballVx;
        }
        // 小球碰到上边界反弹
        if (ballY - BALL_RADIUS <= 0) {
            ballVy = -ballVy;
        }
        // 小球碰到挡板反弹
        if (ballY + BALL_RADIUS >= paddleY && ballX >= paddleX && ballX <= paddleX + PADDLE_WIDTH) {
            ballVy = -ballVy;
        }
        // 小球掉出下边界，游戏结束
        if (ballY + BALL_RADIUS >= HEIGHT) {
            break;
        }

        // 处理键盘输入，控制挡板移动
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 0xe0) {
                ch = _getch();
                if (ch == 75 && paddleX > 0) {  // 左方向键
                    paddleX -= 10;
                }
                if (ch == 77 && paddleX + PADDLE_WIDTH < WIDTH) {  // 右方向键
                    paddleX += 10;
                }
            }
        }

        // 延时
        Sleep(10);
    }

    // 关闭图形窗口
    closegraph();
    return 0;
}