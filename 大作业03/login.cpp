#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <iostream>

// ����̰������Ϸ������
extern void playSnakeGame();

// �Զ��屳����ɫ
#define BACKGROUND_COLOR RGB(240, 240, 240)

// ��ť���Ŷ�����ʱ�� ID
#define BUTTON_SCALE_TIMER_ID 2

// ��ť���Ŷ�������ʱ�䣨���룩
#define BUTTON_SCALE_DURATION 200

// ���ڹ��̺���
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hButton;
    static double buttonScale = 1.0;
    static BOOL isButtonHovered = FALSE;

    switch (msg) {
    case WM_CREATE: {
        // ������ʼ��Ϸ��ť��������ťλ�úʹ�С����Ӧ����Ĵ���
        hButton = CreateWindow(_T("BUTTON"), _T("��ʼ��Ϸ"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            250, 180, 150, 40, hwnd, (HMENU)1, NULL, NULL);
        if (hButton == NULL) {
            MessageBox(hwnd, _T("������ťʧ�ܣ�"), _T("����"), MB_OK | MB_ICONERROR);
            std::cout << "Create button failed, error code: " << GetLastError() << std::endl;
            return -1;
        }
        // ���ð�ť����
        HFONT hFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
        SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);

        // ������������
        HWND hTitle = CreateWindow(_T("STATIC"), _T("̰������Ϸ��¼����"), WS_VISIBLE | WS_CHILD | SS_CENTER,
            0, 50, 600, 50, hwnd, NULL, NULL, NULL);
        if (hTitle == NULL) {
            MessageBox(hwnd, _T("������������ʧ�ܣ�"), _T("����"), MB_OK | MB_ICONERROR);
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
            // ����ʼ��Ϸ��ť����¼�
            MessageBox(hwnd, _T("��Ϸ������ʼ��"), _T("��ʾ"), MB_OK);
            // �رյ�¼����
            DestroyWindow(hwnd);
            // ����̰������Ϸ
            playSnakeGame();
        }
        break;
    }
    case WM_CTLCOLORBTN: {
        // ���ð�ť������ɫ
        HDC hdcBtn = (HDC)wParam;
        SetBkColor(hdcBtn, RGB(0, 128, 255));
        SetTextColor(hdcBtn, RGB(255, 255, 255));
        return (LRESULT)CreateSolidBrush(RGB(0, 128, 255));
    }
    case WM_CTLCOLORSTATIC: {
        // ���þ�̬�ı�������ɫ
        HDC hdcStatic = (HDC)wParam;
        SetBkColor(hdcStatic, BACKGROUND_COLOR);
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        // ���ô��ڱ�����ɫ
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
            // ���Ű�ť��С
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

// ������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    // ��ʼ�������ؼ���
    InitCommonControls();

    // ע�ᴰ����
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("LoginWindowClass");
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, _T("ע�ᴰ����ʧ�ܣ�"), _T("����"), MB_OK | MB_ICONERROR);
        std::cout << "Register window class failed, error code: " << GetLastError() << std::endl;
        return 0;
    }

    // �������ڣ��������ڴ�СΪ 600x400
    HWND hwnd = CreateWindow(wc.lpszClassName, _T("̰������Ϸ��¼����"),
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInstance, NULL);
    if (hwnd == NULL) {
        MessageBox(NULL, _T("��������ʧ�ܣ�"), _T("����"), MB_OK | MB_ICONERROR);
        std::cout << "Create window failed, error code: " << GetLastError() << std::endl;
        return 0;
    }
    std::cout << "Window created successfully, handle: " << hwnd << std::endl;

    // ��ʾ����
    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    // ��Ϣѭ��
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}