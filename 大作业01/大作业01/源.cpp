#include <graphics.h>
#include <conio.h>
#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480
#define PADDLE_WIDTH 80
#define PADDLE_HEIGHT 10
#define BALL_RADIUS 10

int main() {
    // ��ʼ��ͼ�δ���
    initgraph(WIDTH, HEIGHT);

    // ����ĳ�ʼλ��
    int paddleX = (WIDTH - PADDLE_WIDTH) / 2;
    int paddleY = HEIGHT - PADDLE_HEIGHT - 20;

    // С��ĳ�ʼλ�ú��ٶ�
    int ballX = WIDTH / 2;
    int ballY = HEIGHT / 2;
    int ballVx = 3;
    int ballVy = 3;

    while (1) {
        // ����
        cleardevice();

        // ���Ƶ���
        setfillcolor(YELLOW);
        fillrectangle(paddleX, paddleY, paddleX + PADDLE_WIDTH, paddleY + PADDLE_HEIGHT);

        // ����С��
        setfillcolor(RED);
        fillcircle(ballX, ballY, BALL_RADIUS);

        // С���ƶ�
        ballX += ballVx;
        ballY += ballVy;

        // С���������ұ߽練��
        if (ballX - BALL_RADIUS <= 0 || ballX + BALL_RADIUS >= WIDTH) {
            ballVx = -ballVx;
        }
        // С�������ϱ߽練��
        if (ballY - BALL_RADIUS <= 0) {
            ballVy = -ballVy;
        }
        // С���������巴��
        if (ballY + BALL_RADIUS >= paddleY && ballX >= paddleX && ballX <= paddleX + PADDLE_WIDTH) {
            ballVy = -ballVy;
        }
        // С������±߽磬��Ϸ����
        if (ballY + BALL_RADIUS >= HEIGHT) {
            break;
        }

        // ����������룬���Ƶ����ƶ�
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 0xe0) {
                ch = _getch();
                if (ch == 75 && paddleX > 0) {  // �����
                    paddleX -= 10;
                }
                if (ch == 77 && paddleX + PADDLE_WIDTH < WIDTH) {  // �ҷ����
                    paddleX += 10;
                }
            }
        }

        // ��ʱ
        Sleep(10);
    }

    // �ر�ͼ�δ���
    closegraph();
    return 0;
}