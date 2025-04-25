#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <iostream>

// 声明贪吃蛇游戏主函数
extern void playSnakeGame();

// 自定义背景颜色
#define BACKGROUND_COLOR RGB(240, 240, 240)

// 按钮缩放动画定时器 ID
#define BUTTON_SCALE_TIMER_ID 2

// 按钮缩放动画持续时间（毫秒）
#define BUTTON_SCALE_DURATION 200

// 窗口过程函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hButton;
    static double buttonScale = 1.0;
    static BOOL isButtonHovered = FALSE;

    switch (msg) {
    case WM_CREATE: {
        // 创建开始游戏按钮，调整按钮位置和大小以适应更大的窗口
        hButton = CreateWindow(_T("BUTTON"), _T("开始游戏"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            250, 180, 150, 40, hwnd, (HMENU)1, NULL, NULL);
        if (hButton == NULL) {
            MessageBox(hwnd, _T("创建按钮失败！"), _T("错误"), MB_OK | MB_ICONERROR);
            std::cout << "Create button failed, error code: " << GetLastError() << std::endl;
            return -1;
        }
        // 设置按钮字体
        HFONT hFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
        SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);

        // 创建标题文字
        HWND hTitle = CreateWindow(_T("STATIC"), _T("贪吃蛇游戏登录界面"), WS_VISIBLE | WS_CHILD | SS_CENTER,
            0, 50, 600, 50, hwnd, NULL, NULL, NULL);
        if (hTitle == NULL) {
            MessageBox(hwnd, _T("创建标题文字失败！"), _T("错误"), MB_OK | MB_ICONERROR);
            std::cout << "Create title failed, error code: " << GetLastError() << std::endl;
            return -1;
        }
        HFONT hTitleFont = CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
        SendMessage(hTitle, WM_SETFONT, (WPARAM)hTitleFont, TRUE);
        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == 1) {
            // 处理开始游戏按钮点击事件
            MessageBox(hwnd, _T("游戏即将开始！"), _T("提示"), MB_OK);
            // 关闭登录窗口
            DestroyWindow(hwnd);
            // 启动贪吃蛇游戏
            playSnakeGame();
        }
        break;
    }
    case WM_CTLCOLORBTN: {
        // 设置按钮背景颜色
        HDC hdcBtn = (HDC)wParam;
        SetBkColor(hdcBtn, RGB(0, 128, 255));
        SetTextColor(hdcBtn, RGB(255, 255, 255));
        return (LRESULT)CreateSolidBrush(RGB(0, 128, 255));
    }
    case WM_CTLCOLORSTATIC: {
        // 设置静态文本背景颜色
        HDC hdcStatic = (HDC)wParam;
        SetBkColor(hdcStatic, BACKGROUND_COLOR);
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        // 设置窗口背景颜色
        HBRUSH hBrush = CreateSolidBrush(BACKGROUND_COLOR);
        FillRect(hdc, &ps.rcPaint, hBrush);
        DeleteObject(hBrush);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_TIMER: {
        if (wParam == BUTTON_SCALE_TIMER_ID) {
            if (isButtonHovered) {
                buttonScale += 0.1 * 10 / BUTTON_SCALE_DURATION;
                if (buttonScale > 1.2) {
                    buttonScale = 1.2;
                    KillTimer(hwnd, BUTTON_SCALE_TIMER_ID);
                }
            }
            else {
                buttonScale -= 0.1 * 10 / BUTTON_SCALE_DURATION;
                if (buttonScale < 1.0) {
                    buttonScale = 1.0;
                    KillTimer(hwnd, BUTTON_SCALE_TIMER_ID);
                }
            }
            // 缩放按钮大小
            RECT buttonRect;
            GetWindowRect(hButton, &buttonRect);
            int width = (int)((buttonRect.right - buttonRect.left) * buttonScale);
            int height = (int)((buttonRect.bottom - buttonRect.top) * buttonScale);
            int x = buttonRect.left + ((buttonRect.right - buttonRect.left) - width) / 2;
            int y = buttonRect.top + ((buttonRect.bottom - buttonRect.top) - height) / 2;
            ScreenToClient(hwnd, (LPPOINT)&x);
            ScreenToClient(hwnd, (LPPOINT)&y);
            MoveWindow(hButton, x, y, width, height, TRUE);
        }
        break;
    }
    case WM_MOUSEMOVE: {
        POINT mousePos;
        GetCursorPos(&mousePos);
        RECT buttonRect;
        GetWindowRect(hButton, &buttonRect);
        if (PtInRect(&buttonRect, mousePos) && !isButtonHovered) {
            isButtonHovered = TRUE;
            SetTimer(hwnd, BUTTON_SCALE_TIMER_ID, 10, NULL);
        }
        else if (!PtInRect(&buttonRect, mousePos) && isButtonHovered) {
            isButtonHovered = FALSE;
            SetTimer(hwnd, BUTTON_SCALE_TIMER_ID, 10, NULL);
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// 主函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    // 初始化公共控件库
    InitCommonControls();

    // 注册窗口类
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("LoginWindowClass");
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, _T("注册窗口类失败！"), _T("错误"), MB_OK | MB_ICONERROR);
        std::cout << "Register window class failed, error code: " << GetLastError() << std::endl;
        return 0;
    }

    // 创建窗口，调整窗口大小为 600x400
    HWND hwnd = CreateWindow(wc.lpszClassName, _T("贪吃蛇游戏登录界面"),
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInstance, NULL);
    if (hwnd == NULL) {
        MessageBox(NULL, _T("创建窗口失败！"), _T("错误"), MB_OK | MB_ICONERROR);
        std::cout << "Create window failed, error code: " << GetLastError() << std::endl;
        return 0;
    }
    std::cout << "Window created successfully, handle: " << hwnd << std::endl;

    // 显示窗口
    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}