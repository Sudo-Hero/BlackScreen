#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <cwchar>
#include <string>
#include "ScreenBlocker.h"

#define ID_TIMER 1
#define ID_EDIT 2

HWND hwndButtonStart;
HWND hwndButtonReset;
HWND hwndEdit;
HWND hwndTimerLabel;

int countdownTime = 0;
int remainingTime = 0;
bool timerRunning = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        hwndButtonStart = CreateWindow(
            L"BUTTON", L"Start Timer",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 10, 100, 30,
            hwnd, NULL, NULL, NULL
        );

        hwndButtonReset = CreateWindow(
            L"BUTTON", L"Reset Timer",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            120, 10, 100, 30,
            hwnd, NULL, NULL, NULL
        );

        hwndEdit = CreateWindow(
            L"EDIT", L"",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
            230, 10, 50, 20,
            hwnd, (HMENU)ID_EDIT, NULL, NULL
        );

        hwndTimerLabel = CreateWindow(
            L"STATIC", L"00:00",
            WS_VISIBLE | WS_CHILD | SS_CENTER,
            10, 50, 100, 30,
            hwnd, NULL, NULL, NULL
        );

        SetTimer(hwnd, ID_TIMER, 1000, NULL);
        break;

    case WM_COMMAND:
        if ((HWND)lParam == hwndButtonStart && HIWORD(wParam) == BN_CLICKED) {
            if (!timerRunning) {
                int inputValueLength = GetWindowTextLength(hwndEdit) + 1;
                wchar_t* inputValue = new wchar_t[inputValueLength];
                GetWindowText(hwndEdit, inputValue, inputValueLength);

                countdownTime = _wtoi(inputValue);
                remainingTime = countdownTime * 60; // Convert minutes to seconds
                ScreenBlocker();
                OutputDebugStringA(std::to_string(countdownTime).c_str());
                
                delete[] inputValue;

                if (countdownTime > 0) {
                    timerRunning = true;
                }
            }
        }
        else if ((HWND)lParam == hwndButtonReset && HIWORD(wParam) == BN_CLICKED) {
            remainingTime = 0;
            SetWindowText(hwndTimerLabel, L"00:00");
            timerRunning = false;
            OutputDebugStringA("\nBUTTON PRESSED");
        }
        break;

    case WM_TIMER:
        if (timerRunning) {
            if (remainingTime > 0) {
                remainingTime--;
                int minutes = remainingTime / 60;
                int seconds = remainingTime % 60;
                wchar_t timeStr[10];
                swprintf(timeStr, sizeof(timeStr) / sizeof(timeStr[0]), L"%02d:%02d", minutes, seconds);
                SetWindowText(hwndTimerLabel, timeStr);
            }
            else {
                MessageBeep(MB_ICONEXCLAMATION); // Play system default sound
                remainingTime = 0; // Reset timer
                timerRunning = false;
                OutputDebugStringA("\nTime Elapsed");
                UnBlockInputs();
                WakePC();
            }
        }
        break;

    case WM_DESTROY:
        KillTimer(hwnd, ID_TIMER);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"TimerAppClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        return 1;
    }

    HWND hwnd = CreateWindow(
        L"TimerAppClass", L"Timer App",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        300, 250,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) {
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    bool keepRunning = true;

    MSG msg;
    while (keepRunning) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                keepRunning = false;
                break;
            }
        }
    }

    return (int)msg.wParam;
}
