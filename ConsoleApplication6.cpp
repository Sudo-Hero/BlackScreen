#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <cwchar>
#include <string>
#include "Include\\ScreenBlocker.h"
#include <objidl.h>
#include <gdiplus.h>
#include <strsafe.h>
#include <gdiplusinit.h>
#include <iostream>
#pragma comment (lib,"Gdiplus.lib")

using namespace Gdiplus;

#define ID_TIMER 1
#define ID_EDIT 2

HWND hwndButtonStart;
HWND hwndButtonReset;
HWND hwndEdit;
HWND hwndTimerLabel;
HWND hwnd;
HWND hwndTimerLabel2;
int countdownTime = 0;
int remainingTime = 0;
bool timerRunning = false;
MSG msg,msg2;

Font* ppFont;

void RenderImage(HDC hdc) {
    Graphics graphics(hdc);

    // Load the image with a transparent background
    Image image(L"neon.png");

    // Draw the image with transparency
    graphics.DrawImage(&image, 75, 95, 105, 100);
}

void DrawString(HDC hdc, std::wstring string, REAL x , REAL y) {
    Graphics                graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeHighQuality);
    //graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
    PointF                  point(x, y);

    SolidBrush              brush(Color(255, 255, 255, 255));

    Status pk = graphics.DrawString(string.c_str(), -1, ppFont, point, &brush);
    int dd = GetLastError();
}

void LoadFonts(HDC hdc) {
    Graphics                graphics(hdc);
    PrivateFontCollection   pFonts;
    
    FontFamily              *fontFamilies;
    Status                  isOk = pFonts.AddFontFile(L"Font\\Monserrat-Medium.ttf");
    int                     count = pFonts.GetFamilyCount();
    int                     found = 0;
    fontFamilies = new FontFamily[count * sizeof(FontFamily)];
    pFonts.GetFamilies(count, fontFamilies, &found);
    ppFont = new Font(L"Monserrat-Medium", 15, FontStyleRegular, UnitPixel);

    delete[] fontFamilies;

}



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
   
    switch (msg) {
        HDC hdc;
        PAINTSTRUCT ps;
        HBRUSH hbrBkgnd;
        int x;
    case WM_CREATE:
        hdc = BeginPaint(hwnd, &ps);
        LoadFonts(hdc);
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
            108, 50, 58, 20,
            hwnd, (HMENU)ID_EDIT, NULL, NULL
        );

        hwndTimerLabel = CreateWindow(
            L"STATIC", L"00:00",
            WS_VISIBLE | WS_CHILD | SS_CENTER | SS_SIMPLE,
            108, 135, 100, 30,
            hwnd, NULL, NULL, NULL
        );
        ShowWindow(hwndTimerLabel, SW_SHOW);

        SetTimer(hwnd, ID_TIMER, 1000, NULL);
        EndPaint(hwnd, &ps);
        break;

    case WM_COMMAND:
        
        if ((HWND)lParam == hwndButtonStart && HIWORD(wParam) == BN_CLICKED) {
            if (!timerRunning) {
                int inputValueLength = GetWindowTextLength(hwndEdit) + 1;
                wchar_t* inputValue = new wchar_t[inputValueLength];
                GetWindowText(hwndEdit, inputValue, inputValueLength);

                countdownTime = _wtoi(inputValue);
                remainingTime = countdownTime * 60; // Convert minutes to seconds
                //remainingTime = 5;
                delete[] inputValue;

                if (countdownTime > 0) {
                    timerRunning = true;
                }
                EnableWindow(hwndButtonStart, false);
            }
        }
        else if ((HWND)lParam == hwndButtonReset && HIWORD(wParam) == BN_CLICKED) {
            remainingTime = 0;
            SetWindowText(hwndTimerLabel, L"00:00");
            timerRunning = false;
            EnableWindow(hwndButtonStart, true);
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
                OutputDebugStringA("\nWM_TIMER");
                MessageBeep(MB_ICONEXCLAMATION); // Play system default sound
                remainingTime = 0; // Reset timer
                timerRunning = false;
                EnableWindow(hwndButtonStart, true);
                ScreenBlocker();
            }
        }
        break;
    case WM_DESTROY:
        KillTimer(hwnd, ID_TIMER);
        delete ppFont;
        PostQuitMessage(0);
        break;

    case WM_CTLCOLORSTATIC:
        //if (hwnd == hwndTimerLabel) {
            SetBkColor((HDC)wParam, RGB(0, 0, 0));
            SetTextColor((HDC)wParam, RGB(255, 255, 255));
            hbrBkgnd = CreateSolidBrush(RGB(0,0,0));
            x = GetLastError();
        //}
        //DrawString(hdc);
            return (INT_PTR)hbrBkgnd;
        break; 

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        DrawString(hdc, L"Enter Time: ", 10, 50);
        RenderImage(hdc);
        EndPaint(hwnd, &ps);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    GdiplusStartupInput Input;
    ULONG token;
    GdiplusStartup(&token, &Input, NULL);
    int dd = GetLastError();
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
    wcex.hbrBackground = CreateSolidBrush(RGB(27, 27, 27));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"TimerAppClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        return 1;
    }

    hwnd = CreateWindow(
        L"TimerAppClass", L"Timer App",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        300, 250,
        NULL, NULL, hInstance, NULL
    );
    hMainWnd = hwnd;
    HICON hIcon = (HICON)LoadImage(NULL, L"TimerApp.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    int ff = GetLastError();
    if(hIcon) {
        if (hwnd) {
            // Set the window's icon
            SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
            SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            ShowWindow(hwnd, nCmdShow);
            UpdateWindow(hwnd);
        }
    }
    if (!hwnd) {
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    bool keepRunning = true;
    while (keepRunning) {
        while (GetMessage(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.message == WM_QUIT) {
                keepRunning = false;
                break;
        }
    }
    GdiplusShutdown(token);
    return 0;
}
